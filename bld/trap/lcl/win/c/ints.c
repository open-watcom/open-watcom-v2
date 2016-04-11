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


#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <dos.h>
#include "cpuglob.h"
#include "wdebug.h"
#include "di386dll.h"

#define MAX_ISTACK      4096

typedef union {
    char        bytes[8];
    unsigned    short words[4];
} idt;

idt     IdtInt1,IdtInt3;
idt     NewIdtInt1,NewIdtInt3;
char    IStack[MAX_ISTACK];
WORD    SetCount = 0;
WORD    IntAccessed = 0;

extern DWORD    SaveEAX;
extern WORD     DPL;
extern WORD     OurOwnInt;
extern WORD     IDTSel;
extern WORD     InterruptStackSel;
extern DWORD    InterruptStackOff;


extern void GetIDTSel( void );
extern void ReleaseIDTSel( void );
extern void InterruptCallback( void );

extern void ReflectInt1Int3( void );

/*
 * SetDebugInterrupts32
 *
 * called to setup for 32-bit interrupts (by debugger or Dr. WATCOM)
 *
 * - check for WDEBUG.386 (can't work without it!)
 * - stomp the IDT for INT1 and INT3.  What actually happens is that
 *   WDEBUG.386 stomps the IDT to point at its own routine in the 32-bit
 *   world.  This routine sets some internal state so that it knows
 *   that an int1/3 occurred, and then calls the 16-bit routine that
 *   we provide using HookIDT.  This routine must cause some fault,
 *   (in our case, we cause an illegal instruction fault), which then
 *   causes us to go through the normal fault handling.
 *   NOTE: after a int1/3 occurs, the WDEBUG.386 restores the IDT back
 *   to normal.  Thus, when you wish to start handling int1/int3's again,
 *   the IDT must be reset to our handler (DoneWithInterrupt does this)
 *
 * - register our interrupt callback routine with WDEBUG.386.  This routine
 *   is invoked whenever a 32-bit fault occurs.  The routine runs on a
 *   stack that we specify.  WDEBUG.386 copies the register information
 *   into the provided structure (&SaveEAX) before it invokes the callback
 *   routine. NOTE:  "invoking" the callback routine really means that
 *   WDEBUG.386 changes the registers of the Windows VM such that the
 *   next time it runs, it begins execution at the callback routine.
 *   No actual calls occur.
 */
int __export FAR PASCAL SetDebugInterrupts32( void )
{
    SetCount++;
    if( SetCount > 1 ) {
        return( 1 );
    }

    if( CheckWin386Debug() != WGOD_VERSION ) {
        return( 0 );
    }
    DPL = (GetCS() & 0x03) << 5;
    OurOwnInt = 0;

    /*
     * set up our IDT
     */
    GetIDTSel();
    IDTInit( IDTSel );
    HookIDT( ReflectInt1Int3 );

    InterruptStackSel = FP_SEG( IStack );
    InterruptStackOff = (DWORD) (FP_OFF( IStack ) + MAX_ISTACK-16);


    /*
     * set up to be notified of faults by wgod
     */
    RegisterInterruptCallback( (LPVOID) InterruptCallback,
                        (LPVOID) &SaveEAX,
                        (LPVOID) &IStack[MAX_ISTACK-16] );

    return( 1 );


} /* SetDebugInterrupts32 */

/*
 * ResetDebugInterrupts32:
 *
 * This routine is called when someone is done using WINT32.DLL for
 * trapping 32-bit interrupts.  We restore the IDT, and unregister
 * our interrupt callback
 */
void __export FAR PASCAL ResetDebugInterrupts32( void )
{
    SetCount--;
    if( SetCount == 0 ) {
        CopyMemory386( IDTSel, (DWORD) 1*8, GetDS(), (DWORD) &IdtInt1, 8 );
        CopyMemory386( IDTSel, (DWORD) 3*8, GetDS(), (DWORD) &IdtInt3, 8 );
        IDTFini();
        ReleaseIDTSel();
        UnRegisterInterruptCallback( (LPVOID) InterruptCallback );
    }

} /* ResetDebugInterrupts32 */

/*
 * GetDebugInterruptData:
 *
 * called by someone to find out if the fault they got was a 32-bit fault.
 * If it was a 32-bit fault, then the register data is copied into a
 * specified data area.
 *
 * This routine must be followed by a DoneWithInterrupt call, otherwise
 * the INT1/INT3 IDT hooks will not be restored.
 */
BOOL __export FAR PASCAL GetDebugInterruptData( LPVOID data )
{
    if( OurOwnInt == 0 ) {
        return( 0 );
    }
    if( data != NULL ) {
        memcpy( data, &SaveEAX, sizeof( interrupt_struct ) );
    }
    IntAccessed++;
    return( 1 );

} /* GetDebugInterruptData */

/*
 * DoneWithInterrupt:
 *
 * called when someone is done handling a 32-bit fault.  The registers
 * are copied back to the data area for reloading by WDEBUG.386, and
 * the IDT is reset to point at our int1/3 handler.
 */
void __export FAR PASCAL DoneWithInterrupt( LPVOID data )
{
    if( data != NULL ) {
        memcpy( &SaveEAX, data, sizeof( interrupt_struct ) );
    }
    IntAccessed--;
    if( (int)IntAccessed <= 0 ) {
        IDTInit( IDTSel );
        HookIDT( ReflectInt1Int3 );
        OurOwnInt = 0;
    }

} /* DoneWithInterrupt */

int DebuggerCount = 0;

/*
 * IsDebuggerExecuting:
 *
 * called by Dr. WATCOM (or anyone else in the future) to see if there
 * is a debugger using WINT32.DLL
 */
BOOL __export FAR PASCAL IsDebuggerExecuting( void )
{
    return( DebuggerCount );

} /* IsDebuggerExecuting */

/*
 * DebuggerIsExecuting:
 *
 * called by the debugger to let Dr. WATCOM (or anyone else in the future)
 * know that it is executing and should not handle any faults or
 * notifications.
 *
 */
void __export FAR PASCAL DebuggerIsExecuting( int cnt )
{
    DebuggerCount += cnt;

} /* IsDebuggerExecuting */
