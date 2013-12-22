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
#include "cgfront.h"
#include "codegen.h"
#include "fnovload.h"
#include "vfun.h"
#include "extrf.h"


struct walk_data                // data for walker
{   SYMBOL vfun;                // - original virtual function
    void *data;                 // - data for walker
    void (*rtn)(                // - routine called for self, ancestors
        SYMBOL orig,            // - - original virtual function
        SYMBOL vfun,            // - - self or ancestor
        void *data );           // - - data passed
};


static SYMBOL getSymInScope(    // GET A SYMBOL IN A SCOPE
    SCOPE scope,                // - the scope
    SYMBOL orig )               // - original symbol
{
    SEARCH_RESULT *result;      // - search result for symbol
    SYMBOL sym;                 // - the symbol
    arg_list *alist;            // - arguments for function
    NAME name;                  // - symbol name

    name = orig->name->name;
    alist = SymFuncArgList( orig );
    if( name == CppConversionName() ) {
        result = ScopeFindScopedMemberConversion( scope
                                                , scope
                                                , SymFuncReturnType( orig )
                                                , alist->qualifier
                                                );
        if( result == NULL ) {
            sym = NULL;
        } else {
            sym = result->sym;
            ScopeFreeResult( result );
            if( sym != NULL ) {
                if( SymScope( sym ) != scope ) {
                    sym = NULL;
                }
            }
        }
    } else {
        result = ScopeContainsMember( scope, name );
        if( result == NULL ) {
            sym = NULL;
        } else {
            sym = result->sym_name->name_syms;
            // we may have found a type-name, so check for NULL
            if( sym != NULL && SymIsFunction( sym ) ) {
                switch( FuncOverloaded( &sym
                                      , result
                                      , sym
                                      , alist
                                      , NULL ) ) {
                  case FNOV_NONAMBIGUOUS :
                    break;
                  default :
                    sym = NULL;
                    break;
                }
            } else {
                sym = NULL;
            }
            ScopeFreeResult( result );
        }
    }
    return sym;
}


// what about hiding ?
//  - although it's wrong, eveybody uses the same virtual f
//
// A f
// |
// B hide f
// |
// C f
//
static void classVisit(         // VISIT ANCESTRAL CLASS
    SCOPE scope,                // - class scope
    void *_wdata )   // - internal data for walker
{
    struct walk_data *wdata = _wdata;
    SYMBOL sym;                 // - original symbol
    SYMBOL csym;                // - corresponding class symbol

    sym = wdata->vfun;
    csym = getSymInScope( scope, sym );
    if( ( csym != NULL )
      &&( SymIsVirtual( csym ) )
      &&( ScopeSameVFuns( sym, csym ) ) ) {
        (*wdata->rtn)( sym, csym, wdata->data );
    }
}


void VfnAncestralWalk(          // WALK ANCESTRAL VIRTUAL FUNCTIONS
    SYMBOL sym,                 // - a virtual function
    void (*rtn)(                // - routine called for self, ancestors
        SYMBOL orig,            // - - original virtual function
        SYMBOL vfun,            // - - self or ancestor
        void *data ),           // - - data passed
    void *data )                // - data passed to routine
{
    struct walk_data wdata;     // - internal data for walker

    wdata.vfun = sym;
    wdata.data = data;
    wdata.rtn = rtn;
    ScopeWalkAncestry( SymScope( sym ), &classVisit, &wdata );
}


static PTREE genVfunCall(       // DIRECTLY GENERATE VFUN CALL
    target_offset_t vf_offset,  // - offset to VF table ptr
    PTREE node,                 // - original "this" expression
    vindex vf_index,            // - index into VF table
    SYMBOL sym )                // - symbol to access
{
    TYPE vfptr_type;            // - type[ ptr to VF table ]
    TYPE vfn_type;              // - type[ ptr to VF ]

    node = NodeBinary( CO_DOT, node, NodeOffset( vf_offset ) );
    vfptr_type = MakeVFTableFieldType( TRUE );
    node->type = vfptr_type;
    node->flags |= PTF_LVALUE;
    node = PtdExprConst( node );
    node = NodeRvalue( node );
    vfn_type = TypePointedAtModified( vfptr_type );
    node = NodeBinary( CO_DOT, node, NodeOffset( vf_index * CgMemorySize( vfn_type ) ) );
    node->type = vfn_type;
    node->flags |= PTF_LVALUE;
    node = NodeRvalue( node );
    node->type = sym->sym_type;
    return node;
}


// When bound, the function is called directly.
//
// When not bound,
//      - fetch reference
//      - add vf_offset
//      - fetch VFT ptr
//      - index using vf_index
//
static PTREE genVfunIcs(        // GENERATE IC'S FOR CG-GENERATION OF VFUN CALL
    target_offset_t vf_offset,  // - offset to VF table ptr
    PTREE node,                 // - original "this" expression
    vindex vf_index,            // - index into VF table
    SYMBOL baser,               // - basing "this" symbol
    SYMBOL vfun )               // - virtual function
{
    PTREE expr;                 // - expression under construction

    expr = NodeMakeCallee( vfun );
    expr->cgop = CO_IGNORE;
    expr = NodeUnary( CO_CALL_SETUP_VFUN, expr );
    expr->type = vfun->sym_type;
    expr->flags = node->flags;
    expr = PtdVfunAccess( expr, vf_index, vf_offset, baser );
    node = NodeUnaryCopy( CO_VFUN_PTR, node );
    return NodeComma( node, expr );
}


PTREE AccessVirtualFnAddress(   // GET ADDRESS OF VIRTUAL FUNCTION
    PTREE node,                 // - class pointer
    SEARCH_RESULT *result,      // - access info
    SYMBOL sym )                // - symbol to access (virtual fun)
{
    SYMBOL vfun;                // - virtual function to call thru table
    SYMBOL base_this;           // - basing "this" symbol
    target_offset_t this_offset;// - offset to "this" basing symbol
    target_offset_t vf_offset;  // - offset to VF PTR
    vindex vf_index;          // - index in VF table

    vf_offset = result->vf_offset;
    vfun = SymDefaultBase( sym );
    vf_index = vfun->u.member_vf_index - 1;
    if( NodeGetIbpSymbol( node, &base_this, &this_offset ) ) {
        node = genVfunIcs( vf_offset, node, vf_index, base_this, vfun );
    } else {
        node = genVfunCall( vf_offset, node, vf_index, sym );
    }
    return( node );
}


PTREE VfunSetupCall(            // SETUP CODE FOR VIRTUAL FUNCTION CALL
    PTREE expr )                // - expression for setup
{
    return NodeUnaryCopy( CO_CALL_SETUP_IND, expr );
}


PTREE VfnDecorateCall(          // DECORATE VIRTUAL CALL
    PTREE expr,                 // - call-setup expression
    SYMBOL vfun )               // - virtual function
{
    PTREE sym;                  // - node for virtual function call

    vfun = SymDefaultBase( vfun );
    sym = NodeMakeCallee( vfun );
    sym->cgop = CO_IGNORE;
    sym = NodeBinary( CO_VIRT_FUNC, expr, sym );
    sym->flags = expr->flags;
    sym->type = expr->type;
    return sym;
}


// This is kluge because of the lack of a code-generation interface to
// signal a virtual function reference.
//
// This is accomplished by putting out a fake virtual call in dead-code.
//
void VfnReference(              // EMIT VIRTUAL FUNCTION REFERENCE
    SYMBOL vfun )               // - a virtual function
{
    CGLABEL around;             // - label for jump around
    PTREE fake;                 // - fake call expression

    around = CgFrontLabelCs();
    CgFrontGotoNear( IC_LABEL_CS, O_GOTO, around );
    fake = NodeAssignTemporary( MakePointerTo( vfun->sym_type )
                              , NodeMakeCallee( vfun ) );
    fake = NodeRvalue( fake );
    fake = NodeUnaryCopy( CO_CALL_SETUP_IND, fake );
    fake = VfnDecorateCall( fake, vfun );
    fake = NodeBinary( CO_CALL_EXEC_IND, fake, NULL );
    fake->type = SymFuncReturnType( vfun );
    fake->flags |= PTF_MEANINGFUL | PTF_SIDE_EFF;
    fake = NodeDone( fake );
    IcEmitExpr( fake );
    CgFrontLabdefCs( around );
    CgFrontLabfreeCs( 1 );
}
