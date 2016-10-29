/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Reduce memory references to one per instruction.
*
****************************************************************************/


extern int          NumOperands( instruction *ins );
extern void         ReplaceOperand( instruction *ins, name *old, name *new );
extern name         *IndexToTemp( instruction * ins, name * index );
extern name         *FindIndex( instruction *ins );
extern void         NoMemIndex( instruction *ins );
extern instruction  *OneMemRef( instruction *ins );
extern void         FixIndex( void );
extern void         FixFPConsts( instruction *ins );
extern void         FixChoices( void );
extern void         FixMemRefs( void );
extern void         FixSegments( void );
extern void         AddSegment( instruction *ins );
