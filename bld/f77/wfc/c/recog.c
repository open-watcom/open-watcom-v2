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
* Description:  recognize specific tokens
*
****************************************************************************/


#include "ftnstd.h"
#include "opr.h"
#include "opn.h"
#include "errcod.h"
#include "global.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "arutls.h"
#include "dsname.h"
#include "recog.h"
#include "symtab.h"


// Routines with names 'Rec...' attempt to recognize '...',
//   returning true if found, else false
//
// Routines with names 'Req...' require that '...' be found,
//   generating an error message if not found


bool    RecEquSign( void ) {
//====================

    return( OPR_EQU == CITNode->opr );
}


bool    ReqOperator( OPR operator, int error ) {
//===============================================

    if( operator == CITNode->opr )
        return( true );
    Error( error );
    return( false );
}


bool    ReqEquSign( void ) {
//====================

    return( ReqOperator( OPR_EQU, EQ_NO_EQUALS ) );
}


bool    RecColon( void ) {
//==================

    return( OPR_COL == CITNode->opr );
}


bool    RecComma( void ) {
//==================

    return( OPR_COM == CITNode->opr );
}


bool    ReqComma( void ) {
//==================

    return( ReqOperator( OPR_COM, SX_MISSING_COMMA ) );
}


bool    ReqColon( void ) {
//==================

    return( ReqOperator( OPR_COL, SX_NO_COLON ) );
}


bool    RecCloseParen( void ) {
//=======================

    return( OPR_RBR == CITNode->opr );
}


bool    ReqCloseParen( void ) {
//=======================

    return( ReqOperator( OPR_RBR, PC_NO_CLOSEPAREN ) );
}


bool    RecOpenParen( void ) {
//======================

    return( OPR_LBR == CITNode->opr );
}


bool    ReqOpenParen( void ) {
//======================

    return( ReqOperator( OPR_LBR, PC_NO_OPENPAREN ) );
}

bool    RecMul( void ) {
//================

    return( OPR_MUL == CITNode->opr );
}


bool    ReqMul( void ) {
//================

    return( ReqOperator( OPR_MUL, SX_NO_ASTERISK ) );
}


bool    RecDiv( void ) {
//================

    return( OPR_DIV == CITNode->opr );
}


bool    ReqDiv( void ) {
//================

    return( ReqOperator( OPR_DIV, SX_NO_SLASH ) );
}


bool    RecPlus( void ) {
//=================

    return( OPR_PLS == CITNode->opr );
}


bool    RecMin( void ) {
//================

    return( OPR_MIN == CITNode->opr );
}


bool    RecCat( void ) {
//================

    return( OPR_CAT == CITNode->opr );
}


bool    RecNOpr( void ) {
//=================

    return( OPR_PHI == CITNode->opr );
}


bool    RecFBr( void ) {
//================

    return( OPR_FBR == CITNode->opr );
}


bool    RecTrmOpr( void ) {
//===================

    return( OPR_TRM == CITNode->opr );
}


bool    RecEOS( void ) {
//================

    return( ( OPR_TRM == CITNode->opr ) && ( CITNode->oprpos == 9999 ) );
}


bool    ReqEOS( void ) {
//================

    if( RecEOS() )
        return( true );
    Error( SX_EOS_EXPECTED );
    return( false );
}


bool    RecNOpn( void ) {
//=================

    return( CITNode->opn.ds == DSOPN_PHI );
}


bool    ReqNOpn( void ) {
//=================

    if( RecNOpn() )
        return( true );
    OpndErr( SX_UNEXPECTED_OPN );
    return( false );
}


bool    RecKeyWord( char *key ) {
//===============================

    if( CITNode->opn.ds != DSOPN_NAM )
        return( false );
    return( CmpNode2Str( CITNode, key ) );
}


bool    RecName( void ) {
//=================

    return( CITNode->opn.ds == DSOPN_NAM );
}


bool    ReqName( int index ) {
//============================

    if( RecName() )
        return( true );
    KnownClassErr( SX_NO_NAME, index );
    return( false );
}


bool    RecNWL( void ) {
//================

    return( CITNode->opn.us == USOPN_NWL );
}


bool    RecNumber( void ) {
//===================

    return( CITNode->opn.ds == DSOPN_INT );
}


bool    RecLiteral( void ) {
//====================

    return( CITNode->opn.ds == DSOPN_LIT );
}


bool    RecNextOpr( OPR operator ) {
//===================================

    return( operator == CITNode->link->opr );
}


bool    ReqNextOpr( OPR operator, int error ) {
//==============================================

    if( RecNextOpr( operator ) )
        return( true );
    AdvError( error );
    return( false );
}


static  bool    IsVariable( void ) {
//============================

    unsigned_16 flags;

    if( !RecName() )
        return( false );
    LkSym();
    flags = CITNode->flags;
    if( (flags & SY_CLASS) == SY_VARIABLE ) {
        if( flags & SY_SUBSCRIPTED )
            return( false );
        return( true );
    }
    if( (flags & SY_CLASS) == SY_SUBPROGRAM ) {
        if( (flags & SY_SUBPROG_TYPE) != SY_FUNCTION )
            return( false );
        if( (flags & SY_PS_ENTRY) == 0 )
            return( false );
        GetFunctionShadow();
        return( true );
    }
    return( false );
}


bool    RecIntVar( void ) {
//===================

    if( !IsVariable() )
        return( false );
    return( _IsTypeInteger( CITNode->typ ) );
}


bool    ReqIntVar( void ) {
//===================

    if( RecIntVar() )
        return( true );
    Error( SX_NO_INTEGER_VAR );
    return( false );
}


bool    ReqDoVar( void ) {
//==================

    if( IsVariable() ) {
        if( ( CITNode->typ >= FT_INTEGER_1 ) &&
            ( CITNode->typ <= FT_EXTENDED ) ) {
            return( true );
        }
    }
    Error( SX_NO_NUMBER_VAR );
    return( false );
}


bool    RecArrName( void ) {
//====================

    return( (CITNode->opn.us & USOPN_WHAT) == USOPN_ARR );
}
