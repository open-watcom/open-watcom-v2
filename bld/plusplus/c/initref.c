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
#include "objmodel.h"
#include "fnbody.h"

#ifndef NDEBUG
    #include "errdefns.h"
#endif


static SYMBOL inited_temp;      // first init'ed struct temp in parsed tree
static PTREE last_dtor_node;    // last CO_DTOR_NODE
static PTREE last_init_node;    // last initing node


static void zapLifeTime(        // ZAP THE TEMPORARY LIFE-TIME IF REQ'D
    PTREE node,                 // - possible node to be zapped
    SYMBOL sym,                 // - temporary
    symbol_class id )           // - id of temp
{
    if( node->op == PT_SYMBOL
     && node->u.symcg.symbol == sym
     && sym->id == SC_AUTO ) {
        sym->id = id;
        if( id == SC_STATIC ) {
            DgSymbol( sym );
        } else {
            if( last_dtor_node != NULL
             && inited_temp
                == last_dtor_node->u.subtree[0]->u.symcg.symbol ) {
                last_dtor_node->flags |= PTF_BLOCK_INIT;
                CgFrontCodePtr( IC_INIT_REF_BEG, sym );
#if 0
                CgFrontCode( IC_EXPR_DONE );
                FunctionRegistrationFlag();
#else
                FunctionHasCtorTest();
#endif
                LabelDeclInited( sym );
            }
        }
    }
}


static PTREE processInitNode(   // PROCESS A NODE IN INITIALIZATION TREE
    PTREE node )                // - current node
{
    SYMBOL sym;

    switch( node->op ) {
      case PT_SYMBOL :
        if( inited_temp == NULL ) {
            sym = node->u.symcg.symbol;
            if( sym != NULL ) {
                if( StructType( sym->sym_type ) != NULL ) {
                    if( SymIsTemporary( sym ) ) {
                        inited_temp = sym;
                    }
                }
            }
        }
        break;
      case PT_BINARY :
        switch( node->cgop ) {
          case CO_CALL_EXEC :
          case CO_CALL_EXEC_IND :
          case CO_EQUAL :
          case CO_EQUAL_REF :
            last_init_node = node;
            break;
          case CO_DTOR :
            last_dtor_node = node;
            break;
        }
    }
    return node;
}


static boolean returnsStruct(   // TEST IF STRUCT RETURNED BY CALL
    PTREE call )                // - call expression
{
    TYPE func_type;             // - type of function call

    func_type = TypeFunctionCalled( NodeFuncForCall( call )->type );
#if 0
    return NULL != StructType( func_type->of );
#else
    return OMR_CLASS_REF == ObjModelFunctionReturn( func_type );
#endif
}


static PTREE getArgNode(        // GET ARGUMENT NODE OF SPECIFIED TYPE
    PTREE call,                 // - call expression
    unsigned flag )             // - flag for argument
{
    PTREE arg;                  // - current argument

    for( arg = call->u.subtree[1]; ; arg = arg->u.subtree[0] ) {
        DbgVerify( arg != NULL, "getArgNode -- no arg of type" );
        if( arg->flags & flag ) break;
    }
    return PTreeOp( &arg->u.subtree[1] );
}


void InitRefFixup(              // TRAVERSAL FOR REFERENCE INITIALIZATION
    PTREE expr )                // - initialization expression
{
    boolean zapped_auto;        // - TRUE ==> zapped to block scope
    PTREE setup;                // - CO_CALL_SETUP node
    PTREE caller;               // - PT_SYMBOL for caller
    symbol_class id;            // - CLASS FOR TEMP

    inited_temp = NULL;
    last_dtor_node = NULL;
    last_init_node = NULL;
    PTreeTraverseInitRef( expr, processInitNode );
    zapped_auto = FALSE;
    if( inited_temp != NULL
     && last_init_node != NULL ) {
        if( expr->flags & PTF_STATIC_INIT ) {
            id = SC_STATIC;
        } else {
            id = SC_AUTO;
        }
        switch( last_init_node->cgop ) {
          case CO_EQUAL :
          case CO_EQUAL_REF :
            zapLifeTime( last_init_node->u.subtree[0], inited_temp, id );
            break;
          case CO_CALL_EXEC :
            setup = PTreeOpLeft( last_init_node );
            caller = PTreeOpLeft( setup );
            if( SymIsCtor( caller->u.symcg.symbol ) ) {
                zapLifeTime( getArgNode( last_init_node, PTF_ARG_THIS )
                           , inited_temp
                           , id );
                break;
            }
            // drops thru
          case CO_CALL_EXEC_IND :
            if( returnsStruct( last_init_node ) ) {
                zapLifeTime( getArgNode( last_init_node, PTF_ARG_RETURN )
                           , inited_temp
                           , id );
            }
            break;
          DbgDefault( "StaticInitChangeLifeTime -- bad assignment node" );
        }
    }
}
