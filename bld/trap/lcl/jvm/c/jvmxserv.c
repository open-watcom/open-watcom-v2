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


#include <windows.h>
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
#include "javadbg.h"
#include "servio.h"
#include "dllmain.h"

#ifdef DEBUG_TRAP
#define _DBG1( x ) printf x ; fflush( stdout )
#define _DBG( x ) printf x ; fflush( stdout )
#else
#define _DBG1( x ) // cputs x
#define _DBG( x )
#endif


static trap_version     TrapVer;

char    RWBuff[ 0x400 ];

static mx_entry     In[1];
static mx_entry     Out[1];


static void AccTrap( bool want_return )
{
    if( want_return ) {
        PutBuffPacket( RWBuff, TrapRequest( 1, &In, 1, &Out ) );
    } else {
        TrapRequest( 1, &In, 0, NULL );
    }
}


bool Session( void )
{
    unsigned    req;
    bool    want_return;

    for( ;; ) {
        In[0].len = GetPacket();
        In[0].ptr = GetPacketBuffPtr();
        _DBG(("Session got request "));
        req = TRP_REQUEST( In );
        TRP_REQUEST( In ) &= ~0x80;
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
    const char  *err;

    _DBG(("About to remote link in initialize.\n" ));
    err = RemoteLink( "", 1 );
    _DBG(( "Back from PM remote link\n" ));
    if( err != NULL ) {
        _DBG(( "ERROR! '%s'\n", err ));
    }
    if( err != NULL )
        StartupErr( err );
    _DBG(( "No Remote link error. About to TrapInit." ));
    TrapVer = TrapInit( "", RWBuff, FALSE );
    if( RWBuff[0] != '\0' ) {
// NO, NO, NO!  RemoteUnLink();
        StartupErr( RWBuff );
    }
    _DBG(( "No TrapInit error. Initialize complete" ));
    Out[0].len = sizeof( RWBuff );
    Out[0].ptr = (void *)RWBuff;
}


#pragma aux (cdecl) EntryPoint "Java_sun_tools_debug_jvmhelp_EntryPoint_stub" export
void EntryPoint( stack_item *p, ExecEnv *ee )
{
    Initialize();
    RemoteConnect();
    Session();
    TrapFini();
    RemoteDisco();
    RemoteUnLink();
}


extern BOOL __export __stdcall DllMain( HANDLE inst, DWORD reason, LPVOID reserved )
/*********************************************************/
{
    switch( reason ) {
    case DLL_PROCESS_ATTACH:
//      CreateThread( NULL, 64*1024, (LPVOID)Doit, NULL, 0, &tid );
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
