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
* Description:  control flow code generation routines
*
****************************************************************************/


extern void    GStmtAddr( sym_id stmtno );
extern void    GLabel( int label );
extern void    GStmtLabel( sym_id stmt );
extern void    GStmtBr( sym_id stmt );
extern void    GBranch( int label );
extern void    GBrFalse( int label );
extern void    G3WayBranch( sym_id lt, sym_id eq, sym_id gt );
extern void    InitSelect( void );
extern void    FiniSelect( void );
extern void    GAsgnGoTo( bool list );
extern void    GEndBrTab( void );
extern void    GAssign( sym_id label );
extern void    GBreak( FCODE routine );
extern void    GStartSF( void );
extern void    GEndSF( void );
extern void    GStartBlock( void );
extern void    GExecute( void );
extern void    GEndBlock( void );
extern void    GPgmLabel( void );
extern void    GSegLabel( void );
extern void    GBlockLabel( void );
extern void    FreeLabel( int label );
