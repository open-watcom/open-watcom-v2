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
#include <conio.h>
#include <stdlib.h>
#include <ctype.h>
#include <io.h>
#include <process.h>
#include <i86.h>
#include "trpimp.h"
#include "trpcomm.h"
#include "packet.h"
#include "servio.h"
#include "dosxlink.h"
#ifdef ACAD
    #include "adsacc.h"
    #include "adslib.h"
#endif


#ifdef ACAD

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

char    RWBuff[0x400];

static in_mx_entry  In[1];
static mx_entry     Out[1];


static void AccTrap( bool want_return )
{
    if( want_return ) {
        PutBuffPacket( RWBuff, TrapRequest( 1, In, 1, Out ) );
    } else {
        TrapRequest( 1, In, 0, NULL );
    }
}


bool Session( void )
{
    unsigned    req;
    bool        want_return;

    for( ;; ) {
        In[0].len = GetPacket();
        In[0].ptr = GetPacketBuffPtr();
        _DBG(("Session got request "));
        req = TRP_REQUEST( In );
        if( req & REQ_WANT_RETURN ) {
            req &= ~REQ_WANT_RETURN;
            want_return = false;
        } else {
            want_return = true;
        }
        TRP_REQUEST( In ) = req;
        switch( req ) {
        case REQ_PROG_KILL:
            _DBG(("REQ_KILL_PROG\n"));
            AccTrap( true );
            return( true );
        default:
            _DBG(("AccTrap\n"));
            AccTrap( want_return );
            _DBG(("Done AccTrap\n"));
            break;
        }
    }
}


static const char *ServInitialize( void )
{
    const char  *err;

    _DBG(("About to remote link in initialize.\n" ));
    err = RemoteLink( "", true );
    _DBG(( "Back from PM remote link\n" ));
    if( err != NULL ) {
        _DBG(( "ERROR! '%s'\n", err ));
    }
    if( err != NULL ) {
#ifdef ACAD
        for( ;; ) {
            ads_link( RSERR );
        }
#endif
        return( err );
    }
    _DBG(( "No Remote link error. About to TrapInit." ));
    TrapVer = TrapInit( "", RWBuff, false );
    if( RWBuff[0] != '\0' ) {
// NO, NO, NO!  RemoteUnLink();
        err = RWBuff;
        return( err );
    }
    _DBG(( "No TrapInit error. Initialize complete" ));
    Out[0].len = sizeof( RWBuff );
    Out[0].ptr = RWBuff;
    return( err );
}


int main( void )
{
    const char  *err;

#ifdef ACAD
    ACADInit();
#endif
    _DBG(("Calling Initialize\n"));
    err = ServInitialize();
    if( err != NULL ) {
        StartupErr( err );
        return( 1 );
    }
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
