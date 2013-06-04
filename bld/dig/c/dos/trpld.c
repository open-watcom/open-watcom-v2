/****************************************************************************
*
*                            Open Watcom Project
*
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tinyio.h"
#include "exedos.h"
#include "trpimp.h"
#include "tcerr.h"
#include "trpld.h"

#define TRAP_SIGNATURE          0xDEAF

#define NUM_BUFF_RELOCS         16

typedef struct {
    unsigned_16         signature;
    unsigned_16         init_off;
    unsigned_16         req_off;
    unsigned_16         fini_off;
} trap_header;

extern unsigned_32      GetSystemHandle( int );
extern int              PathOpen(char *,unsigned,char *);
extern unsigned         FileClose( int );

static trap_header      far *TrapCode = NULL;
static trap_fini_func   *FiniFunc = NULL;

static char *ReadInTrap( tiny_handle_t fh )
{
    dos_exe_header      hdr;
    unsigned            size;
    unsigned            hdr_size;
    struct {
        unsigned_16     off;
        unsigned_16     seg;
    }                   buff[ NUM_BUFF_RELOCS ], far *p;
    unsigned_16         start_seg;
    unsigned_16         far *fixup;
    tiny_ret_t          ret;
    unsigned            relocs;

    hdr.signature = 0;
    if( TINY_ERROR( TinyFarRead( fh, &hdr, sizeof( hdr ) ) ) ) {
        return( TC_ERR_BAD_TRAP_FILE );
    }
    if( hdr.signature != DOS_SIGNATURE ) {
        return( TC_ERR_BAD_TRAP_FILE );
    }
    hdr_size = hdr.hdr_size * 16;
    size = (hdr.file_size * 0x200) - (-hdr.mod_size & 0x1ff) - hdr_size;
    ret = TinyAllocBlock( (size + 15) >> 4 );
    if( TINY_ERROR( ret ) ) {
        return( TC_ERR_OUT_OF_DOS_MEMORY );
    }
    start_seg = TINY_INFO( ret );
    TrapCode = MK_FP( start_seg, 0 );
    TinySeek( fh, hdr_size, TIO_SEEK_SET );
    TinyFarRead( fh, TrapCode, size );
    TinySeek( fh, hdr.reloc_offset, TIO_SEEK_SET );
    p = &buff[NUM_BUFF_RELOCS];
    for( relocs = hdr.num_relocs; relocs != 0; --relocs ) {
        if( p >= &buff[ NUM_BUFF_RELOCS ] ) {
            if( TINY_ERROR( TinyFarRead( fh, buff, sizeof( buff ) ) ) ) {
                return( TC_ERR_BAD_TRAP_FILE );
            }
            p = buff;
        }
        fixup = MK_FP( p->seg + start_seg, p->off );
        *fixup += start_seg;
        ++p;
    }
    return( NULL );
}

void KillTrap( void )
{
    ReqFunc = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( TrapCode != NULL ) {
        TinyFreeBlock( FP_SEG( TrapCode ) );
        TrapCode = NULL;
    }
}

char *LoadTrap( char *trapbuff, char *buff, trap_version *trap_ver )
{
    char            init_error[256];
    tiny_handle_t   filehndl;
    char            *ptr;
    char            *parm;
    trap_init_func  *init_func;

    if( trapbuff == NULL )
        trapbuff = "std";
    for( ptr = trapbuff; *ptr != '\0' && *ptr != ';'; ++ptr ) ;
    parm = (*ptr != '\0') ? ptr + 1 : ptr;
    filehndl = PathOpen( trapbuff, ptr - trapbuff, "trp" );
    if( filehndl <= 0 ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trapbuff );
        return( buff );
    }
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    ptr = ReadInTrap( GetSystemHandle( filehndl ) );
    FileClose( filehndl );
    if( ptr != NULL ) {
        KillTrap();
        return( ptr );
    }
    if( TrapCode->signature != TRAP_SIGNATURE ) {
        KillTrap();
        return( buff );
    }
    init_func = MK_FP( FP_SEG( TrapCode ), TrapCode->init_off );
    *trap_ver = init_func( parm, init_error, trap_ver->remote );
    if( init_error[0] != '\0' ) {
        strcpy( buff, init_error );
        KillTrap();
        return( buff );
    }
    FiniFunc = MK_FP( FP_SEG( TrapCode ), TrapCode->fini_off );
    if( !TrapVersionOK( *trap_ver ) ) {
        KillTrap();
        return( buff );
    }
    TrapVer = *trap_ver;
    ReqFunc = MK_FP( FP_SEG( TrapCode ), TrapCode->req_off );
    return( NULL );
}
