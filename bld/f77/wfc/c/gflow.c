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


//
// GFLOW     : control flow code generation routines
//

#include "ftnstd.h"
#include "fcodes.h"
#include "global.h"
#include "fcgbls.h"
#include "opn.h"
#include "cpopt.h"
#include "recog.h"
#include "emitobj.h"
#include "utility.h"
#include "tcmplx.h"
#include "declare.h"
#include "cgmagic.h"
#include "gsubprog.h"
#include "gflow.h"



void    GStmtAddr( sym_id stmtno ) {
//==================================

// Generate an entry in the statement table.

    if( stmtno != NULL ) {
        OutPtr( stmtno );
    }
}


void    GLabel( int label ) {
//===========================

// Generate a label.

    EmitOp( FC_DEFINE_LABEL );
    OutU16( label );
}


void    GStmtLabel( sym_id stmt ) {
//=================================

// Generate a statement label.

    EmitOp( FC_STMT_DEFINE_LABEL );
    GStmtAddr( stmt );
}


void    GStmtBr( sym_id stmt ) {
//==============================

// Generate a branch to a statement label.

    EmitOp( FC_STMT_JMP_ALWAYS );
    GStmtAddr( stmt );
}


void    GBranch( int label ) {
//============================

// Generate a branch (non-conditional).

    EmitOp( FC_JMP_ALWAYS );
    OutU16( label );
}


void    GBrFalse( int label ) {
//=============================

// Generate a branch on FALSE.

    EmitOp( FC_JMP_FALSE );
    GenType( CITNode );
    OutU16( label );
}


void    G3WayBranch( sym_id lt, sym_id eq, sym_id gt ) {
//======================================================

// Generate a 3-way branch.

    IfExpr();
    EmitOp( FC_IF_ARITH );
    GenType( CITNode );
    GStmtAddr( lt );
    GStmtAddr( eq );
    GStmtAddr( gt );
}


void    InitSelect( void ) {
//====================

// Initialize SELECT statement.

    sym_id      sel_expr;

    if( !AError ) {
        if( CITNode->opn.ds == DSOPN_NAM ) {
            // must be a variable name
            CSHead->cs_info.cases->sel_expr = CITNode->sym_ptr;
        } else {
            if( CITNode->typ == FT_CHAR ) {
                sel_expr = StaticAlloc( 1, FT_CHAR );
                CSHead->cs_info.cases->sel_expr = sel_expr;
                PushSym( sel_expr );
                EmitOp( FC_CHAR_1_MOVE );
                DumpType( FT_INTEGER_1, 1 );
                OutPtr( NULL );
            } else {
                sel_expr = StaticAlloc( CITNode->size, MapTypes( FT_INTEGER, CITNode->size));
                CSHead->cs_info.cases->sel_expr = sel_expr;
                PushOpn( CITNode );
                EmitOp( FC_POP );  // pop select expression into temporary
                OutPtr( sel_expr );
                DumpTypes( MapTypes( FT_INTEGER, CITNode->size ), CITNode->size, CITNode->typ, CITNode->size );
            }
            EmitOp( FC_EXPR_DONE );
        }
        GBranch( CSHead->branch );          // branch to "select table"
    }
}


void    FiniSelect( void ) {
//====================

// Terminate SELECT statement.

    int         count;
    obj_ptr     curr_obj;
    case_entry  *ptr;

    GLabel( CSHead->branch );                   // label start of select table
    if( CSHead->typ == CS_COMPUTED_GOTO ) {
        EmitOp( FC_COMPUTED_GOTO );
    } else {
        EmitOp( FC_SELECT );
    }
    curr_obj = ObjTell();
    OutU16( 0 );                                // emit count
    count = 0;
    ptr = CSHead->cs_info.cases;
    OutU16( ptr->label.g_label );               // label for default case
    ptr = ptr->link;
    while( ptr != NULL ) {                      // emit lab,hi,lo for ea. case
        if( CSHead->typ == CS_COMPUTED_GOTO ) {
            GStmtAddr( ptr->label.st_label );
        } else {
            OutU16( ptr->label.g_label );
        }
        OutConst32( ptr->high );
        OutConst32( ptr->low );
        count++;
        ptr = ptr->link;
    }
    curr_obj = ObjSeek( curr_obj );
    OutU16( count );
    ObjSeek( curr_obj );
    OutPtr( CSHead->cs_info.cases->sel_expr );
}


void    GAsgnGoTo( bool list ) {
//==============================

// Generate an ASSIGNed GOTO.

    if( !list ) {
        if( WildLabel == NULL ) {
            WildLabel = StaticAlloc( sizeof( inttarg ), FT_INTEGER_TARG );
        }
        EmitOp( FC_PUSH );
        OutPtr( CITNode->sym_ptr );
        EmitOp( FC_POP );
        OutPtr( WildLabel );
        DumpTypes( FT_INTEGER_TARG, sizeof( inttarg ), CITNode->typ, CITNode->size );
        EmitOp( FC_EXPR_DONE );
        GBranch( StNumbers.branches );      // goto select table
    } else {
        EmitOp( FC_ASSIGNED_GOTO_LIST );
        OutPtr( CITNode->sym_ptr );
    }
}


void    GEndBrTab( void ) {
//===================

// Mark the end of the statement table.

    OutPtr( NULL );
}


void    GAssign( sym_id label ) {
//===============================

// Generate an ASSIGN <label> to I.

    EmitOp( FC_ASSIGN );
    OutPtr( label );
    OutPtr( CITNode->sym_ptr );
}


void    GBreak( FCODE routine ) {
//=====================================

// Generate a STOP or a PAUSE.

    EmitOp( routine );
    if( RecNOpn() ) {
        OutPtr( NULL );
    } else {
        OutPtr( CITNode->sym_ptr );
        CITNode->sym_ptr->u.lt.flags |= LT_SCB_REQUIRED;
    }
}


void    GStartSF( void ) {
//==================

// Start a statement function.

    EmitOp( FC_START_SF );
    if( OZOpts & OZOPT_O_INLINE ) {
        SFSymId->u.ns.si.sf.u.sequence = ObjTell();
        OutObjPtr( 0 );
    } else {
        OutPtr( SFSymId );
        SFSymId->u.ns.si.sf.u.location = NextLabel();
        OutU16( NextLabel() );
    }
}


void    GEndSF( void ) {
//================

// Terminate a statement function.

    obj_ptr     curr_obj;

    if( OZOpts & OZOPT_O_INLINE ) {
        EmitOp( FC_END_OF_SEQUENCE );
        curr_obj = ObjSeek( SFSymId->u.ns.si.sf.u.sequence );
        OutObjPtr( curr_obj );
        ObjSeek( curr_obj );
    } else {
        if( TypeCmplx( SFSymId->u.ns.u1.s.typ ) ) {
            EmitOp( FC_CMPLX_EXPR_DONE );
        } else {
            EmitOp( FC_EXPR_DONE );
        }
        EmitOp( FC_END_SF );
        OutPtr( SFSymId );
    }
}


void    GStartBlock( void ) {
//=====================

// Start a REMOTE BLOCK.

    EmitOp( FC_START_RB );
    OutPtr( CITNode->sym_ptr );
}


void    GExecute( void ) {
//==================

// Execute a REMOTE BLOCK.

    EmitOp( FC_EXECUTE );
    OutPtr( CITNode->sym_ptr );
}


void    GEndBlock( void ) {
//===================

// Terminate a REMOTE-block.

    EmitOp( FC_END_RB );
}


void    GPgmLabel( void ) {
//===================

// Generate a program label.

    EmitOp( FC_PROLOGUE );
    OutPtr( SubProgId );
}


void    GSegLabel( void ) {
//===================

// Generate a subprogram label.

    EmitOp( FC_PROLOGUE );
    OutPtr( SubProgId );
}


void    GBlockLabel( void ) {
//=====================

// Generate a block data subprogram label.

    EmitOp( FC_PROLOGUE );
    OutPtr( SubProgId );
}


void    FreeLabel( int label ) {
//==============================

// Generate F-Code indicating we are done with label.

    EmitOp( FC_FREE_LABEL );
    OutU16( label );
}
