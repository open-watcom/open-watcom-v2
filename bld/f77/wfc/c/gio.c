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
// GIO       : I/O code generation routines
//

#include "ftnstd.h"
#include "fcodes.h"
#include "global.h"
#include "rtconst.h"
#include "types.h"
#include "iodefs.h"
#include "fcgbls.h"
#include "stmtsw.h"
#include "opn.h"
#include "cpopt.h"
#include "emitobj.h"
#include "ioperm.h"
#include "ioutls.h"
#include "gio.h"
#include "gstring.h"
#include "gflow.h"


void    GSetIOCB( void ) {
//==================

// Generate a call to set the IOCB.

    EmitOp( FC_SET_IOCB );
}


void    GStartIO( void ) {
//==================

// Generate code to invoke the run-time routine.

    if( !AuxIOStmt() && NotFormatted() ) {
        EmitOp( FC_SET_NOFMT );
    }
    EmitOp( FC_IO_STMTS + IOIndex() );
    // PRINT, READ and WRITE i/o statements can check for END= and ERR=
    // statement labels when RT_ENDIO is generated; auxilliary i/o
    // statements don't generate RT_ENDIO so generate F-Code to check
    // for statement labels.
    if( AuxIOStmt() || Already( IO_NAMELIST ) ) {
        EmitOp( FC_CHK_IO_STMT_LABEL );
    }
}


void    GIOStruct( sym_id sd ) {
//==============================

// Generate code to do structure i/o.

    PushOpn( CITNode );
    if( StmtProc == PR_READ ) {
        EmitOp( FC_INPUT_STRUCT );
    } else {
        EmitOp( FC_OUTPUT_STRUCT );
    }
    OutPtr( sd ); // structure definition
}


static  void    GIORoutine( TYPE typ, uint size ) {
//=================================================

    FCODE   op_code;

    op_code = ParmType( typ, size ) - PT_LOG_1;
    if( StmtProc == PR_READ ) {
        EmitOp( op_code + FC_INP_LOG1 );
    } else {
        EmitOp( op_code + FC_OUT_LOG1 );
    }
}


void    GIOItem( void ) {
//=================

// Generate code to process an i/o list item.

    PushOpn( CITNode );
    GIORoutine( CITNode->typ, CITNode->size );
}


void    GIOArray( void ) {
//==================

// Generate code to do array i/o.

    if( StmtProc == PR_READ ) {
        EmitOp( FC_INP_ARRAY );
    } else {
        EmitOp( FC_PRT_ARRAY );
    }
    OutPtr( CITNode->sym_ptr );
    if( CITNode->opn.us & USOPN_FLD ) {
        OutPtr( CITNode->value.st.field_id );
    } else {
        OutPtr( NULL );
    }
}


void    GIOStructArray( void ) {
//========================

// Generate code to do structured array i/o.

    if( StmtProc == PR_READ ) {
        EmitOp( FC_STRUCT_INP_ARRAY );
    } else {
        EmitOp( FC_STRUCT_PRT_ARRAY );
    }
    OutPtr( CITNode->sym_ptr );
}


void    GStopIO( void ) {
//=================

// Generate code to return a null i/o item to run-time i/o.
// This is done for only PRINT, WRITE and READ statements.

    if( !Already( IO_NAMELIST ) ) {
        EmitOp( FC_ENDIO );
    }
}


void    GPassValue( FCODE rtn ) {
//===============================

// Pass the value of CITNode on the stack and emit fcode for routine.

    PushOpn( CITNode );
    EmitOp( rtn );
    if( ( rtn == FC_SET_UNIT ) || ( rtn == FC_SET_REC ) ||
        ( rtn == FC_SET_RECL ) || ( rtn == FC_SET_BLOCKSIZE ) ) {
        GenType( CITNode );
    }
}


void    GSetNameList( FCODE routine ) {
//=====================================

// Pass the address of NAMELIST data for run-time routine.

    EmitOp( routine );
    OutPtr( CITNode->sym_ptr );
}


void    GPassAddr( FCODE routine ) {
//==================================

// Pass the address of CITNode on the stack and emit fcode for routine.

    PushOpn( CITNode );
    EmitOp( routine );
}


void    GPassStmtNo( sym_id sn, FCODE routine ) {
//===============================================

// Pass the label for a statement number. For example,
//        PRINT 10, ...
// 10     FORMAT( ... )

    EmitOp( routine );
    GStmtAddr( sn );
}


void    GPassLabel( label_id label, RTCODE routine ) {
//==================================================

// Pass the label identifying encoded format string.
// Called when using
//        PRINT <constant character expression>, ...

    EmitOp( FC_PASS_LABEL );
    OutU16( routine );
    OutU16( label );
}


void    GFmtVarSet( void ) {
//====================

// Called when using
//        ASSIGN 10 TO I
//        PRINT I, ...
// 10     FORMAT( ... )

    EmitOp( FC_FMT_ASSIGN );
    OutPtr( CITNode->sym_ptr );
}


static  void    ChkExtendFmt( void ) {
//==============================

    if( Options & OPT_EXTEND_FORMAT ) {
        OutU16( 1 );
    } else {
        OutU16( 0 );
    }
}


void    GFmtArrSet( void ) {
//====================

// Called when using
//        PRINT <character array>, ...

    EmitOp( FC_FMT_ARR_SCAN );
    OutPtr( CITNode->sym_ptr );
    ChkExtendFmt();
}


void    GFmtExprSet( void ) {
//=====================

// Pass the label identifying encoded format string.
// Called when using
//        PRINT <character expression>, ...

    PushOpn( CITNode );
    EmitOp( FC_FMT_SCAN );
    ChkExtendFmt();
}


void    GArrIntlSet( void ) {
//=====================

// Set internal file pointer to array.

    EmitOp( FC_ARR_SET_INTL );
    OutPtr( CITNode->sym_ptr );
    OutPtr( GTempString( 0 ) );
}


void    GIntlSet( void ) {
//==================

// Set internal file pointer to character variable.

    PushOpn( CITNode );
    EmitOp( FC_SET_INTL );
}


void    GCheckEOF( label_id label ) {
//===================================

// Patch the label emitted by GNullEofStmt() to be the label at the end
// of the code of the ATEND statement.

    obj_ptr     curr_obj;

    curr_obj = ObjSeek( AtEndFCode );
    EmitOp( FC_SET_ATEND );
    OutU16( label );
    ObjSeek( curr_obj );
}


void    GNullEofStmt( void ) {
//======================

// Emit the "null" F-Code.
// If an ATEND statement follows, the "null" F-Code will be patched with a
// RT_SET_END F-Code.

    AtEndFCode = ObjTell();
    EmitOp( FC_NULL_FCODE );
    EmitOp( FC_NULL_FCODE );
}
