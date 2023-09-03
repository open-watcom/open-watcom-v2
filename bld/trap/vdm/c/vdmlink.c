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
#ifdef SERVER
    #include <stdlib.h>
#elif defined( __OS2__ )
    #define INCL_DOSNMPIPES
    #define INCL_DOSPROCESS
    #define INCL_DOSERRORS
    #define INCL_NOXLATE_DOS16
    #include <wos2.h>
#else   /* __WINDOWS__ or __NT__ */
    #include <windows.h>
#endif
#include "vdm.h"
#include "bool.h"
#include "watcom.h"
#include "trptypes.h"
#include "packet.h"
#include "trperr.h"
#ifdef SERVER
    #include "tinyio.h"
#elif defined( __OS2__ )
    #include "namepipe.h"
#else   /* __WINDOWS__ or __NT__ */
    #include "namepipe.h"
#endif


#ifdef __OS2__
#ifdef _M_I86
#define OS2DosConnectNPipe      DosConnectNmPipe
#define OS2DosSetNPHandState    DosSetNmPHandState
#define OS2DosDisConnectNPipe   DosDisConnectNmPipe
#define OS2DosCreateNPipe       DosMakeNmPipe
#else
#define OS2DosConnectNPipe      DosConnectNPipe
#define OS2DosSetNPHandState    DosSetNPHState
#define OS2DosDisConnectNPipe   DosDisConnectNPipe
#define OS2DosCreateNPipe       DosCreateNPipe
#endif
#endif

#if defined( __WINDOWS__ ) || defined( __NT__ )
#define WIN_PREF_LEN     (MACH_NAME_LEN + PREFIX_LEN)
#endif

#ifdef SERVER
char    pipeName[MACH_NAME_LEN + PREFIX_LEN + MAX_NAME];
int     pipeHdl = -1;
#elif defined( __OS2__ )
HPIPE   pipeHdl;
#else   /* __WINDOWS__ or __NT__ */
HANDLE  pipeHdl;
#endif


bool ValidName( const char *name )
{
    char *end;
    char *dot;

    end = memchr( name, 0, MAX_NAME );
    if( end == NULL )
        return( false );
    if( end == name )
        return( false );
    dot = memchr( name, '.', end - name );
    if( dot != NULL ) {
        if( dot == name )
            return( false );
        if( dot - name > 8 )
            return( false );
        if( end - dot > 4 )
            return( false );
        if( dot[1] == '.' || dot[2] == '.' || dot[3] == '.' ) {
            return( false );
        }
    } else {
        if( end - name > 8 ) {
            return( false );
        }
    }
    while( name < end ) {
        if( *name <= 0x1f )
            return( false );
        switch( *name ) {
        case '"':
        case '/':
        case '\\':
        case ':':
        case '*':
        case '?':
        case '-':
        case '<':
        case '>':
        case '&':
        case '|':
            return( false );
        }
        ++name;
    }
    return( true );
}

#ifdef SERVER
#ifdef TRAPGUI
const char *RemoteLinkGet( char *parms, size_t len )
{
    /* unused parameters */ (void)len;

    strcpy( parms, pipeName + PREFIX_LEN );
    return( NULL );
}
#endif
#endif

#ifdef SERVER
const char *RemoteLinkSet( const char *parms )
{
    tiny_dos_version    ver;
    char                *p;

    p = pipeName;
    ver = TinyDOSVersion();
    if( ver.major < 20 ) {
        /*
         * in NT
         */
        strcpy( p, NT_MACH_NAME );
        p += MACH_NAME_LEN;
    }
    strcpy( p, PREFIX );
    p += PREFIX_LEN;
    if( *parms == '\0' ) {
        strcpy( p, DEFAULT_LINK_NAME );
    } else if( ValidName( parms ) ) {
        strcpy( p, parms );
    } else {
        return( TRP_ERR_invalid_server_name );
    }
    return( NULL );
}
#elif defined( __OS2__ )
#else   /* __WINDOWS__ or __NT__ */
#endif


const char *RemoteLink( const char *parms, bool server )
{
#ifdef SERVER
    tiny_ret_t          rc;
#elif defined( __OS2__ )
    APIRET              rc;
#else   /* __WINDOWS__ or __NT__ */
#endif

#ifdef SERVER
    if( !server )
#else
    if( server )
#endif
        return( "this should never be seen" );

#ifdef SERVER
    if( parms != NULL ) {
        parms = RemoteLinkSet( parms );
        if( parms != NULL ) {
            return( parms );
        }
    }
    /*
     * Since we can't create the pipe ourselves, we can't reserve the
     * name.  We can at least check that there isn't a pipe with the
     * name already.  We could open a file in TMP or something... but
     * there's no reason for TMP to point to the same place in all
     * DOS boxes.  The result is that it's possible for 2 servers to
     * have the same name if neither are connected to a client.
     */
    pipeHdl = -1;
    rc = TinyOpen( pipeName, TIO_READ_WRITE );
    if( TINY_ERROR( rc ) ) {
        tiny_dos_version    ver;

        ver = TinyDOSVersion();
        if( ver.major >= 20 ) {
            /*
             * in OS/2
             */
            if( TINY_INFO( rc ) == 5 ) {
                return( TRP_ERR_server_name_already_in_use );
            }
        }
    } else {
        pipeHdl = TINY_INFO( rc );
    }
#elif defined( __OS2__ )
    strcpy( PackBuff, PREFIX );
    if( *parms == '\0' ) {
        strcpy( PackBuff + PREFIX_LEN, DEFAULT_LINK_NAME );
    } else if( ValidName( parms ) ) {
        strcpy( PackBuff + PREFIX_LEN, parms );
    } else {
        return( TRP_ERR_invalid_server_name );
    }
    rc = OS2DosCreateNPipe( PackBuff, &pipeHdl,
        NP_NOINHERIT | NP_NOWRITEBEHIND | NP_ACCESS_DUPLEX,
        NP_NOWAIT | NP_READMODE_BYTE | NP_TYPE_BYTE | 1,
        MAX_TRANS, MAX_TRANS, 0 );
    if( rc != 0 ) {
        if( rc == ERROR_PIPE_BUSY )
            return( TRP_ERR_server_name_already_in_use );
        return( TRP_ERR_unable_to_access_server );
    }
#else   /* __WINDOWS__ or __NT__ */
    strcpy( PackBuff, NT_MACH_NAME PREFIX );
    if( *parms == '\0' ) {
        strcpy( PackBuff + WIN_PREF_LEN, DEFAULT_LINK_NAME );
    } else if( ValidName( parms ) ) {
        strcpy( PackBuff + WIN_PREF_LEN, parms );
    } else {
        return( TRP_ERR_invalid_server_name );
    }
    pipeHdl = CreateNamedPipe( PackBuff,
                    PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
                    PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_NOWAIT,
                    1, MAX_TRANS, MAX_TRANS, 0, NULL );
    if( pipeHdl == INVALID_HANDLE_VALUE ) {
        if( GetLastError() == ERROR_PIPE_BUSY )
            return( TRP_ERR_server_name_already_in_use );
        return( TRP_ERR_unable_to_access_server );
    }
#endif
    return( NULL );
}


bool RemoteConnect( void )
{
#ifdef SERVER
    tiny_ret_t  rc;
#elif defined( __OS2__ )
    APIRET      rc;
    int         try;
#else   /* __WINDOWS__ or __NT__ */
    DWORD       mode;
    int         try;
#endif

    /*
     * We initially opened the pipe non-blocking so that we can poll
     * to see if the server has connected to the pipe.
     */
#ifdef SERVER
    if( pipeHdl == -1 ) {
        rc = TinyOpen( pipeName, TIO_READ_WRITE );
        if( TINY_ERROR( rc ) ) {
            /*
             * At this point if TINY_INFO( rc ) == 5 then we know that there's
             * another server with the same name.  But we have no way of
             * indicating this.
             */
            return( false );
        }
        pipeHdl = TINY_INFO( rc );
    }
    return( true );
#elif defined( __OS2__ )
    for( try = 0; try < 25; ++try ) {
        rc = OS2DosConnectNPipe( pipeHdl );
        if( rc == 0 ) {
            /*
             * Now that the server has connected we have to set the pipe
             * to blocking mode.
             */
            rc = OS2DosSetNPHandState( pipeHdl, NP_WAIT | NP_READMODE_BYTE );
            if( rc == 0 )
                return( true );
            OS2DosDisConnectNPipe( pipeHdl );
            break;
        }
        DosSleep( 200 );
    }
    return( false );
#else   /* __WINDOWS__ or __NT__ */
    for( try = 0; try < 25; ++try ) {
        SetLastError( 0 );
        ConnectNamedPipe( pipeHdl, NULL );
        if( GetLastError() == ERROR_PIPE_CONNECTED ) {
            /*
             * Now that the server has connected we have to set the pipe
             * to blocking mode.
             */
            mode = PIPE_READMODE_BYTE | PIPE_WAIT;
            if( SetNamedPipeHandleState( pipeHdl, &mode, NULL, NULL ) )
                return( true );
            DisconnectNamedPipe( pipeHdl );
            break;
        }
        Sleep( 200 );
    }
    return( false );
#endif
}


trap_retval RemoteGet( void *data, trap_elen len )
{
    unsigned_16     incoming;
#ifdef SERVER
    trap_elen       got;
    trap_elen       ret;
#elif defined( __OS2__ )
    APIRET          bytes_read;
    APIRET          ret;
#else   /* __WINDOWS__ or __NT__ */
    ULONG           bytes_read;
    trap_elen       ret;
#endif

    /* unused parameters */ (void)len;

#ifdef SERVER
    TinyRead( pipeHdl, &incoming, sizeof( incoming ) );
    ret = incoming;
    while( incoming != 0 ) {
        got = TinyRead( pipeHdl, data, incoming );
        data = (char *)data + got;
        incoming -= got;
    }
#elif defined( __OS2__ )
    DosRead( pipeHdl, &incoming, sizeof( incoming ), &bytes_read );
    ret = incoming;
    while( incoming != 0 ) {
        DosRead( pipeHdl, data, incoming, &bytes_read );
        data = (char *)data + bytes_read;
        incoming -= bytes_read;
    }
#else   /* __WINDOWS__ or __NT__ */
    ReadFile( pipeHdl, &incoming, sizeof( incoming ), &bytes_read, NULL );
    ret = incoming;
    while( incoming != 0 ) {
        ReadFile( pipeHdl, data, incoming, &bytes_read, NULL );
        data = (char *)data + bytes_read;
        incoming -= bytes_read;
    }
#endif
    return( ret );
}


trap_retval RemotePut( void *data, trap_elen len )
{
    unsigned_16 outgoing;
#ifdef SERVER
#elif defined( __OS2__ )
    APIRET      bytes_written;
#else   /* __WINDOWS__ or __NT__ */
    ULONG       bytes_written;
#endif

    outgoing = len;
#ifdef SERVER
    TinyWrite( pipeHdl, &outgoing, sizeof( outgoing ) );
    if( len > 0 ) {
        TinyWrite( pipeHdl, data, len );
    }
#elif defined( __OS2__ )
    DosWrite( pipeHdl, &outgoing, sizeof( outgoing ), &bytes_written );
    if( len > 0 ) {
        DosWrite( pipeHdl, data, len, &bytes_written );
    }
#else   /* __WINDOWS__ or __NT__ */
    WriteFile( pipeHdl, &outgoing, sizeof( outgoing ), &bytes_written, NULL );
    if( len > 0 ) {
        WriteFile( pipeHdl, data, len, &bytes_written, NULL );
    }
#endif
    return( len );
}


void RemoteDisco( void )
{
#ifdef SERVER
#elif defined( __OS2__ )
    APIRET      rc;
    APIRET      bytes_read;
    char        tmp[MAX_TRANS];
#else   /* __WINDOWS__ or __NT__ */
    BOOL        rc;
    DWORD       bytes_read;
    char        tmp[MAX_TRANS];
#endif

    /*
     * Before disconnecting we must wait until the server has closed
     * their connection.  Otherwise we might throw away the last message
     * in the pipe (usually the message that says 'close the connection').
     */
#ifdef SERVER
    TinyClose( pipeHdl );
    pipeHdl = -1;
#elif defined( __OS2__ )
    do {
        rc = DosRead( pipeHdl, tmp, MAX_TRANS, &bytes_read );
    } while( rc == 0 && bytes_read != 0 );
    OS2DosDisConnectNPipe( pipeHdl );
#else   /* __WINDOWS__ or __NT__ */
    do {
        rc = ReadFile( pipeHdl, tmp, sizeof( tmp ), &bytes_read, NULL );
    } while( rc && bytes_read != 0 );
    DisconnectNamedPipe( pipeHdl );
#endif
}


void RemoteUnLink( void )
{
#ifdef SERVER
    /* nothing */
#elif defined( __OS2__ )
    DosClose( pipeHdl );
#else   /* __WINDOWS__ or __NT__ */
    CloseHandle( pipeHdl );
#endif
}
