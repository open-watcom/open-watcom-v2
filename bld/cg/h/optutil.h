/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Optimizer utility routines.
*
****************************************************************************/


extern ins_entry    *ValidIns( ins_entry *instr );
extern ins_entry    *PrevIns( ins_entry *instr );
extern oc_class     PrevClass( ins_entry *instr );
extern ins_entry    *NextIns( ins_entry *instr );
extern oc_class     NextClass( ins_entry *instr );
extern void         AddInstr( ins_entry *instr, ins_entry *insert );
extern void         DelRef( ins_entry **owner, ins_entry *instr );
extern void         UnLinkInstr( ins_entry *old );
extern ins_entry    *DelInstr( ins_entry *old );
extern void         FreePendingDeletes( void );
