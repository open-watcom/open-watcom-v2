/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "namecod.h"
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


bool    RecEquOpr( void ) {
//====================

    return( CITNode->opr == OPR_EQU );
}


bool    ReqOperator( OPR operator, int error ) {
//===============================================

    if( CITNode->opr == operator )
        return( true );
    Error( error );
    return( false );
}


bool    ReqEquOpr( void ) {
//====================

    return( ReqOperator( OPR_EQU, EQ_NO_EQUALS ) );
}


bool    RecColonOpr( void ) {
//==================

    return( CITNode->opr == OPR_COL );
}


bool    RecCommaOpr( void ) {
//==================

    return( CITNode->opr == OPR_COM );
}


bool    ReqCommaOpr( void ) {
//==================

    return( ReqOperator( OPR_COM, SX_MISSING_COMMA ) );
}


bool    ReqColonOpr( void ) {
//==================

    return( ReqOperator( OPR_COL, SX_NO_COLON ) );
}


bool    RecCloseParenOpr( void ) {
//=======================

    return( CITNode->opr == OPR_RBR );
}


bool    ReqCloseParenOpr( void ) {
//=======================

    return( ReqOperator( OPR_RBR, PC_NO_CLOSEPAREN ) );
}


bool    RecOpenParenOpr( void ) {
//======================

    return( CITNode->opr == OPR_LBR );
}


bool    ReqOpenParenOpr( void ) {
//======================

    return( ReqOperator( OPR_LBR, PC_NO_OPENPAREN ) );
}

bool    RecMulOpr( void ) {
//================

    return( CITNode->opr == OPR_MUL );
}


bool    ReqMulOpr( void ) {
//================

    return( ReqOperator( OPR_MUL, SX_NO_ASTERISK ) );
}


bool    RecDivOpr( void ) {
//================

    return( CITNode->opr == OPR_DIV );
}


bool    ReqDivOpr( void ) {
//================

    return( ReqOperator( OPR_DIV, SX_NO_SLASH ) );
}


bool    RecPlusOpr( void ) {
//=================

    return( CITNode->opr == OPR_PLS );
}


bool    RecMinOpr( void ) {
//================

    return( CITNode->opr == OPR_MIN );
}


bool    RecCatOpr( void ) {
//================

    return( CITNode->opr == OPR_CAT );
}


bool    RecNoOpr( void )
//======================
{
    return( CITNode->opr == OPR_PHI );
}


bool    RecFBrOpr( void ) {
//================

    return( CITNode->opr == OPR_FBR );
}


bool    RecTrmOpr( void ) {
//===================

    return( CITNode->opr == OPR_TRM );
}


bool    RecEOSOpr( void ) {
//================

    return( ( CITNode->opr == OPR_TRM ) && ( CITNode->oprpos == 9999 ) );
}


bool    ReqEOSOpr( void ) {
//================

    if( RecEOSOpr() )
        return( true );
    Error( SX_EOS_EXPECTED );
    return( false );
}


bool    RecNoOpn( void ) {
//=================

    return( CITNode->opn.ds == DSOPN_PHI );
}


bool    ReqNoOpn( void ) {
//=================

    if( RecNoOpn() )
        return( true );
    OpndErr( SX_UNEXPECTED_OPN );
    return( false );
}


bool    RecNameOpn( void ) {
//=================

    return( CITNode->opn.ds == DSOPN_NAM );
}


bool    ReqNameOpn( clsname_id clsname )
//===================================
{
    if( RecNameOpn() )
        return( true );
    KnownClassErr( SX_NO_NAME, clsname );
    return( false );
}


bool    RecKeyWordOpn( const char *key )
//===================================
{
    if( CITNode->opn.ds != DSOPN_NAM )
        return( false );
    return( CmpNode2Str( CITNode, key ) );
}


bool    RecNWL( void ) {
//================

    return( CITNode->opn.us == USOPN_NWL );
}


bool    RecNumberOpn( void ) {
//===================

    return( CITNode->opn.ds == DSOPN_INT );
}


bool    RecLiteralOpn( void ) {
//====================

    return( CITNode->opn.ds == DSOPN_LIT );
}


bool    RecNextOpr( OPR operator ) {
//===================================

    return( CITNode->link->opr == operator );
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

    if( !RecNameOpn() )
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


bool    RecIntVarOpn( void ) {
//===================

    if( !IsVariable() )
        return( false );
    return( _IsTypeInteger( CITNode->typ ) );
}


bool    ReqIntVarOpn( void ) {
//===================

    if( RecIntVarOpn() )
        return( true );
    Error( SX_NO_INTEGER_VAR );
    return( false );
}


bool    ReqDoVarOpn( void ) {
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
