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
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <io.h>
#include <process.h>
#include <i86.h>
#include "trpimp.h"
#include "packet.h"
#ifdef ACAD
    extern void LetACADDie(void);
    #include "adslib.h"
    #define _DBG(x) // printf x; fflush( stdout );
#else
#ifdef DEBUG_TRAP
#define _DBG1( x ) printf x ; fflush( stdout )
#define _DBG( x ) printf x ; fflush( stdout )
#else
#define _DBG1( x ) // cputs x
#define _DBG( x )
#endif
#endif


static trap_version     TrapVer;

char    RWBuff[ 0x400 ];

extern void         Output( char * );
extern void         SayGNiteGracey( int );
extern void         StartupErr(char *);

static mx_entry     In[1];
static mx_entry     Out[1];


static void AccTrap( bool want_return )
{
    if( want_return ) {
        PutBuffPacket( TrapRequest( 1, &In, 1, &Out ), RWBuff );
    } else {
        TrapRequest( 1, &In, 0, NULL );
    }
}


static bool Session( void )
{
    unsigned    req;
    bool        want_return;

    for( ;; ) {
        In[0].len = GetPacket();
        In[0].ptr = GetPacketBuffPtr();
        _DBG(("Session got request "));
        req = *(access_req *)In[0].ptr;
        *(access_req *)In[0].ptr &= ~0x80;
        if( req & 0x80 ) {
            req &= ~0x80;
            want_return = FALSE;
        } else {
            want_return = TRUE;
        }
        switch( req ) {
        case REQ_PROG_KILL:
            _DBG(("REQ_KILL_PROG\n"));
            AccTrap( TRUE );
            return( TRUE );
        default:
            _DBG(("AccTrap\n"));
            AccTrap( want_return );
            _DBG(("Done AccTrap\n"));
            break;
        }
    }
}


void Initialize( void )
{
    char        *err;

    RWBuff[0] = '\0';
    _DBG(("About to remote link in initialize.\n" ));
    err = RemoteLink( RWBuff, 1 );
    _DBG(( "Back from PM remote link\n" ));
    if( err ) {
        _DBG(( "ERROR! '%s'\n", err ));
    }
    if( err != NULL ) {
    #ifdef ACAD
        for( ;; ) ads_link( RSERR );
    #else
        StartupErr( err );
    #endif
    }
    _DBG(( "No Remote link error. About to TrapInit." ));
    TrapVer = TrapInit( NULL, RWBuff, FALSE );
    if( RWBuff[0] != '\0' ) {
// NO, NO, NO!  RemoteUnLink();
        StartupErr( RWBuff );
    }
    _DBG(( "No TrapInit error. Initialize complete" ));
    Out[0].len = sizeof( RWBuff );
    Out[0].ptr = RWBuff;
}


#if defined(ACAD)

int main( int argc, char **argv )
{
    _DBG( ( "Calling ads_init()\r\n" ) );
    ads_init( argc, argv );
    _DBG( ( "After ads_init()\r\n" ) );

#else

int main( void )
{

#endif

    _DBG(("Calling Initialize\n"));
    Initialize();
    _DBG(("Calling RemoteConnect\n"));
    RemoteConnect();
    _DBG(("Calling Session\n"));
    Session();
    _DBG(("Calling TrapFini\n"));
    TrapFini();
    _DBG(("Calling RemoteDisco\n"));
    RemoteDisco();
    _DBG(("Calling RemoteUnLink\n"));
    RemoteUnLink();
    _DBG(("After calling RemoteUnLink\n"));
#ifdef ACAD
    LetACADDie();
#endif
    return( 0 );
}
