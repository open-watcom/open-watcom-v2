/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Prototypes for split.c
*
****************************************************************************/


#define _IsSigned( type )       (Unsigned[type] != type)

extern type_class_def   Unsigned[];
extern type_class_def   Signed[];
extern type_class_def   DoubleClass[];
extern type_class_def   HalfClass[];

extern instruction *(*ReduceTab[])( instruction * );

extern hw_reg_set       Op1Reg( instruction *ins );
extern hw_reg_set       ResultReg( instruction *ins );
extern hw_reg_set       ZapReg( instruction *ins );
extern instruction      *MoveConst( unsigned_32 value, name *result, type_class_def type_class );
extern instruction      *Reduce( instruction *ins );
extern instruction      *SplitUnary( instruction *ins );
extern void             CheckCC( instruction *ins, instruction *new_ins );

extern void             HalfType( instruction *ins );
extern void             ChangeType( instruction *ins, type_class_def type_class );
extern name             *LowPart( name *tosplit, type_class_def type_class );
extern name             *HighPart( name *tosplit, type_class_def type_class );
extern name             *OffsetPart( name *tosplit );
extern name             *SegmentPart( name *tosplit );

extern bool             UnChangeable( instruction *ins );
