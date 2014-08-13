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
* Description:  enum statement processing
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include <stddef.h>
#include <limits.h>
#include "i64.h"

void EnumInit( void )
{
    id_hash_idx   h;

    for( h = 0; h < ID_HASH_SIZE; h++ ) {
        EnumTable[h] = NULL;
    }
    EnumRecSize = 0;
}


local ENUMPTR EnumLkAdd( TAGPTR tag )
{
    ENUMPTR     esym;
    size_t      len;

    VfyNewSym( HashValue, Buffer );
    len = sizeof( ENUMDEFN ) + TokenLen;
    if( len > EnumRecSize )
        EnumRecSize = len;
    esym = (ENUMPTR)CPermAlloc( len );
    memcpy( esym->name, Buffer, TokenLen + 1 );
    esym->parent = tag;
    esym->hash = HashValue;
    esym->src_loc = TokenLoc;
    esym->next_enum = EnumTable[esym->hash];
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
    ENUM_UNDEF = -1,
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

struct { DATA_TYPE decl_type; target_size size; } ItypeTable[ENUM_SIZE] =
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

void get_msg_range( char *buff, enum enum_rng index )
{
    if( index & 1 ) {
        sprintf( buff, "%llu to %llu", RangeTable[index][LOW].u._64[0], RangeTable[index][HIGH].u._64[0] );
    } else {
        sprintf( buff, "%lld to %lld", (long long)RangeTable[index][LOW].u._64[0], (long long)RangeTable[index][HIGH].u._64[0] );
    }
}

TYPEPTR EnumDecl( type_modifiers flags )
{
    TYPEPTR     typ;
    TAGPTR      tag;

    flags = flags;
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
                if( typ->decl_type != TYPE_ENUM ) {
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
        enum enum_rng   start_index;
        enum enum_rng   step;
        enum enum_rng   error;
        uint64          n;
        uint64          Inc;
        bool            minus;
        bool            has_sign;
        ENUMPTR         *prev_lnk;
        ENUMPTR         esym;
        source_loc      error_loc;
        char            buff[50];

        if( CompFlags.make_enums_an_int ) {
            start_index = ENUM_INT;
        } else {
            start_index = ENUM_S8;
        }
        const_index = ENUM_UNDEF;
        NextToken();
        if( CurToken == T_RIGHT_BRACE ) {
            CErr1( ERR_EMPTY_ENUM_LIST );
        }
        U32ToU64( 1, &Inc );
        U64Clear( n );
        minus = FALSE;
        has_sign = FALSE;
        step = 1;
        prev_lnk = &esym;
        esym = NULL;
        while( CurToken == T_ID ) {
            esym = EnumLkAdd( tag );
            *prev_lnk = esym;
            prev_lnk = &esym->thread;
            error_loc = TokenLoc;
            NextToken();
            if( CurToken == T_EQUAL ) {
                NextToken();
                error_loc = TokenLoc;
                ConstExprAndType( &val );
                switch( val.type ) {
                case TYPE_ULONG:
                case TYPE_UINT:
                case TYPE_ULONG64:
                    minus = FALSE;
                    break;
                default:
                    if( val.value.u.sign.v ) {
                        minus = TRUE;
                        step = 2;
                    } else {
                        minus = FALSE;
                    }
                    break;
                }
                n = val.value;
            } else if( has_sign ) {
                if( n.u.sign.v ) {
                    minus = TRUE;
                } else {
                    minus = FALSE;
                }
            }
            for( index = start_index; index < ENUM_SIZE; index += step ) {
                if( minus ) {
                    if( I64Cmp( &n, &( RangeTable[index][LOW] ) ) >= 0 ) {
                        break;
                    }
                } else {
                    if( U64Cmp( &n, &( RangeTable[index][HIGH]) ) <= 0 ) {
                        break;
                    }
                }
            }
            error = ENUM_UNDEF;
            if( !CompFlags.extensions_enabled && ( index > ENUM_INT )) {
                error = ENUM_INT;
            }
            if( index >= ENUM_SIZE ) {
                // overflow signed maximum range
                if( error == ENUM_UNDEF ) {
                    error = const_index;
                }
            } else if(( const_index == ENUM_SIZE - 1 ) && minus ) {
                // overflow unsigned maximum range by any negative signed value
                if( error == ENUM_UNDEF )
                    error = const_index;
                step = 1;
            } else {
                if( !has_sign && minus) {
                    has_sign = TRUE;
                    if( index < const_index ) {
                        // round up to signed
                        index = ( const_index + 1 ) & ~1;
                    }
                }
                if( index > const_index ) {
                    const_index = index;
                    typ->object = GetType( ItypeTable[const_index].decl_type );
                    tag->size = ItypeTable[const_index].size;
                }
            }
            if( error != ENUM_UNDEF ) {
                SetErrLoc( &error_loc );
                get_msg_range( buff, error );
                CErr2p( ERR_ENUM_CONSTANT_OUT_OF_RANGE, buff );
                InitErrLoc();
            }
            esym->value = n;
            EnumTable[esym->hash] = esym;
            if( CurToken == T_RIGHT_BRACE )
                break;
            U64Add( &n, &Inc, &n );
            MustRecog( T_COMMA );
            if( !CompFlags.extensions_enabled
              && !CompFlags.c99_extensions
              && ( CurToken == T_RIGHT_BRACE )) {
                ExpectIdentifier();
            }
        }
        MustRecog( T_RIGHT_BRACE );
    }
    return( typ );
}


ENUMPTR EnumLookup( id_hash_idx h, const char *name )
{
    ENUMPTR     esym;
    size_t      len;

    len = strlen( name ) + 1;
    for( esym = EnumTable[h]; esym != NULL; esym = esym->next_enum ) {
        if( memcmp( esym->name, name, len ) == 0 ) {
            break;
        }
    }
    return( esym );
}


void FreeEnums( void )
{
    ENUMPTR         esym;
    id_hash_idx     h;

    for( h = 0; h < ID_HASH_SIZE; h++ ) {
        for( ; (esym = EnumTable[h]) != NULL; EnumTable[h] = esym->next_enum ) {
            if( !ChkEqSymLevel( esym->parent ) ) {
                break;
            }
        }
    }
}

#ifndef NDEBUG

void DumpEnumTable( void )
{
    ENUMPTR         esym;
    id_hash_idx     h;

    puts( "ENUM TABLE DUMP" );
    for( h = 0; h < ID_HASH_SIZE; h++ ) {
        for( esym = EnumTable[h]; esym != NULL; esym = esym->next_enum ) {
            if( ChkEqSymLevel( esym->parent ) ) {
                printf( "%s = %lld\n", esym->name, esym->value.u._64[0] );
            }
        }
        printf( "---------%d----------\n", h );
    }
}
#endif
