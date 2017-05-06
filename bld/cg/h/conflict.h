/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Conflict manager functions (selection of best register set
*               for variable).
*
****************************************************************************/


extern conflict_node    *AddConflictNode( name *opnd );
extern conflict_node    *FindConflictNode( name *opnd, block *blk, instruction *ins );
extern void             MarkSegment( instruction *ins, name *opnd );
extern conflict_node    *NameConflict( instruction *ins, name *opnd );
extern reg_set_index    GetPossibleForTemp(conflict_node *conf, name *temp);
extern void             MarkPossible( instruction *ins, name *opnd, reg_set_index idx );
extern reg_set_index    MarkIndex( instruction *ins, name *opnd, bool is_temp_index );
extern void             FreePossibleForAlias( conflict_node *conf );
extern void             FreeConflicts( void );
extern void             FreeAConflict( conflict_node *conf );
extern void             InitConflict( void );
extern bool             ConfFrlFree( void );
