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
* Description:  Main loop for extended DOS debugger.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <i86.h>
#include "extender.h"
#include "dpmi.h"
#include "dbgdefn.h"
#include "dsxutil.h"
#include "dbglit.h"
#include "dbgcmdln.h"
#include "dbgmain.h"
#include "trpsys.h"
#include "dbginit.h"
#include "dbgio.h"

extern void                     StartupErr( const char * );

static memptr                   OldInt1b;
static memptr                   OldInt23;
static memptr                   OldInt24;
static memptr                   OldInt28;
static memptr                   Orig28;

dos_memory      RMData;
rm_data         __far *PMData;

#if defined(__OSI__)
short   _ExtenderRealModeSelector;
#endif

#define CTRL_BREAK_VECTOR      0x1b

extern void CheckForBrk(void);
#pragma aux CheckForBrk = "mov  ah,0xb"  "int   0x21"

bool TBreak( void )
{
    bool        tmp;

    tmp = PMData->pending;
    PMData->pending = false;
    return( tmp );
}


unsigned_32 MyGetRMVector( unsigned vect )
{
    void __far  *p;

    p = DPMIGetRealModeInterruptVector( vect );
    return( (FP_SEG( p ) << 16) + (FP_OFF( p ) & 0xffff) );
}

void MySetRMVector( unsigned vect, unsigned seg, unsigned off )
{
    DPMISetRealModeInterruptVector( vect, MK_FP( seg, off ) );
}

void GrabHandlers( void )
{
    PMData->oldint10.a = MyGetRMVector( 0x10 );
    OldInt1b.a = MyGetRMVector( CTRL_BREAK_VECTOR );
    OldInt23.a = MyGetRMVector( 0x23 );
    OldInt24.a = MyGetRMVector( 0x24 );
    OldInt28.a = MyGetRMVector( 0x28 );
    MySetRMVector( 0x10, RMData.segm.rm, RM_OFF( Interrupt10 ) );
    MySetRMVector( CTRL_BREAK_VECTOR, RMData.segm.rm, RM_OFF( Interrupt1b_23 ) );
    MySetRMVector( 0x23, RMData.segm.rm, RM_OFF( Interrupt1b_23 ) );
    MySetRMVector( 0x24, RMData.segm.rm, RM_OFF( Interrupt24 ) );
    MySetRMVector( 0x28, Orig28.s.segment, Orig28.s.offset );
}

void RestoreHandlers( void )
{
    if( PMData->oldint10.a != 0 ) {
        MySetRMVector( 0x10, PMData->oldint10.s.segment,
                             PMData->oldint10.s.offset );
    }
    if( OldInt1b.a ) {
        MySetRMVector( CTRL_BREAK_VECTOR, OldInt1b.s.segment, OldInt1b.s.offset );
    }
    if( OldInt23.a ) {
        MySetRMVector( 0x23, OldInt23.s.segment, OldInt23.s.offset );
    }
    if( OldInt24.a ) {
        MySetRMVector( 0x24, OldInt24.s.segment, OldInt24.s.offset );
    }
    if( OldInt28.a ) {
        MySetRMVector( 0x28, OldInt28.s.segment, OldInt28.s.offset );
    }
}

static void Cleanup( void )
{
    RestoreOrigVectors();
    if( RMData.segm.pm != 0 ) {
        DPMIFreeDOSMemoryBlock( RMData.segm.pm );
    }
}

#pragma aux KillDebugger aborts;
void KillDebugger( int rc )
{
    RestoreHandlers();
    Cleanup();
    _exit( rc );
}

void GUImain( void )
{
#if defined(__OSI__) || __WATCOMC__ < 1000
    {
    long    result;

#if defined(__OSI__)
    _Extender = 1;
#endif
    result = DPMIAllocateLDTDescriptors( 1 );
    if( result < 0 ) {
        StartupErr( LIT_ENG( Unable_to_get_rm_sel ) );
    }
    _ExtenderRealModeSelector = result & 0xffff;
    if( DPMISetSegmentLimit( _ExtenderRealModeSelector, 0xfffff ) ) {
        StartupErr( LIT_ENG( Unable_to_get_rm_sel ) );
    }
    }
#endif
    SaveOrigVectors();
    Orig28.a = MyGetRMVector( 0x28 );

    RMData.dpmi_adr = DPMIAllocateDOSMemoryBlock( _NBPARAS( RMSegEnd - RMSegStart ) );
    if( RMData.segm.pm == 0 ) {
        StartupErr( LIT_ENG( Unable_to_alloc_DOS_mem ) );
    }
    PMData = MK_FP( RMData.segm.pm, 0 );
    _fmemcpy( PMData, RMSegStart, RMSegEnd - RMSegStart );
    if( _osmajor == 2 ) {
        PMData->fail = 0;
    } else {
        PMData->fail = 3;
    }
    DebugMain();
}


int GUISysInit( int param )
{
    param=param;
    return( 1 );
}

void GUISysFini( void  )
{
    DebugFini();
    Cleanup();
}

void WndCleanUp( void )
{
}

bool SysGUI( void )
{
    return( FALSE );
}

void PopErrBox( const char *buff )
{
    WriteText( STD_ERR, buff, strlen( buff ) );
}
