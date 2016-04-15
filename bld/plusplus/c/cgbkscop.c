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
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "ring.h"
#include "carve.h"
#include "vstk.h"
#include "callgrph.h"
#include "cdopt.h"
#include "reposit.h"
#include "scoperes.h"
#include "dumpapi.h"


//typedef struct unr_usage        UNR_USAGE;          // unresolved usage
typedef struct scope_res        SCOPE_RES;          // unresolved scope
typedef struct res_act          RES_ACT;            // resolution actions

#define USAGE_DEFS                  /* unresolved usages                */ \
                                    /* - function usages:               */ \
 USAGE_DEF( FNUSE_CALL           )  /* - - call in a scope              */ \
,USAGE_DEF( FNUSE_CALL_TEMP      )  /* - - call in statement scope      */ \
,USAGE_DEF( FNUSE_CALL_SCOPE     )  /* - - call in block scope          */ \
,USAGE_DEF( FNUSE_CALL_CTOR      )  /* - - ctor call in statement scope */ \
,USAGE_DEF( FNUSE_SCOPE          )  /* - - scope resolution req'd       */ \
                                    /* - scope usages:                  */ \
,USAGE_DEF( SCUSE_DTOR_BLK       )  /* - - dtor: block                  */ \
,USAGE_DEF( SCUSE_DTOR_TEMP      )  /* - - dtor: temporary              */ \
,USAGE_DEF( SCUSE_DTOR_COMPONENT )  /* - - dtor: component              */

#define RES_DEFS        /* type of resolution                           */ \
 RES_DEF( RES_FN_TH )   /* - function resolved to be SF_LONGJUMP        */ \
,RES_DEF( RES_FN_NT )   /* - function resolved to be SF_NO_LONGJUMP     */ \
,RES_DEF( RES_SC_SG )   /* - scope resolved as STAB_GEN                 */ \
,RES_DEF( RES_SC_NG )   /* - scope resolved as ~ STAB_GEN               */

typedef enum {                  // UNR_USE -- unresolved usage
    #define USAGE_DEF(a) a
    USAGE_DEFS
    #undef USAGE_DEF
,   MAX_USAGE_DEF
} UNR_USE;

typedef enum {                  // RES_TYPE -- type of resolution
    #define RES_DEF(a) a
    RES_DEFS
    #undef RES_DEF
,   MAX_RES_DEF
} RES_TYPE;

struct unr_usage                // UNR_USAGE -- unresolved usage
{   UNR_USAGE* next;            // - next in ring
    SCOPE scope;                // - scope referenced
    union {                     // - one of:
        CALLNODE* node;         // - - CALLNODE for function used
        SYMBOL fun;             // - - SYMBOL for function used
        SCOPE_RES* res_scope;   // - - scope to be resolved
        void* unresolved;       // - - general entry
    } u;
    UNR_USE type;               // - type of usage
    unsigned :0;                // - alignment
};

struct scope_res                // SCOPE_RES -- unresolved scope
{   SCOPE_RES* next;            // - next scope resolution
    SCOPE_RES* enclosing;       // - enclosing scope
    SCOPE scope;                // - the scope in question
    UNR_USAGE* unresolved;      // - ring of unresolved
    int toresolve;              // - # items to be resolved
    CALLNODE* func;             // - function containing scope
    DT_METHOD dtm;              // - function dtor method
    unsigned : 0;               // - alignment
    unsigned dtorables;         // - # dtorable items in scope
    unsigned statement : 1;     // - true ==> is statement scope
    unsigned dtor_ct : 1;       // - true ==> has can-throw dtorable
    unsigned call_ct : 1;       // - true ==> has can-throw call
    unsigned scanning : 1;      // - true ==> is being scanned now
    unsigned gen_stab : 1;      // - true ==> scan detected state-table req'd
    unsigned scope_throw : 1;   // - true ==> IC_SCOPE_THROW in scope
    unsigned : 0;               // - alignment
};

struct res_act                  // RES_ACT -- resolution action
{   RES_TYPE type;              // - type of resolution
    unsigned :0;                // - alignment
    union                       // - one of:
    {   CALLNODE* node;         // - - unresolved function
        SCOPE_RES* scope;       // - - unresolved scope
    } u;
};

static VSTK_CTL actions;        // actions pending
static carve_t carveUsage;      // carver: scope uses
static carve_t carveScRes;      // carver: unresolved scopes
static VSTK_CTL open_scopes;    // open scopes
static SCOPE_RES* scopes;       // unresolved scopes


#ifndef NDEBUG

#include "dbg.h"
#include "pragdefn.h"

static char const * usage_names[] = {
    #define USAGE_DEF(a) # a
    USAGE_DEFS
    #undef USAGE_DEF
};

static char const * res_names[] = {
    #define RES_DEF(a) # a
    RES_DEFS
    #undef RES_DEF
};

#define strcase( v ) case v: name = #v; break;

static void _print( char const * msg )
{
    if( PragDbgToggle.callgraph_scan ) {
        printf( msg );
    }
}

static char const* _res_type( RES_TYPE type )
{
    return ( type < MAX_RES_DEF ) ? res_names[ type ] : "BAD RES_TYPE";
}

static char const* _unr_use( UNR_USE type )
{
    return ( type < MAX_USAGE_DEF ) ? usage_names[ type ] : "BAD UNR_USE";
}

static void _printAction( RES_ACT const * ra, char const * msg )
{
    if( PragDbgToggle.callgraph_scan && ra != NULL ) {
        printf( "RES_ACT[%p] %s %p %s\n"
              , ra
              , _res_type( ra->type )
              , ra->u.node
              , msg );
    }
}

static void _printScopeRes( SCOPE_RES const *sr, char const * msg )
{
    if( PragDbgToggle.callgraph_scan ) {
        printf( "SCOPE_RES[%p] %s\n"
                "  next[%p] enclosing[%p] scope[%p] unresolved[%p]\n"
                "  toresolve[%d] func[%p] dtm[%x]\n"
                "  statement-%d dtor_ct-%d call_ct-%d scanning-%d gen_stab-%d"
                " scope_throw-%d\n"
              , sr
              , msg
              , sr->next
              , sr->enclosing
              , sr->scope
              , sr->unresolved
              , sr->toresolve
              , sr->func
              , sr->dtm
              , sr->statement
              , sr->dtor_ct
              , sr->call_ct
              , sr->scanning
              , sr->gen_stab
              , sr->scope_throw );
    }
}


static void _printUnrUsage( UNR_USAGE const *fu, char const * msg )
{
    if( PragDbgToggle.callgraph_scan ) {
        printf( "UNR_USAGE[%p] %s %p %s\n"
              , fu
              , _unr_use( fu->type )
              , fu->u.node
              , msg );
    }
}


static void _printScopeResAll( SCOPE_RES const *sr, char const * msg )
{
    UNR_USAGE* su;
    if( PragDbgToggle.callgraph_scan ) {
        _printScopeRes( sr, msg );
        RingIterBeg( sr->unresolved, su ) {
            _printUnrUsage( su, msg );
        } RingIterEnd( su );
    }
}

static void _printFunction( SYMBOL fun, char const * msg )
{
    if( PragDbgToggle.callgraph_scan ) {
        VBUF vbuf;
        printf( "%s [%p] %s\n"
              , msg
              , fun
              , DbgSymNameFull( fun, &vbuf ) );
        VbufFree( &vbuf );
    }
}

static bool _printCallNode
    ( CALLGRAPH* ctl
    , CALLNODE* node )
{
    UNR_USAGE *fu;
    ctl = ctl;
    if( PragDbgToggle.callgraph_scan ) {
        VBUF vbuf;
        printf( "CALLNODE[%p] unresolved[%p] %s\n"
              , node
              , node->unresolved
              , DbgSymNameFull( node->base.object, &vbuf ) );
        RingIterBeg( node->unresolved, fu ) {
            _printUnrUsage( fu, "" );
        } RingIterEnd( fu );
        VbufFree( &vbuf );
    }
    return false;
}

#define _printAction1   _printAction
#define _printUnrUsage1 _printUnrUsage

#else

#define _print(a)
#define _res_type(a)
#define _unr_use(a)
#define _printAction(a,b)
#define _printAction1(a,b)      a
#define _printScopeRes(a,b)
#define _printScopeResAll(a,b)
#define _printFnUsage(a,b)
#define _printScUsage(a,b)
#define _printFunction(a,b)
#define _printCallNode(a,b)
#define _printUnrUsage(a,b)
#define _printUnrUsage1(a,b)    a

#endif


static SYMBOL symDefaultBase(   // GET ACTUAL SYMBOL FOR A DEFAULT
    SYMBOL fun )                // - function symbol
{
    if( fun != NULL ) {
        fun = SymDefaultBase( fun );
    }
    return fun;
}


static SCOPE_RES* openScopesTop // GET TOP OF OPEN-SCOPES STACK
    ( void )
{
    SCOPE_RES** a_sr;           // - addr[ unresolved scope ]
    SCOPE_RES* sr;              // - unresolved scope

    a_sr = VstkTop( &open_scopes );
    if( a_sr == NULL ) {
        sr = NULL;
    } else {
        sr = *a_sr;
    }
    return sr;
}


static SCOPE_RES* openScopesPush // PUSH OPEN-SCOPES STACK
    ( void )
{
    SCOPE_RES** a_sr;           // - addr[ unresolved scope ]
    SCOPE_RES* sr;              // - unresolved scope
    SCOPE_RES* enclosing;       // - enclosing scope

    enclosing = openScopesTop();
    a_sr = VstkPush( &open_scopes );
    sr = CarveAlloc( carveScRes );
    *a_sr = sr;
    sr->enclosing = enclosing;
    if( NULL != enclosing ) {
        ++ enclosing->toresolve;
        sr->scope_throw = enclosing->scope_throw;
    } else {
        sr->scope_throw = false;
    }
    return sr;
}


static SCOPE_RES* openScopesPop // POP OPEN-SCOPES STACK
    ( void )
{
    SCOPE_RES** a_sr;           // - addr[ unresolved scope ]
    SCOPE_RES* sr;              // - unresolved scope

    a_sr = VstkPop( &open_scopes );
    if( a_sr == NULL ) {
        sr = NULL;
    } else {
        sr = *a_sr;
    }
    return sr;
}


#define OpenScopesIterBeg( it )         \
    { SCOPE_RES** a_sr;                 \
      VstkIterBeg( &open_scopes, a_sr ) { \
        it = *a_sr;
#define OpenScopesIterEnd( it )         \
      }                                 \
    }


static RES_ACT* pushAction      // PUSH ACTION
    ( RES_TYPE type )           // - type of action
{
    RES_ACT* res = VstkPush( &actions );
    res->type = type;
    return res;
}


static RES_ACT* pushActionCaller// PUSH FUNCTION-RELATED ACTION
    ( CALLNODE* node            // - call node
    , RES_TYPE type )           // - type of action
{
    RES_ACT* res;               // - new entry

    if( NULL == node->unresolved ) {
        res = NULL;
    } else {
        res = pushAction( type );
        res->u.node = node;
    }
    return res;
}


static CALLNODE* makeThrowFun   // FUNCTION BECOMES A THROWING FUNCTION
    ( CALLNODE* owner )         // - owner
{
    SYMBOL fun;                 // - owner symbol

    fun = owner->base.object;
    fun = symDefaultBase( fun );
    DbgVerify( (fun->flag & SF_NO_LONGJUMP) == 0, "makeThrowFun -- has SF_NO_LONGJUMP" );
    if( (fun->flag & SF_LONGJUMP) == 0 ) {
        fun->flag |= SF_LONGJUMP;
        _printAction1( pushActionCaller( owner, RES_FN_TH ), "Function resolved: throwable" );
        CgResolve();
    }
    return owner;
}


static CALLNODE* makeNonThrowFun// FUNCTION BECOMES A NON-THROWING FUNCTION
    ( CALLNODE* owner )         // - owner
{
    SYMBOL fun;                 // - owner symbol

    fun = owner->base.object;
    fun = symDefaultBase( fun );
    DbgVerify( (fun->flag & SF_LONGJUMP) == 0, "makeNonThrowFun -- has SF_LONGJUMP" );
    if( (fun->flag & SF_NO_LONGJUMP) == 0 ) {
        fun->flag |= SF_NO_LONGJUMP;
        _printAction1( pushActionCaller( owner, RES_FN_NT ), "Function resolved: non-throwable" );
        CgResolve();
    }
    return owner;
}


static SCOPE_RES* markScopeGen  // MARK SCOPE AS GENERATED
    ( SCOPE_RES* sr )           // - unresolved scope
{
    SCOPE_RES* enc;             // - walks thru enclosing scopes

    for( enc = sr; ; enc = enc->enclosing ) {
        DbgVerify( enc != NULL, "makeScopeGen -- no scopes" );
        enc->gen_stab = true;
#ifndef NDEBUG
        if( enc != sr ) {
            _printScopeRes( enc, "enclosed scope made genable" );
        }
#endif
        if( enc->scope != NULL ) {
            enc->scope->u.s.cg_stab = true;
            break;
        }
    }
    CgrfMarkNodeGen( sr->func );
    return sr;
}


static SCOPE_RES* scopeResolve  // COMPLETE SCOPE RESOLUTION, IF POSSIBLE
    ( SCOPE_RES* sr )           // - scope to be resolved
{
    if( 0 == sr->toresolve && ! sr->scanning ) {
        if( ! sr->gen_stab ) {
            UNR_USAGE* su;      // - current usage
            bool thrdt;         // - throwable dtor found
            thrdt = false;
            RingIterBeg( sr->unresolved, su ) {
                switch( su->type ) {
                  case SCUSE_DTOR_BLK :
                  case SCUSE_DTOR_TEMP :
                  case SCUSE_DTOR_COMPONENT :
                    if( 0 != thrdt ) {
                        sr = markScopeGen( sr );
                        _printScopeRes( sr, "scope genable, throwable not last" );
                        break;
                    }
                    if( su->u.fun->flag & SF_LONGJUMP ) {
                        thrdt = 1;
                    }
                    // drops thru
                  default :
                    continue;
                }
                break;
            } RingIterEnd( su );
        }
        if( sr->gen_stab ) {
            RES_ACT* res = pushAction( RES_SC_SG );
            res->u.scope = sr;
            _printAction( res, "Scope resolved: gen" );
            CgResolve();
        } else {
            RES_ACT* res = pushAction( RES_SC_NG );
            res->u.scope = sr;
            _printAction( res, "Scope resolved: no-gen" );
            CgResolve();
        }
    }
    return sr;
}


static SCOPE_RES* makeScopeGen  // SCOPE BECOMES GENERATED
    ( SCOPE_RES* sr )           // - unresolved scope
{
    sr = markScopeGen( sr );
    return scopeResolve( sr );
}


static SCOPE_RES* newScope      // ALLOCATE A NEW SCOPE TO BE RESOLVED
    ( SCOPE scope               // - scope
    , CALLNODE* caller          // - caller
    , DT_METHOD dtm )           // - dtor method for function
{
    SCOPE_RES* sr;              // - unresolved scope

    sr = openScopesPush();
    sr->next = NULL;
    sr->scope = scope;
    sr->dtm = dtm;
    sr->func = caller;
    sr->unresolved = NULL;
    sr->toresolve = 0;
    sr->dtorables = 0;
    sr->statement = false;
    sr->dtor_ct = false;
    sr->call_ct = false;
    sr->scanning = true;
    sr->gen_stab = false;
    return sr;
}


static void freeScope           // FREE A COMPLETED SCOPE RESOLUTION
    ( SCOPE_RES* res )          // - resolved scope
{
    RingPrune( &scopes, res );
    CarveFree( carveScRes, res );
}


void CgResScBlkScanBegin        // START SCANNING OF A BLOCK SCOPE
    ( SCOPE scope               // - scope
    , CALLNODE* caller          // - caller
    , DT_METHOD dtm )           // - dtor method for function
{
    SCOPE_RES* sr = newScope( scope, caller, dtm );
    sr = sr;
    _printScopeRes( sr, "Start scanning block scope" );
}


void CgResScStmtScanBegin       // START SCANNING OF A STATEMENT SCOPE
    ( SCOPE scope               // - scope
    , CALLNODE* caller          // - caller
    , DT_METHOD dtm )           // - dtor method for function
{
    SCOPE_RES* sr = newScope( scope, caller, dtm );
    sr->statement = true;
    _printScopeRes( sr, "Start scanning statement scope" );
}


SCOPE CgResScScanEnd            // COMPLETE SCANNING OF A SCOPE
    ( void )                    //  ( block or statement scope )
{
    SCOPE_RES* sr;              // - scope completed
    SCOPE_RES* top;             // - enclosing scope or NULL
    SCOPE retn;                 // - current scope

    sr = openScopesPop();
    DbgVerify( sr != NULL, "Scope is NULL" );
    sr->scanning = false;
    _printScopeRes( sr, "Complete scanning scope" );
    RingAppend( &scopes, sr );
    top = sr->enclosing;
    if( top == NULL ) {
        retn = NULL;
    } else {
        if( sr->statement && sr->gen_stab ) {
            makeScopeGen( top );
            _printScopeRes( top, "block scope made genable by stmt scope" );
        }
        retn = top->scope;
    }
    scopeResolve( sr );
    return retn;
}


void CgResScopeGen              // FORCE GENERATION OF CURRENT SCOPE
    ( CALLNODE* owner )         // - function being scanned
{
    SCOPE_RES* sr;              // - current scope

    owner->state_table = true;
    sr = openScopesTop();
    makeScopeGen( sr );
    _printScopeRes( sr, "Force scope gen" );
}


static bool resolveSymbol       // RESOLVE FUNCTION SYMBOL, IF POSSIBLE
    ( SYMBOL fun                // - the function
    , CALLNODE** a_callnode )   // - addr[ NULL or CALLNODE for function ]
{
    bool retn;                  // - return: true ==> process it

    *a_callnode = NULL;
    if( NULL == fun ) return true;
    _printFunction( fun, "resolve:" );
    fun = symDefaultBase( fun );
    if( fun->flag & SF_FN_LONGJUMP ) {
        _print( (fun->flag & SF_LONGJUMP) ? "throwable\n" : "non-throwable\n" );
        retn = true;
    } else {
        if( SymIsInitialized( fun ) ) {
            _print( "not resolved\n" );
            *a_callnode = CgrfCallNode( fun );
            retn = true;
        } else {
            TYPE* except_spec = SymFuncArgList( fun )->except_spec;
            if( NULL != except_spec
             && NULL == *except_spec ) {
                fun->flag |= SF_NO_LONGJUMP;
                _print( "non-throwable with throw()\n" );
                return true;
            } else {
                REPO_REC_FUN* frec = RepoFunRead( fun->name->name );
                if( NULL == frec ) {
                    _print( "assumed throwable, called not initialized\n" );
                    fun->flag |= SF_LONGJUMP;
                    retn = true;
                } else {
                    _print( "deleted: repository has IG_LONGJUMP\n" );
                    DbgVerify( frec->flags & RFFLAG_IG_LONGJUMP
                             , "resolveSymbol -- bad repository function" );
                    RepoFunRelease( frec );
                    retn = false;
                }
            }
        }
    }
    return retn;
}


static UNR_USAGE* addUsage      // ADD A USAGE ENTRY
    ( UNR_USE type              // - type of usage
    , UNR_USAGE** a_hdr         // - addr[ ring hdr ]
    , void* unresolved )        // - unresolved ptr
{
    UNR_USAGE* use = RingCarveAlloc( carveUsage, a_hdr );
    use->type = type;
    use->u.unresolved = unresolved;
    use->scope = NULL;
    return use;
}


static UNR_USAGE* addFnUsage    // ADD A USAGE FOR A FUNCTION
    ( UNR_USE type              // - type of usage
    , CALLNODE* node            // - CALLNODE for function
    , void* unresolved )        // - unresolved ptr
{
    return addUsage( type, &node->unresolved, unresolved );
}


static void addFnUsageSc        // SCOPE PROCESSING AFTER FN RESOLUTION
    ( CALLNODE* node            // - node for unresolved function
    , SCOPE_RES* sr )           // - affected scope
{
    ++sr->toresolve;
    _printUnrUsage1( addFnUsage( FNUSE_SCOPE, node, sr ), "function/scope resolution" );
}


static UNR_USAGE* addScUsage    // ADD A USAGE FOR A SCOPE
    ( UNR_USE type              // - type of usage
    , SCOPE_RES* sr             // - unresolved scope
    , void* unresolved )        // - unresolved ptr
{
    return addUsage( type, &sr->unresolved, unresolved );
}


static void resolvedCallInScope // PROCESS A RESOLVED CALL FOR A SCOPE
    ( SYMBOL called             // - called
    , SCOPE_RES* sr )           // - scope
{
    if( ! sr->gen_stab ) {
        if( called->flag & SF_LONGJUMP ) {
            if( sr->dtorables > 0 || sr->scope_throw ) {
                makeScopeGen( sr );
                _printScopeRes( sr, "throwable call after dtorables" );
            } else {
                sr->call_ct = true;
                _printScopeRes( sr, "throwable call before dtorables" );
            }
        }
    }
}


static void resolvedCallInStmt  // PROCESS A RESOLVED CALL FOR A STATEMENT
    ( SYMBOL called             // - called
    , SCOPE_RES* sr )           // - scope
{
    called = symDefaultBase( called );
    if( ! sr->gen_stab ) {
        if( called->flag & SF_LONGJUMP ) {
            if( sr->dtorables > 0 || sr->scope_throw ) {
                makeScopeGen( sr );
                _printScopeRes( sr, "throwable stmt call, dtorables" );
            } else {
                sr->call_ct = true;
                _printScopeRes( sr, "throwable stmt call, no dtorables" );
            }
        }
    }
}


static void resolvedCtorInStmt  // PROCESS A RESOLVED CTOR CALL FOR A STMT
    ( SYMBOL called             // - called
    , SCOPE_RES* sr )           // - scope
{
    called = symDefaultBase( called );
    if( ! sr->gen_stab ) {
        if( called->flag & SF_LONGJUMP ) {
            if( sr->dtorables > 1 ) {
                makeScopeGen( sr );
                _printScopeRes( sr, "throwable stmt ctor, dtorables" );
            } else {
                sr->call_ct = true;
                _printScopeRes( sr, "throwable stmt ctor, no dtorables" );
            }
        }
    }
}


static void resolveCall         // RESOLVE A CALL, IF POSSIBLE
    ( CALLNODE* caller          // - caller
    , SYMBOL fun                // - function
    , CALLNODE* called )        // - NULL or node for unresolved function
{
    fun = symDefaultBase( fun );
    if( fun->flag & SF_LONGJUMP ) {
        makeThrowFun( caller );
    } else if( fun->flag & SF_NO_LONGJUMP ) {
        // do nothing
    } else {
        _printUnrUsage1( addFnUsage( FNUSE_CALL, called, caller ), "unresolved call" );
    }
}


static void resolveCallEnc      // RESOLVE ENCLOSED CALL, IF POSSIBLE
    ( SCOPE_RES* sr             // - scope enclosing call
    , CALLNODE* called          // - node for "fun" if unresolved
    , SYMBOL fun )              // - function called
{
    if( 0 != sr->dtorables ) {
        fun = symDefaultBase( fun );
        if( called == NULL ) {
            resolvedCallInScope( fun, sr );
        } else {
            _printUnrUsage1( addFnUsage( FNUSE_CALL_SCOPE, called, sr ), "call after dtorable" );
            addFnUsageSc( called, sr );
        }
    }
}


void CgResCall                  // ADD: CALL TO RESOLVE
    ( CALLNODE* caller          // - caller
    , SYMBOL fun )              // - function
{
    SCOPE_RES* sr;              // - an open scope
    CALLNODE* called;           // - NULL or node for unresolved function

    fun = symDefaultBase( fun );
    if( resolveSymbol( fun, &called ) && (fun->flag & SF_NO_LONGJUMP) == 0 ) {
        resolveCall( caller, fun, called );
        OpenScopesIterBeg( sr ) {
            if( sr->statement ) {
                if( called == NULL ) {
                    resolvedCallInStmt( fun, sr );
                } else {
                    _printUnrUsage1( addFnUsage( FNUSE_CALL_TEMP, called, sr ), "call in statement" );
                    addFnUsageSc( called, sr );
                }
            } else {
                resolveCallEnc( sr, called, fun );
            }
        } OpenScopesIterEnd( sr );
    }
}


static void resolvedDtor        // PROCESS A RESOLVED DTOR
    ( SYMBOL dtor               // - the destructor
    , SCOPE_RES* sr )           // - it's scope
{
    if( ! sr->gen_stab ) {
        dtor = symDefaultBase( dtor );
        if( dtor->flag & SF_LONGJUMP ) {
            if( sr->dtor_ct || sr->scope_throw ) {
                makeScopeGen( sr );
                _printScopeRes( sr, "dtor made scope genable" );
            } else {
                sr->dtor_ct = true;
                _printScopeRes( sr, "first throwable dtor in scope" );
            }
        }
        ++ sr->dtorables;
    }
}


// add:
//  FNUSE_SCOPE -- for function, to scope
//  SCUSE_DTOR_... - for scope, referencing function
//
static void unresolvedDtor      // PROCESS AN UNRESOLVED DTOR
    ( SYMBOL dtor               // - the destructor
    , CALLNODE* dtornode        // - node for unresolved dtor
    , SCOPE_RES* sr             // - it's scope
    , UNR_USE type )            // - type of scope usage
{
    _printUnrUsage1( addScUsage( type, sr, symDefaultBase( dtor ) ), "unresolved dtor usage" );
    addFnUsageSc( dtornode, sr );
}


static void resolvedDtorBlk     // PROCESS A RESOLVED DTOR IN SCOPE
    ( SYMBOL dtor               // - the destructor
    , SCOPE_RES* sr )           // - it's scope
{
    dtor = symDefaultBase( dtor );
    if( dtor->flag & SF_LONGJUMP ) {
        if( sr->dtor_ct || sr->scope_throw ) {
            makeScopeGen( sr );
            _printScopeRes( sr, "dtor made scope genable" );
        } else {
            sr->dtor_ct = true;
            _printScopeRes( sr, "first throwable dtor in scope" );
        }
    }
}


static void resolveDtorBlk      // ADD: DTOR IN SCOPE TO RESOLVE
    ( SYMBOL fun )              // - function (DTOR)
{
    SCOPE_RES* sr;              // - an open scope
    bool top_scope;             // - true ==> is top scope
    CALLNODE* called;           // - NULL or node for unresolved dtor

    fun = symDefaultBase( fun );
    if( resolveSymbol( fun, &called ) ) {
        sr = openScopesTop();
        DbgVerify( NULL != sr, "CgResDtorBlk -- no scope" );
        if( fun->flag & SF_FN_LONGJUMP ) {
            resolvedDtorBlk( fun, sr );
            _printUnrUsage1( addScUsage( SCUSE_DTOR_BLK, sr, fun ), "resolved blk dtor" );
        } else {
            unresolvedDtor( fun, called, sr, SCUSE_DTOR_BLK );
        }
        ++ sr->dtorables;
        top_scope = true;
        OpenScopesIterBeg( sr ) {
            if( ! top_scope ) {
                DbgVerify( ! sr->statement, "CgResDtorBlk -- statement scope" );
                resolveCallEnc( sr, called, fun );
            }
            top_scope = false;
        } OpenScopesIterEnd( sr );
    }
}


static void resolveDtorStmt     // ADD: DTOR IN STATEMENT TO RESOLVE
    ( CALLNODE* owner           // - function being scanned
    , SYMBOL ctor               // - function (CTOR)
    , SYMBOL dtor )             // - function (DTOR)
{
    SCOPE_RES* sr;              // - an open scope
    bool top_scope;             // - true ==> is top scope
    CALLNODE* dtornode;         // - NULL or node for unresolved dtor
    CALLNODE* ctornode;         // - NULL or node for unresolved ctor

    dtor = symDefaultBase( dtor );
    ctor = symDefaultBase( ctor );
    if( ! resolveSymbol( dtor, &dtornode ) ) {
        CgResCall( owner, ctor );
    } else {
        sr = openScopesTop();
        DbgVerify( NULL != sr, "CgResDtorStmt -- no scope" );
        DbgVerify( sr->statement, "CgResDtorStmt -- no statement scope" );
        if( dtor->flag & SF_FN_LONGJUMP ) {
            resolvedDtor( dtor, sr );
            _printUnrUsage1( addScUsage( SCUSE_DTOR_TEMP, sr, dtor ), "resolved stmt dtor" );
        } else {
            unresolvedDtor( dtor, dtornode, sr, SCUSE_DTOR_TEMP );
        }
        ++ sr->dtorables;
        if( ctor == NULL ) {
            ctornode = NULL;
        } else {
            resolveSymbol( ctor, &ctornode );
            resolveCall( owner, ctor, ctornode );
        }
        top_scope = true;
        OpenScopesIterBeg( sr ) {
            if( top_scope ) {
                if( ctor != NULL ) {
                    if( ctornode == NULL ) {
                        resolvedCtorInStmt( ctor, sr );
                    } else {
                        _printUnrUsage1( addFnUsage( FNUSE_CALL_CTOR, ctornode, sr ), "ctor in statement" );
                        addFnUsageSc( ctornode, sr );
                    }
                }
                top_scope = false;
            } else {
                DbgVerify( ! sr->statement, "CgResDtorBlk -- statement scope" );
                if( NULL != ctor ) {
                    resolveCallEnc( sr, ctornode, ctor );
                }
                resolveCallEnc( sr, dtornode, dtor );
            }
        } OpenScopesIterEnd( sr );
    }
}


static void resolveDtorComponent// ADD: DTOR IN COMPONENT TO RESOLVE
    ( SYMBOL dtor )             // - function (DTOR)
{
    SCOPE_RES* sr;              // - an open scope
    CALLNODE* dtornode;         // - callnode for dtor

    dtor = symDefaultBase( dtor );
    if( resolveSymbol( dtor, &dtornode ) ) {
        sr = openScopesTop();
        DbgVerify( NULL != sr, "CgResDtorComponent -- no scope" );
        for( ; sr->statement; sr = sr->enclosing );
//      DbgVerify( ! sr->statement, "CgResDtorComponent -- statement scope" );
        if( dtor->flag & SF_FN_LONGJUMP ) {
            resolvedDtor( dtor, sr );
            _printUnrUsage1( addScUsage( SCUSE_DTOR_COMPONENT, sr, dtor ), "resolved component dtor" );
        } else {
            _print( "component dtor" );
            unresolvedDtor( dtor, dtornode, sr, SCUSE_DTOR_COMPONENT );
        }
        ++ sr->dtorables;
    }
}


void CgResScopeCall             // RESOLVING SCOPE-CALL
    ( CALLNODE* owner           // - function being scanned
    , SYMBOL ctor               // - NULL or ctoring function
    , SYMBOL dtor_component     // - NULL or dtor: component
    , SYMBOL dtor_temporary     // - NULL or dtor: temporary
    , SYMBOL dtor_scope )       // - NULL or dtor: scope
{
    if( NULL != dtor_scope ) {
        if( NULL != ctor ) {
            CgResCall( owner, ctor );
        }
        resolveDtorBlk( dtor_scope );
    } else if( NULL != dtor_temporary ) {
        SCOPE_RES* sr = openScopesTop();
        if( sr->statement ) {
            resolveDtorStmt( owner, ctor, dtor_temporary );
        } else {
            CgResScopeCall( owner, ctor, NULL, NULL, dtor_temporary );
        }
    } else if( NULL != dtor_component ) {
        if( NULL != ctor ) {
            CgResCall( owner, ctor );
        }
        resolveDtorComponent( dtor_component );
    } else {
        if( NULL != ctor ) {
            CgResCall( owner, ctor );
        }
    }
}


void CgResScopeThrow            // SCOPE CONTAINS THROW
    ( CALLNODE* owner )         // - owner
{
    SCOPE_RES* sr;              // - current active scope

    owner = makeThrowFun( owner );
    OpenScopesIterBeg( sr ) {
        if( sr->dtorables > 0 ) {
            makeScopeGen( sr );
            _printScopeRes( sr, "throw made scope genable" );
        }
        sr->scope_throw = true;
    } OpenScopesIterEnd( sr )
}


static void resolveFunction     // RESOLUTIONS FOR FUNCTION
    ( CALLNODE* node )          // - call node
{
    SYMBOL fun;                 // - resolved function
    UNR_USAGE* fu;              // - current usage

    fun = node->base.object;
    fun = symDefaultBase( fun );
    RingIterBegSafe( node->unresolved, fu ) {
        switch( fu->type ) {
          DbgDefault( "resolveFunction -- bad resolution code" );
          case FNUSE_CALL :
            if( fun->flag & SF_LONGJUMP ) {
                makeThrowFun( fu->u.node );
            }
            break;
          case FNUSE_SCOPE :
          { SCOPE_RES* sr = fu->u.res_scope;
            -- sr->toresolve;
            scopeResolve( sr );
          } break;
          case FNUSE_CALL_TEMP :
            resolvedCallInStmt( fun, fu->u.res_scope );
            break;
          case FNUSE_CALL_SCOPE :
            resolvedCallInScope( fun, fu->u.res_scope );
            break;
          case FNUSE_CALL_CTOR :
            resolvedCtorInStmt( fun, fu->u.res_scope );
            break;
        }
    } RingIterEndSafe( fu );
    RingCarveFree( carveUsage, &node->unresolved );
}


static void resolveScopeFini    // COMPLETE RESOLVE OF SCOPE
    ( SCOPE_RES* sr )           // - resolved scope
{
    SCOPE_RES* enclosing;       // - enclosing scope

    enclosing = sr->enclosing;
    RingCarveFree( carveUsage, &sr->unresolved );
    freeScope( sr );
    if( NULL != enclosing ) {
        -- enclosing->toresolve;
        scopeResolve( enclosing );
    }
}


static void resolveScopeGenned  // RESOLUTIONS FOR SCOPE WITH STATE TABLE
    ( SCOPE_RES* sr )           // - resolved scope
{
    UNR_USAGE* su;              // - current scope usage
    CALLNODE* node;             // - function for scope
    DT_METHOD dtm;              // - default function dtor method

    _printScopeResAll( sr, "Resolved as gen" );
//  sr->scope->cg_stab = true;
    dtm = sr->dtm;
    node = sr->func;
    CgrfMarkNodeGen( node );
    RingIterBeg( sr->unresolved, su ) {
        switch( su->type ) {
          DbgDefault( "resolveScopeGenned -- bad resolution" );
          case SCUSE_DTOR_BLK :
            if( dtm == DTM_DIRECT_TABLE ) {
                CgrfDtorCall( node, su->u.fun );
                _printFunction( su->u.fun, "call added" );
            }
            // drops thru
          case SCUSE_DTOR_TEMP :
          case SCUSE_DTOR_COMPONENT :
            CgrfDtorAddr( node, su->u.fun );
            _printFunction( su->u.fun, "addr taken" );
            break;
        }
    } RingIterEnd( su );
    resolveScopeFini( sr );
}


static void resolveScopeNoGen   // RESOLUTIONS FOR SCOPE WITHOUT STATE TABLE
    ( SCOPE_RES* sr )           // - resolved scope
{
    UNR_USAGE* su;              // - current scope usage
    CALLNODE* node;             // - function for scope
    DT_METHOD dtm;              // - default function dtor method

    _printScopeResAll( sr, "Resolved as no-gen" );
    dtm = DtmDirect( sr->dtm );
    node = sr->func;
    RingIterBeg( sr->unresolved, su ) {
        switch( su->type ) {
          DbgDefault( "resolveScopeNoGen -- bad resolution" );
          case SCUSE_DTOR_BLK :
            if( dtm == DTM_DIRECT ) {
                CgrfDtorCall( node, su->u.fun );
                _printFunction( su->u.fun, "call added" );
            } else {
                CgrfDtorAddr( node, su->u.fun );
                _printFunction( su->u.fun, "addr taken" );
            }
            break;
          case SCUSE_DTOR_TEMP :
          // for now, put temp dtor in enclosing block scope, since
          // the statement scope might be no-gen while the block scope
          // is gen. We don't yet have support for statement scopes in
          // CGBKMAIN (ctor10.c tests this)
#if 0
            CgrfDtorCall( node, su->u.fun );
            _printFunction( su->u.fun, "call added" );
#else
            _printUnrUsage1( addScUsage( SCUSE_DTOR_BLK, sr->enclosing, su->u.fun ), "temp dtor added to block scope" );
#endif
            break;
        case SCUSE_DTOR_COMPONENT :
            break;
        }
    } RingIterEnd( su );
    resolveScopeFini( sr );
}


void CgResolve                  // RESOLVE ANY PENDING ACTIONS
    ( void )
{
    static bool active;         // - true ==> resolution is active

    if( active ) return;
    active = true;
    for( ; ; ) {
        RES_ACT* res = VstkPop( &actions );
        if( NULL == res ) break;
        _printAction( res, "Executing action" );
        switch( res->type ) {
          DbgDefault( "CgResolve -- invalid type" );
          case RES_FN_TH :
          case RES_FN_NT :
            resolveFunction( res->u.node );
            break;
          case RES_SC_SG :
            resolveScopeGenned( res->u.scope );
            break;
          case RES_SC_NG :
            resolveScopeNoGen( res->u.scope );
            break;
        }
    }
    active = false;
}


bool CgResolveNonThrow          // RESOLVE A FUNCTION AS NON-THROW
    ( CALLGRAPH* ctl            // - call graph
    , CALLNODE* node )          // - possible non-throw
{
    ctl = ctl;
    if( ! node->is_vft ) {
        SYMBOL fun = node->base.object;
        if( fun != NULL ) {
            fun = symDefaultBase( fun );
            if( (fun->flag & SF_FN_LONGJUMP) == 0 ) {
                makeNonThrowFun( node );
            }
        }
    }
    return false;
}


void CgResInit                  // INITIALIZATION
    ( void )
{
    scopes = NULL;
    VstkOpen( &actions, sizeof( RES_ACT ), 32 );
    VstkOpen( &open_scopes, sizeof( SCOPE_RES* ), 32 );
    carveUsage = CarveCreate( sizeof( UNR_USAGE ), 128 );
    carveScRes = CarveCreate( sizeof( SCOPE_RES ), 16 );
}


void CgResFini                  // COMPLETION
    ( void )
{
    VstkClose( &actions );
    VstkClose( &open_scopes );
    CarveDestroy( carveUsage );
    CarveDestroy( carveScRes );
}

#ifndef NDEBUG

void DumpUnresolved( void )     // DBG: callable from debugger
{
    SCOPE_RES* sr;              // - unresolved scope

    CgrfWalkFunctions( DbgCallGraph(), &_printCallNode );
    OpenScopesIterBeg( sr ) {
        _printScopeResAll( sr, "open scope" );
    } OpenScopesIterEnd( sr );
    RingIterBeg( scopes, sr ) {
        _printScopeResAll( sr, "unresolved scope" );
    } RingIterEnd( sr );
}

#endif
