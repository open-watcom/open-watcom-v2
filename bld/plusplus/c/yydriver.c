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


#include <stdlib.h>
#include <string.h>

#include "plusplus.h"
#include "errdefns.h"
#include "ytab.gh"
#include "preproc.h"
#include "fnbody.h"
#include "ptree.h"
#include "enum.h"
#include "class.h"
#include "ring.h"
#include "stack.h"
#include "decl.h"
#include "carve.h"
#include "rewrite.h"
#include "toggle.h"
#include "ppops.h"
#include "gstack.h"
#include "vstk.h"
#include "yydriver.h"
#include "initdefs.h"
#include "pcheader.h"
#include "stats.h"
#include "codegen.h"
#include "namspace.h"
#ifndef NDEBUG
#include "pragdefn.h"
#endif

ExtraRptCtr( lookup_lexical );
ExtraRptCtr( lookup_other );
ExtraRptCtr( found_type );
ExtraRptCtr( found_template );
ExtraRptCtr( found_namespace );
ExtraRptCtr( found_id );

// YY* definitions
#define YYFAR
#ifndef NDEBUG
    #define YYDEBUG
#endif

typedef uint_16         YYACTIONBASETYPE;
typedef uint_16         YYACTIONTYPE;
typedef uint_16         YYBITBASETYPE;
typedef uint_8          YYBITTYPE;
typedef uint_8          YYPLENTYPE;
typedef uint_16         YYPLHSTYPE;

typedef unsigned        YYTOKENTYPE;

typedef union {
    unsigned                    flags;
    YYTOKENTYPE                 token;
    TYPE                        type;
    BASE_CLASS                  *base;
    DECL_SPEC                   *dspec;
    DECL_INFO                   *dinfo;
    REWRITE                     *rewrite;
    SYMBOL_NAME                 sym;
    PTREE                       tree;
} YYSTYPE;

enum {
    RAW_REDUCTION       = 0x8000,
    RAW_UNIT_REDUCTION  = 0x4000,
    RAW_MASK            = 0x3fff,
};

#define STACK_DEPTH     256

#define BLOCK_RESTART_PARSE     8
#define BLOCK_VALUE_STACK       4
#define BLOCK_STATE_STACK       4
#define BLOCK_LOCATION_STACK    4

static carve_t carveRESTART_PARSE;
static carve_t carveVALUE_STACK;
static carve_t carveSTATE_STACK;
static carve_t carveLOCATION_STACK;

typedef struct restart_parse RESTART_PARSE;
typedef struct parse_stack PARSE_STACK;
struct restart_parse {
    RESTART_PARSE       *next;
    PARSE_STACK         *state;
    YYACTIONTYPE        *ssp;
    GLOBAL_STACK        *gstack;
    SCOPE               reset_scope;
};

struct parse_stack {
    PARSE_STACK         *next;
    RESTART_PARSE       *restart;
    TOKEN_LOCN          *lsp;
    TOKEN_LOCN          *lstack;
    YYSTYPE             *vsp;
    YYSTYPE             *vstack;
    YYACTIONTYPE        *ssp;
    YYACTIONTYPE        *sstack;
    YYACTIONTYPE        *exhaust;
    GLOBAL_STACK        *gstack;
    void                *qualifications;
    SCOPE               reset_scope;
    TYPE                class_colon;
    VSTK_CTL            look_ahead_storage;
    unsigned            look_ahead_count;
    unsigned            look_ahead_index;
    TOKEN_LOCN          template_record_locn;
    REWRITE             *template_record_tokens;
    unsigned            no_super_tokens : 1;
    unsigned            use_saved_tokens : 1;
    unsigned            favour_reduce : 1;
    unsigned            favour_shift : 1;
    unsigned            look_ahead_stack : 1;
    unsigned            look_ahead_active : 1;
    unsigned            template_decl : 1;
    unsigned            template_args : 1;
    unsigned            template_class_inst_defer : 1;
    unsigned            special_colon_colon : 1;
};

typedef struct {
    YYSTYPE             yylval;
    TOKEN_LOCN          yylocation;
    YYTOKENTYPE         yytok;
} look_ahead_storage;

static YYSTYPE yylval;
static TOKEN_LOCN yylocation;
static YYTOKENTYPE currToken;
static PARSE_STACK *currParseStack;
static SUICIDE_CALLBACK parserSuicide;

typedef enum {
    P_RELEX,            /* an external parse has occurred; so resynchronize */
    P_SHIFT,            /* automaton shifted a token */
    P_ACCEPT,           /* parse is completed */
    P_SPECIAL,          /* special actions follow ... */
    P_CLASS_TEMPLATE,   /* class template member found */
    P_DEFER_DEFN,       /* don't want to fully define the class instantiation */
    P_ERROR,            /* parse errors follow ... */
    P_SYNTAX,           /* parse cannot continue due to syntax error */
    P_OVERFLOW,         /* parse cannot continue due to parser stack overflow */
    P_DIAGNOSED,        /* parse cannot continue due to diagnosed errors */
    P_NULL
} p_action;

typedef enum {          /* lookahead actions */
    LA_UNDERFLOW,       /* reduce popped off the triggering context */
    LA_DISAMBIGUATE,    /* lookahead requires another disambiguation */
    LA_SYNTAX,          /* found a syntax error */
    LA_NULL,
    LA_MAX
} la_action;

#define LK_DEFS                                 /* lookup return value */ \
    LKDEF( LK_ID,       ID )                    /* identifier */ \
    LKDEF( LK_TYPE,     TYPE_NAME )             /* type name */ \
    LKDEF( LK_TEMPLATE, TEMPLATE_NAME )         /* template name */ \
    LKDEF( LK_NAMESPACE,NAMESPACE_NAME )        /* namespace name */

typedef enum {          /* lookup return value */
    #define LKDEF( e, t )       e,
    LK_DEFS
    #undef LKDEF
} lk_result;

typedef enum {
    LK_LEXICAL  = 0x01, /* lexical lookup required */
    LK_LT_AHEAD = 0x02, /* '<' token is after 'id' */
    LK_NULL     = 0
} lk_control;

enum {
    CH_ALREADY_STARTED  = 0x01,         /* C:: chain has already been started */
    CH_NULL             = 0x00
};

static const unsigned lookupToken[] = {
    #define LKDEF( e, t )       Y_##t,
    LK_DEFS
    #undef LKDEF
};

static const unsigned globalLookupToken[] = {
    #define LKDEF( e, t )       Y_GLOBAL_##t,
    LK_DEFS
    #undef LKDEF
};



#define LA_SHIFT_TOKEN                  Y_SHIFT_SPECIAL
#define LA_REDUCE_TOKEN                 Y_REDUCE_SPECIAL

#define YYACTION_IS_SHIFT( x )          ( (x) < YYUSED )

#define YYACTION_SHIFT_STATE( x )       (x)
#define YYACTION_REDUCE_RULE( x )       ( (x) + YYUSED )

#include "yylex.gh"

#ifndef NDEBUG
static void dump_rule( unsigned rule )
{
    unsigned i;
    const YYTOKENTYPE YYFAR *tok;
    const char YYFAR *p;

    for( p = yytoknames[ yyplhstab[ rule ] ]; *p; ++p ) {
        putchar( *p );
    }
    putchar( ' ' );
    putchar( '<' );
    putchar( '-' );
    tok = &yyrhstoks[ yyrulebase[ rule ] ];
    for( i = yyplentab[ rule ]; i != 0; --i ) {
        putchar( ' ' );
        for( p = yytoknames[ *tok ]; *p; ++p ) {
            putchar( *p );
        }
        ++tok;
    }
    putchar( '\n' );
}
#endif

static void deleteStack( PARSE_STACK * );

static void parserSuicideHandler( void )
{
    while( currParseStack != NULL ) {
        deleteStack( currParseStack );
    }
}

static void parseInit(          // PARSER INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    carveRESTART_PARSE = CarveCreate( sizeof( RESTART_PARSE ),
                                    BLOCK_RESTART_PARSE );
    carveVALUE_STACK = CarveCreate( STACK_DEPTH * sizeof( YYSTYPE ),
                                    BLOCK_VALUE_STACK );
    carveSTATE_STACK = CarveCreate( STACK_DEPTH * sizeof( YYACTIONTYPE ),
                                    BLOCK_STATE_STACK );
    carveLOCATION_STACK = CarveCreate( STACK_DEPTH * sizeof( TOKEN_LOCN ),
                                    BLOCK_LOCATION_STACK );
    currToken = Y_IMPOSSIBLE;
    parserSuicide.call_back = parserSuicideHandler;
    RegisterSuicideCallback( &parserSuicide );
    ExtraRptRegisterCtr( &lookup_lexical, "parser: lexical lookups" );
    ExtraRptRegisterCtr( &lookup_other, "parser: other lookups" );
    ExtraRptRegisterCtr( &found_type, "parser lookup: found type" );
    ExtraRptRegisterCtr( &found_id, "parser lookup: found id" );
    ExtraRptRegisterCtr( &found_template, "parser lookup: found template" );
    ExtraRptRegisterCtr( &found_namespace, "parser lookup: found namespace" );
}

static void parseFini(          // PARSER INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    ParseFlush();
#ifndef NDEBUG
    CarveVerifyAllGone( carveRESTART_PARSE, "RESTART_PARSE" );
    CarveVerifyAllGone( carveVALUE_STACK, "VALUE_STACK" );
    CarveVerifyAllGone( carveSTATE_STACK, "STATE_STACK" );
    CarveVerifyAllGone( carveLOCATION_STACK, "LOCATION_STACK" );
#endif
    CarveDestroy( carveRESTART_PARSE );
    CarveDestroy( carveVALUE_STACK );
    CarveDestroy( carveSTATE_STACK );
    CarveDestroy( carveLOCATION_STACK );
}

INITDEFN( parser, parseInit, parseFini )

static void syncLocation( void )
{
    if( currToken == Y_IMPOSSIBLE ) {
        SrcFileGetTokenLocn( &yylocation );
    }
}

static void fatalParserError( void )
{
    CErr1( ERR_PARSER_DIED );
    CSuicide();
}

static boolean doFnbodyRewrite( void )
{
    switch( ScopeId( CurrScope ) ) {
    case SCOPE_CLASS:
    case SCOPE_TEMPLATE_DECL:
        return( TRUE );
    }
    return( FALSE );
}

static PTREE setLocation( PTREE tree, TOKEN_LOCN *spot )
{
    tree->locn.src_file = spot->src_file;
    tree->locn.line = spot->line;
    tree->locn.column = spot->column;
    return( tree );
}

static PTREE makeUnary( CGOP op, PTREE t )
{
    t = PTreeUnary( op, t );
    setLocation( t, &yylocation );
    return( t );
}

static PTREE makeBinary( CGOP op, PTREE t1, PTREE t2 )
{
    t1 = PTreeBinary( op, t1, t2 );
    setLocation( t1, &yylocation );
    return( t1 );
}

static PTREE currBinary( CGOP op, PTREE t1, PTREE t2 )
{
    TOKEN_LOCN location;

    t1 = PTreeBinary( op, t1, t2 );
    SrcFileGetTokenLocn( &location );
    setLocation( t1, &location );
    return( t1 );
}

static PTREE makeId( void )
{
    PTREE t;

    t = PTreeId( SavedId );
    setLocation( t, &yylocation );
    return( t );
}

static lk_result lexCategory( SCOPE scope, PTREE id, lk_control control )
{
    char *name;
    SYMBOL_NAME sym_name;
    SYMBOL sym;
    TYPE type;
    TYPE class_type;

    name = id->u.id.name;
    if( control & LK_LEXICAL ) {
        ExtraRptIncrementCtr( lookup_lexical );
        sym_name = ScopeYYLexical( scope, name );
    } else {
        ExtraRptIncrementCtr( lookup_other );
        sym_name = ScopeYYMember( scope, name );
    }
    if( sym_name != NULL ) {
        if( sym_name->name_syms == NULL ) {
            sym = sym_name->name_type;
            switch( sym->id ) {
            case SC_CLASS_TEMPLATE:
                ExtraRptIncrementCtr( found_template );
                return( LK_TEMPLATE );
            case SC_NAMESPACE:
                ExtraRptIncrementCtr( found_namespace );
                return( LK_NAMESPACE );
            }
            type = sym->sym_type;
            if( control & LK_LT_AHEAD ) {
                class_type = StructType( type );
                if( class_type != NULL ) {
                    if( class_type->flag & TF1_INSTANTIATION ) {
                        /* make sure typedefs of instantiations are OK */
                        if( class_type->u.c.info->name == name ) {
                            ExtraRptIncrementCtr( found_template );
                            return( LK_TEMPLATE );
                        }
                    }
                }
            }
            id->type = type;
            ExtraRptIncrementCtr( found_type );
            return( LK_TYPE );
        }
    }
    ExtraRptIncrementCtr( found_id );
    return( LK_ID );
}

static int doId( void )
{
    PTREE id;
    lk_control control;
    lk_result id_check;

    id = makeId();
    yylval.tree = id;
    control = LK_LEXICAL;
    if( LAToken == T_LT ) {
        control |= LK_LT_AHEAD;
    }
    id_check = lexCategory( CurrScope, id, control );
    return( lookupToken[ id_check ] );
}

#define nextToken( yyl )        \
        NextToken(); \
        SrcFileGetTokenLocn( yyl );

static void nextRecordedToken( PARSE_STACK *state )
{
    REWRITE *r;

    nextToken( &yylocation );
    r = state->template_record_tokens;
    if( r != NULL ) {
        RewriteRecordToken( r, &(state->template_record_locn) );
    }
}

static void nextYYLexToken( PARSE_STACK *state )
{
    currToken = Y_IMPOSSIBLE;
    if( state->use_saved_tokens ) {
        state->look_ahead_index++;
        if( state->look_ahead_index != state->look_ahead_count ) {
            return;
        }
        if( ! state->look_ahead_active ) {
            while( state->look_ahead_count != 0 ) {
                VstkPop( &(state->look_ahead_storage) );
                state->look_ahead_count--;
            }
            state->look_ahead_index = 0;
        }
        state->use_saved_tokens = FALSE;
    }
    nextRecordedToken( state );
}

static TYPE findGenericType( SCOPE scope, char *name )
{
    TYPE generic_type;
    TYPE test_type;
    SYMBOL sym;
    SEARCH_RESULT *result;

    generic_type = NULL;
    result = ScopeFindNaked( scope, name );
    if( result != NULL ) {
        sym = result->sym_name->name_type;
        ScopeFreeResult( result );
        test_type = TypedefRemove( sym->sym_type );
        if( test_type->id == TYP_GENERIC ) {
            generic_type = sym->sym_type;
        }
    }
    return( generic_type );
}

static SCOPE checkColonColon( SCOPE scope, PTREE id, SCOPE not_nested )
{
    char *name;
    TYPE test_type;
    TYPE scope_type;
    TYPE class_type;
    TYPE id_type;
    SCOPE id_scope;
    SYMBOL sym;
    SEARCH_RESULT *result;

    scope_type = NULL;
    id_scope = NULL;
    name = id->u.id.name;
    if( not_nested != NULL ) {
        result = ScopeFindLexicalColonColon( scope, name );
    } else {
        result = ScopeFindMemberColonColon( scope, name );
    }
    if( result == NULL ) {
        if( not_nested != NULL ) {
            if( ScopeId( not_nested ) == SCOPE_TEMPLATE_DECL ) {
                scope_type = findGenericType( not_nested, name );
            } else {
                CErr2p( ERR_UNDECLARED_CLASS_SYM, name );
            }
        } else {
            CErr2p( ERR_UNDECLARED_NESTED_CLASS_SYM, name );
        }
    } else {
        sym = result->sym_name->name_type;
        if( SymIsNameSpace( sym ) ) {
            scope = sym->u.ns->scope;
            id_scope = scope;
        } else {
            test_type = TypedefRemove( sym->sym_type );
            if( test_type->id != TYP_CLASS ) {
                CErr2p( ERR_NAME_NOT_A_CLASS_OR_NAMESPACE, name );
            } else {
                scope_type = test_type;
                ScopeAmbiguousSymbol( result, sym );
            }
        }
        ScopeFreeResult( result );
    }
    id_type = scope_type;
    class_type = StructType( scope_type );
    if( class_type != NULL ) {
        // member pointers do not need the class to be defined
        if( StructOpened( class_type ) == NULL && CurToken != T_TIMES ) {
            CErr2p( ERR_CLASS_NOT_DEFINED, name );
            id_type = NULL;
        } else {
            scope = class_type->u.c.scope;
            id_scope = scope;
        }
    }
    id->type = id_type;
    id->u.id.scope = id_scope;
    return( scope );
}

static int scopedChain( PARSE_STACK *state, PTREE start, PTREE id, unsigned ctrl )
{
    lk_result id_check;
    char *name;
    SCOPE scope;
    SCOPE lexical_lookup;
    PTREE curr;
    TYPE class_type;

    scope = CurrScope;
    if( start != NULL ) {
        scope = FileScope;
        if( ctrl & CH_ALREADY_STARTED ) {
            class_type = start->u.subtree[1]->type;
            if( class_type != NULL ) {
                scope = class_type->u.c.scope;
                lexical_lookup = NULL;
            } else {
                lexical_lookup = scope;
            }
        } else {
            lexical_lookup = scope;
        }
    } else {
        lexical_lookup = scope;
    }
    id->u.id.scope = scope;
    yylval.tree = NULL;
    curr = start;
    for(;;) {
        /* CurToken == T_SAVED_ID && LAToken == T_COLON_COLON */
        curr = currBinary( CO_COLON_COLON, curr, id );
        nextRecordedToken( state );
        nextRecordedToken( state );
        scope = checkColonColon( scope, id, lexical_lookup );
        name = id->u.id.name;
        lexical_lookup = NULL;
        switch( CurToken ) {
        case T_ID:
            LookPastName();
            /* fall through */
        case T_SAVED_ID:
            id = makeId();
            if( LAToken == T_COLON_COLON ) {
                continue;
            }
            yylval.tree = makeBinary( CO_STORAGE, curr, id );
            /* kludge for constructor name */
            if( name == id->u.id.name && ScopeId( CurrScope ) == SCOPE_FILE ) {
                /* so S::S( T x ) {} works if T is a nested type */
                return( Y_SCOPED_ID );
            }
            id_check = lexCategory( scope, id, LK_NULL );
            switch( id_check ) {
            case LK_ID:
                return( Y_SCOPED_ID );
            case LK_TYPE:
                return( Y_SCOPED_TYPE_NAME );
            case LK_TEMPLATE:
                // until nested templates are allowed
                return( Y_SCOPED_ID );
            case LK_NAMESPACE:
                return( Y_SCOPED_NAMESPACE_NAME );
            DbgDefault( "unknown lexical category" );
            }
            return( Y_IMPOSSIBLE );
        case T_TILDE:
            yylval.tree = makeUnary( CO_TILDE, curr );
            return( Y_SCOPED_TILDE );
        case T_OPERATOR:
            yylval.tree = makeUnary( CO_OPERATOR, curr );
            return( Y_SCOPED_OPERATOR );
        case T_TIMES:
            yylval.tree = makeUnary( CO_INDIRECT, curr );
            return( Y_SCOPED_TIMES );
        default:
            PTreeFreeSubtrees( curr );
            CErr2p( ERR_COLON_COLON_SYNTAX, TokenString() );
            /* error! */
            return( Y_IMPOSSIBLE );
        }
    }
}

static int templateScopedChain( PARSE_STACK *state )
{
    PTREE scope_tree;
    PTREE curr;
    PTREE id;
    TYPE template_type;
    TYPE template_class_type;
    char *name;
    SCOPE scope;
    int adjusted_token;

    template_type = state->class_colon;
    name = SimpleTypeName( template_type );
    template_class_type = StructType( template_type );
    scope_tree = PTreeId( name );
    scope_tree->type = template_class_type;
    if( template_class_type != NULL ) {
        scope_tree->u.id.scope = template_class_type->u.c.scope;
    }
    curr = makeBinary( CO_COLON_COLON, NULL, scope_tree );
    nextRecordedToken( state );
    for(;;) {
        switch( CurToken ) {
        case T_ID:
            LookPastName();
            /* fall through */
        case T_SAVED_ID:
            if( ! state->template_decl ) {
                id = makeId();
                if( LAToken == T_COLON_COLON ) {
                    adjusted_token =
                        scopedChain( state, curr, id, CH_ALREADY_STARTED );
                    /* translate to the correct token value */
                    adjusted_token -= Y_SCOPED_ID;
                    adjusted_token += Y_TEMPLATE_SCOPED_ID;
                    return( adjusted_token );
                }
                yylval.tree = makeBinary( CO_STORAGE, curr, id );
                /* kludge for constructor name */
                if( name == id->u.id.name && ScopeEquivalent( CurrScope, SCOPE_FILE ) ) {
                    /* so S::S( T x ) {} works if T is a nested type */
                    return( Y_TEMPLATE_SCOPED_ID );
                }
                /* template instantiation errors may have occured */
                if( template_class_type != NULL ) {
                    scope = template_class_type->u.c.scope;
                    if( lexCategory( scope, id, LK_NULL ) != LK_ID ) {
                        return( Y_TEMPLATE_SCOPED_TYPE_NAME );
                    }
                }
                return( Y_TEMPLATE_SCOPED_ID );
            }
            if( LAToken == T_COLON_COLON ) {
                nextRecordedToken( state );
                nextRecordedToken( state );
                continue;
            }
            id = makeId();
            yylval.tree = makeBinary( CO_STORAGE, curr, id );
            /* only certain tokens can follow a file-scope declarator-id */
            /* int x(int); int (x)( int ); int x[2]; int x=2; int x; int x,y; */
            /* this heuristic only has to work up to the declarator-id */
            switch( LAToken ) {
            case T_LEFT_PAREN:
            case T_RIGHT_PAREN:
            case T_LEFT_BRACKET:
            case T_EQUAL:
            case T_SEMI_COLON:
            case T_COMMA:
                return( Y_TEMPLATE_SCOPED_ID );
            }
            return( Y_TEMPLATE_SCOPED_TYPE_NAME );
        case T_TILDE:
            yylval.tree = makeUnary( CO_TILDE, curr );
            return( Y_TEMPLATE_SCOPED_TILDE );
        case T_OPERATOR:
            yylval.tree = makeUnary( CO_OPERATOR, curr );
            return( Y_TEMPLATE_SCOPED_OPERATOR );
        case T_TIMES:
            yylval.tree = makeUnary( CO_INDIRECT, curr );
            return( Y_TEMPLATE_SCOPED_TIMES );
        default:
            PTreeFreeSubtrees( curr );
            CErr2p( ERR_COLON_COLON_SYNTAX, TokenString() );
            /* error! */
            return( Y_IMPOSSIBLE );
        }
    }
}

static int globalChain( PARSE_STACK *state )
{
    PTREE tree;
    PTREE id;
    lk_result id_check;

    tree = makeBinary( CO_COLON_COLON, NULL, NULL );
    nextRecordedToken( state );
    switch( CurToken ) {
    case T_ID:
        LookPastName();
        /* fall through */
    case T_SAVED_ID:
        id = makeId();
        if( LAToken == T_COLON_COLON ) {
            return( scopedChain( state, tree, id, CH_NULL ) );
        }
        yylval.tree = makeBinary( CO_STORAGE, tree, id );
        id_check = lexCategory( FileScope, id, LK_LEXICAL );
        return( globalLookupToken[ id_check ] );
    case T_OPERATOR:
        yylval.tree = makeUnary( CO_OPERATOR, tree );
        return( Y_GLOBAL_OPERATOR );
    case T_TILDE:
        yylval.tree = makeUnary( CO_TILDE, tree );
        return( Y_GLOBAL_TILDE );
    case T_NEW:
        yylval.tree = makeUnary( CO_NEW, tree );
        return( Y_GLOBAL_NEW );
    case T_DELETE:
        yylval.tree = makeUnary( CO_DELETE, tree );
        return( Y_GLOBAL_DELETE );
    }
    PTreeFreeSubtrees( tree );
    CErr2p( ERR_COLON_COLON_SYNTAX, TokenString() );
    /* error! */
    return( Y_IMPOSSIBLE );
}

int yylex( PARSE_STACK *state )
/*****************************/
{
    lk_result id_check;
    int token;
    STRING_CONSTANT literal;
    PTREE tree;
    look_ahead_storage *saved_token;
    struct {
        unsigned no_super_token : 1;
        unsigned special_colon_colon : 1;
    } flags;

    state->favour_reduce = FALSE;
    state->favour_shift = FALSE;
    if( state->use_saved_tokens ) {
        saved_token = VstkIndex( &(state->look_ahead_storage), state->look_ahead_index );
        yylval = saved_token->yylval;
        yylocation = saved_token->yylocation;
        token = saved_token->yytok;
        switch( token ) {
        case LA_SHIFT_TOKEN:
            state->favour_shift = TRUE;
            token = YYAMBIGT0;
            break;
        case LA_REDUCE_TOKEN:
            state->favour_reduce = TRUE;
            token = YYAMBIGT0;
            break;
        case Y_TYPE_NAME:
            // this is the only kind of id that can change in an ambiguity zone
            id_check = lexCategory( CurrScope, yylval.tree, LK_LEXICAL );
            if( id_check != LK_TYPE ) {
                yylval.tree->type = NULL;
                token = Y_ID;
            }
            break;
        }
        currToken = token;
        return( currToken );
    }
    flags.no_super_token = FALSE;
    flags.special_colon_colon = FALSE;
    if( state->no_super_tokens ) {
        state->no_super_tokens = FALSE;
        flags.no_super_token = TRUE;
    }
    if( state->special_colon_colon ) {
        state->special_colon_colon = FALSE;
        flags.special_colon_colon = TRUE;
    }
    token = currToken;
    if( token != Y_IMPOSSIBLE ) {
        return( token );
    }
    DbgZapMem( &yylval, 0xef, sizeof( yylval ) );
    switch( CurToken ) {
    case T_COLON_COLON:
        if( flags.no_super_token ) {
            token = yytranslate[ CurToken ];
        } else {
            if( flags.special_colon_colon ) {
                token = templateScopedChain( state );
            } else {
                token = globalChain( state );
            }
        }
        break;
    case T_ID:
        LookPastName();
        /* fall through */
    case T_SAVED_ID:
        if( LAToken == T_COLON_COLON && ! flags.no_super_token ) {
            token = scopedChain( state, NULL, makeId(), CH_NULL );
        } else {
            token = doId();
        }
        break;
    case T_STRING:
        literal = StringCreate( Buffer, TokenLen - 1 );
        yylval.tree = PTreeLiteral( literal );
        setLocation( yylval.tree, &yylocation );
        token = Y_STRING;
        break;
    case T_LSTRING:
        literal = StringCreate( Buffer, TokenLen - 1 );
        yylval.tree = PTreeLiteralWide( literal );
        setLocation( yylval.tree, &yylocation );
        token = Y_STRING;
        break;
    case T_CONSTANT:
        switch( ConstType ) {
        case TYP_FLOAT:
        case TYP_DOUBLE:
        case TYP_LONG_DOUBLE:
            tree = PTreeFloatingConstantStr( Buffer, TokenLen, ConstType );
            break;
        case TYP_SLONG64 :
        case TYP_ULONG64 :
            tree = PTreeInt64Constant( Constant64, ConstType );
            break;
        case TYP_CHAR :
            if( CompFlags.signed_char ) {
                tree = PTreeIntConstant( I32FetchTrunc( Constant64 ), ConstType );
                break;
            }
            // drops thru
        default:
            tree = PTreeIntConstant( U32Fetch( Constant64 ), ConstType );
            break;
        }
        yylval.tree = tree;
        setLocation( yylval.tree, &yylocation );
        token = Y_CONSTANT;
        break;
    case T_TRUE:
        token = Y_TRUE;
        yylval.tree = PTreeBoolConstant( 1 );
        setLocation( yylval.tree, &yylocation );
        break;
    case T_FALSE:
        token = Y_FALSE;
        yylval.tree = PTreeBoolConstant( 0 );
        setLocation( yylval.tree, &yylocation );
        break;
    default:
        token = yytranslate[ CurToken ];
    }
    currToken = token;
    return( token );
}

REWRITE *ParseGetRecordingInProgress( TOKEN_LOCN **plocn )
/********************************************************/
{
    REWRITE *r;

    r = currParseStack->template_record_tokens;
    currParseStack->template_record_tokens = NULL;
    if( plocn != NULL ) {
        *plocn = &(currParseStack->template_record_locn);
    }
    return( r );
}

static boolean tokenMakesPTREE( unsigned token )
{
    switch( token ) {
    case Y_TYPE_NAME:
    case Y_TEMPLATE_NAME:
    case Y_NAMESPACE_NAME:
    case Y_CONSTANT:
    case Y_STRING:
    case Y_ID:
    case Y_GLOBAL_ID:
    case Y_GLOBAL_TYPE_NAME:
    case Y_GLOBAL_TEMPLATE_NAME:
    case Y_GLOBAL_NAMESPACE_NAME:
    case Y_GLOBAL_OPERATOR:
    case Y_GLOBAL_TILDE:
    case Y_GLOBAL_NEW:
    case Y_GLOBAL_DELETE:
    case Y_SCOPED_ID:
    case Y_SCOPED_TYPE_NAME:
    case Y_SCOPED_TEMPLATE_NAME:
    case Y_SCOPED_NAMESPACE_NAME:
    case Y_SCOPED_OPERATOR:
    case Y_SCOPED_TILDE:
    case Y_SCOPED_TIMES:
    case Y_TEMPLATE_SCOPED_ID:
    case Y_TEMPLATE_SCOPED_TYPE_NAME:
    case Y_TEMPLATE_SCOPED_TEMPLATE_NAME:
    case Y_TEMPLATE_SCOPED_NAMESPACE_NAME:
    case Y_TEMPLATE_SCOPED_OPERATOR:
    case Y_TEMPLATE_SCOPED_TILDE:
    case Y_TEMPLATE_SCOPED_TIMES:
        return( TRUE );
    }
    return( FALSE );
}

void ParseFlush( void )
/*********************/
{
    if( tokenMakesPTREE( currToken ) ) {
#if 0
        // NYI: we have a problem when this triggers!
        switch( currToken ) {
        case Y_GLOBAL_ID:
        case Y_GLOBAL_TYPENAME:
        case Y_GLOBAL_TEMPLATE_NAME:
        case Y_GLOBAL_OPERATOR:
        case Y_GLOBAL_TILDE:
        case Y_GLOBAL_NEW:
        case Y_GLOBAL_DELETE:
        case Y_SCOPED_ID:
        case Y_SCOPED_TYPE_NAME:
        case Y_SCOPED_TEMPLATE_NAME:
        case Y_SCOPED_NAMESPACE_NAME:
        case Y_SCOPED_OPERATOR:
        case Y_SCOPED_TILDE:
        case Y_SCOPED_TIMES:
        case Y_TEMPLATE_SCOPED_ID:
        case Y_TEMPLATE_SCOPED_TYPE_NAME:
        case Y_TEMPLATE_SCOPED_TEMPLATE_NAME:
        case Y_TEMPLATE_SCOPED_NAMESPACE_NAME:
        case Y_TEMPLATE_SCOPED_OPERATOR:
        case Y_TEMPLATE_SCOPED_TILDE:
        case Y_TEMPLATE_SCOPED_TIMES:
            ++ErrCount;
            puts( "ParseFlush with mult-token" );
        }
#endif
        PTreeFreeSubtrees( yylval.tree );
        yylval.tree = NULL;
    }
    currToken = Y_IMPOSSIBLE;
}

static PTREE getMultiToken( void )
{
    PTREE tree;

    tree = NULL;
    if( tokenMakesPTREE( currToken ) ) {
        tree = yylval.tree;
        yylval.tree = NULL;
        currToken = Y_IMPOSSIBLE;
    } else {
        ParseFlush();
    }
    return( tree );
}

static DECL_SPEC *sendType( PTREE tree )
{
    TYPE type;
    DECL_SPEC *dspec;
    SCOPE scope;
    PTREE sub_tree;

    scope = NULL;
    if( tree->op == PT_BINARY ) {
        type = tree->u.subtree[1]->type;
        sub_tree = tree->u.subtree[0]->u.subtree[1];
        if( sub_tree != NULL ) {
            DbgAssert( sub_tree->op == PT_ID );
            scope = sub_tree->u.id.scope;
        } else {
            scope = FileScope;
        }
    } else {
        type = tree->type;
    }
    if( type == NULL ) {
        type = TypeError;
    }
    dspec = PTypeActualTypeName( type, tree );
    if( scope != NULL ) {
        dspec->scope = scope;
    }
    return( dspec );
}

static DECL_SPEC *sendClass( PTREE tree )
{
    DECL_SPEC *dspec;

    dspec = sendType( tree );
    if( StructType( dspec->partial ) == NULL ) {
        PTypeRelease( dspec );
        dspec = NULL;
    }
    return( dspec );
}

static void pushDefaultDeclSpec( PARSE_STACK *state )
/***************************************************/
{
    GLOBAL_STACK *top;

    top = GStackPush( &(state->gstack), GS_DECL_SPEC );
    top->u.dspec = PTypeDefault();
}

static void pushUserDeclSpec( PARSE_STACK *state, DECL_SPEC *dspec )
/******************************************************************/
{
    GLOBAL_STACK *top;

    top = GStackPush( &(state->gstack), GS_DECL_SPEC );
    top->u.dspec = dspec;
}

static void zapTemplateClassDeclSpec( PARSE_STACK *state )
/********************************************************/
{
#ifndef NDEBUG
    if( GStackType( state->gstack ) != GS_DECL_SPEC ) {
        CFatal( "incorrect function template declaration" );
    }
#endif
    PTypeClassInstantiationUndo( state->gstack->u.dspec );
}

static void pushClassData( PARSE_STACK *state, type_flag flags, CLASS_INIT extra,
                           TYPE class_mod_list )
/*******************************************************************************/
{
    GLOBAL_STACK **head;

    head = &(state->gstack);
    GStackPush( head, GS_CLASS_DATA );
    ClassPush( &((*head)->u.classdata) );
    if( state->template_decl ) {
        extra |= CLINIT_TEMPLATE_DECL;
    }
    ClassInitState( flags, extra, class_mod_list );
}

static void reuseGStack( PARSE_STACK *state, gstack_type id )
{
    GStackPop( &(state->gstack) );
    GStackPush( &(state->gstack), id );
}

static void setInitWithLocn( DECL_INFO *dinfo, PTREE init, TOKEN_LOCN *locn )
/***************************************************************************/
{
    dinfo->defarg_expr = init;
    TokenLocnAssign( dinfo->init_locn, *locn );
}

static void tryCtorStyleInit( PARSE_STACK *state, DECL_INFO *dinfo )
/******************************************************************/
{
    PTREE expr_list;

    expr_list = dinfo->defarg_expr;
    if( expr_list != NULL ) {
        dinfo->defarg_expr = NULL;
        GStackPush( &(state->gstack), GS_INIT_DATA );
        DataInitStart( &(state->gstack->u.initdata), dinfo );
        DataInitSimpleLocn( &(dinfo->init_locn) );
        DataInitConstructorParms( expr_list );
        GStackPop( &(state->gstack) );
    }
    FreeDeclInfo( dinfo );
}

static p_action doAction( YYTOKENTYPE, PARSE_STACK * );

static void initParseStacks( PARSE_STACK *stack, boolean look_ahead )
{
    /* get new stack */
    StackPush( &currParseStack, stack );
    if( look_ahead ) {
        stack->vstack = NULL;
        stack->vsp = NULL;
        stack->lstack = NULL;
        stack->lsp = NULL;
    } else {
        stack->lstack = CarveAlloc( carveLOCATION_STACK );
        stack->lsp = stack->lstack;
        stack->vstack = CarveAlloc( carveVALUE_STACK );
        stack->vsp = stack->vstack;
        stack->vsp->tree = NULL;
    }
    stack->sstack = CarveAlloc( carveSTATE_STACK );
    stack->ssp = stack->sstack;
    stack->exhaust = &(stack->sstack[STACK_DEPTH-1]);
    *(stack->ssp) = YYSTART;
}

static void commonInit( PARSE_STACK *stack )
{
    stack->restart = NULL;
    stack->gstack = NULL;
    stack->reset_scope = CurrScope;
    stack->qualifications = NULL;
    VstkOpen( &(stack->look_ahead_storage), sizeof(look_ahead_storage), 16 );
    stack->look_ahead_count = 0;
    stack->look_ahead_index = 0;
    stack->template_record_tokens = NULL;
    stack->class_colon = NULL;
    stack->no_super_tokens = FALSE;
    stack->use_saved_tokens = FALSE;
    stack->favour_reduce = FALSE;
    stack->favour_shift = FALSE;
    stack->look_ahead_stack = FALSE;
    stack->look_ahead_active = FALSE;
    stack->template_decl = FALSE;
    stack->template_args = FALSE;
    stack->template_class_inst_defer = FALSE;
    stack->special_colon_colon = FALSE;
}

static void restartInit( PARSE_STACK *stack )
{
    stack->template_decl = FALSE;
    DbgAssert( stack->qualifications == NULL );
    DbgAssert( stack->look_ahead_count == 0 );
    DbgAssert( stack->look_ahead_index == 0 );
    DbgAssert( stack->template_record_tokens == NULL );
    DbgAssert( stack->no_super_tokens == FALSE );
    DbgAssert( stack->use_saved_tokens == FALSE );
    DbgAssert( stack->favour_reduce == FALSE );
    DbgAssert( stack->favour_shift == FALSE );
    DbgAssert( stack->look_ahead_stack == FALSE );
    DbgAssert( stack->look_ahead_active == FALSE );
    DbgAssert( stack->template_decl == FALSE );
    DbgAssert( stack->template_args == FALSE );
    DbgAssert( stack->template_class_inst_defer == FALSE );
    DbgAssert( stack->special_colon_colon == FALSE );
}

static void newLookAheadStack( PARSE_STACK *stack, PARSE_STACK *prev_stack )
{
    initParseStacks( stack, TRUE );
    /* initialize */
    commonInit( stack );
    /* grab top two states from previous stack */
    stack->look_ahead_stack = TRUE;
    stack->ssp[0] = prev_stack->ssp[-1];
    stack->ssp++;
    stack->ssp[0] = prev_stack->ssp[0];
}

static void newExprStack( PARSE_STACK *stack, YYTOKENTYPE tok )
{
    initParseStacks( stack, FALSE );
    /* initialize */
    commonInit( stack );
    /* go to correct state */
    doAction( tok, stack );
}

static void newExceptionStack( PARSE_STACK *stack )
{
    initParseStacks( stack, FALSE );
    /* initialize */
    commonInit( stack );
    /* go to correct state */
    doAction( Y_EXCEPTION_SPECIAL, stack );
}

static void newClassInstStack( PARSE_STACK *stack )
{
    initParseStacks( stack, FALSE );
    /* initialize */
    commonInit( stack );
    /* go to correct state */
    doAction( Y_CLASS_INST_SPECIAL, stack );
}

static void newDeclStack( PARSE_STACK *stack )
{
    initParseStacks( stack, FALSE );
    /* initialize */
    commonInit( stack );
}

static void newClassMemberInstStack( PARSE_STACK *stack )
{
    newDeclStack( stack );
}

static void deleteStack( PARSE_STACK *stack )
{
    PARSE_STACK *check_stack;

    if( stack->vstack ) {
        CarveFree( carveVALUE_STACK, stack->vstack );
    }
    if( stack->sstack ) {
        CarveFree( carveSTATE_STACK, stack->sstack );
    }
    if( stack->lstack ) {
        CarveFree( carveLOCATION_STACK, stack->lstack );
    }
    // remove qualifications before gstack starts being popped because
    // some gstack pop routines can be confused by qualifications
    while( stack->qualifications != NULL ) {
        ScopeQualifyPop();
    }
    while( stack->restart != NULL ) {
        doPopRestartDecl( stack );
    }
    while( stack->gstack != NULL ) {
        GStackPop( &(stack->gstack) );
    }
    VstkClose( &(stack->look_ahead_storage) );
    if( stack->template_record_tokens != NULL ) {
        RewriteFree( stack->template_record_tokens );
    }
    CurrScope = stack->reset_scope;
    check_stack = StackPop( &currParseStack );
#ifndef NDEBUG
    if( check_stack != stack ) {
        CFatal( "incorrect nesting of parse stacks" );
    }
#endif
}

static void pushRestartDecl( PARSE_STACK *state )
{
    RESTART_PARSE *restart;

    restart = CarveAlloc( carveRESTART_PARSE );
    StackPush( &(state->restart), restart );
    restart->state = state;
    restart->ssp = state->ssp;
    restart->gstack = state->gstack;
    restart->reset_scope = CurrScope;
}

static void doPopRestartDecl( PARSE_STACK *state )
{
    RESTART_PARSE *restart;

    restart = StackPop( &(state->restart) );
    CarveFree( carveRESTART_PARSE, restart );
}

#define restartDeclOK( restart ) \
    DbgAssert( restart != NULL ); \
    DbgAssert( restart->ssp >= restart->state->sstack ); \
    DbgAssert( restart->ssp < restart->state->exhaust );

static void popRestartDecl( PARSE_STACK *state )
{
    DbgStmt( RESTART_PARSE *restart );

    DbgStmt( restart = state->restart );
    restartDeclOK( restart );
    DbgAssert( restart->gstack == restart->state->gstack );
    DbgAssert( restart->reset_scope == CurrScope );
    doPopRestartDecl( state );
}

static void syncToRestart( PARSE_STACK *state )
{
    unsigned pop_amount;
    RESTART_PARSE *restart;
    YYACTIONTYPE *state_ssp;
    YYACTIONTYPE *restart_ssp;
    GLOBAL_STACK *restart_gstack;
    #if 0
    PARSE_STACK         *state;
    YYACTIONTYPE        *ssp;
    GLOBAL_STACK        *gstack;
    SCOPE               reset_scope;
    #endif

    restart = state->restart;
    restartDeclOK( restart );
    restart_ssp = restart->ssp;
    state_ssp = state->ssp;
    if( restart_ssp <= state_ssp ) {
        restart_gstack = restart->gstack;
        while( state->gstack != restart_gstack ) {
            GStackPop( &(state->gstack) );
        }
        pop_amount = state_ssp - restart_ssp;
        if( pop_amount != 0 ) {
            state->ssp = restart_ssp;
            state->vsp -= pop_amount;
            state->lsp -= pop_amount;
        }
        DbgAssert( restart->gstack == state->gstack );
        DbgAssert( restart->reset_scope == CurrScope );
        restartInit( state );
    } else {
        fatalParserError();
    }
}

static void setNoSuperTokens( PARSE_STACK *state )
{
    state->no_super_tokens = TRUE;
}

static void setWatchColonColon( PARSE_STACK *state, DECL_SPEC *dspec )
{
    state->class_colon = dspec->partial;
    state->special_colon_colon = TRUE;
}

SYMBOL ParseCurrFunction( void )
/******************************/
{
    PARSE_STACK *parse_stk;
    GLOBAL_STACK *gstk;
    DECL_INFO *dinfo;

    for( parse_stk = currParseStack; parse_stk != NULL; parse_stk = parse_stk->next ) {
        for( gstk = parse_stk->gstack; gstk != NULL; gstk = gstk->next ) {
            if( gstk->id == GS_DECL_INFO ) {
                dinfo = gstk->u.dinfo;
                if( dinfo != NULL && dinfo->fn_defn ) {
                    return( dinfo->sym );
                }
            }
        }
    }
    return( NULL );
}

void ParsePushQualification( void *elt )
/**************************************/
{
    StackPush( &(currParseStack->qualifications), elt );
}

void *ParsePopQualification( void )
/*********************************/
{
    void *elt;

    elt = StackPop( &(currParseStack->qualifications) );
    return( elt );
}

void *ParseCurrQualification( void )
/**********************************/
{
    void *elt;

    elt = NULL;
    if( currParseStack != NULL ) {
        elt = currParseStack->qualifications;
    }
    return( elt );
}

static YYACTIONTYPE GOTOYYAction( PARSE_STACK *state, unsigned rule )
{
    YYACTIONTYPE *ssp;
    YYTOKENTYPE lhs;
    unsigned top_state;
    unsigned raw_action;

    ssp = state->ssp;
    top_state = ssp[0];
    lhs = yyplhstab[ rule ];
    raw_action = yyaction[ lhs + yygotobase[ top_state ] ];
    return( raw_action );
}
#if 0
// doAction contains variables called 'yyaction' so this can't be used
// plus it slowed down the compiler at one point in time
#define GOTOYYAction( state, rule ) \
        ( yyaction[ yyplhstab[(rule)] + yygotobase[ (state)->ssp[0] ] ] )
#endif

static p_action normalYYAction( YYTOKENTYPE t, PARSE_STACK *state, unsigned *pa )
{
    YYACTIONTYPE *ssp;
    YYTOKENTYPE lhs;
    unsigned top_state;
    unsigned bit_index;
    unsigned raw_action;
    unsigned rule;
    unsigned mask;

    ssp = state->ssp;
    top_state = ssp[0];
    bit_index = ( t >> 3 );
    mask = 1 << ( t & 0x07 );
    for(;;) {
        if( yybitcheck[ bit_index + yybitbase[ top_state ] ] & mask ) {
            raw_action = yyaction[ t + yyactionbase[ top_state ] ];
            if(( raw_action & RAW_REDUCTION ) == 0 ) {
                /* we have a shift */
                *pa = raw_action;
                return( P_NULL );
            }
        } else {
            raw_action = yydefaction[ top_state ];
            if( raw_action == YYNOACTION ) {
                return( P_SYNTAX );
            }
        }
        /* we have a reduction */
        rule = raw_action & RAW_MASK;
        if(( raw_action & RAW_UNIT_REDUCTION ) == 0 ) {
            *pa = rule + YYUSED;
            return( P_NULL );
        }
        /* we have a unit reduction */
        lhs = yyplhstab[ rule ];
        top_state = yyaction[ lhs + yygotobase[ ssp[-1] ] ];
        ssp[0] = top_state;
    }
}

static void pushOperatorQualification( PTREE tree )
{
    PTREE scope_tree;
    TYPE class_type;

    scope_tree = tree->u.subtree[0]->u.subtree[1];
    class_type = StructType( scope_tree->type );
    if( class_type != NULL ) {
        ScopeQualifyPush( class_type->u.c.scope, CurrScope );
    }
}

static void lookAheadShift( PARSE_STACK *state, YYACTIONTYPE new_state, YYTOKENTYPE t )
{
#ifndef NDEBUG
    if( PragDbgToggle.dump_parse ) {
        puts( yytoknames[ t ] );
    }
#else
    t = t;
#endif
    /* NYI: should push S::operator qualification but we can't pop it yet */
    state->ssp++;
    *(state->ssp) = new_state;
}

static la_action lookAheadReduce( PARSE_STACK *state, YYACTIONTYPE new_rule )
{
    YYACTIONTYPE yy_action;

#ifndef NDEBUG
    if( PragDbgToggle.dump_parse ) {
        dump_rule( new_rule );
    }
#endif
    state->ssp -= yyplentab[ new_rule ];
    if( state->ssp < state->sstack ) {
        return( LA_UNDERFLOW );
    }
    yy_action = GOTOYYAction( state, new_rule );
    state->ssp++;
    *(state->ssp) = yy_action;
    return( LA_NULL );
}

static la_action lookAheadUseYYAction( PARSE_STACK *state
                                     , YYTOKENTYPE t
                                     , YYACTIONTYPE yyaction )
{
    if( YYACTION_IS_SHIFT( yyaction ) ) {
        lookAheadShift( state, yyaction, t );
        return( LA_NULL );
    }
    return( lookAheadReduce( state, yyaction - YYUSED ) );
}

static la_action lookAheadShiftReduce( YYTOKENTYPE t
                                     , PARSE_STACK *state
                                     , PARSE_STACK *host )
{
    la_action what;
    unsigned yyaction;
    unsigned yyk;

    for(;;) {
        yyk = *(state->ssp);
        if( yyk == YYAMBIGS0 && t == YYAMBIGT0 ) {
            if( host->favour_shift ) {
                yyaction = YYACTION_SHIFT_STATE( YYAMBIGH0 );
            } else if( host->favour_reduce ) {
                yyaction = YYACTION_REDUCE_RULE( YYAMBIGR0 );
            } else {
                what = LA_DISAMBIGUATE;
                break;
            }
        } else {
            if( normalYYAction( t, state, &yyaction ) == P_SYNTAX ) {
                what = LA_SYNTAX;
                break;
            }
        }
        if( YYACTION_IS_SHIFT( yyaction ) ) {
            lookAheadShift( state, yyaction, t );
            what = LA_NULL;
            break;
        }
        what = lookAheadReduce( state, yyaction - YYUSED );
        if( what == LA_UNDERFLOW ) break;
    }
    return( what );
}

static look_ahead_storage *lookAheadSaveToken( PARSE_STACK *state, int tok )
{
    look_ahead_storage *save;

    if( state->use_saved_tokens ) {
        /* token has already been stored */
        return( NULL );
    }
    save = VstkPush( &(state->look_ahead_storage) );
    save->yylval = yylval;
    save->yylocation = yylocation;
    save->yytok = tok;
    state->look_ahead_count++;
    return( save );
}

static void lookAheadUnsaveToken( PARSE_STACK *state, int tok )
{
    look_ahead_storage *save;

#ifndef NDEBUG
    if( state->use_saved_tokens ) {
        CFatal( "trying to unsave a saved token" );
    }
#endif
    save = VstkPop( &(state->look_ahead_storage) );
#ifndef NDEBUG
    if( save->yytok != tok ) {
        CFatal( "trying to unsave an unaligned saved token" );
    }
#else
    tok = tok;
#endif
    state->look_ahead_count--;
}

static void lookAheadUseSavedTokens( PARSE_STACK *state, unsigned index )
{
    state->use_saved_tokens = TRUE;
    state->look_ahead_index = index;
    currToken = Y_IMPOSSIBLE;
}

/*
    parse stack is in this LR0 state:

function-like-cast <- simple-type-specifier . '(' expression-list-opt ')'
type-specifier     <- simple-type-specifier .

    We don't know whether to shift or reduce on a '(' token which means
we don't know whether the tokens up ahead are an expression (shift) or a
declaration fragment (reduce).  The current draft specifies that one
should parse ahead with the current context to see if the fragment can
be an expression or a declaration.  In the event that the fragment can
be both, we favour the interpretation of a declaration.

This is achieved by creating two parse stacks with the top two states of
the current parse stack.  We are finished disambiguation when we pop off
the bottom state.  We parse ahead killing off interpretations that do
not survive scrutiny.

The disambiguation is linear with respect to the number of disambiguations
necessary (no exponential blow-up with nested disambiguations).
*/
static YYACTIONTYPE lookAheadYYAction( YYTOKENTYPE t, PARSE_STACK *state, PARSE_STACK *host )
{
    unsigned save_count;
    YYACTIONTYPE yyaction;
    la_action expr_what;
    la_action decl_what;
    PARSE_STACK *disambig_state;
    look_ahead_storage *ambiguous_left_paren_token;
    auto PARSE_STACK look_ahead_decl_state;
    auto PARSE_STACK look_ahead_expr_state;
    typedef enum la_response {
        ____,                   /* error */
        DIS1,                   /* must disambiguate one of the parses */
        DIS2,                   /* must disambiguate both of the parses */
        DONE,                   /* > DONE means we are done disambiguation */
        DECL,                   /* favour interpretation as a declaration/type */
        EXPR,                   /* favour interpretation as an expression */
    } la_response;
    la_response what_to_do;
    static la_response response[LA_MAX][LA_MAX] = {
    /*
                   \                    declaration
                    \
         expression  \  UNDERFLOW       DISAMBIGUATE    SYNTAX          NULL
                      \ =========       ============    ======          ==== */
    /*    UNDERFLOW */ { DECL,          ____,           EXPR,           DECL },
    /* DISAMBIGUATE */ { ____,          DIS2,           EXPR,           DIS1 },
    /*       SYNTAX */ { DECL,          DECL,           DECL,           DECL },
    /*         NULL */ { DECL,          DIS1,           EXPR,           ____ },
    };

    if( state == host ) {
        state->look_ahead_active = TRUE;
    }
    newLookAheadStack( &look_ahead_expr_state, state );
    newLookAheadStack( &look_ahead_decl_state, state );
#ifndef NDEBUG
    if( PragDbgToggle.dump_parse ) {
        printf( "expr...\n" );
    }
#endif
    lookAheadShift( &look_ahead_expr_state, YYAMBIGH0, t );
#ifndef NDEBUG
    if( PragDbgToggle.dump_parse ) {
        printf( "decl...\n" );
    }
#endif
    lookAheadReduce( &look_ahead_decl_state, YYAMBIGR0 );
    lookAheadShiftReduce( t, &look_ahead_decl_state, host );
    ambiguous_left_paren_token = lookAheadSaveToken( host, t );
    for(;;) {
        for(;;) {
            nextYYLexToken( host );
            t = yylex( host );
            lookAheadSaveToken( host, t );
#ifndef NDEBUG
            if( PragDbgToggle.dump_parse ) {
                printf( "expr...\n" );
            }
#endif
            expr_what = lookAheadShiftReduce( t, &look_ahead_expr_state, host );
#ifndef NDEBUG
            if( PragDbgToggle.dump_parse ) {
                printf( "decl...\n" );
            }
#endif
            decl_what = lookAheadShiftReduce( t, &look_ahead_decl_state, host );
            if( expr_what != decl_what ) break;
            if( expr_what != LA_NULL ) break;
        }
        /* something significant has happened... */
        what_to_do = response[expr_what][decl_what];
        if( what_to_do > DONE ) {
            if( what_to_do == DECL ) {
                CErr1( WARN_AMBIGUOUS_CONSTRUCT_DECL );
                yyaction = YYACTION_REDUCE_RULE( YYAMBIGR0 );
                break;
            }
            if( what_to_do == EXPR ) {
                CErr1( WARN_AMBIGUOUS_CONSTRUCT_EXPR );
                yyaction = YYACTION_SHIFT_STATE( YYAMBIGH0 );
                break;
            }
            fatalParserError();
        }
        if( what_to_do == ____ ) {
            CErr1( WARN_AMBIGUOUS_CONSTRUCT_UNKNOWN );
            yyaction = YYAMBIGH0;
            break;
        }
        CErr1( WARN_AMBIGUOUS_CONSTRUCT_AGAIN );
        disambig_state = &look_ahead_decl_state;
        if( what_to_do == DIS1 && expr_what == LA_DISAMBIGUATE ) {
            disambig_state = &look_ahead_expr_state;
        }
        lookAheadUnsaveToken( host, t );
        save_count = host->look_ahead_count;
        yyaction = lookAheadYYAction( t, disambig_state, host );
        lookAheadUseSavedTokens( host, save_count - 1 );
        lookAheadUseYYAction( disambig_state, t, yyaction );
        if( what_to_do == DIS2 ) {
            disambig_state = &look_ahead_expr_state;
            lookAheadUseYYAction( disambig_state, t, yyaction );
        }
    }
    deleteStack( &look_ahead_decl_state );
    deleteStack( &look_ahead_expr_state );
    if( YYACTION_IS_SHIFT( yyaction ) ) {
        ambiguous_left_paren_token->yytok = LA_SHIFT_TOKEN;
    } else {
        ambiguous_left_paren_token->yytok = LA_REDUCE_TOKEN;
    }
    if( state == host ) {
        lookAheadUseSavedTokens( host, 0 );
        state->look_ahead_active = FALSE;
    }
    return( yyaction );
}

static p_action doAction( YYTOKENTYPE t, PARSE_STACK *state )
{
    p_action what;
    YYSTYPE yyval;
    YYSTYPE *yyvp;
    YYSTYPE *curr_vsp;
    YYACTIONTYPE *curr_ssp;
    TOKEN_LOCN *curr_lsp;
    TOKEN_LOCN *yylp;
    unsigned yyk;
    unsigned yyl;
    unsigned yyaction;
    unsigned get_yyaction;
    unsigned rule;

    #define INC_STACK( kind ) \
        curr_##kind = state->kind; \
        ++curr_##kind; \
        state->kind = curr_##kind;
    for(;;) {
        yyk = *(state->ssp);
        DbgStmt( if( PragDbgToggle.parser_states ) printf( "parser state: %u token: 0x%x\n", yyk, t ); );
        if( yyk == YYAMBIGS0 && t == YYAMBIGT0 ) {
            if( state->favour_shift ) {
                yyaction = YYACTION_SHIFT_STATE( YYAMBIGH0 );
            } else if( state->favour_reduce ) {
                yyaction = YYACTION_REDUCE_RULE( YYAMBIGR0 );
            } else {
                CErr1( WARN_AMBIGUOUS_CONSTRUCT );
                yyaction = lookAheadYYAction( t, state, state );
            }
        } else {
            what = normalYYAction( t, state, &get_yyaction );
            if( what != P_NULL ) {
                return( what );
            }
            yyaction = get_yyaction;
        }
        if( YYACTION_IS_SHIFT( yyaction ) ) {
            if( yyaction == YYSTOP ) {
                return( P_ACCEPT );
            }
            if( state->ssp == state->exhaust ) {
                return( P_OVERFLOW );
            }
            INC_STACK( ssp );
            *curr_ssp = yyaction;
            INC_STACK( vsp );
            *curr_vsp = yylval;
            INC_STACK( lsp );
            TokenLocnAssign( *curr_lsp, yylocation );
            switch( t ) {
            case Y_SCOPED_OPERATOR:
            case Y_TEMPLATE_SCOPED_OPERATOR:
                pushOperatorQualification( yylval.tree );
                break;
            }
#ifndef NDEBUG
            if( PragDbgToggle.dump_parse ) {
                switch( t ) {
                case Y_ID:
                case Y_TYPE_NAME:
                case Y_TEMPLATE_NAME:
                    printf( "%s '%s'\n", yytoknames[ t ], yylval.tree->u.id.name );
                    break;
                default:
                    puts( yytoknames[ t ] );
                }
            }
#endif
            return( P_SHIFT );
        }
        rule = yyaction - YYUSED;
        yyl = yyplentab[ rule ];
        if( yyl != 0 ) {
            state->ssp -= yyl;
            state->vsp -= yyl;
            state->lsp -= yyl;
            if( state->ssp < state->sstack ) {
                fatalParserError();
            }
        } else {
            if( state->ssp == state->exhaust ) {
                return( P_OVERFLOW );
            }
        }
        yyaction = GOTOYYAction( state, rule );
        INC_STACK( ssp );
        *curr_ssp = yyaction;
        INC_STACK( vsp );
        yyvp = curr_vsp;
        yylp = state->lsp;
#ifndef NDEBUG
        if( PragDbgToggle.dump_parse ) {
            dump_rule( rule );
        }
#endif
        what = P_ERROR;
        switch( rule ) {

        default:
            yyval = yyvp[0];
        }
        *curr_vsp = yyval;
        INC_STACK( lsp );
        curr_lsp->src_file = NULL;
        /* reduce as far as possible unless we want to relex */
        if( what != P_ERROR ) {
            if( what == P_RELEX ) {
                ParseFlush();
            }
            return( what );
        }
    }
    #undef INC_STACK
}

static void makeStable( int end_token )
{
    unsigned depth;
    boolean token_absorbed;

    ParseFlush();
    token_absorbed = FALSE;     /* infinite loop protection */
    depth = 0;
    for(;;) {
        if( CurToken == T_EOF ) return;
        if( CurToken == T_LEFT_BRACE ) {
            ++depth;
        } else if( depth == 0 ) {
            switch( CurToken ) {
            case T_IF:
            case T_WHILE:
            case T_DO:
            case T_FOR:
            case T_SWITCH:
            case T_CASE:
            case T_DEFAULT:
            case T_BREAK:
            case T_CONTINUE:
            case T_RETURN:
            case T_GOTO:
            case T_THROW:
            case T_CATCH:
            case T_RIGHT_BRACE:
                if( token_absorbed ) {
                    if( ScopeId( CurrScope ) == SCOPE_BLOCK ) {
                        return;
                    }
                }
                break;
            case T_SEMI_COLON:
                nextToken( &yylocation );
                return;
            default:
                if( CurToken == end_token ) {
                    return;
                }
            }
        } else if( CurToken == T_RIGHT_BRACE ) {
            --depth;
        }
        token_absorbed = TRUE;
        nextToken( &yylocation );
    }
}

static void genIdSyntaxError( int msg )
{
    PTREE id;

    id = yylval.tree;
    SetErrLoc( &(id->locn) );
    CErr2p( msg, id->u.id.name );
}

static void syntaxError( void )
{
    if( CurToken == T_EOF ) {
        CErr1( ERR_PREMATURE_ENDFILE );
    } else if( CurToken == T_BAD_TOKEN ) {
        CErr1( BadTokenInfo );
    } else {
        switch( currToken ) {
        case Y_ID:
            genIdSyntaxError( ERR_SYNTAX_UNDECLARED_ID );
            break;
        case Y_TYPE_NAME:
            genIdSyntaxError( ERR_SYNTAX_TYPE_NAME );
            break;
        case Y_TEMPLATE_NAME:
            genIdSyntaxError( ERR_SYNTAX_TEMPLATE_NAME );
            break;
        default:
            CErr1( ERR_SYNTAX );
        }
    }
}

PTREE ParseExpr( int end_token )
/******************************/
{
    int t;
    PARSE_STACK expr_state;
    p_action what;
    PTREE expr_tree;

    newExprStack( &expr_state, Y_EXPRESSION_SPECIAL );
    syncLocation();
    /* do parse */
    for(;;) {
        do {
            t = yylex( &expr_state );
            what = doAction( t, &expr_state );
        } while( what == P_RELEX );
        if( what != P_SHIFT ) break;
        nextYYLexToken( &expr_state );
    }
    if( what > P_SPECIAL ) {
        if( what > P_ERROR ) {
            switch( what ) {
            case P_SYNTAX:
                syntaxError();
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_EXPRESSION );
                break;
            }
            makeStable( end_token );
            expr_tree = NULL;
        }
#ifndef NDEBUG
        else {
            CFatal( "invalid return from doAction" );
        }
#endif
    } else {
        expr_tree = expr_state.vsp->tree;
    }
    deleteStack( &expr_state );
    return( expr_tree );
}

PTREE ParseMemInit( void )
/************************/
{
    int t;
    PARSE_STACK mem_init_state;
    p_action what;
    PTREE mem_init_tree;

    newExprStack( &mem_init_state, Y_MEM_INIT_SPECIAL );
    syncLocation();
    /* do parse */
    for(;;) {
        do {
            t = yylex( &mem_init_state );
            what = doAction( t, &mem_init_state );
        } while( what == P_RELEX );
        if( what != P_SHIFT ) break;
        nextYYLexToken( &mem_init_state );
    }
    if( what > P_SPECIAL ) {
        if( what > P_ERROR ) {
            switch( what ) {
            case P_SYNTAX:
                syntaxError();
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_EXPRESSION );
                break;
            }
            makeStable( T_LEFT_BRACE );
            mem_init_tree = NULL;
        }
#ifndef NDEBUG
        else {
            CFatal( "invalid return from doAction" );
        }
#endif
    } else {
        mem_init_tree = mem_init_state.vsp->tree;
    }
    deleteStack( &mem_init_state );
    return( mem_init_tree );
}

PTREE ParseDefArg( void )
/***********************/
{
    int t;
    PARSE_STACK defarg_start;
    p_action what;
    PTREE defarg_tree;

    newExprStack( &defarg_start, Y_DEFARG_SPECIAL );
    syncLocation();
    /* do parse */
    for(;;) {
        do {
            t = yylex( &defarg_start );
            what = doAction( t, &defarg_start );
        } while( what == P_RELEX );
        if( what != P_SHIFT ) break;
        nextYYLexToken( &defarg_start );
    }
    if( what > P_SPECIAL ) {
        if( what > P_ERROR ) {
            switch( what ) {
            case P_SYNTAX:
                syntaxError();
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_EXPRESSION );
                break;
            }
            makeStable( T_DEFARG_END );
            defarg_tree = NULL;
        }
#ifndef NDEBUG
        else {
            CFatal( "invalid return from doAction" );
        }
#endif
    } else {
        defarg_tree = defarg_start.vsp->tree;
    }
    deleteStack( &defarg_start );
    return( defarg_tree );
}

DECL_INFO *ParseException( void )
/*******************************/
{
    int t;
    PARSE_STACK except_state;
    p_action what;
    DECL_INFO *exception;
    auto error_state_t check;

    CErrCheckpoint( &check );
    newExceptionStack( &except_state );
    syncLocation();
    pushDefaultDeclSpec( &except_state );
    /* do parse */
    for(;;) {
        do {
            t = yylex( &except_state );
            what = doAction( t, &except_state );
        } while( what == P_RELEX );
        if( what != P_SHIFT ) break;
        nextYYLexToken( &except_state );
    }
    exception = NULL;
    if( what > P_SPECIAL ) {
        if( what > P_ERROR ) {
            switch( what ) {
            case P_SYNTAX:
                syntaxError();
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_EXCEPTION );
                break;
            }
            makeStable( T_RIGHT_PAREN );
        }
#ifndef NDEBUG
        else {
            CFatal( "invalid return from doAction" );
        }
#endif
    } else {
        if( except_state.vsp->dinfo != NULL ) {
            exception = except_state.vsp->dinfo;
        }
    }
    deleteStack( &except_state );
    if( exception != NULL && CErrOccurred( &check ) ) {
        FreeDeclInfo( exception );
        exception = NULL;
    }
    return( exception );
}

void ParseDecls( void )
/*********************/
{
    int t;
    PARSE_STACK decl_state;
    p_action what;

    for(;;) {
        if( CurToken == T_EOF ) break;
        newDeclStack( &decl_state );
        syncLocation();
        pushDefaultDeclSpec( &decl_state );
        do {
            /* do parse */
            for(;;) {
                do {
                    t = yylex( &decl_state );
                    what = doAction( t, &decl_state );
                } while( what == P_RELEX );
                if( what != P_SHIFT ) break;
                nextYYLexToken( &decl_state );
            }
            if( what > P_SPECIAL ) {
                if( what > P_ERROR ) {
                    switch( what ) {
                    case P_SYNTAX:
                        syntaxError();
                        break;
                    case P_OVERFLOW:
                        CErr1( ERR_COMPLICATED_DECLARATION );
                        break;
                    }
                    makeStable( T_SEMI_COLON );
                } else if( what == P_CLASS_TEMPLATE ) {
                    TemplateHandleClassMember( decl_state.vsp->dinfo );
                    syncToRestart( &decl_state );
                    /* we don't want the linkage reset */
                    ParseFlush();
                    syncLocation();
                }
#ifndef NDEBUG
                else {
                    CFatal( "invalid return from doAction" );
                }
#endif
            }
            if( CurToken == T_EOF ) break;
        } while( what == P_CLASS_TEMPLATE );
        deleteStack( &decl_state );
        LinkageReset();
    }
    parseEpilogue();
}

static void parseEpilogue( void )
{
    /* current token state is end-of-file */
    TemplateProcessInstantiations();
    CompFlags.parsing_finished = 1;
}

PTREE ParseExprDecl( void )
/*************************/
{
    int t;
    PARSE_STACK expr_decl_state;
    p_action what;
    PTREE expr_tree;

    newExprStack( &expr_decl_state, Y_EXPR_DECL_SPECIAL );
    syncLocation();
    /* do parse */
    for(;;) {
        do {
            t = yylex( &expr_decl_state );
            what = doAction( t, &expr_decl_state );
        } while( what == P_RELEX );
        if( what != P_SHIFT ) break;
        nextYYLexToken( &expr_decl_state );
    }
    expr_tree = NULL;           /* assume declaration or error */
    if( what > P_SPECIAL ) {
        if( what > P_ERROR ) {
            switch( what ) {
            case P_SYNTAX:
                syntaxError();
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_STATEMENT );
                break;
            }
            makeStable( T_SEMI_COLON );
        }
#ifndef NDEBUG
        else {
            CFatal( "invalid return from doAction" );
        }
#endif
    } else {
        /* statement accepted! */
        if( expr_decl_state.vsp->tree != NULL ) {
            expr_tree = expr_decl_state.vsp->tree;
        }
    }
    deleteStack( &expr_decl_state );

    return( expr_tree );
}

DECL_SPEC *ParseClassInstantiation( REWRITE *defn, boolean defer_defn )
/*********************************************************************/
{
    int t;
    PARSE_STACK instantiate_state;
    p_action what;
    DECL_SPEC *new_type;
    REWRITE *last_rewrite;
    REWRITE *save_token;
    void (*last_source)( void );
    auto error_state_t check;
    auto TOKEN_LOCN locn;

    if( defn == NULL ) {
        return( NULL );
    }
    CErrCheckpoint( &check );
    save_token = RewritePackageToken();
    SrcFileGetTokenLocn( &locn );
    ParseFlush();
    LinkagePushCpp();
    newClassInstStack( &instantiate_state );
    if( defer_defn ) {
        instantiate_state.template_class_inst_defer = TRUE;
    }
    syncLocation();
    pushDefaultDeclSpec( &instantiate_state );
    last_source = SetTokenSource( RewriteToken );
    last_rewrite = RewriteRewind( defn );
    /* do parse */
    for(;;) {
        do {
            t = yylex( &instantiate_state );
            what = doAction( t, &instantiate_state );
        } while( what == P_RELEX );
        if( what != P_SHIFT ) break;
        nextYYLexToken( &instantiate_state );
    }
    RewriteClose( last_rewrite );
    ResetTokenSource( last_source );
    new_type = NULL;
    if( what > P_SPECIAL ) {
        if( what > P_ERROR ) {
            switch( what ) {
            case P_SYNTAX:
                syntaxError();
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_DECLARATION );
                break;
            }
        } else if( what == P_DEFER_DEFN ) {
            ParseFlush();
            t = Y_SEMI_COLON;
            do {
                what = doAction( t, &instantiate_state );
            } while( what == P_RELEX );
#ifndef NDEBUG
            if( what != P_ACCEPT ) {
                CFatal( "invalid return from doAction" );
            }
#endif
            new_type = instantiate_state.vsp->dspec;
        }
#ifndef NDEBUG
        else {
            CFatal( "invalid return from doAction" );
        }
#endif
    } else {
        if( instantiate_state.vsp->dspec != NULL ) {
            new_type = instantiate_state.vsp->dspec;
        }
    }
    deleteStack( &instantiate_state );
    LinkagePop();
    SrcFileResetTokenLocn( &locn );
    RewriteRestoreToken( save_token );
    ParseFlush();
    if( new_type != NULL ) {
        if( CErrOccurred( &check ) ) {
            PTypeRelease( new_type );
            new_type = NULL;
        } else {
            new_type = PTypeDone( new_type, TRUE );
        }
    }
    return( new_type );
}

void ParseClassMemberInstantiation( REWRITE *defn )
/*************************************************/
{
    int t;
    PARSE_STACK instantiate_state;
    p_action what;
    REWRITE *last_rewrite;
    void (*last_source)( void );
    auto TOKEN_LOCN locn;

    if( defn == NULL ) {
        return;
    }
    SrcFileGetTokenLocn( &locn );
    ParseFlush();
    LinkagePushCpp();
    newClassMemberInstStack( &instantiate_state );
    syncLocation();
    pushDefaultDeclSpec( &instantiate_state );
    last_source = SetTokenSource( RewriteToken );
    last_rewrite = RewriteRewind( defn );
    /* do parse */
    for(;;) {
        do {
            t = yylex( &instantiate_state );
            what = doAction( t, &instantiate_state );
        } while( what == P_RELEX );
        if( what != P_SHIFT ) break;
        nextYYLexToken( &instantiate_state );
    }
    RewriteClose( last_rewrite );
    ResetTokenSource( last_source );
    if( what > P_SPECIAL ) {
        if( what > P_ERROR ) {
            switch( what ) {
            case P_SYNTAX:
                syntaxError();
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_DECLARATION );
                break;
            }
        }
#ifndef NDEBUG
        else {
            CFatal( "invalid return from doAction" );
        }
#endif
    }
    deleteStack( &instantiate_state );
    LinkagePop();
    SrcFileResetTokenLocn( &locn );
    CurToken = T_EOF;
    strcpy( Buffer, Tokens[ T_EOF ] );
}

void ParseFunctionInstantiation( REWRITE *defn )
/**********************************************/
{
    int t;
    PARSE_STACK instantiate_state;
    p_action what;
    REWRITE *last_rewrite;
    void (*last_source)( void );
    auto TOKEN_LOCN locn;

    if( defn == NULL ) {
        return;
    }
    SrcFileGetTokenLocn( &locn );
    ParseFlush();
    LinkagePushCpp();
    newDeclStack( &instantiate_state );
    syncLocation();
    pushDefaultDeclSpec( &instantiate_state );
    last_source = SetTokenSource( RewriteToken );
    last_rewrite = RewriteRewind( defn );
    /* do parse */
    for(;;) {
        do {
            t = yylex( &instantiate_state );
            what = doAction( t, &instantiate_state );
        } while( what == P_RELEX );
        if( what != P_SHIFT ) break;
        nextYYLexToken( &instantiate_state );
    }
    RewriteClose( last_rewrite );
    ResetTokenSource( last_source );
    if( what > P_SPECIAL ) {
        if( what > P_ERROR ) {
            switch( what ) {
            case P_SYNTAX:
                syntaxError();
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_DECLARATION );
                break;
            }
        }
#ifndef NDEBUG
        else {
            CFatal( "invalid return from doAction" );
        }
#endif
    }
    deleteStack( &instantiate_state );
    LinkagePop();
    SrcFileResetTokenLocn( &locn );
    CurToken = T_EOF;
    strcpy( Buffer, Tokens[ T_EOF ] );
}

pch_status PCHWriteParserData( void )
{
    return( PCHCB_OK );
}

pch_status PCHReadParserData( void )
{
    if( currParseStack != NULL ) {
        currParseStack->reset_scope = FileScope;
    }
    return( PCHCB_OK );
}

pch_status PCHInitParserData( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniParserData( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}
