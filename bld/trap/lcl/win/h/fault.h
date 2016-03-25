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


/*
 * this is the crap that was on the stack before IntHandler (in int.asm)
 * pushed its registers
 */
typedef struct {
    DWORD oldEBP;       // pushed at start of IntHandler
    WORD retIP;         // far call into
    WORD retCS;         //        IntHandler
    WORD AX;            // set by Windows for us (since AX is trashed)
    WORD intnumber;     // set by Windows for us
    WORD handle;        // set by Windows for us
    WORD IP;            // this
    WORD CS;            //   is the
    WORD FLAGS;         //     interrupt frame
} int_frame;

/*
 * this is everything that is on the stack that we need to know about
 * (all of our registers, plus "int_frame" above).
 */
typedef struct {
    WORD SS;
    WORD GS;
    WORD FS;
    WORD ES;
    WORD DS;
    DWORD EDI;
    DWORD ESI;
    DWORD EBP;
    DWORD ESP;
    DWORD EBX;
    DWORD EDX;
    DWORD ECX;
    DWORD oldEAX;
    int_frame intf;
} fault_frame;

extern void __loadds __cdecl FaultHandler( volatile fault_frame ff );
