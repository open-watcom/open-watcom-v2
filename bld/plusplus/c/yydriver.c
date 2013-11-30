/*
YYDRIVER: driver code to make use of YACC generated parser tables and support

00002 - increment to force compile
*/

#include "plusplus.h"
#include "ytab.h"
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
#include "memmgr.h"
#include "cgfront.h"
#include "rtngen.h"
#ifndef NDEBUG
#include "pragdefn.h"
#include "dbg.h"
#endif

ExtraRptCtr( lookup_lexical );
ExtraRptCtr( lookup_other );
ExtraRptCtr( found_type );
ExtraRptCtr( found_template );
ExtraRptCtr( found_template_id );
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
    SCOPE               scope_member;
    VSTK_CTL            look_ahead_storage;
    unsigned            look_ahead_count;
    unsigned            look_ahead_index;
    TOKEN_LOCN          template_record_locn;
    REWRITE             *template_record_tokens;
    VSTK_CTL            angle_stack;
    char                *expect;
    unsigned            no_super_tokens : 1;
    unsigned            use_saved_tokens : 1;
    unsigned            favour_reduce : 1;
    unsigned            favour_shift : 1;
    unsigned            look_ahead_stack : 1;
    unsigned            look_ahead_active : 1;
    unsigned            template_decl : 1;
    unsigned            template_class_inst_defer : 1;
    unsigned            special_colon_colon : 1;
    unsigned            special_gt_gt : 1;
    unsigned            special_typename : 1;
    unsigned            template_extern : 1;
    unsigned            template_instantiate : 1;
};

typedef struct {
    YYSTYPE             yylval;
    TOKEN_LOCN          yylocation;
    YYTOKENTYPE         yytok;
} look_ahead_storage;

typedef struct {
    unsigned            paren_depth;
} angle_bracket_stack;

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
    LKDEF( LK_UNKN_ID,  UNKNOWN_ID )            /* identifier */ \
    LKDEF( LK_TEMPL_ID, TEMPLATE_ID )           /* template function */ \
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

static void recordTemplateCtorInitializer( PARSE_STACK * );



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

    for( p = yytoknames[yyplhstab[rule]]; *p; ++p ) {
        putchar( *p );
    }
    putchar( ' ' );
    putchar( '<' );
    putchar( '-' );
    tok = &yyrhstoks[yyrulebase[rule]];
    for( i = yyplentab[rule]; i != 0; --i ) {
        putchar( ' ' );
        for( p = yytoknames[*tok]; *p; ++p ) {
            putchar( *p );
        }
        ++tok;
    }
    putchar( '\n' );
}

static void dump_state_stack(const char * label, PARSE_STACK * stack)
{
    static PARSE_STACK * last_stack = NULL;

    if(stack != last_stack)
    {
        printf("===============================================================================\n");
        printf("*** PARSE STACK CHANGE *** New: 0x%.08X Old: 0x%.08X\n", stack, last_stack);
        printf("===============================================================================\n");
        last_stack = stack;
    }    
    
    if(NULL == stack)
    {
        printf("dump_state_stack: NULL stack\n");
    }
    else
    {
        YYACTIONTYPE *  the_ssp = stack->ssp;
        YYACTIONTYPE *  the_sstack = stack->sstack;
        unsigned        index;
        
        printf("dump_state_stack \"%s\" (0x%.08X):\n", label, the_sstack);
        /*
        //  ensure we dump the top of stack (test &(ssp[1]))
        */
#if 0
        for(index = 0; &(the_sstack[index]) < &(the_ssp[1]); index++)
        {
              YYACTIONTYPE x = the_sstack[index];
              printf("  Index: %03d State: %04u\n", index, x);
        }
#else
        printf(" State(s):");
        for(index = 0; &(the_sstack[index]) < &(the_ssp[1]); index++)
        {
              YYACTIONTYPE x = the_sstack[index];
              printf(" %03u", x);
        }
        printf("\n");
#endif
    }
    fflush(stdout);
}

#endif

static void deleteStack( PARSE_STACK * );

static PTREE setAnalysedFlag(
    PTREE expr )
{
    expr->flags |= PTF_ALREADY_ANALYSED;
    return expr;
}

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
    ExtraRptRegisterCtr( &found_template_id, "parser lookup: found template id" );
    ExtraRptRegisterCtr( &found_template, "parser lookup: found template" );
    ExtraRptRegisterCtr( &found_namespace, "parser lookup: found namespace" );
}

static void parseFini(          // PARSER INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    ParseFlush();
    DbgStmt( CarveVerifyAllGone( carveRESTART_PARSE, "RESTART_PARSE" ) );
    DbgStmt( CarveVerifyAllGone( carveVALUE_STACK, "VALUE_STACK" ) );
    DbgStmt( CarveVerifyAllGone( carveSTATE_STACK, "STATE_STACK" ) );
    DbgStmt( CarveVerifyAllGone( carveLOCATION_STACK, "LOCATION_STACK" ) );
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
    switch( ScopeId( GetCurrScope() ) ) {
    case SCOPE_CLASS:
        return( TRUE );
    case SCOPE_TEMPLATE_DECL:
        return( GetCurrScope()->ordered != NULL );
    case SCOPE_TEMPLATE_INST:
        return( GetCurrScope()->owner.inst != NULL );
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

static lk_result lexCategory( SCOPE scope, PTREE id, lk_control control,
                              SYMBOL_NAME *psym_name )
{
    NAME name;
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
    if( psym_name != NULL ) {
        *psym_name = sym_name;
    }
    if( sym_name != NULL ) {
        if( sym_name->name_syms != NULL ) {
            RingIterBeg( sym_name->name_syms, sym ) {
                if( SymIsFunctionTemplateModel( sym ) ) {
                    ExtraRptIncrementCtr( found_template_id );
                    return( LK_TEMPL_ID );
                }
            } RingIterEnd( sym )
        } else if( sym_name->name_type != NULL ) {
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
            if( !( control & LK_LEXICAL )
             && ScopeType( scope, SCOPE_CLASS ) ) {
                if( ScopeClass( scope )->u.c.info->name == name ) {
                    /* see 3.4.3.1 Class members [class.qual]:
                     *
                     * "In a lookup in which the constructor is an
                     * acceptable lookup result, if the
                     * nested-name-specifier nominates a class C, and
                     * the name specified after the
                     * nested-name-specifier, when looked up in C, is
                     * the injected-class-name of C (clause 9), the
                     * name is instead considered to name the
                     * constructor of class C."
                     */
                    ExtraRptIncrementCtr( found_id );
                    return( LK_ID );
                }
            }
            id->type = type;
            ExtraRptIncrementCtr( found_type );
            return( LK_TYPE );
        } else {
            DbgStmt( DumpSymbolName( sym_name ) );
            CFatal( "lexCategory: unable to process id with unknown type" );
        }
    } else {
        return( LK_UNKN_ID );
    }
    ExtraRptIncrementCtr( found_id );
    return( LK_ID );
}

static int doId( SCOPE scope_member )
{
    PTREE id;
    lk_control control;
    lk_result id_check;

    id = makeId();
    yylval.tree = id;
    control = scope_member ? 0 : LK_LEXICAL;
    if( LAToken == T_LT ) {
        control |= LK_LT_AHEAD;
    }
    id_check = lexCategory( scope_member ? scope_member : GetCurrScope(),
                            id, control, &yylval.tree->sym_name );
    return( lookupToken[id_check] );
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
    if( state->special_gt_gt ) {
        state->special_gt_gt = FALSE;
        CurToken = T_GT;
        SrcFileGetTokenLocn( &yylocation );
        return;
    }
    nextRecordedToken( state );
}

static TYPE findGenericType( SCOPE scope, NAME name )
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

static SCOPE checkColonColon( PTREE id, SCOPE scope, SCOPE not_nested,
                              boolean special_typename )
{
    NAME name;
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
    result = NULL;
    if( not_nested != NULL ) {
        /* need to bend over backwards to allow 12.4 Destructors
         * [class.dtor] (14) "the notation for explicit call of a
         * destructor can be used for any scalar type name
         * (5.2.4)." */
        result = ScopeFindLexicalColonColon( not_nested, name, CurToken == T_TILDE );
    }
    if( result == NULL ) {
        if( scope != NULL ) {
            result = ScopeFindMemberColonColon( scope, name );
        } else {
            scope = not_nested;
        }
    } else {
        scope = not_nested;
    }
    if( result == NULL ) {
        if( special_typename ) {
            return( NULL );
        }
        if( not_nested != NULL ) {
            if( ScopeId( not_nested ) == SCOPE_TEMPLATE_DECL ) {
                scope_type = findGenericType( not_nested, name );
            } else {
                CErr2p( ERR_UNDECLARED_CLASSNAMESPACE_SYM, name );
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
            /* see note above regarding 12.4 (14) */
            if( ( test_type->id != TYP_CLASS ) && ( CurToken != T_TILDE ) ) {
                CErr2p( ERR_NAME_NOT_A_CLASS_OR_NAMESPACE, name );
            } else {
                scope_type = test_type;
                ScopeAmbiguousSymbol( result, sym );
            }
        }
        ScopeFreeResult( result );
    }
    id_type = scope_type;
    class_type = BindTemplateClass( StructType( scope_type ), NULL, FALSE );
    if( class_type != NULL ) {
        // member pointers do not need the class to be defined
        if( StructOpened( class_type ) == NULL && CurToken != T_TIMES ) {
            CErr2p( ERR_CLASS_NOT_DEFINED, name );
            id_type = NULL;
        } else {
            id_type = class_type;
            scope = class_type->u.c.scope;
            id_scope = scope;
        }
    }
    id->type = id_type;
    id->u.id.scope = id_scope;
    return( scope );
}

static int scopedChain( PARSE_STACK *state, PTREE start, PTREE id,
                        unsigned ctrl, boolean special_typename )
{
    lk_result id_check;
    NAME name;
    SCOPE scope;
    SCOPE lexical_lookup;
    SCOPE member_lookup;
    PTREE curr;
    TYPE class_type;
    boolean undefined_scope;
    boolean special_template;

    undefined_scope = FALSE;
    scope = GetCurrScope();
    if( start != NULL ) {
        scope = GetFileScope();
        if( ctrl & CH_ALREADY_STARTED ) {
            class_type = start->u.subtree[1]->type;
            if( class_type != NULL ) {
                scope = class_type->u.c.scope;
                member_lookup = scope;
                lexical_lookup = NULL;
            } else {
                member_lookup = NULL;
                lexical_lookup = scope;
                undefined_scope = TRUE;
                if( special_typename ) {
                    scope = NULL;
                }
            }
        } else {
            member_lookup = NULL;
            lexical_lookup = scope;
        }
    } else if( state->scope_member != NULL ) {
        scope = state->scope_member;
        member_lookup = scope;
        lexical_lookup = scope;
    } else {
        member_lookup = NULL;
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
        if( CurToken == T_TEMPLATE ) {
            special_template = TRUE;
            nextRecordedToken( state );
        } else {
            special_template = FALSE;
        }
        if( ! undefined_scope ) {
            scope = checkColonColon( id, member_lookup, lexical_lookup, special_typename );
            if( scope == NULL ) {
                undefined_scope = TRUE;
            }
        }
        name = id->u.id.name;
        lexical_lookup = NULL;
        member_lookup = scope;
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
            yylval.tree->flags |= special_typename ? PTF_TYPENAME : 0;
            if( special_typename && undefined_scope ) {
                if( special_template ) {
                    return( Y_SCOPED_TEMPLATE_NAME );
                } else {
                    return( Y_SCOPED_TYPE_NAME );
                }
            }

            id_check = lexCategory( scope, id, LK_NULL, &yylval.tree->sym_name );
            switch( id_check ) {
            case LK_ID:
                return( Y_SCOPED_ID );
            case LK_UNKN_ID:
                return( Y_SCOPED_UNKNOWN_ID );
            case LK_TEMPL_ID:
                return( Y_SCOPED_TEMPLATE_ID );
            case LK_TYPE:
                return( Y_SCOPED_TYPE_NAME );
            case LK_TEMPLATE:
                return( Y_SCOPED_TEMPLATE_NAME );
            case LK_NAMESPACE:
                return( Y_SCOPED_NAMESPACE_NAME );
            DbgDefault( "unknown lexical category" );
            }
            return( Y_IMPOSSIBLE );
        case T_TILDE:
        case T_ALT_TILDE:
            yylval.tree = makeUnary( CO_TILDE, curr );
            state->scope_member = curr->u.subtree[1]->u.id.scope;
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

static int templateScopedChain( PARSE_STACK *state, boolean special_typename )
{
    lk_result id_check;
    PTREE scope_tree;
    PTREE curr;
    PTREE id;
    TYPE template_type;
    TYPE template_class_type;
    NAME name;
    SCOPE scope;
    int adjusted_token;
    boolean undefined_scope;
    boolean special_template;

    template_type = BindTemplateClass( state->class_colon, NULL, FALSE );
    name = SimpleTypeName( template_type );
    template_class_type = StructType( template_type );
    scope_tree = PTreeId( name );
    scope_tree->type = template_class_type;
    if( template_class_type != NULL ) {
        scope_tree->u.id.scope = template_class_type->u.c.scope;
        undefined_scope = ( template_class_type->flag & TF1_UNBOUND );
    } else {
        undefined_scope = TRUE;
    }
    curr = makeBinary( CO_COLON_COLON, NULL, scope_tree );
    nextRecordedToken( state );
    for(;;) {
        if( CurToken == T_TEMPLATE ) {
            special_template = TRUE;
            nextRecordedToken( state );
        } else {
            special_template = FALSE;
        }
        switch( CurToken ) {
        case T_ID:
            LookPastName();
            /* fall through */
        case T_SAVED_ID:
            if( ! undefined_scope ) {
                id = makeId();
                if( LAToken == T_COLON_COLON ) {
                    adjusted_token = scopedChain( state, curr, id, CH_ALREADY_STARTED, special_typename );
                    /* translate to the correct token value */
                    adjusted_token -= Y_SCOPED_ID;
                    adjusted_token += Y_TEMPLATE_SCOPED_ID;
                    return( adjusted_token );
                }
                yylval.tree = makeBinary( CO_STORAGE, curr, id );
                yylval.tree->flags |= special_typename ? PTF_TYPENAME : 0;
                /* template instantiation errors may have occured */
                if( template_class_type != NULL ) {
                    scope = template_class_type->u.c.scope;

                    id_check = lexCategory( scope, id, LK_NULL, &yylval.tree->sym_name );
                    if( id_check == LK_TEMPLATE ) {
                        return( Y_TEMPLATE_SCOPED_TEMPLATE_NAME );
                    } else if( id_check == LK_TYPE ) {
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
            yylval.tree->flags |= special_typename ? PTF_TYPENAME : 0;
            if( special_typename ) {
                if( special_template ) {
                    return( Y_TEMPLATE_SCOPED_TEMPLATE_NAME );
                } else {
                    return( Y_TEMPLATE_SCOPED_TYPE_NAME );
                }
            }
            return( Y_TEMPLATE_SCOPED_ID );
        case T_TILDE:
        case T_ALT_TILDE:
            yylval.tree = makeUnary( CO_TILDE, curr );
            if( ! undefined_scope ) {
                state->scope_member = curr->u.subtree[1]->u.id.scope;
            }
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

static int globalChain( PARSE_STACK *state, boolean special_typename )
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
            return( scopedChain( state, tree, id, CH_NULL, special_typename ) );
        }
        yylval.tree = makeBinary( CO_STORAGE, tree, id );
        yylval.tree->flags |= special_typename ? PTF_TYPENAME : 0;
        id_check = lexCategory( GetFileScope(), id, LK_LEXICAL, &yylval.tree->sym_name );
        return( globalLookupToken[id_check] );
    case T_OPERATOR:
        yylval.tree = makeUnary( CO_OPERATOR, tree );
        return( Y_GLOBAL_OPERATOR );
    case T_TILDE:
    case T_ALT_TILDE:
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

static int specialAngleBracket( PARSE_STACK *state, int token )
{
    angle_bracket_stack *angle_state;

    angle_state = VstkTop( &(state->angle_stack) );
    if( angle_state != NULL ) {
        if( token == Y_GT ) {
            if( angle_state->paren_depth == 0 ) {
                VstkPop( &(state->angle_stack) );
                token = Y_GT_SPECIAL;
            }
        } else if( CompFlags.enable_std0x && ( token == Y_RSHIFT ) ) {
            // see Right Angle Brackets (N1757/05-0017)
            if( angle_state->paren_depth == 0 ) {
                VstkPop( &(state->angle_stack) );
                token = Y_GT_SPECIAL;
                state->special_gt_gt = TRUE;
            }
        } else if( token == Y_LEFT_BRACE ) {
            angle_state->paren_depth++;
        } else if( token == Y_RIGHT_BRACE ) {
            if( angle_state->paren_depth > 0) {
                angle_state->paren_depth--;
            }
        } else if( token == Y_LEFT_BRACKET ) {
            angle_state->paren_depth++;
        } else if( token == Y_RIGHT_BRACKET ) {
            if( angle_state->paren_depth > 0) {
                angle_state->paren_depth--;
            }
        } else if( token == Y_LEFT_PAREN ) {
            angle_state->paren_depth++;
        } else if( token == Y_RIGHT_PAREN ) {
            if( angle_state->paren_depth > 0) {
                angle_state->paren_depth--;
            }
        }
    }

    return token;
}

static int yylex( PARSE_STACK *state )
/************************************/
{
    lk_result id_check;
    int token;
    STRING_CONSTANT literal;
    PTREE tree;
    look_ahead_storage *saved_token;
    struct {
        unsigned no_super_token : 1;
        unsigned special_colon_colon : 1;
        unsigned special_typename : 1;
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
            id_check = lexCategory( GetCurrScope(), yylval.tree, LK_LEXICAL, &yylval.tree->sym_name );
            if( id_check != LK_TYPE ) {
                yylval.tree->type = NULL;
                token = Y_ID;
            }
            break;
        }
        token = specialAngleBracket( state, token );
        currToken = token;
        return( currToken );
    }
    flags.no_super_token = FALSE;
    flags.special_colon_colon = FALSE;
    flags.special_typename = state->special_typename;
    state->special_typename = FALSE;
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
            token = yytranslate[CurToken];
        } else {
            if( flags.special_colon_colon ) {
                token = templateScopedChain( state, flags.special_typename );
            } else {
                token = globalChain( state, flags.special_typename );
            }
        }
        break;
    case T_ID:
        LookPastName();
        /* fall through */
    case T_SAVED_ID:
        if( LAToken == T_COLON_COLON && ! flags.no_super_token ) {
            token = scopedChain( state, NULL, makeId(), CH_NULL, flags.special_typename );
        } else {
            token = doId( state->scope_member );
            yylval.tree->flags |= flags.special_typename ? PTF_TYPENAME : 0;
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
            tree = PTreeFloatingConstantStr( Buffer, ConstType );
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
        token = yytranslate[CurToken];
    }

    if( ( token != Y_SCOPED_TILDE ) && ( token != Y_TEMPLATE_SCOPED_TILDE ) ) {
        state->scope_member = NULL;
    }
    if( ! state->look_ahead_active ) {
        token = specialAngleBracket( state, token );
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
    case Y_UNKNOWN_ID:
    case Y_TEMPLATE_ID:
    case Y_GLOBAL_ID:
    case Y_GLOBAL_UNKNOWN_ID:
    case Y_GLOBAL_TEMPLATE_ID:
    case Y_GLOBAL_TYPE_NAME:
    case Y_GLOBAL_TEMPLATE_NAME:
    case Y_GLOBAL_NAMESPACE_NAME:
    case Y_GLOBAL_OPERATOR:
    case Y_GLOBAL_TILDE:
    case Y_GLOBAL_NEW:
    case Y_GLOBAL_DELETE:
    case Y_SCOPED_ID:
    case Y_SCOPED_UNKNOWN_ID:
    case Y_SCOPED_TEMPLATE_ID:
    case Y_SCOPED_TYPE_NAME:
    case Y_SCOPED_TEMPLATE_NAME:
    case Y_SCOPED_NAMESPACE_NAME:
    case Y_SCOPED_OPERATOR:
    case Y_SCOPED_TILDE:
    case Y_SCOPED_TIMES:
    case Y_TEMPLATE_SCOPED_ID:
    case Y_TEMPLATE_SCOPED_UNKNOWN_ID:
    case Y_TEMPLATE_SCOPED_TEMPLATE_ID:
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
    if( tokenMakesPTREE( currToken ) && yylval.tree ) {
#ifndef NDEBUG
        // NYI: we have a problem when this triggers!
        switch( currToken ) {
        case Y_GLOBAL_ID:
        case Y_GLOBAL_UNKNOWN_ID:
        case Y_GLOBAL_TEMPLATE_ID:
        case Y_GLOBAL_TEMPLATE_NAME:
        case Y_GLOBAL_OPERATOR:
        case Y_GLOBAL_TILDE:
        case Y_GLOBAL_NEW:
        case Y_GLOBAL_DELETE:
        case Y_SCOPED_ID:
        case Y_SCOPED_UNKNOWN_ID:
        case Y_SCOPED_TEMPLATE_ID:
        case Y_SCOPED_TYPE_NAME:
        case Y_SCOPED_TEMPLATE_NAME:
        case Y_SCOPED_NAMESPACE_NAME:
        case Y_SCOPED_OPERATOR:
        case Y_SCOPED_TILDE:
        case Y_SCOPED_TIMES:
        case Y_TEMPLATE_SCOPED_ID:
        case Y_TEMPLATE_SCOPED_UNKNOWN_ID:
        case Y_TEMPLATE_SCOPED_TEMPLATE_ID:
        case Y_TEMPLATE_SCOPED_TYPE_NAME:
        case Y_TEMPLATE_SCOPED_TEMPLATE_NAME:
        case Y_TEMPLATE_SCOPED_NAMESPACE_NAME:
        case Y_TEMPLATE_SCOPED_OPERATOR:
        case Y_TEMPLATE_SCOPED_TILDE:
        case Y_TEMPLATE_SCOPED_TIMES:
            ++ErrCount;
            puts( "ParseFlush with multi-token" );
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
            scope = GetFileScope();
        }
    } else {
        type = tree->type;
    }
    if( type == NULL ) {
        VBUF vbuf;

        type = MakeType( TYP_TYPENAME );
        FormatPTreeId( tree, &vbuf );
        type->u.n.name = CMemAlloc( VbufLen( &vbuf ) + 1 );
        memcpy( type->u.n.name, VbufString( &vbuf ), VbufLen( &vbuf ) + 1 );
        VbufFree( &vbuf );
        PTreeFreeSubtrees( tree );
        tree = NULL;
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
        CErr2p( ERR_EXPECTED_CLASS_TYPE, dspec->partial );
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
    stack->reset_scope = GetCurrScope();
    stack->qualifications = NULL;
    VstkOpen( &(stack->look_ahead_storage), sizeof(look_ahead_storage), 16 );
    stack->look_ahead_count = 0;
    stack->look_ahead_index = 0;
    stack->template_record_tokens = NULL;
    stack->class_colon = NULL;
    stack->scope_member = NULL;
    VstkOpen( &(stack->angle_stack), sizeof(angle_bracket_stack), 16 );
    stack->expect = NULL;
    stack->no_super_tokens = FALSE;
    stack->use_saved_tokens = FALSE;
    stack->favour_reduce = FALSE;
    stack->favour_shift = FALSE;
    stack->look_ahead_stack = FALSE;
    stack->look_ahead_active = FALSE;
    stack->template_decl = FALSE;
    stack->special_colon_colon = FALSE;
    stack->special_gt_gt = FALSE;
    stack->special_typename = FALSE;
    stack->template_extern = FALSE;
    stack->template_instantiate = FALSE;
}

static void restartInit( PARSE_STACK *stack )
{
    stack->template_decl = FALSE;
    DbgAssert( stack->qualifications == NULL );
    DbgAssert( stack->look_ahead_count == 0 );
    DbgAssert( stack->look_ahead_index == 0 );
    DbgAssert( stack->template_record_tokens == NULL );
    DbgAssert( VstkTop( &(stack->angle_stack) ) == NULL );
    DbgAssert( stack->expect == NULL );
    DbgAssert( stack->no_super_tokens == FALSE );
    DbgAssert( stack->use_saved_tokens == FALSE );
    DbgAssert( stack->favour_reduce == FALSE );
    DbgAssert( stack->favour_shift == FALSE );
    DbgAssert( stack->look_ahead_stack == FALSE );
    DbgAssert( stack->look_ahead_active == FALSE );
    DbgAssert( stack->template_decl == FALSE );
    DbgAssert( stack->special_colon_colon == FALSE );
    DbgAssert( stack->special_gt_gt == FALSE );
    DbgAssert( stack->special_typename == FALSE);
    DbgAssert( stack->template_extern == FALSE);
    DbgAssert( stack->template_instantiate == FALSE);
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

static void doPopRestartDecl( PARSE_STACK *state )
{
    RESTART_PARSE *restart;

    restart = StackPop( &(state->restart) );
    CarveFree( carveRESTART_PARSE, restart );
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
    ScopeAdjustUsing( GetCurrScope(), stack->reset_scope );
    SetCurrScope( stack->reset_scope );
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
    restart->reset_scope = GetCurrScope();

#ifndef NDEBUG
    if( PragDbgToggle.dump_parse ){
        printf("===============================================================================\n");
        printf("*** pushRestartDecl: 0x%.08X 0x%.08X\n", state, restart);
        printf("===============================================================================\n");
    }
#endif
}

#define restartDeclOK( restart ) \
    DbgAssert( restart != NULL ); \
    DbgAssert( restart->ssp >= restart->state->sstack ); \
    DbgAssert( restart->ssp < restart->state->exhaust );

static void popRestartDecl( PARSE_STACK *state )
{
    DbgStmt( RESTART_PARSE *restart );

    DbgStmt( restart = state->restart );
#ifndef NDEBUG
    if( PragDbgToggle.dump_parse ){
        printf("===============================================================================\n");
        printf("*** popRestartDecl: 0x%.08X 0x%.08X\n", state, restart);
        printf("===============================================================================\n");
    }
#endif
    restartDeclOK( restart );
    DbgAssert( restart->gstack == restart->state->gstack );
    DbgAssert( restart->reset_scope == GetCurrScope() );
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
        DbgAssert( restart->reset_scope == GetCurrScope() );
        restartInit( state );

#ifndef NDEBUG
        if( PragDbgToggle.dump_parse ) {
            dump_state_stack("after syncToRestart", state);
        }
#endif

    } else {
        fatalParserError();
    }
}

static void setNoSuperTokens( PARSE_STACK *state )
{
    state->no_super_tokens = TRUE;
}

static void setWatchColonColon( PARSE_STACK *state, PTREE tree, TYPE type )
{
    state->class_colon = type;
    state->special_colon_colon = TRUE;
    if( tree->flags & PTF_TYPENAME ) {
        state->special_typename = TRUE;
        tree->flags &= ~PTF_TYPENAME;
    }
}

static void setTypeMember( PARSE_STACK *state, SCOPE scope_member )
{
    state->scope_member = scope_member;
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
    lhs = yyplhstab[rule];
    raw_action = yyaction[lhs + yygotobase[top_state]];
    return( raw_action );
}
#if 0
// doAction contains variables called 'yyaction' so this can't be used
// plus it slowed down the compiler at one point in time
#define GOTOYYAction( state, rule ) \
        ( yyaction[yyplhstab[(rule)] + yygotobase[(state)->ssp[0]]] )
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
        if( yybitcheck[bit_index + yybitbase[top_state]] & mask ) {
            raw_action = yyaction[t + yyactionbase[top_state]];
            if(( raw_action & RAW_REDUCTION ) == 0 ) {
                /* we have a shift */
                *pa = raw_action;
                return( P_NULL );
            }
        } else {
            raw_action = yydefaction[top_state];
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
        lhs = yyplhstab[rule];
        top_state = yyaction[lhs + yygotobase[ssp[-1]]];
#ifndef NDEBUG
        if( PragDbgToggle.dump_parse ) { 
            printf("=== Unit reduction. New top state %03u Old state %03u ===\n", top_state, ssp[0]);
        }
#endif
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
        ScopeQualifyPush( class_type->u.c.scope, GetCurrScope() );
    }
}

static void lookAheadShift( PARSE_STACK *state, YYACTIONTYPE new_state, YYTOKENTYPE t )
{
#ifndef NDEBUG
    if( PragDbgToggle.dump_parse ) {
        puts( yytoknames[t] );
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
    state->ssp -= yyplentab[new_rule];
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
                // This warning seems inappropriate. --PeterC
                // CErr1( WARN_AMBIGUOUS_CONSTRUCT_DECL );
                yyaction = YYACTION_REDUCE_RULE( YYAMBIGR0 );
                break;
            }
            if( what_to_do == EXPR ) {
                // This warning seems inappropriate. --PeterC
                // CErr1( WARN_AMBIGUOUS_CONSTRUCT_EXPR );
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

    state->expect = NULL;
    for(;;) {
#ifndef NDEBUG
        unsigned stackDepth;
#endif
        yyk = *(state->ssp);
        DbgStmt( if( PragDbgToggle.parser_states ) printf( "parser top state: %u token: 0x%x (%s)\n", yyk, t , yytoknames[t]); );
        DbgStmt(stackDepth = (state->ssp - &(state->sstack[0])) + 1; );

        /* 
        //  DumpStack
        */
#ifndef NDEBUG
        if( PragDbgToggle.dump_parse ) {
            dump_state_stack("in start of doAction loop", state);
        }
#endif

        if( yyk == YYAMBIGS0 && t == YYAMBIGT0 ) {
            if( state->favour_shift ) {
                yyaction = YYACTION_SHIFT_STATE( YYAMBIGH0 );
            } else if( state->favour_reduce ) {
                yyaction = YYACTION_REDUCE_RULE( YYAMBIGR0 );
            } else {
                // This warning seems inappropriate. --PeterC
                // CErr1( WARN_AMBIGUOUS_CONSTRUCT );
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

#ifndef NDEBUG
            if( PragDbgToggle.dump_parse ) {
                dump_state_stack("after yyaction shift", state);
            }
#endif

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
                case Y_UNKNOWN_ID:
                case Y_TEMPLATE_ID:
                case Y_TYPE_NAME:
                case Y_TEMPLATE_NAME:
                    printf( "%s '%s'\n", yytoknames[t], yylval.tree->u.id.name );
                    break;
                default:
                    puts( yytoknames[t] );
                }
            }
#endif
            return( P_SHIFT );
        }
        rule = yyaction - YYUSED;
        yyl = yyplentab[rule];
        if( yyl != 0 ) {
            state->ssp -= yyl;
            state->vsp -= yyl;
            state->lsp -= yyl;
            if( state->ssp < state->sstack ) {
                fatalParserError();
            }
#ifndef NDEBUG
            if( PragDbgToggle.dump_parse ) {
                printf("=== Parser stack reduced by %u levels ===\n", yyl);
            }
#endif
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
            dump_state_stack("shift / reduce?", state);
        }
#endif
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

static void makeStable( TOKEN end_token )
{
    unsigned depth;
    boolean token_absorbed;
    TOKEN alt_token;

    /* also accept alternative tokens (digraphs) */
    if( end_token == T_LEFT_BRACKET ) {
        alt_token = T_ALT_LEFT_BRACKET;
    }
    if( end_token == T_RIGHT_BRACKET ) {
        alt_token = T_ALT_RIGHT_BRACKET;
    }
    if( end_token == T_LEFT_BRACE ) {
        alt_token = T_ALT_LEFT_BRACE;
    }
    if( end_token == T_RIGHT_BRACE ) {
        alt_token = T_ALT_RIGHT_BRACE;
    } else {
        alt_token = end_token;
    }

    PTreeFreeSubtrees( getMultiToken() );
    token_absorbed = FALSE;     /* infinite loop protection */
    depth = 0;
    while( CurToken != T_EOF ) {
        if( ( CurToken == T_LEFT_BRACE ) || ( CurToken == T_ALT_LEFT_BRACE ) ) {
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
            case T_ALT_RIGHT_BRACE:
                if( token_absorbed ) {
                    if( ScopeId( GetCurrScope() ) == SCOPE_BLOCK ) {
                        return;
                    }
                }
                break;
            case T_SEMI_COLON:
                nextToken( &yylocation );
                return;
            default:
                if( ( CurToken == end_token ) || ( CurToken == alt_token ) ) {
                    return;
                }
            }
        } else if( ( CurToken == T_RIGHT_BRACE ) || ( CurToken == T_ALT_RIGHT_BRACE ) ) {
            --depth;
        }
        token_absorbed = TRUE;
        nextToken( &yylocation );
    }
}

static void genIdSyntaxError( int msg, char *expect )
{
    PTREE id;

    id = yylval.tree;
    SetErrLoc( &(id->locn) );
    if( expect != NULL ) {
        CErr( ERR_SYNTAX_UNEXPECTED_ID, id->u.id.name, expect );
    } else {
        CErr2p( msg, id->u.id.name );
    }
}

static void genScopedIdSyntaxError( int msg, char *expect )
{
    PTREE id;
    VBUF vbuf;

    id = yylval.tree;
    SetErrLoc( &(id->locn) );
    FormatPTreeId( id, &vbuf );
    if( expect != NULL ) {
        CErr( ERR_SYNTAX_UNEXPECTED_ID, VbufString( &vbuf ), expect );
    } else {
        CErr2p( msg, VbufString( &vbuf ) );
    }
    VbufFree( &vbuf );
}

static void syntaxError( PARSE_STACK *state )
{
    if( CurToken == T_EOF ) {
        CErr1( ERR_PREMATURE_ENDFILE );
    } else if( CurToken == T_BAD_TOKEN ) {
        CErr1( BadTokenInfo );
    } else {
        switch( currToken ) {
        case Y_ID:
        case Y_TEMPLATE_ID:
        case Y_UNKNOWN_ID:
            genIdSyntaxError( ERR_SYNTAX_UNDECLARED_ID, state->expect );
            break;
        case Y_GLOBAL_ID:
        case Y_GLOBAL_TEMPLATE_ID:
        case Y_SCOPED_ID:
        case Y_SCOPED_TEMPLATE_ID:
            genScopedIdSyntaxError( ERR_SYNTAX_SCOPED_ID, state->expect );
            break;
        case Y_GLOBAL_UNKNOWN_ID:
            genScopedIdSyntaxError( ERR_SYNTAX_UNDECLARED_GLOBAL_ID, NULL );
            break;
        case Y_SCOPED_UNKNOWN_ID:
            genScopedIdSyntaxError( ERR_SYNTAX_UNDECLARED_SCOPED_ID, NULL );
            break;
        case Y_TYPE_NAME:
            genIdSyntaxError( ERR_SYNTAX_TYPE_NAME, state->expect );
            break;
        case Y_GLOBAL_TYPE_NAME:
        case Y_SCOPED_TYPE_NAME:
            genScopedIdSyntaxError( ERR_SYNTAX_UNDECLARED_ID, state->expect );
            break;
        case Y_TEMPLATE_NAME:
            genIdSyntaxError( ERR_SYNTAX_TEMPLATE_NAME, state->expect );
            break;
        case Y_GLOBAL_TEMPLATE_NAME:
        case Y_SCOPED_TEMPLATE_NAME:
            genScopedIdSyntaxError( ERR_SYNTAX_UNDECLARED_ID, state->expect );
            break;
        default:
            if( state->expect != NULL ) {
                CErr( ERR_SYNTAX_UNEXPECTED_TOKEN, TokenString(), state->expect );
            } else {
                CErr1( ERR_SYNTAX );
            }
        }
    }
    state->expect = NULL;
}

static void recordTemplateCtorInitializer( PARSE_STACK *state )
{
    unsigned paren_depth;
    unsigned brace_depth;

    brace_depth = 0;
    paren_depth = 0;

    while( CurToken != T_EOF ) {
        switch( CurToken ) {
        case T_LEFT_PAREN:
            ++paren_depth;
            break;
        case T_RIGHT_PAREN:
            if( paren_depth == 0 ) {
                syntaxError( state );
                break;
            }
            --paren_depth;
            break;
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            if( brace_depth == 0 ) {
                syntaxError( state );
                break;
            }
            --brace_depth;
            break;
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            if( paren_depth == 0 ) {
                return;
            }
            ++brace_depth;
            break;
        }
        nextRecordedToken( state );
    }
    syntaxError( state );
}

static PTREE genericParseExpr( YYTOKENTYPE tok, TOKEN end_token, MSG_NUM err_msg )
{
    int t;
    PARSE_STACK expr_state;
    p_action what;
    PTREE expr_tree;

    newExprStack( &expr_state, tok );
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
    expr_tree = NULL;
    if( what > P_SPECIAL ) {
        if( what > P_ERROR ) {
            switch( what ) {
            case P_SYNTAX:
                syntaxError( &expr_state );
                break;
            case P_OVERFLOW:
                CErr1( err_msg );
                break;
            }
            makeStable( end_token );
#ifndef NDEBUG
        } else {
            CFatal( "invalid return from doAction" );
#endif
        }
    } else {
        expr_tree = expr_state.vsp->tree;
    }
    deleteStack( &expr_state );
    return( expr_tree );
}

PTREE ParseExpr( TOKEN end_token )
{
    return genericParseExpr( Y_EXPRESSION_SPECIAL, end_token, ERR_COMPLICATED_EXPRESSION );
}

PTREE ParseExprDecl( void )
{
    return genericParseExpr( Y_EXPR_DECL_SPECIAL, T_SEMI_COLON, ERR_COMPLICATED_STATEMENT );
}

PTREE ParseMemInit( void )
{
    return genericParseExpr( Y_MEM_INIT_SPECIAL, T_LEFT_BRACE, ERR_COMPLICATED_EXPRESSION );
}

PTREE ParseDefArg( )
{
    return genericParseExpr( Y_DEFARG_SPECIAL, T_DEFARG_END, ERR_COMPLICATED_EXPRESSION );
}

PTREE ParseTemplateIntDefArg( )
{
    return genericParseExpr( Y_TEMPLATE_INT_DEFARG_SPECIAL, T_DEFARG_END, ERR_COMPLICATED_EXPRESSION );
}

PTREE ParseTemplateTypeDefArg( )
{
    return genericParseExpr( Y_TEMPLATE_TYPE_DEFARG_SPECIAL, T_DEFARG_END, ERR_COMPLICATED_EXPRESSION );
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
                syntaxError( &except_state );
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_EXCEPTION );
                break;
            }
            makeStable( T_RIGHT_PAREN );
#ifndef NDEBUG
        } else {
            CFatal( "invalid return from doAction" );
#endif
        }
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

static void parseEpilogue( void )
{
    /* current token state is end-of-file */
    RtnGenerate();
    CompFlags.parsing_finished = 1;
}

void ParseDecls( void )
/*********************/
{
    int t;
    PARSE_STACK decl_state;
    p_action what;

    while( CurToken != T_EOF ) {
        /*
        // We have seen a case where a macro subsitution was returned, leaving 
        // us with T_BAD_CHAR rather than T_BAD_TOKEN. For now,just die and 
        // get the hell out of parsing.
        */
        if( CurToken == T_BAD_CHAR){
            CErr1( ERR_SYNTAX );    /* CErr1( BadTokenInfo ); ? */
            break;
        }

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
                        syntaxError( &decl_state );
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
#ifndef NDEBUG
                } else {
                    CFatal( "invalid return from doAction" );
#endif
                }
            }
            if( CurToken == T_EOF ) break;
        } while( what == P_CLASS_TEMPLATE );
        deleteStack( &decl_state );
        LinkageReset();
    }
    parseEpilogue();
}

DECL_SPEC *ParseClassInstantiation( REWRITE *defn )
/*************************************************/
{
    int t;
    PARSE_STACK instantiate_state;
    p_action what;
    DECL_SPEC *new_type;
    REWRITE *last_rewrite;
    REWRITE *save_token;
    PTREE save_tree;
    int save_yytoken;
    unsigned suppressState;
    void (*last_source)( void );
    auto error_state_t check;
    auto TOKEN_LOCN locn;

    if( defn == NULL ) {
        return( NULL );
    }
    CErrCheckpoint( &check );
    suppressState = CErrUnsuppress();

    save_token = RewritePackageToken();
    save_yytoken = currToken;
    save_tree = yylval.tree;
    yylval.tree = NULL;
    SrcFileGetTokenLocn( &locn );
    ParseFlush();

    LinkagePushCpp();
    newClassInstStack( &instantiate_state );
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
                syntaxError( &instantiate_state );
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_DECLARATION );
                break;
            }
#ifndef NDEBUG
        } else {
            CFatal( "invalid return from doAction" );
#endif
        }
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
    syncLocation();
    currToken = save_yytoken;
    yylval.tree = save_tree;

    CErrSuppressRestore( suppressState );
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
                syntaxError( &instantiate_state );
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_DECLARATION );
                break;
            }
#ifndef NDEBUG
        } else {
            CFatal( "invalid return from doAction" );
#endif
        }
    }
    deleteStack( &instantiate_state );
    LinkagePop();
    SrcFileResetTokenLocn( &locn );
    CurToken = T_EOF;
    strcpy( Buffer, Tokens[T_EOF] );
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
                syntaxError( &instantiate_state );
                break;
            case P_OVERFLOW:
                CErr1( ERR_COMPLICATED_DECLARATION );
                break;
            }
#ifndef NDEBUG
        } else {
            CFatal( "invalid return from doAction" );
#endif
        }
    }
    deleteStack( &instantiate_state );
    LinkagePop();
    SrcFileResetTokenLocn( &locn );
    CurToken = T_EOF;
    strcpy( Buffer, Tokens[T_EOF] );
}

static unsigned decl_paren_depth;
static unsigned decl_bracket_depth;

static void declOnlyReset( void )
{
    decl_paren_depth = 0;
    decl_bracket_depth = 0;
}

/* only rewrite the declaration */
static void declOnlyRewriteToken( void )
{
    RewriteToken();

    if( CurToken == T_LEFT_PAREN ) {
        decl_paren_depth++;
    } else if( CurToken == T_RIGHT_PAREN ) {
        decl_paren_depth--;
    } else if( CurToken == T_LEFT_BRACKET ) {
        decl_bracket_depth++;
    } else if( CurToken == T_RIGHT_BRACKET ) {
        decl_bracket_depth--;
    }

    if( ( CurToken == T_SEMI_COLON ) || (CurToken == T_LEFT_BRACE ) ) {
        CurToken = T_EOF;
    }

    if( ( decl_paren_depth == 0 ) && ( decl_bracket_depth == 0 ) && ( CurToken == T_COLON ) ) {
        CurToken = T_EOF;
    }
}

DECL_INFO *ReparseFunctionDeclaration( REWRITE *defn )
/****************************************************/
{
    int t;
    PARSE_STACK decl_state;
    p_action what;
    DECL_INFO *dinfo;
    REWRITE *last_rewrite;
    REWRITE *save_token;
    PTREE save_tree;
    int save_yytoken;
    void (*last_source)( void );
    auto error_state_t check;
    auto TOKEN_LOCN locn;

    if( defn == NULL ) {
        return( NULL );
    }

    /* Note that errors are suppressed during re-parsing. This is
     * because an error should only result in the candidate function
     * to be ignored.
     */
    CErrSuppress( &check );
    declOnlyReset();

    save_token = RewritePackageToken();
    save_yytoken = currToken;
    save_tree = yylval.tree;
    yylval.tree = NULL;
    SrcFileGetTokenLocn( &locn );
    ParseFlush();

    LinkagePushCpp();
    last_source = SetTokenSource( declOnlyRewriteToken );
    last_rewrite = RewriteRewind( defn );

    newExprStack( &decl_state, Y_FUNCTION_DECL_SPECIAL );
    syncLocation();
    pushDefaultDeclSpec( &decl_state );

    /* do parse */
    for(;;) {
        do {
            t = yylex( &decl_state );
            what = doAction( t, &decl_state );
        } while( what == P_RELEX );
        if( what != P_SHIFT ) break;
        nextYYLexToken( &decl_state );
    }

    RewriteClose( last_rewrite );
    ResetTokenSource( last_source );

    dinfo = NULL;
    if( what <= P_SPECIAL ) {
        dinfo = decl_state.vsp->dinfo;
    }
    deleteStack( &decl_state );
    LinkagePop();

    SrcFileResetTokenLocn( &locn );
    RewriteRestoreToken( save_token );
    ParseFlush();
    syncLocation();

    currToken = save_yytoken;
    yylval.tree = save_tree;

    if( CErrSuppressedOccurred( &check ) ) {
        if( dinfo != NULL ) {
            FreeDeclInfo( dinfo );
            dinfo = NULL;
        }
    }

    return dinfo;
}

pch_status PCHWriteParserData( void )
{
    return( PCHCB_OK );
}

pch_status PCHReadParserData( void )
{
    if( currParseStack != NULL ) {
        currParseStack->reset_scope = GetFileScope();
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
