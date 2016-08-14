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
        #define pick(e,h,ih,wih)    ih,
        #include "diphndls.h"
        #undef pick
    };

    return( Sizes[hk] );
}

dip_status DIPIMPENTRY( Startup )( void )
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

dip_status DIPIMPENTRY( MoreMem )( unsigned size )
{
    size = size;
    return( InfoRelease() );
}

imp_mod_handle DIPIMPENTRY( SymMod )( imp_image_handle *ii, imp_sym_handle *is )
{
    ii = ii;
    return( is->im );
}

search_result DIPIMPENTRY( AddrSym )( imp_image_handle *ii, imp_mod_handle im,
                        address addr, imp_sym_handle *is )
{
    search_result       sr;

    if( im == IMH_NOMOD ) {
        if( ImpInterface.AddrMod( ii, addr, &is->im ) == SR_NONE ) {
            return( SR_NONE );
        }
    } else {
        is->im = im;
    }
    sr = LookupLclAddr( ii, addr, is );
    if( sr != SR_NONE )
        return( sr );
    return( LookupGblAddr( ii, addr, is ) );
}

#define SH_ESCAPE       0xf0

size_t DIPIMPENTRY( SymName )( imp_image_handle *ii, imp_sym_handle *is,
                                location_context *lc,
                                symbol_name sn, char *buff, size_t buff_size )
{
    byte                *sp;
    byte                *ep;
    byte                curr;
    size_t              len;
    char                *mangled_name;
    location_list       ll;
    imp_sym_handle      gbl_is;

    switch( sn ) {
    case SN_EXPRESSION:
        sp = (byte *)is;
        ++is;
        len = 0;
        #define STUFF_IT( c )   if( (len+1) < buff_size ) *ep++ = (c); ++len
        ep = (byte *)buff;
        STUFF_IT( SH_ESCAPE );
        while( sp < (byte *)is ) {
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
    case SN_DEMANGLED:
        len = ImpInterface.SymName( ii, is, lc, SN_OBJECT, NULL, 0 );
        if( len == 0 )
            return( len );
        mangled_name = __alloca( len + 1 );
        ImpInterface.SymName( ii, is, lc, SN_OBJECT, mangled_name, len + 1 );
        if( !__is_mangled( mangled_name, len ) )
            return( 0 );
        return( __demangle_l( mangled_name, len, buff, buff_size ) );
    case SN_OBJECT:
        switch( is->type ) {
        case SH_LCL:
            if( Lcl2GblHdl( ii, is, &gbl_is ) != DS_OK )
                break;
            return( SymHdl2ObjGblName( ii, &gbl_is, buff, buff_size ) );
        case SH_MBR:
            if( ImpInterface.SymLocation( ii, is, lc, &ll ) != DS_OK )
                break;
            if( ll.num != 1 || ll.e[0].type != LT_ADDR )
                break;
            if( ImpInterface.AddrMod( ii, ll.e[0].u.addr, &gbl_is.im ) == SR_NONE )
                break;
            if( LookupGblAddr(ii,ll.e[0].u.addr,&gbl_is) != SR_EXACT )
                break;
            is = &gbl_is;
            /* fall through */
        case SH_GBL:
            return( SymHdl2ObjGblName( ii, is, buff, buff_size ) );
        }
        /* fall through */
    case SN_SOURCE:
        switch( is->type ) {
        case SH_GBL:
            return( SymHdl2GblName( ii, is, buff, buff_size ) );
        case SH_LCL:
            return( SymHdl2LclName( ii, is, buff, buff_size ) );
        case SH_MBR:
            return( SymHdl2MbrName( ii, is, buff, buff_size ) );
        case SH_TYP:
            return( SymHdl2TypName( ii, is, buff, buff_size ) );
        case SH_CST:
            return( SymHdl2CstName( ii, is, buff, buff_size ) );
        }
        break;
    }
    return( 0 );
}


static void CollectSymHdl( const char *ep, imp_sym_handle *is )
{
    byte        *sp;
    byte        curr;
    static byte escapes[] = { SH_ESCAPE, '\0', '`' };

    ++ep;
    sp = (byte *)is;
    ++is;
    while( sp < (byte *)is ) {
        curr = GETU8( ep++ );
        if( curr == SH_ESCAPE )
            curr = escapes[GETU8( ep++ ) - 1];
        *sp++ = curr;
    }
}

static search_result SearchFileScope( imp_image_handle *ii,
                        imp_mod_handle im, lookup_item *li, void *d )
{
    search_result       sr;

    if( im == IMH_NOMOD )
        return( SR_NONE );
    switch( li->type ) {
    case ST_NONE:
        sr = SearchLclMod( ii, im, li, d );
        if( sr != SR_NONE )
            return( sr );
        return( SearchEnumName( ii, im, li, d ) );
    case ST_DESTRUCTOR:
    case ST_OPERATOR:
        return( SR_NONE );
    default:
        return( SearchTypeName( ii, im, li, d ) );
    }
}

static search_result DoLookupSym( imp_image_handle *ii, symbol_source ss,
                         void *source, lookup_item *li, location_context *lc, void *d )
{
    imp_mod_handle      im;
    search_result       sr;
    lookup_item         sym_li;
    char                *buff;
    const char          *src;
    char                *dst;
    size_t              len;
    unsigned            op_len;
    imp_sym_handle      *scope_is;

    lc = lc;
    if( GETU8( li->name.start ) == SH_ESCAPE ) {
        CollectSymHdl( li->name.start, DCSymCreate( ii, d ) );
        return( SR_EXACT );
    }
    if( li->type == ST_NAMESPACE )
        return( SR_NONE );
    sym_li = *li;
    if( ss == SS_SCOPESYM ) {
        char    *scope_name;
        scope_is = source;
        len = ImpInterface.SymName( ii, scope_is, NULL, SN_SOURCE, NULL, 0 );
        scope_name = __alloca( len + 1 );
        ImpInterface.SymName( ii, scope_is, NULL, SN_SOURCE, scope_name, len + 1 );
        sym_li.scope.start = scope_name;
        sym_li.scope.len = len;
        ss = SS_MODULE;
        sym_li.mod = IMH2MH( scope_is->im );
        source = &sym_li.mod;
    }
    if( sym_li.type == ST_OPERATOR ) {
        src = sym_li.name.start;
        len = sym_li.name.len;
        buff = __alloca( len + 20 );
        dst = buff;
        for( ;; ) {
            if( len == 0 )
                break;
            if( src == sym_li.source.start ) {
                op_len = __mangle_operator( src, sym_li.source.len, buff );
                if( op_len == 0 ) {
                    DCStatus( DS_ERR|DS_INVALID_OPERATOR );
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
    im = IMH_NOMOD;
    switch( ss ) {
    case SS_SCOPED:
        if( ImpInterface.AddrMod( ii, *(address *)source, &im ) == SR_NONE ) {
            im = MH2IMH( sym_li.mod );
        } else if( MH2IMH( sym_li.mod ) == IMH_NOMOD || MH2IMH( sym_li.mod ) == im ) {
            if( !sym_li.file_scope && sym_li.type == ST_NONE ) {
                sr = SearchLclScope( ii, im, (address *)source, &sym_li, d );
            }
        } else {
            im = MH2IMH( sym_li.mod );
        }
        if( im != IMH_NOMOD && sr == SR_NONE ) {
            sr = SearchFileScope( ii, im, &sym_li, d );
        }
        break;
    case SS_MODULE:
        im = *(imp_mod_handle *)source;
        if( MH2IMH( sym_li.mod ) == IMH_NOMOD || MH2IMH( sym_li.mod ) == im ) {
            sr = SearchFileScope( ii, im, &sym_li, d );
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
        return( SearchMbr( ii, (imp_type_handle *)source, &sym_li, d ) );
    }
    if( sr == SR_NONE ) {
        switch( sym_li.type ) {
        case ST_NONE:
        case ST_DESTRUCTOR:
        case ST_OPERATOR:
            sr = SearchGbl( ii, im, MH2IMH( sym_li.mod ), &sym_li, d );
            break;
        }
    }
    return( sr );
}

search_result DIPIMPENTRY( LookupSym )( imp_image_handle *ii, symbol_source ss,
                         void *source, lookup_item *li, void *d )
{
    return( DoLookupSym( ii, ss, source, li, NULL, d ) );
}

search_result DIPIMPENTRY( LookupSymEx )( imp_image_handle *ii, symbol_source ss,
                         void *source, lookup_item *li, location_context *lc, void *d )
{
    return( DoLookupSym( ii, ss, source, li, lc, d ) );
}

dip_status DIPIMPENTRY( SymLocation )( imp_image_handle *ii, imp_sym_handle *is,
                                location_context *lc, location_list *ll )
{
    switch( is->type ) {
    case SH_GBL:
        return( SymHdl2GblLoc( ii, is, ll ) );
    case SH_LCL:
        return( SymHdl2LclLoc( ii, is, lc, ll ) );
    case SH_MBR:
        return( SymHdl2MbrLoc( ii, is, lc, ll ) );
    }
    return( DS_ERR|DS_BAD_PARM );
}


dip_status DIPIMPENTRY( SymValue )( imp_image_handle *ii, imp_sym_handle *is,
                                location_context *lc, void *value )
{
    lc = lc;
    switch( is->type ) {
    case SH_CST:
        return( SymHdl2CstValue( ii, is, value ) );
    }
    return( DS_ERR|DS_BAD_PARM );
}


dip_status DIPIMPENTRY( SymType )( imp_image_handle *ii, imp_sym_handle *is,
                                imp_type_handle *it )
{
    switch( is->type ) {
    case SH_GBL:
        return( SymHdl2GblType( ii, is, it ) );
    case SH_LCL:
        return( SymHdl2LclType( ii, is, it ) );
    case SH_MBR:
        return( SymHdl2MbrType( ii, is, it ) );
    case SH_TYP:
        return( SymHdl2TypType( ii, is, it ) );
    case SH_CST:
        return( SymHdl2CstType( ii, is, it ) );
    }
    return( DS_ERR|DS_BAD_PARM );
}

dip_status DIPIMPENTRY( SymInfo )( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc, sym_info *si )
{
    memset( si, 0, sizeof( *si ) );
    switch( is->type ) {
    case SH_GBL:
        return( SymHdl2GblInfo( ii, is, si ) );
    case SH_LCL:
        return( SymHdl2LclInfo( ii, is, si ) );
    case SH_MBR:
        return( SymHdl2MbrInfo( ii, is, si, lc ) );
    case SH_TYP:
        si->kind = SK_TYPE;
        break;
    case SH_CST:
        si->kind = SK_CONST;
        break;
    default:
        return( DS_ERR|DS_BAD_PARM );
    }
    return( DS_OK );

}

dip_status DIPIMPENTRY( SymParmLocation )( imp_image_handle *ii,
    imp_sym_handle *is, location_context *lc, location_list *ll, unsigned parm )
{
    if( is->type != SH_LCL )
        return( DS_FAIL );
    return( SymHdl2LclParmLoc( ii, is, lc, ll, parm ) );
}

typedef struct {
    IMP_SYM_WKR         *walk;
    imp_sym_handle      *is;
    void                *d;
} sym_glue;

static walk_result WalkMySyms( imp_image_handle *ii,
                        imp_mod_handle im, void *d )
{
    sym_glue    *wd = d;
    walk_result wr;

    WalkLclModSymList( ii, im, wd->walk, wd->is, wd->d, &wr );
    return( wr );
}

static walk_result DoWalkSymList( imp_image_handle *ii, symbol_source ss,
                void *t, IMP_SYM_WKR *wk, imp_sym_handle *is, void *d )
{
    imp_mod_handle      im;
    sym_glue            glue;
    walk_result         wr;

    wr = WR_CONTINUE;
    switch( ss ) {
    case SS_TYPE:
        return( WalkTypeSymList( ii, (imp_type_handle *)t, wk, is, d ) );
    case SS_SCOPED:
        return( WalkScopedSymList( ii, (address *)t, wk, is, d ) );
    case SS_BLOCK:
        return( WalkBlockSymList( ii, (scope_block *)t, wk, is, d ) );
    case SS_MODULE:
        im = *(imp_mod_handle *)t;
        if( im == IMH_NOMOD ) {
            glue.walk = wk;
            glue.is   = is;
            glue.d    = d;
            wr = MyWalkModList( ii, WalkMySyms, &glue );
            if( wr == WR_CONTINUE ) {
                wr = WalkGblModSymList( ii, im, wk, is, d );
            }
        } else {
            if( WalkLclModSymList( ii, im, wk, is, d, &wr ) != DS_OK ) {
                wr = WalkGblModSymList( ii, im, wk, is, d );
            }
        }
        break;
    }
    return( wr );
}

walk_result DIPIMPENTRY( WalkSymList )( imp_image_handle *ii, symbol_source ss,
                void *source, IMP_SYM_WKR *wk, imp_sym_handle *is, void *d )
{
    return( DoWalkSymList( ii, ss, source, wk, is, d ) );
}

walk_result DIPIMPENTRY( WalkSymListEx )( imp_image_handle *ii, symbol_source ss,
                void *source, IMP_SYM_WKR *wk, imp_sym_handle *is,
                location_context *lc, void *d )
{
    lc=lc;
    return( DoWalkSymList( ii, ss, source, wk, is, d ) );
}

dip_status DIPIMPENTRY( ModDefault )( imp_image_handle *ii, imp_mod_handle im,
                        default_kind dk, dip_type_info *ti )
{
    ii = ii; im = im; dk = dk; ti = ti;
    return( DS_FAIL );
}

static int GblCmp( gbl_info *g1, gbl_info *g2 )
{
#if defined(__COMPACT__) || defined(__LARGE__) || defined( __HUGE__ )
    unsigned    s1;
    unsigned    s2;

    s1 = FP_SEG( g1 );
    s2 = FP_SEG( g2 );
    if( s1 != s2 )
        return( s1 - s2 );
    return( FP_OFF( g1 ) - FP_OFF( g2 ) );
#else
    return( (char*)g1 - (char*)g2 );
#endif
}

int DIPIMPENTRY( SymCmp )( imp_image_handle *ii, imp_sym_handle *is1,
                        imp_sym_handle *is2 )
{
    ii = ii;
    if( is1->im != is2->im )
        return( is1->im - is2->im );
    switch( is1->type ) {
    case SH_GBL:
        switch( is2->type ) {
        case SH_GBL:
            return( GblCmp( is1->u.gbl, is2->u.gbl ) );
        case SH_LCL:
            if( is2->u.lcl.gbl_link != NULL ) {
                return( GblCmp( is1->u.gbl, is2->u.lcl.gbl_link ) );
            }
            /* fall through */
        default:
             return( -1 );
        }
    case SH_LCL:
        switch( is2->type ) {
        case SH_GBL:
            if( is1->u.lcl.gbl_link != NULL ) {
                return( GblCmp( is1->u.lcl.gbl_link, is2->u.gbl ) );
            }
            return( 1 );
        case SH_LCL:
            return( is1->u.lcl.offset - is2->u.lcl.offset );
        default:
            return( -1 );
        }
    case SH_MBR:
    case SH_TYP:
    case SH_CST:
        switch( is2->type ) {
        case SH_MBR:
        case SH_TYP:
        case SH_CST:
            if( is1->u.typ.t.entry != is2->u.typ.t.entry ) {
                return( is1->u.typ.t.entry - is2->u.typ.t.entry );
            }
            return( is1->u.typ.t.offset - is2->u.typ.t.offset );
        default:
            return( 1 );
        }
    }
    return( 0 );
}

dip_status DIPIMPENTRY( SymAddRef )( imp_image_handle *ii, imp_sym_handle *is )
{
    ii=ii;
    is=is;
    return(DS_OK);
}

dip_status DIPIMPENTRY( SymRelease )( imp_image_handle *ii, imp_sym_handle *is )
{
    ii=ii;
    is=is;
    return(DS_OK);
}

dip_status DIPIMPENTRY( SymFreeAll )( imp_image_handle *ii )
{
    ii=ii;
    return(DS_OK);
}
