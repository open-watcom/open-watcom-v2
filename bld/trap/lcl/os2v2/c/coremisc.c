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

#define OPEN_CREATE  1
#define OPEN_PRIVATE 2

trap_retval ReqFile_get_config( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );

    ret->file.ext_separator = '.';
    ret->file.drv_separator = ':';
    ret->file.path_separator[0] = '\\';
    ret->file.path_separator[1] = '/';
    ret->file.newline[0] = '\r';
    ret->file.newline[1] = '\n';
    return( sizeof( *ret ) );
}

long TryPath(char *name, char *end, const char *ext_list)
{
    long         rc;
    char         *p;
    int          done;
    FILEFINDBUF3 info;
    HDIR         hdl = HDIR_SYSTEM;
    ULONG        count = 1;

    done = 0;
    do {
        if (*ext_list == '\0')
            done = 1;
        for (p = end; *p = *ext_list; ++p, ++ext_list)
            ;
        count = 1;
        rc = DosFindFirst(name, &hdl, FILE_NORMAL, &info, sizeof(info), &count, FIL_STANDARD);
        if (rc == 0) {
            return 0;
        }
    } while (!done);
    return 0xffff0000 | rc;
}

long FindFilePath(char *pgm, char *buffer, const char *ext_list)
{
    char    *p;
    char    *p2;
    char    *p3;
    APIRET  rc;
    int     have_ext;
    int     have_path;

    have_ext = 0;
    have_path = 0;
    for (p = pgm, p2 = buffer; *p2 = *p; ++p, ++p2) {
        switch (*p) {
        case '\\':
        case '/':
        case ':':
            have_path = 1;
            have_ext = 0;
            break;
        case '.':
            have_ext = 1;
            break;
        }
    }
    if (have_ext)
        ext_list = "";
    rc = TryPath(buffer, p2, ext_list);
    if (rc == 0 || have_path)
        return rc;
    if (DosScanEnv("PATH", &p) != 0)
        return(rc);
    for ( ; ; ) {
        if (*p == '\0')
            break;
        p2 = buffer;
        while (*p) {
            if (*p == ';')
                break;
            *p2++ = *p++;
        }
        if (p2[-1] != '\\' && p2[-1] != '/') {
            *p2++ = '\\';
        }
        for (p3 = pgm; *p2 = *p3; ++p2, ++p3)
            ;
        rc = TryPath(buffer, p2, ext_list);
        if (rc == 0)
            break;
        if (*p == '\0')
            break;
        ++p;
    }
    return rc;
}

trap_retval ReqSplit_cmd( void )
{
    char                *cmd;
    char                *start;
    split_cmd_ret       *ret;
    unsigned            len;

    cmd = GetInPtr(sizeof(split_cmd_req));
    len = GetTotalSize() - sizeof(split_cmd_req);
    start = cmd;
    ret = GetOutPtr(0);
    ret->parm_start = 0;
    while( len != 0 ) {
        switch (*cmd) {
        case '\"':
            while( --len && ( *++cmd != '\"' ) )
                ;
            if( len != 0 )
                break;
            /* fall down */
        case '\0':
        case ' ':
        case '\t':
            ret->parm_start = 1;
            /* fall down */
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
    return sizeof(*ret);
}

static long OpenFile( char *name, USHORT mode, int flags )
{
    HFILE       hdl;
    ULONG       action;
    ULONG       openflags;
    ULONG       openmode;
    APIRET      rc;

    if( flags & OPEN_CREATE ) {
        openflags = 0x12;
        openmode = 0x2042;
    } else {
        openflags = 0x01;
        openmode = mode | 0x2040;
    }
    if( flags & OPEN_PRIVATE ) {
        openmode |= 0x80;
    }
    rc = DosOpen( name,         /* name */
                &hdl,           /* handle to be filled in */
                &action,        /* action taken */
                0,              /* initial allocation */
                0,              /* normal file */
                openflags,      /* open the file */
                openmode,       /* deny-none, inheritance */
                0 );            /* reserved */
    if( rc != 0 )
        return( 0xFFFF0000 | rc );
    return( hdl );
}

#define READONLY    0
#define WRITEONLY   1
#define READWRITE   2

trap_retval ReqFile_open( void )
{
    file_open_req       *acc;
    file_open_ret       *ret;
    unsigned_8          flags;
    long                retval;
    static int MapAcc[] = { READONLY, WRITEONLY, READWRITE };

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( acc->mode & TF_CREATE ) {
        flags = OPEN_PRIVATE | OPEN_CREATE;
        acc->mode &= ~TF_CREATE;
    } else {
        flags = OPEN_PRIVATE;
    }
    retval = OpenFile( GetInPtr( sizeof( file_open_req ) ),
                      MapAcc[acc->mode - 1], flags );
    if (retval < 0) {
        ret->err = retval;
        ret->handle = 0;
    } else {
        ret->err = 0;
        ret->handle = retval;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqFile_seek( void )
{
    file_seek_req       *acc;
    file_seek_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosSetFilePtr( acc->handle, acc->pos, acc->mode, &ret->pos );
    return( sizeof( *ret ) );
}


trap_retval ReqFile_read( void )
{
    ULONG               read_len;
    file_read_req       *acc;
    file_read_ret       *ret;
    char                *buff;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    buff = GetOutPtr( sizeof( *ret ) );
    ret->err = DosRead( acc->handle, buff, acc->len, &read_len );
    return( sizeof( *ret ) + read_len );
}

trap_retval ReqFile_write( void )
{
    ULONG               len;
    ULONG               written_len;
    char                *ptr;
    file_write_req      *acc;
    file_write_ret      *ret;

    acc = GetInPtr( 0 );
    ptr = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );
    ret = GetOutPtr( 0 );
    ret->err = DosWrite( acc->handle, ptr, len, &written_len );
    ret->len = written_len;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_close( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosClose( acc->handle );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_erase( void )
{
    file_erase_ret      *ret;

    ret = GetOutPtr(0);
    ret->err = DosDelete((char *)GetInPtr(sizeof(file_erase_req)));
    return( sizeof(*ret) );
}

trap_retval ReqSet_user_screen( void )
{
    return( 0 );
}

trap_retval ReqSet_debug_screen( void )
{
    return( 0 );
}

trap_retval ReqRead_user_keyboard( void )
{
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->key = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_write_console( void )
{
    ULONG        len;
    ULONG        written_len;
    char         *ptr;
    file_write_console_ret      *ret;

    ptr = GetInPtr( sizeof( file_write_console_req ) );
    len = GetTotalSize() - sizeof( file_write_console_req );
    ret = GetOutPtr( 0 );
    ret->err = DosWrite( 2, ptr, len, &written_len );
    ret->len = written_len;
    return( sizeof( *ret ) );
}

trap_retval ReqGet_next_alias( void )
{
    get_next_alias_req  *acc;
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    acc = GetInPtr( 0 );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_run_cmd( void )
{
    char                *src;
    file_run_cmd_ret    *ret;

    src = GetInPtr( sizeof( file_run_cmd_req ) );
    ret = GetOutPtr( 0 );
    return( sizeof( *ret ) );
}

trap_retval ReqGet_err_text( void )
{
    get_err_text_req    *acc;
    char                *err_txt;

    err_txt = GetOutPtr( 0 );
    acc = GetInPtr( 0 );
    strcpy( err_txt, "Unknown error" );
    return( strlen( err_txt ) + 1 );
}
