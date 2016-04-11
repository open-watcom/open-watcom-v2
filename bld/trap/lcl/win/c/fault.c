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
* Description:  Handle interrupts for trap file (called from asm wrapper).
*
****************************************************************************/


#include <stdio.h>
#include <dos.h>
#include <string.h>
#include "wdebug.h"
#include "stdwin.h"
#include "mad.h"
#include "madregs.h"
#include "miscx87.h"
#include "fault.h"
#include "di386cli.h"

/*
 * How we get our registers:
 *
 * INT.ASM has a routine, IntHandler, that is invoked for all faults.
 * In that routine, all registers are pushed on the stack, and
 * we can manipulte those.  This is what is done for a fault in a 16-bit
 * application.
 *
 * If a fault occurs in a 32-bit extended application, then some different
 * stuff happens.  The DLL, WINT32.DLL (in the INT32 directory), contains
 * the entry point, SetDebugInterrupts32, which uses WDEBUG.386 to trap
 * 32-bit faults (Dr. WATCOM uses this DLL as well).  The blood and guts of
 * it is:  - fault occurs
 *         - WDEBUG.386 gets it first, looks at it, sees if the fault took
 *           place on a use32 stack
 *         - if it was NOT on a use32 stack, then the next handler is
 *           invoked.  We get control through IntHandler eventually.
 *         - if it was on a use32 stack,  then we save all the 32-bit
 *           registers into a specified spot in WINT32.DLL and switch context
 *           to a specified routine (in WINT32.DLL)
 *         - the specified routine then does a breakpoint, which then
 *           causes us to end up in IntHandler.
 */

// nyi - put in header or something!
#define SIG_OFF         0
#define SIG_SIZE        4
extern const unsigned short __based(__segname("_CONST")) win386sig[];
extern const unsigned short __based(__segname("_CONST")) win386sig2[];

extern WORD     __far NewAX;
extern WORD     __far NewCS;
extern WORD     __far NewIP;
extern WORD     __far NewFLAGS;
extern WORD     __far OldretCS;
extern WORD     __far OldretIP;
extern WORD     __far Oldintnumber;
extern WORD     __far Oldhandle;

extern WORD     __far NewSS;
extern DWORD    __far NewESP;

extern WORD     __far RetHow;

#define EXCESS_CRAP_ON_STACK    (sizeof( int_frame ) )

struct fp_state         FPResult;

/*
 * saveState:
 *
 * Save current register state from the fault frame to the IntResult
 * data structure.
 */
static void saveState( volatile fault_frame *ff )
{
    IntResult.SS = ff->SS;
    IntResult.GS = ff->GS;
    IntResult.FS = ff->FS;
    IntResult.ES = ff->ES;
    IntResult.DS = ff->DS;
    IntResult.EDI = ff->EDI;
    IntResult.ESI = ff->ESI;
    // we modify ESP be the "real" esp; i.e., the one without the
    // excess crap pushed on by Windows before giving us the fault
    IntResult.ESP = ff->ESP+EXCESS_CRAP_ON_STACK;
    IntResult.EBX = ff->EBX;
    IntResult.EDX = ff->EDX;
    IntResult.ECX = ff->ECX;
    IntResult.EAX = (ff->oldEAX & 0xFFFF0000) + ff->intf.AX;
    IntResult.EBP = ff->intf.oldEBP;
    IntResult.EFlags = ff->intf.FLAGS;
    IntResult.EIP = ff->intf.IP;
    IntResult.CS = ff->intf.CS;
    IntResult.InterruptNumber = ff->intf.intnumber;

} /* saveState */

/*
 * restoreState:
 *
 * Restore register state back to the fault frame from the IntResult
 * data structure.
 */
static void restoreState( volatile fault_frame *ff )
{
    WORD        __far *wptr;

    /*
     * we save all this stuff in our own code segment (see int.asm)
     * because do not have data segment addressability when we need it
     * (when we go to push it onto a new stack)
     */
    wptr = MK_FP( CSAlias, FP_OFF( &NewAX ) );
    *wptr = (WORD) IntResult.EAX;
    wptr = MK_FP( CSAlias, FP_OFF( &NewCS ) );
    *wptr = IntResult.CS;
    wptr = MK_FP( CSAlias, FP_OFF( &NewIP ) );
    *wptr = (WORD) IntResult.EIP;
    wptr = MK_FP( CSAlias, FP_OFF( &NewFLAGS ) );
    *wptr = (WORD) IntResult.EFlags;
    wptr = MK_FP( CSAlias, FP_OFF( &OldretCS ) );
    *wptr = ff->intf.retCS;
    wptr = MK_FP( CSAlias, FP_OFF( &OldretIP ) );
    *wptr = ff->intf.retIP;
    wptr = MK_FP( CSAlias, FP_OFF( &Oldintnumber ) );
    *wptr = ff->intf.intnumber;
    wptr = MK_FP( CSAlias, FP_OFF( &Oldhandle ) );
    *wptr = ff->intf.handle;

    ff->SS = IntResult.SS;
    ff->GS = IntResult.GS;
    ff->FS = IntResult.FS;
    ff->ES = IntResult.ES;
    ff->DS = IntResult.DS;
    ff->EDI = IntResult.EDI;
    ff->ESI = IntResult.ESI;
    // have ESP include the excess crap again
    ff->ESP = IntResult.ESP-EXCESS_CRAP_ON_STACK;
    ff->EBX = IntResult.EBX;
    ff->EDX = IntResult.EDX;
    ff->ECX = IntResult.ECX;
    ff->oldEAX = IntResult.EAX;
    ff->intf.oldEBP = IntResult.EBP;

} /* restoreState */

/*
 * newStack:
 *
 * set a new SS:ESP for us to reload in INT.ASM.  Again, we put it in
 * the code segment because we lack data segment addressability when we
 * need it.
 *
 * Because of all the misc. crap on the stack, we need to do some magical
 * junk in IntHandler (INT.ASM) to handle changing SS or ESP.  We load
 * up the new stack, and then re-push all of the crap (the int_frame struct).
 *
 * Note that this new stack is only used if we are not processing
 * a 32-bit fault.  If we are processing a 32-bit fault, then all of our
 * registers are re-loaded by WDEBUG.386, so no magical crap is required to
 * change the stack.
 */
static void newStack( WORD SS, DWORD ESP )
{
    WORD        __far *wptr;
    DWORD       __far *dwptr;

    wptr = MK_FP( CSAlias, FP_OFF( &NewSS ) );
    dwptr = MK_FP( CSAlias, FP_OFF( &NewESP ) );

    *wptr = SS;
    *dwptr = ESP;

} /* newStack */

/*
 * setRetHow:
 *
 * save the way we need to return from the interrupt. Again, we put it in
 * the code segment because we lack data segemtn addressability when we
 * need it.
 */
static void setRetHow( WORD rc )
{
    WORD        __far *wptr;

    wptr = MK_FP( CSAlias, FP_OFF( &RetHow ) );

    *wptr = rc;

} /* setRetHow */

volatile int AVolatileInt;
/*
 * FaultHandler:
 *
 * Handle all faults.
 *
 * When we get a fault, the first thing we do is check if we are using
 * WDEBUG.386.  If we are, we call GetDebugInterruptData in WINT32.DLL
 * to see if the fault was a 32-bit one.  If it was, the structure
 * IntResult will be filled in.
 *
 * We make sure that we are not already handling a fault.  If we
 * are, we punt and give it to the next guy.
 *
 * We disable the hot key for doing CTRL_ALT_F (in WDEBUG.386), to make
 * sure that we are not interrupted while in the debugger!
 *
 * If we get an INT3, and it was NOT a 32-bit fault, we back up IP.
 * (If it was a 32-bit fault, it is communicated as a breakpoint, and
 * we don't need to back up IP). We then check if we were waiting for
 * the breakpoint, and if we were, we write back the original byte.
 *
 * If it was not a 32-bit fault, we call saveState, which copies the
 * correct data into the IntResult structure (this is the structure
 * that we use to access/update the registers, see accregs.c).
 *
 * We then directed yield to the debugger, and go into a message loop
 * for the debuggee.
 *
 * Once the debuggee exits its message loop, we check to see if we
 * need to do a special request (access a segment or do an I/O redirection).
 *
 * If there was no special request, we then reset the app's registers
 * (with restoreState for a 16-bit fault, and DoneWithInterreupt in WINT32.DLL
 * for a 32-bit fault), re-enable the hot key for async stopping,
 * and return to IntHandler to allow it to restart the debuggee.
 */
void __loadds __cdecl FaultHandler( volatile fault_frame ff )
{
    restart_opts        rc=CHAIN;
    private_msg         pmsg = FAULT_HIT;
    WORD                sig[2];

    if( WDebug386 ) {
        WasInt32 = GetDebugInterruptData( &IntResult );
        if( WasInt32 ) {
            ff.intf.intnumber = IntResult.InterruptNumber;
            Out((OUT_RUN,"***** 32-bit fault %d, cs:eip=%04x:%08lx *****",
                IntResult.InterruptNumber, IntResult.CS, IntResult.EIP ));
        }
    } else {
        WasInt32 = FALSE;
    }
    newStack( 0, 0L );

    /*
     * only one fault at a time
     */
    Out((OUT_RUN,"***** Fault %d, cs:ip=%04x:%04x, ent=%d, state=%d, WasInt32=%d *****",
        ff.intf.intnumber, ff.intf.CS, ff.intf.IP, FaultHandlerEntered,
        (WORD) DebuggerState, WasInt32 ));
    if( FaultHandlerEntered || DebuggerState == ACTIVE ) {
        if( ff.intf.intnumber == WGOD_ASYNCH_STOP_INT ) {
            setRetHow( RESTART_APP );
        } else {
            setRetHow( CHAIN );
        }
        return;
    }
    UseHotKey( 0 );

    ff.ESP = (WORD) ff.ESP;
    ff.intf.oldEBP = (WORD) ff.intf.oldEBP;

    if( ff.intf.intnumber == INT_3 ) {
        if( !WasInt32 ) {
            ff.intf.IP--;
        }
        Out((OUT_ERR,"BP at '(%d) %4.4x:%4.4x %4.4x:%8.8lx'",WasInt32,ff.intf.CS,ff.intf.IP,
        IntResult.CS,IntResult.EIP));
        if( ( WasInt32 && IntResult.CS == DLLLoadCS && IntResult.EIP == DLLLoadIP ) ||
            ( !WasInt32 && ff.intf.CS == DLLLoadCS && ff.intf.IP == DLLLoadIP ) ) {
            Out((OUT_RUN,"Caught DLL Loaded '%4.4x:%4.4x'",DLLLoadCS,DLLLoadIP));
            WriteMem( DLLLoadCS, DLLLoadIP, &DLLLoadSaveByte, 1 );
            ReadMem( IntResult.CS, SIG_OFF, sig, sizeof( DWORD ) );
            if( memcmp( sig, win386sig, 4 ) == 0 ) {
                Out((OUT_RUN,"Zapped sig"));
                WriteMem( IntResult.CS, SIG_OFF, win386sig2, sizeof( DWORD ) );
                pmsg = DLL_LOAD32;
                DLLLoadExpectingInt1 = TRUE;
            } else {
                pmsg = DLL_LOAD;
            }
            DLLLoadCS = 0;
            DLLLoadIP = 0;
        } else if( DebuggerState == WAITING_FOR_BREAKPOINT ) {
            if( (WasInt32 && IntResult.CS == StopNewTask.loc.segment &&
                        IntResult.EIP == StopNewTask.loc.offset ) ||
                (!WasInt32 && ff.intf.CS == StopNewTask.loc.segment &&
                        ff.intf.IP == StopNewTask.loc.offset) ) {
                WriteMem( StopNewTask.loc.segment, StopNewTask.loc.offset,
                            &StopNewTask.value, 1 );
                pmsg = START_BP_HIT;
            }
        }
    } else if( ff.intf.intnumber == 1 && WasInt32 && DLLLoadExpectingInt1 ) {
            // 32-bit dll load from above
        DLLLoadExpectingInt1 = FALSE;
        pmsg = DLL_LOAD;
    } else if( ff.intf.intnumber == WGOD_ASYNCH_STOP_INT ) {
        pmsg = ASYNCH_STOP;
        Out((OUT_RUN,"***** Sending ASYNCH_STOP to debugger"));
    }


    if( !WasInt32 ) {
        saveState( &ff );
    }
    FaultHandlerEntered = TRUE;
    TaskAtFault = GetCurrentTask();

    if( FPUType == X86_NO ) {
        memset( &FPResult, 0, sizeof( FPResult ) );
    } else if( FPUType < X86_387 ) {
        Read8087( &FPResult );
    } else {
        Read387( &FPResult );
    }

    /*
     * switch to debugger
     */
    while( 1 ) {
        if( !ToDebugger( pmsg ) ) break;
        rc = DebugeeWaitForMessage();
        if( rc == RUN_REDIRECT ) {
            ExecuteRedirect();
        } else if( rc == ACCESS_SEGMENT ) {
            AVolatileInt = *(LPINT) MK_FP( SegmentToAccess+1, 0 );
        } else {
            break;
        }
    }
    Out((OUT_RUN,"***** ---> restarting app, rc=%d",rc));

    if( FPUType >= X86_387 ) {
        Write387( &FPResult );
    } else if( FPUType != X86_NO ) {
        Write8087( &FPResult );
    }

    if( !WasInt32 ) {
        restoreState( &ff );
        newStack( IntResult.SS, IntResult.ESP );
    } else {
        WasInt32 = FALSE;
        DoneWithInterrupt( &IntResult );
    }
    TaskAtFault = NULL;

    FaultHandlerEntered = FALSE;
    setRetHow( rc );
    UseHotKey( 1 );

} /* FaultHandler */
