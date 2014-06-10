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
* Description:  Handle processing of declarations within classes.
*
****************************************************************************/


#include "plusplus.h"

#include <limits.h>

#include "walloca.h"
#include "codegen.h"
#include "cgfront.h"
#include "preproc.h"
#include "fnbody.h"
#include "ring.h"
#include "carve.h"
#include "vbuf.h"
#include "name.h"
#include "rewrite.h"
#include "yydriver.h"
#include "template.h"
#include "pcheader.h"
#include "decl.h"
#include "class.h"
#include "brinfo.h"
#include "pragdefn.h"

/*
    Future object model changes:
        - force vfptr to offset 0 (if possible)
                - ensure that the following matches VC++ under -zp8
                        struct S {
                            char c;
                            double d;
                            void virtual foo( int );
                        };
        - optimize more empty bases: when laying down a base class,
          overlay an empty base class on top of it
          (see notes\empty.c compiled with VC++)
        - record padding, stuff empty base classes into the padding
*/

#define ITEM_BITSIZE    (CHAR_BIT * sizeof( indices_t ))

typedef unsigned        indices_t;

static CLASS_DATA *classDataStack;

#define BLOCK_BASE_CLASS        16
#define BLOCK_VF_HIDE           16
static carve_t carveBASE_CLASS;
static carve_t carveVF_HIDE;

struct vf_hide {
    VF_HIDE             *next;
    SYMBOL_NAME         derived;
    SYMBOL_NAME         base;
};

#define _IS_DEFAULT_INLINE      ( OptSize <= 50 )

enum {
    CC_CONST    = 0x01, /* first arg is const */
    CC_REF      = 0x02, /* first arg is a reference */
    CC_NULL     = 0x00
};

typedef enum {
    PROP_BASE   = 0x01, /* propagating from a base class */
    PROP_STATIC = 0x02, /* propagating from a static member */
    PROP_NULL   = 0x00
} prop_type;

static uint_16 classIndex;

#ifndef NDEBUG
void DumpClasses( void )
{
    CLASS_DATA *data;

    printf( "---\n" );
    for( data = classDataStack; data != NULL; data = data->next ) {
        extern void DumpFullType( TYPE );
        printf( "%p: ", data );
        DumpFullType( data->type );
        printf( "'%s' "
            "inline_data %p "
            "\n",
            NameStr( data->name ),
            data->inline_data );
    }
}
#endif

void ClassInit( void )
/********************/
{
    carveBASE_CLASS = CarveCreate( sizeof( BASE_CLASS ), BLOCK_BASE_CLASS );
    carveVF_HIDE = CarveCreate( sizeof( VF_HIDE ), BLOCK_VF_HIDE );
    classIndex = 0;
}

void ClassFini( void )
/********************/
{
    DbgStmt( CarveVerifyAllGone( carveVF_HIDE, "VF_HIDE" ) );
    CarveDestroy( carveVF_HIDE );
    CarveDestroy( carveBASE_CLASS );
}

target_offset_t PackAlignment( target_offset_t pack_amount, target_size_t size )
/******************************************************************************/
{
    target_offset_t pack_adjustment;

    if( pack_amount == 1 ) {
        return( 1 );
    }
    pack_adjustment = pack_amount;
    switch( size ) {
    case 1:
        pack_adjustment = 1;
        break;
    case 2:
        if( pack_adjustment > 2 ) {
            pack_adjustment = 2;
        }
        break;
    case 4:
        if( pack_adjustment > 4 ) {
            pack_adjustment = 4;
        }
        break;
    case 8:
        if( pack_adjustment > 8 ) {
            pack_adjustment = 8;
        }
        break;
    }
    return( pack_adjustment );
}

static target_offset_t packSize( CLASS_DATA *data, TYPE type )
{
    target_size_t align_size;

    align_size = CgMemorySize( AlignmentType( type ) );
    return( PackAlignment( data->pack_amount, align_size ) );
}

static void doAlignment( CLASS_DATA *data, target_offset_t adjustment )
{
    target_size_t old_offset;
    target_size_t calc_offset;

    if( adjustment != 1 ) {
        old_offset = data->offset;
        calc_offset = _RoundUp( old_offset, adjustment );
        if( _CHECK_OFFS( calc_offset, data->offset ) ) {
            CErr1( ERR_MAX_STRUCT_EXCEEDED );
        }
        data->offset = calc_offset;
        if( calc_offset != old_offset && CompFlags.warn_about_padding ) {
            CErr2( WARN_PADDING_ADDED, ( calc_offset - old_offset ) );
        }
    }
}

static target_offset_t addField( CLASS_DATA *data, target_size_t size,
                                 target_offset_t pack_amount )
{
    target_size_t calc_offset;
    target_offset_t start;
    target_offset_t save_end;

    if( pack_amount > data->max_align ) {
        data->max_align = pack_amount;
    }
    save_end = data->offset;
    if( data->is_union ) {
        data->offset = data->start;
    }
    doAlignment( data, pack_amount );
    start = data->offset;
    calc_offset = start;
    calc_offset += size;
    if( _CHECK_OFFS( calc_offset, start ) ) {
        CErr1( ERR_MAX_STRUCT_EXCEEDED );
    }
    data->offset = calc_offset;
    if( data->is_union ) {
        if( save_end > data->offset ) {
            data->offset = save_end;
        }
    }
    return( start );
}

static target_offset_t addTypeField( CLASS_DATA *data, TYPE type )
{
    return( addField( data, CgMemorySize( type ), packSize( data, type ) ) );
}

static target_offset_t addZeroSizedField( CLASS_DATA *data, TYPE type )
{
    return( addField( data, 0, packSize( data, type ) ) );
}

void ClassInitState( type_flag class_variant, CLASS_INIT extra, TYPE class_mod_list )
/***********************************************************************************/
{
    CLASS_DATA *data;
    CLASS_DATA *prev_data;
    unsigned packing;
    TYPE class_mod_type;

    packing = PackAmount;
    if( extra & CLINIT_PACKED ) {
        packing = TARGET_CHAR;
    }
    data = classDataStack;
    data->sym = NULL;
    data->type = NULL;
    data->scope = NULL;
    data->enclosing = NULL;
    data->info = NULL;
    data->bases = NULL;
    data->base_vbptr = NULL;
    data->base_vfptr = NULL;
    data->name = NULL;
    data->saved_inlines = NULL;
    data->nested_inlines = NULL;
    data->inlines = NULL;
    data->defargs = NULL;
    data->vf_hide_list = NULL;
    data->offset = 0;
    data->vb_offset = 0;
    data->vf_offset = 0;
    data->vf_index = 0;
    data->vb_index = 0;
    data->max_align = 0;
    data->pack_amount = packing;
    data->tflag = class_variant;
    data->bitfield = FALSE;
    data->defined = FALSE;
    data->local_class = FALSE;
    data->nested_class = FALSE;
    data->own_vfptr = FALSE;
    data->nameless_OK = FALSE;
    data->generic = FALSE;
    data->specific_defn = FALSE;
    data->is_explicit = TRUE;
    data->is_union = FALSE;
    data->zero_array_defd = FALSE;
    data->member_mod_adjust = FALSE;
    data->class_idiom = FALSE;
    data->class_template = FALSE;
    data->a_public = FALSE;
    data->a_protected = FALSE;
    data->a_private = FALSE;
    data->a_const = FALSE;
    data->a_reference = FALSE;
    data->has_const_copy = FALSE;
    data->has_nonconst_copy = FALSE;
    data->has_const_opeq = FALSE;
    data->has_nonconst_opeq = FALSE;
    data->has_explicit_opeq = FALSE;
    data->in_defn = FALSE;
    class_mod_type = NULL;
    if( class_mod_list != NULL ) {
        class_mod_type = ProcessClassModifiers( class_mod_list, &(data->mod_flags), &(data->fn_flags), &(data->fn_pragma) );
        data->member_mod_adjust = TRUE;
    }
    data->class_mod_type = class_mod_type;
    if( extra & CLINIT_TEMPLATE_DECL ) {
        data->class_template = TRUE;
    }
    prev_data = data->next;
    if( prev_data != NULL ) {
        if( ( prev_data->info != NULL ) && prev_data->info->defined ) {
            data->inline_data = NULL;
        } else {
            /* our inline fns go where any previous class' inline fns
             * go, but only if the previous class isn't already fully
             * defined.
             *
             * Use strict sequential ordering within a class, but
             * nested classes need to be processed first, e.g.
             *
             * struct A {
             *   struct B {
             *     struct C { };
             *   };
             *   struct D {
             *     struct E { };
             *   };
             * };
             *
             * will result in inline fns being processed in the
             * following order:
             *
             * A::B::C, A::B, A::D::E, A::D, A
             *
             * also see bugzilla #63.
             */
            data->inline_data = prev_data->inline_data;
            if( data->inline_data != NULL ) {
                if( prev_data->nested_inlines != data->inline_data->inlines ) {
                    data->saved_inlines = data->inline_data->inlines;
                }
                if( prev_data->nested_inlines != NULL ) {
                    data->inline_data->inlines = prev_data->nested_inlines;
                }
                data->nested_inlines = data->inline_data->inlines;
            }
        }
    } else {
        data->inline_data = NULL;
    }
    switch( ScopeId( GetCurrScope() ) ) {
    case SCOPE_BLOCK:
        data->local_class = TRUE;
        break;
    case SCOPE_CLASS:
        data->nested_class = TRUE;
        break;
    case SCOPE_TEMPLATE_INST:
        data->tflag |= TF1_INSTANTIATION;
        break;
    }
    if( class_variant & (TF1_STRUCT|TF1_UNION) ) {
        data->perm = SF_NULL;
        if( class_variant & TF1_UNION ) {
            data->is_union = TRUE;
        }
    } else {
        data->perm = SF_PRIVATE;
    }
    CErrCheckpoint( &(data->errors) );
}


void ClassAddFunctionMods( TYPE fn_declarator )
/*********************************************/
{
    CLASS_DATA *data;

    data = classDataStack;
    if( data != NULL && data->member_mod_adjust ) {
        SetFnClassMods( fn_declarator, data->fn_flags, data->fn_pragma );
    }
}

void ClassPermission( symbol_flag new_perm )
/******************************************/
{
    classDataStack->perm = new_perm;
}

static void typeError( MSG_NUM msg, TYPE type )
{
    CErr2p( msg, type );
}

static bool verifyNoChangePerm( CLASS_DATA *data, symbol_flag perm, NAME name )
{
    if( perm != data->perm ) {
        /* data->perm is 'protected' or 'public' */
        if( data->perm & SF_PROTECTED ) {
            if( perm & SF_PRIVATE ) {
                CErr2p( ERR_ACCESS_DECL_INCREASE, name );
            } else {
                CErr2p( ERR_ACCESS_DECL_DECREASE, name );
            }
        } else {
            CErr2p( ERR_ACCESS_DECL_INCREASE, name );
        }
        return( TRUE );
    }
    return( FALSE );
}

static bool handleAccessDeclaration( PTREE id_tree )
{
    PTREE scope_tree;
    PTREE name_tree;
    CLASS_DATA *data;
    NAME name;
    TYPE type;
    TYPE udc_return_type;
    SCOPE scope;
    SCOPE curr_scope;
    SEARCH_RESULT *result;
    SYMBOL_NAME sym_name;
    SYMBOL sym;
    SYMBOL curr_sym;
    SYMBOL check_sym;
    SYMBOL access_sym;
    symbol_flag perm;
    symbol_flag curr_perm;
    bool error_diagnosed;
    auto TOKEN_LOCN name_locn;

    error_diagnosed = FALSE;
    data = classDataStack;
    if( data->perm & SF_PRIVATE ) {
        CErr1( ERR_ACCESS_DECL_IN_PRIVATE );
        error_diagnosed = TRUE;
    }
    if( id_tree->op != PT_BINARY || id_tree->cgop != CO_COLON_COLON ) {
        /* error occurred in the C part of C::id (ignore this declaration) */
        return( error_diagnosed );
    }
    name_locn = id_tree->locn;
    name_tree = id_tree->u.subtree[1];
    name = name_tree->u.id.name;
    udc_return_type = NULL;
    if( name == CppConversionName() ) {
        udc_return_type = name_tree->type;
    }
    type = NULL;
    scope_tree = id_tree->u.subtree[0];
    if( scope_tree != NULL ) {
        type = scope_tree->type;
    }
    PTreeFreeSubtrees( id_tree );
    if( type == NULL ) {
        /* error occurred in the C part of C::id (ignore this declaration) */
        return( error_diagnosed );
    }
    curr_scope = data->scope;
    if( ! ScopeDirectBase( curr_scope, type ) ) {
        typeError( ERR_NOT_A_DIRECT_BASE_CLASS, type );
        return( TRUE );
    }
    scope = type->u.c.scope;
    if( udc_return_type != NULL ) {
        result = ScopeFindScopedMemberConversion( curr_scope, scope, udc_return_type, TF1_NULL );
    } else {
        result = ScopeFindScopedMember( curr_scope, scope, name );
    }
    if( result == NULL ) {
        CErr2p( ERR_NOT_A_MEMBER_NAME, name );
        return( TRUE );
    }
    if( result->scope != scope && result->access_decl != scope ) {
        /* we have to find a meaning for this case if we allow it */
        /* (save any code that requires this to work for analysis) AFS */
        ScopeFreeResult( result );
        CErr2p( ERR_NOT_A_MEMBER_NAME, name );
        return( TRUE );
    }
    sym_name = result->sym_name;
    sym = sym_name->name_syms;
    DbgAssert( sym != NULL );
    check_sym = NULL;
    perm = SF_NULL;
    RingIterBeg( sym, curr_sym ) {
        if( udc_return_type != NULL ) {
            /* filter out user-defined conversions with the wrong return type */
            if( !SymIsConversionToType( curr_sym, udc_return_type ) ) {
                continue;
            }
        }
        if( check_sym == NULL ) {
            check_sym = curr_sym;
            perm = check_sym->flag & ( SF_PROTECTED | SF_PRIVATE );
        } else {
            curr_perm = curr_sym->flag & ( SF_PROTECTED | SF_PRIVATE );
            if(( perm ^ curr_perm ) != SF_NULL ) {
                if( ! error_diagnosed ) {
                    CErr2p( ERR_ACCESS_DECL_ALL_SAME, name );
                    error_diagnosed = TRUE;
                }
            }
        }
        if( ! SymIsStatic( sym ) ) {
            /* we don't want static member function ambiguity to save us */
            check_sym = sym;
        }
    } RingIterEnd( curr_sym )
    if( ! error_diagnosed ) {
        error_diagnosed = ScopeAmbiguousSymbol( result, check_sym );
    }
    ScopeFreeResult( result );
    if( error_diagnosed ) {
        return( error_diagnosed );
    }
    if( verifyNoChangePerm( data, perm, name ) ) {
        return( TRUE );
    }
    access_sym = SymCreateAtLocn( type, SC_ACCESS, SF_NULL, name, GetCurrScope(), &name_locn );
    if( access_sym != NULL ) {
        access_sym->u.udc_type = udc_return_type;
    }
    return( FALSE );
}

void ClassAccessDeclaration( PTREE id_tree, TOKEN_LOCN *locn )
/************************************************************/
{
    SetErrLoc( locn );
    if( ! handleAccessDeclaration( id_tree ) ) {
        // no error message printed
        SetErrLoc( NULL );
    }
}

static bool handleAccessTypeDeclaration( DECL_SPEC *dspec, TOKEN_LOCN *locn )
{
    CLASS_DATA *data;
    NAME name;
    TYPE type;
    TYPE base_type;
    SCOPE scope;
    SCOPE curr_scope;
    SEARCH_RESULT *result;
    SYMBOL_NAME sym_name;
    SYMBOL sym;
    PTREE id;
    symbol_flag perm;
    bool error_diagnosed;

    error_diagnosed = FALSE;
    data = classDataStack;
    if( data->perm & SF_PRIVATE ) {
        CErr1( ERR_ACCESS_DECL_IN_PRIVATE );
        error_diagnosed = TRUE;
    }
    type = dspec->partial;
    scope = dspec->scope;
    id = dspec->id;
    if( type == NULL || scope == NULL || id == NULL ) {
        return( error_diagnosed );
    }
    DbgAssert( NodeIsBinaryOp( id, CO_STORAGE ) );
    name = id->u.subtree[1]->u.id.name;
    DbgAssert( name != NULL );
    curr_scope = data->scope;
    base_type = ScopeClass( scope );
    if( base_type == NULL ) {
        CErr2p( ERR_NOT_A_MEMBER_NAME, name );
        return( TRUE );
    }
    if( ! ScopeDirectBase( curr_scope, base_type ) ) {
        typeError( ERR_NOT_A_DIRECT_BASE_CLASS, base_type );
        return( TRUE );
    }
    result = ScopeFindScopedMember( curr_scope, scope, name );
    if( result == NULL ) {
        CErr2p( ERR_NOT_A_MEMBER_NAME, name );
        return( TRUE );
    }
    if( result->scope != scope && result->access_decl != scope ) {
        /* we have to find a meaning for this case if we allow it */
        /* (save any code that requires this to work for analysis) AFS */
        ScopeFreeResult( result );
        CErr2p( ERR_NOT_A_MEMBER_NAME, name );
        return( TRUE );
    }
    sym_name = result->sym_name;
    sym = sym_name->name_type;
    if( sym == NULL ) {
        ScopeFreeResult( result );
        CErr2p( ERR_NOT_A_MEMBER_NAME, name );
        return( TRUE );
    }
    if( ! error_diagnosed ) {
        error_diagnosed = ScopeAmbiguousSymbol( result, sym );
    }
    ScopeFreeResult( result );
    if( error_diagnosed ) {
        return( error_diagnosed );
    }
    perm = sym->flag & ( SF_PROTECTED | SF_PRIVATE );
    if( verifyNoChangePerm( data, perm, name ) ) {
        return( TRUE );
    }
    SymCreateAtLocn( type, SC_TYPEDEF, SF_NULL, name, GetCurrScope(), locn );
    return( FALSE );
}

void ClassAccessTypeDeclaration( DECL_SPEC *dspec, TOKEN_LOCN *locn )
/*******************************************************************/
{
    SetErrLoc( locn );
    if( ! handleAccessTypeDeclaration( dspec, locn ) ) {
        // no error message printed
        SetErrLoc( NULL );
    }
    PTypeRelease( dspec );
}

static void setScopeOwner( TYPE class_type, SCOPE class_scope )
{
    class_type->u.c.scope = class_scope;
    ScopeSetClassOwner( class_scope, class_type );
}

void ClassChangingScope( SYMBOL typedef_sym, SCOPE new_scope )
/************************************************************/
{
    TYPE class_type;
    SCOPE class_scope;

    class_type = StructType( typedef_sym->sym_type );
    class_scope = class_type->u.c.scope;
    ScopeAdjustUsing( GetCurrScope(), NULL );
    ScopeEstablishEnclosing( class_scope, new_scope );
    ScopeAdjustUsing( NULL, GetCurrScope() );
}

static TYPE createClassType( NAME name, type_flag flag )
{
    TYPE class_type;
    SCOPE class_scope;
    CLASSINFO *info;

    class_type = MakeClassType();
    class_scope = ScopeCreate( SCOPE_CLASS );
    // Empty classes must be kept because C++ requires that empty classes
    // be valid and occupy a non-zero amount of memory.
    ScopeKeep( class_scope );
    class_type->flag = flag;
    setScopeOwner( class_type, class_scope );
    ScopeEstablish( class_scope );
    info = class_type->u.c.info;
    if( name != NULL ) {
        info->name = name;
    } else {
        info->unnamed = TRUE;
    }
    info->const_copy = TRUE;
    info->const_assign = TRUE;
    info->ctor_user_code = TRUE;
    info->copy_user_code = TRUE;
    info->dtor_user_code = TRUE;
    info->assign_user_code = TRUE;
    return( class_type );
}

static void classOpen( CLASS_DATA *data, CLASSINFO *info )
{
    if( data->inline_data == NULL ) {
        /* we can stash inline defns here because ClassEnd will execute */
        data->inline_data = data;
    }
    data->in_defn = TRUE;
    info->opened = TRUE;
    info->class_mod = data->class_mod_type;
}

static void newClassType( CLASS_DATA *data, CLASS_DECL declaration )
{
    TYPE class_type;
    TYPE generic_type;
    CLASSINFO *info;

    if( data->generic ) {
        generic_type = MakeType( TYP_GENERIC );
        data->type = generic_type;
    } else {
        class_type = createClassType( data->name, data->tflag );
        data->type = class_type;
        info = class_type->u.c.info;
        data->info = info;
        data->scope = class_type->u.c.scope;
        
        /*
         * A declspec modifier has been applied to the class
         * definition/declaration. Store the information into the
         * CLASS_INFO for subsequent use and checking
         */
        if( data->member_mod_adjust ) {
            info->class_mod = data->class_mod_type;
            info->fn_pragma = data->fn_pragma;
            info->fn_flags = data->fn_flags;    
            info->mod_flags = data->mod_flags;
        }
        
        if( declaration == CLASS_DEFINITION ) {
            if( ScopeType( data->scope->enclosing, SCOPE_TEMPLATE_INST ) ) {
                TYPE unbound_class =
                    data->scope->enclosing->owner.inst->unbound_type;
                DbgAssert( unbound_class != NULL );
                DbgAssert( unbound_class->of == NULL );
                unbound_class->of = class_type;
            }

            classOpen( data, info );
        }
    }
}

static void setClassType( CLASS_DATA *data, TYPE type, CLASS_DECL declaration )
{
    TYPE class_type;
    CLASSINFO *info;

    class_type = TypedefRemove( type );
    data->name = SimpleTypeName( class_type );
    data->type = class_type;
    info = class_type->u.c.info;
    data->info = info;
    data->scope = class_type->u.c.scope;
    
    /*
     * A declspec modifier has been applied to the class
     * definition/declaration. If the definition is not consistent
     * with any previous declarations, then generate an error
     */
    if( info->class_mod != NULL ) {
        if( data->member_mod_adjust ) {
            if( ! IdenticalClassModifiers( info->class_mod,
                                           data->class_mod_type ) ) {
                CErr1( ERR_MULTIPLE_PRAGMA_MODS );
            }
        } else {
            data->class_mod_type = info->class_mod;
            data->fn_pragma = info->fn_pragma;
            data->fn_flags = info->fn_flags;
            data->mod_flags = info->mod_flags;
        }

        data->member_mod_adjust = TRUE;
    }

    if( declaration == CLASS_DEFINITION ) {
        classOpen( data, info );
    }
}

TYPE ClassUnboundTemplate( NAME name )
/************************************/
{
    TYPE type;

    type = createClassType( name, TF1_STRUCT | TF1_UNBOUND );
    return( type );
}

static SYMBOL getClassSym( CLASS_DATA *data )
{
    SYMBOL sym;

    sym = AllocSymbol();
    sym->id = SC_TYPEDEF;
    sym->sym_type = data->type;
    return( sym );
}

static void injectClassName( NAME name, TYPE type, TOKEN_LOCN *locn )
{
    SCOPE scope = type->u.c.scope;
    SYMBOL sym = AllocSymbol();
    SYMBOL_NAME sym_name = AllocSymbolName( name, scope->enclosing );

    sym->id = SC_TYPEDEF;
    sym->sym_type = type;
    sym_name->name_type = sym;

    SymbolLocnDefine( locn, sym );
    sym->name = sym_name;
    sym->next = sym;

    HashInsert( scope->names, sym_name, name );
}

static void newClassSym( CLASS_DATA *data, CLASS_DECL declaration, PTREE id )
{
    SYMBOL sym;

    sym = getClassSym( data );
    SymbolLocnDefine( &(id->locn), sym );
    if( declaration == CLASS_DEFINITION ) {
        InsertSymbol( GetCurrScope(), sym, data->name );
    } else {
        data->sym = sym;
    }

    if( declaration == CLASS_DEFINITION ) {
        injectClassName( data->name, data->type, &(id->locn) );
    }
}

TYPE ClassTagDefinition( TYPE type, NAME name )
/*********************************************/
{
    TYPE class_type;
    CLASSINFO *info;

    class_type = StructType( type );
    if( class_type == NULL ) {
        return( NULL );
    }
    // 14.2.1 para 2 <class T> can be referenced as:
    // class T, struct T        if type is a class
    // union T                  if type is an union
    if( type->id == TYP_TYPEDEF ) {
        if( ScopeType( type->u.t.scope, SCOPE_TEMPLATE_PARM )
         || ScopeType( type->u.t.scope, SCOPE_TEMPLATE_SPEC_PARM ) ) {
            if( type->u.t.sym->name->name == name ) {
                return( class_type );
            }
        }
    }
    info = class_type->u.c.info;
    if( info->unnamed || info->name != name ) {
        return( NULL );
    }
    return( class_type );
}

static CLNAME_STATE processClassTemplate( CLASS_DATA *data, SCOPE scope,
                                          CLASS_DECL declaration,
                                          PTREE id )
{
    CLNAME_STATE ret;

    data->nameless_OK = TRUE;
    if( declaration == CLASS_DECLARATION ) {
        TemplateClassDeclaration( id, scope, data->name );
        ret = CLNAME_NULL;
    } else {
        if( TemplateClassDefinition( id, scope, data->name ) ) {
            ret = CLNAME_PROBLEM;
        } else {
            ret = CLNAME_CONTINUE;
        }
    }
    PTreeFreeSubtrees( id );
    return( ret );
}

TYPE ClassPreDefined( NAME name, TOKEN_LOCN *locn )
/*************************************************/
{
    TYPE class_type;
    PTREE id;
    SYMBOL_NAME sym_name;
    SYMBOL_NAME std_sym_name;
    SYMBOL std_sym;
    auto CLASS_DATA data;

    id = NULL;
    ClassPush( &data );
    ClassInitState( TF1_NULL, CLINIT_NULL, NULL );
    std_sym_name = ScopeYYMember( GetFileScope(), CppSpecialName( SPECIAL_STD ) );
    std_sym = ( std_sym_name != NULL ) ? std_sym_name->name_type : NULL;
    if( ( std_sym != NULL ) && ( std_sym->id == SC_NAMESPACE ) ) {
        sym_name = ScopeYYMember( std_sym->u.ns->scope, name );
        if( sym_name != NULL ) {
            id = PTreeId( name );
            id = PTreeSetLocn( id, locn );
            id = PTreeBinary( CO_COLON_COLON, PTreeId( std_sym_name->name ), id );
            id = PTreeSetLocn( id, locn );
            id->sym_name = sym_name;
        }
    }
    if( id == NULL ) {
        id = PTreeId( name );
        id = PTreeSetLocn( id, locn );
    }
    ClassName( id, CLASS_DECLARATION );
    class_type = data.type;
    ClassPop( &data );
    return( class_type );
}

CLNAME_STATE ClassName( PTREE id, CLASS_DECL declaration )
/********************************************************/
{
    bool scoped_id;
    bool something_went_wrong;
    NAME name;
    CLASS_DATA *data;
    CLASS_DATA *enclosing_data;
    TYPE class_type;
    TYPE type;
    SCOPE scope;
    SYMBOL sym;
    SYMBOL_NAME sym_name;
    SEARCH_RESULT *result;

    data = classDataStack;
    if( id == NULL ) {
        if( data->class_template ) {
            CErr1( ERR_CLASS_TEMPLATE_MUST_BE_NAMED );
        }
        newClassType( data, declaration );
        return( CLNAME_NULL );
    }

    something_went_wrong = FALSE;
    scoped_id = FALSE;
    scope = GetCurrScope();

    sym_name = NULL;
    if( id->op == PT_ID ) {
        name = id->u.id.name;
    } else if( id->op == PT_ERROR ) {
        something_went_wrong = TRUE;
        name = NameDummy();
    } else {
        sym_name = id->sym_name;
        DbgAssert( sym_name != NULL );

        if( ClassTypeName( sym_name ) ) {
            PTREE right = id->u.subtree[1];

            scoped_id = TRUE;
            name = right->u.id.name;

            scope = sym_name->containing;
        } else {
            CErr2p( ERR_QUALIFIED_NAME_NOT_CLASS, id );
            something_went_wrong = TRUE;
            name = NameDummy();
        }
    }

    data->name = name;
    if( declaration == CLASS_GENERIC ) {
        data->generic = TRUE;
        newClassType( data, declaration );
        newClassSym( data, declaration, id );
        PTreeFreeSubtrees( id );
        return( CLNAME_NULL );
    }
    if( declaration != CLASS_REFERENCE ) {
        if( declaration == CLASS_DECLARATION ) {
            data->class_idiom = TRUE;
        }
        if( data->class_template ) {
            return( processClassTemplate( data, scope, declaration, id ) );
        }
    }

    if( ! scoped_id ) {
        result = ScopeFindLexicalClassType( scope, name );
        if( result != NULL ) {
            sym_name = result->sym_name;
            scope = result->scope;
            ScopeFreeResult( result );
        } else {
            sym_name = NULL;
            scope = GetCurrScope();
        }
    }

    if( sym_name != NULL ) {
        sym = sym_name->name_type;
        type = sym->sym_type;
        class_type = ClassTagDefinition( type, name );
        if( class_type == NULL ) {
            /* 'class C' references can't be OK for 'typedef struct S C;' */
            sym_name = NULL;
        } else {
            if( data->is_union ) {
                if(( class_type->flag & TF1_UNION ) == 0 ) {
                    typeError( ERR_CLASS_REF_UNION_CLASS, class_type );
                }
            } else {
                if(( class_type->flag & TF1_UNION ) != 0 ) {
                    typeError( ERR_CLASS_REF_CLASS_UNION, class_type );
                }
            }
        }
    }
    if( sym_name != NULL ) {
        if( declaration != CLASS_REFERENCE ) {
            if( scoped_id || ( scope == GetCurrScope() ) ) {
                if( declaration == CLASS_DEFINITION && TypeDefined( type ) ) {
                    typeError( ERR_CLASS_REDEFINED, type );
                    newClassType( data, declaration );
                } else {
                    if( declaration == CLASS_DEFINITION ) {
                        if( data->nested_class ) {
                            enclosing_data = data->next;
                            if( enclosing_data != NULL ) {
                                if( !scoped_id
                                 && ( name == enclosing_data->name ) ) {
                                    newClassType( data, declaration );
                                    newClassSym( data, declaration, id );
                                    PTreeFreeSubtrees( id );
                                    return( CLNAME_NULL );
                                } else if(( enclosing_data->perm ^ sym->flag ) & SF_ACCESS ) {
                                    typeError( ERR_CLASS_ACCESS, type );
                                }
                            }
                        }
                        SymbolLocnDefine( &(id->locn), sym );
                        injectClassName( data->name, TypedefRemove( sym->sym_type ), NULL );
                    }
                    setClassType( data, type, declaration );
                    if( declaration == CLASS_DECLARATION ) {
                        data->nameless_OK = TRUE;
                    }
                }
            } else {
                newClassType( data, declaration );
                newClassSym( data, declaration, id );
            }
        } else {
            data->type = type;
        }
    } else {
        newClassType( data, declaration );
        newClassSym( data, declaration, id );
    }
    PTreeFreeSubtrees( id );
    return( CLNAME_NULL );
}

void ClassSpecificInstantiation( PTREE id, CLASS_DECL declaration, tc_directive tcd_control )
/*******************************************************************************************/
{
    TYPE type;
    CLASS_DATA *data;
    SCOPE enclosing;

    enclosing = GetCurrScope()->enclosing;
    data = classDataStack;

    if( NodeIsBinaryOp( id, CO_STORAGE ) ) {
        type = id->u.subtree[1]->type;
    } else {
        type = id->type;
    }

    switch( declaration ) {
    case CLASS_DEFINITION:
        if( ScopeType( GetCurrScope(), SCOPE_FILE ) ) {
            /* old template specialization syntax */
            data->specific_defn = TRUE;
            data->tflag |= TF1_SPECIFIC | TF1_INSTANTIATION;
            TemplateSpecificDefnStart( id, type );
        } else if ( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_INST ) ) {
            /* new template specialization syntax: instantiation */
            if( ScopeType( enclosing, SCOPE_TEMPLATE_SPEC_PARM )
             && ( ScopeOrderedFirst( enclosing ) == NULL ) ) {
                // empty spec-parm scope => explicit specialization
                data->tflag |= TF1_SPECIFIC;
            }
        } else if ( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_DECL )
                 && ScopeType( enclosing, SCOPE_FILE ) ) {
            /* new template specialization syntax: definition */
            TemplateSpecializationDefn( type );
        } else {
            CErr1( ERR_ONLY_GLOBAL_SPECIFICS );
        }
        ClassName( id, declaration );
        break;
    case CLASS_DECLARATION:
        if( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_INST ) ) {
            ClassName( id, declaration );
            break;
        } else if( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_DECL )
                && ScopeType( GetCurrScope()->enclosing, SCOPE_FILE ) ) {
            /* new template specialization syntax: declaration */
            TemplateSpecializationDefn( type );
            ClassName( id, declaration );
            break;
        }
        data->nameless_OK = TRUE;
        /* fall through */
    case CLASS_REFERENCE:
        data = classDataStack;
        setClassType( data, type, declaration );
        if( tcd_control != TCD_NULL ) {
            TemplateClassDirective( type, &(id->locn), tcd_control );
        }
        NodeFreeDupedExpr( id );
        break;
    }
}

static uint_16 nextClassIndex( void )
{
    uint_16 old_index;
    uint_16 new_index;

    old_index = classIndex;
    new_index = old_index + 1;
    if( new_index < old_index ) {
        new_index = old_index;
    } else {
        classIndex = new_index;
    }
    return( new_index );
}

void ClassStart( void )
/*********************/
{
    CLASS_DATA *data;
    CLASSINFO *info;
    TYPE type;
    SCOPE scope;

    data = classDataStack;
    data->start = data->offset;
    type = data->type;
    info = type->u.c.info;
    info->index = nextClassIndex();
    scope = data->scope;
    data->enclosing = GetCurrScope();
    SetCurrScope( scope );
    BrinfOpenScope( GetCurrScope() );
}

static void changeToInlineFunction( DECL_INFO *dinfo )
{
    SYMBOL sym;
    TYPE type;

    sym = dinfo->sym;
    if( sym == NULL ) {
        return;
    }
    type = sym->sym_type;
    if( FunctionDeclarationType( type ) != NULL ) {
        sym->sym_type = MakeInlineFunction( type );
    }
}

void ClassProcessFunction( DECL_INFO *inline_func, bool is_inline )
{
    REWRITE *last_rewrite;
    void (*last_source)( void );

    ParseFlush();
    last_source = SetTokenSource( RewriteToken );
    last_rewrite = RewriteRewind( inline_func->body );
    if( is_inline ) {
        changeToInlineFunction( inline_func );
    }
    FunctionBody( inline_func );
    RewriteClose( last_rewrite );
    ResetTokenSource( last_source );
}

static void defineInlineFuncsAndDefArgExprs( CLASS_DATA *data )
{
    DECL_INFO *curr;
    DECL_INFO *parm;
    REWRITE *last_rewrite;
    REWRITE *defarg_rewrite;
    void (*last_source)( void );
    TOKEN_LOCN locn;
    SCOPE save_scope;
    SCOPE sym_scope;
    PTREE defarg_expr;

    ExpectingToken( T_RIGHT_BRACE );

    SrcFileGetTokenLocn( &locn );
    // process default args
    save_scope = GetCurrScope();
    for(;;) {
        curr = RingPop( &(data->defargs) );
        if( curr == NULL ) break;
        sym_scope = SymScope( curr->sym );
        ScopeAdjustUsing( GetCurrScope(), sym_scope );
        SetCurrScope( sym_scope );
        RingIterBeg( curr->parms, parm ) {
            if( parm->has_defarg ) {
                ParseFlush();
                last_source = SetTokenSource( RewriteToken );
                defarg_rewrite = parm->defarg_rewrite;
                last_rewrite = RewriteRewind( defarg_rewrite );
                defarg_expr = ParseDefArg();
                parm->defarg_rewrite = NULL;
                parm->defarg_expr = defarg_expr;
                if( defarg_expr == NULL ) {
                    // we had some parsing problem in the defarg expr
                    parm->has_defarg = FALSE;
                }
                RewriteClose( last_rewrite );
                RewriteFree( defarg_rewrite );
                ResetTokenSource( last_source );
            }
        } RingIterEnd( parm )
        ProcessDefArgs( curr ); // frees 'curr'
    }
    ScopeAdjustUsing( GetCurrScope(), save_scope );
    SetCurrScope( save_scope );
    // process inline functions
    for(;;) {
        curr = RingPop( &(data->inlines) );
        if( curr == NULL ) break;
        ClassProcessFunction( curr, TRUE );
        FreeDeclInfo( curr );
    }
    SrcFileResetTokenLocn( &locn );
    CurToken = T_RIGHT_BRACE;
    strcpy( Buffer, Tokens[ T_RIGHT_BRACE ] );
}

static void checkClassStatus( CLASS_DATA *data )
{
    CLASSINFO *info;

    if( data->a_private && ! data->a_public && ! data->a_protected ) {
        if( data->bases == NULL && ScopeFriends( data->scope ) == NULL ) {
            CErr1( WARN_ALL_PRIVATE_IN_CLASS );
        }
    }
    info = data->info;
    if( ! info->has_ctor ) {
        if( ! IsCgTypeAggregate( data->type, FALSE ) ) {
            if( data->a_const ) {
                CErr1( ERR_CONST_MEMBER_MEANS_CTOR );
            }
            if( data->a_reference ) {
                CErr1( ERR_REFERENCE_MEMBER_MEANS_CTOR );
            }
        }
    } else {
        /* see 9.2 Class members [class.mem]: "14 In addition, if
           class T has a user-declared constructor (12.1), every
           non-static data member of class T shall have a name
           different from T." */
        SYMBOL sym;
        SYMBOL_NAME sym_name;

        sym_name = HashLookup( data->scope->names, data->name );

        if( sym_name != NULL ) {
            RingIterBeg( sym_name->name_syms, sym ) {
                if( sym->id == SC_MEMBER ) {
                    CErr2p( ERR_MEMBER_SAME_NAME_AS_CLASS, sym );
                }
            } RingIterEnd( sym )
        }
    }
}

static target_offset_t nonVirtualField( TYPE type )
{
    CLASSINFO *info;

    info = type->u.c.info;
    return( addField( classDataStack, info->vsize, info->max_align ) );
}

static SYMBOL insertDefaultFunc( SCOPE scope, TYPE fn_type, NAME name )
{
    // SF_INITIALIZED prevents the user from defining the class
    // themselves (see FNBODY check for GeneratedDefaultFunction check)
    return SymCreateAtLocn( MakeCommonCodeData( fn_type )
                          , SC_MEMBER
                          , SF_INITIALIZED
                          , name
                          , scope
                          , NULL );
}

static TYPE addClassFnMods( SCOPE scope, TYPE fn_type )
{
    CLASS_DATA *data;

    if( fn_type->u.f.pragma == NULL ) {
        data = classDataStack;
        if( data != NULL && data->member_mod_adjust && data->scope == scope ) {
            ClassAddFunctionMods( fn_type );
        }
    }
    fn_type = CheckDupType( fn_type );
    return( fn_type );
}

SYMBOL ClassAddDefaultCtor( SCOPE scope )
/***************************************/
{
    TYPE class_type;
    TYPE fn_type;
    TYPE ret_type;
    CLASSINFO *info;
    SYMBOL sym;

    class_type = ScopeClass( scope );
    ret_type = MakeReferenceTo( class_type );
    fn_type = MakeModifiableFunction( ret_type, NULL );
    if( _IS_DEFAULT_INLINE ) {
        fn_type->flag |= TF1_INLINE;
    }
    fn_type = addClassFnMods( scope, fn_type );
    sym = insertDefaultFunc( scope, fn_type, CppConstructorName() );
    info = class_type->u.c.info;
    info->ctor_defined = TRUE;
    info->ctor_user_code = FALSE;
    return( sym );
}

SYMBOL ClassAddDefaultDtor( SCOPE scope )
/***************************************/
{
    TYPE fn_type;
    TYPE ret_type;
    TYPE class_type;
    CLASSINFO *info;
    SYMBOL sym;

    ret_type = TypePtrToVoid();
    fn_type = MakeModifiableFunction( ret_type, NULL );
    if( _IS_DEFAULT_INLINE ) {
        fn_type->flag |= TF1_INLINE;
    }
    fn_type = addClassFnMods( scope, fn_type );
    sym = insertDefaultFunc( scope, fn_type, CppDestructorName() );
    class_type = ScopeClass( scope );
    info = class_type->u.c.info;
    info->dtor_defined = TRUE;
    info->dtor_user_code = FALSE;
    return( sym );
}

static SYMBOL checkPresence( SCOPE scope, NAME name )
{
    SEARCH_RESULT *result;
    SYMBOL syms;

    syms = NULL;
    result = ScopeContainsMember( scope, name );
    if( result != NULL ) {
        syms = result->sym_name->name_syms;
        ScopeFreeResult( result );
    }
    return( syms );
}

static SYMBOL findMember( SYMBOL syms, TYPE class_type,
                          bool (*check)( SYMBOL, TYPE ) )
{
    SYMBOL sym;

    RingIterBeg( syms, sym ) {
        if( check( sym, class_type ) ) {
            return( sym );
        }
    } RingIterEnd( sym )
    return( NULL );
}

static TYPE buildCopyAssign( SCOPE scope, type_flag fn_flags, TYPE arg_type, TYPE class_type )
{
    TYPE ret_type;
    TYPE fn_type;

    arg_type = MakeReferenceTo( arg_type );
    ret_type = MakeReferenceTo( class_type );
    fn_type = MakeModifiableFunction( ret_type, arg_type, NULL );
    fn_type->flag |= fn_flags;
    fn_type = addClassFnMods( scope, fn_type );
    return( fn_type );
}

SYMBOL ClassAddDefaultCopy( SCOPE scope )
/***************************************/
{
    type_flag fn_flags;
    TYPE class_type;
    TYPE arg_type;
    TYPE fn_type;
    CLASSINFO *info;
    SYMBOL syms;
    SYMBOL sym;
    NAME name;

    class_type = ScopeClass( scope );
    name = CppConstructorName();
    syms = checkPresence( scope, name );
    if( syms != NULL ) {
        if( findMember( syms, class_type, ClassIsDefaultCopy ) != NULL ) {
            return( NULL );
        }
    }
    info = class_type->u.c.info;
    arg_type = class_type;
    if( info->const_copy ) {
        arg_type = MakeConstTypeOf( arg_type );
    }
    fn_flags = TF1_NULL;
    if( _IS_DEFAULT_INLINE ) {
        fn_flags |= TF1_INLINE;
    }
    fn_type = buildCopyAssign( scope, fn_flags, arg_type, class_type );
    sym = insertDefaultFunc( scope, fn_type, name );
    info->copy_defined = TRUE;
    info->copy_user_code = FALSE;
    return( sym );
}

SYMBOL ClassAddDefaultAssign( SCOPE scope )
/*****************************************/
{
    type_flag fn_flags;
    TYPE class_type;
    TYPE arg_type;
    TYPE fn_type;
    CLASSINFO *info;
    SYMBOL syms;
    SYMBOL sym;
    NAME name;

    class_type = ScopeClass( scope );
    name = CppOperatorName( CO_EQUAL );
    syms = checkPresence( scope, name );
    info = class_type->u.c.info;
    if( syms != NULL ) {
        if( findMember( syms, class_type, ClassIsDefaultAssign ) != NULL ) {
            info->has_def_opeq = TRUE;
            return( NULL );
        }
    }
    arg_type = class_type;
    if( info->const_assign ) {
        arg_type = MakeConstTypeOf( arg_type );
    }
    fn_flags = TF1_NULL;
    if( _IS_DEFAULT_INLINE ) {
        fn_flags |= TF1_INLINE;
    }
    fn_type = buildCopyAssign( scope, fn_flags, arg_type, class_type );
    sym = insertDefaultFunc( scope, fn_type, name );
    info->assign_defined = TRUE;
    info->assign_user_code = FALSE;
    return( sym );
}

static bool addDefaultCtor( CLASS_DATA *data, SCOPE scope, TYPE class_type )
{
    CLASSINFO *info;

    info = class_type->u.c.info;
    if( info->needs_ctor || data->member_mod_adjust ) {
        if( !info->has_ctor ) {
            ClassAddDefaultCtor( scope );
            return( TRUE );
        }
    } else {
        info->ctor_user_code = FALSE;
    }
    return( FALSE );
}

static bool addDefaultCopy( CLASS_DATA *data, SCOPE scope, TYPE class_type )
{
    CLASSINFO *info;

    info = class_type->u.c.info;
    if( info->needs_ctor || data->member_mod_adjust ) {
        if( ClassAddDefaultCopy( scope ) != NULL ) {
            return( TRUE );
        }
    } else {
        info->copy_user_code = FALSE;
    }
    return( FALSE );
}

static bool addDefaultDtor( CLASS_DATA *data, SCOPE scope, TYPE class_type )
{
    CLASSINFO *info;

    info = class_type->u.c.info;
    if( info->needs_dtor || data->member_mod_adjust ) {
        if( !info->has_dtor ) {
            ClassAddDefaultDtor( scope );
            return( TRUE );
        }
    } else {
        info->dtor_user_code = FALSE;
    }
    return( FALSE );
}

static bool addDefaultOpEq( CLASS_DATA *data, SCOPE scope, TYPE class_type )
{
    CLASSINFO *info;

    info = class_type->u.c.info;
    if( info->needs_assign || data->member_mod_adjust || data->has_explicit_opeq ) {
        /* make sure the class doesn't contain a const member or ref member */
        if( ! info->const_ref ) {
            if( ClassAddDefaultAssign( scope ) != NULL ) {
                return( TRUE );
            }
        }
    } else {
        info->assign_user_code = FALSE;
    }
    return( FALSE );
}

static void setAbstractStatus( CLASS_DATA *data, CLASSINFO *info )
{
    BASE_CLASS *base;
    TYPE base_type;
    CLASSINFO *base_info;

    info->abstract_OK = TRUE;
    RingIterBeg( data->bases, base ) {
        base_type = StructType( base->type );
        base_info = base_type->u.c.info;
        if( base_info->abstract_OK ) {
            if( base_info->abstract ) {
                /* at least one known abstract base */
                info->abstract_OK = FALSE;
                break;
            }
        } else {
            /* at least one possible abstract base */
            info->abstract_OK = FALSE;
            break;
        }
    } RingIterEnd( base )
}

// note: info->passed_ref is already set when:
//       - set in any base class
//       - has an explicit copy constructor
// It is set here when either:
//       - class requires destruction
//       - class has special fields (vfptr,vbptr)
//
static void setRefPassing(      // SET passed_ref IN CLASSINFO
    TYPE cltype,                // - class type
    CLASSINFO* info )           // - and its info data
{
    if( ! info->passed_ref ) {
        if( info->needs_dtor
         || TypeHasSpecialFields( cltype ) ) {
            info->passed_ref = TRUE;
        }
    }
}


static void warnAboutHiding( CLASS_DATA *data )
{
    vindex vf_index;
    unsigned i;
    unsigned m;
    size_t amt;
    indices_t *indices_used;
    SYMBOL base_sym;
    SYMBOL derived_sym;
    SYMBOL derived_head;
    VF_HIDE *curr;

    if( ! MsgWillPrint( WARN_HIDDEN_VIRTUAL ) ) {
        RingCarveFree( carveVF_HIDE, &(data->vf_hide_list) );
        return;
    }
    vf_index = data->vf_index;
    amt = (( vf_index / ITEM_BITSIZE ) + 1 ) * sizeof( indices_t );
    indices_used = alloca( amt );
    for( ; (curr = RingPop( &data->vf_hide_list )) != NULL; ) {
        memset( indices_used, 0, amt );
        derived_head = curr->derived->name_syms;
        RingIterBeg( derived_head, derived_sym ) {
            if( SymIsVirtual( derived_sym ) && ! SymIsDefArg( derived_sym ) ) {
                vf_index = derived_sym->u.member_vf_index;
                if( vf_index != 0 ) {
                    i = vf_index / ITEM_BITSIZE;
                    m = 1 << ( vf_index % ITEM_BITSIZE );
                    indices_used[i] |= m;
                }
            }
        } RingIterEnd( derived_sym );
        RingIterBeg( curr->base->name_syms, base_sym ) {
            if( SymIsVirtual( base_sym ) && ! SymIsDefArg( base_sym ) ) {
                vf_index = base_sym->u.member_vf_index;
                if( vf_index != 0 ) {
                    i = vf_index / ITEM_BITSIZE;
                    m = 1 << ( vf_index % ITEM_BITSIZE );
                    if( (indices_used[i] & m) == 0 ) {
                        if( CErr1( WARN_HIDDEN_VIRTUAL ) & MS_PRINTED ) {
                            InfMsgPtr( INF_BASE_VFN, base_sym );
                            InfMsgPtr( INF_HIDDEN_WHY, data->type );
                        }
                    }
                }
            }
        } RingIterEnd( base_sym );
        CarveFree( carveVF_HIDE, curr );
    }
}

static void createVFPtrField( CLASS_DATA *data, bool do_creation )
{
    TYPE vfptr_type;
    CLASSINFO *info;

    if( do_creation ) {
        vfptr_type = MakeVFTableFieldType( TRUE );
        data->vf_offset = addTypeField( data, vfptr_type );
#ifndef NDEBUG
    } else if( data->own_vfptr ) {
        CFatal( "vfptr created twice" );
#endif
    }
    info = data->info;
    info->has_vfptr = TRUE;
    data->own_vfptr = TRUE;
}

DECL_SPEC *ClassEnd( void )
/*************************/
{
    target_offset_t size_of_vbases;
    target_offset_t vb_offset;
    target_offset_t vf_offset;
    TYPE type;
    SCOPE scope;
    CLASS_DATA *data;
    CLASSINFO *info;
    BASE_CLASS *base;
    BASE_CLASS *vbptr_base;
    BASE_CLASS *vfptr_base;
    bool gen_defaults;

    data = classDataStack;
    data->is_explicit = FALSE;
    ClassPermission( SF_NULL );
    type = data->type;
    scope = data->scope;
    info = data->info;
    if( data->a_const || data->a_reference ) {
        /* must be set before we add compiler generated default member fns */
        /* because we can't generate a default op= unless this is set */
        info->const_ref = TRUE;
    }
    /*  has_const_copy  has_nonconst_copy       const_copy
        has_const_opeq  has_nonconst_opeq       const_assign
            FALSE           FALSE                 inherit
            TRUE            FALSE                 set TRUE
            FALSE           TRUE                  set FALSE
            TRUE            TRUE                  set TRUE
    */
    if( data->has_const_copy ) {
        info->const_copy = TRUE;
        info->passed_ref = TRUE;
    } else {
        if( data->has_nonconst_copy ) {
            info->const_copy = FALSE;
            info->passed_ref = TRUE;
        }
    }
    if( data->has_const_opeq ) {
        info->const_assign = TRUE;
    } else {
        if( data->has_nonconst_opeq ) {
            info->const_assign = FALSE;
        }
    }
    if( info->unnamed ) {
        if( info->needs_ctor || info->needs_dtor ) {
            CErr1( WARN_UNNAMED_CLASS_HAS_SPECIAL_MEMBER );
        }
    }
    gen_defaults = FALSE;
    gen_defaults |= addDefaultCtor( data, scope, type );
    gen_defaults |= addDefaultCopy( data, scope, type );
    gen_defaults |= addDefaultDtor( data, scope, type );
    gen_defaults |= addDefaultOpEq( data, scope, type );
    if( data->own_vfptr ) {
        createVFPtrField( data, TRUE );
    }
    data->defined = TRUE;
    info = data->info;
    vb_offset = data->vb_offset;
    vbptr_base = data->base_vbptr;
    if( vbptr_base != NULL ) {
        vb_offset += vbptr_base->delta;
    }
    info->vb_offset = vb_offset;
    vf_offset = data->vf_offset;
    vfptr_base = data->base_vfptr;
    if( vfptr_base != NULL ) {
        vf_offset += vfptr_base->delta;
    }
    info->vf_offset = vf_offset;
    info->last_vfn = data->vf_index;
    info->last_vbase = data->vb_index;
    if( info->has_ctor || info->has_dtor ) {
        if( info->has_vfn ) {
            info->has_vcdtor = TRUE;
        }
    }
    if( info->has_vfptr || info->has_vbptr ) {
        info->has_comp_info = TRUE;
    }
    if( ! info->abstract_OK ) {
        setAbstractStatus( data, info );
    }
    doAlignment( data, data->max_align );
    size_of_vbases = data->offset;
    /* add virtual bases to end of class */
    RingIterBeg( data->bases, base ) {
        if( _IsVirtualBase( base ) ) {
            if( TypeCtorDispRequired( type, base->type ) ) {
                /* add ctor-disp before each virtual base if needed */
                base->flag |= IN_CTOR_DISP;
                addField( data, TARGET_UINT, TARGET_CHAR );
            }
            base->delta = nonVirtualField( base->type );
        }
    } RingIterEnd( base )
    size_of_vbases = data->offset - size_of_vbases;
    if( data->offset == 0 ) {
        /* an empty class must have a non-zero size */
        addField( data, TARGET_CHAR, TARGET_CHAR );
        info->empty = TRUE;
    }
    doAlignment( data, data->max_align );
    info->size = data->offset;
    info->vsize = info->size - size_of_vbases;
    info->max_align = data->max_align;
    info->defined = TRUE;
    data->in_defn = FALSE;
    data->member_mod_adjust = FALSE;
    info->class_mod = data->class_mod_type;
    checkClassStatus( data );
    warnAboutHiding( data );
    ScopeEnd( SCOPE_CLASS );
    SetCurrScope( data->enclosing );
    if( data->specific_defn ) {
        TemplateSpecificDefnEnd();
    }
    type = CheckDupType( data->type );
    if( CErrOccurred( &(data->errors) ) ) {
        info->corrupted = TRUE;
    } else {
        setRefPassing( type, info );
    }
    defineInlineFuncsAndDefArgExprs( data );
    if( ( data->inline_data != NULL ) && ( data->next != NULL ) ) {
        data->next->nested_inlines = data->inline_data->inlines;
        if( data->saved_inlines != NULL ) {
            data->inline_data->inlines = data->saved_inlines;
        }
    }
    if( ! info->corrupted ) {
        BrinfDeclClass( type );
    }
    return( ClassRefDef() );
}

static unsigned nextBitfield( CLASS_DATA *data, TYPE typ, unsigned width )
{
    unsigned bit_offset;
    unsigned unit_width;

    if( data->bitfield && data->is_union ) {
        data->bit_offset = 0;
    }
    if( ! data->bitfield || width > data->bit_available ) {
        data->boffset = addTypeField( data, typ );
        data->bit_offset = 0;
        unit_width = CgMemorySize( typ ) * TARGET_BITS_CHAR;
        data->bit_available = unit_width;
        data->bit_unit_width = unit_width;
        data->bitfield = TRUE;
    }
    bit_offset = data->bit_offset;
    data->bit_offset += width;
    data->bit_available -= width;
    return( bit_offset );
}

static void abandonBitfield( TYPE typ )
{
    CLASS_DATA *data;

    data = classDataStack;
    if( data->bitfield || typ == NULL ) {
        data->bitfield = FALSE;
        return;
    }
    doAlignment( data, CgMemorySize( typ ) );
}

static void propagateKnowledge( CLASSINFO *from, CLASSINFO *to, prop_type what )
{
    if(( what & PROP_STATIC ) == 0 ) {
        /* non-static data member or a base class */
        if( from->corrupted ) {
            to->corrupted = TRUE;
        }
        if( from->needs_ctor ) {
            to->needs_ctor = TRUE;
        }
        if( from->needs_dtor ) {
            to->needs_dtor = TRUE;
        }
        if( from->needs_assign ) {
            to->needs_assign = TRUE;
        }
        #if 0
        if( from->const_ref ) {
            to->const_ref = TRUE;
        }
        #endif
        if( !from->const_copy ) {
            to->const_copy = FALSE;
        }
        if( !from->const_assign ) {
            to->const_assign = FALSE;
        }
        if( from->passed_ref ) {
            to->passed_ref = TRUE;
        }
        if( from->has_comp_info ) {
            to->has_comp_info = TRUE;
        }
        if( from->has_mutable ) {
            to->has_mutable = TRUE;
        }
    }
    if( what & PROP_BASE ) {
        if( from->has_data ) {
            to->has_data = TRUE;
        }
        if( from->has_vcdtor ) {
            to->has_vcdtor = TRUE;
        }
        if( from->has_vfn ) {
            to->has_vfn = TRUE;
        }
        if( from->has_udc ) {
            to->has_udc = TRUE;
        }
        if( from->needs_vdtor ) {
            to->needs_vdtor = TRUE;
        }
        if( from->lattice ) {
            to->lattice = TRUE;
        }
    }
}

bool ClassIsDefaultCtor( SYMBOL sym, TYPE class_type )
/****************************************************/
{
    /* name has already been checked */
    class_type = class_type;
    return TypeHasNumArgs( sym->sym_type, 0 );
}

static bool isDefaultCopy( SYMBOL sym, TYPE class_type, unsigned *arg_info )
{
    arg_list *args;
    TYPE arg_type;
    TYPE test_type;
    type_flag flags;

    *arg_info = CC_NULL;
    if( ! TypeHasNumArgs( sym->sym_type, 1 ) ) {
        return( FALSE );
    }
    args = SymFuncArgList( sym );
    arg_type = args->type_list[0];
    test_type = ClassTypeForType( arg_type );
    if( test_type == NULL ) {
        return( FALSE );
    }
    if( test_type == class_type ) {
        test_type = TypeReference( arg_type );
        if( test_type == NULL ) {
            test_type = arg_type;
        } else {
            *arg_info |= CC_REF;
        }
        TypeModFlags( test_type, &flags );
        if( flags & TF1_CONST ) {
            *arg_info |= CC_CONST;
        }
        return( TRUE );
    }
    return( FALSE );
}

static bool isDefaultAssign( SYMBOL sym, TYPE class_type, unsigned *arg_info )
{
    return( isDefaultCopy( sym, class_type, arg_info ) );
}

bool ClassIsDefaultCopy( SYMBOL sym, TYPE class_type )
/****************************************************/
{
    unsigned arg_info;

    return( isDefaultCopy( sym, class_type, &arg_info ) );
}

bool ClassIsDefaultAssign( SYMBOL sym, TYPE class_type )
/******************************************************/
{
    unsigned arg_info;

    return( isDefaultAssign( sym, class_type, &arg_info ) );
}

bool GeneratedDefaultFunction(  // IS SYMBOL A DEFAULT CTOR,COPY, OR DTOR?
    SYMBOL sym )                // - symbol to check
{
    NAME name;
    TYPE class_type;
    CLASSINFO *info;

    class_type = SymClass( sym );
    if( class_type == NULL ) {
        /* not a member function */
        return( FALSE );
    }
    info = class_type->u.c.info;
    name = sym->name->name;
    if( name == CppConstructorName() ) {
        if( info->ctor_defined ) {
            if( ClassIsDefaultCtor( sym, class_type ) ) {
                return( TRUE );
            }
        }
        if( info->copy_defined ) {
            if( ClassIsDefaultCopy( sym, class_type ) ) {
                return( TRUE );
            }
        }
    } else if( name == CppDestructorName() ) {
        if( info->dtor_defined ) {
            /* cannot overload a destructor */
            return( TRUE );
        }
    } else if( name == CppOperatorName( CO_EQUAL ) ) {
        if( info->assign_defined ) {
            if( ClassIsDefaultAssign( sym, class_type ) ) {
                info->has_def_opeq = TRUE;
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

static void checkAllBasesForVDtor( CLASSINFO *info )
{
    TYPE base_type;
    BASE_CLASS *curr;
    CLASSINFO *base_info;
    bool ok_to_diagnose;

    RingIterBeg( info->bases, curr ) {
        base_type = curr->type;
        base_info = base_type->u.c.info;
        if( base_info->needs_dtor && ! base_info->needs_vdtor ) {
            /* base class needs a dtor but it isn't virtual! */
            ok_to_diagnose = FALSE;
            if( ScopeHasVirtualFunctions( base_type->u.c.scope ) ) {
                /* base has virtual functions -> always warn */
                ok_to_diagnose = TRUE;
            } else if(( curr->flag & IN_ACCESS_SPECIFIED ) != IN_PRIVATE ) {
                /* base is not private -> always warn */
                ok_to_diagnose = TRUE;
            }
            if( ok_to_diagnose ) {
                CErr2p( WARN_BASE_CLASS_HAS_NO_VDTOR, base_type );
            }
        }
    } RingIterEnd( curr )
}

static bool hasNonRefArg( SYMBOL sym, TYPE class_type )
{
    arg_list *args;
    TYPE arg_type;
    TYPE test_type;
    unsigned arg_index;

    args = SymFuncArgList( sym );
    if( args->num_args == 0 ) {
        return( FALSE );
    }
    for( arg_index = 0; arg_index < args->num_args; ++arg_index ) {
        arg_type = args->type_list[ arg_index ];
        test_type = ClassTypeForType( arg_type );
        if( test_type != NULL ) {
            if( test_type == class_type ) {
                test_type = TypeReference( arg_type );
                if( test_type == NULL ) {
                    return( TRUE );
                }
            }
        }
    }
    return( FALSE );
}

static void verifyCtor( CLASS_DATA *data, SYMBOL sym )
{
    unsigned arg_info;
    TYPE class_type;
    SYMBOL base_sym;
    CLASSINFO *info;

#ifndef NDEBUG
    if( sym->name->name != CppConstructorName() ) {
        CFatal( "ctor check used on non-ctor symbol" );
    }
#endif
    class_type = data->type;
    info = data->info;
    if( isDefaultCopy( sym, class_type, &arg_info ) ) {
        if(( arg_info & CC_CONST ) == 0 ) {
            data->has_nonconst_copy = TRUE;
        } else {
            data->has_const_copy = TRUE;
        }
        if(( arg_info & CC_REF ) == 0 ) {
            CErr1( ERR_CTOR_BAD_ARG_LIST );
            info->corrupted = TRUE;
        } else {
            if( sym->id == SC_DEFAULT ) {
                base_sym = SymDefaultBase( sym );
                if( hasNonRefArg( base_sym, class_type ) ) {
                    CErr1( ERR_CTOR_BAD_ARG_LIST );
                    info->corrupted = TRUE;
                }
            }
        }
    } else {
        if( data->is_explicit ) {
            if( ClassIsDefaultCtor( sym, class_type ) ) {
                info->has_def_ctor = TRUE;
            }
        }
    }
}

static void addVFHide( CLASS_DATA *data, SYMBOL d, SYMBOL b )
{
    SYMBOL_NAME derived;
    SYMBOL_NAME base;
    VF_HIDE *curr;
    VF_HIDE *hide;

    base = b->name;
    derived = d->name;
    RingIterBeg( data->vf_hide_list, curr ) {
        if( curr->derived == derived ) {
            return;
        }
    } RingIterEnd( curr )
    hide = CarveAlloc( carveVF_HIDE );
    hide->derived = derived;
    hide->base = base;
    RingPush( &data->vf_hide_list, hide );
}

static bool checkForCallingConventionChange( SYMBOL sym, SYMBOL above )
{
    TYPE fn_type1 = FunctionDeclarationType( sym->sym_type );
    TYPE fn_type2 = FunctionDeclarationType( above->sym_type );
    AUX_INFO *prag1 = TypeHasPragma( fn_type1 );
    AUX_INFO *prag2 = TypeHasPragma( fn_type2 );

    if( prag1 == prag2 ) {
        return( FALSE );
    }
    if( PragmasTypeEquivalent( prag1, prag2 ) ) {
        return( FALSE );
    }
    CErr2p( ERR_OVERRIDE_CHANGES_CONVENTION, above );
    return( TRUE );
}

static void handleFunctionMember( CLASS_DATA *data, SYMBOL sym, NAME name )
{
    vindex vf_index;
    unsigned arg_info;
    bool is_virtual;
    bool is_pure;
    bool is_different;
    find_virtual_status fv_status;
    CLASSINFO *info;
    TYPE fn_type;
    TYPE type;
    SYMBOL search_sym[2];

    type = sym->sym_type;
    fn_type = FunctionDeclarationType( type );
    if(( fn_type->flag & TF1_PLUSPLUS ) == 0 ) {
        /* all member functions must be linkage "C++" */
        /* this may not be needed with change in verifySpecialFunction()@TYPE */
        type = MakePlusPlusFunction( type );
        sym->sym_type = type;
    }
    if( data->member_mod_adjust && data->mod_flags != TF1_NULL ) {
        type = MakeModifiedType( type, data->mod_flags );
        sym->sym_type = type;
    }
    info = data->info;
    info->has_fn = TRUE;
    if( sym->id == SC_DEFAULT ) {
        /* most checks can be performed on the base symbol */
        if( name == CppConstructorName() ) {
            verifyCtor( data, sym );
        }
        return;
    }
    is_pure = ( fn_type->flag & TF1_PURE ) != 0;
    is_virtual = ( fn_type->flag & TF1_VIRTUAL ) != 0;
    if( is_virtual ) {
        if( SymIsStatic( sym ) ) {
            CErr1( ERR_NO_STATIC_VIRTUAL );
            is_virtual = FALSE;
        }
        if( data->is_union ) {
            CErr1( ERR_UNION_NO_VIRTUAL_FUNCTIONS );
            is_virtual = FALSE;
        }
    }
    /* p. 306 ARM "can be virtual" column: exclude ctors, new, and delete */
    if( name == CppConstructorName() ) {
        if( data->is_explicit ) {
            info->has_ctor = TRUE;
            info->needs_ctor = TRUE;
        }
        verifyCtor( data, sym );
        return;
    }
    /* used to have "|| data->is_union" here; why? */
    /* deleted it 93/04/22 AFS because unions with dtor/assign didn't work */
    if( SymIsStatic( sym ) ) {
        /* operator new/delete are static member functions */
        if( is_pure ) {
            CErr1( ERR_PURE_VIRTUAL_FUNCTIONS_ONLY );
        }
        return;
    }
    vf_index = 0;
    fv_status = FVS_NULL;
    if( data->bases != NULL ) {
        search_sym[0] = sym;
        fv_status = ScopeFindVirtual( GetCurrScope(), search_sym, name );
        if( fv_status & FVS_NAME_SAME_TABLE ) {
            addVFHide( data, sym, search_sym[1] );
        }
        if( fv_status & FVS_VIRTUAL_ABOVE ) {
            SYMBOL above = search_sym[0];
            DbgAssert( above != NULL );
            is_different = checkForCallingConventionChange( sym, above );
            // add "&& ! is_different" when we change the object model
            if(( fv_status & FVS_USE_INDEX ) != 0 ) {
                vf_index = above->u.member_vf_index;
                if( fv_status & FVS_RETURN_THUNK ) {
                    /* if we need to apply a return thunk we need our own vftable */
                    if( ! data->own_vfptr ) {
                        /* all the previous indices (including this one) are OK */
                        createVFPtrField( data, FALSE );
                    }
                    if( SymIsEllipsisFunc( search_sym[0] ) ) {
                        CErr2p( ERR_CANT_GENERATE_RETURN_THUNK, above );
                    }
                }
            }
        }
    }
    if( fv_status & FVS_VIRTUAL_ABOVE ) {
        if( ! is_virtual ) {
            if( is_pure ) {
                type = MakePureVirtualFunction( type );
            } else {
                type = MakeVirtualFunction( type );
            }
            sym->sym_type = type;
            is_virtual = TRUE;
        }
    } else {
        if( is_pure ) {
            if( ! is_virtual ) {
                CErr1( ERR_PURE_VIRTUAL_FUNCTIONS_ONLY );
                is_virtual = TRUE;
            }
        }
    }
    if( is_pure ) {
        info->has_pure = TRUE;
        info->abstract = TRUE;
        info->abstract_OK = TRUE;
    }
    if( is_virtual ) {
        if( vf_index == 0 ) {
            if( data->vf_index == 0 ) {
                /* first virtual function! */
                createVFPtrField( data, FALSE );
            }
            vf_index = ++(data->vf_index);
        }
        sym->u.member_vf_index = vf_index;
        info->needs_ctor = TRUE;
        info->has_vfn = TRUE;
    }
    if( name == CppDestructorName() ) {
        if( data->is_explicit ) {
            info->has_dtor = TRUE;
            info->needs_dtor = TRUE;
        }
        if( is_virtual ) {
            info->needs_vdtor = TRUE;
            checkAllBasesForVDtor( info );
        }
        if( is_pure ) {
            CErr1( WARN_VDTOR_MUST_BE_DEFINED );
        }
    } else if( name == CppOperatorName( CO_EQUAL ) ) {
        if( data->is_explicit ) {
            data->has_explicit_opeq = TRUE;
        }
        if( isDefaultAssign( sym, data->type, &arg_info ) ) {
            if( data->is_explicit ) {
                info->needs_assign = TRUE;
            }
            if(( arg_info & CC_CONST ) == 0 ) {
                data->has_nonconst_opeq = TRUE;
            } else {
                data->has_const_opeq = TRUE;
            }
        }
    } else if( name == CppConversionName() ) {
        info->has_udc = TRUE;
    }
}

static void verifyClassMember( CLASS_DATA *data, TYPE typ, bool static_member )
{
    prop_type prop_control;
    CLASSINFO *info;

    info = typ->u.c.info;
    prop_control = PROP_NULL;
    if( static_member ) {
        prop_control |= PROP_STATIC;
    } else {
        if( info->zero_array ) {
            CErr2p( WARN_ZERO_ARRAY_CLASS_USED, typ );
        }
    }
    if( data->is_union ) {
        if( info->needs_ctor ) {
            CErr1( ERR_UNION_CANT_HAVE_MEMBER_WITH_CTOR );
        }
        if( info->needs_dtor ) {
            CErr1( ERR_UNION_CANT_HAVE_MEMBER_WITH_DTOR );
        }
        if( info->needs_assign ) {
            CErr1( ERR_UNION_CANT_HAVE_MEMBER_WITH_ASSIGN );
        }
    }
    propagateKnowledge( info, data->info, prop_control );
}

static CLASS_DATA *findClassData( SCOPE scope )
{
    CLASS_DATA *data;

    for( data = classDataStack; data != NULL; data = data->next ) {
        if( data->scope == scope ) {
            return( data );
        }
    }
    return( NULL );
}

static void setHasData( CLASS_DATA *data )
{
    CLASSINFO *info;

    if( data->zero_array_defd ) {
        CErr1( ERR_ZERO_ARRAY_MUST_BE_LAST );
    }
    info = data->info;
    info->has_data = TRUE;
}

static void checkConstMutable( CLASS_DATA *data, CLASSINFO *info, type_flag mod_flags )
{
    if( mod_flags & ( TF1_CONST | TF1_MUTABLE ) ) {
        if( mod_flags & TF1_CONST ) {
            if( mod_flags & TF1_MUTABLE ) {
                CErr1( ERR_MUTABLE_CANT_BE_CONST );
            }
            data->a_const = TRUE;
        }
        if( mod_flags & TF1_MUTABLE ) {
            info->has_mutable = TRUE;
        }
    }
}

void ClassMember( SCOPE scope, SYMBOL sym )
/*****************************************/
{
    CLASSINFO *info;
    SYMBOL base_sym;
    CLASS_DATA *data;
    TYPE no_mem_model_type;
    TYPE class_type;
    TYPE base_type;
    TYPE typ;
    TYPE sym_type;
    type_flag class_mod_flags;
    type_flag mod_flags;
    type_flag base_type_flags;
    type_flag explicit_flags;
    NAME scope_name;
    NAME name;
    struct {
        unsigned static_member : 1;
        unsigned zero_sized_array : 1;
    } flags;

    /* members can be inserted in enclosing class definitions */
    data = findClassData( scope );
    if( data == NULL ) {
        /* member is being injected after the class is defined */
        if( sym->id == SC_DEFAULT ) {
            base_sym = SymDefaultBase( sym );
            CErr2p( WARN_DEFAULT_ARG_ADDED_TO_MEMBER_FN, base_sym );
        }
        return;
    }
    sym->flag |= data->perm;
    switch( data->perm ) {
    case SF_PRIVATE:
        data->a_private = TRUE;
        break;
    case SF_PROTECTED:
        if( data->is_union ) {
            CErr1( WARN_UNION_PROTECTED_MEMBER );
        }
        data->a_protected = TRUE;
        break;
    default:
        data->a_public = TRUE;
    }
    name = sym->name->name;
    class_type = ScopeClass( scope );
    info = class_type->u.c.info;
    scope_name = info->name;
    flags.static_member = FALSE;
    flags.zero_sized_array = FALSE;
    switch( sym->id ) {
    case SC_TYPEDEF:
    case SC_CLASS_TEMPLATE:
    case SC_ENUM:
        return;
    case SC_ACCESS:
        return;
    case SC_STATIC:
    case SC_STATIC_FUNCTION_TEMPLATE:
        flags.static_member = TRUE;
        if( data->is_union ) {
            if( ! SymIsFunction( sym ) ) {
                CErr1( ERR_UNION_NO_STATIC_MEMBERS );
            }
        }
        break;
    case SC_AUTO:
    case SC_EXTERN:
    case SC_EXTERN_FUNCTION_TEMPLATE:
    case SC_REGISTER:
        CErr1( ERR_INVALID_STG_CLASS_FOR_MEMBER );
        sym->id = SC_NULL;
        break;
    }
    if( SymIsFunction( sym ) ) {
        handleFunctionMember( data, sym, name );
        return;
    }
    sym_type = sym->sym_type;
    typ = TypeGetActualFlags( sym_type, &mod_flags );
    if( flags.static_member ) {
        if( data->member_mod_adjust ) {
            class_mod_flags = data->mod_flags;
            if( class_mod_flags != TF1_NULL ) {
                sym_type = MakeModifiedType( sym_type, class_mod_flags );
                sym->sym_type = sym_type;
                mod_flags |= class_mod_flags;
            }
        }
    } else {
        checkConstMutable( data, info, mod_flags );
    }
    switch( typ->id ) {
    case TYP_BITFIELD:
        sym->u.member_offset = data->boffset;
        setHasData( data );
        break;
    default:
        switch( typ->id ) {
        case TYP_CLASS:
            verifyClassMember( data, typ, flags.static_member );
            break;
        case TYP_POINTER:
            if(( typ->flag & TF1_REFERENCE ) != 0 && ! flags.static_member ) {
                data->a_reference = TRUE;
            }
            break;
        case TYP_ARRAY:
            if( CompFlags.extensions_enabled ) {
                /* MS/Borland allow zero sized arrays at the end of a class */
                if( typ->u.a.array_size == 0 && ! flags.static_member ) {
                    if( data->bases != NULL ) {
                        CErr2p( ERR_CANNOT_HAVE_ZERO_ARRAY_AND_BASES, sym );
                    } else {
                        CErr2p( WARN_NON_PORTABLE_DECL_ZERO_SIZED_ARRAY, sym );
                    }
                    sym->sym_type = DupArrayTypeForArrayFlag( sym_type, 1,
                                                              TF1_ZERO_SIZE );
                    info->zero_array = TRUE;
                    flags.zero_sized_array = TRUE;
                }
            }
            base_type = ArrayModifiedBaseType( typ );
            base_type = TypeGetActualFlags( base_type, &base_type_flags );
            if( base_type->id == TYP_CLASS ) {
                verifyClassMember( data, base_type, flags.static_member );
            }
            if( ! flags.static_member ) {
                checkConstMutable( data, info, mod_flags | base_type_flags );
            }
            break;
        }
        if( data->bases == NULL ) {
            /* classes with no bases may be initialized field by field */
            /* so all fields must be in increasing order of offsets */
            abandonBitfield( NULL );
        }
        if( ! flags.static_member ) {
            if( mod_flags & TF1_MEM_MODEL ) {
                no_mem_model_type = typ;
                explicit_flags = ExplicitModifierFlags( sym->sym_type );
                if( explicit_flags & TF1_MEM_MODEL ) {
                    CErr1( ERR_MEM_MODEL_CLASS );
                } else {
                    mod_flags &= ~TF1_MEM_MODEL;
                    if( mod_flags != TF1_NULL ) {
                        no_mem_model_type = MakeModifiedType( typ, mod_flags );
                    }
                }
                sym->sym_type = no_mem_model_type;
            }
            setHasData( data );
            if( flags.zero_sized_array ) {
                data->zero_array_defd = TRUE;
                sym->u.member_offset = addZeroSizedField( data, typ );
            } else {
                sym->u.member_offset = addTypeField( data, typ );
            }
        } else {
            if( data->local_class ) {
                CErr1( ERR_LOCAL_CLASS_NO_STATIC_MEMBERS );
            }
        }
    }
}

void ClassBitfield( DECL_SPEC *dspec, PTREE name_tree, PTREE width_tree )
/***********************************************************************/
{
    TYPE base_type;
    TYPE bitfield_type;
    SYMBOL sym;
    NAME name;
    target_size_t base_width;
    target_long safe_width;
    unsigned width;
    unsigned start;
    CLASS_DATA *data;

    safe_width = width_tree->u.int_constant;
    PTreeFree( width_tree );
    base_type = CheckBitfieldType( dspec, safe_width );
    base_width = CgMemorySize( base_type ) * TARGET_BITS_CHAR;
    if( safe_width < 0 ) {
        CErr1( ERR_WIDTH_NEGATIVE );
        width = 1;
    } else if( safe_width > base_width ) {
        CErr1( ERR_FIELD_TOO_WIDE );
        width = base_width;
    } else {
        width = safe_width;
    }
    data = classDataStack;
    if( data->bitfield && base_width != data->bit_unit_width ) {
        /* base type has changed size; abandon current bit-field */
        abandonBitfield( NULL );
    }
    if( name_tree == NULL ) {
        /* unnamed bitfield */
        if( data->is_union ) {
            CErr1( ERR_UNION_UNNAMED_BITFIELD );
        }
        if( width == 0 ) {
            abandonBitfield( base_type );
        } else {
            nextBitfield( data, base_type, width );
        }
        return;
    } else {
        name_tree = ProcessBitfieldId( name_tree );
    }
    name = name_tree->u.id.name;
    if( width == 0 ) {
        CErr1( ERR_WIDTH_0 );
        width = 1;
    }
    start = nextBitfield( data, base_type, width );
    bitfield_type = MakeBitfieldType( dspec, base_type, start, width );
    sym = AllocSymbol();
    sym->sym_type = bitfield_type;
    SymbolLocnDefine( &(name_tree->locn), sym );
    InsertSymbol( GetCurrScope(), sym, name );        /* calls ClassMember */
    PTreeFree( name_tree );
}

inherit_flag ClassBaseQualifiers( inherit_flag old, inherit_flag add )
/********************************************************************/
{
    if( old & add ) {
        CErr1( ERR_REPEATED_BASE_QUALIFIERS );
    }
    old |= add;
    if( old & IN_ACCESS_SPECIFIED ) {
        switch( old & IN_ACCESS_SPECIFIED ) {
        case IN_PROTECTED:
        case IN_PRIVATE:
        case IN_PUBLIC:
            break;
        default:
            CErr1( ERR_TOO_MANY_ACCESS_SPECIFIERS );
        }
    }
    return( old );
}

static BASE_CLASS *baseAlloc( TYPE typ, inherit_flag flags )
{
    BASE_CLASS *base;

    base = CarveAlloc( carveBASE_CLASS );
    base->next = NULL;
    base->type = typ;
    base->delta = 0;
    base->vb_index = 0;
    base->flag = flags;
    return( base );
}

static void baseFree( void *p )
{
    CarveFree( carveBASE_CLASS, p );
}

static BASE_CLASS *findBase( BASE_CLASS *ring, TYPE type )
{
    BASE_CLASS *curr;

    RingIterBeg( ring, curr ) {
        if( curr->type == type ) {
            return( curr );
        }
    } RingIterEnd( curr )
    return( NULL );
}

static BASE_CLASS *findVBase( BASE_CLASS *ring, TYPE type )
{
    BASE_CLASS *curr;

    RingIterBeg( ring, curr ) {
        if( _IsVirtualBase( curr ) ) {
            if( curr->type == type ) {
                return( curr );
            }
        }
    } RingIterEnd( curr )
    return( NULL );
}

BASE_CLASS *ClassBaseSpecifier( inherit_flag flags, DECL_SPEC *dspec )
/********************************************************************/
{
    bool error_detected;
    TYPE base_type;
    BASE_CLASS *base;
    CLASS_DATA *data;

    base_type = BindTemplateClass( dspec->partial, NULL, FALSE );
    PTypeRelease( dspec );
    error_detected = FALSE;
    base_type = StructType( base_type );
    if( classDataStack->is_union ) {
        CErr1( ERR_UNION_CANNOT_HAVE_BASE );
        error_detected = TRUE;
    }
    if( base_type != NULL ) {
        if( ! TypeDefined( base_type ) ) {
            if( TypePartiallyDefined( base_type ) ) {
                CErr2p( ERR_CANNOT_INHERIT_PARTIALLY_DEFINED, base_type );
            } else {
                CErr2p( ERR_CANNOT_INHERIT_UNDEFINED, base_type );
            }
            error_detected = TRUE;
        }
        if( base_type->u.c.info->zero_array ) {
            CErr1( ERR_CANNOT_INHERIT_CLASS_WITH_ZERO_ARRAY );
            error_detected = TRUE;
        }
        if( base_type->flag & TF1_UNION ) {
            CErr1( ERR_UNION_CANNOT_BE_BASE );
            error_detected = TRUE;
        }
        if(( flags & IN_ACCESS_SPECIFIED ) == 0 ) {
            /* no access specified for base class */
            data = classDataStack;
            if( data->perm & SF_PRIVATE ) {
                flags |= IN_PRIVATE;
                CErr1( WARN_PRIVATE_BASE_ASSUMED );
            } else {
                flags |= IN_PUBLIC;
            }
        }
    }
    if( error_detected ) {
        base_type = NULL;
    }
    flags |= IN_DIRECT;
    base = baseAlloc( base_type, flags );
    return( base );
}

static BASE_CLASS *addBase( BASE_CLASS **ring, BASE_CLASS *base, BASE_CLASS *insert )
{
    BASE_CLASS *curr;

    if( _IsVirtualBase( base ) ) {
        /* keep virtual bases at the front of the list */
        if( insert == NULL ) {
            RingIterBeg( *ring, curr ) {
                if( ! _IsVirtualBase( curr ) ) break;
                insert = curr;
            } RingIterEnd( curr )
        }
        RingInsert( ring, base, insert );
        insert = base;
    } else {
        RingAppend( ring, base );
    }
    return( insert );
}

static BASE_CLASS *grabVBases( BASE_CLASS **ring, BASE_CLASS *base )
{
    BASE_CLASS *curr;
    BASE_CLASS *check_base;
    BASE_CLASS *insert;
    CLASSINFO *info;
    CLASS_DATA *data;
    bool reuse_indices;

    insert = NULL;
    info = base->type->u.c.info;
    if( info->last_vbase == 0 ) {
        /* no virtual bases to promote up */
        return( insert );
    }
    reuse_indices = FALSE;
    data = classDataStack;
    if( _IsDirectNonVirtualBase( base ) ) {
        /* we are grabbing virtual bases from a non-virtual base */
        if( data->vb_index == 0 ) {
            /* make use of the vbptr */
            data->vb_index = info->last_vbase;
            data->vb_offset = info->vb_offset;
            data->base_vbptr = base;
            reuse_indices = TRUE;
        }
    }
    RingIterBeg( info->bases, curr ) {
        if( _IsVirtualBase( curr ) ) {
            check_base = findVBase( *ring, curr->type );
            if( check_base != NULL ) {
                check_base->flag |= IN_INDIRECT_VIRTUAL | IN_LATTICE;
            } else {
                check_base = baseAlloc( curr->type, IN_INDIRECT_VIRTUAL );
                insert = addBase( ring, check_base, insert );
            }
            if( reuse_indices && check_base->vb_index == 0 ) {
                check_base->vb_index = curr->vb_index;
            }
        } else {
            /* no more virtual bases */
            break;
        }
    } RingIterEnd( curr )
    return( insert );
}

BASE_CLASS *ClassBaseList( BASE_CLASS *list, BASE_CLASS *base )
/*************************************************************/
{
    TYPE base_type;
    BASE_CLASS *check_base;
    BASE_CLASS *insert;

    base_type = base->type;
    if( base_type == NULL ) {
        /* some error occurred with base class */
        baseFree( base );
        return( list );
    }
    check_base = findBase( list, base_type );
    if( check_base != NULL ) {
        if( _IsDirectBase( check_base ) ) {
            CErr1( ERR_REPEATED_BASE_CLASS );
            baseFree( base );
            return( list );
        }
    }
    if( _IsVirtualBase( base ) ) {
        if( check_base != NULL ) {
            /* direct virtual base matches an indirect virtual base */
            /* note: grabVBases is not necessary */
            check_base->flag |= base->flag | IN_LATTICE;
            baseFree( base );
            return( list );
        }
    }
    insert = grabVBases( &list, base );
    addBase( &list, base, insert );
    return( list );
}

static void processVFPtrBase( CLASS_DATA *data, BASE_CLASS *base )
{
    TYPE base_type;
    CLASSINFO *base_info;
    vindex base_vfindex;
    vindex class_vfindex;

    if( _IsVirtualBase( base ) ) {
        return;
    }
    base_type = base->type;
    base_info = base_type->u.c.info;
#ifdef OPTIMIZE_EMPTY
    if( base_info->empty ) {
        data->last_empty = base;
    }
#endif
    base_vfindex = base_info->last_vfn;
    if( base_vfindex == 0 ) {
        return;
    }
    class_vfindex = data->vf_index;
    if( class_vfindex == 0 || class_vfindex < base_vfindex ) {
        /* find direct non-virtual base with the most entries in its vftable */
        data->vf_index = base_vfindex;
        data->base_vfptr = base;
    }
}

static void processBase( CLASS_DATA *data, BASE_CLASS *base )
{
    TYPE base_type;
    CLASSINFO *info;
    CLASSINFO *base_info;
    TYPE vbptr_type;
    TYPE base_class_mod;

    info = data->info;
    base_type = base->type;
    base_info = base_type->u.c.info;
    base_class_mod = base_info->class_mod;
    if( base_class_mod != NULL ) {
        data->base_class_with_mod = base_type;
    }
    if( _IsVirtualBase( base ) ) {
        if( base->flag & IN_LATTICE ) {
            info->lattice = TRUE;
        }
        info->needs_ctor = TRUE;
        if( base->vb_index == 0 ) {
            if( data->vb_index == 0 ) {
                vbptr_type = MakeVBTableFieldType( TRUE );
                data->vb_offset = addTypeField( data, vbptr_type );
                info->has_vbptr = TRUE;
            }
            base->vb_index = ++(data->vb_index);
        }
    } else {
        if( base != data->base_vfptr ) {
#ifdef OPTIMIZE_EMPTY
            if( base != data->last_empty ) {
                base->delta = nonVirtualField( base->type );
            }
#else
            base->delta = nonVirtualField( base->type );
#endif
        } else {
            data->vf_offset = base_info->vf_offset;
            base->flag |= IN_USING_VF_INDICES;
            info->has_vfptr = TRUE;
        }
    }
    propagateKnowledge( base_info, info, PROP_BASE );
}

static void inheritBaseMods( CLASS_DATA *data, BASE_CLASS *bases )
{
    TYPE base_type;
    TYPE base_class_with_mod;
    TYPE base_class_mod;
    TYPE test_base_class_mod;
    BASE_CLASS *curr;

    base_class_with_mod = data->base_class_with_mod;
    base_class_mod = base_class_with_mod->u.c.info->class_mod;
    RingIterBeg( bases, curr ) {
        base_type = curr->type;
        test_base_class_mod = base_type->u.c.info->class_mod;
        if( test_base_class_mod == NULL ) {
            CErr( WARN_CONFLICTING_CLASS_MODS, base_type, base_class_with_mod );
            base_class_mod = NULL;
            break;
        }
        if( ! IdenticalClassModifiers( base_class_mod, test_base_class_mod ) ) {
            CErr( WARN_CONFLICTING_CLASS_MODS, base_type, base_class_with_mod );
            base_class_mod = NULL;
            break;
        }
    } RingIterEnd( curr )
    if( base_class_mod != NULL ) {
        base_class_mod = AbsorbBaseClassModifiers( base_class_mod, &(data->mod_flags), &(data->fn_flags), &(data->fn_pragma) );
        data->member_mod_adjust = TRUE;
        data->class_mod_type = base_class_mod;
    }
}

void ClassBaseClause( BASE_CLASS *bases )
/***************************************/
{
    CLASS_DATA *data;
    BASE_CLASS *curr;
    BASE_CLASS *base_vfptr;
    TYPE base_class_with_mod;
    CLASSINFO *info;

    data = classDataStack;
    info = data->info;
    if( info->unnamed ) {
        CErr1( WARN_UNNAMED_CLASS_INHERITS );
    }
    info->bases = bases;
    data->bases = bases;
    data->base_class_with_mod = NULL;
#ifdef OPTIMIZE_EMPTY
    data->last_empty = NULL;
#endif
    RingIterBeg( bases, curr ) {
        processVFPtrBase( data, curr );
    } RingIterEnd( curr )
    base_vfptr = data->base_vfptr;
    if( base_vfptr != NULL ) {
        base_vfptr->delta = nonVirtualField( base_vfptr->type );
    }
    RingIterBeg( bases, curr ) {
        processBase( data, curr );
    } RingIterEnd( curr )
#ifdef OPTIMIZE_EMPTY
    {
    BASE_CLASS *last_empty;
    last_empty = data->last_empty;
    if( last_empty != NULL ) {
        last_empty->delta = addField( data, 0, TARGET_CHAR );
    }
    }
#endif
    if( bases != NULL ) {
        base_class_with_mod = data->base_class_with_mod;
        if( base_class_with_mod != NULL ) {
            /* a base class has member mod adjustments */
            if( ! data->member_mod_adjust ) {
                /* derived class can inherit the base class mods */
                inheritBaseMods( data, bases );
            }
        }
    }
}

DECL_SPEC *ClassRefDef( void )
/****************************/
{
    DECL_SPEC *dspec;
    CLASS_DATA *data;
    SYMBOL sym;

    data = classDataStack;
    dspec = PTypeActualType( data->type );
    if( data->defined ) {
        dspec->type_defined = TRUE;
    } else {
        dspec->type_elaborated = TRUE;
    }
    if( data->nameless_OK ) {
        dspec->nameless_allowed = TRUE;
    }
    if( data->class_idiom ) {
        dspec->class_idiom = TRUE;
    }
    sym = data->sym;
    if( sym != NULL ) {
        if( data->generic ) {
            dspec->generic = TRUE;
        }
        dspec->typedef_defined = sym;
        dspec->type_declared = TRUE;
    }
    dspec->name = data->name;
    return( dspec );
}

bool ClassOKToRewrite( void )
/***************************/
{
    CLASS_DATA *data;

    data = classDataStack;
    if( data == NULL ) {
        return( FALSE );
    }
    data = data->inline_data;
    if( data == NULL ) {
        return( FALSE );
    }

#ifndef NDEBUG
    if( ! ScopeType( GetCurrScope(), SCOPE_CLASS ) ) {
        DbgAssert( ScopeType( GetCurrScope()->enclosing, SCOPE_CLASS ) );
    }
#endif

    return( TRUE );
    // to disable: return( FALSE );
}

void ClassStoreDefArg( DECL_INFO *dinfo )
/***************************************/
{
    CLASS_DATA *data;

    data = classDataStack;
    DbgAssert( data != NULL );
    data = data->inline_data;
    DbgAssert( data != NULL );
    // RingAppend guarantees FIFO processing which is important because we
    // want any error messages to be sequential and all nested class inlines
    // to be processed before their host class inlines are processed
    RingAppend( &(data->defargs), dinfo );
}

void ClassStoreInlineFunc( DECL_INFO *dinfo )
/*******************************************/
{
    CLASS_DATA *data;

    data = classDataStack;
#ifndef NDEBUG
    if( data == NULL ) {
        CFatal( "inline function is not in a class definition" );
    }
#else
    if( data == NULL ){
        CErr1( ERR_PARSER_DIED );
        CSuicide();
    }
#endif
    data = data->inline_data;
#ifndef NDEBUG
    if( data == NULL ) {
        CFatal( "inline function is not nested in a class definition" );
    }
#endif
    if( dinfo->scope == NULL ) {
        dinfo->scope = GetCurrScope();
    }
    // RingAppend guarantees FIFO processing which is important because we
    // want any error messages to be sequential and all nested class inlines
    // to be processed before their host class inlines are processed
    RingAppend( &(data->inlines), dinfo );
}

void ClassMakeUniqueName( TYPE class_type, NAME signature )
/**********************************************************/
{
    size_t len;
    uint_32 count;
    uint_32 hash;
    uint_32 mask;
    SYMBOL stop;
    SYMBOL curr;
    CLASSINFO *info;
    char buff[ 2 + sizeof( uint_32 ) * 2 + 1 ];

    info = class_type->u.c.info;
    if( info->name != NULL ) {
        return;
    }
    hash = CgMemorySize( class_type );
    if( info->anonymous ) {
        hash += ( info->index << 8 );
    }
    stop = ScopeOrderedStart( class_type->u.c.scope );
    count = 0;
    curr = NULL;
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        ++count;
        hash <<= 8;
        hash += count;
        hash *= TypeHash( curr->sym_type );
        hash += CgMemorySize( curr->sym_type );
        mask = hash & ~0x0fff;
        if( mask != 0 ) {
            mask ^= mask >> 8;
            hash ^= mask;
        }
    }
    if( signature == NULL ) {
        len = 0;
    } else {
        len = strlen( NameStr( signature ) );
    }
    hash *= len + 1;
    buff[0] = '_';
    buff[1] = '_';
    ultoa( hash, &buff[2], 31 );
    if( len != 0 ) {
        VBUF big_buff;

        VbufInit( &big_buff );
        VbufConcStr( &big_buff, buff );
        VbufConcStr( &big_buff, NameStr( signature ) );
        info->name = NameCreateLen( VbufString( &big_buff ), VbufLen( &big_buff ) );
        VbufFree( &big_buff );
    } else {
        info->name = NameCreateNoLen( buff );
    }
}

static void doPromotion( SYMBOL_NAME sym_name )
{
    SCOPE scope;
    SYMBOL sym;
    TYPE type;
    NAME name;

    sym = sym_name->name_syms;
    sym_name->name_syms = NULL;
#ifndef NDEBUG
    if( sym == NULL ) {
        CFatal( "missing check for anonymous unions" );
    }
#endif
    name = sym_name->name;
    sym = ScopePromoteSymbol( GetCurrScope(), sym, name );
    if( sym == NULL ) {
        CErr2p( ERR_UNION_COLLISION_MEMBER, name );
    } else {
        scope = SymScope( sym );
        type = ScopeClass( scope );
        if( type != NULL && type->u.c.info->name == name ) {
            CErr2p( ERR_ANONYMOUS_SAME_NAME_AS_CLASS, name );
        }
    }
}

static void promoteMembers( TYPE class_type, SYMBOL owner )
{
    bool problems;
    bool promote_to_class;
    SCOPE scope;
    SYMBOL stop;
    SYMBOL curr;
    SYMBOL next;
    TYPE occupy_space;
    SYMBOL place_holder;
    NAME name;

    promote_to_class = FALSE;
    if( ScopeType( GetCurrScope(), SCOPE_CLASS ) ) {
        promote_to_class = TRUE;
    }
    scope = class_type->u.c.scope;
    problems = FALSE;
    if( ScopeFriends( scope ) != NULL ) {
        CErr1( ERR_UNION_CANT_HAVE_FRIENDS );
        problems = TRUE;
    }
    stop = ScopeOrderedStart( scope );
    curr = NULL;
    for(;;) {
        curr = ScopeOrderedNext( stop, curr );
        if( curr == NULL ) break;
        if( curr->flag & SF_PRIVATE ) {
            CErr2p( ERR_UNION_PRIVATE_MEMBER, curr );
            problems = TRUE;
        }
        if( SymIsFunction( curr ) ) {
            CErr2p( ERR_UNION_FUNCTION_MEMBER, curr );
            problems = TRUE;
        } else if( SymIsTypedef( curr ) ) {
            if( ! SymIsEnumAnonymousTypedef( curr ) ) {
                CErr2p( ERR_UNION_TYPEDEF_MEMBER, curr );
            }
            problems = TRUE;
        } else if( SymIsEnumeration( curr ) ) {
            CErr2p( ERR_UNION_ENUMERATION_MEMBER, curr );
            problems = TRUE;
        }
        if( ! problems ) {
            curr->flag |= SF_ANONYMOUS;
            if( promote_to_class ) {
                curr->flag |= owner->flag & SF_ACCESS;
                /* looking ahead to anonymous structs */
                curr->u.member_offset += owner->u.member_offset;
            } else {
                curr->id = owner->id;
                curr->u.alias = owner;
            }
        }
    }
    if( problems ) {
        class_type->u.c.info->corrupted = TRUE;
        return;
    }
    /* weird start/next loop is necessary because 'curr' SYMBOL is modified */
    stop = ScopeOrderedStart( scope );
    curr = ScopeOrderedNext( stop, NULL );
    for(;;) {
        if( curr == NULL ) break;
        next = ScopeOrderedNext( stop, curr );
        doPromotion( curr->name );
        curr = next;
    }
    /* simple way to make sure scope is cleared out */
    ScopeBurn( scope );
    scope = ScopeCreate( SCOPE_CLASS );
    ScopeOpen( scope );
    occupy_space = MakeInternalType( CgMemorySize( class_type ) );
    place_holder = SymMakeDummy( occupy_space, &name );
    InsertSymbol( GetCurrScope(), place_holder, name );
    ScopeEnd( SCOPE_CLASS );
    setScopeOwner( class_type, scope );
}

bool ClassAnonymousUnion( DECL_SPEC *dspec )
/******************************************/
{
    SYMBOL sym;
    NAME name;
    TYPE class_type;
    CLASSINFO *info;
    stg_class_t stg_class;
    bool emit_init;

    class_type = StructType( dspec->partial );
    if( class_type == NULL ) {
        return( FALSE );
    }
    info = class_type->u.c.info;
    if( info->name != NULL ) {
        return( FALSE );
    }
    if(( class_type->flag & TF1_UNION ) == 0 ) {
        /* anonymous struct */
        if( ! ScopeType( GetCurrScope(), SCOPE_CLASS ) ) {
            CErr1( ERR_UNNAMED_CLASS_USELESS );
            return( FALSE );
        }
    }
    info->anonymous = TRUE;
    emit_init = FALSE;
    stg_class = dspec->stg_class;
    sym = SymMakeDummy( class_type, &name );
    SymbolLocnDefine( NULL, sym );
    if( ScopeType( GetCurrScope(), SCOPE_FILE ) ) {
        if( stg_class != STG_STATIC ) {
            CErr1( ERR_GLOBAL_ANONYMOUS_UNION_MUST_BE_STATIC );
        }
        sym->id = SC_STATIC;
        emit_init = TRUE;
    } else {
        /*
            It is not clear whether static anonymous unions should be
            allowed in functions even though there are well-defined
            semantics. We implement them anyway (AFS 21-jan-91)
            (static anonymous unions in classes are not implemented
             since there is no way to define it outside the class)
        */
        if( ScopeType( GetCurrScope(), SCOPE_BLOCK ) ) {
            if( stg_class & ~ (STG_STATIC|STG_AUTO) ) {
                CErr1( ERR_FUNCTION_ANONYMOUS_UNION );
            } else if( stg_class & STG_STATIC ) {
                sym->id = SC_STATIC;
                emit_init = TRUE;
            }
        } else if( dspec->stg_class != STG_NULL ) {
            CErr1( ERR_NONGLOBAL_ANONYMOUS_UNION_CANT_BE_ANYTHING );
        }
    }
    // marking symbol as ref'd doesn't matter except for the auto case;
    // here we have not marked aliases as ref'd so we have the problem
    // when inlining a function with an auto anonymous union multiple
    // times that an inline alias for the anon-union symbol is not
    // created (it checks to make sure the sym is ref'd or init'd) AFS 97/05/16
    sym->flag |= SF_REFERENCED;
    sym = InsertSymbol( GetCurrScope(), sym, name );
    if( emit_init ) {
        DgSymbol( sym );
    }
    promoteMembers( class_type, sym );
    return( TRUE );
}

static bool duplicateMemInit( PTREE curr, PTREE test )
{
    if( curr->op != test->op ) {
        return( FALSE );
    }
    if( curr->op == PT_ID ) {
        if( curr->u.id.name == test->u.id.name ) {
            CErr2p( ERR_DUPLICATE_MEMBER_INIT, test->u.id.name );
            return( TRUE );
        }
    } else {
        if( TypesIdentical( curr->type, test->type ) ) {
            CErr2p( ERR_DUPLICATE_BASE_INIT, test->type );
            return( TRUE );
        }
    }
    return( FALSE );
}

static bool verifyBaseClassInit( PTREE base, SCOPE scope )
{
    TYPE class_type;
    SCOPE class_scope;

    class_type = StructType( base->type );
    class_scope = ScopeNearestNonTemplate( scope );
    if( class_type != NULL ) {
        if( ScopeDirectBase( class_scope, class_type ) ) {
            return( FALSE );
        }
        if( ScopeIndirectVBase( class_scope, class_type ) ) {
            return( FALSE );
        }
    }
    CErr2p( ERR_NOT_DIRECT_BASE_INIT, base->type );
    return( TRUE );
}

static bool verifyMemberInit( PTREE id, SCOPE scope )
{
    NAME name;
    SEARCH_RESULT *result;
    SYMBOL sym;

    name = id->u.id.name;
    result = ScopeContainsMember( ScopeNearestNonTemplate( scope ), name );
    if( result == NULL ) {
        CErr2p( ERR_NOT_MEMBER_MEMBER_INIT, name );
        return( TRUE );
    }
    sym = result->sym_name->name_syms;
    ScopeFreeResult( result );
    if( sym == NULL ) {
        CErr2p( ERR_NOT_MEMBER_MEMBER_INIT, name );
        return( TRUE );
    }
    if( SymIsFunction( sym ) ) {
        CErr2p( ERR_FN_MEMBER_MEMBER_INIT, name );
        return( TRUE );
    }
    if( SymIsStaticMember( sym ) ) {
        CErr2p( ERR_STATIC_MEMBER_MEMBER_INIT, name );
        return( TRUE );
    }
    return( FALSE );
}

static bool symInitialized( PTREE mem_init, SYMBOL sym )
{
    PTREE curr;
    PTREE test;
    NAME name;

    name = sym->name->name;
    for( curr = mem_init; curr != NULL; curr = curr->u.subtree[0] ) {
        test = curr->u.subtree[1]->u.subtree[0];
        if( test->op == PT_ID ) {
            if( test->u.id.name == name ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

static bool verifyAllConstRefsInit( SCOPE scope, PTREE mem_init )
{
    SYMBOL stop;
    SYMBOL curr;
    TYPE type;
    type_flag flags;
    bool error_detected;

    error_detected = FALSE;
    stop = ScopeOrderedStart( scope );
    for( curr = NULL; (curr = ScopeOrderedNext( stop, curr )) != NULL; ) {
        if( ! SymIsData( curr ) || SymIsStaticMember( curr ) )
            continue;
        type = TypeModFlags( curr->sym_type, &flags );
        if( flags & TF1_CONST ) {
            if( ! symInitialized( mem_init, curr ) ) {
                if( ConstNeedsExplicitInitializer( curr->sym_type ) ) {
                    CErr2p( ERR_MEMBER_WILL_NOT_BE_INIT, curr );
                    error_detected = TRUE;
                }
            }
        } else if( type->id == TYP_POINTER ) {
            if( type->flag & TF1_REFERENCE ) {
                if( ! symInitialized( mem_init, curr ) ) {
                    CErr2p( ERR_MEMBER_WILL_NOT_BE_INIT, curr );
                    error_detected = TRUE;
                }
            }
        }
    }
    return( error_detected );
}

static PTREE verifyMemInit( PTREE mem_init )
{
    PTREE curr;
    PTREE test;
    PTREE member;
    PTREE test_member;
    SYMBOL sym;
    SCOPE scope;
    bool error_detected;

    sym = ScopeFunctionInProgress();
    scope = SymScope( sym );
    error_detected = FALSE;
    for( curr = mem_init; curr != NULL; curr = curr->u.subtree[0] ) {
        member = curr->u.subtree[1]->u.subtree[0];
        /* check for duplicates */
        for( test = mem_init; test != curr; test = test->u.subtree[0] ) {
            test_member = test->u.subtree[1]->u.subtree[0];
            if( duplicateMemInit( member, test_member ) ) {
                error_detected = TRUE;
                break;
            }
        }
        member->type = BindTemplateClass( member->type, &member->locn, TRUE );
        if( member->op == PT_ID ) {
            if( verifyMemberInit( member, scope ) ) {
                error_detected = TRUE;
            }
        } else {
            if( verifyBaseClassInit( member, scope ) ) {
                error_detected = TRUE;
            }
        }
    }
    if( verifyAllConstRefsInit( scope, mem_init ) ) {
        error_detected = TRUE;
    }
    if( error_detected ) {
        PTreeFreeSubtrees( mem_init );
        mem_init = NULL;
    }
    return( mem_init );
}

PTREE ClassMemInit( SYMBOL ctor, REWRITE *mem_initializer )
/*********************************************************/
{
    PTREE mem_init;
    REWRITE *save_token;
    REWRITE *last_rewrite;
    void (*last_source)( void );

    mem_init = NULL;
    if( mem_initializer != NULL ) {
        save_token = RewritePackageToken();
        last_source = SetTokenSource( RewriteToken );
        last_rewrite = RewriteRewind( mem_initializer );
        ParseFlush();
        mem_init = ParseMemInit();
        ParseFlush();
        RewriteClose( last_rewrite );
        ResetTokenSource( last_source );
        RewriteFree( mem_initializer );
        RewriteRestoreToken( save_token );
        mem_init = verifyMemInit( mem_init );
    } else {
        /* copy ctor doesn't need mem-inits for const or ref members */
        if( ! ClassIsDefaultCopy( ctor, SymClass( ctor ) ) ) {
            mem_init = verifyMemInit( mem_init );
        }
    }
    return( mem_init );
}

void ClassPush( CLASS_DATA *data )
/********************************/
{
    DbgStmt( memset( data, -1, sizeof( *data ) ) );
    data->next = classDataStack;
    classDataStack = data;
}

void ClassPop( CLASS_DATA *data )
/*******************************/
{
    if( data->in_defn ) {
        // we are popping before we are done
        data->info->corrupted = TRUE;
        RingCarveFree( carveVF_HIDE, &(data->vf_hide_list) );
    }
    classDataStack = data->next;
}

static bool insideClassDefinition( void )
{
    return( classDataStack != NULL );
}

void ClassCtorNullBody( SYMBOL ctor )
/***********************************/
{
    SCOPE sym_scope;
    CLASSINFO *info;

    info = SymClassInfo( ctor );
    if( info == NULL ) {
        return;
    }
    sym_scope = SymScope( ctor );
    if( ! ClassIsDefaultCtor( ctor, ScopeClass( sym_scope ) ) ) {
        /* NYI: optimize copy ctor and other ctors */
        return;
    }
    if( insideClassDefinition() ) {
        info->ctor_user_code = FALSE;
    } else {
        /* out-of-line definition */
        if(( GenSwitches & DBG_LOCALS ) == 0 ) {
            if( IsSrcFilePrimary( ctor->locn->tl.src_file ) ) {
                if( info->ctor_user_code_checked ) {
                    CErr1( WARN_OPTIMIZE_IF_EARLIER );
                } else if( ! SymIsInline( ctor ) ) {
                    CErr1( WARN_OPTIMIZE_IF_INLINE );
                }
            }
        }
    }
}

void ClassDtorNullBody( SYMBOL dtor )
/***********************************/
{
    CLASSINFO *info;

    info = SymClassInfo( dtor );
    if( info == NULL ) {
        return;
    }
    if( insideClassDefinition() ) {
        info->dtor_user_code = FALSE;
    } else {
        /* out-of-line definition */
        if(( GenSwitches & DBG_LOCALS ) == 0 ) {
            if( IsSrcFilePrimary( dtor->locn->tl.src_file ) ) {
                //
                // If dtor is:
                //      empty and pure and virtual
                // DO NOT complain at any warning level that defining it inside class definition
                //    "may improve code quality"
                // as there is no way to define at the same time
                // function body and "pure virtuality" 
                // 
                // class X {
                //      virtual ~X() = 0;           // OK
                //      virtual ~X() { } = 0;       // impossible
                //      virtual ~X() = 0 { };       // impossible
                // };
                //
                TYPE fn_type = FunctionDeclarationType( dtor->sym_type );
                if( ((fn_type->flag & TF1_PURE   )!=0) &&
                    ((fn_type->flag & TF1_VIRTUAL)!=0) ) {
                    return;
                }
                if( info->dtor_user_code_checked ) {
                    CErr1( WARN_OPTIMIZE_IF_EARLIER );
                } else if( ! SymIsInline( dtor ) ) {
                    CErr1( WARN_OPTIMIZE_IF_INLINE );
                }
            }
        }
    }
}

void ClassAssignNullBody( SYMBOL op_eq )
/**************************************/
{
    op_eq = op_eq;
}


bool ClassParmIsRef( TYPE class_type )
/************************************/
{
    CLASSINFO *info;

    info = class_type->u.c.info;
    return info->passed_ref;
}


bool ClassNeedsAssign( TYPE class_type, bool exact )
/**************************************************/
{
    CLASSINFO *info;

    info = class_type->u.c.info;
    if( info->needs_assign ) {
        return( TRUE );
    }
    /* if we don't know the exact type of the source _and_ destination,
       we have to be careful */
    if( !exact && TypeHasSpecialFields( class_type ) ) {
        return( TRUE );
    }
    return( FALSE );
}

static bool genDefaultCtor( TYPE class_type )
{
    NAME name;
    SCOPE scope;
    SYMBOL syms;
    SYMBOL sym;

    name = CppConstructorName();
    scope = class_type->u.c.scope;
    syms = checkPresence( scope, name );
    sym = findMember( syms, class_type, ClassIsDefaultCtor );
    if( sym->flag & SF_REFERENCED ) {
        GenerateDefaultCtor( sym );
        return( TRUE );
    }
    sym->flag &= ~ SF_INITIALIZED;
    return( FALSE );
}

static bool genDefaultCopy( TYPE class_type )
{
    NAME name;
    SCOPE scope;
    SYMBOL syms;
    SYMBOL sym;

    name = CppConstructorName();
    scope = class_type->u.c.scope;
    syms = checkPresence( scope, name );
    sym = findMember( syms, class_type, ClassIsDefaultCopy );
    if( sym->flag & SF_REFERENCED ) {
        GenerateDefaultCopy( sym );
        return( TRUE );
    }
    sym->flag &= ~ SF_INITIALIZED;
    return( FALSE );
}

static bool genDefaultDtor( TYPE class_type )
{
    NAME name;
    SCOPE scope;
    SYMBOL syms;
    SYMBOL sym;

    name = CppDestructorName();
    scope = class_type->u.c.scope;
    syms = checkPresence( scope, name );
    /* dtors cannot be overloaded */
    sym = syms;
    if( sym->flag & SF_REFERENCED ) {
        GenerateDefaultDtor( sym );
        return( TRUE );
    }
    sym->flag &= ~ SF_INITIALIZED;
    return( FALSE );
}

static bool genDefaultAssign( TYPE class_type )
{
    NAME name;
    SCOPE scope;
    SYMBOL syms;
    SYMBOL sym;

    name = CppOperatorName( CO_EQUAL );
    scope = class_type->u.c.scope;
    syms = checkPresence( scope, name );
    sym = findMember( syms, class_type, ClassIsDefaultAssign );
    if( sym->flag & SF_REFERENCED ) {
        GenerateDefaultAssign( sym );
        return( TRUE );
    }
    sym->flag &= ~ SF_INITIALIZED;
    return( FALSE );
}

bool ClassDefineRefdDefaults( void )
/**********************************/
{
    bool something_defined;
    TYPE head;
    TYPE curr;
    SCOPE save_scope;
    CLASSINFO *info;

    head = PTypeListOfTypes( TYP_CLASS );
    something_defined = FALSE;
    save_scope = GetCurrScope();
    RingIterBeg( head, curr ) {
        info = curr->u.c.info;
        if( ! info->corrupted ) {
            if( info->ctor_defined && ! info->ctor_gen ) {
                if( genDefaultCtor( curr ) ) {
                    info->ctor_gen = TRUE;
                    something_defined = TRUE;
                }
            }
            if( info->copy_defined && ! info->copy_gen ) {
                if( genDefaultCopy( curr ) ) {
                    info->copy_gen = TRUE;
                    something_defined = TRUE;
                }
            }
            if( info->dtor_defined && ! info->dtor_gen ) {
                if( genDefaultDtor( curr ) ) {
                    info->dtor_gen = TRUE;
                    something_defined = TRUE;
                }
            }
            if( info->assign_defined && ! info->assign_gen ) {
                if( genDefaultAssign( curr ) ) {
                    info->assign_gen = TRUE;
                    something_defined = TRUE;
                }
            }
        }
    } RingIterEnd( curr )
    ScopeAdjustUsing( GetCurrScope(), save_scope );
    SetCurrScope( save_scope );

    return( something_defined );
}


bool ClassCorrupted(            // TEST IF CLASS (FOR TYPE) IS CORRUPTED
    TYPE type )                 // - should be a class type
{
    type = StructType( type );
    if( type == NULL || type->u.c.info->corrupted ) {
        return( TRUE );
    }
    return( FALSE );
}

static void markFreeBaseClass( void *p )
{
    BASE_CLASS *b = p;

    b->flag |= IN_FREE;
}

static void saveBaseClass( void *e, carve_walk_base *d )
{
    BASE_CLASS *b = e;
    BASE_CLASS *next_save;
    TYPE type_save;

    if( b->flag & IN_FREE ) {
        return;
    }
    next_save = b->next;
    b->next = BaseClassGetIndex( next_save );
    type_save = b->type;
    b->type = TypeGetIndex( type_save );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *b );
    b->next = next_save;
    b->type = type_save;
}

pch_status PCHWriteBases( void )
{
    auto carve_walk_base data;

    PCHWriteVar( classIndex );
    CarveWalkAllFree( carveBASE_CLASS, markFreeBaseClass );
    CarveWalkAll( carveBASE_CLASS, saveBaseClass, &data );
    PCHWriteCVIndexTerm();
    return( PCHCB_OK );
}

pch_status PCHReadBases( void )
{
    BASE_CLASS *b;
    auto cvinit_t data;

    PCHReadVar( classIndex );
    CarveInitStart( carveBASE_CLASS, &data );
    for( ; (b = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *b );
        b->next = BaseClassMapIndex( b->next );
        b->type = TypeMapIndex( b->type );
    }
    return( PCHCB_OK );
}

BASE_CLASS *BaseClassGetIndex( BASE_CLASS *e )
{
    return( CarveGetIndex( carveBASE_CLASS, e ) );
}

BASE_CLASS *BaseClassMapIndex( BASE_CLASS *e )
{
    return( CarveMapIndex( carveBASE_CLASS, e ) );
}

pch_status PCHInitBases( bool writing )
{
    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carveBASE_CLASS ) );
    } else {
        carveBASE_CLASS = CarveRestart( carveBASE_CLASS );
        CarveMapOptimize( carveBASE_CLASS, PCHReadCVIndex() );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniBases( bool writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveBASE_CLASS );
    }
    return( PCHCB_OK );
}
