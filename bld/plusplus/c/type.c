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
* Description:  C++ type system.
*
****************************************************************************/


#include "plusplus.h"

#include <stddef.h>
#include <stdarg.h>
#include <assert.h>

#include "memmgr.h"
#include "errdefns.h"
#include "ptree.h"
#include "cgfront.h"
#include "ring.h"
#include "carve.h"
#include "preproc.h"
#include "dbg.h"
#include "class.h"
#include "name.h"
#include "toggle.h"
#include "fnovload.h"
#include "fnbody.h"
#include "pragdefn.h"
#include "rewrite.h"
#include "codegen.h"
#include "defarg.h"
#include "vstk.h"
#include "pstk.h"
#include "template.h"
#include "initdefs.h"
#include "pcheader.h"
#include "stats.h"
#include "objmodel.h"
#include "cdopt.h"
#include "fmttype.h"
#include "dwarfdbg.h"
#include "rtti.h"

#define TYPE_HASH_MODULUS       (1<<5)  // modulus when type hashed
#define TYPE_HASH_MASK          (TYPE_HASH_MODULUS-1)// mask for above modulus
#define ARGS_HASH               8       // max arg.s for fn.s hashing
#define ARGS_MAX                16      // max arg.s for fn.s lists

#define zero_table( table ) memset( table, 0, sizeof( table ) )

TYPE TypeError;
TYPE TypeCache[ TYPC_LAST ];

static TYPE basicTypes[TYP_MAX];
static TYPE typeTable[TYP_MAX];
static TYPE fnHashTable[ARGS_HASH][TYPE_HASH_MODULUS];
static TYPE fnTable[ARGS_MAX-ARGS_HASH];
static TYPE pointerHashTable[TYPE_HASH_MODULUS];
static TYPE bitfieldHashTable[TYPE_HASH_MODULUS];
static TYPE arrayHashTable[TYPE_HASH_MODULUS];
static TYPE modifierHashTable[TYPE_HASH_MODULUS];
static TYPE* typeHashTables[TYP_MAX];
static TYPE uniqueTypes;
static AUX_INFO *cdeclPragma;
static type_flag defaultFunctionMemFlag;
static type_flag defaultDataMemFlag;
static unsigned typeHashCtr;

#define BLOCK_TYPE              64
#define BLOCK_DECL_SPEC         8
#define BLOCK_CLASSINFO         16
#define BLOCK_DECL_INFO         4

static carve_t carveTYPE;
static carve_t carveDECL_SPEC;
static carve_t carveCLASSINFO;
static carve_t carveDECL_INFO;

/* special member function checking return status */
enum {
    SM_RETURN_DECLARATOR        = 0x01, /* found return type declarators */
    SM_NOT_A_FUNCTION           = 0x02, /* found declarators before function */
    SM_CV_FUNCTION_ERROR        = 0x04, /* found () const/volatile use error */
                                        /* private flags (never returned) */
    SM_SAW_CV_FUNCTION          = 0x20, /* () const/volatile seen */
    SM_SAW_FUNCTION             = 0x40, /* function declarator seen */
    SM_SAW_DECLARATOR           = 0x80, /* non-function declarator seen */
    SM_NULL                     = 0x00
};

/* special id checking information */
enum {
    IDI_CLASS_TEMPLATE_MEMBER   = 0x01, /* id is a member of a class template */
    IDI_NULL                    = 0x00
};

enum {
    PA_NULL                     = 0x00,
    PA_FUNCTION                 = 0x01
};

enum {
    TB_BINDS                    = 0x01,
    TB_NEEDS_TRIVIAL            = 0x02,
    TB_NEEDS_DERIVED            = 0x04,
    TB_NULL                     = 0x00
};

enum {
    MTT_INLINE                  = 0x01,
    MTT_COPY_PRAGMA             = 0x02,
    MTT_REUSE_ARGLIST           = 0x04,
    MTT_NULL                    = 0x00
};

enum {
    DF_REUSE_ARGLIST            = 0x01,
    DF_NULL                     = 0x00
};

enum {
    DA_DEFARGS_PRESENT          = 0x01,
    DA_REWRITES                 = 0x02,
    DA_NULL                     = 0x00
};

typedef struct {
    PSTK_CTL    with_generic;
    PSTK_CTL    without_generic;
    PSTK_CTL    bindings;
    SCOPE       parm_scope;
    unsigned    num_explicit;
} type_bind_info;

// masking

// For TypeDefModifierRemoveOnly
#define MASK_TD_REMOVE_ONLY \
    ( 1 << TYP_MODIFIER ) | \
    ( 1 << TYP_TYPEDEF )

// For TypeDefModifierRemove
#define MASK_TD_REMOVE      \
    ( 1 << TYP_MODIFIER ) | \
    ( 1 << TYP_TYPEDEF )  | \
    ( 1 << TYP_ENUM )     | \
    ( 1 << TYP_BOOL )     | \
    ( 1 << TYP_CHAR )

#define TypeIdMasked( typ, mask ) ( (mask) & ( 1 << ( (typ)->id ) ) )

#define TypeStrip( type, mask ) if( type != NULL ) \
    for( ; TypeIdMasked( type, mask ); type = type->of );
#define TypeStripTdMod( type ) TypeStrip( type, MASK_TD_REMOVE_ONLY )
#define TypeStripTdModEnumChar( type ) TypeStrip( type, MASK_TD_REMOVE )



// extra reporting (debug only)

ExtraRptCtr( types_defined );
ExtraRptCtr( types_alloced );
ExtraRptCtr( ctr_dups );
ExtraRptCtr( ctr_dup_succ );
ExtraRptCtr( ctr_dup_succ_probes );
ExtraRptCtr( ctr_dup_fail );
ExtraRptCtr( ctr_dup_fail_probes );
ExtraRptCtr( ctr_dup_fns );
ExtraRptCtr( ctr_dup_fns_big );
ExtraRptCtr( ctr_lookup );
ExtraRptCtr( ctr_cg_dups );
ExtraRptCtr( ctr_cg_dups_fail );
ExtraRptTable( ctr_type_ids, TYP_MAX, 1 );
ExtraRptTable( ctr_fn_args, ARGS_MAX+1+1, 1 );


static DECL_INFO *makeDeclInfo( PTREE id )
{
    DECL_INFO *dinfo;

    dinfo = CarveAlloc( carveDECL_INFO );
    dinfo->next = NULL;
    dinfo->id = id;
    dinfo->scope = NULL;
    dinfo->friend_scope = NULL;
    dinfo->list = NULL;
    dinfo->parms = NULL;
    dinfo->sym = NULL;
    dinfo->generic_sym = NULL;
    dinfo->proto_sym = NULL;
    dinfo->name = NULL;
    dinfo->type = NULL;
    dinfo->defarg_expr = NULL;
    dinfo->body = NULL;
    dinfo->mem_init = NULL;
    dinfo->defarg_rewrite = NULL;
    dinfo->init_locn.src_file = NULL;
    dinfo->sym_used = FALSE;
    dinfo->friend_fn = FALSE;
    dinfo->fn_defn = FALSE;
    dinfo->template_member = FALSE;
    dinfo->has_dspec = FALSE;
    dinfo->has_defarg = FALSE;
    dinfo->explicit_parms = FALSE;
    dinfo->free = FALSE;
    return( dinfo );
}


TYPE GetBasicType( type_id id )
/*****************************/
/* return a pointer to one of the predefined "standard" types */
{
    DbgAssert( basicTypes[ id ] != NULL );
    return( basicTypes[ id ] );
}

static CLASSINFO *newINFO( void )
{
    CLASSINFO *info;

    info = CarveAlloc( carveCLASSINFO );
    info->bases = NULL;
    info->friends = NULL;
    info->name = NULL;
    info->cdopt_cache = NULL;
    info->size = 0;
    info->vsize = 0;
    info->refno = NULL_CGREFNO;
    info->dbg_no_vbases = 0;
    info->class_mod = NULL;
    /*
     *  Carl 12-Aug-2008.
     *  Added a copy of the class modifiers to the CLASS_INFO structure so that
     *  type modifiers can be added and retained from a class declaration.
     */
    info->fn_pragma = NULL;
    info->fn_flags = TF1_NULL;
    info->mod_flags = TF1_NULL;

    info->last_vfn = 0;
    info->last_vbase = 0;
    info->vf_offset = 0;
    info->vb_offset = 0;
    info->index = 0;
    info->max_align = TARGET_CHAR;

    info->has_def_opeq = FALSE;
    info->has_ctor = FALSE;
    info->has_def_ctor = FALSE;
    info->has_dtor = FALSE;
    info->has_pure = FALSE;
    info->has_vfptr = FALSE;
    info->has_vbptr = FALSE;
    info->has_data = FALSE;
    info->has_vfn = FALSE;
    info->has_vcdtor = FALSE;
    info->needs_ctor = FALSE;
    info->needs_dtor = FALSE;
    info->needs_vdtor = FALSE;
    info->needs_assign = FALSE;
    info->defined = FALSE;
    info->unnamed = FALSE;
    info->corrupted = FALSE;
    info->abstract = FALSE;
    info->abstract_OK = FALSE;
    info->const_copy = FALSE;
    info->const_assign = FALSE;
    info->const_ref = FALSE;
    info->anonymous = FALSE;

    info->ctor_defined = FALSE;
    info->copy_defined = FALSE;
    info->dtor_defined = FALSE;
    info->assign_defined = FALSE;
    info->ctor_gen = FALSE;
    info->copy_gen = FALSE;
    info->dtor_gen = FALSE;
    info->assign_gen = FALSE;
    info->ctor_user_code = FALSE;
    info->copy_user_code = FALSE;
    info->dtor_user_code = FALSE;
    info->assign_user_code = FALSE;
    info->ctor_user_code_checked = FALSE;
    info->copy_user_code_checked = FALSE;
    info->dtor_user_code_checked = FALSE;
    info->assign_user_code_checked = FALSE;
    info->opened = FALSE;
    info->zero_array = FALSE;
    info->passed_ref = FALSE;

    info->free = FALSE;
    info->lattice = FALSE;
    info->vftable_done = FALSE;
    info->vbtable_done = FALSE;
    info->has_udc = FALSE;
    info->common = FALSE;
    info->has_comp_info = FALSE;
    info->has_mutable = FALSE;
    info->empty = FALSE;
    info->has_fn = FALSE;

    return( info );
}

TYPE MakeType( type_id id )
/*************************/
{
    TYPE new_type;

    new_type = CarveAlloc( carveTYPE );
    ExtraRptIncrementCtr( types_alloced );
    ExtraRptIncrementCtr( types_defined );
    new_type->next = NULL;
    new_type->of = NULL;
    new_type->u.i.init1 = NULL;
    new_type->u.i.init2 = NULL;
    new_type->id = id;
    new_type->dbg.handle = typeHashCtr++;
    new_type->dbg.pch_handle = 0;
    new_type->flag = TF1_NULL;
    new_type->dbgflag = TF2_HASH;
    return( new_type );
}

TYPE MakeClassType( void )
/************************/
{
    TYPE new_type;

    new_type = MakeType( TYP_CLASS );
    new_type->u.c.info = newINFO();
    return( new_type );
}

static void typeFree( TYPE type )
{
    DbgVerify( ! ( type->dbgflag & TF2_DBG_IN_PCH ), "type was in a pre-compiled header!" );
    switch( type->id ) {
    case TYP_CLASS:
        DbgAssert( type->u.c.info->cdopt_cache == NULL );
        CarveFree( carveCLASSINFO, type->u.c.info );
        break;
    }
    CarveFree( carveTYPE, type );
    ExtraRptDecrementCtr( types_defined );
}

TYPE MakeArrayType( unsigned long size )
/**************************************/
{
    TYPE    new_type;

    new_type = MakeType( TYP_ARRAY );
    new_type->u.a.array_size = size;
    return( new_type );
}

TYPE MakeArrayOf( unsigned long size, TYPE base )
/***********************************************/
{
    TYPE array_type;

    array_type = MakeArrayType( size );
    return( MakeTypeOf( array_type, base ) );
}

// An expandable type will have the array size change and so cannot
// be placed within any type-compression lists.
//
TYPE MakeExpandableType( type_id base_id )
/****************************************/
{
    TYPE expands;

    expands = MakeArrayType( 1 );
    expands->of = GetBasicType( base_id );
    return expands;
}

TYPE MakeModifiedType( TYPE type, type_flag flag )
/************************************************/
{
    if( flag != TF1_NULL ) {
        type = MakeTypeOf( MakeFlagModifier( flag ), type );
    }
    return( type );
}

TYPE MakeCommonCodeData( TYPE type )
/**********************************/
{
    return( MakeModifiedType( type, TF1_COMMON ) );
}

TYPE MakeForceInMemory( TYPE type )
/*********************************/
{
    return( MakeModifiedType( type, TF1_IN_MEM ) );
}

TYPE MakeInternalType( target_size_t size )
/*****************************************/
{
    return( MakeArrayOf( size, GetBasicType( TYP_UCHAR ) ) );
}

static TYPE makeFullModifier( type_flag flag, void *base, AUX_INFO *pragma )
{
    TYPE new_type;

    new_type = MakeType( TYP_MODIFIER );
    new_type->flag = flag;
    new_type->u.m.base = base;
    new_type->u.m.pragma = pragma;
    return( new_type );
}

static TYPE dupModifier( TYPE type )
{
    return( makeFullModifier( type->flag, type->u.m.base, type->u.m.pragma ) );
}

TYPE MakeFlagModifier( type_flag flag )
/*************************************/
{
    return( makeFullModifier( flag, NULL, NULL ) );
}

TYPE MakeVolatileModifier( void )
/*******************************/
{
    return( MakeFlagModifier( TF1_VOLATILE ) );
}

TYPE MakeConstModifier( void )
/****************************/
{
    return( MakeFlagModifier( TF1_CONST ) );
}

TYPE MakeConstTypeOf( TYPE type )
/*******************************/
{
    return( MakeTypeOf( MakeConstModifier(), type ) );
}

TYPE MakeVolatileTypeOf( TYPE type )
/**********************************/
{
    return( MakeTypeOf( MakeVolatileModifier(), type ) );
}

static TYPE makeCompilerData( TYPE type, type_flag flags )
{
    if( defaultDataMemFlag & ( TF1_FAR | TF1_HUGE ) ) {
#if _CPU == 8086
        if(( flags & TF1_CONST ) == 0 ) {
            if( TargetSwitches & WINDOWS ) {
                /* R/W data must be near for 16-bit Windows */
                flags |= TF1_NEAR;
            } else {
                flags |= TF1_FAR;
            }
        } else {
            flags |= TF1_FAR;
        }
#else
        flags |= TF1_FAR;
#endif
    }
    return( MakeModifiedType( type, flags ) );
}

TYPE MakeCompilerReadWriteData( TYPE type )
/*****************************************/
{
    return( makeCompilerData( type, TF1_NULL ) );
}

TYPE MakeCompilerConstData( TYPE type )
/*************************************/
{
    return( makeCompilerData( type, TF1_CONST ) );
}

TYPE MakeCompilerConstCommonData( TYPE type )
/*******************************************/
{
    return( makeCompilerData( type, TF1_COMMON | TF1_CONST ) );
}

static TYPE makeCompilerReadWriteCommonData( TYPE type )
/******************************************************/
{
    return( makeCompilerData( type, TF1_COMMON ) );
}

static void verifyBasedRef( SEARCH_RESULT *result, SYMBOL sym )
{
    SCOPE found;

    found = result->scope;
    if( ScopeType( found, SCOPE_FILE ) ) {
        return;
    }
    if( ScopeType( found, SCOPE_BLOCK ) ) {
        return;
    }
    if( result->no_this ) {
        if( ! SymIsStaticDataMember( sym ) ) {
            CErr2p( ERR_CANNOT_ALWAYS_ACCESS, sym );
        }
    }
}

/*
  Based pointer examples:

  __segment seg_var;
  char __far *fp;

  (1) char __based(seg_var) *bp;        -- fetch segment from seg_var
      char __based((__segment)fp)) *ip; -- use of 'fp''s seg value
      char __based(fp) *pp;             -- add offset to fp to produce pointer
      char __based(void) *vp;           -- just an offset (based on nothing)
      char __based((__segment)__self) *sp; -- inherits base from expression

  (2) -- pre-defined segment names
      __based(__segname("_CODE"))       (code segment)
      __based(__segname("_CONST"))      (data segment)
      __based(__segname("_DATA"))       (data segment)
      __based(__segname("_STACK"))      (stack segment)
      __based(__segname("_MYSEG"))      (user-defined segment)
*/
TYPE MakeBasedModifier( type_flag mod, boolean seg_cast, PTREE base )
/*******************************************************************/
{
    int based_msg;
    SEARCH_RESULT *result;
    SYMBOL sym;
    SYMBOL_NAME sym_name;
    TYPE type;
    NAME id;
    void *base_expr;

    base_expr = NULL;
    switch( mod ) {
    case TF1_BASED_VOID:
    case TF1_BASED_SELF:
        break;
    case TF1_BASED_STRING:
        base_expr = base->u.string;
        PTreeFree( base );
        break;
    default:
        mod = TF1_BASED_VOID;
        id = base->u.id.name;
        PTreeFree( base );
        result = ScopeFindNaked( GetCurrScope(), id );
        if( result == NULL ) {
            CErr2p( ERR_UNDECLARED_SYM, id );
        } else {
            sym_name = result->sym_name;
            sym = sym_name->name_syms;
            if( ! SymIsAnError( sym ) ) {
                if( seg_cast ) {
                    CErr2p( ERR_BASED_EXTRACT_NO_SEG, sym );
                } else {
                    type = sym->sym_type;
                    if( IntegralType( type ) != NULL ) {
                        mod = TF1_BASED_FETCH;
                        base_expr = sym;
                    } else {
                        type = PointerType( type );
                        based_msg = ERR_BASED_FETCH_NO_SEG;
                        if( type != NULL ) {
                            if( type->id != TYP_POINTER ) {
                                type = NULL;
                            } else if( type->flag & TF1_REFERENCE ) {
                                type = NULL;
                            } else if( BasedType( type->of ) != NULL ) {
                                based_msg = ERR_BASED_ON_BASED;
                                type = NULL;
                            }
                        }
                        if( type == NULL ) {
                            CErr2p( based_msg, sym );
                        } else {
                            mod = TF1_BASED_ADD;
                            base_expr = sym;
                        }
                    }
                    if( base_expr != NULL ) {
                        if( result->scope != GetCurrScope() ) {
                            verifyBasedRef( result, sym );
                        }
                    }
                }
                if( base_expr != NULL ) {
                    if( ScopeCheckSymbol( result, sym ) ) {
                        mod = TF1_BASED_VOID;
                        base_expr = NULL;
                    }
                }
            }
            ScopeFreeResult( result );
        }
    }
    return( makeFullModifier( mod, base_expr, NULL ) );
}

TYPE MakeBasedModifierOf(       // MAKE BASED MODIFIER FOR A TYPE
    TYPE type,                  // - basic type
    type_flag flags,            // - modifier flags
    void *baser )               // - baser
{
    if( flags == TF1_NULL ) {
        return( type );
    }
    return MakeTypeOf( makeFullModifier( flags, baser, NULL ), type );
}

static type_flag convertCVSpec( specifier_t spec )
{
    type_flag flag;

    flag = TF1_NULL;
    if( spec & STY_CONST ) {
        flag |= TF1_CONST;
    }
    if( spec & STY_VOLATILE ) {
        flag |= TF1_VOLATILE;
    }
    if( spec & STY_UNALIGNED ) {
        flag |= TF1_UNALIGNED;
    }
    return( flag );
}

static TYPE makeAnyPointer( type_id id, specifier_t cv_flags )
{
    TYPE ptype;
    type_flag cvflag;

    ptype = MakeType( id );
    cvflag = convertCVSpec( cv_flags );
    if( cvflag ) {
        /* build this backwards, since later processing reverses the list */
        ptype->of = MakeFlagModifier( cvflag );
    }
    return( ptype );
}

TYPE MakePointerType( type_flag ref_flag, specifier_t cv_flags )
/**************************************************************/
{
    TYPE ptype;

    ptype = makeAnyPointer( TYP_POINTER, cv_flags );
    ptype->flag |= ref_flag;
    return( ptype );
}

static TYPE makeMemberPointerType( TYPE class_type, specifier_t cv_flags )
/************************************************************************/
{
    TYPE ptype;

    ptype = makeAnyPointer( TYP_MEMBER_POINTER, cv_flags );
    ptype->u.mp.host = TypedefRemove( class_type );
    return( ptype );
}

static TYPE extractMemberType( PTREE tree )
{
    /* we have:

                        CO_INDIRECT
                            |
                        CO_COLON_COLON
                        /       \
                                <id>
    */
    TYPE type;

    type = tree->u.subtree[0]->u.subtree[1]->type;
    PTreeFreeSubtrees( tree );
    return( type );
}

TYPE MakeMemberPointer( PTREE class_tree, specifier_t cv_flags )
/**************************************************************/
{
    TYPE type;

    type = extractMemberType( class_tree );
    if( type != NULL ) {
        type = makeMemberPointerType( type, cv_flags );
    }
    return( type );
}

TYPE MakeSeg16Pointer( specifier_t cv_flags )
/*******************************************/
{
    TYPE ptr_type;
    TYPE mod_type;

    ptr_type = makeAnyPointer( TYP_POINTER, cv_flags );
    mod_type = MakeFlagModifier( TF1_SET_FAR16 );
    mod_type->of = ptr_type;
    return( mod_type );
}

TYPE MakeMemberPointerTo( TYPE class_type, TYPE base_type )
/*********************************************************/
{
    TYPE mptr_type;
    TYPE unmod_type;
    type_flag flags;
    void *base;

    class_type = StructType( class_type );
    mptr_type = makeMemberPointerType( class_type, STY_NULL );
    base_type = TypeReferenced( base_type );
    unmod_type = TypeModExtract( base_type, &flags, &base, TC1_NOT_ENUM_CHAR );
    if( flags & TF1_MPTR_REMOVE ) {
        flags &= ~TF1_MPTR_REMOVE;
        base_type = MakeBasedModifierOf( unmod_type, flags, base );
    }
    return MakeTypeOf( mptr_type, base_type );
}

static boolean newTypeIllegal( TYPE type )
{
    boolean error_occurred;
    TYPE test_type;
    type_flag flags;

    error_occurred = FALSE;
    test_type = TypeModFlags( type, &flags );
    switch( test_type->id ) {
    case TYP_FUNCTION:
        CErr1( ERR_NEW_TYPE_FUNCTION );
        error_occurred = TRUE;
        break;
    case TYP_VOID:
        CErr1( ERR_NEW_TYPE_VOID );
        error_occurred = TRUE;
        break;
    }
    if( flags & TF1_CV_MASK ) {
        CErr1( WARN_NEW_TYPE_CONST_VOLATILE );
    }
    return( error_occurred );
}

void CheckNewModifier( type_flag flags )
/**************************************/
{
    if( flags & ~TF1_MEM_MODEL ) {
        CErr1( ERR_INVALID_NEW_MODIFIER );
    } else {
        flags &= TF1_MEM_MODEL;
        if( flags != defaultDataMemFlag ) {
            CErr1( ERR_INVALID_NEW_MODIFIER );
        }
    }
}

PTREE MakeNewExpr( PTREE gbl, PTREE placement, DECL_INFO *dinfo, PTREE init )
/***************************************************************************/
{
    PTREE nelem_expr;
    PTREE new_parms1;
    PTREE new_parms2;
    PTREE new_expr;
    PTREE new_type;
    CGOP new_op;
    TYPE type;

    new_op = CO_NEW;
    if( gbl != NULL ) {
        if( gbl->u.subtree[0] != NULL ) {
            new_op = CO_NEW_G;
        }
    }
    nelem_expr = dinfo->defarg_expr;
    if( nelem_expr != NULL ) {
        // detach from 'dinfo'
        dinfo->defarg_expr = NULL;
    }
    type = dinfo->type;
    FreeDeclInfo( dinfo );
    if( newTypeIllegal( type ) ) {
        PTreeFreeSubtrees( init );
        new_expr = PTreeErrorNode( placement );
    } else {
        new_type = PTreeType( type );
        new_parms2 = PTreeBinary( CO_NEW_PARMS2, nelem_expr, init );
        new_parms1 = PTreeBinary( CO_NEW_PARMS1, placement, new_parms2 );
        new_expr = PTreeBinary( new_op, new_type, new_parms1 );
    }
    return( new_expr );
}

DECL_INFO *MakeNewTypeId( PTREE type_id )
/***************************************/
{
    TYPE new_type;
    DECL_INFO *dinfo;

    new_type = type_id->type;
    PTreeFree( type_id );
    dinfo = makeDeclInfo( NULL );
    dinfo->type = new_type;
    return( dinfo );
}

static boolean dupCompare( TYPE test1, TYPE test2 )
{
    arg_list *args1;
    arg_list *args2;
    TYPE *p1;
    TYPE *p2;
    unsigned i;

    /* check TypedefReset if TYP_TYPEDEFs are ever checked */
    /* check typesBind/performBinding (in this module) if comparisons change */
    /* check TYPECOMP.C if comparisons change */
    switch( test1->id ) {
    case TYP_BITFIELD:
        if( test1->u.b.field_start != test2->u.b.field_start ) {
            return( FALSE );
        }
        if( test1->u.b.field_width != test2->u.b.field_width ) {
            return( FALSE );
        }
        break;
    case TYP_FUNCTION:
        if( test1->u.f.pragma != test2->u.f.pragma ) {
            return( FALSE );
        }
        args1 = test1->u.f.args;
        args2 = test2->u.f.args;
        if( args1 == args2 ) {
            return( TRUE );
        }
        if( args1->num_args != args2->num_args ) {
            return( FALSE );
        }
        if( args1->qualifier != args2->qualifier ) {
            return( FALSE );
        }
        p1 = args1->type_list;
        p2 = args2->type_list;
        for( i = args1->num_args; i != 0; --i ) {
            if( *p1 != *p2 ) {
                return( FALSE );
            }
            ++p1;
            ++p2;
        }
        p1 = args1->except_spec;
        p2 = args2->except_spec;
        if( p1 != p2 ) {
            TYPE tp1, tp2;
            if( p1 == NULL || p2 == NULL ) {
                return( FALSE );
            }
            for(;;) {
                tp1 = *p1;
                if( tp1 == NULL ) return FALSE;
                tp2 = *p2;
                if( tp2 == NULL ) return FALSE;
                if( tp1 != tp2 ) return FALSE;
                ++p1;
                ++p2;
            }
        }
        break;
    case TYP_ARRAY:
        if( test1->u.a.array_size != test2->u.a.array_size ) {
            return( FALSE );
        }
        break;
    case TYP_MODIFIER:
        if( test1->u.m.base != test2->u.m.base ) {
            return( FALSE );
        }
        if( test1->u.m.pragma != test2->u.m.pragma ) {
            return( FALSE );
        }
        break;
    case TYP_MEMBER_POINTER:
        if( test1->u.mp.host != test2->u.mp.host ) {
            return( FALSE );
        }
        break;
    case TYP_GENERIC:
        if( test1->u.g.index != test2->u.g.index ) {
            return( FALSE );
        }
        break;
    case TYP_ERROR:
        if( test1->u.e.fmt != test2->u.e.fmt ) {
            return( FALSE );
        }
        break;
    case TYP_POINTER:
        break;
    default:
#ifndef NDEBUG
        CFatal( "unknown type being compared" );
#else
        return( FALSE );
#endif
    }
    return( TRUE );
}

void TypedefReset( SYMBOL sym, TYPE type )
/****************************************/
{
    TYPE typedef_type;

    /* we are tweaking the type of a typedef that we want permanently changed */
    typedef_type = sym->sym_type;
    DbgAssert( typedef_type->id == TYP_TYPEDEF );
    /* typedef types are not dupCompare()'d */
    typedef_type->of = type;
}

void CheckUniqueType( TYPE newtype )
/**********************************/
{
    #ifdef XTRA_RPT
        ExtraRptTabIncr( ctr_type_ids, newtype->id, 0 );
        ExtraRptTabIncr( ctr_type_ids, TYP_MAX-1, 0 );
        if( newtype->id == TYP_FUNCTION ) {
            unsigned num_args = newtype->u.f.args->num_args;
            if( num_args > ARGS_MAX ) {
                num_args = ARGS_MAX;
            }
            ExtraRptTabIncr( ctr_fn_args, num_args, 0 );
            ExtraRptTabIncr( ctr_fn_args, ARGS_MAX+1, 0 );
        }
    #endif
    RingPush( &uniqueTypes, newtype );
}


static TYPE typeDuplicated(     // GET DUPLICATED TYPE
    TYPE newtype,               // - new type
    TYPE* head )                // - list head
{
    TYPE check;
    TYPE newtype_of;
    TYPE prev;
    type_flag newtype_flag;

    prev = NULL;
    newtype_of = newtype->of;
    newtype_flag = newtype->flag;
    ExtraRptZeroCtr( ctr_lookup );
    RingIterBeg( *head, check ) {
        ExtraRptIncrementCtr( ctr_lookup );
        if( check->of == newtype_of ) {
            if( check->flag == newtype_flag ) {
                if( dupCompare( check, newtype ) ) {
                    ExtraRptAddtoCtr( ctr_dup_succ_probes, ctr_lookup );
                    ExtraRptIncrementCtr( ctr_dup_succ );
                    typeFree( newtype );
                    newtype = RingPromote( head, check, prev );
                    return newtype;
                }
            }
        }
        prev = check;
    } RingIterEnd( check )
    #ifdef XTRA_RPT
        if( CompFlags.codegen_active ) {
            ExtraRptIncrementCtr( ctr_cg_dups_fail );
        }
        ExtraRptTabIncr( ctr_type_ids, newtype->id, 0 );
        ExtraRptTabIncr( ctr_type_ids, TYP_MAX-1, 0 );
        if( newtype->id == TYP_FUNCTION ) {
            unsigned num_args = newtype->u.f.args->num_args;
            if( num_args > ARGS_MAX ) {
                num_args = ARGS_MAX;
            }
            ExtraRptTabIncr( ctr_fn_args, num_args, 0 );
            ExtraRptTabIncr( ctr_fn_args, ARGS_MAX+1, 0 );
        }
    #endif
    ExtraRptAddtoCtr( ctr_dup_fail_probes, ctr_lookup );
    ExtraRptIncrementCtr( ctr_dup_fail );
    newtype = RingPush( head, newtype );
    return newtype;
}

#define typeHashVal( t ) ((t)->of->dbg.handle & TYPE_HASH_MASK )

TYPE CheckDupType( TYPE newtype )
/*******************************/
{
    unsigned num_args;
    type_id id;
    TYPE *head;

    DbgAssert( newtype->next == NULL );
    id = newtype->id;
    ExtraRptIncrementCtr( ctr_dups );
    #ifdef XTRA_RPT
        if( CompFlags.codegen_active ) {
            ExtraRptIncrementCtr( ctr_cg_dups );
        }
    #endif
#ifndef NDEBUG
    if( id == TYP_MODIFIER ) {
        if( newtype->flag == TF1_NULL ) {
            if( newtype->u.m.base == NULL ) {
                if( newtype->u.m.pragma == NULL ) {
                    CFatal( "empty modifier created" );
                }
            }
        }
    }
#endif
    head = typeHashTables[ id ];
    if( head == NULL ) {
        switch( id ) {
        case TYP_FUNCTION:
            DbgVerify( newtype->of != NULL
                     , "type: attempt to set base type to NULL" );
            num_args = newtype->u.f.args->num_args;
            if( num_args < ARGS_HASH ) {
                ExtraRptIncrementCtr( ctr_dup_fns );
                head = &fnHashTable[ num_args ][ typeHashVal( newtype ) ];
            } else if( num_args < ARGS_MAX ) {
                ExtraRptIncrementCtr( ctr_dup_fns );
                head = &fnTable[ num_args - ARGS_HASH ];
            } else {
                ExtraRptIncrementCtr( ctr_dup_fns_big );
                head = &typeTable[ id ];
            }
            newtype = typeDuplicated( newtype, head );
            break;
        case TYP_MEMBER_POINTER:
        case TYP_GENERIC:
            newtype = typeDuplicated( newtype, &typeTable[ id ] );
            break;
        default:
            #ifdef XTRA_RPT
                if( CompFlags.codegen_active ) {
                    ExtraRptIncrementCtr( ctr_cg_dups_fail );
                }
                ExtraRptTabIncr( ctr_type_ids, id, 0 );
                ExtraRptTabIncr( ctr_type_ids, TYP_MAX-1, 0 );
                if( id == TYP_FUNCTION ) {
                    unsigned num_args = newtype->u.f.args->num_args;
                    if( num_args > ARGS_MAX ) {
                        num_args = ARGS_MAX;
                    }
                    ExtraRptTabIncr( ctr_fn_args, num_args, 0 );
                    ExtraRptTabIncr( ctr_fn_args, ARGS_MAX+1, 0 );
                }
            #endif
            newtype = RingPush( &typeTable[ id ], newtype );
        }
    } else {
        if( head == arrayHashTable ) {
            if( newtype->u.a.array_size == 0 ) {
                /* lets us change the size without worrying about duplicates */
                return( newtype );
            }
        }
        DbgVerify( newtype->of != NULL
                 , "type: attempt to set base type to NULL" );
        newtype = typeDuplicated( newtype
                                , &head[ typeHashVal( newtype ) ] );
    }
    return newtype;
}


static void traverseTypeRing(   // TRAVERSE RING OF TYPES
    TYPE ring,                  // - the ring
    void (*rtn)( TYPE, void* ), // - the routine
    void* data )                // - data for the routine
{
    TYPE curr;

    RingIterBeg( ring, curr ) {
        (*rtn)( curr, data );
    } RingIterEnd( curr )
}


static void traverseTypeHashed( // TRAVERSE VECTOR OF HASHED TYPE RINGS
    TYPE* vector,               // - the vector
    void (*rtn)( TYPE, void* ), // - the routine
    void* data )                // - data for the routine
{
    int idx;                    // - index

    for( idx = 0; idx < TYPE_HASH_MODULUS; ++ idx ) {
        traverseTypeRing( *vector, rtn, data );
        ++ vector;
    }
}


void TypeTraverse( type_id id, void (*rtn)( TYPE, void *), void *data )
/*********************************************************************/
{
    unsigned num_args;

    switch( id ) {
      case TYP_POINTER :
        traverseTypeHashed( pointerHashTable, rtn, data );
        break;
      case TYP_BITFIELD :
        traverseTypeHashed( bitfieldHashTable, rtn, data );
        break;
      case TYP_ARRAY :
        traverseTypeHashed( arrayHashTable, rtn, data );
        break;
      case TYP_MODIFIER :
        traverseTypeHashed( modifierHashTable, rtn, data );
        break;
      case TYP_FUNCTION :
        traverseTypeRing( typeTable[id], rtn, data );
        for( num_args = 0; num_args < ARGS_HASH; ++num_args ) {
            traverseTypeHashed( fnHashTable[ num_args ], rtn, data );
        }
        for( ; num_args < ARGS_MAX; ++num_args ) {
            traverseTypeRing( fnTable[ num_args - ARGS_HASH ], rtn, data );
        }
        break;
      default :
        traverseTypeRing( typeTable[id], rtn, data );
        break;
    }
}

TYPE TypeUserConversion( DECL_SPEC *dspec, DECL_INFO *dinfo )
/***********************************************************/
{
    TYPE cnv_id_type;

    if( dinfo == NULL ) {
        dinfo = makeDeclInfo( NULL );
    }
    dinfo = FinishDeclarator( dspec, dinfo );
    cnv_id_type = dinfo->type;
    FreeDeclInfo( dinfo );
    return( cnv_id_type );
}

static DECL_INFO *prependTypeToDeclarator( DECL_INFO *dinfo, TYPE type )
{
    TYPE end;

    if( type != NULL ) {
        end = type;
        for(;;) {
            if( end->of == NULL ) break;
            end = end->of;
        }
        end->of = dinfo->list;
        dinfo->list = type;
    }
    return( dinfo );
}

DECL_INFO *MakeNewDeclarator( DECL_SPEC *dspec,DECL_INFO *ptrs,DECL_INFO*arrays)
/******************************************************************************/
{
    if( arrays == NULL ) {
        arrays = makeDeclInfo( NULL );
    }
    if( ptrs != NULL ) {
        arrays = prependTypeToDeclarator( arrays, ptrs->list );
        /* we've detached the list of types */
        ptrs->list = NULL;
        FreeDeclInfo( ptrs );
    }
    arrays = FinishDeclarator( dspec, arrays );
#ifndef NDEBUG
    if( PragDbgToggle.dump_types ) {
        DumpFullType( arrays->type );
    }
#endif
    return( arrays );
}

DECL_INFO *MakeNewPointer( specifier_t cv_spec, DECL_INFO *dinfo, PTREE member )
/******************************************************************************/
{
    TYPE ptr_type;
    TYPE class_type;

    if( dinfo == NULL ) {
        dinfo = makeDeclInfo( NULL );
    }
    if( member != NULL ) {
        class_type = extractMemberType( member );
        ptr_type = makeMemberPointerType( class_type, cv_spec );
    } else {
        ptr_type = MakePointerType( TF1_NULL, cv_spec );
    }
    return( prependTypeToDeclarator( dinfo, ptr_type ) );
}

DECL_INFO *MakeNewDynamicArray( PTREE nelem )
/*******************************************/
{
    DECL_INFO *dinfo;

    dinfo = makeDeclInfo( NULL );
    dinfo->defarg_expr = nelem;
    return( dinfo );
}

DECL_INFO *AddArrayDeclarator( DECL_INFO *dinfo, PTREE size )
/***********************************************************/
{
    TYPE array_type;

    if( size != NULL ) {
        CheckDimension( size );
        array_type = MakeArrayType( size->u.uint_constant );
        PTreeFree( size );
    } else {
        array_type = MakeArrayType( 0 );
    }
    return( prependTypeToDeclarator( dinfo, array_type ) );
}

static void freeDeclList( DECL_INFO **arg_decls )
{
    DECL_INFO *curr;

    RingIterBegSafe( *arg_decls, curr ) {
        FreeDeclInfo( curr );
    } RingIterEndSafe( curr )
    *arg_decls = NULL;
}

static void stripDefArg( DECL_INFO *dinfo )
{
    PTREE expr;
    REWRITE *rewrite;

    if( dinfo->has_defarg ) {
        expr = dinfo->defarg_expr;
        if( expr != NULL ) {
            PTreeFreeSubtrees( expr );
            dinfo->defarg_expr = NULL;
        }
        rewrite = dinfo->defarg_rewrite;
        if( rewrite != NULL ) {
            RewriteFree( rewrite );
            dinfo->defarg_rewrite = NULL;
        }
        dinfo->has_defarg = FALSE;
    }
}

static void removeDefaultArgs( DECL_INFO *dinfo )
{
    DECL_INFO *curr;

    RingIterBeg( dinfo, curr ) {
        stripDefArg( curr );
    } RingIterEnd( curr )
}

static boolean cantHaveDefaultArgs( int err_msg, DECL_INFO *dinfo )
{
    DECL_INFO *curr;

    RingIterBeg( dinfo, curr ) {
        if( curr->has_defarg ) {
            CErr1( err_msg );
            return( TRUE );
        }
    } RingIterEnd( curr )
    return( FALSE );
}

void ForceNoDefaultArgs( DECL_INFO *dinfo, int err_msg )
/******************************************************/
{
    DECL_INFO *parms;

    parms = dinfo->parms;
    if( cantHaveDefaultArgs( err_msg, parms )){
        removeDefaultArgs( parms );
    }
}

void FreeTemplateArgs( DECL_INFO * dinfo)
{
    freeDeclList( &dinfo );
}

void FreeArgs( DECL_INFO *dinfo )
{
    cantHaveDefaultArgs( ERR_DEFAULT_ARGS_IN_A_TYPE, dinfo );
    freeDeclList( &dinfo );
}

static boolean isVoid( TYPE type )
{
    type = TypedefRemove( type );
    if( type->id == TYP_VOID ) {
        return( TRUE );
    }
    return( FALSE );
}

static TYPE *makeExceptionList( PTREE tree )
{
    TYPE any_exception;
    TYPE *vector;
    TYPE *curr_type;
    PTREE curr;
    unsigned count;

    any_exception = GetBasicType( TYP_DOT_DOT_DOT );
    count = 0;
    for( curr = tree; curr != NULL; curr = curr->u.type.next ) {
        if( curr->type == any_exception ) {
            return( NULL );
        }
        ++count;
    }
    vector = CPermAlloc( ( count + 1 ) * sizeof( TYPE ) );
    curr_type = vector;
    for( curr = tree; curr != NULL; curr = curr->u.type.next ) {
        *curr_type = curr->type;
        ++curr_type;
    }
    *curr_type = NULL;
    return( vector );
}

static void abstractDiagnose( unsigned msg, TYPE type )
{
    type = ArrayBaseType( type );
    CErr2p( msg, type );
    ScopeNotePureFunctions( type );
}

TYPE MakeFnType( DECL_INFO **arg_decls, specifier_t cv, PTREE exception_spec )
/****************************************************************************/
{
    unsigned num_args;
    unsigned void_args;
    TYPE type;
    TYPE fntype;
    TYPE *currarg;
    DECL_INFO *curr;
    arg_list *args;
    PTREE next;

    num_args = 0;
    void_args = 0;
    RingIterBeg( *arg_decls, curr ) {
        type = curr->type;
        if( isVoid( type ) ) {
            ++void_args;
        } else if( TypeAbstract( type ) ) {
            abstractDiagnose( ERR_ARG_ABSTRACT_TYPE, type );
        }
        num_args++;
    } RingIterEnd( curr )
    if( num_args > 0 ) {
        if( void_args > 0 ) {
            if( num_args > 1 || void_args > 1 ) {
                CErr1( ERR_NO_VOID_PARMS );
            }
            freeDeclList( arg_decls );
            num_args = 0;
        }
    }
    args = AllocArgListPerm( num_args );
    // if cv includes more than const or volatile, fix default op= qualifiers
    args->qualifier = convertCVSpec( cv );
    args->except_spec = makeExceptionList( exception_spec );
    for( ; exception_spec != NULL; exception_spec = next ) {
        next = exception_spec->u.type.next;
        PTreeFreeSubtrees( exception_spec );
    }
    currarg = args->type_list;
    RingIterBeg( *arg_decls, curr ) {
        *currarg = curr->type;
        TypeStripTdMod( ( *currarg ) );
        currarg++;
    } RingIterEnd( curr )
    fntype = MakeType( TYP_FUNCTION );
    fntype->u.f.args = args;
    return( fntype );
}

static TYPE buildFnType( TYPE ret_type, va_list count_args, va_list use_args )
{
    TYPE arg_type;
    TYPE fn_type;
    TYPE *curr_arg;
    arg_list *args;
    unsigned arg_count;

    arg_count = 0;
    for(;;) {
        arg_type = va_arg( count_args, TYPE );
        if( arg_type == NULL ) break;
        ++arg_count;
    }
    va_end( count_args );
    args = AllocArgListPerm( arg_count );
    curr_arg = args->type_list;
    for(;;) {
        arg_type = va_arg( use_args, TYPE );
        if( arg_type == NULL ) break;
        *curr_arg = arg_type;
        ++curr_arg;
    }
    va_end( use_args );
    fn_type = MakeType( TYP_FUNCTION );
    fn_type->of = ret_type;
    fn_type->flag |= TF1_PLUSPLUS;
    fn_type->u.f.args = args;

    return( fn_type );
}

TYPE MakeSimpleFunction( TYPE return_type, ... )
/**********************************************/
{
    TYPE fn_type;
    va_list args1;
    va_list args2;

    va_start( args1, return_type );
    va_start( args2, return_type );
    fn_type = buildFnType( return_type, args1, args2 );
    return( CheckDupType( fn_type ) );
}

TYPE MakeModifiableFunction( TYPE return_type, ... )
/**************************************************/
{
    TYPE fn_type;
    va_list args1;
    va_list args2;

    va_start( args1, return_type );
    va_start( args2, return_type );
    fn_type = buildFnType( return_type, args1, args2 );
    return( fn_type );
}


TYPE MakeTypeOf(                // MAKE UNIQUE TYPE OF
    TYPE new_type,              // - a new type
    TYPE of_type )              // - the (already unique) "of" type
{
    new_type->of = of_type;
    return( CheckDupType( new_type ) );
}

#ifndef NDEBUG

typedef struct {
    unsigned long sum;
    unsigned long max;
    unsigned long chains;
} DUMP_STATS;

static void dumpStatsZero(      // ZERO DUMP STATISTICS
    DUMP_STATS *stats )
{
    stats->sum = 0;
    stats->max = 0;
    stats->chains = 0;
}

static void dumpStats(          // PRINT STATISTICS
    DUMP_STATS *stats )
{
    printf( "total types:   %6u\n", stats->sum );
    printf( "longest chain: %6u\n", stats->max );
    printf( "average chain: %6u\n", stats->sum / stats->chains );
    putchar( '\n' );
}

static TYPE* dumpTypeRing(      // DUMP A TYPE RING
    TYPE* ring,
    DUMP_STATS *stats )
{
    long length = 0;
    TYPE curr;

    ++ stats->chains;
    RingIterBeg( *ring, curr ) {
        DumpType( curr );
        ++length;
    } RingIterEnd( curr )
    stats->sum += length;
    if( length > stats->max ) {
        stats->max = length;
    }
    putchar( '\n' );
    return ring + 1;
}

static TYPE* dumpTypeVector(    // DUMP VECTOR OF TYPE RINGS
    TYPE* vector,
    DUMP_STATS *stats,
    const char* fmt )
{
    int j;
    for( j = 0; j < TYPE_HASH_MODULUS; ++j ) {
        printf( fmt, j );
        vector = dumpTypeRing( vector, stats );
        putchar( '\n' );
    }
    return vector;
}

void dumpTypeTables( void )
{
    DUMP_STATS stats;
    int i;
    TYPE *head;

    dumpStatsZero( &stats );
    head = &typeTable[0];
    for( i = 0; i < TYP_MAX; ++i ) {
        switch( i ) {
          case TYP_POINTER :
            dumpTypeVector( pointerHashTable
                          , &stats
                          , "pointer types %3d\n" );
            break;
          case TYP_BITFIELD :
            dumpTypeVector( bitfieldHashTable
                          , &stats
                          , "bitfield types %3d\n" );
            break;
          case TYP_ARRAY :
            dumpTypeVector( arrayHashTable
                          , &stats
                          , "array types %3d\n" );
            break;
          case TYP_MODIFIER :
            dumpTypeVector( modifierHashTable
                          , &stats
                          , "modifier types %3d\n" );
            break;
          default :
            printf( "type id %3d:\n", i );
            head = dumpTypeRing( head, &stats );
            break;
        }
    }
    dumpStats( &stats );
    dumpStatsZero( &stats );
    head = &fnHashTable[0][0];
    for( i = 0; i < ARGS_HASH; ++i ) {
        char buf[32];
        sprintf( buf, "args %3d %%3d:\n", i );
        head = dumpTypeVector( head, &stats, buf );
    }
    head = &fnTable[0];
    for( i = ARGS_HASH; i < ARGS_MAX; ++i ) {
        printf( "args %3d:\n", i );
        head = dumpTypeRing( head, &stats );
    }
    dumpStatsZero( &stats );
}
#endif

TYPE PTypeListOfTypes( type_id id )
/*********************************/
{
    /* won't work for optimized search types (i.e., TYP_FUNCTION) */
    return( typeTable[id] );
}

void PTypeSignedChar( void )
/**************************/
{
    basicTypes[ TYP_CHAR ]->of = basicTypes[ TYP_SCHAR ];
}

static void initCacheAfterOptions( void )
{
    TYPE void_type;
    TYPE void_handler_fun_of_void;

    void_type = GetBasicType( TYP_VOID );
    void_handler_fun_of_void = MakeModifiableFunction( void_type, NULL );
    void_handler_fun_of_void->u.f.pragma = GetTargetHandlerPragma();
    void_handler_fun_of_void = CheckDupType( void_handler_fun_of_void );
    TypeSetCache( TYPC_VOID_HANDLER_FUN_OF_VOID, void_handler_fun_of_void );
}

void PTypeCheckInit( void )
/*************************/
{
    TYPE basic_type;
    unsigned index;

    // code that must execute after the command line options have been parsed
    for( index = TYP_MIN; index < TYP_MAX; index++ ) {
        basic_type = basicTypes[ index ];
        if( basic_type != NULL ) {
            basicTypes[ index ] = CheckDupType( basic_type );
        }
    }
    initCacheAfterOptions();
    if( IsBigCode() ) {
        defaultFunctionMemFlag = TF1_FAR;
    } else {
        defaultFunctionMemFlag = TF1_NEAR;
    }
    if( IsHugeData() ) {
        defaultDataMemFlag = TF1_HUGE;
    } else if( IsBigData() ) {
        defaultDataMemFlag = TF1_FAR;
    } else {
        defaultDataMemFlag = TF1_NEAR;
    }
}

static type_id findTypeId( scalar_t scalar )
/******************************************/
{
    switch( scalar ) {
    case STM_BOOL:
        return( TYP_BOOL );
    case STM_CHAR:
        return( TYP_CHAR );
    case STM_WCHAR:
        return( TYP_WCHAR );
    case STM_CHAR | STM_SIGNED:
        return( TYP_SCHAR );
    case STM_CHAR | STM_UNSIGNED:
        return( TYP_UCHAR );
    case STM_SHORT:
        return( TYP_SSHORT );
    case STM_SHORT | STM_INT:
        return( TYP_SSHORT );
    case STM_SHORT | STM_SIGNED:
        return( TYP_SSHORT );
    case STM_SHORT | STM_SIGNED | STM_INT:
        return( TYP_SSHORT );
    case STM_SHORT | STM_UNSIGNED:
        return( TYP_USHORT );
    case STM_SHORT | STM_UNSIGNED | STM_INT:
        return( TYP_USHORT );
    case STM_SIGNED:
        return( TYP_SINT );
    case STM_INT:
        return( TYP_SINT );
    case STM_INT | STM_SIGNED:
        return( TYP_SINT );
    case STM_INT | STM_UNSIGNED:
        return( TYP_UINT );
    case STM_UNSIGNED:
        return( TYP_UINT );
    case STM_LONG:
        return( TYP_SLONG );
    case STM_LONG | STM_SIGNED:
        return( TYP_SLONG );
    case STM_LONG | STM_UNSIGNED:
        return( TYP_ULONG );
    case STM_LONG | STM_INT:
        return( TYP_SLONG );
    case STM_LONG | STM_INT | STM_SIGNED:
        return( TYP_SLONG );
    case STM_LONG | STM_INT | STM_UNSIGNED:
        return( TYP_ULONG );
    case STM_INT64:
        return( TYP_SLONG64 );
    case STM_INT64 | STM_SIGNED:
        return( TYP_SLONG64 );
    case STM_INT64 | STM_UNSIGNED:
        return( TYP_ULONG64 );
    case STM_INT64 | STM_INT:
        return( TYP_SLONG64 );
    case STM_INT64 | STM_INT | STM_SIGNED:
        return( TYP_SLONG64 );
    case STM_INT64 | STM_INT | STM_UNSIGNED:
        return( TYP_ULONG64 );
    case STM_FLOAT:
        return( TYP_FLOAT );
    case STM_DOUBLE:
        return( TYP_DOUBLE );
    case STM_VOID:
        return( TYP_VOID );
    case STM_LONG | STM_DOUBLE:
        return( TYP_LONG_DOUBLE );
    case STM_LONG | STM_CHAR:
        return( TYP_WCHAR );
    case STM_SEGMENT:
        return( TYP_USHORT );
    default:
        return( TYP_MAX );
    }
}

static TYPE findScalarType( DECL_SPEC *dspec )
{
    scalar_t scalar;
    TYPE type;

    if( dspec->is_default ) {
        type = TypeGetCache( TYPC_DEFAULT_INT );
    } else {
        scalar = dspec->scalar;
        switch( scalar ) {
        case STM_NULL:
            type = TypeGetCache( TYPC_DEFAULT_INT );
            break;
        case STM_INT:
            type = TypeGetCache( TYPC_CLEAN_INT );
            break;
        case STM_SEGMENT:
            type = TypeGetCache( TYPC_SEGMENT_SHORT );
            break;
        default:
            type = GetBasicType( findTypeId( scalar ) );
        }
    }
    return( type );
}

static void figureOutStgClass( DECL_SPEC *dspec )
{
    if( dspec->linkage != NULL && dspec->stg_class == STG_NULL ) {
        dspec->stg_class = STG_EXTERN;
    }
}

static void figureOutDSpec( DECL_SPEC *dspec )
{
    specifier_t specs;
    type_flag cv_flags;

    if( dspec->decl_checked ) return;
    figureOutStgClass( dspec );
    if( dspec->partial == NULL ) {
        dspec->partial = findScalarType( dspec );
    }
    specs = dspec->specifier & STY_DSPEC_MOD;
    if( specs != STY_NULL ) {
        cv_flags = convertCVSpec( specs );
        dspec->partial = MakeModifiedType( dspec->partial, cv_flags );
    }
    dspec->decl_checked = TRUE;
}

TYPE TypedefRemove( TYPE typ )
/****************************/
{
    for(;;) {
        if( typ == NULL ) break;
        if( typ->id != TYP_TYPEDEF ) break;
        typ = typ->of;
    }
    return( typ );
}

TYPE TypedefModifierRemoveOnly( TYPE typ )
/****************************************/
{
    TypeStripTdMod( typ );
    return typ;
}

TYPE TypedefModifierRemove( TYPE typ )
/************************************/
{
    TypeStripTdModEnumChar( typ );
    return( typ );
}

TYPE AlignmentType( TYPE typ )
/****************************/
{
    for(;;) {
        TypeStripTdModEnumChar( typ );
        switch( typ->id ) {
        case TYP_CLASS:
            switch( typ->u.c.info->max_align ) {
            case TARGET_CHAR:
                return( GetBasicType( TYP_UCHAR ) );
            case TARGET_SHORT:
                return( GetBasicType( TYP_USHORT ) );
            case TARGET_LONG:
                return( GetBasicType( TYP_ULONG ) );
            case TARGET_DOUBLE:
                return( GetBasicType( TYP_DOUBLE ) );
            default:
                return( GetBasicType( TYP_UCHAR ) );
            }
            break;
        case TYP_ARRAY:
            typ = typ->of;
            break;
        case TYP_POINTER:
        case TYP_MEMBER_POINTER:
            return( GetBasicType( TYP_UINT ) );
        case TYP_LONG_DOUBLE:
            return( GetBasicType( TYP_DOUBLE ) );
        default:
            return( typ );
        }
    }
}

SCOPE TypeScope( TYPE typ )
/*************************/
{
    SCOPE scope;

    scope = NULL;
    typ = ClassTypeForType( typ );
    if( typ != NULL ) {
        scope = typ->u.c.scope;
    }
    return( scope );
}

CLASS_INST *TypeClassInstantiation( TYPE type )
/****************************************/
{
    if( type != NULL ) {
        TypeStripTdMod( type );
        if( type->id == TYP_CLASS ) {
            if( type->flag & TF1_INSTANTIATION ) {
                SCOPE inst_scope = type->u.c.scope->enclosing;
                return inst_scope->owner.inst;
            }
        }
    }
    return NULL;
}

static NAME betterAnonEnumName( SYMBOL sym, TYPE typ )
{
    SYMBOL next = sym->thread;

    if( next->id != SC_ENUM ) {
        return( NULL );
    }
    if( next->sym_type != typ ) {
        return( NULL );
    }
    return( next->name->name );
}

NAME AnonymousEnumExtraName( TYPE type )
{
    SYMBOL sym;

    if( CompFlags.fixed_name_mangling ) {
        DbgAssert( type->id == TYP_ENUM );
        DbgAssert( TypeIsAnonymousEnum( type ) );
        sym = type->u.t.sym;
        if( sym != NULL ) {
            if( type->flag & TF1_UNNAMED ) {
                return( betterAnonEnumName( sym, type ) );
            }
        }
    }
    return( NULL );
}

NAME SimpleTypeName( TYPE typ )
/*****************************/
{
    NAME name;
    SYMBOL sym;

    name = NULL;
    if( typ != NULL ) {
        switch( typ->id ) {
        case TYP_TYPEDEF:
        case TYP_ENUM:
            sym = typ->u.t.sym;
            if( sym != NULL ) {
                if(( typ->flag & TF1_UNNAMED ) == 0 ) {
                    name = sym->name->name;
                }
            }
            break;
        case TYP_CLASS:
            name = typ->u.c.info->name;
        }
    }
    return( name );
}

boolean TypeDefined( TYPE typ )
/*****************************/
{
    for(;;) {
        if( typ == NULL ) return( FALSE );
        TypeStripTdMod( typ );
        switch( typ->id ) {
        case TYP_CLASS:
            if( typ->u.c.info->defined ) {
                return( TRUE );
            }
            return( FALSE );
        case TYP_ARRAY:
            if( typ->u.a.array_size == 0 ) {
                return( FALSE );
            }
            typ = typ->of;
            continue;
        default:
            return( TRUE );
        }
    }
}

boolean TypePartiallyDefined( TYPE typ )
/**************************************/
{
    for(;;) {
        if( typ == NULL ) return( FALSE );
        TypeStripTdMod( typ );
        switch( typ->id ) {
        case TYP_CLASS:
            if( typ->u.c.info->defined ) {
                return( TRUE );
            }
            if( StructOpened( typ ) != NULL ) {
                /* we are in the midst of defining the class */
                return( TRUE );
            }
            return( FALSE );
        case TYP_ARRAY:
            if( typ->u.a.array_size == 0 ) {
                return( FALSE );
            }
            typ = typ->of;
            continue;
        default:
            return( TRUE );
        }
    }
}

static boolean verifyAbstractStatus( TYPE class_type )
{
    CLASSINFO *info;

    info = class_type->u.c.info;
    if( info->abstract_OK ) {
        return( info->abstract );
    }
    if( ! TypeDefined( class_type ) ) {
        /* we have no way of knowing */
        return( FALSE );
    }
    if( ScopeHasPureFunctions( class_type->u.c.scope ) ) {
        info->abstract = TRUE;
    }
    info->abstract_OK = TRUE;
    return( info->abstract );
}

boolean TypeAbstract( TYPE typ )
/******************************/
{
    for(;;) {
        if( typ == NULL ) return( FALSE );
        TypeStripTdMod( typ );
        switch( typ->id ) {
        case TYP_CLASS:
            return( verifyAbstractStatus( typ ) );
        case TYP_ARRAY:
            typ = typ->of;
            continue;
        default:
            return( FALSE );
        }
    }
}

TYPE TypeNeedsCtor( TYPE type )
/*****************************/
{
    CLASSINFO   *info;
    TYPE        result;

    for(;;) {
        TypeStripTdMod( type );
        if( type->id == TYP_ARRAY ) {
            type = type->of;
        } else if( type->id == TYP_CLASS ) {
            info = type->u.c.info;
            if( info->needs_ctor && ! info->corrupted ) {
                result = type;
            } else {
                result = NULL;
            }
            break;
        } else {
            result = NULL;
            break;
        }
    }
    return( result );
}

#define MASK_TYPE_CHAR_STR  \
    ( 1 << TYP_CHAR  )    | \
    ( 1 << TYP_SCHAR )    | \
    ( 1 << TYP_UCHAR )    | \
    ( 1 << TYP_WCHAR )

boolean TypeIsCharString( TYPE type )
/***********************************/
{
    boolean retn = FALSE;       // - TRUE ==> is a character string

    // includes L"string" (wide chars)
    TypeStripTdMod( type );
    if( type->id == TYP_ARRAY ) {
        type = type->of;
        TypeStripTdMod( type );
        retn = TypeIdMasked( type, MASK_TYPE_CHAR_STR );
    }
    return( retn );
}

static DECL_SPEC *makeDeclSpec( void )
/************************************/
{
    DECL_SPEC *spec;

    spec = CarveAlloc( carveDECL_SPEC );
    spec->prev = NULL;
    spec->scope = NULL;
    spec->partial = NULL;
    spec->linkage = NULL;
    spec->typedef_defined = NULL;
    spec->name = NULL;
    spec->id = NULL;
    spec->prev_sym = NULL;
    spec->ms_declspec_fnmod = NULL;
    spec->stg_class = STG_NULL;
    spec->scalar = STM_NULL;
    spec->specifier = STY_NULL;
    spec->ms_declspec = STS_NULL;
    spec->type_defined = FALSE;
    spec->type_declared = FALSE;
    spec->ctor_name = FALSE;
    spec->is_default = FALSE;
    spec->diagnosed = FALSE;
    spec->decl_checked = FALSE;
    spec->type_elaborated = FALSE;
    spec->nameless_allowed = FALSE;
    spec->generic = FALSE;
    spec->class_instantiation = FALSE;
    spec->no_more_linkage = FALSE;
    spec->arg_declspec = FALSE;
    spec->class_idiom = FALSE;
    return( spec );
}

static void checkStgClass( DECL_SPEC *d1, DECL_SPEC *d2 )
/*******************************************************/
{
    stg_class_t any_repeats;

    /* any storage class specifiers to the right of a class name remove
       the possibility of the decl-spec being a constructor name */
    if( d2->stg_class != STG_NULL ) {
        d1->ctor_name = 0;
    }
    if( d2->linkage != NULL ) {
        if( d1->no_more_linkage || d1->linkage == NULL ) {
            CErr1( ERR_MISPLACED_EXTERN_LINKAGE );
            LinkagePop();
        } else {
            d1->linkage = LinkageMergePop();
        }
        d2->linkage = NULL;
    } else {
        d1->no_more_linkage = TRUE;
    }
    any_repeats = d1->stg_class & d2->stg_class;
    if( any_repeats != STG_NULL ) {
        CErr1( ERR_TOO_MANY_SC_SPECIFIERS );
        d2->stg_class &= ~any_repeats;
    }
    d1->stg_class |= d2->stg_class;
    switch( d1->stg_class ) {
    case STG_NULL:
    case STG_EXTERN:
    case STG_STATIC:
    case STG_AUTO:
    case STG_REGISTER:
    case STG_TYPEDEF:
    case STG_MUTABLE:
    case STG_TYPEDEF|STG_MUTABLE:
        return;
    }
    d1->stg_class = STG_NULL;
    CErr1( ERR_TOO_MANY_SC_SPECIFIERS );
}

static void checkScalar( DECL_SPEC *d1, DECL_SPEC *d2 )
/*****************************************************/
{
    scalar_t combo;
    scalar_t any_repeats;

    any_repeats = d1->scalar & d2->scalar;
    if( any_repeats != STM_NULL ) {
        if( any_repeats == STM_LONG ) {
            // 'long long' was found
            // quietly convert it to '__int64'
            // (this will accept bad stuff like 'long signed long')
            d1->scalar &= ~STM_LONG;
            d2->scalar &= ~STM_LONG;
            d1->scalar |= STM_INT64;
        } else {
            CErr1( ERR_ILLEGAL_TYPE_COMBO );
            d2->scalar &= ~any_repeats;
        }
    }
    d1->scalar |= d2->scalar;
    combo = d1->scalar;
    if( combo == STM_NULL ) {
        return;
    }
    /* any scalar type specifiers remove the possibility of the
       decl-spec being a constructor name */
    d1->ctor_name = 0;
    if( combo == ( STM_LONG | STM_CHAR ) ) {
        /* warn about deprecated use of 'long char' */
        CErr1( WARN_LONG_CHAR_DEPRECATED );
    }
    if( findTypeId( combo ) != TYP_MAX ) return;
    d1->scalar = STM_NULL;
    CErr1( ERR_ILLEGAL_TYPE_COMBO );
}

static void checkSpecifier( DECL_SPEC *d1, DECL_SPEC *d2 )
/********************************************************/
{
    specifier_t any_repeats;

    any_repeats = d1->specifier & d2->specifier;
    if( any_repeats != STY_NULL ) {
        CErr1( ERR_ILLEGAL_QUALIFIER_COMBO );
        d2->specifier &= ~any_repeats;
    }
    d1->specifier |= d2->specifier;
    /* any type modifiers to the right of a class name remove
       the possibility of the decl-spec being a constructor name */
    if( d2->specifier != STY_NULL ) {
        d1->ctor_name = 0;
    }
}

static void checkMSDeclspec( DECL_SPEC *d1, DECL_SPEC *d2 )
/********************************************************/
{
    TYPE ms_fnmod1;
    TYPE ms_fnmod2;

    d1->ms_declspec |= d2->ms_declspec;
    ms_fnmod2 = d2->ms_declspec_fnmod;
    if( ms_fnmod2 != NULL ) {
        ms_fnmod1 = d1->ms_declspec_fnmod;
        if( ms_fnmod1 != NULL ) {
            typeFree( ms_fnmod1 );
        }
        d1->ms_declspec_fnmod = ms_fnmod2;
    }
    /* any type modifiers to the right of a class name remove
       the possibility of the decl-spec being a constructor name */
    if( d2->ms_declspec != STS_NULL ) {
        d1->ctor_name = 0;
    }
}

static void checkType( DECL_SPEC *d1, DECL_SPEC *d2 )
/***************************************************/
{
    scalar_t combined;

    combined = d1->scalar | d2->scalar;
    if( combined != STM_NULL ) {
        /* e.g., long T x; */
        if( d1->partial != NULL || d2->partial != NULL ) {
            CErr1( ERR_TOO_MANY_TYPES_IN_DSPEC );
            d1->scalar = STM_NULL;
        }
    }
    if( d1->partial != NULL ) {
        if( d2->partial != NULL ) {
            CErr1( ERR_TOO_MANY_TYPES_IN_DSPEC );
        }
        /* any types to the right of a class name remove the
           possibility of the decl-spec being a constructor name */
        d1->ctor_name = 0;
    } else if( d2->partial != NULL ) {
        /* types need a lot of info to stay with the combination */
        d1->partial = d2->partial;
        d1->scope = d2->scope;
        d1->ctor_name = d2->ctor_name;
        d1->type_defined = d2->type_defined;
        d1->type_declared = d2->type_declared;
        d1->typedef_defined = d2->typedef_defined;
        d1->type_elaborated = d2->type_elaborated;
        d1->nameless_allowed = d2->nameless_allowed;
        d1->generic = d2->generic;
        d1->class_instantiation = d2->class_instantiation;
        d1->class_idiom = d2->class_idiom;
        d1->name = d2->name;
        /* fields that will be freed when 'd2' is freed */
        d1->id = d2->id;
        d2->id = NULL;
    }
}

DECL_SPEC *PTypeCombine( DECL_SPEC *d1, DECL_SPEC *d2 )
/*****************************************************/
{
    checkStgClass( d1, d2 );
    checkScalar( d1, d2 );
    checkSpecifier( d1, d2 );
    checkMSDeclspec( d1, d2 );
    checkType( d1, d2 );
    PTypeRelease( d2 );

    return( d1 );
}

static SYMBOL checkPreviouslyDeclared( SYMBOL sym, NAME name )
{
    SYMBOL class_template;
    SYMBOL free_sym;
    SCOPE scope;
    SYMBOL_NAME sym_name;
    SEARCH_RESULT *result;

    free_sym = NULL;
    result = ScopeFindLexicalClassType( GetCurrScope(), name );
    if( result != NULL ) {
        free_sym = sym;
        sym_name = result->sym_name;
        sym = sym_name->name_type;
        if( ScopeCheckSymbol( result, sym ) ) {
            sym = NULL;
        }
        ScopeFreeResult( result );
    } else {
        class_template = ClassTemplateLookup( GetCurrScope(), name );
        if( class_template == NULL ) {
            if( sym != NULL ) {
                scope = ScopeNearestNonClass( GetCurrScope() );
                ClassChangingScope( sym, scope );
                sym = ScopeInsert( scope, sym, name );
            }
        } else {
            free_sym = sym;
            sym = class_template;
        }
    }
    if( free_sym != NULL && SymScope( free_sym ) == NULL ) {
        FreeSymbol( free_sym );
    }
    return( sym );
}

static DECL_SPEC *checkForClassFriends( DECL_SPEC *dspec, boolean decl_done )
{
    NAME name;
    TYPE type;
    TYPE original_type;
    SYMBOL sym;
    SCOPE scope;
    boolean bad_friend;

    sym = dspec->typedef_defined;
    if(( dspec->specifier & STY_FRIEND ) == 0 || ! decl_done ) {
        if( sym != NULL && ! dspec->generic ) {
            scope = GetCurrScope();
            if( ! dspec->class_idiom ) {
                scope = ScopeNearestNonClass( scope );
                ClassChangingScope( sym, scope );
            }
            sym = ScopeInsert( scope, sym, dspec->name );
            dspec->typedef_defined = NULL;
        }
        return( dspec );
    }
    bad_friend = FALSE;
    figureOutStgClass( dspec );
    if( dspec->stg_class != STG_NULL ) {
        bad_friend = TRUE;
    }
    if( dspec->scalar != STM_NULL ) {
        bad_friend = TRUE;
    }
    if( dspec->specifier != STY_FRIEND ) {
        bad_friend = TRUE;
    }
    if( bad_friend ) {
        CErr1( ERR_FRIEND_BAD );
    }
    original_type = dspec->partial;
    type = original_type;
    if( type != NULL ) {
        type = StructType( type );
    }
    name = dspec->name;
    if( dspec->type_defined ) {
        CErr1( ERR_CLASS_FRIEND_DEFINES_SOMETHING );
        bad_friend = TRUE;
    } else if( type == NULL || ! dspec->type_elaborated ) {
        /* p. 249 ARM */
        if( type == NULL ) {
            if( original_type != TypeError ) {
                CErr1( ERR_CLASS_FRIEND_NO_CLASS );
            }
            bad_friend = TRUE;
        } else if( CErr1( ANSI_CLASS_FRIEND_NEEDS_CLASS ) & MS_WARNING ) {
            name = SimpleTypeName( type );
        } else {
            bad_friend = TRUE;
        }
    }
    if( ! ScopeType( GetCurrScope(), SCOPE_CLASS ) ) {
        CErr1( ERR_FRIEND_NOT_IN_CLASS );
        bad_friend = TRUE;
    }
    dspec->specifier &= ~STY_FRIEND;
    if( !bad_friend ) {
        if( sym != NULL ) {
            sym = checkPreviouslyDeclared( sym, name );
        }
        if( ( sym != NULL ) && ( sym->id != SC_TYPEDEF ) ) {
            ScopeAddFriendSym( GetCurrScope(), sym );
        } else {
            if( sym != NULL ) {
                type = StructType( sym->sym_type );
            }

            if( type != NULL ) {
                ScopeAddFriendType( GetCurrScope(), type, sym );
            }
        }
        /* make sure we don't need a declarator for this declaration */
        dspec->nameless_allowed = TRUE;
    }
    return( dspec );
}

DECL_SPEC *PTypeDone( DECL_SPEC *dspec, boolean decl_done )
/*********************************************************/
{
    dspec = checkForClassFriends( dspec, decl_done );
    return( dspec );
}

static void noExtraDeclSpec( DECL_SPEC *dspec )
/*********************************************/
{
    if( dspec->stg_class != STG_NULL ) {
        dspec->stg_class = STG_NULL;
        CErr1( ANSI_TYPE_SPECIFIER_STGCLASS );
    }
    if( dspec->ms_declspec != STS_NULL ) {
        dspec->ms_declspec = STS_NULL;
        CErr1( ERR_UNEXPECTED_DECLSPEC_MOD );
    }
}

DECL_SPEC *CheckTypeSpecifier( DECL_SPEC *dspec )
/***********************************************/
{
    figureOutStgClass( dspec );
    noExtraDeclSpec( dspec );
    if( dspec->specifier & STY_FN_MODIFIER ) {
        dspec->specifier &= ~STY_FN_MODIFIER;
        CErr1( ANSI_TYPE_SPECIFIER_QUALIFIER );
    }
    if( dspec->type_declared || dspec->type_defined ) {
        CErr1( ERR_TYPE_SPECIFIER_DEFINES );
    }
    return( dspec );
}

DECL_SPEC *PTypeStgClass( stg_class_t val )
/*****************************************/
{
    DECL_SPEC *spec;

    spec = makeDeclSpec();
    spec->stg_class = val;
    return( spec );
}

DECL_SPEC *PTypeMSDeclSpec( DECL_SPEC *dspec, PTREE id )
/******************************************************/
{
    NAME        name;
    DECL_SPEC   *spec;

    if( id == NULL ) {
        if( dspec == NULL )
            dspec = makeDeclSpec();
        return( dspec );
    }
    name = id->u.id.name;
    spec = makeDeclSpec();
    if( strcmp( name, "dllimport" ) == 0 ) {
        spec->ms_declspec = STS_DLLIMPORT;
    } else if( strcmp( name, "dllexport" ) == 0 ) {
        spec->ms_declspec = STS_DLLEXPORT;
    } else if( strcmp( name, "thread" ) == 0 ) {
        spec->ms_declspec = STS_THREAD;
    } else if( strcmp( name, "naked" ) == 0 ) {
        spec->ms_declspec = STS_NAKED;
    } else {
        PTreeErrorExprName( id, ERR_UNSUPPORTED_DECLSPEC, name );
    }
    PTreeFree( id );
    if( dspec != NULL ) {
        spec = PTypeCombine( dspec, spec );
    }
    return( spec );
}

DECL_SPEC *PTypeMSDeclSpecModifier( DECL_SPEC *dspec, TYPE fnmod_type )
/*********************************************************************/
{
    DECL_SPEC *spec;

    spec = makeDeclSpec();
    spec->ms_declspec = STS_MODIFIER;
    spec->ms_declspec_fnmod = fnmod_type;
    if( dspec != NULL ) {
        spec = PTypeCombine( dspec, spec );
    }
    return( spec );
}

void PTypeForceLinkagePush( DECL_SPEC *dspec )
/********************************************/
{
    DbgAssert( dspec->linkage == NULL );
    dspec->linkage = CurrLinkage;
}

DECL_SPEC *PTypeLinkage( void )
/*****************************/
{
    DECL_SPEC *dspec;

    dspec = makeDeclSpec();
    dspec->linkage = CurrLinkage;
    return( dspec );
}

DECL_SPEC *PTypeScalar( scalar_t val )
/************************************/
{
    DECL_SPEC *spec;

    spec = makeDeclSpec();
    spec->scalar = val;
    return( spec );
}

DECL_SPEC *PTypeSpecifier( specifier_t val )
/******************************************/
{
    DECL_SPEC *spec;

    spec = makeDeclSpec();
    spec->specifier = val;
    return( spec );
}

DECL_SPEC *CheckNoDeclarator( DECL_SPEC *dspec )
/**********************************************/
{
    boolean no_special_case;

    /* defining a new class or enum doesn't need a declarator */
    figureOutStgClass( dspec );
    if( dspec->type_declared || dspec->type_defined || dspec->nameless_allowed){
        if( dspec->specifier != STY_NULL ) {
            dspec->specifier = STY_NULL;
            CErr1( ANSI_TYPE_SPECIFIER_QUALIFIER );
        }
        no_special_case = TRUE;
        if( dspec->type_defined ) {
            if( ClassAnonymousUnion( dspec ) ) {
                no_special_case = FALSE;
            }
        }
        if( no_special_case ) {
            noExtraDeclSpec( dspec );
        }
    } else {
        CErr1( ERR_NO_DECLARATOR );
    }
    return( dspec );
}

void CheckDimension( PTREE dim_expr )
/***********************************/
{
    /* dim_expr is a PT_[U]INT_CONSTANT */
    if( dim_expr->u.uint_constant == 0 ) {
        CErr1( ERR_ZERO_DIMENSION );
        dim_expr->u.uint_constant = 1;
    } else if( dim_expr->op == PT_INT_CONSTANT ) {
        if( dim_expr->u.int_constant < 0 ) {
            CErr1( ERR_NEGATIVE_DIMENSION );
            dim_expr->u.int_constant = - dim_expr->u.int_constant;
        }
    }
}

static TYPE declSpecType( DECL_SPEC *dspec )
{
    TYPE decl_type;

    decl_type = TypeError;
    if( dspec->specifier & STY_FN_MODIFIER ) {
        CErr1( ERR_BAD_FN_MODIFIER );
    } else {
        figureOutDSpec( dspec );
        decl_type = dspec->partial;
    }
    return( decl_type );
}

PTREE DoDeclSpec( DECL_SPEC *dspec )
/**********************************/
{
    return( PTreeType( declSpecType( dspec ) ) );
}

static TYPE removeModifiers( TYPE start, TYPE end )
{
    TYPE mod_list;
    TYPE mod_type;

    mod_list = NULL;
    while( start != end ) {
        if( start->id == TYP_MODIFIER ) {
            mod_type = dupModifier( start );
            mod_type->of = mod_list;
            mod_list = mod_type;
        }
        start = start->of;
    }
    return( mod_list );
}

static TYPE replaceModifiers( TYPE mod_list, TYPE base_type )
{
    TYPE mod_type;

    while( mod_list != NULL ) {
        mod_type = mod_list;
        mod_list = mod_list->of;
        base_type = MakeTypeOf( mod_type, base_type );
    }
    return( base_type );
}

static TYPE changeToPointer( TYPE type, TYPE array_type )
{
    TYPE base_type;
    TYPE ptr_type;
    TYPE mod_list;

    // the ordering of replaceModifiers and MakePointerType is critical for
    // making 'near -> [] -> int' into '* -> near -> int' work properly
    // (the effect on 'const' and 'volatile' is still open so we may have
    // to split the modifiers into memory model and cv-qualifier types)
    mod_list = removeModifiers( type, array_type );
    base_type = array_type->of;
    base_type = replaceModifiers( mod_list, base_type );
    ptr_type = MakePointerType( TF1_NULL, STY_NULL );
    base_type = MakeTypeOf( ptr_type, base_type );
    return( base_type );
}

static TYPE dupArray( TYPE type, TYPE ref_type, target_size_t size, type_flag flag )
{
    TYPE base_type;
    TYPE array_type;
    TYPE mod_list;

    mod_list = removeModifiers( type, ref_type );
    base_type = ref_type->of;
    array_type = MakeArrayType( size );
    array_type->flag |= flag;
    base_type = MakeTypeOf( array_type, base_type );
    base_type = replaceModifiers( mod_list, base_type );
    return( base_type );
}

static TYPE dupFunction( TYPE fn_type, unsigned control )
{
    unsigned i;
    arg_list *old_args;
    arg_list *args;
    TYPE new_fn_type;

    args = fn_type->u.f.args;
    if(( control & DF_REUSE_ARGLIST ) == 0 ) {
        old_args = args;
        args = AllocArgListPerm( old_args->num_args );
        args->except_spec = old_args->except_spec;
        args->qualifier = old_args->qualifier;
        for( i = 0; i < old_args->num_args; ++i ) {
            args->type_list[i] = old_args->type_list[i];
        }
    }
    new_fn_type = MakeType( TYP_FUNCTION );
    new_fn_type->flag = fn_type->flag;
    new_fn_type->of = fn_type->of;
    new_fn_type->u.f.args = args;
    new_fn_type->u.f.pragma = fn_type->u.f.pragma;
    return( new_fn_type );
}

TYPE AddNonFunctionPragma( TYPE mod_type, TYPE base_type )
/********************************************************/
{
    TYPE type;
    TYPE mod_list;

    type = base_type;
    TypeStripTdMod( type );
    mod_list = removeModifiers( base_type, type );
    type = replaceModifiers( mod_list, type );
    return( MakeTypeOf( mod_type, type ) );
}

static TYPE makeThunkType( TYPE type, unsigned mtt_control )
{
    TYPE new_fn_type;       // - new function type
    TYPE old_fn_type;       // - old function type
    arg_list* args;         // - current argument list

    old_fn_type = FunctionDeclarationType( type );
    args = TypeArgList( old_fn_type );
    if( args->except_spec != NULL ) {
        // original fn will have except-spec processing so thunk doesn't need it
        new_fn_type = dupFunction( old_fn_type, DF_NULL );
        args = TypeArgList( new_fn_type );
        args->except_spec = NULL;
    } else {
        new_fn_type = dupFunction( old_fn_type, DF_REUSE_ARGLIST );
    }
    new_fn_type->flag &= TF1_FN_THUNK_KEEP;
    if( ! ( mtt_control & MTT_COPY_PRAGMA ) ) {
        new_fn_type->u.f.pragma = NULL;
    }
    if( mtt_control & MTT_INLINE ) {
        new_fn_type->flag |= TF1_INLINE;
    }
    new_fn_type = CheckDupType( new_fn_type );
    new_fn_type = MakeCommonCodeData( new_fn_type );
    return( new_fn_type );
}

TYPE MakeThunkFunction( TYPE type )
/*********************************/
{
    return makeThunkType( type, MTT_INLINE );
}

TYPE MakeThunkPragmaFunction( TYPE type )
/***************************************/
{
    return makeThunkType( type, MTT_INLINE | MTT_COPY_PRAGMA );
}

TYPE RemoveFunctionPragma( TYPE type )
/************************************/
{
    TYPE new_fn_type;
    TYPE fn_type;
    TYPE mod_list;

    fn_type = FunctionDeclarationType( type );
    mod_list = removeModifiers( type, fn_type );
    new_fn_type = dupFunction( fn_type, DF_REUSE_ARGLIST );
    new_fn_type->u.f.pragma = NULL;
    new_fn_type = CheckDupType( new_fn_type );
    new_fn_type = replaceModifiers( mod_list, new_fn_type );
    return( new_fn_type );
}

static TYPE adjustFullFunctionType( TYPE type
                                  , type_flag on_flags
                                  , type_flag off_flags
                                  , TYPE new_ret
                                  , AUX_INFO *new_pragma )
{
    TYPE new_fn_type;
    TYPE fn_type;
    TYPE mod_list;
    type_flag old_flags;
    type_flag new_flags;

    fn_type = FunctionDeclarationType( type );
    old_flags = fn_type->flag;
    new_flags = ( old_flags | on_flags ) & ~off_flags;
    if( new_flags == old_flags ) {
        /* no change in flags! */
        if( new_ret == NULL && new_pragma == NULL ) {
            /* no change to function type */
            return( type );
        }
    }
    mod_list = removeModifiers( type, fn_type );
    new_fn_type = dupFunction( fn_type, DF_REUSE_ARGLIST );
    new_fn_type->flag = new_flags;
    if( new_ret != NULL ) {
        new_fn_type->of = new_ret;
    }
    if( new_pragma != NULL ) {
        new_fn_type->u.f.pragma = new_pragma;
    }
    new_fn_type = CheckDupType( new_fn_type );
    new_fn_type = replaceModifiers( mod_list, new_fn_type );
    return( new_fn_type );
}

static TYPE adjustFunctionType( TYPE type, type_flag new_flags, TYPE new_ret )
{
    return( adjustFullFunctionType( type, new_flags, TF1_NULL, new_ret, NULL ) );
}

TYPE MakeNonInlineFunction( TYPE type )
/*************************************/
{
    return( adjustFullFunctionType( type, TF1_NULL, TF1_INLINE, NULL, NULL ) );
}

TYPE MakeVirtualFunction( TYPE type )
/***********************************/
{
    return( adjustFunctionType( type, TF1_VIRTUAL, NULL ) );
}

TYPE MakeInlineFunction( TYPE type )
/**********************************/
{
    return( adjustFunctionType( type, TF1_INLINE, NULL ) );
}

TYPE MakePureFunction( TYPE type )
/********************************/
{
    return( adjustFunctionType( type, TF1_PURE, NULL ) );
}

TYPE MakePureVirtualFunction( TYPE type )
/***************************************/
{
    return( adjustFunctionType( type, TF1_PURE | TF1_VIRTUAL, NULL ) );
}

TYPE MakePlusPlusFunction( TYPE type )
/************************************/
{
    return( adjustFunctionType( type, TF1_PLUSPLUS, NULL ) );
}

TYPE ChangeFunctionPragma( TYPE type, AUX_INFO *new_pragma )
/**********************************************************/
{
    return( adjustFullFunctionType( type, TF1_NULL, TF1_NULL, NULL, new_pragma ) );
}

TYPE AddFunctionFlag( TYPE type, type_flag additional )
/*****************************************************/
{
    return( adjustFunctionType( type, additional, NULL ) );
}

TYPE RemoveFunctionFlag( TYPE type, type_flag take_away )
/*******************************************************/
{
    return( adjustFullFunctionType( type, TF1_NULL, take_away, NULL, NULL ) );
}

TYPE MakeCombinedFunctionType( TYPE fn1, TYPE fn2, type_flag new_flags )
/**********************************************************************/
{
    type_flag add_flags;
    TYPE fn_type;

    fn_type = FunctionDeclarationType( fn2 );
    add_flags = fn_type->flag & ~TF1_FN_PERMANENT;
    if( add_flags != TF1_NULL ) {
        fn1 = adjustFunctionType( fn1, add_flags, NULL );
    }
    if( new_flags != TF1_NULL ) {
        fn1 = MakeModifiedType( fn1, new_flags );
    }
    return( fn1 );
}

static TYPE adjustParmType( TYPE parm_type )
{
    TYPE parm;
    TYPE ptr_type;

    parm = parm_type;
    TypeStripTdMod( parm );
    switch( parm->id ) {
    case TYP_FUNCTION:
        if( parm->u.f.args->qualifier != TF1_NULL ) {
            CErr1( ERR_CONST_VOLATILE_IN_A_TYPE );
        }
        ptr_type = MakePointerType( TF1_NULL, STY_NULL );
        parm_type = MakeTypeOf( ptr_type, parm_type );
        break;
    case TYP_ARRAY:
        parm_type = changeToPointer( parm_type, parm );
        break;
    }
    return( parm_type );
}

TYPE PointerTypeForArray( TYPE array )
/************************************/
{
    TYPE type;

    type = array;
    TypeStripTdMod( type );
    if( type->id == TYP_ARRAY ) {
        array = changeToPointer( array, type );
    }
    return( array );
}

TYPE DupArrayTypeForArrayFlag( TYPE array, target_size_t size, type_flag flag )
/*****************************************************************************/
{
    TYPE type;

    type = array;
    TypeStripTdMod( type );
    if( type->id == TYP_ARRAY ) {
        array = dupArray( array, type, size, flag );
    }
    return( array );
}

TYPE DupArrayTypeForArray( TYPE array, target_size_t size )
/*********************************************************/
{
    return( DupArrayTypeForArrayFlag( array, size, TF1_NULL ) );
}

TYPE TypeOverrideModifiers( TYPE prev_type, TYPE curr_type )
/**********************************************************/
{
    TYPE prev_unmod;
    TYPE curr_unmod;
    type_flag prev_flags;
    type_flag curr_flags;
    void *prev_base;
    void *curr_base;

    // override modifiers in prev with curr's modifiers
    // but keep any of prev's non-memory model modifiers
    prev_unmod = TypeModFlagsBaseEC( prev_type, &prev_flags, &prev_base );
    curr_unmod = TypeModFlagsBaseEC( curr_type, &curr_flags, &curr_base );
    prev_flags &= ~TF1_MEM_MODEL;
    curr_flags |= prev_flags;
    curr_type = MakeBasedModifierOf( prev_unmod, curr_flags, curr_base );
    return( curr_type );
}

static PTREE nameOfId( PTREE id )
{
    if( id == NULL || id->op == PT_ID ) {
        return( id );
    }
    if( ( id->op == PT_BINARY ) && ( id->cgop == CO_TEMPLATE ) ) {
        CFatal( "template-id not supported in this context" );
    }
#ifndef NDEBUG
    if( id->op != PT_BINARY || ( id->cgop != CO_COLON_COLON && id->cgop != CO_STORAGE )) {
        CFatal( "corrupted qualified id" );
    }
#endif
    return( id->u.subtree[1] );
}

static int scanDeclarator( TYPE declarator_list, unsigned *count )
{
    arg_list *args;
    int status;
    boolean cv_function_OK;

    /*
        for "int X::* (*pf)( int )" the declarator_list is:
            TYP_MEMBER_POINTER
            TYP_FUNCTION
            TYP_POINTER
    */
    status = SM_NULL;
    while( declarator_list != NULL ) {
        cv_function_OK = FALSE;
        switch( declarator_list->id ) {
        case TYP_FUNCTION:
            if( status & SM_SAW_FUNCTION ) {
                status |= SM_RETURN_DECLARATOR | SM_SAW_DECLARATOR;
                status &= ~SM_NOT_A_FUNCTION;
                if( status & SM_SAW_CV_FUNCTION ) {
                    status |= SM_CV_FUNCTION_ERROR;
                }
            } else if( status & SM_SAW_DECLARATOR ) {
                status |= SM_RETURN_DECLARATOR;
            }
            status &= ~SM_SAW_CV_FUNCTION;
            args = declarator_list->u.f.args;
            if( count != NULL ) {
                *count = args->num_args;
            }
            if( args->qualifier != TF1_NULL ) {
                /* we have <declarator> ( <args> ) const/volatile */
                status |= SM_SAW_CV_FUNCTION;
            }
            status |= SM_SAW_FUNCTION;
            break;
        case TYP_MODIFIER:
            break;
        case TYP_MEMBER_POINTER:
            cv_function_OK = TRUE;
            /* fall through */
        default:
            if( status & SM_SAW_CV_FUNCTION ) {
                if( ! cv_function_OK ) {
                    status |= SM_CV_FUNCTION_ERROR;
                }
            }
            if( status & SM_SAW_FUNCTION ) {
                status |= SM_NOT_A_FUNCTION;
            }
            status |= SM_SAW_DECLARATOR;
            status &= ~SM_SAW_CV_FUNCTION;
        }
        declarator_list = declarator_list->of;
    }
    if(( status & SM_SAW_FUNCTION ) == 0 ) {
        /* function type was never found! */
        status |= SM_NOT_A_FUNCTION;
    }
    status &= ~( SM_SAW_FUNCTION | SM_SAW_DECLARATOR | SM_SAW_CV_FUNCTION );
    return( status );
}

static type_flag convertFnSpec( specifier_t spec )
{
    type_flag flag;

    flag = TF1_NULL;
    if( spec & STY_VIRTUAL ) {
        flag |= TF1_VIRTUAL;
    }
    if( spec & STY_INLINE ) {
        flag |= TF1_INLINE;
    }
    if( spec & STY_EXPLICIT ) {
        flag |= TF1_EXPLICIT;
    }
    return( flag );
}

static boolean applyFnSpec( TYPE declarator, DECL_SPEC *dspec, int status )
{
    specifier_t fn_spec;
    type_flag flags;
    TYPE type;
    TYPE fn_type;

    fn_spec = dspec->specifier;
    flags = convertFnSpec( fn_spec );
    if( flags == TF1_NULL && ( fn_spec & STY_FRIEND ) == 0 ) {
        return( FALSE );
    }
    fn_type = NULL;
    for( type = declarator; type != NULL; type = type->of ) {
        if( type->id == TYP_FUNCTION ) {
            fn_type = type;
            /* keep going; we want the last function type */
        }
    }
    if( fn_type == NULL ) {
        CErr1( ERR_BAD_FN_MODIFIER );
    } else {
        fn_type->flag |= flags;
        if( status & SM_NOT_A_FUNCTION ) {
            CErr1( ERR_BAD_FIV_MODIFIER );
        } else {
            if( fn_spec & STY_FRIEND ) {
                /* friend function */
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

static void applyFnClassMods( TYPE declarator, DECL_SPEC *dspec )
{
    TYPE type;
    TYPE fn_type;

    if( ! dspec->arg_declspec ) {
        if( ScopeType( GetCurrScope(), SCOPE_CLASS ) ) {
            /* we are building up a member function declaration */
            for( type = declarator; type != NULL; type = type->of ) {
                if( type->id == TYP_FUNCTION ) {
                    fn_type = type;
                    /* keep going; we want the last function type */
                }
            }
            if( fn_type->u.f.pragma == NULL ) {
                /* only apply class modifiers if no #pragma used in fn */
                ClassAddFunctionMods( fn_type );
            }
        }
    }
}

static void checkUserConversion( TYPE return_type, int status, unsigned arg_cnt)
{
    if( status & SM_NOT_A_FUNCTION ) {
        CErr1( ERR_USER_CONV_BAD_DECL );
    } else if( return_type != TypeGetCache( TYPC_DEFAULT_INT ) || status & SM_RETURN_DECLARATOR ) {
        CErr1( ERR_USER_CONV_BAD_RETURN );
    }
    if( status == SM_NULL && arg_cnt != 0 ) {
        CErr1( ERR_USER_CONV_BAD_FUNC );
    }
}

static void checkDestructor( TYPE return_type, int status, unsigned arg_count )
{
    if( status & SM_NOT_A_FUNCTION ) {
        CErr1( ERR_DESTRUCTOR_BAD_DECL );
    } else if( return_type != TypeGetCache( TYPC_DEFAULT_INT ) || status & SM_RETURN_DECLARATOR ) {
        CErr1( ERR_DESTRUCTOR_BAD_RETURN );
    }
    if( status == SM_NULL && arg_count != 0 ) {
        CErr1( ERR_DESTRUCTOR_BAD_FUNC );
    }
}

static void checkOperator( TYPE return_type, int status, NAME name )
{
    return_type = return_type;
    if( status & SM_NOT_A_FUNCTION ) {
        CErr2p( ERR_OPERATOR_BAD_DECL, name );
    }
}

static void checkUsefulParms( int status, DECL_INFO *dinfo )
{
    if( status & SM_NOT_A_FUNCTION ) {
        /* this isn't a function so the saved parms are not useful */
        FreeArgs( dinfo->parms );
        dinfo->parms = NULL;
        dinfo->explicit_parms = FALSE;
    }
}

static void setStorageClass( SYMBOL sym, stg_class_t sc )
{
    sc &= STG_TYPEDEF|STG_REGISTER|STG_AUTO|STG_EXTERN|STG_STATIC;
    switch( sc ) {
    case STG_TYPEDEF:
        sym->id = SC_TYPEDEF;
        break;
    case STG_REGISTER:
        sym->id = SC_REGISTER;
        break;
    case STG_AUTO:
        sym->id = SC_AUTO;
        break;
    case STG_EXTERN:
        sym->id = SC_EXTERN;
        break;
    case STG_STATIC:
        sym->id = SC_STATIC;
        break;
    }
}

static void reportRepeatedInfo( TOKEN token )
{
    CErr2p( INF_REPEATED_ITEM, Tokens[token] );
}

static void dumpInfoFunctionMods( type_flag repeats )
{
    if( repeats & TF1_LOADDS ) {
        reportRepeatedInfo( T___LOADDS );
    }
    if( repeats & TF1_SAVEREGS ) {
        reportRepeatedInfo( T___SAVEREGS );
    }
    if( repeats & TF1_INTERRUPT ) {
        reportRepeatedInfo( T___INTERRUPT );
    }
}

static TYPE combineFunctionMods( TYPE fnmod_type, TYPE curr_type )
{
    type_flag curr_flag;
    type_flag fnmod_flag;
    AUX_INFO *curr_pragma;
    AUX_INFO *fnmod_pragma;

    if( fnmod_type == NULL ) {
        return( curr_type );
    }
    curr_pragma = curr_type->u.m.pragma;
    fnmod_pragma = fnmod_type->u.m.pragma;
    if( fnmod_pragma != NULL ) {
        if( curr_pragma != NULL && curr_pragma != fnmod_pragma ) {
            CErr1( ERR_MULTIPLE_PRAGMA_MODS );
        }
    } else if( curr_pragma != NULL ) {
        fnmod_type->u.m.pragma = curr_pragma;
    }
    curr_flag = curr_type->flag;
    fnmod_flag = fnmod_type->flag;
    if( fnmod_flag & curr_flag ) {
        if( CErr1( WARN_REPEATED_FUNCTION_MODS ) & MS_PRINTED ) {
            dumpInfoFunctionMods( fnmod_flag & curr_flag );
        }
    }
    fnmod_type->flag |= curr_flag;
    typeFree( curr_type );
    return( fnmod_type );
}

static boolean validModifierCombo( type_flag flag )
{
    flag &= TF1_MEM_MODEL;
    switch( flag ) {
    case TF1_NEAR:
    case TF1_FAR:
    case TF1_FAR16:
    case TF1_HUGE:
    case TF1_BASED_STRING:
    case TF1_BASED_VOID:
    case TF1_BASED_SELF:
    case TF1_BASED_FETCH:
    case TF1_BASED_ADD:
    case TF1_NULL:
        return( TRUE );
    }
    return( FALSE );
}

static void dumpInfoDataMods( type_flag repeats )
{
    if( repeats & TF1_NEAR ) {
        reportRepeatedInfo( T___NEAR );
    }
    if( repeats & TF1_FAR ) {
        reportRepeatedInfo( T___FAR );
    }
    if( repeats & TF1_FAR16 ) {
        reportRepeatedInfo( T___FAR16 );
    }
    if( repeats & TF1_HUGE ) {
        reportRepeatedInfo( T___HUGE );
    }
    if( repeats & TF1_DLLEXPORT ) {
        reportRepeatedInfo( T___EXPORT );
    }
}

static TYPE combineNormalMods( TYPE mod_type, TYPE curr_type )
{
    type_flag curr_flag;
    type_flag mod_flag;

    if( mod_type == NULL ) {
        curr_type->flag &= ~TF1_HUG_FUNCTION;
        return( curr_type );
    }
    curr_flag = curr_type->flag;
    curr_flag &= ~TF1_HUG_FUNCTION;
    mod_flag = mod_type->flag;
    if((( mod_flag & curr_flag ) & TF1_BASED ) != TF1_NULL ) {
        CErr1( ERR_REPEATED_BASED_MODS );
        curr_flag &= ~TF1_BASED;
    } else if( curr_flag & TF1_BASED ) {
        mod_type->u.m.base = curr_type->u.m.base;
    }
    if(( mod_flag & curr_flag ) != TF1_NULL ) {
        if( CErr1( WARN_REPEATED_DATA_MODS ) & MS_PRINTED ) {
            dumpInfoDataMods( mod_flag & curr_flag );
        }
    }
    if( ! validModifierCombo( mod_flag | curr_flag ) ) {
        CErr1( ERR_UNCOMBINABLE_MEM_MODS );
        curr_flag &= ~TF1_MEM_MODEL;
    }
    mod_type->flag |= curr_flag;
    typeFree( curr_type );
    return( mod_type );
}

static TYPE extractMemoryModel( TYPE mod_type, TYPE *leftover )
{
    type_flag mod_flag;

    *leftover = NULL;
    mod_flag = mod_type->flag & TF1_CV_MASK;
    if( mod_flag != TF1_NULL ) {
        mod_type->flag &= ~mod_flag;
        *leftover = MakeFlagModifier( mod_flag );
    }
    return( mod_type );
}

static TYPE extractFunctionMods( TYPE curr, TYPE *ptail, boolean stop_at_nonmod )
{
    TYPE head;
    TYPE tail;
    TYPE next;
    TYPE prev;

    head = NULL;
    tail = NULL;
    prev = curr;
    for( curr = curr->of; curr != NULL; curr = next ) {
        next = curr->of;
        if( curr->id != TYP_MODIFIER ) {
            if( stop_at_nonmod ) {
                break;
            }
            prev = curr;
            continue;
        }
        if( curr->flag & (TF1_TYP_FUNCTION|TF1_HUG_FUNCTION) ) {
            prev->of = next;
            if( tail == NULL ) {
                tail = curr;
            }
            curr->of = head;
            head = curr;
        } else {
            prev = curr;
        }
    }
    *ptail = tail;
    return( head );
}

static TYPE linkAfterPrev( TYPE list, TYPE prev, TYPE head, TYPE tail )
{
    if( prev != NULL ) {
        tail->of = prev->of;
        prev->of = head;
    } else {
        tail->of = list;
        list = head;
    }
    return( list );
}

static TYPE makeMSDeclSpecType( DECL_SPEC *dspec )
{
    ms_declspec_t ms_dspec;
    type_flag flags;
    TYPE type;
    TYPE fnmod_type;

    ms_dspec = dspec->ms_declspec;
    flags = TF1_NULL;
    if( ms_dspec & STS_DLLEXPORT ) {
        flags = TF1_DLLEXPORT;
    } else if( ms_dspec & STS_DLLIMPORT ) {
        flags = TF1_DLLIMPORT;
    }
    if( ms_dspec & STS_THREAD ) {
        flags |= TF1_THREAD;
    }
    type = NULL;
    if( flags != TF1_NULL ) {
        type = MakeFlagModifier( flags );
    }
    if( ms_dspec & STS_MODIFIER ) {
        fnmod_type = dspec->ms_declspec_fnmod;
        fnmod_type->of = type;
        type = fnmod_type;
    }
    if( ms_dspec & STS_NAKED ) {
        fnmod_type = MakeFlagModifier( TF1_TYP_FUNCTION | TF1_NAKED );
        fnmod_type->of = type;
        type = fnmod_type;
    }
    return( type );
}


void SetFnClassMods( TYPE fn_type, type_flag fn_flag, AUX_INFO *fn_pragma )
/*************************************************************************/
{
    fn_type->flag |= fn_flag;
    fn_type->u.f.pragma = fn_pragma;
}

static void adjustMemberFn( TYPE fn_type, TYPE member_ptr )
{
    TYPE class_type;
    TYPE class_mod;
    type_flag fn_flag;
    type_flag mod_flag;
    AUX_INFO *fn_pragma;

    DbgAssert( fn_type->id == TYP_FUNCTION );
    DbgAssert( member_ptr->id == TYP_MEMBER_POINTER );
    // a member pointer can only point to "C++" fns
    fn_type->flag |= TF1_PLUSPLUS | TF1_PLUSPLUS_SET;
    class_type = StructType( member_ptr->u.mp.host );
    if( class_type == NULL ) {
        return;
    }
    class_mod = class_type->u.c.info->class_mod;
    if( class_mod == NULL ) {
        return;
    }
    AbsorbBaseClassModifiers( class_mod, &mod_flag, &fn_flag, &fn_pragma );
    SetFnClassMods( fn_type, fn_flag, fn_pragma );
}

static TYPE adjustModifiers( DECL_SPEC *dspec, TYPE list )
{
    TYPE *prev_link;
    TYPE outermost_list;
    TYPE outer;
    TYPE prev;
    TYPE next;
    TYPE curr;
    TYPE head;
    TYPE tail;
    TYPE scan;
    TYPE any_fn_mod;
    TYPE last_fn_mod;
    TYPE last_fn_type;
    TYPE last_prev;

    /* outermost modifers must be moved to the end of the list */
    any_fn_mod = NULL;
    outermost_list = NULL;
    if( dspec->ms_declspec != STS_NULL ) {
        outermost_list = makeMSDeclSpecType( dspec );
        for( outer = outermost_list; outer != NULL; outer = outer->of ) {
            if( outer->flag & (TF1_TYP_FUNCTION|TF1_HUG_FUNCTION) ) {
                any_fn_mod = outermost_list;
                break;
            }
        }
    }
    prev_link = &list;
    for( curr = list; curr != NULL; curr = next ) {
        next = curr->of;
        switch( curr->id ) {
        case TYP_MODIFIER:
            if( curr->flag & TF1_OUTERMOST ) {
                curr->flag &= ~TF1_OUTERMOST;
                *prev_link = next;
                curr->of = outermost_list;
                outermost_list = curr;
                /* leave prev_link as is */
                continue;
            }
            if( curr->flag & (TF1_TYP_FUNCTION|TF1_HUG_FUNCTION) ) {
                any_fn_mod = curr;
            }
            break;
        case TYP_FUNCTION:
            for( scan = next; scan != NULL; scan = scan->of ) {
                if( scan->id != TYP_MODIFIER ) {
                    if( scan->id == TYP_MEMBER_POINTER ) {
                        adjustMemberFn( curr, scan );
                    }
                    break;
                }
            }
            break;
        }
        prev_link = &(curr->of);
    }
    *prev_link = outermost_list;
    if( any_fn_mod == NULL ) {
        return( list );
    }
    /* function modifiers must be shuffled to come before the TYP_FUNCTION */
    last_fn_mod = NULL;
    last_fn_type = NULL;
    prev = NULL;
    for( curr = list; curr != NULL; curr = curr->of ) {
        if( curr->id == TYP_FUNCTION ) {
            head = extractFunctionMods( curr, &tail, TRUE );
            if( head != NULL ) {
                list = linkAfterPrev( list, prev, head, tail );
                prev = tail;
            }
            last_fn_mod = NULL;
            last_fn_type = curr;
            last_prev = prev;
        } else if( curr->id == TYP_MODIFIER ) {
            if( curr->flag & (TF1_TYP_FUNCTION|TF1_HUG_FUNCTION) ) {
                last_fn_mod = curr;
            }
        }
        prev = curr;
    }
    if( last_fn_type != NULL && last_fn_mod != NULL ) {
        head = extractFunctionMods( last_fn_type, &tail, FALSE );
        if( head != NULL ) {
            list = linkAfterPrev( list, last_prev, head, tail );
        }
    }
    return( list );
}

static TYPE massageFunctionTypeInDSpec( TYPE *dspec_type, TYPE list )
{
    TYPE base_type;
    TYPE new_fn_type;
    TYPE fn_type;
    TYPE mod_list;
    TYPE mod_type;

    base_type = *dspec_type;
    fn_type = FunctionDeclarationType( base_type );
    if( fn_type == NULL ) {
        return( list );
    }
    mod_list = removeModifiers( base_type, fn_type );
    while( mod_list != NULL ) {
        mod_type = mod_list;
        mod_list = mod_list->of;
        mod_type->of = list;
        list = mod_type;
    }
    new_fn_type = dupFunction( fn_type, DF_REUSE_ARGLIST );
    new_fn_type->of = list;
    new_fn_type->flag |= TF1_PLUSPLUS_SET;
    list = new_fn_type;
    *dspec_type = fn_type->of;
    return( list );
}

static TYPE make16BitEquivalent( TYPE type )
{
    TYPE check;
    TYPE mod_list;

    /* we don't want to shrink "int" enums down to a short */
    check = type;
    TypeStripTdMod( check );
    switch( check->id ) {
    case TYP_SINT:
        type = GetBasicType( TYP_SSHORT );
        break;
    case TYP_UINT:
        type = GetBasicType( TYP_USHORT );
        break;
    case TYP_POINTER:
        mod_list = removeModifiers( type, check );
        type = MakeModifiedType( check->of, TF1_SET_FAR16 );
        type = MakePointerTo( type );
        type = replaceModifiers( mod_list, type );
        break;
    }
    return( type );
}

static TYPE makeFar16FunctionType( TYPE fn )
{
    arg_list *args;
    TYPE *parm;
    unsigned i;

    fn = dupFunction( fn, DF_NULL );
    fn->of = make16BitEquivalent( fn->of );
    args = fn->u.f.args;
    parm = args->type_list;
    for( i = args->num_args; i != 0; --i ) {
        *parm = make16BitEquivalent( *parm );
        ++parm;
    }
    return( CheckDupType( fn ) );
}

static TYPE makeModifiedTypeOf( TYPE mod, TYPE of )
{
    if( of->id == TYP_FUNCTION ) {
        if( mod->flag & TF1_MEM_MODEL ) {
            if( mod->flag & TF1_FAR16 ) {
                of = makeFar16FunctionType( of );
                if( of->u.f.pragma == NULL ) {
                    /* default to "cdecl" for far16 functions */
                    of->u.f.pragma = cdeclPragma;
                    of->flag &= ~TF1_PLUSPLUS;
                }
            }
        } else {
            if( of->flag & TF1_INTERRUPT ) {
                /* interrupt implies default far */
                mod->flag |= TF1_FAR;
            }
        }
        mod->flag &= ~TF1_CV_MASK;
        if( ( mod->flag == TF1_NULL )
         && ( mod->u.m.base == NULL )
         && ( mod->u.m.pragma == NULL ) ) {
            return of;
        }
    }
    return( MakeTypeOf( mod, of ) );
}

static void checkForUnnamedStruct( DECL_SPEC *dspec, NAME id )
{
    TYPE base_type;

    if( ! dspec->type_defined ) {
        return;
    }
    base_type = StructType( dspec->partial );
    if( base_type == NULL ) {
        return;
    }
    if( ! base_type->u.c.info->unnamed ) {
        return;
    }
    ClassMakeUniqueName( base_type, id );
}

static msg_status_t errWithSymLoc( unsigned msg, SYMBOL sym )
{
    SetErrLoc( &(sym->locn->tl) );
    return( CErr1( msg ) );
}

DECL_INFO *FinishDeclarator( DECL_SPEC *dspec, DECL_INFO *dinfo )
/***************************************************************/
{
    NAME id;
    SYMBOL sym;
    PTREE id_tree;
    TYPE test_type;
    TYPE of_type;
    TYPE prev_type;
    TYPE curr_type;
    TYPE next_type;
    TYPE fnmod_type;
    TYPE mod_type;
    TYPE leftover_type;
    unsigned arg_count;
    int status;
    int msg_num;
    type_flag mod_flags;
    type_flag fn_flag;
    type_flag ptr_flags;
    AUX_INFO *fn_pragma;
    struct {
        unsigned memory_model_movement : 1;
        unsigned add_type : 1;
        unsigned diagnose_sym : 1;
        unsigned OK_to_add_fnmod : 1;
    } flag;

    id_tree = nameOfId( dinfo->id );
    id = NULL;
    if( id_tree != NULL ) {
        id = id_tree->u.id.name;
    }
    figureOutDSpec( dspec );
    checkForUnnamedStruct( dspec, id );
    curr_type = dinfo->list;
    curr_type = adjustModifiers( dspec, curr_type );
    prev_type = dspec->partial;
    curr_type = massageFunctionTypeInDSpec( &prev_type, curr_type );
    status = scanDeclarator( curr_type, &arg_count );
    if( status & SM_CV_FUNCTION_ERROR ) {
        CErr1( ERR_CONST_VOLATILE_IN_A_TYPE );
    }
    if( applyFnSpec( curr_type, dspec, status ) ) {
        dinfo->friend_fn = TRUE;
        if( dspec->stg_class != STG_NULL ) {
            CErr1( ERR_FRIEND_BAD );
            dspec->stg_class = STG_NULL;
        }
    } else {
        if(( status & SM_NOT_A_FUNCTION ) == 0 ) {
            /* these don't apply to friend functions */
            applyFnClassMods( curr_type, dspec );
        }
    }
    if( dinfo->parms != NULL ) {
        /* make sure the parms are useful */
        checkUsefulParms( status, dinfo );
    }
    if( id_tree != NULL ) {
        switch( id_tree->cgop ) {
        case CO_NAME_CONVERT:
            checkUserConversion( prev_type, status, arg_count );
            prev_type = id_tree->type;
            break;
        case CO_NAME_DTOR:
            checkDestructor( prev_type, status, arg_count );
            break;
        case CO_NAME_OPERATOR:
            checkOperator( prev_type, status, id );
            switch( id_tree->id_cgop ) {
            case CO_NEW:
            case CO_DELETE:
            case CO_NEW_ARRAY:
            case CO_DELETE_ARRAY:
            case CO_CALL:
                break;
            default:
                ForceNoDefaultArgs( dinfo, ERR_DEFAULT_ARGS_OPERATOR );
            }
            break;
        }
    }
    of_type = prev_type;
    TypeStripTdMod( of_type );
    fnmod_type = NULL;
    mod_type = NULL;
    while( curr_type != NULL ) {
        /*
          prev_type     compressed base type (cannot be modified!)
          of_type       prev_type with modifiers removed (cannot be modified!)
          curr_type     current type being analysed
          fnmod_type    a TYP_MODIFIER that applies to functions
          mod_type      a TYP_MODIFIER that applies to non-functions
        */
        next_type = curr_type->of;
        flag.memory_model_movement = FALSE;
        flag.add_type = TRUE;
        switch( curr_type->id ) {
        case TYP_MODIFIER:
            flag.add_type = FALSE;
            if( curr_type->flag & TF1_TYP_FUNCTION ) {
                curr_type->flag &= ~TF1_TYP_FUNCTION;
                fnmod_type = combineFunctionMods( fnmod_type, curr_type );
            } else {
                mod_type = combineNormalMods( mod_type, curr_type );
            }
            break;
        case TYP_FUNCTION:
            flag.memory_model_movement = TRUE;
            if( fnmod_type != NULL ) {
                curr_type->flag |= fnmod_type->flag;
                curr_type->u.f.pragma = fnmod_type->u.m.pragma;
                typeFree( fnmod_type );
                fnmod_type = NULL;
            }
            fn_flag = curr_type->flag;
            if( fn_flag & TF1_PLUSPLUS_SET ) {
                // TF1_PLUSPLUS is already set correctly
                fn_flag &= ~ TF1_PLUSPLUS_SET;
            } else {
                // override based on current linkage
                if( CurrLinkage != CppLinkage ) {
                    fn_flag &= ~ TF1_PLUSPLUS;
                } else {
                    fn_flag |= TF1_PLUSPLUS;
                }
            }
            curr_type->flag = fn_flag;
            /* check return type */
            switch( of_type->id ) {
            case TYP_FUNCTION:
                CErr1( ERR_FUNCTION_CANT_RETURN_A_FUNCTION );
                break;
            case TYP_ARRAY:
                CErr1( ERR_FUNCTION_CANT_RETURN_AN_ARRAY );
                break;
            case TYP_POINTER:
                TypeModFlags( of_type->of, &ptr_flags );
                ptr_flags &= TF1_BASED;
                if( ptr_flags == TF1_BASED_SELF ) {
                    CErr1( ERR_FUNCTION_CANT_RETURN_BASED_SELF );
                }
                break;
            case TYP_CLASS:
                if( TypeAbstract( of_type ) ) {
                    abstractDiagnose( ERR_FUNCTION_CANT_RETURN_AN_ABSTRACT, of_type );
                }
                break;
            }
            break;
        case TYP_ARRAY:
            flag.memory_model_movement = TRUE;
            /* check base type */
            switch( of_type->id ) {
            case TYP_FUNCTION:
                CErr1( ERR_CANT_HAVE_AN_ARRAY_OF_FUNCTIONS );
                break;
            case TYP_VOID:
                CErr1( ERR_CANT_HAVE_AN_ARRAY_OF_VOID );
                break;
            case TYP_ARRAY:
                if( of_type->u.a.array_size == 0 ) {
                    CErr1( ERR_DIMENSION_REQUIRED );
                }
                break;
            case TYP_POINTER:
                if( of_type->flag & TF1_REFERENCE ) {
                    CErr1( ERR_CANT_HAVE_AN_ARRAY_OF_REFERENCES );
                }
                break;
            case TYP_CLASS:
                if( of_type->u.c.info->zero_array ) {
                    CErr2p( WARN_ZERO_ARRAY_CLASS_USED, of_type );
                }
                break;
            }
            break;
        case TYP_POINTER:
            if( curr_type->flag & TF1_REFERENCE ) {
                /* check reference base type */
                if( mod_type != NULL ) {
                    if( mod_type->flag & TF1_BASED ) {
                        CErr1( ERR_CANT_HAVE_BASED_REFERENCE );
                    }
                }
                switch( of_type->id ) {
                case TYP_POINTER:
                    if( of_type->flag & TF1_REFERENCE ) {
                        CErr1( ERR_CANT_HAVE_REFERENCE_TO_REFERENCE );
                        // prevents problems
                        curr_type->flag &= ~TF1_REFERENCE;
                    }
                    break;
                case TYP_VOID:
                    CErr1( ERR_CANT_HAVE_REFERENCE_TO_VOID );
                    break;
                }
            } else {
                /* check pointer base type */
                switch( of_type->id ) {
                case TYP_POINTER:
                    if( of_type->flag & TF1_REFERENCE ) {
                        CErr1( ERR_CANT_HAVE_POINTER_TO_REFERENCE );
                    }
                    break;
                }
            }
            break;
        case TYP_MEMBER_POINTER:
            /* check member pointer base type */
            if( mod_type != NULL ) {
                if( mod_type->flag & TF1_MEM_MODEL ) {
                    CErr1( ERR_CANT_HAVE_MEMBER_POINTER_TO_MEM_MODEL );
                }
            }
            switch( of_type->id ) {
            case TYP_POINTER:
                if( of_type->flag & TF1_REFERENCE ) {
                    CErr1( ERR_CANT_HAVE_MEMBER_POINTER_TO_REFERENCE );
                }
                break;
            }
            break;
        }
        if( flag.add_type ) {
            if( mod_type != NULL ) {
                if( flag.memory_model_movement ) {
                    if(( mod_type->flag & TF1_MOD_MOVE ) == 0 ) {
                        flag.memory_model_movement = FALSE;
                    }
                }
                if( flag.memory_model_movement ) {
                    /*
                        "int near a[];" parses as "int (near (a[]));"
                        rather than what people want "int ((near a)[]);"
                        but "int near *p;" is OK as "int (near (*p));"
                        (this is only done for array and function declarators)
                    */
                    mod_type = extractMemoryModel( mod_type, &leftover_type );
                    if( leftover_type != NULL ) {
                        prev_type = makeModifiedTypeOf( leftover_type, prev_type );
                    }
                } else {
                    prev_type = makeModifiedTypeOf( mod_type, prev_type );
                    mod_type = NULL;
                }
            } else {
                if( prev_type->id == TYP_FUNCTION ) {
                    if( prev_type->flag & TF1_INTERRUPT ) {
                        /* interrupt implies default far */
                        prev_type = MakeModifiedType( prev_type, TF1_FAR );
                    }
                }
            }
            prev_type = MakeTypeOf( curr_type, prev_type );
            of_type = prev_type;
        }
        curr_type = next_type;
    }
    if( mod_type != NULL ) {
        if( dspec->stg_class & STG_MUTABLE ) {
            mod_type->flag |= TF1_MUTABLE;
        }
        prev_type = makeModifiedTypeOf( mod_type, prev_type );
        mod_type = NULL;
    } else {
        if( dspec->stg_class & STG_MUTABLE ) {
            prev_type = MakeModifiedType( prev_type, TF1_MUTABLE );
        } else {
            if( prev_type->id == TYP_FUNCTION ) {
                if( prev_type->flag & TF1_INTERRUPT ) {
                    /* interrupt implies default far */
                    prev_type = MakeModifiedType( prev_type, TF1_FAR );
                }
            }
        }
    }
    if( fnmod_type != NULL ) {
        flag.OK_to_add_fnmod = TRUE;
        fn_pragma = fnmod_type->u.m.pragma;
        if( fn_pragma != NULL && ! PragmaOKForVariables( fn_pragma ) ) {
            flag.OK_to_add_fnmod = FALSE;
        }
        if( fnmod_type->flag & ~TF1_OK_FOR_VAR ) {
            fnmod_type->flag &= TF1_OK_FOR_VAR;
            flag.OK_to_add_fnmod = FALSE;
        }
        if( ! flag.OK_to_add_fnmod ) {
            if( FunctionDeclarationType( prev_type ) == NULL ) {
                CErr1( ERR_FUNCTION_MOD_ON_VAR );
            }
        }
        if( fn_pragma != NULL || fnmod_type->flag != TF1_NULL ) {
            prev_type = makeModifiedTypeOf( fnmod_type, prev_type );
        } else {
            typeFree( fnmod_type );
            fnmod_type = NULL;
        }
    }
    if( dinfo->id != NULL ) {
        flag.diagnose_sym = FALSE;
        if( dspec->stg_class & STG_TYPEDEF ) {
            if( DefaultIntType( prev_type ) != NULL ) {
                msg_num = ERR_TYPEDEF_ONLY_DEFAULT_INT;
                flag.diagnose_sym = TRUE;
            }
        } else {
            test_type = TypeGetActualFlags( prev_type, &mod_flags );
            if( test_type->id == TYP_VOID ) {
                msg_num = ERR_CANT_HAVE_VOID_VARIABLE;
                flag.diagnose_sym = TRUE;
            }
            mod_flags &= TF1_BASED;
            if( mod_flags != TF1_NULL && mod_flags != TF1_BASED_STRING ) {
                msg_num = ERR_CANT_HAVE_BASED_VARIABLE;
                flag.diagnose_sym = TRUE;
            }
            if( flag.diagnose_sym ) {
                prev_type = TypeError;
            } else {
                prev_type = BindTemplateClass( prev_type, &id_tree->locn, FALSE );
            }
        }
        sym = AllocSymbol();
        sym->sym_type = prev_type;
        if( prev_type->id == TYP_ERROR ) {
            sym->flag |= SF_ERROR;
        }
        setStorageClass( sym, dspec->stg_class );
        SymbolLocnDefine( &(id_tree->locn), sym );
        if( ! PragToggle.unreferenced ) {
            sym->flag |= SF_NO_REF_WARN;
        }
        dinfo->sym = sym;
        dinfo->name = id;
        if( dinfo->scope != NULL ) {
            /* C::id type declaration */
            ScopeQualifyPop();
        }
        if( flag.diagnose_sym ) {
            CErr2p( msg_num, dinfo->name );
        } else {
            // (1) make sure there were decl-specs
            // (2) typedef int I, *PI; is common
            if( ! dspec->is_default && sym->id != SC_TYPEDEF ) {
                SYMBOL prev_sym;

                prev_sym = dspec->prev_sym;
                if( prev_sym != NULL ) {
                    TYPE typ1 = TypedefModifierRemoveOnly( prev_sym->sym_type );
                    TYPE typ2 = TypedefModifierRemoveOnly( prev_type );
                    if( ! TypesIdentical( typ1, typ2 ) ) {
                        if( errWithSymLoc( WARN_DECL_NOT_SAME_TYPE, sym ) & MS_PRINTED ) {
                            CErr2p( INF_OTHER_DEFN, prev_sym );
                        }
                    }
                } else {
                    dspec->prev_sym = sym;
                }
            }
        }
    }
#ifndef NDEBUG
    if( PragDbgToggle.dump_types ) {
        DumpFullType( prev_type );
    }
#endif
    dinfo->type = prev_type;
    if( FunctionDeclarationType( prev_type ) != NULL ) {
        if( dspec->type_defined ) {
            CErr1( ERR_TYPE_CREATED_IN_RETURN );
            CErr1( INF_MISSING_SEMICOLON_AFTER_CLASS_ENUM_DEFN );
        }
    }
    return( dinfo );
}


DECL_INFO *AddMSCVQualifierKludge( type_flag extra_cv, DECL_INFO *dinfo )
/***********************************************************************/
{
    TYPE cv_type;

    if( extra_cv != TF1_NULL ) {
        cv_type = MakeFlagModifier( extra_cv );
        dinfo = prependTypeToDeclarator( dinfo, cv_type );
    }
    return( dinfo );
}


boolean IdenticalClassModifiers( TYPE cmod1, TYPE cmod2 )
/*******************************************************/
{
#ifndef NDEBUG
    {
        TYPE test1 = cmod1;
        TYPE test2 = cmod2;

        DbgVerify(( test1 != NULL ) && ( test2 != NULL ),
                "IdenticalClassModifiers: not a class mod type" );
        if( test1->id == TYP_MODIFIER ) {
            DbgVerify( test1->flag != TF1_NULL,
                    "IdenticalClassModifiers: not a class mod type" );
            test1 = test1->of;
        }
        if( test2->id == TYP_MODIFIER ) {
            DbgVerify( test2->flag != TF1_NULL,
                    "IdenticalClassModifiers: not a class mod type" );
            test2 = test2->of;
        }
        DbgVerify(( test1->id == TYP_FUNCTION ) && ( test2->id == TYP_FUNCTION ),
                "IdenticalClassModifiers: not a class mod type" );
        DbgVerify( test1->u.f.args->num_args == 0,
                "IdenticalClassModifiers: not a class mod type" );
        DbgVerify( test2->u.f.args->num_args == 0,
                "IdenticalClassModifiers: not a class mod type" );
        test1 = test1->of;
        test2 = test2->of;
        DbgVerify(( test1->id == TYP_VOID ) && ( test2->id == TYP_VOID ),
                "IdenticalClassModifiers: not a class mod type" );
    }
#endif
    if( cmod1 == cmod2 ) {
        return( TRUE );
    }
#if _CPU == 8086
    if( cmod1->id == TYP_MODIFIER ) {
        if( cmod2->id != TYP_MODIFIER ) {
            return( FALSE );
        }
        if( cmod1->flag != cmod2->flag ) {
            return( FALSE );
        }
        cmod1 = cmod1->of;
        cmod2 = cmod2->of;
    } else {
        if( cmod2->id == TYP_MODIFIER ) {
            return( FALSE );
        }
    }
#else
    // ignore modifers in comparison
    if( cmod1->id == TYP_MODIFIER ) {
        cmod1 = cmod1->of;
    }
    if( cmod2->id == TYP_MODIFIER ) {
        cmod2 = cmod2->of;
    }
#endif
    if( cmod1->id != TYP_FUNCTION ) {
        return( FALSE );
    }
    if( cmod2->id != TYP_FUNCTION ) {
        return( FALSE );
    }
    if( cmod1->flag != cmod2->flag ) {
        return( FALSE );
    }
    if( cmod1->u.f.pragma != cmod2->u.f.pragma ) {
        return( FALSE );
    }
    return( TRUE );
}

TYPE AbsorbBaseClassModifiers( TYPE class_mod, type_flag *pmflags, type_flag *pfflags, AUX_INFO **pfpragma )
/**********************************************************************************************************/
{
    TYPE keep_type;
    TYPE fn_type;
    type_flag mod_flags;

    DbgVerify( IdenticalClassModifiers( class_mod, class_mod ),
               "AbsorbBaseClassModifiers: invalid class mod" );
    mod_flags = TF1_NULL;
    if( class_mod->id == TYP_MODIFIER ) {
        fn_type = class_mod->of;
#if _CPU == 8086
        mod_flags = class_mod->flag;
        keep_type = class_mod;
#else
        keep_type = fn_type;
#endif
    } else {
        fn_type = class_mod;
        keep_type = fn_type;
    }
    DbgAssert( fn_type->id == TYP_FUNCTION );
    *pmflags = mod_flags;
    *pfflags = fn_type->flag;
    *pfpragma = fn_type->u.f.pragma;
    return( keep_type );
}


TYPE ProcessClassModifiers( TYPE list, type_flag *pmflags, type_flag *pfflags, AUX_INFO **pfpragma )
/**************************************************************************************************/
{
    TYPE fn_type;
    TYPE next;
    type_flag list_flag;
    type_flag fn_flags;
    type_flag mod_flags;
    AUX_INFO *fn_pragma;

    mod_flags = TF1_NULL;
    fn_flags = TF1_NULL;
    fn_pragma = NULL;
    for(; list != NULL; list = next ) {
        next = list->of;
        list_flag = list->flag;
        if( list_flag & TF1_TYP_FUNCTION ) {
            if( list->u.m.pragma != NULL ) {
                fn_pragma = list->u.m.pragma;
            }
            fn_flags |= list_flag;
        } else if( list_flag & TF1_MOD_MEMBER ) {
            mod_flags |= list_flag;
        }
        typeFree( list );
    }
    mod_flags = ( mod_flags & ~TF1_OUTERMOST ) & TF1_MOD_MEMBER;
    fn_flags = ( fn_flags & ~TF1_TYP_FUNCTION ) & TF1_FN_MEMBER;
    *pmflags = mod_flags;
    *pfflags = fn_flags;
    *pfpragma = fn_pragma;
    fn_type = MakeModifiableFunction( GetBasicType( TYP_VOID ), NULL );
    fn_type->flag = fn_flags;
    fn_type->u.f.pragma = fn_pragma;
    fn_type = CheckDupType( fn_type );
    fn_type = MakeModifiedType( fn_type, mod_flags );
    return( fn_type );
}


TYPE MakeClassModDeclSpec( DECL_SPEC *dspec )
/*******************************************/
{
    TYPE type;

    type = NULL;
    if( dspec->ms_declspec != STS_NULL ) {
        type = makeMSDeclSpecType( dspec );
        type->flag |= TF1_OUTERMOST;
        DbgAssert( type->of == NULL );
    }
    PTypeRelease( dspec );
    return( type );
}


TYPE TypeClassModCombine( TYPE list, TYPE e )
/*******************************************/
{
    if( e != NULL ) {
        DbgAssert( e->of == NULL );
        e->of = list;
        list = e;
    }
    return( list );
}


void PTypeClassInstantiationUndo( DECL_SPEC *dspec )
/**************************************************/
{
    PTREE id;

    /* must be kept in synch with PTypeActualTypeName/PTypeClassInstantiation */
#ifndef NDEBUG
    if( dspec->decl_checked ||
        dspec->partial == NULL ||
        ! dspec->class_instantiation ||
        ! dspec->ctor_name ||
        dspec->id == NULL ) {
        CFatal( "template class undo was too late" );
    }
#endif
    dspec->partial = NULL;
    dspec->class_instantiation = FALSE;
    dspec->ctor_name = FALSE;
    id = dspec->id;
    dspec->id = NULL;
    PTreeFreeSubtrees( id );
}

DECL_SPEC *PTypeClassInstantiation( TYPE typ, PTREE id )
/******************************************************/
{
    DECL_SPEC *spec;

    if( typ != NULL ) {
        spec = PTypeActualTypeName( typ, id );
        spec->class_instantiation = TRUE;
    } else {
        spec = NULL;
    }
    
    return( spec );
}

DECL_SPEC *PTypeActualTypeName( TYPE typ, PTREE id )
/**************************************************/
{
    DECL_SPEC *spec;

    spec = makeDeclSpec();
    spec->partial = typ;
    /* could be a constructor name */
    spec->id = id;
    spec->ctor_name = 1;
    return( spec );
}

DECL_SPEC *PTypeActualType( TYPE typ )
/************************************/
{
    DECL_SPEC *spec;

    spec = makeDeclSpec();
    spec->partial = typ;
    return( spec );
}

DECL_SPEC *PTypeTypeid( PTREE tree )
/**********************************/
{
    TYPE type;

    type = tree->type;
    PTreeFree( tree );
    return( PTypeActualType( type ) );
}

DECL_SPEC *PTypeExpr( PTREE expr )
/********************************/
{
    TYPE type;

    expr = AnalyseValueExpr( expr );
    if( expr->op != PT_ERROR ) {
        type = expr->type;
    } else {
        type = TypeError;
    }
    PTreeFreeSubtrees( expr );
    return( PTypeActualType( type ) );
}

DECL_SPEC *PTypeDefault( void )
/*****************************/
{
    DECL_SPEC *spec;

    spec = makeDeclSpec();
    spec->scalar = STM_INT;
    spec->is_default = 1;

    return( spec );
}

void PTypeRelease( DECL_SPEC *dspec )
/***********************************/
{
    PTREE id;

    id = dspec->id;
    if( id != NULL ) {
        PTreeFreeSubtrees( id );
    }
    CarveFree( carveDECL_SPEC, dspec );
}

void PTypePop( DECL_SPEC *dspec )
/*******************************/
{
    if( dspec->linkage != NULL ) {
        LinkagePop();
    }
    PTypeRelease( dspec );
}

TYPE MakePragma( char *name )
/***************************/
{
    TYPE type;
    AUX_INFO *pragma;

    pragma = PragmaLookup( name, M_UNKNOWN );
    if( pragma == NULL ) {
        CErr2p( ERR_PRAGMA_NOT_FOUND, name );
    }
    type = MakePragmaModifier( pragma );
    type->flag |= TF1_TYP_FUNCTION;
    return( type );
}

TYPE MakeIndexPragma( unsigned index )
/************************************/
{
    TYPE type;
    AUX_INFO *pragma;

    pragma = PragmaLookup( NULL, index );
    type = MakePragmaModifier( pragma );
    type->flag |= TF1_TYP_FUNCTION;
    return( type );
}

TYPE MakePragmaModifier( AUX_INFO *pragma )
/*****************************************/
{
    return( makeFullModifier( TF1_NULL, NULL, pragma ) );
}

TYPE DefaultIntType( TYPE type )
/******************************/
{
    TypeStrip( type, ( 1 << TYP_MODIFIER ) );
    if( type == TypeGetCache( TYPC_DEFAULT_INT ) ) {
        return( type );
    }
    return( NULL );
}

TYPE CleanIntType( TYPE type )
/****************************/
{
    if( type == TypeGetCache( TYPC_CLEAN_INT ) ) {
        return( type );
    }
    if( type->id != TYP_SINT ) {
        return( NULL );
    }
    if(( type->flag & TF1_CLEAN ) == 0 ) {
        return( NULL );
    }
    return( type );
}

TYPE SegmentShortType( TYPE type )
/********************************/
{
    if( type == TypeGetCache( TYPC_SEGMENT_SHORT ) ) {
        return( type );
    }
    if( type->id != TYP_USHORT ) {
        return( NULL );
    }
    if(( type->flag & TF1_SEGMENT ) == 0 ) {
        return( NULL );
    }
    return( type );
}


#define MASK_TYPE_INTEGRAL  \
    ( 1 << TYP_ERROR )    | \
    ( 1 << TYP_BOOL  )    | \
    ( 1 << TYP_CHAR  )    | \
    ( 1 << TYP_SCHAR )    | \
    ( 1 << TYP_UCHAR )    | \
    ( 1 << TYP_WCHAR )    | \
    ( 1 << TYP_SSHORT )   | \
    ( 1 << TYP_USHORT )   | \
    ( 1 << TYP_SINT  )    | \
    ( 1 << TYP_UINT  )    | \
    ( 1 << TYP_SLONG )    | \
    ( 1 << TYP_ULONG )    | \
    ( 1 << TYP_SLONG64 )  | \
    ( 1 << TYP_ULONG64 )  | \
    ( 1 << TYP_ENUM  )

TYPE IntegralType( TYPE type )
/****************************/
{
    TypeStripTdMod( type );
    if( ! TypeIdMasked( type, MASK_TYPE_INTEGRAL ) ) {
        type = NULL;
    }
    return type;
}

#define MASK_TYPE_FLOATING  \
    ( 1 << TYP_ERROR )    | \
    ( 1 << TYP_FLOAT  )   | \
    ( 1 << TYP_DOUBLE  )  | \
    ( 1 << TYP_LONG_DOUBLE )

TYPE FloatingType( TYPE type )
/****************************/
{
    TypeStripTdMod( type );
    if( ! TypeIdMasked( type, MASK_TYPE_FLOATING ) ) {
        type = NULL;
    }
    return type;
}

#define MASK_TYPE_SIGNED_INT \
    ( 1 << TYP_SCHAR )    | \
    ( 1 << TYP_SSHORT )   | \
    ( 1 << TYP_SINT  )    | \
    ( 1 << TYP_SLONG )    | \
    ( 1 << TYP_SLONG64 )


boolean SignedIntType( TYPE type )
/********************************/
{
    TypeStripTdModEnumChar( type );
    return TypeIdMasked( type, MASK_TYPE_SIGNED_INT );
}

#define MASK_TYPE_UNSIGNED_INT  \
    ( 1 << TYP_UCHAR )    | \
    ( 1 << TYP_WCHAR )    | \
    ( 1 << TYP_USHORT )   | \
    ( 1 << TYP_UINT  )    | \
    ( 1 << TYP_ULONG )    | \
    ( 1 << TYP_ULONG64 )


boolean UnsignedIntType( TYPE type )
/**********************************/
{
    TypeStripTdModEnumChar( type );
    return TypeIdMasked( type, MASK_TYPE_UNSIGNED_INT );
}

#define MASK_TYPE_ARITH     \
    ( 1 << TYP_ERROR )    | \
    ( 1 << TYP_BOOL  )    | \
    ( 1 << TYP_CHAR  )    | \
    ( 1 << TYP_SCHAR )    | \
    ( 1 << TYP_UCHAR )    | \
    ( 1 << TYP_WCHAR )    | \
    ( 1 << TYP_SSHORT )   | \
    ( 1 << TYP_USHORT )   | \
    ( 1 << TYP_SINT  )    | \
    ( 1 << TYP_UINT  )    | \
    ( 1 << TYP_SLONG )    | \
    ( 1 << TYP_ULONG )    | \
    ( 1 << TYP_SLONG64 )  | \
    ( 1 << TYP_ULONG64 )  | \
    ( 1 << TYP_ENUM  )    | \
    ( 1 << TYP_FLOAT )    | \
    ( 1 << TYP_DOUBLE )   | \
    ( 1 << TYP_LONG_DOUBLE ) | \
    ( 1 << TYP_BITFIELD )

TYPE ArithType( TYPE type )
/*************************/
{
    TypeStripTdMod( type );
    if( ! TypeIdMasked( type, MASK_TYPE_ARITH ) ) {
        type = NULL;
    }
    return type;
}

TYPE FunctionDeclarationType( TYPE type )
/***************************************/
{
    TypeStripTdMod( type );
    if( type->id == TYP_FUNCTION ) {
        return( type );
    }
    return( NULL );
}

#define MASK_TYPE_ENUM        \
    ( 1 << TYP_ENUM )       | \
    ( 1 << TYP_MODIFIER  )  | \
    ( 1 << TYP_TYPEDEF  )

TYPE EnumType( TYPE type )
/************************/
{
    for( ; ; type = type->of ) {
        if( TypeIdMasked( type, MASK_TYPE_ENUM ) ) {
            if( type->id == TYP_ENUM ) break;
        } else {
            type = NULL;
            break;
        }
    }
    return( type );
}

#define MASK_TYPE_BOOL        \
    ( 1 << TYP_BOOL )       | \
    ( 1 << TYP_MODIFIER  )  | \
    ( 1 << TYP_TYPEDEF  )

TYPE BoolType( TYPE type )
/************************/
{
    for( ; ; type = type->of ) {
        if( TypeIdMasked( type, MASK_TYPE_BOOL ) ) {
            if( type->id == TYP_BOOL ) break;
        } else {
            type = NULL;
            break;
        }
    }
    return( type );
}

#define MASK_TYPE_GENERIC     \
    ( 1 << TYP_GENERIC )    | \
    ( 1 << TYP_MODIFIER  )  | \
    ( 1 << TYP_TYPEDEF  )

TYPE GenericType( TYPE type )
/***************************/
{
    for( ; ; type = type->of ) {
        if( TypeIdMasked( type, MASK_TYPE_GENERIC ) ) {
            if( type->id == TYP_GENERIC ) break;
        } else {
            type = NULL;
            break;
        }
    }
    return( type );
}

#define MASK_TYPE_ELABOR      \
    ( 1 << TYP_ENUM    )    | \
    ( 1 << TYP_CLASS   )

TYPE ElaboratableType( TYPE type )
/********************************/
{
    type = TypedefRemove( type );
    if( ! TypeIdMasked( type, MASK_TYPE_ELABOR ) ) {
        type = NULL;
    }
    return type;
}

// 'ClassType' is used by code generator!
TYPE StructType( TYPE type )
/**************************/
{
    if( type != NULL ) {
        TypeStripTdMod( type );
        if( type->id == TYP_CLASS ) {
            return( type );
        }
    }
    return( NULL );
}

TYPE PolymorphicType( TYPE type )
/*******************************/
{
    type = StructType( type );
    if( type != NULL && type->u.c.info->has_vfn ) {
        return( type );
    }
    return( NULL );
}

TYPE StructOpened( TYPE type )
/****************************/
{
    type = StructType( type );
    if( type != NULL && type->u.c.info->opened ) {
        return( type );
    }
    return( NULL );
}

TYPE AbstractClassType( TYPE type )
/*********************************/
{
    type = StructType( type );
    if( type != NULL ) {
        if( verifyAbstractStatus( type ) ) {
            return( type );
        }
    }
    return( NULL );
}

TYPE BasedType( TYPE type )
/*************************/
{
    for( ; ; type = type->of ) {
        if( type == NULL ) {
            break;
        }
        if( ( type->id == TYP_MODIFIER ) && ( type->flag & TF1_BASED ) ) {
            break;
        }
        if( ( type->id != TYP_MODIFIER ) && ( type->id != TYP_TYPEDEF ) ) {
            type = NULL;
            break;
        }
    }
    return( type );
}

TYPE BasedPtrType( TYPE type )
/****************************/
{
    type = PointerTypeEquivalent( type );
    if( type != NULL ) {
        type = BasedType( type->of );
    }
    return( type );
}

TYPE PointerType( TYPE type )
/***************************/
{
    TypeStripTdMod( type );
    if( type->id == TYP_POINTER && ( type->flag & TF1_REFERENCE ) == 0 ) {
        return( type );
    }
    return( NULL );
}

TYPE ArrayType( TYPE type )
/*************************/
{
    TypeStripTdMod( type );
    if( type->id != TYP_ARRAY ) {
        type = NULL;
    }
    return( type );
}

TYPE VoidType( TYPE type )
/************************/
{
    TypeStripTdMod( type );
    if( type->id != TYP_VOID ) {
        type = NULL;
    }
    return( type );
}

TYPE TypedefedType( TYPE type )
/*****************************/
{
    TypeStrip( type, ( 1 << TYP_MODIFIER ) );

    if( type->id == TYP_TYPEDEF ) {
        TypeStripTdMod( type );
    } else if ( type->id != TYP_CLASS ) {
        type = NULL;
    }
    return( type );
}

TYPE ArrayBaseType( TYPE type )
/*****************************/
{
    for(;;) {
        TypeStripTdMod( type );
        if( type->id != TYP_ARRAY ) break;
        type = type->of;
    }
    return( type );
}

#define MASK_TYPE_TDEF_ARR  \
    ( 1 << TYP_TYPEDEF )  | \
    ( 1 << TYP_ARRAY )

TYPE ArrayModifiedBaseType( TYPE type )
/*************************************/
{
    TypeStripTdMod( type );
    for(;;) {
        if( ! TypeIdMasked( type, MASK_TYPE_TDEF_ARR ) ) break;
        type = type->of;
    }
    return( type );
}

TYPE Integral64Type             // GET 64-BIT TYPE IF POSSIBLE
    ( TYPE type )               // - type
{
    type = TypedefModifierRemoveOnly( type );
    switch( type->id ) {
      case TYP_ENUM :
        if( NULL == Integral64Type( type->of ) ) {
            type = NULL;
            break;
        }
        // drops thru
      case TYP_ULONG64 :
      case TYP_SLONG64 :
        break;
      default :
        type = NULL;
        break;
    }
    return type;
}

#define MASK_TYPE_INT64  \
    ( 1 << TYP_SLONG64 )  | \
    ( 1 << TYP_ULONG64 )


TYPE CheckBitfieldType( DECL_SPEC *dspec, target_long width )
/***********************************************************/
{
    TYPE base_type;
    TYPE i64_type;

    if( dspec->is_default && ! dspec->diagnosed ) {
        CErr1( ERR_BITFIELD_NO_BASE_TYPE );
        dspec->diagnosed = TRUE;
    }
    figureOutStgClass( dspec );
    if( dspec->stg_class & ~(STG_MUTABLE) ) {
        CErr1( ERR_BITFIELD_STG_CLASS );
        dspec->stg_class &= STG_MUTABLE;
    }
    if( dspec->specifier & ~(STY_CONST|STY_VOLATILE) ) {
        CErr1( ERR_BITFIELD_QUALIFIER );
        dspec->specifier &= ~(STY_CONST|STY_VOLATILE);
    }
    figureOutDSpec( dspec );
    base_type = dspec->partial;
    TypeStripTdMod( base_type );
    if( IntegralType( base_type ) == NULL ) {
        CErr1( ERR_BITFIELD_BAD_BASE_TYPE );
        base_type = GetBasicType( TYP_ULONG );
        dspec->partial = base_type;
    } else {
        i64_type = base_type;
        TypeStripTdModEnumChar( i64_type );
        if( TypeIdMasked( i64_type, MASK_TYPE_INT64 ) ) {
            if( width > (TARGET_LONG*TARGET_BITS_CHAR) ) {
                CErr1( ERR_64BIT_BITFIELD );
            } else {
                if( SignedIntType( i64_type ) ) {
                    base_type = GetBasicType( TYP_SLONG );
                } else {
                    base_type = GetBasicType( TYP_ULONG );
                }
                dspec->partial = base_type;
            }
        }
    }
    return( base_type );
}

PTREE ProcessBitfieldId( PTREE id_tree )
/**************************************/
{
    if( id_tree->op != PT_ID ) {
        id_tree = CutAwayQualification( id_tree );
        CErr2p( ERR_INVALID_BITFIELD_ID, id_tree->u.id.name );
    } else if( id_tree->cgop != CO_NAME_NORMAL ) {
        CErr2p( ERR_INVALID_BITFIELD_ID, id_tree->u.id.name );
    }
    return( id_tree );
}

TYPE MakeBitfieldType( DECL_SPEC *dspec, TYPE base_type, unsigned start, unsigned width )
/***************************************************************************************/
{
    TYPE type;
    type_flag flag;

    if( CleanIntType( base_type ) != NULL ) {
        /* signed-ness of an unadorned int is implementation defined */
        base_type = GetBasicType( TYP_UINT );
    }
    type = MakeType( TYP_BITFIELD );
    type->u.b.field_start = start;
    type->u.b.field_width = width;
    type = MakeTypeOf( type, base_type );
    TypeModFlags( dspec->partial, &flag );
    flag &= TF1_CONST | TF1_VOLATILE;
    if( dspec->stg_class & STG_MUTABLE ) {
        flag |= TF1_MUTABLE;
    }
    type = MakeModifiedType( type, flag );
    return( type );
}

TYPE MakePointerTo( TYPE base )
/*****************************/
{
    for( ; base->id == TYP_TYPEDEF; base = base->of );
    return( MakeTypeOf( MakeType( TYP_POINTER ), base ) );
}

TYPE MakeFarPointerToNear( TYPE base )
/************************************/
{
    TYPE ptr_type;

#ifndef NDEBUG
    {
        type_flag mod;

        TypeModFlagsEC( base, &mod );
        if( ! ( mod & (TF1_FAR|TF1_HUGE) ) ) {
            CFatal( "dual far/near pointer must point to a far object" );
        }
    }
#endif
    for( ; base->id == TYP_TYPEDEF; base = base->of );
    ptr_type = MakeType( TYP_POINTER );
    ptr_type->flag |= TF1_FAR_BUT_NEAR;
    return( MakeTypeOf( ptr_type, base ) );
}

TYPE MakeReferenceTo( TYPE base )
/*******************************/
{
    TYPE new_type;

    for( ; base->id == TYP_TYPEDEF; base = base->of );
    new_type = MakeType( TYP_POINTER );
    new_type->flag |= TF1_REFERENCE;
    return( MakeTypeOf( new_type, base ) );
}

TYPE MakeConstReferenceTo(      // MAKE A CONST REFERENCE TO A TYPE
/*************************/
    TYPE type )                 // - the type
{
    return MakeReferenceTo( MakeModifiedType( type, TF1_CONST ) );
}

TYPE GetReferenceTo( TYPE base )
/******************************/
{
    if( TypeReference( base ) == NULL ) {
        base = MakeReferenceTo( base );
    }
    return( base );
}

TYPE MakeTypedefOf( TYPE base, SCOPE scope, SYMBOL sym )
/******************************************************/
{
    TYPE new_type;

    new_type = MakeType( TYP_TYPEDEF );
    new_type->u.t.sym = sym;
    new_type->u.t.scope = scope;
    return( MakeTypeOf( new_type, base ) );
}

/* these routines check the decl-spec usage to verify that it makes sense */

void CheckFunctionDSpec( DECL_SPEC *dspec )
/*****************************************/
{
    // a function may have:
    //   extern static
    figureOutStgClass( dspec );
    if( dspec->stg_class & ~( STG_EXTERN | STG_STATIC ) ) {
        CErr1( ERR_INVALID_STG_CLASS_FOR_FUNC );
    }
}

void CheckDeclarationDSpec( DECL_SPEC *dspec, SCOPE scope )
/*********************************************************/
{
    // declarations in file-scope or block-scope may have:
    //  file-scope
    //      typedef, extern, static
    //  block-scope
    //      typedef, extern, static, auto, register
    figureOutStgClass( dspec );
    if( dspec->stg_class & (STG_TYPEDEF|STG_EXTERN|STG_STATIC|STG_REGISTER|STG_AUTO) ) {
        if( dspec->stg_class & (STG_REGISTER|STG_AUTO) ) {
            if( ScopeType( scope, SCOPE_FILE ) ) {
                CErr1( ERR_INVALID_STG_CLASS_FOR_FILE_SCOPE );
            }
        }
    } else if( dspec->stg_class & STG_MUTABLE ) {
        CErr1( ERR_INVALID_USE_OF_MUTABLE );
        dspec->stg_class &= ~STG_MUTABLE;
    }
}

DECL_SPEC *CheckArgDSpec( DECL_SPEC *dspec )
/******************************************/
{
    dspec->arg_declspec = TRUE;
    figureOutStgClass( dspec );
    if( dspec->stg_class & ~( STG_AUTO | STG_REGISTER ) ) {
        CErr1( ERR_INVALID_STG_CLASS_FOR_PARM );
        dspec->stg_class &= ( STG_AUTO | STG_REGISTER );
    }
    if( dspec->type_defined ) {
        if( ! dspec->generic ) {
            /* a type can be defined in a template argument */
            CErr1( ERR_TYPE_CREATED_IN_ARG );
        }
    }
    return( dspec );
}


derived_status TypeDerived( TYPE type_derived, TYPE type_potential_base )
/***********************************************************************/
{
    SCOPE derived;              // - scope for potential derived
    SCOPE potential_base;       // - scope for potential base
    derived_status retn;        // - return

    derived = TypeScope( type_derived );
    if( derived == NULL ) {
        retn = DERIVED_NO;
    } else {
        potential_base = TypeScope( type_potential_base );
        if( potential_base == NULL ) {
            retn = DERIVED_NO;
        } else {
            retn = ScopeDerived( derived, potential_base );
        }
    }
    return retn;
}

TYPE TypeCommonBase( TYPE class_1, TYPE class_2 )
/***********************************************/
{
    SCOPE scope_1;
    SCOPE scope_2;
    SCOPE scope;

    scope_1 = TypeScope( class_1 );
    if( scope_1 == NULL ) {
        return( NULL );
    }
    scope_2 = TypeScope( class_2 );
    if( scope_2 == NULL ) {
        return( NULL );
    }
    scope = ScopeCommonBase( scope_1, scope_2 );
    if( scope == NULL ) {
        return( NULL );
    }
    return( ScopeClass( scope ) );
}

AUX_INFO *TypeHasPragma( TYPE type )
/**********************************/
{
    AUX_INFO *pragma;

    for( ; type != NULL; type = type->of ) {
        if( type->id == TYP_MODIFIER ) {
            pragma = type->u.m.pragma;
            if( pragma != NULL ) {
                return( pragma );
            }
            continue;
        }
        if( type->id != TYP_TYPEDEF ) {
            break;
        }
    }
    if( type->id == TYP_FUNCTION ) {
        return( type->u.f.pragma );
    }
    return( NULL );
}

#define defaultMemoryFlag( t ) ( (t)->id == TYP_FUNCTION    \
                               ? defaultFunctionMemFlag     \
                               : defaultDataMemFlag )

type_flag DefaultMemoryFlag(    // GET DEFAULT MEMORY FLAG FOR A TYPE
    TYPE type )                 // - unmodified type
{
    return defaultMemoryFlag( type );
}

TYPE TypeModExtract(            // EXTRACT MODIFIER INFORMATION
    TYPE type,                  // - input type
    type_flag *flags,           // - addr[ modifier flags]
    void *baser,                // - addr[__based element]
    type_exclude mask )         // - exclusions
{
    void **a_baser = baser;     // - addr[__based element]
    type_flag flag;             // - accumulated flags
    type_flag mod_flag;         // - current modifier's flags

    *a_baser = NULL;
    flag = TF1_NULL;
    for( ; type != NULL; type = type->of ) {
        switch( type->id ) {
          case TYP_MODIFIER :
            mod_flag = type->flag;
            flag |= mod_flag;
            if( mod_flag & TF1_BASED ) {
                *a_baser = type->u.m.base;
            }
            continue;
          case TYP_TYPEDEF :
            continue;
          case TYP_BOOL :
          case TYP_CHAR :
          case TYP_ENUM :
            if(( mask & TC1_NOT_ENUM_CHAR ) == 0 ) {
                type = type->of;
            }
            // drops thru
          default :
            if( mask & TC1_NOT_MEM_MODEL ) {
                if( (flag & TF1_MEM_MODEL) == 0 ) {
                    flag |= defaultMemoryFlag( type );
                }
            }
            break;
        }
        break;
    }
    *flags = flag;
    return( type );
}

TYPE TypeGetActualFlags( TYPE type, type_flag *flags )
/****************************************************/
{
    type_flag flag;             // - accumulated flags
    TYPE elem_type;

    flag = TF1_NULL;
    for( ; type != NULL; type = type->of ) {
        switch( type->id ) {
          case TYP_MODIFIER :
            flag |= type->flag;
            continue;
          case TYP_TYPEDEF :
            continue;
          default :
            break;
        }
        break;
    }
    // Note: Any cv-qualifiers applied to an array type affect the
    // array element type, not the array type (3.9.3 (2))
    for( elem_type = type; elem_type != NULL; elem_type = elem_type->of ) {
        if( elem_type->id == TYP_MODIFIER ) {
            flag |= elem_type->flag;
        } else if( ( elem_type->id != TYP_TYPEDEF )
                && ( elem_type->id != TYP_ARRAY ) ) {
            break;
        }
    }
    *flags = flag;
    return( type );
}


TYPE TypeModFlagsEC(            // GET MODIFIER FLAGS, UNMODIFIED TYPE
                                // don't skip char and enum
    TYPE type,                  // - type
    type_flag *flags )          // - ptr( flag )
{
    type_flag flag;             // - accumulated flags

    flag = TF1_NULL;
    for( ; type != NULL; type = type->of ) {
        switch( type->id ) {
          case TYP_MODIFIER :
            flag |= type->flag;
            continue;
          case TYP_TYPEDEF :
            continue;
          default :
            if( (flag & TF1_MEM_MODEL) == 0 ) {
                flag |= defaultMemoryFlag( type );
            }
            break;
        }
        break;
    }
    *flags = flag;
    return( type );
}


TYPE TypeModFlagsBaseEC(        // GET MODIFIER FLAGS & BASE, UNMODIFIED TYPE
                                // don't skip char and enum
    TYPE type,                  // - type
    type_flag *flags,           // - ptr( flag )
    void **a_baser )            // - ptr( base )
{
    type_flag flag;             // - accumulated flags
    type_flag mod_flag;         // - current modifier's flags

    *a_baser = NULL;
    flag = TF1_NULL;
    for( ; type != NULL; type = type->of ) {
        switch( type->id ) {
          case TYP_MODIFIER :
            mod_flag = type->flag;
            flag |= mod_flag;
            if( mod_flag & TF1_BASED ) {
                *a_baser = type->u.m.base;
            }
            continue;
          case TYP_TYPEDEF :
            continue;
          default :
            if( (flag & TF1_MEM_MODEL) == 0 ) {
                flag |= defaultMemoryFlag( type );
            }
            break;
        }
        break;
    }
    *flags = flag;
    return( type );
}


TYPE TypeModFlags(              // GET MODIFIER FLAGS, UNMODIFIED TYPE
                                // skip char and enum
    TYPE type,                  // - type
    type_flag *flags )          // - ptr( flag )
{
    type_flag flag;             // - accumulated flags

    flag = TF1_NULL;
    for( ; type != NULL; type = type->of ) {
        switch( type->id ) {
          case TYP_MODIFIER :
            flag |= type->flag;
            continue;
          case TYP_TYPEDEF :
            continue;
          case TYP_BOOL :
          case TYP_CHAR :
          case TYP_ENUM :
            type = type->of;
            // drops thru
          default :
            if( (flag & TF1_MEM_MODEL) == 0 ) {
                flag |= defaultMemoryFlag( type );
            }
            break;
        }
        break;
    }
    *flags = flag;
    return( type );
}

type_flag TypeExplicitModFlags( TYPE type )
/*****************************************/
{
    type_flag flag = TF1_NULL;
    for( ; ( type != NULL ) && ( type->id == TYP_MODIFIER );
         type = type->of ) {
        flag |= type->flag;
    }

    return flag;
}


type_flag BaseTypeClassFlags( TYPE type )
/***************************************/
{
    type_flag flags;

    flags = TF1_NULL;
    if( type != NULL ) {
        type = TypeModFlagsEC( type, &flags );
        if( type->id == TYP_POINTER ) {         /* covers T* and T& */
            TypeModFlagsEC( type->of, &flags );
        }
    }
    return( flags & TF1_THIS_MASK );
}


TYPE TypeReference(             // GET REFERENCE TYPE
    TYPE type )                 // - the type
{
    if( type != NULL ) {
        type = TypedefModifierRemoveOnly( type );
        if( ( type->id == TYP_POINTER ) && ( type->flag & TF1_REFERENCE ) ) {
            type = type->of;
        } else {
            type = NULL;
        }
    }
    return( type );
}

PTREE MakeConstructorId( DECL_SPEC *dspec )
/*****************************************/
{
    PTREE id_tree;
    SCOPE scope;

    id_tree = NULL;
    if( dspec->ctor_name ) {
        scope = dspec->scope;
        if( scope == NULL || ScopeType( scope, SCOPE_CLASS ) ) {
            dspec->scope = NULL;
            dspec->partial = NULL;
            id_tree = dspec->id;
            dspec->id = NULL;
            id_tree = CheckScopedId( id_tree );
        }
    }
    return( id_tree );
}

static void freeTypeList( TYPE type )
{
    TYPE next;
    TYPE curr;

    for( curr = type; curr != NULL; curr = next ) {
        next = curr->of;
        typeFree( curr );
    }
}

void FreeDeclInfo( DECL_INFO *dinfo )
/***********************************/
{
    if( dinfo == NULL ) {
        return;
    }
    if( dinfo->id != NULL ) {
        PTreeFreeSubtrees( dinfo->id );
    }
    if( dinfo->type == NULL ) {
        freeTypeList( dinfo->list );
    }
    if( dinfo->sym != NULL && dinfo->sym_used == FALSE ) {
        FreeSymbol( dinfo->sym );
    }
    if( dinfo->parms != NULL ) {
        freeDeclList( &(dinfo->parms) );
    }
    if( dinfo->defarg_expr != NULL ) {
        PTreeFreeSubtrees( dinfo->defarg_expr );
    }
    if( dinfo->body != NULL ) {
        RewriteFree( dinfo->body );
    }
    if( dinfo->mem_init != NULL ) {
        RewriteFree( dinfo->mem_init );
    }
    if( dinfo->defarg_rewrite != NULL ) {
        RewriteFree( dinfo->defarg_rewrite );
    }
    CarveFree( carveDECL_INFO, dinfo );
}

static PTREE verifyQualifiedId( DECL_SPEC *dspec, PTREE id, SCOPE *scope, unsigned *pinfo )
{
    NAME name;
    TYPE class_type;
    TYPE scope_class_type;
    PTREE name_tree;
    PTREE scope_tree;
    SCOPE check_scope;
    SCOPE qualifying_scope;
    struct {
        unsigned not_OK : 1;
        unsigned strip_qualification : 1;
    } flag;

    *scope = NULL;
    *pinfo = IDI_NULL;
    name_tree = nameOfId( id );
    if( name_tree == id ) {
        return( name_tree );
    }
    DbgAssert( id->cgop == CO_COLON_COLON );
    /* we have a scope qualified id */
    scope_tree = id->u.subtree[0];
    if( scope_tree == NULL ) {
        /* we have ::<id> */
        if( dspec->specifier & STY_FRIEND ) {
            if( ScopeType( GetCurrScope(), SCOPE_CLASS ) ) {
                /* friend int ::foo( int ); */
                return( id );
            }
        }
        CErr1( ERR_CANNOT_USE_QUALIFIED_DECLARATOR );
        return( CutAwayQualification( id ) );
    }
    flag.strip_qualification = FALSE;
    scope_class_type = scope_tree->type;
    if( scope_class_type == NULL ) {
        // namespace qualification
        DbgAssert( scope_tree->op == PT_TYPE );
        qualifying_scope = scope_tree->u.type.scope;
        if( ScopeId( GetCurrScope() ) != SCOPE_FILE ) {
            flag.not_OK = TRUE;
            class_type = ScopeClass( GetCurrScope() );
            if( class_type != NULL ) {
                /* we're in a class scope */
                if( dspec->specifier & STY_FRIEND ) {
                    /* scope qualified friends are OK */
                    flag.not_OK = FALSE;
                }
            }
            if( flag.not_OK ) {
                CErr1( ERR_CANNOT_USE_NAMESPACE_QUALIFIED_DECLARATOR );
                return( CutAwayQualification( id ) );
            }
        }
    } else {
        if( scope_class_type->flag & TF1_UNBOUND ) {
            /* we are parsing a class template declaration */
            *pinfo |= IDI_CLASS_TEMPLATE_MEMBER;
            return( id );
        }
        if( ! ScopeEquivalent( GetCurrScope(), SCOPE_FILE ) ) {
            flag.not_OK = TRUE;
            class_type = ScopeClass( ScopeNearestNonTemplate( GetCurrScope() ) );
            if( class_type != NULL ) {
                /* we're in a class scope */
                if( dspec->specifier & STY_FRIEND ) {
                    /* scope qualified friends are OK */
                    flag.not_OK = FALSE;
                } else {
                    if( class_type == scope_class_type ) {
                        /* scope qualification within your own class is OK */
                        flag.not_OK = FALSE;
                        flag.strip_qualification = TRUE;
                    }
                }
            }
            if( flag.not_OK ) {
                CErr1( ERR_CANNOT_USE_QUALIFIED_DECLARATOR );
                return( CutAwayQualification( id ) );
            }
        }
        qualifying_scope = scope_class_type->u.c.scope;
        name = name_tree->u.id.name;
        /* must check destructors early on because all dtors map to the same name */
        if( name == CppDestructorName() ) {
            check_scope = TypeScope( name_tree->type );
            if( check_scope != qualifying_scope ) {
                CErr2p( ERR_NOT_A_MEMBER_NAME, name );
                return( CutAwayQualification( id ) );
            }
        }
    }
    if( flag.strip_qualification ) {
        id = CutAwayQualification( id );
    } else {
        ScopeQualifyPush( qualifying_scope, qualifying_scope );
        *scope = qualifying_scope;
    }
    return( id );
}

DECL_INFO *MakeDeclarator( DECL_SPEC *dspec, PTREE id )
/*****************************************************/
{
    DECL_INFO *dinfo;
    SCOPE scope;
    unsigned id_info;

    id = verifyQualifiedId( dspec, id, &scope, &id_info );
    dinfo = makeDeclInfo( id );
    dinfo->scope = scope;
    if( id_info & IDI_CLASS_TEMPLATE_MEMBER ) {
        dinfo->template_member = TRUE;
    }
    return( dinfo );
}

DECL_INFO *MakeAbstractDeclarator( TYPE type )
/********************************************/
{
    DECL_INFO *new_declarator;

    new_declarator = MakeDeclarator( NULL, NULL );
    if( type != NULL ) {
        new_declarator = prependTypeToDeclarator( new_declarator, type );
    }
    return( new_declarator );
}

DECL_INFO *AddExplicitParms( DECL_INFO *dinfo, DECL_INFO *args )
/**************************************************************/
{
    if( dinfo->explicit_parms ) {
        FreeArgs( args );
    } else {
        dinfo->parms = args;
        dinfo->explicit_parms = TRUE;
    }
    return( dinfo );
}

DECL_INFO *AddDeclarator( DECL_INFO *dinfo, TYPE type )
/*****************************************************/
{
    return( prependTypeToDeclarator( dinfo, type ) );
}

static DECL_INFO *typeDeclInfo( TYPE type )
{
    DECL_INFO *new_declarator;

    new_declarator = MakeDeclarator( NULL, NULL );
    new_declarator->type = type;
    return( new_declarator );
}

DECL_INFO *DeclSpecDeclarator( DECL_SPEC *dspec )
/***********************************************/
{
    DECL_INFO *dinfo;

    dinfo = typeDeclInfo( declSpecType( dspec ) );
    if( dspec->generic ) {
        dinfo->generic_sym = dspec->typedef_defined;
        dinfo->name = dspec->name;
    }
    return( dinfo );
}

static void noDuplicateNames( DECL_INFO *head, DECL_INFO *check )
{
    SYMBOL arg_sym;
    NAME check_name;
    DECL_INFO *curr;

    check_name = check->name;
    if( check_name == NULL ) {
        return;
    }
    RingIterBeg( head, curr ) {
        if( curr->name == check_name ) {
            CErr2p( ERR_DUPLICATE_ARG_NAME, check_name );
            check->name = NameDummy();
            arg_sym = check->sym;
            if( arg_sym != NULL ) {
                arg_sym->flag |= SF_REFERENCED;
            }
            return;
        }
    } RingIterEnd( curr )
}

DECL_INFO *AddArgument( DECL_INFO *dinfo, DECL_INFO *new_dinfo )
/**************************************************************/
{
    SYMBOL parm_sym;
    TYPE parm_type;

    noDuplicateNames( dinfo, new_dinfo );
    RingAppend( &dinfo, new_dinfo );
    parm_type = adjustParmType( new_dinfo->type );
    new_dinfo->type = parm_type;
    parm_sym = dinfo->sym;
    if( parm_sym != NULL ) {
        parm_sym->sym_type = parm_type;
    }
    return( dinfo );
}

DECL_INFO *AddEllipseArg( DECL_INFO *dinfo )
/******************************************/
{
    DECL_INFO *new_dinfo;

    new_dinfo = typeDeclInfo( GetBasicType( TYP_DOT_DOT_DOT ) );
    return( AddArgument( dinfo, new_dinfo ) );
}

PTREE TypeDeclarator( DECL_INFO *dinfo )
/**************************************/
{
    TYPE type;

    type = dinfo->type;
    FreeDeclInfo( dinfo );
    return( PTreeType( type ) );
}

static TYPE functionReduce( TYPE type, unsigned arg_index )
{
    TYPE fn_type;
    TYPE new_fn_type;
    TYPE mod_list;
    arg_list *old_args;
    arg_list *args;
    unsigned i;

    /* 'type' is {TYP_MODIFIER}*TYP_FUNCTION (checked by SymIsFunction) */
    fn_type = type;
    TypeStripTdMod( fn_type );
    mod_list = removeModifiers( type, fn_type );
    old_args = fn_type->u.f.args;
    args = AllocArgListPerm( arg_index );
    args->except_spec = old_args->except_spec;
    args->qualifier = old_args->qualifier;
    for( i = 0; i < arg_index; ++i ) {
        args->type_list[i] = old_args->type_list[i];
    }
    new_fn_type = MakeType( TYP_FUNCTION );
    new_fn_type->flag = fn_type->flag;
    new_fn_type->u.f.args = args;
    new_fn_type->u.f.pragma = fn_type->u.f.pragma;
    new_fn_type = MakeTypeOf( new_fn_type, fn_type->of );
    return( replaceModifiers( mod_list, new_fn_type ) );
}

static SYMBOL tryInsertion( SCOPE scope, SYMBOL sym, NAME name )
{
    DECL_INFO *tmp_dinfo;
    SYMBOL decl_sym;

    tmp_dinfo = makeDeclInfo( NULL );
    tmp_dinfo->sym = sym;
    tmp_dinfo->name = name;
    InsertDeclInfo( scope, tmp_dinfo );
    decl_sym = tmp_dinfo->sym;
    FreeDeclInfo( tmp_dinfo );
    return( decl_sym );
}

SYMBOL InsertSymbol( SCOPE scope, SYMBOL sym, NAME name )
/*******************************************************/
{
    return( tryInsertion( scope, sym, name ) );
}

static boolean defaultArgCantExist( SCOPE scope, DECL_INFO *dinfo, SYMBOL sym )
{
    SYMBOL decl_sym;
    SYMBOL base_sym;

    decl_sym = tryInsertion( scope, sym, dinfo->name );
    if( decl_sym != sym ) {
        /* declaration returned a previous SYMBOL! */
        if( decl_sym->id != SC_DEFAULT ) {
            /*
              leaving out the default arg in a call would cause an ambiguity
              with another base symbol of the same name
            */
            CErr2p( WARN_DEFAULT_ARG_HITS_ANOTHER_SYM, decl_sym );
            return( FALSE );
        }
        base_sym = SymDefaultBase( decl_sym );
        if( base_sym == dinfo->sym ) {
            /* previously defined default argument of this base symbol */
            CErr2p( ERR_DEFAULT_ARG_REDEFINED, decl_sym );
        } else {
            /* previously defined default argument of another base symbol */
            CErr2p( WARN_DEFAULT_ARG_HITS_ANOTHER_ARG, decl_sym );
        }
        return( FALSE );
    }
    return( TRUE );
}

static SYMBOL defaultArgMustExist( SCOPE scope, DECL_INFO *dinfo, SYMBOL sym )
{
    SYMBOL decl_sym;
    SYMBOL base_sym;

    decl_sym = tryInsertion( scope, sym, dinfo->name );
    if( decl_sym != sym ) {
        /* declaration returned a previous SYMBOL! */
        if( decl_sym->id != SC_DEFAULT ) {
            /*
              leaving out the default arg in a call would cause an ambiguity
              with another base symbol of the same name
            */
            CErr2p( WARN_DEFAULT_ARG_HITS_ANOTHER_SYM, decl_sym );
            return( NULL );
        }
        base_sym = SymDefaultBase( decl_sym );
        if( base_sym == dinfo->sym ) {
            /* previously defined default argument of this base symbol */
            return( decl_sym );
        }
        /* previously defined default argument of another base symbol */
        CErr2p( WARN_DEFAULT_ARG_HITS_ANOTHER_ARG, base_sym );
    } else {
        /* default argument was never defined for this base symbol */
        CErr2p( ERR_DEFAULT_ARGS_MISSING_FOR_SYM, sym );
    }
    return( NULL );
}


/*
    Default argument nomenclature:

        defaultProto    -- SC_DEFAULT sym with same name as function
        defaultValue    -- function that is { return expr; }
*/
static SYMBOL makeDefaultProto( DECL_INFO *dinfo, unsigned arg_index, DECL_INFO *parm )
{
    TYPE base_type;
    SYMBOL base_sym;
    SYMBOL sym;
    TOKEN_LOCN *defarg_locn;

    defarg_locn = &(parm->init_locn);
    if( defarg_locn->src_file == NULL ) {
        defarg_locn = NULL;
    }
    base_sym = dinfo->sym;
    base_type = base_sym->sym_type;
    sym = AllocSymbol();
    sym->id = SC_DEFAULT;
    sym->sym_type = functionReduce( base_type, arg_index );
    sym->thread = base_sym;
    sym->u.defarg_info = NULL;
    SymbolLocnDefine( defarg_locn, sym );
    parm->proto_sym = sym;
    return( sym );
}

static unsigned declareDefaultProtos( SCOPE scope, DECL_INFO *dinfo )
{
    unsigned control;
    unsigned arg_index;
    boolean is_template;
    DECL_INFO *curr;
    SYMBOL def_arg_sym;
    SYMBOL head;
    SYMBOL *prev_arg;

    control = DA_NULL;
    head = NULL;
    prev_arg = &head;
    arg_index = 0;
    is_template = SymIsFunctionTemplateModel( dinfo->sym );
    RingIterBeg( dinfo->parms, curr ) {
        if( curr->type->id == TYP_DOT_DOT_DOT ) break;
        def_arg_sym = NULL;
        if( curr->has_defarg ) {
            def_arg_sym = makeDefaultProto( dinfo, arg_index, curr );
            if( ! defaultArgCantExist( scope, dinfo, def_arg_sym ) ) {
                stripDefArg( curr );
                def_arg_sym = NULL;
            } else {
                if( curr->defarg_rewrite != NULL ) {
                    if( is_template ) {
                        RewriteFree( curr->defarg_rewrite );
                        curr->defarg_rewrite = NULL;
                    }
                    control |= DA_REWRITES;
                }
            }
            control |= DA_DEFARGS_PRESENT;
        } else {
            if( control & DA_DEFARGS_PRESENT ) {
                def_arg_sym = makeDefaultProto( dinfo, arg_index, curr );
                def_arg_sym = defaultArgMustExist( scope, dinfo, def_arg_sym );
            }
        }
        if( def_arg_sym != NULL ) {
            *prev_arg = def_arg_sym;
            prev_arg = &(def_arg_sym->thread);
        }
        ++arg_index;
    } RingIterEnd( curr )
    *prev_arg = dinfo->sym;
    dinfo->proto_sym = head;
    return( is_template ? 0 : control );
}

static void deferDefaultRewrites( DECL_INFO *dinfo )
{
    DECL_INFO *curr;
    DECL_INFO *fn_dinfo;
    DECL_INFO *parm_dinfo;

    fn_dinfo = makeDeclInfo( NULL );
    fn_dinfo->sym = dinfo->sym;
    fn_dinfo->sym_used = TRUE;
    RingIterBeg( dinfo->parms, curr ) {
        if( curr->type->id == TYP_DOT_DOT_DOT ) break;
        parm_dinfo = makeDeclInfo( NULL );
        parm_dinfo->type = curr->type;
        if( curr->has_defarg ) {
            DbgAssert( curr->defarg_expr == NULL );
            DbgAssert( curr->defarg_rewrite != NULL );
            parm_dinfo->has_defarg = TRUE;
            parm_dinfo->defarg_rewrite = curr->defarg_rewrite;
            parm_dinfo->proto_sym = curr->proto_sym;
            TokenLocnAssign( parm_dinfo->init_locn, curr->init_locn );
            curr->defarg_rewrite = NULL;
            stripDefArg( curr );
        }
        RingAppend( &(fn_dinfo->parms), parm_dinfo );
    } RingIterEnd( curr )
    ClassStoreDefArg( fn_dinfo );
}

void DeclareDefaultArgs( SCOPE scope, DECL_INFO *dinfo )
/******************************************************/
{
    unsigned dp_control;

    if( dinfo->parms != NULL ) {
        dp_control = declareDefaultProtos( scope, dinfo );
        if( dp_control & DA_DEFARGS_PRESENT ) {
            if( dp_control & DA_REWRITES ) {
                // pack things away to be evaluated
                DbgAssert( scope == SymScope( dinfo->sym ) );
                deferDefaultRewrites( dinfo );
            } else {
                DefineDefaultValues( dinfo );
            }
        }
    }
}

static SCOPE undefdFriendScope( void )
{
    return( ScopeNearestFile( GetCurrScope() ) );
}

static SCOPE figureOutFriendScope( SCOPE scope, DECL_INFO *dinfo )
{
    SEARCH_RESULT *result;
    SYMBOL sym_list;
    FNOV_RESULT check;

    if( dinfo->scope == NULL && dinfo->id->op != PT_ID ) {
        /* friend int ::foo( int ); */
        scope = GetFileScope();
    }
    result = ScopeFindNakedFriend( scope, dinfo->name );
    if( result == NULL ) {
        scope = undefdFriendScope();
    } else {
        scope = result->scope;
        sym_list = result->sym_name->name_syms;
        if( sym_list == NULL ) {
            scope = undefdFriendScope();
        } else {
            check = IsOverloadedFuncDistinct( &sym_list
                                            , dinfo->sym
                                            , dinfo->name
                                            , FNC_DEFAULT );
            if( check != FNOV_EXACT_MATCH ) {
                scope = undefdFriendScope();
            } else {
                if( ScopeAmbiguousSymbol( result, sym_list ) ) {
                    scope = undefdFriendScope();
                }
            }
        }
        ScopeFreeResult( result );
    }
    dinfo->friend_scope = scope;
    return( scope );
}

static boolean memberCheck( SCOPE scope, DECL_INFO *dinfo, TYPE *scope_type )
{
    SCOPE id_scope;
    TYPE class_type;

    *scope_type = NULL;
    id_scope = dinfo->scope;
    if( id_scope != NULL ) {
        scope = id_scope;
    } else if( dinfo->friend_fn ) {
        scope = figureOutFriendScope( scope, dinfo );
    }
    class_type = ScopeClass( scope );
    if( class_type != NULL ) {
        *scope_type = class_type;
        return( TRUE );
    }
    return( FALSE );
}

static unsigned howManyArgs( boolean non_static_member, TYPE fn_type )
{
    unsigned num_args;

    num_args = 0;
    if( non_static_member ) {
        ++num_args;
    }
    num_args += fn_type->u.f.args->num_args;
    return( num_args );
}

static boolean isVoidPtr( TYPE type )
{
    type = TypedefRemove( type );
    if( type->id == TYP_POINTER ) {
        if(( type->flag & TF1_REFERENCE ) == 0 ) {
            if( isVoid( type->of ) ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

static boolean isClassPtr( TYPE type )
{
    type = TypedefRemove( type );
    if( type->id == TYP_POINTER ) {
        if(( type->flag & TF1_REFERENCE ) == 0 ) {
            if( StructType( type->of ) != NULL ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

static TYPE classOrClassRef( TYPE type )
{
    TYPE class_type;

    class_type = StructType( type );
    if( class_type == NULL ) {
        TypeStripTdMod( type );
        if( type->id == TYP_POINTER ) {
            if(( type->flag & TF1_REFERENCE ) != 0 ) {
                class_type = StructType( type->of );
            }
        }
    }
    return( class_type );
}

static TYPE enumOrEnumRef( TYPE type )
{
    TYPE enum_type;

    enum_type = EnumType( type );
    if( enum_type == NULL ) {
        TypeStripTdMod( type );
        if( type->id == TYP_POINTER ) {
            if(( type->flag & TF1_REFERENCE ) != 0 ) {
                enum_type = EnumType( type->of );
            }
        }
    }
    return( enum_type );
}

static TYPE genericOrGenericRef( TYPE type )
{
    TYPE generic_type;

    generic_type = GenericType( type );
    if( generic_type == NULL ) {
        TypeStripTdMod( type );
        if( type->id == TYP_POINTER ) {
            if(( type->flag & TF1_REFERENCE ) != 0 ) {
                generic_type = GenericType( type->of );
            }
        }
    }
    return( generic_type );
}

static boolean arrowReturnOK( TYPE type, NAME arrow_name, boolean undefd_OK )
{
    TYPE class_type;
    SCOPE scope;
    SEARCH_RESULT *result;

    if( isClassPtr( type ) ) {
        return( TRUE );
    }
    class_type = classOrClassRef( type );
    if( class_type != NULL ) {
        if( TypeDefined( class_type ) ) {
            scope = TypeScope( class_type );
            result = ScopeFindMember( scope, arrow_name );
            if( result != NULL ) {
                ScopeFreeResult( result );
                return( TRUE );
            }
        } else {
            if( undefd_OK ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

boolean ArrowMemberOK( SYMBOL sym )
/*********************************/
{
    TYPE fn_type;

    if( sym != NULL ) {
        fn_type = FunctionDeclarationType( sym->sym_type );
        if( fn_type != NULL ) {
            if( arrowReturnOK( fn_type->of, sym->name->name, FALSE ) ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

static boolean isSize_t( TYPE arg )
{
    TypeStripTdMod( arg );
    switch( arg->id ) {
    case TYP_UINT:
#if TARGET_UINT == TARGET_ULONG
    case TYP_ULONG:             // accept 'unsigned long' if it's the same size
#endif
#if TARGET_UINT == TARGET_SHORT
    case TYP_USHORT:            // accept 'unsigned short' if it's the same size
#endif
        return( TRUE );
    }
    return( FALSE );
}

static void verifyIncDecSecondArg( TYPE fn_type, boolean non_static_member )
{
    int arg_idx;
    TYPE arg;
    TYPE int_type;

    arg_idx = 0;
    if( ! non_static_member ) {
        ++arg_idx;
    }
    arg = fn_type->u.f.args->type_list[ arg_idx ];
    int_type = IntegralType( arg );
    if( int_type == NULL || int_type->id != TYP_SINT ) {
        CErr1( ERR_OPERATOR_INC_DEC_SECOND_ARG );
    }
}

static void verifyNewFirstArg( NAME name, TYPE fn_type )
{
    TYPE arg;

    arg = fn_type->u.f.args->type_list[0];
    if( ! isSize_t( arg ) ) {
        CErr2p( ERR_OPERATOR_NEW_FIRST_ARG, name );
    }
}

static void verifyDelFirstArg( NAME name, TYPE fn_type )
{
    TYPE arg;

    arg = fn_type->u.f.args->type_list[0];
    if( ! isVoidPtr( arg ) ) {
        CErr2p( ERR_OPERATOR_DEL_FIRST_ARG, name );
    }
}

static void verifyDelSecondArg( NAME name, TYPE fn_type )
{
    TYPE arg;

    arg = fn_type->u.f.args->type_list[1];
    if( ! isSize_t( arg ) ) {
        CErr2p( ERR_OPERATOR_DEL_SECOND_ARG, name );
    }
}

static boolean checkOperatorArgs( TYPE fn_type )
{
    arg_list *args;
    TYPE *curr;
    TYPE *stop;

    args = fn_type->u.f.args;
    stop = &(args->type_list[ args->num_args ]);
    for( curr = &(args->type_list[ 0 ]); curr != stop; ++curr ) {
        if( classOrClassRef( *curr ) ) {
            return( TRUE );
        }
        if( enumOrEnumRef( *curr ) ) {
            return( TRUE );
        }
        if( genericOrGenericRef( *curr ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static void changeName( DECL_INFO *dinfo, NAME name )
{
    PTREE id;

    id = nameOfId( dinfo->id );
    if( id != NULL ) {
        id->u.id.name = name;
    }
    dinfo->name = name;
}

type_flag FunctionThisFlags( SYMBOL sym )
/***************************************/
{
    type_flag flags;

    if( SymIsThisFuncMember( sym ) ) {
        flags = TypeArgList( sym->sym_type )->qualifier;
    } else {
        flags = TF1_NULL;
    }
    return( flags );
}

type_flag FunctionThisMemFlags( SYMBOL sym )
/******************************************/
{
    type_flag flags;

    if( SymIsThisFuncMember( sym ) ) {
        flags = TypeArgList( sym->sym_type )->qualifier;
        flags |= defaultDataMemFlag;
    } else {
        flags = TF1_NULL;
    }
    return( flags );
}

static TYPE functionReturnsThis( SYMBOL sym, TYPE scope_type )
{
    type_flag flags;
    TYPE this_type;

    flags = FunctionThisFlags( sym );
    scope_type = MakeModifiedType( scope_type, flags );
    this_type = MakeReferenceTo( scope_type );
    return( adjustFunctionType( sym->sym_type, TF1_NULL, this_type ) );
}

static TYPE functionReturnsPtrVoid( SYMBOL sym )
{
    return( adjustFunctionType( sym->sym_type, TF1_NULL, TypeGetCache( TYPC_VOID_PTR ) ) );
}

static boolean quietStaticMember( SYMBOL sym, PTREE id )
{
    if( id->cgop == CO_NAME_OPERATOR ) {
        switch( id->id_cgop ) {
        case CO_NEW:
        case CO_DELETE:
        case CO_NEW_ARRAY:
        case CO_DELETE_ARRAY:
            sym->id = SC_STATIC;
            return( TRUE );
        }
    }
    return( FALSE );
}

static void extraInfoAboutArgs( unsigned num_args, boolean is_a_member,
                                                   boolean non_static_member )
{
    int msg;

    msg = INF_NUMBER_ARGS;
    if( is_a_member ) {
        if( non_static_member ) {
            msg = INF_NUMBER_ARGS_HAS_THIS;
        } else {
            msg = INF_NUMBER_ARGS_NO_THIS;
        }
    }
    InfMsgInt( msg, num_args );
}

static void checkForExplicit( TYPE fn_type )
{
    if( fn_type->flag & TF1_EXPLICIT ) {
        CErr1( ERR_EXPLICIT_FNS );
    }
}

static void verifyNotInNameSpace( SCOPE scope, DECL_INFO *dinfo, NAME name )
{
    SCOPE friend_scope = dinfo->friend_scope;

    if( friend_scope != NULL ) {
        scope = friend_scope;
    }
    if( ScopeIsGlobalNameSpace( scope ) == NULL ) {
        CErr2p( ANSI_NAME_CANT_BE_IN_NAMESPACE, name );
    }
}

static TYPE stripPragma( TYPE fn_type, SYMBOL sym )
{
    if( fn_type->u.f.pragma != NULL ) {
        // quietly remove stray __cdecl/__stdcall modifiers
        TYPE sym_type = RemoveFunctionPragma( sym->sym_type );
        sym->sym_type = sym_type;
        fn_type = FunctionDeclarationType( sym_type );
    }
    return( fn_type );
}

void VerifySpecialFunction( SCOPE scope, DECL_INFO *dinfo )
/*********************************************************/
{
    NAME name;
    NAME scope_name;
    type_flag cv_qualifiers;
    PTREE id;
    SYMBOL sym;
    TYPE type;
    TYPE scope_type;
    TYPE class_type;
    TYPE fn_type;
    TYPE udc_type;
    derived_status udc_class_check;
    unsigned num_args;
    boolean is_a_member;
    boolean is_out_of_class_member;
    boolean non_static_member;
    boolean check_args_for_class;

    id = nameOfId( dinfo->id );
    if( id == NULL ) {
        return;
    }
    sym = dinfo->sym;
    if( ! SymIsFunction( sym ) ) {
        return;
    }
    name = id->u.id.name;
    is_a_member = memberCheck( scope, dinfo, &scope_type );
    is_out_of_class_member = is_a_member && ( dinfo->scope != NULL ) && ( dinfo->scope != scope );
    non_static_member = FALSE;
    scope_name = NULL;
    if( is_a_member ) {
        scope_name = SimpleTypeName( scope_type );
        if( sym->id != SC_STATIC ) {
            if( ! quietStaticMember( sym, id ) ) {
                non_static_member = TRUE;
            }
        }
    }
    type = sym->sym_type;
    fn_type = FunctionDeclarationType( type );
    cv_qualifiers = fn_type->u.f.args->qualifier;
    if( is_a_member ) {
        if(( fn_type->flag & TF1_PLUSPLUS ) == 0 ) {
            /* all member functions must be linkage "C++" */
            type = MakePlusPlusFunction( type );
            sym->sym_type = type;
        }
        if( ! non_static_member ) {
            if( cv_qualifiers & TF1_CV_MASK ) {
                CErr1( ERR_STATIC_CANT_BE_CONST_VOLATILE );
            }
        }
        if( ( fn_type->flag & TF1_VIRTUAL ) && is_out_of_class_member ) {
            CErr1( ERR_OUT_OF_CLASS_VIRTUAL );
        }
    } else {
        if( fn_type->flag & TF1_VIRTUAL ) {
            CErr1( ERR_VIRTUAL_ONLY_IN_CLASS );
        }
        if( cv_qualifiers & TF1_CV_MASK ) {
            CErr1( ERR_NON_MEMBER_CANT_BE_CONST_VOLATILE );
        }
    }
    num_args = howManyArgs( non_static_member, fn_type );
    switch( id->cgop ) {
    case CO_NAME_OPERATOR:
        checkForExplicit( fn_type );
        break;
    case CO_NAME_DTOR:
        checkForExplicit( fn_type );
        if( is_a_member ) {
            if( SimpleTypeName( id->type ) != scope_name ) {
                CErr1( ERR_DESTRUCTOR_IN_WRONG_CLASS );
            }
            if( !non_static_member ) {
                CErr1( ERR_DESTRUCTOR_NOT_MEMBER );
            }
            if( cv_qualifiers & TF1_CV_MASK ) {
                CErr1( ERR_DTOR_CANT_BE_CONST_VOLATILE );
            }
            sym->sym_type = functionReturnsPtrVoid( sym );
        } else {
            CErr1( ERR_DESTRUCTOR_NOT_MEMBER );
        }
        return;
    case CO_NAME_CONVERT:
        checkForExplicit( fn_type );
        if( ! non_static_member ) {
            CErr1( ERR_USER_CONV_NOT_MEMBER );
        } else {
            udc_type = TypedefModifierRemoveOnly( fn_type->of );
            if( udc_type->id == TYP_VOID ) {
                CErr1( ERR_USER_CONV_VOID );
            } else {
                class_type = classOrClassRef( udc_type );
                if( class_type != NULL ) {
                    udc_class_check = TypeDerived( scope_type, class_type );
                    if( udc_class_check != DERIVED_NO ) {
                        CErr1( ANSI_USER_CONV_REF_DERIVED );
                    }
                }
            }
        }
        return;
    default:
        if( is_a_member && name == scope_name ) {
            /* a constructor! */
            changeName( dinfo, CppConstructorName() );
            if( ( DefaultIntType( fn_type->of ) == NULL ) || ( fn_type->of->id == TYP_MODIFIER ) ) {
                CErr1( ERR_CTOR_RETURNS_NOTHING );
            }
            if( ! non_static_member ) {
                CErr1( ERR_CTOR_CANT_BE_STATIC );
            }
            if( cv_qualifiers & TF1_CV_MASK ) {
                CErr1( ERR_CTOR_CANT_BE_CONST_VOLATILE );
            }
            if( fn_type->flag & TF1_VIRTUAL ) {
                CErr1( ERR_CTOR_CANT_BE_VIRTUAL );
            }
            if( is_a_member ) {
                sym->sym_type = functionReturnsThis( sym, scope_type );
            }
            if( ( fn_type->flag & TF1_EXPLICIT ) && is_out_of_class_member ) {
                CErr1( ERR_OUT_OF_CLASS_EXPLICIT );
            }
        } else {
            /* not a special function */
            checkForExplicit( fn_type );
        }
        return;
    }
    check_args_for_class = TRUE;
    switch( id->id_cgop ) {
    case CO_PRE_PLUS_PLUS:
    case CO_PRE_MINUS_MINUS:
        if( num_args != 1 && num_args != 2 ) {
            CErr2p( ERR_OPERATOR_MUST_HAVE_1_OR_2_ARGS, name );
            extraInfoAboutArgs( num_args, is_a_member, non_static_member );
        }
        if( num_args > 1 ) {
            verifyIncDecSecondArg( fn_type, non_static_member );
        }
        break;
    case CO_ARROW:
        if( num_args != 1 ) {
            CErr2p( ERR_OPERATOR_MUST_HAVE_1_ARG, name );
            extraInfoAboutArgs( num_args, is_a_member, non_static_member );
        }
        if( ! non_static_member ) {
            CErr2p( ERR_OPERATOR_NOT_MEMBER, name );
            check_args_for_class = FALSE;
        }
        if( ! arrowReturnOK( fn_type->of, name, TRUE ) ) {
            if( ! TypeIsClassInstantiation( scope_type ) ) {
                // CErr1( WARN_OPERATOR_ARROW_WONT_WORK );
                //   This warns (inappropriately) when a class nested
                //   inside a class template defines an operator->(). I
                //   question the value of this warning in general, but
                //   I won't remove it for now in case we want to add it
                //   again later. --PeterC
            }
        }
        break;
    case CO_INDEX:
        if( num_args != 2 ) {
            CErr2p( ERR_OPERATOR_MUST_HAVE_2_ARGS, name );
            extraInfoAboutArgs( num_args, is_a_member, non_static_member );
        }
        if( ! non_static_member ) {
            CErr2p( ERR_OPERATOR_NOT_MEMBER, name );
            check_args_for_class = FALSE;
        }
        break;
    case CO_CALL:
        if( ! non_static_member ) {
            CErr2p( ERR_OPERATOR_NOT_MEMBER, name );
            check_args_for_class = FALSE;
        }
        break;
    case CO_EQUAL:
        if( num_args != 2 ) {
            CErr2p( ERR_OPERATOR_MUST_HAVE_2_ARGS, name );
            extraInfoAboutArgs( num_args, is_a_member, non_static_member );
        }
        if( ! non_static_member ) {
            CErr2p( ERR_OPERATOR_NOT_MEMBER, name );
            check_args_for_class = FALSE;
        }
        break;
    case CO_PLUS:
    case CO_MINUS:
    case CO_TIMES:
    case CO_AND:
        if( num_args != 1 && num_args != 2 ) {
            CErr2p( ERR_OPERATOR_MUST_HAVE_1_OR_2_ARGS, name );
            extraInfoAboutArgs( num_args, is_a_member, non_static_member );
        }
        break;
    case CO_EXCLAMATION:
    case CO_TILDE:
        if( num_args != 1 ) {
            CErr2p( ERR_OPERATOR_MUST_HAVE_1_ARG, name );
            extraInfoAboutArgs( num_args, is_a_member, non_static_member );
        }
        break;
    case CO_NEW:
    case CO_NEW_ARRAY:
        check_args_for_class = FALSE;
        if( ! isVoidPtr( fn_type->of ) ) {
            CErr2p( ERR_OPERATOR_NEW_RETURNS_VOID, name );
        }
        if( num_args == 0 ) {
            CErr2p( ERR_OPERATOR_NEW_MUST_HAVE_ARGS, name );
            extraInfoAboutArgs( num_args, is_a_member, non_static_member );
        } else {
            verifyNewFirstArg( name, fn_type );
        }
        if( ! is_a_member ) {
            verifyNotInNameSpace( scope, dinfo, name );
            fn_type = stripPragma( fn_type, sym );
        }
        break;
    case CO_DELETE:
    case CO_DELETE_ARRAY:
        check_args_for_class = FALSE;
        if( ! isVoid( fn_type->of ) ) {
            CErr2p( ERR_OPERATOR_DEL_RETURNS_VOID, name );
        }
        if( is_a_member ) {
            if( num_args != 1 && num_args != 2 ) {
                CErr2p( ERR_OPERATOR_MUST_HAVE_1_OR_2_ARGS, name );
                extraInfoAboutArgs( num_args, is_a_member, non_static_member );
            }
        } else {
            if( num_args != 1 ) {
                CErr2p( ERR_OPERATOR_MUST_HAVE_1_ARG, name );
                extraInfoAboutArgs( num_args, is_a_member, non_static_member );
            }
            verifyNotInNameSpace( scope, dinfo, name );
            fn_type = stripPragma( fn_type, sym );
        }
        if( num_args > 0 ) {
            verifyDelFirstArg( name, fn_type );
            if( is_a_member && num_args > 1 ) {
                verifyDelSecondArg( name, fn_type );
            }
        }
        break;
    default:
        if( num_args != 2 ) {
            CErr2p( ERR_OPERATOR_MUST_HAVE_2_ARGS, name );
            extraInfoAboutArgs( num_args, is_a_member, non_static_member );
        }
    }
    if( check_args_for_class ) {
        boolean operator_OK;

        operator_OK = TRUE;
        if( ! non_static_member ) {
            operator_OK = checkOperatorArgs( fn_type );
        }
        if( ! operator_OK ) {
            CErr2p( ERR_OPERATOR_MUST_TAKE_CLASS_OR_ENUM, name );
        }
    }
}

static void complainAboutMemInit( DECL_INFO *dinfo )
{
    if( dinfo->mem_init == NULL ) {
        return;
    }
    if( dinfo->name == CppConstructorName() ) {
        return;
    }
    RewriteFree( dinfo->mem_init );
    dinfo->mem_init = NULL;
    CErr1( ERR_MEM_INIT_MUST_BE_CTOR );
}

static void verifyNoDefaultArgs( DECL_INFO *dinfo )
{
    DECL_INFO *parms;

    parms = dinfo->parms;
    if( parms == NULL ) {
        return;
    }
    cantHaveDefaultArgs( ERR_DEFAULT_ARGS_IN_A_TYPE, parms );
}

type_flag ExplicitModifierFlags( TYPE type )
/******************************************/
{
    type_flag flag;

    flag = TF1_NULL;
    for(;;) {
        switch( type->id ) {
        case TYP_MODIFIER:
            flag |= type->flag;
            break;
        default:
            return( flag );
        }
        type = type->of;
    }
}

static SCOPE blockScopeExtern( void )
{
    return( ScopeNearestFile( GetCurrScope() ) );
}

DECL_INFO *InsertDeclInfo( SCOPE insert_scope, DECL_INFO *dinfo )
/***************************************************************/
{
    type_flag flags;
    type_flag explicit_flags;
    TYPE type;
    SCOPE check_scope;
    SCOPE scope;
    SYMBOL sym;
    SYMBOL check_sym;
    SYMBOL decl_sym;
    boolean is_a_function;
    boolean is_template_function;
    boolean is_redefined;
    boolean is_block_sym;

    scope = ScopeNearestNonTemplate( insert_scope );
    VerifySpecialFunction( scope, dinfo );
    complainAboutMemInit( dinfo );
    sym = dinfo->sym;
    is_block_sym = FALSE;
    is_a_function = SymIsFunction( sym );
    is_template_function = FALSE;
    check_scope = dinfo->scope;
    if( check_scope != NULL ) {
        /* C::id style declaration */
        scope = check_scope;
        check_sym = ScopeInsert( scope, sym, dinfo->name );
        if( check_sym == sym ) {
            if( ! dinfo->sym_used ) {
                CErr2p( ERR_NOT_A_MEMBER, check_sym );
            }
        } else {
            if( dinfo->fn_defn ) {
                if( check_sym != NULL && check_sym->id == SC_DEFAULT ) {
                    CErr2p( ERR_NOT_A_MEMBER, sym );
                }
                if( ! ScopeEnclosed( ScopeNearestNonTemplate( GetCurrScope() ),
                                     scope ) ) {
                    CErr1( ERR_CURRSCOPE_DOESNT_ENCLOSE );
                }
            }
        }
    } else {
        /* unadorned id style declaration */
        scope = insert_scope;
        if( dinfo->friend_fn ) {
            if( ScopeId( scope ) == SCOPE_TEMPLATE_DECL ) {
                is_template_function = TRUE;
                TemplateFunctionCheck( sym, dinfo );
            }
            scope = dinfo->friend_scope;
        } else if( ScopeId( scope ) == SCOPE_TEMPLATE_DECL ) {
            is_template_function = TRUE;
            TemplateFunctionCheck( sym, dinfo );
            scope = ScopeNearestFileOrClass( GetCurrScope() );
        } else if( ScopeId( scope ) == SCOPE_BLOCK ||
                   ScopeId( scope ) == SCOPE_FUNCTION ) {
            /* handle promotions from local scope to file scope */
            if( sym->id == SC_EXTERN ) {
                scope = blockScopeExtern();
                is_block_sym = TRUE;
            } else if( is_a_function ) {
                switch( sym->id ) {
                case SC_TYPEDEF:
                    break;
                case SC_NULL:
                    if( errWithSymLoc( WARN_LOCAL_FN_PROTOTYPE, sym ) & MS_PRINTED ) {
                        CErr2p( INF_FUNCTION_PROTOTYPE, sym->sym_type );
                    }
                    scope = blockScopeExtern();
                    is_block_sym = TRUE;
                    break;
                default:
                    if( errWithSymLoc( ERR_STATIC_FN_DECL_IN_FUNCTION, sym ) & MS_PRINTED ) {
                        CErr2p( INF_FUNCTION_PROTOTYPE, sym->sym_type );
                    }
                    sym->id = SC_EXTERN;
                    scope = blockScopeExtern();
                    is_block_sym = TRUE;
                }
            } else {
                switch( sym->id ) {
                case SC_TYPEDEF:
                case SC_STATIC:
                    break;
                default:
                    type = TypeGetActualFlags( sym->sym_type, &flags );
                    if( flags & ~TF1_OK_FOR_AUTO ) {
                        explicit_flags = ExplicitModifierFlags( sym->sym_type );
                        if( explicit_flags & ~TF1_OK_FOR_AUTO ) {
                            errWithSymLoc( ERR_BAD_MODIFY_AUTO, sym );
                        } else {
                            flags &= TF1_OK_FOR_AUTO;
                            if( flags != TF1_NULL ) {
                                type = MakeModifiedType( type, flags );
                            }
                        }
                        sym->sym_type = type;
                    }
                    if( TargetSwitches & FLOATING_SS ) {
                        sym->sym_type = MakeModifiedType( sym->sym_type, TF1_FAR );
                    }
                }
            }
        } else if( is_a_function && ScopeLocalClass( scope ) ) {
            if( ! dinfo->fn_defn && sym->id != SC_DEFAULT ) {
                errWithSymLoc( WARN_LOCAL_CLASS_FUNCTION, sym );
            }
        }
        decl_sym = sym;
        if( ScopeType( scope, SCOPE_CLASS )
         && ( scope->owner.type != NULL )
         && ( dinfo->name == scope->owner.type->u.c.info->name ) ) {
            switch( decl_sym->id ) {
            case SC_TYPEDEF:
                CErr2p( ERR_TYPEDEF_SAME_NAME_AS_CLASS, dinfo->name );
                break;
            case SC_CLASS_TEMPLATE:
                CErr2p( ERR_TYPEDEF_SAME_NAME_AS_CLASS, dinfo->name );
                break;
            case SC_ENUM:
                CErr2p( ERR_ENUM_SAME_NAME_AS_CLASS, dinfo->name );
                break;
            case SC_STATIC:
            case SC_STATIC_FUNCTION_TEMPLATE:
                CErr2p( ERR_STATIC_SAME_NAME_AS_CLASS, dinfo->name );
                break;
            }
        }
        is_redefined = ScopeCarefulInsert( scope, &decl_sym, dinfo->name );
        check_sym = decl_sym;
        if( is_a_function ) {
            if( check_sym == sym ) {
                /* brand new declaration */
                if( dinfo->friend_fn ) {
                    if( ScopeId( scope ) == SCOPE_FILE ) {
                        if( ! CompFlags.extensions_enabled ) {
                            /* required by the ANSI C++ draft */
                            if( check_sym->id == SC_FUNCTION_TEMPLATE ) {
                                check_sym->id = SC_EXTERN_FUNCTION_TEMPLATE;
                            } else {
                                check_sym->id = SC_EXTERN;
                            }
                        }
                    }
                }
                if( is_template_function ) {
                    TemplateFunctionDeclaration( sym, dinfo->fn_defn );
                }
            }
        } else {
            if( check_sym != NULL && check_sym != sym ) {
                /* symbol was previously declared */
                if( is_redefined ) {
                    CErr2p( ERR_SYM_ALREADY_DEFINED, check_sym );
                }
            }
        }
        if( is_block_sym && check_sym == sym ) {
            DbgAssert( GetCurrScope() == insert_scope );
            DbgAssert( scope != insert_scope );
            SymMakeAlias( sym, &(sym->locn->tl) );
        }
    }
    dinfo->sym = check_sym;
    dinfo->sym_used = TRUE;
    if( check_sym != NULL ) {
        if( dinfo->friend_fn ) {
            SCOPE class_scope = ScopeNearestNonTemplate( insert_scope );
            if( ScopeType( class_scope, SCOPE_CLASS ) ) {
                if( dinfo->scope != NULL && dinfo->fn_defn ) {
                    CErr2p( ERR_INLINE_MEMBER_FRIEND, check_sym );
                }
                ScopeAddFriendSym( class_scope, check_sym );
            } else {
                CErr1( ERR_FRIEND_NOT_IN_CLASS );
            }
        }
        if( is_a_function ) {
            DeclareDefaultArgs( scope, dinfo );
        } else {
            verifyNoDefaultArgs( dinfo );
        }
    }
    return( dinfo );
}

void InsertArgs( DECL_INFO **args )
/*********************************/
{
    DECL_INFO *curr;
    NAME name;

    RingIterBeg( *args, curr ) {
        if( curr->type->id == TYP_DOT_DOT_DOT ) break;
        if( curr->sym == NULL ) {
            curr->sym = SymMakeDummy( curr->type, &name );
            curr->name = name;
        }
        InsertDeclInfo( GetCurrScope(), curr );
    } RingIterEnd( curr )
    freeDeclList( args );
}

#define MASK_TYPE_POINTS      \
    ( 1 << TYP_POINTER   )  | \
    ( 1 << TYP_FUNCTION  )  | \
    ( 1 << TYP_ARRAY     )

static boolean typePoints(      // TEST IF POINTER, ARRAY, REFERENCE, FUNCTION
    TYPE type )                 // - type to be tested
{
    boolean retn;               // - TRUE ==> type points at something

    if( type == NULL ) {
        retn = FALSE;
    } else {
        retn = TypeIdMasked( type, MASK_TYPE_POINTS );
    }
    return retn;
}

TYPE TypePointedAtModified(     // GET MODIFIED TYPE POINTED AT
    TYPE type )                 // - input type
{
    TYPE original;              // - original type

    if( NULL == type ) {
        return type;
    }
    original = type;
    TypeStripTdMod( type );
    if( typePoints( type ) ) {
        if( type->id == TYP_FUNCTION ) {
            type = original;
        } else {
            type = type->of;
        }
    } else {
        type = NULL;
    }
    return( type );
}

TYPE TypePointedAt(             // GET FLAGS, UNMODIFIED TYPE POINTED AT
    TYPE type,                  // - a pointer type
    type_flag *flags )          // - modifier flags for type pointed at
{
    return( TypeGetActualFlags( TypePointedAtModified( type ), flags ) );
}

TYPE TypePointedAtReplace( TYPE ptr_type, TYPE new_base )
/*******************************************************/
{
    TYPE old_base;
    TYPE mod_list;

    TypeStripTdMod( ptr_type );
    old_base = ptr_type->of;
    TypeStripTdMod( old_base );
    mod_list = removeModifiers( ptr_type->of, old_base );
    new_base = replaceModifiers( mod_list, new_base );
    return( MakePointerTo( new_base ) );
}


arg_list *TypeArgList(          // GET ARGUMENT LIST FOR A FUNCTION TYPE
    TYPE type )                 // - a function type
{
    TYPE fn_type;

    fn_type = FunctionDeclarationType( type );
    if( fn_type != NULL ) {
        return( fn_type->u.f.args );
    }
    return( NULL );
}

boolean TypeHasReverseArgs( TYPE type )
/*************************************/
{
    AUX_INFO *fn_pragma;

    type = FunctionDeclarationType( type );
    if( type == NULL ) {
        return( FALSE );
    }
    if( TypeHasEllipsisArg( type ) ) {
        return( FALSE );
    }
    fn_pragma = TypeHasPragma( type );
    if( fn_pragma == NULL ) {
        return( FALSE );
    }
    return( ReverseParms( fn_pragma ) );
}

boolean TypeHasNumArgs( TYPE fn_type, unsigned num_args )
/*******************************************************/
{
    arg_list *args;

    args = TypeArgList( fn_type );
    if( args == NULL ) {
        return( FALSE );
    }
    if( TypeHasEllipsisArg( fn_type ) ) {
        if( num_args >= ( args->num_args - 1 ) ) {
            /* handles case where ( int, ... ) has >= 1 parms */
            return( TRUE );
        }
    } else if( args->num_args == num_args ) {
        /* handles normal case */
        return( TRUE );
    }
    return( FALSE );
}

uint_32 TypeHash( TYPE type )
/***************************/
{
    arg_list *args;
    unsigned num_args;
    uint_32 hash;

    hash = 17;
    for(;;) {
        if( type == NULL ) break;
        hash <<= 4;
        hash *= type->id;
        hash += type->id;
        if( type->id == TYP_FUNCTION ) {
            args = type->u.f.args;
            num_args = args->num_args;
            hash += num_args + 1;
            type = type->of;
            if( num_args != 0 ) {
                if((( num_args & hash ) & 1 ) != 0 ) {
                    num_args = hash % num_args;
                    type = args->type_list[num_args];
                }
            }
        } else {
            type = type->of;
        }
    }
    return( hash | 1 );
}

static TYPE makeVTableType( TYPE type, boolean add_in_consts )
{
    type = MakePointerTo( type );
    if( add_in_consts ) {
        type = MakeConstTypeOf( type );
    }
    return( type );
}

TYPE MakeVFTableFieldType( boolean add_in_consts )
/************************************************/
{
    TYPE vftable;

    vftable = TypeGetCache( TYPC_VOID_FUN_OF_VOID );
    vftable = MakePointerTo( vftable );
    vftable = MakeConstTypeOf( vftable );
    vftable = makeVTableType( vftable, add_in_consts );
    return( vftable );
}

TYPE MakeVBTableFieldType( boolean add_in_consts )
/************************************************/
{
    TYPE vbtable;

    vbtable = GetBasicType( TYP_UINT );
    vbtable = MakeConstTypeOf( vbtable );
    vbtable = makeVTableType( vbtable, add_in_consts );
    return( vbtable );
}

boolean VerifyPureFunction( DECL_INFO *dinfo )
/*****************************************************/
{
    SYMBOL sym;
    TYPE type;
    TYPE fn_type;

    sym = dinfo->sym;
    type = sym->sym_type;
    fn_type = FunctionDeclarationType( type );
    if( fn_type != NULL ) {
        sym->sym_type = MakePureFunction( type );
        return( TRUE );
    }
    return( FALSE );
}

void VerifyMemberFunction( DECL_SPEC *dspec, DECL_INFO *dinfo )
/*************************************************************/
{
    dspec = dspec;
    FreeDeclInfo( dinfo );
}

static SYMBOL alreadyDefined( NAME name )
{
    return( ScopeAlreadyExists( GetFileScope(), name ) );
}


SYMBOL MakeTypeidSym( TYPE type )
/*******************************/
{
    unsigned len;
    SYMBOL sym;
    TYPE typeid_type;
    NAME name;

    name = CppTypeidName( &len, type );
    sym = alreadyDefined( name );
    if( sym != NULL ) {
        return( sym );
    }
    typeid_type = MakeExpandableType( TYP_UCHAR );
    DbgAssert( typeid_type->id == TYP_ARRAY );
    typeid_type->u.a.array_size = TypeidSize( len );
    typeid_type = makeCompilerReadWriteCommonData( typeid_type );
    sym = AllocSymbol();
    sym->id = SC_PUBLIC;
    // we don't want this symbol referenced unless we will generate it
    sym->sym_type = typeid_type;
    sym = tryInsertion( GetFileScope(), sym, name );
    LinkageSet( sym, "C++" );
    return( sym );
}

SYMBOL MakeVATableSym( SCOPE class_scope )
/****************************************/
{
    SYMBOL sym;
    TYPE vatable_type;
    NAME name;

    name = CppVATableName( class_scope );
    sym = alreadyDefined( name );
    if( sym != NULL ) {
        return( sym );
    }
    vatable_type = MakeExpandableType( TYP_UCHAR );
    vatable_type = MakeCompilerConstCommonData( vatable_type );
    sym = AllocSymbol();
    sym->id = SC_PUBLIC;
    sym->flag |= SF_REFERENCED;
    sym->sym_type = vatable_type;
    sym = tryInsertion( GetFileScope(), sym, name );
    LinkageSet( sym, "C++" );
    return( sym );
}

SYMBOL MakeVBTableSym( SCOPE scope, unsigned num_vbases, target_offset_t delta )
/******************************************************************************/
{
    SYMBOL sym;
    TYPE vbtable_type;
    NAME name;

    name = CppVBTableName( scope, delta );
    sym = alreadyDefined( name );
    if( sym != NULL ) {
        return( sym );
    }
    vbtable_type = GetBasicType( TYP_UINT );
    vbtable_type = MakeArrayOf( num_vbases + 1, vbtable_type );
    vbtable_type = MakeCompilerConstCommonData( vbtable_type );
    sym = AllocSymbol();
    sym->id = SC_PUBLIC;
    sym->flag |= SF_REFERENCED;
    sym->sym_type = vbtable_type;
    sym = tryInsertion( GetFileScope(), sym, name );
    LinkageSet( sym, "C++" );
    return( sym );
}

SYMBOL MakeVMTableSym( SCOPE from, SCOPE to, boolean *had_to_define )
/*******************************************************************/
{
    SYMBOL sym;
    TYPE vmtable_type;
    unsigned num_vbases;
    NAME name;

    *had_to_define = FALSE;
    name = CppIndexMappingName( from, to );
    sym = alreadyDefined( name );
    if( sym != NULL ) {
        return( sym );
    }
    *had_to_define = TRUE;
    num_vbases = ScopeClass( from )->u.c.info->last_vbase;
    vmtable_type = GetBasicType( TYP_UINT );
    vmtable_type = MakeArrayOf( num_vbases + 1, vmtable_type );
    vmtable_type = MakeCompilerConstCommonData( vmtable_type );
    sym = AllocSymbol();
    sym->id = SC_PUBLIC;
    sym->flag |= SF_REFERENCED;
    sym->sym_type = vmtable_type;
    sym = tryInsertion( GetFileScope(), sym, name );
    LinkageSet( sym, "C++" );
    return( sym );
}

SYMBOL MakeVFTableSym( SCOPE scope, unsigned num_vfns, target_offset_t delta )
/****************************************************************************/
{
    SYMBOL sym;
    TYPE vftable_type;
    NAME name;

    name = CppVFTableName( scope, delta );
    sym = alreadyDefined( name );
    if( sym != NULL ) {
        return( sym );
    }
    vftable_type = TypeGetCache( TYPC_VOID_FUN_OF_VOID );
    vftable_type = MakePointerTo( vftable_type );
    vftable_type = MakeArrayOf( num_vfns + 1, vftable_type );
    vftable_type = MakeCompilerConstCommonData( vftable_type );
    sym = AllocSymbol();
    sym->id = SC_PUBLIC;
    sym->flag |= SF_REFERENCED;
    sym->sym_type = vftable_type;
    sym = tryInsertion( GetFileScope(), sym, name );
    LinkageSet( sym, "C++" );
    return( sym );
}

void TypedefUsingDecl( DECL_SPEC *dspec, SYMBOL typedef_sym, TOKEN_LOCN *locn )
/*****************************************************************************/
{
    PTREE id;
    TYPE type;
    NAME name;
    TOKEN_LOCN id_locn;

    if( dspec != NULL ) {
        DbgAssert( locn == NULL );
        DbgAssert( typedef_sym == NULL );
        type = dspec->partial;
        id = dspec->id;
        DbgAssert( NodeIsBinaryOp( id, CO_STORAGE ) );
        name = id->u.subtree[1]->u.id.name;
        DbgAssert( name != NULL );
        PTreeExtractLocn( id, &id_locn );
        locn = &id_locn;
    } else {
        DbgAssert( locn != NULL );
        DbgAssert( typedef_sym != NULL && typedef_sym->id == SC_TYPEDEF );
        type = typedef_sym->sym_type;
        name = typedef_sym->name->name;
    }
    SymCreateAtLocn( type, SC_TYPEDEF, SF_NULL, name, GetCurrScope(), locn );
}

boolean TypeHasVirtualBases( TYPE type )
/**************************************/
{
    boolean retn;

    type = StructType( type );
    if( type == NULL ) {
        retn = FALSE;
    } else {
        retn = ScopeHasVirtualBases( TypeScope( type ) );
    }
    return retn;
}

boolean TypeIsClassInstantiation( TYPE type )
/*******************************************/
{
    if( type != NULL ) {
        TypeStripTdMod( type );
        if( type->id == TYP_CLASS ) {
            if( type->flag & TF1_INSTANTIATION ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

boolean TypeIsConst( TYPE type )
/******************************/
{
    type_flag flags;

    TypeModFlags( type, &flags );
    return( flags & TF1_CONST );
}

boolean TypeHasEllipsisArg( TYPE type )
/*************************************/
{
    unsigned num_args;
    arg_list *args;
    TYPE arg_type;

    type = FunctionDeclarationType( type );
    if( type != NULL ) {
        args = type->u.f.args;
        num_args = args->num_args;
        if( num_args > 0 ) {
            arg_type = args->type_list[ num_args - 1 ];
            if( arg_type->id == TYP_DOT_DOT_DOT ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

boolean TypeVAStartWontWork( TYPE fn_type, int *msg )
/***************************************************/
{
    unsigned num_args;
    arg_list *args;
    TYPE arg_type;
    TYPE test_type;

    DbgAssert( fn_type != NULL && fn_type->id == TYP_FUNCTION );
    *msg = ERR_NULL;
    if( ! TypeHasEllipsisArg( fn_type ) ) {
        return( FALSE );
    }
    args = fn_type->u.f.args;
    num_args = args->num_args;
    if( num_args > 0 ) {
        /* move to index of ... arg */
        --num_args;
        if( num_args == 0 ) {
            /* no arg before the ... arg */
            *msg = WARN_NO_ARG_BEFORE_ELLIPSE;
            return( TRUE );
        }
        /* move to index of arg before ... arg */
        --num_args;
        arg_type = args->type_list[ num_args ];
        test_type = TypeReference( arg_type );
        if( test_type != NULL ) {
            /* arg before ... arg is a reference arg */
            *msg = WARN_REF_ARG_BEFORE_ELLIPSE;
            return( TRUE );
        }
        arg_type = StructType( arg_type );
        if( arg_type != NULL
         && OMR_CLASS_REF == ObjModelArgument( arg_type ) ) {
            /* arg before ... arg is a class arg (WATCOM passes these as refs) */
            *msg = WARN_CLASS_ARG_BEFORE_ELLIPSE;
            return( TRUE );
        }
    }
    return( FALSE );
}

TYPE MemberPtrType( TYPE type )
/*****************************/
{
    if( type != NULL ) {
        TypeStripTdMod( type );
        if( type->id != TYP_MEMBER_POINTER ) {
            type = NULL;
        }
    }
    return type;
}

TYPE MemberPtrClass( TYPE type )
/******************************/
{
    if( type != NULL ) {
        type = StructType( type->u.mp.host );
    }
    return( type );
}

boolean TypeCtorDispRequired( TYPE host_class, TYPE base_class )
/**************************************************************/
{
    SCOPE base_scope;
    CLASSINFO *info;

    base_scope = base_class->u.c.scope;
    if( ! ScopeHasVirtualFunctions( base_scope ) ) {
        /* virtual function thunks are the only things that need a ctor-disp */
        return( FALSE );
    }
    info = host_class->u.c.info;
    if( !info->has_vcdtor ) {
        /* no class in the hierarchy has an explicit ctor/dtor in */
        /* the presence of virtual functions */
        return( FALSE );
    }
    return( TRUE );
}

boolean TypeHasSpecialFields( TYPE type )
/***************************************/
{
    CLASSINFO *info;

    type = StructType( type );
    if( type != NULL ) {
        info = type->u.c.info;
        if( info->has_comp_info ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

boolean TypeParmSize( TYPE fn_type, target_size_t *parm_size )
/************************************************************/
{
    unsigned i;
    target_size_t size;
    arg_list *args;
    TYPE *p;

    if( TypeHasEllipsisArg( fn_type ) ) {
        return( FALSE );
    }
    size = 0;
    args = fn_type->u.f.args;
    p = args->type_list;
    for( i = args->num_args; i != 0; --i ) {
        size += CgMemorySize( *p );
        size = _RoundUp( size, sizeof( target_int ) );
        ++p;
    }
    *parm_size = size;
    return( TRUE );
}

boolean CurrFunctionHasEllipsis( void )
/*************************************/
{
    SYMBOL sym;

    sym = ScopeFunctionInProgress();
    if( sym != NULL ) {
        return( TypeHasEllipsisArg( sym->sym_type ) );
    }
    return( FALSE );
}

boolean TypeIsAnonymousEnum( TYPE type )
/**************************************/
{
    TypeStripTdMod( type );
    if( type->id == TYP_ENUM && ( type->flag & TF1_UNNAMED ) != 0 ) {
        return( TRUE );
    }
    return( FALSE );
}

static boolean markAllUnused( SCOPE scope, void (*diag)( SYMBOL ) )
{
    type_flag all_flags;
    type_flag curr_flags;
    SYMBOL curr;
    SYMBOL stop;
    TYPE generic_type;

    DbgAssert( ScopeType( scope, SCOPE_TEMPLATE_DECL ) );
    all_flags = TF1_USED;
    curr = NULL;
    stop = ScopeOrderedStart( scope );
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        generic_type = TypedefRemove( curr->sym_type );
        DbgAssert( curr->id == SC_TYPEDEF && generic_type->id == TYP_GENERIC );
        curr_flags = generic_type->flag;
        if( diag != NULL && ( curr_flags & TF1_USED ) == 0 ) {
            (*diag)( curr );
        }
        all_flags &= curr_flags;
        generic_type->flag = curr_flags & ~TF1_USED;
    }
    return(( all_flags & TF1_USED ) != 0 );
}

static void pushArguments( PSTK_CTL *stk, arg_list *args )
{
    unsigned i;
    TYPE *p;
    TYPE type;

    p = args->type_list;
    for( i = args->num_args; i != 0; --i ) {
        type = *p;
        TypeStripTdMod( type );
#ifndef NDEBUG
        if( PragDbgToggle.dump_types ) {
            printf( "arg #%u\n", ( args->num_args - i ) + 1 );
            DumpFullType( type );
        }
#endif
        PstkPush( stk, type );
        ++p;
    }
}

static void pushPrototypeAndArguments( type_bind_info *data,
                                       PTREE p_args, PTREE a_args,
                                       unsigned control )
{
    unsigned i;
    PTREE p;
    PTREE a;
    PTREE p_tree;
    TYPE p_type;
    TYPE a_type;
    TYPE refed_type;

    for( i = 0; ( p_args != NULL ) && ( a_args != NULL );
         p_args = p_args->u.subtree[0], a_args = a_args->u.subtree[0], i++ ) {

        p = p_args->u.subtree[1];
        a = a_args->u.subtree[1];

        if( p->op == PT_TYPE ) {
            p_type = p->type;
            if( p_type->id == TYP_DOT_DOT_DOT ) {
                /* anything after the ... cannot participate in binding */
                break;
            }
        } else {
            p_type = NULL;
        }

        if( a->op == PT_TYPE ) {
            a_type = a->type;
        } else {
            a_type = NULL;
        }

#ifndef NDEBUG
        if( PragDbgToggle.dump_types ) {
            printf( "p_arg #%u\n", i + 1 );
            if( p->op == PT_TYPE ) {
                DumpFullType( p_type );
            }
            printf( "a_arg #%u\n", i + 1 );
            if( a->op == PT_TYPE ) {
                DumpFullType( a_type );
            }
        }
#endif

        if( p->op == PT_TYPE ) {
            if( control & PA_FUNCTION ) {
                /*
                    has reference &?
                         \ arg
                    proto \     Y           N
                    ------+------------------------
                      Y   | strip both  strip proto
                      N   | strip arg       _
                */
                refed_type = TypeReference( a_type );
                if( refed_type != NULL ) {
                    a_type = refed_type;
                }

                refed_type = TypeReference( p_type );
                if( refed_type != NULL ) {
                    // [temp.deduct.call] (3) If P is a reference
                    // type, the type referred to by P is used for
                    // type deduction.
                    p_type = refed_type;
                } else {
                    // [temp.deduct.call] (2) If P is not a reference
                    // type:

                    // - If A is a cv-qualified type, the top level
                    // cv-qualifiers of A's type are ignored for type
                    // deduction.
                    TypeStripTdMod( a_type );

                    // - If A is an array type, ...
                    // - If A is a function type, ...
                    a_type = adjustParmType( a_type );
                }
            }

            p_tree = PTreeType( p_type );
            p_tree->filler = 0;
            PstkPush( &(data->with_generic), p_tree );

            if( control & PA_FUNCTION ) {
                PstkPush( &(data->with_generic),
                          PTreeType( TypeGetCache( TYPC_FIRST_LEVEL ) ) );
            }
        } else if( p->op == PT_INT_CONSTANT ) {
            PstkPush( &(data->with_generic),
                      PTreeInt64Constant( p->u.int64_constant, p->type->id ) );
        } else if( p->op == PT_ID ) {
            PstkPush( &(data->with_generic),
                      PTreeId( p->u.id.name ) );
        } else if( p->op == PT_SYMBOL ) {
            PstkPush( &(data->with_generic),
                      PTreeIdSym( p->u.symcg.symbol ) );
        } else {
#ifndef NDEBUG
            DumpPTree( p );
#endif
            CFatal( "pushPrototypeAndArguments" );
        }

        if( a->op == PT_TYPE ) {
            PstkPush( &(data->without_generic), PTreeType( a_type ) );
        } else if( a->op == PT_INT_CONSTANT ) {
            PstkPush( &(data->without_generic),
                      PTreeInt64Constant( a->u.int64_constant, a->type->id ) );
        } else if( a->op == PT_SYMBOL ) {
            PstkPush( &(data->without_generic),
                      PTreeIdSym( a->u.symcg.symbol ) );
        } else {
#ifndef NDEBUG
            DumpPTree( a );
#endif
            CFatal( "pushPrototypeAndArguments" );
        }
    }
}

static void checkTemplateClass( PSTK_CTL *stk, TYPE class_type )
{
    SCOPE parm_scope;
    SYMBOL curr;
    SYMBOL stop;

    parm_scope = TemplateClassParmScope( class_type );
    if( parm_scope == NULL ) {
        return;
    }
    curr = NULL;
    stop = ScopeOrderedStart( parm_scope );
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        if( curr->id == SC_TYPEDEF ) {
            PstkPush( stk, curr->sym_type );
        }
    }
}

static void scanForGenerics( PSTK_CTL *stk, TYPE type )
{
    for(;;) {
        switch( type->id ) {
        case TYP_POINTER:
        case TYP_TYPEDEF:
        case TYP_ARRAY:
        case TYP_MODIFIER:
            type = type->of;
            break;
        case TYP_CLASS:
            if(( type->flag & ( TF1_UNBOUND | TF1_INSTANTIATION )) == 0 ) {
                return;
            }
            /* class was generated by a class template; check its arguments */
            checkTemplateClass( stk, type );
            return;
        case TYP_FUNCTION:
            pushArguments( stk, type->u.f.args );
            type = type->of;
            break;
        case TYP_MEMBER_POINTER:
            PstkPush( stk, type->u.mp.host );
            type = type->of;
            break;
        case TYP_GENERIC:
            type->flag |= TF1_USED;
            return;
        default:
            return;
        }
    }
}

boolean FunctionUsesAllTypes( SYMBOL sym, SCOPE scope, void (*diag)( SYMBOL ) )
/*****************************************************************************/
{
    TYPE fn_type;
    TYPE *top;
    auto PSTK_CTL type_stack;

    fn_type = FunctionDeclarationType( sym->sym_type );
    if( fn_type == NULL ) {
        return( FALSE );
    }
    markAllUnused( scope, NULL );
    PstkOpen( &type_stack );
    pushArguments( &type_stack, fn_type->u.f.args );
    for(;;) {
        top = (TYPE *)PstkPop( &type_stack );
        if( top == NULL ) break;
        if( *top != NULL ) {
            scanForGenerics( &type_stack, *top );
        }
    }
    PstkClose( &type_stack );
    return( markAllUnused( scope, diag ) );
}

static void clearGenericBindings( PSTK_CTL *stk, SCOPE decl_scope )
{
    TYPE *top;
    TYPE bound_type;
    SYMBOL stop, curr;

    DbgAssert( stk != NULL );

    for(;;) {
        top = (TYPE *)PstkPop( stk );
        if( top == NULL ) break;
        if( *top == NULL ) continue;
        bound_type = *top;
        switch( bound_type->id ) {
        case TYP_GENERIC:
        case TYP_CLASS:
            if( bound_type->of != NULL ) {
                bound_type->of = NULL;
            }
            break;
#ifndef NDEBUG
        default:
            CFatal( "bound generic type corrupted" );
#endif
        }
    }

    PstkClose( stk );

    if( decl_scope != NULL ) {
        stop = ScopeOrderedStart( decl_scope );
        curr = NULL;
        for(;;) {
            curr = ScopeOrderedNext( stop, curr );
            if( curr == NULL ) break;

            if( curr->sym_type->id == TYP_TYPEDEF ) {
                bound_type = curr->sym_type->of;
                if( bound_type->id == TYP_GENERIC ) {
                    bound_type->of = NULL;
                }
            }
        }
    }
}

boolean TypeBasesEqual( type_flag flags, void *base1, void *base2 )
/*****************************************************************/
{
    /* assumes both are the same type of base already */
    if(( flags & TF1_BASED ) == TF1_BASED_STRING ) {
        return( StringSame( base1, base2 ) );
    }
    if( base1 != base2 ) {
        /* handles TF1_BASED_FETCH and TF1_BASED_ADD */
        return( FALSE );
    }
    return( TRUE );
}

static boolean compareClassTypes( TYPE b_type, TYPE u_type,
    type_bind_info *data )
{
    CLASSINFO *b_info;
    CLASSINFO *u_info;
    SCOPE b_parm_scope;
    SCOPE u_parm_scope;
    SYMBOL b_curr;
    SYMBOL b_stop;
    SYMBOL u_curr;
    SYMBOL u_stop;
    TYPE already_bound;
    PTREE u_tree;

    if(( b_type->flag & TF1_INSTANTIATION ) == 0 ) {
        return( TRUE );
    }
    if(( u_type->flag & TF1_UNBOUND ) == 0 ) {
        return( TRUE );
    }
    already_bound = u_type->of;
    if( already_bound != NULL ) {
        /* template class was already bound */
        if( already_bound == b_type ) {
            return( FALSE );
        }
        return( TRUE );
    }
    b_info = b_type->u.c.info;
    u_info = u_type->u.c.info;
    if( b_info->name != u_info->name ) {
        return( TRUE );
    }
    /* both are instantiations of the same class */
    b_parm_scope = TemplateClassParmScope( b_type );
    u_parm_scope = TemplateClassParmScope( u_type );
    if( b_parm_scope == NULL || u_parm_scope == NULL ) {
        return( TRUE );
    }
    b_curr = NULL;
    u_curr = NULL;
    b_stop = ScopeOrderedStart( b_parm_scope );
    u_stop = ScopeOrderedStart( u_parm_scope );
    for(;;) {
        b_curr = ScopeOrderedNext( b_stop, b_curr );
        u_curr = ScopeOrderedNext( u_stop, u_curr );
        if( b_curr == NULL ) break;
        if( u_curr == NULL ) break;
        if( b_curr->id == SC_TYPEDEF ) {
            PstkPush( &(data->without_generic),
                      PTreeType( b_curr->sym_type ) );
            u_tree = PTreeType( u_curr->sym_type );
            u_tree->filler = 0;
            PstkPush( &(data->with_generic), u_tree );
        } else if( b_curr->id == SC_STATIC ) {
            if( u_curr->id == SC_ADDRESS_ALIAS ) {
                PstkPush( &(data->without_generic),
                          PTreeIntConstant( b_curr->u.sval,
                                            b_curr->sym_type->id ) );
                PstkPush( &(data->with_generic),
                          PTreeIdSym( u_curr->u.alias ) );
            } else if( u_curr->id == SC_STATIC ) {
                if( b_curr->u.sval != u_curr->u.sval ) {
                    return( TRUE );
                }
            } else {
                return( TRUE );
            }
        } else if( b_curr->id != u_curr->id ) {
            DbgAssert( b_curr == NULL && u_curr == NULL );
        }
    }
    DbgAssert( b_curr == NULL && u_curr == NULL );
    return( FALSE );
}

static boolean modifiersMatch( type_flag b_flags, type_flag u_flags,
                               void *b_base, void *u_base )
{
    //if( u_flags != b_flags )
    if(( u_flags ^ b_flags ) & ~TF1_MOD_IGNORE ) {
        return( FALSE );
    }
    if( b_flags & TF1_BASED ) {
        if( ! TypeBasesEqual( b_flags, b_base, u_base ) ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

static unsigned typesBind( type_bind_info *data, boolean is_function )
{
    type_flag t_flags;
    type_flag b_flags;
    type_flag u_flags;
    type_flag g_flags;
    type_flag d_flags;
    void *t_base;
    void *b_base;
    void *u_base;
    void *g_base;
    TYPE b_type;
    TYPE u_type;
    TYPE g_type;
    TYPE t_unmod_type;
    TYPE b_unmod_type;
    TYPE u_unmod_type;
    PTREE *b_top;
    PTREE *u_top;
    PTREE u_tree;
    TYPE *pb;
    TYPE *pu;
    arg_list *b_args;
    arg_list *u_args;
    type_flag u_cv_mask;
    boolean u_allow_base;
    TYPE match;
    unsigned i;
    unsigned status;
    struct {
        unsigned        arg_1st_level : 1;
    } flags;

    status = TB_BINDS;
    for(;;) {
        flags.arg_1st_level = FALSE;

        b_top = (PTREE *)PstkPop( &(data->without_generic) );
        u_top = (PTREE *)PstkPop( &(data->with_generic) );
        if( b_top == NULL || u_top == NULL ) {
            if( b_top != NULL ) {
                PTreeFree( *b_top );
            }
            if( u_top != NULL ) {
                PTreeFree( *u_top );
            }
            break;
        }

        if( ( (*u_top)->op == PT_TYPE ) && (*u_top)->type == TypeGetCache( TYPC_FIRST_LEVEL ) ) {
            flags.arg_1st_level = TRUE;
            PTreeFree( *u_top );
            u_top = (PTREE *)PstkPop( &(data->with_generic) );
            if( u_top == NULL ) {
                if( b_top != NULL ) {
                    PTreeFree( *b_top );
                }
                break;
            }
        }

        if( (*u_top)->op == PT_INT_CONSTANT ) {
            if( (*b_top)->op == PT_INT_CONSTANT ) {
                if( !I64Cmp( &(*u_top)->u.int64_constant, &(*b_top)->u.int64_constant ) ) {
                    PTreeFree( *b_top );
                    PTreeFree( *u_top );
                    continue;
                }
            }
            PTreeFree( *b_top );
            PTreeFree( *u_top );
            return( TB_NULL );
        } else if( (*u_top)->op == PT_ID ) {
            /* TODO: check type */
            if( (*b_top)->op == PT_INT_CONSTANT ) {
                SYMBOL sym = ScopeYYMember( data->parm_scope, (*u_top)->u.id.name )->name_syms;

                if( ( sym->id == SC_NULL ) && ( sym->u.sval == 0 ) ) {
                    sym->id = SC_STATIC;
                    DgStoreConstScalar( *b_top, (*b_top)->type, sym );
                }

                if( sym->u.sval != (*b_top)->u.int_constant ) {
                    // already bound to different value
                    PTreeFree( *b_top );
                    PTreeFree( *u_top );
                    return( TB_NULL );
                }

                PTreeFree( *b_top );
                PTreeFree( *u_top );
                continue;
            } else if( (*b_top)->op == PT_ID ) {
                SYMBOL sym = ScopeYYMember( data->parm_scope, (*u_top)->u.id.name )->name_syms;

                // using SC_NULL here is a bit dirty...
                if( ( sym->id == SC_NULL ) && ( sym->u.sval == 0 ) ) {
                    sym->u.sval = (target_int)(*b_top)->u.id.name;
                }

                if( ( sym->id != SC_NULL ) || ( sym->u.sval != (target_int)(*b_top)->u.id.name ) ) {
                    // already bound to different value
                    PTreeFree( *b_top );
                    PTreeFree( *u_top );
                    return( TB_NULL );
                }

                PTreeFree( *b_top );
                PTreeFree( *u_top );
                continue;
            }
            PTreeFree( *b_top );
            PTreeFree( *u_top );
            return( TB_NULL );

        } else if( ( (*u_top)->op != PT_TYPE ) || ( (*b_top)->op != PT_TYPE ) ) {
            PTreeFree( *b_top );
            PTreeFree( *u_top );
            CFatal( "typesBind failed" );
        }

        b_type = (*b_top)->type;
        if( b_type == NULL ) {
            return( TB_NULL );
        }
        b_unmod_type = TypeModFlagsBaseEC( b_type, &b_flags, &b_base );
        u_type = (*u_top)->type;

        if( flags.arg_1st_level ) {
            u_cv_mask = TF1_CONST | TF1_VOLATILE;
            u_allow_base = TRUE;
        } else {
            /*
             * In order to implement [conv.qual] we need to pass
             * cv-qualifier information down the stack. We do this by
             * using the "filler" field in the PTREE struct.
             */
            u_cv_mask = (*u_top)->filler & TF1_CONST;
            if( u_cv_mask & TF1_CONST ) {
                u_cv_mask |= TF1_VOLATILE;
            }
            u_allow_base = ( (*u_top)->filler & 0x80 ) != 0;
        }

        PTreeFree( *b_top );
        PTreeFree( *u_top );

        if( b_unmod_type == NULL ) {
            return( TB_NULL );
        }
        if( u_type == NULL ) {
            return( TB_NULL );
        }
        u_unmod_type = TypeModFlagsBaseEC( u_type, &u_flags, &u_base );
        if( ( b_unmod_type == u_unmod_type )
         && ( b_unmod_type->id != TYP_GENERIC ) ) {
            if( is_function ) {
                continue;
            }

            d_flags = u_flags ^ ( b_flags & u_flags );
            if( d_flags != TF1_NULL ) {
                /*
                    make sure flags in the arg type are in the parm type

                    u b  u not in b | (b&u) u^(b&u)
                    = =  ========== | ===== =======
                    0 0       0     |   0      0
                    0 1       0     |   0      0
                    1 0       1     |   0      1
                    1 1       0     |   1      0
                */
                // allow trivial conversion
                if( d_flags & ~u_cv_mask ) {
                    return( TB_NULL );
                }
                status |= TB_NEEDS_TRIVIAL;
                u_flags &= ~d_flags;
            }
            if( ! modifiersMatch( b_flags, u_flags, b_base, u_base ) ) {
                return( TB_NULL );
            }
            /* types on top of stack match exactly */
            continue;
        }
        if( ( b_unmod_type->id != u_unmod_type->id )
         || ( u_unmod_type->id == TYP_GENERIC) ) {
            if( u_unmod_type->id != TYP_GENERIC ) {
                if( is_function ) {
                    continue;
                }
                return( TB_NULL );
            }
            /* we don't want any extra mem-model flags */
            b_unmod_type = TypeModExtract( b_type, &b_flags, &b_base, TC1_NOT_ENUM_CHAR );
            /* generics can be anything so default memory models can't be trusted */
            u_unmod_type = TypeModExtract( u_type, &u_flags, &u_base, TC1_NOT_ENUM_CHAR );

            /*
                we have to split modifiers so that
                "<generic> const *" matches "char __based() volatile const *"
                and binds <generic> to "char based() volatile"
            */
            d_flags = u_flags ^ ( b_flags & u_flags );
            if( d_flags != TF1_NULL ) {
                /*
                    make sure flags in the unbound type are in the bound type

                    u b  u not in b | (b&u) u^(b&u)
                    = =  ========== | ===== =======
                    0 0       0     |   0      0
                    0 1       0     |   0      0
                    1 0       1     |   0      1
                    1 1       0     |   1      0
                */
                // allow trivial conversion
                if( d_flags & ~u_cv_mask ) {
                    return( TB_NULL );
                }
                status |= TB_NEEDS_TRIVIAL;
                u_flags &= ~d_flags;
            }
            /* 'u_flags' is known to be a subset of 'b_flags' now */
            if( u_flags & TF1_BASED ) {
                /* subset relation isn't final for based mod; check indices */
                if(( u_flags & TF1_BASED ) != ( b_flags & TF1_BASED )) {
                    return( TB_NULL );
                }
                /* check that based modifiers match up to the generic */
                if( ! TypeBasesEqual( u_flags, b_base, u_base ) ) {
                    return( TB_NULL );
                }
            }
            /* build up the generic binding type */
            g_flags = b_flags & ~u_flags;
            g_type = b_unmod_type;
            if( CompFlags.modifier_bind_compatibility && CompFlags.extensions_enabled ) {
                if( g_flags & TF1_BASED ) {
                    g_base = b_base;
                } else {
                    g_base = NULL;
                }
                if( g_flags != TF1_NULL ) {
                    g_type = MakeBasedModifierOf( g_type, g_flags, g_base );
                }
            } else {
                g_base = NULL;
                g_flags &= TF1_CONST | TF1_VOLATILE;
                if( g_flags != TF1_NULL ) {
                    g_type = MakeModifiedType( g_type, g_flags );
                }
            }
            match = u_unmod_type->of;
            if( match != NULL ) {
                /* generic type was bound; check the bound type */
                if( flags.arg_1st_level
                 && ( u_unmod_type->u.g.index <= data->num_explicit ) ) {
                    /* but don't care if it was an explicit specification */
                    continue;
                }
                t_unmod_type = TypeModExtract( match, &t_flags, &t_base, TC1_NOT_ENUM_CHAR );
                if( ! TypeCompareExclude( b_unmod_type, t_unmod_type, TC1_NOT_ENUM_CHAR ) ) {
                    /* unmodified types are different */
                    return( TB_NULL );
                }
                if(( t_flags & ~TF1_MEM_MODEL ) != ( g_flags & ~TF1_MEM_MODEL )) {
                    /* the non-memory model flags are different */
                    return( TB_NULL );
                }
                if( t_flags & TF1_MEM_MODEL ) {
                    if( g_flags & TF1_MEM_MODEL ) {
                        if( ! modifiersMatch( t_flags, g_flags, t_base, g_base ) ) {
                            return( TB_NULL );
                        }
                    } /* else new generic type has no flags so keep old binding */
                } else {
                    if( g_flags & TF1_MEM_MODEL ) {
                        /* old binding had no modifiers; add them */
                        u_unmod_type->of = g_type;
                    } /* else neither type had memory model flags */
                }
                continue;
            }
            /* bind the generic type */
            PstkPush( &data->bindings, u_unmod_type );
            u_unmod_type->of = g_type;
            continue;
        }
        // allow trivial conversion
        d_flags = u_flags ^ ( b_flags & u_flags );
        /* flags up to the types don't match */
        if( ( d_flags != TF1_NULL )
         && ( ( d_flags & ~u_cv_mask ) == TF1_NULL ) ) {
            /* only const/volatile don't match */
            status |= TB_NEEDS_TRIVIAL;
            b_flags |= d_flags;
        }
        if( ! is_function
         && ! modifiersMatch( b_flags, u_flags, b_base, u_base ) ) {
            return( TB_NULL );
        }
        switch( b_unmod_type->id ) {
        case TYP_CLASS:
            if( is_function && !( u_unmod_type->flag & TF1_UNBOUND ) ) {
                break;
            }

            if( compareClassTypes( b_unmod_type, u_unmod_type, data ) ) {
                if( u_allow_base && u_unmod_type->flag & TF1_UNBOUND ) {
                    b_unmod_type = ScopeFindBoundBase( b_unmod_type, u_unmod_type );
                    if( b_unmod_type != NULL ) {
                        if( compareClassTypes( b_unmod_type, u_unmod_type, data ) ) {
                            return( TB_NULL );
                        }
                        // OK, we bound to a base class of the bound type
                        status |= TB_NEEDS_DERIVED;
                    } else {
                        return( TB_NULL );
                    }
                } else {
                    return( TB_NULL );
                }
            }
            PstkPush( &data->bindings, u_unmod_type );
            u_unmod_type->of = b_unmod_type;
            break;
        case TYP_POINTER:
            if( ( b_unmod_type->flag ^ u_unmod_type->flag ) & TF1_REFERENCE ) {
                return( TB_NULL );
            }
            PstkPush( &(data->without_generic), PTreeType( b_unmod_type->of ) );
            u_tree = PTreeType( u_unmod_type->of );
            u_tree->filler = u_cv_mask & TF1_CONST;
            if( ! flags.arg_1st_level ) {
                u_tree->filler &= u_flags;
            } else {
                u_tree->filler |= 0x80;
            }
            PstkPush( &(data->with_generic), u_tree );
            break;
        case TYP_MEMBER_POINTER:
            PstkPush( &(data->without_generic), PTreeType( b_unmod_type->u.mp.host ) );
            u_tree = PTreeType( u_unmod_type->u.mp.host );
            u_tree->filler = 0;
            PstkPush( &(data->with_generic), u_tree );

            PstkPush( &(data->without_generic), PTreeType( b_unmod_type->of ) );
            u_tree = PTreeType( u_unmod_type->of );
            u_tree->filler = 0;
            PstkPush( &(data->with_generic), u_tree );
            break;
        case TYP_ARRAY:
            if( b_unmod_type->u.a.array_size != u_unmod_type->u.a.array_size ) {
                return( TB_NULL );
            }
            PstkPush( &(data->without_generic), PTreeType( b_unmod_type->of ) );
            u_tree = PTreeType( u_unmod_type->of );
            u_tree->filler = 0;
            PstkPush( &(data->with_generic), u_tree );
            break;
        case TYP_FUNCTION:
            if( b_unmod_type->u.f.pragma != u_unmod_type->u.f.pragma ) {
                return( TB_NULL );
            }
            b_args = b_unmod_type->u.f.args;
            u_args = u_unmod_type->u.f.args;
            if( b_args != u_args ) {
                if( b_args->num_args != u_args->num_args ) {
                    return( TB_NULL );
                }
                if( b_args->qualifier != u_args->qualifier ) {
                    return( TB_NULL );
                }
                pb = b_args->type_list;
                pu = u_args->type_list;
                for( i = b_args->num_args; i != 0; --i ) {
                    PstkPush( &(data->without_generic), PTreeType( *pb ) );
                    u_tree = PTreeType( *pu );
                    u_tree->filler = 0;
                    PstkPush( &(data->with_generic), u_tree );
                    ++pb;
                    ++pu;
                }
            }
            PstkPush( &(data->without_generic), PTreeType( b_unmod_type->of ) );
            u_tree = PTreeType( u_unmod_type->of );
            u_tree->filler = 0;
            PstkPush( &(data->with_generic), u_tree );
            break;
        default:
            if( ! TypeCompareExclude( b_unmod_type, u_unmod_type, TC1_NOT_ENUM_CHAR ) ) {
                return( TB_NULL );
            }
        }
    }
    DbgAssert( b_top == NULL && u_top == NULL );
    return( status );
}

typedef enum {
    BIND_BIND_TYPE      = 1,    // dup old_type and bind sub-types
    BIND_DUP_TYPE       = 2,    // CheckDupType on *link_type
    BIND_CLASS_TEMPLATE = 3,    // collect bound arg-types and bind class type
    BIND_NULL           = 0
} binding_op;

typedef struct bind_stack {
    TYPE        old_type;       // type being bound
    TYPE        *link_type;     // address to stuff new type into
    arg_list    *type_args;     // class template type arguments
    binding_op  op;             // operation
    int         : 0;
} TYPE_BIND_STACK;

static void binderInit( type_bind_info *data, unsigned num_explicit )
{
    PstkOpen( &(data->with_generic) );
    PstkOpen( &(data->without_generic) );
    PstkOpen( &(data->bindings) );
    data->parm_scope = NULL;
    data->num_explicit = num_explicit;
}

static void binderFini( type_bind_info *data )
{
    PTREE *top;

    for(;;) {
        top = (PTREE *)PstkPop( &(data->with_generic) );
        if( top == NULL ) break;
        PTreeFree( *top );
    }

    for(;;) {
        top = (PTREE *)PstkPop( &(data->without_generic) );
        if( top == NULL ) break;
        PTreeFree( *top );
    }

    PstkClose( &(data->with_generic) );
    PstkClose( &(data->without_generic) );
    data->parm_scope = NULL;
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

static void injectTemplateParm( SCOPE scope, PTREE parm, NAME name )
{
    SYMBOL addr_sym;
    SYMBOL sym;
    TYPE parm_type;

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
    if( sym != NULL ) {
        if( name == NULL ) {
            name = NameDummy();
        }
        sym = ScopeInsert( scope, sym, name );
    }
}

int BindExplicitTemplateArguments( SCOPE parm_scope, PTREE templ_args )
/*********************************************************************/
{
    SCOPE decl_scope;
    SYMBOL curr, stop;
    PTREE node;
    PTREE parm;
    TYPE typ;
    NAME name;
    int num_explicit;
    boolean something_went_wrong;

    num_explicit = 0;
    something_went_wrong = FALSE;
    decl_scope = parm_scope->enclosing;
    if( ( decl_scope == NULL ) && ( templ_args == NULL ) ) {
        return num_explicit;
    }

    node = templ_args;
    stop = ScopeOrderedStart( decl_scope );
    curr = NULL;
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) {
            break;
        }

        name = curr->name->name;
        if( ( node != NULL ) && ( node->u.subtree[1] != NULL ) ) {
            // inject an explicitly specified parameter
            parm = node->u.subtree[1];

            if( parm->op == PT_TYPE ) {
                // type parameter
                if( ( curr->sym_type->id != TYP_TYPEDEF )
                 || ( curr->sym_type->of->id != TYP_GENERIC ) ) {
                    something_went_wrong = TRUE;
                    break;
                }
                
                typ = parm->type;
                curr->sym_type->of->of = typ;
            } else if( parm->op == PT_INT_CONSTANT ) {
                // non-type parameter (integral constant)
                if( ! IntegralType( curr->sym_type ) ) {
                    something_went_wrong = TRUE;
                    break;
                }
            } else if( parm->op == PT_SYMBOL ) {
                // non-type parameter (symbol)
            }

            // TODO: check template parameter type
            injectTemplateParm( parm_scope, parm, name );
            num_explicit++;

            node = node->u.subtree[0];
        } else {
            // no more explicit parameters
            if( name == NULL ) {
                name = NameDummy();
            }

            if( (typ = GenericType( curr->sym_type )) != NULL ) {
                ScopeInsert( parm_scope, templateArgTypedef( typ ), name );
            } else if( (typ = IntegralType( curr->sym_type )) != NULL ) {
                ScopeInsert( parm_scope, templateArgSym( SC_NULL, typ ), name );
            } else {
                CFatal( "not yet implemented" );
            }

            node = NULL;
        }
    }

    if( something_went_wrong || ( node != NULL ) ) {
        return -1;
    }

    return num_explicit;
}

boolean BindGenericTypes( SCOPE parm_scope, PTREE parms, PTREE args,
                          boolean is_function, unsigned int explicit_args )
/*************************************************************************/
{
    SYMBOL curr, stop;
    unsigned bind_status;
    boolean result;
    auto type_bind_info data;

    DbgAssert( parm_scope != NULL );

    binderInit( &data, explicit_args );
    data.parm_scope = parm_scope;

    pushPrototypeAndArguments( &data, parms, args,
                               is_function ? PA_FUNCTION : PA_NULL );
    result = FALSE;
    bind_status = typesBind( &data, is_function );
    if( bind_status != TB_NULL ) {
        result = TRUE;

        stop = ScopeOrderedStart( parm_scope );
        curr = NULL;
        for(;;) {
            curr = ScopeOrderedNext( stop, curr );
            if( curr == NULL ) break;

            if( ( curr->sym_type->id == TYP_TYPEDEF )
             && ( curr->sym_type->of->id == TYP_GENERIC ) ) {
                if( curr->sym_type->of->of == NULL ) {
                    result = FALSE;
                    break;
                } else {
                    curr->sym_type->of = curr->sym_type->of->of;
                }
            } else if( curr->id == SC_NULL ) {
                result &= curr->u.sval != 0;
            }
        }
    }

    clearGenericBindings( &data.bindings, parm_scope->enclosing );
    binderFini( &data );
    return( result );
}

static void initBasicTypes( void )
{
    type_id *p;
    static type_id basics_init_list[] = {
        TYP_ERROR,
        TYP_BOOL,
        TYP_CHAR,
        TYP_SCHAR,
        TYP_UCHAR,
        TYP_WCHAR,
        TYP_SSHORT,
        TYP_USHORT,
        TYP_SINT,
        TYP_UINT,
        TYP_SLONG,
        TYP_ULONG,
        TYP_SLONG64,
        TYP_ULONG64,
        TYP_FLOAT,
        TYP_DOUBLE,
        TYP_LONG_DOUBLE,
        TYP_VOID,
        TYP_DOT_DOT_DOT,
        TYP_MAX
    };

    for( p = basics_init_list; *p != TYP_MAX; ++p ) {
        basicTypes[ *p ] = MakeType( *p );
    }
    /*
      'char' must be distinct from 'signed char' and 'unsigned char'
      for overload resolution but it still has "signed-ness"
    */
    basicTypes[ TYP_CHAR ]->of = basicTypes[ TYP_UCHAR ];
    basicTypes[ TYP_BOOL ]->of = basicTypes[ TYP_UCHAR ];
}

static TYPE makeErrFormat( tfmt_index fi )
{
    TYPE fmt_type;
    TYPE ret_type;

    fmt_type = MakeType( TYP_ERROR );
    fmt_type->flag |= TF1_SPECIAL_FMT;
    fmt_type->u.e.fmt = fi;
    ret_type = CheckDupType( fmt_type );
    DbgAssert( ret_type == fmt_type );  // make sure they are unique!
    return( ret_type );
}

static void initErrFormat( tfmt_index fi, typc_index ti )
{
    TYPE fmt_type;

    fmt_type = makeErrFormat( fi );
    TypeSetCache( ti, fmt_type );
}

static void initCache( void )
{
    TYPE void_type;
    TYPE stdop_void;
    TYPE const_stdop_void;
    TYPE volatile_stdop_void;
    TYPE const_volatile_stdop_void;
    TYPE stdop_arith_void;
    TYPE const_stdop_arith_void;
    TYPE segment_short;
    TYPE clean_int;
    TYPE ptr_void;
    TYPE default_int;
    TYPE void_fun_of_void;
    TYPE void_fun_of_ptr_void;
    TYPE ptr_void_fun_of_void;
    TYPE ptr_stdop_void;
    TYPE ptr_const_stdop_void;
    TYPE ptr_volatile_stdop_void;
    TYPE ptr_const_volatile_stdop_void;
    TYPE ptr_stdop_arith_void;
    TYPE ptr_const_stdop_arith_void;
    TYPE cdtor_arg;
    TYPE ptr_void_fun_of_cdtor_arg;
    TYPE void_member_ptr;
    TYPE first_level;

    segment_short = MakeType( TYP_USHORT );
    segment_short->flag |= TF1_SEGMENT;
    segment_short = CheckDupType( segment_short );
    TypeSetCache( TYPC_SEGMENT_SHORT, segment_short );

    clean_int = MakeType( TYP_SINT );
    clean_int->flag |= TF1_CLEAN;
    clean_int = CheckDupType( clean_int );
    TypeSetCache( TYPC_CLEAN_INT, clean_int );

    void_type = GetBasicType( TYP_VOID );
    ptr_void = MakePointerTo( void_type );
    TypeSetCache( TYPC_VOID_PTR, ptr_void );

    default_int = MakeType( TYP_SINT );
    default_int->flag |= TF1_DEFAULT;
    default_int = CheckDupType( default_int );
    TypeSetCache( TYPC_DEFAULT_INT, default_int );

    void_fun_of_void = MakeSimpleFunction( void_type, NULL );
    TypeSetCache( TYPC_VOID_FUN_OF_VOID, void_fun_of_void );

    void_fun_of_ptr_void = MakeSimpleFunction( void_type, ptr_void, NULL );
    TypeSetCache( TYPC_VOID_FUN_OF_PTR_VOID, void_fun_of_ptr_void );

    ptr_void_fun_of_void = MakeSimpleFunction( ptr_void, void_type, NULL );
    TypeSetCache( TYPC_PTR_VOID_FUN_OF_VOID, ptr_void_fun_of_void );

    stdop_void = MakeType( TYP_VOID );
    stdop_void->flag |= TF1_STDOP;
    stdop_void = CheckDupType( stdop_void );
    ptr_stdop_void = MakePointerTo( stdop_void );
    TypeSetCache( TYPC_PTR_STDOP_VOID, ptr_stdop_void );

    const_stdop_void = MakeConstTypeOf( stdop_void );
    ptr_const_stdop_void = MakePointerTo( const_stdop_void );
    TypeSetCache( TYPC_PTR_CONST_STDOP_VOID, ptr_const_stdop_void );

    volatile_stdop_void = MakeVolatileTypeOf( stdop_void );
    ptr_volatile_stdop_void = MakePointerTo( volatile_stdop_void );
    TypeSetCache( TYPC_PTR_VOLATILE_STDOP_VOID, ptr_volatile_stdop_void );

    const_volatile_stdop_void = MakeConstTypeOf( volatile_stdop_void );
    ptr_const_volatile_stdop_void = MakePointerTo( const_volatile_stdop_void );
    TypeSetCache( TYPC_PTR_CONST_VOLATILE_STDOP_VOID, ptr_const_volatile_stdop_void );

    stdop_arith_void = MakeType( TYP_VOID );
    stdop_arith_void->flag |= TF1_STDOP_ARITH;
    stdop_arith_void = CheckDupType( stdop_arith_void );
    ptr_stdop_arith_void = MakePointerTo( stdop_arith_void );
    TypeSetCache( TYPC_PTR_STDOP_ARITH_VOID, ptr_stdop_arith_void );

    const_stdop_arith_void = MakeConstTypeOf( stdop_arith_void );
    ptr_const_stdop_arith_void = MakePointerTo( const_stdop_arith_void );
    TypeSetCache( TYPC_PTR_CONST_STDOP_ARITH_VOID, ptr_const_stdop_arith_void );

    cdtor_arg = GetBasicType( TYP_UINT );
    TypeSetCache( TYPC_CDTOR_ARG, cdtor_arg );

    ptr_void_fun_of_cdtor_arg = MakeSimpleFunction( ptr_void, cdtor_arg, NULL );
    TypeSetCache( TYPC_PTR_VOID_FUN_OF_CDTOR_ARG, ptr_void_fun_of_cdtor_arg );

    void_member_ptr = MakeMemberPointerTo( void_type, void_type );
    TypeSetCache( TYPC_VOID_MEMBER_PTR, void_member_ptr );

    initErrFormat( TFMT_CLASS_TEMPLATE, TYPC_CLASS_TEMPLATE );

    first_level = MakeType( TYP_ERROR );
    first_level->flag |= TF1_FIRST;
    first_level = CheckDupType( first_level );
    TypeSetCache( TYPC_FIRST_LEVEL, first_level );
}

TYPE MakeNamespaceType( void )
/****************************/
{
    return( makeErrFormat( TFMT_NAMESPACE ) );
}

static void typesInit(          // TYPES INITIALIZATION
    INITFINI* defn )            // - definition
{
    if( CompFlags.dll_subsequent ) {
        typeHashCtr = 0;
        uniqueTypes = NULL;
        zero_table( basicTypes );
        zero_table( typeTable );
        zero_table( fnHashTable );
        zero_table( fnTable );
        zero_table( pointerHashTable );
        zero_table( bitfieldHashTable );
        zero_table( arrayHashTable );
        zero_table( modifierHashTable );
        zero_table( typeHashTables );
        zero_table( TypeCache );
    }
    typeHashTables[ TYP_POINTER ] = pointerHashTable;
    typeHashTables[ TYP_BITFIELD ] = bitfieldHashTable;
    typeHashTables[ TYP_ARRAY ] = arrayHashTable;
    typeHashTables[ TYP_MODIFIER ] = modifierHashTable;
    defn = defn;
    carveDECL_SPEC = CarveCreate( sizeof( DECL_SPEC ), BLOCK_DECL_SPEC );
    carveTYPE = CarveCreate( sizeof( struct type ), BLOCK_TYPE );
    carveCLASSINFO = CarveCreate( sizeof( CLASSINFO ), BLOCK_CLASSINFO );
    carveDECL_INFO = CarveCreate( sizeof(struct decl_info), BLOCK_DECL_INFO );
    initBasicTypes();
    initCache();
    TypeError = basicTypes[ TYP_ERROR ];
    CompInfo.ptr_diff_near = basicTypes[ TYP_SINT ];
    CompInfo.ptr_diff_far = basicTypes[ TYP_SINT ];
    CompInfo.ptr_diff_far16 = basicTypes[ TYP_SSHORT ];
    CompInfo.ptr_diff_huge = basicTypes[ TYP_SLONG ];
    ClassInit();
    cdeclPragma = PragmaLookup( NULL, M_CDECL );
    ExtraRptRegisterCtr( &types_defined, "unique type entries defined" );
    ExtraRptRegisterCtr( &types_alloced, "type entries allocated" );
    ExtraRptRegisterCtr( &ctr_dups, "type duplication checks" );
    ExtraRptRegisterCtr( &ctr_lookup, NULL );
    ExtraRptRegisterCtr( &ctr_dup_succ_probes, NULL );
    ExtraRptRegisterCtr( &ctr_dup_fail_probes, NULL );
    ExtraRptRegisterCtr( &ctr_dup_succ, "dup. checks found" );
    ExtraRptRegisterCtr( &ctr_dup_fail, "dup. checks failed" );
    ExtraRptRegisterAvg( &ctr_dup_succ_probes
                       , &ctr_dup_succ
                       , "average probes per check (found)" );
    ExtraRptRegisterAvg( &ctr_dup_fail_probes
                       , &ctr_dup_fail
                       , "average probes per check (failure)" );
    ExtraRptRegisterCtr( &ctr_dup_fns
                       , "type duplication checks -- in fn table" );
    ExtraRptRegisterCtr( &ctr_dup_fns_big
                       , "type duplication checks -- not in fn table" );
    ExtraRptRegisterCtr( &ctr_cg_dups, "type duplication checks (back-end)" );
    ExtraRptRegisterCtr( &ctr_cg_dups_fail, "dup. checks failed (back-end)" );
#ifdef XTRA_RPT
    {
#define ENTRY_ERROR "TYP_ERROR",
#define ENTRY_BOOL "TYP_BOOL",
#define ENTRY_CHAR "TYP_CHAR",
#define ENTRY_SCHAR "TYP_SCHAR",
#define ENTRY_UCHAR "TYP_UCHAR",
#define ENTRY_WCHAR "TYP_WCHAR",
#define ENTRY_SSHORT "TYP_SSHORT",
#define ENTRY_USHORT "TYP_USHORT",
#define ENTRY_SINT "TYP_SINT",
#define ENTRY_UINT "TYP_UINT",
#define ENTRY_SLONG "TYP_SLONG",
#define ENTRY_ULONG "TYP_ULONG",
#define ENTRY_SLONG64 "TYP_SLONG64",
#define ENTRY_ULONG64 "TYP_ULONG64",
#define ENTRY_FLOAT "TYP_FLOAT",
#define ENTRY_DOUBLE "TYP_DOUBLE",
#define ENTRY_LONG_DOUBLE "TYP_LONG_DOUBLE",
#define ENTRY_ENUM "TYP_ENUM",
#define ENTRY_POINTER "TYP_POINTER",
#define ENTRY_TYPEDEF "TYP_TYPEDEF",
#define ENTRY_CLASS "TYP_CLASS",
#define ENTRY_BITFIELD "TYP_BITFIELD",
#define ENTRY_FUNCTION "TYP_FUNCTION",
#define ENTRY_ARRAY "TYP_ARRAY",
#define ENTRY_DOT_DOT_DOT "TYP_DOT_DOT_DOT",
#define ENTRY_VOID "TYP_VOID",
#define ENTRY_MODIFIER "TYP_MODIFIER",
#define ENTRY_MEMBER_POINTER "TYP_MEMBER_POINTER",
#define ENTRY_GENERIC "TYP_GENERIC",
        static char const * const typeIdNames[] = {
            #include "type_arr.h"
            "Total"
        };
        ExtraRptRegisterTab( "type id frequency table", typeIdNames, ctr_type_ids, TYP_MAX, 1 );
        ExtraRptRegisterTab( "number of fn arguments frequency table", NULL, ctr_fn_args, ARGS_MAX+1+1, 1 );
    }
#endif
}

static void markFreeType( void *p )
{
    TYPE s = p;

    s->id = TYP_FREE;
}

#ifndef NDEBUG
unsigned num_refs;

static void initXrefType( void *e, carve_walk_base *d )
{
    TYPE s = e;

    if( s->id == TYP_FREE ) {
        return;
    }
    d = d;
    s->u.b.field_start = 0;
    s->u.b.field_width = 0;
}
static void xrefType( void *e, carve_walk_base *d )
{
    TYPE s = e;
    TYPE of;

    if( s->id == TYP_FREE ) {
        return;
    }
    d = d;
    of = s->of;
    if( of != 0 ) {
        of->u.b.field_start++;
        if( s->id == TYP_FUNCTION ) {
            of->u.b.field_width++;
        }
    }
    if( ( s->id == TYP_POINTER ) && ( s->flag & TF1_REFERENCE ) ) {
        ++num_refs;
    }
}
static void dumpXrefType( void *e, carve_walk_base *d )
{
    FILE *fp = d->extra;
    TYPE s = e;

    if( s->id == TYP_FREE ) {
        return;
    }
    fprintf( fp, "%u %u %u\n", s->id, s->u.b.field_start, s->u.b.field_width );
}
void DumpOfRefs( void )
{
    FILE *fp;
    auto carve_walk_base data;

    if( CompFlags.extra_stats_wanted ) {
        CarveWalkAllFree( carveTYPE, markFreeType );
        CarveWalkAll( carveTYPE, initXrefType, &data );
        CarveWalkAll( carveTYPE, xrefType, &data );
        fp = fopen( "of_ref.lst", "w" );
        fprintf( fp, "num_refs: %u\n", num_refs );
        data.extra = fp;
        CarveWalkAll( carveTYPE, dumpXrefType, &data );
        fclose( fp );
    }
}
#endif


static void freeTypeName( void *e, carve_walk_base *d )
{
    TYPE t = e;

    d = d;
    if( t->id == TYP_TYPENAME ) {
        CMemFreePtr( &t->u.n.name );
    }
}


static void typesFini(          // COMPLETION OF TYPES PROCESSING
    INITFINI* defn )            // - definition
{
    auto carve_walk_base data;

    defn = defn;
    ClassFini();
#ifndef NDEBUG
    //DumpOfRefs();
    CarveVerifyAllGone( carveDECL_SPEC, "DECL_SPEC" );
    CarveVerifyAllGone( carveDECL_INFO, "DECL_INFO" );
#endif
    CarveWalkAllFree( carveTYPE, markFreeType );
    CarveWalkAll( carveTYPE, freeTypeName, &data );
    CarveDestroy( carveTYPE );
    CarveDestroy( carveDECL_SPEC );
    CarveDestroy( carveCLASSINFO );
    CarveDestroy( carveDECL_INFO );
}

INITDEFN( types, typesInit, typesFini )

TYPE TypeGetIndex( TYPE type )
/****************************/
{
    return( CarveGetIndex( carveTYPE, type ) );
}

TYPE TypeMapIndex( TYPE i )
/*************************/
{
    return( CarveMapIndex( carveTYPE, i ) );
}

CLASSINFO *ClassInfoGetIndex( CLASSINFO *cinfo )
/**********************************************/
{
    return( CarveGetIndex( carveCLASSINFO, cinfo ) );
}

CLASSINFO *ClassInfoMapIndex( CLASSINFO *cinfo )
/**********************************************/
{
    return( CarveMapIndex( carveCLASSINFO, cinfo ) );
}

DECL_INFO *DeclInfoGetIndex( DECL_INFO *dinfo )
/**********************************************/
{
    return( CarveGetIndex( carveDECL_INFO, dinfo ) );
}

DECL_INFO *DeclInfoMapIndex( DECL_INFO *dinfo )
/**********************************************/
{
    return( CarveMapIndex( carveDECL_INFO, dinfo ) );
}

static void markFreeClassInfo( void *p )
{
    CLASSINFO *s = p;

    s->free = TRUE;
}

static void markFreeDeclInfo( void *p )
{
    DECL_INFO *s = p;

    s->free = TRUE;
}

typedef struct {
    carve_walk_base     base;
    unsigned            count;
    arg_list            **translate;
    type_dbgflag        dbgflag_mask;
} type_pch_walk;

typedef struct {
    carve_walk_base     base;
    char                *curr;
    size_t              amount;
} type_reloc_pch_walk;

static int cmpArgList( const void *lp, const void *rp )
{
    arg_list *left = *(arg_list **)lp;
    arg_list *right = *(arg_list **)rp;

    if( left < right ) {
        return( -1 );
    } else if( left > right ) {
        return( 1 );
    }
    return( 0 );
}


static unsigned pchCountHashes(
    TYPE* vector )
{
    unsigned count = 0;
    unsigned idx;

    for( idx = 0; idx < TYPE_HASH_MODULUS; ++idx ) {
        count += RingCount( *vector );
        ++ vector;
    }
    return count;
}

static void pchWriteArgLists( type_pch_walk *data )
{
    unsigned count;
    unsigned i;
    unsigned except_spec_count;
    TYPE *head;
    TYPE curr;
    TYPE *save_except_spec;
    TYPE *etype;
    arg_list **table;
    arg_list **p;
    arg_list **stop;
    arg_list **s;
    arg_list *args;

    // figure out max size of translation table
    count = RingCount( typeTable[ TYP_FUNCTION ] );
    for( i = 0; i < ARGS_HASH; ++i ) {
        count += pchCountHashes( fnHashTable[i] );
    }
    head = &fnTable[0];
    for( i = ARGS_HASH; i < ARGS_MAX; ++i ) {
        count += RingCount( *head );
        ++head;
    }
    RingIterBeg( uniqueTypes, curr ) {
        if( curr->id == TYP_FUNCTION ) {
            ++count;
        }
    } RingIterEnd( curr )
    // allocate and fill translation table
    // (protect against case where count == 0)
    table = CMemAlloc( ( count + 1 ) * sizeof( arg_list * ) );
    p = table;
    RingIterBeg( typeTable[ TYP_FUNCTION ], curr ) {
        *p = curr->u.f.args;
        ++p;
    } RingIterEnd( curr )
    head = &fnHashTable[0][0];
    for( i = 0; i < ARGS_HASH; ++i ) {
        int j;
        for( j = 0; j < TYPE_HASH_MODULUS; ++j ) {
            RingIterBeg( *head, curr ) {
                *p = curr->u.f.args;
                ++p;
            } RingIterEnd( curr )
            ++head;
        }
    }
    head = &fnTable[0];
    for( i = ARGS_HASH; i < ARGS_MAX; ++i ) {
        RingIterBeg( *head, curr ) {
            *p = curr->u.f.args;
            ++p;
        } RingIterEnd( curr )
        ++head;
    }
    RingIterBeg( uniqueTypes, curr ) {
        if( curr->id == TYP_FUNCTION ) {
            *p = curr->u.f.args;
            ++p;
        }
    } RingIterEnd( curr )
    qsort( table, count, sizeof( arg_list * ), cmpArgList );
    // remove duplicates
    s = &table[0];
    stop = &table[count];
    for( p = &table[1]; p < stop; ++p ) {
        if( *s == *p ) {
            --count;
        } else {
            ++s;
            *s = *p;
        }
    }
    PCHWriteUInt( count );
    for( p = table; p < &table[count]; ++p ) {
        args = *p;
        save_except_spec = args->except_spec;
        except_spec_count = 0;
        if( save_except_spec != NULL ) {
            for( etype = save_except_spec; *etype != NULL; ++etype ) {
                ++except_spec_count;
            }
        }
        args->except_spec = PCHSetUInt( except_spec_count );
        PCHWrite( args, offsetof( arg_list, type_list ) );
        for( i = 0; i < args->num_args; ++i ) {
            PCHWriteCVIndex( (cv_index)TypeGetIndex( args->type_list[i] ) );
        }
        if( save_except_spec != NULL ) {
            for( etype = save_except_spec; *etype != NULL; ++etype ) {
                PCHWriteCVIndex( (cv_index)TypeGetIndex( *etype ) );
            }
        }
        args->except_spec = save_except_spec;
    }
    data->count = count;
    data->translate = table;
}

static int cmpFindArgList( const void *kp, const void *tp )
{
    arg_list *key = *(arg_list **)kp;
    arg_list *table = *(arg_list **)tp;

    if( key < table ) {
        return( -1 );
    } else if( key > table ) {
        return( 1 );
    }
    return( 0 );
}

static arg_list *argListMapIndex( type_pch_walk *d, arg_list *index )
{
    if( PCHGetUInt( index ) < PCH_FIRST_INDEX ) {
        return( NULL );
    }
    DbgAssert( !( PCHGetUInt( index ) >= d->count + PCH_FIRST_INDEX ) );
    return d->translate[PCHGetUInt( index ) - PCH_FIRST_INDEX];
}

static arg_list *argListGetIndex( type_pch_walk *d, arg_list *arg )
{
    arg_list **found;

    if( arg == NULL ) {
        return( PCHSetUInt( PCH_NULL_INDEX ) );
    }
    found = bsearch( &arg, d->translate, d->count, sizeof( arg_list * ), cmpFindArgList );
    if( found == NULL ) {
        DbgAssert( 0 );
        return( PCHSetUInt( PCH_ERROR_INDEX ) );
    }
    return( PCHSetUInt( ( found - d->translate ) + PCH_FIRST_INDEX ) );
}

static void saveType( void *e, carve_walk_base *d )
{
    type_pch_walk *ed = (type_pch_walk *)d;
    TYPE s = e;
    TYPE save_next;
    TYPE save_of;
    SYMBOL save_sym;
    SCOPE save_scope;
    CLASSINFO *save_info;
    TYPE save_type;
    void *save_base;
    AUX_INFO *save_pragma;
    arg_list *save_args;
    char *save_string;

    if( s->id == TYP_FREE ) {
        return;
    }
    save_next = s->next;
    s->next = TypeGetIndex( save_next );
    save_of = s->of;
    s->of = TypeGetIndex( save_of );
    switch( s->id ) {
    case TYP_TYPEDEF:
    case TYP_ENUM:
        save_sym = s->u.t.sym;
        s->u.t.sym = SymbolGetIndex( save_sym );
        save_scope = s->u.t.scope;
        s->u.t.scope = ScopeGetIndex( save_scope );
        break;
    case TYP_CLASS:
        save_scope = s->u.c.scope;
        s->u.c.scope = ScopeGetIndex( save_scope );
        save_info = s->u.c.info;
        s->u.c.info = CarveGetIndex( carveCLASSINFO, save_info );
        break;
    case TYP_MEMBER_POINTER:
        save_type = s->u.mp.host;
        s->u.mp.host = TypeGetIndex( save_type );
        break;
    case TYP_MODIFIER:
        save_base = s->u.m.base;
        if( s->flag & TF1_BASED ) {
            switch( s->flag & TF1_BASED ) {
            case TF1_BASED_FETCH:
            case TF1_BASED_ADD:
                s->u.m.base = SymbolGetIndex( save_base );
                break;
            case TF1_BASED_STRING:
                s->u.m.base = StringGetIndex( save_base );
                break;
            }
        }
        save_pragma = s->u.m.pragma;
        s->u.m.pragma = PragmaGetIndex( save_pragma );
        break;
    case TYP_FUNCTION:
        save_args = s->u.f.args;
        s->u.f.args = argListGetIndex( ed, save_args );
        save_pragma = s->u.f.pragma;
        s->u.f.pragma = PragmaGetIndex( save_pragma );
        break;
    case TYP_TYPENAME:
        save_string = s->u.n.name;
        s->u.n.name = PCHSetUInt( strlen( s->u.n.name ) );
        break;
    }
    s->dbgflag |= ed->dbgflag_mask;
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    s->next = save_next;
    s->of = save_of;
    switch( s->id ) {
    case TYP_TYPEDEF:
    case TYP_ENUM:
        s->u.t.sym = save_sym;
        s->u.t.scope = save_scope;
        break;
    case TYP_CLASS:
        s->u.c.scope = save_scope;
        s->u.c.info = save_info;
        break;
    case TYP_MEMBER_POINTER:
        s->u.mp.host = save_type;
        break;
    case TYP_MODIFIER:
        s->u.m.base = save_base;
        s->u.m.pragma = save_pragma;
        break;
    case TYP_FUNCTION:
        s->u.f.pragma = save_pragma;
        s->u.f.args = save_args;
        break;
    case TYP_TYPENAME:
        PCHWrite( save_string, PCHGetUInt( s->u.n.name ) );
        s->u.n.name = save_string;
        break;
    }
}

static void saveClassInfo( void *e, carve_walk_base *d )
{
    CLASSINFO *s = e;
    BASE_CLASS *save_bases;
    NAME save_name;
    TYPE save_class_mod;
    AUX_INFO *save_pragma;
    FRIEND *friend;
    signed char friend_is_type;
    CDOPT_CACHE *save_cdopt_cache;

    if( s->free ) {
        return;
    }
    save_bases = s->bases;
    s->bases = BaseClassGetIndex( save_bases );
    save_name = s->name;
    s->name = NameGetIndex( save_name );
    save_cdopt_cache = s->cdopt_cache;
    s->cdopt_cache = NULL;
    save_class_mod = s->class_mod;
    s->class_mod = TypeGetIndex( save_class_mod );
    save_pragma = s->fn_pragma;
    s->fn_pragma = PragmaGetIndex( save_pragma );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    RingIterBeg( s->friends, friend ) {
        friend_is_type = FriendIsType( friend );
        PCHWriteVar( friend_is_type );
        if( friend_is_type ) {
            PCHWriteCVIndex( (cv_index)TypeGetIndex( FriendGetType( friend ) ) );
        } else {
            PCHWriteCVIndex( (cv_index)SymbolGetIndex( FriendGetSymbol( friend ) ) );
        }
    } RingIterEnd( friend )
    friend_is_type = -1;
    PCHWriteVar( friend_is_type );
    s->fn_pragma = save_pragma;
    s->class_mod = save_class_mod;
    s->cdopt_cache = save_cdopt_cache;
    s->name = save_name;
    s->bases = save_bases;
}

static void saveDeclInfo( void *e, carve_walk_base *d )
{
    DECL_INFO *s = e;
    DECL_INFO *save_next;
    DECL_INFO *save_parms;
    PTREE save_id;
    SCOPE save_scope;
    SCOPE save_friend_scope;
    SYMBOL save_sym;
    SYMBOL save_generic_sym;
    SYMBOL save_proto_sym;
    TYPE save_list;
    TYPE save_type;
    PTREE save_defarg_expr;
    REWRITE *save_body;
    REWRITE *save_mem_init;
    REWRITE *save_defarg_rewrite;
    NAME save_name;
    SRCFILE save_locn_src_file;

    if( s->free ) {
        return;
    }
    save_next = s->next;
    s->next = DeclInfoGetIndex( save_next );
    save_parms = s->parms;
    s->parms = DeclInfoGetIndex( save_parms );
    save_id = s->id;
    s->id = PTreeGetIndex( save_id );
    save_scope = s->scope;
    s->scope = ScopeGetIndex( save_scope );
    save_friend_scope = s->friend_scope;
    s->friend_scope = ScopeGetIndex( save_friend_scope );
    save_sym = s->sym;
    s->sym = SymbolGetIndex( save_sym );
    save_generic_sym = s->generic_sym;
    s->generic_sym = SymbolGetIndex( save_generic_sym );
    save_proto_sym = s->proto_sym;
    s->proto_sym = SymbolGetIndex( save_proto_sym );
    save_list = s->list;
    s->list = TypeGetIndex( save_list );
    save_type = s->type;
    s->type = TypeGetIndex( save_type );
    save_defarg_expr = s->defarg_expr;
    s->defarg_expr = PTreeGetIndex( save_defarg_expr );
    save_body = s->body;
    s->body = RewriteGetIndex( save_body );
    save_mem_init = s->mem_init;
    s->mem_init = RewriteGetIndex( save_mem_init );
    save_defarg_rewrite = s->defarg_rewrite;
    s->defarg_rewrite = RewriteGetIndex( save_defarg_rewrite );
    save_name = s->name;
    s->name = NameGetIndex( save_name );
    save_locn_src_file = s->init_locn.src_file;
    s->init_locn.src_file = SrcFileGetIndex( save_locn_src_file );

    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );

    s->next = save_next;
    s->parms = save_parms;
    s->id = save_id;
    s->scope = save_scope;
    s->friend_scope = save_friend_scope;
    s->sym = save_sym;
    s->generic_sym = save_generic_sym;
    s->proto_sym = save_proto_sym;
    s->list = save_list;
    s->type = save_type;
    s->defarg_expr = save_defarg_expr;
    s->body = save_body;
    s->mem_init = save_mem_init;
    s->defarg_rewrite = save_defarg_rewrite;
    s->name = save_name;
    s->init_locn.src_file = save_locn_src_file;
}

static void writeType( TYPE t )
{
    PCHWriteCVIndex( (cv_index)TypeGetIndex( t ) );
}

static void writeTypeHashed( TYPE *vector )
{
    int j;

    for( j = 0; j < TYPE_HASH_MODULUS; ++j ) {
        writeType( vector[j] );
    }
}

pch_status PCHWriteTypes( void )
{
    unsigned i;
    unsigned tci;
    auto type_pch_walk type_data;
    auto carve_walk_base data;

    memset( &type_data, 0, sizeof( type_data ) );
    writeType( TypeError );
    writeType( uniqueTypes );
    writeType( CompInfo.ptr_diff_near );
    writeType( CompInfo.ptr_diff_far );
    writeType( CompInfo.ptr_diff_far16 );
    writeType( CompInfo.ptr_diff_huge );
    for( tci = 0; tci < TYPC_LAST; ++tci ) {
        writeType( TypeCache[tci] );
    }
    for( i = 0; i < TYP_MAX; ++i ) {
        writeType( basicTypes[i] );
        writeType( typeTable[i] );
        switch(i) {
        case TYP_POINTER:
            writeTypeHashed( pointerHashTable );
            break;
        case TYP_BITFIELD:
            writeTypeHashed( bitfieldHashTable );
            break;
        case TYP_ARRAY:
            writeTypeHashed( arrayHashTable );
            break;
        case TYP_MODIFIER:
            writeTypeHashed( modifierHashTable );
            break;
        }
    }
    for( i = 0; i < ARGS_HASH; ++i ) {
        writeTypeHashed( fnHashTable[i] );
    }
    for( i = ARGS_HASH; i < ARGS_MAX; ++i ) {
        writeType( fnTable[i-ARGS_HASH] );
    }
    PCHWriteUInt( PCHGetUInt( PragmaGetIndex( cdeclPragma ) ) );
    pchWriteArgLists( &type_data );
    CarveWalkAllFree( carveTYPE, markFreeType );
    if( CompFlags.pch_debug_info_opt ) {
        type_data.dbgflag_mask = TF2_DBG_IN_PCH;
        PCHRelocStart( PCHRELOC_TYPE );
        CarveWalkAll( carveTYPE, saveType, &type_data.base );
        PCHRelocStop( PCHRELOC_TYPE );
    } else {
        CarveWalkAll( carveTYPE, saveType, &type_data.base );
    }
    PCHWriteCVIndexTerm();
    CarveWalkAllFree( carveCLASSINFO, markFreeClassInfo );
    CarveWalkAll( carveCLASSINFO, saveClassInfo, &data );
    PCHWriteCVIndexTerm();
    CarveWalkAllFree( carveDECL_INFO, markFreeDeclInfo );
    CarveWalkAll( carveDECL_INFO, saveDeclInfo, &data );
    PCHWriteCVIndexTerm();
    CMemFree( type_data.translate );
    return( PCHCB_OK );
}

static void readType( TYPE *t )
{
    *t = TypeMapIndex( (TYPE)PCHReadCVIndex() );
}

static void readTypeHashed( TYPE *vector )
{
    int j;

    for( j = 0; j < TYPE_HASH_MODULUS; ++j ) {
        readType( &vector[j] );
    }
}

static void readTypes( type_pch_walk *type_data )
{
    unsigned int l;
    TYPE t;
    auto cvinit_t data;

    CarveInitStart( carveTYPE, &data );
    for( ; (t = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *t );
        t->next = TypeMapIndex( t->next );
        t->of = TypeMapIndex( t->of );
        switch( t->id ) {
        case TYP_TYPEDEF:
        case TYP_ENUM:
            t->u.t.sym = SymbolMapIndex( t->u.t.sym );
            t->u.t.scope = ScopeMapIndex( t->u.t.scope );
            break;
        case TYP_CLASS:
            t->u.c.scope = ScopeMapIndex( t->u.c.scope );
            t->u.c.info = CarveMapIndex( carveCLASSINFO, t->u.c.info );
            break;
        case TYP_MEMBER_POINTER:
            t->u.mp.host = TypeMapIndex( t->u.mp.host );
            break;
        case TYP_MODIFIER:
            if( t->flag & TF1_BASED ) {
                switch( t->flag & TF1_BASED ) {
                case TF1_BASED_FETCH:
                case TF1_BASED_ADD:
                    t->u.m.base = SymbolMapIndex( t->u.m.base );
                    break;
                case TF1_BASED_STRING:
                    t->u.m.base = StringMapIndex( t->u.m.base );
                    break;
                }
            }
            t->u.m.pragma = PragmaMapIndex( t->u.m.pragma );
            break;
        case TYP_FUNCTION:
            t->u.f.args = argListMapIndex( type_data, t->u.f.args );
            t->u.f.pragma = PragmaMapIndex( t->u.f.pragma );
            break;
        case TYP_TYPENAME:
            l = PCHGetUInt( t->u.n.name );
            t->u.n.name = CMemAlloc( l + 1 );
            PCHRead( t->u.n.name, l );
            t->u.n.name[l] = '\0';
            break;
        default :
            break;
        }
    }
}

static void readClassInfos( void )
{
    CLASSINFO *ci;
    signed char friend_is_type;
    auto cvinit_t data;

    CarveInitStart( carveCLASSINFO, &data );
    for( ; (ci = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *ci );
        ci->bases = BaseClassMapIndex( ci->bases );
        ci->name = NameMapIndex( ci->name );
        ci->cdopt_cache = NULL;
        ci->class_mod = TypeMapIndex( ci->class_mod );
        ci->fn_pragma = PragmaMapIndex( ci->fn_pragma );
        ci->friends = NULL;
        for( ;; ) {
            PCHReadVar( friend_is_type );
            if( friend_is_type == -1 )
                break;
            if( friend_is_type ) {
                ScopeRawAddFriendType( ci, TypeMapIndex( (TYPE)PCHReadCVIndex() ) );
            } else {
                ScopeRawAddFriendSym( ci, SymbolMapIndex( (SYMBOL)PCHReadCVIndex() ) );
            }
        }
    }
}

static void readDeclInfos( void )
{
    DECL_INFO *dinfo;
    auto cvinit_t data;

    CarveInitStart( carveDECL_INFO, &data );
    for( ; (dinfo = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *dinfo );

        dinfo->next = DeclInfoMapIndex( dinfo->next );
        dinfo->parms = DeclInfoMapIndex( dinfo->parms );
        dinfo->id = PTreeMapIndex( dinfo->id );
        dinfo->scope = ScopeMapIndex( dinfo->scope );
        dinfo->friend_scope = ScopeMapIndex( dinfo->friend_scope );
        dinfo->sym = SymbolMapIndex( dinfo->sym );
        dinfo->generic_sym = SymbolMapIndex( dinfo->generic_sym );
        dinfo->proto_sym = SymbolMapIndex( dinfo->proto_sym );
        dinfo->list = TypeMapIndex( dinfo->list );
        dinfo->type = TypeMapIndex( dinfo->type );
        dinfo->defarg_expr = PTreeMapIndex( dinfo->defarg_expr );
        dinfo->body = RewriteMapIndex( dinfo->body );
        dinfo->mem_init = RewriteMapIndex( dinfo->mem_init );
        dinfo->defarg_rewrite = RewriteMapIndex( dinfo->defarg_rewrite );
        dinfo->name = NameMapIndex( dinfo->name );
        dinfo->init_locn.src_file = SrcFileMapIndex( dinfo->init_locn.src_file );
    }
}

pch_status PCHReadTypes( void )
{
    unsigned i;
    unsigned j;
    unsigned tci;
    unsigned arglist_count;
    unsigned except_spec_count;
    arg_list **translate;
    arg_list *args;
    arg_list **set;
    auto type_pch_walk type_data;
    auto arg_list tmp_arglist;

    readType( &TypeError );
    readType( &uniqueTypes );
    readType( &CompInfo.ptr_diff_near );
    readType( &CompInfo.ptr_diff_far );
    readType( &CompInfo.ptr_diff_far16 );
    readType( &CompInfo.ptr_diff_huge );
    for( tci = 0; tci < TYPC_LAST; ++tci ) {
        readType( &TypeCache[tci] );
    }
    for( i = 0; i < TYP_MAX; ++i ) {
        readType( &basicTypes[i] );
        readType( &typeTable[i] );
        switch( i ) {
        case TYP_POINTER:
            readTypeHashed( pointerHashTable );
            break;
        case TYP_BITFIELD:
            readTypeHashed( bitfieldHashTable );
            break;
        case TYP_ARRAY:
            readTypeHashed( arrayHashTable );
            break;
        case TYP_MODIFIER:
            readTypeHashed( modifierHashTable );
            break;
        }
    }
    for( i = 0; i < ARGS_HASH; ++i ) {
        readTypeHashed( fnHashTable[i] );
    }
    for( i = ARGS_HASH; i < ARGS_MAX; ++i ) {
        readType( &fnTable[i - ARGS_HASH] );
    }
    cdeclPragma = PragmaMapIndex( PCHSetUInt( PCHReadUInt() ) );
    arglist_count = PCHReadUInt();
    translate = CMemAlloc( arglist_count * sizeof( arg_list * ) );
    type_data.count = arglist_count;
    type_data.translate = translate;
    set = translate;
    for( i = 0; i < arglist_count; ++i ) {
        PCHRead( &tmp_arglist, offsetof( arg_list, type_list ) );
        args = AllocArgListPerm( tmp_arglist.num_args );
        args->qualifier = tmp_arglist.qualifier;
        for( j = 0; j < tmp_arglist.num_args; ++j ) {
            args->type_list[j] = TypeMapIndex( (TYPE)PCHReadCVIndex() );
        }
        except_spec_count = PCHGetUInt( tmp_arglist.except_spec );
        if( except_spec_count != 0 ) {
            args->except_spec = CPermAlloc( ( except_spec_count + 1 ) * sizeof( TYPE ) );
            for( j = 0; j < except_spec_count; ++j ) {
                args->except_spec[j] = TypeMapIndex( (TYPE)PCHReadCVIndex() );
            }
            args->except_spec[j] = NULL;
        } else {
            args->except_spec = NULL;
        }
        *set = args;
        ++set;
    }
    readTypes( &type_data );
    readClassInfos();
    readDeclInfos();
    CMemFreePtr( &translate );
    return( PCHCB_OK );
}

pch_status PCHInitTypes( boolean writing )
{
    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carveTYPE ) );
        PCHWriteCVIndex( CarveLastValidIndex( carveCLASSINFO ) );
        PCHWriteCVIndex( CarveLastValidIndex( carveDECL_INFO ) );
    } else {
        carveTYPE = CarveRestart( carveTYPE );
        CarveMapOptimize( carveTYPE, PCHReadCVIndex() );
        carveCLASSINFO = CarveRestart( carveCLASSINFO );
        CarveMapOptimize( carveCLASSINFO, PCHReadCVIndex() );
        carveDECL_INFO = CarveRestart( carveDECL_INFO );
        CarveMapOptimize( carveDECL_INFO, PCHReadCVIndex() );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniTypes( boolean writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveTYPE );
        CarveMapUnoptimize( carveCLASSINFO );
        CarveMapUnoptimize( carveDECL_INFO );
    }
    return( PCHCB_OK );
}

static void relocType( void *e, carve_walk_base *d )
{
    type_reloc_pch_walk *ed = (type_reloc_pch_walk *)d;
    TYPE s = e;
    unsigned pch_type_index;
    TYPE pch_type;

#define __type_reloc_size \
        ( _pch_align_size( sizeof( pch_type_index ) ) + \
          _pch_align_size( sizeof( *pch_type ) ) )
    if( s->id == TYP_FREE ) {
        return;
    }
    if( ! ( s->dbgflag & TF2_DBG_IN_PCH ) ) {
        return;
    }
    pch_type_index = *((unsigned*)(ed->curr));
    if( pch_type_index != d->index ) {
        return;
    }
    DbgAssert( ed->amount >= __type_reloc_size );
    DbgAssert(( ed->amount % __type_reloc_size ) == 0 );
    pch_type = (TYPE) &(ed->curr[ sizeof( pch_type_index ) ]);
    if( s->dbgflag & TF2_DWARF ) {
        if( s->dbgflag & TF2_SYMDBG ) {
            pch_type->dbgflag |= TF2_PCH_DBG_EXTERN;
            pch_type->dbg.pch_handle = DwarfDebugOffset( s->dbg.handle );
        }
    }
    ed->curr += __type_reloc_size;
    ed->amount -= __type_reloc_size;
#undef __type_reloc_size
}

pch_status PCHRelocTypes( char *block, unsigned size )
{
    auto type_reloc_pch_walk type_data;

    type_data.curr = block;
    type_data.amount = size;
    CarveWalkAllFree( carveTYPE, markFreeType );
    CarveWalkAll( carveTYPE, relocType, &type_data.base );
    DbgAssert( type_data.amount == 0 );
    return( PCHCB_OK );
}
