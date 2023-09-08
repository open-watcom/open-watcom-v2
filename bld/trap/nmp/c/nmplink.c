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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#if defined( __OS2__ )
#include <wos2.h>
#elif defined( __NT__ ) || defined( __WINDOWS__ )
#include <windows.h>
#endif
#include "nmp.h"
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"


#ifdef DEBUG
    #define dbg(a) mywrite(BHANDLE_STDERR,a,strlen(a))
#else
    #define dbg(x)
#endif

bhandle ConnHdl;
bhandle ReadHdl;
bhandle WriteHdl;
bhandle BindHdl;

static char     MachBuff[MACH_NAME + 1];
static char     NameBuff[MAX_PIPE_NAME + 1];
static char     *NameEnd;

static bhandle PipeOpen( char *name )
{
    char        buff[MAX_PIPE_NAME + 1];
    char        *end;

    end = buff;
    if( MachBuff[0] != '\0' ) {
        buff[0] = '\\';
        buff[1] = '\\';
        strcpy( buff + 2, MachBuff );
        end = buff + strlen( buff );
    }
    strcpy( end, name );
    return( myopen( buff ) );
}


static const char *OpenRequest( void )
{
    trap_elen  bytes;

    BindHdl = PipeOpen( BINDERY );
    if( BindHdl == BHANDLE_INVALID )
        return( TRP_ERR_NMPBIND_not_found );
    NameBuff[0] = OPEN_REQUEST;
    bytes = mywrite( BindHdl, NameBuff, strlen( NameBuff ) + 1 );
    if( bytes == 0 )
        return( TRP_ERR_NMPBIND_not_found );
    bytes = myread( BindHdl, NameBuff, 1 );
    if( bytes == 0 )
        return( TRP_ERR_NMPBIND_not_found );
    myclose( BindHdl );
    return( NULL );
}


static void DoOpen( bhandle *phdl, char *suff )
{
    strcpy( NameEnd, suff );
    dbg( "DoOpen " );
    dbg( NameBuff + 1 );
    dbg( "\r\n" );
    for( ;; ) {
        *phdl = PipeOpen( NameBuff + 1 );
        if( *phdl != BHANDLE_INVALID )
            break;
        mysnooze();
    }
    *NameEnd = '\0';
}


static const char *ValidName( const char *name )
{
    int         len;

    len = 0;
    MachBuff[0] = '\0';
    while( *name != '\0' ) {
        if( *name == '@' ) {
            strcpy( MachBuff, name + 1 );
            return( name );
        }
        if( !isalnum( *name ) )
            return( NULL );
        ++name;
        ++len;
    }
    return( name );
}

#ifdef SERVER
#ifdef TRAPGUI
const char *RemoteLinkGet( char *parms, size_t len )
{
    *NameEnd = '\0';
    strcpy( parms, NameBuff + 1 + PREFIX_LEN );
    if( *MachBuff != '\0' ) {
        len = NameEnd - ( NameBuff + 1 + PREFIX_LEN );
        parms[len++] = '@';
        strcpy( parms + len, MachBuff );
    }
    return( NULL );
}
#endif
#endif

const char *RemoteLinkSet( const char *parms )
{
    const char  *end;

    end = ValidName( parms );
    strcpy( NameBuff + 1, PREFIX );
    if( end == NULL ) {
        return( TRP_ERR_invalid_server_name_format_is );
    } else {
        if( end == parms ) {
            strcpy( NameBuff + 1 + PREFIX_LEN, DEFAULT_LINK_NAME );
        } else {
            memcpy( NameBuff + 1 + PREFIX_LEN, parms, end - parms );
            NameBuff[end - parms + 1 + PREFIX_LEN] = '\0';
        }
    }
    NameEnd = NameBuff + strlen( NameBuff );
    return( NULL );
}


const char *RemoteLink( const char *parms, bool server )
{
    const char  *err;

    /* unused parameters */ (void)server;

    err = NULL;
    if( parms != NULL ) {
        err = RemoteLinkSet( parms );
    }
    if( err == NULL ) {
        err = OpenRequest();
        if( err == NULL ) {
            if( NameBuff[0] != BIND_ACK ) {
#ifdef SERVER
                err = TRP_ERR_server_name_already_in_use;
#else
                err = TRP_ERR_server_not_found;
#endif
            } else {
                DoOpen( &ConnHdl, CONN_SUFF );
            }
        }
    }
    return( err );
}


static void ConnRequest( char request )
{
    int         bytes;

    NameBuff[0] = request;
    bytes = mywrite( ConnHdl, NameBuff, strlen( NameBuff ) + 1 );
    bytes = myread( ConnHdl, NameBuff, 1 );
}

bool RemoteConnect( void )
{
    ConnRequest( CONNECT_REQUEST );
    if( NameBuff[0] == BIND_ACK ) {
        DoOpen( &ReadHdl, READ_SUFF );
        DoOpen( &WriteHdl, WRITE_SUFF );
        ConnRequest( CONNECT_DONE );
        return( true );
    }
    return( false );
}


trap_retval RemoteGet( void *data, trap_elen len )
{
    trap_elen      bytes_read;
    trap_elen      tmp;

    bytes_read = myread( ReadHdl, data, len );
    switch( bytes_read ) {
    case 0:
        return( REQUEST_FAILED );
    case 1:
        tmp = myread( ReadHdl, &bytes_read, sizeof( trap_elen ) );
        if( tmp != sizeof( trap_elen ) )
            return( REQUEST_FAILED );
        break;
    }
    return( bytes_read );
}


trap_retval RemotePut( void *data, trap_elen len )
{
    trap_elen  bytes_written;
    trap_elen  real_len;

    real_len = len;
    if( len == 0 )
        len = 1;       /* Can't write zero bytes */
    bytes_written = mywrite( WriteHdl, data, len );
    if( bytes_written != len )
        return( REQUEST_FAILED );
    if( len == 1 ) {
        /* Send true length through */
        bytes_written = mywrite( WriteHdl, &real_len, sizeof( trap_elen ) );
        if( bytes_written != sizeof( trap_elen ) ) {
            return( REQUEST_FAILED );
        }
    }
    return( len );
}


void RemoteDisco( void )
{
    ConnRequest( DISCO_REQUEST );
    myclose( ReadHdl );
    myclose( WriteHdl );
}


void RemoteUnLink( void )
{
    myclose( ConnHdl );
}
