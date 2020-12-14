/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Routines to emit type debugging information.
*
****************************************************************************/


#include "cvars.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "cgprotos.h"
#include "feprotos.h"
#include "cgen.h"


static dbug_type DBTypeStruct( TYPEPTR typ );
static dbug_type DBTypeEnum( TYPEPTR typ );
static void InitDBType( void );

//void RevTypeList();

static void InitDBType( void )
{
    TYPEPTR     typ;

    ScopeStruct = DBScope( "struct" );
    ScopeUnion = DBScope( "union" );
    ScopeEnum = DBScope( "enum" );
    typ = GetType( TYP_PLAIN_CHAR );
    if( typ->decl_type == TYP_UCHAR ) {
        typ->u1.debug_type = DBScalar( "char", TY_UINT_1 );
    } else {
        typ->u1.debug_type = DBScalar( "char", TY_INT_1 );
    }
    B_Int_1  = DBScalar( "signed char", TY_INT_1 );
    B_UInt_1 = DBScalar( "unsigned char", TY_UINT_1 );
    B_Short  = DBScalar( "short", TY_INT_2 );
    B_UShort = DBScalar( "unsigned short", TY_UINT_2 );
#if TARGET_INT == 4
    B_Int    = DBScalar( "int", TY_INT_4 );
    B_UInt   = DBScalar( "unsigned int", TY_UINT_4 );
#else
    B_Int    = DBScalar( "int", TY_INT_2 );
    B_UInt   = DBScalar( "unsigned int", TY_UINT_2 );
#endif
    B_Int32  = DBScalar( "long", TY_INT_4 );
    B_UInt32 = DBScalar( "unsigned long", TY_UINT_4 );
    B_Int64  = DBScalar( "__int64", TY_INT_8 );
    B_UInt64 = DBScalar( "unsigned __int64", TY_UINT_8 );
    B_Bool   = DBScalar( "_Bool", TY_UINT_1 );
    DebugNameList = NULL;
}

#if 0
static void RevTypeList( void )
{
    TYPEPTR     previous, current, following;

    previous = NULL;
    current = TypeHead;
    for( following = current->next_type; following != NULL; following = following->next_type ) {
        current->next_type = previous;
        previous = current;
        current = following;
    }
    current->next_type = previous;
    TypeHead = current;
}
#endif

static void EmitADBType( TYPEPTR typ )
{
    switch( typ->decl_type ) {
    case TYP_STRUCT:
    case TYP_UNION:
    case TYP_ENUM:
        if( !CompFlags.dump_types_with_names ) break;
        if( typ->u.tag->name[0] == '\0' ) break;
        goto dump_type;
    case TYP_TYPEDEF:
        if( !CompFlags.dump_types_with_names ) break;
        if( CompFlags.no_debug_type_names ) break;
    dump_type:
#if 0
        if( DebugFlag >= 2 ) {
            DumpType( typ, 0 );
            printf( "\n" );
        }
#endif
        DBType( typ );
        break;
    default:
        break;
    }
}

void EmitDBType( void )
{
//    RevTypeList();
    InitDBType();
    WalkTypeList( EmitADBType );
}

static dbug_type DBIntegralType( DATA_TYPE decl_type )
{
    dbug_type   ret_val;

    switch( decl_type ) {
    case TYP_CHAR:
        ret_val = B_Int_1;
        break;
    case TYP_UCHAR:
        ret_val = B_UInt_1;
        break;
    case TYP_SHORT:
        ret_val = B_Short;
        break;
    case TYP_USHORT:
        ret_val = B_UShort;
        break;
    case TYP_INT:
    default:
        ret_val = B_Int;
        break;
    case TYP_UINT:
        ret_val = B_UInt;
        break;
    case TYP_LONG:
        ret_val = B_Int32;
        break;
    case TYP_ULONG:
        ret_val = B_UInt32;
        break;
    case TYP_LONG64:
        ret_val = B_Int64;
        break;
    case TYP_ULONG64:
        ret_val = B_UInt64;
        break;
    case TYP_BOOL:
        ret_val = B_Bool;
        break;
    }
    return( ret_val );
}

static dbug_type DoBasedPtr( TYPEPTR typ, cg_type cgtype )
{
    dbug_type       ret_val;
    dbg_loc         dl;
    SYM_HANDLE      sym_handle;
    SYM_ENTRY       sym;
    bool            have_retval;

    have_retval = false;
    ret_val = 0;
    dl = DBLocInit();
    sym_handle = typ->u.p.based_sym;
    if( sym_handle == SYM_NULL ) {
        dl = DBLocConst( dl, 0 );
        dl = DBLocOp( dl, DB_OP_MK_FP, 0 );
    } else {
        if( sym_handle == Sym_CS ) {
            ret_val = DBPtr( cgtype, DBType( typ->object ) );
            have_retval = true;
        } else if( sym_handle == Sym_SS ) {
            ret_val = DBPtr( cgtype, DBType( typ->object ) );
            have_retval = true;
        } else {
            SymGet( &sym, sym_handle );
            if( sym.name[0] == '.' ) {  /* if segment label */
                ret_val = DBPtr( cgtype, DBType( typ->object ) );
                have_retval = true;
            } else {
                dl = DBLocSym( dl, (CGSYM_HANDLE)sym_handle );
                dl = DBLocOp( dl, DB_OP_POINTS, TY_UINT_2 );
                dl = DBLocConst( dl, 0 );
                dl = DBLocOp( dl, DB_OP_MK_FP, 0 );
            }
        }
    }
    if (!have_retval) {
        ret_val = DBBasedPtr( cgtype, DBType( typ->object ), dl );
    }
    DBLocFini( dl );
    return( ret_val );
}

dbug_type DBType( TYPEPTR typ )
{
    dbug_type       ret_val;
    dbg_proc        pr;
    TYPEPTR         *parm_types;
    TYPEPTR         parm_type;
    target_size     size;
    SYM_ENTRY       sym;
    struct debug_fwd_types fwd_info, *fip;
    cg_type         cgtype;

    if( typ->u1.debug_type == DBG_FWD_TYPE ) {
        fip = DebugNameList;
        while( fip->typ != typ )  fip = fip->next;
        if( fip->debug_name == NULL ) {
            fip->debug_name = DBBegName( "", fip->scope );
        }
        typ->u1.debug_type = DBForward( fip->debug_name );
    }
    if( typ->u1.debug_type != DBG_NIL_TYPE )  return( typ->u1.debug_type );

    fwd_info.next = DebugNameList;
    fwd_info.typ = typ;
    fwd_info.debug_name = NULL;
    fwd_info.scope = DBG_NIL_TYPE;
    /* default is INT */
    ret_val = B_Int;
    switch( typ->decl_type ) {
    case TYP_VOID:
        ret_val = DBScalar( "void", TY_DEFAULT );
        break;
    case TYP_FLOAT:
        ret_val = DBScalar( "float", TY_SINGLE );
        break;
    case TYP_DOUBLE:
        ret_val = DBScalar( "double", TY_DOUBLE );
        break;
    case TYP_LONG_DOUBLE:
        ret_val = DBScalar( "long double", TY_LONG_DOUBLE );
        break;
    case TYP_ARRAY:
        size = TypeSize( typ );
        if( size != 0 ) {
            --size;
        }
        ret_val = DBIntArrayCG( CGenType(typ), size, DBType( typ->object ) );
        break;
    case TYP_POINTER:
        cgtype = PtrType( typ->object, typ->u.p.decl_flags );
        if( typ->u.p.decl_flags & FLAG_BASED ) {
            ret_val = DoBasedPtr( typ, cgtype );
        } else {
            ret_val = DBPtr( cgtype, DBType( typ->object ) );
        }
        break;
    case TYP_STRUCT:
    case TYP_UNION:
        fwd_info.scope = (typ->decl_type == TYP_STRUCT)
                                    ? ScopeStruct : ScopeUnion;
        if( typ->u.tag->name[0] != '\0' ) {
            fwd_info.debug_name = DBBegName( typ->u.tag->name,
                                        fwd_info.scope );
        }
        DebugNameList = &fwd_info;
        typ->u1.debug_type = DBG_FWD_TYPE;
        ret_val = DBTypeStruct( typ );
        if( fwd_info.debug_name != NULL ) {
            ret_val = DBEndName( fwd_info.debug_name, ret_val );
        }
        DebugNameList = fwd_info.next;
        break;
    case TYP_FUNCTION:
        cgtype = TY_CODE_PTR;
        pr = DBBegProc( cgtype, DBType( typ->object ) );
        if( typ->u.fn.parms != NULL ) {
            for( parm_types = typ->u.fn.parms; (parm_type = *parm_types) != NULL; ++parm_types ) {
                if( parm_type->decl_type == TYP_DOT_DOT_DOT ) {
                    break;
                }
                DBAddParm( pr, DBType( parm_type ) );
            }
        }
        ret_val = DBEndProc( pr );
        break;
    case TYP_TYPEDEF:
        if( typ->type_flags & TF2_DUMMY_TYPEDEF ) {
            ret_val = DBType( typ->object );
        } else {
            SymGet( &sym, typ->u.typedefn );
            if( !CompFlags.no_debug_type_names ) {
                fwd_info.debug_name = DBBegName( sym.name, DBG_NIL_TYPE );
            }
            typ->u1.debug_type = DBG_FWD_TYPE;
            DebugNameList = &fwd_info;
            ret_val = DBType( typ->object );
            if( fwd_info.debug_name != NULL ) {
                ret_val = DBEndName( fwd_info.debug_name, ret_val );
                if( GenSwitches & DBG_CV ) {
                    DBTypeDef( sym.name, ret_val ); //get codeview typedef out
                }
            }
            DebugNameList = fwd_info.next;
        }
        break;
    case TYP_ENUM:
        ret_val = DBTypeEnum( typ );
        break;
    default:
        ret_val = DBIntegralType( typ->decl_type );
        break;
    }
    typ->u1.debug_type = ret_val;
    return( ret_val );
}


static void DumpFieldList( dbg_struct st, target_size bias,
                          FIELDPTR pfield, TYPEPTR field_obj )
{
    TYPEPTR field_typ;

    for( ; pfield != NULL; pfield = pfield->next_field ) {
        field_typ = pfield->field_type;
        if( pfield->name[0] == '\0' ) {
            /* anonymous struct/union -- suck up to this level */
            while( field_typ->decl_type == TYP_TYPEDEF ) {
                field_typ = field_typ->object;
            }
            DumpFieldList( st, bias + pfield->offset,
                        field_typ->u.tag->u.field_list, NULL );
        } else if(( field_typ->decl_type == TYP_FIELD ) ||
            ( field_typ->decl_type == TYP_UFIELD ) ) {
            field_typ->u1.debug_type = DBIntegralType(field_typ->u.f.field_type);
            DBAddBitField( st, bias + pfield->offset,
                field_typ->u.f.field_start,
                field_typ->u.f.field_width, pfield->name,
                field_typ->u1.debug_type );
        } else if( field_obj != NULL
                && field_typ->decl_type == TYP_ARRAY
                && field_typ->u.array->dimension == 0 ) {
            DBAddField( st, bias + pfield->offset,
                pfield->name, DBType( field_obj ));
        } else {
            DBAddField( st, bias + pfield->offset,
                pfield->name, DBType( pfield->field_type ));
        }
    }
}

static dbug_type DBTypeStruct( TYPEPTR typ )
{
    dbug_type   ret_val;
    dbg_struct  st;
    TYPEPTR     obj;

    if( typ->object != NULL ) {
        /* structure has a zero length array as last field */
        obj = typ->object;
    } else {
        obj = NULL;
    }
    st = DBBegNameStruct( typ->u.tag->name, CGenType( typ ),
                                ( typ->decl_type == TYP_STRUCT ) );
    ret_val = DBStructForward( st );
    if( ret_val != DBG_NIL_TYPE ) {
         typ->u1.debug_type = ret_val;
    }
    DumpFieldList( st, 0, typ->u.tag->u.field_list, obj );
    ret_val = DBEndStruct( st );
    return( ret_val );
}


static dbug_type DBTypeEnum( TYPEPTR typ )
{
    dbug_type   ret_val;
    dbg_enum    e;
    ENUMPTR     ep;

    e = DBBegEnum( CGenType( typ->object ) );
    for( ep = typ->u.tag->u.enum_list; ep != NULL; ep = ep->thread ) {
        DBAddConst64( e, ep->name, ep->value );
    }
    ret_val = DBEndEnum( e );
    if( typ->u.tag->name[0] != '\0' ) {
        DBEndName( DBBegName( typ->u.tag->name, ScopeEnum ), ret_val );
    }
    return( ret_val );
}

dbug_type FEDbgType( CGSYM_HANDLE cgsym_handle )
{
    SYM_HANDLE     sym_handle = (SYM_HANDLE)cgsym_handle;

    return( DBType( SymGetPtr( sym_handle )->sym_type ) );
}

dbug_type FEDbgRetType( CGSYM_HANDLE cgsym_handle )
{
    SYM_HANDLE     sym_handle = (SYM_HANDLE)cgsym_handle;
    TYPEPTR        typ;

    typ = SymGetPtr( sym_handle )->sym_type;
    if( typ->decl_type == TYP_FUNCTION ) {
        return( DBType( typ->object ) );
    } else {
        return( DBG_NIL_TYPE );
    }
}
