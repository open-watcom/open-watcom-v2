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
* Description:  Save/restore register state.
*
****************************************************************************/


#include "commonui.h"
#include "wdebug.h"
#include "intdata.h"

/*
 * SaveState - save current register state
 */
void SaveState( interrupt_struct *idata, fault_frame *ff )
{
    idata->SS = ff->SS;
    idata->GS = ff->GS;
    idata->FS = ff->FS;
    idata->ES = ff->ES;
    idata->DS = ff->DS;
    idata->EDI = ff->EDI;
    idata->ESI = ff->ESI;
    idata->EBP = ff->EBP;
    idata->ESP = ff->ESP + EXCESS_CRAP_ON_STACK;
    idata->EBX = ff->EBX;
    idata->EDX = ff->EDX;
    idata->ECX = ff->ECX;
    idata->EAX = (ff->oldEAX & 0xFFFF0000) + ff->AX;
    idata->EBP = ff->oldEBP;
    idata->EFlags = ff->FLAGS;
    idata->EIP = ff->IP;
    idata->CS = ff->CS;
    idata->InterruptNumber = ff->intnumber;

} /* SaveState */

/*
 * RestoreState - restore register state
 */
void RestoreState( interrupt_struct *idata, fault_frame *ff )
{
    ff->SS = idata->SS;
    ff->GS = idata->GS;
    ff->FS = idata->FS;
    ff->ES = idata->ES;
    ff->DS = idata->DS;
    ff->EDI = idata->EDI;
    ff->ESI = idata->ESI;
    ff->oldEBP = idata->EBP;
    ff->ESP = idata->ESP - EXCESS_CRAP_ON_STACK;
    ff->EBX = idata->EBX;
    ff->EDX = idata->EDX;
    ff->ECX = idata->ECX;
    ff->AX = idata->EAX;
    ff->IP = idata->EIP;
    ff->CS = idata->CS;
    ff->FLAGS = idata->EFlags;

} /* RestoreState */
