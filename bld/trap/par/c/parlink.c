/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Parallel link remote communications core.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__NETWARE__)
#include <dos.h>
#else
#include <i86.h>
#endif
#include "bool.h"
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"
#include "nothing.h"
#include "parlink.h"
#include "portio.h"
#include "pardata.h"


#if defined(_DBG)
    #define my_inp(x)       dbg_inp(hwd->controller.port + x)
    #define my_outp(x,y)    dbg_outp(hwd->controller.port + x,y)
#else
    #define my_inp(x)       inp(hwd->controller.port + x)
    #define my_outp(x,y)    outp(hwd->controller.port + x,y)
#endif

#if !defined(_DBG)
    #define dbgrtn(x)
#elif defined( SERVER )
    #define dbgrtn(x)       printf( x )
#else
    #include <conio.h>
    #define dbgrtn(x)       cputs( x )
#endif

static hw_data  hwdata;

static char     InvalidPort[] = TRP_ERR_invalid_parallel_port_number;

#if defined(_DBG)
char dbg_inp( int port )
{
    char x;

    x = inp( port );
  #ifdef SERVER
    printf( "in %x=%2.2x ", port, x );
  #else
    {
        char buf[10];

        itoa( port, buf, 16 );
        cputs( " in " ); cputs( buf );
        itoa( x, buf, 16 );
        cputs( "=" ); cputs( buf );
    }
  #endif
    return( x );
}

void dbg_outp( int port, char x )
{
    outp( port, x );
  #ifdef SERVER
    printf( "out %x=%2.2x ", port, x );
  #else
    {
        char buf[10];

        itoa( port, buf, 16 );
        cputs( " out " ); cputs( buf );
        itoa( x, buf, 16 );
        cputs( "=" ); cputs( buf );
    }
  #endif
}
#endif

#ifdef _WIN64
#include "parpro64.c"
#else
#include "parproc.c"
#endif

trap_retval RemoteGet( void *data, trap_elen len )
{
    return( DataGet( &hwdata, data, len ) );
}

trap_retval RemotePut( void *data, trap_elen len )
{
    return( DataPut( &hwdata, data, len ) );
}

bool RemoteConnect( void )
{
#ifdef SERVER
    return( DataConnect( &hwdata, true ) );
#else
    return( DataConnect( &hwdata, false ) );
#endif
}

void RemoteDisco( void )
{
    DataDisconnect( &hwdata );
}

#ifdef SERVER
#ifdef TRAPGUI
const char *RemoteLinkGet( char *parms, size_t len )
{
    int     num;

    /* unused parameters */ (void)len;

    num = NumPrinters();
    if( num == 0 )
        return( TRP_ERR_parallel_port_not_present );
    if( num >= 1 && PrnAddress( 0 ) == hwdata.controller.port ) {
        parms[0] = '1';
        parms[1] = '\0';
    } else if( num >= 2 && PrnAddress( 1 ) == hwdata.controller.port ) {
        parms[0] = '2';
        parms[1] = '\0';
    } else if( num >= 3 && PrnAddress( 2 ) == hwdata.controller.port ) {
        parms[0] = '3';
        parms[1] = '\0';
    } else {
        sprintf( parms, "P%X", hwdata.controller.port );
    }
    return( NULL );
}
#endif
#endif

const char *RemoteLinkSet( const char *parms )
{
    int             printer;
    unsigned short  port;
    char            ch;

    ch = *parms++;
    port = 0;
    if( ch == '\0' || ch >= '1' && ch <= '3' && *parms == '\0' ) {
        if( ch == '\0' )
            ch = '1';
        printer = ch - '1';
        if( NumPrinters() <= printer ) {
            return( TRP_ERR_parallel_port_not_present );
        }
        port = PrnAddress( printer );
    } else if( ch == 'p' || ch == 'P' ) {
        for( ;; ) {
            ch = *parms++;
            if( ch == 0 )
                break;
            if( ch == ' ' )
                break;
            if( ch == '\t' )
                break;
            if( ch >= 'A' && ch <= 'F' ) {
                ch = ch - 'A' + 0x0a;
            } else if( ch >= 'a' && ch <= 'f' ) {
                ch = ch - 'a' + 0x0a;
            } else if( ch >= '0' && ch <= '9' ) {
                ch = ch - '0';
            } else {
                return( InvalidPort );
            }
            port <<= 4;
            port += ch;
        }
        if( port == 0 ) {
            return( InvalidPort );
        }
    } else {
        return( InvalidPort );
    }
    hwdata.controller.port = port;
    return( NULL );
}

const char *RemoteLink( const char *parms, bool server )
{
    const char  *err;

    /* unused parameters */ (void)server;

    dbgrtn( "\r\n-RemoteLink-" );
    err = InitSys();
    if( err != NULL ) {
        return( err );
    }
    if( parms != NULL ) {
        err = RemoteLinkSet( parms );
        if( err != NULL ) {
            return( err );
        }
    }
    if( !AccessPorts( hwdata.controller.port, 3 ) ) {
        return( TRP_ERR_cannot_access_parallel_ports );
    }
    DataReset( &hwdata, false );
    return( NULL );
}

void RemoteUnLink( void )
{
    FiniSys();
    FreePorts( hwdata.controller.port, 3 );
    hwdata.controller.port = 0;
}
