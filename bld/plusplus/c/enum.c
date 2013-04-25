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

#include <limits.h>

#include "cgfront.h"
#include "errdefns.h"
#include "stringl.h"
#include "ptree.h"
#include "enum.h"
#include "i64.h"


union _us                   // to enable either signed or unsigned treatment
{   signed_64 s64val;       // - signed/unsigned 64
#if defined( WATCOM_BIG_ENDIAN )
    struct {
        unsigned pad_u;
        unsigned uval;      // - unsigned
    };
    struct {
        unsigned pad_i;
        int sval;           // - signed
    };
#else
    unsigned uval;          // - unsigned
    int sval;               // - signed
#endif
};

struct enum_range           // describe range for enum
{   union _us lo;           // - low value
    union _us hi;           // - high value
    type_id id;             // - corresponding type
    unsigned :0;            // alignment
};

static struct enum_range const range_table[] =
#if _CPU == 386 || _CPU == _AXP
{   {   I64Val( 0xFFFFFFFF, 0xFFFFFF80 )
      , I64Val( 0x00000000, 0x0000007F )
      , TYP_SCHAR
    }
,   {   I64Val( 0x00000000, 0x00000000 )
      , I64Val( 0x00000000, 0x000000FF )
      , TYP_UCHAR
    }
,   {   I64Val( 0xFFFFFFFF, 0xFFFF8000 )
      , I64Val( 0x00000000, 0x00007FFF )
      , TYP_SSHORT
    }
,   {   I64Val( 0x00000000, 0x00000000 )
      , I64Val( 0x00000000, 0x0000FFFF )
      , TYP_USHORT
    }
,   {   I64Val( 0xFFFFFFFF, 0x80000000 )
      , I64Val( 0x00000000, 0x7FFFFFFF )
      , TYP_SINT
    }
,   {   I64Val( 0x00000000, 0x00000000 )
      , I64Val( 0x00000000, 0xFFFFFFFF )
      , TYP_UINT
    }
,   {   I64Val( 0x80000000, 0x00000000 )
      , I64Val( 0x7FFFFFFF, 0xFFFFFFFF )
      , TYP_SLONG64
    }
,   {   I64Val( 0x00000000, 0x00000000 )
      , I64Val( 0xFFFFFFFF, 0xFFFFFFFF )
      , TYP_ULONG64
    }
};
    #define RANGE_INDEX_SINT 4
#elif _CPU == 8086
{   {   I64Val( 0xFFFFFFFF, 0xFFFFFF80 )
      , I64Val( 0x00000000, 0x0000007F )
      , TYP_SCHAR
    }
,   {   I64Val( 0x00000000, 0x00000000 )
      , I64Val( 0x00000000, 0x000000FF )
      , TYP_UCHAR
    }
,   {   I64Val( 0xFFFFFFFF, 0xFFFF8000 )
      , I64Val( 0x00000000, 0x00007FFF )
      , TYP_SINT
    }
,   {   I64Val( 0x00000000, 0x00000000 )
      , I64Val( 0x00000000, 0x0000FFFF )
      , TYP_UINT
    }
,   {   I64Val( 0xFFFFFFFF, 0x80000000 )
      , I64Val( 0x00000000, 0x7FFFFFFF )
      , TYP_SLONG
    }
,   {   I64Val( 0x00000000, 0x00000000 )
      , I64Val( 0x00000000, 0xFFFFFFFF )
      , TYP_ULONG
    }
,   {   I64Val( 0x80000000, 0x00000000 )
      , I64Val( 0x7FFFFFFF, 0xFFFFFFFF )
      , TYP_SLONG64
    }
,   {   I64Val( 0x00000000, 0x00000000 )
      , I64Val( 0xFFFFFFFF, 0xFFFFFFFF )
      , TYP_ULONG64
    }
};
    #define RANGE_INDEX_SINT 2
#else
#error unknown _CPU
#endif

#define ENUM_RNG_MAX ( sizeof(range_table) / sizeof(range_table[0]) )


// Note: During overloading, it is a standard conversion from an enum to
//       the first of int, unsigned int, long, unsigned long which will
//       hold all the values. The underlying type must be either the first
//       of these which holds all the values or must be a type corresponding
//       to the algorithm in FNOVRANK for enum->arith conversions.
//
//       The underlying types are assigned so as to not affect overloading
//       when -ei flag is used or not.
//
static type_id figureOutBaseType( ENUM_DATA *edata )
{
    type_id     base_type;
    unsigned    index;
    unsigned    step;

    index = edata->index;
    step = ( edata->has_sign ) ? 2 : 1;
    for( ; index < ENUM_RNG_MAX; index += step ) {
        if( edata->next_signed ) {
            if( I64Cmp( &edata->next_value, &(range_table[ index ].lo.s64val) ) >= 0 ) break;
        } else {
            if( U64Cmp( &edata->next_value, &(range_table[ index ].hi.s64val) ) <= 0 ) break;
        }
    }
    if( index >= ENUM_RNG_MAX ) {
        CErr1( ERR_NO_ENUM_TYPE_POSSIBLE );
        edata->next_value.u._32[ 0 ] = 0;
        edata->next_value.u._32[ 1 ] = 0;
        index = ENUM_RNG_MAX - 1;
    }
    edata->index = (uint_8)index;
    base_type = range_table[ index ].id;
    return base_type;
}

void InitEnumState( ENUM_DATA *edata, PTREE id )
/**********************************************/
{
    edata->type = TypeError;
    if( CompFlags.make_enums_an_int ) {
        edata->base_id = TYP_SINT;
        edata->index = RANGE_INDEX_SINT;
    } else {
        edata->base_id = TYP_SCHAR;
        edata->index = 0;
    }
    edata->next_value.u._32[0] = 0;
    edata->next_value.u._32[1] = 0;
    edata->next_signed = FALSE;
    edata->has_sign = FALSE;
    SrcFileGetTokenLocn( &(edata->locn) );
    if( id != NULL ) {
        edata->locn = id->locn;
    }
    edata->id = id;
    edata->sym = NULL;
}

void EnumDefine( ENUM_DATA *edata )
/*********************************/
{
    NAME enum_typedef_name;
    SYMBOL sym;
    TYPE enum_type;
    TYPE base_type;

    // 7.2 para 5
    // If the  enumerator-list is empty, the underlying type is as if the
    // enumeration had a single enumerator with value 0.
    base_type = GetBasicType( edata->base_id );
    enum_type = MakeType( TYP_ENUM );
    enum_type->of = base_type;
    if( edata->id != NULL ) {
        DbgAssert( edata->id->op == PT_ID );
        enum_typedef_name = edata->id->u.id.name;
        PTreeFreeSubtrees( edata->id );
        edata->id = NULL;
    } else {
        enum_typedef_name = NULL;
    }
    if( enum_typedef_name != NULL ) {
        /* we have a named enum type */
        sym = AllocSymbol();
        sym->sym_type = enum_type;
    } else {
        sym = SymMakeDummy( enum_type, &enum_typedef_name );
        enum_type->flag |= TF1_UNNAMED;
    }
    sym->id = SC_TYPEDEF;
    edata->sym = sym;
    SymbolLocnDefine( &(edata->locn), sym );
    sym = InsertSymbol( GetCurrScope(), sym, enum_typedef_name );
    enum_type->u.t.scope = GetCurrScope();
    enum_type->u.t.sym = sym;
    edata->type = CheckDupType( enum_type );
}


void MakeEnumMember( ENUM_DATA *edata, PTREE id, PTREE val )
/**********************************************************/
{
    SYMBOL sym;
    signed_64 incr;

    /* enter the enumerated constant into the symbol table */
    sym = AllocSymbol();
    sym->id = SC_ENUM;
    sym->sym_type = edata->type;
    SymbolLocnDefine( &(id->locn), sym );
    sym = InsertSymbol( GetCurrScope(), sym, id->u.id.name );

    if(sym == NULL) /* error will have been reported */
        return;

    /* add the value into the symbol */
    if( val != NULL ) {
        /* value was specified */
        DbgVerify( val->op == PT_INT_CONSTANT, "Bad enum value" );
        if( SignedIntType( val->type ) ) {
            edata->next_signed = TRUE;
        } else {
            edata->next_signed = FALSE;
        }
        if( NULL == Integral64Type( val->type ) ) {
            if( edata->next_signed
             && val->u.int_constant >= 0 ) {
                edata->next_signed = FALSE;
            }
        } else {
            if( edata->next_signed
             && ! val->u.int64_constant.u.sign.v ) {
                edata->next_signed = FALSE;
            }
        }
        if( ! edata->has_sign
         && edata->next_signed ) {
            edata->has_sign = TRUE;
            if ( edata->index & 1 ) {
                // move to next bigger signed range
                ++ edata->index;
            }
        }
        edata->next_value = val->u.int64_constant;
        PTreeFree( val );
    } else if( edata->next_signed ) {
        if( edata->next_value.u.sign.v == 0 ) {
            edata->next_signed = FALSE;
        }
    }
    PTreeFree( id );
    edata->base_id = figureOutBaseType( edata );
    edata->type->of = GetBasicType( edata->base_id );
    SymBindConstant( sym, edata->next_value );
    if( ! edata->next_signed ) {
        // value is not signed negative (to avoid sign extension later on)
        sym->flag |= SF_ENUM_UINT;
    }
    incr.u._32[ I64LO32 ] = 1;
    incr.u._32[ I64HI32 ] = 0;
    U64Add( &edata->next_value, &incr, &edata->next_value );
    // what about wrap around ? to zero?
}

DECL_SPEC *MakeEnumType( ENUM_DATA *edata )
/*****************************************/
{
    DECL_SPEC *dspec;

    dspec = PTypeActualType( edata->type );
    dspec->type_defined = TRUE;
    dspec->type_declared = TRUE;
    return( dspec );
}

static boolean enumNameOK( TYPE type, NAME name )
{
    TYPE enum_type;

    if( type->id == TYP_TYPEDEF ) {
        if( ScopeType( type->u.t.scope, SCOPE_TEMPLATE_PARM )
         || ScopeType( type->u.t.scope, SCOPE_TEMPLATE_PARM ) ) {
            if( type->u.t.sym->name->name == name ) {
                // 14.2.1 para 2
                // <class T> can be ref'd as enum T if an enum type is used
                return( TRUE );
            }
        }
    }
    enum_type = EnumType( type );
    if( SimpleTypeName( enum_type ) != name ) {
        return( FALSE );
    }
    return( TRUE );
}

DECL_SPEC *EnumReference( ENUM_DATA *edata )
/******************************************/
{
    NAME name;
    DECL_SPEC *dspec;
    TYPE type;
    TYPE ref_type;
    SEARCH_RESULT *result;
    SYMBOL sym;
    SYMBOL_NAME sym_name;

    ref_type = TypeError;

    if( edata->id != NULL ) {
        if( edata->id->op == PT_ID ) {
            name = edata->id->u.id.name;

            result = ScopeFindLexicalEnumType( GetCurrScope(), name );
            if( result != NULL ) {
                sym_name = result->sym_name;
                sym = sym_name->name_type;
                type = sym->sym_type;
                if( ! enumNameOK( type, name ) ) {
                    ScopeFreeResult( result );
                    result = NULL;
                }
            }
            if( result != NULL ) {
                if( ScopeCheckSymbol( result, sym ) == FALSE ) {
                    /* no errors */
                    ref_type = type;
                }
                ScopeFreeResult( result );
            } else {
                CErr2p( ERR_UNDECLARED_ENUM_SYM, name );
            }
        } else {
            /* we are dealing with a scoped enum name here */
            PTREE right;

            DbgAssert( NodeIsBinaryOp( edata->id, CO_STORAGE ) );

            right = edata->id->u.subtree[1];
            DbgAssert( ( right->op == PT_ID ) );

            name = right->u.id.name;
            sym_name = edata->id->sym_name;

            DbgAssert( sym_name != NULL );
            sym = sym_name->name_type;
            type = sym->sym_type;

            if( ! enumNameOK( type, name ) ) {
                CErr2p( ERR_UNDECLARED_ENUM_SYM, name );
            } else {
                ref_type = type;
            }
        }

        PTreeFreeSubtrees( edata->id );
        edata->id = NULL;
    } else {
        CErr1( ERR_CANNOT_REFERENCE_UNNAMED_ENUM );
    }
    dspec = PTypeActualType( ref_type );
    dspec->nameless_allowed = TRUE;
    dspec->type_elaborated = TRUE;
    dspec->name = name;
    return( dspec );
}
