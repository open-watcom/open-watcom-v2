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


#include <stdlib.h>
#include <string.h>
#include <i86.h>
#include "extender.h"
#include "dpmi.h"
#include "dbgdefn.h"
#include "dsxutil.h"
#include "tinyio.h"
#include "dbglit.h"

extern void                     ProcCmd( void );
extern void                     DebugMain( void );
extern void                     DebugFini( void );
extern void                     StartupErr( char * );
extern void                     SaveOrigVectors( void );
extern void                     RestoreOrigVectors(void);

#ifndef _NEC_PC
static memptr                   OldInt1b;
#endif
static memptr                   OldInt23;
static memptr                   OldInt24;
static memptr                   OldInt28;
static memptr                   Orig28;

dos_memory      RMData;
rm_data         __far *PMData;

#if defined(__OSI__) || __WATCOMC__ < 1000
short   _ExtenderRealModeSelector;
#endif

#if defined(_NEC_PC)
 #define CTRL_BREAK_VECTOR      0x6
#else
 #define CTRL_BREAK_VECTOR      0x1b
#endif

extern void CheckForBrk(void);
#pragma aux CheckForBrk = "mov  ah,0xb"  "int   0x21"

char TBreak( void )
{
    char        tmp;

#ifdef _NEC_PC
    /*
        If we grab the STOP key vector we get spurious interrupts for
        some reason on the NEC. So instead, we leave it along and force
        DOS to check for breaks for us.
    */
    CheckForBrk();
#endif
    tmp = PMData->pending;
    PMData->pending = 0;
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
#ifndef _NEC_PC
    PMData->oldint10.a = MyGetRMVector( 0x10 );
    OldInt1b.a = MyGetRMVector( CTRL_BREAK_VECTOR );
#endif
    OldInt23.a = MyGetRMVector( 0x23 );
    OldInt24.a = MyGetRMVector( 0x24 );
    OldInt28.a = MyGetRMVector( 0x28 );
#ifndef _NEC_PC
    MySetRMVector( 0x10, RMData.s.rm, RM_OFF( Interrupt10 ) );
    MySetRMVector( CTRL_BREAK_VECTOR, RMData.s.rm, RM_OFF( Interrupt1b_23 ) );
#endif
    MySetRMVector( 0x23, RMData.s.rm, RM_OFF( Interrupt1b_23 ) );
    MySetRMVector( 0x24, RMData.s.rm, RM_OFF( Interrupt24 ) );
    MySetRMVector( 0x28, Orig28.s.segment, Orig28.s.offset );
}

void RestoreHandlers( void )
{
#ifndef _NEC_PC
    if( PMData->oldint10.a != 0 ) {
        MySetRMVector( 0x10, PMData->oldint10.s.segment,
                             PMData->oldint10.s.offset );
    }
    if( OldInt1b.a ) {
        MySetRMVector( CTRL_BREAK_VECTOR, OldInt1b.s.segment, OldInt1b.s.offset );
    }
#endif
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

static void Cleanup()
{
    RestoreOrigVectors();
    if( RMData.s.pm != 0 ) {
        DPMIFreeDOSMemoryBlock( RMData.s.pm );
    }
}

#pragma aux KillDebugger aborts;
void KillDebugger( int rc )
{
    RestoreHandlers();
    Cleanup();
    TinyTerminateProcess( rc );
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
        StartupErr( LIT( Unable_to_get_rm_sel ) );
    }
    _ExtenderRealModeSelector = result & 0xffff;
    if( DPMISetSegmentLimit( _ExtenderRealModeSelector, 0xfffff ) ) {
        StartupErr( LIT( Unable_to_get_rm_sel ) );
    }
    }
#endif
    SaveOrigVectors();
    Orig28.a = MyGetRMVector( 0x28 );

    RMData.a = DPMIAllocateDOSMemoryBlock( _NBPARAS( RMSegEnd - RMSegStart ) );
    if( RMData.s.pm == 0 ) {
        StartupErr( LIT( Unable_to_alloc_DOS_mem ) );
    }
    PMData = MK_FP( RMData.s.pm, 0 );
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

void WndCleanUp()
{
}

bool SysGUI()
{
    return( FALSE );
}
