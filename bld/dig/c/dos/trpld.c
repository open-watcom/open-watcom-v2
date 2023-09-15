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
* Description:  Trap module loader for DOS.
*
****************************************************************************/


#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tinyio.h"
#include "exedos.h"
#include "trpld.h"
#include "roundmac.h"


#define TRAP_SIGNATURE          0xDEAF

#define NUM_BUFF_RELOCS         16

typedef struct {
    unsigned_16         signature;
    unsigned_16         init_off;
    unsigned_16         req_off;
    unsigned_16         fini_off;
} trap_header;

static trap_header      __far *TrapCode = NULL;
static trap_fini_func   *FiniFunc = NULL;

static digld_error ReadInTrap( FILE *fp )
{
    dos_exe_header      hdr;
    unsigned            image_size;
    unsigned            hdr_size;
    struct {
        unsigned_16     off;
        unsigned_16     seg;
    }                   buff[ NUM_BUFF_RELOCS ], __far *p;
    unsigned_16         start_seg;
    unsigned_16         __far *fixup;
    tiny_ret_t          ret;
    unsigned            relocs;

    hdr.signature = 0;
    if( DIGLoader( Read )( fp, &hdr, sizeof( hdr ) ) ) {
        return( DIGS_ERR_CANT_LOAD_MODULE );
    }
    if( hdr.signature != EXESIGN_DOS ) {
        return( DIGS_ERR_BAD_MODULE_FILE );
    }
    hdr_size = hdr.hdr_size * 16;
    image_size = (hdr.file_size * 0x200) - (-hdr.mod_size & 0x1ff) - hdr_size;
    ret = TinyAllocBlock( __ROUND_UP_SIZE_TO_PARA( image_size ) );
    if( TINY_ERROR( ret ) ) {
        return( DIGS_ERR_OUT_OF_DOS_MEMORY );
    }
    start_seg = TINY_INFO( ret );
    TrapCode = _MK_FP( start_seg, 0 );
    DIGLoader( Seek )( fp, hdr_size, DIG_SEEK_ORG );
    DIGLoader( Read )( fp, TrapCode, image_size );
    DIGLoader( Seek )( fp, hdr.reloc_offset, DIG_SEEK_ORG );
    p = &buff[NUM_BUFF_RELOCS];
    for( relocs = hdr.num_relocs; relocs != 0; --relocs ) {
        if( p >= &buff[ NUM_BUFF_RELOCS ] ) {
            if( DIGLoader( Read )( fp, buff, sizeof( buff ) ) ) {
                return( DIGS_ERR_BAD_MODULE_FILE );
            }
            p = buff;
        }
        fixup = _MK_FP( p->seg + start_seg, p->off );
        *fixup += start_seg;
        ++p;
    }
    return( DIGS_OK );
}

void UnLoadTrap( void )
{
    ReqFunc = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( TrapCode != NULL ) {
        TinyFreeBlock( _FP_SEG( TrapCode ) );
        TrapCode = NULL;
    }
}

digld_error LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    FILE            *fp;
    trap_init_func  *init_func;
    char            filename[_MAX_PATH];
    const char      *base_name;
    digld_error     err;
    size_t          len;

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
    base_name = parms;
    len = 0;
    for( ; *parms != '\0'; parms++ ) {
        if( *parms == TRAP_PARM_SEPARATOR ) {
            parms++;
            break;
        }
        len++;
    }
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, len, ".trp", filename, sizeof( filename ) ) == 0 ) {
        return( DIGS_ERR_CANT_FIND_MODULE );
    }
    fp = DIGLoader( Open )( filename );
    if( fp == NULL ) {
        return( DIGS_ERR_CANT_LOAD_MODULE );
    }
    err = ReadInTrap( fp );
    DIGLoader( Close )( fp );
    if( err != DIGS_OK ) {
        return( err );
    }
    err = DIGS_ERR_BAD_MODULE_FILE;
    if( TrapCode->signature == TRAP_SIGNATURE ) {
        init_func = _MK_FP( _FP_SEG( TrapCode ), TrapCode->init_off );
        FiniFunc = _MK_FP( _FP_SEG( TrapCode ), TrapCode->fini_off );
        ReqFunc = _MK_FP( _FP_SEG( TrapCode ), TrapCode->req_off );
        *trap_ver = init_func( parms, buff, trap_ver->remote );
        err = DIGS_ERR_BUF;
        if( buff[0] == '\0' ) {
            if( TrapVersionOK( *trap_ver ) ) {
                TrapVer = *trap_ver;
                return( DIGS_OK );
            }
            err = DIGS_ERR_WRONG_MODULE_VERSION;
        }
    }
    UnLoadTrap();
    return( err );
}
