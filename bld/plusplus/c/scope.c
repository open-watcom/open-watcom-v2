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
#include <stdio.h>
#include <limits.h>

#include "memmgr.h"
#include "ring.h"
#include "stack.h"
#include "name.h"
#include "hashtab.h"
#include "carve.h"
#include "cgfront.h"
#include "class.h"
#include "decl.h"
#include "dbg.h"
#include "fnovload.h"
#include "cgsegid.h"
#include "vstk.h"
#include "pstk.h"
#include "pragdefn.h"
#include "fnbody.h"
#include "initdefs.h"
#include "pcheader.h"
#include "template.h"
#include "stats.h"
#include "preproc.h"
#include "typesig.h"
#include "brinfo.h"
#include "conpool.h"
#include "fmtsym.h"
#include "vfun.h"
#include "yydriver.h"

#define _ScopeMask( i )         ( 1 << (i) )

#define _IsClassScope( s )      \
        ((s)->id == SCOPE_CLASS)
#define _IsFunctionScope( s )   \
        ((s)->id == SCOPE_FUNCTION)
#define _IsBlockScope( s )   \
        ((s)->id == SCOPE_BLOCK)
#define _IsFileScope( s )   \
        ((s)->id == SCOPE_FILE)

static int hashTableSize[SCOPE_MAX] = {
    #define SCOPE_DEF(a,b) b
    SCOPE_DEFS
    #undef SCOPE_DEF
};

typedef enum file_scope_control {
    FS_GLOBAL           = 0x01,
    FS_UNNAMED          = 0x02,
    FS_NULL             = 0x00
} fs_control;

typedef boolean (*special_name_fn)( SYMBOL_NAME );

typedef struct save_mapping SAVE_MAPPING;
struct save_mapping {
    SAVE_MAPPING        *next;
    SCOPE               from;
    SCOPE               to;
    SYMBOL              sym;
    target_offset_t     map_0;
};

typedef struct base_path BASE_PATH;
struct base_path {
    BASE_PATH           *next;
    BASE_PATH           *prev;
    BASE_CLASS          *base;
    SCOPE               scope;
    inherit_flag        flag;
    unsigned            checked_private : 1;
    unsigned            failed_private : 1;
};

typedef struct symbol_exclude SYMBOL_EXCLUDE;
struct symbol_exclude {
    SYMBOL_EXCLUDE      *next;
    SYMBOL              sym;
};

typedef struct path_cap PATH_CAP;
struct path_cap {
    PATH_CAP            *next;          /* NULL terminated list */
    BASE_PATH           *head;
    BASE_PATH           *tail;
    SCOPE               access_decl;
    SYMBOL_EXCLUDE      *exclude;
    SYMBOL_NAME         sym_name;
    SYMBOL              sym;
    inherit_flag        flag;
    unsigned            throw_away : 1;
    unsigned            across_virtual_base : 1;
};

typedef struct base_stack BASE_STACK;
struct base_stack {
    BASE_STACK          *next;
    BASE_STACK          *parent;
    BASE_CLASS          *base;
    SCOPE               scope;
    SYMBOL              access_changed;
    void                *hold;
    inherit_flag        access_perm;
    unsigned            used : 1;
};

typedef enum {          /* return value for base class walking routines */
    WALK_NORMAL,        /* keep going; nothing special has happened */
    WALK_FINISH,        /* terminate entire walk traversal */
    WALK_ABANDON,       /* abandon this base class and its base classes */
} walk_status;

typedef walk_status (*walk_routine)( BASE_STACK *, void * );

typedef struct {
    SCOPE               derived;
    SCOPE               base;
    unsigned            copies;
    unsigned            virtual_base : 1;
    unsigned            is_derived : 1;
} scope_derived_walk;

typedef struct {
    SCOPE               start;
    GEN_LEAP            *list;
    unsigned            count;
} rtti_leap_walk;

typedef struct {
    char                *name;
    TYPE                found;
} bound_base_walk;

typedef struct {
    SCOPE               found;
    unsigned            copies;
} count_copy_walk;

typedef struct {
    SCOPE               base;
    unsigned            virtual_base : 1;
    unsigned            is_derived : 1;
} derived_walk;

typedef struct {
    PSTK_CTL            common_bases;
    int                 colour;
} common_base_walk;

typedef struct {
    unsigned            depth;
    SCOPE               base;
    unsigned            many_pathes : 1;
} base_depth_walk;

typedef struct {
    void                (*rtn)( SCOPE, void * );
    void                *data;
} all_bases_walk;

typedef struct {
    SCOPE               derived;
    SCOPE               base;
    SCOPE               found;
    SYMBOL              vfn_sym;
    SYMBOL              vfn_override;
    target_offset_t     this_delta;
    target_offset_t     retn_delta;
} vfn_opt_walk;

typedef struct {
    CLASS_VBTABLE       *tables;
    SCOPE               start;
    unsigned            already_done : 1;
} vbtable_walk;

typedef struct {
    CLASS_VFTABLE       *tables;
    CLASS_VFTABLE       *curr;
    BASE_STACK          *top;
    BASE_STACK          *parent;
    SCOPE               start;
    SCOPE               final;
    SCOPE               base;
    SCOPE               derived;
    THUNK_ACTION        *thunk;
    VSTK_CTL            disambig;
    unsigned            OK_to_diagnose : 1;
    unsigned            already_done : 1;
    unsigned            thunk_code : 1;
} vftable_walk;

typedef struct {                                /* I - input, O - output */
    SCOPE               start;                  /* I: type of (*p) in "p->C::a" */
    SCOPE               disambiguate;           /* I: C in "p->C::a" */
    SCOPE               ignore;                 /* I: don't search here */
    char                *name;                  /* I: a in "p->C::a" */
    TYPE                type;                   /* I: T in "p->operator T()" */
    TYPE                fn_type;                /* I: type of virtual fn */
    SYMBOL              fn_sym;                 /* I: sym of virtual fn */
    special_name_fn     is_special;             /* I: fn to check for special names */
    unsigned            consider_mask;          /* I: mask to check curr scope */
    BASE_STACK          *top;                   /* T: temp for storing 'top' */
    MSG_NUM             error_msg;              /* O: error message to use */
    MSG_NUM             info_msg;               /* O: info message for above msg */
    SYMBOL              info1;                  /* O: parm for first info msg */
    SYMBOL              info2;                  /* O: parm for second info msg */
    SYMBOL_NAME         vfn_name;               /* O: vfn that shares table */
    FNOV_LIST           *user_conv_list;        /* O: list of user convs in hierarchy */
    PATH_CAP            *paths;                 /* O: paths to name */
    unsigned            path_count;             /* O: # of paths to name */
    inherit_flag        perm;                   /* O: permission of path to name */
    type_flag           this_qualifier;         /* I: T cv-qual *this; cv-qual */
    unsigned            virtual_override : 1;   /* I: find vfns with same name*/
    unsigned            user_conversion : 1;    /* I: find conversion to 'type' */
    unsigned            specific_user_conv : 1; /* I: must find specific conv */
    unsigned            best_user_conv : 1;     /* I: must find best conv */
    unsigned            check_special : 1;      /* I: use is_special() */
    unsigned            no_inherit : 1;         /* I: no base classes searched */
    unsigned            only_inherit : 1;       /* I: only base classes searched */
    unsigned            only_bases : 1;         /* I: direct base classes searched */
    unsigned            ok_to_diagnose : 1;     /* I: flag any errors found */
    unsigned            find_all : 1;           /* I: find all pathes to base */
    unsigned            ignore_access : 1;      /* I: access isn't important */
    unsigned            saw_class : 1;          /* I: class scope was seen */
    unsigned            ambiguous : 1;          /* O: ambiguity detected */
    unsigned            overload_reqd : 1;      /* O: overload affects ambiguity */
    unsigned            use_this : 1;           /* O: access could use "this" */
    unsigned            no_this : 1;            /* O: access can't use "this" */
    unsigned            saw_function : 1;       /* O: function scope was seen */
    unsigned            use_index : 1;          /* O: use index of virtual fn */
    unsigned            return_thunk : 1;       /* O: vfn needs return thunk */
    unsigned            protected_OK : 1;       /* O: protected sym can be accessed */
    unsigned            file_class_done : 1;    /* O: C::id search from file-scope done */
    unsigned            file_ns_done : 1;       /* O: N::id search from file-scope done */
    unsigned            same_table : 1;         /* O: vfn name is in same table */
    unsigned            lookup_error : 1;       /* O: error to report from lookup */
    unsigned            member_lookup : 1;
} lookup_walk;

typedef struct access_data {
    BASE_PATH           *path;
    SCOPE               access;
    SCOPE               member;
    SCOPE               located;
    inherit_flag        perm;
    unsigned            protected_OK : 1;
} access_data;

typedef struct qualify_stack QUALIFICATION;
struct qualify_stack {
    QUALIFICATION       *next;
    SCOPE               reset;
    SCOPE               access;
    SCOPE               enclosing;
};

SCOPE g_CurrScope;
SCOPE g_FileScope;
SCOPE g_InternalScope;
SYMBOL ChipBugSym;
SYMBOL DFAbbrevSym;
SYMBOL PCHDebugSym;

extern SCOPE    GetCurrScope(void)
{
    return g_CurrScope;
}

extern SCOPE    SetCurrScope(SCOPE newScope)
{
    SCOPE oldScope = g_CurrScope;
    g_CurrScope = newScope;

#ifndef NDEBUG
    if( PragDbgToggle.dump_scopes )
    {
        printf("Set new scope to 0x%.08X\n", newScope);
        if(newScope)
        {
            printf("===============================================================================\n");
            DumpScope(newScope);
            printf("===============================================================================\n");
        }
    }
#endif

    return oldScope;
}

extern SCOPE    GetFileScope(void)
{
    return g_FileScope;
}

extern SCOPE    SetFileScope(SCOPE newScope)
{
    SCOPE oldScope = g_FileScope;
    g_FileScope = newScope;
    return oldScope;
}

extern SCOPE    GetInternalScope(void)
{
    return g_InternalScope;
}

extern SCOPE    SetInternalScope(SCOPE newScope)
{
    SCOPE oldScope = g_InternalScope;
    g_InternalScope = newScope;
    return oldScope;
}



#define BLOCK_SYM_REGION        64
#define BLOCK_USING_NS          64
#define BLOCK_NAME_SPACE        64
#define BLOCK_SCOPE             64
#define BLOCK_SYMBOL            64
#define BLOCK_SYMBOL_NAME       64
#define BLOCK_BASE_STACK        16
#define BLOCK_BASE_PATH         16
#define BLOCK_PATH_CAP          16
#define BLOCK_SEARCH_RESULT     16
#define BLOCK_GEN_LEAP          16
#define BLOCK_QUALIFICATION     16
#define BLOCK_SYMBOL_EXCLUDE    16
static carve_t carveSYM_REGION;
static carve_t carveUSING_NS;
static carve_t carveNAME_SPACE;
static carve_t carveSCOPE;
static carve_t carveSYMBOL;
static carve_t carveSYMBOL_NAME;
static carve_t carveBASE_STACK;
static carve_t carveBASE_PATH;
static carve_t carvePATH_CAP;
static carve_t carveSEARCH_RESULT;
static carve_t carveGEN_LEAP;
static carve_t carveQUALIFICATION;
static carve_t carveSYMBOL_EXCLUDE;

ExtraRptCtr( syms_defined );
ExtraRptCtr( scopes_alloced );
ExtraRptCtr( scopes_kept );
ExtraRptCtr( scopes_searched );
ExtraRptCtr( scopes_closed );
ExtraRptCtr( nonempty_scopes_closed );
ExtraRptCtr( cnv_total );
ExtraRptCtr( cnv_quick );
ExtraRptCtr( cnv_found );

static SAVE_MAPPING *mappingList;       // member pointer mapping array list
static NAME uniqueNameSpaceName;        // name for unique namespaces
static NAME_SPACE *allNameSpaces;       // list of all namespaces

static inherit_flag verifyAccess( access_data * );
static inherit_flag checkBaseAccess( SCOPE, SCOPE, derived_status );

#ifndef NDEBUG
static void printScopeName( SCOPE scope, char *suffix )
{
    char *name;

    switch( scope->id ) {
    case SCOPE_CLASS:
        name = ScopeClass( scope )->u.c.info->name;
        if( name == NULL ) {
            name = "**un-named**";
        }
        printf( "%s ", name );
        break;
    case SCOPE_FILE:
        name = ScopeNameSpaceFormatName( scope );
        if( name == NULL ) {
            printf( "FILE " );
        } else {
            printf( "%s ", name );
        }
        break;
    case SCOPE_FUNCTION:
        printf( "%s() ", ScopeFunction( scope )->name->name );
        break;
    case SCOPE_BLOCK:
        printf( "{} " );
        break;
    default:
        printf( "SCOPE(%02x) ", scope->id );
    }
    if( suffix != NULL ) {
        fputs( suffix, stdout );
    }
}

static void printSymbolName( SYMBOL sym )
{
    printScopeName( SymScope( sym ), NULL );
    printf( ":: %s ", sym->name->name );
}

static void dumpThunk( THUNK_ACTION *thunk )
{
    SYMBOL sym;

    if( ! PragDbgToggle.dump_vftables ) {
        return;
    }
    printf( "thunk:" );
    if( thunk->last_entry ) {
        printf( " last_entry" );
    }
    if( thunk->possible_ambiguity ) {
        printf( " possible_ambiguity" );
    }
    if( ScopePureVirtualThunk( thunk ) != NULL ) {
        printf( " pure_virtual" );
    }
    putchar( '\n' );
    printf( "- " );
    sym = thunk->sym;
    printSymbolName( sym );
    putchar( '\n' );
    if( thunk->delta ) {
        printf( "- this -= %xh\n", thunk->delta );
    }
    if( thunk->ctor_disp ) {
        printf( "- this = *(this - sizeof(unsigned))\n" );
        if( thunk->input_virtual ) {
            printf( "- this += %xh\n", thunk->in.vb_offset );
            printf( "- this += (*this)[%u]\n", thunk->in.vb_index );
        }
        if( thunk->in.delta ) {
            printf( "- this += %xh\n", thunk->in.delta );
        }
    }
    sym = thunk->override;
    if( sym != NULL ) {
        printf( "- " );
        printSymbolName( sym );
        putchar( '\n' );
    } else {
        printf( "- (no override)\n" );
    }
    if( thunk->output_virtual ) {
        printf( "- ret += %xh\n", thunk->out.vb_offset );
        printf( "- ret += (*ret)[%u]\n", thunk->out.vb_index );
    }
    if( thunk->out.delta ) {
        printf( "- ret += %xh\n", thunk->out.delta );
    }
    printf( "--\n" );
}

static void dumpVFN( SYMBOL sym, vftable_walk *data, CLASS_VFTABLE *table, unsigned vf_index )
{
    BASE_STACK *top;
    BASE_CLASS *base;

    if( ! PragDbgToggle.dump_vftables ) {
        return;
    }
    printSymbolName( sym );
    printf( "offset=%04xh index=%u\n", table->h.exact_delta, vf_index + VFUN_BASE );
    for( top = data->top; top != NULL; top = top->parent ) {
        printScopeName( top->scope, NULL );
        base = top->base;
        if( base != NULL ) {
            if( _IsDirectVirtualBase( base ) ) {
                printf( "===> " );
            } else {
                printf( "-%xh-> ", base->delta );
            }
        }
    }
    putchar( '\n' );
}

static void dumpDerivation( MEMBER_PTR_CAST *data )
{
    if( ! PragDbgToggle.dump_member_ptr ) {
        return;
    }
    if( data->safe ) {
        printScopeName( data->base, "-> " );
    } else {
        printScopeName( data->base, "<- " );
    }
    printScopeName( data->derived, "\n" );
}

static void dumpData( MEMBER_PTR_CAST *data )
{
    if( ! PragDbgToggle.dump_member_ptr ) {
        return;
    }
    printf(
        "delta:         %u "
        "single_test:   %u "
        "vb_index:      %u "
        "mapping:       %08x "
        "\n"
        "init_conv:     %u "
        "mapping_reqd:  %u "
        "delta_reqd:    %u "
        "test_reqd:     %u "
        "\n"
        "singlemapping: %u "
        "\n"
        ,
        data->delta,
        data->single_test,
        data->vb_index,
        data->mapping,
        data->init_conv,
        data->mapping_reqd,
        data->delta_reqd,
        data->test_reqd,
        data->single_mapping
        );

}

static boolean recordableScope( SCOPE scope )
{
    switch( scope->id ) {
    case SCOPE_TEMPLATE_DECL:
    case SCOPE_TEMPLATE_INST:
        return( FALSE );
    }
    return( TRUE );
}
#else
#define printSymbolName( s )
#define dumpThunk( t )
#define dumpVFN( s, d, t, i )
#define dumpDerivation( d )
#define dumpData( d )
#endif

static void reinitScope( SCOPE scope )
{
    // keep ->in_unnamed setting
    scope->s.dtor_reqd = FALSE;
    scope->s.dtor_naked = FALSE;
    scope->s.try_catch = FALSE;
    scope->ordered = NULL;
    scope->owner.sym = NULL;
    scope->names = HashCreate( hashTableSize[scope->id] );
}

static SCOPE makeScope( scope_type_t scope_type )
{
    SCOPE new_scope;

    ExtraRptIncrementCtr( scopes_alloced );
    ExtraRptIncrementCtr( scopes_kept );
    new_scope = CarveAlloc( carveSCOPE );
    new_scope->id = scope_type;
    new_scope->s.keep = FALSE;
    new_scope->s.dtor_reqd = FALSE;
    new_scope->s.dtor_naked = FALSE;
    new_scope->s.try_catch = FALSE;
    new_scope->s.arg_check = FALSE;
    new_scope->s.cg_stab = FALSE;
    new_scope->s.in_unnamed = FALSE;
    new_scope->s.fn_template = FALSE;
    new_scope->s.dirty = FALSE;
    new_scope->enclosing = NULL;
    new_scope->ordered = NULL;
    new_scope->owner.sym = NULL;
    new_scope->names = HashCreate( hashTableSize[scope_type] );
    new_scope->using_list = NULL;
    DbgStmt( if( recordableScope( new_scope ) ) DbgRememberScope( new_scope ); );
    return( new_scope );
}

static SYMBOL injectOpDel( NAME name )
{
    SYMBOL sym;

    sym = SymCreateFileScope( TypeVoidFunOfPtrVoid(), SC_EXTERN, 0, name );
    LinkageSet( sym, "C++" );
    return( sym );
}

static void injectGlobalOpDelete( void )
{
    /* extern void operator delete( void * ); */
    injectOpDel( CppOperatorName( CO_DELETE ) );

    /* extern void operator delete []( void * ); */
    injectOpDel( CppOperatorName( CO_DELETE_ARRAY ) );
}

static void injectOpNew( TYPE fn_type, NAME name )
{
    SYMBOL sym;

    sym = SymCreateFileScope( fn_type, SC_EXTERN, 0, name );
    LinkageSet( sym, "C++" );
}

static void injectGlobalOpNew( void )
{
    TYPE pvoid_type;
    TYPE size_type;
    TYPE size_fn_type;
    TYPE placement_fn_type;
    NAME simple_name;
    NAME array_name;

    pvoid_type = TypePtrToVoid();
    size_type = GetBasicType( TYP_UINT );
    simple_name = CppOperatorName( CO_NEW );
    array_name = CppOperatorName( CO_NEW_ARRAY );

    /* extern void *operator new( unsigned ); */
    size_fn_type = MakeSimpleFunction( pvoid_type, size_type, NULL );
    injectOpNew( size_fn_type, simple_name );

    /* extern void *operator new []( unsigned ); */
    injectOpNew( size_fn_type, array_name );

    /* extern void *operator new( unsigned, void * ); */
    placement_fn_type = MakeSimpleFunction( pvoid_type, size_type, pvoid_type, NULL );
    injectOpNew( placement_fn_type, simple_name );

    /* extern void *operator new []( unsigned, void * ); */
    injectOpNew( placement_fn_type, array_name );
}

static void injectChipBug( void )
{
    TYPE type;
    NAME name;
    SYMBOL sym;

    name = CppSpecialName( SPECIAL_CHIPBUG );
    type = GetBasicType( TYP_UINT );
    type = MakeModifiedType( type, TF1_NEAR );
    sym = SymCreateFileScope( type, SC_EXTERN, 0, name );
    LinkageSet( sym, "C" );
    ChipBugSym = sym;
}

static void injectBool( void )
{
    NAME name;

    /* 'bool' cannot be a keyword because the idiots at MS use it as an id */
    if( CompFlags.extensions_enabled ) {
        name = NameCreateNoLen( Tokens[T_BOOL] );
        KwDisable( T_BOOL );
        SymCreateFileScope( GetBasicType( TYP_BOOL ), SC_TYPEDEF, 0, name );
    }
}

static void injectDwarfAbbrev( void )
{
    TYPE type;
    NAME name;
    SYMBOL sym;

    name = CppSpecialName( SPECIAL_DWARF_ABBREV );
    type = GetBasicType( TYP_CHAR );
    sym = SymCreateFileScope( type, SC_EXTERN, 0, name );
    LinkageSet( sym, "C" );
    DFAbbrevSym = sym;
}

void ScopeCreatePCHDebugSym( void )
/*********************************/
{
    TYPE type;
    NAME name;
    SYMBOL sym;

    if( CompFlags.pch_debug_info_write ) {
        name = PCHDebugInfoName();
        type = GetBasicType( TYP_CHAR );
        sym = SymCreateFileScope( type, SC_PUBLIC, 0, name );
        PCHDebugSym = sym;
    }else if( CompFlags.pch_debug_info_read ){
        name = PCHDebugInfoName();
        type = GetBasicType( TYP_CHAR );
        sym = SymCreateFileScope( type, SC_EXTERN, 0, name );
        PCHDebugSym = sym;
    }else{
        PCHDebugSym = NULL;
    }
}

static SCOPE initGlobalNamespaceScope( SCOPE scope )
{
    scope->names = HashMakeMax( scope->names );
    ScopeKeep( scope );
    return( scope );
}

static SCOPE makeFileScope( fs_control control, SYMBOL sym )
{
    SCOPE scope;
    NAME_SPACE *ns;

    scope = makeScope( SCOPE_FILE );
    ns = CarveAlloc( carveNAME_SPACE );
    ns->sym = sym;
    ns->scope = scope;
    ns->all = allNameSpaces;
    ns->s.global_fs = FALSE;
    ns->s.free = FALSE;
    ns->s.unnamed = FALSE;
    if( control & FS_GLOBAL ) {
        ns->s.global_fs = TRUE;
    } else if( control & FS_UNNAMED ) {
        ns->s.unnamed = TRUE;
        scope->s.in_unnamed = TRUE;
    }
    allNameSpaces = ns;
    scope->owner.ns = ns;
    return( scope );
}

static void scopeOpenMaybeNull( SCOPE scope )
{
    SCOPE enclosing;

    enclosing = GetCurrScope();
    scope->enclosing = enclosing;
    if( enclosing != NULL && enclosing->s.in_unnamed ) {
        scope->s.in_unnamed = TRUE;
    }
    SetCurrScope(scope);
}

static void scopeBeginFileScope( void )
{
    SCOPE scope;

    scope = makeFileScope( FS_GLOBAL, NULL );
    scopeOpenMaybeNull( scope );
}

static void scopeInit(          // SCOPES INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    PCHActivate();
    carveSYM_REGION = CarveCreate( sizeof( SYM_REGION ), BLOCK_SYM_REGION );
    carveUSING_NS = CarveCreate( sizeof( USING_NS ), BLOCK_USING_NS );
    carveNAME_SPACE = CarveCreate( sizeof( NAME_SPACE ), BLOCK_NAME_SPACE );
    carveSCOPE = CarveCreate( sizeof( struct scope ), BLOCK_SCOPE );
    carveSYMBOL = CarveCreate( sizeof( struct symbol ), BLOCK_SYMBOL );
    carveSYMBOL_NAME = CarveCreate( sizeof( struct symbol_name ),
                                    BLOCK_SYMBOL_NAME );
    carveBASE_STACK = CarveCreate( sizeof( BASE_STACK ), BLOCK_BASE_STACK );
    carveBASE_PATH = CarveCreate( sizeof( BASE_PATH ), BLOCK_BASE_PATH );
    carvePATH_CAP = CarveCreate( sizeof( PATH_CAP ), BLOCK_PATH_CAP );
    carveSEARCH_RESULT = CarveCreate( sizeof( SEARCH_RESULT ),
                                      BLOCK_SEARCH_RESULT );
    carveGEN_LEAP = CarveCreate( sizeof( GEN_LEAP ),
                                      BLOCK_GEN_LEAP );
    carveQUALIFICATION = CarveCreate( sizeof( QUALIFICATION ),
                                      BLOCK_QUALIFICATION );
    carveSYMBOL_EXCLUDE = CarveCreate( sizeof( SYMBOL_EXCLUDE ),
                                       BLOCK_SYMBOL_EXCLUDE );
    SetCurrScope(NULL);
    uniqueNameSpaceName = NULL;
    allNameSpaces = NULL;
    PCHDebugSym = NULL;
    mappingList = NULL;
    HashPostInit( NULL );
    scopeBeginFileScope();
    SetFileScope(initGlobalNamespaceScope( GetCurrScope() ));
    scopeBeginFileScope();
    SetInternalScope(GetCurrScope());
    ScopeKeep( GetInternalScope());
    SetCurrScope(GetFileScope());
    HashPostInit( GetFileScope() );
    BrinfOpenScope( GetFileScope() );
    BrinfOpenScope( GetInternalScope() );
    injectGlobalOpNew();
    injectGlobalOpDelete();
    injectChipBug();
    injectDwarfAbbrev();
    injectBool();
    if( ! CompFlags.enable_std0x ) {
        KwDisable( T_STATIC_ASSERT );
        KwDisable( T_DECLTYPE );
    }
    ExtraRptRegisterCtr( &syms_defined, "symbols defined" );
    ExtraRptRegisterCtr( &scopes_alloced, "scopes allocated" );
    ExtraRptRegisterCtr( &scopes_kept, "scopes kept" );
    ExtraRptRegisterCtr( &scopes_searched, "scopes searched" );
    ExtraRptRegisterCtr( &scopes_closed, "scopes closed" );
    ExtraRptRegisterCtr( &nonempty_scopes_closed, "non-empty scopes closed" );
    ExtraRptRegisterCtr( &cnv_total, "ScopeBestConversion calls" );
    ExtraRptRegisterCtr( &cnv_quick, "ScopeBestConversion quick exits" );
    ExtraRptRegisterCtr( &cnv_found, "ScopeBestConversion finds a UDC" );
}

static void scopeFini(          // SCOPES COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    DbgStmt( CarveVerifyAllGone( carveBASE_STACK, "BASE_STACK" ) );
    DbgStmt( CarveVerifyAllGone( carveBASE_PATH, "BASE_PATH" ) );
    DbgStmt( CarveVerifyAllGone( carvePATH_CAP, "PATH_CAP" ) );
    DbgStmt( CarveVerifyAllGone( carveSEARCH_RESULT, "SEARCH_RESULT" ) );
    DbgStmt( CarveVerifyAllGone( carveGEN_LEAP, "GEN_LEAP" ) );
    DbgStmt( CarveVerifyAllGone( carveQUALIFICATION, "QUALIFICATION" ) );
    DbgStmt( CarveVerifyAllGone( carveSYMBOL_EXCLUDE, "SYMBOL_EXCLUDE" ) );
    if( CompFlags.extensions_enabled ) {
        KwEnable( T_BOOL );
    }
    if( !CompFlags.enable_std0x ) {
        KwEnable( T_STATIC_ASSERT );
        KwEnable( T_DECLTYPE );
    }
    CarveDestroy( carveSYM_REGION );
    CarveDestroy( carveUSING_NS );
    CarveDestroy( carveNAME_SPACE );
    CarveDestroy( carveSCOPE );
    CarveDestroy( carveSYMBOL );
    CarveDestroy( carveSYMBOL_NAME );
    CarveDestroy( carveBASE_STACK );
    CarveDestroy( carveBASE_PATH );
    CarveDestroy( carvePATH_CAP );
    CarveDestroy( carveSEARCH_RESULT );
    CarveDestroy( carveGEN_LEAP );
    CarveDestroy( carveQUALIFICATION );
    CarveDestroy( carveSYMBOL_EXCLUDE );
}

INITDEFN( scopes, scopeInit, scopeFini )

static SCOPE findCommonEnclosing( SCOPE scope1, SCOPE scope2 )
{
    SCOPE i1;
    SCOPE i2;
    SCOPE it;

    for( i1 = scope1; i1 != NULL; i1 = i1->enclosing ) {
        if( i1 == scope2 ) {
            return( i1 );
        }
        i1->s.colour = TRUE;
    }
    for( i2 = scope2; i2 != NULL; i2 = i2->enclosing ) {
        if( i2 == scope1 ) {
            return( i2 );
        }
        i2->s.colour = FALSE;
    }
    for( it = scope1; it != NULL; it = it->enclosing ) {
        if( ! it->s.colour ) {
            return( it );
        }
    }
    DbgNever();
    return( GetFileScope() );
}

static void addLexicalTrigger( SCOPE gets_trigger, SCOPE using_scope,
                               boolean append )
{
    USING_NS *lexical_entry;

    // trigger == NULL: push
    lexical_entry = CarveAlloc( carveUSING_NS );
    lexical_entry->using_scope = using_scope;
    lexical_entry->trigger = NULL;
    if( append ) {
        RingAppend( &gets_trigger->using_list, lexical_entry );
    } else {
        RingPush( &gets_trigger->using_list, lexical_entry );
    }
}

static void addUsingDirective( SCOPE gets_using, SCOPE using_scope, SCOPE trigger )
{
    USING_NS *using_entry;
    USING_NS *curr;

#ifndef NDEBUG
    if( PragDbgToggle.dump_using_dir ) {
        printf( "using directive: in " );
        printScopeName( gets_using, "using " );
        printScopeName( using_scope, "trigger " );
        printScopeName( trigger, "\n" );
    }
#endif
    using_entry = NULL;
    RingIterBeg( gets_using->using_list, curr ) {
        if( curr->using_scope != using_scope ) continue;
        // relation between scopes should not change so trigger
        // should be equal
        DbgAssert( curr->trigger == NULL || curr->trigger == trigger );
        if( curr->trigger == trigger ) {
            using_entry = curr;
            break;
        }
    } RingIterEnd( curr )
    if( using_entry == NULL ) {
        // trigger != NULL: append
        using_entry = CarveAlloc( carveUSING_NS );
        using_entry->using_scope = using_scope;
        using_entry->trigger = trigger;
        RingAppend( &gets_using->using_list, using_entry );

        addLexicalTrigger( trigger, using_scope, FALSE );
#ifndef NDEBUG
    } else {
        USING_NS *curr;
        RingIterBeg( trigger->using_list, curr ) {
            if( curr->using_scope != using_scope ) continue;
            if( curr->trigger == NULL ) {
                break;
            }
        } RingIterEnd( curr )
        DbgAssert( curr->trigger == NULL );
#endif
    }
}

void ScopeRestoreUsing( SCOPE scope, boolean append )
/***************************************************/
{
    USING_NS *curr;

    RingIterBeg( scope->using_list, curr ) {
        if( curr->trigger != NULL ) {
            addLexicalTrigger( curr->trigger, curr->using_scope, append );
        }
    } RingIterEnd( curr )
}

void ScopeAddUsing( SCOPE using_scope, SCOPE trigger )
/****************************************************/
{
    SCOPE gets_using;

    gets_using = GetCurrScope();
    DbgAssert( using_scope != NULL );
    // NYI: to emulate MS/MetaWare bug, set trigger to CurrScope
    if( trigger == NULL ) {
        trigger = findCommonEnclosing( gets_using, using_scope );
    }
    if( trigger == using_scope ) {
        CErr1( WARN_USELESS_USING_DIRECTIVE );
        return;
    }
    addUsingDirective( gets_using, using_scope, trigger );
}

SCOPE ScopeIsGlobalNameSpace( SCOPE scope )
/*****************************************/
{
    NAME_SPACE *ns;

    if( scope != NULL ) {
        if( _IsFileScope( scope ) ) {
            ns = scope->owner.ns;
            if( ns->s.global_fs ) {
                return( scope );
            }
        }
    }
    return( NULL );
}

SCOPE ScopeIsUnnamedNameSpace( SCOPE scope )
/******************************************/
{
    NAME_SPACE *ns;

    if( scope != NULL ) {
        if( _IsFileScope( scope ) ) {
            ns = scope->owner.ns;
            if( ns->s.unnamed ) {
                return( scope );
            }
        }
    }
    return( NULL );
}

SCOPE ScopeCreate( scope_type_t scope_type )
/******************************************/
{
    DbgAssert( scope_type != SCOPE_FILE );
    return( makeScope( scope_type ) );
}

void FreeSymbol( SYMBOL sym )
/***************************/
{
    CarveFree( carveSYMBOL, sym );
}

void FreeSymbolName( SYMBOL_NAME sym_name )
/*****************************************/
{
    RingCarveFree( carveSYMBOL, &(sym_name->name_syms) );
    FreeSymbol( sym_name->name_type );
    CarveFree( carveSYMBOL_NAME, sym_name );
}

void ScopeClear( SCOPE scope )
/****************************/
{
    ScopeWalkNames( scope, FreeSymbolName );
    HashDestroy( scope->names );
    reinitScope( scope );
}

void ScopeBurn( SCOPE scope )
/***************************/
{
    DbgStmt( if( recordableScope( scope ) ) DbgForgetScope( scope ); );
    ScopeWalkNames( scope, FreeSymbolName );
    HashDestroy( scope->names );
    CarveFree( carveSCOPE, scope );
    ExtraRptDecrementCtr( scopes_kept );
}

void ScopeSetClassOwner( SCOPE scope, TYPE class_type )
/*****************************************************/
{
    scope->owner.type = class_type;
}

SCOPE ScopeSetContaining( SYMBOL_NAME sym_name, SCOPE new_containing )
/********************************************************************/
{
    SCOPE old_containing;

    old_containing = sym_name->containing;
    sym_name->containing = new_containing;
    return( old_containing );
}

#define doScopeEstablish( sc, cs )              \
    {                                           \
        SCOPE enclosing;                        \
                                                \
        enclosing = cs;                         \
        sc->enclosing = enclosing;              \
        if( enclosing->s.in_unnamed ) {         \
            sc->s.in_unnamed = TRUE;            \
        }                                       \
    }


SCOPE ScopeSetEnclosing( SCOPE scope, SCOPE new_enclosing )
/*********************************************************/
{
    SCOPE old_enclosing;

    // don't propagate ->in_unnamed
    old_enclosing = scope->enclosing;
    scope->enclosing = new_enclosing;
    return( old_enclosing );
}

SCOPE ScopeEstablishEnclosing( SCOPE scope, SCOPE new_enclosing )
/***************************************************************/
{
    SCOPE old_enclosing;

    old_enclosing = scope->enclosing;
    doScopeEstablish( scope, new_enclosing );
    return( old_enclosing );
}

void ScopeOpen( SCOPE scope )
/***************************/
{
    doScopeEstablish( scope, GetCurrScope() );
    SetCurrScope(scope);
    BrinfOpenScope( GetCurrScope() );
}

void ScopeEstablish( SCOPE scope )
/********************************/
{
    doScopeEstablish( scope, GetCurrScope() );
}

static SCOPE findFunctionScope( SCOPE scope )
{
    for( ; scope != NULL; scope = scope->enclosing ) {
        if( _IsFunctionScope( scope ) ) {
            break;
        }
    }
    return( scope );
}

static SCOPE findBlockScope( SCOPE scope )
{
    for( ; scope != NULL; scope = scope->enclosing ) {
        if( _IsBlockScope( scope ) ) {
            break;
        }
    }
    return( scope );
}

static void diagnoseSym( SYMBOL sym )
{
    if( sym->flag & ( SF_REFERENCED | SF_NO_REF_WARN ) ) {
        return;
    }
    if( SymIsInline( sym ) ) {
        return;
    }
    if( SymIsTemporary( sym ) ) {
        return;
    }
    if( sym->id == SC_DEFAULT ) {
        return;
    }
    if( SymIsFunction( sym )
     && sym->flag & SF_ADDR_TAKEN ) {
        return;
    }
    if( SymIsAnonymous( sym ) ) {
        CErr2p( WARN_ANON_NOT_REFERENCED, sym );
    } else {
        if( ScopeType( SymScope( sym ), SCOPE_FUNCTION ) ) {
            CErr2p( WARN_PARM_NOT_REFERENCED, sym );
        } else {
            CErr2p( WARN_SYM_NOT_REFERENCED, sym );
        }
    }
}

static void scopeEnsureMemberRefed( SYMBOL sym )
{
    switch( sym->id ) {
    case SC_MEMBER:
    case SC_STATIC:
        if( ! GeneratedDefaultFunction( sym ) ) {
            diagnoseSym( sym );
        }
        break;
    }
}

static void diagnoseTypedef( SYMBOL sym )
{
    SCOPE scope;
    SCOPE sym_scope;
    TYPE class_type;

    sym_scope = SymScope( sym );
    scope = findBlockScope( sym_scope );
    DbgAssert( scope != NULL );         // this is always true? AFS
    if( scope != NULL ) {
        /* we have a local class */
        class_type = ClassTagDefinition( sym->sym_type, sym->name->name );
        if( class_type != NULL ) {
            CLASSINFO *info = class_type->u.c.info;

            if( ! info->has_fn ) {
                ScopeWalkOrderedSymbols( class_type->u.c.scope, &scopeEnsureMemberRefed );
            }
        }
    }
}

static void scopeEnsureRefed( SYMBOL sym )
{
    switch( sym->id ) {
    case SC_STATIC:
    case SC_AUTO:
    case SC_REGISTER:
        diagnoseSym( sym );
        break;
    }
}

static void handleBlockSyms( SYMBOL sym )
{
    switch( sym->id ) {
    case SC_TYPEDEF:
        diagnoseTypedef( sym );
        break;
    case SC_STATIC:
    case SC_AUTO:
    case SC_REGISTER:
        diagnoseSym( sym );
        break;
    }
}

static void handleFileSyms( SYMBOL sym )
{
    switch( sym->id ) {
    case SC_NULL:
        if( SymIsFunction( sym ) ) {
            sym->id = SC_EXTERN;
        }
        break;
    case SC_STATIC:
        if( SymIsFunction( sym ) ) {
            if( sym->flag & SF_REFERENCED ) {
                if( ! SymIsInitialized( sym ) ) {
                    /* Check to see if we have a matching aux entry with code attached */
                    AUX_ENTRY *paux = NULL;
                    paux = AuxLookup( sym->name->name );
                    if( !paux || !paux->info || !paux->info->code ) {
                        if( sym != ModuleInitFuncSym() ) {
                            CErr2p( ERR_FUNCTION_NOT_DEFINED, sym );
                            sym->id = SC_EXTERN;
                        }
                    }
                }
            }
        }
        if( ! SymIsConstantInt( sym ) && ! SymIsConstantNoCode( sym ) ) {
            diagnoseSym( sym );
        }
        break;
    }
}

static void handleUnnamedNameSpaceSyms( SYMBOL sym )
{
    switch( sym->id ) {
    case SC_NULL:
    case SC_PUBLIC:
        sym->id = SC_STATIC;
        break;
    }
    handleFileSyms( sym );
}

static void processNameSpaces( void )
{
    NAME_SPACE *curr;

    if( CompFlags.namespace_checks_done ) {
        return;
    }
    CompFlags.namespace_checks_done = TRUE;
    for( curr = allNameSpaces; curr != NULL; curr = curr->all ) {
        if( curr->s.unnamed ) {
            ScopeWalkOrderedSymbols( curr->scope, &handleUnnamedNameSpaceSyms );
        } else {
            ScopeWalkOrderedSymbols( curr->scope, &handleFileSyms );
        }
    }
}

static USING_NS *pruneMatchingUsing( SCOPE host, SCOPE using )
{
    USING_NS **head;
    USING_NS *curr;
    USING_NS *prev;

    head = &host->using_list;
    prev = NULL;
    RingIterBeg( *head, curr ) {
        if( curr->trigger == NULL && curr->using_scope == using ) {
            return( RingPruneWithPrev( head, curr, prev ) );
        }
        prev = curr;
    } RingIterEnd( curr )
    DbgAssert( ErrCount != 0 ); // should never get here on clean source
    return( NULL );
}

SCOPE ScopeClose( void )
/**********************/
{
    USING_NS *use;
    USING_NS *lex_use;
    NAME_SPACE *ns;
    SCOPE trigger;
    SCOPE dropping_scope;

    dropping_scope = GetCurrScope();
    SetCurrScope(dropping_scope->enclosing);
    ExtraRptIncrementCtr( scopes_closed );
    RingIterBegSafe( dropping_scope->using_list, use ) {
        trigger = use->trigger;
        if( trigger != NULL ) {
            lex_use = pruneMatchingUsing( trigger, use->using_scope );
            DbgAssert( lex_use != NULL || ErrCount != 0 );
            CarveFree( carveUSING_NS, lex_use );
        }
    } RingIterEndSafe( use )
    if( ! HashEmpty( dropping_scope->names ) ) {
        ExtraRptIncrementCtr( nonempty_scopes_closed );
        dropping_scope->s.keep = TRUE;
        switch( dropping_scope->id ) {
        case SCOPE_BLOCK:
            ScopeWalkOrderedSymbols( dropping_scope, &handleBlockSyms );
            break;
        case SCOPE_FILE:
            ns = dropping_scope->owner.ns;
            if( ns->s.global_fs ) {
                processNameSpaces();
            }
            break;
        case SCOPE_FUNCTION:
            ScopeWalkOrderedSymbols( dropping_scope, &scopeEnsureRefed );
            break;
        }
    }
    /* if this scope must be kept then its enclosing scope must be kept */
    if( GetCurrScope() != NULL ) {
        GetCurrScope()->s.keep |= dropping_scope->s.keep;
    }
    BrinfCloseScope( dropping_scope );
    if( dropping_scope->s.keep == FALSE ) {
        ScopeBurn( dropping_scope );
        dropping_scope = NULL;
    }
    return( dropping_scope );
}

static void pruneScopeUsing( SCOPE scope )
{
    SCOPE trigger;
    USING_NS *use;
    USING_NS *lex_use;

    RingIterBegSafe( scope->using_list, use ) {
        trigger = use->trigger;
        if( trigger != NULL ) {
            lex_use = pruneMatchingUsing( trigger, use->using_scope );
            DbgAssert( lex_use != NULL || ErrCount != 0 );
            CarveFree( carveUSING_NS, lex_use );
        }
    } RingIterEndSafe( use )
}

void ScopeAdjustUsing( SCOPE prev_scope, SCOPE new_scope )
/********************************************************/
{
    SCOPE scope;

    if( prev_scope == new_scope ) {
        return;
    }

    /*
     * when switching scopes we also need to update information about
     * "using namespaces"
     */

    if( prev_scope != NULL ) {
        scope = prev_scope;
        while( scope->enclosing != NULL ) {
            pruneScopeUsing( scope );
            scope = scope->enclosing;
        }
    }

    if( new_scope != NULL ) {
        scope = new_scope;
        while( scope->enclosing != NULL ) {
            ScopeRestoreUsing( scope, FALSE );
            scope = scope->enclosing;
        }
    }
}

void ScopeJumpForward( SCOPE scope )
/**********************************/
{
#ifndef NDEBUG
    if( ! ScopeEnclosed( GetCurrScope(), scope ) ) {
        CFatal( "invalid scope jump forward" );
    }
#endif
    SetCurrScope(scope);
}

void ScopeJumpBackward( SCOPE scope )
/***********************************/
{
#ifndef NDEBUG
    if( ! ScopeEnclosed( scope, GetCurrScope() ) ) {
        CFatal( "invalid scope jump backward" );
    }
#endif
    SetCurrScope(scope);
}

static void scopeWalkSymbolNameSymbols( SYMBOL_NAME name, void *data )
/********************************************************************/
{
    SYMBOL sym;
    void (*rtn)( SYMBOL ) = data;

    RingIterBeg( name->name_syms, sym ) {
        (*rtn)( sym );
    } RingIterEnd( sym )
}

void ScopeWalkSymbols( SCOPE scope, void (*walker)( SYMBOL ) )
/************************************************************/
{
    if( scope != NULL ) {
        HashWalkData( scope->names, &scopeWalkSymbolNameSymbols, walker );
    }
}

void ScopeWalkNames( SCOPE scope, void (*walker)( SYMBOL_NAME ) )
/***************************************************************/
{
    HashWalk( scope->names, walker );
}

void ScopeWalkOrderedSymbols( SCOPE scope, void (*walker)( SYMBOL ) )
/*******************************************************************/
{
    SYMBOL sym;
    SYMBOL stop;
    SYMBOL next;

    stop = scope->ordered;
    if( stop == NULL ) {
        return;
    }
    next = stop->thread;
    do {
        sym = next;
        next = next->thread;
        walker( sym );
    } while( sym != stop );
}

SYMBOL ScopeOrderedFirst( SCOPE scope )
/*************************************/
{
    SYMBOL first;

    first = ScopeOrderedNext( ScopeOrderedStart( scope ), NULL );
#ifndef NDEBUG
    {
        SYMBOL curr;
        SYMBOL stop;

        curr = NULL;
        stop = ScopeOrderedStart( scope );
        curr = ScopeOrderedNext( stop, curr );
        if( curr != first ) {
            CFatal( "scope: first symbol corrupted" );
        }
    }
#endif
    return( first );
}

SYMBOL ScopeOrderedLast( SCOPE scope )
/************************************/
{
    SYMBOL last;

    last = ScopeOrderedStart( scope );
#ifndef NDEBUG
    {
        SYMBOL curr;
        SYMBOL stop;

        stop = ScopeOrderedStart( scope );
        curr = ScopeOrderedNext( stop, last );
        if( curr != NULL ) {
            CFatal( "scope: last symbol corrupted" );
        }
    }
#endif
    return( last );
}

SYMBOL ScopeOrderedStart( SCOPE scope )
/*************************************/
{
    return( scope->ordered );
}

SYMBOL ScopeOrderedNext( SYMBOL stop, SYMBOL curr )
/*************************************************/
{
    if( stop == NULL ) {
        return( NULL );
    }
    if( curr == NULL ) {
        curr = stop;
    } else if( curr == stop ) {
        return( NULL );
    }
    return( curr->thread );
}

void ScopeWalkAllNameSpaces( void (*rtn)(NAME_SPACE *, void *), void *data )
/**************************************************************************/
{
    NAME_SPACE *curr;

    for( curr = allNameSpaces; curr != NULL; curr = curr->all ) {
        (*rtn)( curr, data );
    }
}

void ScopeWalkDataMembers( SCOPE scope, void (*rtn)(SYMBOL, void *), void *data )
/*********************************************************************************/
{
    SYMBOL stop;                // - first symbol for scope
    SYMBOL next;                // - next symbol in scope

    stop = ScopeOrderedStart( scope );
    if( stop != NULL ) {
        for( next = NULL; (next = ScopeOrderedNext( stop, next )) != NULL; ) {
            if( SymIsThisDataMember( next ) ) {
                (*rtn)( next, data );
            }
        }
    }
}

void ScopeBeginFunction( SYMBOL sym )
/***********************************/
{
    ScopeBegin( SCOPE_FUNCTION );
    GetCurrScope()->owner.sym = sym;
    ScopeKeep( GetCurrScope() );
}

void ScopeBeginBlock( unsigned index )
/************************************/
{
    ScopeBegin( SCOPE_BLOCK );
    GetCurrScope()->owner.index = index;
}

SCOPE ScopeBegin( scope_type_t scope_type )
/*****************************************/
{
    SCOPE scope;

    scope = ScopeCreate( scope_type );
    ScopeOpen( scope );
    return( scope );
}

SCOPE ScopeOpenNameSpace( NAME name, SYMBOL sym )
/***********************************************/
{
    SCOPE scope;
    fs_control control;

    control = FS_NULL;
    if( name == NULL ) {
        control |= FS_UNNAMED;
    }
    scope = makeFileScope( control, sym );
    ScopeKeep( scope );
    ScopeOpen( scope );
    return( scope );
}

void ScopeEndFileScope( void )
/****************************/
{
    DbgAssert( GetFileScope() == NULL || ! GetFileScope()->s.in_unnamed );
    DbgAssert( GetInternalScope() == NULL || ! GetInternalScope()->s.in_unnamed );
    SetCurrScope(GetInternalScope());
    ScopeEnd( SCOPE_FILE );
    SetCurrScope(GetFileScope());
    ScopeEnd( SCOPE_FILE );
}

SCOPE ScopeEnd( scope_type_t scope_type )
/***************************************/
{
#ifndef NDEBUG
    if( GetCurrScope()->id != scope_type ) {
        CFatal( "scope terminated incorrectly" );
    }
#else
    scope_type = scope_type;
#endif
    return( ScopeClose() );
}

static void markInMem( SYMBOL sym )
{
    TYPE type;
    type_flag flags;

    if( SymIsAutomatic( sym ) ) {
        type = sym->sym_type;
        TypeGetActualFlags( type, &flags );
        if( 0 == ( flags & TF1_STAY_MEMORY ) ) {
            sym->sym_type = MakeForceInMemory( type );
        }
    }
}

void ScopeMarkVisibleAutosInMem( void )
/*************************************/
{
    SCOPE scope;

    scope = GetCurrScope();
    if( _IsBlockScope( scope ) ) {
        for(;;) {
            DbgAssert( _IsBlockScope( scope ) );
            ScopeWalkSymbols( scope, markInMem );
            scope = scope->enclosing;
            if( _IsFunctionScope( scope ) ) {
                ScopeWalkSymbols( scope, markInMem );
                break;
            }
        }
    }
}

SYMBOL AllocSymbol( void )
/************************/
{
    SYMBOL  sym;

    sym = CarveAlloc( carveSYMBOL );
    sym->next = NULL;
    sym->sym_type = NULL;
    sym->name = NULL;
    sym->locn = NULL;
    sym->flag = SF_NULL;
    sym->flag2 = SF2_NULL;
    sym->id = SC_NULL;
    sym->segid = SEG_NULL;
    sym->u.tinfo = NULL;
    sym->thread = NULL;
    return( sym );
}


SYMBOL AllocTypedSymbol( TYPE type )
/**********************************/
{
    SYMBOL sym;

    sym = AllocSymbol();
    sym->sym_type = type;
    return sym;
}


SYMBOL_NAME AllocSymbolName( NAME name, SCOPE scope )
/***************************************************/
{
    SYMBOL_NAME new_sym;

    new_sym = CarveAlloc( carveSYMBOL_NAME );
    new_sym->next = NULL;
    new_sym->name_type = NULL;
    new_sym->name_syms = NULL;
    new_sym->name = name;
    new_sym->containing = scope;
    return( new_sym );
}

boolean EnumTypeName( SYMBOL_NAME sym_name )
/******************************************/
{
    SYMBOL sym;
    TYPE type;

    sym = sym_name->name_type;
    if( sym == NULL ) {
        return( FALSE );
    }
    type = TypedefRemove( sym->sym_type );
    if( type->id != TYP_ENUM ) {
        return( FALSE );
    }
    return( TRUE );
}

boolean ClassTypeName( SYMBOL_NAME sym_name )
/*******************************************/
{
    SYMBOL sym;
    TYPE type;

    sym = sym_name->name_type;
    if( sym == NULL ) {
        return( FALSE );
    }
    type = TypedefRemove( sym->sym_type );
    if( type->id != TYP_CLASS ) {
        return( FALSE );
    }
    return( TRUE );
}

static boolean colonColonName( SYMBOL_NAME sym_name )
{
    SYMBOL sym;
    TYPE type;

    sym = sym_name->name_type;
    if( sym == NULL ) {
        return( FALSE );
    }
    if( SymIsNameSpace( sym ) ) {
        return( TRUE );
    }
    type = TypedefRemove( sym->sym_type );
    if( type->id != TYP_CLASS ) {
        return( FALSE );
    }
    return( TRUE );
}

static boolean colonColonTildeName( SYMBOL_NAME sym_name )
{
    SYMBOL sym;
    TYPE type;

    sym = sym_name->name_type;
    if( sym == NULL ) {
        return( FALSE );
    }
    if( SymIsNameSpace( sym ) ) {
        return( TRUE );
    }
    type = TypedefedType( sym->sym_type );
    if( type == NULL ) {
        return( FALSE );
    }
    return( TRUE );
}

static boolean nameSpaceName( SYMBOL_NAME sym_name )
{
    SYMBOL sym;

    sym = sym_name->name_type;
    if( sym == NULL ) {
        return( FALSE );
    }
    return( SymIsNameSpace( sym ) );
}

boolean VariableName( SYMBOL_NAME sym_name )
/******************************************/
{
    SYMBOL var_sym;
    TYPE var_type;

    var_sym = sym_name->name_syms;
    if( var_sym == NULL ) {
        return( FALSE );
    }
    var_type = FunctionDeclarationType( var_sym->sym_type );
    if( var_type != NULL ) {
        return( FALSE );
    }
    return( TRUE );
}

static void addOrdered( SCOPE scope, SYMBOL sym )
{
    ExtraRptIncrementCtr( syms_defined );
    scope->s.dirty = TRUE;
    if( sym->id == SC_DEFAULT ) {
        return;
    }
    /* based on RING.C algorithms but with a different 'next' */
    if( scope->ordered == NULL ) {
        sym->thread = sym;
        scope->ordered = sym;
    } else {
        SYMBOL old_tail;

        old_tail = scope->ordered;
        sym->thread = old_tail->thread;
        old_tail->thread = sym;
        scope->ordered = sym;
    }
}

static SCOPE findAccessScope( void )
{
    SCOPE scope;
    QUALIFICATION *qual;

    scope = GetCurrScope();
    qual = ParseCurrQualification();
    if( qual != NULL ) {
        scope = qual->access;
    }
    return( scope );
}


boolean ScopeAccessType( scope_type_t scope_type )
/************************************************/
{
    SCOPE scope;

    scope = findAccessScope();
    if( scope->id == scope_type ) {
        return( TRUE );
    }
    return( FALSE );
}


boolean ScopeType( SCOPE scope, scope_type_t scope_type )
/*******************************************************/
{
    if( scope == NULL ) {
        return( FALSE );
    }
    if( scope->id == scope_type ) {
        return( TRUE );
    }
    return( FALSE );
}

boolean ScopeEquivalent( SCOPE scope, scope_type_t scope_type )
/*************************************************************/
{
    if( scope_type == SCOPE_FILE ) {
        scope = ScopeNearestNonTemplate( scope );
    }

    return( ScopeType( scope, scope_type ) );
}

NAME ScopeUnnamedNamespaceName( TOKEN_LOCN *locn )
/************************************************/
{
    NAME ns_name;

    if( ! CompFlags.extensions_enabled ) {
        ns_name = uniqueNameSpaceName;
        if( ns_name == NULL ) {
            ns_name = CppNameUniqueNS( locn );
            uniqueNameSpaceName = ns_name;
        }
    } else {
        ns_name = CppNameUniqueNS( locn );
    }
    return( ns_name );
}

NAME ScopeNameSpaceName( SCOPE scope )
/*************************************/
{
    NAME_SPACE *ns;
    SYMBOL ns_sym;

    if( _IsFileScope( scope ) ) {
        ns = scope->owner.ns;
        ns_sym = ns->sym;
        if( ns_sym != NULL ) {
            return( ns_sym->name->name );
        }
    }
    return( NULL );
}

char *ScopeNameSpaceFormatName( SCOPE scope )
/*******************************************/
{
    NAME_SPACE *ns;
    SYMBOL ns_sym;

    if( _IsFileScope( scope ) ) {
        ns = scope->owner.ns;
        if( ns->s.unnamed ) {
            return( "<unique>" );
        }
        ns_sym = ns->sym;
        if( ns_sym != NULL ) {
            return( ns_sym->name->name );
        }
    }
    return( NULL );
}

SYMBOL ScopeFunction( SCOPE scope )
/*********************************/
{
    SYMBOL sym;

    sym = NULL;
    if( _IsFunctionScope( scope ) ) {
        sym = scope->owner.sym;
    }
    return( sym );
}

TYPE ScopeClass( SCOPE scope )
/****************************/
{
    TYPE class_type;

    class_type = NULL;
    if( _IsClassScope( scope ) ) {
        class_type = scope->owner.type;
    }
    return( class_type );
}

unsigned ScopeIndex( SCOPE scope )
/********************************/
{
    unsigned scope_index;

    scope_index = 0;
    if( _IsBlockScope( scope ) ) {
        scope_index = scope->owner.index;
    }
    return( scope_index );
}

BASE_CLASS *ScopeInherits( SCOPE scope )
/**************************************/
{
    TYPE type;
    BASE_CLASS *inherited;

    inherited = NULL;
    if( _IsClassScope( scope ) ) {
        type = ScopeClass( scope );
        if( type != NULL ) {
            inherited = type->u.c.info->bases;
        }
    }
    return( inherited );
}

BASE_CLASS *ScopeFindVFPtrInVBase( TYPE class_type )
/**************************************************/
{
    CLASSINFO *info;
    BASE_CLASS *base;

    RingIterBeg( class_type->u.c.info->bases, base ) {
        if( _IsVirtualBase( base ) ) {
            info = base->type->u.c.info;
            if( info->has_vfptr ) {
                return( base );
            }
        }
    } RingIterEnd( base )
    DbgNever();
    return( NULL );
}

FRIEND *ScopeFriends( SCOPE scope )
/*********************************/
{
    TYPE type;
    FRIEND *friends;

    friends = NULL;
    if( _IsClassScope( scope ) ) {
        type = ScopeClass( scope );
        if( type != NULL ) {
            friends = type->u.c.info->friends;
        }
    }
    return( friends );
}

boolean ScopeDirectBase( SCOPE scope, TYPE type )
/***********************************************/
{
    BASE_CLASS *base;
    TYPE base_type;

    RingIterBeg( ScopeInherits( scope ), base ) {
        if( _IsDirectBase( base ) ) {
            base_type = StructType( base->type );
            if( base_type == type ) {
                return( TRUE );
            }
        }
    } RingIterEnd( base )
    return( FALSE );
}

boolean ScopeIndirectVBase( SCOPE scope, TYPE type )
/**************************************************/
{
    BASE_CLASS *base;
    TYPE base_type;

    RingIterBeg( ScopeInherits( scope ), base ) {
        if( _IsIndirectVirtualBase( base ) ) {
            base_type = StructType( base->type );
            if( base_type == type ) {
                return( TRUE );
            }
        }
    } RingIterEnd( base )
    return( FALSE );
}

boolean ScopeHasVirtualBases( SCOPE scope )
/*****************************************/
{
    TYPE class_type;

    class_type = ScopeClass( scope );
    if( class_type == NULL ) {
        return( FALSE );
    }
    if( class_type->u.c.info->last_vbase == 0 ) {
        return( FALSE );
    }
    return( TRUE );
}

boolean ScopeHasVirtualFunctions( SCOPE scope )
/*********************************************/
{
    TYPE class_type;

    class_type = ScopeClass( scope );
    if( class_type == NULL ) {
        return( FALSE );
    }
    if( class_type->u.c.info->has_vfn == FALSE ) {
        return( FALSE );
    }
    return( TRUE );
}


SYMBOL_NAME scopeInsertName( SCOPE scope, SYMBOL sym, NAME name )
{
    SYMBOL_NAME sym_name;
    SYMBOL_NAME check_name;
    SCOPE enclosing;

    sym_name = HashLookup( scope->names, name );
    if( sym_name == NULL ) {
        if( scope->s.arg_check ) {
            /* args are treated as if they were declared in the outermost block */
            enclosing = scope->enclosing;
            while( ! _IsFunctionScope( enclosing ) ) {
                // ctors have extra scopes in the way
                enclosing = enclosing->enclosing;
            }
            if( _IsFunctionScope( enclosing ) ) {
                check_name = HashLookup( enclosing->names, name );
                if( check_name != NULL ) {
                    CErr2p( ERR_SYM_ALREADY_DEFINED, check_name->name_syms );
                }
            }
        }
        sym_name = AllocSymbolName( name, scope );
        HashInsert( scope->names, sym_name, name );
        ScopeKeep( scope );
    }
    sym->name = sym_name;
    return( sym_name );
}

void ScopeInsertErrorSym( SCOPE scope, PTREE id )
/***********************************************/
{
    NAME name;                  // - id name
    SYMBOL sym;                 // - symbol
    SYMBOL_NAME sym_name;       // - symbol name

    if( ! CompFlags.no_error_sym_injection ) {
        name = id->u.id.name;
        sym = AllocSymbol();
        sym->sym_type = TypeError;
        sym->flag |= SF_ERROR;
        SymbolLocnDefine( &(id->locn), sym );
        sym_name = scopeInsertName( scope, sym, name );
        _AddSymToRing( &(sym_name->name_syms), sym );
        addOrdered( scope, sym );
    }
}

SYMBOL ScopePromoteSymbol( SCOPE scope, SYMBOL sym, NAME name )
/*************************************************************/
{
    SYMBOL_NAME sym_name;       // - symbol name

    // this function is used to promote out anonymous union members
    sym_name = scopeInsertName( scope, sym, name );
    if( sym_name->name_syms == NULL && sym_name->name_type == NULL ) {
        _AddSymToRing( &(sym_name->name_syms), sym );
        addOrdered( scope, sym );
        return( sym );
    }
    return( NULL );
}


boolean ScopeCarefulInsert( SCOPE scope, SYMBOL *psym, NAME name )
/****************************************************************/
{
    decl_check decl_status;
    SYMBOL_NAME sym_name;
    SYMBOL sym;
    SYMBOL decl_sym;

    sym = *psym;
    sym_name = scopeInsertName( scope, sym, name );
    if( sym_name->containing != scope ) {
        /* adjust symbol name when we have found an injected class name */
        sym_name->containing = scope;
    }
    if( _IsFileScope( scope ) ) {
        LinkageSet( sym, NULL );
    }
    decl_sym = DeclCheck( sym_name, sym, &decl_status );
    if( decl_sym != sym ) {
        /* either an error occurred (decl_sym==NULL) or we should use decl_sym*/
        if(( decl_status & DC_KEEP_SYMBOL ) == 0 ) {
            FreeSymbol( sym );
        }
        *psym = decl_sym;
        return( ( decl_status & DC_REDEFINED ) != 0 );
    }
    addOrdered( scope, sym );
    if( _IsClassScope( scope ) ) {
        ClassMember( scope, sym );
    }
    DeclDefaultStorageClass( scope, sym );
    ExtraRptSymDefn( sym );
    return( FALSE );
}


SYMBOL ScopeInsert( SCOPE scope, SYMBOL sym, NAME name )
/******************************************************/
{
    ScopeCarefulInsert( scope, &sym, name );
    return( sym );
}

void ScopeRawAddFriendSym( CLASSINFO *info, SYMBOL sym )
/******************************************************/
{
    FRIEND *new_friend;

    new_friend = CPermAlloc( sizeof( FRIEND ) );
    new_friend->next = NULL;
    new_friend->u.sym = sym;
    RingAppend( &(info->friends), new_friend );
}

void ScopeRawAddFriendType( CLASSINFO *info, TYPE type )
/*******************************************************/
{
    FRIEND *new_friend;

    new_friend = CPermAlloc( sizeof( FRIEND ) );
    new_friend->next = NULL;
    new_friend->u.type = type;
    new_friend->u.is_type = TRUE;
    RingAppend( &(info->friends), new_friend );
}

void ScopeAddFriendSym( SCOPE scope, SYMBOL sym )
/***********************************************/
{
    SCOPE sym_scope;
    SYMBOL friendly_sym;
    TYPE scopes_class_type;
    TYPE class_type;
    FRIEND *a_friend;
    boolean OK_for_friend;

    class_type = StructType( sym->sym_type );
    if( ScopeLocalClass( scope ) ) {
        /* local classes have restrictions on friends */
        if( SymIsFunction( sym ) ) {
            /* it better be the containing function */
            if( sym != ScopeFunctionInProgress() ) {
                CErr2p( ERR_LOCAL_CLASS_NO_FRIEND_FNS, sym );
            }
        } else {
            OK_for_friend = TRUE;
            if( TypeDefined( class_type ) ) {
                OK_for_friend = FALSE;
            }
            sym_scope = SymScope( sym );
            if( findBlockScope( sym_scope ) != findBlockScope( scope ) ) {
                OK_for_friend = FALSE;
            }
            if( ! OK_for_friend ) {
                CErr2p( ERR_LOCAL_CLASS_FRIEND_CLASS, sym );
            }
        }
    }
    RingIterBeg( ScopeFriends( scope ), a_friend ) {
        if( FriendIsSymbol( a_friend ) ) {
            friendly_sym = FriendGetSymbol( a_friend );
            if( friendly_sym->id == SC_CLASS_TEMPLATE ) {
                /* friendly class templates */
                if( friendly_sym == sym ) {
                    CErr2p( WARN_CLASS_TEMPLATE_FRIEND_REPEATED, sym );
                    return;
                }
            } else {
                /* friendly functions */
                if( SymIsFunction( friendly_sym ) ) {
                    if( friendly_sym == sym ) {
                        CErr2p( WARN_FN_FRIEND_REPEATED, sym );
                        return;
                    }
                }
            }
        }
    } RingIterEnd( a_friend )
    scopes_class_type = ScopeClass( scope );
    ScopeRawAddFriendSym( scopes_class_type->u.c.info, sym );
}

void ScopeAddFriendType( SCOPE scope, TYPE type, SYMBOL sym )
/***********************************************************/
{
    SCOPE sym_scope;
    TYPE friendly_type;
    TYPE scopes_class_type;
    TYPE class_type;
    FRIEND *a_friend;
    boolean OK_for_friend;

    class_type = StructType( type );
    if( ScopeLocalClass( scope ) ) {
        /* local classes have restrictions on friends */
        OK_for_friend = TRUE;
        if( TypeDefined( class_type ) ) {
            OK_for_friend = FALSE;
        } else if( sym != NULL ) {
            sym_scope = SymScope( sym );
            if( findBlockScope( sym_scope ) != findBlockScope( scope ) ) {
                OK_for_friend = FALSE;
            }
        }
        if( ! OK_for_friend ) {
            CErr2p( ERR_LOCAL_CLASS_FRIEND_CLASS, sym );
        }
    }
    RingIterBeg( ScopeFriends( scope ), a_friend ) {
        if( FriendIsType( a_friend ) ) {
            friendly_type = FriendGetType( a_friend );
            if( friendly_type == class_type ) {
                CErr2p( WARN_CLASS_FRIEND_REPEATED, type );
                return;
            }
        }
    } RingIterEnd( a_friend )
    scopes_class_type = ScopeClass( scope );
    ScopeRawAddFriendType( scopes_class_type->u.c.info, type );
}

static BASE_PATH *newPath( PATH_CAP *cap )
{
    BASE_PATH *new_path;

    new_path = CarveAlloc( carveBASE_PATH );
    new_path->prev = NULL;
    new_path->base = NULL;
    new_path->scope = NULL;
    new_path->flag = IN_NULL;
    new_path->checked_private = FALSE;
    new_path->failed_private = FALSE;
    if( cap->tail != NULL ) {
        BASE_PATH *head;

        head = cap->head;
        head->prev = new_path;
        new_path->next = head;
        cap->head = new_path;
    } else {
        new_path->next = NULL;
        cap->head = new_path;
        cap->tail = new_path;
    }
    return( new_path );
}

static void deletePath( BASE_PATH *path )
{
    CarveFree( carveBASE_PATH, path );
}

static void pushBase( BASE_STACK **hdr, BASE_CLASS *base, SCOPE scope, BASE_STACK *parent )
{
    BASE_STACK *elt;

    elt = StackCarveAlloc( carveBASE_STACK, hdr );
    elt->parent = parent;
    elt->base = base;
    elt->scope = scope;
    elt->access_changed = NULL;
    elt->hold = NULL;
    elt->access_perm = IN_NULL;
    elt->used = FALSE;
}

static BASE_STACK *popBase( BASE_STACK **hdr )
{
    BASE_STACK *top;

    for(;;) {
        top = *hdr;
        if( top == NULL )
            return( NULL );
        if( top->used == FALSE )
            break;
        *hdr = top->next;
        CarveFree( carveBASE_STACK, top );
    }
    top->used = TRUE;
    return( top );
}

static void cleanStack( BASE_STACK **hdr )
{
    BASE_STACK *top;

    for(;;) {
        top = popBase( hdr );
        if( top == NULL ) {
            break;
        }
    }
}

static void walkOneScope( SCOPE scope, walk_routine rtn, void *parm )
{
    BASE_STACK *stack;

    stack = NULL;
    pushBase( &stack, NULL, scope, NULL );
    (*rtn)( stack, parm );
    cleanStack( &stack );
}

/*
        visits  A B C D  in the following order: H, E, F, G, C, D, A, B
                 \| |/
                  E F G
                   \|/
                    H

        note: if the search changes what it is trying to find during the
              traversal, the search routine must verify that it is below
              the point in the traversal that it changed course
*/
static void walkDirectBases( SCOPE scope, walk_routine rtn, void *parm )
{
    walk_status status;
    BASE_CLASS *curr;
    BASE_STACK *top;
    BASE_STACK *stack;

    stack = NULL;
    pushBase( &stack, NULL, scope, NULL );
    status = (*rtn)( stack, parm );
    if( status == WALK_FINISH ) {
        cleanStack( &stack );
        return;
    }
    if( status == WALK_ABANDON ) {
        popBase( &stack );
    }
    for( ; (top = popBase( &stack )) != NULL; ) {
        RingIterBeg( ScopeInherits( top->scope ), curr ) {
            if( _IsDirectBase( curr ) ) {
                pushBase( &stack, curr, curr->type->u.c.scope, top );
                status = (*rtn)( stack, parm );
                if( status == WALK_FINISH ) {
                    cleanStack( &stack );
                    return;
                }
                if( status == WALK_ABANDON ) {
                    popBase( &stack );
                }
            }
        } RingIterEnd( curr )
    }
}

static void walkInheritedDirectBases( SCOPE scope, walk_routine rtn, void *parm)
{
    walk_status status;
    BASE_CLASS *curr;
    BASE_STACK *top;
    BASE_STACK *stack;

    stack = NULL;
    pushBase( &stack, NULL, scope, NULL );
    for( ; (top = popBase( &stack )) != NULL; ) {
        RingIterBeg( ScopeInherits( top->scope ), curr ) {
            if( _IsDirectBase( curr ) ) {
                pushBase( &stack, curr, curr->type->u.c.scope, top );
                status = (*rtn)( stack, parm );
                if( status == WALK_FINISH ) {
                    cleanStack( &stack );
                    return;
                }
                if( status == WALK_ABANDON ) {
                    popBase( &stack );
                }
            }
        } RingIterEnd( curr )
    }
}

static void walkImmediateBases( SCOPE scope, walk_routine rtn, void *parm )
{
    walk_status status;
    BASE_CLASS *curr;
    BASE_STACK *top;
    BASE_STACK *stack;

    stack = NULL;
    pushBase( &stack, NULL, scope, NULL );
    for( ; (top = popBase( &stack )) != NULL; ) {
        RingIterBeg( ScopeInherits( top->scope ), curr ) {
            if( _IsDirectBase( curr ) ) {
                pushBase( &stack, curr, curr->type->u.c.scope, top );
                status = (*rtn)( stack, parm );
                if( status == WALK_FINISH ) {
                    cleanStack( &stack );
                    return;
                }
                popBase( &stack );
            }
        } RingIterEnd( curr )
    }
}

static void walkVisitOnce( SCOPE scope, walk_routine rtn, void *parm )
{
    walk_status status;
    BASE_CLASS *curr;
    BASE_STACK *top;
    BASE_STACK *stack;

    stack = NULL;
    pushBase( &stack, NULL, scope, NULL );
    status = (*rtn)( stack, parm );
    if( status == WALK_FINISH ) {
        cleanStack( &stack );
        return;
    }
    if( status == WALK_ABANDON ) {
        popBase( &stack );
    }
    top = stack;
    RingIterBeg( ScopeInherits( scope ), curr ) {
        if( _IsVirtualBase( curr ) ) {
            pushBase( &stack, curr, curr->type->u.c.scope, top );
            status = (*rtn)( stack, parm );
            if( status == WALK_FINISH ) {
                cleanStack( &stack );
                return;
            }
            if( status == WALK_ABANDON ) {
                popBase( &stack );
            }
        }
    } RingIterEnd( curr )
    for( ; (top = popBase( &stack )) != NULL; ) {
        RingIterBeg( ScopeInherits( top->scope ), curr ) {
            if( _IsDirectBase( curr ) && ! _IsVirtualBase( curr ) ) {
                pushBase( &stack, curr, curr->type->u.c.scope, top );
                status = (*rtn)( stack, parm );
                if( status == WALK_FINISH ) {
                    cleanStack( &stack );
                    return;
                }
                if( status == WALK_ABANDON ) {
                    popBase( &stack );
                }
            }
        } RingIterEnd( curr )
    }
}

static inherit_flag findPtrOffset( BASE_STACK *top, SCOPE scope, CLASS_TABLE *table )
{
    inherit_flag access_flags;
    target_offset_t delta;
    TYPE virtual_base;
    BASE_CLASS *base;
    BASE_CLASS *vbase;

    delta = table->delta;
    access_flags = IN_NULL;
    virtual_base = NULL;
    for( ; top != NULL; top = top->parent ) {
        base = top->base;
        if( base == NULL )
            break;
        access_flags |= base->flag;
        if( _IsDirectVirtualBase( base ) ) {
            if( virtual_base == NULL ) {
                virtual_base = base->type;
                break;
            }
        } else {
            delta += base->delta;
        }
    }
    table->delta = delta;
    if( virtual_base != NULL ) {
        table->vb_offset = ScopeClass( scope )->u.c.info->vb_offset;
        vbase = ScopeFindVBase( scope, virtual_base );
        if( vbase->flag & IN_CTOR_DISP ) {
            table->ctor_disp = TRUE;
        } else {
            table->ctor_disp = FALSE;
        }
        table->vb_index = vbase->vb_index;
        delta += vbase->delta;
    }
    table->exact_delta = delta;
    return( access_flags );
}

static walk_status countBases( BASE_STACK *top, void *parm )
{
    boolean err;
    SCOPE scope;
    TYPE base_type;
    unsigned *data = parm;

    (*data)++;
    scope = top->scope;
    base_type = ScopeClass( scope );
    TypeSigFind( TSA_NO_ACCESS, base_type, NULL, &err );
    DbgAssert( ! err );
    return( WALK_NORMAL );
}


unsigned ScopeRttiClasses( TYPE class_type )
/******************************************/
{
    unsigned count;

    count = 0;
    walkVisitOnce( class_type->u.c.scope, countBases, &count );
    return( count );
}

#ifndef NDEBUG
static walk_status verifyOfIsNull( BASE_STACK *top, void *parm )
{
    SCOPE scope;
    TYPE base_type;

    parm = parm;
    scope = top->scope;
    base_type = ScopeClass( scope );
    DbgAssert( base_type->of == NULL );
    return( WALK_NORMAL );
}
#endif

static walk_status addLeaps( BASE_STACK *top, void *parm )
{
    rtti_leap_walk *data = parm;
    SCOPE scope;
    TYPE base_type;
    GEN_LEAP *leap;

    scope = top->scope;
    base_type = ScopeClass( scope );
    leap = (GEN_LEAP *) base_type->of;
    if( leap != NULL ) {
        DbgAssert( leap->type == base_type );
        leap->control |= RL_AMBIGUOUS;
        return( WALK_NORMAL );
    }
    data->count++;
    leap = RingCarveAlloc( carveGEN_LEAP, &(data->list) );
    leap->type = base_type;
    leap->vb_index = 0;
    leap->offset = 0;
    leap->control = RL_NULL;
    base_type->of = (TYPE) leap;
    return( WALK_NORMAL );
}

static walk_status fillLeaps( BASE_STACK *top, void *parm )
{
    rtti_leap_walk *data = parm;
    SCOPE scope;
    TYPE base_type;
    GEN_LEAP *leap;
    inherit_flag access_flags;
    auto CLASS_TABLE location;

    scope = top->scope;
    base_type = ScopeClass( scope );
    leap = (GEN_LEAP *) base_type->of;
    DbgAssert( leap != NULL );
    if( leap->control & RL_AMBIGUOUS ) {
        return( WALK_NORMAL );
    }
    location.delta = 0;
    access_flags = findPtrOffset( top, data->start, &location );
    if(( access_flags & ( IN_PRIVATE | IN_PROTECTED )) == 0 ) {
        leap->control |= RL_PUBLIC;
    }
    if( location.delta != location.exact_delta ) {
        leap->vb_index = location.vb_index;
        leap->control |= RL_VIRTUAL;
    }
    leap->offset = location.delta;
    return( WALK_NORMAL );
}

static walk_status clearLeapOf( BASE_STACK *top, void *parm )
{
    SCOPE scope;
    TYPE base_type;

    parm = parm;
    scope = top->scope;
    base_type = ScopeClass( scope );
    base_type->of = NULL;
    return( WALK_NORMAL );
}

unsigned ScopeRttiLeaps( TYPE class_type, GEN_LEAP **head )
/*********************************************************/
{
    SCOPE scope;
    auto rtti_leap_walk data;

    /* kludge alert: this routine uses TYP_CLASS->of to cache GEN_LEAPs */
    scope = class_type->u.c.scope;
    DbgStmt( walkVisitOnce( scope, verifyOfIsNull, NULL ) );
    data.count = 0;
    data.list = NULL;
    data.start = scope;
    walkVisitOnce( scope, addLeaps, &data );
    walkDirectBases( scope, fillLeaps, &data );
    walkVisitOnce( scope, clearLeapOf, NULL );
    *head = data.list;
    return( data.count );
}


void ScopeRttiFreeLeaps( GEN_LEAP *head )
/***************************************/
{
    RingCarveFree( carveGEN_LEAP, &head );
}


static walk_status findBoundBase( BASE_STACK *top, void *parm )
{
    bound_base_walk *data = parm;
    TYPE base_type;
    CLASSINFO *info;
    SCOPE scope;

    scope = top->scope;
    base_type = ScopeClass( scope );
    if( base_type->flag & TF1_INSTANTIATION ) {
        info = base_type->u.c.info;
        if( info->name == data->name ) {
            if( data->found != NULL ) {
                data->found = NULL;
                return( WALK_FINISH );
            }
            data->found = base_type;
            // don't WALK_ABANDON since this class may inherit from its own template
        }
    }
    return( WALK_NORMAL );
}


TYPE ScopeFindBoundBase( TYPE b_derived, TYPE u_base )
/****************************************************/
{
    bound_base_walk data;

    data.name = u_base->u.c.info->name;
    data.found = NULL;
    walkVisitOnce( b_derived->u.c.scope, findBoundBase, &data );
    return( data.found );
}

static walk_status countScope( BASE_STACK *top, void *parm )
{
    count_copy_walk *data = parm;
    SCOPE scope;

    scope = top->scope;
    if( scope == data->found ) {
        data->copies++;
        return( WALK_ABANDON );
    }
    return( WALK_NORMAL );
}

static unsigned findNumberCopies( SCOPE derived, SCOPE base )
{
    count_copy_walk data;

    data.found = base;
    data.copies = 0;
    walkVisitOnce( derived, countScope, &data );
    return( data.copies );
}

static walk_status derivedBase( BASE_STACK *top, void *parm )
{
    scope_derived_walk *data = parm;
    BASE_CLASS *base;

    if( top->scope == data->base ) {
        data->copies++;
        data->is_derived = TRUE;
        for( ; top != NULL; top = top->parent ) {
            base = top->base;
            if( base == NULL )
                break;
            if( _IsVirtualBase( base ) ) {
                data->virtual_base = TRUE;
                break;
            }
        }
        return( WALK_ABANDON );
    }
    return( WALK_NORMAL );
}

static walk_status quickDerivedBase( BASE_STACK *top, void *parm )
{
    derived_walk *data = parm;

    if( top->scope == data->base ) {
        data->is_derived = TRUE;
        return( WALK_FINISH );
    }
    return( WALK_NORMAL );
}

static boolean notABase( SCOPE derived, SCOPE possible_base )
{
    TYPE derived_type;
    TYPE base_type;

    derived_type = ScopeClass( derived );
    base_type = ScopeClass( possible_base );
    if( base_type->u.c.info->index > derived_type->u.c.info->index ) {
        /* impossible for 'derived' to be derived from 'possible_base' */
        /* since the 'possible_base' was declared after the 'derived' class */
        return( TRUE );
    }
    /* equality must be "don't know" since we use the same idx when we run out */
    return( FALSE );
}

static derived_status isQuickScopeDerived( SCOPE derived, SCOPE possible_base )
{
    auto derived_walk data;

#ifndef NDEBUG
    if( derived == NULL || ! _IsClassScope( derived ) ) {
        CFatal( "bad derived scope used for derivation check" );
    }
#endif
    if( derived == possible_base ) {
        return( DERIVED_YES );
    }
    if( notABase( derived, possible_base ) ) {
        return( DERIVED_NO );
    }
    data.base = possible_base;
    data.virtual_base = FALSE;
    data.is_derived = FALSE;
    walkVisitOnce( derived, quickDerivedBase, &data );
    if( ! data.is_derived ) {
        return( DERIVED_NO );
    }
    return( DERIVED_YES );
}

static derived_status isScopeDerived( scope_derived_walk *data )
{
    SCOPE derived;
    SCOPE possible_base;

    DbgAssert( data->derived != NULL && _IsClassScope( data->derived ) );
    derived = data->derived;
    possible_base = data->base;
    data->copies = 0;
    data->virtual_base = FALSE;
    data->is_derived = FALSE;
    if( derived == possible_base ) {
        data->copies++;
        data->is_derived = TRUE;
        return( DERIVED_YES );
    }
    if( notABase( derived, possible_base ) ) {
        return( DERIVED_NO );
    }
    walkVisitOnce( derived, derivedBase, data );
    if( ! data->is_derived ) {
        return( DERIVED_NO );
    }
    if( data->virtual_base ) {
        return( DERIVED_YES_BUT_VIRTUAL );
    }
    return( DERIVED_YES );
}

static boolean isFriendly( SCOPE check, SCOPE friendly )
{
    FRIEND *a_friend;
    SYMBOL sym;
    TYPE type;

    RingIterBeg( ScopeFriends( friendly ), a_friend ) {
        if( FriendIsType( a_friend ) ) {
            /* friendly classes */
            type = FriendGetType( a_friend );
            if( ( type->id == TYP_CLASS )
             && ( type->flag & TF1_UNBOUND )
             && ( type->of != NULL ) ) {
                type = type->of;
            }
            if( type->u.c.scope == check ) {
                return( TRUE );
            }
        } else {
            sym = FriendGetSymbol( a_friend );
            if( sym->id == SC_CLASS_TEMPLATE ) {
                /* friendly class templates */
                type = ScopeClass( check );
                if( type != NULL ) {
                    if( type->flag & TF1_INSTANTIATION ) {
                        if( type->u.c.info->name == sym->name->name ) {
                            /* class scope must be instantiated from class template */
                            return( TRUE );
                        }
                    }
                }
            } else {
                /* friendly functions */
                if( SymIsFunction( sym ) ) {
                    SYMBOL curr_sym = ScopeFunctionInProgress();
                    if( sym == curr_sym ) {
                        return( TRUE );
                    }
                    if( ( curr_sym != NULL )
                     && ( curr_sym->flag & SF_TEMPLATE_FN )
                     && ( sym == curr_sym->u.alias ) ) {
                        return( TRUE );
                    }
                }
            }
        }
    } RingIterEnd( a_friend )
    return( FALSE );
}

static SCOPE nextAccessScope( SCOPE scope )
{
    if( _IsClassScope( scope ) ) {
        /* skip enclosing class scopes */
        do {
            scope = scope->enclosing;
            if( scope == NULL ) break;
        } while( _IsClassScope( scope ) );
    } else {
        scope = scope->enclosing;
    }
    return( scope );
}

static boolean isScopeFriend( SCOPE access, SCOPE friendly )
{
    SCOPE check;

    if( ScopeFriends( friendly ) == NULL ) {
        /* no friends? no external scopes can access members */
        return( FALSE );
    }
    for( check = access; check != NULL; check = nextAccessScope( check ) ) {
        if( _IsClassScope( check ) || _IsFunctionScope( check ) ) {
            if( isFriendly( check, friendly ) ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

static boolean protectedPathOK( access_data *data )
{
    BASE_PATH *path;
    SCOPE check;

    data->perm = IN_PRIVATE;
    for( path = data->path; path != NULL; path = path->prev ) {
        check = path->scope;
        data->path = path;
        data->located = check;
        if( verifyAccess( data ) == IN_PUBLIC ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static inherit_flag verifyAccess( access_data *data )
{
    inherit_flag perm;
    SCOPE located;
    SCOPE access;
    SCOPE class_scope;
    BASE_PATH *path;

    perm = data->perm;
    if( perm == IN_PUBLIC ) {
        /* accessing public members is always OK */
        return( IN_PUBLIC );
    }
    /* see if we have visited this BASE_PATH entry before */
    path = data->path;
    if( path != NULL ) {
        if( path->checked_private ) {
            if( path->failed_private ) {
                if( perm == IN_PRIVATE ) {
                    return( IN_PRIVATE );
                }
            } else {
                return( IN_PUBLIC );
            }
        }
        path->checked_private = TRUE;
    }
    located = data->located;
    class_scope = data->member;
    while( class_scope != NULL ) {
        /* see 11.8 Nested classes [class.access.nest]: A nested class
         * is a member and as such has the same access rights as any
         * other member. */
        if( _IsClassScope( class_scope ) && ( class_scope == located ) ) {
            /* accessing your own members is always OK */
            return( IN_PUBLIC );
        }
        class_scope = class_scope->enclosing;
    }
    access = data->access;
    if( isScopeFriend( access, located ) ) {
        /* accessing a friend's members is always OK */
        return( IN_PUBLIC );
    }
    if( path != NULL ) {
        path->failed_private = TRUE;
    }
    if( perm == IN_PRIVATE ) {
        return( IN_PRIVATE );
    }
    if( perm == IN_PROTECTED ) {
        if( !data->protected_OK ) {
            if( path != NULL ) {
                if( protectedPathOK( data ) ) {
                    return( IN_PUBLIC );
                }
            }
            return( IN_PROTECTED );
        }
    }
    return( IN_PUBLIC );
}

static void newAccessData( access_data *data, inherit_flag perm, SCOPE located )
{
    SCOPE class_scope;
    SCOPE access;
    SCOPE check;

    /* find the lexically enclosing class scope (if present) */
    /* e.g, void C::foo -> C, void C::F::Q::bar -> Q */
    access = findAccessScope();
    class_scope = NULL;
    for( check = access; check != NULL; check = nextAccessScope( check ) ) {
        if( _IsClassScope( check ) ) {
            class_scope = check;
            break;
        }
    }
    data->path = NULL;
    data->access = access;
    data->member = class_scope;
    data->located = located;
    data->perm = perm;
    data->protected_OK = FALSE;
}

derived_status ScopeDerived( SCOPE derived, SCOPE possible_base )
/***************************************************************/
{
    derived_status status;
    inherit_flag violation;
    auto scope_derived_walk data;

    data.derived = derived;
    data.base = possible_base;
    status = isScopeDerived( &data );
    if( status != DERIVED_NO ) {
        if( data.copies != 1 ) {
            status = DERIVED_YES_BUT_AMBIGUOUS;
        } else {
            violation = checkBaseAccess( derived, possible_base, status );
            if( violation != IN_PUBLIC ) {
                if( violation == IN_PRIVATE ) {
                    status = DERIVED_YES_BUT_PRIVATE;
                } else {
                    status = DERIVED_YES_BUT_PROTECTED;
                }
            }
        }
    }
    return( status );
}

static walk_status colourScope( BASE_STACK *top, void *parm )
{
    common_base_walk *data = parm;
    SCOPE scope;
    TYPE class_type;
    CLASSINFO *info;

    scope = top->scope;
    class_type = ScopeClass( scope );
    info = class_type->u.c.info;
    info->common = data->colour;
    return( WALK_NORMAL );
}

static walk_status findCommonBase( BASE_STACK *top, void *parm )
{
    common_base_walk *data = parm;
    SCOPE scope;
    TYPE class_type;
    CLASSINFO *info;

    scope = top->scope;
    class_type = ScopeClass( scope );
    info = class_type->u.c.info;
    if( info->common == 0 ) {
        return( WALK_NORMAL );
    }
    PstkPush( &(data->common_bases), scope );
    /* every base of this class is obviously a common base */
    return( WALK_ABANDON );
}

SCOPE ScopeCommonBase( SCOPE scope_1, SCOPE scope_2 )
/***************************************************/
{
    common_base_walk data;
    SCOPE *top;
    SCOPE scope;
    SCOPE common;
    int status;

    PstkOpen( &data.common_bases );
    data.colour = 0;
    walkVisitOnce( scope_1, colourScope, &data );
    data.colour = 1;
    walkVisitOnce( scope_2, colourScope, &data );
    walkVisitOnce( scope_1, findCommonBase, &data );
    common = NULL;
    for( ; (top = (SCOPE *)PstkPop( &data.common_bases )) != NULL; ) {
        scope = *top;
        status = ScopeDerived( scope_1, scope );
        if( status == DERIVED_YES || status == DERIVED_YES_BUT_VIRTUAL ) {
            status = ScopeDerived( scope_2, scope );
            if( status == DERIVED_YES || status == DERIVED_YES_BUT_VIRTUAL ) {
                /* both scopes have the common base in an accessible manner */
                common = scope;
                break;
            }
        }
    }
    PstkClose( &data.common_bases );
    return( common );
}

static walk_status calcBaseDepth( BASE_STACK *top, void *parm )
{
    base_depth_walk *data = parm;
    BASE_CLASS *base;
    unsigned depth;

    if( top->scope == data->base ) {
        depth = 0;
        for( ; top != NULL; top = top->parent ) {
            base = top->base;
            if( base == NULL )
                break;
            ++depth;
        }
        if( depth < data->depth ) {
            data->depth = depth;
        }
        if( ! data->many_pathes ) {
            return( WALK_FINISH );
        }
    }
    return( WALK_NORMAL );
}

derived_status ScopeDerivedCount( SCOPE derived, SCOPE possible_base, unsigned *depth )
/*************************************************************************************/
{
    derived_status status;
    auto base_depth_walk data;

    *depth = 0;
    data.base = possible_base;
    data.depth = UINT_MAX;
    data.many_pathes = FALSE;
    status = ScopeDerived( derived, possible_base );
    switch( status ) {
    case DERIVED_YES_BUT_VIRTUAL:
        data.many_pathes = TRUE;
        /* fall through */
    case DERIVED_YES:
        walkDirectBases( derived, calcBaseDepth, &data );
        *depth = data.depth;
        break;
    }
    return( status );
}

static PATH_CAP *newCap( void )
{
    PATH_CAP *cap;

    cap = CarveAlloc( carvePATH_CAP );
    cap->head = NULL;
    cap->tail = NULL;
    cap->access_decl = NULL;
    cap->sym_name = NULL;
    cap->sym = NULL;
    cap->exclude = NULL;
    cap->flag = IN_NULL;
    cap->throw_away = FALSE;
    cap->across_virtual_base = FALSE;
    return( cap );
}

static void deleteCap( PATH_CAP *cap )
{
    BASE_PATH *path;
    BASE_PATH *next;

    for( path = cap->head; path != NULL; path = next ) {
        next = path->next;
        deletePath( path );
    }
    RingCarveFree( carveSYMBOL_EXCLUDE, &(cap->exclude) );
    CarveFree( carvePATH_CAP, cap );
}

static boolean okToUseAccess( lookup_walk *data, SYMBOL sym, BASE_PATH *path )
{
    SCOPE base_scope;

    base_scope = sym->sym_type->u.c.scope;
    if( base_scope != path->scope ) {
        /* access doesn't affect this path */
        return( FALSE );
    }
    if( data->specific_user_conv ) {
        if( ! TypesIdentical( data->type, sym->u.udc_type ) ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

static PATH_CAP *recordPath( lookup_walk *data, BASE_STACK *top,
                             SYMBOL_NAME sym_name, SYMBOL sym )
{
    BASE_CLASS *base;
    PATH_CAP *cap;
    BASE_PATH *path;
    BASE_PATH *last_path;
    SCOPE scope;
    SCOPE disambiguate_scope;
    SCOPE access_scope;
    SYMBOL access_sym;

    cap = newCap();
    cap->next = data->paths;
    data->paths = cap;
    data->path_count++;
    cap->sym_name = sym_name;
    cap->sym = sym;
    cap->throw_away = FALSE;
    disambiguate_scope = data->disambiguate;
    if( disambiguate_scope != NULL && _IsClassScope( disambiguate_scope ) ) {
        cap->throw_away = TRUE;
    }
    access_scope = NULL;
    last_path = NULL;
    for( ; top != NULL; top = top->parent ) {
        path = newPath( cap );
        scope = top->scope;
        if( scope == disambiguate_scope ) {
            cap->throw_away = FALSE;
        }
        path->scope = scope;
        base = top->base;
        path->base = base;
        if( base != NULL ) {
            if( base->flag & IN_VIRTUAL ) {
                cap->across_virtual_base = TRUE;
            }
            path->flag = base->flag;
        }
        access_sym = top->access_changed;
        if( access_sym != NULL ) {
            if( okToUseAccess( data, access_sym, last_path ) ) {
                /* override base access via the access declaration */
                last_path->flag = top->access_perm;
                access_scope = scope;
            }
        }
        last_path = path;
    }
    cap->access_decl = access_scope;
    return( cap );
}

static void recordLocation( lookup_walk *data,
                            SCOPE scope, SYMBOL_NAME sym_name )
{
    BASE_STACK *stack;

    stack = NULL;
    pushBase( &stack, NULL, scope, NULL );
    recordPath( data, stack, sym_name, NULL );
    cleanStack( &stack );
}

static SYMBOL foundUserConv( lookup_walk *data, SYMBOL syms )
{
    type_flag reqd_this_qualifier;
    type_flag curr_this_qualifier;
    SYMBOL sym;
    TYPE fn_type;
    TYPE reqd_type;

    /* probably should perform a full overload resolution (AFS) */
    reqd_this_qualifier = data->this_qualifier;
    reqd_type = data->type;
    RingIterBeg( syms, sym ) {
        if( SymIsConversionToType( sym, reqd_type ) ) {
            fn_type = FunctionDeclarationType( sym->sym_type );
            curr_this_qualifier = fn_type->u.f.args->qualifier;
            curr_this_qualifier &= reqd_this_qualifier;
            if( curr_this_qualifier == reqd_this_qualifier ) {
                if( data->virtual_override ) {
                    if(( fn_type->flag & TF1_VIRTUAL ) != 0 ) {
                        return( sym );
                    }
                } else {
                    return( sym );
                }
            }
        }
    } RingIterEnd( sym )
    return( NULL );
}

static inherit_flag makePerm( symbol_flag flag )
{
    if( flag & SF_PRIVATE ) {
        return( IN_PRIVATE );
    }
    if( flag & SF_PROTECTED ) {
        return( IN_PROTECTED );
    }
    return( IN_PUBLIC );
}

static FNOV_RESULT distinctVirtualFunction( SYMBOL *sym, SYMBOL fun2, NAME name )
{
    SYMBOL chk_sym;
    FNOV_RESULT check;

    check = IsOverloadedFuncDistinct( sym, fun2, name, FNC_DEFAULT );
    switch( check ) {
    case FNOV_NOT_DISTINCT_RETURN:
    case FNOV_EXACT_MATCH:
        chk_sym = *sym;
        if( chk_sym->id == SC_DEFAULT ) {
            *sym = NULL;
            check = FNOV_DISTINCT;
        }
        break;
    }
    return( check );
}

static FNOV_RESULT matchVirtualFunction( SYMBOL *sym, lookup_walk *data )
{
    return( distinctVirtualFunction( sym, data->fn_sym, data->name ) );
}


boolean ScopeSameVFuns( SYMBOL fun1, SYMBOL fun2 )
/************************************************/
{
    SYMBOL *a_fun1;             // - addr[ fun1 ]
    NAME name;                  // - name for checking
    boolean retn;               // - TRUE ==> is same virtual function

    a_fun1 = &fun1;
    name = fun1->name->name;
    switch( distinctVirtualFunction( a_fun1, fun2, name ) ) {
      case FNOV_NOT_DISTINCT_RETURN:
      case FNOV_EXACT_MATCH:
        retn = TRUE;
        break;
      default :
        retn = FALSE;
        break;
    }
    return retn;
}

static TYPE symReturnsClassRefPtr( SYMBOL sym, boolean *is_reference )
{
    TYPE fn_type;
    TYPE ptr_type;

    fn_type = FunctionDeclarationType( sym->sym_type );
    ptr_type = TypedefModifierRemove( fn_type->of );
    *is_reference = FALSE;
    if( ptr_type->flag & TF1_REFERENCE ) {
        *is_reference = TRUE;
    }
    /* needs to return TYP_MODIFIER flags so we can verify they are identical */
    return( StructType( ptr_type->of ) );
}

static boolean badVirtualReturn( SYMBOL old, SYMBOL new, boolean *return_thunk )
{
    SCOPE old_scope;
    SCOPE new_scope;
    TYPE old_base;
    TYPE new_base;
    boolean old_ref;
    boolean new_ref;
    derived_status derived;
    SEARCH_RESULT *result;

    old_base = symReturnsClassRefPtr( old, &old_ref );
    if( old_base == NULL ) {
        return( TRUE );
    }
    old_scope = old_base->u.c.scope;
    new_base = symReturnsClassRefPtr( new, &new_ref );
    if( new_base == NULL ) {
        return( TRUE );
    }
    new_scope = new_base->u.c.scope;
    if( old_ref != new_ref ) {
        /* both return types must be both pointers or references */
        return( TRUE );
    }
    /* 'new_scope' must be derived from 'old_scope' to be valid */
    derived = ScopeDerived( new_scope, old_scope );
    switch( derived ) {
    case DERIVED_YES:
    case DERIVED_YES_BUT_VIRTUAL:
        result = ScopeBaseResult( new_scope, old_scope );
        *return_thunk = TRUE;
        if( result->non_virtual && result->delta == 0 ) {
            *return_thunk = FALSE;
        }
        ScopeFreeResult( result );
        return( FALSE );
    }
    return( TRUE );
}

static SYMBOL sameVirtualFnSignature( SYMBOL_NAME sym_name, lookup_walk *data )
{
    SYMBOL chk_sym;
    TYPE fn_type;
    FNOV_RESULT check;
    boolean complain_if_virtual;
    boolean return_thunk;

    complain_if_virtual = FALSE;
    chk_sym = sym_name->name_syms;
    check = matchVirtualFunction( &chk_sym, data );
    switch( check ) {
    case FNOV_NOT_DISTINCT_RETURN:
        if( badVirtualReturn( chk_sym, data->fn_sym, &return_thunk ) ) {
            if( data->ok_to_diagnose ) {
                complain_if_virtual = TRUE;
            } else {
                chk_sym = NULL;
            }
        } else {
            if( return_thunk ) {
                data->return_thunk = TRUE;
            }
        }
        break;
    case FNOV_EXACT_MATCH:
        break;
    default:
        chk_sym = NULL;
    }
    if( chk_sym != NULL ) {
        fn_type = FunctionDeclarationType( chk_sym->sym_type );
        if( fn_type == NULL ) {
            chk_sym = NULL;
        } else {
            if(( fn_type->flag & TF1_VIRTUAL ) == 0 ) {
                chk_sym = NULL;
            } else {
                if( complain_if_virtual ) {
                    CErr2p( ERR_ATTEMPT_TO_OVERRIDE_RETURN, chk_sym );
                }
            }
        }
    }
    return( chk_sym );
}

static boolean anyVirtualFns( SYMBOL_NAME sym_name )
{
    SYMBOL sym;
    TYPE fn_type;

    RingIterBeg( sym_name->name_syms, sym ) {
        fn_type = FunctionDeclarationType( sym->sym_type );
        if( fn_type == NULL ) break;
        if( fn_type->flag & TF1_VIRTUAL ) {
            return( TRUE );
        }
    } RingIterEnd( sym )
    return( FALSE );
}

static SYMBOL recordVirtualOverride( lookup_walk *data, BASE_STACK *top, SYMBOL_NAME sym_name )
{
    BASE_CLASS *base;

    if( ! anyVirtualFns( sym_name ) ) {
        return( NULL );
    }
    if( ! data->same_table ) {
        data->same_table = 1;
        data->vfn_name = sym_name;
        for( ; top != NULL; top = top->parent ) {
            base = top->base;
            if( base != NULL ) {
                if( !_UsingVFIndices( base ) ) {
                    data->same_table = 0;
                    break;
                }
            }
        }
    }
    return( sameVirtualFnSignature( sym_name, data ) );
}

static walk_status memberSearch( BASE_STACK *top, void *parm )
{
    lookup_walk *data = parm;
    SYMBOL sym;
    SYMBOL syms;
    SYMBOL_NAME sym_name;
    SCOPE scope;
    SCOPE disambiguate;
    PATH_CAP *cap;

    scope = top->scope;
    if( data->name == NULL ) {
        disambiguate = data->disambiguate;
        if( disambiguate != NULL ) {
            /* we are looking for a specific base class */
            /* if disambig cannot be derived from scope; skip this subgraph */
            if( notABase( scope, disambiguate ) ) {
                return( WALK_ABANDON );
            }
            if( scope == disambiguate ) {
                recordPath( data, top, NULL, NULL );
                if( ! data->find_all ) {
                    return( WALK_FINISH );
                }
            }
        }
        return( WALK_NORMAL );
    }
    sym_name = HashLookup( scope->names, data->name );
    if( sym_name == NULL ) {
        return( WALK_NORMAL );
    }
    sym = NULL;
    if( data->check_special ) {
        if( ! data->is_special( sym_name ) ) {
            /* this wasn't what we were looking for; keep going */
            return( WALK_NORMAL );
        }
    } else {
        syms = sym_name->name_syms;
        if( syms != NULL && syms->id == SC_ACCESS ) {
            /* this member was an access control; record and keep going */
            top->access_perm = makePerm( syms->flag );
            top->access_changed = syms;
            return( WALK_NORMAL );
        }
        if( data->specific_user_conv ) {
            sym = foundUserConv( data, syms );
            if( sym == NULL ) {
                return( WALK_NORMAL );
            }
        } else if( data->virtual_override ) {
            sym = recordVirtualOverride( data, top, sym_name );
            if( sym == NULL ) {
                return( WALK_NORMAL );
            }
        }
    }
    if( scope == data->ignore ) {
        /* we don't want to look here for objects and fns */
        return( WALK_NORMAL );
    }
    cap = recordPath( data, top, sym_name, sym );
    if( data->best_user_conv ) {
        return( WALK_NORMAL );
    }
    if( data->disambiguate == NULL || !cap->throw_away ) {
        return( WALK_ABANDON );
    }
    return( WALK_NORMAL );
}


#ifndef NDEBUG
static void dumpSearch( lookup_walk *data )
{
    PATH_CAP *cap;
    BASE_PATH *path;

    if( ! PragDbgToggle.dump_scopes ) {
        return;
    }
    for( cap = data->paths; cap != NULL; cap = cap->next ) {
        if( cap->throw_away ) {
            putchar( '*' );
        } else {
            putchar( ' ' );
        }
        path = cap->head;
        for(;;) {
            printScopeName( path->scope, NULL );
            path = path->next;
            if( path == NULL )
                break;
            switch( path->flag & IN_ACCESS_SPECIFIED ) {
            case IN_PRIVATE:
                printf( "pri" );
                break;
            case IN_PROTECTED:
                printf( "pro" );
                break;
            case IN_PUBLIC:
                printf( "pub" );
                break;
            }
            if( _IsDirectVirtualBase( path ) ) {
                printf( "===> " );
            } else {
                printf( "---> " );
            }
        }
        if( cap->sym_name != NULL ) {
            printf( ". %s\n", cap->sym_name->name );
        } else {
            putchar( '\n' );
        }
    }
    putchar( '\n' );
}
#endif

static void newLookupData( lookup_walk *data, NAME name )
{
    DbgStmt( memset( data, -1, sizeof( *data ) ) );
    data->start = NULL;
    data->disambiguate = NULL;
    data->consider_mask = ~0;
    data->ignore = NULL;
    data->name = name;
    data->type = NULL;
    data->fn_type = NULL;
    data->fn_sym = NULL;
    data->is_special = NULL;
    data->top = NULL;
    data->info1 = NULL;
    data->info2 = NULL;
    data->user_conv_list = NULL;
    data->paths = NULL;
    data->path_count = 0;
    data->perm = IN_NULL;
    data->this_qualifier = TF1_NULL;
    data->virtual_override = FALSE;
    data->user_conversion = FALSE;
    data->specific_user_conv = FALSE;
    data->best_user_conv = FALSE;
    data->check_special = FALSE;
    data->no_inherit = FALSE;
    data->only_inherit = FALSE;
    data->only_bases = FALSE;
    data->ok_to_diagnose = FALSE;
    data->find_all = FALSE;
    data->ignore_access = FALSE;
    data->ambiguous = FALSE;
    data->overload_reqd = FALSE;
    data->use_this = FALSE;
    data->no_this = FALSE;
    data->saw_function = FALSE;
    data->saw_class = FALSE;
    data->use_index = FALSE;
    data->return_thunk = FALSE;
    data->protected_OK = FALSE;
    data->file_class_done = FALSE;
    data->file_ns_done = FALSE;
    data->same_table = FALSE;
    data->lookup_error = FALSE;
    data->member_lookup = FALSE;
}

static void removeDead( lookup_walk *data )
{
    PATH_CAP *cap;
    PATH_CAP *next;
    PATH_CAP **prev;

    DbgStmt( dumpSearch( data ) );
    prev = &(data->paths);
    for( cap = data->paths; cap != NULL; cap = next ) {
        next = cap->next;
        if( cap->throw_away ) {
            *prev = next;
            deleteCap( cap );
            data->path_count--;
        } else {
            prev = &(cap->next);
        }
    }
}

static void delLookupData( lookup_walk *data )
{
    PATH_CAP *cap;

    for( cap = data->paths; cap != NULL; cap = cap->next ) {
        cap->throw_away = TRUE;
    }
    removeDead( data );
}

static void reduceToOnePath( lookup_walk *data )
{
    PATH_CAP *cap;
    unsigned count;

    count = data->path_count;
    if( count <= 1 ) {
        return;
    }
    --count;
    for( cap = data->paths; cap != NULL; cap = cap->next ) {
        if( count == 0 )
            break;
        cap->throw_away = TRUE;
        --count;
    }
    removeDead( data );
}

static SEARCH_RESULT *newResult( SCOPE scope, SYMBOL_NAME sym_name )
{
    SEARCH_RESULT *result;

    result = CarveAlloc( carveSEARCH_RESULT );
    result->sym_name = sym_name;
    result->sym = NULL;
    result->region = NULL;
    result->scope = scope;
    result->start = NULL;
    result->access_decl = NULL;
    result->delta = 0;
    result->exact_delta = 0;
    result->vb_index = 0;
    result->vb_offset = 0;
    result->offset = 0;
    result->vf_offset = 0;
    result->perm = IN_NULL;
    result->simple = FALSE;
    result->non_virtual = FALSE;
    result->ambiguous = FALSE;
    result->mixed_static = FALSE;
    result->use_this = FALSE;
    result->no_this = FALSE;
    result->cant_be_auto = FALSE;
    result->protected_OK = FALSE;
    result->ignore_access = FALSE;
    result->lookup_error = FALSE;
    result->errlocn.src_file = NULL;

    return( result );
}

static void applyDominance( lookup_walk *data )
{
    /*
        if a path ends in a class that is a base class of the end of
        another path, remove it because it is dominated (ARM p. 205)

        optimizations:
            - use faster derived check instead of the general one that
              counts the number of classes involved
            - only check two accesses if at least one of them goes
              through a virtual base class
    */
    PATH_CAP *cap1;
    PATH_CAP *cap2;
    PATH_CAP *next1;
    PATH_CAP *next2;
    BASE_PATH *path1;
    BASE_PATH *path2;
    SCOPE scope1;
    SCOPE scope2;
    TYPE class_type;
    derived_status status;

    class_type = ScopeClass( data->start );
    if( ! class_type->u.c.info->lattice ) {
        /* dominance can only occur if a virtual base is shared */
        return;
    }
    for( cap1 = data->paths; cap1 != NULL; cap1 = next1 ) {
        next1 = cap1->next;
        path1 = cap1->tail;
        scope1 = path1->scope;
        for( cap2 = next1; cap2 != NULL; cap2 = next2 ) {
            next2 = cap2->next;
            if( cap2->throw_away ) {
                continue;
            }
            path2 = cap2->tail;
            scope2 = path2->scope;
            if( scope1 == scope2 ) {
                continue;
            }
            if( ! cap1->across_virtual_base && ! cap2->across_virtual_base ) {
                /* at least one of the pathes must be through a virtual base */
                continue;
            }
            status = isQuickScopeDerived( scope1, scope2 );
            if( status != DERIVED_NO ) {
                cap2->throw_away = TRUE;
            } else {
                status = isQuickScopeDerived( scope2, scope1 );
                if( status != DERIVED_NO ) {
                    cap1->throw_away = TRUE;
                }
            }
        }
    }
    removeDead( data );
}

static SCOPE differentScopesAmbiguity( lookup_walk *data )
{
    /*
        if a path ends in a class that is different than the end class
        in another path, we have ambiguity
    */
    PATH_CAP *next;
    PATH_CAP *cap;
    SCOPE scope;

    DbgAssert( data->path_count > 1 );
    cap = data->paths;
    scope = cap->tail->scope;
    for( ; (next = cap->next) != NULL; cap = next ) {
        if( next->tail->scope != scope ) {
            data->ambiguous = TRUE;
            return( NULL );
        }
    }
    return( scope );
}

static void differentCopiesAmbiguity( lookup_walk *data )
{
    /*
        we know every path ends in the same scope therefore there must be
        one unique copy of the class.  The exception to this is that we
        may find a special member that is not duplicated for every class
        (i.e., enumerated constant, static member, or typedef).
    */
    TYPE class_type;
    PATH_CAP *cap;
    BASE_PATH *path;
    SCOPE scope;
    SYMBOL sym;
    SYMBOL_NAME sym_name;
    CLASSINFO *info;
    struct {
        unsigned static_found : 1;
        unsigned nonstatic_found : 1;
    } flag;

    cap = data->paths;
    path = cap->tail;
    scope = path->scope;
    class_type = ScopeClass( scope );
    info = class_type->u.c.info;
    sym_name = cap->sym_name;
    if( sym_name == NULL ) {
        data->ambiguous = TRUE;
        return;
    }
    if( data->check_special && data->is_special( sym_name ) ) {
        return;
    }
    if( VariableName( sym_name ) ) {
        sym = sym_name->name_syms;
        if( sym->id == SC_ENUM ) {
            return;
        }
        if( sym->id == SC_STATIC ) {
            return;
        }
        data->ambiguous = TRUE;
        return;
    }
    /* we found a function */
    flag.static_found = FALSE;
    flag.nonstatic_found = FALSE;
    RingIterBeg( sym_name->name_syms, sym ) {
        if( SymIsStatic( sym ) ) {
            flag.static_found = TRUE;
        } else {
            flag.nonstatic_found = TRUE;
        }
    } RingIterEnd( sym )
    if( flag.nonstatic_found ) {
        if( flag.static_found ) {
            data->overload_reqd = TRUE;
        } else {
            data->ambiguous = TRUE;
        }
    }
}

static void applyAmbiguity( SCOPE start, lookup_walk *data )
{
    unsigned copies;
    unsigned disambig_copies;
    SCOPE disambig;
    SCOPE scope;

    DbgAssert( data->path_count > 1 );
    scope = differentScopesAmbiguity( data );
    if( data->ambiguous ) {
        reduceToOnePath( data );
        return;
    }
    copies = findNumberCopies( start, scope );
    if( copies > 1 ) {
        disambig = data->disambiguate;
        if( disambig != NULL ) {
            // to count a disambiguated member; count copies
            // within 'disambig' and multiply by number of
            // copies of 'disambig' within 'start'
            disambig_copies = findNumberCopies( start, disambig );
            DbgAssert( disambig_copies != 0 );
            copies = findNumberCopies( disambig, scope );
            DbgAssert( copies != 0 );
            copies *= disambig_copies;
        }
    }
    DbgAssert( copies >= 1 );
    if( copies > 1 ) {
        differentCopiesAmbiguity( data );
        if( data->ambiguous ) {
            reduceToOnePath( data );
        }
    }
}

static inherit_flag setAccess( PATH_CAP *cap )
{
    SCOPE scope;
    BASE_PATH *path;
    BASE_PATH *next;
    inherit_flag perm;
    auto access_data access_data;

    perm = IN_PUBLIC;
    for( path = cap->head; path != NULL; path = next ) {
        next = path->next;
        if( next == NULL )
            break;
        perm = next->flag & IN_ACCESS_SPECIFIED;
        scope = path->scope;
        newAccessData( &access_data, perm, scope );
        access_data.path = path;
        perm = verifyAccess( &access_data );
        if( perm != IN_PUBLIC ) {
            break;
        }
    }
    cap->flag = perm;
    return( perm );
}

static boolean setProtectedAccess( lookup_walk *data, PATH_CAP *cap )
{
    SYMBOL_NAME sym_name;
    SYMBOL syms;
    SYMBOL sym;
    BASE_PATH *path;
    boolean protected_checked;
    auto access_data access_data;

    if( data->protected_OK ) {
        return( TRUE );
    }
    if( cap == NULL ) {
        cap = data->paths;
    }
    path = cap->tail;
    newAccessData( &access_data, IN_PROTECTED, path->scope );
    access_data.path = path;
    sym = cap->sym;
    protected_checked = FALSE;
    if( sym != NULL ) {
        if( sym->flag & SF_PROTECTED ) {
            protected_checked = TRUE;
            if( protectedPathOK( &access_data ) ) {
                data->protected_OK = TRUE;
            }
        }
    } else {
        sym_name = cap->sym_name;
        if( sym_name != NULL ) {
            syms = sym_name->name_syms;
            if( syms == NULL ) {
                syms = sym_name->name_type;
            }
            RingIterBeg( syms, sym ) {
                if( sym->flag & SF_PROTECTED ) {
                    protected_checked = TRUE;
                    if( protectedPathOK( &access_data ) ) {
                        data->protected_OK = TRUE;
                    }
                    break;
                }
            } RingIterEnd( sym )
        }
    }
    return( protected_checked );
}

static void findBestAccess( lookup_walk *data )
{
    /*
      find the best access path through a virtual base lattice
    */
    PATH_CAP *first_cap;
    PATH_CAP *cap;
    inherit_flag perm;

    first_cap = data->paths;
    for( cap = first_cap; cap != NULL; cap = cap->next ) {
        perm = setAccess( cap );
        cap->flag = perm;
        if( perm != IN_PUBLIC ) {
            /* base access error someplace */
            cap->throw_away = TRUE;
        } else {
            first_cap = cap;
        }
    }
    if( first_cap->throw_away ) {
        /* every path had a problem so keep one */
        first_cap->throw_away = FALSE;
    }
    removeDead( data );
    if( data->path_count == 1 ) {
        return;
    }
    /* we have more than one path that have good base access so
       now we consider the symbol access.  We have 3 cases:
        (1) PUBLIC      action: keep one
        (2) PRIVATE     action: keep one
        (3) PROTECTED   action: keep any that allows protected access
    */
    first_cap = data->paths;
    for( cap = first_cap; cap != NULL; cap = cap->next ) {
        if( setProtectedAccess( data, cap ) ) {
            if( data->protected_OK ) {
                first_cap = cap;
                for( cap = first_cap->next; cap != NULL; cap = cap->next ) {
                    cap->throw_away = TRUE;
                }
                break;
            }
            cap->throw_away = TRUE;
        } else {
            /* symbol doesn't need protected access checked */
            reduceToOnePath( data );
            return;
        }
    }
    if( first_cap->throw_away ) {
        /* every path had a problem so keep one */
        first_cap->throw_away = FALSE;
    }
    removeDead( data );
}

static boolean symbolIsExcluded( PATH_CAP *cap, SYMBOL sym )
{
    SYMBOL_EXCLUDE *exclude;

    RingIterBeg( cap->exclude, exclude ) {
        if( exclude->sym == sym ) {
            return( TRUE );
        }
    } RingIterEnd( exclude )
    return( FALSE );
}

static void excludeSymbol( PATH_CAP *cap, SYMBOL sym )
{
    SYMBOL_EXCLUDE *exclude;

    if( symbolIsExcluded( cap, sym ) ) {
        return;
    }
    exclude = RingCarveAlloc( carveSYMBOL_EXCLUDE, &cap->exclude );
    exclude->sym = sym;
}

static void applyUniqueSymbolName( lookup_walk *data )
{
    /* make sure all SYMBOL_NAMEs are unique */
    SYMBOL_NAME sym_name1;
    SYMBOL_NAME sym_name2;
    PATH_CAP *cap1;
    PATH_CAP *cap2;
    PATH_CAP *next1;
    PATH_CAP *next2;

    for( cap1 = data->paths; cap1 != NULL; cap1 = next1 ) {
        next1 = cap1->next;
        if( cap1->throw_away ) {
            continue;
        }
        sym_name1 = cap1->sym_name;
        for( cap2 = next1; cap2 != NULL; cap2 = next2 ) {
            next2 = cap2->next;
            if( cap2->throw_away ) {
                continue;
            }
            sym_name2 = cap2->sym_name;
            if( sym_name1 == sym_name2 ) {
                /* same SYMBOL_NAME (we only need one for overloading) */
                cap2->throw_away = TRUE;
            }
        }
    }
    removeDead( data );
}

static void performOverride( PATH_CAP *cap1, PATH_CAP *cap2 )
{
    /* SYMBOLs in 'cap1' override SYMBOLs in 'cap2' */
    SYMBOL sym1;
    SYMBOL sym2;
    TYPE return_type2;

    RingIterBeg( cap2->sym_name->name_syms, sym2 ) {
        return_type2 = FunctionDeclarationType( sym2->sym_type )->of;
        RingIterBeg( cap1->sym_name->name_syms, sym1 ) {
            if( SymIsConversionToType( sym1, return_type2 ) ) {
                excludeSymbol( cap2, sym2 );
                break;
            }
        } RingIterEnd( sym1 )
    } RingIterEnd( sym2 )
}

static void applyOverrideConversion( lookup_walk *data )
{
    PATH_CAP *cap1;
    PATH_CAP *cap2;
    PATH_CAP *next1;
    PATH_CAP *next2;
    BASE_PATH *path1;
    BASE_PATH *path2;
    SCOPE scope1;
    SCOPE scope2;
    derived_status status;

    for( cap1 = data->paths; cap1 != NULL; cap1 = next1 ) {
        next1 = cap1->next;
        path1 = cap1->tail;
        scope1 = path1->scope;
        for( cap2 = next1; cap2 != NULL; cap2 = next2 ) {
            next2 = cap2->next;
            path2 = cap2->tail;
            scope2 = path2->scope;
            status = isQuickScopeDerived( scope1, scope2 );
            if( status != DERIVED_NO ) {
                performOverride( cap1, cap2 );
            } else {
                status = isQuickScopeDerived( scope2, scope1 );
                if( status != DERIVED_NO ) {
                    performOverride( cap2, cap1 );
                }
            }
        }
    }
}

static FNOV_LIST *gatherOverloadList( lookup_walk *data )
{
    FNOV_LIST *root;
    PATH_CAP *cap;
    PATH_CAP *next;
    SYMBOL sym;

    root = NULL;
    for( cap = data->paths; cap != NULL; cap = next ) {
        next = cap->next;
        RingIterBeg( cap->sym_name->name_syms, sym ) {
            if( ! symbolIsExcluded( cap, sym ) ) {
                BuildUdcList( &root, sym );
            }
        } RingIterEnd( sym )
    }
    return( root );
}

static boolean findBestConversion( lookup_walk *data )
{
    applyUniqueSymbolName( data );
    applyOverrideConversion( data );
    data->user_conv_list = gatherOverloadList( data );
    return( TRUE );
}

static void applySameVTable( lookup_walk *data )
{
    /* reduce to one unique SYMBOL (favour function using scope's vftable) */
    PATH_CAP *cap;
    BASE_CLASS *base;
    BASE_PATH *path;
    PATH_CAP *best_cap;

    best_cap = NULL;
    for( cap = data->paths; cap != NULL; cap = cap->next ) {
        for( path = cap->tail; path != NULL; path = path->prev ) {
            base = path->base;
            if( base == NULL )
                continue;
            if( !_UsingVFIndices( base ) ) {
                break;
            }
        }
        if( path == NULL ) {
            best_cap = cap;
        } else {
            cap->throw_away = TRUE;
        }
    }
    if( best_cap == NULL ) {
        data->paths->throw_away = FALSE;
    } else {
        data->use_index = TRUE;
    }
    removeDead( data );
    /* one path will be left */
}

static boolean findVirtualOverride( lookup_walk *data )
{
    applyDominance( data );
    if( data->path_count != 0 ) {
        applySameVTable( data );
        return( TRUE );
    }
    return( FALSE );
}

static void applyDisambiguation( lookup_walk *data )
{
    /* pathes are marked 'throw_away' if disambiguation scope was not in path */
    /* e.g., p->C::x (we only want all pathes to 'x' that go through 'C') */
    removeDead( data );
}

static boolean findSinglePath( lookup_walk *data, SCOPE start )
{
    data->start = start;
    if( data->no_inherit ) {
        walkOneScope( start, memberSearch, data );
    } else if( data->only_inherit ) {
        walkInheritedDirectBases( start, memberSearch, data );
    } else if( data->only_bases ) {
        walkImmediateBases( start, memberSearch, data );
    } else {
        walkDirectBases( start, memberSearch, data );
    }
    applyDisambiguation( data );
    if( data->paths == NULL ) {
        return( FALSE );
    }
    if( data->best_user_conv ) {
        return( findBestConversion( data ) );
    }
    if( data->virtual_override ) {
        return( findVirtualOverride( data ) );
    }
    if( data->path_count == 1 ) {
        return( TRUE );
    }
    applyDominance( data );
    if( data->path_count == 1 ) {
        return( TRUE );
    }
    if( data->ignore_access ) {
        reduceToOnePath( data );
        return( TRUE );
    }
    applyAmbiguity( start, data );
    if( data->path_count == 1 ) {
        return( TRUE );
    }
    findBestAccess( data );
    return( TRUE );
}

static boolean findMember( lookup_walk *data, SCOPE scope )
{
    PATH_CAP *cap;

    if( ! findSinglePath( data, scope ) ) {
        return( FALSE );
    }
    if( ! data->ignore_access ) {
        cap = data->paths;
        if(( cap->flag & IN_ACCESS_SPECIFIED ) == 0 ) {
            setAccess( cap );
        }
        setProtectedAccess( data, NULL );
    }
    return( TRUE );
}

static void setConsiderMask( lookup_walk *data, unsigned lexical )
{
    SCOPE disambig;
    unsigned mask;

    mask = ~0;
    if( data->user_conversion ) {
        mask = _ScopeMask( SCOPE_CLASS );
    }
    disambig = data->disambiguate;
    if( disambig != NULL ) {
        switch( disambig->id ) {
        case SCOPE_CLASS:
            mask = _ScopeMask( SCOPE_CLASS ) | lexical;
            break;
        case SCOPE_FILE:
            mask = _ScopeMask( SCOPE_BLOCK ) | _ScopeMask( SCOPE_FILE );
            break;
        DbgDefault( "unexpected disambig scope type" );
        }
    }
    data->consider_mask = mask;
}

static SYMBOL_NAME doLookup( lookup_walk *data, SCOPE scope )
{
    SYMBOL_NAME sym_name;

    sym_name = HashLookup( scope->names, data->name );
    if( sym_name == NULL ) {
        return( NULL );
    }
    if( data->check_special ) {
        if( ! data->is_special( sym_name ) ) {
            /* this name wasn't what we are looking for */
            return( NULL );
        }
    }
    return( sym_name );
}

static SYMBOL_NAME doRecordedLookup( lookup_walk *data, SCOPE scope )
{
    SYMBOL_NAME sym_name;

    sym_name = doLookup( data, scope );
    if( sym_name != NULL ) {
        recordLocation( data, scope, sym_name );
    }
    return( sym_name );
}

static boolean allFunctionNames( lookup_walk *data )
{
    PATH_CAP *cap;
    PATH_CAP *next;
    FNOV_LIST *root;
    SYMBOL_NAME sym_name;
    SYMBOL sym;
    SYMBOL *pinfo;
    auto SYMBOL info[2];

    root = NULL;
    pinfo = &info[1];
    DbgStmt( info[0] = (SYMBOL)-1 );
    DbgStmt( info[1] = (SYMBOL)-1 );
    for( cap = data->paths; cap != NULL; cap = next ) {
        DbgAssert( &info[0] <= pinfo && pinfo < &info[2] );
        next = cap->next;
        sym_name = cap->sym_name;
        sym = sym_name->name_syms;
        if( sym == NULL ) {
            DbgAssert( sym_name->name_type != NULL );
            *pinfo = sym_name->name_type;
            if( pinfo == &info[0] ) {
                break;
            }
            --pinfo;
        } else {
            if( ! SymIsFunction( sym ) ) {
                *pinfo = sym;
                if( pinfo == &info[0] ) {
                    break;
                }
                --pinfo;
            } else {
                info[0] = sym;
            }
        }
    }
    if( pinfo != &info[1] ) {
        DbgAssert( info[0] != (SYMBOL)-1 );
        DbgAssert( info[1] != (SYMBOL)-1 );
        data->info1 = info[0];
        data->info2 = info[1];
        return( FALSE );
    }
    return( TRUE );
}

static boolean removeDuplicateNS( lookup_walk *data )
{
    PATH_CAP *dead;
    PATH_CAP *cap;
    PATH_CAP *next;
    SYMBOL_NAME sym_name;
    SCOPE scope;

    for( cap = data->paths; cap != NULL; cap = next ) {
        next = cap->next;
        sym_name = cap->sym_name;
        scope = sym_name->containing;
        scope->s.colour = FALSE;
    }
    dead = NULL;
    for( cap = data->paths; cap != NULL; cap = next ) {
        next = cap->next;
        sym_name = cap->sym_name;
        scope = sym_name->containing;
        if( scope->s.colour ) {
            cap->throw_away = TRUE;
            dead = cap;
        } else {
            scope->s.colour = TRUE;
        }
    }
    if( dead != NULL ) {
        removeDead( data );
        return( TRUE );
    }
    return( FALSE );
}

static boolean processNSLookup( lookup_walk *data )
{
    PATH_CAP *cap;
    SYMBOL sym;
    SYMBOL aliasee;
    unsigned path_count;
    boolean dead;

    path_count = data->path_count;
    if( path_count <= 1 ) {
        return( path_count != 0 );
    }
    if( removeDuplicateNS( data ) ) {
        path_count = data->path_count;
        if( path_count <= 1 ) {
            return( path_count != 0 );
        }
    }

    // remove duplicate aliases for the same entity
    aliasee = NULL;
    dead = FALSE;
    for( cap = data->paths; cap != NULL; cap = cap->next ) {
        sym = cap->sym_name->name_syms;
        if( sym != NULL ) {
            sym = SymDeAlias( sym );
            if( aliasee == NULL ) {
                aliasee = sym;
            } else if( aliasee == sym ) {
                cap->throw_away = TRUE;
                dead = TRUE;
            }
        }
    }
    if( dead ) {
        removeDead( data );
        path_count = data->path_count;
        if( path_count <= 1 ) {
            return( path_count != 0 );
        }
    }

    if( ! allFunctionNames( data ) ) {
        data->lookup_error = TRUE;
        data->error_msg = ERR_AMBIGUOUS_NAMESPACE_LOOKUP;
        data->info_msg = INF_AMBIGUOUS_NAMESPACE_SYM;
        reduceToOnePath( data );
    }
    return( TRUE );
}

static boolean tryDisambigLookup( lookup_walk *data, SCOPE scope,
                                  PSTK_CTL *from_stack, PSTK_CTL *to_stack,
                                  PSTK_CTL *cycle )
{
    USING_NS *curr;
    SCOPE edge_scope;
    SCOPE *top;

    scope = scope;
    DbgAssert( PstkTopElement( to_stack ) == NULL );
    PstkPopAll( to_stack );
    for( ; (top = (SCOPE *)PstkPop( from_stack )) != NULL; ) {
        RingIterBeg( (*top)->using_list, curr ) {
            if( curr->trigger != NULL ) {
                edge_scope = curr->using_scope;
                if( ! PstkContainsElement( cycle, edge_scope ) ) {
                    PstkPush( cycle, edge_scope );
                    if( doRecordedLookup( data, edge_scope ) == NULL ) {
                        PstkPush( to_stack, edge_scope );
                    }
                }
            }
        } RingIterEnd( curr )
    }
    return( processNSLookup( data ) );
}

static boolean disambigNSLookup( lookup_walk *data, SCOPE scope )
{
    boolean retn;
    SYMBOL_NAME sym_name;
    PSTK_CTL *curr;
    PSTK_CTL *next;
    auto PSTK_CTL stack1;
    auto PSTK_CTL stack2;
    auto PSTK_CTL cycle;

    sym_name = doRecordedLookup( data, scope );
    if( sym_name != NULL ) {
        return( TRUE );
    }
    PstkOpen( &stack1 );
    PstkOpen( &stack2 );
    PstkOpen( &cycle );
    curr = &stack1;
    next = &stack2;
    PstkPush( curr, scope );
    PstkPush( &cycle, scope );
    for(;;) {
        retn = tryDisambigLookup( data, scope, curr, next, &cycle );
        if( retn != FALSE || PstkTopElement( next ) == NULL ) {
            break;
        }
        retn = tryDisambigLookup( data, scope, next, curr, &cycle );
        if( retn != FALSE || PstkTopElement( curr ) == NULL ) {
            break;
        }
    }
    PstkClose( &stack1 );
    PstkClose( &stack2 );
    PstkClose( &cycle );
    return( retn );
}

static boolean simpleNSLookup( lookup_walk *data, SCOPE scope )
{
    boolean retn;
    SCOPE trigger_scope;
    SCOPE top_scope;
    SCOPE edge_scope;
    SCOPE *top;
    USING_NS *curr;
    auto PSTK_CTL cycle;
    auto PSTK_CTL stack;

    PstkOpen( &stack );
    PstkPush( &stack, scope );
    PstkOpen( &cycle );
    PstkPush( &cycle, scope );
    doRecordedLookup( data, scope );
    edge_scope = NULL;
    RingIterBeg( scope->using_list, curr ) {
        if( data->member_lookup ?
            ( curr->trigger != NULL ) : (curr->trigger == NULL ) ) {
            edge_scope = curr->using_scope;
            if( ! PstkContainsElement( &cycle, edge_scope ) ) {
                PstkPush( &cycle, edge_scope );
                PstkPush( &stack, edge_scope );
                doRecordedLookup( data, edge_scope );
            }
        }
    } RingIterEnd( curr )
    if( edge_scope != NULL ) {
        // sym was found in this scope or this scope has triggers
        for( ; (top = (SCOPE *)PstkPop( &stack )) != NULL; ) {
            top_scope = *top;
            RingIterBeg( top_scope->using_list, curr ) {
                trigger_scope = curr->trigger;
                if( data->member_lookup ? 
                    ( trigger_scope != NULL ) :
                    ( trigger_scope == scope || trigger_scope == top_scope ) ) {
                    edge_scope = curr->using_scope;
                    if( ! PstkContainsElement( &cycle, edge_scope ) ) {
                        PstkPush( &cycle, edge_scope );
                        PstkPush( &stack, edge_scope );
                        doRecordedLookup( data, edge_scope );
                    }
                }
            } RingIterEnd( curr )
        }
    }
    retn = processNSLookup( data );
    PstkClose( &cycle );
    PstkClose( &stack );
    return( retn );
}

static boolean searchScope( lookup_walk *data, SCOPE scope )
{
    SCOPE disambig;

    DbgAssert( data->is_special == NULL || data->check_special );
    ExtraRptIncrementCtr( scopes_searched );
    if(( _ScopeMask( scope->id ) & data->consider_mask ) == 0 ) {
        return( FALSE );
    }
    if( _IsClassScope( scope ) ) {
        DbgAssert( data->disambiguate == NULL || _IsClassScope( data->disambiguate ) );
        return( findMember( data, scope ) );
    }
    disambig = data->disambiguate;
    if( disambig != NULL ) {
        if( _IsClassScope( disambig ) ) {
            if( _IsFileScope( scope ) && ! data->file_class_done ) {
                // reached file scope looking for C::id;
                // shift gears and look for 'id' in C
                data->file_class_done = TRUE;
                return( findMember( data, disambig ) );
            }
            return( FALSE );
        }
        DbgAssert( _IsFileScope( disambig ) );
        DbgAssert( _IsFileScope( scope ) || _IsBlockScope( scope ) );
        // NYI: N::id searching
        if( data->file_ns_done ) {
            return( FALSE );
        }
        data->file_ns_done = TRUE;
        scope = disambig;
        return( disambigNSLookup( data, disambig ) );
    }
    DbgAssert( ! data->user_conversion );
    return( simpleNSLookup( data, scope ) );
}

static void lexicalLookup( lookup_walk *data, SCOPE curr )
{
    setConsiderMask( data, _ScopeMask( SCOPE_FILE ) );
    for( ; curr != NULL; curr = curr->enclosing ) {
        if( _IsClassScope( curr ) ) {
            if( data->use_this || data->no_this || data->saw_class ) {
                data->use_this = FALSE;
                data->no_this = TRUE;
            } else {
                data->use_this = TRUE;
            }
            data->saw_class = TRUE;
        } else {
            data->use_this = FALSE;
            data->no_this = FALSE;
        }
        if( searchScope( data, curr ) )
            break;
        if( _IsFunctionScope( curr ) ) {
            data->saw_function = TRUE;
        }
    }
}

BASE_CLASS *ScopeFindVBase( SCOPE scope, TYPE vbase )
/***************************************************/
{
    BASE_CLASS *curr;

    RingIterBeg( ScopeInherits( scope ), curr ) {
        if( curr->type == vbase && _IsVirtualBase( curr ) ) {
            return( curr );
        }
    } RingIterEnd( curr )
    return( NULL );
}

static void addClassPath( lookup_walk *data, SEARCH_RESULT *result )
{
    PATH_CAP *cap;
    BASE_CLASS *base;
    BASE_CLASS *vbase;
    BASE_PATH *path;
    BASE_PATH *head;
    BASE_PATH *tail;
    CLASSINFO *info;
    target_offset_t delta;
    TYPE last_virtual_base;
    SCOPE class_scope;

    cap = data->paths;
    head = cap->head;
    tail = cap->tail;
    delta = 0;
    last_virtual_base = NULL;
    for( path = head; path != NULL; path = path->next ) {
        base = path->base;
        if( base == NULL )
            continue;
        if( _IsDirectVirtualBase( base ) ) {
            last_virtual_base = base->type;
            delta = 0;
        } else {
            delta += base->delta;
        }
    }
    result->delta = delta;
    if( last_virtual_base != NULL ) {
        class_scope = head->scope;
        info = ScopeClass( class_scope )->u.c.info;
        vbase = ScopeFindVBase( class_scope, last_virtual_base );
        result->vb_offset = info->vb_offset;
        result->vb_index = vbase->vb_index;
        delta += vbase->delta;
    } else {
        result->non_virtual = TRUE;
    }
    result->exact_delta = delta;
    result->vf_offset = ScopeClass( tail->scope )->u.c.info->vf_offset;
}

static void makeClassResult( lookup_walk *data, SEARCH_RESULT *result )
{
    PATH_CAP *cap;
    SYMBOL sym;

    if( data->ambiguous ) {
        result->ambiguous = TRUE;
    }
    if( data->overload_reqd ) {
        result->mixed_static = TRUE;
    }
    if( data->use_this ) {
        result->use_this = TRUE;
    }
    if( data->no_this ) {
        result->no_this = TRUE;
    }
    if( data->protected_OK ) {
        result->protected_OK = TRUE;
    }
    cap = data->paths;
    if( data->user_conversion ) {
        result->sym = cap->sym;
    }
    result->start = data->start;
    result->perm = cap->flag;
    result->access_decl = cap->access_decl;
    addClassPath( data, result );
    /* add member offset to result */
    sym = result->sym_name->name_syms;
    if( sym != NULL ) {
        if( sym->id == SC_MEMBER ) {
            result->offset = sym->u.member_offset;
        }
    }
}

static SYM_REGION *newSYM_REGION( SYMBOL from, SYMBOL to )
{
    SYM_REGION *region;

    region = CarveAlloc( carveSYM_REGION );
    region->from = from;
    region->to = to;
    return( region );
}

static void makeNameSpaceResult( lookup_walk *data, SEARCH_RESULT *result )
{
    PATH_CAP *cap;
    PATH_CAP *next;
    SYMBOL_NAME sym_name;
    SYMBOL name_syms;
    SYM_REGION *region;
    SYM_REGION **h;

    h = &(result->region);
    for( cap = data->paths; cap != NULL; cap = next ) {
        next = cap->next;
        sym_name = cap->sym_name;
        name_syms = sym_name->name_syms;
        DbgAssert( name_syms != NULL );
        region = newSYM_REGION( RingFirst( name_syms ), RingLast( name_syms ) );
        RingAppend( h, region );
    }
}

static SEARCH_RESULT *makeResult( lookup_walk *data )
{
    PATH_CAP *cap;
    SEARCH_RESULT *result;
    SCOPE scope;

    cap = data->paths;
    if( cap == NULL ) {
        return( NULL );
    }
    scope = cap->tail->scope;
    result = newResult( scope, cap->sym_name );
    switch( scope->id ) {
    case SCOPE_CLASS:
        makeClassResult( data, result );
        break;
    case SCOPE_FILE:
        result->simple = TRUE;
        if( data->path_count > 1 ) {
            makeNameSpaceResult( data, result );
        }
        break;
    default:
        result->simple = TRUE;
        if( data->saw_function ) {
            result->cant_be_auto = TRUE;
        }
    }
    if( data->lookup_error ) {
        result->lookup_error = TRUE;
        result->error_msg = data->error_msg;
        result->info_msg = data->info_msg;
        result->info1 = data->info1;
        result->info2 = data->info2;
    }
    delLookupData( data );
    return( result );
}

static void initVirtualSearch( lookup_walk *data, SYMBOL sym, NAME name )
{
    TYPE type;
    TYPE fn_type;

    newLookupData( data, name );
    type = sym->sym_type;
    fn_type = FunctionDeclarationType( type );
    if( name == CppConversionName() ) {
        data->type = fn_type->of;
        data->this_qualifier = fn_type->u.f.args->qualifier;
        data->user_conversion = TRUE;
        data->specific_user_conv = TRUE;
    }
    data->fn_type = fn_type;
    data->fn_sym = sym;
    data->virtual_override = TRUE;
    data->ignore_access = TRUE;
    data->only_inherit = TRUE;
    data->consider_mask = _ScopeMask( SCOPE_CLASS );
}

static void assignLocation( CLASS_TABLE *to, CLASS_TABLE *from )
{
    to->vb_offset = from->vb_offset;
    to->vb_index = from->vb_index;
    to->delta = from->delta;
    to->exact_delta = from->exact_delta;
}

static walk_status collectVBTable( BASE_STACK *top, void *parm )
{
    vbtable_walk *data = parm;
    SCOPE scope;
    TYPE class_type;
    CLASSINFO *info;
    CLASS_VBTABLE *table;
    CLASS_VBTABLE *curr;
    unsigned max_vbase;
    target_offset_t delta;
    BASE_CLASS *base;
    unsigned amount;
    auto CLASS_TABLE location;

    scope = top->scope;
    class_type = ScopeClass( scope );
    info = class_type->u.c.info;
    max_vbase = info->last_vbase;
    if( max_vbase == 0 ) {
        /* no virtual bases in this subgraph */
        return( WALK_ABANDON );
    }
    location.delta = info->vb_offset;
    findPtrOffset( top, data->start, &location );
    delta = location.exact_delta;
    RingIterBeg( data->tables, curr ) {
        if( curr->h.exact_delta == delta ) {
            return( WALK_NORMAL );
        }
    } RingIterEnd( curr )
    amount = sizeof( CLASS_VBTABLE ) + max_vbase * sizeof( TYPE );
    table = RingAlloc( &(data->tables), amount );
    assignLocation( &(table->h), &location );
    table->h.count = max_vbase;
    table->data[ max_vbase ] = NULL;
    if( ! data->already_done ) {
        RingIterBeg( ScopeInherits( scope ), base ) {
            if( _IsVirtualBase( base ) ) {
                table->data[ base->vb_index - VFUN_BASE ] = base->type;
            }
        } RingIterEnd( base )
    }
    return( WALK_NORMAL );
}

CLASS_VBTABLE *ScopeCollectVBTable( SCOPE scope, scv_control control )
/********************************************************************/
{
    CLASSINFO *info;
    auto vbtable_walk data;

    data.tables = NULL;
    data.start = scope;
    data.already_done = FALSE;
    if(( control & SCV_CTOR ) != 0 ) {
        info = ScopeClass( scope )->u.c.info;
        if( info->vbtable_done ) {
            data.already_done = TRUE;
        }
        info->vbtable_done = TRUE;
    }
    walkDirectBases( scope, collectVBTable, &data );
    /* caller must do a RingFree */
    return( data.tables );
}

static boolean overloadedSym( SYMBOL derived_sym, SYMBOL base_sym )
{
    FNOV_RESULT check;
    NAME derived_name;

    if( derived_sym == base_sym ) {
        return( FALSE );
    }
    derived_name = derived_sym->name->name;
    if( derived_name != base_sym->name->name ) {
        return( FALSE );
    }
    check = AreFunctionsDistinct( &base_sym, derived_sym, derived_name );
    switch( check ) {
    case FNOV_NOT_DISTINCT_RETURN:
    case FNOV_EXACT_MATCH:
        return( TRUE );
    }
    return( FALSE );
}

static boolean scopeInBaseStack( SCOPE scope, BASE_STACK *top )
{
    for( ; top != NULL; top = top->parent ) {
        if( top->scope == scope ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static SYMBOL findOverride( BASE_STACK *top, SYMBOL curr_override, SYMBOL base_sym )
{
    CLASS_VFTABLE *table;
    THUNK_ACTION *thunk;
    SYMBOL sym;

    table = top->hold;
    if( table == NULL ) {
        return( NULL );
    }
    thunk = table->data;
    for(;;) {
        sym = thunk->sym;
        if( sym != NULL && overloadedSym( sym, base_sym ) ) {
            SCOPE sym_scope = SymScope( sym );
            if( scopeInBaseStack( sym_scope, top ) ) {
                if( curr_override != NULL ) {
                    derived_status dominated;
                    dominated = isQuickScopeDerived( SymScope( curr_override ), sym_scope );
                    if( dominated != DERIVED_NO ) {
                        sym = NULL;
                    }
                }
                return( sym );
            }
            break;
        }
        if( thunk->last_entry )
            break;
        ++thunk;
    }
    return( NULL );
}

static void fillInput( THUNK_ACTION *thunk, vftable_walk *data, TYPE ctor_disp )
{
    BASE_STACK *top;
    BASE_CLASS *base;
    BASE_CLASS *vbase;
    BASE_CLASS *ctor_disp_base;
    TYPE virtual_base;
    TYPE host_class;
    target_offset_t delta;
    SCOPE final;
    SCOPE host_scope;

    final = data->final;
    for( top = data->top; top != NULL; top = top->parent ) {
        if( top->scope == final ) {
            break;
        }
    }
    virtual_base = NULL;
    delta = 0;
    for( ; top != NULL; top = top->parent ) {
        base = top->base;
        if( base != NULL ) {
            if( _IsDirectVirtualBase( base ) ) {
                virtual_base = base->type;
                break;
            }
            delta += base->delta;
        }
    }
    host_scope = data->start;
    vbase = NULL;
    if( virtual_base != NULL ) {
        vbase = ScopeFindVBase( host_scope, virtual_base );
    }
    host_class = ScopeClass( host_scope );
    if( ! TypeCtorDispRequired( host_class, ctor_disp ) ) {
        ctor_disp_base = ScopeFindVBase( host_scope, ctor_disp );
        thunk->delta += ctor_disp_base->delta;
        thunk->delta -= delta;
        if( vbase != NULL ) {
            thunk->delta -= vbase->delta;
        }
        return;
    }
    thunk->ctor_disp = TRUE;
    thunk->in.delta = delta;
    if( vbase != NULL ) {
        thunk->input_virtual = TRUE;
        thunk->in.vb_offset = host_class->u.c.info->vb_offset;
        thunk->in.vb_index = vbase->vb_index;
    }
}

static void pushDisambig( vftable_walk *data, SYMBOL override_sym )
{
    TYPE class_type;
    TYPE ret_type;
    TYPE fn_type;
    SCOPE scope;
    SCOPE *pscope;

    fn_type = FunctionDeclarationType( override_sym->sym_type );
    ret_type = TypePointedAtModified( fn_type->of );
    scope = NULL;
    if( ret_type != NULL ) {
        class_type = StructType( ret_type );
        if( class_type != NULL ) {
            scope = class_type->u.c.scope;
        }
    }
    pscope = VstkPush( &(data->disambig) );
    *pscope = scope;
}

static boolean nonTrivialReturnConversions( VSTK_CTL *vstk, SCOPE *derived )
{
    SCOPE *pscope;
    SCOPE scope;
    SCOPE last_scope;
    SCOPE change_occurred;
    int i;
    int n;

    last_scope = NULL;
    change_occurred = NULL;
    n = VstkDimension( vstk );
    for( i = 0; i <= n; ++i ) {
        pscope = VstkIndex( vstk, i );
        scope = *pscope;
        if( scope == NULL ) {
            return( FALSE );
        }
        if( scope != last_scope ) {
            /* last_scope will be NULL the first time through */
            /* i.e., no change has occurred */
            change_occurred = last_scope;
        }
        last_scope = scope;
    }
    *derived = last_scope;
    return( change_occurred != NULL );
}

static walk_status doReturnCast( BASE_STACK *top, void *parm )
{
    vftable_walk *data = parm;
    SCOPE scope;
    SCOPE *pscope;
    BASE_CLASS *base;
    THUNK_ACTION *thunk;
    TYPE vbase;
    target_offset_t delta;
    VSTK_CTL *vstk;
    BASE_STACK *curr;
    BASE_STACK *parent;

    scope = top->scope;
    if( scope == data->base ) {
        /* must find data->parent in derivation path */
        parent = data->parent;
        if( parent != NULL ) {
            curr = top->parent;
            for(;;) {
                if( curr == NULL ) {
                    return( WALK_NORMAL );
                }
                if( curr == parent )
                    break;
                curr = curr->parent;
            }
        }
        for( vstk = &(data->disambig); (pscope = VstkTop( vstk )) != NULL; VstkPop( vstk ) ) {
            if( scope != *pscope ) {
                /* now find the next accessible base */
                data->base = *pscope;
                data->parent = top;
                return( WALK_NORMAL );
            }
        }
        /* stack is exhausted so we've found a good path */
        vbase = NULL;
        delta = 0;
        for( ; top != NULL; top = top->parent ) {
            base = top->base;
            if( base == NULL )
                break;
            if( _IsVirtualBase( base ) ) {
                vbase = base->type;
                break;
            }
            delta += base->delta;
        }
        thunk = data->thunk;
        if( delta != 0 ) {
            thunk->out.delta = delta;
        }
        if( vbase != NULL ) {
            thunk->output_virtual = TRUE;
            scope = data->derived;
            thunk->out.vb_offset = ScopeClass( scope )->u.c.info->vb_offset;
            base = ScopeFindVBase( scope, vbase );
            thunk->out.vb_index = base->vb_index;
        }
        return( WALK_FINISH );
    }
    return( WALK_NORMAL );
}

static void clearVstk( VSTK_CTL *vstk )
{
    SCOPE *pscope;

    for(;;) {
        pscope = VstkPop( vstk );
        if( pscope == NULL ) {
            break;
        }
    }
}

static void fillOutput( THUNK_ACTION *thunk, vftable_walk *data )
{
    SCOPE derived;
    SCOPE *pscope;
    VSTK_CTL *vstk;

    vstk = &(data->disambig);
    if( nonTrivialReturnConversions( vstk, &derived ) ) {
        data->thunk = thunk;
        pscope = VstkTop( vstk );
        data->base = *pscope;
        data->derived = derived;
        data->parent = NULL;
        walkDirectBases( derived, doReturnCast, data );
    }
    clearVstk( vstk );
}

static void analyseVFN( THUNK_ACTION *thunk, vftable_walk *data )
{
    SYMBOL base_sym;
    SYMBOL test_sym;
    SYMBOL last_override_sym;
    BASE_STACK *first_top;
    BASE_STACK *top;
    BASE_CLASS *base;
    TYPE virtual_base;
    TYPE virtual_leap;
    TYPE fn_type;
    target_offset_t delta;
    target_offset_t prev_delta;
    target_offset_t leap_delta;

    base_sym = thunk->sym;
    virtual_base = NULL;
    virtual_leap = NULL;
    last_override_sym = NULL;
    delta = 0;
    leap_delta = 0;
    prev_delta = 0;
    first_top = data->top;
    for( top = first_top; top != NULL; top = top->parent ) {
        if( virtual_base == NULL ) {
            delta += prev_delta;
            base = top->base;
            if( base != NULL ) {
                if( _IsDirectVirtualBase( base ) ) {
                    virtual_base = base->type;
                    leap_delta = delta;
                    prev_delta = 0;
                } else {
                    prev_delta = base->delta;
                }
            }
        }
        if( top != first_top ) {
            test_sym = findOverride( top, thunk->override, base_sym );
            if( test_sym != NULL ) {
                last_override_sym = test_sym;
                if( thunk->override == NULL ) {
                    pushDisambig( data, base_sym );
                }
                pushDisambig( data, test_sym );
                if( virtual_base != NULL ) {
                    virtual_leap = virtual_base;
                    /* this entry may have an ambiguity ... */
                    thunk->possible_ambiguity = TRUE;
                    /* ... so this table should be checked */
                    data->curr->ambiguities = TRUE;
                }
                thunk->override = test_sym;
                thunk->delta = delta;
            }
        }
    }
    if( last_override_sym == NULL ) {
        /* no override found in this path */
        return;
    }
    if( thunk->override != NULL ) {
        data->final = SymScope( thunk->override );
        /* check for complicated input adjustment (i.e., more than a delta) */
        if( data->thunk_code ) {
            if( virtual_leap != NULL ) {
                thunk->delta = leap_delta;
                fillInput( thunk, data, virtual_leap );
            }
            if( thunk->delta != 0 || thunk->ctor_disp ) {
                thunk->non_empty = TRUE;
            }
            /* check for any output (return value) adjustment */
            fillOutput( thunk, data );
            if( thunk->out.delta != 0 || thunk->output_virtual ) {
                thunk->non_empty = TRUE;
            }
        }
    } else {
        fn_type = FunctionDeclarationType( thunk->sym->sym_type );
        if( fn_type->flag & TF1_PURE ) {
            /* we have a pure function with no override */
            thunk->non_empty = TRUE;
        }
    }
}


static void checkAmbiguousOverride( THUNK_ACTION *thunk, vftable_walk *data )
{
    SYMBOL test_sym;
    SYMBOL base_sym;
    SYMBOL override_sym;
    SYMBOL test_override_sym;
    SCOPE override_scope;
    SCOPE test_override_scope;
    BASE_STACK *top;
    BASE_STACK *first_top;
    BASE_CLASS *base;
    TYPE virtual_base;
    derived_status dominated;

    override_sym = thunk->override;
    if( override_sym == NULL ) {
        return;
    }
    override_scope = SymScope( override_sym );
    base_sym = thunk->sym;
#ifndef NDEBUG
    if( PragDbgToggle.dump_vftables ) {
        printf( "Searching for: base(" );
        printSymbolName( base_sym );
        printf( ") override(" );
        printSymbolName( override_sym );
        printf( ")\n" );
    }
#endif
    for( top = data->top; top != NULL; top = top->parent ) {
        DbgStmt( if( PragDbgToggle.dump_vftables ) printScopeName( top->scope, NULL ); );
        if( top->scope == override_scope ) {
            break;
        }
    }
    DbgStmt( if( PragDbgToggle.dump_vftables ) putchar( '\n' ); );
    if( top != NULL ) {
        /* quick check to see if override is in this path succeeded! */
        return;
    }
    test_override_sym = NULL;
    virtual_base = NULL;
    first_top = data->top;
    for( top = first_top; top != NULL; top = top->parent ) {
        DbgStmt( if( PragDbgToggle.dump_vftables ) printScopeName( top->scope, "\n" ); );
        if( virtual_base == NULL ) {
            base = top->base;
            if( base != NULL ) {
                if( _IsDirectVirtualBase( base ) ) {
                    virtual_base = base->type;
                }
            }
        }
        if( top != first_top ) {
            if( virtual_base != NULL ) {
                // ambiguities can only occur with overrides
                // crossing virtual base boundaries
                test_sym = findOverride( top, NULL, base_sym );
                if( test_sym != NULL ) {
                    test_override_sym = test_sym;
#ifndef NDEBUG
                    if( PragDbgToggle.dump_vftables ) {
                        printf( "found override(" );
                        printSymbolName( test_override_sym );
                        printf( ")\n" );
                    }
#endif
                }
            }
        }
    }
    if( test_override_sym != NULL ) {
        test_override_scope = SymScope( test_override_sym );
        dominated = isQuickScopeDerived( override_scope, test_override_scope );
        if( dominated == DERIVED_NO ) {
            // mark table as corrupted
            data->curr->corrupted = TRUE;
            if( data->OK_to_diagnose ) {
                // 'base_sym' can be overridden in (at least) two different
                // ways, namely, 'override_sym' and 'test_override_sym'
                CErr( ERR_VFTABLE_CONSTRUCTION_AMBIGUITY, base_sym );
                InfMsgPtr( INF_VFTABLE_OVERRIDE, override_sym );
                InfMsgPtr( INF_VFTABLE_OVERRIDE, test_override_sym );
            }
        }
        // prevent the message from the other path
        thunk->possible_ambiguity = FALSE;
    }
}

static void handleVFN( vftable_walk *data, SYMBOL sym )
{
    SYMBOL introducing_sym;
    unsigned vf_index;
    CLASS_VFTABLE *table;
    THUNK_ACTION *thunk;
    THUNK_ACTION *thunk_table;

    table = data->curr;
    thunk_table = table->data;
    vf_index = sym->u.member_vf_index - VFUN_BASE;
    thunk = &thunk_table[ vf_index ];
    introducing_sym = thunk->sym;
    if( introducing_sym == NULL ) {
        thunk->sym = sym;
        table->amt_left--;
    } else if( thunk->override == NULL ) {
        /* should still check for an override */
    } else {
        if( thunk->possible_ambiguity ) {
            checkAmbiguousOverride( thunk, data );
        } else {
            sym = NULL;
        }
    }
    if( sym != NULL ) {
        dumpVFN( sym, data, table, vf_index );
        analyseVFN( thunk, data );
        dumpThunk( thunk );
    }
}

static void scanForVFNs( SYMBOL_NAME sym_name, void *_data )
{
    vftable_walk *data = _data;
    TYPE fn_type;
    SYMBOL sym;

    RingIterBeg( sym_name->name_syms, sym ) {
        fn_type = FunctionDeclarationType( sym->sym_type );
        if( fn_type == NULL ) break;
        if(( fn_type->flag & TF1_VIRTUAL ) == 0 ) {
            /* skip non-virtual functions */
            continue;
        }
        if( sym->id == SC_DEFAULT ) {
            /* don't involve default args */
            continue;
        }
        if( sym->u.member_vf_index == 0 ) {
            /* virtual function was never allocated an index (error) */
            continue;
        }
        handleVFN( data, sym );
    } RingIterEnd( sym )
}

static void doubleCheckEntries( vftable_walk *data, CLASS_VFTABLE *table )
{
    THUNK_ACTION *thunk;
    SYMBOL sym;

    thunk = table->data;
    for(;;) {
        sym = thunk->sym;
        if( sym != NULL && thunk->possible_ambiguity ) {
            handleVFN( data, sym );
        }
        if( thunk->last_entry )
            break;
        ++thunk;
    }
}

static void initThunk( THUNK_ACTION *init )
{
    init->sym = NULL;
    init->thunk = NULL;
    init->delta = 0;
    init->in.delta = 0;
    init->override = NULL;
    init->out.delta = 0;
    init->ctor_disp = FALSE;
    init->input_virtual = FALSE;
    init->output_virtual = FALSE;
    init->non_empty = FALSE;
    init->last_entry = FALSE;
    init->possible_ambiguity = FALSE;
}

static walk_status collectVFTable( BASE_STACK *top, void *parm )
{
    vftable_walk *data = parm;
    SCOPE scope;
    TYPE class_type;
    CLASSINFO *info;
    CLASS_VFTABLE *curr;
    CLASS_VFTABLE *table;
    unsigned max_vfn;
    target_offset_t delta;
    THUNK_ACTION *init;
    unsigned i;
    unsigned amount;
    auto CLASS_TABLE location;

    data->top = top;
    scope = top->scope;
    DbgStmt( if( PragDbgToggle.dump_vftables ) printScopeName( scope, "collectVFTable()\n" ); );
    class_type = ScopeClass( scope );
    info = class_type->u.c.info;
    if( ! info->has_vfn ) {
        /* no virtual functions in this subgraph */
        return( WALK_ABANDON );
    }
    if( ! info->has_vfptr ) {
        /* no virtual function pointer in this class */
        return( WALK_NORMAL );
    }
    location.delta = info->vf_offset;
    findPtrOffset( top, data->start, &location );
    delta = location.exact_delta;
    RingIterBeg( data->tables, curr ) {
        if( curr->h.exact_delta == delta ) {
            if( ! data->already_done ) {
                top->hold = curr;
                data->curr = curr;
                if( curr->amt_left != 0 ) {
                    HashWalkData( scope->names, &scanForVFNs, data );
                } else {
                    HashWalkData( scope->names, &scanForVFNs, data );
                    if( curr->ambiguities ) {
                        doubleCheckEntries( data, curr );
                    }
                }
            }
            return( WALK_NORMAL );
        }
    } RingIterEnd( curr )
    max_vfn = info->last_vfn;
    amount = sizeof( CLASS_VFTABLE ) + ( max_vfn - 1 ) * sizeof( THUNK_ACTION );
    table = RingAlloc( &(data->tables), amount );
    assignLocation( &(table->h), &location );
    if( location.ctor_disp ) {
        table->h.ctor_disp = TRUE;
    } else {
        table->h.ctor_disp = FALSE;
    }
    table->h.count = max_vfn;
    table->amt_left = max_vfn;
    table->ambiguities = FALSE;
    table->corrupted = FALSE;
    if( ! data->already_done ) {
        init = table->data;
        for( i = 0; i < max_vfn; ++i ) {
            initThunk( init );
            ++init;
        }
        --init;
        init->last_entry = TRUE;
        top->hold = table;
        data->curr = table;
        HashWalkData( scope->names, &scanForVFNs, data );
    }
    return( WALK_NORMAL );
}

CLASS_VFTABLE *ScopeCollectVFTable( SCOPE scope, scv_control control )
/********************************************************************/
{
    CLASSINFO *info;
    auto vftable_walk data;

    data.tables = NULL;
    data.start = scope;
    data.final = NULL;
    data.curr = NULL;
    data.top = NULL;
    data.already_done = FALSE;
    data.OK_to_diagnose = FALSE;
    data.thunk_code = FALSE;
    if( control & SCV_CTOR ) {
        info = ScopeClass( scope )->u.c.info;
        if( info->vftable_done ) {
            data.already_done = TRUE;
        } else {
            data.thunk_code = TRUE;
        }
        info->vftable_done = TRUE;
    }
    if(( control & SCV_NO_DIAGNOSE ) == 0 ) {
        data.OK_to_diagnose = TRUE;
    }
    VstkOpen( &data.disambig, sizeof( SCOPE ), 8 );
    DbgStmt( if( PragDbgToggle.dump_vftables ) printScopeName( scope, "collecting virtual function table\n" ); );
    walkDirectBases( scope, collectVFTable, &data );
    VstkClose( &data.disambig );
    /* caller must do a RingFree */
    return( data.tables );
}

boolean ScopeHasPureFunctions( SCOPE scope )
/******************************************/
{
    CLASS_VFTABLE *tables;
    CLASS_VFTABLE *table;
    THUNK_ACTION *thunk;
    boolean has_a_pure;

    DbgVerify( TypeDefined( ScopeClass( scope ) )
             , "undefined type passed to ScopeHasPureFunctions" );
    // NYI: could set doCollectVFTable to not worry about thunk contents
    tables = ScopeCollectVFTable( scope, SCV_NO_DIAGNOSE );
    has_a_pure = FALSE;
    RingIterBeg( tables, table ) {
        if( table->corrupted ) {
            continue;
        }
        thunk = table->data;
        for(;;) {
            if( ScopePureVirtualThunk( thunk ) != NULL ) {
                has_a_pure = TRUE;
                break;
            }
            if( thunk->last_entry )
                break;
            ++thunk;
        }
        if( has_a_pure ) {
            break;
        }
    } RingIterEnd( table )
    // NYI: we could cache this ring of tables in case it is needed again
    RingFree( &tables );
    return( has_a_pure );
}

void ScopeNotePureFunctions( TYPE type )
/**************************************/
{
    TYPE class_type;
    CLASSINFO *info;
    SCOPE scope;
    CLASS_VFTABLE *tables;
    CLASS_VFTABLE *table;
    THUNK_ACTION *thunk;
    SYMBOL pure_fn;

    if( ! TypeDefined( type ) ) {
        // things are not setup yet
        return;
    }
    class_type = StructType( type );
    info = class_type->u.c.info;
    scope = class_type->u.c.scope;
    // NYI: could set doCollectVFTable to not worry about thunk contents
    tables = ScopeCollectVFTable( scope, SCV_NO_DIAGNOSE );
    if( info->has_pure ) {
        InfMsgPtr( INF_CLASS_CONTAINS_PURE, type );
    } else {
        InfMsgPtr( INF_CLASS_DIDNT_DEFINE_PURE, type );
    }
    RingIterBeg( tables, table ) {
        if( table->corrupted ) {
            continue;
        }
        thunk = table->data;
        for(;;) {
            pure_fn = ScopePureVirtualThunk( thunk );
            if( pure_fn != NULL ) {
                CErr( INF_PURE_FUNCTION, pure_fn, &pure_fn->locn->tl );
            }
            if( thunk->last_entry )
                break;
            ++thunk;
        }
    } RingIterEnd( table )
    // NYI: we could cache this ring of tables in case it is needed again
    RingFree( &tables );
}

SYMBOL ScopePureVirtualThunk( THUNK_ACTION *thunk )
/*************************************************/
{
    SYMBOL final_sym;
    TYPE fn_type;

    final_sym = thunk->sym;
    if( thunk->override != NULL ) {
        final_sym = thunk->override;
    }
    if( final_sym != NULL ) {
        fn_type = FunctionDeclarationType( final_sym->sym_type );
        if( fn_type != NULL && ( fn_type->flag & TF1_PURE ) != TF1_NULL ) {
            return( final_sym );
        }
    }
    return( NULL );
}

static boolean isIdentityMapping( SCOPE from, SCOPE to, target_offset_t *except)
{
    TYPE find_base;
    TYPE from_type;
    TYPE to_type;
    CLASSINFO *from_info;
    CLASSINFO *to_info;
    BASE_CLASS *from_base;
    BASE_CLASS *to_base;
    target_offset_t from_last;
    target_offset_t to_last;
    target_offset_t from_index;
    target_offset_t to_index;
    target_offset_t from_done;

    except[0] = 0;
    from_type = ScopeClass( from );
    from_info = from_type->u.c.info;
    to_type = ScopeClass( to );
    to_info = to_type->u.c.info;
    from_last = from_info->last_vbase;
    to_last = to_info->last_vbase;
    from_done = from_last;
    if( from_last > to_last ) {
        /* special case: should generate:
            if( new_index > to_last )
                new_index = 0;
        */
        from_done = to_last;
        except[0] = to_last;
        except[1] = 0;
    }
    RingIterBeg( from_info->bases, from_base ) {
        if( _IsVirtualBase( from_base ) ) {
            from_index = from_base->vb_index;
            find_base = from_base->type;
            RingIterBeg( to_info->bases, to_base ) {
                if( _IsVirtualBase( to_base ) ) {
                    if( to_base->type == find_base ) {
                        --from_done;
                        to_index = to_base->vb_index;
                        if( to_index != from_index ) {
                            if( except[0] != 0 ) {
                                /* too many exceptions (only one allowed) */
                                except[0] = 0;
                                return( FALSE );
                            }
                            except[0] = from_index;
                            except[1] = to_index;
                        }
                        break;          /* we found our matching vbase */
                    }
                } else {
                    break;              /* no more vbases */
                }
            } RingIterEnd( to_base )
        } else {
            break;                      /* no more vbases */
        }
    } RingIterEnd( from_base )
    if( from_done != 0 ) {
        /* somehow we didn't find all the bases so no optimizations */
        except[0] = 0;
    }
    if( except[0] != 0 ) {
        return( FALSE );
    }
    return( TRUE );
}

static void emitOffset( target_offset_t offset )
{
    PTREE expr;

    expr = NodeOffset( offset );
    DgStoreScalar( expr, 0, expr->type );
    PTreeFreeSubtrees( expr );
}

static void emitIndexMapping( SAVE_MAPPING *mapping )
{
    TYPE find_base;
    TYPE from_type;
    TYPE to_type;
    CLASSINFO *from_info;
    CLASSINFO *to_info;
    BASE_CLASS *from_base;
    BASE_CLASS *to_base;
    target_offset_t from_index;
    target_offset_t to_index;
    SYMBOL sym;

    sym = mapping->sym;
    DgSymbolLabel( sym );
    sym->flag |= SF_INITIALIZED;
    emitOffset( mapping->map_0 );
    from_type = ScopeClass( mapping->from );
    from_info = from_type->u.c.info;
    to_type = ScopeClass( mapping->to );
    to_info = to_type->u.c.info;
    for( from_index = 1; from_index <= from_info->last_vbase; ++from_index ) {
        RingIterBeg( from_info->bases, from_base ) {
            if( ! _IsVirtualBase( from_base ) )
                break;
            if( from_index == from_base->vb_index ) {
                to_index = 0;
                find_base = from_base->type;
                RingIterBeg( to_info->bases, to_base ) {
                    if( ! _IsVirtualBase( to_base ) )
                        break;
                    if( to_base->type == find_base ) {
                        to_index = to_base->vb_index;
                        break;  /* we found our matching vbase */
                    }
                } RingIterEnd( to_base )
                emitOffset( to_index * TARGET_UINT );
                break;
            }
        } RingIterEnd( from_base )
    }
    CgSegId( sym );
}

void ScopeEmitIndexMappings( void )
/*********************************/
{
    SAVE_MAPPING *m;

    while( mappingList != NULL ) {
        m = mappingList;
        mappingList = m->next;
        emitIndexMapping( m );
        CMemFree( m );
    }
}

static SYMBOL genIndexMapping( target_offset_t map_0, SCOPE from, SCOPE to )
{
    SAVE_MAPPING *mapping;
    boolean new_sym;
    SYMBOL sym;

    sym = MakeVMTableSym( from, to, &new_sym );
    if( new_sym ) {
        mapping = CMemAlloc( sizeof( SAVE_MAPPING ) );
        mapping->sym = sym;
        mapping->map_0 = map_0;
        mapping->from = from;
        mapping->to = to;
        mapping->next = mappingList;
        mappingList = mapping;
    }
    return( sym );
}

static walk_status findMembPtrCast( BASE_STACK *top, void *parm )
{
    MEMBER_PTR_CAST *data = parm;
    BASE_CLASS *base;
    BASE_CLASS *vbase;
    SCOPE base_scope;
    SCOPE derived_scope;
    SCOPE from_scope;
    SCOPE to_scope;
    target_offset_t delta;
    target_offset_t map_0;
    target_offset_t except[2];
    boolean base_has_vbases;
    boolean derived_has_vbases;
    boolean identity_mapping;

    base_scope = data->base;
    if( top->scope != base_scope ) {
        return( WALK_NORMAL );
    }
    base = NULL;
    delta = 0;
    for( ; top != NULL; top = top->parent ) {
        base = top->base;
        if( base != NULL ) {
            if( _IsDirectVirtualBase( base ) ) {
                break;
            }
            delta += base->delta;
        }
    }
    if( delta != 0 ) {
        data->delta = delta;
        data->delta_reqd = TRUE;
    }
    derived_scope = data->derived;
    vbase = NULL;
    if( base != NULL ) {
        vbase = ScopeFindVBase( derived_scope, base->type );
    }
    base_has_vbases = ScopeHasVirtualBases( base_scope );
    if( data->safe ) {
        if( data->init_conv || ! base_has_vbases ) {
            /* no virtual bases at the base class */
            if( vbase == NULL ) {
                /* no virtual bases introduced */
                if( data->delta_reqd ) {
                    /* some code is being generated so we can save a copy */
                    data->vb_index = 0;
                    data->mapping_reqd = TRUE;
                }
            } else {
                /* a virtual base was introduced by the cast */
                data->vb_index = vbase->vb_index * TARGET_UINT;
                data->mapping_reqd = TRUE;
            }
            return( WALK_FINISH );
        }
    }
    if( ! data->safe && ! base_has_vbases ) {
        /* unsafe conversion to base without any virtual bases */
        /* action: zap vbase index to 0 and apply any delta req'd (no test) */
        derived_has_vbases = ScopeHasVirtualBases( derived_scope );
        if( data->delta_reqd || derived_has_vbases ) {
            data->vb_index = 0;
            data->mapping_reqd = TRUE;
        }
        return( WALK_FINISH );
    }
    /* cases:
        (1) safe conversion from a base with virtual bases
        (2) unsafe conversion to a base with virtual bases
    */
    if( data->delta_reqd ) {
        data->test_reqd = TRUE;
    }
    map_0 = 0;
    to_scope = base_scope;
    from_scope = data->derived;
    if( data->safe ) {
        to_scope = from_scope;
        from_scope = base_scope;
        if( vbase != NULL ) {
            map_0 = vbase->vb_index;
        }
    }
    identity_mapping = isIdentityMapping( from_scope, to_scope, except );
    if( map_0 == 0 ) {
        if( identity_mapping ) {
            /* the entire mapping is benign */
            return( WALK_FINISH );
        }
        if( except[0] != 0 ) {
            /* almost an identity mapping so we handle the exception */
            data->single_test = except[0] * TARGET_UINT;
            /* if this is 0, any index above 'single_test' maps to 0 */
            data->vb_index = except[1] * TARGET_UINT;
            data->single_mapping = TRUE;
            data->mapping_reqd = TRUE;
            return( WALK_FINISH );
        }
    } else {
        if( identity_mapping ) {
            /* 0 maps to k but all others don't need mapping */
            data->single_test = 0;
            data->vb_index = map_0 * TARGET_UINT;
            data->single_mapping = TRUE;
            data->mapping_reqd = TRUE;
            return( WALK_FINISH );
        }
    }
    data->mapping = genIndexMapping( map_0, from_scope, to_scope );
    data->mapping_reqd = TRUE;
    return( WALK_FINISH );
}

void ScopeMemberPtrCastAction( MEMBER_PTR_CAST *cast_data )
/*********************************************************/
{
    cast_data->delta = 0;
    cast_data->single_test = 0;
    cast_data->vb_index = 0;
    cast_data->mapping = NULL;
    cast_data->delta_reqd = FALSE;
    cast_data->mapping_reqd = FALSE;
    cast_data->test_reqd = FALSE;
    cast_data->single_mapping = FALSE;
    dumpDerivation( cast_data );
    walkDirectBases( cast_data->derived, findMembPtrCast, cast_data );
    dumpData( cast_data );
}

find_virtual_status ScopeFindVirtual( SCOPE scope, SYMBOL sym[2], NAME name )
/***************************************************************************/
{
    find_virtual_status status;
    auto lookup_walk data;

    status = FVS_NULL;
    initVirtualSearch( &data, sym[0], name );
    data.ok_to_diagnose = TRUE;
    sym[0] = NULL;
    sym[1] = NULL;
    if( searchScope( &data, scope ) ) {
        *sym = data.paths->sym;
        if( data.use_index ) {
            status |= FVS_USE_INDEX;
            if( data.return_thunk ) {
                status |= FVS_RETURN_THUNK;
            }
        }
        delLookupData( &data );
        status |= FVS_VIRTUAL_ABOVE;
    }
    if( data.same_table ) {
        sym[1] = data.vfn_name->name_syms;
        status |= FVS_NAME_SAME_TABLE;
    }
    return( status );
}

static SEARCH_RESULT *addBaseDelta( SEARCH_RESULT *result, SCOPE from, SCOPE to)
{
    auto lookup_walk data;

    DbgAssert( _IsClassScope( from ) && _IsClassScope( to ) );
    newLookupData( &data, NULL );
    data.disambiguate = to;
    if( searchScope( &data, from ) ) {
        if( result == NULL ) {
            result = newResult( to, NULL );
        }
        addClassPath( &data, result );
        delLookupData( &data );
    }
    return( result );
}

SEARCH_RESULT *ScopeBaseResult( SCOPE from, SCOPE to )
/****************************************************/
{
    return( addBaseDelta( NULL, from, to ) );
}

SEARCH_RESULT *ScopeResultFromBase( TYPE class_type, BASE_CLASS *base )
/*********************************************************************/
{
    CLASSINFO *info;
    target_offset_t delta;
    SEARCH_RESULT *result;

    result = newResult( base->type->u.c.scope, NULL );
    delta = base->delta;
    if( _IsVirtualBase( base ) ) {
        info = class_type->u.c.info;
        result->exact_delta = delta;
        result->vb_offset = info->vb_offset;
        result->vb_index = base->vb_index;
        result->delta = 0;
    } else {
        result->non_virtual = TRUE;
        result->delta = delta;
        result->exact_delta = delta;
    }
    return( result );
}

static inherit_flag checkBaseAccess( SCOPE d, SCOPE b, derived_status status )
{
    PATH_CAP *cap;
    inherit_flag perm;
    inherit_flag best_perm;
    auto lookup_walk data;

    DbgAssert( _IsClassScope( d ) && _IsClassScope( b ) );
    newLookupData( &data, NULL );
    data.disambiguate = b;
    if( status == DERIVED_YES_BUT_VIRTUAL ) {
        data.find_all = TRUE;
    }
    if( searchScope( &data, d ) ) {
        for( cap = data.paths; cap != NULL; cap = cap->next ) {
            perm = setAccess( cap );
            cap->flag = perm;
            if( perm == IN_PUBLIC ) {
                break;
            }
        }
        best_perm = IN_PUBLIC;
        if( cap == NULL ) {
            best_perm = IN_PRIVATE;
            for( cap = data.paths; cap != NULL; cap = cap->next ) {
                perm = cap->flag;
                if( perm == IN_PROTECTED ) {
                    best_perm = perm;
                    break;
                }
            }
        }
        delLookupData( &data );
        return( best_perm );
    }
    DbgStmt( CFatal( "checkBaseAccess: could not find base class" ) );
    return( IN_PRIVATE );
}

SEARCH_RESULT *ScopeFindBaseMember( SCOPE scope, NAME name )
/**********************************************************/
{
    SEARCH_RESULT *result;

    /* specialized version for accessing component ctors, dtors, and op='s */
    result = ScopeContainsMember( scope, name );
    if( result != NULL ) {
        result->protected_OK = TRUE;
    }
    return( result );
}

SEARCH_RESULT *ScopeFindSymbol( SYMBOL sym )
/******************************************/
{
    SCOPE scope;
    SEARCH_RESULT *result;

    scope = SymScope( sym );
    result = newResult( scope, sym->name );
    result->ignore_access = TRUE;
    if( SymIsUDC( sym ) ) {
        result->sym = sym;
    } else if( SymIsThisDataMember( sym ) ) {
        result->offset = sym->u.member_offset;
        result->non_virtual = TRUE;
    }
    return( result );
}

SEARCH_RESULT *ScopeFindScopedMember( SCOPE scope, SCOPE disambig, NAME name )
/****************************************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    newLookupData( &data, name );
    data.disambiguate = disambig;
    setConsiderMask( &data, 0 );
    searchScope( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeFindMember( SCOPE scope, NAME name )
/******************************************************/
{
    return( ScopeFindScopedMember( scope, NULL, name ) );
}

SEARCH_RESULT *ScopeFindScopedMemberConversion( SCOPE scope, SCOPE disambig, TYPE t, type_flag this_qualifier )
/*************************************************************************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    /* finds specific user-defined conversion (e.g., ->operator int()) */
    newLookupData( &data, CppConversionName() );
    data.disambiguate = disambig;
    data.type = t;
    data.this_qualifier = this_qualifier;
    data.user_conversion = TRUE;
    data.specific_user_conv = TRUE;
    setConsiderMask( &data, 0 );
    searchScope( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeFindScopedNakedConversion( SCOPE scope, SCOPE disambig, TYPE t, type_flag this_qualifier )
/************************************************************************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    /* finds specific user-defined conversion (e.g., ->operator int()) */
    newLookupData( &data, CppConversionName() );
    data.disambiguate = disambig;
    data.type = t;
    data.this_qualifier = this_qualifier;
    data.user_conversion = TRUE;
    data.specific_user_conv = TRUE;
    lexicalLookup( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeFindNakedConversion( SCOPE scope, TYPE type, type_flag this_qualifier )
/*****************************************************************************************/
{
    return( ScopeFindScopedNakedConversion( scope, NULL, type, this_qualifier));
}

FNOV_LIST *ScopeConversionList( SCOPE scope, type_flag this_qualifier, TYPE type )
/********************************************************************************/
{
    CLASSINFO *info;
    TYPE class_type;
    auto lookup_walk data;

    /* finds list of user-defined conversions to 'type' */
    ExtraRptIncrementCtr( cnv_total );
    class_type = ScopeClass( scope );
    info = class_type->u.c.info;
    if( !info->has_udc ) {
        ExtraRptIncrementCtr( cnv_quick );
        return( NULL );
    }
    newLookupData( &data, CppConversionName() );
    data.type = type;
    data.this_qualifier = this_qualifier;
    data.user_conversion = TRUE;
    data.best_user_conv = TRUE;
    setConsiderMask( &data, 0 );
    if( searchScope( &data, scope ) ) {
        delLookupData( &data );
        ExtraRptIncrementCtr( cnv_found );
    }
    return( data.user_conv_list );
}

SEARCH_RESULT *ScopeContainsMember( SCOPE scope, NAME name )
/**********************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    newLookupData( &data, name );
    data.no_inherit = TRUE;
    searchScope( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeContainsNaked( SCOPE scope, NAME name )
/*********************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    newLookupData( &data, name );
    searchScope( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeFindScopedNaked( SCOPE scope, SCOPE disambig, NAME name )
/***************************************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    newLookupData( &data, name );
    data.disambiguate = disambig;
    lexicalLookup( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeFindNaked( SCOPE scope, NAME name )
/*****************************************************/
{
    return( ScopeFindScopedNaked( scope, NULL, name ) );
}

SEARCH_RESULT *ScopeFindNakedFriend( SCOPE scope, NAME name )
/***********************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    newLookupData( &data, name );
    if( _IsClassScope( scope ) ) {
        data.ignore = scope;
    }
    lexicalLookup( &data, scope );
    result = makeResult( &data );
    return( result );
}

SYMBOL_NAME ScopeYYLexical( SCOPE scope, NAME name )
/**************************************************/
{
    SYMBOL_NAME sym_name;
    auto lookup_walk data;

    newLookupData( &data, name );
    data.ignore_access = TRUE;
    for(;;) {
        if( scope == NULL ) {
            sym_name = NULL;
            break;
        }
        if( searchScope( &data, scope ) ) {
            sym_name = data.paths->sym_name;
            delLookupData( &data );
            break;
        }
        scope = scope->enclosing;
    }
    return( sym_name );
}

SYMBOL_NAME ScopeYYMember( SCOPE scope, NAME name )
/*************************************************/
{
    SYMBOL_NAME sym_name;
    auto lookup_walk data;

    newLookupData( &data, name );
    data.member_lookup = TRUE;
    data.ignore_access = TRUE;
    if( searchScope( &data, scope ) ) {
        sym_name = data.paths->sym_name;
        delLookupData( &data );
        return( sym_name );
    }
    return( NULL );
}

SYMBOL ScopeAlreadyExists( SCOPE scope, NAME name )
/*************************************************/
{
    SYMBOL sym;
    SEARCH_RESULT *result;

    sym = NULL;
    result = ScopeFindNaked( scope, name );
    if( result != NULL ) {
        sym = result->sym_name->name_syms;
        ScopeFreeResult( result );
    }
    return( sym );
}

/*
    ScopeFindLexicalClassType is required for instances where an id must
    be resolved to a class-name.  Cases include "struct id".
    The lookup must search enclosing lexical scopes until it finds a
    type-name corresponding to the id (if one is present).
*/
SEARCH_RESULT *ScopeFindLexicalClassType( SCOPE scope, NAME name )
/****************************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    newLookupData( &data, name );
    data.check_special = TRUE;
    data.is_special = ClassTypeName;
    lexicalLookup( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeFindLexicalEnumType( SCOPE scope, NAME name )
/***************************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    newLookupData( &data, name );
    data.check_special = TRUE;
    data.is_special = EnumTypeName;
    lexicalLookup( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeFindLexicalColonColon( SCOPE scope, NAME name, boolean tilde )
/********************************************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    // 'name' occurs before a '::'
    newLookupData( &data, name );
    data.check_special = TRUE;
    data.is_special = tilde ? colonColonTildeName : colonColonName;
    lexicalLookup( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeFindLexicalNameSpace( SCOPE scope, NAME name )
/****************************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    // 'name' occurs before a '::'
    newLookupData( &data, name );
    data.check_special = TRUE;
    data.is_special = nameSpaceName;
    lexicalLookup( &data, scope );
    result = makeResult( &data );
    return( result );
}

SEARCH_RESULT *ScopeFindMemberColonColon( SCOPE scope, NAME name )
/****************************************************************/
{
    SEARCH_RESULT *result;
    auto lookup_walk data;

    newLookupData( &data, name );
    data.check_special = TRUE;
    data.is_special = colonColonName;
    searchScope( &data, scope );
    result = makeResult( &data );
    return( result );
}

static walk_status findExactOverride( BASE_STACK *top, void *parm )
{
    vfn_opt_walk *data = parm;
    NAME name;
    SYMBOL vfn_dummy;
    SYMBOL vfn_base;
    SYMBOL vfn_override;
    SYMBOL_NAME sym_name;
    SCOPE scope;
    FNOV_RESULT check;
    FNOV_RESULT save_check;
    BASE_STACK *save_top;
    BASE_STACK *curr;
    target_offset_t vfn_thunk_delta;
    auto CLASS_TABLE location;

    scope = top->scope;
    if( scope == data->base ) {
        if( data->found != NULL ) {
            /* found base class again! */
            /* override is too ugly to find for now */
            data->vfn_override = NULL;
            return( WALK_FINISH );
        }
        data->found = scope;
        vfn_base = data->vfn_sym;
        vfn_override = NULL;
        name = vfn_base->name->name;
        for( curr = top->parent; curr != NULL; curr = curr->parent ) {
            sym_name = HashLookup( curr->scope->names, name );
            if( sym_name != NULL && anyVirtualFns( sym_name ) ) {
                vfn_dummy = sym_name->name_syms;
                check = distinctVirtualFunction( &vfn_dummy, vfn_base, name );
                switch( check ) {
                case FNOV_EXACT_MATCH:
                case FNOV_NOT_DISTINCT_RETURN:
                    if( SymIsVirtual( vfn_dummy ) ) {
                        vfn_override = vfn_dummy;
                        save_top = curr;
                        save_check = check;
                    }
                    break;
                }
            }
        }
        if( vfn_override != NULL ) {
            if( save_check != FNOV_EXACT_MATCH ) {
                /* return conversions are too ugly for now */
                return( WALK_FINISH );
            }
            location.delta = 0;
            findPtrOffset( top, data->derived, &location );
            vfn_thunk_delta = location.exact_delta;
            location.delta = 0;
            findPtrOffset( save_top, data->derived, &location );
            vfn_thunk_delta -= location.exact_delta;
            data->this_delta = vfn_thunk_delta;
            data->vfn_override = vfn_override;
        }
        return( WALK_ABANDON );
    }
    return( WALK_NORMAL );
}

SYMBOL ScopeFindExactVfun(      // FIND EXACT VIRTUAL FUNCTION IN DERIVED CLASS
    SYMBOL vfun,                // - virtual fun in a base class
    SCOPE scope,                // - scope for derived class
    target_offset_t* a_adj_this,// - adjustment for this
    target_offset_t* a_adj_retn)// - adjustment for return
{
    auto vfn_opt_walk data;

    data.derived = scope;
    data.base = SymScope( vfun );
    data.vfn_sym = vfun;
    data.vfn_override = NULL;
    data.found = NULL;
    data.this_delta = 0;
    data.retn_delta = 0;
    walkDirectBases( scope, findExactOverride, &data );
    *a_adj_this = data.this_delta;
    *a_adj_retn = data.retn_delta;
    return( data.vfn_override );
}

static msg_status_t lookupError( SEARCH_RESULT *result, MSG_NUM msg )
{
    if( result->errlocn.src_file != NULL ) {
        SetErrLoc( &result->errlocn );
    }
    return( CErr1( msg ) );
}

static boolean searchError( SEARCH_RESULT *result, SYMBOL sym, unsigned msg )
{
#if 1
    if( result->errlocn.src_file != NULL ) {
        SetErrLoc( &result->errlocn );
    }
    CErr2p( msg, sym );
#else
    AccessErrMsg( msg, sym, &result->errlocn );
#endif
    return TRUE;
}

static boolean diagnoseAmbiguity( SEARCH_RESULT *result, SYMBOL sym )
{
    if( result->ambiguous ) {
        return searchError( result, sym, ERR_AMBIGUOUS_MEMBER );
    }
    return( FALSE );
}

boolean ScopeImmediateCheck( SEARCH_RESULT *result )
/**************************************************/
{
    SYMBOL sym;

    if( ! _IsClassScope( result->scope ) ) {
        return( FALSE );
    }
    sym = result->sym;
    if( sym == NULL ) {
        sym = result->sym_name->name_syms;
    }
    return( diagnoseAmbiguity( result, sym ) );
}

boolean ScopeAmbiguousSymbol( SEARCH_RESULT *result, SYMBOL sym )
/***************************************************************/
{
    if( ! _IsClassScope( result->scope ) ) {
        return( FALSE );
    }
    /* report ambiguity error */
    if( result->mixed_static ) {
        /* fn overload isn't ambiguous provided static member fn is chosen */
        if( ! SymIsStatic( sym ) ) {
            result->ambiguous = TRUE;
        }
    }
    return( diagnoseAmbiguity( result, sym ) );
}

void ScopeDontCheckSymbol( SEARCH_RESULT *result )
/************************************************/
{
    if( result != NULL ) {
        result->ignore_access = TRUE;
    }
}

boolean ScopeCheckSymbol( SEARCH_RESULT *result, SYMBOL sym )
/***********************************************************/
{
    SCOPE located;
    inherit_flag perm;
    int err_msg;
    msg_status_t msg_status;
    auto access_data access_data;

    if( result->lookup_error ) {
        msg_status = lookupError( result, result->error_msg );
        if( msg_status & MS_PRINTED ) {
            MSG_NUM info_msg = result->info_msg;
            CErr2p( info_msg, result->info1 );
            CErr2p( info_msg, result->info2 );
            if(( msg_status & MS_WARNING ) == 0 ) {
                return( TRUE );
            }
        }
    }
    located = result->scope;
    if( result->cant_be_auto ) {
        switch( sym->id ) {
        case SC_AUTO:
        case SC_REGISTER:
            if( DefargBeingCompiled() ) {
                if( ScopeType( located, SCOPE_FUNCTION ) ) {
                    err_msg = ERR_DEFAULT_ARG_USES_ARG;
                } else {
                    err_msg = ERR_DEFAULT_ARG_USES_LOCAL;
                }
            } else {
                if( ScopeType( located, SCOPE_FUNCTION ) ) {
                    err_msg = ERR_LOCAL_ACCESSING_ARG;
                } else {
                    err_msg = ERR_LOCAL_ACCESSING_AUTO;
                }
            }
            return searchError( result, sym, err_msg );
        }
    }
    if( ! _IsClassScope( located ) ) {
        return( FALSE );
    }
    if( result->ignore_access ) {
        return( FALSE );
    }
    if( ScopeAmbiguousSymbol( result, sym ) ) {
        return( TRUE );
    }
    /* report base class access error */
    perm = result->perm;
    if( perm != IN_PUBLIC ) {
        err_msg = ERR_ACCESS_THROUGH_PRIVATE;
        if( perm == IN_PROTECTED ) {
            err_msg = ERR_ACCESS_THROUGH_PROTECTED;
        }
        return searchError( result, sym, err_msg );
    }
    /* report access permission error */
    perm = makePerm( sym->flag );
    newAccessData( &access_data, perm, located );
    if( result->protected_OK ) {
        access_data.protected_OK = TRUE;
    }
    perm = verifyAccess( &access_data );
    if( perm != IN_PUBLIC ) {
        err_msg = ERR_PRIVATE_MEMBER;
        if( perm == IN_PROTECTED ) {
            err_msg = ERR_PROTECTED_MEMBER;
        }
        return searchError( result, sym, err_msg );
    }
    return( FALSE );
}

static SYM_REGION *dupSymRegionList( SYM_REGION *list )
{
    SYM_REGION *old_region;
    SYM_REGION *new_region;
    SYM_REGION *h;

    h = NULL;
    RingIterBeg( list, old_region ) {
        new_region = newSYM_REGION( old_region->from, old_region->to );
        RingAppend( &h, new_region );
    } RingIterEnd( old_region )
    return( h );
}

SEARCH_RESULT *ScopeDupResult( SEARCH_RESULT *result )
/****************************************************/
{
    SEARCH_RESULT *new_result;

    if( result != NULL ) {
        /* change to reference counts if this gets complicated */
        new_result = newResult( NULL, NULL );
        *new_result = *result;
        if( new_result->region != NULL ) {
            new_result->region = dupSymRegionList( result->region );
        }
        result = new_result;
    }
    return( result );
}

void ScopeFreeResult( SEARCH_RESULT *result )
/*******************************************/
{
    SYM_REGION **region_head;

    if( result != NULL ) {
        region_head = &(result->region);
        if( *region_head != NULL ) {
            RingCarveFree( carveSYM_REGION, region_head );
        }
        CarveFree( carveSEARCH_RESULT, result );
    }
}


boolean ScopeEnclosed( SCOPE encloser, SCOPE enclosed )
/*****************************************************/
{
    for( ; enclosed != NULL; enclosed = enclosed->enclosing ) {
        if( encloser == enclosed ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

void ScopeKeep( SCOPE scope )
/***************************/
{
    scope->s.keep = TRUE;
}

void ScopeArgumentCheck( SCOPE scope )
/************************************/
{
    scope->s.arg_check = TRUE;
}

void ScopeQualifyPush( SCOPE scope, SCOPE access )
/************************************************/
{
    QUALIFICATION *qual;

    qual = CarveAlloc( carveQUALIFICATION );
    qual->access = access;
    qual->reset = GetCurrScope();
    if( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_DECL )
     || ScopeType( GetCurrScope(), SCOPE_TEMPLATE_PARM ) ) {
        // need to keep the template decl/parm scope at the top
        qual->enclosing = GetCurrScope()->enclosing;
        ScopeSetEnclosing( GetCurrScope(), scope );
    } else {
        qual->enclosing = NULL;
        SetCurrScope(scope);
    }
    ParsePushQualification( qual );
}

SCOPE ScopeQualifyPop( void )
/***************************/
{
    QUALIFICATION *qual;
    SCOPE scope_popped;

    scope_popped = GetCurrScope();
    qual = ParsePopQualification();
    if( qual != NULL ) {
        if( qual->enclosing != NULL ) {
            ScopeSetEnclosing( qual->reset, qual->enclosing );
        }
        SetCurrScope( qual->reset );
        CarveFree( carveQUALIFICATION, qual );
    }
    return( scope_popped );
}

SCOPE ScopeEnclosingId( SCOPE scope, scope_type_t id )
/****************************************************/
{
    for( ; scope != NULL; scope = scope->enclosing ) {
        if( scope->id == id ) {
            break;
        }
    }
    return( scope );
}

SCOPE ScopeNearestNonTemplate( SCOPE scope )
/******************************************/
{
    for( ; scope != NULL; scope = scope->enclosing ) {
        if( ( scope->id != SCOPE_TEMPLATE_DECL )
         && ( scope->id != SCOPE_TEMPLATE_INST )
         && ( scope->id != SCOPE_TEMPLATE_PARM )
         && ( scope->id != SCOPE_TEMPLATE_SPEC_PARM ) ) {
            break;
        }
    }
    return( scope );
}

SCOPE ScopeNearestNonClass( SCOPE scope )
/***************************************/
{
    for( ; scope != NULL; scope = scope->enclosing ) {
        /* we have too many weird scope types so we check for positive cases */
        if( _IsBlockScope( scope ) )
            break;
        if( _IsFileScope( scope ) ) {
            break;
        }
    }
    return( scope );
}

SCOPE ScopeNearestFile( SCOPE scope )
/***********************************/
{
    for( ; scope != NULL; scope = scope->enclosing ) {
        if( _IsFileScope( scope ) ) {
            break;
        }
    }
    return( scope );
}

SCOPE ScopeNearestFileOrClass( SCOPE scope )
/***********************************/
{
    for( ; scope != NULL; scope = scope->enclosing ) {
        if( _IsFileScope( scope ) )
            break;
        if( _IsClassScope( scope ) ) {
            break;
        }
    }
    return( scope );
}

SCOPE ScopeFunctionScopeInProgress( void )
/****************************************/
{
    return( findFunctionScope( GetCurrScope() ) );
}



SYMBOL ScopeFunctionScope( SCOPE scope )
/**************************************/
{
    scope = findFunctionScope( scope );
    if( scope == NULL ) {
        return( NULL );
    } else {
        return( ScopeFunction( scope ) );
    }
}

SYMBOL ScopeFunctionInProgress( void )
/************************************/
{
    return ScopeFunctionScope( GetCurrScope() );
}


SYMBOL ScopeFuncParm( unsigned parm_no )
/**************************************/
{
    SYMBOL stopper;
    SYMBOL sym;

    stopper = ScopeOrderedStart( findFunctionScope( GetCurrScope() ) );
    for( sym = NULL; ; --parm_no ) {
        sym = ScopeOrderedNext( stopper, sym );
        if( parm_no == 0 ) {
            break;
        }
    }
    return( sym );
}

void ScopeWalkDirectBases(          // WALK DIRECT BASES
    SCOPE scope,                    // - scope
    void (*rtn)(BASE_CLASS*,void*)  // - routine
    , void *data )                  // - data to be passed
{
    BASE_CLASS *base;

    RingIterBeg( ScopeInherits( scope ), base ) {
        if( ! _IsVirtualBase( base ) ) {
            (*rtn)( base, data );
        }
    } RingIterEnd( base )
}

void ScopeWalkVirtualBases(         // WALK VIRTUAL BASES
    SCOPE scope,                    // - scope
    void (*rtn)(BASE_CLASS*,void*)  // - routine
    , void *data )                  // - data to be passed
{
    BASE_CLASS *base;

    RingIterBeg( ScopeInherits( scope ), base ) {
        if(  _IsVirtualBase( base ) ) {
            (*rtn)( base, data );
        }
    } RingIterEnd( base )
}

void ScopeMemberModuleFunction( SCOPE init_scope, SCOPE fn_scope )
/****************************************************************/
{
    fn_scope = findFunctionScope( fn_scope );
    if( fn_scope != NULL ) {
        // don't propagate ->in_unnamed
        fn_scope->enclosing = init_scope;
    }
}

void ScopeRestoreModuleFunction( SCOPE fn_scope )
/***********************************************/
{
    fn_scope = findFunctionScope( fn_scope );
    if( fn_scope != NULL ) {
        // don't propagate ->in_unnamed
        fn_scope->enclosing = GetFileScope();
    }
}

boolean ScopeLocalClass( SCOPE scope )
/************************************/
{
    if( ! _IsClassScope( scope ) ) {
        return( FALSE );
    }
    scope = findFunctionScope( scope );
    if( scope != NULL ) {
        return( TRUE );
    }
    return( FALSE );
}

SCOPE ScopeHostClass( SCOPE class_scope )
/***************************************/
{
    SCOPE next;

#ifndef NDEBUG
    if( ! _IsClassScope( class_scope ) ) {
        CFatal( "ScopeHostClass passed a non-class scope" );
    }
#endif
    /* find host class for a class scope */
    for(;;) {
        /* class_scope is a class scope here */
        next = class_scope->enclosing;
        if( ! _IsClassScope( next ) )
            break;
        class_scope = next;
    }
    return( class_scope );
}

target_offset_t ScopeVBPtrOffset( SCOPE scope )
/*********************************************/
{
    TYPE class_type;

    class_type = ScopeClass( scope );
    return( class_type->u.c.info->vb_offset );
}

/*
** entry points for #pragma code when symbol accesses are required
*/

SYMBOL ScopeASMUseSymbol( NAME name, boolean *uses_auto )
/*******************************************************/
{
    SYMBOL sym;

    sym = ScopeASMLookup( name );
    if( sym == NULL ) {
        CErr2p( ERR_UNDECLARED_SYM, name );
        return( NULL );
    }
    sym->flag |= SF_ADDR_TAKEN | SF_CG_ADDR_TAKEN;
    sym = SymMarkRefed( sym );
    if( SymIsAutomatic( sym ) ) {
        sym->sym_type = MakeForceInMemory( sym->sym_type );
        *uses_auto = TRUE;
    }
    return( sym );
}

void ScopeASMUsesAuto( void )
/***************************/
{
    SYMBOL fn_sym;

    fn_sym = ScopeFunctionInProgress();
    fn_sym->flag |= SF_DONT_INLINE;
}

SYMBOL ScopeASMLookup( NAME name )
/********************************/
{
    SYMBOL          sym;
    SEARCH_RESULT   *result;

    sym = NULL;
    result = ScopeFindNaked( GetCurrScope(), NameCreateNoLen( name ) );
    if( result != NULL ) {
        if( result->simple ) {
            sym = result->sym_name->name_syms;
            if( sym != NULL ) {
                if( SymIsFunction( sym ) ) {
                    sym = ActualNonOverloadedFunc( sym, result );
                } else if( SymIsEnumeration( sym ) ) {
                    // we'll pretend we can't see enum constants
                    sym = NULL;
                }
            }
        }
        ScopeFreeResult( result );
    }
    return( sym );
}

SYMBOL ScopeIntrinsic( boolean turn_on )
/**************************************/
{
    char *name;
    SEARCH_RESULT *result;
    SYMBOL_NAME sym_name;
    SYMBOL sym;
    TYPE type;

    name = NameCreateLen( Buffer, TokenLen );
    result = ScopeFindNaked( ScopeNearestFile( GetCurrScope() ), name );
    if( result == NULL ) {
        return( NULL );
    }
    sym_name = result->sym_name;
    ScopeFreeResult( result );
    RingIterBeg( sym_name->name_syms, sym ) {
        if( LinkageIsC( sym ) ) {
            type = sym->sym_type;
            if( FunctionDeclarationType( type ) != NULL ) {
                if( turn_on ) {
                    type = AddFunctionFlag( type, TF1_INTRINSIC );
                } else {
                    type = RemoveFunctionFlag( type, TF1_INTRINSIC );
                }
                sym->sym_type = type;
                return( sym );
            }
        }
    } RingIterEnd( sym )
    return( NULL );
}

static void changeSymType( SYMBOL sym, TYPE type )
{
    if( sym->id == SC_TYPEDEF ) {
        TypedefReset( sym, type );
    } else {
        sym->sym_type = type;
    }
}

static boolean changePragmaType(// TEST IF NEW NEW PRAGMA TYPE REQUIRED
    SYMBOL sym,                 // - old symbol
    AUX_INFO *auxinfo )        // - new aux info
{
    boolean     retn;           // - return: TRUE ==> change required
    AUX_INFO    *old_pragma;    // - old aux info

    old_pragma = TypeHasPragma( sym->sym_type );
    if( old_pragma == NULL ) {
        retn = TRUE;
    } else if( old_pragma == auxinfo ) {
        retn = FALSE;
    } else if( PragmaChangeConsistent( old_pragma, auxinfo ) ) {
        retn = TRUE;
    } else {
        CErr2p( WARN_PRAGMA_MERGE, sym );
        retn = FALSE;
    }
    return retn;
}

static void changeNonFunction( SYMBOL sym, AUX_INFO *auxinfo )
{
    TYPE type;                  // - modifier type

    if( changePragmaType( sym, auxinfo ) ) {
        type = MakePragmaModifier( auxinfo );
        changeSymType( sym, AddNonFunctionPragma( type, sym->sym_type ) );
    }
}

void ScopeAuxName( char *id, AUX_INFO *auxinfo )
/***********************************************/
{
    NAME name;
    SEARCH_RESULT *result;
    SYMBOL_NAME sym_name;
    SYMBOL sym;
    SYMBOL syms;
    SYMBOL aux_sym;
    TYPE ret_type;
    TYPE arg_type;
    TYPE fn_type;
    unsigned c_linkage_count;
    unsigned count;

    name = NameCreateNoLen( id );
    result = ScopeFindNaked( GetFileScope(), name );
    if( result == NULL ) {
        /* name is not defined (in file scope) */
        /* action: create 'extern "C" void name(...);' */
        ret_type = GetBasicType( TYP_VOID );
        arg_type = GetBasicType( TYP_DOT_DOT_DOT );
        fn_type = MakeModifiableFunction( ret_type, arg_type, NULL );
        fn_type->u.f.pragma = auxinfo;
        fn_type = CheckDupType( fn_type );
        sym = SymCreateAtLocn( fn_type, SC_EXTERN, 0, name, GetFileScope(), NULL );
        LinkageSet( sym, "C" );
        return;
    }
    aux_sym = NULL;
    c_linkage_count = 0;
    count = 0;
    sym_name = result->sym_name;
    ScopeFreeResult( result );
    syms = sym_name->name_syms;
    if( syms == NULL ) {
        syms = sym_name->name_type;
    }
    RingIterBeg( syms, sym ) {

        /*
         *  Check to see if we are defining code and we already have a symbol 
         *  defined that has code attached ( a function body )
         */ 
        if( auxinfo && auxinfo->code && SymIsInitialized( sym ) && SymIsFunction( sym ) ){
            CErr2p( ERR_FUNCTION_REDEFINITION, sym );   //ERR_SYM_ALREADY_DEFINED, sym );
        }
        
        fn_type = FunctionDeclarationType( sym->sym_type );
        if( fn_type == NULL ) {
            changeNonFunction( sym, auxinfo );
            return;
        }
        if( SymIsDefArg( sym ) ) {
            continue;
        }
        if( SymIsFunctionTemplateModel( sym ) ) {
            // can generate lots of C++ functions
            count += 2;
        } else {
            if( c_linkage_count == 0 ) {
                aux_sym = sym;
            }
            if( LinkageIsC( sym ) ) {
                ++c_linkage_count;
            }
            ++count;
        }
    } RingIterEnd( sym )
    if( c_linkage_count == 1 || count == 1 ) {
        /* name is defined with "C" linkage */
        /* OR name is defined once with "C++" linkage */
        /* action: set pragma entry in function type */
        if( changePragmaType( aux_sym, auxinfo ) ) {
            fn_type = ChangeFunctionPragma( aux_sym->sym_type, auxinfo );
            changeSymType( aux_sym, fn_type );
        }
        return;
    }
    /* name is defined more than once with "C++" linkage */
    CErr2p( ERR_PRAGMA_AUX_CANNOT_OVERLOAD, syms );
}


void ScopeResultErrLocn(        // SET ERROR LOCATION IN SEARCH RESULT
    SEARCH_RESULT *result,      // - search result
    TOKEN_LOCN *locn )          // - potential error location
{
    if( locn != NULL && locn->src_file != NULL ) {
        result->errlocn = *locn;
    }
}

static walk_status markNotVisited( BASE_STACK *top, void *parm )
{
    SCOPE scope;
    TYPE class_type;

    parm = parm;
    scope = top->scope;
    class_type = ScopeClass( scope );
    class_type->flag &= ~TF1_VISITED;
    return( WALK_NORMAL );
}

static walk_status walkNotVisited( BASE_STACK *top, void *parm )
{
    all_bases_walk *data = parm;
    SCOPE scope;
    TYPE class_type;

    scope = top->scope;
    class_type = ScopeClass( scope );
    if( class_type->flag & TF1_VISITED ) {
        return( WALK_ABANDON );
    }
    data->rtn( scope, data->data );
    class_type->flag |= TF1_VISITED;
    return( WALK_NORMAL );
}

void ScopeWalkAncestry(         // VISIT ONCE ALL CLASSES IN ANCESTRY
    SCOPE scope,                // - class scope
    void (*rtn)(                // - walker routine
        SCOPE,                  // -- base class scope
        void * ),               // -- user data
    void *data )                // - user supplied data
{
    auto all_bases_walk walk_data;

    walkVisitOnce( scope, markNotVisited, NULL );
    walk_data.rtn = rtn;
    walk_data.data = data;
    walkDirectBases( scope, walkNotVisited, &walk_data );
}

boolean ScopeDebugable(         // DETERMINE IF SCOPE TO BE DEBUGGED
    SCOPE scope )               // - the scope
{
    boolean retn;               // - TRUE ==> pass scope to debugger

    if( NULL == scope ) {
        retn = FALSE;
    } else if( ScopeFunction( scope->enclosing ) ) {
        retn = FALSE;
    } else if( HashEmpty( scope->names ) ) {
        retn = FALSE;
    } else {
        retn = TRUE;
    }
    return retn;
}

SYMBOL_NAME SymbolNameGetIndex( SYMBOL_NAME e )
/*********************************************/
{
    return( CarveGetIndex( carveSYMBOL_NAME, e ) );
}

SYMBOL_NAME SymbolNameMapIndex( SYMBOL_NAME i )
/*********************************************/
{
    return( CarveMapIndex( carveSYMBOL_NAME, i ) );
}

NAME_SPACE *NameSpaceGetIndex( NAME_SPACE *e )
/********************************************/
{
    return( CarveGetIndex( carveNAME_SPACE, e ) );
}

NAME_SPACE *NameSpaceMapIndex( NAME_SPACE *i )
/********************************************/
{
    return( CarveMapIndex( carveNAME_SPACE, i ) );
}

SYMBOL SymbolGetIndex( SYMBOL e )
/*******************************/
{
    return( CarveGetIndex( carveSYMBOL, e ) );
}

SYMBOL SymbolMapIndex( SYMBOL i )
/*******************************/
{
    if( i == NULL ) {
        return i;
    }
    return( CarveMapIndex( carveSYMBOL, i ) );
}

SCOPE ScopeGetIndex( SCOPE e )
/****************************/
{
    return( CarveGetIndex( carveSCOPE, e ) );
}

SCOPE ScopeMapIndex( SCOPE i )
/****************************/
{
    return( CarveMapIndex( carveSCOPE, i ) );
}

static void markFreeSymRegion( void *p )
{
    SYM_REGION *u = p;

    u->from = NULL;
}

static void saveSymRegion( void *e, carve_walk_base *d )
{
    SYM_REGION *r = e;
    SYM_REGION *save_next;
    SYMBOL save_from;
    SYMBOL save_to;

    if( r->from == NULL ) {
        return;
    }
    save_next = r->next;
    r->next = CarveGetIndex( carveSYM_REGION, save_next );
    save_from = r->from;
    r->from = SymbolGetIndex( save_from );
    save_to = r->to;
    r->to = SymbolGetIndex( save_to );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *r );
    r->next = save_next;
    r->from = save_from;
    r->to = save_to;
}

static void markFreeUsingNS( void *p )
{
    USING_NS *u = p;

    u->using_scope = NULL;
}

static void saveUsingNS( void *e, carve_walk_base *d )
{
    USING_NS *u = e;
    USING_NS *save_next;
    SCOPE save_trigger;
    SCOPE save_using_scope;

    if( u->using_scope == NULL ) {
        return;
    }
    save_next = u->next;
    u->next = CarveGetIndex( carveUSING_NS, save_next );
    save_trigger = u->trigger;
    u->trigger = ScopeGetIndex( save_trigger );
    save_using_scope = u->using_scope;
    u->using_scope = ScopeGetIndex( save_using_scope );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *u );
    u->next = save_next;
    u->trigger = save_trigger;
    u->using_scope = save_using_scope;
}

static void markFreeNameSpace( void *p )
{
    NAME_SPACE *ns = p;

    ns->s.free = TRUE;
}

static void saveNameSpace( void *e, carve_walk_base *d )
{
    NAME_SPACE *ns = e;
    SYMBOL save_sym;
    SCOPE save_scope;
    NAME_SPACE *save_all;

    if( ns->s.free ) {
        return;
    }
    save_sym = ns->sym;
    ns->sym = SymbolGetIndex( save_sym );
    save_scope = ns->scope;
    ns->scope = ScopeGetIndex( save_scope );
    save_all = ns->all;
    ns->all = NameSpaceGetIndex( save_all );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *ns );
    ns->sym = save_sym;
    ns->scope = save_scope;
    ns->all = save_all;
}

static void markFreeScope( void *p )
{
    SCOPE s = p;

    s->id = SCOPE_FREE;
}

static void saveScope( void *e, carve_walk_base *d )
{
    SCOPE s = e;
    SCOPE save_enclosing;
    HASHTAB save_names;
    SYMBOL save_ordered;
    SYMBOL save_owner_sym;
    TYPE save_owner_type;
    NAME_SPACE *save_owner_ns;
    USING_NS *save_using_list;
    TEMPLATE_INFO *save_owner_tinfo;

    if( s->id == SCOPE_FREE ) {
        return;
    }
    save_enclosing = s->enclosing;
    s->enclosing = ScopeGetIndex( save_enclosing );
    save_names = s->names;
    s->names = HashGetIndex( save_names );
    save_ordered = s->ordered;
    s->ordered = SymbolGetIndex( save_ordered );
    save_using_list = s->using_list;
    s->using_list = CarveGetIndex( carveUSING_NS, save_using_list );
    switch( s->id ) {
    case SCOPE_FILE:
        save_owner_ns = s->owner.ns;
        DbgAssert( save_owner_ns != NULL );
        s->owner.ns = NameSpaceGetIndex( save_owner_ns );
        break;
    case SCOPE_FUNCTION:
        save_owner_sym = s->owner.sym;
        s->owner.sym = SymbolGetIndex( save_owner_sym );
        break;
    case SCOPE_CLASS:
        save_owner_type = s->owner.type;
        s->owner.type = TypeGetIndex( save_owner_type );
        break;
    case SCOPE_TEMPLATE_PARM:
        save_owner_tinfo = s->owner.tinfo;
        if( save_owner_tinfo != NULL ) {
            DbgAssert( s->s.fn_template == FALSE );
            s->owner.tinfo = TemplateClassInfoGetIndex( save_owner_tinfo );
        }
        break;
    }
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    s->enclosing = save_enclosing;
    s->names = save_names;
    s->ordered = save_ordered;
    s->using_list = save_using_list;
    switch( s->id ) {
    case SCOPE_FILE:
        s->owner.ns = save_owner_ns;
        break;
    case SCOPE_FUNCTION:
        s->owner.sym = save_owner_sym;
        break;
    case SCOPE_CLASS:
        s->owner.type = save_owner_type;
        break;
    case SCOPE_TEMPLATE_PARM:
        s->owner.tinfo = save_owner_tinfo;
        break;
    }
}

static void markFreeSymbolName( void *p )
{
    SYMBOL_NAME s = p;

    s->containing = NULL;
}

static void saveSymbolName( void *e, carve_walk_base *d )
{
    SYMBOL_NAME s = e;
    SYMBOL_NAME save_next;
    SYMBOL save_name_type;
    SYMBOL save_name_syms;
    NAME save_name;
    SCOPE save_containing;

    if( s->containing == NULL ) {
        DbgStmt( CarveVerifyFreeElement( carveSYMBOL_NAME, e ) );
        return;
    }
    save_next = s->next;
    s->next = SymbolNameGetIndex( save_next );
    save_name_type = s->name_type;
    s->name_type = SymbolGetIndex( save_name_type );
    save_name_syms = s->name_syms;
    s->name_syms = SymbolGetIndex( save_name_syms );
    save_name = s->name;
    s->name = NameGetIndex( save_name );
    save_containing = s->containing;
    s->containing = ScopeGetIndex( save_containing );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    s->next = save_next;
    s->name_type = save_name_type;
    s->name_syms = save_name_syms;
    s->name = save_name;
    s->containing = save_containing;
}

static void markFreeSymbol( void *p )
{
    SYMBOL s = p;

    s->id = SC_FREE;
}

static void saveSymbol( void *e, carve_walk_base *d )
{
    SYMBOL s = e;
    SYMBOL save_next;
    SYMBOL save_thread;
    TYPE save_sym_type;
    SYMBOL_NAME save_name;
    SYM_TOKEN_LOCN *save_locn;
    TEMPLATE_INFO *save_u_tinfo;
    FN_TEMPLATE *save_u_defn;
    SYMBOL save_u_sym;
    TYPE save_u_type;
    NAME_SPACE *save_u_ns;
    PTREE save_u_defarg_info;
    POOL_CON* save_con;

    if( s->id == SC_FREE ) {
        return;
    }
    save_next = s->next;
    if( s == s->next ) {
        s->next = (SYMBOL)d->index;
    } else {
        s->next = SymbolGetIndex( save_next );
    }
    save_thread = s->thread;
    s->thread = SymbolGetIndex( save_thread );
    save_sym_type = s->sym_type;
    s->sym_type = TypeGetIndex( save_sym_type );
    save_name = s->name;
    s->name = SymbolNameGetIndex( save_name );
    save_locn = s->locn;
    s->locn = SymTokenLocnGetIndex( save_locn );
    switch( s->id ) {
    case SC_NAMESPACE:
        save_u_ns = s->u.ns;
        s->u.ns = NameSpaceGetIndex( save_u_ns );
        break;
    case SC_CLASS_TEMPLATE:
        save_u_tinfo = s->u.tinfo;
        s->u.tinfo = TemplateClassInfoGetIndex( save_u_tinfo );
        break;
    case SC_FUNCTION_TEMPLATE:
    case SC_EXTERN_FUNCTION_TEMPLATE:
    case SC_STATIC_FUNCTION_TEMPLATE:
        save_u_defn = s->u.defn;
        s->u.defn = TemplateFunctionInfoGetIndex( save_u_defn );
        break;
    case SC_DEFAULT:
        save_u_defarg_info = s->u.defarg_info;
        s->u.defarg_info = PTreeGetIndex( save_u_defarg_info );
        break;
    case SC_VIRTUAL_FUNCTION:
    case SC_ADDRESS_ALIAS:
        save_u_sym = s->u.sym;
        s->u.sym = SymbolGetIndex( save_u_sym );
        break;
    case SC_ACCESS:
        save_u_type = s->u.type;
        s->u.type = TypeGetIndex( save_u_type );
        break;
    default:
        if( s->flag & ( SF_ADDR_THUNK | SF_TEMPLATE_FN | SF_ALIAS ) ) {
            save_u_sym = s->u.sym;
            s->u.sym = SymbolGetIndex( save_u_sym );
        } else if( s->flag & SF_CONSTANT_INT64 ) {
            save_con = s->u.pval;
            s->u.pval = ConstantPoolGetIndex( save_con );
        } else if( (s->flag & SF_ANONYMOUS) && ( s->id != SC_MEMBER ) ) {
            save_u_sym = s->u.sym;
            s->u.sym = SymbolGetIndex( save_u_sym );
        }
    }
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    s->next = save_next;
    s->thread = save_thread;
    s->sym_type = save_sym_type;
    s->name = save_name;
    s->locn = save_locn;
    switch( s->id ) {
    case SC_NAMESPACE:
        s->u.ns = save_u_ns;
        break;
    case SC_CLASS_TEMPLATE:
        s->u.tinfo = save_u_tinfo;
        break;
    case SC_FUNCTION_TEMPLATE:
    case SC_EXTERN_FUNCTION_TEMPLATE:
    case SC_STATIC_FUNCTION_TEMPLATE:
        s->u.defn = save_u_defn;
        break;
    case SC_DEFAULT:
        s->u.defarg_info = save_u_defarg_info;
        break;
    case SC_VIRTUAL_FUNCTION:
    case SC_ADDRESS_ALIAS:
        s->u.sym = save_u_sym;
        break;
    case SC_ACCESS:
        s->u.type = save_u_type;
        break;
    default:
        DbgVerify( SymIsThunk( s ) == (( s->flag & SF_ADDR_THUNK ) != 0 ), "SF_ADDR_THUNK not set properly" );
        if( s->flag & ( SF_ADDR_THUNK | SF_TEMPLATE_FN | SF_ALIAS ) ) {
            s->u.sym = save_u_sym;
        } else if( s->flag & SF_CONSTANT_INT64 ) {
            s->u.pval = save_con;
        } else if( (s->flag & SF_ANONYMOUS) && ( s->id != SC_MEMBER ) ) {
            s->u.sym = save_u_sym;
        }
    }
}

static void saveMappingList( void )
{
    unsigned count;
    SYMBOL save_sym;
    SCOPE save_from;
    SCOPE save_to;
    SAVE_MAPPING *m;

    count = 0;
    for( m = mappingList; m != NULL; m = m->next ) {
        ++count;
    }
    PCHWriteUInt( count );
    for( m = mappingList; m != NULL; m = m->next ) {
        save_sym = m->sym;
        m->sym = SymbolGetIndex( save_sym );
        save_from = m->from;
        m->from = ScopeGetIndex( save_from );
        save_to = m->to;
        m->to = ScopeGetIndex( save_to );
        PCHWriteVar( *m );
        m->sym = save_sym;
        m->from = save_from;
        m->to = save_to;
    }
}

pch_status PCHWriteScopes( void )
{
    auto carve_walk_base data;

    PCHWriteUInt( PCHGetUInt( NameGetIndex( uniqueNameSpaceName ) ) );
    PCHWriteCVIndex( (cv_index)NameSpaceGetIndex( allNameSpaces ) );
    PCHWriteCVIndex( (cv_index)ScopeGetIndex( GetCurrScope() ) );
    PCHWriteCVIndex( (cv_index)ScopeGetIndex( GetFileScope() ) );
    PCHWriteCVIndex( (cv_index)ScopeGetIndex( GetInternalScope() ) );
    PCHWriteCVIndex( (cv_index)SymbolGetIndex( ChipBugSym ) );
    PCHWriteCVIndex( (cv_index)SymbolGetIndex( DFAbbrevSym ) );
    PCHWriteCVIndex( (cv_index)SymbolGetIndex( PCHDebugSym ) );
    CarveWalkAllFree( carveSYM_REGION, markFreeSymRegion );
    CarveWalkAll( carveSYM_REGION, saveSymRegion, &data );
    PCHWriteCVIndexTerm();
    CarveWalkAllFree( carveUSING_NS, markFreeUsingNS );
    CarveWalkAll( carveUSING_NS, saveUsingNS, &data );
    PCHWriteCVIndexTerm();
    CarveWalkAllFree( carveNAME_SPACE, markFreeNameSpace );
    CarveWalkAll( carveNAME_SPACE, saveNameSpace, &data );
    PCHWriteCVIndexTerm();
    CarveWalkAllFree( carveSCOPE, markFreeScope );
    CarveWalkAll( carveSCOPE, saveScope, &data );
    PCHWriteCVIndexTerm();
    CarveWalkAllFree( carveSYMBOL_NAME, markFreeSymbolName );
    CarveWalkAll( carveSYMBOL_NAME, saveSymbolName, &data );
    PCHWriteCVIndexTerm();
    CarveWalkAllFree( carveSYMBOL, markFreeSymbol );
    CarveWalkAll( carveSYMBOL, saveSymbol, &data );
    PCHWriteCVIndexTerm();
    saveMappingList();
    return( PCHCB_OK );
}

static void readMappingList( void )
{
    unsigned        count;
    SAVE_MAPPING    *m;

    while( mappingList != NULL ) {
        m = mappingList;
        mappingList = m->next;
        CMemFree( m );
    }
    for( count = PCHReadUInt(); count != 0; --count ) {
        m = CMemAlloc( sizeof( SAVE_MAPPING ) );
        PCHReadVar( *m );
        m->sym = SymbolMapIndex( m->sym );
        m->from = ScopeMapIndex( m->from );
        m->to = ScopeMapIndex( m->to );
        m->next = mappingList;
        mappingList = m;
    }
}

static void readSymRegion( void )
{
    SYM_REGION *u;
    auto cvinit_t data;

    CarveInitStart( carveSYM_REGION, &data );
    for( ; (u = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *u );
        u->next = CarveMapIndex( carveSYM_REGION, u->next );
        u->from = SymbolMapIndex( u->from );
        u->to = SymbolMapIndex( u->to );
    }
}

static void readUsingNS( void )
{
    USING_NS *u;
    auto cvinit_t data;

    CarveInitStart( carveUSING_NS, &data );
    for( ; (u = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *u );
        u->next = CarveMapIndex( carveUSING_NS, u->next );
        u->trigger = ScopeMapIndex( u->trigger );
        u->using_scope = ScopeMapIndex( u->using_scope );
    }
}

static void readNameSpaces( void )
{
    NAME_SPACE *ns;
    auto cvinit_t data;

    CarveInitStart( carveNAME_SPACE, &data );
    for( ; (ns = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *ns );
        ns->sym = SymbolMapIndex( ns->sym );
        ns->scope = ScopeMapIndex( ns->scope );
        ns->all = NameSpaceMapIndex( ns->all );
    }
}

static void readScopes( void )
{
    SCOPE s;
    auto cvinit_t data;

    CarveInitStart( carveSCOPE, &data );
    for( ; (s = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *s );
        s->enclosing = ScopeMapIndex( s->enclosing );
        s->names = HashMapIndex( s->names );
        s->ordered = SymbolMapIndex( s->ordered );
        s->using_list = CarveMapIndex( carveUSING_NS, s->using_list );
        // used to indicate changes from creation time (or PCH creation time)
        s->s.dirty = FALSE;
        switch( s->id ) {
        case SCOPE_FUNCTION:
            s->owner.sym = SymbolMapIndex( s->owner.sym );
            break;
        case SCOPE_CLASS:
            s->owner.type = TypeMapIndex( s->owner.type );
            break;
        case SCOPE_FILE:
            DbgAssert( s->owner.ns != NULL );
            s->owner.ns = NameSpaceMapIndex( s->owner.ns );
            break;
        case SCOPE_TEMPLATE_PARM:
            DbgAssert( s->s.fn_template == FALSE );
            s->owner.tinfo = TemplateClassInfoMapIndex( s->owner.tinfo );
            break;
        default :
            break;
        }
    }
}

static void readSymbolNames( void )
{
    SYMBOL_NAME sn;
    auto cvinit_t data;

    CarveInitStart( carveSYMBOL_NAME, &data );
    for( ; (sn = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *sn );
        sn->next = SymbolNameMapIndex( sn->next );
        sn->name_type = SymbolMapIndex( sn->name_type );
        sn->name_syms = SymbolMapIndex( sn->name_syms );
        sn->name = NameMapIndex( sn->name );
        sn->containing = ScopeMapIndex( sn->containing );
    }
}

static void readSymbols( void )
{
    cv_index i;
    SYMBOL sym;
    auto cvinit_t data;

    CarveInitStart( carveSYMBOL, &data );
    for( ; (i = PCHReadCVIndex()) != CARVE_NULL_INDEX; ) {
        sym = CarveInitElement( &data, i );
        PCHReadVar( *sym );
        if( i == (cv_index)sym->next ) {
            // most symbols are not overloaded
            sym->next = sym;
        } else {
            sym->next = SymbolMapIndex( sym->next );
        }
        sym->thread = SymbolMapIndex( sym->thread );
        sym->sym_type = TypeMapIndex( sym->sym_type );
        sym->name = SymbolNameMapIndex( sym->name );
        sym->locn = SymTokenLocnMapIndex( sym->locn );
        switch( sym->id ) {
        case SC_NAMESPACE:
            sym->u.ns = NameSpaceMapIndex( sym->u.ns );
            break;
        case SC_CLASS_TEMPLATE:
            sym->u.tinfo = TemplateClassInfoMapIndex( sym->u.tinfo );
            break;
        case SC_FUNCTION_TEMPLATE:
        case SC_EXTERN_FUNCTION_TEMPLATE:
        case SC_STATIC_FUNCTION_TEMPLATE:
            sym->u.defn = TemplateFunctionInfoMapIndex( sym->u.defn );
            break;
        case SC_DEFAULT:
            sym->u.defarg_info = PTreeMapIndex( sym->u.defarg_info );
            break;
        case SC_VIRTUAL_FUNCTION:
        case SC_ADDRESS_ALIAS:
            sym->u.sym = SymbolMapIndex( sym->u.sym );
            break;
        case SC_ACCESS:
            sym->u.type = TypeMapIndex( sym->u.type );
            break;
        default:
            if( sym->flag & ( SF_ADDR_THUNK | SF_TEMPLATE_FN | SF_ALIAS ) ) {
                sym->u.sym = SymbolMapIndex( sym->u.sym );
            } else if( sym->flag & SF_CONSTANT_INT64 ) {
                sym->u.pval = ConstantPoolMapIndex( sym->u.pval );
            } else if( (sym->flag & SF_ANONYMOUS) && ( sym->id != SC_MEMBER ) ) {
                sym->u.sym = SymbolMapIndex( sym->u.sym );
            }
        }
    }
}

pch_status PCHReadScopes( void )
{
    uniqueNameSpaceName = NameMapIndex( PCHSetUInt( PCHReadUInt() ) );
    allNameSpaces = NameSpaceMapIndex( (NAME_SPACE *)PCHReadCVIndex() );
    SetCurrScope(ScopeMapIndex( (SCOPE)PCHReadCVIndex() ));
    SetFileScope(ScopeMapIndex( (SCOPE)PCHReadCVIndex() ));
    SetInternalScope(ScopeMapIndex( (SCOPE)PCHReadCVIndex() ));
    ChipBugSym = SymbolMapIndex( (SYMBOL)PCHReadCVIndex() );
    DFAbbrevSym = SymbolMapIndex( (SYMBOL)PCHReadCVIndex() );
    PCHDebugSym = SymbolMapIndex( (SYMBOL)PCHReadCVIndex() );
    readSymRegion();
    readUsingNS();
    readNameSpaces();
    readScopes();
    readSymbolNames();
    readSymbols();
    readMappingList();
    return( PCHCB_OK );
}

pch_status PCHInitScopes( boolean writing )
{
    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carveUSING_NS ) );
        PCHWriteCVIndex( CarveLastValidIndex( carveNAME_SPACE ) );
        PCHWriteCVIndex( CarveLastValidIndex( carveSCOPE ) );
        PCHWriteCVIndex( CarveLastValidIndex( carveSYMBOL_NAME ) );
        PCHWriteCVIndex( CarveLastValidIndex( carveSYMBOL ) );
    } else {
        carveUSING_NS = CarveRestart( carveUSING_NS );
        CarveMapOptimize( carveUSING_NS, PCHReadCVIndex() );
        carveNAME_SPACE = CarveRestart( carveNAME_SPACE );
        CarveMapOptimize( carveNAME_SPACE, PCHReadCVIndex() );
        carveSCOPE = CarveRestart( carveSCOPE );
        CarveMapOptimize( carveSCOPE, PCHReadCVIndex() );
        carveSYMBOL_NAME = CarveRestart( carveSYMBOL_NAME );
        CarveMapOptimize( carveSYMBOL_NAME, PCHReadCVIndex() );
        carveSYMBOL = CarveRestart( carveSYMBOL );
        CarveMapOptimize( carveSYMBOL, PCHReadCVIndex() );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniScopes( boolean writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveUSING_NS );
        CarveMapUnoptimize( carveNAME_SPACE );
        CarveMapUnoptimize( carveSCOPE );
        CarveMapUnoptimize( carveSYMBOL_NAME );
        CarveMapUnoptimize( carveSYMBOL );
    }
    return( PCHCB_OK );
}


/*
   Three functions to set the owner information for TEMPLATE_PARM scopes
*/


void ScopeSetParmClass( SCOPE parm_scope, TEMPLATE_INFO * info )
/**************************************************************/
{
    DbgAssert( parm_scope->id == SCOPE_TEMPLATE_PARM );
    parm_scope->owner.tinfo = info;
    parm_scope->s.fn_template = FALSE;
}


void ScopeSetParmFn( SCOPE parm_scope, FN_TEMPLATE *defn )
/*************************************************************/
{
    DbgAssert( parm_scope->id == SCOPE_TEMPLATE_PARM );
    parm_scope->owner.defn = defn;
    parm_scope->s.fn_template = TRUE;
}


void ScopeSetParmCopy( SCOPE parm_scope, SCOPE old_parm_scope )
/*************************************************************/
{
    DbgAssert( parm_scope->id == SCOPE_TEMPLATE_PARM );
    DbgAssert( old_parm_scope->id == SCOPE_TEMPLATE_PARM );
    parm_scope->owner.tinfo = old_parm_scope->owner.tinfo;
    parm_scope->s.fn_template = old_parm_scope->s.fn_template;
}
