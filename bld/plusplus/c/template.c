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


#include <string.h>

#include "plusplus.h"
#include "cgfront.h"
#include "errdefns.h"
#include "rewrite.h"
#include "ring.h"
#include "stack.h"
#include "memmgr.h"
#include "class.h"
#include "name.h"
#include "yydriver.h"
#include "fnovload.h"
#include "context.h"
#include "template.h"
#include "pcheader.h"
#include "initdefs.h"

#define BLOCK_TEMPLATE_INFO     16
#define BLOCK_CLASS_INST        32
#define BLOCK_TEMPLATE_MEMBER   32
#define BLOCK_FN_TEMPLATE_DEFN  16
static carve_t carveTEMPLATE_INFO;
static carve_t carveCLASS_INST;
static carve_t carveTEMPLATE_MEMBER;
static carve_t carveFN_TEMPLATE_DEFN;

static TEMPLATE_DATA *currentTemplate;
static TEMPLATE_INFO *allClassTemplates;
static FN_TEMPLATE_DEFN *allFunctionTemplates;  // only ones with definitions
static struct {
    unsigned            max_depth;
    unsigned            curr_depth;
    tc_fn_control       fn_control;
    SYMBOL              translate_fn;
    TYPE                extra_member_class;
    unsigned            keep_going : 1;
    unsigned            extra_members : 1;
    unsigned            depth_diagnosed : 1;
} templateData;

static CNV_DIAG diagTempParm =  // DIAGNOSIS FOR TEMPLATE ARGUMENT CONVERSION
    { ERR_PARM_IMPOSSIBLE
    , ERR_PARM_AMBIGUOUS
    , ERR_CALL_WATCOM
    , ERR_PARM_PRIVATE
    , ERR_PARM_PROTECTED
    };

enum template_context_type {
    TCTX_CLASS_DEFN     = 1,    // instantiating template class defn
    TCTX_FN_DEFN        = 2,    // instantiating template function defn
    TCTX_MEMBER_DEFN    = 3,    // instantiating template class member defn
    TCTX_FN_BIND        = 4,    // binding a function template to arguments
    TCTX_FN_BIND_AND_GEN= 5,    // binding a function template to a fn sym
    TCTX_NULL           = 0
};

typedef struct template_context TEMPLATE_CONTEXT;
struct template_context {
    TEMPLATE_CONTEXT            *prev;
    TOKEN_LOCN                  *locn;
    union {
        SYMBOL                  sym;    // FN_DEFN, FN_BIND
        SCOPE                   scope;  // CLASS_DEFN
        void                    *any;
    } u;
    enum template_context_type  id;
};

static struct {
    NESTED_POST_CONTEXT         registration;
    TEMPLATE_CONTEXT            *inst_stack;
} activeInstantiations;

static SUICIDE_CALLBACK templateSuicide;

static void templateSuicideHandler( void )
{
    activeInstantiations.inst_stack = NULL;
    templateData.curr_depth = 0;
}

static SYMBOL firstSymbol( SCOPE scope )
{
    SYMBOL first;

    first = ScopeOrderedFirst( scope );
    DbgAssert( first != NULL );
    return( first );
}

static void verifyOKToProceed( TOKEN_LOCN *locn )
{
    templateData.curr_depth++;
    if( templateData.curr_depth > templateData.max_depth ) {
        if( ! templateData.depth_diagnosed ) {
            templateData.depth_diagnosed = TRUE;
            SetErrLoc( locn );
            CErr2( ERR_TEMPLATE_DEPTH_EXHAUSTED, templateData.max_depth * 2 );
            CSuicide();
        }
    }
}

static void pushInstContext( TEMPLATE_CONTEXT *ctx, enum template_context_type id,
                             TOKEN_LOCN *locn, void *extra_info )
{
    verifyOKToProceed( locn );
    ctx->id = id;
    ctx->locn = locn;
    ctx->u.any = extra_info;
    StackPush( &(activeInstantiations.inst_stack), ctx );
}

static void popInstContext( void )
{
    templateData.curr_depth--;
    StackPop( &(activeInstantiations.inst_stack) );
}

static TYPE extractTemplateClass( TEMPLATE_CONTEXT *ctx )
{
    SYMBOL sym;

    sym = firstSymbol( ctx->u.scope );
    if( sym == NULL ) {
        return( TypeError );
    }
    DbgAssert( sym->id == SC_TYPEDEF );
    return( sym->sym_type );
}

static void displayActiveInstantiations( NESTED_POST_CONTEXT *blk )
{
    TEMPLATE_CONTEXT *ctx;

#ifndef NDEBUG
    if( blk != &(activeInstantiations.registration) ) {
        CFatal( "registered call-back for template locations incorrect" );
    }
#else
    blk = blk;
#endif
    Stack_forall( activeInstantiations.inst_stack, ctx ) {
        if( ctx->locn != NULL ) {
            switch( ctx->id ) {
            case TCTX_CLASS_DEFN:
                AddNoteMessage( INF_TEMPLATE_CLASS_DEFN_TRACEBACK,
                                extractTemplateClass( ctx ), ctx->locn );
                break;
            case TCTX_FN_DEFN:
                AddNoteMessage( INF_TEMPLATE_FN_DEFN_TRACEBACK,
                                ctx->u.sym, ctx->locn );
                break;
            case TCTX_MEMBER_DEFN:
                AddNoteMessage( INF_TEMPLATE_MEMBER_DEFN_TRACEBACK,
                                ctx->locn );
                break;
            case TCTX_FN_BIND:
                AddNoteMessage( INF_TEMPLATE_FN_BIND_TRACEBACK,
                                ctx->u.sym, ctx->locn );
                break;
            case TCTX_FN_BIND_AND_GEN:
                AddNoteMessage( INF_TEMPLATE_FN_BIND_AND_GEN_TRACEBACK,
                                ctx->u.sym, ctx->locn );
                break;
            }
        }
    }
}

void TemplateSetDepth( unsigned depth )
/*************************************/
{
    if( depth < 2 ) {
        /* we need at least 2 in order to bind a function */
        depth = 2;
    }
    templateData.max_depth = depth;
}

static void templateInit( INITFINI* defn )
{
    defn = defn;
    if( CompFlags.dll_subsequent ) {
        currentTemplate = NULL;
        allClassTemplates = NULL;
        allFunctionTemplates = NULL;
        memset( &templateData, 0, sizeof( templateData ) );
    }
    templateData.max_depth = TEMPLATE_MAX_DEPTH;
    activeInstantiations.registration.call_back = displayActiveInstantiations;
    CtxRegisterPostContext( &(activeInstantiations.registration) );
    templateSuicide.call_back = templateSuicideHandler;
    RegisterSuicideCallback( &templateSuicide );
    carveTEMPLATE_INFO = CarveCreate( sizeof( TEMPLATE_INFO ), BLOCK_TEMPLATE_INFO );
    carveCLASS_INST = CarveCreate( sizeof( CLASS_INST ), BLOCK_CLASS_INST );
    carveTEMPLATE_MEMBER = CarveCreate( sizeof( TEMPLATE_MEMBER ), BLOCK_TEMPLATE_MEMBER );
    carveFN_TEMPLATE_DEFN = CarveCreate( sizeof( FN_TEMPLATE_DEFN ), BLOCK_FN_TEMPLATE_DEFN );
}

static void templateFini( INITFINI *defn )
{
    defn = defn;
    CarveDestroy( carveTEMPLATE_INFO );
    CarveDestroy( carveCLASS_INST );
    CarveDestroy( carveTEMPLATE_MEMBER );
    CarveDestroy( carveFN_TEMPLATE_DEFN );
}

INITDEFN( template, templateInit, templateFini )

static TYPE setArgIndex( SYMBOL sym, unsigned index )
{
    TYPE type;

    type = sym->sym_type;
    DbgAssert( type->next == NULL && type->id == TYP_GENERIC );
    type->u.g.index = index;
    type = CheckDupType( type );
    sym->sym_type = type;
    return( type );
}

static void injectGenericTypes( DECL_INFO *args )
{
    char *name;
    PTREE def_arg;
    DECL_INFO *curr;
    SYMBOL sym;
    unsigned index;
    boolean unnamed_diagnosed;

    unnamed_diagnosed = FALSE;
    index = 1;
    RingIterBeg( args, curr ) {
        name = curr->name;
        sym = curr->generic_sym;
        if( sym != NULL ) {
            curr->type = setArgIndex( sym, index );
            ++index;
            sym = ScopeInsert( CurrScope, sym, name );
            def_arg = curr->defarg_expr;
            if( def_arg != NULL ) {
                PTreeErrorExpr( def_arg, ERR_NO_TYPE_DEFAULTS );
                PTreeFreeSubtrees( def_arg );
                curr->defarg_expr = NULL;
            }
        } else if( name == NULL ) {
            if( ! unnamed_diagnosed ) {
                CErr1( ERR_NO_UNNAMED_TEMPLATE_ARGS );
                unnamed_diagnosed = TRUE;
            }
        }
    } RingIterEnd( curr )
}

void TemplateDeclInit( TEMPLATE_DATA *data, DECL_INFO *args )
/***********************************************************/
{
    StackPush( &currentTemplate, data );
    CErrCheckpoint( &(data->errors) );
    if( ProcessTemplateArgs( args ) ) {
        data->all_generic = TRUE;
    } else {
        data->all_generic = FALSE;
    }
    data->args = args;
    data->decl_scope = ScopeBegin( SCOPE_TEMPLATE_DECL );
    data->defn = NULL;
    data->member_defn = NULL;
    data->template_name = NULL;
    data->locn.src_file = NULL;
    data->defn_found = FALSE;
    data->member_found = FALSE;
    data->defn_added = FALSE;
    injectGenericTypes( args );
}

static unsigned getArgList( DECL_INFO *args, TYPE *type_list, char **names )
{
    DECL_INFO *curr;
    unsigned count;

    count = 0;
    RingIterBeg( args, curr ) {
        if( type_list != NULL ) {
            type_list[count] = curr->type;
        }
        if( names != NULL ) {
            names[count] = curr->name;
        }
        ++count;
    } RingIterEnd( curr )
    return( count );
}

static unsigned getTinfoArgList( DECL_INFO *args, TEMPLATE_INFO *tinfo )
{
    return( getArgList( args, tinfo->type_list, tinfo->arg_names ) );
}

static TEMPLATE_INFO *newTemplateInfo( TEMPLATE_DATA *data )
{
    DECL_INFO *args;
    TEMPLATE_INFO *tinfo;
    unsigned arg_count;

    args = data->args;
    arg_count = getArgList( args, NULL, NULL );
    tinfo = RingCarveAlloc( carveTEMPLATE_INFO, &allClassTemplates );
    tinfo->instantiations = NULL;
    tinfo->member_defns = NULL;
    tinfo->unbound_type = ClassUnboundTemplate( data->template_name );
    tinfo->corrupted = FALSE;
    tinfo->num_args = arg_count;
    tinfo->arg_names = CPermAlloc( arg_count * sizeof( char * ) );
    tinfo->sym = NULL;
    tinfo->type_list = CPermAlloc( arg_count * sizeof( TYPE ) );
    tinfo->defn = data->defn;
    tinfo->defn_found = FALSE;
    tinfo->free = FALSE;
    if( data->defn_found ) {
        tinfo->defn_found = TRUE;
    }
    getTinfoArgList( args, tinfo );
    return( tinfo );
}

static SYMBOL newTemplateSymbol( TEMPLATE_DATA *data )
{
    SYMBOL sym;
    SCOPE scope;
    TEMPLATE_INFO *tinfo;

    scope = CurrScope;
    tinfo = newTemplateInfo( data );
    sym = AllocSymbol();
    sym->id = SC_CLASS_TEMPLATE;
    sym->u.tinfo = tinfo;
    sym->sym_type = TypeGetCache( TYPC_CLASS_TEMPLATE );
    tinfo->sym = sym;
    if( data->locn.src_file != NULL ) {
        SymbolLocnDefine( &(data->locn), sym );
    }
    sym = ScopeInsert( scope, sym, data->template_name );
    return( sym );
}

void TemplateUsingDecl( SYMBOL sym, TOKEN_LOCN *locn )
/****************************************************/
{
    SYMBOL new_sym;

    DbgAssert( sym->id == SC_CLASS_TEMPLATE );
    new_sym = SymCreateAtLocn( sym->sym_type
                             , SC_CLASS_TEMPLATE
                             , SF_NULL
                             , sym->name->name
                             , CurrScope
                             , locn );
    if( new_sym != NULL ) {
        new_sym->u.tinfo = sym->u.tinfo;
    }
}

SYMBOL ClassTemplateLookup( char *name )
/**************************************/
{
    SCOPE file_scope;
    SEARCH_RESULT *result;
    SYMBOL_NAME sym_name;
    SYMBOL sym;

    file_scope = ScopeNearestFile( CurrScope );
    result = ScopeFindNaked( file_scope, name );
    if( result != NULL ) {
        sym_name = result->sym_name;
        ScopeFreeResult( result );
        sym = sym_name->name_type;
        if( sym != NULL && SymIsClassTemplateModel( sym ) ) {
            return( sym );
        }
    }
    return( NULL );
}

static boolean templateArgListsSame( DECL_INFO *args, TEMPLATE_INFO *tinfo )
{
    unsigned curr_count;
    unsigned i;
    DECL_INFO *curr;

    curr_count = getArgList( args, NULL, NULL );
    if( curr_count != tinfo->num_args ) {
        return( FALSE );
    }
    i = 0;
    RingIterBeg( args, curr ) {
        if( ! TypesIdentical( curr->type, tinfo->type_list[i] ) ) {
            return( FALSE );
        }
        ++i;
    } RingIterEnd( curr )
    return( TRUE );
}

static boolean sameArgNames( DECL_INFO *args, char **names )
{
    DECL_INFO *curr;

    RingIterBeg( args, curr ) {
        if( *names != curr->name ) {
            return( FALSE );
        }
        ++names;
    } RingIterEnd( curr )
    return( TRUE );
}

static char **getUniqueArgNames( DECL_INFO *args, TEMPLATE_INFO *tinfo )
{
    unsigned arg_count;
    char **arg_names;

    arg_names = tinfo->arg_names;
    if( ! sameArgNames( args, arg_names ) ) {
        arg_count = getArgList( args, NULL, NULL );
        arg_names = CPermAlloc( arg_count * sizeof( char * ) );
        getArgList( args, NULL, arg_names );
    }
    return( arg_names );
}

static void mergeClassTemplates( TEMPLATE_DATA *data, SYMBOL old_sym )
{
    DECL_INFO *args;
    TEMPLATE_INFO *tinfo;
    REWRITE *defn;

    tinfo = old_sym->u.tinfo;
    if( tinfo->corrupted ) {
        return;
    }
    args = data->args;
    if( ! templateArgListsSame( args, tinfo ) ) {
        CErr2p( ERR_CANT_OVERLOAD_CLASS_TEMPLATES, old_sym );
        return;
    }
    defn = data->defn;
    if( data->defn_found ) {
        if( tinfo->defn_found ) {
            CErr2p( ERR_CANT_REDEFINE_CLASS_TEMPLATES, old_sym );
            RewriteFree( defn );
            data->defn = NULL;
        } else {
            RewriteFree( tinfo->defn );
            tinfo->defn = defn;
            tinfo->defn_found = TRUE;
            data->defn_added = TRUE;
            tinfo->arg_names = getUniqueArgNames( args, tinfo );
            getTinfoArgList( args, tinfo );
        }
    } else {
        RewriteFree( defn );
        data->defn = NULL;
    }
}

static void addMemberEntry( TEMPLATE_INFO *tinfo, REWRITE *r, char **arg_names )
{
    TEMPLATE_MEMBER *extra_defn;

    extra_defn = RingCarveAlloc( carveTEMPLATE_MEMBER
                               , &(tinfo->member_defns) );
    extra_defn->defn = r;
    extra_defn->arg_names = arg_names;
}

static void addClassTemplateMember( TEMPLATE_DATA *data, SYMBOL sym )
{
    char **arg_names;
    DECL_INFO *args;
    TEMPLATE_INFO *tinfo;

    if( ! data->member_found ) {
        return;
    }
    tinfo = sym->u.tinfo;
    if( tinfo->corrupted ) {
        return;
    }
    args = data->args;
    if( ! templateArgListsSame( args, tinfo ) ) {
        CErr2p( ERR_CANT_OVERLOAD_CLASS_TEMPLATES, sym );
        return;
    }
    arg_names = getUniqueArgNames( args, tinfo );
    addMemberEntry( tinfo, data->member_defn, arg_names );
}

static TYPE doParseClassTemplate( TEMPLATE_INFO *tinfo, REWRITE *defn,
                                  TOKEN_LOCN *locn, tc_instantiate control )
{
    TYPE new_type;
    DECL_SPEC *dspec;
    auto TEMPLATE_CONTEXT context;

    new_type = TypeError;
    if( ! tinfo->corrupted ) {
        pushInstContext( &context, TCTX_CLASS_DEFN, locn, CurrScope );
        dspec = ParseClassInstantiation( defn, ( control & TCI_NO_CLASS_DEFN ) != 0 );
        popInstContext();
        if( dspec != NULL ) {
            new_type = dspec->partial;
            PTypeRelease( dspec );
        } else {
            tinfo->corrupted = TRUE;
        }
    }
    return( new_type );
}

static void defineAllClassDecls( SYMBOL sym )
{
    /*
        template <class T>
            class F;

        F<int> *p;
        typedef F<double> & REF;
        template <class T>
            class F {
                T x;
            };
             ^ we have to visit F<int> and F<double> to define them with
               this definition
    */
    TEMPLATE_INFO *tinfo;
    CLASS_INST *curr;
    SCOPE file_scope;
    SCOPE save_enclosing;
    SCOPE save_scope;
    SCOPE inst_scope;
    SCOPE parm_scope;
    SCOPE old_parm_scope;
    auto TOKEN_LOCN location;

    SrcFileGetTokenLocn( &location );
    save_scope = CurrScope;
    tinfo = sym->u.tinfo;
    file_scope = SymScope( tinfo->sym );
    RingIterBeg( tinfo->instantiations, curr ) {
        if( curr->specific ) {
            /* we shouldn't mess around with specific instantiations */
            continue;
        }
        /* we have to splice in the new parm scope under the old INST scope */
        /* because the parm names may have changed but the class type must be */
        /* exactly as before (it may have been used in typedefs) */
        inst_scope = curr->scope;
        old_parm_scope = inst_scope->enclosing;
        CurrScope = file_scope;
        parm_scope = ScopeBegin( SCOPE_TEMPLATE_PARM );
        ScopeSetParmCopy( parm_scope, old_parm_scope );
        copyWithNewNames( old_parm_scope, tinfo->arg_names );
        save_enclosing = ScopeEstablishEnclosing( inst_scope, parm_scope );
        CurrScope = inst_scope;
        doParseClassTemplate( tinfo, tinfo->defn, &location, TCI_NULL );
        ScopeSetEnclosing( inst_scope, save_enclosing );
    } RingIterEnd( curr )
    CurrScope = save_scope;
}

extern int WalkTemplateInst( SYMBOL sym, AInstSCOPE fscope  )
{
    TEMPLATE_INFO *tinfo;
    CLASS_INST *curr;

    tinfo = sym->u.tinfo;
    RingIterBeg( tinfo->instantiations, curr ) {
        if( !fscope( curr->scope ) ){
            return( FALSE );
        }
    } RingIterEnd( curr )
    return( TRUE );
}

void TemplateDeclFini( void )
/***************************/
{
    TEMPLATE_DATA *data;
    char *name;
    SYMBOL sym;
    SCOPE decl_scope;

    decl_scope = ScopeEnd( SCOPE_TEMPLATE_DECL );
    data = currentTemplate;
    name = data->template_name;
    sym = NULL;
    if( name != NULL && ScopeId( CurrScope ) == SCOPE_FILE ) {
        sym = ClassTemplateLookup( name );
        if( sym != NULL ) {
            mergeClassTemplates( data, sym );
        } else {
            sym = newTemplateSymbol( data );
        }
        addClassTemplateMember( data, sym );
    }
    FreeArgs( data->args );
    if( CErrOccurred( &(data->errors) ) ) {
        if( sym != NULL ) {
            sym->u.tinfo->corrupted = TRUE;
        }
    } else {
        if( data->defn_added ) {
            defineAllClassDecls( sym );
        }
    }
    StackPop( &currentTemplate );
}

static void diagnoseUnusedArg( SYMBOL sym )
{
    CErr2p( ERR_FUNCTION_TEMPLATE_MUST_USE_ALL_ARGS, sym );
}

void TemplateFunctionCheck( SYMBOL sym, DECL_INFO *dinfo )
/********************************************************/
{
    TEMPLATE_DATA *data;

    if( ! SymIsFunction( sym ) ) {
        CErr1( ERR_NO_VARIABLE_TEMPLATES );
        return;
    }
    ForceNoDefaultArgs( dinfo, ERR_FUNCTION_TEMPLATE_NO_DEFARGS );
    data = currentTemplate;
    if( ! data->all_generic ) {
        CErr1( ERR_FUNCTION_TEMPLATE_ONLY_GENERICS );
    } else if( FunctionUsesAllTypes( sym, CurrScope, diagnoseUnusedArg ) ) {
        sym->id = SC_FUNCTION_TEMPLATE;
        sym->u.defn = NULL;
        sym->sym_type = MakePlusPlusFunction( sym->sym_type );
    }
}

static FN_TEMPLATE_DEFN *buildFunctionDefn( REWRITE *r, SYMBOL sym )
{
    unsigned count;
    char **arg_names;
    TYPE *type_list;
    DECL_INFO *args;
    TEMPLATE_DATA *data;
    FN_TEMPLATE_DEFN *fn_defn;

    data = currentTemplate;
    if( CErrOccurred( &(data->errors) ) ) {
        RewriteFree( r );
        return( NULL );
    }
    args = data->args;
    count = getArgList( args, NULL, NULL );
    arg_names = CPermAlloc( count * sizeof( char * ) );
    type_list = CPermAlloc( count * sizeof( TYPE ) );
    fn_defn = RingCarveAlloc( carveFN_TEMPLATE_DEFN, &allFunctionTemplates );
    fn_defn->sym = sym;
    fn_defn->defn = r;
    fn_defn->num_args = count;
    fn_defn->arg_names = arg_names;
    fn_defn->type_list = type_list;
    getArgList( args, type_list, arg_names );
    return( fn_defn );
}

void TemplateFunctionAttachDefn( DECL_INFO *dinfo )
/*************************************************/
{
    SYMBOL sym;
    REWRITE *r;

    sym = dinfo->sym;
    if( sym == NULL ) {
        return;
    }
    r = dinfo->body;
    dinfo->body = NULL;
    if( sym->u.defn != NULL ) {
        RewriteFree( r );
        CErr2p( ERR_FUNCTION_TEMPLATE_ALREADY_HAS_DEFN, sym );
    } else {
        sym->u.defn = buildFunctionDefn( r, sym );
    }
    FreeDeclInfo( dinfo );
}

#ifndef NDEBUG
static void verifySyms( SYMBOL syms )
{
    SYMBOL check;

    RingIterBeg( syms, check ) {
        if( SymIsFunctionTemplateModel( check ) ) {
            return;
        }
    } RingIterEnd( check )
    CFatal( "trying to generate a template function without any templates" );
}
#endif

static TYPE attemptGen( arg_list *args, SYMBOL fn_template, TOKEN_LOCN *locn,
                        bgt_control *pcontrol )
{
    TYPE fn_type;
    TYPE bound_type;
    auto TEMPLATE_CONTEXT context;

    fn_type = FunctionDeclarationType( fn_template->sym_type );
    if( fn_type == NULL || ! TypeHasNumArgs( fn_type, args->num_args ) ) {
        return( NULL );
    }
    pushInstContext( &context, TCTX_FN_BIND, locn, fn_template );
    bound_type = BindGenericTypes( args, fn_template, locn, pcontrol );
    popInstContext();
    return( bound_type );
}

static SYMBOL buildTemplateFn( TYPE bound_type, SYMBOL fn_template, TOKEN_LOCN *locn )
{
    SYMBOL new_sym;
    symbol_flag new_flags;

    if( bound_type == NULL ) {
        return( NULL );
    }
    new_flags = ( fn_template->flag & SF_FN_TEMPLATE_COPY );
    new_sym = SymCreateAtLocn( bound_type
                             , 0
                             , new_flags | SF_TEMPLATE_FN
                             , fn_template->name->name
                             , SymScope( fn_template )
                             , locn );
    new_sym->u.alias = fn_template;
    return new_sym;
}

unsigned TemplateFunctionGenerate( SYMBOL *psym, arg_list *args, TOKEN_LOCN *locn,
/********************************************************************************/
                                   SYMBOL *ambigs, boolean no_trivials )
{
    SYMBOL generated_fn;
    SYMBOL syms;
    SYMBOL sym;
    SYMBOL fn_template;
    bgt_control bind_control;
    bgt_control control;
    unsigned i;
    TYPE check_fn_type;
    TYPE final_fn_type;
    auto TYPE fn_types[BGT_MAX][2];
    auto SYMBOL fn_templates[BGT_MAX][2];

#ifndef NDEBUG
    verifySyms( *psym );
    ambigs[0] = (SYMBOL)-1;
    ambigs[1] = (SYMBOL)-1;
#endif
    syms = *psym;
    if( ScopeId( SymScope( syms ) ) != SCOPE_FILE ) {
        return( FNOV_NO_MATCH );
    }
    bind_control = BGT_EXACT;
    if( ! no_trivials ) {
        bind_control = BGT_TRIVIAL;
    }
    fn_types[BGT_EXACT][0] = NULL;      /* exact match function types */
    fn_types[BGT_EXACT][1] = NULL;
    fn_types[BGT_TRIVIAL][0] = NULL;    /* trivial conversion function types */
    fn_types[BGT_TRIVIAL][1] = NULL;
    fn_types[BGT_DERIVED][0] = NULL;    /* derived class conversion function types */
    fn_types[BGT_DERIVED][1] = NULL;
    RingIterBeg( syms, sym ) {
        if( SymIsFunctionTemplateModel( sym ) ) {
            control = bind_control;
            check_fn_type = attemptGen( args, sym, locn, &control );
            if( check_fn_type != NULL ) {
                fn_types[control][1] = fn_types[control][0];
                fn_types[control][0] = check_fn_type;
                fn_templates[control][1] = fn_templates[control][0];
                fn_templates[control][0] = sym;
                if( fn_types[BGT_EXACT][1] != NULL ) {
                    /* two exact matches! */
                    ambigs[0] = fn_templates[BGT_EXACT][0];
                    ambigs[1] = fn_templates[BGT_EXACT][1];
                    return( FNOV_AMBIGUOUS );
                }
            }
        }
    } RingIterEnd( sym )
    for( i = BGT_EXACT; i < BGT_MAX; ++i ) {
        fn_template = fn_templates[ i ][0];
        final_fn_type = fn_types[ i ][0];
        if( fn_types[ i ][1] != NULL ) {
            /* two matches for a type of conversion! */
            ambigs[0] = fn_templates[i][0];
            ambigs[1] = fn_templates[i][1];
            return( FNOV_AMBIGUOUS );
        }
        if( final_fn_type != NULL ) break;
    }
    generated_fn = buildTemplateFn( final_fn_type, fn_template, locn );
    if( generated_fn != NULL ) {
        *psym = generated_fn;
        return( FNOV_NONAMBIGUOUS );
    }
    return( FNOV_NO_MATCH );
}

static void commonTemplateClass( TEMPLATE_DATA *data, PTREE id )
{
    data->template_name = id->u.id.name;
    if( id->locn.src_file != NULL ) {
        TokenLocnAssign( data->locn, id->locn );
    }
}

void TemplateClassDeclaration( PTREE id )
/***************************************/
{
    TEMPLATE_DATA *data;
    TOKEN_LOCN *locn;
    REWRITE *r;

    data = currentTemplate;
    r = ParseGetRecordingInProgress( &locn );
    data->defn = r;
    commonTemplateClass( data, id );
}

boolean TemplateClassDefinition( PTREE id )
/*****************************************/
{
    TEMPLATE_DATA *data;
    TOKEN_LOCN *locn;
    REWRITE *r;

    data = currentTemplate;
    r = ParseGetRecordingInProgress( &locn );
    if( r == NULL ) {
        CErr1( ERR_SYNTAX );
        return( TRUE );
    }
    r = RewritePackageClassTemplate( r, locn );
    data->defn = r;
    data->defn_found = TRUE;
    commonTemplateClass( data, id );
    return( r == NULL );
}

static boolean okForTemplateParm( PTREE parm )
{
    SYMBOL sym;
    SCOPE scope;

    sym = parm->u.symcg.symbol;
    sym = SymDeAlias( sym );
    parm->u.symcg.symbol = sym;
    if( SymIsStaticMember( sym ) ) {
        return( TRUE );
    }
    scope = SymScope( sym );
    if( ScopeId( scope ) == SCOPE_FILE ) {
        switch( sym->id ) {
        case SC_PUBLIC:
        case SC_EXTERN:
        case SC_NULL:
            return( TRUE );
        }
    }
    return( FALSE );
}

static PTREE templateParmSimpleEnough( TYPE arg_type, PTREE parm )
{
    PTREE sym_parm;

    switch( parm->op ) {
    case PT_ERROR:
    case PT_INT_CONSTANT:
        return( parm );
    case PT_SYMBOL:
        if( okForTemplateParm( parm ) ) {
            if( PointerType( arg_type ) != NULL ) {
                if( SymIsFunction( parm->u.symcg.symbol ) ) {
                    parm->type = arg_type;
                    return( parm );
                }
            } else if( TypeReference( arg_type ) != NULL ) {
                parm->type = arg_type;
                return( parm );
            }
        }
        break;
    case PT_UNARY:
        if( PointerType( arg_type ) != NULL ) {
            if( parm->cgop == CO_ADDR_OF ) {
                sym_parm = parm->u.subtree[0];
                if( sym_parm->op == PT_SYMBOL ) {
                    parm->u.subtree[0] = NULL;
                    PTreeFreeSubtrees( parm );
                    /* reduce parm to a PT_SYMBOL */
                    parm = sym_parm;
                    if( okForTemplateParm( parm ) ) {
                        parm->type = arg_type;
                        return( parm );
                    }
                }
            }
        }
        break;
    case PT_BINARY:
        if( parm->cgop == CO_CONVERT ) {
            sym_parm = parm->u.subtree[1];
            if( sym_parm->op == PT_SYMBOL ) {
                parm->u.subtree[1] = NULL;
                PTreeFreeSubtrees( parm );
                /* reduce parm to a PT_SYMBOL */
                parm = sym_parm;
                if( okForTemplateParm( parm ) ) {
                    parm->type = arg_type;
                    return( parm );
                }
            }
        }
        break;
    }
    PTreeErrorExpr( parm, ERR_INVALID_TEMPLATE_PARM );
    return( parm );
}

static boolean suitableForIntegralParm( PTREE parm )
{
    switch( parm->op ) {
    case PT_INT_CONSTANT:
        return( TRUE );
    }
    return( FALSE );
}

static boolean suitableForAddressParm( PTREE parm )
{
    switch( parm->op ) {
    case PT_SYMBOL:
        return( TRUE );
    }
    return( FALSE );
}

static PTREE processIndividualParm( TYPE arg_type, PTREE parm )
{
    if( ScopeAccessType( SCOPE_TEMPLATE_DECL ) ) {
        if( ! currentTemplate->all_generic ) {
            PTreeFreeSubtrees( parm );
            return( NodeZero() );
        }
    }
    parm = AnalyseRawExpr( parm );
    if( parm->op == PT_ERROR ) {
        return( parm );
    }
    parm = CastImplicit( parm, arg_type, CNV_FUNC_ARG, &diagTempParm );
    if( parm->op == PT_ERROR ) {
        return( parm );
    }
    parm = templateParmSimpleEnough( arg_type, parm );
    if( IntegralType( arg_type ) != NULL ) {
        if( ! suitableForIntegralParm( parm ) ) {
            PTreeErrorExpr( parm, ERR_TEMPLATE_ARG_NON_CONSTANT );
        }
    } else {
        if( ! suitableForAddressParm( parm ) ) {
            PTreeErrorExpr( parm, ERR_TEMPLATE_ARG_NOT_SYMBOL );
        }
    }
    return( parm );
}

static PTREE processClassTemplateParms( TEMPLATE_INFO *tinfo, PTREE parms )
{
    PTREE list;
    PTREE parm;
    TYPE arg_type;
    unsigned msg;
    unsigned num_parms;
    unsigned i;
    boolean something_went_wrong;

    parms = NodeReverseArgs( &num_parms, parms );
    something_went_wrong = FALSE;
    if( tinfo->corrupted ) {
        something_went_wrong = TRUE;
    } else if( num_parms != tinfo->num_args ) {
        msg = ERR_TOO_FEW_TEMPLATE_PARAMETERS;
        if( num_parms > tinfo->num_args ) {
            msg = ERR_TOO_MANY_TEMPLATE_PARAMETERS;
        }
        CErr1( msg );
        something_went_wrong = TRUE;
    } else if( ! something_went_wrong ) {
        i = 0;
        for( list = parms; list != NULL; list = list->u.subtree[0] ) {
            arg_type = TypedefRemove( tinfo->type_list[i] );
            if( arg_type->id == TYP_GENERIC ) {
                arg_type = NULL;
            }
            parm = list->u.subtree[1];
            if( parm->op != PT_TYPE ) {
                if( arg_type != NULL ) {
                    parm = processIndividualParm( arg_type, parm );
                    if( parm->op == PT_ERROR ) {
                        something_went_wrong = TRUE;
                    }
                } else {
                    /* non-type parameter supplied for type argument */
                    PTreeErrorExpr( parm, ERR_NON_TYPE_PROVIDED_FOR_TYPE );
                    something_went_wrong = TRUE;
                }
            } else {
                if( arg_type != NULL ) {
                    /* type parameter supplied for non-type argument */
                    PTreeErrorExpr( parm, ERR_TYPE_PROVIDED_FOR_NON_TYPE );
                    something_went_wrong = TRUE;
                }
            }
            list->u.subtree[1] = parm;
            ++i;
        }
    }
    if( something_went_wrong ) {
        NodeFreeDupedExpr( parms );
        parms = NULL;
    }
    return( parms );
}


static boolean sameConstantInt( SYMBOL s1, SYMBOL s2 )
{
    INT_CONSTANT con1;
    INT_CONSTANT con2;

    SymConstantValue( s1, &con1 );
    SymConstantValue( s2, &con2 );
    return 0 == U64Cmp( &con1.value, &con2.value );
}

boolean TemplateParmEqual( SYMBOL parm1, SYMBOL parm2 )
/*****************************************************/
{
    SYMBOL sym1;
    SYMBOL sym2;

    if( SymIsConstantInt( parm1 ) && SymIsConstantInt( parm2 ) ) {
        return sameConstantInt( parm1, parm2 );
    }
    if( SymIsTypedef( parm1 ) && SymIsTypedef( parm2 ) ) {
        return( TypesIdentical( parm1->sym_type, parm2->sym_type ) );
    }
    sym1 = SymAddressOf( parm1 );
    sym2 = SymAddressOf( parm2 );
    if( sym1 != NULL && sym2 != NULL ) {
        return( sym1 == sym2 );
    }
    return( FALSE );
}

static boolean parmsDifferent( SYMBOL temp_arg, PTREE temp_parm )
{
    SYMBOL sym;

    if( SymIsConstantInt( temp_arg ) ) {
        INT_CONSTANT con;
        switch( temp_parm->op ) {
        case PT_INT_CONSTANT:
            SymConstantValue( temp_arg, &con );
            if( 0 == U64Cmp( &con.value, &temp_parm->u.int64_constant ) ) {
                return( FALSE );
            }
            break;
        }
    } else if( SymIsTypedef( temp_arg ) ) {
        if( temp_parm->op == PT_TYPE ) {
            if( TypesIdentical( temp_arg->sym_type, temp_parm->type ) ) {
                return( FALSE );
            }
        }
    } else {
        sym = SymAddressOf( temp_arg );
        if( sym != NULL ) {
            if( temp_parm->op == PT_SYMBOL ) {
                if( sym == temp_parm->u.symcg.symbol ) {
                    return( FALSE );
                }
            }
        }
    }
    return( TRUE );
}

static SCOPE sameParms( CLASS_INST *inst, PTREE parms )
{
    SCOPE parm_scope;
    SCOPE inst_scope;
    SYMBOL curr;
    SYMBOL stop;
    PTREE list;
    PTREE parm;

    list = parms;
    inst_scope = inst->scope;
    parm_scope = inst_scope->enclosing;
    curr = NULL;
    stop = ScopeOrderedStart( parm_scope );
    for(;;) {
        if( list == NULL ) {
            DbgAssert( ScopeOrderedNext( stop, curr ) == NULL );
            return( inst_scope );
        }
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        parm = list->u.subtree[1];
        if( parmsDifferent( curr, parm ) ) break;
        list = list->u.subtree[0];
    }
    return( NULL );
}

static SCOPE findInstScope( TEMPLATE_INFO *tinfo, PTREE parms, CLASS_INST **inst )
{
    CLASS_INST *curr;
    SCOPE inst_scope;

    RingIterBeg( tinfo->instantiations, curr ) {
        inst_scope = sameParms( curr, parms );
        if( inst_scope != NULL ) {
            *inst = curr;
            return( inst_scope );
        }
    } RingIterEnd( curr )
    return( NULL );
}

static TYPE checkAlreadyDone( TEMPLATE_INFO *tinfo, PTREE parms, CLASS_INST **inst )
{
    SCOPE inst_scope;

    inst_scope = findInstScope( tinfo, parms, inst );
    if( inst_scope != NULL ) {
        /* return previously instantiated class type */
        return( firstSymbol( inst_scope )->sym_type );
    }
    return( NULL );
}

static void setDirectiveFlags( CLASS_INST *inst, tc_instantiate control )
{
    if( control & TCI_NO_MEMBERS ) {
        inst->dont_process = TRUE;
    } else if( control & TCI_EXPLICIT_FULL ) {
        inst->dont_process = FALSE;
        inst->must_process = TRUE;
    }
}

static CLASS_INST *newClassInstantiation( TEMPLATE_INFO *tinfo, SCOPE scope,
                                          tc_instantiate control )
{
    CLASS_INST *new_inst;

    new_inst = RingCarveAlloc( carveCLASS_INST, &(tinfo->instantiations) );
    new_inst->scope = scope;
    new_inst->locn.src_file = NULL;
    new_inst->must_process = FALSE;
    new_inst->dont_process = FALSE;
    new_inst->processed = FALSE;
    new_inst->specific = FALSE;
    new_inst->locn_set = FALSE;
    new_inst->free = FALSE;
    if( control & TCI_SPECIFIC ) {
        new_inst->specific = TRUE;
    }
    setDirectiveFlags( new_inst, control );
    templateData.keep_going = TRUE;
    return( new_inst );
}

static SYMBOL templateArgSym( symbol_class sc, TYPE type )
{
    SYMBOL sym;

    sym = AllocSymbol();
    sym->id = sc;
    sym->sym_type = type;
    return( sym );
}

static SYMBOL templateArgTypedef( TYPE type )
{
    SYMBOL tsym;

    tsym = templateArgSym( SC_TYPEDEF, type );
    return tsym;
}

static SYMBOL dupTemplateParm( SYMBOL old_parm )
{
    SYMBOL sym;

    sym = AllocSymbol();
    sym->id = old_parm->id;
    sym->sym_type = old_parm->sym_type;
    sym->flag = old_parm->flag;
    switch( old_parm->id ) {
    case SC_STATIC:
        if( old_parm->flag & SF_CONSTANT_INT64 ) {
            sym->flag |= SF_CONSTANT_INT64;
            sym->u.pval = old_parm->u.pval;
        } else {
            sym->u.uval = old_parm->u.uval;
        }
        break;
    case SC_ADDRESS_ALIAS:
        sym->u.alias = old_parm->u.alias;
        break;
    }
    return( sym );
}

static void injectTemplateParms( TEMPLATE_INFO *tinfo, SCOPE scope, PTREE parms )
{
    PTREE list;
    PTREE parm;
    char **pname;
    char *name;
    SYMBOL addr_sym;
    SYMBOL sym;
    TYPE parm_type;

    pname = NULL;
    if( tinfo != NULL ) {
        pname = tinfo->arg_names;
    }
    for( list = parms; list != NULL; list = list->u.subtree[0] ) {
        sym = NULL;
        parm = list->u.subtree[1];
        parm_type = parm->type;
        switch( parm->op ) {
        case PT_INT_CONSTANT:
            sym = templateArgSym( SC_STATIC, parm_type );
            DgStoreConstScalar( parm, parm_type, sym );
            break;
        case PT_TYPE:
            sym = templateArgTypedef( parm_type );
            break;
        case PT_SYMBOL:
            addr_sym = parm->u.symcg.symbol;
            if( PointerType( parm_type ) != NULL ) {
                parm_type = MakePointerTo( addr_sym->sym_type );
            } else {
                parm_type = addr_sym->sym_type;
            }
            sym = templateArgSym( SC_ADDRESS_ALIAS, parm_type );
            sym->u.alias = addr_sym;
            break;
        DbgDefault( "template parms are corrupted" );
        }
        if( pname != NULL ) {
            name = *pname;
            ++pname;
        } else {
            name = NameDummy();
        }
        if( sym != NULL ) {
            sym = ScopeInsert( scope, sym, name );
        }
    }
}

static TYPE instantiateClass( TEMPLATE_INFO *tinfo, REWRITE *defn, PTREE parms,
                              TOKEN_LOCN *locn, tc_instantiate control )
{
    boolean nothing_to_do;
    TYPE already_instantiated;
    TYPE new_type;
    SCOPE file_scope;
    SCOPE save_scope;
    SCOPE inst_scope;
    SCOPE parm_scope;
    CLASS_INST *curr_instantiation;

    curr_instantiation = NULL;
    already_instantiated = checkAlreadyDone( tinfo, parms, &curr_instantiation );
    if( already_instantiated != NULL ) {
        setDirectiveFlags( curr_instantiation, control );
        nothing_to_do = FALSE;
        if( TypeDefined( already_instantiated ) ) {
            nothing_to_do = TRUE;
        } else if( control & TCI_NO_CLASS_DEFN ) {
            nothing_to_do = TRUE;
        } else {
            already_instantiated = StructType( already_instantiated );
            if( StructOpened( already_instantiated ) != NULL ) {
                /* we're already in the middle of defining this type */
                nothing_to_do = TRUE;
            }
        }
        if( nothing_to_do ) {
            NodeFreeDupedExpr( parms );
            return( already_instantiated );
        }
    }
    save_scope = CurrScope;
    file_scope = SymScope( tinfo->sym );
    if( already_instantiated != NULL ) {
        inst_scope = curr_instantiation->scope;
        parm_scope = inst_scope->enclosing;
        ScopeClear( parm_scope );
        ScopeSetParmClass( parm_scope, tinfo );
        CurrScope = inst_scope;
    } else {
        CurrScope = file_scope;
        parm_scope = ScopeBegin( SCOPE_TEMPLATE_PARM );
        ScopeSetParmClass( parm_scope, tinfo );
        inst_scope = ScopeBegin( SCOPE_TEMPLATE_INST );
        curr_instantiation = newClassInstantiation( tinfo, inst_scope, control );
        curr_instantiation->locn = *locn;
        curr_instantiation->locn_set = TRUE;
    }
    injectTemplateParms( tinfo, parm_scope, parms );
    NodeFreeDupedExpr( parms );
    new_type = doParseClassTemplate( tinfo, defn, locn, control );
    CurrScope = save_scope;
    return( new_type );
}

static TYPE instantiateUnboundClass( TEMPLATE_INFO *tinfo, PTREE parms, char *name )
{
    TYPE new_type;
    SCOPE file_scope;
    SCOPE save_scope;
    SCOPE parm_scope;

    save_scope = CurrScope;
    file_scope = SymScope( tinfo->sym );
    CurrScope = file_scope;
    parm_scope = ScopeBegin( SCOPE_TEMPLATE_PARM );
    ScopeSetParmClass( parm_scope, tinfo );
    injectTemplateParms( tinfo, parm_scope, parms );
    new_type = ClassUnboundTemplate( name );
    ScopeOpen( new_type->u.c.scope );
    CurrScope = save_scope;
    return( new_type );
}

DECL_SPEC *TemplateClassInstantiation( PTREE tid, PTREE parms, tc_instantiate control )
/*************************************************************************************/
{
    SYMBOL class_template;
    char *template_name;
    TYPE type_instantiated;
    TEMPLATE_INFO *tinfo;
    TEMPLATE_DATA *data;

    template_name = tid->u.id.name;
    type_instantiated = TypeError;
    class_template = ClassTemplateLookup( template_name );
    if( class_template != NULL ) {
        tinfo = class_template->u.tinfo;
        parms = processClassTemplateParms( tinfo, parms );
        if( parms != NULL ) {
            /* parms have been validated; we can instantiate the class! */
            if( ScopeAccessType( SCOPE_TEMPLATE_DECL ) ) {
                type_instantiated = tinfo->unbound_type;
                data = currentTemplate;
                if( data->all_generic ) {
                    /* we could be inside a function template */
                    type_instantiated = instantiateUnboundClass( tinfo, parms,
                                                                 template_name );
                }
                NodeFreeDupedExpr( parms );
            } else {
                type_instantiated = instantiateClass( tinfo, tinfo->defn, parms,
                                                      &(tid->locn), control );
            }
        }
    } else {
        PTreeFreeSubtrees( parms );
    }
    if( control & TCI_NO_DECL_SPEC ) {
        PTreeFreeSubtrees( tid );
        return( NULL );
    }
    return( PTypeClassInstantiation( type_instantiated, tid ) );
}

void TemplateClassDirective( PTREE list, tc_directive tcd_control )
/*****************************************************************/
{
    DECL_SPEC *dspec;
    PTREE tid;
    PTREE parms;
    tc_instantiate tci_control;

    tid = list->u.subtree[0];
    parms = list->u.subtree[1];
    PTreeFree( list );
    tci_control = TCI_NO_DECL_SPEC;
    if( tcd_control & TCD_EXTERN ) {
        tci_control |= TCI_NO_MEMBERS;
    }
    if( tcd_control & TCD_INSTANTIATE ) {
        tci_control |= TCI_EXPLICIT_FULL;
    }
    dspec = TemplateClassInstantiation( tid, parms, tci_control );
    DbgAssert( dspec == NULL );
}

static PTREE fakeUpParm( SYMBOL sym )
{
    PTREE parm;

    parm = NULL;
    switch( sym->id ) {
    case SC_STATIC:
        parm = PTreeIntConstant( sym->u.uval, TYP_SINT );
        parm->type = sym->sym_type;
        break;
    case SC_ADDRESS_ALIAS:
        parm = MakeNodeSymbol( sym->u.alias );
        break;
    }
    DbgAssert( parm != NULL );
    return( parm );
}

static PTREE fakeUpTemplateParms( SCOPE parm_scope, arg_list *type_args )
{
    TYPE *curr_type_arg;
    PTREE parm;
    PTREE parms;
    SYMBOL curr;
    SYMBOL stop;
    unsigned num_parms;

    curr_type_arg = type_args->type_list;
    parms = NULL;
    curr = NULL;
    stop = ScopeOrderedStart( parm_scope );
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        if( curr->id == SC_TYPEDEF ) {
            parm = PTreeType( *curr_type_arg );
            ++curr_type_arg;
        } else {
            parm = fakeUpParm( curr );
        }
        parms = PTreeBinary( CO_LIST, parms, parm );
    }
    parms = NodeReverseArgs( &num_parms, parms );
    return( parms );
}

static TYPE makeBoundClass( char *name, SCOPE parm_scope, arg_list *type_args,
                            TOKEN_LOCN *locn )
{
    TYPE type_instantiated;
    SYMBOL class_template;
    TEMPLATE_INFO *tinfo;
    PTREE parms;

    type_instantiated = NULL;
    class_template = ClassTemplateLookup( name );
    if( class_template != NULL ) {
        parms = fakeUpTemplateParms( parm_scope, type_args );
        tinfo = class_template->u.tinfo;
        type_instantiated = instantiateClass( tinfo, tinfo->defn, parms, locn, TCI_NULL );
    }
    return( type_instantiated );
}

TYPE TemplateUnboundInstantiate( TYPE unbound_class, arg_list *type_args, TOKEN_LOCN *locn )
/******************************************************************************************/
{
    char *template_name;
    SCOPE parm_scope;
    TYPE new_type;

    new_type = unbound_class->of;
    if( new_type == NULL ) {
        parm_scope = TemplateClassParmScope( unbound_class );
        if( parm_scope != NULL ) {
            template_name = SimpleTypeName( unbound_class );
            DbgAssert( template_name != NULL );
            new_type = makeBoundClass( template_name, parm_scope, type_args, locn );
        }
    }
    return( new_type );
}

void TemplateHandleClassMember( DECL_INFO *dinfo )
/************************************************/
{
    TEMPLATE_DATA *data;
    TOKEN_LOCN *locn;
    REWRITE *r;

    DbgAssert( dinfo->template_member );
    r = ParseGetRecordingInProgress( &locn );
    r = RewritePackageClassTemplateMember( r, locn );
    data = currentTemplate;
    data->member_defn = r;
    data->member_found = TRUE;
    data->template_name = SimpleTypeName( dinfo->id->u.subtree[0]->type );
    FreeDeclInfo( dinfo );
}

static void copyWithNewNames( SCOPE old_scope, char **names )
{
    SYMBOL curr;
    SYMBOL stop;
    SYMBOL sym;

    curr = NULL;
    stop = ScopeOrderedStart( old_scope );
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        sym = dupTemplateParm( curr );
        sym = ScopeInsert( CurrScope, sym, *names );
        ++names;
    }
}

static boolean sameParmArgNames( SCOPE parm_scope, char **arg_names )
{
    SYMBOL curr;
    SYMBOL stop;

    curr = NULL;
    stop = ScopeOrderedStart( parm_scope );
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        if( curr->name->name != *arg_names ) {
            return( FALSE );
        }
        ++arg_names;
    }
    return( TRUE );
}

static void instantiateMember( TEMPLATE_INFO *tinfo, CLASS_INST *instance,
                               TEMPLATE_MEMBER *member )
{
    SCOPE save_scope;
    SCOPE file_scope;
    SCOPE inst_scope;
    SCOPE class_inst_scope;
    SCOPE class_parm_scope;
    SCOPE parm_scope;
    char **member_arg_names;
    TOKEN_LOCN *locn;
    auto TEMPLATE_CONTEXT context;

    templateData.fn_control = TCF_NULL;
    if( instance->must_process ) {
        templateData.fn_control |= TCF_GEN_FUNCTION;
    }
    save_scope = CurrScope;
    class_inst_scope = instance->scope;
    class_parm_scope = class_inst_scope->enclosing;
    member_arg_names = member->arg_names;
    if( tinfo->arg_names != member_arg_names ||
        ! sameParmArgNames( class_parm_scope, member_arg_names ) ) {
        file_scope = SymScope( tinfo->sym );
        CurrScope = file_scope;
        parm_scope = ScopeBegin( SCOPE_TEMPLATE_PARM );
        ScopeSetParmClass( parm_scope, tinfo );
        ScopeEstablishEnclosing( class_inst_scope, parm_scope );
        copyWithNewNames( class_parm_scope, member_arg_names );
    } else {
        /* template instantiation parms are identical to class instantiation */
        parm_scope = class_parm_scope;
        CurrScope = parm_scope;
    }
    inst_scope = ScopeBegin( SCOPE_TEMPLATE_INST );
    locn = NULL;
    if( instance->locn_set ) {
        locn = &(instance->locn);
    }
    pushInstContext( &context, TCTX_MEMBER_DEFN, locn, NULL );
    ParseClassMemberInstantiation( member->defn );
    popInstContext();
    ScopeSetEnclosing( class_inst_scope, class_parm_scope );
    CurrScope = save_scope;
}

static TYPE classTemplateType( CLASS_INST *instance )
{
    SCOPE inst_scope;
    SYMBOL class_sym;

    inst_scope = instance->scope;
    class_sym = firstSymbol( inst_scope );
    return( class_sym->sym_type );
}

static boolean classTemplateWasDefined( CLASS_INST *instance )
{
    return( TypeDefined( classTemplateType( instance ) ) );
}

static boolean processSymDefn( SYMBOL sym, FN_TEMPLATE_DEFN *defn )
{
    SYMBOL check_template;
    SYMBOL fn_template;
    boolean binding_OK;
    auto TEMPLATE_CONTEXT context;

    if( sym->id == SC_DEFAULT || SymIsInitialized( sym ) ) {
        /* function doesn't have to be generated */
        return( FALSE );
    }
    fn_template = defn->sym;
    check_template = SymIsFunctionTemplateInst( sym );
    if( check_template != NULL ) {
        if( check_template != fn_template ) {
            /* function doesn't match this template */
            return( FALSE );
        }
    } else {
        if( !SymIsFnTemplateMatchable( sym ) ) {
            if( ! CompFlags.prototype_instantiate ) {
                return( FALSE );
            }
        }
    }
    /* calls TemplateFunctionInstantiate with generic types bound */
    pushInstContext( &context, TCTX_FN_BIND_AND_GEN, &sym->locn->tl, fn_template );
    binding_OK = BindFunction( sym, fn_template );
    popInstContext();
    if( binding_OK ) {
        DbgAssert( SymIsInitialized( sym ) );
        return( TRUE );
    }
    return( FALSE );
}

static void injectFunctionTemplateArgs( SYMBOL sym )
{
    unsigned i;
    SYMBOL parm_sym;
    FN_TEMPLATE_DEFN *defn;
    TYPE type;
    TYPE *types;
    char **names;

    defn = sym->u.defn;
    DbgAssert( defn != NULL );
    types = defn->type_list;
    names = defn->arg_names;
    for( i = defn->num_args; i != 0; --i ) {
        type = TypedefModifierRemove( *types );
        DbgAssert( type->id == TYP_GENERIC && type->of != NULL );
        parm_sym = templateArgTypedef( type->of );
        ScopeInsert( CurrScope, parm_sym, *names );
        ++types;
        ++names;
    }
}

static boolean makeSureSymIsAMember( SCOPE scope, SYMBOL sym )
{
    if( templateData.extra_members ) {
        if( ScopeClass( scope ) == NULL ) {
            SetErrLoc( &sym->locn->tl );
            CErr2p( ERR_INVALID_TEMPLATE_MEMBER, templateData.extra_member_class );
            return( TRUE );
        }
    }
    return( FALSE );
}

SYMBOL TemplateFunctionTranslate( SYMBOL sym, SCOPE *parse_scope )
/****************************************************************/
{
    SCOPE sym_scope;
    SYMBOL replace_sym;

    sym_scope = SymScope( sym );
    replace_sym = templateData.translate_fn;
    if( replace_sym != NULL ) {
        /* so any inline functions that are parsed during this function are OK */
        templateData.translate_fn = NULL;
        sym = replace_sym;
        *parse_scope = sym_scope->enclosing;
    } else {
        makeSureSymIsAMember( sym_scope, sym );
        *parse_scope = sym_scope;
    }
    return( sym );
}

tc_fn_control TemplateFunctionControl( void )
/*******************************************/
{
    tc_fn_control ret;

    ret = templateData.fn_control;
    templateData.fn_control = TCF_NULL;
    return( ret );
}


boolean TemplateVerifyDecl( SYMBOL sym )
/**************************************/
{
    SCOPE sym_scope;

    sym_scope = SymScope( sym );
    return( makeSureSymIsAMember( sym_scope, sym ) );
}

void TemplateFunctionInstantiate( SYMBOL fn_sym, SYMBOL fn_template, void *hdl )
/******************************************************************************/
{
    SYMBOL save_fn;
    SCOPE save_scope;
    SCOPE parm_scope;
    SCOPE inst_scope;
    auto TEMPLATE_CONTEXT context;

    /* all of the TYP_GENERIC types have their '->of' set to the proper type */
    save_scope = CurrScope;
    CurrScope = SymScope( fn_template );
    parm_scope = ScopeBegin( SCOPE_TEMPLATE_PARM );
    ScopeSetParmFn( parm_scope, fn_template->u.defn );
    injectFunctionTemplateArgs( fn_template );
    inst_scope = ScopeBegin( SCOPE_TEMPLATE_INST );
    ClearAllGenericBindings( hdl );
    fn_sym->flag |= SF_TEMPLATE_FN;
    fn_sym->u.alias = fn_template;
    save_fn = templateData.translate_fn;
    templateData.translate_fn = fn_sym;
    pushInstContext( &context, TCTX_FN_DEFN, &fn_sym->locn->tl, fn_sym );
    ParseFunctionInstantiation( fn_template->u.defn->defn );
    popInstContext();
    templateData.translate_fn = save_fn;
    CurrScope = save_scope;
}

static void processFunctionTemplateDefns( void )
{
    SYMBOL fn_sym;
    FN_TEMPLATE_DEFN *curr_defn;

    RingIterBeg( allFunctionTemplates, curr_defn ) {
        RingIterBeg( curr_defn->sym->name->name_syms, fn_sym ) {
            if( ! SymIsFunctionTemplateModel( fn_sym ) ) {
                processSymDefn( fn_sym, curr_defn );
            }
        } RingIterEnd( fn_sym )
    } RingIterEnd( curr_defn )
}

static void processNewFileSyms( NAME_SPACE *ns, SYMBOL old_last, SYMBOL curr_last )
{
    FN_TEMPLATE_DEFN *defn;
    SYMBOL fn_sym;
    SYMBOL curr;
    SYMBOL stop;

    curr = old_last;
    stop = ScopeOrderedStart( ns->scope );
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        DbgAssert( ! SymIsFunctionTemplateModel( curr ) );
        if( SymIsFunction( curr ) ) {
            RingIterBeg( curr->name->name_syms, fn_sym ) {
                if( SymIsFunctionTemplateModel( fn_sym ) ) {
                    defn = fn_sym->u.defn;
                    if( defn != NULL ) {
                        if( processSymDefn( curr, defn ) ) {
                            break;
                        }
                    }
                }
            } RingIterEnd( fn_sym )
        }
        if( curr == curr_last ) break;
    }
}

static void freeDefns( void )
{
    REWRITE *r;
    TEMPLATE_INFO *tinfo;
    TEMPLATE_MEMBER *member;
    FN_TEMPLATE_DEFN *curr_fn;

    RingIterBeg( allClassTemplates, tinfo ) {
        r = tinfo->defn;
        tinfo->defn = NULL;
        RewriteFree( r );
        RingIterBeg( tinfo->member_defns, member ) {
            r = member->defn;
            member->defn = NULL;
            RewriteFree( r );
        } RingIterEnd( member )
    } RingIterEnd( tinfo )
    RingIterBeg( allFunctionTemplates, curr_fn ) {
        r = curr_fn->defn;
        curr_fn->defn = NULL;
        RewriteFree( r );
    } RingIterEnd( curr_fn )
}

static void initLastSym( NAME_SPACE *ns, void *data )
{
    data = data;
    DbgAssert( data == NULL );
    ns->last_sym = ScopeOrderedLast( ns->scope );
}

static void finishUpNameSpace( NAME_SPACE *ns, void *data )
{
    SYMBOL last_sym;
    SYMBOL curr_last;

    data = data;
    DbgAssert( data == NULL );
    last_sym = ns->last_sym;
    curr_last = ScopeOrderedLast( ns->scope );
    if( curr_last != last_sym ) {
        // could generate more symbols by processing these ones
        processNewFileSyms( ns, last_sym, curr_last );
        // ScopeOrderedLast( ns->scope ) may different than 'curr_last'
        ns->last_sym = curr_last;
        templateData.keep_going = TRUE;
    }
}

void TemplateProcessInstantiations( void )
/****************************************/
{
    TEMPLATE_INFO *curr_tinfo;
    CLASS_INST *curr_instance;
    TEMPLATE_MEMBER *curr_member;

    /* NYI: only define extra members that are required */
    ScopeWalkAllNameSpaces( initLastSym, NULL );
    // instantiate any template functions (first pass)
    processFunctionTemplateDefns();
    for(;;) {
        verifyOKToProceed( NULL );
        templateData.keep_going = FALSE;
        // instantiate extra class members
        templateData.extra_members = TRUE;
        RingIterBeg( allClassTemplates, curr_tinfo ) {
            RingIterBeg( curr_tinfo->instantiations, curr_instance ) {
                if( curr_instance->processed || curr_instance->specific
                    || curr_instance->dont_process ) {
                    continue;
                }
                if( ! classTemplateWasDefined( curr_instance ) ) {
                    continue;
                }
                templateData.extra_member_class = classTemplateType( curr_instance );
                curr_instance->processed = TRUE;
                RingIterBeg( curr_tinfo->member_defns, curr_member ) {
                    // loop nesting is critical because extra members cannot be
                    // generated if a class has not been instantiated
                    instantiateMember( curr_tinfo, curr_instance, curr_member );
                } RingIterEnd( curr_member )
            } RingIterEnd( curr_instance )
        } RingIterEnd( curr_tinfo )
        templateData.extra_members = FALSE;
        // instantiate any template functions (delta from previous pass)
        ScopeWalkAllNameSpaces( finishUpNameSpace, NULL );
        if( ! templateData.keep_going ) break;
    }
    templateData.extra_members = FALSE;
    freeDefns();
}

boolean TemplateMemberCanBeIgnored( void )
/****************************************/
{
    return( templateData.extra_members );
}

void TemplateSpecificDefnStart( char *name, PTREE parms )
/*******************************************************/
{
    SYMBOL class_template;
    CLASS_INST *instance;
    TEMPLATE_INFO *tinfo;
    SCOPE inst_scope;
    SCOPE parm_scope;

    class_template = ClassTemplateLookup( name );
    tinfo = class_template->u.tinfo;
    if( tinfo->corrupted ) {
        return;
    }
    parms = processClassTemplateParms( tinfo, parms );
    if( parms != NULL ) {
        /* parms have been validated; we can instantiate the class! */
        instance = NULL;
        inst_scope = findInstScope( tinfo, parms, &instance );
        if( inst_scope != NULL ) {
            instance->specific = TRUE;
            CurrScope = inst_scope;
        } else {
            parm_scope = ScopeBegin( SCOPE_TEMPLATE_PARM );
            ScopeSetParmClass( parm_scope, tinfo );
            inst_scope = ScopeBegin( SCOPE_TEMPLATE_INST );
            instance = newClassInstantiation( tinfo, inst_scope, TCI_SPECIFIC );
            injectTemplateParms( NULL, parm_scope, parms );
        }
        DbgAssert( instance->specific );
        NodeFreeDupedExpr( parms );
    } else {
        tinfo->corrupted = TRUE;
    }
}

void TemplateSpecificDefnEnd( void )
/**********************************/
{
    if( ScopeId( CurrScope ) == SCOPE_TEMPLATE_INST ) {
        ScopeEnd( SCOPE_TEMPLATE_INST );
        ScopeEnd( SCOPE_TEMPLATE_PARM );
    }
}

SCOPE TemplateClassInstScope( TYPE class_type )
/*********************************************/
{
    SCOPE inst_scope;

    inst_scope = NULL;
    if( class_type->flag & TF1_INSTANTIATION ) {
        inst_scope = class_type->u.c.scope->enclosing;
    }
    return( inst_scope );
}

SCOPE TemplateClassParmScope( TYPE class_type )
/*********************************************/
{
    SCOPE inst_scope;
    SCOPE parm_scope;

    parm_scope = NULL;
    if( class_type->flag & TF1_INSTANTIATION ) {
        inst_scope = TemplateClassInstScope( class_type );
        if( inst_scope != NULL ) {
            parm_scope = inst_scope->enclosing;
        }
    } else if( class_type->flag & TF1_UNBOUND ) {
        parm_scope = class_type->u.c.scope->enclosing;
    }
    return( parm_scope );
}

SYMBOL TemplateSymFromClass( TYPE class_type )
/********************************************/
{
    SYMBOL sym;
    SCOPE inst_scope;

    sym = NULL;
    inst_scope = TemplateClassInstScope( class_type );
    if( inst_scope != NULL ) {
        sym = firstSymbol( inst_scope );
    }
    return( sym );
}

SYMBOL TemplateSetFnMatchable( SYMBOL sym )
/*****************************************/
{
    DbgAssert( sym->u.alias == NULL && SymIsFunction( sym ) );
    sym->flag |= SF_TEMPLATE_FN;
    return( sym );
}

boolean TemplateUnboundSame( TYPE ub1, TYPE ub2 )
/***********************************************/
{
    SYMBOL ub1_curr;
    SYMBOL ub1_stop;
    SYMBOL ub2_curr;
    SYMBOL ub2_stop;
    CLASSINFO *ub1_info;
    CLASSINFO *ub2_info;
    type_flag uf1;
    type_flag uf2;
    SCOPE ub1_parm_scope;
    SCOPE ub2_parm_scope;

    uf1 = ub1->flag;
    uf2 = ub2->flag;
    if( uf1 != uf2 || ( uf1 & TF1_UNBOUND ) == 0 ) {
        return( FALSE );
    }
    DbgAssert( ub1->of == NULL && ub2->of == NULL );
    ub1_info = ub1->u.c.info;
    ub2_info = ub2->u.c.info;
    if( ub1_info->name != ub2_info->name ) {
        return( FALSE );
    }
    /* both are instantiations of the same class */
    ub1_parm_scope = TemplateClassParmScope( ub1 );
    ub2_parm_scope = TemplateClassParmScope( ub2 );
    if( ub1_parm_scope == NULL || ub2_parm_scope == NULL ) {
        return( FALSE );
    }
    ub1_curr = NULL;
    ub2_curr = NULL;
    ub1_stop = ScopeOrderedStart( ub1_parm_scope );
    ub2_stop = ScopeOrderedStart( ub2_parm_scope );
    for(;;) {
        ub1_curr = ScopeOrderedNext( ub1_stop, ub1_curr );
        ub2_curr = ScopeOrderedNext( ub2_stop, ub2_curr );
        if( ub1_curr == NULL ) break;
        if( ub2_curr == NULL ) break;
        DbgAssert( ub1_curr->id == ub2_curr->id );
        if( ub1_curr->id != SC_TYPEDEF ) {
            if( ! TemplateParmEqual( ub1_curr, ub2_curr ) ) {
                return( FALSE );
            }
        } else {
            if( ! TypesIdentical( ub1_curr->sym_type, ub2_curr->sym_type ) ) {
                return( FALSE );
            }
        }
    }
    DbgAssert( ub1_curr == NULL && ub2_curr == NULL );
    return( TRUE );
}

TEMPLATE_INFO *TemplateClassInfoGetIndex( TEMPLATE_INFO *e )
{
    return( CarveGetIndex( carveTEMPLATE_INFO, e ) );
}

TEMPLATE_INFO *TemplateClassInfoMapIndex( TEMPLATE_INFO *e )
{
    return( CarveMapIndex( carveTEMPLATE_INFO, e ) );
}

FN_TEMPLATE_DEFN *TemplateFunctionInfoGetIndex( FN_TEMPLATE_DEFN *e )
{
    return( CarveGetIndex( carveFN_TEMPLATE_DEFN, e ) );
}

FN_TEMPLATE_DEFN *TemplateFunctionInfoMapIndex( FN_TEMPLATE_DEFN *e )
{
    return( CarveMapIndex( carveFN_TEMPLATE_DEFN, e ) );
}

static void markFreeTemplateInfo( void *p )
{
    TEMPLATE_INFO *s = p;

    s->free = TRUE;
}

static void markFreeClassInst( void *p )
{
    CLASS_INST *s = p;

    s->free = TRUE;
}

static void markFreeFnTemplateDefn( void *p )
{
    FN_TEMPLATE_DEFN *s = p;

    s->type_list = NULL;
}

static void saveTemplateInfo( void *p, carve_walk_base *d )
{
    TEMPLATE_INFO *s = p;
    TEMPLATE_INFO *save_next;
    REWRITE *save_defn;
    CLASS_INST *save_instantiations;
    TYPE save_unbound_type;
    SYMBOL save_sym;
    TEMPLATE_MEMBER *member;
    void *nti;
    unsigned i;
    auto void *member_buff[2];

    if( s->free ) {
        return;
    }
    save_next = s->next;
    s->next = CarveGetIndex( carveTEMPLATE_INFO, save_next );
    save_defn = s->defn;
    s->defn = RewriteGetIndex( save_defn );
    save_instantiations = s->instantiations;
    s->instantiations = CarveGetIndex( carveCLASS_INST, save_instantiations );
    save_unbound_type = s->unbound_type;
    s->unbound_type= TypeGetIndex( save_unbound_type );
    save_sym = s->sym;
    s->sym= SymbolGetIndex( save_sym );
    PCHWriteCVIndex( d->index );
    PCHWrite( s, sizeof( *s ) );
    for( i = 0; i < s->num_args; ++i ) {
        nti = NameGetIndex( s->arg_names[i] );
        PCHWrite( &nti, sizeof( nti ) );
    }
    for( i = 0; i < s->num_args; ++i ) {
        nti = TypeGetIndex( s->type_list[i] );
        PCHWrite( &nti, sizeof( nti ) );
    }
    RingIterBeg( s->member_defns, member ){
        member_buff[0] = RewriteGetIndex( member->defn );
        if( member->arg_names != s->arg_names ) {
            member_buff[1] = s->arg_names;
            PCHWrite( member_buff, sizeof( member_buff ) );
            for( i = 0; i < s->num_args; ++i ) {
                nti = NameGetIndex( member->arg_names[i] );
                PCHWrite( &nti, sizeof( nti ) );
            }
        } else {
            member_buff[1] = NULL;
            PCHWrite( member_buff, sizeof( member_buff ) );
        }
    } RingIterEnd( member )
    member_buff[0] = NULL;
    member_buff[1] = NULL;
    PCHWrite( member_buff, sizeof( member_buff ) );
    s->next = save_next;
    s->defn = save_defn;
    s->instantiations = save_instantiations;
    s->unbound_type = save_unbound_type;
    s->sym = save_sym;
}

static void saveClassInst( void *p, carve_walk_base *d )
{
    CLASS_INST *s = p;
    CLASS_INST *save_next;
    SCOPE save_scope;
    SRCFILE save_locn_src_file;

    if( s->free ) {
        return;
    }
    save_next = s->next;
    s->next = CarveGetIndex( carveCLASS_INST, save_next );
    save_scope = s->scope;
    s->scope = ScopeGetIndex( save_scope );
    save_locn_src_file = s->locn.src_file;
    s->locn.src_file = SrcFileGetIndex( save_locn_src_file );
    PCHWriteCVIndex( d->index );
    PCHWrite( s, sizeof( *s ) );
    s->next = save_next;
    s->scope = save_scope;
    s->locn.src_file = save_locn_src_file;
}

static void saveFnTemplateDefn( void *p, carve_walk_base *d )
{
    unsigned i;
    void *nti;
    FN_TEMPLATE_DEFN *s = p;
    FN_TEMPLATE_DEFN *save_next;
    SYMBOL save_sym;
    REWRITE *save_defn;

    if( s->type_list == NULL ) {
        return;
    }
    save_next = s->next;
    s->next = CarveGetIndex( carveFN_TEMPLATE_DEFN, save_next );
    save_sym = s->sym;
    s->sym = SymbolGetIndex( save_sym );
    save_defn = s->defn;
    s->defn = RewriteGetIndex( save_defn );
    PCHWriteCVIndex( d->index );
    PCHWrite( s, sizeof( *s ) );
    for( i = 0; i < s->num_args; ++i ) {
        nti = NameGetIndex( s->arg_names[i] );
        PCHWrite( &nti, sizeof( nti ) );
    }
    for( i = 0; i < s->num_args; ++i ) {
        nti = TypeGetIndex( s->type_list[i] );
        PCHWrite( &nti, sizeof( nti ) );
    }
    s->next = save_next;
    s->sym = save_sym;
    s->defn = save_defn;
}

pch_status PCHWriteTemplates( void )
{
    cv_index terminator = CARVE_NULL_INDEX;
    TEMPLATE_INFO *all_class_templates;
    FN_TEMPLATE_DEFN *all_function_templates;
    auto carve_walk_base data;

    PCHWrite( &templateData.max_depth, sizeof( templateData.max_depth ) );
    all_class_templates = TemplateClassInfoGetIndex( allClassTemplates );
    PCHWrite( &all_class_templates, sizeof( all_class_templates ) );
    all_function_templates = TemplateFunctionInfoGetIndex( allFunctionTemplates );
    PCHWrite( &all_function_templates, sizeof( all_function_templates ) );
    CarveWalkAllFree( carveCLASS_INST, markFreeClassInst );
    CarveWalkAll( carveCLASS_INST, saveClassInst, &data );
    PCHWriteCVIndex( terminator );
    CarveWalkAllFree( carveFN_TEMPLATE_DEFN, markFreeFnTemplateDefn );
    CarveWalkAll( carveFN_TEMPLATE_DEFN, saveFnTemplateDefn, &data );
    PCHWriteCVIndex( terminator );
    CarveWalkAllFree( carveTEMPLATE_INFO, markFreeTemplateInfo );
    CarveWalkAll( carveTEMPLATE_INFO, saveTemplateInfo, &data );
    PCHWriteCVIndex( terminator );
    return( PCHCB_OK );
}

pch_status PCHReadTemplates( void )
{
    cv_index i;
    unsigned j;
    size_t arg_names_size;
    size_t type_list_size;
    char **arg_names;
    TYPE *type_list;
    CLASS_INST *ci;
    FN_TEMPLATE_DEFN *ftd;
    TEMPLATE_INFO *ti;
    REWRITE *memb_defn;
    char **memb_arg_names;
    auto void *member_buff[2];
    auto cvinit_t data;

    PCHRead( &templateData.max_depth, sizeof( templateData.max_depth ) );
    PCHRead( &allClassTemplates, sizeof( allClassTemplates ) );
    allClassTemplates = TemplateClassInfoMapIndex( allClassTemplates );
    PCHRead( &allFunctionTemplates, sizeof( allFunctionTemplates ) );
    allFunctionTemplates = TemplateFunctionInfoMapIndex( allFunctionTemplates );
    CarveInitStart( carveCLASS_INST, &data );
    for(;;) {
        i = PCHReadCVIndex();
        if( i == CARVE_NULL_INDEX ) break;
        ci = CarveInitElement( &data, i );
        PCHRead( ci, sizeof( *ci ) );
        ci->next = CarveMapIndex( carveCLASS_INST, ci->next );
        ci->scope = ScopeMapIndex( ci->scope );
        ci->locn.src_file = SrcFileMapIndex( ci->locn.src_file );
    }
    CarveInitStart( carveFN_TEMPLATE_DEFN, &data );
    for(;;) {
        i = PCHReadCVIndex();
        if( i == CARVE_NULL_INDEX ) break;
        ftd = CarveInitElement( &data, i );
        PCHRead( ftd, sizeof( *ftd ) );
        ftd->next = CarveMapIndex( carveFN_TEMPLATE_DEFN, ftd->next );
        ftd->sym = SymbolMapIndex( ftd->sym );
        ftd->defn = RewriteMapIndex( ftd->defn );
        arg_names_size = ftd->num_args * sizeof( char * );
        arg_names = CPermAlloc( arg_names_size );
        ftd->arg_names = arg_names;
        type_list_size = ftd->num_args * sizeof( TYPE );
        type_list = CPermAlloc( type_list_size );
        ftd->type_list = type_list;
        PCHRead( arg_names, arg_names_size );
        PCHRead( type_list, type_list_size );
        for( j = 0; j < ftd->num_args; ++j ) {
            arg_names[j] = NameMapIndex( arg_names[j] );
            type_list[j] = TypeMapIndex( type_list[j] );
        }
    }
    CarveInitStart( carveTEMPLATE_INFO, &data );
    for(;;) {
        i = PCHReadCVIndex();
        if( i == CARVE_NULL_INDEX ) break;
        ti = CarveInitElement( &data, i );
        PCHRead( ti, sizeof( *ti ) );
        ti->next = CarveMapIndex( carveTEMPLATE_INFO, ti->next );
        ti->defn = RewriteMapIndex( ti->defn );
        ti->instantiations = CarveMapIndex( carveCLASS_INST, ti->instantiations );
        ti->member_defns = NULL;
        ti->unbound_type= TypeMapIndex( ti->unbound_type );
        ti->sym= SymbolMapIndex( ti->sym );
        arg_names_size = ti->num_args * sizeof( char * );
        arg_names = CPermAlloc( arg_names_size );
        ti->arg_names = arg_names;
        type_list_size = ti->num_args * sizeof( TYPE );
        type_list = CPermAlloc( type_list_size );
        ti->type_list = type_list;
        PCHRead( arg_names, arg_names_size );
        PCHRead( type_list, type_list_size );
        for( j = 0; j < ti->num_args; ++j ) {
            arg_names[j] = NameMapIndex( arg_names[j] );
            type_list[j] = TypeMapIndex( type_list[j] );
        }
        for(;;) {
            PCHRead( member_buff, sizeof( member_buff ) );
            if( member_buff[0] == NULL ) break;
            memb_defn = RewriteMapIndex( member_buff[0] );
            if( member_buff[1] != NULL ) {
                memb_arg_names = CPermAlloc( arg_names_size );
                member_buff[1] = memb_arg_names;
                PCHRead( memb_arg_names, arg_names_size );
                for( j = 0; j < ti->num_args; ++j ) {
                    memb_arg_names[j] = NameMapIndex( memb_arg_names[j] );
                }
            } else {
                memb_arg_names = arg_names;
            }
            addMemberEntry( ti, memb_defn, memb_arg_names );
        }
    }
    return( PCHCB_OK );
}

pch_status PCHInitTemplates( boolean writing )
{
    cv_index n;

    if( writing ) {
        n = CarveLastValidIndex( carveCLASS_INST );
        PCHWriteCVIndex( n );
        n = CarveLastValidIndex( carveFN_TEMPLATE_DEFN );
        PCHWriteCVIndex( n );
        n = CarveLastValidIndex( carveTEMPLATE_MEMBER );
        PCHWriteCVIndex( n );
        n = CarveLastValidIndex( carveTEMPLATE_INFO );
        PCHWriteCVIndex( n );
    } else {
        carveCLASS_INST = CarveRestart( carveCLASS_INST );
        n = PCHReadCVIndex();
        CarveMapOptimize( carveCLASS_INST, n );
        carveFN_TEMPLATE_DEFN = CarveRestart( carveFN_TEMPLATE_DEFN );
        n = PCHReadCVIndex();
        CarveMapOptimize( carveFN_TEMPLATE_DEFN, n );
        carveTEMPLATE_MEMBER = CarveRestart( carveTEMPLATE_MEMBER );
        n = PCHReadCVIndex();
        CarveMapOptimize( carveTEMPLATE_MEMBER, n );
        carveTEMPLATE_INFO = CarveRestart( carveTEMPLATE_INFO );
        n = PCHReadCVIndex();
        CarveMapOptimize( carveTEMPLATE_INFO, n );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniTemplates( boolean writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveCLASS_INST );
        CarveMapUnoptimize( carveFN_TEMPLATE_DEFN );
        CarveMapUnoptimize( carveTEMPLATE_MEMBER );
        CarveMapUnoptimize( carveTEMPLATE_INFO );
    }
    return( PCHCB_OK );
}
