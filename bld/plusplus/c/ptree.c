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

#include <stddef.h>

#include "memmgr.h"
#include "stringl.h"
#include "cgfront.h"
#include "fnovload.h"
#include "ppintnam.h"
#include "name.h"
#include "class.h"
#include "carve.h"
#include "vstk.h"
#include "initdefs.h"
#include "stats.h"
#include "floatsup.h"
#include "codegen.h"
#include "pcheader.h"
#include "conpool.h"
#include "defarg.h"


#define MAX_DIGITS          30
#define BLOCK_PTREE         32
static carve_t carvePTREE;

ExtraRptCtr( nodes_defined );       // # nodes currently defined
ExtraRptCtr( nodes_hiwater );       // hi-water mark for above
ExtraRptCtr( total_frees );         // total # PTreeFreeSubtrees
ExtraRptCtr( null_frees );          // # PTreeFreeSubtrees( NULL )
ExtraRptCtr( simple_frees );        // # PTreeFreeSubtrees( node )

// for use by IsLinkerConstant traversal routine
static PTREE linkerConstantSymbolNode;
static target_size_t linkerConstantOffset;
static boolean linkerConstantFlag;

// enumerate OP_CNV_NO_... as 0, 1, 2, ...
//
#define OPCNV( code, contents ) __PASTE( OP_CNV_NO_, code )
enum
#include "ppopscnv.h"
;
#undef OPCNV

// enumerate OPCNV_... as 0x01, 0x02, 0x03, ...
//
#define OPCNV( code, contents ) \
    __PASTE( OPCNV_, code ) = ( 1 + __PASTE( OP_CNV_NO_, code ) ) \
                              << PTO_CNV_SHIFT
enum
#include "ppopscnv.h"
;
#undef OPCNV

static PTS_FLAG ptreePTSFlags[] =       // one for each PTREE node type
#define PTOP( code, bits ) bits
#include "ptreeop.h"
#undef PTOP
;

static PTO_FLAG oper_flags[] ={ // definitions for unary, binary oper.s
#include "ppopsflg.h"
};

static void ptreeInit(          // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    carvePTREE = CarveCreate( sizeof( struct parse_tree_node ), BLOCK_PTREE );
    ExtraRptRegisterCtr( &nodes_hiwater, "high-water mark: PTREE nodes used" );
    ExtraRptRegisterCtr( &total_frees, "total # of PTreeFreeSubtrees" );
    ExtraRptRegisterCtr( &null_frees, "total # of NULL PTreeFreeSubtrees" );
    ExtraRptRegisterCtr( &simple_frees, "total # of simple PTreeFreeSubtrees" );
}

static void ptreeFini(          // COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    DbgStmt( CarveVerifyAllGone( carvePTREE, "PTREE" ) );
    CarveDestroy( carvePTREE );
}

INITDEFN( parse_tree, ptreeInit, ptreeFini )

PTREE PTreeSetLocn(             // SET LOCATION IN A PTREE NODE
    PTREE tree,                 // - node
    TOKEN_LOCN *locn )          // - location
{
    if( ( locn != NULL ) && ( locn->src_file != NULL ) ) {
        TokenLocnAssign( tree->locn, *locn );
    }
    return tree;
}

PTREE PTreeAlloc( void )
/**********************/
{
    PTREE tree;                 // - allocated entry

    ExtraRptIncrementCtr( nodes_defined );
    ExtraRptHighWater( nodes_defined, nodes_hiwater );
    tree = CarveAlloc( carvePTREE );
    tree->op = PT_NULL;
    tree->cgop = CO_NOP;
    tree->id_cgop = CO_NOP;
    tree->flags = PTF_NULL;
    tree->type = NULL;
    tree->sym_name = NULL;
    tree->locn.src_file = NULL;
    tree->locn.line = 0;
    tree->locn.column = 0;
    tree->decor = NULL;
    return tree;
}

PTREE PTreeAssignReloc( PTREE to, PTREE from, RELOC_LIST *list )
/**************************************************************/
{
    PTREE partner;

    if( to == NULL ) {
        ExtraRptIncrementCtr( nodes_defined );
        ExtraRptHighWater( nodes_defined, nodes_hiwater );
        to = CarveAlloc( carvePTREE );
    }
    *to = *from;
    switch( to->op ) {
    case PT_DUP_EXPR:
        if( to->u.dup.subtree[0] != NULL ) {
            /* disable former duplicated expression */
            from->u.dup.subtree[0] = NULL;
            from->u.dup.node = NULL;
            /* inform partner about the change */
            partner = to->u.dup.node;
            partner->u.dup.node = to;
        }
        break;
    case PT_SYMBOL:
        to->u.symcg.result = ScopeDupResult( from->u.symcg.result );
        break;
    }
    to->decor = PtdDuplicateReloc( from, list );
    return to;
}

PTREE PTreeAssign( PTREE to, PTREE from )
/***************************************/
{
    return PTreeAssignReloc( to, from, NULL );
}

PTREE PTreeFree( PTREE tree )
/***************************/
{
    float_handle fp_val;

    if( tree != NULL ) {
#ifndef NDEBUG
        if( tree->op == PT_DUP_EXPR ) {
            if( tree->u.dup.subtree[0] != NULL ) {
                CFatal( "trying to free a duplicated expr with a dangling reference" );
            }
        }
#endif
        if( tree->decor ) {
            PtdFree( tree );
        }
        if( tree->op == PT_FLOATING_CONSTANT ) {
            fp_val = tree->u.floating_constant;
            tree->u.floating_constant = NULL;
            if( fp_val != NULL ) {
                BFFree( fp_val );
            }
        }
        ExtraRptDecrementCtr( nodes_defined );
        CarveFree( carvePTREE, tree );
    }
    return NULL;
}

void PTreeFreeSubtrees( PTREE tree )
/**********************************/
{
    ExtraRptIncrementCtr( total_frees );
    if( tree == NULL ) {
        ExtraRptIncrementCtr( null_frees );
        return;
    }
    if( ptreePTSFlags[ tree->op ] & PTS_OPERATOR ) {
        PTreeTraversePostfix( tree, PTreeFree );
    } else {
        ExtraRptIncrementCtr( simple_frees );
        PTreeFree( tree );
    }
}


static PTREE makeExpr( ptree_op_t op, CGOP cgop, PTREE sub_1, PTREE sub_2 )
{
    PTREE new_tree;

    new_tree = PTreeAlloc();
    new_tree->op = op;
    new_tree->cgop = cgop;
    new_tree->u.subtree[0] = sub_1;
    new_tree->u.subtree[1] = sub_2;
    return new_tree;
}

PTREE PTreeBinary( CGOP cgop, PTREE left, PTREE right )
/*****************************************************/
{
    PTREE node = makeExpr( PT_BINARY, cgop, left, right );
    if( cgop != CO_COLON_COLON ) {
        node->flags |= PTF_LV_CHECKED;
    }
    return node;
}


PTREE PTreeUnary( CGOP cgop, PTREE expr )
/***************************************/
{
    PTREE node = makeExpr( PT_UNARY, cgop, expr, NULL );
    node->flags |= PTF_LV_CHECKED;
    return node;
}

PTREE PTreeReplaceLeft( PTREE expr, PTREE new_left )
/**************************************************/
{
    expr->u.subtree[0] = new_left;
    return expr;
}

PTREE PTreeReplaceRight( PTREE expr, PTREE new_right )
/****************************************************/
{
    expr->u.subtree[1] = new_right;
    return expr;
}


static PTREE strLiteral         // MAKE A STRING LITERAL NODE
    ( STRING_CONSTANT str       // - the string
    , type_id base )            // - the base type
{
    PTREE new_tree;
    target_size_t str_len;

    str_len = StringAWStrLen( str );
    new_tree = PTreeAlloc();
    new_tree->op = PT_STRING_CONSTANT;
    new_tree->u.string = str;
    new_tree->type = MakeArrayOf( str_len, GetBasicType( base ) );
    new_tree->flags |= PTF_LVALUE | PTF_LV_CHECKED;
    return new_tree;
}

PTREE PTreeLiteral( STRING_CONSTANT str )
/***************************************/
{
    return strLiteral( str, TYP_CHAR );
}

PTREE PTreeLiteralWide( STRING_CONSTANT str )
/*******************************************/
{
    return strLiteral( str, TYP_WCHAR );
}

PTREE PTreeStringLiteralConcat( PTREE left, PTREE right )
/*******************************************************/
{
    STRING_CONSTANT left_str;
    STRING_CONSTANT right_str;
    STRING_CONSTANT new_str;
    TOKEN_LOCN err_locn;
    PTREE new_literal;

    PTreeExtractLocn( right, &err_locn );
    left_str = left->u.string;
    right_str = right->u.string;
    if( left_str->wide_string != right_str->wide_string ) {
        PTreeSetErrLoc( right );
        CErr1( ERR_MISMATCHED_WIDE_STRING_CONCATENATION );
    }
    new_str = StringConcat( left_str, right_str );
    if( ! SrcFileAreTLSameLine( &(left->locn), &(right->locn) ) ) {
        StringConcatDifferentLines( new_str );
    }
    if( new_str->wide_string ) {
        new_literal = strLiteral( new_str, TYP_WCHAR );
    } else {
        new_literal = strLiteral( new_str, TYP_CHAR );
    }
    PTreeSetLocn( new_literal, &err_locn );
    PTreeFree( left );
    PTreeFree( right );
    return new_literal;
}

PTREE PTreeMSSizeofKludge( PTREE type_id )
/****************************************/
{
    TYPE tdef;

    tdef = type_id->type;
    DbgAssert( tdef != NULL );
    PTreeFreeSubtrees( type_id );
    return( PTreeType( tdef ) );
}

PTREE PTreeTListAppend( PTREE start, PTREE new_tree )
/***************************************************/
{
    new_tree->u.type.next = start;
    return new_tree;
}

PTREE ThrowsAnything( void )
/**************************/
{
    PTREE dot_dot_dot;

    dot_dot_dot = PTreeType( GetBasicType( TYP_DOT_DOT_DOT ) );
    return PTreeTListAppend( NULL, dot_dot_dot );
}

PTREE PTreeType( TYPE type )
/**************************/
{
    PTREE new_tree;

    new_tree = PTreeAlloc();
    new_tree->op = PT_TYPE;
    new_tree->type = type;
    new_tree->u.type.next = NULL;
    new_tree->u.type.scope = NULL;
    new_tree->flags |= PTF_LV_CHECKED;
    return new_tree;
}

static PTREE ptreeSetConstantType( PTREE node, type_id id )
{
    node->type = GetBasicType( id );
    node->flags |= PTF_LV_CHECKED;
    return node;
}

static PTREE allocConstant( uint_8 op, type_id id )
{
    PTREE new_tree;

    new_tree = PTreeAlloc();
    new_tree->op = op;
    new_tree = ptreeSetConstantType( new_tree, id );
    return new_tree;
}


PTREE PTreeBoolConstant( int v )
/******************************/
{
    return( PTreeIntConstant( v, TYP_BOOL ) );
}


PTREE PTreeIntConstant( int v, type_id id )
/*****************************************/
{
    PTREE new_tree;

    new_tree = allocConstant( PT_INT_CONSTANT, id );
    Int64From32( new_tree->type, v, &new_tree->u.int64_constant );
    return( new_tree );
}


PTREE PTreeInt64Constant( signed_64 v, type_id id )
/*************************************************/
{
    PTREE new_tree;

    new_tree = allocConstant( PT_INT_CONSTANT, id );
    new_tree->u.int64_constant = v;
    return new_tree;
}

static float_handle makeFPRep( const char *buff )
{
    return BFCnvSF( buff );
}

PTREE PTreeFloatingConstantStr( const char *buff, type_id id )
/************************************************************/
{
    float_handle fp_rep;

    fp_rep = makeFPRep( buff );
    return PTreeFloatingConstant( fp_rep, id );
}

PTREE PTreeFloatingConstant( float_handle rep, type_id id )
/*******************************************************/
{
    PTREE new_tree;

    new_tree = allocConstant( PT_FLOATING_CONSTANT, id );
    new_tree->u.floating_constant = rep;
    new_tree = PTreeCheckFloatRepresentation( new_tree );
    return new_tree;
}

PTREE PTreeId( NAME id )
/**********************/
{
    PTREE new_tree;

    new_tree = PTreeAlloc();
    new_tree->op = PT_ID;
    new_tree->cgop = CO_NAME_NORMAL;
    new_tree->u.id.name = id;
    new_tree->u.id.scope = NULL;
    return new_tree;
}

PTREE PTreeIc(                  // CREATE PT_IC NODE
    CGINTEROP opcode,           // - IC opcode
    CGVALUE value )             // - IC value
{
    PTREE node;                 // - new node

    node = PTreeAlloc();
    node->op = PT_IC;
    node->u.ic.opcode = opcode;
    node->u.ic.value = value;
    node->flags |= PTF_LV_CHECKED;
    return node;
}

PTREE PTreeIdSym( SYMBOL sym )
/****************************/
{
    return PTreeId( sym->name->name );
}

PTREE PTreeThis( void )
/*********************/
{
    PTREE node;                 // - new node

    node = PTreeId( CppSpecialName( SPECIAL_THIS ) );
    node->cgop = CO_NAME_THIS;
    return node;
}

PTREE PTreeCDtorExtra( void )
/***************************/
{
    PTREE node;                 // - new node

    node = PTreeId( CppSpecialName( SPECIAL_CDTOR_EXTRA ) );
    node->cgop = CO_NAME_CDTOR_EXTRA;
    return node;
}


boolean PTreePropogateError(    // CHECK AND PROPOGATE ERRORS FROM SUB-TREES
    PTREE curr )                // - current node
{
    unsigned op;                // - current expr op
    PTREE left;                 // - left subtree
    PTREE right;                // - right subtree

    DbgAssert( curr != NULL );
    op = curr->op;
    if( ptreePTSFlags[ op ] & PTS_OPERATOR ) {
        left = curr->u.subtree[0];
        if( left != NULL && left->op == PT_ERROR ) {
            PTreeErrorNode( curr );
            return TRUE;
        }
    }
    if( ptreePTSFlags[ op ] & PTS_BINARY ) {
        right = curr->u.subtree[1];
        if( right != NULL && right->op == PT_ERROR ) {
            PTreeErrorNode( curr );
            return TRUE;
        }
    }
    return FALSE;
}


PTREE PTreeTraversePostfix(     // TRAVERSE A PTREE IN (LEFT,RIGHT,SELF) ORDER
    PTREE tree,                 // - tree to be traversed
    PTREE (*visit_rtn)          // - visiting routine
        ( PTREE curr ) )        // - - addr( current node )
{
    PTREE parent;
    PTREE temp;
    PTREE left;
    PTREE right;

    /*
        The states represent locations in the traditional recursive version
        of a postfix traversal function.

        postfix( tree )
        {
                                        PTREE_LEFT
            postfix( tree->left )
                                        PTREE_RIGHT
            postfix( tree->right )
                                        PTREE_PROCESS
            process( tree )
                                        PTREE_UNWIND
        }

        note: this routine is heavily used so it has been optimized
        to use gotos since the current codegen can't optimize an
        assignment of a constant to a variable and an immediate
        switch statement to a simple goto.  The switch() style has
        been preserved for less heavily used traversal routines.
    */
    if( tree == NULL || tree->op == PT_ERROR ) {
        return tree;
    }
    parent = NULL;
    PTREE_LEFT:
        if( tree->flags & PTF_DUP_VISITED ) {
            goto PTREE_UNWIND;
        } else if( ptreePTSFlags[ tree->op ] & PTS_OPERATOR ) {
            left = tree->u.subtree[0];
            if( left != NULL ) {
                /* has a left subtree; store current parent */
                temp = tree;
                tree = left;
                temp->u.subtree[0] = parent;
                temp->flags |= PTF_TRAVERSE_LEFT;
                parent = temp;

                /* descend to left child node */
                goto PTREE_LEFT;
            }
            /* descend to right child node */
            goto PTREE_RIGHT;
        } else {
            /* no subtrees; process this node */
            goto PTREE_PROCESS;
        }
    PTREE_RIGHT:
        if( ptreePTSFlags[ tree->op ] & PTS_BINARY ) {
            right = tree->u.subtree[1];
            if( right != NULL ) {
                /* has a right subtree; store current parent */
                temp = tree;
                tree = right;
                temp->u.subtree[1] = parent;
                temp->flags &= ~PTF_TRAVERSE_LEFT;
                parent = temp;

                /* descend to right child node */
                goto PTREE_LEFT;
            } else {
                /* no right subtree; process this node */
                goto PTREE_PROCESS;
            }
        } else {
            /* no right subtree; process this node */
            goto PTREE_PROCESS;
        }
    PTREE_PROCESS:
        /* process node; may replace parent's pointer to this node */
        tree = (*visit_rtn)( tree );
        goto PTREE_UNWIND;
    PTREE_UNWIND:
        if( parent == NULL ) {
            return tree;
        }
        if( parent->flags & PTF_TRAVERSE_LEFT ) {
            parent->flags &= ~PTF_TRAVERSE_LEFT;
            /* set child pointer of parent to current subtree */
            temp = parent;
            parent = temp->u.subtree[0];
            temp->u.subtree[0] = tree;
            tree = temp;

            /* processed a left child; try for right hand child of parent */
            goto PTREE_RIGHT;
        } else {
            /* set child pointer of parent to current subtree */
            temp = parent;
            parent = temp->u.subtree[1];
            temp->u.subtree[1] = tree;
            tree = temp;

            /* processed a right child; process parent */
            goto PTREE_PROCESS;
        }
}

// The following is a cut-paste version of PTreeTraversePostfix, with
// traversals to the left of commas eliminated
//
PTREE PTreeTraverseInitRef(     // TRAVERSE A PTREE FOR REFERENCE INIT.
    PTREE tree,                 // - tree to be traversed
    PTREE (*visit_rtn)          // - visiting routine
        ( PTREE curr ) )        // - - addr( current node )
{
    PTREE parent;
    PTREE temp;
    PTREE left;
    PTREE right;
    PTREE_STATE state;

    /*
        The states represent locations in the traditional recursive version
        of a postfix traversal function.

        postfix( tree )
        {
                                        PTREE_LEFT
            postfix( tree->left )
                                        PTREE_RIGHT
            postfix( tree->right )
                                        PTREE_PROCESS
            process( tree )
                                        PTREE_UNWIND
        }
    */
    if( tree == NULL || tree->op == PT_ERROR ) {
        return tree;
    }
    state = PTREE_LEFT;
    parent = NULL;
    for(;;) {
        switch( state ) {
        case PTREE_LEFT:
            if( ptreePTSFlags[ tree->op ] & PTS_OPERATOR ) {
                left = tree->u.subtree[0];
                if( left != NULL
                 && !( tree->op == PT_BINARY && tree->cgop == CO_COMMA ) ) {
                    /* has a left subtree; store current parent */
                    temp = tree;
                    tree = left;
                    temp->u.subtree[0] = parent;
                    temp->flags |= PTF_TRAVERSE_LEFT;
                    parent = temp;

                    /* descend to left child node */
                    state = PTREE_LEFT;
                } else {
                    /* descend to right child node */
                    state = PTREE_RIGHT;
                }
            } else {
                /* no subtrees; process this node */
                state = PTREE_PROCESS;
            }
            continue;
        case PTREE_RIGHT:
            if( ptreePTSFlags[ tree->op ] & PTS_BINARY ) {
                right = tree->u.subtree[1];
                if( right != NULL ) {
                    /* has a right subtree; store current parent */
                    temp = tree;
                    tree = right;
                    temp->u.subtree[1] = parent;
                    temp->flags &= ~PTF_TRAVERSE_LEFT;
                    parent = temp;

                    /* descend to right child node */
                    state = PTREE_LEFT;
                } else {
                    /* no right subtree; process this node */
                    state = PTREE_PROCESS;
                }
            } else {
                /* no right subtree; process this node */
                state = PTREE_PROCESS;
            }
            continue;
        case PTREE_PROCESS:
            /* process node; may replace parent's pointer to this node */
            tree = (*visit_rtn)( tree );
            state = PTREE_UNWIND;
            /* fall through */
        case PTREE_UNWIND:
            if( parent == NULL ) {
                break;
            }
            if( parent->flags & PTF_TRAVERSE_LEFT ) {
                parent->flags &= ~PTF_TRAVERSE_LEFT;
                /* set child pointer of parent to current subtree */
                temp = parent;
                parent = temp->u.subtree[0];
                temp->u.subtree[0] = tree;
                tree = temp;

                /* processed a left child; try for right hand child of parent */
                state = PTREE_RIGHT;
            } else {
                /* set child pointer of parent to current subtree */
                temp = parent;
                parent = temp->u.subtree[1];
                temp->u.subtree[1] = tree;
                tree = temp;

                /* processed a right child; process parent */
                state = PTREE_PROCESS;
            }
            continue;
        }
        break;
    }
    return tree;
}


PTREE PTreeTraversePrefix(      // TRAVERSE A PTREE IN (SELF,LEFT,RIGHT) ORDER
    PTREE tree,                 // - tree to be traversed
    PTREE (*visit_rtn)          // - visiting routine
        ( PTREE curr ) )        // - - addr( current node )
{
    PTREE parent;
    PTREE temp;
    PTREE_STATE state;

    /*
        The states represent locations in the traditional recursive version
        of a prefix traversal function.

        prefix( tree )
        {
                                        PTREE_PROCESS
            process( tree )
                                        PTREE_LEFT
            postfix( tree->left )
                                        PTREE_RIGHT
            postfix( tree->right )
                                        PTREE_UNWIND
        }
    */
    if( tree == NULL || tree->op == PT_ERROR ) {
        return tree;
    }
    state = PTREE_PROCESS;
    parent = NULL;
    for(;;) {
        switch( state ) {
        case PTREE_PROCESS:
            /* process node */
            tree = (*visit_rtn)( tree );
            state = PTREE_LEFT;
            /* fall through */
        case PTREE_LEFT:
            if( ptreePTSFlags[ tree->op ] & PTS_OPERATOR ) {
                if( tree->u.subtree[0] != NULL ) {
                    /* has a left subtree; store current parent */
                    temp = tree;
                    tree = temp->u.subtree[0];
                    temp->u.subtree[0] = parent;
                    temp->flags |= PTF_TRAVERSE_LEFT;
                    parent = temp;

                    /* descend to left child node */
                    state = PTREE_PROCESS;
                } else {
                    /* descend to right child node */
                    state = PTREE_RIGHT;
                }
            } else {
                /* no subtrees; unwind up to parent */
                state = PTREE_UNWIND;
            }
            continue;
        case PTREE_RIGHT:
            if( ( ptreePTSFlags[ tree->op ] & PTS_BINARY )
              &&( tree->u.subtree[1] != NULL ) ) {
                /* has a right subtree; store current parent */
                temp = tree;
                tree = temp->u.subtree[1];
                temp->u.subtree[1] = parent;
                temp->flags &= ~PTF_TRAVERSE_LEFT;
                parent = temp;

                /* descend to right child node */
                state = PTREE_PROCESS;
            } else {
                /* no right subtree; unwind up to parent */
                state = PTREE_UNWIND;
            }
            continue;
        case PTREE_UNWIND:
            if( parent == NULL ) {
                break;
            }
            if( parent->flags & PTF_TRAVERSE_LEFT ) {
                parent->flags &= ~PTF_TRAVERSE_LEFT;
                /* set child pointer of parent to new node */
                temp = parent;
                parent = temp->u.subtree[0];
                temp->u.subtree[0] = tree;
                tree = temp;

                /* processed a left child; try for right hand child of parent */
                state = PTREE_RIGHT;
            } else {
                /* set child pointer of parent to new node */
                temp = parent;
                parent = temp->u.subtree[1];
                temp->u.subtree[1] = tree;
                tree = temp;

                /* processed a right child; unwind to parent */
                state = PTREE_UNWIND;
            }
            continue;
        }
        break;
    }
    return tree;
}

static TOKEN_LOCN *extract_locn;

static PTREE set_error_location( PTREE node )
{
    if( node->locn.src_file != NULL ) {
        extract_locn = &(node->locn);
    }
    return node;
}

PTREE PTreeExtractLocn( PTREE expr, TOKEN_LOCN *locn )
/***************************************************/
{
    if( expr != NULL && expr->locn.src_file != NULL ) {
        *locn = expr->locn;
        return expr;
    }
    // traverse tree looking for location information
    extract_locn = NULL;
    expr = PTreeTraversePostfix( expr, &set_error_location );
    if( extract_locn == NULL ) {
        memset( locn, 0, sizeof( *locn ) );
    } else {
        *locn = *extract_locn;
    }
    return expr;
}

PTREE PTreeSetErrLoc(           // SET THE ERROR LOCATION FOR A TREE
    PTREE tree )                // - tree with error
{
    TOKEN_LOCN err_locn;

    if( tree != NULL ) {
        PTreeExtractLocn( tree, &err_locn );
        SetErrLoc( &err_locn );
    }
    return tree;
}

PTREE PTreeErrorNode(           // MAKE NODE AN ERROR NODE
    PTREE curr )                // - node in question
{
    PTS_FLAG flags;

    if( curr == NULL ) {
        curr = PTreeAlloc();
    } else if( curr->op == PT_DUP_EXPR ) {
        NodeUnduplicate( curr );
    } else {
        flags = ptreePTSFlags[ curr->op ];
        if( flags & PTS_OPERATOR ) {
            if( flags & PTS_BINARY ) {
                NodeFreeDupedExpr( curr->u.subtree[1] );
            }
            NodeFreeDupedExpr( curr->u.subtree[0] );
            curr->u.subtree[0] = NULL;
            curr->u.subtree[1] = NULL;
        }
    }
    curr->op = PT_ERROR;
    curr->cgop = CO_NOP;
    return curr;
}


msg_status_t PTreeErrorExpr(    // ISSUE ERROR MESSAGE FOR PTREE NODE
    PTREE expr,                 // - node for error
    unsigned err_code )         // - error code
{
    msg_status_t status;
    PTreeSetErrLoc( expr );
    status = CErr1( err_code );
    if( status & MS_PRINTED )       // don't issue a note if warning was suppressed
        InfClassDecl( expr->type ); // issue the symbol name where we hit this error. (useful for template expansion errors)
    if(( status & MS_WARNING ) == 0 ) {
        PTreeErrorNode( expr );
    }
    return status;
}


void PTreeErrorExprName(        // ISSUE ERROR MESSAGE FOR PTREE NODE, NAME
    PTREE expr,                 // - node for error
    unsigned err_code,          // - error code
    NAME name )                 // - name
{
    PTreeSetErrLoc( expr );
    if(( CErr2p( err_code, name ) & MS_WARNING ) == 0 ) {
        PTreeErrorNode( expr );
    }
}


void PTreeErrorExprSym(         // ISSUE ERROR MESSAGE FOR PTREE NODE, SYMBOL
    PTREE expr,                 // - node for error
    unsigned err_code,          // - error code
    SYMBOL sym )                // - symbol
{
    PTreeSetErrLoc( expr );
    if(( CErr2p( err_code, sym ) & MS_WARNING ) == 0 ) {
        PTreeErrorNode( expr );
    }
}


void PTreeErrorExprSymInf(      // ISSUE ERROR MESSAGE FOR PTREE NODE, SYMBOL
    PTREE expr,                 // - node for error
    unsigned err_code,          // - error code
    SYMBOL sym )                // - symbol
{
    msg_status_t retn;          // - message status
    PTreeSetErrLoc( expr );
    retn = CErr1( err_code );
    if( ( retn & MS_WARNING ) == 0 ) {
        PTreeErrorNode( expr );
    }
    if( ( retn & MS_PRINTED ) != 0 ) {
        InfSymbolDeclaration( sym );
    }
}

void PTreeErrorExprType(        // ISSUE ERROR MESSAGE FOR PTREE NODE, TYPE
    PTREE expr,                 // - node for error
    unsigned err_code,          // - error code
    TYPE type )                 // - type
{
    PTreeSetErrLoc( expr );
    if(( CErr2p( err_code, type ) & MS_WARNING ) == 0 ) {
        PTreeErrorNode( expr );
    }
}

void PTreeErrorExprNameType(    // ISSUE ERROR MESSAGE FOR PTREE NODE, NAME, TYPE
    PTREE expr,                 // - node for error
    unsigned err_code,          // - error code
    NAME name,                  // - name
    TYPE type )                 // - type
{
    PTreeSetErrLoc( expr );
    if(( CErr( err_code, name, type ) & MS_WARNING ) == 0 ) {
        PTreeErrorNode( expr );
    }
}


msg_status_t PTreeWarnExpr(     // ISSUE WARNING MESSAGE FOR PTREE NODE
    PTREE expr,                 // - node for error
    unsigned err_code )         // - error code
{
    PTreeSetErrLoc( expr );
    return CErr1( err_code );
}


PTO_FLAG PTreeOpFlags(          // GET FLAGS FOR A PTREE NODE
    PTREE curr )                // - current node
{
    PTO_FLAG flags;             // - flags for the node

    flags = PTO_NULL;
    if( ptreePTSFlags[ curr->op ] & PTS_HAS_CGOP ) {
        flags |= oper_flags[ curr->cgop ];
    }
    return flags;
}

PTREE PTreeNonZeroConstantExpr( PTREE expr )
/******************************************/
{
    if( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_DECL ) ) {
        PTreeFreeSubtrees( expr );
        expr = PTreeIntConstant( 1, TYP_SINT );
    } else {
        expr = PTreeForceIntegral( expr );
        if( expr == NULL ) {
            /* set to 1 if expression was not constant */
            expr = PTreeIntConstant( 1, TYP_SINT );
        }
    }
    return expr;
}


PTREE PTreeForceIntegral( PTREE cexpr )
/*************************************/
{
    cexpr = AnalyseValueExpr( cexpr );
    switch( cexpr->op ) {
    case PT_INT_CONSTANT:
        return cexpr;
    case PT_FLOATING_CONSTANT: {
        int sign;
        target_long result;

        CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
        sign = BFSign( cexpr->u.floating_constant );
        result = BFGetLong( &(cexpr->u.floating_constant) );
        cexpr->op = PT_INT_CONSTANT;
        cexpr->u.int_constant = result;
        cexpr = ptreeSetConstantType( cexpr
                                    , sign < 0 ? TYP_SINT : TYP_UINT );
        return cexpr;
    }
    }
    if( cexpr->op != PT_ERROR ) {
        PTreeErrorExpr( cexpr, ERR_NOT_A_CONSTANT_EXPR );
    }
    PTreeFreeSubtrees( cexpr );
    return NULL;
}

static TYPE mustBeLexicalTypeName( SCOPE scope, NAME name )
{
    SEARCH_RESULT *result;
    TYPE dtor_type;
    SYMBOL sym;
    SYMBOL_NAME sym_name;

    result = ScopeFindNaked( scope, name );
    if( result == NULL ) {
        CErr2p( ERR_INVALID_DESTRUCTOR_NAME, name );
        return TypeError;
    }
    sym_name = result->sym_name;
    sym = sym_name->name_type;
    if( sym == NULL ) {
        ScopeFreeResult( result );
        result = ScopeFindLexicalClassType( scope, name );
        if( result == NULL ) {
            CErr2p( ERR_INVALID_DESTRUCTOR_NAME, name );
            return TypeError;
        }
        sym_name = result->sym_name;
        sym = sym_name->name_type;
    }
    dtor_type = sym->sym_type;
    ScopeFreeResult( result );
    return dtor_type;
}

PTREE SimpleDestructorId( TYPE type )
/***********************************/
{
    PTREE id;

    id = PTreeId( CppDestructorName() );
    id->cgop = CO_NAME_DTOR;
    id->type = type;
    return id;
}

static PTREE makeDestructorId( SCOPE scope, PTREE id, TYPE class_type )
{
    NAME name_of_class;

    name_of_class = id->u.id.name;
    PTreeFree( id );
    if( class_type == NULL ) {
        class_type = mustBeLexicalTypeName( scope, name_of_class );
    }
    return SimpleDestructorId( class_type );
}

PTREE MakeDestructorId( PTREE id )
/********************************/
{
    return makeDestructorId( GetCurrScope(), id, NULL );
}

PTREE MakeDestructorIdFromType( DECL_SPEC *dspec )
/************************************************/
{
    PTREE id;

    id = MakeIdFromType( dspec );
    return makeDestructorId( GetCurrScope(), id, NULL );
}

PTREE MakeOperatorId( CGOP op )
/*****************************/
{
    PTREE op_tree;

    op_tree = PTreeId( CppOperatorName( op ) );
    op_tree->cgop = CO_NAME_OPERATOR;
    op_tree->id_cgop = op;
    return op_tree;
}

PTREE MakeUserConversionId( DECL_SPEC *dspec, DECL_INFO *dinfo )
/**************************************************************/
{
    TYPE conversion_type;
    PTREE id_tree;

    conversion_type = TypeUserConversion( dspec, dinfo );
    id_tree = PTreeId( CppConversionName() );
    id_tree->cgop = CO_NAME_CONVERT;
    id_tree->type = conversion_type;
    return id_tree;
}

static PTREE makeTypeNode( DECL_SPEC *dspec )
{
    PTREE type_tree;

    type_tree = DoDeclSpec( dspec );
    PTypeRelease( dspec );
    return type_tree;
}

PTREE MakeFunctionLikeCast( DECL_SPEC *dspec, PTREE expr_list )
/*************************************************************/
{
    PTREE ctor_type;
    PTREE expr;

    ctor_type = makeTypeNode( dspec );
    if( expr_list != NULL && expr_list->u.subtree[0] == NULL ) {
        // 5.2.3 type( expr ) = (type)expr
        expr = expr_list->u.subtree[1];
        PTreeFree( expr_list );
        return MakeNormalCast( ctor_type, expr );
    }
    return PTreeBinary( CO_CTOR, ctor_type, expr_list );
}

PTREE MakeNormalCast( PTREE type_node, PTREE expr_node )
/******************************************************/
{
    /* mark as being specified by the programmer */
    type_node->cgop = CO_USER_CAST;
    return PTreeBinary( CO_CONVERT, type_node, expr_node );
}

PTREE MakeMemInitItem( DECL_SPEC *dspec, PTREE id, PTREE expr_list, TOKEN_LOCN *locn )
/************************************************************************************/
{
    PTREE tree;

    if( id == NULL ) {
        id = makeTypeNode( dspec );
    }
    tree = PTreeBinary( CO_CTOR, id, expr_list );
    tree = PTreeSetLocn( tree, locn );
    return tree;
}

PTREE MakeIdFromType( DECL_SPEC *dspec )
/**************************************/
{
    PTREE id_tree;

    id_tree = dspec->id;
    dspec->id = NULL;
    PTypeRelease( dspec );
    return CheckScopedId( id_tree );
}

PTREE MakeDeleteExpr( PTREE gbl, CGOP op, PTREE expr )
/*******************************************************/
{
    PTreeFreeSubtrees( gbl );
    return PTreeUnary( op, expr );
}

PTREE MakeGlobalId( PTREE global_id )
/***********************************/
{
    PTREE old_id;
    PTREE new_id;

    old_id = global_id->u.subtree[1];
    new_id = PTreeId( old_id->u.id.name );
    new_id = PTreeCopySrcLocation( new_id, global_id->u.subtree[1] );
    new_id = PTreeBinary( CO_COLON_COLON, NULL, new_id );
    new_id = PTreeCopySrcLocation( new_id, global_id->u.subtree[0] );
    PTreeFreeSubtrees( global_id );
    return new_id;
}

static PTREE keepRightId( PTREE scoped_id )
{
    PTREE id;

    /* detach id and free the rest */
    id = scoped_id->u.subtree[1];
    scoped_id->u.subtree[1] = NULL;
    PTreeFreeSubtrees( scoped_id );
    return id;
}

PTREE CutAwayQualification( PTREE scoped_id )
/*******************************************/
{
    return keepRightId( scoped_id );
}

PTREE MakeGlobalOperatorId( PTREE global_operator, CGOP op )
/**********************************************************/
{
    PTREE id;

    id = MakeOperatorId( op );
    id = PTreeCopySrcLocation( id, global_operator );
    id = PTreeBinary( CO_COLON_COLON, NULL, id );
    id = PTreeCopySrcLocation( id, global_operator->u.subtree[0] );
    PTreeFreeSubtrees( global_operator );
    return id;
}

static PTREE useScopeIfPossible( PTREE scoped_thing, PTREE id )
{
    TYPE class_type;
    PTREE colon_colon_tree;
    PTREE old_scope_tree;
    PTREE new_scope_tree;
    SCOPE scope;

    colon_colon_tree = scoped_thing->u.subtree[0];
    old_scope_tree = colon_colon_tree->u.subtree[1];
    if( old_scope_tree != NULL ) {
        scope = old_scope_tree->u.id.scope;
        if( scope != NULL ) {
            class_type = StructType( old_scope_tree->type );
            new_scope_tree = PTreeType( class_type );
            new_scope_tree->u.type.scope = scope;
            new_scope_tree = PTreeCopySrcLocation( new_scope_tree, old_scope_tree );
            id = PTreeBinary( CO_COLON_COLON, new_scope_tree, id );
            id = PTreeCopySrcLocation( id, colon_colon_tree );
        }
    }
    return id;
}

PTREE CheckScopedId( PTREE id )
/*****************************/
{
    if( id->op != PT_BINARY ) {
        return id;
    }
    return MakeScopedId( id );
}

PTREE MakeScopedId( PTREE scoped_id )
/***********************************/
{
    PTREE old_id;
    PTREE new_id;

    old_id = scoped_id->u.subtree[1];
    scoped_id->u.subtree[1] = NULL;
    new_id = useScopeIfPossible( scoped_id, old_id );
    PTreeFreeSubtrees( scoped_id );
    return new_id;
}

PTREE MakeScopedDestructorId( PTREE scoped_tilde, PTREE id )
/**********************************************************/
{
    TYPE class_type;
    TYPE unbound_type;
    PTREE tree;

    if( ( id->op == PT_ID ) && ( id->cgop == CO_NAME_NORMAL )
     && ( id->type != NULL ) && ArithType( id->type ) ) {
        id = makeDestructorId( NULL, id, id->type );
        id = PTreeCopySrcLocation( id, scoped_tilde );
        if( ! TypesIdentical( scoped_tilde->u.subtree[0]->u.subtree[1]->type,
                              id->type ) ) {
            CErr1( ERR_INVALID_SCALAR_DESTRUCTOR );
            id = PTreeErrorNode( id );
        }
    } else {
        tree = useScopeIfPossible( scoped_tilde, NULL );
        if( tree != NULL ) {
            class_type = StructType( tree->u.subtree[0]->type );
            unbound_type = NULL;
            if( class_type->flag & TF1_UNBOUND ) {
                unbound_type = class_type;
            }
            id = makeDestructorId( class_type->u.c.scope, id, unbound_type );
            id = PTreeCopySrcLocation( id, scoped_tilde );
            id = PTreeReplaceRight( tree, id );
        }
    }
    PTreeFreeSubtrees( scoped_tilde );
    return id;
}

PTREE MakeScopedUserConversionId( PTREE scoped_operator, PTREE cnv_id )
/*********************************************************************/
{
    PTREE tree;

    cnv_id = PTreeCopySrcLocation( cnv_id, scoped_operator );
    tree = useScopeIfPossible( scoped_operator, cnv_id );
    PTreeFreeSubtrees( scoped_operator );
    return tree;
}

PTREE MakeScopedOperatorId( PTREE scoped_operator, CGOP op )
/**********************************************************/
{
    PTREE id;

    id = MakeOperatorId( op );
    id = PTreeCopySrcLocation( id, scoped_operator );
    id = useScopeIfPossible( scoped_operator, id );
    PTreeFreeSubtrees( scoped_operator );
    return id;
}

PTF_FLAG PTreeEffFlags(     // GET MEANINGFUL FLAGS FOR A PTREE OPERAND
    PTREE node )            // - node
{
    PTF_FLAG flags;         // - flags

    if( node == NULL ) {
        flags = PTF_NULL;
    } else {
        flags = node->flags & ( PTF_SIDE_EFF | PTF_MEANINGFUL );
    }
    return flags;
}


PTREE *PTreeRef(                // FIND REFERENCE TO OPERAND (PAST COMMAS)
    PTREE *tgt )                // - target location
{
    PTREE node;                 // - target node

    for( ; ; ) {
        node = *tgt;
        if( node == NULL ) break;
        switch( node->op ) {
          case PT_BINARY :
            if( node->cgop == CO_COMMA ) {
                tgt = &((*tgt)->u.subtree[1]);
                continue;
            }
            break;
          default :
            break;
        }
        break;
    }
    return tgt;
}


PTREE *PTreeRefLeft(            // FIND REFERENCE TO LEFT OPERAND
    PTREE expr )                // - expression
{
    return PTreeRef( &expr->u.subtree[0] );
}


PTREE *PTreeRefRight(           // FIND REFERENCE TO RIGHT OPERAND
    PTREE expr )                // - expression
{
    return PTreeRef( &expr->u.subtree[1] );
}


PTREE PTreeOp(                  // FETCH (OVER COMMAS, DUP'S) PTREE PTR.
    PTREE *addr_expr )          // - addr( PTREE node )
{
    PTREE expr;                 // - next operand expression

    for( ; ; ) {
        expr = *PTreeRef( addr_expr );
        if( expr == NULL ) break;
        if( expr->op != PT_DUP_EXPR ) break;
        addr_expr = &expr->u.dup.subtree[0];
    }
    return expr;
}


PTREE PTreeOpLeft(              // GET LEFT NODE, SKIPPING "," OPERATOR, DUPS
    PTREE expr )                // - expression
{
    return PTreeOp( &expr->u.subtree[0] );
}


PTREE PTreeOpRight(             // GET RIGHT NODE, SKIPPING "," OPERATOR, DUPS
    PTREE expr )                // - expression
{
    return PTreeOp( &expr->u.subtree[1] );
}

static boolean symAccessNeedsCode( SYMBOL sym )
{
    if( SymIsAutomatic( sym ) ) {
        return TRUE;
    }
    if( SymIsDllImport( sym ) ) {
        return TRUE;
    }
    return FALSE;
}

static PTREE linker_constant_tree_node( PTREE expr )
/**************************************************/
{
    PTREE       other_expr;
    SYMBOL      sym;

    if( !linkerConstantFlag ) {
        return expr;
    }

    switch( expr->op ) {
    case PT_INT_CONSTANT:
        if( linkerConstantSymbolNode == NULL ) {
            linkerConstantSymbolNode = expr;
        }
        break;
    case PT_TYPE:
        break;
    case PT_STRING_CONSTANT:
    case PT_FLOATING_CONSTANT:
        if( linkerConstantSymbolNode != NULL ) {
            linkerConstantFlag = FALSE;
        } else {
            linkerConstantSymbolNode = expr;
        }
        break;
    case PT_SYMBOL:
        if( ( expr->cgop != CO_NAME_NORMAL )
          ||( linkerConstantSymbolNode != NULL ) ) {
            linkerConstantFlag = FALSE;
        } else {
            sym = expr->u.symcg.symbol;
            if( sym != NULL && symAccessNeedsCode( sym ) ) {
                linkerConstantFlag = FALSE;
            } else {
                linkerConstantSymbolNode = expr;
            }
        }
        break;
    case PT_UNARY:
        switch( expr->cgop ) {
        case CO_ADDR_OF:
            other_expr = linkerConstantSymbolNode;
            if( other_expr != NULL
             && other_expr->op == PT_SYMBOL
             && other_expr->cgop == CO_NAME_NORMAL ) {
                if( symAccessNeedsCode( other_expr->u.symcg.symbol ) ) {
                    linkerConstantFlag = FALSE;
                }
            }
            break;
        case CO_INDIRECT:
            break;
        case CO_BITFLD_CONVERT:
            if( linkerConstantSymbolNode != NULL ) {
                linkerConstantFlag = FALSE;
            }
            break;
        default:
            linkerConstantFlag = FALSE;
            break;
        }
        break;
    case PT_BINARY:
        switch( expr->cgop ) {
        case CO_LIST:
        case CO_CONVERT:
            break;
        case CO_MINUS:
            other_expr = expr->u.subtree[1];
            switch( other_expr->op ) {
            case PT_INT_CONSTANT:
                linkerConstantOffset -= other_expr->u.uint_constant;
                break;
            default:
                linkerConstantFlag = FALSE;
                break;
            }
            break;
        case CO_PLUS:
        case CO_DOT:
            other_expr = expr->u.subtree[1];
            switch( other_expr->op ) {
            case PT_INT_CONSTANT:
                linkerConstantOffset += other_expr->u.uint_constant;
                break;
            default:
                linkerConstantFlag = FALSE;
                break;
            }
            break;
        default:
            linkerConstantFlag = FALSE;
            break;
        }
        break;
    default:
        linkerConstantFlag = FALSE;
        break;
    }
    return expr;
}

boolean IsLinkerConstant( PTREE tree, PTREE *ctree, target_size_t *offset )
/*************************************************************************/
// return TRUE when it is a constant
{
    // handle "int a = ( b=1, 2 );"
    if( tree->flags & PTF_SIDE_EFF ) {
        return FALSE;
    }

    if( NodeIsUnaryOp( tree, CO_MEMPTR_CONST ) ) {
        *ctree = tree;
        *offset = 0;
        return TRUE;
    }

    // traverse tree making sure that all operations and components
    // are allowable for constant expressions
    // update linkerConstantSymbolNode linkerConstantOffset
    // and linkerConstantFlag as we go
    linkerConstantOffset = 0;
    linkerConstantFlag = TRUE;
    linkerConstantSymbolNode = NULL;
    PTreeTraversePostfix( tree, &linker_constant_tree_node );

    if( !linkerConstantFlag || ( linkerConstantSymbolNode == NULL ) ) {
        return FALSE;
    } else {
        *ctree = linkerConstantSymbolNode;
        *offset = linkerConstantOffset;
        return TRUE;
    }
}

boolean IsStringConstant( PTREE node, boolean *multi_line_concat )
/****************************************************************/
{
    STRING_CONSTANT str;
    boolean retn;

    *multi_line_concat = FALSE;
    retn = FALSE;
    if( node->op == PT_STRING_CONSTANT ) {
        str = node->u.string;
        if( str->concat && str->multi_line ) {
            *multi_line_concat = TRUE;
        }
        retn = TRUE;
    }
    return retn;
}

SYMBOL FunctionSymbol( PTREE node )
/*********************************/
{
    SYMBOL  sym;

    if( node == NULL ) {
        return NULL;
    }
    if( node->op == PT_UNARY && node->cgop == CO_ADDR_OF ) {
        node = PTreeOpLeft( node );
    }
    if( node->op == PT_SYMBOL && node->cgop == CO_NAME_NORMAL ) {
        sym = node->u.symcg.symbol;
        if( !SymIsFunction( sym ) ) {
            sym = NULL;
        }
    } else {
        sym = NULL;
    }
    return sym;
}

PTREE PTreeDupExpr(             // MAKE DUPLICATE-EXPRESSION NODE
    PTREE expr )                // - expression to be duplicated
{
    PTREE dup;                  // - duplication node

    dup = PTreeAlloc();
    dup->op = PT_DUP_EXPR;
    dup->u.dup.subtree[0] = expr;
    dup->u.dup.node = NULL;
    dup->type = expr->type;
    dup->sym_name = expr->sym_name;
    dup->flags = expr->flags;
    return PTreeCopySrcLocation( dup, expr );
}

PTREE PTreeCopySrcLocation(     // COPY LOCATION OF SOURCE
    PTREE tgt,                  // - target node
    PTREE src )                 // - source node
{
    if(( tgt != NULL )&&( src != NULL )&&( src->locn.src_file != NULL )) {
        TokenLocnAssign( tgt->locn, src->locn );
    }
    return tgt;
}


PTREE PTreePromoteLocn(         // ENSURE TOP OF TREE HAS SOURCE LOCATION
    PTREE tree )                // - the tree
{
    PTreeExtractLocn( tree, &tree->locn );
    return tree;
}

PTREE PTreeIntrinsicOperator( PTREE expr, CGOP cgop )
/***************************************************/
{
    PTO_FLAG flags;
    PTREE arg1;
    PTREE op1;
    PTREE arg2;
    PTREE op2;
    PTREE old_expr;
    TYPE expr_type;

    old_expr = expr;
    expr_type = expr->type;
    arg1 = expr->u.subtree[1];
    op1 = arg1->u.subtree[1];
    arg1->u.subtree[1] = NULL;
    flags = oper_flags[ cgop ];
    if( flags & PTO_UNARY ) {
        expr = NodeUnary( cgop, op1 );
    } else {
        arg2 = arg1->u.subtree[0];
        op2 = arg2->u.subtree[1];
        arg2->u.subtree[1] = NULL;
        expr = NodeBinary( cgop, op1, op2 );
    }
    expr->type = expr_type;
    PTreeFreeSubtrees( old_expr );
    return expr;
}

PTREE MakeBuiltinIsFloat( PTREE expr )
/************************************/
{
    TYPE type;

    type = FloatingType( expr->type );
    PTreeFree( expr );
    return( PTreeIntConstant( ( type != NULL ) ? 1 : 0, TYP_SINT ) );
}

void PTreeDeleteSizeExpr( PTREE expr )
/************************************/
{
    PTreeSetErrLoc( expr );
    CErr1( ANSI_NO_DELETE_SIZE_EXPR );
    PTreeFreeSubtrees( expr );
}

PTREE PTreeOffsetof( PTREE type, PTREE name )
/*******************************************/
{
    PTREE tree;

    tree = PTreeBinary( CO_OFFSETOF, type, NULL );
    tree = PtdOffsetofExpr( tree, name );
    return tree;
}

PTREE PTreeCheckFloatRepresentation( PTREE tree )
/***********************************************/
{
    type_id id;

    id = TypedefModifierRemove( tree->type )->id;
    switch( id ){
    case TYP_FLOAT:
        tree->u.floating_constant = BFCheckFloatLimit( tree->u.floating_constant );
        break;
    case TYP_DOUBLE:
    case TYP_LONG_DOUBLE:
        tree->u.floating_constant = BFCheckDblLimit( tree->u.floating_constant );
        break;
    }
    return tree;
}

unsigned PTreeGetFPRaw( PTREE tree, char *buff, unsigned len )
/************************************************************/
{
    float_handle cg_float;
    char *end;

    DbgAssert( tree != NULL && tree->op == PT_FLOATING_CONSTANT );
    DbgAssert( len >= 128 );
    cg_float = tree->u.floating_constant;
    end = BFCnvFS( cg_float, buff, len );
    len = end - buff;
    DbgAssert( buff[len] == '\0' );
    // returns strlen( buff ) + 1
    return len + 1;
}


PTREE PTreeGetIndex( PTREE e )
{
    return CarveGetIndex( carvePTREE, e );
}

PTREE PTreeMapIndex( PTREE e )
{
    return CarveMapIndex( carvePTREE, e );
}

static void markFreePTree( void *p )
{
    PTREE s = p;

    s->op = PT_FREE;
}

static void savePTree( void *p, carve_walk_base *d )
{
    PTREE s = p;
    TYPE save_type;
    PTD *save_decor;
    SRCFILE save_locn;
    SCOPE save_scope;
    PTREE save_next;
    NAME save_name;
    STRING_LITERAL *save_string;
    SYMBOL save_symbol;
    CGVALUE save_value;
    float_handle save_float;
    unsigned fp_len;
    PTREE save_subtree[2];
    auto char buff[128];

    if( s->op == PT_FREE ) {
        return;
    }
    save_type = s->type;
    s->type = TypeGetIndex( save_type );
    save_decor = s->decor;
    s->decor = PtdGetIndex( save_decor );
    save_locn = s->locn.src_file;
    s->locn.src_file = SrcFileGetIndex( save_locn );
    switch( s->op ) {
    case PT_NULL:
    case PT_INT_CONSTANT:
        // nothing to do
        break;
    case PT_STRING_CONSTANT:
        save_string = s->u.string;
        s->u.string = StringGetIndex( save_string );
        break;
    case PT_FLOATING_CONSTANT:
        save_float = s->u.floating_constant;
        fp_len = PTreeGetFPRaw( s, buff, sizeof( buff ) );
        s->u.floating_constant = PCHSetUInt( fp_len );
        break;
    case PT_SYMBOL:
        save_symbol = s->u.symcg.symbol;
        s->u.symcg.symbol = SymbolGetIndex( save_symbol );
        DbgAssert( s->u.symcg.result == NULL );
        break;
    case PT_DUP_EXPR:
        save_subtree[0] = s->u.dup.subtree[0];
        save_subtree[1] = s->u.dup.node;
        s->u.dup.subtree[0] = PTreeGetIndex( save_subtree[0] );
        s->u.dup.node = PTreeGetIndex( save_subtree[1] );
        break;
    case PT_IC:
        save_value = s->u.ic.value;
        s->u.ic.value = CgioGetIndex( s->u.ic.opcode, save_value );
        break;
    case PT_TYPE:
        save_next = s->u.type.next;
        save_scope = s->u.type.scope;
        s->u.type.next = PTreeGetIndex( save_next );
        s->u.type.scope = ScopeGetIndex( save_scope );
        break;
    case PT_ID:
        save_name = s->u.id.name;
        save_scope = s->u.id.scope;
        s->u.id.name = NameGetIndex( save_name );
        s->u.id.scope = ScopeGetIndex( save_scope );
        break;
    case PT_UNARY:
    case PT_BINARY:
        save_subtree[0] = s->u.subtree[0];
        save_subtree[1] = s->u.subtree[1];
        s->u.subtree[0] = PTreeGetIndex( save_subtree[0] );
        s->u.subtree[1] = PTreeGetIndex( save_subtree[1] );
        break;
    DbgDefault( "unknown PTREE op" );
    }
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    s->type = save_type;
    s->decor = save_decor;
    s->locn.src_file = save_locn;
    switch( s->op ) {
    case PT_NULL:
    case PT_INT_CONSTANT:
        // nothing to do
        break;
    case PT_STRING_CONSTANT:
        s->u.string = save_string;
        break;
    case PT_FLOATING_CONSTANT:
        s->u.floating_constant = save_float;
        PCHWrite( buff, fp_len );
        break;
    case PT_SYMBOL:
        s->u.symcg.symbol = save_symbol;
        break;
    case PT_DUP_EXPR:
        s->u.dup.subtree[0] = save_subtree[0];
        s->u.dup.node = save_subtree[1];
        break;
    case PT_IC:
        s->u.ic.value = save_value;
        break;
    case PT_TYPE:
        s->u.type.next = save_next;
        s->u.type.scope = save_scope;
        break;
    case PT_ID:
        s->u.id.name = save_name;
        s->u.id.scope = save_scope;
        break;
    case PT_UNARY:
    case PT_BINARY:
        s->u.subtree[0] = save_subtree[0];
        s->u.subtree[1] = save_subtree[1];
        break;
    DbgDefault( "unknown PTREE op" );
    }
}

pch_status PCHWritePTrees( void )
{
    auto carve_walk_base data;

    CarveWalkAllFree( carvePTREE, markFreePTree );
    CarveWalkAll( carvePTREE, savePTree, &data );
    PCHWriteCVIndexTerm();
    DefArgPCHWrite();
    return PCHCB_OK;
}

pch_status PCHReadPTrees( void )
{
    PTREE r;
    unsigned len;
    auto cvinit_t data;
    auto char buff[128];

    CarveInitStart( carvePTREE, &data );
    for( ; (r = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *r );
        r->type = TypeMapIndex( r->type );
        r->decor = PtdMapIndex( r->decor );
        r->locn.src_file = SrcFileMapIndex( r->locn.src_file );
        switch( r->op ) {
        case PT_NULL:
            break;
        case PT_STRING_CONSTANT:
            r->u.string = StringMapIndex( r->u.string );
            break;
        case PT_INT_CONSTANT:
            break;
        case PT_FLOATING_CONSTANT:
            len = PCHGetUInt( r->u.floating_constant );
            PCHRead( buff, len );
            DbgAssert( len > 1 );
            r->u.floating_constant = makeFPRep( buff );
            break;
        case PT_ID:
            r->u.id.name = NameMapIndex( r->u.id.name );
            r->u.id.scope = ScopeMapIndex( r->u.id.scope );
            break;
        case PT_TYPE:
            r->u.type.next = PTreeMapIndex( r->u.type.next );
            r->u.type.scope = ScopeMapIndex( r->u.type.scope );
            break;
        case PT_SYMBOL:
            r->u.symcg.symbol = SymbolMapIndex( r->u.symcg.symbol );
            r->u.symcg.result = NULL;
            break;
        case PT_DUP_EXPR:
            r->u.dup.subtree[0] = PTreeMapIndex( r->u.dup.subtree[0] );
            r->u.dup.node = PTreeMapIndex( r->u.dup.node );
            break;
        case PT_IC:
            r->u.ic.value = CgioMapIndex( r->u.ic.opcode, r->u.ic.value );
            break;
        case PT_UNARY:
        case PT_BINARY:
            r->u.subtree[0] = PTreeMapIndex( r->u.subtree[0] );
            r->u.subtree[1] = PTreeMapIndex( r->u.subtree[1] );
            break;
        DbgDefault( "unknown PTREE op" );
        }
    }
    DefArgPCHRead();
    return PCHCB_OK;
}

pch_status PCHInitPTrees( boolean writing )
{
    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carvePTREE ) );
    } else {
        carvePTREE = CarveRestart( carvePTREE );
        CarveMapOptimize( carvePTREE, PCHReadCVIndex() );
    }
    return PCHCB_OK;
}

pch_status PCHFiniPTrees( boolean writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carvePTREE );
    }
    return PCHCB_OK;
}

PTREE PTreeCopyPrefix(         // COPY A PTREE IN (SELF,LEFT,RIGHT) ORDER
/********************/
    PTREE tree,                // - tree to be copied
    PTREE (*copy_rtn)          // - copy routine
        ( PTREE curr,          // - - addr( current node )
          void *param ),       // - - parameter
    void *param  )             // param to pass to copy_rtn
{
    PTREE parent;               // parent of tree
    PTREE temp;                 // used to traverse tree
    PTREE_STATE state;          // current state
    PTREE parent_copy;          // parent of curr_copy
    PTREE grandparent_copy;     // grandparent of curr_copy node
    PTREE temp_copy;            // used in copying
    PTREE *curr_copy;           // current copy node
    PTREE the_copy;             // head of copy tree

    /*
        The states represent locations in the traditional recursive version
        of a prefix traversal function.

        prefix( tree )
        {
                                        PTREE_PROCESS
            process( tree )
                                        PTREE_LEFT
            postfix( tree->left )
                                        PTREE_RIGHT
            postfix( tree->right )
                                        PTREE_UNWIND
        }
    */
    if( tree == NULL || tree->op == PT_ERROR ) {
        return tree;
    }
    state = PTREE_PROCESS;
    parent = NULL;
    parent_copy = NULL;
    grandparent_copy = NULL;
    curr_copy = &the_copy;
    for(;;) {
        switch( state ) {
        case PTREE_PROCESS:
            /* process node */
            *curr_copy = (*copy_rtn)( tree, param );
            state = PTREE_LEFT;
            /* fall through */
        case PTREE_LEFT:
            if( ( ptreePTSFlags[ tree->op ] & PTS_OPERATOR ) &&
                ( tree->op != PT_DUP_EXPR ||
                  ( ((*curr_copy)->u.dup.subtree[0]->flags & PTF_DEFARG_COPY)
                      == 0) ) ) {
                if( tree->u.subtree[0] != NULL ) {
                    /* has a left subtree; store current parent */
                    temp = tree;
                    tree = temp->u.subtree[0];
                    temp->u.subtree[0] = parent;
                    temp->flags |= PTF_TRAVERSE_LEFT;
                    parent = temp;

                    temp_copy = grandparent_copy;
                    grandparent_copy = parent_copy;
                    parent_copy = *curr_copy;
                    curr_copy = &((*curr_copy)->u.subtree[0]);
                    if( grandparent_copy != NULL ) {
                        if( grandparent_copy->u.subtree[0] == parent_copy ) {
                            grandparent_copy->u.subtree[0] = temp_copy;
                            grandparent_copy->flags |= PTF_TRAVERSE_LEFT;
                        } else {
                            grandparent_copy->u.subtree[1] = temp_copy;
                        }
                    }

                    /* descend to left child node */
                    state = PTREE_PROCESS;
                } else {
                    /* descend to right child node */
                    state = PTREE_RIGHT;
                }
            } else {
                /* no subtrees; unwind up to parent */
                state = PTREE_UNWIND;
            }
            continue;
        case PTREE_RIGHT:
            if( ( ptreePTSFlags[ tree->op ] & PTS_BINARY ) &&
                tree->u.subtree[1] != NULL ) {
                /* has a right subtree; store current parent */
                temp = tree;
                tree = temp->u.subtree[1];
                temp->u.subtree[1] = parent;
                temp->flags &= ~PTF_TRAVERSE_LEFT;
                parent = temp;

                if( parent->u.subtree[0] != NULL ) {
                    // we were left, so we're going right now
                    if( parent_copy != NULL ) {
                        curr_copy = &(parent_copy->u.subtree[1]);
                    } else {
                        curr_copy = &(the_copy->u.subtree[1]);
                        parent_copy = the_copy;
                    }
                } else {
                    // we're going right, but didn't go left (NULL)
                    temp_copy = grandparent_copy;
                    grandparent_copy = parent_copy;
                    parent_copy = *curr_copy;
                    curr_copy = &((*curr_copy)->u.subtree[1]);
                    if( grandparent_copy != NULL ) {
                        if( grandparent_copy->u.subtree[0] == parent_copy ) {
                            grandparent_copy->u.subtree[0] = temp_copy;
                            grandparent_copy->flags |= PTF_TRAVERSE_LEFT;
                        } else {
                            grandparent_copy->u.subtree[1] = temp_copy;
                        }
                    }
                }

                /* descend to right child node */
                state = PTREE_PROCESS;
            } else {
                if( tree->u.subtree[0] != NULL ) { // we did actually go left
                    /* no right subtree; unwind up to parent */
                    temp_copy = parent_copy;
                    parent_copy = grandparent_copy;
                    if( parent_copy != NULL ) {
                        if( parent_copy->flags & PTF_TRAVERSE_LEFT ) {
                            parent_copy->flags &= ~PTF_TRAVERSE_LEFT;
                            grandparent_copy = parent_copy->u.subtree[0];
                            parent_copy->u.subtree[0] = temp_copy;
                            curr_copy = &(parent_copy->u.subtree[0]);
                        } else {
                            grandparent_copy = parent_copy->u.subtree[1];
                            parent_copy->u.subtree[1] = temp_copy;
                            curr_copy = &(parent_copy->u.subtree[1]);
                        }
                    } else {
                        curr_copy = &the_copy; // unwound to start of call
                    }
                }
                state = PTREE_UNWIND;
            }
            continue;
        case PTREE_UNWIND:
            if( parent == NULL ) {
                break;
            }

            if( parent->flags & PTF_TRAVERSE_LEFT ) {
                parent->flags &= ~PTF_TRAVERSE_LEFT;
                /* set child pointer of parent to new node */
                temp = parent;
                parent = temp->u.subtree[0];
                temp->u.subtree[0] = tree;
                tree = temp;

                /* processed a left child; try for right hand child of parent */
                state = PTREE_RIGHT;
            } else {
                /* set child pointer of parent to new node */
                temp = parent;
                parent = temp->u.subtree[1];
                temp->u.subtree[1] = tree;
                tree = temp;

                temp_copy = parent_copy;
                parent_copy = grandparent_copy;
                if( parent_copy != NULL ) {
                    if( parent_copy->flags & PTF_TRAVERSE_LEFT ) {
                        parent_copy->flags &= ~PTF_TRAVERSE_LEFT;
                        grandparent_copy = parent_copy->u.subtree[0];
                        parent_copy->u.subtree[0] = temp_copy;
                        curr_copy = &(parent_copy->u.subtree[0]);
                    } else {
                        grandparent_copy = parent_copy->u.subtree[1];
                        parent_copy->u.subtree[1] = temp_copy;
                        curr_copy = &(parent_copy->u.subtree[1]);
                    }
                } else {
                    curr_copy = &the_copy; // unwound to start of call
                }


                /* processed a right child; unwind to parent */
                state = PTREE_UNWIND;
            }
            continue;
        }
        break;
    }
    return the_copy;
}
