/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2018 The Open Watcom Contributors. All Rights Reserved.
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
**  ========================================================================
*
* Description:  Prototypes specific to x87 FPU regsiter allocation. These
*               functions are called from generic code and must be stubbed
*               for non-x86 platforms.
*
****************************************************************************/


extern void             FPSetStack( name * );
extern void             FPNotStack( name * );
extern void             FPPushParms( pn, call_state * );
extern bool             FPDivIsADog( type_class_def type_class );
extern bool             FPStackOp( name * );
extern bool             FPIsConvert( instruction * );
extern type_class_def   FPInsClass( instruction * );
extern bool             FPIsStack( name * );
extern void             FPInit( void );
extern void             FPExpand( void );
extern void             FPOptimize( void );
extern void             FPRegAlloc( void );
extern void             FPParms( void );
extern bool             FPSideEffect( instruction * );
extern bool             FPStackIns( instruction * );
extern bool             FPStackReg( name * );
extern bool             FPInsIntroduced( instruction * );
extern bool             FPFreeIns( instruction * );
extern int              FPStackExit( block * );
extern int              FPStkOver( instruction *, int );
extern void             FPCalcStk( instruction *, int * );
extern void             FPPostSched( block * );
extern void             FPPreSched( block * );
