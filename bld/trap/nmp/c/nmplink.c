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


#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include "nmp.h"
#include "packet.h"
#include "trptypes.h"
#include "trperr.h"

int     ConnHdl;
int     ReadHdl;
int     WriteHdl;
int     BindHdl = NULL;

extern          void SetLinkName( char* );

#ifdef DEBUG
    #define dbg(a) mywrite(2,a,strlen(a))
#else
    #define dbg(x)
#endif

static char     MachBuff[ MACH_NAME+1 ];
       char     NameBuff[ MAX_PIPE_NAME+1 ];
static char     *NameEnd;

static int PipeOpen( char *name )
{
    char        buff[ MAX_PIPE_NAME+1 ];
    char        *end;

    end = buff;
    if( MachBuff[0] != '\0' ) {
        buff[0] = '\\';
        buff[1] = '\\';
        strcpy( buff+2, MachBuff );
        end = buff + strlen( buff );
    }
    strcpy( end, name );
    return( myopen( buff ) );
}



static char *OpenRequest()
{
    unsigned short      bytes;

    BindHdl = PipeOpen( BINDERY );
    if( BindHdl == -1 ) return( TRP_ERR_NMPBIND_not_found );
    NameBuff[0] = OPEN_REQUEST;
    bytes = mywrite( BindHdl, NameBuff, strlen( NameBuff )+1 );
    if( bytes == 0 ) return( TRP_ERR_NMPBIND_not_found );
    bytes = myread( BindHdl, NameBuff, 1 );
    if( bytes == 0 ) return( TRP_ERR_NMPBIND_not_found );
    myclose( BindHdl );
    return( NULL );
}


void DoOpen( int *phdl, char *suff )
{
    strcpy( NameEnd, suff );
    dbg( "DoOpen " );
    dbg( NameBuff+1 );
    dbg( "\r\n" );
    for( ;; ) {
        *phdl = PipeOpen( NameBuff+1 );
        if( *phdl != -1 ) break;
        mysnooze();
    }
    *NameEnd = '\0';
}


char *ValidName( char *name )
{
    int         len;

    len = 0;
    MachBuff[ 0 ] = '\0';
    while( *name ) {
        if( *name == '@' ) {
            strcpy( MachBuff, name+1 );
            return( name );
            break;
        }
        if( !isalnum( *name ) ) return( NULL );
        ++name;
        ++len;
    }
    return( name );
}

char    DefLinkName[] = DEFAULT_NAME;

char *RemoteLink( char *config, char server )
{
    char        *msg;
    char        *end;

    server=server;
    end = ValidName( config );
    strcpy( NameBuff+1, PREFIX );
    if( end == NULL ) {
        return( TRP_ERR_invalid_server_name_format_is );
    } else {
        if( end == config ) {
            strcpy( NameBuff+1 + PREFIX_LEN, DefLinkName );
        } else {
            memcpy( NameBuff+1 + PREFIX_LEN, config, end-config );
            NameBuff[ end-config+1+PREFIX_LEN ] = '\0';
        }
    }
    msg = OpenRequest();
    NameEnd = NameBuff + strlen( NameBuff );
    if( msg != NULL ) return( msg );
    if( NameBuff[0] != BIND_ACK ) {
        #ifdef SERVER
            return( TRP_ERR_server_name_already_in_use );
        #else
            return( TRP_ERR_server_not_found );
        #endif
    } else {
        DoOpen( &ConnHdl, CONN_SUFF );
    }
    return( NULL );
}


static void ConnRequest( char request )
{
    int         bytes;

    NameBuff[0] = request;
    bytes = mywrite( ConnHdl, NameBuff, strlen( NameBuff )+1 );
    bytes = myread( ConnHdl, NameBuff, 1 );
}

char RemoteConnect( void )
{
    ConnRequest( CONNECT_REQUEST );
    if( NameBuff[0] == BIND_ACK ) {
        DoOpen( &ReadHdl, READ_SUFF );
        DoOpen( &WriteHdl, WRITE_SUFF );
        ConnRequest( CONNECT_DONE );
        return( 1 );
    }
    return( 0 );
}


unsigned RemoteGet( void *data, unsigned length )
{
    unsigned short      bytes_read;
    unsigned short      tmp;

    bytes_read = myread( ReadHdl, data, length );
    switch( bytes_read ) {
    case 0:
        return( REQUEST_FAILED );
    case 1:
        tmp = myread( ReadHdl, (char*)&bytes_read, sizeof( unsigned short ) );
        if( tmp != sizeof( unsigned short ) ) return( REQUEST_FAILED );
        break;
    }
    return( bytes_read );
}


unsigned RemotePut( void *data, unsigned length )
{
    unsigned short      bytes_written;
    unsigned short      real_length;

    real_length = length;
    if( length == 0 ) length = 1;       /* Can't write zero bytes */
    bytes_written = mywrite( WriteHdl, data, length );
    if( bytes_written != length ) return( REQUEST_FAILED );
    if( length == 1 ) {
        /* Send true length through */
        bytes_written = mywrite( WriteHdl, (char*)&real_length, sizeof( unsigned short ) );
        if( bytes_written != sizeof( unsigned short ) ) return( REQUEST_FAILED );
    }
    return( length );
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
