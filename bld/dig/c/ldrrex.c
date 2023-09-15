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


#define RELOC_BUFF_SIZE 64

typedef struct {
#ifdef __WATCOMC__
    unsigned_32         sig;
#endif
    unsigned_8          init_rtn[1];    /* offset is start of routine */
} *module;

static void loader_unload_image( module modhdl )
{
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
    unsigned            i;
    unsigned_32         *fixup_loc;
    unsigned            buff[RELOC_BUFF_SIZE];
    module              modhdl;

    (void)filename;
    (void)init_func;

    *mod_hdl = NULL;
    if( DIGLoader( Read )( fp, &hdr, sizeof( hdr ) ) )
        return( DIGS_ERR_CANT_LOAD_MODULE );
    if( hdr.signature != EXESIGN_REX )
        return( DIGS_ERR_BAD_MODULE_FILE );
    hdr_size = hdr.hdr_size * 16;
    image_size = (hdr.file_size * 0x200) - (-hdr.mod_size & 0x1ff) - hdr_size;
    bss_size = hdr.min_data * _4K;
    modhdl = DIGCli( Alloc )( image_size + bss_size );
    if( modhdl == NULL )
        return( DIGS_ERR_OUT_OF_MEMORY );
    DIGLoader( Seek )( fp, hdr_size, DIG_SEEK_ORG );
    if( DIGLoader( Read )( fp, modhdl, image_size ) ) {
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
            fixup_loc = (void *)((char *)modhdl + (buff[i] & ~0x80000000));
            *fixup_loc += (unsigned_32)modhdl;
        }
        hdr.num_relocs -= bunch;
    }
#ifdef __LINUX__
    /*
     * On some platforms (such as AMD64 or x86 with NX bit), it is required
     * to map the code pages loaded from the BPD as executable, otherwise
     * a segfault will occur when attempting to run any BPD code.
     */
    mprotect((void*)__ROUND_DOWN_SIZE_4K( (u_long)modhdl ), __ROUND_UP_SIZE_4K( image_size ), PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
    memset( (char *)modhdl + image_size, 0, bss_size );
#ifdef __WATCOMC__
    *init_func = (modhdl->sig == MODSIG) ? modhdl->init_rtn : NULL;
#else
    *init_func = modhdl->init_rtn;
#endif
    *mod_hdl = modhdl;
    return( DIGS_OK );
}
