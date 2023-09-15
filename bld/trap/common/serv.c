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
* Description:  Mainline for remote debug servers.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#if defined(__WATCOMC__)
    #include <process.h>
#endif
#if defined( __WINDOWS__ ) || defined( __NT__ )
  #include <windows.h>
#endif
#include "banner.h"
#include "trpld.h"
#include "trperr.h"
#include "packet.h"
#include "servname.rh"
#include "servio.h"
#include "nothing.h"

#include "clibext.h"


extern trap_version     TrapVersion;

static bool             OneShot;

void ServError( const char *msg )
{
    OutputLine( msg );
}

void ServMessage( const char *msg )
{
    ServError( msg );
}

static const char *ServInitialize( void )
{
    const char  *err;
    char        trapparms[PARMS_MAXLEN];
    char        cmd_line[PARMS_MAXLEN];

    _bgetcmd( cmd_line, sizeof( cmd_line ) );
    err = ParseCommandLine( cmd_line, trapparms, RWBuff, &OneShot );
    if( err == NULL ) {
        err = RemoteLink( RWBuff, true );
        if( err == NULL ) {
            switch( LoadTrap( trapparms, RWBuff, &TrapVersion ) ) {
            #define DIGS_ERROR(e,t) case e: err = t; break;
            DIGS_ERRORS( "TRAP Loader: ", RWBuff )
            #undef DIGS_ERROR
            default: err = DIGS_ERRORS_default( "TRAP Loader: " ); break;
            }
        }
    }
    return( err );
}

static void OpeningStatement( void )
{
    OutputLine( banner1t( SERVNAME ) );
    OutputLine( banner1v( _XXXSERV_VERSION_ ) );
    OutputLine( banner2 );
    OutputLine( banner2a( 1988 ) );
    OutputLine( banner3 );
    OutputLine( banner3a );
}

int main( int argc, char **argv )
{
    char        key;
    const char  *err;

#ifndef __WATCOMC__
    _argc = argc;
    _argv = argv;
#else
    /* unused parameters */ (void)argc; (void)argv;
#endif

    err = ServInitialize();
    if( err != NULL ) {
        StartupErr( err );
        return( 1 );
    }
    OpeningStatement();
    for( ;; ) {
        OutputLine( TRP_MSG_press_q );
        for( ;; ) {
            if( RemoteConnect() )
                break;
            NothingToDo();
            if( KeyPress() ) {
                key = KeyGet();
                if( key == 'q' || key == 'Q' ) {
                    UnLoadTrap();
                    RemoteUnLink();
                    ServTerminate( 0 );
                    // never return
                }
            }
        }
        OutputLine( TRP_MSG_session_started );
        OutputLine( "" );
        Session();
#ifndef NETWARE
        /* flush keyboard input */
        while( KeyPress() )
            KeyGet();
#endif
        OutputLine( "" );
        OutputLine( "" );
        OutputLine( TRP_MSG_session_ended );
        OutputLine( "" );
        RemoteDisco();
        if( OneShot ) {
            UnLoadTrap();
            RemoteUnLink();
            ServTerminate( 0 );
            // never return
        }
    }
    return( 0 );
}
