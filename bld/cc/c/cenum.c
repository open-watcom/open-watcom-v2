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


#include "cvars.h"
#include "cgswitch.h"
#include <stddef.h>
#include <limits.h>
#include "i64.h"

void EnumInit()
{
    int i;

    for( i=0; i < ENUM_HASH_SIZE; i++ ) {
        EnumTable[i] = NULL;
    }
    EnumRecSize = 0;
}


local ENUM_HANDLE EnumLkAdd( TAGPTR tag )
{
    ENUMPTR     esym;
    int         len;

    VfyNewSym( HashValue, Buffer );
    len = sizeof(ENUMDEFN) + strlen(Buffer);
    if( len > EnumRecSize ) EnumRecSize = len;
    esym = (ENUMPTR) CPermAlloc( len );
    strcpy( esym->name, Buffer );
    esym->parent = tag;
    esym->hash = HashValue;
    esym->next_enum = EnumTable[ esym->hash ];
    ++EnumCount;
    if( tag->u.enum_list == NULL ) {
        tag->u.enum_list = esym;
    }
#if 0
    if( CompFlags.emit_browser_info ) {
        esym->xref = NewXref( NULL );
    }
#endif
    return( esym );
}
#if defined( WATCOM_BIG_ENDIAN )
#   define i64val(h,l) { h, l }
#else
#   define i64val(h,l) { l, h }
#endif
enum enum_rng {
    ENUM_S8,
    ENUM_U8,
    ENUM_S16,
#if TARGET_INT == 2
    ENUM_INT = ENUM_S16,
#endif
    ENUM_U16,
    ENUM_S32,
#if TARGET_INT == 4
    ENUM_INT = ENUM_S32,
#endif
    ENUM_U32,
    ENUM_S64,
    ENUM_U64,
    ENUM_SIZE,
};
enum low_high {
    LOW  =  0,
    HIGH =  1,
};
static uint64 const RangeTable[ENUM_SIZE][2] =
{ //  low                              high
    { i64val( 0xFFFFFFFF, 0xFFFFFF80 ),i64val( 0x00000000, 0x0000007F ) },//s8
    { i64val( 0x00000000, 0x00000000 ),i64val( 0x00000000, 0x000000FF ) },//u8
    { i64val( 0xFFFFFFFF, 0xFFFF8000 ),i64val( 0x00000000, 0x00007FFF ) },//s16
    { i64val( 0x00000000, 0x00000000 ),i64val( 0x00000000, 0x0000FFFF ) },//u16
    { i64val( 0xFFFFFFFF, 0x80000000 ),i64val( 0x00000000, 0x7FFFFFFF ) },//s32
    { i64val( 0x00000000, 0x00000000 ),i64val( 0x00000000, 0xFFFFFFFF ) },//u32
    { i64val( 0x80000000, 0x00000000 ),i64val( 0x7FFFFFFF, 0xFFFFFFFF ) },//s64
    { i64val( 0x00000000, 0x00000000 ),i64val( 0xFFFFFFFF, 0xFFFFFFFF ) },//u64
};
static uint64 const Inc[1] = i64val( 0x00000000, 0x00000001 );
struct { DATA_TYPE decl_type; int size; } ItypeTable[ENUM_SIZE] =
{
    { TYPE_CHAR, TARGET_CHAR  },    //S8
    { TYPE_UCHAR,TARGET_CHAR  },    //U8
#if TARGET_INT == 2
    { TYPE_INT,  TARGET_INT  },     //S16
    { TYPE_UINT, TARGET_INT  },     //U16
#else
    { TYPE_SHORT, TARGET_SHORT },   //S16
    { TYPE_USHORT,TARGET_SHORT },   //U16
#endif
#if TARGET_INT == 4
    { TYPE_INT,   TARGET_INT  },    //S32
    { TYPE_UINT,  TARGET_INT  },    //U32
#else
    { TYPE_LONG,  TARGET_LONG },    //S32
    { TYPE_ULONG, TARGET_LONG },    //U32
#endif
    { TYPE_LONG64, TARGET_LONG64  },//S64
    { TYPE_ULONG64, TARGET_LONG64 },//U64
};


TYPEPTR EnumDecl( int flags )
{
    TYPEPTR     typ;
    TAGPTR      tag;


    NextToken();
    if( CurToken == T_ID ) {
        /* could be: (1) "enum" <id> ";"
                     (2) "enum" <id> <variable_name> ";"
                     (3) "enum" <id> "{" <enum_const_decl> ... "}"
        */
        tag = TagLookup();
        NextToken();
        if( CurToken != T_LEFT_BRACE ) {
            typ = tag->sym_type;
            if( typ == NULL ) {
                CErr1( ERR_INCOMPLETE_ENUM_DECL );
                typ = TypeDefault();
            } else {
                if( typ->decl_type != TYPE_ENUM ) {         /* 18-jan-89 */
                    CErr2p( ERR_DUPLICATE_TAG, tag->name );
                }
                typ->u.tag = tag;
            }
            return( typ );
        }
        tag = VfyNewTag( tag, TYPE_ENUM );
    } else {
        tag = NullTag();
    }
    typ = TypeNode( TYPE_ENUM, GetType( TYPE_INT ) );
    typ->u.tag = tag;
    tag->sym_type = typ;
    tag->size = TARGET_INT;
    tag->u.enum_list = NULL;
    if( CurToken == T_LEFT_BRACE ) {
        const_val       val;
        enum enum_rng   index;
        enum enum_rng   const_index;
        uint64          n;
        bool            sign;
        bool            minus;
        ENUM_HANDLE    *prev_lnk;
        ENUM_HANDLE     esym;

        if( CompFlags.make_enums_an_int ) {
            index = ENUM_INT;
        }else{
            index = ENUM_S8;
        }
        const_index = ENUM_INT;
        NextToken();
        if( CurToken == T_RIGHT_BRACE ) {
            CErr1( ERR_EMPTY_ENUM_LIST );
        }
        U32ToU64( 0, &n );
        sign = FALSE;
        minus = FALSE;
        prev_lnk = &esym;
        esym = NULL;
        while( CurToken == T_ID ) {
            esym = EnumLkAdd( tag );
            *prev_lnk = esym;
            prev_lnk = &esym->thread;
            NextToken();
            if( CurToken == T_EQUAL ) {
                NextToken();
                ConstExprAndType( &val );
                switch( val.type ){
                default:
                    I32ToI64( val.val32, &val.val64 );
                case TYPE_LONG64:
                    if( val.val64.u.sign.v ){
                        minus = TRUE;
                        sign = TRUE;
                    }else{
                        minus = FALSE;
                    }
                    break;
                case TYPE_ULONG:
                case TYPE_UINT:
                    U32ToU64( val.val32, &val.val64 );
                case TYPE_ULONG64:
                    minus = FALSE;
                    break;
                }
                n = val.val64;
            }
            if( sign ) {
                if( index & 1 ) ++index;
                for( ; index < ENUM_SIZE; index += 2 ) {
                    if( minus ){
                        if( I64Cmp( &n, &(RangeTable[ index ][LOW] )) >= 0 ) break;
                    }else{
                        if( U64Cmp( &n, &(RangeTable[ index ][HIGH]) ) <= 0 ) break;
                    }
                }
                if( index == ENUM_SIZE ) {
                    CErr1( ERR_ENUM_CONSTANT_TOO_LARGE );
                }
            } else {
                for( ; index < ENUM_SIZE; index += 1 ) {
                    if( U64Cmp( &n, &(RangeTable[ index ][HIGH]) ) <= 0 ) break;
                }
            }
            EnumTable[ esym->hash ] = esym;             /* 08-nov-94 */
            if( index > const_index ){ // change type of enum to fit const
                if( CompFlags.extensions_enabled  ) {
                    typ->object = GetType( ItypeTable[index].decl_type );
                    tag->size   = ItypeTable[index].size;
                }else{
                    CErr1( ERR_ENUM_CONSTANT_TOO_LARGE );
                }
                const_index = index;
            }
            esym->value = n;
            if( CurToken == T_RIGHT_BRACE ) break;
            U64Add( &n,Inc,&n);
            MustRecog( T_COMMA );
            if( CurToken == T_RIGHT_BRACE ) {
                if( !CompFlags.extensions_enabled ) {
                    ExpectIdentifier();         /* 13-may-91 */
                }
            }
        }
        MustRecog( T_RIGHT_BRACE );
        typ->object = GetType( ItypeTable[index].decl_type );
        tag->size   = ItypeTable[index].size;
    }
    return( typ );
}




int EnumLookup( int hash_value, char *name, struct enum_info *eip )
{
    ENUMPTR     esym;

    for( esym = EnumTable[hash_value]; esym; ) {
        if( strcmp( esym->name, name ) == 0 ) {
            eip->value = esym->value;
            eip->parent = esym->parent;
            eip->level = esym->parent->level;
//          eip->enum_entry = esym;
            return( 1 );            /* indicate ENUM was found */
        }
        esym = esym->next_enum;
    }
//  eip->enum_entry = NULL;
    eip->level = -1;                /* indicate not found */
    return( 0 );                    /* indicate this was not an ENUM */
}


void FreeEnums()
{
    ENUMPTR     esym;
    int         i;

    for( i = 0; i < ENUM_HASH_SIZE; i++ ) {
        for( ; esym = EnumTable[i]; ) {
            if( esym->parent->level != SymLevel ) break;
            EnumTable[i] = esym->next_enum;
        }
    }
}

#ifndef NDEBUG

void DumpEnumTable()
{
    ENUMPTR     esym;
    int         i;

    puts( "ENUM TABLE DUMP" );
    for( i=0; i < ENUM_HASH_SIZE; i++ ) {
        for( esym = EnumTable[i]; esym; ) {
            if( esym->parent->level == SymLevel ) {
                printf( "%s = %d\n", esym->name, esym->value );
            }
            esym = esym->next_enum;
        }
        printf( "---------%d----------\n", i );
    }
}
#endif
