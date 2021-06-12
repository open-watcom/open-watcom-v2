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
* Description:  Miscellaneous support routines for Watcom debugging format.
*
****************************************************************************/


#include "dipwat.h"
#if defined( _M_I86 )
#include <i86.h>
#endif
#include "demangle.h"
#include "walloca.h"
#include "watlcl.h"
#include "wattype.h"
#include "watgbl.h"


const char DIPImp( Name )[] = "WATCOM";

unsigned DIPIMPENTRY( HandleSize )( handle_kind hk )
{
    static unsigned_8 Sizes[] = {
        #define pick(e,hdl,imphdl,wvimphdl) imphdl,
        #include "diphndls.h"
        #undef pick
    };

    return( Sizes[hk] );
}

dip_status DIPImp( Startup )( void )
{
    return( DS_OK );
}

void DIPIMPENTRY( Shutdown )( void )
{
    FiniDemand();
}

void DIPIMPENTRY( Cancel )( void )
{
    KillLclLoadStack();
    KillTypeLoadStack();
    InfoUnlock();
}

dip_status DIPIMPENTRY( MoreMem )( size_t size )
{
    /* unused parameters */ (void)size;

    return( InfoRelease() );
}

imp_mod_handle DIPIMPENTRY( SymMod )( imp_image_handle *iih, imp_sym_handle *ish )
{
    /* unused parameters */ (void)iih;

    return( ish->imh );
}

search_result DIPIMPENTRY( AddrSym )( imp_image_handle *iih, imp_mod_handle imh,
                        address addr, imp_sym_handle *ish )
{
    search_result       sr;

    if( imh == IMH_NOMOD ) {
        if( ImpInterface.AddrMod( iih, addr, &ish->imh ) == SR_NONE ) {
            return( SR_NONE );
        }
    } else {
        ish->imh = imh;
    }
    sr = LookupLclAddr( iih, addr, ish );
    if( sr != SR_NONE )
        return( sr );
    return( LookupGblAddr( iih, addr, ish ) );
}

#define SH_ESCAPE       0xf0
#define STUFF_IT( c )   \
    if((len + 1) < buff_size ) \
        *ep++ = (c); \
    ++len

size_t DIPIMPENTRY( SymName )( imp_image_handle *iih, imp_sym_handle *ish,
    location_context *lc, symbol_name_type snt, char *buff, size_t buff_size )
{
    byte                *sp;
    byte                *ep;
    byte                curr;
    size_t              len;
    char                *mangled_name;
    location_list       ll;
    imp_sym_handle      gbl_ish;

    switch( snt ) {
    case SNT_EXPRESSION:
        sp = (byte *)ish;
        ++ish;
        len = 0;
        ep = (byte *)buff;
        STUFF_IT( SH_ESCAPE );
        while( sp < (byte *)ish ) {
            curr = *sp++;
            switch( curr ) {
            case SH_ESCAPE:
                curr = 1;
                STUFF_IT( SH_ESCAPE );
                break;
            case '\0':
                curr = 2;
                STUFF_IT( SH_ESCAPE );
                break;
            case '`':
                curr = 3;
                STUFF_IT( SH_ESCAPE );
                break;
            }
            STUFF_IT( curr );
        }
        if( buff_size > 0 )
            *ep++ = '\0';
        return( len );
    case SNT_DEMANGLED:
        len = ImpInterface.SymName( iih, ish, lc, SNT_OBJECT, NULL, 0 );
        if( len == 0 )
            return( len );
        mangled_name = walloca( len + 1 );
        ImpInterface.SymName( iih, ish, lc, SNT_OBJECT, mangled_name, len + 1 );
        if( !__is_mangled( mangled_name, len ) )
            return( 0 );
        return( __demangle_l( mangled_name, len, buff, buff_size ) );
    case SNT_OBJECT:
        switch( ish->type ) {
        case SH_LCL:
            if( Lcl2GblHdl( iih, ish, &gbl_ish ) != DS_OK )
                break;
            return( SymHdl2ObjGblName( iih, &gbl_ish, buff, buff_size ) );
        case SH_MBR:
            if( ImpInterface.SymLocation( iih, ish, lc, &ll ) != DS_OK )
                break;
            if( ll.num != 1 || ll.e[0].type != LT_ADDR )
                break;
            if( ImpInterface.AddrMod( iih, ll.e[0].u.addr, &gbl_ish.imh ) == SR_NONE )
                break;
            if( LookupGblAddr( iih, ll.e[0].u.addr, &gbl_ish ) != SR_EXACT )
                break;
            ish = &gbl_ish;
            /* fall through */
        case SH_GBL:
            return( SymHdl2ObjGblName( iih, ish, buff, buff_size ) );
        }
        /* fall through */
    case SNT_SOURCE:
        switch( ish->type ) {
        case SH_GBL:
            return( SymHdl2GblName( iih, ish, buff, buff_size ) );
        case SH_LCL:
            return( SymHdl2LclName( iih, ish, buff, buff_size ) );
        case SH_MBR:
            return( SymHdl2MbrName( iih, ish, buff, buff_size ) );
        case SH_TYP:
            return( SymHdl2TypName( iih, ish, buff, buff_size ) );
        case SH_CST:
            return( SymHdl2CstName( iih, ish, buff, buff_size ) );
        }
        break;
    }
    return( 0 );
}


static void CollectSymHdl( const char *ep, imp_sym_handle *ish )
{
    byte        *sp;
    byte        curr;
    static byte escapes[] = { SH_ESCAPE, '\0', '`' };

    ++ep;
    sp = (byte *)ish;
    ++ish;
    while( sp < (byte *)ish ) {
        curr = GETU8( ep++ );
        if( curr == SH_ESCAPE )
            curr = escapes[GETU8( ep++ ) - 1];
        *sp++ = curr;
    }
}

static search_result SearchFileScope( imp_image_handle *iih,
                        imp_mod_handle imh, lookup_item *li, void *d )
{
    search_result       sr;

    if( imh == IMH_NOMOD )
        return( SR_NONE );
    switch( li->type ) {
    case ST_NONE:
        sr = SearchLclMod( iih, imh, li, d );
        if( sr != SR_NONE )
            return( sr );
        return( SearchEnumName( iih, imh, li, d ) );
    case ST_DESTRUCTOR:
    case ST_OPERATOR:
        return( SR_NONE );
    default:
        return( SearchTypeName( iih, imh, li, d ) );
    }
}

static search_result DoLookupSym( imp_image_handle *iih, symbol_source ss,
                         void *source, lookup_item *li, location_context *lc, void *d )
{
    imp_mod_handle      imh;
    search_result       sr;
    lookup_item         sym_li;
    char                *buff;
    const char          *src;
    char                *dst;
    size_t              len;
    unsigned            op_len;
    imp_sym_handle      *scope_ish;

    /* unused parameters */ (void)lc;

    if( GETU8( li->name.start ) == SH_ESCAPE ) {
        CollectSymHdl( li->name.start, DCSymCreate( iih, d ) );
        return( SR_EXACT );
    }
    if( li->type == ST_NAMESPACE )
        return( SR_NONE );
    sym_li = *li;
    if( ss == SS_SCOPESYM ) {
        char    *scope_name;
        scope_ish = source;
        len = ImpInterface.SymName( iih, scope_ish, NULL, SNT_SOURCE, NULL, 0 );
        scope_name = walloca( len + 1 );
        ImpInterface.SymName( iih, scope_ish, NULL, SNT_SOURCE, scope_name, len + 1 );
        sym_li.scope.start = scope_name;
        sym_li.scope.len = len;
        ss = SS_MODULE;
        sym_li.mod = IMH2MH( scope_ish->imh );
        source = &sym_li.mod;
    }
    if( sym_li.type == ST_OPERATOR ) {
        src = sym_li.name.start;
        len = sym_li.name.len;
        buff = walloca( len + 20 );
        dst = buff;
        for( ;; ) {
            if( len == 0 )
                break;
            if( src == sym_li.source.start ) {
                op_len = __mangle_operator( src, sym_li.source.len, buff );
                if( op_len == 0 ) {
                    DCStatus( DS_ERR | DS_INVALID_OPERATOR );
                    return( SR_NONE );
                }
                dst += op_len;
                src += sym_li.source.len;
                len -= sym_li.source.len;
            } else {
                *dst++ = *src++;
                --len;
            }
        }
        sym_li.name.len = dst - buff;
        sym_li.name.start = buff;
    }
    sr = SR_NONE;
    imh = IMH_NOMOD;
    switch( ss ) {
    case SS_SCOPED:
        if( ImpInterface.AddrMod( iih, *(address *)source, &imh ) == SR_NONE ) {
            imh = MH2IMH( sym_li.mod );
        } else if( MH2IMH( sym_li.mod ) == IMH_NOMOD || MH2IMH( sym_li.mod ) == imh ) {
            if( !sym_li.file_scope && sym_li.type == ST_NONE ) {
                sr = SearchLclScope( iih, imh, (address *)source, &sym_li, d );
            }
        } else {
            imh = MH2IMH( sym_li.mod );
        }
        if( imh != IMH_NOMOD && sr == SR_NONE ) {
            sr = SearchFileScope( iih, imh, &sym_li, d );
        }
        break;
    case SS_MODULE:
        imh = *(imp_mod_handle *)source;
        if( MH2IMH( sym_li.mod ) == IMH_NOMOD || MH2IMH( sym_li.mod ) == imh ) {
            sr = SearchFileScope( iih, imh, &sym_li, d );
        }
        break;
    case SS_TYPE:
        if( MH2IMH( sym_li.mod ) != IMH_NOMOD )
            return( SR_NONE );
        switch( sym_li.type ) {
        case ST_TYPE:
        case ST_STRUCT_TAG:
        case ST_CLASS_TAG:
        case ST_UNION_TAG:
        case ST_ENUM_TAG:
            return( SR_NONE );
        }
        return( SearchMbr( iih, (imp_type_handle *)source, &sym_li, d ) );
    }
    if( sr == SR_NONE ) {
        switch( sym_li.type ) {
        case ST_NONE:
        case ST_DESTRUCTOR:
        case ST_OPERATOR:
            sr = SearchGbl( iih, imh, MH2IMH( sym_li.mod ), &sym_li, d );
            break;
        }
    }
    return( sr );
}

search_result DIPIMPENTRY( LookupSym )( imp_image_handle *iih, symbol_source ss,
                         void *source, lookup_item *li, void *d )
{
    return( DoLookupSym( iih, ss, source, li, NULL, d ) );
}

search_result DIPIMPENTRY( LookupSymEx )( imp_image_handle *iih, symbol_source ss,
                         void *source, lookup_item *li, location_context *lc, void *d )
{
    return( DoLookupSym( iih, ss, source, li, lc, d ) );
}

dip_status DIPIMPENTRY( SymLocation )( imp_image_handle *iih, imp_sym_handle *ish,
                                location_context *lc, location_list *ll )
{
    switch( ish->type ) {
    case SH_GBL:
        return( SymHdl2GblLoc( iih, ish, ll ) );
    case SH_LCL:
        return( SymHdl2LclLoc( iih, ish, lc, ll ) );
    case SH_MBR:
        return( SymHdl2MbrLoc( iih, ish, lc, ll ) );
    }
    return( DS_ERR | DS_BAD_PARM );
}


dip_status DIPIMPENTRY( SymValue )( imp_image_handle *iih, imp_sym_handle *ish,
                                location_context *lc, void *value )
{
    /* unused parameters */ (void)lc;

    switch( ish->type ) {
    case SH_CST:
        return( SymHdl2CstValue( iih, ish, value ) );
    }
    return( DS_ERR | DS_BAD_PARM );
}


dip_status DIPIMPENTRY( SymType )( imp_image_handle *iih, imp_sym_handle *ish, imp_type_handle *ith )
{
    switch( ish->type ) {
    case SH_GBL:
        return( SymHdl2GblType( iih, ish, ith ) );
    case SH_LCL:
        return( SymHdl2LclType( iih, ish, ith ) );
    case SH_MBR:
        return( SymHdl2MbrType( iih, ish, ith ) );
    case SH_TYP:
        return( SymHdl2TypType( iih, ish, ith ) );
    case SH_CST:
        return( SymHdl2CstType( iih, ish, ith ) );
    }
    return( DS_ERR | DS_BAD_PARM );
}

dip_status DIPIMPENTRY( SymInfo )( imp_image_handle *iih, imp_sym_handle *ish,
                        location_context *lc, sym_info *si )
{
    memset( si, 0, sizeof( *si ) );
    switch( ish->type ) {
    case SH_GBL:
        return( SymHdl2GblInfo( iih, ish, si ) );
    case SH_LCL:
        return( SymHdl2LclInfo( iih, ish, si ) );
    case SH_MBR:
        return( SymHdl2MbrInfo( iih, ish, si, lc ) );
    case SH_TYP:
        si->kind = SK_TYPE;
        break;
    case SH_CST:
        si->kind = SK_CONST;
        break;
    default:
        return( DS_ERR | DS_BAD_PARM );
    }
    return( DS_OK );

}

dip_status DIPIMPENTRY( SymParmLocation )( imp_image_handle *iih,
    imp_sym_handle *ish, location_context *lc, location_list *ll, unsigned parm )
{
    if( ish->type != SH_LCL )
        return( DS_FAIL );
    return( SymHdl2LclParmLoc( iih, ish, lc, ll, parm ) );
}

typedef struct {
    DIP_IMP_SYM_WALKER  *walk;
    imp_sym_handle      *ish;
    void                *d;
} sym_glue;

static walk_result WalkMySyms( imp_image_handle *iih, imp_mod_handle imh, void *d )
{
    sym_glue    *wd = d;
    walk_result wr;

    WalkLclModSymList( iih, imh, wd->walk, wd->ish, wd->d, &wr );
    return( wr );
}

static walk_result DoWalkSymList( imp_image_handle *iih, symbol_source ss,
                void *t, DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish, void *d )
{
    imp_mod_handle      imh;
    sym_glue            glue;
    walk_result         wr;

    wr = WR_CONTINUE;
    switch( ss ) {
    case SS_TYPE:
        return( WalkTypeSymList( iih, (imp_type_handle *)t, wk, ish, d ) );
    case SS_SCOPED:
        return( WalkScopedSymList( iih, (address *)t, wk, ish, d ) );
    case SS_BLOCK:
        return( WalkBlockSymList( iih, (scope_block *)t, wk, ish, d ) );
    case SS_MODULE:
        imh = *(imp_mod_handle *)t;
        if( imh == IMH_NOMOD ) {
            glue.walk = wk;
            glue.ish   = ish;
            glue.d    = d;
            wr = MyWalkModList( iih, WalkMySyms, &glue );
            if( wr == WR_CONTINUE ) {
                wr = WalkGblModSymList( iih, imh, wk, ish, d );
            }
        } else {
            if( WalkLclModSymList( iih, imh, wk, ish, d, &wr ) != DS_OK ) {
                wr = WalkGblModSymList( iih, imh, wk, ish, d );
            }
        }
        break;
    }
    return( wr );
}

walk_result DIPIMPENTRY( WalkSymList )( imp_image_handle *iih, symbol_source ss,
                void *source, DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish, void *d )
{
    return( DoWalkSymList( iih, ss, source, wk, ish, d ) );
}

walk_result DIPIMPENTRY( WalkSymListEx )( imp_image_handle *iih, symbol_source ss,
                void *source, DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish,
                location_context *lc, void *d )
{
    /* unused parameters */ (void)lc;

    return( DoWalkSymList( iih, ss, source, wk, ish, d ) );
}

dip_status DIPIMPENTRY( ModDefault )( imp_image_handle *iih, imp_mod_handle imh,
                        default_kind dk, dig_type_info *ti )
{
    /* unused parameters */ (void)iih; (void)imh; (void)dk; (void)ti;

    return( DS_FAIL );
}

static int GblCmp( gbl_info *g1, gbl_info *g2 )
{
#if defined(__COMPACT__) || defined(__LARGE__) || defined( __HUGE__ )
    unsigned    s1;
    unsigned    s2;

    s1 = _FP_SEG( g1 );
    s2 = _FP_SEG( g2 );
    if( s1 < s2 )
        return( -1 );
    if( s1 > s2 )
        return( 1 );
    if( _FP_OFF( g1 ) < _FP_OFF( g2 ) )
        return( -1 );
    if( _FP_OFF( g1 ) > _FP_OFF( g2 ) )
        return( 1 );
    return( 0 );
#else
    if( (char *)g1 < (char *)g2 )
        return( -1 );
    if( (char *)g1 > (char *)g2 )
        return( 1 );
    return( 0 );
#endif
}

int DIPIMPENTRY( SymCmp )( imp_image_handle *iih, imp_sym_handle *ish1, imp_sym_handle *ish2 )
{
    /* unused parameters */ (void)iih;

    if( ish1->imh < ish2->imh )
        return( -1 );
    if( ish1->imh > ish2->imh )
        return( 1 );
    switch( ish1->type ) {
    case SH_GBL:
        switch( ish2->type ) {
        case SH_GBL:
            return( GblCmp( ish1->u.gbl, ish2->u.gbl ) );
        case SH_LCL:
            if( ish2->u.lcl.gbl_link != NULL ) {
                return( GblCmp( ish1->u.gbl, ish2->u.lcl.gbl_link ) );
            }
            /* fall through */
        default:
            return( -1 );
        }
    case SH_LCL:
        switch( ish2->type ) {
        case SH_GBL:
            if( ish1->u.lcl.gbl_link != NULL ) {
                return( GblCmp( ish1->u.lcl.gbl_link, ish2->u.gbl ) );
            }
            return( 1 );
        case SH_LCL:
            if( ish1->u.lcl.offset < ish2->u.lcl.offset )
                return( -1 );
            if( ish1->u.lcl.offset > ish2->u.lcl.offset )
                return( 1 );
            return( 0 );
        default:
            return( -1 );
        }
    case SH_MBR:
    case SH_TYP:
    case SH_CST:
        switch( ish2->type ) {
        case SH_MBR:
        case SH_TYP:
        case SH_CST:
            if( ish1->u.typ.t.entry < ish2->u.typ.t.entry )
                return( -1 );
            if( ish1->u.typ.t.entry > ish2->u.typ.t.entry )
                return( 1 );
            if( ish1->u.typ.t.offset < ish2->u.typ.t.offset )
                return( -1 );
            if( ish1->u.typ.t.offset > ish2->u.typ.t.offset )
                return( 1 );
            return( 0 );
        default:
            return( 1 );
        }
    }
    return( 0 );
}

dip_status DIPIMPENTRY( SymAddRef )( imp_image_handle *iih, imp_sym_handle *ish )
{
    /* unused parameters */ (void)iih; (void)ish;

    return( DS_OK );
}

dip_status DIPIMPENTRY( SymRelease )( imp_image_handle *iih, imp_sym_handle *ish )
{
    /* unused parameters */ (void)iih; (void)ish;

    return( DS_OK );
}

dip_status DIPIMPENTRY( SymFreeAll )( imp_image_handle *iih )
{
    /* unused parameters */ (void)iih;

    return( DS_OK );
}
