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


#include "plusplus.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "cgaux.h"
#include "vstk.h"
#include "initdefs.h"

typedef struct {                // CGEXPR -- codegen expression
    cg_name expr;               // - expression
    cg_type type;               // - expression type
    unsigned garbage : 1;       // - true ==> is garbage
} CGEXPR;

static VSTK_CTL expressions;    // expressions stack


static void init(               // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkOpen( &expressions, sizeof( CGEXPR ), 128 );
}


static void fini(               // COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkClose( &expressions );
}

INITDEFN( cg_expr, init, fini );


void CgExprPush(                // PUSH CG EXPRESSION RESULT
    cg_name expr,               // - expression
    cg_type type )              // - expression type
{
    CGEXPR* top;                // - next top of stack

    top = VstkPush( &expressions );
    top->expr = expr;
    top->type = type;
    top->garbage = false;
}


cg_name CgExprPop(              // POP CG EXPRESSION
    void )
{
    cg_type not_used;

    return CgExprPopType( &not_used );
}


cg_name CgExprPopType(          // POP CG EXPRESSION and TYPE
    cg_type* a_type )           // - addr[ type ]
{
    CGEXPR* top;                // - next top of stack
    cg_name retn;               // - NULL or top expression

    top = VstkPop( &expressions );
    if( top == NULL ) {
        retn = NULL;
    } else if( top->garbage ) {
        *a_type = TY_UINT_4;
        retn = CGInteger( 12345, TY_UINT_4 );
    } else {
        *a_type = top->type;
        retn = top->expr;
    }
    return retn;
}


void CgPushGarbage(             // PUSH GARBAGE (TO BE TRASHED/POPPED)
    void )
{
    CGEXPR* top;                // - next top of stack

    top = VstkPush( &expressions );
    top->expr = NULL;
    top->type = 0;
    top->garbage = true;
}


bool CgExprPopGarbage(          // POP EXPR STACK IF TOP EXPR IS GARBAGE
    void )
{
    CGEXPR* top;                // - top of stack
    bool retb;                  // - true ==> garbage was popped

    top = VstkTop( &expressions );
    if( top == NULL ) {
        retb = true;
    } else if( top->garbage ) {
        VstkPop( &expressions );
        retb = true;
    } else {
        retb = false;
    }
    return( retb );
}


unsigned CgExprStackSize(       // RETURN # TEMPS STACKED
    void )
{
    return VstkDimension( &expressions );
}


#if 0
static SYMBOL getExprTempSym(   // EMIT CGDone, CGTrash, OR COPY TO TEMP
    cg_name expr,               // - expression
    cg_type type,               // - type of expression
    FN_CTL* fctl,               // - function control
    DGRP_FLAGS pop_type )       // - type of popping destruction
{
    SYMBOL temp;                // - NULL or copied temp

    if( pop_type & DGRP_DONE ) {
        CGDone( expr );
        temp = NULL;
    } else if( pop_type & DGRP_TRASH ) {
        CGTrash( expr );
        temp = NULL;
    } else if( fctl->temp_dtoring ) {
        temp = CgVarTemp( BETypeLength( type ) );
        CGDone( CGLVAssign( CgSymbol( temp ), expr, type ) );
    } else {
        temp = NULL;
    }
    fctl->temp_dtoring = false;
    return temp;
}
#endif


void CgExprDtored(              // DTOR CG EXPRESSION
    cg_name expr,               // - expression
    cg_type type,               // - expression type
    DGRP_FLAGS pop_type,        // - type of popping destruction
    FN_CTL* fctl )              // - function control
{
#if 0
    cg_type type;               // - expression type

    switch( CgExprStackSize() ) {
      case 0 :
        break;
      case 1 :
      { bool temp_dtoring = fctl->temp_dtoring;
        SYMBOL temp = getExprTempSym( &type, fctl, pop_type );
        if( temp_dtoring ) {
            if( fctl->ctor_test ) {
                pop_type |= DGRP_CTOR;
            }
            CgDestructExprTemps( pop_type, fctl );
            if( NULL != temp ) {
                CgExprPush( CgFetchSym( temp ), type );
            }
        }
      } break;
      DbgDefault( "CgExprDtored -- too many temps" );
    }
#else
    SYMBOL temp;                // - NULL or copied temp

    DbgVerify( 0 == CgExprStackSize(), "CgExprDtored -- more than one expr" );
    if( expr != NULL ) {
        if( pop_type & DGRP_DONE ) {
            CGDone( expr );
            temp = NULL;
        } else if( pop_type & DGRP_TRASH ) {
            CGTrash( expr );
            temp = NULL;
        } else if( fctl->temp_dtoring ) {
            temp = CgVarTempTyped( type );
            CGDone( CGLVAssign( CgSymbol( temp ), expr, type ) );
        } else {
            CgExprPush( expr, type );
            temp = NULL;
        }
        if( fctl->temp_dtoring ) {
            fctl->temp_dtoring = false;
            if( fctl->ctor_test ) {
                pop_type |= DGRP_CTOR;
            }
            CgDestructExprTemps( pop_type, fctl );
            if( NULL != temp ) {
                CgExprPush( CgFetchSym( temp ), type );
            }
        }
    }
#endif
}


void CgExprAttr(                // SET CONST/VOLATILE/etc. ATTRIBUTES FOR EXPR
    cg_sym_attr attr )          // - attribute
{
    cg_type type;                   // - expression type
    cg_name expr;                   // - top expression

    expr = CgExprPopType( &type );
    expr = CGAttr( expr, attr );
    CgExprPush( expr, type );
}

void CgExprPushWithAttr(        // PUSH EXPR WITH ATTRIBUTES
    cg_name expr,               // - expression
    cg_type type,               // - expression type
    cg_sym_attr attr )          // - expression attribute
{
    CgExprPush( CGAttr( expr, attr ), type );
}
