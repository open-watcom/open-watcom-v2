/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Watcom debugging information type support.
*
****************************************************************************/


#include "dipwat.h"
#include "wattype.h"
#include "walloca.h"
#include "watlcl.h"
#include "watgbl.h"

#include "clibext.h"


#define NEXT_TYPE( p )  ((p) + GETU8(p))

typedef struct typeinfo {
    const char          *start;
    const char          *end;
    word                entry;
    imp_mod_handle      imh;
    struct typeinfo     *prev;
} typeinfo;

static typeinfo *Type;

static void PushLoad( typeinfo *new )
{
    new->prev = Type;
    new->start = NULL;
    Type = new;
}

static void FreeLoad( void )
{
    if( Type->start != NULL ) {
        InfoSpecUnlock( Type->start );
        Type->start = NULL;
    }
}

static void PopLoad( void )
{
    FreeLoad();
    Type = Type->prev;
}

void KillTypeLoadStack( void )
{
    Type = NULL;
}

static dip_status LoadType( imp_image_handle *iih, imp_mod_handle imh, word entry )
{
    FreeLoad();
    Type->start = InfoLoad( iih, imh, DMND_TYPES, entry, NULL );
    if( Type->start == NULL ) {
        return( DS_FAIL );
    }
    Type->end = Type->start + InfoSize( iih, imh, DMND_TYPES, entry );
    Type->entry = entry;
    Type->imh = imh;
    return( DS_OK );
}

static const char *NamePtr( const char *p )
{
    unsigned    index;

    switch( GETU8( p + 1 ) ) {
    case NAME_TYPE | TYPE_SCALAR:
        p += 3;
        break;
    case NAME_TYPE | TYPE_SCOPE:
        p += 2;
        break;
    case NAME_TYPE | TYPE_NAME:
        p = GetIndex( p + 2, &index );
        p = GetIndex( p, &index );
        break;
    case ENUM_TYPE | ENUM_CONST_BYTE:
        p += 3;
        break;
    case ENUM_TYPE | ENUM_CONST_WORD:
        p += 4;
        break;
    case ENUM_TYPE | ENUM_CONST_LONG:
        p += 6;
        break;
    case ENUM_TYPE | ENUM_CONST_I64:
        p += 10;
        break;
    case STRUCT_TYPE | ST_BIT_BYTE:
        p += 2;
        /* fall through */
    case STRUCT_TYPE | ST_FIELD_BYTE:
        p += 3;
        p = GetIndex( p, &index );
        break;
    case STRUCT_TYPE | ST_BIT_WORD:
        p += 2;
        /* fall through */
    case STRUCT_TYPE | ST_FIELD_WORD:
        p += 4;
        p = GetIndex( p, &index );
        break;
    case STRUCT_TYPE | ST_BIT_LONG:
        p += 2;
        /* fall through */
    case STRUCT_TYPE | ST_FIELD_LONG:
        p += 6;
        p = GetIndex( p, &index );
        break;
    case STRUCT_TYPE | ST_FIELD_LOC:
        p = SkipLocation( p + 3 );
        p = GetIndex( p, &index );
        break;
    case STRUCT_TYPE | ST_BIT_LOC:
        p = SkipLocation( p + 3 ) + 2;
        p = GetIndex( p, &index );
        break;
    default:
        p += GETU8( p );
        break;
    }
    return( p );
}

static const char *BaseTypePtr( const char *p )
{
    byte        kind;
    byte        subkind;
    unsigned    index;

    kind = GETU8( p + 1 );
    subkind = kind & SUBCLASS_MASK;
    switch( kind & CLASS_MASK ) {
    case NAME_TYPE:
        if( subkind == TYPE_NAME ) {
            p = GetIndex( p, &index );
            return( p );
        }
        break;
    case ARRAY_TYPE:
        {
            static const byte Incr[] = { 3, 4, 6, 0, 8, 10 };

            if( subkind == ARRAY_TYPE_INDEX ) {
                p = GetIndex( p + 2, &index );
            } else {
                p += Incr[subkind];
            }
        }
        return( p );
    case SUBRANGE_TYPE:
        {
            static const byte Incr[] = { 4, 6, 10 };

            p += Incr[subkind];
        }
        return( p );
    case POINTER_TYPE:
        return( p + 2 );
    case STRUCT_TYPE:
        switch( subkind ) {
        case STRUCT_LIST:
            return( NULL );
        case ST_BIT_BYTE:
            p += 2;
            /* fall through */
        case ST_FIELD_BYTE:
            p += 3;
            break;
        case ST_BIT_WORD:
            p += 2;
            /* fall through */
        case ST_FIELD_WORD:
            p += 4;
            break;
        case ST_BIT_LONG:
            p += 2;
            /* fall through */
        case ST_FIELD_LONG:
            p += 6;
            break;
        case ST_FIELD_LOC:
            p = SkipLocation( p + 3 );
            break;
        case ST_BIT_LOC:
            p = SkipLocation( p + 3 ) + 2;
            break;
        case ST_INHERIT:
            p = SkipLocation( p + 2 );
            break;
        }
        return( p );
     case PROC_TYPE:
         return( p + 2 );
    }
    return( NULL );
}

static unsigned long GetScalar( address addr, unsigned scalar )
{
    location_list       src_ll;
    location_list       dst_ll;
    union {
        unsigned_8      u8;
        signed_8        s8;
        unsigned_16     u16;
        signed_16       s16;
        unsigned_32     u32;
        signed_32       s32;
    }                   tmp;

    LocationCreate( &src_ll, LT_ADDR, &addr );
    LocationCreate( &dst_ll, LT_INTERNAL, &tmp );
    DCAssignLocation( &dst_ll, &src_ll, (scalar & SCLR_LEN_MASK) + 1 );
    switch( scalar ) {
    case SCLR_UNSIGNED | 0:
        return( tmp.u8 );
    case SCLR_UNSIGNED | 1:
        return( tmp.u16 );
    case SCLR_UNSIGNED | 3:
        return( tmp.u32 );
    case SCLR_INTEGER  | 0:
        return( tmp.s8 );
    case SCLR_INTEGER  | 1:
        return( tmp.s16 );
    case SCLR_INTEGER  | 3:
        return( tmp.s32 );
    }
    return( 0 );
}

#define TYPE_CACHE_SIZE 64              // must be power of 2

typedef struct {
    imp_image_handle    *iih;
    imp_mod_handle      imh;
    unsigned            index;
    struct type_pos     t;
} type_cache;

static type_cache       TypeCache[TYPE_CACHE_SIZE];
static int              TypeCacheRover;

static dip_status FindTypeCache( imp_image_handle *iih, imp_mod_handle imh,
                        unsigned index, imp_type_handle *ith )
{
    int         i;
    type_cache  *cache;

    for( i = 0; i < TYPE_CACHE_SIZE; ++i ) {
        cache = TypeCache + i;
        if( cache->iih == iih && cache->imh == imh && cache->index == index ) {
            ith->imh = imh;
            ith->f.all = 0;
            ith->t.entry = cache->t.entry;
            ith->t.offset = cache->t.offset;
            return( LoadType( iih, imh, ith->t.entry ) );
        }
    }
    return( DS_FAIL );
}


static void SetTypeCache( imp_image_handle *iih, imp_mod_handle imh,
                        unsigned index, imp_type_handle *ith )
{
    type_cache  *cache;

    cache = TypeCache + TypeCacheRover++;
    TypeCacheRover &= TYPE_CACHE_SIZE - 1;
    cache->iih = iih;
    cache->imh = imh;
    cache->index = index;
    cache->t.entry = ith->t.entry;
    cache->t.offset = ith->t.offset;
}


void ClearTypeCache( imp_image_handle *iih )
{
    int         i;

    for( i = 0; i < TYPE_CACHE_SIZE; ++i ) {
        if( TypeCache[i].iih == iih ) {
            TypeCache[i].iih = NULL;
        }
    }
}


static dip_status FindRawTypeHandle( imp_image_handle *iih, imp_mod_handle imh,
                unsigned index, imp_type_handle *ith )
{
    const char  *p;
    byte        kind;
    word        entry;
    unsigned    count;

    if( index == 0 )
        return( DS_FAIL );
    if( FindTypeCache( iih, imh, index, ith ) == DS_OK ) {
        return( DS_OK );
    }
    count = index;
    for( entry = 0; LoadType( iih, imh, entry ) == DS_OK; entry++ ) {
        for( p = Type->start; p < Type->end; p = NEXT_TYPE( p ) ) {
            kind = GETU8( p + 1 );
            if( kind == (NAME_TYPE | TYPE_EOF) ) {
                FreeLoad();
                return( DS_FAIL );
            }
            switch( kind & CLASS_MASK ) {
            case ENUM_TYPE:
                if( (kind & SUBCLASS_MASK) == ENUM_LIST ) {
                    --count;
                }
                break;
            case STRUCT_TYPE:
                if( (kind & SUBCLASS_MASK) == STRUCT_LIST ) {
                    --count;
                }
                break;
            case NAME_TYPE:
                if( (kind & SUBCLASS_MASK) != TYPE_CUE_TABLE ) {
                    --count;
                }
                break;
            default:
                --count;
                break;
            }
            if( count == 0 ) {
                ith->imh = imh;
                ith->f.all = 0;
                ith->t.entry = entry;
                ith->t.offset = p - Type->start;
                SetTypeCache( iih, imh, index, ith );
                return( DS_OK );
            }
        }
    }
    return( DS_FAIL );
}

static int CharName( const char *name, unsigned len )
{
    if( len > 4 ) {
        name += len - 5;
        if( name[0] != ' ' )
            return( 0 );
        ++name;
    }
    if( memcmp( name, "char", 4 ) == 0 )
        return( 1 );
    return( 0 );
}


static byte GetRealTypeHandle( imp_image_handle *iih, imp_type_handle *ith )
{
    const char  *p;
    const char  *start;
    unsigned    index;
    byte        is_char;
    unsigned    len;
    byte        kind;

    ith->f.s.chr = 0;
    is_char = 1;
    for( ;; ) {
        p = Type->start + ith->t.offset;
        kind = GETU8( p + 1 );
        if( kind != (NAME_TYPE | TYPE_NAME) ) {
            if( is_char
              && GETU8( p ) >= 7
              && kind == (NAME_TYPE | TYPE_SCALAR)
              && CharName( p + 3, GETU8( p ) - 3 ) ) {
                ith->f.s.chr = 1;
            }
            return( kind );
        }
        len = GETU8( p );
        start = p;
        p = GetIndex( p + 2, &index ); /* scope index */
        p = GetIndex( p, &index );
        len -= (p - start);
        if( !(is_char && len >= 4 && CharName( p, len )) ) {
            is_char = 0;
        }
        if( FindRawTypeHandle( iih, ith->imh, index, ith ) != DS_OK )
            return( NO_TYPE );
        if( is_char )
            ith->f.s.chr = 1;
        is_char = 0;
    }
}

static dip_status DoFindTypeHandle( imp_image_handle *iih, imp_mod_handle imh,
                unsigned index, imp_type_handle *ith )
{
    byte                type;
    const char          *p;
    imp_type_handle     base_ith;
    dip_status          ds;

    ds = FindRawTypeHandle( iih, imh, index, ith );
    if( ds == DS_OK ) {
        type = GetRealTypeHandle( iih, ith );
        switch( type & CLASS_MASK ) {
        case NO_TYPE:
            ds = DS_FAIL;
            break;
        case ARRAY_TYPE:
            if( stricmp( ImpInterface.ModSrcLang( iih, imh ), "fortran" ) == 0 ) {
                ith->f.s.col_major = 1;
                ith->f.s.array_ss = 0;
                base_ith = *ith;
                for( ;; ) {
                    p = Type->start + base_ith.t.offset;
                    if( (GETU8( p + 1 ) & CLASS_MASK) != ARRAY_TYPE )
                        break;
                    p = BaseTypePtr( p );
                    GetIndex( p, &index );
                    FindRawTypeHandle( iih, base_ith.imh, index, &base_ith );
                    ith->f.s.array_ss++;
                }
            }
            break;
        }
    }
    return( ds );
}

dip_status FindTypeHandle( imp_image_handle *iih, imp_mod_handle imh,
                unsigned index, imp_type_handle *ith )
{
    typeinfo            typeld;
    dip_status          ds;

    PushLoad( &typeld );
    ds = DoFindTypeHandle( iih, imh, index, ith );
    PopLoad();
    return( ds );
}

typedef enum {
    ET_ENUM = 1,
    ET_TYPE = 2
} type_or_enum;

#define SCOPESDEF() \
    pick( STRUCT_IDX,   "struct" ) \
    pick( CLASS_IDX,    "class" ) \
    pick( UNION_IDX,    "union" ) \
    pick( ENUM_IDX,     "enum" )

enum {
    #define pick(e,t)   e,
    SCOPESDEF()
    #undef pick
    NUM_IDX
};

static const char *Scopes[] = {
    #define pick(e,t)   t,
    SCOPESDEF()
    #undef pick
};


struct name_state {
    unsigned    scope_idx[NUM_IDX];
    unsigned    curr_idx;
    unsigned    hit_eof         : 1;
    const char  *header;
};

static void InitNameState( struct name_state *state )
{
    memset( state, 0, sizeof( *state ) );
}

static const char *FindAName( struct name_state *state, const char *p,
                        type_or_enum which, lookup_item *li )
{
    const char  *name;
    size_t      len;
    const char  *lookup_name;
    size_t      lookup_len;
    strcompn_fn *scompn;
    unsigned    index;
    unsigned    i;
    symbol_type type;
    byte        kind;

    scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
    lookup_name = li->name.start;
    lookup_len = li->name.len;
    for( ; p < Type->end; p = NEXT_TYPE( p ) ) {
        state->curr_idx++;
        kind = GETU8( p + 1 );
        if( kind == (NAME_TYPE | TYPE_EOF) ) {
            state->hit_eof = 1;
            break;
        }
        switch( kind & CLASS_MASK ) {
        case ENUM_TYPE:
            if( (kind & SUBCLASS_MASK) == ENUM_LIST ) {
                state->header = p;
            } else {
                state->curr_idx--;
            }
            if( (which & ET_ENUM) == 0 )
                continue;
            break;
        case NAME_TYPE:
            if( (which & ET_TYPE) == 0 )
                continue;
            switch( kind & SUBCLASS_MASK ) {
            case TYPE_SCALAR:
                type = ST_TYPE;
                break;
            case TYPE_SCOPE:
                name = NamePtr( p );
                len = GETU8( p ) - ( name - p );
                for( i = 0; i < NUM_IDX; ++i ) {
                    if( memcmp( name, Scopes[i], len ) == 0 ) {
                        state->scope_idx[i] = state->curr_idx;
                        break;
                    }
                }
                continue;
            case TYPE_NAME:
                GetIndex( p + 2, &index );
                if( index == 0 ) {
                    type = ST_TYPE;
                } else {
                    for( i = 0; i < NUM_IDX; ++i ) {
                        if( state->scope_idx[i] == index ) {
                            break;
                        }
                    }
                    type = ST_STRUCT_TAG + i;
                }
                break;
            default:
                continue;
            }
            if( type == ST_TYPE && li->type == ST_NONE )
                break;
            if( type == li->type )
                break;
            continue;
        case STRUCT_TYPE:
            if( (kind & SUBCLASS_MASK) == STRUCT_LIST ) {
                state->header = p;
            } else {
                state->curr_idx--;
            }
            continue;
        default:
            continue;
        }
        name = NamePtr( p );
        len = GETU8( p ) - ( name - p );
        if( len == lookup_len && scompn( name, lookup_name, len ) == 0 ) {
            return( p );
        }
    }
    return( NULL );
}

search_result LookupTypeName( imp_image_handle *iih, imp_mod_handle imh,
                lookup_item *li, imp_type_handle *ith )
{
    word                entry;
    struct name_state   state;
    const char          *p;
    typeinfo            typeld;
    search_result       sr;

    sr = SR_NONE;
    PushLoad( &typeld );
    InitNameState( &state );
    for( entry = 0; LoadType( iih, imh, entry ) == DS_OK; entry++ ) {
        p = FindAName( &state, Type->start, ET_TYPE, li );
        if( p != NULL ) {
            ith->imh = imh;
            ith->f.all = 0;
            ith->t.entry = entry;
            ith->t.offset = p - Type->start;
            sr = SR_EXACT;
            break;
        }
        if( state.hit_eof ) {
            break;
        }
    }
    PopLoad();
    return( sr );
}

static search_result SearchEnumTypeName( imp_image_handle *iih, imp_mod_handle imh,
                         lookup_item *li, void *d, type_or_enum which )
{
    word                entry;
    struct name_state   state;
    search_result       sr;
    imp_sym_handle      *ish;
    const char          *p;
    typeinfo            typeld;

    sr = SR_NONE;
    PushLoad( &typeld );
    InitNameState( &state );
    for( entry = 0; LoadType( iih, imh, entry ) == DS_OK; entry++ ) {
        p = FindAName( &state, Type->start, which, li );
        if( p != NULL ) {
            ish = DCSymCreate( iih, d );
            ish->imh = imh;
            ish->name_off = (byte)( NamePtr( p ) - p );
            ish->u.typ.t.entry = entry;
            ish->u.typ.t.offset = p - Type->start;
            if( (GETU8( p + 1 ) & CLASS_MASK) == ENUM_TYPE ) {
                ish->type = SH_CST;
                ish->u.typ.h.offset = state.header - Type->start;
                ish->u.typ.h.entry = entry;
            } else {
                ish->type = SH_TYP;
            }
            /* we really should continue searching for more names that
               match, but we're going to early out because I know that
               the symbolic info format is too weak to have more than
               one type name or enum const that will match */
            sr = SR_EXACT;
            break;
        }
        if( state.hit_eof ) {
            break;
        }
    }
    PopLoad();
    return( sr );
}

search_result SearchEnumName( imp_image_handle *iih, imp_mod_handle imh,
                         lookup_item *li, void *d )
{
    return( SearchEnumTypeName( iih, imh, li, d, ET_ENUM ) );
}

search_result SearchTypeName( imp_image_handle *iih, imp_mod_handle imh,
                         lookup_item *li, void *d )
{
    return( SearchEnumTypeName( iih, imh, li, d, ET_TYPE ) );
}

walk_result DIPIMPENTRY( WalkTypeList )( imp_image_handle *iih, imp_mod_handle imh,
                        DIP_IMP_TYPE_WALKER *wk, imp_type_handle *ith, void *d )
{
    const char  *p;
    byte        kind;
    typeinfo    typeld;
    walk_result wr;

    wr = WR_CONTINUE;
    PushLoad( &typeld );
    ith->imh = imh;
    ith->f.all = 0;
    for( ith->t.entry = 0; LoadType( iih, imh, ith->t.entry ) == DS_OK; ith->t.entry++ ) {
        kind = 0;
        for( p = Type->start; p < Type->end; p = NEXT_TYPE( p ) ) {
            kind = GETU8( p + 1 );
            if( kind == (NAME_TYPE | TYPE_EOF) ) {
                break;
            }
            switch( kind & CLASS_MASK ) {
            case NAME_TYPE:
                switch( kind & SUBCLASS_MASK ) {
                case TYPE_SCOPE:
                case TYPE_CUE_TABLE:
                    continue;
                }
                break;
            case ENUM_TYPE:
                if( (kind & SUBCLASS_MASK) != ENUM_LIST ) {
                    continue;
                }
                break;
            case STRUCT_TYPE:
                if( (kind & SUBCLASS_MASK) != STRUCT_LIST ) {
                    continue;
                }
                break;
            case PROC_TYPE:
                if( (kind & SUBCLASS_MASK) == PROC_EXT_PARMS ) {
                    continue;
                }
                break;
            }
            ith->t.offset = p - Type->start;
            wr = wk( iih, ith, d );
            if( wr != WR_CONTINUE ) {
                break;
            }
        }
        if( wr != WR_CONTINUE || kind == (NAME_TYPE | TYPE_EOF) ) {
            break;
        }
    }
    PopLoad();
    return( wr );
}

static void ScalarInfo( unsigned info, dig_type_info *ti )
{
    ti->size = (info & SCLR_LEN_MASK) + 1;
    switch( info & SCLR_CLASS_MASK ) {
    case SCLR_INTEGER:
        ti->modifier = TM_SIGNED;
        ti->kind = TK_INTEGER;
        break;
    case SCLR_UNSIGNED:
        ti->modifier = TM_UNSIGNED;
        ti->kind = TK_INTEGER;
        break;
    case SCLR_FLOAT:
        ti->kind = TK_REAL;
        break;
    case SCLR_VOID:
        ti->kind = TK_VOID;
        break;
    case SCLR_COMPLEX:
        ti->kind = TK_COMPLEX;
        break;
    }
}


static dip_status GetTypeInfo(imp_image_handle *iih, imp_type_handle *ith,
                    location_context *lc, dig_type_info *ti, unsigned *ndims )
{
    const char          *p;
    byte                subkind;
    imp_type_handle     tmp_ith;
    address             addr;
    array_info          info;
    byte                is_char;
    byte                save_major;
    dip_status          ds;
    unsigned            index;
    unsigned            count;
    unsigned long       max;
    addr_off            offset = 0;
    unsigned            skip;
    typeinfo            typeld;
    location_list       ll;

    ds = DS_OK;
    PushLoad( &typeld );
    ti->kind = TK_NONE;
    ti->size = 0;
    ti->modifier = TM_NONE;
    ti->deref = false;
    if( ith->f.s.gbl ) {
        ti->kind = GblTypeClassify( ith->t.offset );
    } else if( ith->f.s.sclr ) {
        ScalarInfo( ith->t.offset, ti );
    } else {
        is_char = ith->f.s.chr;
        ds = LoadType( iih, ith->imh, ith->t.entry );
        if( ds == DS_OK ) {
            if( !GetRealTypeHandle( iih, ith ) ) {
                ds = DS_FAIL;
            } else {
                p = ith->t.offset + Type->start;
                subkind = GETU8( p + 1 ) & SUBCLASS_MASK;
                switch( GETU8( p + 1 ) & CLASS_MASK ) {
                case NAME_TYPE:
                    if( subkind == TYPE_SCALAR ) {
                        ScalarInfo( GETU8( p + 2 ), ti );
                        if( is_char && ti->kind == TK_INTEGER && ti->size == 1 ) {
                            ti->kind = TK_CHAR;
                        }
                    }
                    break;
                case ARRAY_TYPE:
                    FreeLoad();
                    save_major = ith->f.s.col_major;
                    ith->f.s.col_major = 0;
                    ImpInterface.TypeArrayInfo( iih, ith, lc, &info, NULL );
                    ImpInterface.TypeBase( iih, ith, &tmp_ith, NULL, NULL );
                    GetTypeInfo( iih, &tmp_ith, lc, ti, ndims );
                    ith->f.s.col_major = save_major;
                    if( ndims != NULL )
                        ++*ndims;
                    ti->kind = TK_ARRAY;
                    ti->size *= info.num_elts;
                    ti->modifier = TM_NONE;
                    ti->deref = false;
                    Type->start = NULL;
                    break;
                case SUBRANGE_TYPE:
                    ImpInterface.TypeBase( iih, ith, &tmp_ith, NULL, NULL );
                    ImpInterface.TypeInfo( iih, &tmp_ith, lc, ti );
                    break;
                case POINTER_TYPE:
                    {
#define POINTER_INFO() \
    pick( 2, TM_NEAR,   false ) \
    pick( 4, TM_FAR,    false ) \
    pick( 4, TM_HUGE,   false ) \
    pick( 2, TM_NEAR,   true ) \
    pick( 4, TM_FAR,    true ) \
    pick( 4, TM_HUGE,   true ) \
    pick( 4, TM_NEAR,   false ) \
    pick( 6, TM_FAR,    false ) \
    pick( 4, TM_NEAR,   true ) \
    pick( 6, TM_FAR,    true )

                        static const unsigned char PSize[] = {
                            #define pick(s,m,d)     s,
                            POINTER_INFO()
                            #undef pick
                        };
                        static const type_modifier PMods[] = {
                            #define pick(s,m,d)     m,
                            POINTER_INFO()
                            #undef pick
                        };
                        static const bool PDeref[] = {
                            #define pick(s,m,d)     d,
                            POINTER_INFO()
                            #undef pick
                        };

                        ti->kind = TK_POINTER;
                        ti->size = PSize[subkind];
                        ti->modifier = PMods[subkind];
                        if( PDeref[subkind] ) {
                            ti->deref = true;
                        }
                    }
                    break;
                case ENUM_TYPE:
                    ScalarInfo( GETU8( p + 4 ), ti );
                    ti->kind = TK_ENUM;
                    break;
                case STRUCT_TYPE:
                    if( GETU8( p ) > 4 ) {
                        ti->size = GETU32( p + 4 );
                    } else {
                        max = 0;
                        for( count = GETU16( p + 2 ); count > 0; --count ) {
                            skip = 2;
                            p = NEXT_TYPE( p );
                            switch( GETU8( p + 1 ) ) {
                            case STRUCT_TYPE | ST_BIT_BYTE:
                                skip += 2;
                                /* fall through */
                            case STRUCT_TYPE | ST_FIELD_BYTE:
                                skip += 1;
                                offset = GETU8( p + 2 );
                                break;
                            case STRUCT_TYPE | ST_BIT_WORD:
                                skip += 2;
                                /* fall through */
                            case STRUCT_TYPE | ST_FIELD_WORD:
                                skip += 2;
                                offset = GETU16( p + 2 );
                                break;
                            case STRUCT_TYPE | ST_BIT_LONG:
                                skip += 2;
                                /* fall through */
                            case STRUCT_TYPE | ST_FIELD_LONG:
                                skip += 4;
                                offset = GETU32( p + 2 );
                                break;
                            case STRUCT_TYPE | ST_FIELD_LOC:
                            case STRUCT_TYPE | ST_BIT_LOC:
                            case STRUCT_TYPE | ST_INHERIT:
                                skip = 0;
                                break;
                            }
                            if( skip > 0 ) {
                                GetIndex( p + skip, &index );
                                /* Pulling a trick. We know that the size
                                   field will only be absent in older objects,
                                   and thus will only have one type section per
                                   module. */
                                FindTypeHandle( iih, ith->imh, index, &tmp_ith );
                                GetTypeInfo( iih, &tmp_ith, lc, ti, NULL );
                                offset += ti->size;
                                if( offset > max ) {
                                    max = offset;
                                }
                            }
                        }
                        ti->size = max;
                    }
                    ti->kind = TK_STRUCT;
                    break;
                case PROC_TYPE:
                    {
                        static const char FMods[] = {
                            TM_NEAR,
                            TM_FAR,
                            TM_NEAR,
                            TM_FAR
                        };

                        ti->modifier = FMods[subkind];
                        ti->kind = TK_FUNCTION;
                    }
                    break;
                case CHAR_TYPE:
                    switch( subkind ) {
                    case CHAR_BYTE_LEN:
                        ti->size = GETU8( p + 2 );
                        break;
                    case CHAR_WORD_LEN:
                        ti->size = GETU16( p + 2 );
                        break;
                    case CHAR_LONG_LEN:
                        ti->size = GETU32( p + 2);
                        break;
                    case CHAR_DESC_LEN:
                        GetAddress( iih, p + 3, &addr, 0 );
                        ti->size = GetScalar( addr, GETU8( p + 2 ) );
                        break;
                    case CHAR_DESC386_LEN:
                        GetAddress( iih, p + 3, &addr, 1 );
                        ti->size = GetScalar( addr, GETU8( p + 2 ) );
                        break;
                    case CHAR_DESC_LOC:
                        EvalLocation( iih, lc, p + 3, &ll );
                        ti->size = GetScalar( ll.e[0].u.addr, GETU8( p + 2 ) );
                        break;
                    }
                    ti->kind = TK_STRING;
                }
            }
        }
    }
    PopLoad();
    return( ds );
}

dip_status DIPIMPENTRY( TypeInfo )(imp_image_handle *iih, imp_type_handle *ith,
                        location_context *lc, dig_type_info *ti )
{
    return( GetTypeInfo( iih, ith, lc, ti, NULL ) );
}

dip_status DIPIMPENTRY( TypeBase )(imp_image_handle *iih, imp_type_handle *ith,
                 imp_type_handle *base_ith,
                 location_context *lc, location_list *ll )
{
    unsigned    index;
    const char  *p;
    dip_status  ds;
    typeinfo    typeld;

    /* unused parameters */ (void)lc; (void)ll;

    PushLoad( &typeld );
    *base_ith = *ith;
    ds = LoadType( iih, ith->imh, ith->t.entry );
    if( ds == DS_OK ) {
        p = ith->t.offset + Type->start;
        if( base_ith->f.s.col_major ) {
            if( base_ith->f.s.array_ss > 1 ) {
                base_ith->f.s.array_ss--;
            } else {
                base_ith->f.s.col_major = 0;
                do {
                    p = BaseTypePtr( p );
                    GetIndex( p, &index );
                    FindRawTypeHandle( iih, base_ith->imh, index, base_ith );
                    p = base_ith->t.offset + Type->start;
                } while( (GETU8( p + 1 ) & CLASS_MASK) == ARRAY_TYPE );
            }
        } else if( GETU8( p + 1 ) == (ENUM_TYPE | ENUM_LIST) ) {
            base_ith->t.offset = GETU8( p + 4 );
            base_ith->f.s.sclr = 1;
        } else {
            p = BaseTypePtr( p );
            if( p == NULL ) {
                ds = DS_FAIL;
            } else {
                GetIndex( p, &index );
                PopLoad();
                FindTypeHandle( iih, ith->imh, index, base_ith );
                return( ds );
            }
        }
    }
    PopLoad();
    return( ds );
}

dip_status DIPIMPENTRY( TypeArrayInfo )(imp_image_handle *iih, imp_type_handle *ith,
             location_context *lc, array_info *ai, imp_type_handle *index_ith )
{
    const char          *p;
    unsigned            index_idx;
    unsigned            scalar;
    imp_type_handle     tmp_ith;
    address             addr;
    byte                is_32;
    long                hi = 0;
    dig_type_info       ti;
    unsigned            count;
    dip_status          ds;
    typeinfo            typeld;

    if( ith->f.s.col_major ) {
        tmp_ith = *ith;
        tmp_ith.f.s.col_major = 0;
        for( count = tmp_ith.f.s.array_ss-1; count != 0; --count ) {
            ImpInterface.TypeBase( iih, &tmp_ith, &tmp_ith, NULL, NULL );
        }
        ImpInterface.TypeArrayInfo( iih, &tmp_ith, lc, ai, index_ith );
        ai->num_dims = tmp_ith.f.s.array_ss - ai->num_dims + 1;
        ai->column_major = 1;
        return( DS_OK );
    }
    PushLoad( &typeld );
    ds = LoadType( iih, ith->imh, ith->t.entry );
    if( ds == DS_OK ) {
        is_32 = 1;
        index_idx = 0;
        scalar = SCLR_UNSIGNED | 3;
        p = ith->t.offset + Type->start;
        switch( GETU8( p + 1 ) ) {
        case ARRAY_TYPE | ARRAY_BYTE_INDEX:
            ai->num_elts = GETU8( p + 2 ) + 1;
            ai->low_bound = 0;
            break;
        case ARRAY_TYPE | ARRAY_WORD_INDEX:
            ai->num_elts = GETU16( p + 2 ) + 1;
            ai->low_bound = 0;
            break;
        case ARRAY_TYPE | ARRAY_LONG_INDEX:
            ai->num_elts = GETU32( p + 2 ) + 1;
            ai->low_bound = 0;
            break;
        case ARRAY_TYPE | ARRAY_TYPE_INDEX:
            GetIndex( p + 2, &index_idx );
            ds = FindTypeHandle( iih, ith->imh, index_idx, &tmp_ith );
            if( ds == DS_OK ) {
                ds = LoadType( iih, tmp_ith.imh, tmp_ith.t.entry );
                if( ds == DS_OK ) {
                    p = tmp_ith.t.offset + Type->start;
                    switch( GETU8( p + 1 ) ) {
                    case SUBRANGE_TYPE | SUBRANGE_BYTE:
                        ai->low_bound = GETS8( p + 2 );
                        hi = GETS8( p + 3 );
                        break;
                    case SUBRANGE_TYPE | SUBRANGE_WORD:
                        ai->low_bound = GETS16( p + 2 );
                        hi = GETS16( p + 4 );
                        break;
                    case SUBRANGE_TYPE | SUBRANGE_LONG:
                        ai->low_bound = GETS32( p + 2 );
                        hi = GETS32( p + 6 );
                        break;
                    }
                    ai->num_elts = (hi - ai->low_bound) + 1;
                }
            }
            break;
        case ARRAY_TYPE | ARRAY_DESC_INDEX:
            is_32 = 0;
            /* fall through */
        case ARRAY_TYPE | ARRAY_DESC386_INDEX:
            GetAddress( iih, p + 4, &addr, is_32 );
            scalar = GETU8( p + 2 );
            ai->low_bound = GetScalar( addr, scalar );
            addr.mach.offset += (scalar & SCLR_LEN_MASK) + 1;
            ai->num_elts = GetScalar( addr, GETU8( p + 3 ) );
            break;
        }
    }
    PopLoad();
    if( ds == DS_OK ) {
        ImpInterface.TypeBase( iih, ith, &tmp_ith, NULL, NULL );
        ai->num_dims = 1;
        ds = GetTypeInfo( iih, &tmp_ith, lc, &ti, &ai->num_dims );
        if( ds == DS_OK ) {
            ai->stride = ti.size;
            ai->column_major = 0;
            if( index_ith != NULL ) {
                if( index_idx != 0 ) {
                    FindTypeHandle( iih, ith->imh, index_idx, index_ith );
                } else {
                    index_ith->imh = ith->imh;
                    index_ith->f.all = 0;
                    index_ith->f.s.sclr = 1;
                    index_ith->t.offset = scalar;
                }
            }
        }
    }
    return( ds );
}

dip_status DIPIMPENTRY( TypeProcInfo )(imp_image_handle *iih, imp_type_handle *ith,
                 imp_type_handle *parm_ith, unsigned num )
{
    const char  *p;
    const char  *end;
    unsigned    index;
    dip_status  ds;
    typeinfo    typeld;

    PushLoad( &typeld );
    ds = LoadType( iih, ith->imh, ith->t.entry );
    if( ds == DS_OK ) {
        p = Type->start + ith->t.offset;
        end = NEXT_TYPE( p );
        p = GetIndex( p + 2, &index );
        if( num == 0 ) {
            /* nothing to do */
        } else if( num <= GETU8( p ) ) {
            ++p;
            for( ;; ) {
                if( p == end ) {
                    /* handle EXT_PARMS record */
                    p += 2;
                    end = NEXT_TYPE( p );
                }
                p = GetIndex( p, &index );
                --num;
                if( num == 0 ) {
                    break;
                }
            }
        } else {
            ds = DS_NO_PARM;
        }
    }
    PopLoad();
    if( ds == DS_OK )
        return( FindTypeHandle( iih, ith->imh, index, parm_ith ) );
    return( ds );
}

dip_status DIPIMPENTRY( TypePtrAddrSpace )( imp_image_handle *iih,
                    imp_type_handle *ith, location_context *lc, address *addr )
{
    const char          *p;
    const char          *end;
    dip_status          ds;
    location_list       ll;
    unsigned            dummy;
    typeinfo            typeld;

    PushLoad( &typeld );
    ds = LoadType( iih, ith->imh, ith->t.entry );
    if( ds == DS_OK ) {
        p = Type->start + ith->t.offset;
        end = NEXT_TYPE( p );
        p = GetIndex( p + 2, &dummy );
        if( p >= end ) {
            ds = DS_FAIL;
        } else {
            LocationCreate( &ll, LT_ADDR, addr );
            PushBaseLocation( &ll );
            ds = EvalLocation( iih, lc, p, &ll );
        }
    }
    PopLoad();
    if( ds == DS_OK ) {
        if( ll.num != 1 || ll.e[0].type != LT_ADDR ) {
            ds = DS_ERR | DS_BAD_LOCATION;
        } else {
            *addr = ll.e[0].u.addr;
        }
    }
    return( ds );
}

unsigned SymHdl2CstName( imp_image_handle *iih, imp_sym_handle *ish,
                        char *buff, unsigned buff_size )
{
    const char  *p;
    unsigned    len;
    typeinfo    typeld;

    len = 0;
    PushLoad( &typeld );
    if( LoadType( iih, ish->imh, ish->u.typ.t.entry ) == DS_OK ) {
        p = Type->start + ish->u.typ.t.offset;
        len = GETU8( p ) - ish->name_off;
        if( buff_size > 0 ) {
            --buff_size;
            if( buff_size > len )
                buff_size = len;
            memcpy( buff, p + ish->name_off, buff_size );
            buff[buff_size] = '\0';
        }
    }
    PopLoad();
    return( len );
}

unsigned SymHdl2TypName( imp_image_handle *iih, imp_sym_handle *ish,
                        char *buff, unsigned buff_size )
{
    return( SymHdl2CstName( iih, ish, buff, buff_size ) );
}

unsigned SymHdl2MbrName( imp_image_handle *iih, imp_sym_handle *ish,
                        char *buff, unsigned buff_size )
{
    return( SymHdl2CstName( iih, ish, buff, buff_size ) );
}

dip_status SymHdl2CstValue( imp_image_handle *iih, imp_sym_handle *ish, void *d )
{
    const char          *p;
    const char          *e;
    unsigned_64         val;
    dip_status          ds;
    typeinfo            typeld;

    PushLoad( &typeld );
    ds = LoadType( iih, ish->imh, ish->u.typ.t.entry );
    if( ds == DS_OK ) {
        memset( &val, 0, sizeof( val ) );
        p = Type->start + ish->u.typ.t.offset;
        switch( GETU8( p + 1 ) ) {
        case ENUM_TYPE | ENUM_CONST_BYTE:
            val.u._32[0] = GETS8( p + 2 );
            break;
        case ENUM_TYPE | ENUM_CONST_WORD:
            val.u._32[0] = GETS16( p + 2 );
            break;
        case ENUM_TYPE | ENUM_CONST_LONG:
            val.u._32[0] = GETS32( p + 2 );
            break;
        case ENUM_TYPE | ENUM_CONST_I64:
            memcpy( &val, p + 2, sizeof( val ) );
            break;
        }
        e = Type->start + ish->u.typ.h.offset;
        memcpy( d, &val, (GETU8( e + 4 ) & SCLR_LEN_MASK) + 1 );
    }
    PopLoad();
    return( ds );
}

dip_status SymHdl2CstType( imp_image_handle *iih, imp_sym_handle *ish,
                        imp_type_handle *ith )
{
    /* unused parameters */ (void)iih;

    ith->imh = ish->imh;
    ith->t = ish->u.typ.h;
    ith->f.all = 0;
    return( DS_OK );
}

dip_status SymHdl2TypType( imp_image_handle *iih, imp_sym_handle *ish,
                        imp_type_handle *ith )
{
    /* unused parameters */ (void)iih;

    ith->imh = ish->imh;
    ith->t = ish->u.typ.t;
    ith->f.all = 0;
    return( DS_OK );
}

dip_status SymHdl2MbrType( imp_image_handle *iih, imp_sym_handle *ish,
                        imp_type_handle *ith )
{
    const char  *p;
    unsigned    index;
    dip_status  ds;
    typeinfo    typeld;

    PushLoad( &typeld );
    ds = LoadType( iih, ish->imh, ish->u.typ.t.entry );
    if( ds == DS_OK ) {
        p = Type->start + ish->u.typ.t.offset;
        p = BaseTypePtr( p );
        GetIndex( p, &index );
    }
    PopLoad();
    if( ds == DS_OK )
        return( FindTypeHandle( iih, ish->imh, index, ith ) );
    return( ds );
}


struct pending_entry {
    struct pending_entry        *prev;
    unsigned short              off;
};

struct anc_graph {
    struct anc_graph            *prev;
    union {
        struct pending_entry    *todo;
        struct {
            unsigned short      off;
            unsigned short      count;
        }                       s;
    }                           u;
    word                        entry;
};

dip_status SymHdl2MbrLoc( imp_image_handle *iih, imp_sym_handle *ish,
                         location_context *lc, location_list *ll )
{
    const char          *p;
    unsigned            count;
    struct anc_graph    *pending, *new, *tmp;
    imp_type_handle     new_ith;
    unsigned            index;
    dip_status          ds;
    location_list       adj;
    addr_off            offset = 0;
    unsigned            bit_start;
    unsigned            bit_len;
    location_info       info;
    typeinfo            typeld;

    PushLoad( &typeld );
    ds = LoadType( iih, ish->imh, ish->u.typ.t.entry );
    if( ds == DS_OK ) {
        p = Type->start + ish->u.typ.t.offset;
        switch( GETU8( p + 1 ) ) {
        case STRUCT_TYPE | ST_FIELD_LOC:
        case STRUCT_TYPE | ST_BIT_LOC:
            info = InfoLocation( p + 3 );
            break;
        default:
            info = NEED_BASE;
            break;
        }
        if( info & (NEED_BASE | EMPTY_EXPR) ) {
            ds = LoadType( iih, ish->imh, ish->u.typ.h.entry );
            if( ds == DS_OK ) {
                p = Type->start + ish->u.typ.h.offset;
                pending = NULL;
                count = GETU16( p + 2 );
                for( ;; ) {
                    if( count == 0 ) {
                        if( pending == NULL ) {
                            ds = DS_FAIL;
                            break;
                        }
                        ds = LoadType( iih, ish->imh, pending->entry );
                        if( ds != DS_OK ) {
                            break;
                        }
                        count = pending->u.s.count;
                        p = Type->start + pending->u.s.off;
                        pending = pending->prev;
                    }
                    --count;
                    p = NEXT_TYPE( p );
                    if( GETU8( p + 1 ) == (STRUCT_TYPE | ST_INHERIT) ) {
                        new = walloca( sizeof( *new ) );
                        new->entry = Type->entry;
                        new->u.s.off = p - Type->start;
                        new->u.s.count = count;
                        new->prev = pending;
                        pending = new;
                        p = SkipLocation( p + 2 );
                        GetIndex( p, &index );
                        ds = DoFindTypeHandle( iih, ish->imh, index, &new_ith );
                        if( ds != DS_OK ) {
                            break;
                        }
                        p = Type->start + new_ith.t.offset;
                        count = GETU16( p + 2 );
                    } else {
                        if( ish->u.typ.t.entry == Type->entry
                          && ish->u.typ.t.offset == (p - Type->start) ) {
                            break;
                        }
                    }
                }
                if( ds == DS_OK ) {
                    FreeLoad();
                    /* reverse the inheritance list */
                    new = NULL;
                    while( pending != NULL ) {
                        tmp = pending;
                        pending = tmp->prev;
                        tmp->prev = new;
                        new = tmp;
                    }
                    /* do the adjustors at each level */
                    ds = DCItemLocation( lc, CI_OBJECT, ll );
                    if( ds != DS_OK ) {
                        DCStatus( ds );
                    } else {
                        while( new != NULL ) {
                            ds = LoadType( iih, ish->imh, new->entry );
                            if( ds != DS_OK ) {
                                break;
                            }
                            PushBaseLocation( ll );
                            p = new->u.s.off + Type->start;
                            ds = EvalLocation( iih, lc, p + 2, &adj );
                            if( ds != DS_OK ) {
                                break;
                            }
                            if( adj.num != 1 || adj.e[0].type != LT_ADDR ) {
                                ds = DS_ERR | DS_BAD_LOCATION;
                                DCStatus( ds );
                                break;
                            }
                            LocationAdd( ll, adj.e[0].u.addr.mach.offset * 8 );
                            new = new->prev;
                        }
                        if( ds == DS_OK ) {
                            ds = LoadType( iih, ish->imh, ish->u.typ.t.entry );
                            if( ds == DS_OK ) {
                                p = Type->start + ish->u.typ.t.offset;
                            }
                        }
                    }
                }
            }
        }
        if( ds == DS_OK ) {
            /* do field offset and bit field selection */
            bit_start = 0;
            bit_len = 0;
            switch( GETU8( p + 1 ) ) {
            case STRUCT_TYPE | ST_BIT_BYTE:
                bit_start = GETU8( p + 3 );
                bit_len   = GETU8( p + 4 );
                /* fall through */
            case STRUCT_TYPE | ST_FIELD_BYTE:
                offset = GETU8( p + 2 );
                break;
            case STRUCT_TYPE | ST_BIT_WORD:
                bit_start = GETU8( p + 4 );
                bit_len   = GETU8( p + 5 );
                /* fall through */
            case STRUCT_TYPE | ST_FIELD_WORD:
                offset = GETU16( p + 2 );
                break;
            case STRUCT_TYPE | ST_BIT_LONG:
                bit_start = GETU8( p + 6 );
                bit_len   = GETU8( p + 7 );
                /* fall through */
            case STRUCT_TYPE | ST_FIELD_LONG:
                offset = GETU32( p + 2 );
                break;
            case STRUCT_TYPE | ST_FIELD_LOC:
            case STRUCT_TYPE | ST_BIT_LOC:
                if( info & (NEED_BASE | EMPTY_EXPR) )
                    PushBaseLocation( ll );
                ds = EvalLocation( iih, lc, p + 3, ll );
                if( ds == DS_OK ) {
                    if( GETU8( p + 1 ) == (STRUCT_TYPE | ST_BIT_LOC) ) {
                        p = SkipLocation( p + 3 );
                        bit_start = GETU8( p );
                        bit_len = GETU8( p + 1 );
                    }
                    offset = 0;
                }
            }
        }
    }
    PopLoad();
    if( ds == DS_OK ) {
        LocationAdd( ll, offset * 8 + bit_start );
        LocationTrunc( ll, bit_len );
    }
    return( ds );
}

dip_status SymHdl2MbrInfo( imp_image_handle *iih, imp_sym_handle *ish,
                        sym_info *si, location_context *lc )
{
    const char          *p;
    unsigned            index;
    dip_status          ds;
    unsigned            attrib;
    imp_type_handle     tmp_ith;
    imp_sym_handle      func_ish;
    byte                kind;
    location_list       ll;
    typeinfo            typeld;

    PushLoad( &typeld );
    ds = LoadType( iih, ish->imh, ish->u.typ.t.entry );
    if( ds == DS_OK ) {
        p = Type->start + ish->u.typ.t.offset;
        switch( GETU8( p + 1 ) ) {
        case STRUCT_TYPE | ST_FIELD_LOC:
        case STRUCT_TYPE | ST_BIT_LOC:
            attrib = GETU8( p + 2 );
            break;
        default:
            attrib = 0;
            break;
        }
        p = BaseTypePtr( p );
        GetIndex( p, &index );
        ds = FindRawTypeHandle( iih, ish->imh, index, &tmp_ith );
        if( ds == DS_OK ) {
            kind = GetRealTypeHandle( iih, &tmp_ith ) & CLASS_MASK;
        }
    }
    PopLoad();
    if( ds == DS_OK ) {
        switch( kind ) {
        case PROC_TYPE:
            si->kind = SK_PROCEDURE;
            ds = SymHdl2MbrLoc( iih, ish, lc, &ll );
            if( ds == DS_OK ) {
                func_ish.imh = ish->imh;
                if( LookupLclAddr( iih, ll.e[0].u.addr, &func_ish ) == SR_EXACT ) {
                    ds = SymHdl2LclInfo( iih, &func_ish, si );
                }
            }
            break;
        default:
            si->kind = SK_DATA;
            break;
        }
        si->is_member = 1;
        if( attrib & 0x01 )
            si->compiler = 1;
        if( attrib & 0x02 )
            si->is_public = 1;
        if( attrib & 0x04 )
            si->is_protected = 1;
        if( attrib & 0x08 ) {
            si->is_private = 1;
        }
    }
    return( ds );
}

dip_status DIPIMPENTRY( TypeThunkAdjust )( imp_image_handle *iih,
                    imp_type_handle *oith, imp_type_handle *mith,
                    location_context *lc, address *addr )
{
    const char          *p;
    unsigned            count;
    struct anc_graph    *pending, *new, *tmp;
    imp_type_handle     new_ith;
    unsigned            index;
    dip_status          ds;
    location_list       adj;
    typeinfo            typeld;

    if( oith->imh != mith->imh )
        return( DS_FAIL );
    PushLoad( &typeld );
    ds = LoadType( iih, oith->imh, oith->t.entry );
    if( ds == DS_OK ) {
        p = Type->start + oith->t.offset;
        pending = NULL;
        count = GETU16( p + 2 );
        for( ;; ) {
            if( count == 0 ) {
                if( pending == NULL ) {
                    ds = DS_FAIL;
                    break;
                }
                ds = LoadType( iih, oith->imh, pending->entry );
                if( ds != DS_OK ) {
                    break;
                }
                count = pending->u.s.count;
                p = Type->start + pending->u.s.off;
                pending = pending->prev;
            }
            --count;
            p = NEXT_TYPE( p );
            if( GETU8( p + 1 ) == (STRUCT_TYPE | ST_INHERIT) ) {
                new = walloca( sizeof( *new ) );
                new->entry = Type->entry;
                new->u.s.off = p - Type->start;
                new->u.s.count = count;
                new->prev = pending;
                pending = new;
                p = SkipLocation( p + 2 );
                GetIndex( p, &index );
                ds = DoFindTypeHandle( iih, oith->imh, index, &new_ith );
                if( ds != DS_OK ) {
                    break;
                }
                if( new_ith.t.offset == mith->t.offset
                  && new_ith.t.entry == mith->t.entry )
                    break;
                p = Type->start + new_ith.t.offset;
                count = GETU16( p + 2 );
            }
        }
        if( ds == DS_OK ) {
            /* reverse the inheritance list */
            new = NULL;
            while( pending != NULL ) {
                tmp = pending;
                pending = tmp->prev;
                tmp->prev = new;
                new = tmp;
            }
            /* do the adjustors at each level */
            while( new != NULL ) {
                ds = LoadType( iih, oith->imh, new->entry );
                if( ds != DS_OK ) {
                    break;
                }
                p = new->u.s.off + Type->start;
                ds = EvalLocation( iih, lc, p + 2, &adj );
                if( ds == DS_OK ) {
                    break;
                }
                if( adj.num != 1 || adj.e[0].type != LT_ADDR ) {
                    ds = DS_ERR | DS_BAD_LOCATION;
                    DCStatus( ds );
                    break;
                }
                addr->mach.offset += adj.e[0].u.addr.mach.offset;
                new = new->prev;
            }
        }
    }
    PopLoad();
    return( ds );
}

search_result SearchMbr( imp_image_handle *iih, imp_type_handle *ith,
                 lookup_item *li, void *d )
{
    const char          *p;
    unsigned            count;
    search_result       sr;
    struct anc_graph    *pending, *new;
    imp_type_handle     new_ith;
    unsigned            index;
    strcompn_fn         *scompn;
    imp_sym_handle      *ish;
    const char          *name;
    size_t              len;
    const char          *lookup_name;
    size_t              lookup_len;
    typeinfo            typeld;

    sr = SR_NONE;
    PushLoad( &typeld );
    if( LoadType( iih, ith->imh, ith->t.entry ) == DS_OK ) {
        p = Type->start + ith->t.offset;
        if( (GETU8( p + 1 ) & CLASS_MASK) == STRUCT_TYPE ) {
            scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
            lookup_name = li->name.start;
            lookup_len = li->name.len;
            pending = NULL;
            count = GETU16( p + 2 );
            for( ;; ) {
                if( count == 0 ) {
                    if( pending == NULL )
                        break;
                    if( LoadType( iih, ith->imh, pending->entry ) != DS_OK ) {
                        sr = SR_FAIL;
                        break;
                    }
                    count = pending->u.s.count;
                    p = Type->start + pending->u.s.off;
                    pending = pending->prev;
                }
                --count;
                p = NEXT_TYPE( p );
                if( GETU8( p + 1 ) == (STRUCT_TYPE | ST_INHERIT) ) {
                    new = walloca( sizeof( *new ) );
                    new->entry = Type->entry;
                    new->u.s.off = p - Type->start;
                    new->u.s.count = count;
                    new->prev = pending;
                    pending = new;
                    p = SkipLocation( p + 2 );
                    GetIndex( p, &index );
                    if( DoFindTypeHandle( iih, ith->imh, index, &new_ith ) != DS_OK ) {
                        sr = SR_FAIL;
                        break;
                    }
                    p = Type->start + new_ith.t.offset;
                    count = GETU16( p + 2 );
                } else {
                    name = NamePtr( p );
                    len = GETU8( p ) - ( name - p );
                    if( len == lookup_len && scompn( name, lookup_name, len ) == 0 ) {
                        ish = DCSymCreate( iih, d );
                        ish->u.typ.t.offset = p - Type->start;
                        ish->u.typ.t.entry = Type->entry;
                        ish->name_off = (byte)( name - p );
                        ish->imh = ith->imh;
                        ish->u.typ.h = ith->t;
                        ish->type = SH_MBR;
                        sr = SR_EXACT;
                    }
                }
            }
        }
    }
    PopLoad();
    return( sr );
}

walk_result WalkTypeSymList( imp_image_handle *iih, imp_type_handle *ith,
                 DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish, void *d )
{
    const char                  *p;
    unsigned                    count;
    walk_result                 wr;
    struct anc_graph            *pending, *new;
    imp_type_handle             new_ith;
    unsigned                    index;
    struct pending_entry        *list, *new_entry, *used;
    typeinfo                    typeld;
    byte                        kind;

    wr = WR_STOP;
    PushLoad( &typeld );
    if( LoadType( iih, ith->imh, ith->t.entry ) == DS_OK ) {
        ish->imh = ith->imh;
        ish->u.typ.h = ith->t;
        list = NULL;
        used = NULL;
        pending = NULL;
        p = Type->start + ith->t.offset;
        kind = GETU8( p + 1 ) & CLASS_MASK;
        if( kind == STRUCT_TYPE || kind == ENUM_TYPE ) {
            ish->type = ( kind == STRUCT_TYPE ) ? SH_MBR : SH_CST;
            count = GETU16( p + 2 );
            wr = WR_CONTINUE;
            for( ;; ) {
                while( count != 0 ) {
                    /* structure list is backwards -- reverse it */
                    p = NEXT_TYPE( p );
                    if( used == NULL ) {
                        new_entry = walloca( sizeof( *new_entry ) );
                    } else {
                        new_entry = used;
                        used = used->prev;
                    }
                    new_entry->off = p - Type->start;
                    new_entry->prev = list;
                    list = new_entry;
                    --count;
                }
                if( list == NULL ) {
                    if( pending == NULL )
                        break;
                    wk( iih, SWI_INHERIT_END, NULL, d );
                    if( LoadType( iih, ith->imh, pending->entry ) != DS_OK ) {
                        wr = WR_STOP;
                        break;
                    }
                    list = pending->u.todo;
                    pending = pending->prev;
                }
                p = list->off + Type->start;
                if( GETU8( p + 1 ) == (STRUCT_TYPE | ST_INHERIT) ) {
                    if( wk( iih, SWI_INHERIT_START, NULL, d ) == WR_CONTINUE ) {
                        if( list->prev != NULL ) {
                            new = walloca( sizeof( *new ) );
                            new->entry = Type->entry;
                            new->u.todo = list->prev;
                            new->prev = pending;
                            pending = new;
                        }
                        p = SkipLocation( p + 2 );
                        GetIndex( p, &index );
                        if( DoFindTypeHandle( iih, ith->imh, index, &new_ith ) != DS_OK ) {
                            wr = WR_STOP;
                            break;
                        }
                        list = NULL;
                        p = Type->start + new_ith.t.offset;
                        count = GETU16( p + 2 );
                        /* setting count will cause the list to be reversed */
                        continue;
                    }
                } else {
                    ish->u.typ.t.offset = p - Type->start;
                    ish->u.typ.t.entry = Type->entry;
                    ish->name_off = (byte)( NamePtr( p ) - p );
                    wr = wk( iih, SWI_SYMBOL, ish, d );
                    if( wr != WR_CONTINUE ) {
                        break;
                    }
                }
                new_entry = list->prev;
                list->prev = used;
                used = list;
                list = new_entry;
            }
        }
    }
    PopLoad();
    return( wr );
}

imp_mod_handle DIPIMPENTRY( TypeMod )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih;

    return( ith->imh );
}

const char *FindSpecCueTable( imp_image_handle *iih, imp_mod_handle imh, const char **base )
{
    typeinfo            typeld;
    const char          *p;
    const char          *ret;
    word                entry;
    unsigned long       offset;
    unsigned            size;

    ret = NULL;
    *base = NULL;
    PushLoad( &typeld );
    if( LoadType( iih, imh, 0 ) == DS_OK ) {
        for( p = Type->start; p < Type->end; p = NEXT_TYPE( p ) ) {
            if( GETU8( p + 1 ) == (NAME_TYPE | TYPE_CUE_TABLE) ) {
                offset = GETU32( p + 2 );
                for( entry = 0; (size = InfoSize( iih, imh, DMND_TYPES, entry )) != 0; entry++ ) {
                    if( size > offset )
                        break;
                    offset -= size;
                }
                if( size != 0 ) {
                    if( LoadType( iih, imh, entry ) == DS_OK ) {
                        p = Type->start;
                        *base = p;
                        InfoSpecLock( p ); /* so that the PopLoad doesn't free it */
                        ret = p + (unsigned)offset;
                    }
                }
                break;
            }
        }
    }
    PopLoad();
    return( ret );
}

int DIPIMPENTRY( TypeCmp )( imp_image_handle *iih, imp_type_handle *ith1, imp_type_handle *ith2 )
{
    /* unused parameters */ (void)iih;

    if( ith1->imh < ith2->imh )
        return( -1 );
    if( ith1->imh > ith2->imh )
        return( 1 );
    if( ith1->t.entry < ith2->t.entry )
        return( -1 );
    if( ith1->t.entry > ith2->t.entry )
        return( 1 );
    if( ith1->t.offset < ith2->t.offset )
        return( -1 );
    if( ith1->t.offset > ith2->t.offset )
        return( 1 );
    return( 0 );
}


size_t DIPIMPENTRY( TypeName )( imp_image_handle *iih, imp_type_handle *ith,
                unsigned num, symbol_type *tag, char *buff, size_t buff_size )
{
    /* unused parameters */ (void)iih; (void)ith; (void)num; (void)tag; (void)buff; (void)buff_size;

    //NYI: stub implementation
    return( 0 );
}

dip_status DIPIMPENTRY( TypeAddRef )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeRelease )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeFreeAll )( imp_image_handle *iih )
{
    /* unused parameters */ (void)iih;

    return( DS_OK );
}

