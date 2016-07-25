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
* Description:  Loop optimizations.
*
****************************************************************************/


block    *AddPreBlock( block *postblk );
void     MarkLoop( void );
void     UnMarkLoop( void );
block    *NextInLoop( block *blk );
block    *NextInProg( block *blk );
void     MakeJumpBlock( block *cond_blk, block_edge *exit_edge );
void     UnMarkInvariants( void );
void     MarkInvariants( void );
bool     InvariantOp( name *op );
bool     Hoistable( instruction *ins, block *blk );
induction    *FindIndVar( name *op );
void     CommonInvariant( void );
void    FiniIndVars( void );
bool    Inducable( block *blk, instruction *ins );
void    SuffixPreHeader( instruction *ins );
void    MoveDownLoop( block *cond );
instruction     *DupIns( instruction *blk_end, instruction *ins, name *var, signed_32 adjust );
instruction     *DupInstrs( instruction *blk_end, instruction *first, instruction *last, induction *var, signed_32 adjust );
bool    AnalyseLoop( induction *var, bool *ponecond, instruction **pcond, block **pcond_blk );
bool    CalcFinalValue( induction *var, block *blk, instruction *ins, signed_32 *final, signed_32 *initial );
bool    LoopInvariant( void );
bool    LoopRegInvariant( void );
void    LoopEnregister( void );
bool    IndVars( void );
bool    TransLoops( bool unroll );
