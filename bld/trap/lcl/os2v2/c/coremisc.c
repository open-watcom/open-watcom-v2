/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Support routines for core file debugging.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define INCL_DOSEXCEPTIONS
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#define INCL_DOSERRORS
#define INCL_DOSSESMGR
#define INCL_DOSMODULEMGR
#include <os2.h>
#include "trpimp.h"
#include "trpcomm.h"
#include "coremisc.h"
#include "os2trap.h"


#define OPEN_CREATE  1
#define OPEN_PRIVATE 2

#define TRPH2LH(th)     (HFILE)((th)->handle.u._32[0])
#define LH2TRPH(th,lh)  (th)->handle.u._32[0]=(unsigned_32)lh;(th)->handle.u._32[1]=0

static const ULONG      local_seek_method[] = { FILE_BEGIN, FILE_CURRENT, FILE_END };

trap_retval TRAP_FILE( get_config )( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );

    ret->file.ext_separator = '.';
    ret->file.drv_separator = ':';
    ret->file.path_separator[0] = '\\';
    ret->file.path_separator[1] = '/';
    ret->file.line_eol[0] = '\r';
    ret->file.line_eol[1] = '\n';
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Split_cmd )( void )
{
    const char          *cmd;
    const char          *start;
    split_cmd_ret       *ret;
    size_t              len;

    start = cmd = GetInPtr( sizeof( split_cmd_req ) );
    len = GetTotalSizeIn() - sizeof( split_cmd_req );
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
    while( len > 0 ) {
        switch( *cmd ) {
        case '\"':
            cmd++;
            while( --len > 0 && ( *cmd++ != '\"' ) )
                {}
            if( len == 0 )
                continue;
            switch( *cmd ) {
            CASE_SEPS
                ret->parm_start = 1;
                break;
            }
            len = 0;
            continue;
        CASE_SEPS
            ret->parm_start = 1;
            /* fall through */
        case '/':
        case '=':
        case '(':
        case ';':
        case ',':
            len = 0;
            continue;
        }
        ++cmd;
        --len;
    }
    ret->parm_start += cmd - start;
    ret->cmd_end = cmd - start;
    return( sizeof( *ret ) );
}

long OpenFile( char *name, USHORT mode, int flags )
{
    HFILE       hdl;
    ULONG       action;
    ULONG       openflags;
    ULONG       openmode;
    APIRET      rc;

    if( flags & OPEN_CREATE ) {
        openflags = OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS;
        openmode = OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE;
    } else {
        openflags = OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        openmode = OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYNONE | mode;
    }
    if( flags & OPEN_PRIVATE ) {
        openmode |= OPEN_FLAGS_NOINHERIT;
    }
    rc = DosOpen( name,         /* name */
                &hdl,           /* handle to be filled in */
                &action,        /* action taken */
                0,              /* initial allocation */
                FILE_NORMAL,    /* normal file */
                openflags,      /* open the file */
                openmode,       /* deny-none, inheritance */
                0 );            /* reserved */
    if( rc != 0 )
        return( (-1L << 16) | rc );
    return( hdl );
}

#define READONLY    0
#define WRITEONLY   1
#define READWRITE   2

trap_retval TRAP_FILE( open )( void )
{
    file_open_req       *acc;
    file_open_ret       *ret;
    unsigned_8          flags;
    long                retval;
    int                 mode;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    mode = READONLY;
    if( acc->mode & DIG_OPEN_WRITE ) {
        mode = WRITEONLY;
        if( acc->mode & DIG_OPEN_READ ) {
            mode = READWRITE;
        }
    }
    flags = OPEN_PRIVATE;
    if( acc->mode & DIG_OPEN_CREATE ) {
        flags |= OPEN_CREATE;
    }
    retval = OpenFile( GetInPtr( sizeof( file_open_req ) ), mode, flags );
    if (retval < 0) {
        ret->err = retval;
        retval = 0;
    }
    LH2TRPH( ret, retval );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( seek )( void )
{
    file_seek_req       *acc;
    file_seek_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosSetFilePtr( TRPH2LH( acc ), acc->pos, local_seek_method[acc->mode], (PULONG)&ret->pos );
    return( sizeof( *ret ) );
}


trap_retval TRAP_FILE( read )( void )
{
    ULONG               read_len;
    file_read_req       *acc;
    file_read_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosRead( TRPH2LH( acc ), GetOutPtr( sizeof( *ret ) ), acc->len, &read_len );
    return( sizeof( *ret ) + read_len );
}

trap_retval TRAP_FILE( write )( void )
{
    ULONG               written_len;
    file_write_req      *acc;
    file_write_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosWrite( TRPH2LH( acc ), GetInPtr( sizeof( *acc ) ),
                    GetTotalSizeIn() - sizeof( *acc ), &written_len );
    ret->len = written_len;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( close )( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosClose( TRPH2LH( acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( erase )( void )
{
    file_erase_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = DosDelete( GetInPtr( sizeof( file_erase_req ) ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Set_user_screen )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Set_debug_screen )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Read_user_keyboard )( void )
{
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->key = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( write_console )( void )
{
    ULONG        written_len;
    char         *ptr;
    file_write_console_ret      *ret;

    ptr = GetInPtr( sizeof( file_write_console_req ) );
    ret = GetOutPtr( 0 );
    ret->err = DosWrite( 2, ptr, GetTotalSizeIn() - sizeof( file_write_console_req ), &written_len );
    ret->len = written_len;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
{
    get_next_alias_req  *acc;
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    acc = GetInPtr( 0 );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( run_cmd )( void )
{
    char                *src;
    file_run_cmd_ret    *ret;

    src = GetInPtr( sizeof( file_run_cmd_req ) );
    ret = GetOutPtr( 0 );
    return( sizeof( *ret ) );
}

#if 0
trap_retval TRAP_CORE( Get_err_text )( void )
{
    get_err_text_req    *acc;
    char                *err_txt;

    err_txt = GetOutPtr( 0 );
    acc = GetInPtr( 0 );
    strcpy( err_txt, "Unknown error" );
    return( strlen( err_txt ) + 1 );
}
#endif
