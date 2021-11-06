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
* Description:  Remote file access.
*
****************************************************************************/


//#define LOGGING 1

#ifdef __NT__
#include <windows.h>
#endif
#include "wio.h"
#include "dbgdefn.h"
#if !defined( BUILD_RFX )
#include "dbgdata.h"
#else
#include "rfxdata.h"
#endif
#include "dbgio.h"
#include "trpfile.h"
#include "filermt.h"
#include "filelcl.h"
#include "trapglbl.h"
#include "remfile.h"
#include "trpld.h"


#define CACHED_HANDLES    16

#define SUPP_FILE_SERVICE( in, request )    \
    in.supp.core_req    = REQ_PERFORM_SUPPLEMENTARY_SERVICE; \
    in.supp.id          = SuppFileId;       \
    in.req              = request;

#define LOC_NIL_HANDLE  ((loc_handle)-1)

typedef int             loc_handle;

/* Remote file "cache" - correlates remote and local file handles */
typedef struct _fcache_t {
    loc_handle  lochandle;
    sys_handle  remhandle;
} fcache_t;

extern trap_elen        MaxPacketLen;

file_components         RemFile;

#ifdef LOGGING
static FILE             *logf;
#endif
static trap_shandle     SuppFileId = 0;
static fcache_t         fcache[CACHED_HANDLES];
static const int        local_seek_method[] = { SEEK_SET, SEEK_CUR, SEEK_END };
static const unsigned_8 remote_seek_method[] = { TF_SEEK_ORG, TF_SEEK_CUR, TF_SEEK_END };

/* Return local handle of remote file equivalent */
static loc_handle GetCachedHandle( sys_handle remote )
{
    int i;

    for( i = 0; i < CACHED_HANDLES; i++ ) {
        if( fcache[i].remhandle.u._64[0] == remote.u._64[0] ) {
            return( fcache[i].lochandle );
        }
    }
    return( LOC_NIL_HANDLE );
}

/* Initialize local/remote handle cache */
static void InitHandleCache( void )
{
    int     i;

#ifdef LOGGING
    logf = fopen("wdrem.log", "wt");
    fprintf(logf, "InitHandleCache called\n");
#endif

    for( i = 0; i < CACHED_HANDLES; i++ ) {
        SET_SYSHANDLE_NULL( fcache[i].remhandle );
        fcache[i].lochandle = LOC_NIL_HANDLE;
    }
}

/* Add entry for local/remote "cached" file */
static int AddCachedHandle( loc_handle local, sys_handle remote )
{
    int     i;

    for( i = 0; i < CACHED_HANDLES; i++ ) {
        if( fcache[i].lochandle == LOC_NIL_HANDLE ) {
            fcache[i].remhandle = remote;
            fcache[i].lochandle = local;
            return( 0 );
        }
    }
    return( -1 );
}

/* Remove cached file entry from the list */
static int DelCachedHandle( loc_handle local )
{
    int     i;

    for( i = 0; i < CACHED_HANDLES; i++ ) {
        if( fcache[i].lochandle == local ) {
            SET_SYSHANDLE_NULL( fcache[i].remhandle );
            fcache[i].lochandle = LOC_NIL_HANDLE;
            return( 0 );
        }
    }
    return( -1 );
}

bool InitFileSupp( void )
{
    file_get_config_req acc;

    InitHandleCache();

    SuppFileId = GETSUPPID( FILE_SUPP_NAME );
    if( SuppFileId == 0 )
        return( false );
    SUPP_FILE_SERVICE( acc, REQ_FILE_GET_CONFIG );
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( RemFile ), &RemFile );
    return( true );
}

bool HaveRemoteFiles( void )
{
    return( SuppFileId != 0 );
}

//NYI: The 'bool executable' should be changed to allow different file types
size_t RemoteStringToFullName( bool executable, const char *name, char *res,
                                 trap_elen res_len )
{
    in_mx_entry         in[2];
    mx_entry            out[2];
    file_string_to_fullpath_req acc;
    file_string_to_fullpath_ret ret;
    file_handle         fh;
#ifdef __NT__
    char short_filename[MAX_PATH + 1] = "";
#endif

    if( SuppFileId == 0 ) {
        fh = LclStringToFullName( name, strlen( name ), res );
        if( fh == NIL_HANDLE )
            return( 0 );
        FileClose( fh );
        return( strlen( res ) );
    }
    SUPP_FILE_SERVICE( acc, REQ_FILE_STRING_TO_FULLPATH );
    acc.file_type = ( executable ? TF_TYPE_EXE : TF_TYPE_PRS );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = (void *)name;
#ifdef __NT__
    // check whether short filename is necessary
    switch( SysConfig.os ) {
    case DIG_OS_AUTOCAD:
    case DIG_OS_DOS:
    case DIG_OS_RATIONAL:
    case DIG_OS_PHARLAP:
    case DIG_OS_WINDOWS:
        // convert long file name to short "DOS" compatible form
        {
            GetShortPathNameA( name, short_filename, MAX_PATH );
            if( *short_filename != NULLCHAR ) {
                in[1].ptr = short_filename;
            }
        }
        break;
    }
#endif
    in[1].len = (trap_elen)( strlen( in[1].ptr ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = res;
    out[1].len = res_len;
    TrapAccess( 2, in, 2, out );
    CONV_LE_32( ret.err );
    if( ret.err != 0 ) {
        *res = NULLCHAR;
        return( 0 );
    } else {
        return( strlen( res ) );
    }
}

sys_handle RemoteOpen( const char *name, obj_attrs oattrs )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    file_open_req       acc;
    file_open_ret       ret;
    loc_handle          lochandle;
    sys_handle          sh;

    if( SuppFileId == 0 ) {
        SET_SYSHANDLE_NULL( sh );
        return( sh );
    }

    SUPP_FILE_SERVICE( acc, REQ_FILE_OPEN );
    acc.mode = 0;
    if( oattrs & OP_READ )
        acc.mode |= TF_READ;
    if( oattrs & OP_WRITE )
        acc.mode |= TF_WRITE;
    if( oattrs & OP_CREATE ) {
        acc.mode |= TF_CREATE;
        if( oattrs & OP_EXEC ) {
            acc.mode |= TF_EXEC;
        }
    }
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = (trap_elen)( strlen( name ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    CONV_LE_32( ret.err );
    CONV_LE_64( ret.handle );
    if( ret.err != 0 ) {
        StashErrCode( ret.err, OP_REMOTE );
        SET_SYSHANDLE_NULL( sh );
        return( sh );
    } else {
        /* See if the file is available locally. If so, open it here as
         * well as on the remote machine.
         */
        // TODO: check if remote file is the same!

#ifdef LOGGING
        fprintf( logf, "Trying to open local copy of remote file (remote handle %d)\n", ret.handle );
        fprintf( logf, "%s\n", name );
#endif

        if( (lochandle = open( name, O_RDONLY | O_BINARY, 0 )) != LOC_NIL_HANDLE ) {
            if( AddCachedHandle( lochandle, ret.handle ) != 0 )
                close( lochandle );
#ifdef LOGGING
            fprintf( logf, "Success\n", name );
#endif
        }
        return( ret.handle );
    }
}

static size_t doWrite( sys_handle sh, const void *buff, size_t len )
{
    in_mx_entry             in[2];
    mx_entry                out[1];
    file_write_req          acc;
    file_write_ret          ret;
    size_t                  total;
    trap_elen               piece_len;

    SUPP_FILE_SERVICE( acc, REQ_FILE_WRITE );
    acc.handle = sh;
    CONV_LE_64( acc.handle );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );

    piece_len = (trap_elen)( MaxPacketLen - sizeof( acc ) );
    total = 0;
    while( len > 0 ) {
        if( piece_len > len )
            piece_len = (trap_elen)len;
        in[1].ptr = buff;
        in[1].len = piece_len;
        TrapAccess( 2, in, 1, out );
        CONV_LE_32( ret.err );
        CONV_LE_16( ret.len );
        if( ret.err != 0 ) {
            StashErrCode( ret.err, OP_REMOTE );
            return( ERR_RETURN );
        }
        total += ret.len;
        if( ret.len != piece_len )
            break;
        buff = (char *)buff + ret.len;
        len -= ret.len;
    }
    return( total );
}

size_t RemoteWrite( sys_handle sh, const void *buff, size_t len )
{
    if( SuppFileId == 0 )
        return( 0 );

    return( doWrite( sh, buff, len ) );
}

static size_t doWriteConsole( const void *buff, size_t len )
{
    in_mx_entry             in[2];
    mx_entry                out[1];
    file_write_console_req  acc;
    file_write_console_ret  ret;
    size_t                  total;
    trap_elen               piece_len;

    SUPP_FILE_SERVICE( acc, REQ_FILE_WRITE_CONSOLE );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );

    piece_len = (trap_elen)( MaxPacketLen - sizeof( acc ) );
    total = 0;
    while( len > 0 ) {
        if( piece_len > len )
            piece_len = (trap_elen)len;
        in[1].ptr = buff;
        in[1].len = piece_len;
        TrapAccess( 2, in, 1, out );
        CONV_LE_32( ret.err );
        CONV_LE_16( ret.len );
        if( ret.err != 0 ) {
            StashErrCode( ret.err, OP_REMOTE );
            return( ERR_RETURN );
        }
        total += ret.len;
        if( ret.len != piece_len )
            break;
        buff = (char *)buff + ret.len;
        len -= ret.len;
    }
    return( total );
}

size_t RemoteWriteConsole( const void *buff, size_t len )
{
    if( SuppFileId == 0 )
        return( 0 );

    return( doWriteConsole( buff, len ) );
}

size_t RemoteWriteConsoleNL( void )
{
    if( SuppFileId == 0 )
        return( 0 );

    return( doWriteConsole( RemFile.line_eol, ( RemFile.line_eol[1] != NULLCHAR ) ? 2 : 1 ) );
}

static size_t doRead( sys_handle sh, void *buff, size_t len )
{
    in_mx_entry         in[1];
    mx_entry            out[2];
    file_read_req       acc;
    file_read_ret       ret;
    size_t              total;
    trap_elen           piece_len;
    trap_retval         read_len;

    SUPP_FILE_SERVICE( acc, REQ_FILE_READ );
    acc.handle = sh;
    CONV_LE_64( acc.handle );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );

    piece_len = (trap_elen)( MaxPacketLen - sizeof( file_read_req ) );
    total = 0;
    while( len > 0 ) {
        if( piece_len > len )
            piece_len = (trap_elen)len;
        out[1].ptr = buff;
        out[1].len = piece_len;
        acc.len = piece_len;
        CONV_LE_16( acc.len );
        read_len = (trap_retval)TrapAccess( 1, in, 2, out ) - sizeof( ret );
        CONV_LE_32( ret.err );
        if( ret.err != 0 ) {
            StashErrCode( ret.err, OP_REMOTE );
            return( ERR_RETURN );
        }
        total += read_len;
        if( read_len != piece_len )
            break;
        buff = (char *)buff + read_len;
        len -= read_len;
    }
    return( total );
}

size_t RemoteRead( sys_handle sh, void *buff, size_t len )
{
    loc_handle  lochandle;

    if( SuppFileId == 0 )
        return( 0 );

    /* Try reading from local copy first */
    lochandle = GetCachedHandle( sh );
    if( lochandle != LOC_NIL_HANDLE )
        return( read( lochandle, buff, len ) );

    return( doRead( sh, buff, len ) );
}

unsigned long RemoteSeek( sys_handle sh, unsigned long pos, seek_method method )
{
    file_seek_req       acc;
    file_seek_ret       ret;
    loc_handle          lochandle;

    if( SuppFileId == 0 )
        return( 0 );

    /* Seek on local copy too (if available) */
    lochandle = GetCachedHandle( sh );
    if( lochandle != LOC_NIL_HANDLE ) {
        lseek( lochandle, pos, local_seek_method[method] );
    }

    SUPP_FILE_SERVICE( acc, REQ_FILE_SEEK );
    acc.handle = sh;
    /* Magic again! The seek mode mapped exactly to our definition! */
    acc.mode = remote_seek_method[method];
    acc.pos = pos;
    CONV_LE_64( acc.handle );
    CONV_LE_32( acc.pos );
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    CONV_LE_32( ret.pos );
    CONV_LE_32( ret.err );
    if( ret.err != 0 ) {
        StashErrCode( ret.err, OP_REMOTE );
        return( ERR_SEEK );
    } else {
        return( ret.pos );
    }
}

error_handle RemoteClose( sys_handle sh )
{
    file_close_req      acc;
    file_close_ret      ret;
    loc_handle          lochandle;

    if( SuppFileId == 0 )
        return( 0 );

    lochandle = GetCachedHandle( sh );
    if( lochandle != LOC_NIL_HANDLE ) {
        close( lochandle );
        DelCachedHandle( lochandle );
#ifdef LOGGING
        fprintf( logf, "Closing remote file handle %d\n", sh );
#endif
    }

    SUPP_FILE_SERVICE( acc, REQ_FILE_CLOSE );
    acc.handle = sh;
    CONV_LE_64( acc.handle );
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    CONV_LE_32( ret.err );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

error_handle RemoteErase( const char *name )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    file_erase_req      acc;
    file_erase_ret      ret;

    if( SuppFileId == 0 )
        return( 0 );
    SUPP_FILE_SERVICE( acc, REQ_FILE_ERASE );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = (trap_elen)( strlen( name ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    CONV_LE_32( ret.err );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}

#if !defined( BUILD_RFX )
error_handle RemoteFork( const char *cmd, trap_elen len )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    file_run_cmd_req    acc;
    file_run_cmd_ret    ret;

    if( SuppFileId == 0 )
        return( 0 );
    SUPP_FILE_SERVICE( acc, REQ_FILE_RUN_CMD );
    acc.chk_size = CheckSize;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = (char *)cmd;
    in[1].len = len;
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    CONV_LE_32( ret.err );
    return( StashErrCode( ret.err, OP_REMOTE ) );
}
#endif
