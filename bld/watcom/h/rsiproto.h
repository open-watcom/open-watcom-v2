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


int D32DebugBreakOp(char *Int_3);
int D32DebugRead(OFFSET32 off, SELECTOR sel, int translate, char *to, int n);
int D32DebugWrite(OFFSET32 off, SELECTOR sel, int translate, char *from, int n);
int D32DebugSetBreak(OFFSET32 off, SELECTOR sel, int translate, char *to, char *from);
int D32DebugInit(TSF32 far *process_regs);
void D32DebugFini( void );
int D32DebugRun(TSF32 far *process_regs);
int D32DebugLoad(char *filename, TSF32 far *tspv);
SELECTOR D32RelSel(SELECTOR objno, unsigned sel_adjust);
SELECTOR D32UnrelSel(SELECTOR sel);
OFFSET32 D32RelOff(SELECTOR objno, OFFSET32 off);
OFFSET32 D32UnrelOff(SELECTOR objno, OFFSET32 off);
void D32Unrelocate(Fptr32 *fptr);
void D32Relocate(Fptr32 *fptr);
int D32AddressUse32(SELECTOR sel);
void D32HookTimer(int);
void D32UnHookTimer();
long D32LinearAddress(SELECTOR sel, OFFSET32 off);
int D32AddressCheck(SELECTOR sel, OFFSET32 off, OFFSET32 for_length);
