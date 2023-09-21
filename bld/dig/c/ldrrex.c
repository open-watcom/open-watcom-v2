/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  DIP/MAD/TRP module loader for Pharlap REX file format.
*
****************************************************************************/


#include "enterdb.h"

/*
 * Pharlap executable Loader (used by 32-bit code only)
 *
 * used also for TRAP/DIP/MAD load on Linux/QNX/OSX host OS
 * until native shared libraries will be supported by OW
 *
 * Usage of the Prarlap executable Loader
 *
 *  Host OS     TRAP    MAD     DIP
 *
 *  DOS         -       yes     yes
 *  QNX         yes     yes     yes
 *  LINUX       -       yes     yes
 *  OSX         -       yes     yes
 */

#if defined(__WATCOMC__) && defined(__386__) && !defined(NDEBUG)
#define WATCOM_DEBUG_SYMBOLS
#endif

#define RELOC_BUFF_SIZE 64

typedef struct {
#ifdef __WATCOMC__
    char        signature[4];
#endif
    char        init_rtn[1];    /* offset is start of routine */
} image_hdr;

typedef struct {
    char            *modname;
} *module;

#ifdef WATCOM_DEBUG_SYMBOLS
unsigned short GetCS(void);
#pragma aux GetCS \
     __parm __caller [] = \
        "mov ax,cs" \
     __value [__ax]
#endif

static void loader_unload_image( module modhdl )
{
#ifdef WATCOM_DEBUG_SYMBOLS
    if( modhdl->modname != NULL ) {
        /*
         * Notify the Watcom Debugger of module unload and let it remove symbolic info
         */
        DebuggerUnloadUserModule( modhdl->modname );
        free( modhdl->modname );
    }
#endif
    DIGCli( Free )( modhdl );
}

static digld_error loader_load_image( FILE *fp, const char *filename, module *mod_hdl, void **init_func )
{
    simple_header       hdr;
    unsigned            image_size;
    unsigned            hdr_size;
    unsigned            bss_size;
    unsigned            reloc_size;
    unsigned            bunch;
    size_t              i;
#ifdef WATCOM_DEBUG_SYMBOLS
    size_t              len;
#endif
    unsigned_32         *fixup_loc;
    unsigned            buff[RELOC_BUFF_SIZE];
    module              modhdl;
    char                *image_ptr;

#ifndef WATCOM_DEBUG_SYMBOLS
    (void)filename;
#endif
    (void)init_func;

    *mod_hdl = NULL;
    if( DIGLoader( Read )( fp, &hdr, sizeof( hdr ) ) )
        return( DIGS_ERR_CANT_LOAD_MODULE );
    if( hdr.signature != EXESIGN_REX )
        return( DIGS_ERR_BAD_MODULE_FILE );
    hdr_size = hdr.hdr_size * 16;
    image_size = (hdr.file_size * 0x200) - (-hdr.mod_size & 0x1ff) - hdr_size;
    bss_size = hdr.min_data * _4K;
#if defined( __LINUX__ )
    /*
     * the content in memory must be aligned to the _4K boundary,
     * so one _4K page is added
     */
    modhdl = DIGCli( Alloc )( sizeof( *modhdl ) + image_size + bss_size + _4K );
#else
    modhdl = DIGCli( Alloc )( sizeof( *modhdl ) + image_size + bss_size );
#endif
    if( modhdl == NULL )
        return( DIGS_ERR_OUT_OF_MEMORY );
#if defined( __LINUX__ )
    /*
     * align memory pointer to the _4K boundary
     */
    image_ptr = (char *)__ROUND_UP_SIZE_4K( (unsigned_32)modhdl + sizeof( *modhdl ) );
#else
    image_ptr = (char *)( (unsigned_32)modhdl + sizeof( *modhdl ) );
#endif
    modhdl->modname = NULL;
    DIGLoader( Seek )( fp, hdr_size, DIG_SEEK_ORG );
    if( DIGLoader( Read )( fp, image_ptr, image_size ) ) {
        loader_unload_image( modhdl );
        return( DIGS_ERR_CANT_LOAD_MODULE );
    }
    DIGLoader( Seek )( fp, hdr.reloc_offset, DIG_SEEK_ORG );
    bunch = RELOC_BUFF_SIZE;
    reloc_size = RELOC_BUFF_SIZE * sizeof( buff[0] );
    while( hdr.num_relocs != 0 ) {
        if( RELOC_BUFF_SIZE > hdr.num_relocs ) {
            bunch = hdr.num_relocs;
            reloc_size = bunch * sizeof( buff[0] );
        }
        if( DIGLoader( Read )( fp, buff, reloc_size ) ) {
            loader_unload_image( modhdl );
            return( DIGS_ERR_CANT_LOAD_MODULE );
        }
        for( i = 0; i < bunch; ++i ) {
            fixup_loc = (void *)(image_ptr + (buff[i] & ~0x80000000));
            *fixup_loc += (unsigned_32)image_ptr;
        }
        hdr.num_relocs -= bunch;
    }
#if defined( __LINUX__ )
    /*
     * On some platforms (such as AMD64 or x86 with NX bit), it is required
     * to map the code pages loaded from the BPD as executable, otherwise
     * a segfault will occur when attempting to run any BPD code.
     */
    mprotect( (void *)image_ptr, image_size, PROT_READ | PROT_WRITE | PROT_EXEC );
#endif
    memset( image_ptr + image_size, 0, bss_size );
#ifdef __WATCOMC__
    *init_func = SIGN_EQUAL( ((image_hdr *)image_ptr)->signature, MODSIGN ) ? ((image_hdr *)image_ptr)->init_rtn : NULL;
#else
    *init_func = ((image_hdr *)image_ptr)->init_rtn;
#endif
    *mod_hdl = modhdl;
#ifdef WATCOM_DEBUG_SYMBOLS
    /*
     * Store the file name in the modhdl structure; this must be the real
     * file name where the debugger will try to load symbolic info from
     *
     * remove driver from begining of file name
     */
    if( filename[0] != '\0' && filename[1] == ':' ) {
        filename += 2;
    }
    len = strlen( filename );
    if( len > 0 ) {
        modhdl->modname = malloc( len + 4 + 1 );
        if( modhdl->modname != NULL ) {
            size_t  pos;

            pos = len;
            for( i = 0; i < len; i++ ) {
                switch( *filename ) {
                case '\\':
                case '/':
                case ':':
                    pos = len;
                    break;
                case '.':
                    pos = i;
                    break;
                }
                modhdl->modname[i] = *filename++;
            }
            strcpy( modhdl->modname + pos, ".sym" );
            /*
             * Notify the Watcom Debugger of module load and let it load symbolic info
             */
            DebuggerLoadUserModule( modhdl->modname, GetCS(), (unsigned long)image_ptr );
        }
    }
#endif
    return( DIGS_OK );
}
