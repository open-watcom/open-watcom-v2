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


#include "i64.h"
#include "cvars.h"
#include <limits.h>


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


#define DATA_QUAD_SEG_SIZE      (16*1024)
#define DATA_QUADS_PER_SEG      (DATA_QUAD_SEG_SIZE/sizeof(DATA_QUAD))

#define MAX_DATA_QUAD_SEGS (LARGEST_DATA_QUAD_INDEX/DATA_QUADS_PER_SEG + 1)

DATA_QUAD       *DataQuadSegs[MAX_DATA_QUAD_SEGS];/* segments for data quads*/
int             LastDataQuadSegIndex;
DATA_QUAD       *DataQuadPtr;
int             DataQuadIndex;
int             LastDataQuadIndex;

void InitDataQuads()
{
    DataQuadIndex = DATA_QUADS_PER_SEG;
    DataQuadSegIndex = -1;
    memset( DataQuadSegs, 0, MAX_DATA_QUAD_SEGS * sizeof(DATA_QUAD *) );
}

void FreeDataQuads()
{
    unsigned    i;

    for( i = 0; i < MAX_DATA_QUAD_SEGS; i++ ) {
        if( DataQuadSegs[i] == NULL ) break;
        FEfree( DataQuadSegs[i] );
    }
    InitDataQuads();
}

void PageOutDataQuads()
{
}

int StartDataQuadAccess()
{
    if( DataQuadSegIndex != -1 ) {
        DataQuadSegIndex = 0;
        DataQuadIndex = 0;
        DataQuadPtr = DataQuadSegs[ 0 ];
        return( 1 );                    // indicate data quads exist
    }
    return( 0 );                        // indicate no data quads
}

DATA_QUAD *NextDataQuad()
{
    DATA_QUAD   *dq_ptr;

    if( DataQuadIndex >= (DATA_QUADS_PER_SEG-1) ) {
        ++DataQuadSegIndex;
        DataQuadIndex = 0;
        DataQuadPtr = DataQuadSegs[ DataQuadSegIndex ];
    }
    ++DataQuadIndex;
    dq_ptr = DataQuadPtr;
    ++DataQuadPtr;
    return( dq_ptr );
}

void GenDataQuad( DATA_QUAD *dq )
{
    if( DataQuadIndex >= (DATA_QUADS_PER_SEG-1) ) {
        if( DataQuadSegIndex == MAX_DATA_QUAD_SEGS ) {
            CErr1( ERR_INTERNAL_LIMIT_EXCEEDED );
            CSuicide();
        }
        ++DataQuadSegIndex;
        DataQuadIndex = 0;
        DataQuadPtr = (DATA_QUAD *)FEmalloc( DATA_QUAD_SEG_SIZE );
        DataQuadSegs[ DataQuadSegIndex ] = DataQuadPtr;
    }
    memcpy( DataQuadPtr, dq, sizeof(DATA_QUAD) );
    ++DataQuadIndex;
    ++DataQuadPtr;
    DataQuadPtr->opr = T_EOF;
}


local void ZeroBytes( long n )
{
    auto DATA_QUAD dq;

    dq.opr = T_CONSTANT;
    dq.flags = Q_DATA;
    dq.u.long_values[0] = n;
    dq.u.long_values[1] = 0;
    GenDataQuad( &dq );
}

local void ChkConstant( unsigned long value, unsigned long max_value )
{
    if( value > max_value ) {                   /* 13-sep-91 */
        if( (value | (max_value >> 1)) != ~0UL ) {
            CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
        }
    }
}

local void StoreIValue( TOKEN int_type, unsigned long value )
{
    DATA_QUAD           *dq_ptr;
    auto DATA_QUAD      dq;

    dq.flags = 0;
    dq.opr = 0;
    dq_ptr = &dq;
    if( LastDataQuadIndex == DataQuadIndex  &&
        LastDataQuadSegIndex == DataQuadSegIndex ) {
        dq_ptr = DataQuadPtr - 1;
    }
    if( dq_ptr->opr == int_type  &&             /* 06-apr-92 */
        dq_ptr->flags == (Q_DATA | Q_REPEATED_DATA)  &&
        dq_ptr->u.long_values[0] == value ) {
        dq_ptr->u.long_values[1]++;             /* increment repeat count */
    } else if( dq_ptr->opr == int_type  &&  dq_ptr->flags == Q_DATA ) {
        if( dq_ptr->u.long_values[0] == value ) {
            dq_ptr->flags |= Q_REPEATED_DATA;
            dq_ptr->u.long_values[1] = 2;       /* repeat count */
        } else {
            dq_ptr->flags |= Q_2_INTS_IN_ONE;
            dq_ptr->u.long_values[1] = value;
        }
    } else {
        dq.opr = int_type;
        dq.flags = Q_DATA;
        dq.u.long_values[0] = value;
        if( value != 0 ) CompFlags.non_zero_data = 1;
        GenDataQuad( &dq );
        LastDataQuadIndex = DataQuadIndex;
        LastDataQuadSegIndex = DataQuadSegIndex;
    }
}

local void StoreIValue64( uint64 value )
{
    DATA_QUAD           *dq_ptr;
    auto DATA_QUAD      dq;

    dq.opr = T___INT64;
    dq.flags = Q_DATA;
    dq_ptr = &dq;
    dq.u.long64 = value;
    CompFlags.non_zero_data = 1;
    GenDataQuad( &dq );
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
}addrfold_info;

local void AddrFold( TREEPTR tree, addrfold_info *info ){
// Assume tree has been const folded
    SYM_ENTRY           sym;
    long                offset;

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
            __LDFD( (long_double near *)&ld,
                   (double near *)&d );
            info->offset = (long)d;
        #else
            info->offset = (long)tree->op.float_value->ld.value;
        #endif
        }
        if( info->offset != 0 ) CompFlags.non_zero_data = 1;
        break;
    case OPR_PUSHSTRING:
        info->state = IS_ADDR;
        if( info->addr_set ){
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
        if( info->addr_set ){
            info->is_error = TRUE;
        }
        info->addr_set = TRUE;
        info->is_str = FALSE;
        SymGet( &sym, tree->op.sym_handle );
        info->sym_h = tree->op.sym_handle;
        CompFlags.non_zero_data = 1;
        if( sym.stg_class == SC_AUTO ){
            info->is_error = TRUE;
        }
        break;
    case OPR_INDEX:
        if(  tree->right->op.opr == OPR_PUSHINT ){
            AddrFold( tree->left, info );
            offset = tree->right->op.long_value;
        }else if( tree->left->op.opr == OPR_PUSHINT ){
            AddrFold( tree->right, info );
            offset = tree->left->op.long_value;
        }else{
            info->is_error = TRUE;
        }
        if( info->state == IS_VALUE ){ // must be foldable
            info->is_error = TRUE;
        }
        info->offset +=  offset * SizeOfArg( tree->expr_type );
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_ADD:
    case OPR_SUB:
        if(  tree->right->op.opr == OPR_PUSHINT ){
            AddrFold( tree->left, info );
            if( tree->op.opr == OPR_ADD ) {
                info->offset = info->offset+tree->right->op.long_value;
            } else {
                info->offset = info->offset-tree->right->op.long_value;
            }
        }else if( tree->left->op.opr == OPR_PUSHINT ){
            AddrFold( tree->right, info );
            if( tree->op.opr == OPR_ADD ) {
                info->offset = tree->left->op.long_value+info->offset;
            } else {
                info->offset = tree->left->op.long_value-info->offset;
            }
        }else{
            info->is_error = TRUE;
        }
        if( info->state == IS_VALUE ){ // must be foldable
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
        if( info->state == IS_VALUE ){ // must be foldable
            info->is_error = TRUE;
        }
        info->offset += tree->right->op.long_value;
        if( tree->op.flags & OPFLAG_RVALUE ) {
            info->state = IS_VALUE;
        }
        break;
    case OPR_POINTS:
        AddrFold( tree->left, info );
        if( info->state == IS_VALUE ){ // must be foldable
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

local void StorePointer( TYPEPTR typ, TOKEN int_type )
{
    int                 flags;
    TREEPTR             tree;
    TYPEPTR             typ2;
    int                 address_wanted;
    auto DATA_QUAD      dq;
    addrfold_info       info;

    dq.flags = Q_DATA;
    flags = 0;
    if( CompFlags.strings_in_code_segment ) flags = FLAG_CONST;/*01-sep-89*/
    if( typ->decl_type == TYPE_POINTER ) {
        if( typ->u.p.decl_flags & (FLAG_FAR|FLAG_HUGE) ) {
            dq.flags |= Q_FAR_POINTER;
            flags = FLAG_FAR;
        } else if( typ->u.p.decl_flags & FLAG_NEAR ) {
            dq.flags |= Q_NEAR_POINTER;
            flags = 0;
        } else {
            typ2 = typ->object;
            while( typ2->decl_type == TYPE_TYPEDEF ) typ2 = typ2->object;
            if( typ2->decl_type == TYPE_FUNCTION ) {
                dq.flags |= Q_CODE_POINTER;
            } else if( TypeSize( typ ) == TARGET_FAR_POINTER ) {
                dq.flags |= Q_FAR_POINTER;
            }
        }
    }

    address_wanted = 0;
    if( CurToken == T_RIGHT_BRACE) { // t some x = {}
        dq.opr = T_ID;
        dq.u.var.sym_handle = 0;
        dq.u.var.offset = 0;
    }else{
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
        if( info.state == IS_VALUE ){ // must be foldable  into addr+offset
            info.is_error = TRUE;
        }
        if( info.is_str ){ // need to fix cgen an DATAQUADS to handle str+off
            if( info.offset != 0 ){
                info.is_error = TRUE;
            }
        }
        if( info.is_error ){
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
            dq.opr = T_ID;
            dq.u.var.sym_handle = 0;
            dq.u.var.offset = 0;
        }else{
            if( info.is_str ){
                dq.opr = T_STRING;
                dq.u.string_leaf = info.str_h;
            }else{
                dq.opr = T_ID;
                dq.u.var.sym_handle = info.sym_h;
                dq.u.var.offset = info.offset;
            }
        }
    }
    if( typ->decl_type == TYPE_POINTER ) {
        GenDataQuad( &dq );
    } else if( dq.opr == T_STRING ) {           /* 05-jun-91 */
        if( TypeSize( typ ) != DataPtrSize  ||  CompFlags.strict_ANSI ) {
            CErr1( ERR_INVALID_INITIALIZER );
        }
        GenDataQuad( &dq );
    } else { /* dq.opr == T_ID */
        if( dq.u.var.sym_handle != 0 ) {
            if( TypeSize( typ ) != DataPtrSize ) {
                CErr1( ERR_INVALID_INITIALIZER );
            }
            GenDataQuad( &dq );
        } else {
            StoreIValue( int_type, dq.u.var.offset );
        }
    }
}

local StoreInt64( TYPEPTR typ )
{
    TREEPTR     tree;
    auto DATA_QUAD dq;

    dq.opr = T___INT64;
    dq.flags = Q_DATA;
    U32ToU64( 0, &dq.u.long64 );
    if( CurToken != T_RIGHT_BRACE ) {
        tree = SingleExpr();
        tree = InitAsgn( typ, tree ); // as if we are assigning
        if( tree->op.opr == OPR_PUSHINT || tree->op.opr == OPR_PUSHFLOAT ) {
            CastConstValue( tree, typ->decl_type );
            dq.u.long64 = tree->op.ulong64_value;
        }else{
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
        }
        FreeExprTree( tree );
        CompFlags.non_zero_data = 1;
    }
    GenDataQuad( &dq );
}

local FIELDPTR InitBitField( FIELDPTR field )
{
    TYPEPTR             typ;
    unsigned long       value;
    uint64              value64;
    unsigned long       bit_value;
    unsigned long       offset;
    int                 token;
    TOKEN               int_type;

    token = CurToken;
    if( CurToken == T_LEFT_BRACE ) NextToken();
    typ = field->field_type;
    switch( typ->u.f.field_type ) {
    case TYPE_CHAR:
    case TYPE_UCHAR:
        int_type = T_CHAR;
        break;
    case TYPE_SHORT:
    case TYPE_USHORT:
        int_type = T_SHORT;
        break;
    case TYPE_INT:
    case TYPE_UINT:
        int_type = T_INT;
        break;
    case TYPE_LONG:
    case TYPE_ULONG:
        int_type = T_LONG;
        break;
    case TYPE_LONG64:
    case TYPE_ULONG64:
        int_type = T___INT64;
        U32ToU64( 0, &value64 );
        break;
    }
    offset = field->offset;
    value = 0;
    while( typ->decl_type == TYPE_FIELD ||
           typ->decl_type == TYPE_UFIELD ) {
        bit_value = 0;
        if( CurToken != T_RIGHT_BRACE ) bit_value = ConstExpr();
        ChkConstant( bit_value, BitMask[ typ->u.f.field_width - 1 ] );
        bit_value &= BitMask[ typ->u.f.field_width - 1 ];
        if( int_type == T___INT64 ){
            uint64 tmp;
            U32ToU64( bit_value, &tmp );
            U64ShiftL( &tmp, typ->u.f.field_start, &tmp );
            value64.u._32[L] |= tmp.u._32[L];
            value64.u._32[H] |= tmp.u._32[H];
        }else{
            value |= bit_value << typ->u.f.field_start;
        }
        field = field->next_field;
        if( field == NULL ) break;
        if( field->offset != offset ) break;    /* bit field done */
        typ = field->field_type;
        if( CurToken == T_EOF ) break;
        if( CurToken != T_RIGHT_BRACE ) MustRecog( T_COMMA );
    }
    if( int_type == T___INT64 ){
        StoreIValue64( value64 );
    }else{
        StoreIValue( int_type, value );
    }
    if( token == T_LEFT_BRACE ) {
        if( CurToken == T_COMMA ) NextToken();
        MustRecog( T_RIGHT_BRACE );
    }
    return( field );
}


local void InitArray( TYPEPTR typ )
{
    unsigned long       n;
    unsigned long       array_size;

    array_size = TypeSize( typ );
    n = 0;
    for( ;; ) {
        InitSymData( typ->object, 1 );
        n++;
        if( n == array_size ){
            break;
        }
        if( CurToken == T_EOF ) break;
        if( CurToken == T_RIGHT_BRACE ) break;
        MustRecog( T_COMMA );
        if( CurToken == T_RIGHT_BRACE ) break;
    }
    if( array_size == 0 ){
        typ->u.array->dimension = n;
    }else if( array_size > n ){
        ZeroBytes( (array_size-n) * SizeOfArg( typ->object ) );
    }
}

local void InitStruct( TYPEPTR typ )
{
    FIELDPTR            field;
    unsigned long       n;
    unsigned            offset;

    n = typ->u.tag->size;      /* get full size of the struct */
    offset = 0;
    for( field = typ->u.tag->u.field_list; field; ) {
        /* The first field might not start at offset 0;  19-mar-91 */
        if( field->offset != offset ) {                 /* 14-dec-88 */
            ZeroBytes( field->offset - offset );        /* padding */
        }
        typ = field->field_type;
        offset = field->offset + SizeOfArg( typ );      /* 19-dec-88 */
        if( typ->decl_type == TYPE_FIELD  ||
            typ->decl_type == TYPE_UFIELD ) {
            field = InitBitField( field );
        } else {
            InitSymData( typ, 1 );
            field = field->next_field;
        }
        if( field == NULL ) break;
        if( CurToken == T_EOF ) break;
        if( CurToken != T_RIGHT_BRACE )  MustRecog( T_COMMA );
    }
    if( (unsigned)n > offset ) {        /* 14-dec-88, 07-jun-92 */
        ZeroBytes( (unsigned)n - offset );      /* padding */
    }
}

local void InitUnion( TYPEPTR typ )
{
    FIELDPTR            field;
    unsigned long       n;

    n = typ->u.tag->size;      /* get full size of the union */
    field = typ->u.tag->u.field_list;
    for(;;) {                           // skip unnamed bit fields
        if( field == NULL ) break;              // 12-nov-94
        typ = field->field_type;
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        if( field->name[0] != '\0' ) break;
        if( typ->decl_type == TYPE_STRUCT ) break;      /* 03-feb-95 */
        if( typ->decl_type == TYPE_UNION ) break;
        field = field->next_field;
    }
    if( field != NULL ) {               /* 18-oct-94 */
        if( typ->decl_type == TYPE_FIELD  ||    /* 12-nov-94 */
            typ->decl_type == TYPE_UFIELD ) {
            InitBitField( field );
        } else {
            InitSymData( typ, 1 );
        }
        n -= SizeOfArg( typ );  /* subtract size of the first type */
        if( n != 0 ) ZeroBytes( n );/* pad the rest */
    }
}

void InitSymData( TYPEPTR typ, int level )
{
    int                 token;

    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    if( typ->decl_type == TYPE_ENUM ) typ = typ->object;        /* 07-nov-90 */
    token = CurToken;
    if( CurToken == T_LEFT_BRACE ) {
        NextToken();
        if( CurToken == T_RIGHT_BRACE  ||  CurToken == T_COMMA ) {
            CErr1( ERR_EMPTY_INITIALIZER_LIST );
        }
    }
    switch( typ->decl_type ) {
    case TYPE_ARRAY:
        if( CharArray( typ->object ) ) {
            InitCharArray( typ );
        } else if( WCharArray( typ->object ) ) {
            InitWCharArray( typ );
        } else {
            if( token != T_LEFT_BRACE  &&  level == 0 ) {
                CErr1( ERR_NEED_BRACES );
            }
            InitArray( typ );
        }
        break;
    case TYPE_STRUCT:
        if( token != T_LEFT_BRACE  &&  level == 0 ) {
            CErr1( ERR_NEED_BRACES );
        }
        InitStruct( typ );
        break;
    case TYPE_UNION:
        if( token != T_LEFT_BRACE  &&  level == 0 ) {
            CErr1( ERR_NEED_BRACES );
        }
        InitUnion( typ );
        break;
    case TYPE_CHAR:
    case TYPE_UCHAR:
        StorePointer( typ, T_CHAR );
        break;
    case TYPE_SHORT:
    case TYPE_USHORT:
        StorePointer( typ, T_SHORT );
        break;
    case TYPE_INT:
    case TYPE_UINT:
        StorePointer( typ, T_INT );
        break;
    case TYPE_LONG:
    case TYPE_ULONG:
        StorePointer( typ, T_LONG );
        break;
    case TYPE_LONG64:
    case TYPE_ULONG64:
        StoreInt64( typ );
        break;
    case TYPE_FLOAT:
        StoreFloat( T_FLOAT );
        break;
    case TYPE_DOUBLE:
        StoreFloat( T_DOUBLE );
        break;
    case TYPE_POINTER:
        StorePointer( typ, T_ID );
        break;
    }
    if( token == T_LEFT_BRACE ) {
        if( CurToken == T_COMMA ) {
            NextToken();
        }
        if( CurToken != T_RIGHT_BRACE ){
            CErr1( ERR_TOO_MANY_INITS );
        }
        while( CurToken != T_RIGHT_BRACE ){
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
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        if( typ->decl_type == TYPE_CHAR || typ->decl_type == TYPE_UCHAR ){
            return( TRUE );
        }
    }
    return( FALSE );
}


local int WCharArray( TYPEPTR typ )
{
    if( CurToken == T_STRING  &&  CompFlags.wide_char_string ) {
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
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
    auto DATA_QUAD      dq;

/*      This function handles the initialization of statements like:  */
/*              char  name[4] = "abcd";  */

    str_lit = GetLiteral();
    len = str_lit->length;
    if( typ->u.array->dimension == 0 )  typ->u.array->dimension = len;
    size = typ->u.array->dimension;
    if( len > size ) {
        if( (len - size) > 1 ) {
            CWarn1( WARN_LIT_TOO_LONG, ERR_LIT_TOO_LONG );
        }
        str_lit->length = size; /* chop the string */
    }
    dq.opr = T_CONST;
    dq.flags = Q_DATA;
    dq.u.string_leaf = str_lit;
    GenDataQuad( &dq );
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
    auto DATA_QUAD      dq;

    dq.opr = T_SHORT;
    dq.flags = Q_DATA;

/*      This function handles the initialization of statements like:  */
/*              wchar_t  name[4] = "abcd";  */

    str_lit = GetLiteral();
    len = str_lit->length / sizeof(unsigned short);
    if( typ->u.array->dimension == 0 ) {
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
        GenDataQuad( &dq );
        ++i;
    }
    if( i < size ) {
        ZeroBytes( (size - i) * sizeof(unsigned short) );
    }
    CMemFree( str_lit );
 }


local StoreFloat( int float_type )
{
    TREEPTR     tree;
    auto DATA_QUAD dq;

    dq.opr = float_type;
    dq.flags = Q_DATA;
    dq.u.double_value = 0.0;
    if( CurToken != T_RIGHT_BRACE ) {
        tree = SingleExpr();
        switch( tree->op.opr ) {
        case OPR_PUSHINT:
            if( tree->op.const_type == TYPE_ULONG ) {
                dq.u.double_value = tree->op.ulong_value;
            } else {
                dq.u.double_value = tree->op.long_value;
            }
            break;
        case OPR_PUSHFLOAT:
            if( tree->op.float_value->len != 0 ) {
                dq.u.double_value = atof( tree->op.float_value->string );
            } else {
                #ifdef _LONG_DOUBLE_
                    long_double ld;

                    ld = tree->op.float_value->ld;
                    __LDFD( (long_double near *)&ld,
                            (double near *)&dq.u.double_value );
                #else
                    dq.u.double_value = tree->op.float_value->ld.value;
                #endif
            }
            break;
        default:
            CErr1( ERR_NOT_A_CONSTANT_EXPR );
            break;
        }
        FreeExprTree( tree );
        if( dq.u.double_value != 0.0 ) CompFlags.non_zero_data = 1;
    }
    GenDataQuad( &dq );
}

local void GenStaticDataQuad( SYM_HANDLE sym_handle )
{
    auto DATA_QUAD      dq;

    dq.opr = T_STATIC;
    dq.flags = Q_DATA;
    dq.u.var.sym_handle = sym_handle;
    dq.u.var.offset = 0;
    GenDataQuad( &dq );
}

void StaticInit( SYMPTR sym, SYM_HANDLE sym_handle )
{
    TYPEPTR             typ;
    TYPEPTR             struct_typ;
    FIELDPTR            field;

    GenStaticDataQuad( sym_handle );
    CompFlags.non_zero_data = 0;
    struct_typ = NULL;
    typ = sym->sym_type;
    for(;;) {
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        if( typ->decl_type != TYPE_STRUCT )  break;     /* 17-mar-92 */
        if( struct_typ == NULL )  struct_typ = typ;
        field = typ->u.tag->u.field_list;
        if( field == NULL )  break;                     /* 10-sep-92 */
        while( field->next_field != NULL ) field = field->next_field;
        typ = field->field_type;
    }
    if( typ->decl_type == TYPE_ARRAY  &&  TypeSize( typ ) == 0 ) {
        if( struct_typ == NULL ) {
            sym->sym_type = ArrayNode( typ->object ); /* 18-oct-88 */
        } else {
            sym->sym_type = TypeNode( TYPE_STRUCT,
                                            ArrayNode( typ->object ) );
            sym->sym_type->u.tag = struct_typ->u.tag;
            struct_typ = sym->sym_type;
        }
    } else {
        struct_typ = NULL;
    }
    SymReplace( sym, sym_handle );              /* 31-aug-88 */
    InitSymData( sym->sym_type, 0 );
    SymGet( sym, sym_handle );          /* 31-aug-88 */
    if( struct_typ != NULL ) {          /* 17-mar-92 */
        /* structure contains a unspecified length array as last field */
        struct_typ->object->u.array->dimension = typ->u.array->dimension;
        typ->u.array->dimension = 0;    /* reset back to 0 */
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
    /* StaticInit will change sym2.sym_type if it is an
        incomplete array type */
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
    int         token;

    for( field = typ->u.tag->u.field_list; field; ) {
        token = CurToken;
        if( token == T_LEFT_BRACE )  NextToken();  //allow {}, and extra {expr}..}
        typ2 = field->field_type;
        while( typ2->decl_type == TYPE_TYPEDEF ) typ2 = typ2->object;
        if( CurToken == T_RIGHT_BRACE ) {
            value = IntLeaf( 0 );
        } else {
            value = CommaExpr();
        }
        opnd = VarLeaf( sym, sym_handle );
        if( typ2->decl_type == TYPE_UNION ){
            FIELDPTR    ufield;

            ufield = typ2->u.tag->u.field_list;
            typ2 = ufield->field_type;
            while( typ2->decl_type == TYPE_TYPEDEF ) typ2 = typ2->object;
        }
        opnd = ExprNode( opnd, OPR_DOT, UIntLeaf( base+field->offset ) );
        opnd->expr_type = typ2;
        opnd->op.result_type = typ2;
        AddStmt( AsgnOp( opnd, T_ASSIGN_LAST, value ) );
        if( token == T_LEFT_BRACE )  MustRecog( T_RIGHT_BRACE );
        if( CurToken == T_EOF ) break;
        field = field->next_field;
        if( field == NULL )break;
        if( CurToken != T_RIGHT_BRACE ) {
            MustRecog( T_COMMA );
        }
    }
}

static int SimpleUnion( TYPEPTR typ ){
    FIELDPTR    field;

    field = typ->u.tag->u.field_list;
    typ = field->field_type;
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    switch( typ->decl_type ) {
    case TYPE_ARRAY:
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_FIELD:
    case TYPE_UFIELD:
        return( 0 );        // give up on these
    }
    return( 1 );
}

static int SimpleStruct( TYPEPTR typ )
{
    FIELDPTR    field;

    if( typ->decl_type == TYPE_UNION ){
        return( 0 );
    }
    for( field = typ->u.tag->u.field_list; field; ) {
        typ = field->field_type;
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        switch( typ->decl_type ) {
        case TYPE_UNION:
            if( SimpleUnion( typ ) ){
                break;        // go 1 deep to get by MFC examples
            }
        case TYPE_ARRAY:
        case TYPE_STRUCT:
        case TYPE_FIELD:
        case TYPE_UFIELD:
            return( 0 );        // give up on these
        }
        field = field->next_field;
    }
    return( 1 );
}


local void InitArrayVar( SYMPTR sym, SYM_HANDLE sym_handle, TYPEPTR typ)
{
    unsigned    i;
    unsigned    n;
    TYPEPTR     typ2;
    SYM_HANDLE  sym2_handle;
    SYM_ENTRY   sym2;
    TREEPTR     opnd;
    TREEPTR     value;
    int         token;

    typ2 = typ->object;
    while( typ2->decl_type == TYPE_TYPEDEF ) typ2 = typ2->object;
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
                SetSegAlign( &sym2 );                     /* 02-feb-92 */
            }
            SymReplace( &sym2, sym2_handle );
            GenStaticDataQuad( sym2_handle );
            InitWCharArray( typ );
            AssignAggregate( VarLeaf( sym, sym_handle ),
                             VarLeaf( &sym2, sym2_handle ), typ );
        } else {
            n = typ->u.array->dimension;
            i = 0;
            for(;;) {   // accept some c++ { {1},.. }
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
                if( i == n ){
                    CErr1( ERR_TOO_MANY_INITS );
               }
            }
            if( n == 0 ){
                typ->u.array->dimension = i;
            }else{
                while( i < n ){
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
    case TYPE_STRUCT:
    case TYPE_UNION:
        if( SimpleStruct( typ2 ) ) {
            unsigned base;
            unsigned size;

            NextToken();                    // skip over T_LEFT_BRACE
            n = typ->u.array->dimension;
            i = 0;
            base = 0;
            size = SizeOfArg( typ2 );
            for(;;) {
                token = CurToken;
                if( token == T_LEFT_BRACE )  NextToken();
                InitStructVar( base, sym, sym_handle, typ2 );
                if( token == T_LEFT_BRACE )  MustRecog( T_RIGHT_BRACE );
                ++i;
                if( CurToken == T_EOF ) break;
                if( CurToken == T_RIGHT_BRACE )break;
                MustRecog( T_COMMA );
                if( CurToken == T_RIGHT_BRACE )break;
                if( i == n ){
                    CErr1( ERR_TOO_MANY_INITS );
               }
               base += size;
            }
            if( n == 0 ){
                typ->u.array->dimension = i;
            }else{
                while( i < n ){ // mop up
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
        SrcLineNum = TokenLine;         /* 15-mar-88 */
        SrcFno   = TokenFno;
        typ = sym->sym_type;
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
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

