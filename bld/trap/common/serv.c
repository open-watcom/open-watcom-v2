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
#include "tcerr.h"


extern trap_version     TrapVersion;
static bool             OneShot;

extern char             RWBuff[ 0x400 ];
extern char             ServName[];

extern void             Output( char * );
extern void             SayGNiteGracey( int );
extern void             StartupErr(char *);
extern int              KeyPress(void);
extern int              KeyGet(void);
extern char             *LoadTrap( char *,char *,trap_version *);
extern void             KillTrap(void);
extern void             NothingToDo(void);
extern bool             Session( void );
extern bool             ParseCommandLine( char *cmdline, char *trap, char *parm, bool *oneshot );

void ServError( char *msg )
{
    Output( msg );
    Output( "\r\n" );
}

void ServMessage( char *msg )
{
    ServError( msg );
}

void Initialize( void )
{

    char        *err;
    char        trap[ 128 ];
    char        cmdline[ 256 ];

    getcmd( cmdline );
    ParseCommandLine( cmdline, trap, RWBuff, &OneShot );
    err = RemoteLink( RWBuff, 1 );
    if( err == NULL ) {
        err = LoadTrap( trap[0] == '\0' ? NULL : trap, RWBuff, &TrapVersion );
    }
    if( err != NULL ) {
        StartupErr( err );
    }
}

void OpeningStatement( void )
{
    Output( "Open Watcom " );
    Output( ServName );
    Output( " Version " _XXXSERV_VERSION_ "\r\n" );
    Output( banner2( "1988" ) "\r\n" );
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
            while( KeyPress() ) KeyGet(); /* flush keyboard input */
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
