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
* Description:  Win386 debugging interface.
*
****************************************************************************/


#ifndef _DI386CLI_H_INCLUDED
#define _DI386CLI_H_INCLUDED

#include "di386typ.h"

#ifndef global_di386
#define global_di386 extern
#endif

global_di386 IsDebuggerExecuting_func    *IsDebuggerExecuting;
global_di386 DoneWithInterrupt_func      *DoneWithInterrupt;
global_di386 GetDebugInterruptData_func  *GetDebugInterruptData;
global_di386 ResetDebugInterrupts32_func *ResetDebugInterrupts32;
global_di386 SetDebugInterrupts32_func   *SetDebugInterrupts32;
global_di386 DebuggerIsExecuting_func    *DebuggerIsExecuting;

#undef global_di386

extern bool     WDebug386;

extern bool     Start386Debug( void );
extern void     Done386Debug( void );

#endif /* _DI386CLI_H_INCLUDED */
