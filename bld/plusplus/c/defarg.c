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
#include "name.h"
#include "initdefs.h"
#include "fnbody.h"
#include "stats.h"
#include "defarg.h"
#include "ring.h"
#include "codegen.h"
#include "pcheader.h"
#include "carve.h"

static CNV_DIAG diagDefarg =        // diagnosis for def. argument conversion
    {   ERR_DEFARG_IMPOSSIBLE
    ,   ERR_DEFARG_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_DEFARG_PRIVATE
    ,   ERR_DEFARG_PROTECTED
    };

ExtraRptCtr( ctr_defargs );
ExtraRptCtr( ctr_defargs_intconst );
ExtraRptCtr( ctr_defargs_fpconst );
ExtraRptCtr( ctr_defargs_symbol );
ExtraRptCtr( ctr_defargs_complex );

#define BLOCK_RELOC_LIST        16
static carve_t carveRELOC_LIST;

#ifndef NDEBUG

// for debugging, keep list of defarg ptree so we can free them in our
// fini routine
static PTREE DefargList = NULL;

void DefArgPCHWrite( void )
/*************************/
{
    PTreePCHWrite( DefargList );
}

void DefArgPCHRead( void )
/************************/
{
    DefargList = PTreePCHRead();
}

static void defargFreePtrees( void )  // FREE ALL DEFARG PTREES
{
    PTREE curr;
    PTREE next;
    PTREE defarg_info;

    next = NULL;
    for( curr = DefargList; curr != NULL; curr = next ) {
        next = curr->u.subtree[1];
        defarg_info = curr->u.subtree[0];
        NodeFreeDupedExpr( defarg_info->u.type.next );
        PTreeFree( defarg_info );
        PTreeFree( curr );
    }
    DefargList = NULL;
}

static PTREE storeIfDebug( PTREE defarg_info )
{
    PTREE link_node;

    // for debugging, keep list of defarg ptree
    // so we can free them in our fini routine
    link_node = PTreeAlloc();
    link_node->op = PT_BINARY;
    // store defarg info at subtree zero
    link_node->u.subtree[0] = defarg_info;
    // link to rest of list
    link_node->u.subtree[1] = DefargList;
    DefargList = link_node;
    return link_node;
}

static PTREE retrieveIfDebug( PTREE defarg_info )
{
    return defarg_info == NULL ? NULL : defarg_info->u.subtree[0];
}
#else

#define storeIfDebug(defarg_info) defarg_info
#define retrieveIfDebug(defarg_info) defarg_info

#endif

static PTREE copyRtn (  // Copy Routine -- pass to PTreeCopyPrefix
/******************/
    PTREE curr,         // - addr( current node )
    void *param )       // - param
{

    PTREE copy;
    RELOC_LIST *reloc_list;
    PTO_FLAG flags;
    PTREE partner;
    PTREE partner_copy;

    reloc_list = (RELOC_LIST *)param;

    copy = NULL;
    switch( curr->op ) { // allocate and copy ourselves for special cases
    case PT_FLOATING_CONSTANT :
    case PT_DUP_EXPR :
    case PT_SYMBOL :
        copy = PTreeAlloc();
        *copy = *curr;
        copy->decor = PtdDuplicateReloc( curr, reloc_list );
        break;
    }

    switch( curr->op ) {
    case PT_FLOATING_CONSTANT :
        copy->u.floating_constant = BFCopy( curr->u.floating_constant );
        break;
    case PT_DUP_EXPR :
        if( curr->flags & PTF_DUP_VISITED ) {
            // have already copied partner
            curr->flags &= ~PTF_DUP_VISITED;
            partner = curr->u.dup.node;
            DbgAssert( partner != NULL );
            partner_copy = partner->u.dup.node;
            DbgAssert( partner_copy != NULL );
            DbgAssert( partner_copy->u.dup.node == curr );
            copy->u.dup.subtree[0] = partner_copy->u.dup.subtree[0];
            partner_copy->u.dup.node = copy;
            copy->u.dup.node = partner_copy;
            partner->u.dup.node = curr;
        } else {
            partner = curr->u.dup.node;
            partner->flags |= PTF_DUP_VISITED;
            curr->u.dup.node = copy;
        }
        break;
    case PT_SYMBOL :
        // relocate symbol
        copy->u.symcg.symbol = SymReloc( curr->u.symcg.symbol, reloc_list );
        copy->u.symcg.result = NULL;
        break;
    default :
        copy = PTreeAssignReloc( NULL, curr, reloc_list );
        flags = PTreeOpFlags(curr);
        if( flags & PTO_BINARY ) {
            copy->u.subtree[0] = NULL;
            copy->u.subtree[1] = NULL;
        }
        if( flags & PTO_UNARY ) {
            copy->u.subtree[0] = NULL;
        }
        break;
    }
    copy->flags |= PTF_DEFARG_COPY; // mark this ptree as a copy for defargs
    return copy;
}

static PTREE defaultArgSymError( MSG_NUM msg, PTREE expr, SYMBOL sym )
{
    PTreeSetErrLoc( expr );
    if( sym != NULL ) {
        CErr2p( msg, sym );
        sym->flag |= SF_ERROR;
    } else {
        CErr1( msg );
    }
    expr = PTreeErrorNode( expr );
    return( expr );
}


static PTREE symCheck( PTREE expr )
{
    SYMBOL sym;
    SCOPE scope;

    if( expr->op != PT_SYMBOL ) {
        return( expr );
    }
    sym = expr->u.symcg.symbol;
    if( sym == NULL ) {
        expr = defaultArgSymError( ERR_DEFAULT_ARG_USES_THIS, expr, sym );
    } else {
        if( ! SymIsTemporary( sym ) ) {
            scope = SymScope( sym );
            if( ScopeType( scope, SCOPE_FUNCTION ) ) {
                expr = defaultArgSymError( ERR_DEFAULT_ARG_USES_ARG, expr, sym );
            } else if( ScopeType( scope, SCOPE_BLOCK ) ) {
                expr = defaultArgSymError( ERR_DEFAULT_ARG_USES_LOCAL,expr,sym);
            }
        }
    }
    return( expr );
}

bool AddDefaultArgs(            // ADD DEFAULT ARGUMENTS, AS REQ'D
/******************/
    SYMBOL func,                // - symbol being called
    PTREE expr )                // - expression for call
{
    SCOPE defarg_temps;         // scope containing defarg temps
    PTREE *args;                // addr( call arguments )
    PTREE init_node;            // node to setup function symbol
    PTREE func_node;            // node for function symbol
//    PTREE arg;                  // new argument
    PTREE arg_locn;             // expr used to give locn for CO_LIST node
    TYPE arg_type;              // argument type in prototype
    arg_list *proto_without;    // function arg list without default arg
    arg_list *proto_with;       // function arg list with arg for def arg's value
    PTREE defarg_expr;          // expression for current default arg
    SYMBOL curr;                // loop variable
    SYMBOL stop;                // for loop stop condition
    RELOC_LIST *reloc_list;     // relocation list being built
    RELOC_LIST *reloc_elem;     // current relocation list element
    PTREE defarg_info;          // PTREE containing the expr and scope

    if( expr == NULL || expr->op == PT_ERROR ) {
        return TRUE;
    }
    arg_locn = expr;
    init_node = expr->u.subtree[0];
    func_node = init_node->u.subtree[0];
    if( func_node->op != PT_SYMBOL ) {
        /* virtual function calls (e.g., p->foo(1); ) */
        func_node = NULL;
    } else {
        /* normal named calls (e.g., foo(1); ) */
        func = func_node->u.symcg.symbol;
    }
    if( func->id == SC_DEFAULT ) {
        args = PTreeRefRight( expr );
        while( *args != NULL ) {
            args = PTreeRefLeft( *args );
        }
//        arg = NULL;
        do {
            defarg_expr = NULL;
            reloc_list = NULL;
            defarg_info = retrieveIfDebug( func->u.defarg_info );
            if( NULL == defarg_info
             || defarg_info->u.type.next->op == PT_ERROR ) {
                PTreeErrorNode( expr );
                return FALSE;
            }
            defarg_temps = defarg_info->u.type.scope;
            if( defarg_temps != NULL && defarg_temps->ordered != NULL ) {
                curr = NULL;
                stop = ScopeOrderedStart( defarg_temps );
                for(;;) {
                    curr = ScopeOrderedNext( stop, curr );
                    if( curr == NULL ) break;

                    // build reloc-list
                    // that is, list of: [curr, dest]
                    // pass this list to copyRtn so that all references
                    // to curr can be changed to dest
                    reloc_elem = RingCarveAlloc( carveRELOC_LIST, &reloc_list );
                    reloc_elem->orig = curr;
                    // copy temps to CurrScope
                    reloc_elem->dest = SymCreateCurrScope( curr->sym_type
                                                         , curr->id
                                                         , curr->flag
                                                         , NameDummy() );
                    DbgAssert( reloc_elem->dest != NULL );
                }
            }
            // make copy of expr to use
            defarg_expr = PTreeCopyPrefix( defarg_info->u.type.next
                                         , &copyRtn
                                         , (void *)reloc_list );
            defarg_expr = PTreeCopySrcLocation( defarg_expr, arg_locn );
            if( reloc_list != NULL ) {
                RingCarveFree( carveRELOC_LIST, &reloc_list );
            }
            // coded like this to handle: foo( int = 0, ... )
            proto_without = TypeArgList( func->sym_type );
            proto_with = TypeArgList( func->thread->sym_type );
            arg_type = proto_with->type_list[ proto_without->num_args + 1 - 1 ];
            if( ! NodeConvertArgument( &defarg_expr, arg_type ) ) {
                PTreeErrorNode( defarg_expr );
                break;
            }
            defarg_expr = NodeArg( defarg_expr );
            *args = defarg_expr;
            args = &defarg_expr->u.subtree[0]; // used if more than one defarg
            func->flag |= SF_REFERENCED;
            func = func->thread;
        } while( func->id == SC_DEFAULT );
        SymSetNvReferenced( func );
        if( func_node != NULL ) {
            func_node->u.symcg.symbol = func; // put symbol without defargs at start of list
            func_node->type = func->sym_type;
            init_node->type = func->sym_type;
        }
    }
    return TRUE;
}

static void analyseDefaultExpr( // ANALYSE A DEFAULT ARGUMENT EXPRESSION
/*****************************/
    DECL_INFO *arg_dinfo )      // -declaration information
{
    TYPE type_ret;              // type to convert defarg expression to
    PTREE expr;                 // expression given for default argument
    PTREE defarg_info;          // default argument info (expr and scope)
    SYMBOL func;                // function with default arguments
    FUNCTION_DATA fn_data;

    expr = arg_dinfo->defarg_expr;
    type_ret = arg_dinfo->type;
    func = arg_dinfo->proto_sym;

    ExtraRptIncrementCtr( ctr_defargs );

    defarg_info = PTreeType( NULL );
    func->u.defarg_info = storeIfDebug(defarg_info);

    defarg_info->u.type.scope = ScopeBegin( SCOPE_FUNCTION );
    FunctionBodyDefargStartup(func, &fn_data);

    expr = AnalyseRawExpr( expr );
    if( expr->op != PT_ERROR ) {
        expr = CastImplicit( expr, type_ret, CNV_FUNC_DARG, &diagDefarg );
    }

#ifndef NDEBUG
    if( expr != NULL && expr->op != PT_ERROR ) {
        int complex = 1;
        switch( expr->op ) {
        case PT_INT_CONSTANT:
            ExtraRptIncrementCtr( ctr_defargs_intconst );
            complex = 0;
            break;
        case PT_FLOATING_CONSTANT:
            ExtraRptIncrementCtr( ctr_defargs_fpconst );
            complex = 0;
            break;
        default:
            if( NodeIsUnaryOp( expr, CO_FETCH ) ) {
                PTREE sub = expr->u.subtree[0];
                if( sub->op == PT_SYMBOL ) {
                    ExtraRptIncrementCtr( ctr_defargs_symbol );
                    complex = 0;
                }
            }
        }
        if( complex ) {
            ExtraRptIncrementCtr( ctr_defargs_complex );
        }
    }
#endif

    defarg_info->u.type.next = PTreeTraversePostfix( expr, &symCheck );

    FunctionBodyDefargShutdown(&fn_data);
    defarg_info->u.type.scope = ScopeEnd( SCOPE_FUNCTION );
}


void DefineDefaultValues( DECL_INFO *dinfo )
/******************************************/
{
    DECL_INFO *curr;

    RingIterBeg( dinfo->parms, curr ) {
        if( curr->type->id == TYP_DOT_DOT_DOT ) break;
        if( curr->has_defarg ) {
            DbgAssert( curr->defarg_expr != NULL );
            DbgAssert( curr->defarg_rewrite == NULL );
            analyseDefaultExpr( curr );
            curr->defarg_expr = NULL;
        }
    } RingIterEnd( curr )
}


void ProcessDefArgs( DECL_INFO *dinfo )
/*************************************/
{
    SCOPE save_scope;
    SCOPE scope;

    scope = SymScope( dinfo->sym );
    save_scope = GetCurrScope();
    SetCurrScope(scope);
    DefineDefaultValues( dinfo );
    FreeDeclInfo( dinfo );
    SetCurrScope(save_scope);
}


static void init(               // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    ExtraRptRegisterCtr( &ctr_defargs, "# defargs defined" );
    ExtraRptRegisterCtr( &ctr_defargs_intconst, "# defargs that are integral constants" );
    ExtraRptRegisterCtr( &ctr_defargs_fpconst, "# defargs that are floating point constants" );
    ExtraRptRegisterCtr( &ctr_defargs_symbol, "# defargs that are symbols" );
    ExtraRptRegisterCtr( &ctr_defargs_complex, "# defargs that are more complex" );
    DbgStmt( DefargList = NULL );
    carveRELOC_LIST = CarveCreate( sizeof( RELOC_LIST ), BLOCK_RELOC_LIST );
}

static void fini(               // COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    DbgStmt( defargFreePtrees() );
    DbgStmt( CarveVerifyAllGone( carveRELOC_LIST, "RELOC_LIST" ) );
    CarveDestroy( carveRELOC_LIST );
    if( carveRELOC_LIST != NULL ) {
        carveRELOC_LIST = NULL;
    }
}

INITDEFN( defarg, init, fini );
