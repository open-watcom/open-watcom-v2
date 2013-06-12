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

#include "fnovload.h"
#include "memmgr.h"
#include "ring.h"
#include "cgfront.h"
#include "template.h"
#include "decl.h"
#include "brinfo.h"

typedef enum base_info {
    IS_REF      = 0x01,
    IS_CONST    = 0x02,
    IS_DLLIMPORT= 0x04,
    IS_NULL     = 0x00
} base_info;

typedef enum prev_sym_adjust {
    PSA_USE_CURR_MODS   = 0x01,
    PSA_NULL            = 0x00
} prev_sym_adjust;

typedef enum fn_stg_class_status {
    FSCS_SAME_FN        = 0x01,
    FSCS_DIFF_FN        = 0x02,
    FSCS_NULL           = 0x00
} fn_stg_class_status;

static TYPE baseTypeInfo( TYPE type, base_info *extra )
{
    type_flag flags;

    *extra = IS_NULL;
    for(;;) {
        type = TypeGetActualFlags( type, &flags );
        if( flags & TF1_CONST ) {
            *extra |= IS_CONST;
        }
        if( flags & TF1_DLLIMPORT ) {
            *extra |= IS_DLLIMPORT;
        }
        switch( type->id ) {
        case TYP_POINTER:
            if( type->flag & TF1_REFERENCE ) {
                *extra |= IS_REF;
            }
            break;
        case TYP_ARRAY:
            type = type->of;
            continue;
        }
        break;
    }
    return( type );
}

boolean ConstNeedsExplicitInitializer( TYPE type )
/************************************************/
{
    CLASSINFO *info;
    SYMBOL dummy;

    type = StructType( type );
    if( type == NULL ) {
        /* const non-class types need explicit initializers */
        return( TRUE );
    }
    info = type->u.c.info;
    if( info->needs_ctor ) {
        /* if class doesn't have a default ctor, it needs an explicit init */
        if( ClassDefaultCtorFind( type, &dummy, NULL ) != CNV_OK ) {
            return( TRUE );
        }
    } else {
        /* doesn't need a constructor */
        if( info->has_data ) {
            /* has some fields */
            return( TRUE );
        }
    }
    return( FALSE );
}

static boolean diagnoseAbstractDefn( SYMBOL sym, TYPE type )
{
    type = ArrayBaseType( type );
    CErr( ERR_CANNOT_DEFINE_ABSTRACT_VARIABLE, sym, type );
    ScopeNotePureFunctions( type );
    return( FALSE );
}

boolean DeclNoInit( DECL_INFO *dinfo )
/************************************/
{
    SYMBOL sym;
    SCOPE scope;
    TYPE type;
    base_info info;

    sym = dinfo->sym;
    if( sym == NULL ) {
        return( FALSE );
    }
    if( SymIsTypedef( sym ) || SymIsFunction( sym ) || sym->id == SC_EXTERN ) {
        return( FALSE );
    }
    type = sym->sym_type;
    if( SymIsStaticMember( sym ) ) {
        if( ScopeType( GetCurrScope(), SCOPE_CLASS ) ) {
            if( TypeAbstract( type ) ) {
                return( diagnoseAbstractDefn( sym, type ) );
            }
            return( FALSE );
        }
        /* "int C::a;" instead of "static int a;" in "class C" */
        CompFlags.external_defn_found = TRUE;
        if( SymIsInitialized( sym ) ) {
            if( ! ( sym->flag & SF_IN_CLASS_INIT ) ) {
                if( ! TemplateMemberCanBeIgnored() ) {
                    CErr2p( ERR_CANNOT_INIT_AGAIN, sym );
                }
                return( FALSE );
            } else {
                /* reset in-class initialization flag to get the
                 * symbol exported */
                sym->flag &= ~SF_IN_CLASS_INIT;
            }
        }
        if( ! TypeDefined( type ) ) {
            CErr2p( ERR_CANNOT_DEFINE_VARIABLE, sym );
            return( FALSE );
        }
        if( TypeAbstract( type ) ) {
            return( diagnoseAbstractDefn( sym, type ) );
        }
    } else {
        if( ! TypeDefined( type ) ) {
            CErr2p( ERR_CANNOT_DEFINE_VARIABLE, sym );
            return( FALSE );
        }
        if( TypeAbstract( type ) ) {
            return( diagnoseAbstractDefn( sym, type ) );
        }
        scope = SymScope( sym );
        if( ScopeType( scope, SCOPE_TEMPLATE_INST ) ) {
            if( TemplateVerifyDecl( sym ) ) {
                return( FALSE );
            }
        }
        if( sym->id != SC_STATIC && ScopeType( scope, SCOPE_FILE ) ) {
            CompFlags.external_defn_found = TRUE;
            if( LinkageIsC( sym ) ) {
                CompFlags.extern_C_defn_found = 1;
            }
        }
        if( ! ScopeType( scope, SCOPE_FILE ) && ! ScopeType( scope, SCOPE_BLOCK ) ){
            return( FALSE );
        }
        if( SymIsInitialized( sym ) ) {
            return( FALSE );
        }
    }
    type = baseTypeInfo( type, &info );
    switch( info ) {
    case IS_REF:
        CErr1( ERR_REFERENCE_MUST_BE_INITIALIZED );
        return( FALSE );
    case IS_CONST:
        if( ConstNeedsExplicitInitializer( type )
          && ! SymIsInitialized( sym ) ) {
            CErr1( ERR_CONST_MUST_BE_INITIALIZED );
            return( FALSE );
        }
    }
    return( TRUE );
}

boolean DeclWithInit( DECL_INFO *dinfo )
/**************************************/
{
    TYPE type;
    SYMBOL sym;
    SCOPE scope;

    sym = dinfo->sym;
    if( sym == NULL ) {
        return( FALSE );
    }
    if( SymIsTypedef( sym ) ) {
        CErr1( ERR_CANNOT_INIT_TYPEDEF );
        return( FALSE );
    }
    if( SymIsFunction( sym ) ) {
        CErr1( ERR_CANNOT_INIT_FUNCTION );
        return( FALSE );
    }
    if( SymIsInitialized( sym ) ) {
        if( SymIsStaticMember( sym ) && TemplateMemberCanBeIgnored() ) {
            return( FALSE );
        }
        CErr2p( ERR_CANNOT_INIT_AGAIN, sym );
        return( FALSE );
    }
    if( sym->id == SC_PUBLIC ) {
        CErr2p( ERR_SYM_ALREADY_DEFINED, sym );
        return( FALSE );
    }
    scope = SymScope( sym );
    if( ScopeEquivalent( scope, SCOPE_FILE ) ) {
        if( ScopeType( GetCurrScope(), SCOPE_TEMPLATE_INST ) ) {
            if( TemplateVerifyDecl( sym ) ) {
                return( FALSE );
            }
        }
        if( sym->id != SC_STATIC ) {
            CompFlags.external_defn_found = TRUE;
            if( LinkageIsC( sym ) ) {
                CompFlags.extern_C_defn_found = 1;
            }
        }
        if( ! ScopeType( GetCurrScope(), SCOPE_FILE ) ) {
            CErr2p( ERR_CANNOT_INIT_IN_NON_FILE_SCOPE, sym );
        }
    } else if( SymIsStaticMember( sym ) && ! ScopeType( GetCurrScope(), SCOPE_CLASS ) ) {
        /* "int C::a;" instead of "static int a;" in "class C" */
        CompFlags.external_defn_found = TRUE;
    }
    type = sym->sym_type;
    if( TypeDefined( type ) ) {
        if( TypeAbstract( type ) ) {
            return( diagnoseAbstractDefn( sym, type ) );
        }
        return( TRUE );
    }
    /* must only be unknown array size with defined base type */
    type = TypedefModifierRemove( type );
    if( type->id == TYP_ARRAY ) {
        if( type->u.a.array_size == 0 ) {
            if( TypeDefined( type->of ) ) {
                if( TypeAbstract( type->of ) ) {
                    return( diagnoseAbstractDefn( sym, type ) );
                }
                return( TRUE );
            }
        }
    }
    CErr2p( ERR_CANNOT_DEFINE_VARIABLE, sym );
    return( FALSE );
}

static void handleInlineFunction( SYMBOL sym )
{
    switch( sym->id ) {
    case SC_STATIC:
    case SC_TYPEDEF:
    case SC_FUNCTION_TEMPLATE:
    case SC_STATIC_FUNCTION_TEMPLATE:
    case SC_EXTERN:
    case SC_EXTERN_FUNCTION_TEMPLATE:
        return;
    }
    sym->id = SC_STATIC;
}

void DeclDefaultStorageClass( SCOPE scope, SYMBOL sym )
/*****************************************************/
{
    TYPE fn_type;
    base_info info;

    if( sym->id == SC_DEFAULT ) {
        return;
    }
    if( scope->id == SCOPE_FILE ) {
        fn_type = FunctionDeclarationType( sym->sym_type );
        if( fn_type != NULL && ( fn_type->flag & TF1_INLINE ) != 0 ) {
            handleInlineFunction( sym );
        }
    }
    if( sym->id != SC_NULL ) {
        return;
    }
    switch( scope->id ) {
    case SCOPE_CLASS:
        sym->id = SC_MEMBER;
        break;
    case SCOPE_FILE:
        baseTypeInfo( sym->sym_type, &info );
        if( info & IS_CONST ) {
            /* default linkage is internal for a const object */
            sym->id = SC_STATIC;
        } else if( info & IS_DLLIMPORT ) {
            /* default linkage is extern for a __declspec(dllimport) object */
            sym->id = SC_EXTERN;
        }
        break;
    case SCOPE_BLOCK:
    case SCOPE_FUNCTION:
        sym->id = SC_AUTO;
        break;
#ifndef NDEBUG
    case SCOPE_TEMPLATE_PARM:
    case SCOPE_TEMPLATE_SPEC_PARM:
    case SCOPE_TEMPLATE_DECL:
    case SCOPE_TEMPLATE_INST:
        break;
    default:
        CFatal( "unknown scope type" );
#endif
    }
}

static boolean commonCheck( SYMBOL prev, SYMBOL curr, decl_check *control )
{
    SCOPE scope;

    if( SymIsStaticMember( prev ) ) {
        switch( curr->id ) {
        case SC_NULL:
        case SC_STATIC:
            *control |= DC_REDEFINED;
            return( TRUE );
        }
    }
    scope = SymScope( prev );
    if( ScopeType( scope, SCOPE_FILE ) ) {
        switch( prev->id ) {
        case SC_EXTERN:
            switch( curr->id ) {
            case SC_NULL:
                if( SymIsInitialized( prev ) ) {
                    *control |= DC_REDEFINED;
                }
                /* fall through */
            case SC_EXTERN:
                prev->id = curr->id;
                return( TRUE );
            }
            break;
        case SC_PUBLIC:
            switch( curr->id ) {
            case SC_NULL:
                *control |= DC_REDEFINED;
                /* fall through */
            case SC_EXTERN:
                return( TRUE );
            }
            break;
        case SC_STATIC:
            if( curr->id == SC_EXTERN ) {
                // p.98 ARM (7.1.1)
                return( TRUE );
            }
            break;
        }
    }
    return( FALSE );
}

static fn_stg_class_status checkFnStorageClass( SYMBOL prev, SYMBOL curr, decl_check *control )
{
    if( SymIsAlias( prev ) ) {
        if( SymIsAlias( curr ) ) {
            if( prev->u.alias == curr->u.alias ) {
                return( FSCS_SAME_FN );
            }
            return( FSCS_DIFF_FN );
        }
        CErr2p( ERR_PREV_USING_DECL, prev );
        return( FSCS_NULL );
    }
    if( SymIsAlias( curr ) ) {
        CErr2p( ERR_CURR_USING_DECL, prev );
        return( FSCS_NULL );
    }
    /* check storage class specifiers for two identical functions */
    if( commonCheck( prev, curr, control ) ) {
        return( FSCS_SAME_FN );
    }
    if( curr->id == SC_DEFAULT ) {
        if( prev->id == SC_DEFAULT ) {
            if( SymDefaultBase( curr ) == SymDefaultBase( prev ) ) {
                return( FSCS_SAME_FN );
            }
        }
        return( FSCS_DIFF_FN );
    }
    if( prev->id == SC_DEFAULT ) {
        CErr2p( WARN_FN_HITS_ANOTHER_ARG, prev );
        return( FSCS_DIFF_FN );
    }
    switch( curr->id ) {
    case SC_NULL:
    case SC_FUNCTION_TEMPLATE:
        break;
    case SC_EXTERN:
        if( prev->id == SC_NULL ) {
            prev->id = SC_EXTERN;
        } else if( prev->id != SC_EXTERN ) {
            CErr2p( ERR_CONFLICTING_STORAGE_CLASSES, prev );
            return( FSCS_NULL );
        }
        break;
    case SC_EXTERN_FUNCTION_TEMPLATE:
        if( prev->id == SC_FUNCTION_TEMPLATE ) {
            prev->id = SC_EXTERN_FUNCTION_TEMPLATE;
        } else if( prev->id != SC_EXTERN_FUNCTION_TEMPLATE ) {
            CErr2p( ERR_CONFLICTING_STORAGE_CLASSES, prev );
            return( FSCS_NULL );
        }
        break;
    case SC_STATIC:
        if( prev->id == SC_NULL && CompFlags.extensions_enabled ) {
            prev->id = SC_STATIC;
        } else if( prev->id != SC_STATIC ) {
            CErr2p( ERR_CONFLICTING_STORAGE_CLASSES, prev );
            return( FSCS_NULL );
        }
        break;
    case SC_STATIC_FUNCTION_TEMPLATE:
        if( prev->id == SC_FUNCTION_TEMPLATE && CompFlags.extensions_enabled ) {
            prev->id = SC_STATIC_FUNCTION_TEMPLATE;
        } else if( prev->id != SC_STATIC_FUNCTION_TEMPLATE ) {
            CErr2p( ERR_CONFLICTING_STORAGE_CLASSES, prev );
            return( FSCS_NULL );
        }
        break;
    default:
        switch( prev->id ) {
        case SC_DEFAULT:
            DbgUseless();
            return( FSCS_DIFF_FN );
        default:
            if( prev->id != curr->id ) {
                CErr2p( ERR_CONFLICTING_STORAGE_CLASSES, prev );
                return( FSCS_NULL );
            }
        }
    }
    return( FSCS_SAME_FN );
}

static boolean checkVarStorageClass( SYMBOL prev, SYMBOL curr, decl_check *control )
{
    /* check storage class specifiers for two identical variables */
    if( commonCheck( prev, curr, control ) ) {
        return( TRUE );
    }
    *control |= DC_REDEFINED;
    switch( curr->id ) {
    case SC_NULL:
    case SC_ENUM:
        break;
    default:
        switch( prev->id ) {
        case SC_ENUM:
            break;
        default:
            if( prev->id != curr->id ) {
                CErr2p( ERR_CONFLICTING_STORAGE_CLASSES, prev );
            }
        }
    }
    return( FALSE );
}

static SYMBOL combineVariables( SYMBOL prev, SYMBOL curr, prev_sym_adjust control, type_flag new_flags )
{
    TYPE sym_type;

    if( !SymIsInitialized( prev ) ) {
        /* transfer symbol location if previous version wasn't initialized */
        SymLocnCopy( prev, curr );
    }
    sym_type = prev->sym_type;
    if( control & PSA_USE_CURR_MODS ) {
        sym_type = TypeOverrideModifiers( sym_type, curr->sym_type );
    }
    if( new_flags != TF1_NULL ) {
        sym_type = MakeModifiedType( sym_type, new_flags );
    }
    prev->sym_type = sym_type;
    return( prev );
}

static boolean memModelModifiersOK( type_flag prev_flags, type_flag curr_flags,
                                    prev_sym_adjust *adjust )
{
    // MSC allows:
    //          extern int __far x;
    //          int __based(__segname("_DATA")) x = 2;
    // or:
    //          extern int __based(__segname("_DATA")) y;
    //          int __far y = 3;

    prev_flags &= TF1_MEM_MODEL;
    curr_flags &= TF1_MEM_MODEL;
    if( curr_flags == prev_flags ) {
        // identical is OK
        return( TRUE );
    }
    switch( prev_flags ) {
    case TF1_FAR:
        if( curr_flags == TF1_BASED_STRING ) {
            // use new modifiers
            *adjust |= PSA_USE_CURR_MODS;
            return( TRUE );
        }
        break;
    case TF1_BASED_STRING:
        if( curr_flags == TF1_FAR ) {
            // keep old modifiers
            return( TRUE );
        }
        break;
    }
    // memory model modifiers are different
    return( FALSE );
}

static type_flag cannotChangeFlags( type_flag prev, type_flag curr,
                                    boolean ignore_mem_model )
{
    /* truth table:

        pc oa r  (p^c) & ~((c & a) | (p & o))
        00 00 0a   0   0 x
        00 01 0a   0   0 x
        00 10 0a   0   0 x
        00 11 0a   0   0 x
        01 00 1d   1   1 1    0    0    0
        01 01 0b   1   0 0    1    1    0
        01 10 1d   1   1 1    0    0    0
        01 11 0b   1   0 0    1    1    0
        10 00 1d   1   1 1    0    0    0
        10 01 1d   1   1 1    0    0    0
        10 10 0c   1   0 0    0    1    1
        10 11 0c   1   0 0    0    1    1
        11 00 0a   0   0 x
        11 01 0a   0   0 x
        11 10 0a   0   0 x
        11 11 0a   0   0 x
                       ^r

        Code equivalent:

            if p == c
                return 0        // modifiers are identical (case a)
            if p == 0 && c == 1 && a == 1
                return 0        // this modifier can be added (case b)
            if p == 1 && c == 0 && o == 1
                return 0        // this modifier can be omitted (case c)
            // p != c here
            return 1            // this modifier can't change (case d)
    */
    type_flag result;

    prev &= ~TF1_MOD_IGNORE;
    curr &= ~TF1_MOD_IGNORE;
    if( ignore_mem_model ) {
        prev &= ~TF1_MEM_MODEL;
        curr &= ~TF1_MEM_MODEL;
    }
    result = ( prev ^ curr );
    result &= ~(( curr & TF1_MOD_ADD_LATER ) | ( prev & TF1_MOD_OMIT_LATER ));
    return( result );
}

static void prevCurrErr( unsigned msg, SYMBOL prev, SYMBOL curr )
{
    if( CErr2p( msg, prev ) & MS_PRINTED ) {
        CErr2p( INF_CURR_DECL_TYPE, curr->sym_type );
    }
}

static boolean combinableSyms( SYMBOL prev, SYMBOL curr, prev_sym_adjust *adjust, type_flag *new_flags )
{
    TYPE prev_type;
    TYPE curr_type;
    TYPE prev_unmod;
    TYPE curr_unmod;
    type_flag prev_flags;
    type_flag curr_flags;
    void *prev_base;
    void *curr_base;
    target_size_t curr_size;
    boolean ret;
    struct {
        unsigned file_scope : 1;
    } flag;

    *adjust = PSA_NULL;
    *new_flags = TF1_NULL;
    ret = TRUE;
    flag.file_scope = FALSE;
    if( ScopeType( SymScope( curr ), SCOPE_FILE ) ) {
        flag.file_scope = TRUE;
        if( LinkageSpecified() && ! LinkageIsCurr( prev ) ) {
            CErr2p( ERR_CONFLICTING_LINKAGE_SPEC, prev );
            ret = FALSE;
        }
    }
    prev_type = prev->sym_type;
    curr_type = curr->sym_type;
    if( TypesIdentical( prev_type, curr_type ) ) {
        return( ret );
    }
    prev_unmod = TypeModFlagsBaseEC( prev_type, &prev_flags, &prev_base );
    curr_unmod = TypeModFlagsBaseEC( curr_type, &curr_flags, &curr_base );
    if(( prev_flags ^ curr_flags ) & ~TF1_MOD_IGNORE ) {
        if( memModelModifiersOK( prev_flags, curr_flags, adjust ) ) {
            if( cannotChangeFlags( prev_flags, curr_flags, TRUE ) != TF1_NULL ) {
                prevCurrErr( ERR_CONFLICTING_MODIFIERS, prev, curr );
                ret = FALSE;
            }
        } else {
            prevCurrErr( ERR_CONFLICTING_MODIFIERS, prev, curr );
            ret = FALSE;
        }
    }
    if( ret && ( prev_flags & TF1_BASED ) != TF1_NULL ) {
        if((( prev_flags ^ curr_flags ) & TF1_BASED ) == TF1_NULL ) {
            if( ! TypeBasesEqual( prev_flags, prev_base, curr_base ) ) {
                prevCurrErr( ERR_CONFLICTING_MODIFIERS, prev, curr );
                ret = FALSE;
            }
        }
    }
    if( ret ) {
        *new_flags = ( curr_flags & ~prev_flags ) & TF1_MOD_ADD_LATER;
    }
    if( TypesIdentical( prev_unmod, curr_unmod ) ) {
        return( ret );
    }
    if( prev_unmod->id == TYP_ARRAY && curr_unmod->id == TYP_ARRAY ) {
        curr_size = curr_unmod->u.a.array_size;
        if( prev_unmod->u.a.array_size == 0 ) {
            if( curr_size != 0 ) {
                if( TypesIdentical( prev_unmod->of, curr_unmod->of ) ) {
                    prev_type = DupArrayTypeForArray( prev_type, curr_size );
                    prev->sym_type = prev_type;
                    return( ret );
                }
            }
        } else {
            /* (this is accepted practice)
                struct S {
                    static int a[10];
                };
                int S::a[];

                extern char buff[256];
                char buff[] = { 'a' };
            */
            if( SymIsStaticMember( prev ) || flag.file_scope ) {
                if( curr_size == 0 ) {
                    if( TypesIdentical( prev_unmod->of, curr_unmod->of ) ) {
                        return( ret );
                    }
                }
            }
        }
    }
    return( FALSE );
}

static SYMBOL combineFunctions( SYMBOL prev_fn, SYMBOL curr_fn )
{
    void        *prev_base;
    void        *curr_base;
    AUX_INFO    *prev_pragma;
    AUX_INFO    *curr_pragma;
    type_flag   new_flags;
    type_flag   prev_flags;
    type_flag   curr_flags;
    type_flag   prev_fn_flags;
    type_flag   curr_fn_flags;
    TYPE        prev_type;
    TYPE        curr_type;
    TYPE        unmod_prev_type;
    TYPE        unmod_curr_type;
    arg_list    *prev_args;
    arg_list    *curr_args;
    struct {
        unsigned check_bases : 1;
    } flag;

    if( prev_fn->id == SC_DEFAULT || curr_fn->id == SC_DEFAULT ) {
        return( prev_fn );
    }
    prev_type = prev_fn->sym_type;
    curr_type = curr_fn->sym_type;
    /* check modifiers */
    unmod_prev_type = TypeModFlagsBaseEC( prev_type, &prev_flags, &prev_base );
    unmod_curr_type = TypeModFlagsBaseEC( curr_type, &curr_flags, &curr_base );
    flag.check_bases = TRUE;
    if(( prev_flags ^ curr_flags ) & ~TF1_MOD_IGNORE ) {
        if( cannotChangeFlags( prev_flags, curr_flags, FALSE ) != TF1_NULL ) {
            prevCurrErr( ERR_CONFLICTING_MODIFIERS, prev_fn, curr_fn );
            curr_flags = prev_flags;
            flag.check_bases = FALSE;
        }
    }
    if( flag.check_bases && ( prev_flags & TF1_BASED ) != TF1_NULL ) {
        DbgAssert((( prev_flags ^ curr_flags ) & TF1_BASED ) == TF1_NULL );
        if( ! TypeBasesEqual( prev_flags, prev_base, curr_base ) ) {
            prevCurrErr( ERR_CONFLICTING_MODIFIERS, prev_fn, curr_fn );
            curr_flags = prev_flags;
        }
    }
    new_flags = ( curr_flags & ~prev_flags ) & TF1_MOD_ADD_LATER;
    /* check function type */
    prev_fn_flags = unmod_prev_type->flag;
    curr_fn_flags = unmod_curr_type->flag;
    if(( prev_fn_flags ^ curr_fn_flags ) & TF1_PLUSPLUS ) {
        if( LinkageSpecified() ) {
            CErr2p( ERR_CONFLICTING_LINKAGE_SPEC, prev_fn );
        }
    } else if((( ~prev_fn_flags ) & curr_fn_flags ) & TF1_FN_CANT_ADD_LATER ) {
        /* diagnose fn properties that must be present in the first declaration */
        /*
            prev curr added? ~prev (~prev)&curr
               0 0      0      1         0
               0 1      1      1         1
               1 0      0      0         0
               1 1      0      0         0
        */
        CErr2p( ERR_CANNOT_REDECLARE_FUNCTION_PROPERTIES, prev_fn );
    }
    prev_args = unmod_prev_type->u.f.args;
    curr_args = unmod_curr_type->u.f.args;
    if( prev_args->qualifier != curr_args->qualifier ) {
        prevCurrErr( ERR_CONFLICTING_MODIFIERS, prev_fn, curr_fn );
    }
    curr_pragma = TypeHasPragma( unmod_curr_type );
    if( curr_pragma != NULL ) {
        prev_pragma = TypeHasPragma( unmod_prev_type );
        if( prev_pragma == NULL || prev_pragma != curr_pragma ) {
            prevCurrErr( ERR_CONFLICTING_PRAGMA_MODIFIERS, prev_fn, curr_fn );
        }
    }
    if( SymIsClassMember( curr_fn ) &&
        SymScope( curr_fn ) == ScopeNearestFileOrClass( GetCurrScope() ) ) {
        // see C++98 9.3 (2)
        CErr2p( ERR_CANNOT_REDECLARE_MEMBER_FUNCTION, prev_fn );
    }
    prev_fn->sym_type = MakeCombinedFunctionType( prev_type, curr_type, new_flags );
    if( ! SymIsInitialized( prev_fn ) ) {
        /* transfer symbol location if previous version wasn't initialized */
        SymLocnCopy( prev_fn, curr_fn );
    }
    return( prev_fn );
}

/* see 3.6.1 Main function [basic.start.main] */
static void verifyMainFunction( SYMBOL sym )
{
    TYPE fn_type;

    fn_type = FunctionDeclarationType( sym->sym_type );

    if( fn_type->flag & TF1_INLINE ) {
        CErr1( ERR_MAIN_CANNOT_BE_INLINE );
    }

    switch( sym->id ) {
    case SC_STATIC:
        CErr1( ERR_MAIN_CANNOT_BE_STATIC );
        break;
    case SC_FUNCTION_TEMPLATE:
    case SC_EXTERN_FUNCTION_TEMPLATE:
    case SC_STATIC_FUNCTION_TEMPLATE:
        CErr1( ERR_MAIN_CANNOT_BE_FN_TEMPLATE );
        break;
    }

    if( ( fn_type->of->id != TYP_SINT )
     || ( ! CompFlags.extensions_enabled
       && DefaultIntType( fn_type->of ) ) ) {
        CErr1( ANSI_MAIN_MUST_RETURN_INT );
    }
}

void DeclVerifyNoOtherCLinkage( SYMBOL sym, SYMBOL exclude )
/**********************************************************/
{
    SYMBOL check;

    RingIterBeg( sym->name->name_syms, check ) {
        if( LinkageIsC( check ) && check != exclude ) {
            CErr2p( ERR_ONLY_ONE_C_LINKAGE, check );
            break;
        }
    } RingIterEnd( check )
}

static void verifyCanBeOverloaded( SYMBOL_NAME sym_name, SYMBOL sym, NAME name )
{
    SCOPE scope;

    if( MainProcedure( sym ) ) {
        CErr1( ERR_MAIN_CANNOT_BE_OVERLOADED );
    } else if( name == CppOperatorName( CO_DELETE ) ) {
        CErr2p( ERR_FUNCTION_CANNOT_BE_OVERLOADED, name );
    } else if( name == CppOperatorName( CO_DELETE_ARRAY ) ) {
        CErr2p( ERR_FUNCTION_CANNOT_BE_OVERLOADED, name );
    } else if( sym->id != SC_DEFAULT ) {
        scope = sym_name->containing;
        if( CurrLinkage == CLinkage ) {
            /* new symbol doesn't have linkage set yet but it will be "C" */
            if( ScopeId( scope ) == SCOPE_FILE ) {
                DeclVerifyNoOtherCLinkage( sym_name->name_syms, NULL );
            }
        }
    }
}

static boolean userWantsTypeSymOverload( SYMBOL typedef_sym, SYMBOL non_typedef_sym )
{
    SRCFILE typedef_file;
    SRCFILE non_typedef_file;

    typedef_file = typedef_sym->locn->tl.src_file;
    non_typedef_file = non_typedef_sym->locn->tl.src_file;
    if( SrcFileSame( typedef_file, non_typedef_file ) ) {
        // same source file? user probably intends for this to work
        return( TRUE );
    }
    if( IsSrcFileLibrary( typedef_file ) && IsSrcFileLibrary( non_typedef_file ) ) {
        // both system header files? vendor probably intends for this to work
        return( TRUE );
    }
    return( FALSE );
}

SYMBOL DeclCheck( SYMBOL_NAME sym_name, SYMBOL sym, decl_check *control )
/***********************************************************************/
{
    TYPE sym_type;
    TYPE chk_type;
    TYPE new_type;
    SYMBOL chk_sym;
    SYMBOL ret_sym;
    FNOV_RESULT check;
    boolean new_sym_is_function;
    NAME name;
    prev_sym_adjust sym_adjust;
    fn_stg_class_status fn_sc_status;
    type_flag new_flags;

    *control = DC_NULL;
    name = sym_name->name;
    sym_type = TypedefRemove( sym->sym_type );
    ret_sym = sym;
    switch( sym->id ) {
    case SC_NAMESPACE:
        chk_sym = sym_name->name_type;
        if( chk_sym != NULL ) {
            DbgAssert( chk_sym->id != SC_NAMESPACE );
            CErr2p( ERR_NAME_USED_BY_NON_NAMESPACE, chk_sym );
            return( NULL );
        }
        chk_sym = sym_name->name_syms;
        if( chk_sym != NULL ) {
            CErr2p( ERR_NAME_USED_BY_NON_NAMESPACE, chk_sym );
            return( NULL );
        }
        BrinfDeclNamespace( sym );
        _AddSymToRing( &(sym_name->name_type), sym );
        break;
    case SC_CLASS_TEMPLATE:
        chk_sym = sym_name->name_type;
        if( chk_sym != NULL ) {
            if( chk_sym->id == SC_CLASS_TEMPLATE ) {
                CErr2p( ERR_CANT_OVERLOAD_CLASS_TEMPLATES, chk_sym );
            } else {
                CErr2p( ERR_NAME_USED_BY_NON_CLASS_TEMPLATE, chk_sym );
            }
            return( NULL );
        }
        chk_sym = sym_name->name_syms;
        if( chk_sym != NULL ) {
            CErr2p( ERR_NAME_USED_BY_NON_CLASS_TEMPLATE, chk_sym );
            return( NULL );
        }
        BrinfDeclTemplateClass( sym );
        _AddSymToRing( &(sym_name->name_type), sym );
        break;
    case SC_TYPEDEF:
        chk_sym = sym_name->name_type;
        if( chk_sym != NULL ) {
            /* typedef is already present so make sure it redefines itself */
            chk_type = TypedefRemove( chk_sym->sym_type );
            if( sym_type != chk_type ) {
                switch( chk_sym->id ) {
                case SC_CLASS_TEMPLATE:
                    CErr2p( ERR_NAME_USED_BY_CLASS_TEMPLATE, chk_sym );
                    return( NULL );
                case SC_NAMESPACE:
                    CErr2p( ERR_NAME_USED_BY_NAMESPACE, chk_sym );
                    return( NULL );
                }
                if( TypesIdentical( sym_type, chk_type ) ) {
                    CErr2p( WARN_BENIGN_TYPEDEF_REDEFN, chk_sym );
                    BrinfReferenceSymbol( &sym->locn->tl, chk_sym );
                } else if( ( chk_type->id == TYP_CLASS )
                        && ( chk_type->u.c.scope == GetCurrScope() ) ) {
                    /* already diagnosed (class name injection) */
                } else {
                    CErr2p( ERR_INVALID_TYPEDEF_REDEFINITION, chk_sym );
                }
                return( NULL );
            }
            return( chk_sym );
        }
        chk_sym = sym_name->name_syms;
        if( chk_sym != NULL ) {
            if( ElaboratableType( sym_type ) == NULL ) {
                CErr2p( ERR_CURR_MUST_BE_ELABORATED_TYPEDEF, chk_sym );
                return( NULL );
            }
            if( ! userWantsTypeSymOverload( sym, chk_sym ) ) {
                CErr2p( WARN_NON_TYPEDEF_HAS_SAME_NAME, chk_sym );
            }
        }
        new_type = MakeTypedefOf( sym_type, sym_name->containing, sym );
        sym->sym_type = new_type;
        BrinfDeclTypedef( sym );
        _AddSymToRing( &(sym_name->name_type), sym );
        break;
    default:
        new_sym_is_function = SymIsFunction( sym );
        chk_sym = sym_name->name_syms;
        if( chk_sym == NULL ) {
            chk_sym = sym_name->name_type;
            if( chk_sym != NULL ) {
                switch( chk_sym->id ) {
                case SC_CLASS_TEMPLATE:
                    CErr2p( ERR_NAME_USED_BY_CLASS_TEMPLATE, chk_sym );
                    return( NULL );
                case SC_NAMESPACE:
                    CErr2p( ERR_NAME_USED_BY_NAMESPACE, chk_sym );
                    return( NULL );
                case SC_TYPEDEF:
                    if( ElaboratableType( chk_sym->sym_type ) == NULL ) {
                        CErr2p( ERR_PREV_MUST_BE_ELABORATED_TYPEDEF, chk_sym );
                        return( NULL );
                    }
                    if( ! userWantsTypeSymOverload( chk_sym, sym ) ) {
                        CErr2p( WARN_TYPEDEF_HAS_SAME_NAME, chk_sym );
                    }
                    break;
                }
            }
            BrinfDeclSymbol( sym );
            _AddSymToRing( &(sym_name->name_syms), sym );
            if( new_sym_is_function ) {
                TYPE retn_type = FunctionDeclarationType( sym->sym_type )->of;
                type_flag flag; // - accumulated flags

                flag = TypeExplicitModFlags( retn_type );
                if( flag & TF1_CV_MASK ) {
                    TYPE type = TypedefedType( retn_type );
                    if( ( type == NULL ) || ( type->id != TYP_CLASS ) ) {
                        CErr2p( WARN_MEANINGLESS_QUALIFIER_IN_RETURN_TYPE,
                                retn_type );
                    }
                }

                if( MainProcedure( sym ) ) {
                    verifyMainFunction( sym );
                } else if( DefaultIntType( retn_type ) ) {
                    CErr2p( ERR_FUNCTION_BAD_RETURN, sym_name->name );
                }
            } else if ( ( sym_name->name != CppSpecialName( SPECIAL_RETURN_VALUE ) )
                     && DefaultIntType( sym->sym_type ) ) {
                CErr2p( ERR_MISSING_DECL_SPECS, sym_name->name );
            }
        } else {
            if( SymIsFunction( chk_sym ) ) {
                /* previously declared function of the same name */
                if( ! new_sym_is_function ) {
                    CErr2p( ERR_MIXING_FUNCTIONS_AND_VARS, name );
                    return( NULL );
                }
                check = IsOverloadedFuncDistinct( &chk_sym, sym, name, FNC_NO_DEALIAS );
                switch( check ) {
                case FNOV_EXACT_MATCH:
                    /* we want to use the previous SYMBOL */
                    fn_sc_status = checkFnStorageClass( chk_sym, sym, control );
                    if( fn_sc_status != FSCS_NULL ) {
                        if( fn_sc_status & FSCS_SAME_FN ) {
                            return( combineFunctions( chk_sym, sym ) );
                        }
                        if( fn_sc_status & FSCS_DIFF_FN ) {
                            *control |= DC_KEEP_SYMBOL;
                            ret_sym = chk_sym;
                            /* insert it to make sure any calls are ambiguous */
                            break;
                        }
                    }
                    return( NULL );
                case FNOV_NOT_DISTINCT_RETURN:
                    CErr2p( ERR_ATTEMPT_TO_OVERLOAD_RETURN, chk_sym );
                    /* insert it to make sure any calls are ambiguous */
                    break;
                case FNOV_NOT_DISTINCT:
                    CErr2p( ERR_FUNCTION_NOT_DISTINCT, chk_sym );
                    /* insert it to make sure any calls are ambiguous */
                    break;
                }
                verifyCanBeOverloaded( sym_name, sym, name );
                BrinfDeclSymbol( sym );
                _AddSymToRing( &(sym_name->name_syms), sym );
            } else {
                /* previously declared variable of the same name */
                if( new_sym_is_function ) {
                    CErr2p( ERR_MIXING_FUNCTIONS_AND_VARS, name );
                } else {
                    if( checkVarStorageClass( chk_sym, sym, control ) ) {
                        if( combinableSyms( chk_sym, sym, &sym_adjust, &new_flags ) ) {
                            BrinfReferenceSymbol( &sym->locn->tl, chk_sym );
                            return( combineVariables( chk_sym, sym, sym_adjust, new_flags ) );
                        }
                    }
                    CErr2p( ERR_SYM_ALREADY_DEFINED, chk_sym );
                }
                return( NULL );
            }
        }
    }
    return( ret_sym );
}

DECL_INFO *DeclFunction( DECL_SPEC *dspec, DECL_INFO *dinfo )
/***********************************************************/
{
    SYMBOL sym;

    if( dspec != NULL ) {
        CheckFunctionDSpec( dspec );
        dinfo->has_dspec = TRUE;
    }
    sym = dinfo->sym;
    if( ! SymIsFunction( sym ) ) {
        if( dspec != NULL ) {
            CErr2p( ERR_INCORRECT_FUNCTION_DECL, dinfo->name );
        }
        return( dinfo );
    }
    dinfo->fn_defn = TRUE;
    dinfo = InsertDeclInfo( GetCurrScope(), dinfo );
    /* must refetch 'sym' */
    sym = dinfo->sym;
    if( sym != NULL ) {
        if( dspec != NULL && dspec->linkage == NULL ) {
            /* linkage has not been set explicitly for this function */
            if( ScopeType( SymScope( sym ), SCOPE_FILE ) ) {
                if( ! LinkageIsCurr( sym ) ) {
                    /* see if any change is required */
                    if( LinkageIsC( sym ) ) {
                        LinkagePushC();
                    } else {
                        LinkagePushCpp();
                    }
                    PTypeForceLinkagePush( dspec );
                }
            }
        }
    }
    return( dinfo );
}
