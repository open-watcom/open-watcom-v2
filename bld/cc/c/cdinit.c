/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Initialization of data ( e.g. int foo[] = { 0, 1, 2 }; )
*
****************************************************************************/


#include "cvars.h"
#include <limits.h>
#include "i64.h"
#include "roundmac.h"


#define DATA_QUAD_SEG_SIZE_DEF  _32K
#define DATA_QUAD_MAX_INDEX_DEF 0xFFFFF

#define DATA_QUAD_PER_SEG       (DATA_QUAD_SEG_SIZE_DEF / sizeof( DATA_QUAD_LIST ))
#define DATA_QUAD_SEG_SIZE      (DATA_QUAD_PER_SEG * sizeof( DATA_QUAD_LIST ))
#define DATA_QUAD_SEG_MAX       (DATA_QUAD_MAX_INDEX_DEF / DATA_QUAD_PER_SEG + 1)

typedef enum {
    IS_VALUE,
    IS_ADDR
} fold_state;

typedef struct {
    target_ssize        offset;
    union {
        STR_HANDLE      str_h;
        SYM_HANDLE      sym_h;
    } u;
    bool                is_str;
    bool                addr_set;
    bool                is_error;
    fold_state          state;
} addrfold_info;

/*
 * use a double-linked list of dataquads to facilitate insertions
 */
typedef struct data_quad_list {
    DATA_QUAD               dq;
    target_size             size;
    struct data_quad_list   *prev;
    struct data_quad_list   *next;
} DATA_QUAD_LIST;

static DATA_QUAD_LIST   *DataQuadSegs[DATA_QUAD_SEG_MAX]; /* segments for data quads */
static DATA_QUAD_LIST   *CurDataQuad;
static int              DataQuadSegIndex;
static int              DataQuadIndex;

static DATA_QUAD_LIST   *NewDataQuad( void );
static bool             CharArray( TYPEPTR typ );
static bool             WCharArray( TYPEPTR typ );
static void             InitCharArray( TYPEPTR typ );
static void             InitWCharArray( TYPEPTR typ );
static void             StoreFloat( DATA_TYPE dtype, target_size size );
static void             StoreInt64( TYPEPTR typ );

bool DataQuadsAvailable( void )
{
    DATA_QUAD_LIST  *dql = DataQuadSegs[0];

    return( dql != NULL && dql->next != NULL );
}

void InitDataQuads( void )
{
    DataQuadIndex = DATA_QUAD_PER_SEG;
    DataQuadSegIndex = -1;
    memset( DataQuadSegs, 0, sizeof( DataQuadSegs ) );
    /*
     * put a guard at the start
     */
    CurDataQuad = NewDataQuad();
    CurDataQuad->prev = NULL;
    CurDataQuad->next = NULL;
    CurDataQuad->size = 0;
}

void FreeDataQuads( void )
{
    unsigned    i;

    for( i = 0; i < DATA_QUAD_SEG_MAX; i++ ) {
        if( DataQuadSegs[i] == NULL )
            break;
        FEfree( DataQuadSegs[i] );
        DataQuadSegs[i] = NULL;
    }
}

void *StartDataQuadAccess( void )
{
    void    *cur_dqp;

    if( DataQuadsAvailable() ) {
        cur_dqp = CurDataQuad;
        CurDataQuad = DataQuadSegs[0]->next;
        return( cur_dqp );  /* indicate data quads exist */
    }
    return( NULL );         /* indicate no data quads */
}

void EndDataQuadAccess( void *p )
{
    CurDataQuad = p;
}

DATA_QUAD *NextDataQuad( void )
{
    DATA_QUAD   *dq_ptr;

    if( CurDataQuad == NULL )
        return( NULL );
    dq_ptr = &CurDataQuad->dq;
    CurDataQuad = CurDataQuad->next;
    return( dq_ptr );
}

static DATA_QUAD_LIST *NewDataQuad( void )
{
    static DATA_QUAD_LIST   *DataQuadPtr;
    DATA_QUAD_LIST          *dql;

    if( DataQuadIndex >= (DATA_QUAD_PER_SEG - 1) ) {
        if( DataQuadSegIndex == DATA_QUAD_SEG_MAX ) {
            CErr1( ERR_INTERNAL_LIMIT_EXCEEDED );
            CSuicide();
        }
        ++DataQuadSegIndex;
        DataQuadIndex = 0;
        DataQuadPtr = FEmalloc( DATA_QUAD_SEG_SIZE );
        DataQuadSegs[DataQuadSegIndex] = DataQuadPtr;
    }
    dql = DataQuadPtr;
    ++DataQuadIndex;
    ++DataQuadPtr;
    return( dql );
}

static void SplitDataQuad( DATA_QUAD_LIST *dql, target_size size )
/*****************************************************************
 * splits the dataquad pointed to by dql so that the current one
 * will have size "size" and the new one "oldsize - size"
 */
{
    DATA_QUAD_LIST  *ndql;
    DATA_QUAD       *ndq;
    DATA_QUAD       *dq;
    target_size     oldsize;

    ndql = NewDataQuad();
    ndql->next = dql->next;
    ndql->prev = dql;
    dql->next = ndql;
    if( ndql->next != NULL )
        ndql->next->prev = ndql;
    oldsize = dql->size;
    ndql->size = oldsize - size;
    dql->size = size;

    ndq = &ndql->dq;
    dq = &dql->dq;
    memcpy( ndq, dq, sizeof( *dq ) );

    if( dq->flags & Q_DATA ) {
        if( dq->flags & Q_2_INTS_IN_ONE ) {
            dq->flags = ndq->flags = Q_DATA;
            ndq->u_long_value1 = dq->u_long_value2;
            ndq->u_long_value2 = 0;
            dq->u_long_value2 = 0;
            size = 0;
        } else if( dq->flags & Q_REPEATED_DATA ) {
            dq->u_rpt_count = size / ( oldsize / dq->u_rpt_count );
            ndq->u_rpt_count -= dq->u_rpt_count;
            size = 0;
        } else if( dq->type == QDT_CONSTANT ) {
            dq->u_size = size;
            ndq->u_size -= size;
            size = 0;
        } else if( dq->type == QDT_CONST ) {
            dq->u.string_leaf->length = size;
            ndq->u.string_leaf->literal += size;
            ndq->u.string_leaf->length = oldsize - size;
            size = 0;
        }
    }
    if( size != 0 ) {
        /*
         * can't happen !
         */
        CErr2p( ERR_FATAL_ERROR, "Bad initializer quad" );
        CSuicide();
    }
}

static void DeleteDataQuad( DATA_QUAD_LIST *dql )
{
    dql->prev->next = dql->next;
    if( dql->next != NULL ) {
        dql->next->prev = dql->prev;
    }
}

static void GenDataQuad( DATA_QUAD *dq, target_size size )
{
    DATA_QUAD_LIST  *dql;
    target_size     cursize;

    dql = CurDataQuad->next;
    if( dql != NULL ) {
        /*
         * overwrite the current dataquad
         */
        cursize = dql->size;
        while( size > cursize && dql->next != NULL ) {
            DeleteDataQuad( dql );
            dql = dql->next;
            cursize += dql->size;
        }
        /*
         * dql now takes up cursize bytes but was defined to
         * take up dql->size bytes: split into a dataquad with
         * dql->size - (cursize - size) bytes and one with
         * cursize - size bytes.
         */
        if( size < cursize ) {
            SplitDataQuad( dql, dql->size - ( cursize - size ) );
        }
    } else {
        dql = NewDataQuad();
        CurDataQuad->next = dql;
        dql->prev = CurDataQuad;
        dql->next = NULL;
    }
    dql->size = size;
    memcpy( &dql->dq, dq, sizeof( DATA_QUAD ) );
    CurDataQuad = dql;
}

static void ZeroBytes( target_size n )
{
    DATA_QUAD   dq;

    if( n == 0 )
        return;
    dq.type = QDT_CONSTANT;
    dq.flags = Q_DATA;
    dq.u_size = n;
    GenDataQuad( &dq, n );
}

static void RelSeekBytes( target_ssize n )
{
    DATA_QUAD_LIST  *dql;

    dql = CurDataQuad;
    while( n < 0 && n <= -(target_ssize)dql->size ) {
        n += dql->size;
        dql = dql->prev;
    }
    while( n > 0 && dql->next != NULL ) {
        dql = dql->next;
        n -= dql->size;
    }
    /*
     * now -dql->size < n <= 0 or dql->next == NULL
     */
    if( n < 0 ) {
        SplitDataQuad( dql, dql->size + n );
    }
    CurDataQuad = dql;
    if( n > 0 ) {
        /*
         * dql->next == NULL
         */
        ZeroBytes( n );
    }
}

static void StoreIValue( DATA_TYPE dtype, int value, target_size size )
{
    static DATA_QUAD_LIST   *LastCurDataQuad;
    DATA_QUAD_LIST          *dql;
    DATA_QUAD               *dq_ptr;
    DATA_QUAD               dq;

    dq.flags = 0;
    dq.type = 0;
    dq_ptr = &dq;
    if( LastCurDataQuad == CurDataQuad ) {
        dq_ptr = &CurDataQuad->dq;
    }
    if( (DATA_TYPE)dq_ptr->type == dtype
      && dq_ptr->flags == (Q_DATA | Q_REPEATED_DATA)
      && dq_ptr->u_long_value1 == value ) {
        dq_ptr->u_rpt_count++;                  /* increment repeat count */
        CurDataQuad->size += size;
    } else if( (DATA_TYPE)dq_ptr->type == dtype
      && dq_ptr->flags == Q_DATA ) {
        if( dq_ptr->u_long_value1 == value ) {
            dq_ptr->flags |= Q_REPEATED_DATA;
            dq_ptr->u_rpt_count = 2;            /* repeat count */
        } else {
            dq_ptr->flags |= Q_2_INTS_IN_ONE;
            dq_ptr->u_long_value2 = value;
        }
        CurDataQuad->size += size;
    } else {
        dq.type = (enum quad_type)dtype;
        dq.flags = Q_DATA;
        dq.u_long_value1 = value;
        if( value != 0 )
            CompFlags.non_zero_data = true;
        GenDataQuad( &dq, size );
        LastCurDataQuad = CurDataQuad;
        return;
    }
    /*
     * if the next dataquad is non-empty we'll have to delete it
     */
    dql = CurDataQuad->next;
    if( dql != NULL ) {
        if( dql->size > size ) {
            SplitDataQuad( dql, size );
        }
        /*
         * no need to "free" the next one, just remove it from the list
         */
        DeleteDataQuad( dql );
    }
}

static void StoreIValue64( DATA_TYPE dtype, int64 value )
{
    DATA_QUAD           dq;

    dq.type = (enum quad_type)dtype;
    dq.flags = Q_DATA;
    dq.u.long64 = value;
    CompFlags.non_zero_data = true;
    GenDataQuad( &dq, TARGET_LONG64 );
}

static void AddrFold( TREEPTR tree, addrfold_info *info )
/********************************************************
 * Assume tree has been const folded
 */
{
    SYM_ENTRY           sym;
    target_ssize        offset;

    offset = 0;
    switch( tree->op.opr ) {
    case OPR_PUSHINT:
        info->state = IS_ADDR;
        info->offset = tree->op.u2.long_value;
        if( info->offset != 0 )
            CompFlags.non_zero_data = true;
        break;
    case OPR_PUSHFLOAT:
        info->state = IS_ADDR;
        if( tree->op.u2.float_value->len != 0 ) {
            info->offset = (target_ssize)atof( tree->op.u2.float_value->string );
        } else {
#ifdef _LONG_DOUBLE_
            long_double ld;
            double d;

            ld = tree->op.u2.float_value->ld;
            __iLDFD( &ld, &d );
            info->offset = (target_ssize)d;
#else
            info->offset = (target_ssize)tree->op.u2.float_value->ld.u.value;
#endif
        }
        if( info->offset != 0 )
            CompFlags.non_zero_data = true;
        break;
    case OPR_PUSHSTRING:
        info->state = IS_ADDR;
        if( info->addr_set ) {
            info->is_error = true;
        }
        info->addr_set = true;
        info->is_str = true;
        info->u.str_h = tree->op.u2.string_handle;
        tree->op.u2.string_handle->ref_count++;
        CompFlags.non_zero_data = true;
        break;
    case OPR_PUSHADDR:
        info->state = IS_ADDR;
        /* fall through */
    case OPR_PUSHSYM:
        if( info->addr_set ) {
            info->is_error = true;
        }
        info->addr_set = true;
        info->is_str = false;
        SymGet( &sym, tree->op.u2.sym_handle );
        info->u.sym_h = tree->op.u2.sym_handle;
        CompFlags.non_zero_data = true;
        if( sym.attribs.stg_class == SC_AUTO ) {
            info->is_error = true;
        }
        break;
    case OPR_INDEX:
        if( tree->right->op.opr == OPR_PUSHINT ) {
            AddrFold( tree->left, info );
            offset = tree->right->op.u2.long_value;
        } else if( tree->left->op.opr == OPR_PUSHINT ) {
            AddrFold( tree->right, info );
            offset = tree->left->op.u2.long_value;
        } else {
            info->is_error = true;
        }
        if( info->state == IS_VALUE ) { // must be foldable
            info->is_error = true;
        }
        info->offset += offset * SizeOfArg( tree->u.expr_type );
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_ADD:
    case OPR_SUB:
        if( tree->right->op.opr == OPR_PUSHINT ) {
            AddrFold( tree->left, info );
            if( tree->op.opr == OPR_ADD ) {
                info->offset = info->offset + tree->right->op.u2.long_value;
            } else {
                info->offset = info->offset - tree->right->op.u2.long_value;
            }
        } else if( tree->left->op.opr == OPR_PUSHINT ) {
            AddrFold( tree->right, info );
            if( tree->op.opr == OPR_ADD ) {
                info->offset = tree->left->op.u2.long_value + info->offset;
            } else {
                info->offset = tree->left->op.u2.long_value - info->offset;
            }
        } else {
            info->is_error = true;
        }
        if( info->state == IS_VALUE ) { // must be foldable
            info->is_error = true;
        }
        break;
    case OPR_ADDROF:
        AddrFold( tree->right, info );
        info->state = IS_ADDR;
        CompFlags.non_zero_data = true;
        break;
    case OPR_ARROW:
        AddrFold( tree->left, info );
        if( info->state == IS_VALUE ) { // must be foldable
            info->is_error = true;
        }
        info->offset += tree->right->op.u2.long_value;
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_POINTS:
        AddrFold( tree->left, info );
        if( info->state == IS_VALUE ) { // must be foldable
            info->is_error = true;
        }
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_DOT:
        AddrFold( tree->left, info );
        info->offset += tree->right->op.u2.long_value;
        CompFlags.non_zero_data = true;
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_CONVERT:   // should check for pointer to smaller
    case OPR_CONVERT_PTR:
        AddrFold( tree->right, info );
        break;
    case OPR_ERROR:     // error has already been issued
        break;
    default:
        info->is_error = true;
        break;
    }
}

static void StorePointer( TYPEPTR typ, target_size size )
{
    TREEPTR             tree;
    TYPEPTR             typ2;
    DATA_QUAD           dq;
    addrfold_info       info;

    dq.flags = Q_DATA;
    if( typ->decl_type == TYP_POINTER ) {
        if( typ->u.p.decl_flags & (FLAG_FAR | FLAG_HUGE) ) {
            dq.flags |= Q_FAR_POINTER;
        } else if( typ->u.p.decl_flags & FLAG_NEAR ) {
            dq.flags |= Q_NEAR_POINTER;
        } else {
            typ2 = typ->object;
            SKIP_TYPEDEFS( typ2 );
            if( typ2->decl_type == TYP_FUNCTION ) {
                dq.flags |= Q_CODE_POINTER;
            } else if( TypeSize( typ ) == TARGET_FAR_POINTER ) {
                dq.flags |= Q_FAR_POINTER;
            }
        }
    }

    dq.type = QDT_ID;
    dq.u.var.sym_handle = SYM_NULL;
    dq.u.var.offset = 0;
    if( CurToken != T_RIGHT_BRACE ) {
        tree = AddrExpr();
        tree = InitAsgn( typ, tree ); // as if we are assigning
        info.offset = 0;
        info.u.sym_h = SYM_NULL;
        info.addr_set = false;
        info.state = IS_VALUE;
        info.is_str = false;
        info.is_error = false;
        AddrFold( tree, &info );
        if( info.state == IS_VALUE ) { // must be foldable  into addr+offset
            info.is_error = true;
        }
        if( info.is_str ) { // need to fix cgen an DATAQUADS to handle str+off
            if( info.offset != 0 ) {
                info.is_error = true;
            }
        }
        if( info.is_error ) {
            if( tree->op.opr != OPR_ERROR ) {
                CErr1( ERR_NOT_A_CONSTANT_EXPR );
            }
        } else {
            if( info.is_str ) {
                dq.type = QDT_STRING;
                dq.u.string_leaf = info.u.str_h;
            } else {
                dq.u.var.sym_handle = info.u.sym_h;
                dq.u.var.offset = info.offset;
            }
        }
        FreeExprTree( tree );
    }
    if( typ->decl_type == TYP_POINTER ) {
        GenDataQuad( &dq, size );
    } else if( dq.type == QDT_STRING ) {
        if( TypeSize( typ ) != DataPtrSize
          || CompFlags.strict_ANSI ) {
            CErr1( ERR_INVALID_INITIALIZER );
        }
        GenDataQuad( &dq, size );
    } else { /* dq.type == QDT_ID */
        if( dq.u.var.sym_handle != SYM_NULL ) {
            if( TypeSize( typ ) != DataPtrSize ) {
                CErr1( ERR_INVALID_INITIALIZER );
            }
            GenDataQuad( &dq, size );
        } else {
            StoreIValue( typ->decl_type, dq.u.var.offset, size );
        }
    }
}

static void StoreInt64( TYPEPTR typ )
{
    TREEPTR     tree;
    DATA_QUAD   dq;

    dq.type = (enum quad_type)typ->decl_type;
    dq.flags = Q_DATA;
    U32ToU64( 0, &dq.u.long64 );
    if( CurToken != T_RIGHT_BRACE ) {
        tree = SingleExpr();
        tree = InitAsgn( typ, tree ); // as if we are assigning
        if( IsConstLeaf( tree ) ) {
            CastConstValue( tree, typ->decl_type );
            dq.u.long64 = tree->op.u2.ulong64_value;
        } else {
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
        }
        FreeExprTree( tree );
        CompFlags.non_zero_data = true;
    }
    GenDataQuad( &dq, TARGET_LONG64 );
}

static void LoadBitField( uint64 *val64 )
{
    DATA_QUAD           *dqp;

    val64->u._32[I64LO32] = 0;
    val64->u._32[I64HI32] = 0;
    if( CurDataQuad->next == NULL )
        return;
    dqp = &CurDataQuad->next->dq;
    if( dqp->type == QDT_CONSTANT )
        return;
    if( CurDataQuad->next->size == TARGET_LONG64 ) {
        val64->u._32[I64LO32] = dqp->u.long64.u._32[I64LO32];
        val64->u._32[I64HI32] = dqp->u.long64.u._32[I64HI32];
    } else {
        val64->u._32[I64LO32] = dqp->u.ulong_values[0];
    }
}

static void ResetBitField( uint64 *val64, unsigned start, unsigned width )
{
    uint64      mask;

    /*
     * mask = ( 1 << width ) - 1;
     */
    mask.u._32[I64LO32] = 1;
    mask.u._32[I64HI32] = 0;
    U64ShiftL( &mask, width, &mask );
    if( mask.u._32[I64LO32] == 0 )
        mask.u._32[I64HI32]--;
    mask.u._32[I64LO32]--;
    U64ShiftL( &mask, start, &mask );

    val64->u._32[I64HI32] &= ~mask.u._32[I64HI32];
    val64->u._32[I64LO32] &= ~mask.u._32[I64LO32];
}

static void InitBitField( FIELDPTR field )
{
    TYPEPTR             typ;
    target_size         size;
    uint64              value64;
    const_val           bit_value;
    TOKEN               token;
    bool                is64bit;
    DATA_TYPE           dtype;
    unsigned            width;

    token = CurToken;
    if( CurToken == T_LEFT_BRACE )
        NextToken();
    typ = field->field_type;
    size = SizeOfArg( typ );
    dtype = typ->u.f.field_type;
    is64bit = ( dtype == TYP_LONG64 || dtype == TYP_ULONG64 );
    LoadBitField( &value64 );
    if( typ->u.f.field_type == TYP_BOOL ) {
        width = 1;
    } else {
        width = typ->u.f.field_width;
    }
    ResetBitField( &value64, typ->u.f.field_start, width );
    if( CurToken != T_RIGHT_BRACE ) {
        if( ConstExprAndType( &bit_value ) ) {
            if( CheckAssignBits( &bit_value.value, width, true ) ) {
                CWarn1( ERR_CONSTANT_TOO_BIG );
            }
            U64ShiftL( &bit_value.value, typ->u.f.field_start, &bit_value.value );
            value64.u._32[I64LO32] |= bit_value.value.u._32[I64LO32];
            value64.u._32[I64HI32] |= bit_value.value.u._32[I64HI32];
        }
    }
    if( is64bit ) {
        StoreIValue64( dtype, value64 );
    } else {
        StoreIValue( dtype, value64.u._32[I64LO32], size );
    }
    if( token == T_LEFT_BRACE ) {
        if( CurToken == T_COMMA )
            NextToken();
        MustRecog( T_RIGHT_BRACE );
    }
}


static void *DesignatedInit( TYPEPTR typ, TYPEPTR ctyp, void *field )
/********************************************************************
 * Detects a C99 designated initializer
 *
 * typ is the (perhaps subaggregate) object that is initialized.
 * ctyp refers to the current object which is the object
 * associated with the closest brace pair. It may or may not be
 * equal to typ
 */
{
    TREEPTR         tree;
    target_size     offs;
    static bool     new_field = true;

    if( !CompFlags.extensions_enabled
      && ( CompVars.cstd < STD_C99 ) ) {
        return( field );
    }

    if( CurToken != T_LEFT_BRACKET
      && CurToken != T_DOT ) {
        new_field = true;
        return( field );
    }
    /*
     * if designator refers to outer type: back out
     */
    if( typ != ctyp
      && new_field )
        return( NULL );

    new_field = false;
    if( typ->decl_type == TYP_ARRAY ) {
        if( CurToken != T_LEFT_BRACKET )
            return( NULL );
        NextToken();
        tree = SingleExpr();
        if( IsConstLeaf( tree ) ) {
            CastConstValue( tree, typ->decl_type );
            *(target_size *)field = tree->op.u2.ulong_value;
        } else {
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
        }
        FreeExprTree( tree );
        MustRecog( T_RIGHT_BRACKET );
    } else {
        if( CurToken != T_DOT )
            return( NULL );
        NextToken();
        if( CurToken != T_ID ) {
            CErr1( ERR_EXPECTING_ID );
        }
        offs = 0;
        field = SearchFields( &typ, &offs, Buffer );
        if( field == NULL ) {
            CErr3p( ERR_NAME_NOT_FOUND_IN_STRUCT, Buffer, typ->u.tag->name );
        }
        NextToken();
    }

    if( CurToken != T_LEFT_BRACKET
      && CurToken != T_DOT ) {
        new_field = true;
        MustRecog( T_EQUAL );
    }
    return( field );
}

static bool DesignatedInSubAggregate( DATA_TYPE decl_type )
{
    switch( decl_type ) {
    case TYP_ARRAY:
    case TYP_STRUCT:
    case TYP_UNION:
    case TYP_FCOMPLEX:
    case TYP_DCOMPLEX:
    case TYP_LDCOMPLEX:
        /*
         * A subaggregate can be stopped by a designated initializer.
         * in that case the comma was already eaten...
         */
        return( CurToken == T_DOT || CurToken == T_LEFT_BRACKET );
    default:
        return( false );
    }
}

static void InitArray( TYPEPTR typ, TYPEPTR ctyp )
{
    target_size n;
    target_size m;
    target_size *pm;
    target_size array_size;
    target_size elem_size;

    array_size = TypeSize( typ );
    n = 0;
    pm = &m;
    for( ;; ) {
        m = n;
        pm = DesignatedInit( typ, ctyp, pm );
        if( pm == NULL )
            break;
        if( m != n ) {
            elem_size = SizeOfArg( typ->object );
            if( typ->u.array->unspecified_dim
              && m > array_size ) {
                RelSeekBytes( ( array_size - n ) * elem_size );
                ZeroBytes( ( m - array_size ) * elem_size );
            } else {
                RelSeekBytes( ( m - n ) * elem_size );
            }
            n = m;
        }
        n++;
        if( n > array_size ) {
            if( !typ->u.array->unspecified_dim )
                break;
            array_size = n;
            /*
             * clear out the new element just in case
             */
            elem_size = SizeOfArg( typ->object );
            ZeroBytes( elem_size );
            RelSeekBytes( -(target_ssize)elem_size );
        }
        InitSymData( typ->object, ctyp, 1 );
        if( CurToken == T_EOF )
            break;
        if( CurToken == T_RIGHT_BRACE )
            break;
        if( DesignatedInSubAggregate( typ->object->decl_type ) )
            continue;
        if( n < array_size
          || typ == ctyp
          || typ->u.array->unspecified_dim ) {
            MustRecog( T_COMMA );
        }
        if( CurToken == T_RIGHT_BRACE ) {
            break;
        }
    }
    if( typ->u.array->unspecified_dim ) {
        typ->u.array->dimension = array_size;
    }
    if( array_size > n ) {
        RelSeekBytes( ( array_size - n ) * SizeOfArg( typ->object ) );
    }
}

static void InitStructUnion( TYPEPTR typ, TYPEPTR ctyp, FIELDPTR field )
/***********************************************************************
 * Initialize struct or union fields
 * Detects a C99 designated initializer for fields
 */
{
    TYPEPTR         ftyp;
    target_size     n;
    target_size     offset;

    /*
     * get full size of the struct or union
     */
    n = typ->u.tag->size;
    offset = 0;
    for( ;; ) {
        field = DesignatedInit( typ, ctyp, field );
        if( field == NULL )
            break;
        /*
         * The first field might not start at offset 0
         */
        if( field->offset != offset ) {
            RelSeekBytes( field->offset - offset );
        }
        ftyp = field->field_type;
        offset = field->offset + SizeOfArg( ftyp );
        if( ftyp->decl_type == TYP_FIELD
          || ftyp->decl_type == TYP_UFIELD ) {
            InitBitField( field );
        } else {
            InitSymData( ftyp, ctyp, 1 );
        }
        field = field->next_field;
        if( typ->decl_type == TYP_UNION ) {
            if( offset < n ) {
                ZeroBytes( n - offset );    /* pad the rest */
            }
            offset = n;
            /*
             * designated initializers may still override this field
             */
            field = NULL;
        }
        if( CurToken == T_EOF )
            break;
        if( CurToken == T_RIGHT_BRACE )
            break;
        if( DesignatedInSubAggregate( ftyp->decl_type ) )
            continue;
        if( field != NULL
          || typ == ctyp ) {
            MustRecog( T_COMMA );
        }
        if( CurToken == T_RIGHT_BRACE ) {
            break;
        }
    }
    RelSeekBytes( n - offset );
}

static void InitStruct( TYPEPTR typ, TYPEPTR ctyp )
{
    InitStructUnion( typ, ctyp, typ->u.tag->u.field_list );
}

static void InitUnion( TYPEPTR typ, TYPEPTR ctyp )
{
    FIELDPTR            field;
    TYPEPTR             ftyp;

    /*
     * skip unnamed bit fields
     */
    for( field = typ->u.tag->u.field_list; field != NULL; field = field->next_field ) {
        ftyp = field->field_type;
        SKIP_TYPEDEFS( ftyp );
        if( field->name[0] != '\0' )
            break;
        if( ftyp->decl_type == TYP_STRUCT )
            break;
        if( ftyp->decl_type == TYP_UNION ) {
            break;
        }
    }
    InitStructUnion( typ, ctyp, field );
}

void InitSymData( TYPEPTR typ, TYPEPTR ctyp, int level )
{
    TOKEN           token;
    target_size     size;

    token = CurToken;
    if( CurToken == T_LEFT_BRACE ) {
        NextToken();
        if( CurToken == T_RIGHT_BRACE
          || CurToken == T_COMMA ) {
            CErr1( ERR_EMPTY_INITIALIZER_LIST );
        }
    }
    /*
     * skip typedefs, go into enum base
     */
    typ = SkipTypeFluff( typ );
    size = SizeOfArg( typ );
    switch( typ->decl_type ) {
    case TYP_ARRAY:
        if( CharArray( typ->object ) ) {
            InitCharArray( typ );
        } else if( WCharArray( typ->object ) ) {
            InitWCharArray( typ );
        } else {
            if( token == T_LEFT_BRACE ) {
                ctyp = typ;
            } else if( level == 0 ) {
                CErr1( ERR_NEED_BRACES );
            }
            if( typ == ctyp ) {
                /*
                 * initialize new current type
                 *
                 * first zero out the whole array; otherwise
                 * overlapping fields caused by designated
                 * initializers will make life very difficult
                 */
                ZeroBytes( size );
                RelSeekBytes( -(target_ssize)size );
            }
            InitArray( typ, ctyp );
        }
        break;
    case TYP_FCOMPLEX:
    case TYP_DCOMPLEX:
    case TYP_LDCOMPLEX:
    case TYP_STRUCT:
        if( token == T_LEFT_BRACE ) {
            ctyp = typ;
        } else if( level == 0 ) {
            CErr1( ERR_NEED_BRACES );
        }
        if( typ == ctyp ) {
            /*
             * initialize new current type
             *
             * zero out all fields; otherwise overlapping fields caused
             * by designated initializers will make life very difficult
             */
            ZeroBytes( size );
            RelSeekBytes( -(target_ssize)size );
        }
        InitStruct( typ, ctyp );
        break;
    case TYP_UNION:
        if( token == T_LEFT_BRACE ) {
            ctyp = typ;
        } else if( level == 0 ) {
            CErr1( ERR_NEED_BRACES );
        }
        InitUnion( typ, ctyp );
        break;
    case TYP_CHAR:
    case TYP_UCHAR:
    case TYP_BOOL:
    case TYP_SHORT:
    case TYP_USHORT:
    case TYP_INT:
    case TYP_UINT:
    case TYP_LONG:
    case TYP_ULONG:
    case TYP_POINTER:
        StorePointer( typ, size );
        break;
    case TYP_LONG64:
    case TYP_ULONG64:
        StoreInt64( typ );
        break;
    case TYP_FLOAT:
    case TYP_DOUBLE:
    case TYP_FIMAGINARY:
    case TYP_DIMAGINARY:
        StoreFloat( typ->decl_type, size );
        break;
    case TYP_LONG_DOUBLE:
    case TYP_LDIMAGINARY:
//        StoreFloat( typ->decl_type, size );
        StoreFloat( TYP_DOUBLE, size );
        break;
    default:
        break;
    }
    if( token == T_LEFT_BRACE ) {
        if( CurToken == T_COMMA ) {
            NextToken();
        }
        if( CurToken != T_RIGHT_BRACE ) {
            CErr1( ERR_TOO_MANY_INITS );
        }
        while( CurToken != T_RIGHT_BRACE ) {
            if( CurToken == T_EOF )
                break;
            if( CurToken == T_SEMI_COLON )
                break;
            if( CurToken == T_LEFT_BRACE )
                break;
            NextToken();
        }
        MustRecog( T_RIGHT_BRACE );
    }
}


static bool CharArray( TYPEPTR typ )
{
    if( CurToken == T_STRING ) {
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYP_CHAR
          || typ->decl_type == TYP_UCHAR ) {
            return( true );
        }
    }
    return( false );
}


static bool WCharArray( TYPEPTR typ )
{
    if( CurToken == T_STRING ) {
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYP_SHORT
          || typ->decl_type == TYP_USHORT ) {
            return( true );
        }
    }
    return( false );
}

static void InitCharArray( TYPEPTR typ )
/***************************************
 * This function handles the initialization of statements like:
 * char  name[4] = "abcd";
 */
{
    target_size         len;
    STR_HANDLE          str_lit;
    target_size         size;
    DATA_QUAD           dq;

    str_lit = GetLiteral();
    if( CompFlags.wide_char_string )
        CErr1( ERR_TYPE_MISMATCH );
    len = str_lit->length;
    if( typ->u.array->unspecified_dim )
        typ->u.array->dimension = len;
    size = typ->u.array->dimension;
    if( len > size ) {
        if( ( len - size ) > 1 ) {
            CWarn1( ERR_LIT_TOO_LONG );
        }
        /*
         * chop the string
         */
        len = size;
        str_lit->length = size;
    }
    dq.type = QDT_CONST;
    dq.flags = Q_DATA;
    dq.u.string_leaf = str_lit;
    GenDataQuad( &dq, len );
    if( size > len ) {                  /* if array > len of literal */
        ZeroBytes( size - len );
    }
    CompFlags.non_zero_data = true;
}


static void InitWCharArray( TYPEPTR typ )
/****************************************
 * This function handles the initialization of statements like:
 * wchar_t  name[5] = L"abcd";
 */
{
    target_size         len;
    target_size         i;
    STR_HANDLE          str_lit;
    unsigned short      value;
    unsigned short      *pwc;
    target_size         size;
    DATA_QUAD           dq;

    dq.type = QDT_SHORT;
    dq.flags = Q_DATA;
    str_lit = GetLiteral();
    if( !CompFlags.wide_char_string )
        CErr1( ERR_TYPE_MISMATCH );
    len = str_lit->length / sizeof( unsigned short );
    if( typ->u.array->unspecified_dim ) {
        typ->u.array->dimension = len;
    }
    size = typ->u.array->dimension;
    if( len > size ) {
        if( ( len - size ) > 1 ) {
            CWarn1( ERR_LIT_TOO_LONG );
        }
        len = size;
    }
    pwc = (unsigned short *)str_lit->literal;
    for( i = 0; i < len; ++i ) {
        value = *pwc++;
        if( value != 0 )
            CompFlags.non_zero_data = true;
        dq.u_long_value1 = value;
        GenDataQuad( &dq, TARGET_WCHAR );
    }
    if( i < size ) {
        ZeroBytes( ( size - i ) * sizeof( unsigned short ) );
    }
    FreeLiteral( str_lit );
}


static void StoreFloat( DATA_TYPE dtype, target_size size )
{
    TREEPTR     tree;
    DATA_QUAD   dq;

    dq.type = (enum quad_type)dtype;
    dq.flags = Q_DATA;
    dq.u.double_value = 0.0;
    if( CurToken != T_RIGHT_BRACE ) {
        tree = SingleExpr();
        if( IsConstLeaf( tree ) ) {
            CastConstValue( tree, dtype );
#ifdef _LONG_DOUBLE_
            {
                long_double ld;

                ld = tree->op.u2.float_value->ld;
                __iLDFD( &ld, &dq.u.double_value );
            }
#else
            dq.u.double_value = tree->op.u2.float_value->ld.u.value;
#endif
        } else {
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
        }
        FreeExprTree( tree );
        if( dq.u.double_value != 0.0 ) {
            CompFlags.non_zero_data = true;
        }
    }
    GenDataQuad( &dq, size );
}

static void GenStaticDataQuad( SYM_HANDLE sym_handle )
{
    DATA_QUAD       dq;

    dq.type = QDT_STATIC;
    dq.flags = Q_DATA;
    dq.u.var.sym_handle = sym_handle;
    dq.u.var.offset = 0;
    GenDataQuad( &dq, 0 );
}

void StaticInit( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TYPEPTR             typ;
    TYPEPTR             struct_typ;
    TYPEPTR             last_array;

    GenStaticDataQuad( sym_handle );
    CompFlags.non_zero_data = false;
    struct_typ = NULL;
    last_array = NULL;
    typ = sym->sym_type;
    /*
     * Follow chain of typedefs/structs/arrays
     */
    for( ;; ) {
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYP_ARRAY ) {
            /*
             * Remember innermost array type
             */
            last_array = typ;
            typ = typ->object;
        } else if( typ->decl_type == TYP_STRUCT ) {
            FIELDPTR    field;

            /*
             * Remember outermost structure type
             */
            if( struct_typ == NULL ) {
                /*
                 * last_array cannot to be outside this struct!
                 */
                last_array = NULL;
                struct_typ = typ;
            }
            /*
             * Determine the type of the last field in the struct
             */
            field = typ->u.tag->u.field_list;
            if( field == NULL )
                break;
            while( field->next_field != NULL )
                field = field->next_field;
            typ = field->field_type;
        } else {
            break;
        }
    }
    typ = last_array;
    /*
     * If innermost array had unspecified dimension, create new types whose
     * dimensions will be determined by number of initializers
     */
    if( (typ != NULL)
      && typ->u.array->unspecified_dim ) {
        if( struct_typ == NULL ) {
            /*
             * Array was not inside struct
             */
            sym->sym_type = ArrayNode( typ->object );
            sym->sym_type->u.array->unspecified_dim = true;
        } else {
            typ = sym->sym_type;
            /*
             * Create new structure type
             */
            sym->sym_type = TypeNode( TYP_STRUCT, ArrayNode( last_array->object ) );
            sym->sym_type->u.tag = struct_typ->u.tag;
            struct_typ = sym->sym_type;
            /*
             * Create new array types as necessary
             */
            for( ;; ) {
                SKIP_TYPEDEFS( typ );
                if( typ->decl_type != TYP_ARRAY )
                    break;
                sym->sym_type = ArrayNode( sym->sym_type );
                sym->sym_type->u.array->unspecified_dim = true;
                typ = typ->object;
            }
            typ = last_array;
        }
    } else {
        struct_typ = NULL;
    }
    SymReplace( sym, sym_handle );
    InitSymData( sym->sym_type, sym->sym_type, 0 );
    SymGet( sym, sym_handle );
    if( struct_typ != NULL ) {
        /*
         * Structure contains an unspecified length array as last field
         */
        struct_typ->object->u.array->dimension = typ->u.array->dimension;
        typ->u.array->unspecified_dim = true;
        typ->u.array->dimension = 0;    /* Reset back to 0 */
    }
    if( sym->u.var.segid == SEG_NULL ) {
        SetFarHuge( sym, false );
        SetSegment( sym );
        SetSegAlign( sym );
    }
}

static void AssignAggregate( TREEPTR lvalue, TREEPTR rvalue, TYPEPTR typ )
{
    TREEPTR tree;

    tree = ExprNode( lvalue, OPR_EQUALS, rvalue );
    tree->right->op.opr = OPR_PUSHSYM;
    tree->u.expr_type = typ;
    tree->op.u2.result_type = typ;
    AddStmt( tree );
}

static void AggregateVarDeclEquals( SYMPTR sym, SYM_HANDLE sym_handle )
{
    SYM_HANDLE  sym2_handle;
    SYM_ENTRY   sym2;

    sym2_handle = MakeNewDotSym( &sym2, 'X', sym->sym_type, SC_STATIC );
    sym2.flags |= SYM_INITIALIZED;
    CompFlags.initializing_data = true;
    StaticInit( &sym2, sym2_handle );
    CompFlags.initializing_data = false;
    SymReplace( &sym2, sym2_handle );
    /*
     * StaticInit will change sym2.sym_type if it is an incomplete array type
     */
    sym->sym_type = sym2.sym_type;
    AssignAggregate( VarLeaf( sym, sym_handle ),
                     VarLeaf( &sym2, sym2_handle ), sym->sym_type );
}

static void InitStructField( SYMPTR sym, SYM_HANDLE sym_handle, target_size base, FIELDPTR field, TREEPTR value )
{
    TREEPTR     opnd;
    TYPEPTR     typ;
    FIELDPTR    ufield;

    typ = field->field_type;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYP_UNION ) {
        ufield = typ->u.tag->u.field_list;
        typ = ufield->field_type;
        SKIP_TYPEDEFS( typ );
    }
    opnd = VarLeaf( sym, sym_handle );
    opnd = ExprNode( opnd, OPR_DOT, UIntLeaf( base + field->offset ) );
    opnd->u.expr_type = typ;
    opnd->op.u2.result_type = typ;
    AddStmt( AsgnOp( opnd, T_ASSIGN_LAST, value ) );
}

static void InitStructUnionVarZero( SYMPTR sym, SYM_HANDLE sym_handle, int index, TYPEPTR typ )
{
    FIELDPTR    field;
    target_size base;

    base = index * SizeOfArg( typ );
    for( field = typ->u.tag->u.field_list; field != NULL; field = field->next_field ) {
        InitStructField( sym, sym_handle, base, field, IntLeaf( 0 ) );
    }
}

static void InitStructUnionVar( SYMPTR sym, SYM_HANDLE sym_handle, int index, TYPEPTR typ )
{
    TREEPTR     value;
    FIELDPTR    field;
    FIELDPTR    new_field;
    FIELDPTR    last_field;
    TOKEN       token;
    target_size base;

    base = index * SizeOfArg( typ );
    last_field = typ->u.tag->u.field_list;
    for( field = typ->u.tag->u.field_list; field != NULL; field = field->next_field ) {
        new_field = DesignatedInit( typ, typ, field );
        if( new_field != NULL ) {
            for( field = last_field->next_field; field != NULL; field = field->next_field ) {
                 if( field->offset >= new_field->offset  )
                     break;
                 InitStructField( sym, sym_handle, base, field, IntLeaf( 0 ) );
            }
            field = new_field;
        }
        token = CurToken;
        if( token == T_LEFT_BRACE ) // allow {}, and extra {expr}..}
            NextToken();
        if( CurToken == T_RIGHT_BRACE ) {
            value = IntLeaf( 0 );
        } else {
            value = CommaExpr();
        }
        InitStructField( sym, sym_handle, base, field, value );
        if( last_field->offset < field->offset )
            last_field = field;
        if( token == T_LEFT_BRACE )
            MustRecog( T_RIGHT_BRACE );
        if( CurToken == T_EOF )
            break;
        if( CurToken != T_RIGHT_BRACE ) {
            MustRecog( T_COMMA );
        }
        if( field->next_field == NULL ) {
            break;
        }
    }
}

static bool SimpleUnion( TYPEPTR typ )
{
    FIELDPTR    field;

    field = typ->u.tag->u.field_list;
    typ = field->field_type;
    SKIP_TYPEDEFS( typ );
    switch( typ->decl_type ) {
    case TYP_ARRAY:
    case TYP_STRUCT:
    case TYP_UNION:
    case TYP_FIELD:
    case TYP_UFIELD:
        return( false );        // give up on these
    default:
        break;
    }
    return( true );
}

static bool SimpleStruct( TYPEPTR typ )
{
    FIELDPTR    field;

    if( typ->decl_type == TYP_UNION ) {
        return( false );
    }
    for( field = typ->u.tag->u.field_list; field != NULL; field = field->next_field ) {
        typ = field->field_type;
        SKIP_TYPEDEFS( typ );
        switch( typ->decl_type ) {
        case TYP_UNION:
            if( SimpleUnion( typ ) ) {
                break;              // go 1 deep to get by MFC examples
            }
            /* fall through */
        case TYP_ARRAY:
        case TYP_STRUCT:
        case TYP_FIELD:
        case TYP_UFIELD:
            return( false );        // give up on these
        default:
            break;
        }
    }
    return( true );
}

static void InitArraySimpleVar( SYMPTR sym, SYM_HANDLE sym_handle, TYPEPTR typ, int index, TREEPTR value )
{
    TREEPTR     opnd;

    opnd = VarLeaf( sym, sym_handle );
    opnd = ExprNode( opnd, OPR_INDEX, IntLeaf( index ) );
    opnd->u.expr_type = typ;
    opnd->op.u2.result_type = typ;
    AddStmt( AsgnOp( opnd, T_ASSIGN_LAST, value ) );
}

static void InitArrayVar( SYMPTR sym, SYM_HANDLE sym_handle, TYPEPTR typ )
{
    target_size i;
    target_size n;
    TYPEPTR     typ2;
    SYM_HANDLE  sym2_handle;
    SYM_ENTRY   sym2;
    TOKEN       token;
    target_size j;
    target_size dim;

    typ2 = typ->object;
    SKIP_TYPEDEFS( typ2 );
    switch( typ2->decl_type ) {
    case TYP_CHAR:
    case TYP_UCHAR:
    case TYP_SHORT:
    case TYP_USHORT:
    case TYP_INT:
    case TYP_UINT:
    case TYP_LONG:
    case TYP_ULONG:
    case TYP_LONG64:
    case TYP_ULONG64:
    case TYP_FLOAT:
    case TYP_DOUBLE:
    case TYP_POINTER:
    case TYP_LONG_DOUBLE:
    case TYP_FIMAGINARY:
    case TYP_DIMAGINARY:
    case TYP_LDIMAGINARY:
    case TYP_BOOL:
        /*
         * skip over T_LEFT_BRACE
         */
        NextToken();
        if( CharArray( typ->object ) ) {
            sym2_handle = MakeNewDotSym( &sym2, 'X', typ, SC_STATIC );
            sym2.flags |= SYM_INITIALIZED;
            if( sym2.u.var.segid == SEG_NULL ) {
                SetFarHuge( &sym2, false );
                SetSegment( &sym2 );
                SetSegAlign( &sym2 );
            }
            SymReplace( &sym2, sym2_handle );
            GenStaticDataQuad( sym2_handle );
            InitCharArray( typ );
            AssignAggregate( VarLeaf( sym, sym_handle ),
                             VarLeaf( &sym2, sym2_handle ), typ );
        } else if( WCharArray( typ->object ) ) {
            sym2_handle = MakeNewDotSym( &sym2, 'X', typ, SC_STATIC );
            sym2.flags |= SYM_INITIALIZED;
            if( sym2.u.var.segid == SEG_NULL ) {
                SetFarHuge( &sym2, false );
                SetSegment( &sym2 );
                SetSegAlign( &sym2 );
            }
            SymReplace( &sym2, sym2_handle );
            GenStaticDataQuad( sym2_handle );
            InitWCharArray( typ );
            AssignAggregate( VarLeaf( sym, sym_handle ),
                             VarLeaf( &sym2, sym2_handle ), typ );
        } else {
            n = typ->u.array->dimension;
            dim = 0;
            i = 0;
            for( ;; ) {     // accept some C++ { {1},.. }
                if( DesignatedInit( typ, sym->sym_type, &i ) != NULL ) {
                    for( j = dim; j < i; j++ ) {
                        InitArraySimpleVar( sym, sym_handle, typ2, j, IntLeaf( 0 ) );
                    }
                }
                token = CurToken;
                if( token == T_LEFT_BRACE )
                    NextToken();
                InitArraySimpleVar( sym, sym_handle, typ2, i, CommaExpr() );
                if( token == T_LEFT_BRACE )
                    MustRecog( T_RIGHT_BRACE );
                ++i;
                if( dim < i )
                    dim = i;
                if( CurToken == T_EOF )
                    break;
                if( CurToken == T_RIGHT_BRACE )
                    break;
                MustRecog( T_COMMA );
                if( CurToken == T_RIGHT_BRACE )
                    break;
                if( n
                  && i >= n ) {
                    CErr1( ERR_TOO_MANY_INITS );
                }
            }
            if( typ->u.array->unspecified_dim ) {
                typ->u.array->dimension = dim;
            } else {
                for( i = dim; i < n; i++ ) {
                    InitArraySimpleVar( sym, sym_handle, typ2, i, IntLeaf( 0 ) );
                }
            }
        }
        MustRecog( T_RIGHT_BRACE );
        break;
    case TYP_FCOMPLEX:
    case TYP_DCOMPLEX:
    case TYP_LDCOMPLEX:
    case TYP_STRUCT:
    case TYP_UNION:
        if( SimpleStruct( typ2 ) ) {
            /*
             * skip over T_LEFT_BRACE
             */
            NextToken();
            n = typ->u.array->dimension;
            dim = 0;
            i = 0;
            for( ;; ) {
                if( DesignatedInit( typ, typ, &i ) != NULL ) {
                    for( j = dim; j < i; j++ ) {
                        InitStructUnionVarZero( sym, sym_handle, j, typ2 );
                    }
                }
                token = CurToken;
                if( token == T_LEFT_BRACE ) {
                    NextToken();
                }
                InitStructUnionVar( sym, sym_handle, i, typ2 );
                if( token == T_LEFT_BRACE ) {
                    MustRecog( T_RIGHT_BRACE );
                }
                ++i;
                if( dim < i )
                    dim = i;
                if( CurToken == T_EOF )
                    break;
                if( CurToken == T_RIGHT_BRACE )
                    break;
                MustRecog( T_COMMA );
                if( CurToken == T_RIGHT_BRACE )
                    break;
                if( n
                  && i >= n ) {
                    CErr1( ERR_TOO_MANY_INITS );
                }
            }
            if( typ->u.array->unspecified_dim ) {
                typ->u.array->dimension = dim;
            } else {
                for( i = dim; i < n; i++ ) { // mop up
                    InitStructUnionVarZero( sym, sym_handle, i, typ2 );
                }
            }
            /*
             * skip over T_RIGHT_BRACE
             */
            NextToken();
            break;
        }
        /* fall through */
    default:
        AggregateVarDeclEquals( sym, sym_handle );
        break;
    }
}

void VarDeclEquals( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TYPEPTR     typ;

    if( SymLevel == 0
      || sym->attribs.stg_class == SC_STATIC ) {
        if( sym->flags & SYM_INITIALIZED ) {
            CErrSymName( ERR_VAR_ALREADY_INITIALIZED, sym, sym_handle );
        }
        sym->flags |= SYM_INITIALIZED;
        CompFlags.initializing_data = true;
        StaticInit( sym, sym_handle );
        CompFlags.initializing_data = false;
    } else {
        SymReplace( sym, sym_handle );
        SrcLoc = TokenLoc;
        typ = sym->sym_type;
        SKIP_TYPEDEFS( typ );
        /*
         * check for { before checking for array, struct or union
         */
        if( CurToken != T_LEFT_BRACE
          && typ->decl_type != TYP_ARRAY ) {
            AddStmt( AsgnOp( VarLeaf( sym, sym_handle ), T_ASSIGN_LAST, CommaExpr() ) );
            sym->flags |= SYM_ASSIGNED;
        } else if( typ->decl_type == TYP_ARRAY ) {
            if( CurToken == T_LEFT_BRACE
              && CompFlags.auto_agg_inits ) {
                InitArrayVar( sym, sym_handle, typ );
            } else {
                AggregateVarDeclEquals( sym, sym_handle );
            }
        } else if( typ->decl_type == TYP_STRUCT
          || typ->decl_type == TYP_UNION ) {
            if( CurToken == T_LEFT_BRACE
              && CompFlags.auto_agg_inits
              && SimpleStruct( typ ) ) {
                NextToken();    /* skip T_LEFT_BRACE */
                InitStructUnionVar( sym, sym_handle, 0, typ );
                NextToken();    /* skip T_RIGHT_BRACE */
            } else {
                AggregateVarDeclEquals( sym, sym_handle );
            }
        } else {
            NextToken();
            AddStmt( AsgnOp( VarLeaf( sym, sym_handle ), T_ASSIGN_LAST, CommaExpr() ) );
            sym->flags |= SYM_ASSIGNED;
            MustRecog( T_RIGHT_BRACE );
        }
    }
}
