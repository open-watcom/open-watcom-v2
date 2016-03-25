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
#include <stdlib.h>
#include <malloc.h>
#include <conio.h>
#include <ctype.h>
#include <process.h>
#include <dos.h>

#include "dbgdefn.h"
#include "dbgreg.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgio.h"
#include "trpimp.h"

extern  void    InitTrap(bool);
extern  void    FiniTrap();
extern  char    *RealFName( char *, int * );
extern long DoLoad( char *args, unsigned long* );
extern trap_types DoRunProg( bool single, char *flag );
extern unsigned RemoteGetLibName( unsigned long lib_hdl, void *ptr, unsigned buff_len );
extern void ReadDbgRegs();
extern void WriteDbgRegs();
extern bool KillProgOvlay();
extern int printf(char*,...);


struct location_context Context;
dbg_switches            DbgSwitches;
char                    TrpBuff[ TRP_LEN + 1 ];
char                    *TxtBuff;
char                    *TrpFile;
char                    NullStr[] = { '\0' };
int                     MaxOnLine = { 0 };
int                     Typing = { 0 };
int                     ErrorStatus = { 0 };
int                     DefaultLocation = { 0 };
system_config           SysConfig;

unsigned         OvlSize;
unsigned         CheckSize;
machine_state   RegArea;
machine_state   *DbgRegs = &RegArea;
address NilAddr;
unsigned         TaskId;

void InitLC( struct location_context *new, bool dummy )
{
    dummy=dummy;
    memset( new, 0, sizeof( *new ) );
    new->execution = Context.execution;
    new->frame = Context.frame;
    new->stack = Context.stack;
    new->have_stack = TRUE;
    new->maybe_have_frame = TRUE;
    new->maybe_have_object = TRUE;
    new->use = 1;
}

void *DbgAlloc( int size )
{
    return( malloc( size ) );
}

void DbgFree( void * chunk )
{
    free( chunk );
}

InitIt( char *trp, void __far *hab, void __far *hwnd )
{
    TxtBuff = DbgAlloc( 512 );
    TrpFile = DbgAlloc( 512 );
    strcpy( TrpFile, trp );
    InitTrap( TRUE );  // Init rfx
    TellHandles( hab, hwnd );
}

int LoadIt()
{
    unsigned long handle;
    return( DoLoad( getcmd( TxtBuff ), &handle ) == 0 );
}

int RunIt()
{
    char        why;
    trap_types  trap;

    for( ;; ) {
        switch( trap = DoRunProg( FALSE, &why ) ) {
        case TRAP_SKIP:
            if( why & CHANGE_LIBRARIES ) {
                AddLibInfo();
            }
            break;
        case TRAP_BREAK_POINT:
            ReadDbgRegs();
            DbgRegs->cpu.dr.EIP++;
            WriteDbgRegs();
            return( 1 );
        default:
            return( 0 );
        }
    }
}

KillIt()
{
    KillProgOvlay();
    return( 0 );
}

FiniIt()
{
    KillProgOvlay();
    FiniTrap();
    return( 0 );
}

void GrabHandlers()
{
}

void RestoreHandlers()
{
}

void FreeRing()
{
}

void WndUser()
{
}

void StartupErr( const char *err )
{
    printf( "%s\n", err );
    exit( 1 );
}
#pragma off(unreferenced)
void SectLoad( unsigned sect_id )
{
}
void AddrFix( address *addr )
{
}
int SameAddrSpace( address a, address b )
{
    return( a.mach.segment == b.mach.segment );
}
void RemapSection( unsigned section, unsigned new_seg )
{
}
void InvalidateTblCache()
{
}
void CheckForNewThreads( bool set_exec )
{
}
SetupCall()
{
}
void FreeThreads()
{
}
char *RealFName( char *name, int *loc )
{
    *loc = 0;
    return( name );
}
void AddrFloat( address *addr )
{
}
void AddAliasInfo( unsigned seg, unsigned alias )
{
}
void AddLibInfo()
{
    unsigned            module;

    module = 0;
    for( ;; ) {
        module = RemoteGetLibName( module, TxtBuff, TXT_LEN );
        if( module == 0 ) break;
    }
}

#ifdef STANDALONE
main()
{
    InitIt( "DSTD32", NULL, NULL );
    LoadIt();
    RunIt();
    KillIt();
    FiniIt();
}
#endif
