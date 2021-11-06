/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Windows 3.x interrupts and faults handling
*                (called from asm wrapper)
*
****************************************************************************/


typedef enum {
    KILL_APP        = 0,
    RESTART_APP     = 1,
    CHAIN           = 2,
    RUN_REDIRECT    = 3,
    ACCESS_SEGMENT  = 4,
    NOACTION        = 0xFFFF,
} appl_action;

#pragma pack ( __push, 1 )
typedef struct interrupt_struct {
    unsigned long   EAX;
    unsigned long   EBX;
    unsigned long   ECX;
    unsigned long   EDX;
    unsigned long   EDI;
    unsigned long   ESI;
    unsigned long   EFlags;
    unsigned long   EBP;
    unsigned long   EIP;
    unsigned long   ESP;
    unsigned short  SS;
    unsigned short  CS;
    unsigned short  DS;
    unsigned short  ES;
    unsigned short  FS;
    unsigned short  GS;
    unsigned short  InterruptNumber;
} interrupt_struct;

/*
 * this is the crap that was on the stack before IntHandler (in int.asm)
 * pushed its registers
 */
typedef struct {
    unsigned long       oldEBP;
    unsigned short      retIP;
    unsigned short      retCS;
    unsigned short      AX;
    unsigned short      intnumber;
    unsigned short      handle;
    unsigned short      IP;
    unsigned short      CS;
    unsigned short      FLAGS;
} int_frame;

/*
 * this is everything that is on the stack that we need to know about
 * (all of our registers, plus "int_frame" above).
 */
typedef struct {
    unsigned short      SS;
    unsigned short      GS;
    unsigned short      FS;
    unsigned short      ES;
    unsigned short      DS;
    unsigned long       EDI;
    unsigned long       ESI;
    unsigned long       EBP;
    unsigned long       ESP;
    unsigned long       EBX;
    unsigned long       EDX;
    unsigned long       ECX;
    unsigned long       oldEAX;
    int_frame           intf;
} fault_frame;
#pragma pack( __pop )

#ifdef TRAP
extern void __loadds __cdecl __near FaultHandler( volatile fault_frame ff );
#else
extern appl_action __cdecl __far FaultHandler( volatile fault_frame ff );
#endif
