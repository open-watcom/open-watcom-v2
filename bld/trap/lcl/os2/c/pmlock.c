/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#include <string.h>
#include <env.h>
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#include <wos2.h>
#include <os2dbg.h>
#include "os2v2acc.h"
#include "trperr.h"
#include "wdpmhelp.h"


#define STACK_SIZE      8192

static byte             thestack[STACK_SIZE];
static HFILE            PmInh;
static HFILE            PmOuth;
static HFILE            HisInh;
static HFILE            HisOuth;
static bool             Response;
static bool             HaveHelper;
static USHORT           lockSID;

static int SpawnLocker( HFILE inh, HFILE outh )
{
    STARTDATA       start;
    char            parms[20];
    PID             pid;

    parms[0] = inh + ADJUST_HFILE;
    parms[1] = ' ';
    parms[2] = outh + ADJUST_HFILE;
    parms[3] = '\0';

    start.Length = offsetof( STARTDATA, IconFile );
    start.Related = 1;
    start.FgBg = 1;
    start.TraceOpt = 0;
    start.PgmTitle = TRP_The_WATCOM_Debugger;
    start.PgmName = "WDPMHELP.EXE";
    start.PgmInputs = (PBYTE)parms;
    start.TermQ = 0;
    start.Environment = NULL;
    start.InheritOpt = 1;
    start.SessionType = SSF_TYPE_PM;
    return( DosStartSession( (PVOID)&start, &lockSID, &pid ) );
}

static void PmHelp( int command )
{
    USHORT          dummy;
    pmhelp_packet   data;

    if( !HaveHelper )
        return;
    data.command = command;
    DosWrite( PmOuth, &data, sizeof( data ), &dummy );
}


static void __far SwitchBack( void )
{
    USHORT          rc;
    pmhelp_packet   data;
    USHORT          dummy;

    for( ;; ) {
        rc = DosRead( PmInh, &data, sizeof( data ), &dummy );
        if( data.command == PMHELP_SWITCHBACK ) {
            Response = 1;
            DosSelectSession( 0, 0 );
        }
    }
}

void StopPMHelp( void )
{
    if( !HaveHelper )
        return;
    PmHelp( PMHELP_EXIT );
    DosClose( PmInh );
    DosClose( PmOuth );
    DosClose( HisOuth );
    DosClose( HisInh );
}

void PMLock( unsigned long pid, unsigned long tid )
{
    /* unused parameters */ (void)pid; (void)tid;

    PmHelp( PMHELP_LOCK );
}

void PMUnLock( void )
{
    PmHelp( PMHELP_UNLOCK );
}

int PMFlip( void )
{
    if( !HaveHelper )
        return( FALSE );
    Response = 0;
    DosSelectSession( lockSID, 0 );
    while( !Response )
        DosSleep( 100 );
    return( TRUE );
}

void StartPMHelp( void )
{
    TID         tid;

    HaveHelper = FALSE;
    if( DosMakePipe( &PmInh, &HisOuth, sizeof( pmhelp_packet ) ) )
        return;
    if( DosMakePipe( &HisInh, &PmOuth, sizeof( pmhelp_packet ) ) )
        return;
    if( SpawnLocker( HisInh, HisOuth ) )
        return;
    if( DosCreateThread( SwitchBack, &tid, thestack + STACK_SIZE ) )
        return;
    HaveHelper = TRUE;
}
