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
* Description:  Mainline for remote debug servers.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#if defined(__WATCOMC__)
    #include <process.h>
#else
    #include "clibext.h"
#endif
#if defined(__AXP__) && defined(__NT__)
    #include <windows.h>
#endif
#include "banner.h"
#include "trpimp.h"
#include "trperr.h"
#include "packet.h"
#include "servname.h"
#include "tcerr.h"
#include "servio.h"
#include "nothing.h"


extern trap_version     TrapVersion;

static bool             OneShot;

void ServError( const char *msg )
{
    Output( msg );
    Output( "\r\n" );
}

void ServMessage( const char *msg )
{
    ServError( msg );
}

void Initialize( void )
{

    const char  *err;
    char        trapparms[PARMS_MAXLEN];
    char        cmdline[PARMS_MAXLEN];

#define servparms RWBuff

    getcmd( cmdline );
    ParseCommandLine( cmdline, trapparms, servparms, &OneShot );
    err = RemoteLink( servparms, TRUE );

#undef servparms

    if( err == NULL ) {
        err = LoadTrap( trapparms, RWBuff, &TrapVersion );
    }
    if( err != NULL ) {
        StartupErr( err );
    }
}

void OpeningStatement( void )
{
#if defined( VERSION_ON_EXTRA_LINE )
    Output( banner1w1( SERVNAME ) "\r\n" );
    Output( banner1w2( _XXXSERV_VERSION_ ) "\r\n" );
#else
    Output( banner1w( SERVNAME, _XXXSERV_VERSION_ ) "\r\n" );
#endif
    Output( banner2 "\r\n" );
    Output( banner2a( "1988" ) "\r\n" );
    Output( banner3 "\r\n" );
    Output( banner3a "\r\n" );
}

int main( int argc, char **argv )
{
    char key;

#ifndef __WATCOMC__
    _argc = argc;
    _argv = argv;
#endif

    Initialize();
    OpeningStatement();
    for( ;; ) {
        Output( TRP_MSG_press_q );
        Output( "\r\n" );
        for( ;; ) {
            if( RemoteConnect() ) break;
            NothingToDo();
            if( KeyPress() ) {
                key = KeyGet();
                if( key == 'q' || key == 'Q' ) {
                    KillTrap();
                    RemoteUnLink();
                    SayGNiteGracey( 0 );
                }
            }
        }
        Output( TRP_MSG_session_started );
        Output( "\r\n\r\n" );
        Session();
#ifndef NETWARE
        /* flush keyboard input */
        while( KeyPress() )
            KeyGet();
#endif
        Output( "\r\n\r\n" );
        Output( TRP_MSG_session_ended );
        Output( "\r\n\r\n" );
        RemoteDisco();
        if( OneShot ) {
            KillTrap();
            RemoteUnLink();
            SayGNiteGracey( 0 );
        }
    }
    return( 0 );
}
