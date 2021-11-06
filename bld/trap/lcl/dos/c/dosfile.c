/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Low-level trap file I/O for DOS.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "tinyio.h"
#include "dosver.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "dosenv.h"
#include "doschk.h"
#include "dosextx.h"
#include "dosfile.h"


#define TRPH2LH(th)     (tiny_handle_t)((th)->handle.u._32[0])
#define LH2TRPH(th,lh)  (th)->handle.u._32[0]=(unsigned_32)lh;(th)->handle.u._32[1]=0

/* fork.asm prototype */
extern tiny_ret_t   __near Fork( const char __far *, unsigned );

const char DosExtList[] = DOSEXTLIST;

static const seek_info  local_seek_method[] = { TIO_SEEK_SET, TIO_SEEK_CUR, TIO_SEEK_END };

trap_retval TRAP_FILE( get_config )( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );

    ret->file.ext_separator = '.';
    ret->file.path_separator[0] = '\\';
    ret->file.path_separator[1] = '/';
    ret->file.path_separator[2] = ':';
    ret->file.line_eol[0] = '\r';
    ret->file.line_eol[1] = '\n';
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( open )( void )
{
    tiny_ret_t      rc;
    int             mode;
    char            *filename;
    file_open_req   *acc;
    file_open_ret   *ret;
    static int MapAcc[] = { TIO_READ, TIO_WRITE, TIO_READ_WRITE };

    acc = GetInPtr( 0 );
    filename = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    if( acc->mode & TF_CREATE ) {
        rc = TinyCreate( filename, TIO_NORMAL );
    } else {
        mode = MapAcc[acc->mode - 1];
        if( IsDOS3 )
            mode |= 0x80; /* set no inheritance */
        rc = TinyOpen( filename, mode );
    }
    LH2TRPH( ret, TINY_INFO( rc ) );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( seek )( void )
{
    tiny_ret_t      rc;
    file_seek_req   *acc;
    file_seek_ret   *ret;
    uint_32         pos;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    rc = TinyLSeek( TRPH2LH( acc ), acc->pos, local_seek_method[acc->mode], &pos );
    ret->pos = pos;
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( read )( void )
{
    tiny_ret_t      rc;
    file_read_req   *acc;
    file_read_ret   *ret;
    char            *buff;
    unsigned        len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    buff = GetOutPtr( sizeof( *ret ) );
    rc = TinyRead( TRPH2LH( acc ), buff, acc->len );
    if( TINY_ERROR( rc ) ) {
        ret->err = TINY_INFO( rc );
        len = 0;
    } else {
        ret->err = 0;
        len = TINY_INFO( rc );
    }
    return( sizeof( *ret ) + len );
}

trap_retval TRAP_FILE( write )( void )
{
    tiny_ret_t      rc;
    file_write_req  *acc;
    file_write_ret  *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    rc = TinyWrite( TRPH2LH( acc ), GetInPtr( sizeof( *acc ) ), ( GetTotalSizeIn() - sizeof( *acc ) ) );
    ret->len = TINY_INFO( rc );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( write_console )( void )
{
    tiny_ret_t              rc;
    file_write_console_ret  *ret;

    ret = GetOutPtr( 0 );
    rc = TinyWrite( TINY_ERR, GetInPtr( sizeof( file_write_console_req ) ), ( GetTotalSizeIn() - sizeof( file_write_console_req ) ) );
    ret->len = TINY_INFO( rc );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( close )( void )
{
    tiny_ret_t      rc;
    file_close_req  *acc;
    file_close_ret  *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    rc = TinyClose( TRPH2LH( acc ) );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( erase )( void )
{
    tiny_ret_t      rc;
    file_erase_ret  *ret;

    ret = GetOutPtr( 0 );
    rc = TinyDelete( (char *)GetInPtr( sizeof( file_erase_req ) ) );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    return( sizeof( *ret ) );
}

static tiny_ret_t TryPath( const char *name, char *end, const char *ext_list )
{
    tiny_ret_t  rc;
    char        *p;
    int         done;
    int         mode;

    done = 0;
    mode = 0 ; //IsDOS3 ? 0x40 : 0;
    do {
        if( *ext_list == '\0' ) done = 1;
        for( p = end; *p = *ext_list; ++p, ++ext_list )
            {}
        rc = TinyOpen( name, mode );
        if( TINY_OK( rc ) ) {
            TinyClose( TINY_INFO( rc ) );
            return( rc );
        }
        ++ext_list;
    } while( !done );
    return( rc );
}

unsigned long FindProgFile( const char *pgm, char *buffer, const char *ext_list )
{
    const char  __far *path;
    char        *p2;
    const char  *p3;
    tiny_ret_t  rc;
    int         have_ext;
    int         have_path;

    have_ext = 0;
    have_path = 0;
    for( p3 = pgm, p2 = buffer; (*p2 = *p3) != '\0'; ++p3, ++p2 ) {
        switch( *p3 ) {
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
    if( have_ext )
        ext_list = "";
    rc = TryPath( buffer, p2, ext_list );
    if( TINY_OK( rc ) || have_path )
        return( rc );
    path = DOSEnvFind( "PATH" );
    if( path == NULL )
        return( rc );
    for( ;; ) {
        if( *path == '\0' )
            break;
        p2 = buffer;
        while( *path ) {
            if( *path == ';' )
                break;
            *p2++ = *path++;
        }
        if( p2[-1] != '\\' && p2[-1] != '/' ) {
            *p2++ = '\\';
        }
        for( p3 = pgm; *p2 = *p3; ++p2, ++p3 )
            {}
        rc = TryPath( buffer, p2, ext_list );
        if( TINY_OK( rc ) )
            break;
        if( *path == '\0' )
            break;
        ++path;
    }
    return( rc );
}


trap_retval TRAP_FILE( string_to_fullpath )( void )
{
    char                        *name;
    char                        *fullname;
    const char                  *ext_list;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;
    tiny_ret_t                  rc;

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    if( acc->file_type == TF_TYPE_EXE ) {
        ext_list = DosExtList;
    } else {
        ext_list = "";
    }
    rc = FindProgFile( name, fullname, ext_list );
    if( TINY_OK( rc ) ) {
        ret->err = 0;
    } else {
        ret->err = TINY_INFO( rc );
        *fullname = '\0';
    }
    return( sizeof( *ret ) + 1 + strlen( fullname ) );
}

trap_retval TRAP_FILE( run_cmd )( void )
{
    file_run_cmd_ret    *ret;
#if defined(__WINDOWS__)

    ret = GetOutPtr( 0 );
    ret->err = 0;
#else
    bool                chk;
    char                buff[_MAX_PATH];
    file_run_cmd_req    *acc;
    unsigned            len;
    tiny_ret_t          rc;

    acc = GetInPtr( 0 );
    len = GetTotalSizeIn() - sizeof( *acc );
    ret = GetOutPtr( 0 );

    chk = CheckPointMem( ON_DISK, acc->chk_size, buff );
    rc = Fork( GetInPtr( sizeof( *acc ) ), len );
    if( chk )
        CheckPointRestore( ON_DISK );
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
#endif
    return( sizeof( *ret ) );
}
