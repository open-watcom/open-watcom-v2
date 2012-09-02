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
* Description:  Initialization of data ( e.g. int foo[] = { 0, 1, 2 }; )
*
****************************************************************************/


#include "cvars.h"
#include <limits.h>
#include "i64.h"


local unsigned long BitMask[] = {
    0x00000001,
    0x00000003,
    0x00000007,
    0x0000000F,
    0x0000001F,
    0x0000003F,
    0x0000007F,
    0x000000FF,
    0x000001FF,
    0x000003FF,
    0x000007FF,
    0x00000FFF,
    0x00001FFF,
    0x00003FFF,
    0x00007FFF,
    0x0000FFFF,
    0x0001FFFF,
    0x0003FFFF,
    0x0007FFFF,
    0x000FFFFF,
    0x001FFFFF,
    0x003FFFFF,
    0x007FFFFF,
    0x00FFFFFF,
    0x01FFFFFF,
    0x03FFFFFF,
    0x07FFFFFF,
    0x0FFFFFFF,
    0x1FFFFFFF,
    0x3FFFFFFF,
    0x7FFFFFFF,
    0xFFFFFFFF
 };

/* use a double-linked list of dataquads to facilitate insertions */
typedef struct data_quad_list {
    DATA_QUAD               dq;
    unsigned long           size;
    struct data_quad_list   *prev, *next;
} DATA_QUAD_LIST;

#define DATA_QUAD_SEG_SIZE      (32 * 1024)
#define DATA_QUADS_PER_SEG      (DATA_QUAD_SEG_SIZE / sizeof( DATA_QUAD_LIST ))

#define MAX_DATA_QUAD_SEGS (LARGEST_DATA_QUAD_INDEX / DATA_QUADS_PER_SEG + 1)

static DATA_QUAD_LIST  *DataQuadSegs[MAX_DATA_QUAD_SEGS];/* segments for data quads*/
static DATA_QUAD_LIST  *CurDataQuad;
static int             DataQuadSegIndex;
static int             DataQuadIndex;

local   DATA_QUAD_LIST  *NewDataQuad( void );
local   int             CharArray( TYPEPTR typ );
local   int             WCharArray( TYPEPTR typ );
local   void            InitCharArray( TYPEPTR typ );
local   void            InitWCharArray( TYPEPTR typ );
local   void            StoreFloat( DATA_TYPE dtype, unsigned long size );
local   void            StoreInt64( TYPEPTR typ );

int DataQuadsAvailable( void )
{
    DATA_QUAD_LIST  *dql = DataQuadSegs[ 0 ];

    return( dql != NULL && dql->next != NULL );
}

void InitDataQuads( void )
{
    DataQuadIndex = DATA_QUADS_PER_SEG;
    DataQuadSegIndex = -1;
    memset( DataQuadSegs, 0, sizeof( DataQuadSegs ) );
    /* put a guard at the start */
    CurDataQuad = NewDataQuad();
    CurDataQuad->prev = NULL;
    CurDataQuad->next = NULL;
    CurDataQuad->size = 0;
}

void FreeDataQuads( void )
{
    unsigned    i;

    for( i = 0; i < MAX_DATA_QUAD_SEGS; i++ ) {
        if( DataQuadSegs[i] == NULL ) break;
        FEfree( DataQuadSegs[i] );
    }
    InitDataQuads();
}

void *StartDataQuadAccess( void )
{
    void    *cur_dqp;

    if( DataQuadsAvailable() ) {
        cur_dqp = CurDataQuad;
        CurDataQuad = DataQuadSegs[ 0 ]->next;
        return( cur_dqp );              // indicate data quads exist
    }
    return( NULL );                     // indicate no data quads
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

local DATA_QUAD_LIST *NewDataQuad( void )
{
    static DATA_QUAD_LIST   *DataQuadPtr;
    DATA_QUAD_LIST          *dql;

    if( DataQuadIndex >= (DATA_QUADS_PER_SEG - 1) ) {
        if( DataQuadSegIndex == MAX_DATA_QUAD_SEGS ) {
            CErr1( ERR_INTERNAL_LIMIT_EXCEEDED );
            CSuicide();
        }
        ++DataQuadSegIndex;
        DataQuadIndex = 0;
        DataQuadPtr = FEmalloc( DATA_QUAD_SEG_SIZE );
        DataQuadSegs[ DataQuadSegIndex ] = DataQuadPtr;
    }
    dql = DataQuadPtr;
    ++DataQuadIndex;
    ++DataQuadPtr;
    return dql;
}

/* splits the dataquad pointed to by dql so that the current one
   will have size "size" and the new one "oldsize - size" */
local void SplitDataQuad( DATA_QUAD_LIST *dql, unsigned long size )
{
    DATA_QUAD_LIST  *ndql;
    DATA_QUAD       *ndq;
    DATA_QUAD       *dq;
    unsigned long   oldsize;

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
            ndq->u.long_values[0] = dq->u.long_values[1];
            ndq->u.long_values[1] = dq->u.long_values[1] = 0;
            size = 0;
        } else if( dq->flags & Q_REPEATED_DATA ) {
            dq->u.long_values[1] = size / (oldsize / dq->u.long_values[1]);
            ndq->u.long_values[1] -= dq->u.long_values[1];
            size = 0;
        } else if( dq->type == QDT_CONSTANT ) {
            dq->u.long_values[0] = size;
            ndq->u.long_values[0] -= dq->u.long_values[0];
            size = 0;
        } else if( dq->type == QDT_CONST ) {
            dq->u.string_leaf->length = size;
            ndq->u.string_leaf->literal += size;
            ndq->u.string_leaf->length = oldsize - size;
            size = 0;
        }
    }
    if( size != 0 ) {
        /* can't happen ! */
        CErr2p( ERR_FATAL_ERROR, "Bad initializer quad" );
        CSuicide();
    }
}

local void DeleteDataQuad( DATA_QUAD_LIST *dql )
{
    dql->prev->next = dql->next;
    if( dql->next != NULL )
        dql->next->prev = dql->prev;
}

local void GenDataQuad( DATA_QUAD *dq, unsigned long size )
{
    DATA_QUAD_LIST  *dql;
    unsigned long   cursize;

    dql = CurDataQuad->next;
    if( dql != NULL ) {
        /* overwrite the current dataquad */
        cursize = dql->size;
        while( size > cursize && dql->next != NULL ) {
            DeleteDataQuad( dql );
            dql = dql->next;
            cursize += dql->size;
        }
        /* dql now takes up cursize bytes but was defined to
           take up dql->size bytes: split into a dataquad with
           dql->size - (cursize - size) bytes and one with
           cursize - size bytes.
        */
        if( size < cursize )
            SplitDataQuad( dql, dql->size - (cursize - size) );
    } else {
        dql = NewDataQuad();
        CurDataQuad->next = dql;
        dql->prev = CurDataQuad;
        dql->next = NULL;
    }
    dql->size = size;
    memcpy( &dql->dq, dq, sizeof(DATA_QUAD) );
    CurDataQuad = dql;
}

local void ZeroBytes( long n )
{
    DATA_QUAD   dq;

    if( n == 0 ) return;
    dq.type = QDT_CONSTANT;
    dq.flags = Q_DATA;
    dq.u.long_values[0] = n;
    dq.u.long_values[1] = 0;
    GenDataQuad( &dq, n );
}

local void RelSeekBytes( long n )
{
    DATA_QUAD_LIST  *dql;

    dql = CurDataQuad;
    while( n < 0 && n <= -(long)dql->size ) {
        n += dql->size;
        dql = dql->prev;
    }
    while( n > 0 && dql->next != NULL ) {
        dql = dql->next;
        n -= dql->size;
    }
    /* now -dql->size < n <= 0 or dql->next == NULL */
    if( n < 0 ) {
        SplitDataQuad( dql, dql->size + n );
    }
    CurDataQuad = dql;
    if( n > 0 ) {
        /* dql->next == NULL */
        ZeroBytes( n );
    }
}

local void ChkConstant( unsigned long value, unsigned long max_value )
{
    if( value > max_value ) {                   /* 13-sep-91 */
        if( (value | (max_value >> 1)) != ~0UL ) {
            CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
        }
    }
}

local void StoreIValue( DATA_TYPE dtype, unsigned long value,
                        unsigned long size )
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
    if( dq_ptr->type == dtype  &&             /* 06-apr-92 */
        dq_ptr->flags == (Q_DATA | Q_REPEATED_DATA)  &&
        dq_ptr->u.long_values[0] == value ) {
        dq_ptr->u.long_values[1]++;             /* increment repeat count */
        CurDataQuad->size += size;
    } else if( dq_ptr->type == dtype  &&  dq_ptr->flags == Q_DATA ) {
        if( dq_ptr->u.long_values[0] == value ) {
            dq_ptr->flags |= Q_REPEATED_DATA;
            dq_ptr->u.long_values[1] = 2;       /* repeat count */
        } else {
            dq_ptr->flags |= Q_2_INTS_IN_ONE;
            dq_ptr->u.long_values[1] = value;
        }
        CurDataQuad->size += size;
    } else {
        dq.type = dtype;
        dq.flags = Q_DATA;
        dq.u.long_values[0] = value;
        if( value != 0 ) CompFlags.non_zero_data = 1;
        GenDataQuad( &dq, size );
        LastCurDataQuad = CurDataQuad;
        return;
    }
    /* if the next dataquad is non-empty we'll have to delete it */
    dql = CurDataQuad->next;
    if( dql != NULL ) {
        if( dql->size > size ) {
            SplitDataQuad( dql, size );
        }
        /* no need to "free" the next one, just remove it from the list */
        DeleteDataQuad( dql );
    }
}

local void StoreIValue64( DATA_TYPE dtype, uint64 value )
{
    DATA_QUAD           *dq_ptr;
    DATA_QUAD           dq;

    dq.type = dtype;
    dq.flags = Q_DATA;
    dq_ptr = &dq;
    dq.u.long64 = value;
    CompFlags.non_zero_data = 1;
    GenDataQuad( &dq, sizeof( int64 ) );
}


typedef struct {
    long                       offset;
    union {
        STR_HANDLE       str_h;
        SYM_HANDLE       sym_h;
    };
    bool                      is_str;
    bool                      addr_set;
    bool                      is_error;
    enum { IS_VALUE, IS_ADDR } state;
} addrfold_info;

local void AddrFold( TREEPTR tree, addrfold_info *info )
{
// Assume tree has been const folded
    SYM_ENTRY           sym;
    long                offset = 0;

    switch( tree->op.opr ) {
    case OPR_PUSHINT:
        info->state = IS_ADDR;
        info->offset = tree->op.long_value;
        if( info->offset != 0 ) CompFlags.non_zero_data = 1;
        break;
    case OPR_PUSHFLOAT:
        info->state = IS_ADDR;
        if( tree->op.float_value->len != 0 ) {
            info->offset = (long)atof( tree->op.float_value->string );
        } else {
#ifdef _LONG_DOUBLE_
            long_double ld;
            double d;

            ld = tree->op.float_value->ld;
            __LDFD( (long_double near *)&ld, (double near *)&d );
            info->offset = (long)d;
#else
            info->offset = (long)tree->op.float_value->ld.value;
#endif
        }
        if( info->offset != 0 ) CompFlags.non_zero_data = 1;
        break;
    case OPR_PUSHSTRING:
        info->state = IS_ADDR;
        if( info->addr_set ) {
            info->is_error = TRUE;
        }
        info->addr_set = TRUE;
        info->is_str = TRUE;
        info->str_h = tree->op.string_handle;
        tree->op.string_handle->ref_count++;
        CompFlags.non_zero_data = 1;
        break;
    case OPR_PUSHADDR:
        info->state = IS_ADDR;
    case OPR_PUSHSYM:
        if( info->addr_set ) {
            info->is_error = TRUE;
        }
        info->addr_set = TRUE;
        info->is_str = FALSE;
        SymGet( &sym, tree->op.sym_handle );
        info->sym_h = tree->op.sym_handle;
        CompFlags.non_zero_data = 1;
        if( sym.stg_class == SC_AUTO ) {
            info->is_error = TRUE;
        }
        break;
    case OPR_INDEX:
        if( tree->right->op.opr == OPR_PUSHINT ) {
            AddrFold( tree->left, info );
            offset = tree->right->op.long_value;
        } else if( tree->left->op.opr == OPR_PUSHINT ) {
            AddrFold( tree->right, info );
            offset = tree->left->op.long_value;
        } else {
            info->is_error = TRUE;
        }
        if( info->state == IS_VALUE ) { // must be foldable
            info->is_error = TRUE;
        }
        info->offset +=  offset * SizeOfArg( tree->expr_type );
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_ADD:
    case OPR_SUB:
        if( tree->right->op.opr == OPR_PUSHINT ) {
            AddrFold( tree->left, info );
            if( tree->op.opr == OPR_ADD ) {
                info->offset = info->offset+tree->right->op.long_value;
            } else {
                info->offset = info->offset-tree->right->op.long_value;
            }
        } else if( tree->left->op.opr == OPR_PUSHINT ) {
            AddrFold( tree->right, info );
            if( tree->op.opr == OPR_ADD ) {
                info->offset = tree->left->op.long_value+info->offset;
            } else {
                info->offset = tree->left->op.long_value-info->offset;
            }
        } else {
            info->is_error = TRUE;
        }
        if( info->state == IS_VALUE ) { // must be foldable
            info->is_error = TRUE;
        }
        break;
    case OPR_ADDROF:
        AddrFold( tree->right, info );
        info->state = IS_ADDR;
        CompFlags.non_zero_data = 1;
        break;
    case OPR_ARROW:
        AddrFold( tree->left, info );
        if( info->state == IS_VALUE ) { // must be foldable
            info->is_error = TRUE;
        }
        info->offset += tree->right->op.long_value;
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_POINTS:
        AddrFold( tree->left, info );
        if( info->state == IS_VALUE ) { // must be foldable
            info->is_error = TRUE;
        }
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_DOT:
        AddrFold( tree->left, info );
        info->offset += tree->right->op.long_value;
        CompFlags.non_zero_data = 1;
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_CONVERT:  //should check for pointer to smaller
    case OPR_CONVERT_PTR:
        AddrFold( tree->right, info );
        break;
    case OPR_ERROR:                 // error has already been issued
        break;
    default:
        info->is_error = TRUE;
        break;
    }
}

local void StorePointer( TYPEPTR typ, unsigned long size )
{
    type_modifiers      flags;
    TREEPTR             tree;
    TYPEPTR             typ2;
    int                 address_wanted;
    DATA_QUAD           dq;
    addrfold_info       info;

    dq.flags = Q_DATA;
    flags = FLAG_NONE;
    if( CompFlags.strings_in_code_segment )
        flags = FLAG_CONST;  /*01-sep-89*/
    if( typ->decl_type == TYPE_POINTER ) {
        if( typ->u.p.decl_flags & (FLAG_FAR|FLAG_HUGE) ) {
            dq.flags |= Q_FAR_POINTER;
            flags = FLAG_FAR;
        } else if( typ->u.p.decl_flags & FLAG_NEAR ) {
            dq.flags |= Q_NEAR_POINTER;
            flags = FLAG_NONE;
        } else {
            typ2 = typ->object;
            SKIP_TYPEDEFS( typ2 );
            if( typ2->decl_type == TYPE_FUNCTION ) {
                dq.flags |= Q_CODE_POINTER;
            } else if( TypeSize( typ ) == TARGET_FAR_POINTER ) {
                dq.flags |= Q_FAR_POINTER;
            }
        }
    }

    address_wanted = 0;
    if( CurToken == T_RIGHT_BRACE) { // t some x = {}
        dq.type = QDT_ID;
        dq.u.var.sym_handle = 0;
        dq.u.var.offset = 0;
    } else {
        tree = AddrExpr();
        tree = InitAsgn( typ, tree ); // as if we are assigning
        info.offset = 0;
        info.sym_h = 0;
        info.addr_set = FALSE;
        info.state = IS_VALUE;
        info.is_str = FALSE;
        info.is_error = FALSE;
        AddrFold( tree, &info );
        FreeExprTree( tree );
        if( info.state == IS_VALUE ) { // must be foldable  into addr+offset
            info.is_error = TRUE;
        }
        if( info.is_str ) { // need to fix cgen an DATAQUADS to handle str+off
            if( info.offset != 0 ) {
                info.is_error = TRUE;
            }
        }
        if( info.is_error ) {
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
            dq.type = QDT_ID;
            dq.u.var.sym_handle = 0;
            dq.u.var.offset = 0;
        } else {
            if( info.is_str ) {
                dq.type = QDT_STRING;
                dq.u.string_leaf = info.str_h;
            } else {
                dq.type = QDT_ID;
                dq.u.var.sym_handle = info.sym_h;
                dq.u.var.offset = info.offset;
            }
        }
    }
    if( typ->decl_type == TYPE_POINTER ) {
        GenDataQuad( &dq, size );
    } else if( dq.type == QDT_STRING ) {           /* 05-jun-91 */
        if( TypeSize( typ ) != DataPtrSize  ||  CompFlags.strict_ANSI ) {
            CErr1( ERR_INVALID_INITIALIZER );
        }
        GenDataQuad( &dq, size );
    } else { /* dq.type == QDT_ID */
        if( dq.u.var.sym_handle != 0 ) {
            if( TypeSize( typ ) != DataPtrSize ) {
                CErr1( ERR_INVALID_INITIALIZER );
            }
            GenDataQuad( &dq, size );
        } else {
            StoreIValue( typ->decl_type, dq.u.var.offset, size );
        }
    }
}

local void StoreInt64( TYPEPTR typ )
{
    TREEPTR     tree;
    DATA_QUAD   dq;

    dq.type = typ->decl_type;
    dq.flags = Q_DATA;
    U32ToU64( 0, &dq.u.long64 );
    if( CurToken != T_RIGHT_BRACE ) {
        tree = SingleExpr();
        tree = InitAsgn( typ, tree ); // as if we are assigning
        if( IsConstLeaf( tree ) ) {
            CastConstValue( tree, typ->decl_type );
            dq.u.long64 = tree->op.ulong64_value;
        } else {
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
        }
        FreeExprTree( tree );
        CompFlags.non_zero_data = 1;
    }
    GenDataQuad( &dq, sizeof( int64 ) );
}

local FIELDPTR InitBitField( FIELDPTR field )
{
    TYPEPTR             typ;
    unsigned long       value;
    unsigned long       size;
    uint64              value64;
    unsigned long       bit_value;
    unsigned long       offset;
    TOKEN               token;
    int                 is64bit;

    token = CurToken;
    if( CurToken == T_LEFT_BRACE )
        NextToken();
    typ = field->field_type;
    size = SizeOfArg( typ );
    is64bit = ( typ->u.f.field_type == TYPE_LONG64 )
           || ( typ->u.f.field_type == TYPE_ULONG64 );
    if( is64bit )
        U32ToU64( 0, &value64 );
    offset = field->offset;
    value = 0;
    while( typ->decl_type == TYPE_FIELD ||
           typ->decl_type == TYPE_UFIELD ) {
        bit_value = 0;
        if( CurToken != T_RIGHT_BRACE )
            bit_value = ConstExpr();
        if( typ->u.f.field_type == TYPE_BOOL ) {
            if( bit_value != 0 ) {
                bit_value = 1;
            }
        } else {
            ChkConstant( bit_value, BitMask[typ->u.f.field_width - 1] );
            bit_value &= BitMask[typ->u.f.field_width - 1];
        }
        if( is64bit ) {
            uint64 tmp;
            U32ToU64( bit_value, &tmp );
            U64ShiftL( &tmp, typ->u.f.field_start, &tmp );
            value64.u._32[L] |= tmp.u._32[L];
            value64.u._32[H] |= tmp.u._32[H];
        } else {
            value |= bit_value << typ->u.f.field_start;
        }
        field = field->next_field;
        if( field == NULL )
            break;
        if( field->offset != offset )
            break;    /* bit field done */
        typ = field->field_type;
        if( CurToken == T_EOF )
            break;
        if( CurToken != T_RIGHT_BRACE ) {
            MustRecog( T_COMMA );
        }
    }
    if( is64bit ) {
        StoreIValue64( typ->u.f.field_type, value64 );
    } else {
        StoreIValue( typ->u.f.field_type, value, size );
    }
    if( token == T_LEFT_BRACE ) {
        if( CurToken == T_COMMA ) NextToken();
        MustRecog( T_RIGHT_BRACE );
    }
    return( field );
}


/* typ is the (perhaps subaggregate) object that is initialized.
   ctyp refers to the current object which is the object
   associated with the closest brace pair. It may or may not be
   equal to typ */

/* Detects a C99 designated initializer */
local void *DesignatedInit( TYPEPTR typ, TYPEPTR ctyp, void *field )
{
    TREEPTR             tree;
    unsigned long       offs;
    static int          new_field = 1;

    if( !CompFlags.extensions_enabled && !CompFlags.c99_extensions ) {
        return( field );
    }

    if( CurToken != T_LEFT_BRACKET && CurToken != T_DOT ) {
        new_field = 1;
        return( field );
    }

    /* if designator refers to outer type: back out */
    if(typ != ctyp && new_field)
        return( NULL );

    new_field = 0;
    if( typ->decl_type == TYPE_ARRAY ) {
        if( CurToken != T_LEFT_BRACKET )
            return( NULL );
        NextToken();
        tree = SingleExpr();
        if( IsConstLeaf( tree ) ) {
            CastConstValue( tree, typ->decl_type );
            *(unsigned long *)field = tree->op.ulong_value;
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

    if( CurToken != T_LEFT_BRACKET && CurToken != T_DOT ) {
        new_field = 1;
        MustRecog( T_EQUAL );
    }
    return( field );
}

local bool DesignatedInSubAggregate( DATA_TYPE decl_type )
{
    switch( decl_type ) {
    case TYPE_ARRAY:
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_FCOMPLEX:
    case TYPE_DCOMPLEX:
    case TYPE_LDCOMPLEX:
        /* A subaggregate can be stopped by a designated initializer.
           in that case the comma was already eaten... */
        return ( CurToken == T_DOT || CurToken == T_LEFT_BRACKET );
    default:
        return FALSE;
    }
}

local void InitArray( TYPEPTR typ, TYPEPTR ctyp )
{
    unsigned long       n;
    unsigned long       m;
    unsigned long       *pm;
    unsigned long       array_size;
    unsigned long       elem_size;

    array_size = TypeSize( typ );
    n = 0;
    pm = &m;
    for( ;; ) {
        m = n;
        pm = DesignatedInit( typ, ctyp, pm );
        if( pm == NULL ) break;
        if( m != n ) {
            elem_size = SizeOfArg( typ->object );
            if( typ->u.array->unspecified_dim && m > array_size ) {
                RelSeekBytes( ( array_size - n ) * elem_size );
                ZeroBytes( (m - array_size) * elem_size );
            } else {
                RelSeekBytes( ( m - n ) * elem_size );
            }
            n = m;
        }
        n++;
        if( n > array_size ) {
            if( !typ->u.array->unspecified_dim ) break;
            array_size = n;
            /* clear out the new element just in case */
            elem_size = SizeOfArg( typ->object );
            ZeroBytes( elem_size );
            RelSeekBytes( -elem_size );
        }
        InitSymData( typ->object, ctyp, 1 );
        if( CurToken == T_EOF ) break;
        if( CurToken == T_RIGHT_BRACE ) break;
        if( DesignatedInSubAggregate( typ->object->decl_type ) ) continue;
        if( n < array_size || typ == ctyp || typ->u.array->unspecified_dim ) {
            MustRecog( T_COMMA );
        }
        if( CurToken == T_RIGHT_BRACE ) break;
    }
    if( typ->u.array->unspecified_dim ) {
        typ->u.array->dimension = array_size;
    }
    if( array_size > n ) {
        RelSeekBytes( ( array_size - n ) * SizeOfArg( typ->object ) );
    }
}

/* Detects a C99 designated initializer for fields */
/* Initialize struct or union fields */
local void InitStructUnion( TYPEPTR typ, TYPEPTR ctyp, FIELDPTR field )
{
    TYPEPTR             ftyp;
    unsigned long       n;
    unsigned long       offset;

    n = typ->u.tag->size;      /* get full size of the struct or union */
    offset = 0;

    for( ;; ) {
        field = DesignatedInit( typ, ctyp, field );
        if( field == NULL ) break;
        /* The first field might not start at offset 0;  19-mar-91 */
        if( field->offset != offset ) {                 /* 14-dec-88 */
            RelSeekBytes( field->offset - offset );
        }
        ftyp = field->field_type;
        offset = field->offset + SizeOfArg( ftyp );      /* 19-dec-88 */
        if( ftyp->decl_type == TYPE_FIELD  ||
            ftyp->decl_type == TYPE_UFIELD ) {
            field = InitBitField( field );
        } else {
            InitSymData( ftyp, ctyp, 1 );
            field = field->next_field;
        }
        if( typ->decl_type == TYPE_UNION ) {
            if( offset != n ) {
                ZeroBytes( n - offset );    /* pad the rest */
            }
            offset = n;
            /* designated initializers may still override this field */
            field = NULL;
        }
        if( CurToken == T_EOF ) break;
        if( CurToken == T_RIGHT_BRACE ) break;
        if( DesignatedInSubAggregate( ftyp->decl_type ) ) continue;
        if( field != NULL || typ == ctyp ) {
            MustRecog( T_COMMA );
        }
        if( CurToken == T_RIGHT_BRACE ) break;
    }
    RelSeekBytes( (unsigned)n - offset );
}

local void InitStruct( TYPEPTR typ, TYPEPTR ctyp )
{
    InitStructUnion( typ, ctyp, typ->u.tag->u.field_list );
}

local void InitUnion( TYPEPTR typ, TYPEPTR ctyp )
{
    FIELDPTR            field;
    TYPEPTR             ftyp;

    field = typ->u.tag->u.field_list;
    for( ;; ) {                             // skip unnamed bit fields
        if( field == NULL ) break;              // 12-nov-94
        ftyp = field->field_type;
        SKIP_TYPEDEFS( ftyp );
        if( field->name[0] != '\0' ) break;
        if( ftyp->decl_type == TYPE_STRUCT ) break;      /* 03-feb-95 */
        if( ftyp->decl_type == TYPE_UNION ) break;
        field = field->next_field;
    }
    InitStructUnion( typ, ctyp, field );
}

void InitSymData( TYPEPTR typ, TYPEPTR ctyp, int level )
{
    TOKEN               token;
    unsigned long       size;

    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_ENUM ) typ = typ->object;        /* 07-nov-90 */
    token = CurToken;
    if( CurToken == T_LEFT_BRACE ) {
        NextToken();
        if( CurToken == T_RIGHT_BRACE  ||  CurToken == T_COMMA ) {
            CErr1( ERR_EMPTY_INITIALIZER_LIST );
        }
    }
    size = SizeOfArg( typ );
    switch( typ->decl_type ) {
    case TYPE_ARRAY:
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
            if( typ == ctyp ) { /* initialize new current type */
                /* first zero out the whole array; otherwise
                   overlapping fields caused by designated
                   initializers will make life very difficult */
                ZeroBytes( size );
                RelSeekBytes( -size );
            }
            InitArray( typ, ctyp );
        }
        break;
    case TYPE_FCOMPLEX:
    case TYPE_DCOMPLEX:
    case TYPE_LDCOMPLEX:
    case TYPE_STRUCT:
        if( token == T_LEFT_BRACE ) {
            ctyp = typ;
        } else if( level == 0 ) {
            CErr1( ERR_NEED_BRACES );
        }
        if( typ == ctyp ) { /* initialize new current type */
            /* zero out all fields; otherwise overlapping fields caused
               by designated initializers will make life very difficult */
            ZeroBytes( size );
            RelSeekBytes( -size );
        }
        InitStruct( typ, ctyp );
        break;
    case TYPE_UNION:
        if( token == T_LEFT_BRACE ) {
            ctyp = typ;
        } else if( level == 0 ) {
            CErr1( ERR_NEED_BRACES );
        }
        InitUnion( typ, ctyp );
        break;
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_BOOL:
    case TYPE_SHORT:
    case TYPE_USHORT:
    case TYPE_INT:
    case TYPE_UINT:
    case TYPE_LONG:
    case TYPE_ULONG:
    case TYPE_POINTER:
        StorePointer( typ, size );
        break;
    case TYPE_LONG64:
    case TYPE_ULONG64:
        StoreInt64( typ );
        break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_FIMAGINARY:
    case TYPE_DIMAGINARY:
        StoreFloat( typ->decl_type, size );
        break;
    case TYPE_LONG_DOUBLE:
    case TYPE_LDIMAGINARY:
        //StoreFloat( typ->decl_type, size );
        StoreFloat( TYPE_DOUBLE, size );
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
          if( CurToken == T_EOF ) break;
          if( CurToken == T_SEMI_COLON )break;
          if( CurToken == T_LEFT_BRACE )break;
           NextToken();
        }
        MustRecog( T_RIGHT_BRACE );
    }
}


local int CharArray( TYPEPTR typ )
{
    if( CurToken == T_STRING ) {
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_CHAR || typ->decl_type == TYPE_UCHAR ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


local int WCharArray( TYPEPTR typ )
{
    if( CurToken == T_STRING ) {
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_SHORT || typ->decl_type == TYPE_USHORT ) {
            return( 1 );
        }
    }
    return( 0 );
}

local void InitCharArray( TYPEPTR typ )
{
    unsigned            len;
    STRING_LITERAL      *str_lit;
    unsigned long       size;
    DATA_QUAD           dq;

/*      This function handles the initialization of statements like:  */
/*              char  name[4] = "abcd";  */

    str_lit = GetLiteral();
    if( CompFlags.wide_char_string )
        CErr1( ERR_TYPE_MISMATCH );
    len = str_lit->length;
    if( typ->u.array->unspecified_dim )  typ->u.array->dimension = len;
    size = typ->u.array->dimension;
    if( len > size ) {
        if( (len - size) > 1 ) {
            CWarn1( WARN_LIT_TOO_LONG, ERR_LIT_TOO_LONG );
        }
        str_lit->length = len = size; /* chop the string */
    }
    dq.type = QDT_CONST;
    dq.flags = Q_DATA;
    dq.u.string_leaf = str_lit;
    GenDataQuad( &dq, len );
    if( size > len ) {                  /* if array > len of literal */
        ZeroBytes( size - len );
    }
    CompFlags.non_zero_data = 1;
}


local void InitWCharArray( TYPEPTR typ )
{
    unsigned            len;
    unsigned            i;
    STRING_LITERAL      *str_lit;
    unsigned            value;
    unsigned short      *pwc;
    unsigned long       size;
    DATA_QUAD           dq;

    dq.type = QDT_SHORT;
    dq.flags = Q_DATA;

/*      This function handles the initialization of statements like:  */
/*              wchar_t  name[5] = L"abcd";  */

    str_lit = GetLiteral();
    if( !CompFlags.wide_char_string )
        CErr1( ERR_TYPE_MISMATCH );
    len = str_lit->length / sizeof(unsigned short);
    if( typ->u.array->unspecified_dim ) {
        typ->u.array->dimension = len;
    }
    size = typ->u.array->dimension;
    if( len > size ) {
        if( (len - size) > 1 ) {
            CWarn1( WARN_LIT_TOO_LONG, ERR_LIT_TOO_LONG );
        }
        len = size;
    }
    pwc = (unsigned short *)str_lit->literal;
    i = 0;
    while( i < len ) {
        value = *pwc++;
        if( value != 0 ) CompFlags.non_zero_data = 1;
        dq.u.long_values[0] = value;
        GenDataQuad( &dq, sizeof( target_short ) );
        ++i;
    }
    if( i < size ) {
        ZeroBytes( (size - i) * sizeof(unsigned short) );
    }
    FreeLiteral( str_lit );
 }


local void StoreFloat( DATA_TYPE dtype, unsigned long size )
{
    TREEPTR     tree;
    DATA_QUAD   dq;

    dq.type = dtype;
    dq.flags = Q_DATA;
    dq.u.double_value = 0.0;
    if( CurToken != T_RIGHT_BRACE ) {
        tree = SingleExpr();
        if( IsConstLeaf( tree ) ) {
            CastConstValue( tree, dtype );
            {
#ifdef _LONG_DOUBLE_
                long_double ld;

                ld = tree->op.float_value->ld;
                __LDFD( (long_double near *)&ld, (double near *)&dq.u.double_value );
#else
                dq.u.double_value = tree->op.float_value->ld.value;
#endif
            }
        } else {
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
        }
        FreeExprTree( tree );
        if( dq.u.double_value != 0.0 ) CompFlags.non_zero_data = 1;
    }
    GenDataQuad( &dq, size );
}

local void GenStaticDataQuad( SYM_HANDLE sym_handle )
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
    CompFlags.non_zero_data = 0;
    struct_typ = NULL;
    last_array = NULL;
    typ = sym->sym_type;
    /* Follow chain of typedefs/structs/arrays */
    for( ;; ) {
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_ARRAY ) {
            /* Remember innermost array type */
            last_array = typ;
            typ = typ->object;
        } else if( typ->decl_type == TYPE_STRUCT ) {
            FIELDPTR    field;

            /* Remember outermost structure type */
            if( struct_typ == NULL ) {
                /* last_array cannot to be outside this struct! */
                last_array = NULL;
                struct_typ = typ;
            }
            /* Determine the type of the last field in the struct */
            field = typ->u.tag->u.field_list;
            if( field == NULL )  break;                     /* 10-sep-92 */
            while( field->next_field != NULL ) field = field->next_field;
            typ = field->field_type;
        } else {
            break;
        }
    }
    typ = last_array;
    /* If innermost array had unspecified dimension, create new types whose
     * dimensions will be determined by number of initializers
     */
    if( (typ != NULL) && typ->u.array->unspecified_dim ) {
        if( struct_typ == NULL ) {
            /* Array was not inside struct */
            sym->sym_type = ArrayNode( typ->object ); /* 18-oct-88 */
            sym->sym_type->u.array->unspecified_dim = TRUE;
        } else {
            typ = sym->sym_type;
            /* Create new structure type */
            sym->sym_type = TypeNode( TYPE_STRUCT, ArrayNode( last_array->object ) );
            sym->sym_type->u.tag = struct_typ->u.tag;
            struct_typ = sym->sym_type;
            /* Create new array types as necessary */
            for( ;; ) {
                SKIP_TYPEDEFS( typ );
                if( typ->decl_type != TYPE_ARRAY )  break;
                sym->sym_type = ArrayNode( sym->sym_type );
                sym->sym_type->u.array->unspecified_dim = TRUE;
                typ = typ->object;
            }
            typ = last_array;
        }
    } else {
        struct_typ = NULL;
    }
    SymReplace( sym, sym_handle );              /* 31-aug-88 */
    InitSymData( sym->sym_type, sym->sym_type, 0 );
    SymGet( sym, sym_handle );                  /* 31-aug-88 */
    if( struct_typ != NULL ) {                  /* 17-mar-92 */
        /* Structure contains an unspecified length array as last field */
        struct_typ->object->u.array->dimension = typ->u.array->dimension;
        typ->u.array->unspecified_dim = TRUE;
        typ->u.array->dimension = 0;    /* Reset back to 0 */
    }
    if( sym->u.var.segment == 0 ) {             /* 01-dec-91 */
        SetFarHuge( sym, 0 );
        SetSegment( sym );
        SetSegAlign( sym );                     /* 02-feb-92 */
    }
}

local void AssignAggregate( TREEPTR lvalue, TREEPTR rvalue, TYPEPTR typ )
{
    TREEPTR tree;

    tree = ExprNode( lvalue, OPR_EQUALS, rvalue );
    tree->right->op.opr = OPR_PUSHSYM;
    tree->expr_type = typ;
    tree->op.result_type = typ;
    AddStmt( tree );
}

local void AggregateVarDeclEquals( SYMPTR sym, SYM_HANDLE sym_handle )
{
    SYM_HANDLE  sym2_handle;
    SYM_ENTRY   sym2;

    sym2_handle = MakeNewSym( &sym2, 'X', sym->sym_type, SC_STATIC );
    sym2.flags |= SYM_INITIALIZED;
    CompFlags.initializing_data = 1;
    StaticInit( &sym2, sym2_handle );
    CompFlags.initializing_data = 0;
    SymReplace( &sym2, sym2_handle );
    /* StaticInit will change sym2.sym_type if it is an incomplete array type */
    sym->sym_type = sym2.sym_type;
    AssignAggregate( VarLeaf( sym, sym_handle ),
                     VarLeaf( &sym2, sym2_handle ), sym->sym_type );
}

local void InitStructVar( unsigned base, SYMPTR sym, SYM_HANDLE sym_handle, TYPEPTR typ)
{
    TYPEPTR     typ2;
    TREEPTR     opnd;
    TREEPTR     value;
    FIELDPTR    field;
    TOKEN       token;

    for( field = typ->u.tag->u.field_list; field; ) {
        token = CurToken;
        if( token == T_LEFT_BRACE )  NextToken();  //allow {}, and extra {expr}..}
        typ2 = field->field_type;
        SKIP_TYPEDEFS( typ2 );
        if( CurToken == T_RIGHT_BRACE ) {
            value = IntLeaf( 0 );
        } else {
            value = CommaExpr();
        }
        opnd = VarLeaf( sym, sym_handle );
        if( typ2->decl_type == TYPE_UNION ) {
            FIELDPTR    ufield;

            ufield = typ2->u.tag->u.field_list;
            typ2 = ufield->field_type;
            SKIP_TYPEDEFS( typ2 );
        }
        opnd = ExprNode( opnd, OPR_DOT, UIntLeaf( base + field->offset ) );
        opnd->expr_type = typ2;
        opnd->op.result_type = typ2;
        AddStmt( AsgnOp( opnd, T_ASSIGN_LAST, value ) );
        if( token == T_LEFT_BRACE )  MustRecog( T_RIGHT_BRACE );
        if( CurToken == T_EOF ) break;
        field = field->next_field;
        if( field == NULL ) break;
        if( CurToken != T_RIGHT_BRACE ) {
            MustRecog( T_COMMA );
        }
    }
}

static int SimpleUnion( TYPEPTR typ )
{
    FIELDPTR    field;

    field = typ->u.tag->u.field_list;
    typ = field->field_type;
    SKIP_TYPEDEFS( typ );
    switch( typ->decl_type ) {
    case TYPE_ARRAY:
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_FIELD:
    case TYPE_UFIELD:
        return( 0 );        // give up on these
    default:
        break;
    }
    return( 1 );
}

static int SimpleStruct( TYPEPTR typ )
{
    FIELDPTR    field;

    if( typ->decl_type == TYPE_UNION ) {
        return( 0 );
    }
    for( field = typ->u.tag->u.field_list; field; ) {
        typ = field->field_type;
        SKIP_TYPEDEFS( typ );

        switch( typ->decl_type ) {
        case TYPE_UNION:
            if( SimpleUnion( typ ) ) {
                break;        // go 1 deep to get by MFC examples
            }
        case TYPE_ARRAY:
        case TYPE_STRUCT:
        case TYPE_FIELD:
        case TYPE_UFIELD:
            return( 0 );        // give up on these
        default:
            break;
        }
        field = field->next_field;
    }
    return( 1 );
}


local void InitArrayVar( SYMPTR sym, SYM_HANDLE sym_handle, TYPEPTR typ )
{
    unsigned    i;
    unsigned    n;
    TYPEPTR     typ2;
    SYM_HANDLE  sym2_handle;
    SYM_ENTRY   sym2;
    TREEPTR     opnd;
    TREEPTR     value;
    TOKEN       token;

    typ2 = typ->object;
    SKIP_TYPEDEFS( typ2 );
    switch( typ2->decl_type ) {
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_USHORT:
    case TYPE_INT:
    case TYPE_UINT:
    case TYPE_LONG:
    case TYPE_ULONG:
    case TYPE_LONG64:
    case TYPE_ULONG64:
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
    case TYPE_POINTER:
    case TYPE_LONG_DOUBLE:
    case TYPE_FIMAGINARY:
    case TYPE_DIMAGINARY:
    case TYPE_LDIMAGINARY:
    case TYPE_BOOL:
        NextToken();                    // skip over T_LEFT_BRACE
        if( CharArray( typ->object ) ) {
            sym2_handle = MakeNewSym( &sym2, 'X', typ, SC_STATIC );
            sym2.flags |= SYM_INITIALIZED;
            if( sym2.u.var.segment == 0 ) {             /* 01-dec-91 */
                SetFarHuge( &sym2, 0 );
                SetSegment( &sym2 );
                SetSegAlign( &sym2 );                     /* 02-feb-92 */
            }
            SymReplace( &sym2, sym2_handle );
            GenStaticDataQuad( sym2_handle );
            InitCharArray( typ );
            AssignAggregate( VarLeaf( sym, sym_handle ),
                             VarLeaf( &sym2, sym2_handle ), typ );
        } else if( WCharArray( typ->object ) ) {
            sym2_handle = MakeNewSym( &sym2, 'X', typ, SC_STATIC );
            sym2.flags |= SYM_INITIALIZED;
            if( sym2.u.var.segment == 0 ) {             /* 01-dec-91 */
                SetFarHuge( &sym2, 0 );
                SetSegment( &sym2 );
                SetSegAlign( &sym2 );                   /* 02-feb-92 */
            }
            SymReplace( &sym2, sym2_handle );
            GenStaticDataQuad( sym2_handle );
            InitWCharArray( typ );
            AssignAggregate( VarLeaf( sym, sym_handle ),
                             VarLeaf( &sym2, sym2_handle ), typ );
        } else {
            n = typ->u.array->dimension;
            i = 0;
            for( ;; ) {     // accept some C++ { {1},.. }
                token = CurToken;
                if( token == T_LEFT_BRACE )  NextToken();
                opnd = VarLeaf( sym, sym_handle );
                value = CommaExpr();
                opnd = ExprNode( opnd, OPR_INDEX, IntLeaf( i ) );
                opnd->expr_type = typ2;
                opnd->op.result_type = typ2;
                AddStmt( AsgnOp( opnd, T_ASSIGN_LAST, value ) );
                if( token == T_LEFT_BRACE )  MustRecog( T_RIGHT_BRACE );
                ++i;
                if( CurToken == T_EOF ) break;
                if( CurToken == T_RIGHT_BRACE )break;
                MustRecog( T_COMMA );
                if( CurToken == T_RIGHT_BRACE )break;
                if( i == n ) {
                    CErr1( ERR_TOO_MANY_INITS );
               }
            }
            if( typ->u.array->unspecified_dim ) {
                typ->u.array->dimension = i;
            } else {
                while( i < n ) {
                    value = IntLeaf( 0 );
                    opnd = VarLeaf( sym, sym_handle );
                    opnd = ExprNode( opnd, OPR_INDEX, IntLeaf( i ) );
                    opnd->expr_type = typ2;
                    opnd->op.result_type = typ2;
                    AddStmt( AsgnOp( opnd, T_ASSIGN_LAST, value ) );
                    ++i;
                }
            }
        }
        MustRecog( T_RIGHT_BRACE );
        break;
    case TYPE_FCOMPLEX:
    case TYPE_DCOMPLEX:
    case TYPE_LDCOMPLEX:
    case TYPE_STRUCT:
    case TYPE_UNION:
        if( SimpleStruct( typ2 ) ) {
            unsigned    base;
            unsigned    size;

            NextToken();                    // skip over T_LEFT_BRACE
            n = typ->u.array->dimension;
            i = 0;
            base = 0;
            size = SizeOfArg( typ2 );
            for( ;; ) {
                token = CurToken;
                if( token == T_LEFT_BRACE ) {
                    NextToken();
                }
                InitStructVar( base, sym, sym_handle, typ2 );
                if( token == T_LEFT_BRACE ) {
                    MustRecog( T_RIGHT_BRACE );
                }
                ++i;
                if( CurToken == T_EOF ) break;
                if( CurToken == T_RIGHT_BRACE ) break;
                MustRecog( T_COMMA );
                if( CurToken == T_RIGHT_BRACE ) break;
                if( i == n ) {
                    CErr1( ERR_TOO_MANY_INITS );
               }
               base += size;
            }
            if( typ->u.array->unspecified_dim ) {
                typ->u.array->dimension = i;
            } else {
                while( i < n ) { // mop up
                    base += size;
                    InitStructVar( base, sym, sym_handle, typ2 );
                    ++i;
                }
            }
           NextToken();                    // skip over T_RIGHT_BRACE
           break;
        }
    default:
        AggregateVarDeclEquals( sym, sym_handle );
        break;
    }
}

void VarDeclEquals( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TYPEPTR     typ;

    if( SymLevel == 0  ||  sym->stg_class == SC_STATIC ) {
        if( sym->flags & SYM_INITIALIZED ) {
            CErrSymName( ERR_VAR_ALREADY_INITIALIZED, sym, sym_handle );
        }
        sym->flags |= SYM_INITIALIZED;
        CompFlags.initializing_data = 1;
        StaticInit( sym, sym_handle );
        CompFlags.initializing_data = 0;
    } else {
        SymReplace( sym, sym_handle );          /* 31-aug-88 */
        SrcLoc = TokenLoc;
        typ = sym->sym_type;
        SKIP_TYPEDEFS( typ );
        /* 07-jun-89  check for { before checking for array,struct
           or union  */
        if( CurToken != T_LEFT_BRACE  &&
            typ->decl_type != TYPE_ARRAY ) {            /* 27-jun-89 */
            AddStmt( AsgnOp( VarLeaf( sym, sym_handle ),
                   T_ASSIGN_LAST, CommaExpr() ) );
        } else if( typ->decl_type == TYPE_ARRAY ) {
            if( CurToken == T_LEFT_BRACE && CompFlags.auto_agg_inits ) {
                InitArrayVar( sym, sym_handle, typ );
            } else {
                AggregateVarDeclEquals( sym, sym_handle );
            }
        } else if( typ->decl_type == TYPE_STRUCT ||
                   typ->decl_type == TYPE_UNION ) {
            if( CurToken == T_LEFT_BRACE && CompFlags.auto_agg_inits && SimpleStruct( typ ) ) {
                NextToken();  //T_LEFT_BRACE
                InitStructVar( 0, sym, sym_handle, typ );
                NextToken(); //T_RIGHT_BRACE
            } else {
                AggregateVarDeclEquals( sym, sym_handle );
            }
        } else {
            NextToken();
            AddStmt( AsgnOp( VarLeaf( sym, sym_handle ),
                   T_ASSIGN_LAST, CommaExpr() ) );
            MustRecog( T_RIGHT_BRACE );
        }
    }
}
