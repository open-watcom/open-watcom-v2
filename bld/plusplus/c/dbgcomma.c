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
#ifdef __WATCOMC__
#include <process.h>
#endif
#include "name.h"
#include "cgfront.h"
#include "initdefs.h"
#include "dbg.h"
#include "errdefns.h"

#define RTN_COMMA_AT "__dbg_comma_at"
#define RTN_COMMA_OP "__dbg_comma_op"

static SYMBOL   sym_at;           // - atom routine
static SYMBOL   sym_op;           // - operator routine

static NAME     name_at;
static NAME     name_op;

static SYMBOL defineRoutine     // DEFINE R/T ROUTINE
    ( NAME name )               // - routine name
{
    SYMBOL sym;                 // - created symbol
    TYPE sym_type;              // - symbol's type
    symbol_flag flags;          // - symbol's flags

    sym_type = TypeVoidFunOfVoid();
    flags = SF_REFERENCED | SF_NO_LONGJUMP;
    sym = SymCreateFileScope( sym_type, SC_EXTERN, flags, name );
    LinkageSet( sym, "C" );
    return sym;
}


static void init                // INITIALIZATION FOR MODULE
    ( INITFINI* defn )          // - definition
{
    defn = defn;

    if( NULL != CppGetEnv( "WPPCOMOP" ) ) {
        name_op = NameCreateNoLen( RTN_COMMA_OP );
        sym_op = defineRoutine( name_op );
    } else {
        sym_op = NULL;
    }
    if( NULL != CppGetEnv( "WPPCOMAT" ) ) {
        name_at = NameCreateNoLen( RTN_COMMA_AT );
        sym_at = defineRoutine( name_at );
    } else {
        sym_at = NULL;
    }
}


static void fini(               // COMPLETION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
}


INITDEFN( comma_insertion, init, fini )


static PTREE insert             // INSERT AT NODE
    ( PTREE node                // - node
    , NAME name )               // - routine name
{
    PTREE left;                 // - new left operand
    PTREE comma;                // - new comma node

    left = PTreeId( name );
    left = PTreeCopySrcLocation( left, node );
    left = PTreeBinary( CO_CALL, left, NULL );
    left = PTreeCopySrcLocation( left, node );
    comma = PTreeBinary( CO_COMMA, left, node );
    comma = PTreeCopySrcLocation( comma, node );
    return comma;
}


static void insertOperand       // INSERT FOR OPERAND, IF POSSIBLE
    ( PTREE* a_node )           // - addr[ operand ]
{
    PTREE node = *a_node;       // - operand
    SEARCH_RESULT *result;      // - result of lookup
    SYMBOL sym;                 // - a symbol looked up

    if( NULL != node ) {
        switch( node->op ) {
          case PT_ID :
            if( NULL != sym_at ) {
                if( node->op == PT_ID ) {
                    switch( node->cgop ) {
                      case CO_NAME_CDTOR_EXTRA :
                      case CO_NAME_DTOR :
                        break;
                      default :
                        result = ScopeFindNaked( GetCurrScope(), node->u.id.name );
                        if( NULL == result ) break;
                        sym = result->sym_name->name_syms;
                        if( NULL == sym ) break;
                        if( SymIsAnError( sym ) ) break;
                        if( SymIsConstantInt( sym ) ) break;
                        // drops thru
                      case CO_NAME_THIS :
                      case CO_NAME_CONVERT :
                        node = insert( node, name_at );
                        break;
                    }
                }
            }
            break;
          case PT_BINARY :
            switch( node->cgop ) {
              case CO_ARROW :
              case CO_DOT :
                node = insert( node, name_op );
                break;
              default :
                break;
            }
            break;
          default :
            break;
        }
        *a_node = node;
    }
}


static PTREE insertNode         // INSERTION FOR A NODE
    ( PTREE node )              // - node
{
    switch( node->op ) {
      case PT_UNARY :
        if( NULL != sym_op ) {
            switch( node->cgop ) {
              case CO_SEGNAME :
                break;
              case CO_THROW :
              case CO_DELETE :
              case CO_DELETE_G :
              case CO_DELETE_ARRAY :
              case CO_DELETE_G_ARRAY :
                insertOperand( &node->u.subtree[0] );
                break;
              case CO_SIZEOF_TYPE :
              case CO_OFFSETOF :
              case CO_TYPEID_TYPE :
                node = insert( node, name_op );
                break;
              default :
                insertOperand( &node->u.subtree[0] );
                node = insert( node, name_op );
                break;
            }
        }
        break;
      case PT_BINARY :
        if( NULL != sym_op ) {
            switch( node->cgop ) {
              case CO_COMMA :
              case CO_COLON :
                insertOperand( &node->u.subtree[0] );
                insertOperand( &node->u.subtree[1] );
                break;
              case CO_LIST :
              case CO_INIT :
              case CO_RETURN :
              case CO_CONVERT :
              case CO_DYNAMIC_CAST :
              case CO_STATIC_CAST :
              case CO_REINTERPRET_CAST :
              case CO_CONST_CAST :
                insertOperand( &node->u.subtree[1] );
                break;
              case CO_ARROW :
              case CO_DOT :
              case CO_STORAGE :
              case CO_OPERATOR :
              case CO_COLON_COLON :
              case CO_SEG_OP :
              case CO_NEW_PARMS1 :
              case CO_NEW_PARMS2 :
                break;
              case CO_SIZEOF_TYPE :
              case CO_NEW :
              case CO_NEW_G :
              case CO_NEW_ARRAY :
                node = insert( node, name_op );
                break;
              default :
                insertOperand( &node->u.subtree[0] );
                insertOperand( &node->u.subtree[1] );
                node = insert( node, name_op );
                break;
            }
        }
        break;
      case PT_IC :
      case PT_SYMBOL :
      case PT_DUP_EXPR :
        CFatal( "DbgComma -- bad parse tree" );
      default :
        break;
    }
    return node;
}


PTREE DbgCommaInsertion         // COMMA INSERTION ROUTINE
    ( PTREE expr )              // - expression
{
    if( NULL != sym_op || NULL != sym_at ) {
        expr = PTreeTraversePostfix( expr, &insertNode );
    }
    return expr;
}
