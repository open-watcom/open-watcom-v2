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


#include "dipwat.h"
#include "demangle.h"
#include <malloc.h>
#include <string.h>
#if defined(M_I86)
#include <i86.h>
#endif


extern void             InfoUnlock();
extern dip_status       InfoRelease();
extern void             FiniDemand();
extern search_result    LookupLclAddr( imp_image_handle *, address, imp_sym_handle * );
extern search_result    LookupGblAddr( imp_image_handle *, address, imp_sym_handle * );
extern unsigned         SymHdl2CstName( imp_image_handle *, imp_sym_handle *, char *, unsigned );
extern unsigned         SymHdl2TypName( imp_image_handle *, imp_sym_handle *, char *, unsigned );
extern unsigned         SymHdl2MbrName( imp_image_handle *, imp_sym_handle *, char *, unsigned );
extern unsigned         SymHdl2GblName( imp_image_handle *, imp_sym_handle *, char *, unsigned );
extern unsigned         SymHdl2ObjGblName( imp_image_handle *, imp_sym_handle *, char *, unsigned );
extern unsigned         SymHdl2LclName( imp_image_handle *, imp_sym_handle *, char *, unsigned );
extern dip_status       SymHdl2CstValue( imp_image_handle *, imp_sym_handle *, void * );
extern dip_status       SymHdl2CstType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );
extern dip_status       SymHdl2TypType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );
extern dip_status       SymHdl2MbrType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );
extern dip_status       SymHdl2LclType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );
extern dip_status       SymHdl2GblType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );
extern dip_status       SymHdl2MbrLoc( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
extern dip_status       SymHdl2LclLoc( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
extern dip_status       SymHdl2GblLoc( imp_image_handle *, imp_sym_handle *, location_list * );
extern dip_status       SymHdl2GblInfo( imp_image_handle *, imp_sym_handle *, sym_info * );
extern dip_status       SymHdl2LclInfo( imp_image_handle *, imp_sym_handle *, sym_info * );
extern dip_status       SymHdl2MbrInfo( imp_image_handle *, imp_sym_handle *, sym_info *, location_context *  );
extern dip_status       SymHdl2LclParmLoc( imp_image_handle *, imp_sym_handle *, location_context *, location_list *, unsigned );
extern search_result    SearchGbl( imp_image_handle *, imp_mod_handle, imp_mod_handle, lookup_item *, void * );
extern search_result    SearchLclScope( imp_image_handle *, imp_mod_handle, address *, lookup_item *, void * );
extern search_result    SearchLclMod( imp_image_handle *, imp_mod_handle, lookup_item *, void * );
extern search_result    SearchEnumName( imp_image_handle *, imp_mod_handle, lookup_item *, void * );
extern search_result    SearchTypeName( imp_image_handle *, imp_mod_handle, lookup_item *, void * );
extern search_result    SearchMbr( imp_image_handle *, imp_type_handle *, lookup_item *, void * );
extern dip_status       WalkLclModSymList( imp_image_handle *, imp_mod_handle, IMP_SYM_WKR *, imp_sym_handle *, void *, walk_result * );
extern walk_result      WalkGblModSymList( imp_image_handle *, imp_mod_handle, IMP_SYM_WKR *, imp_sym_handle *, void * );
extern walk_result      WalkScopedSymList( imp_image_handle *, address *, IMP_SYM_WKR *, imp_sym_handle *, void * );
extern walk_result      WalkBlockSymList( imp_image_handle *, scope_block *, IMP_SYM_WKR *, imp_sym_handle *, void * );
extern walk_result      WalkTypeSymList( imp_image_handle *, imp_type_handle *, IMP_SYM_WKR *, imp_sym_handle *, void * );
extern void             KillLclLoadStack(void);
extern void             KillTypeLoadStack(void);
extern dip_status       Lcl2GblHdl( imp_image_handle *, imp_sym_handle *, imp_sym_handle * );


const char DIPImpName[] = "WATCOM";

unsigned DIPENTRY DIPImpQueryHandleSize( handle_kind hk )
{
    static unsigned_8 Sizes[] = {
        sizeof( imp_image_handle ),
        sizeof( imp_type_handle ),
        sizeof( imp_cue_handle ),
        sizeof( imp_sym_handle )
    };

    return( Sizes[ hk ] );
}

dip_status DIPENTRY DIPImpStartup()
{
    return( DS_OK );
}

void DIPENTRY DIPImpShutdown()
{
    FiniDemand();
}

void DIPENTRY DIPImpCancel()
{
    KillLclLoadStack();
    KillTypeLoadStack();
    InfoUnlock();
}

dip_status DIPENTRY DIPImpMoreMem( unsigned size )
{
    size = size;
    return( InfoRelease() );
}

imp_mod_handle DIPENTRY DIPImpSymMod( imp_image_handle *ii, imp_sym_handle *is )
{
    ii = ii;
    return( is->im );
}

search_result DIPENTRY DIPImpAddrSym( imp_image_handle *ii, imp_mod_handle im,
                        address addr, imp_sym_handle *is )
{
    search_result       sr;

    if( im == NO_MOD ) {
        if( ImpInterface.addr_mod( ii, addr, &is->im ) == SR_NONE ) return( SR_NONE );
    } else {
        is->im = im;
    }
    sr = LookupLclAddr( ii, addr, is );
    if( sr != SR_NONE ) return( sr );
    return( LookupGblAddr( ii, addr, is ) );
}

#define SH_ESCAPE       '\xf0'

unsigned DIPENTRY DIPImpSymName( imp_image_handle *ii, imp_sym_handle *is,
                                location_context *lc,
                                symbol_name sn, char *name, unsigned max )
{
    byte                *sp;
    byte                *ep;
    byte                curr;
    unsigned            len;
    char                *buff;
    location_list       ll;
    imp_sym_handle      gbl_is;

    switch( sn ) {
    case SN_EXPRESSION:
        sp = (byte *)is;
        ++is;
        len = 0;
        #define STUFF_IT( c )   if( (len+1) < max ) *ep++ = (c); ++len
        ep = name;
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
        if( max > 0 ) *ep++ = '\0';
        return( len );
    case SN_DEMANGLED:
        len = ImpInterface.sym_name( ii, is, lc, SN_OBJECT, NULL, 0 );
        if( len == 0 ) return( len );
        buff = __alloca( len + 1 );
        ImpInterface.sym_name( ii, is, lc, SN_OBJECT, buff, len + 1 );
        if( !__is_mangled( buff, len ) ) return( 0 );
        return( __demangle_l( buff, len, name, max ) );
    case SN_OBJECT:
        switch( is->type ) {
        case SH_LCL:
            if( Lcl2GblHdl( ii, is, &gbl_is ) != DS_OK ) break;
            return( SymHdl2ObjGblName( ii, &gbl_is, name, max ) );
        case SH_MBR:
            if( ImpInterface.sym_location( ii, is, lc, &ll ) != DS_OK ) break;
            if( ll.num != 1 || ll.e[0].type != LT_ADDR ) break;
            if( ImpInterface.addr_mod( ii, ll.e[0].u.addr, &gbl_is.im ) == SR_NONE ) break;
            if( LookupGblAddr(ii,ll.e[0].u.addr,&gbl_is) != SR_EXACT ) break;
            is = &gbl_is;
            /* fall through */
        case SH_GBL:
            return( SymHdl2ObjGblName( ii, is, name, max ) );
        }
        /* fall through */
    case SN_SOURCE:
        switch( is->type ) {
        case SH_GBL:
            return( SymHdl2GblName( ii, is, name, max ) );
        case SH_LCL:
            return( SymHdl2LclName( ii, is, name, max ) );
        case SH_MBR:
            return( SymHdl2MbrName( ii, is, name, max ) );
        case SH_TYP:
            return( SymHdl2TypName( ii, is, name, max ) );
        case SH_CST:
            return( SymHdl2CstName( ii, is, name, max ) );
        }
        break;
    }
    return( 0 );
}


static void CollectSymHdl( byte *ep, imp_sym_handle *is )
{
    byte        *sp;
    byte        curr;
    static byte escapes[] = { SH_ESCAPE, '\0', '`' };

    ++ep;
    sp = (byte *)is;
    ++is;
    while( sp < (byte *)is ) {
        curr = *ep++;
        if( curr == SH_ESCAPE ) curr = escapes[ *ep++ - 1 ];
        *sp++ = curr;
    }
}

static search_result SearchFileScope( imp_image_handle *ii,
                        imp_mod_handle im, lookup_item *li, void *d )
{
    search_result       sr;

    if( im == (imp_mod_handle)NO_MOD ) return( SR_NONE );
    switch( li->type ) {
    case ST_NONE:
        sr = SearchLclMod( ii, im, li, d );
        if( sr != SR_NONE ) return( sr );
        return( SearchEnumName( ii, im, li, d ) );
    case ST_DESTRUCTOR:
    case ST_OPERATOR:
        return( SR_NONE );
    default:
        return( SearchTypeName( ii, im, li, d ) );
    }
}

search_result DoLookupSym( imp_image_handle *ii, symbol_source ss,
                         void *source, lookup_item *li, location_context *lc, void *d )
{
    imp_mod_handle      im;
    search_result       sr;
    lookup_item         item;
    char                *buff;
    char                *src;
    char                *dst;
    unsigned            len;
    unsigned            op_len;
    imp_sym_handle      *scope_is;

    if( *li->name.start == SH_ESCAPE ) {
        CollectSymHdl( li->name.start, DCSymCreate( ii, d ) );
        return( SR_EXACT );
    }
    if( li->type == ST_NAMESPACE ) return( SR_NONE );
    item = *li;
    if( ss == SS_SCOPESYM ) {
        scope_is = source;
        len = ImpInterface.sym_name( ii, scope_is, NULL, SN_SOURCE, NULL, 0 );
        item.scope.start = __alloca( len + 1 );
        ImpInterface.sym_name( ii, scope_is, NULL, SN_SOURCE, item.scope.start, len + 1 );
        item.scope.len = len;
        ss = SS_MODULE;
        item.mod = scope_is->im;
        source = &item.mod;
    }
    if( item.type == ST_OPERATOR ) {
        src = item.name.start;
        len = item.name.len;
        buff = __alloca( len + 20 );
        dst = buff;
        for( ;; ) {
            if( len == 0 ) break;
            if( src == item.source.start ) {
                op_len = __mangle_operator( src, item.source.len, buff );
                if( op_len == 0 ) {
                    DCStatus( DS_ERR|DS_INVALID_OPERATOR );
                    return( SR_NONE );
                }
                dst += op_len;
                src += item.source.len;
                len -= item.source.len;
            } else {
                *dst++ = *src++;
                --len;
            }
        }
        item.name.len = dst - buff;
        item.name.start = buff;
    }
    sr = SR_NONE;
    switch( ss ) {
    case SS_SCOPED:
        if( ImpInterface.addr_mod( ii, *(address *)source, &im ) == SR_NONE ) {
            im = item.mod;
        } else if( item.mod == NO_MOD || item.mod == im ) {
            if( !item.file_scope && item.type == ST_NONE ) {
                sr = SearchLclScope( ii, im, (address *)source, &item, d );
            }
        } else {
            im = item.mod;
        }
        if( im != NO_MOD && sr == SR_NONE ) {
            sr = SearchFileScope( ii, im, &item, d );
        }
        break;
    case SS_MODULE:
        im = *(imp_mod_handle *)source;
        if( item.mod == NO_MOD || item.mod == im ) {
            sr = SearchFileScope( ii, im, &item, d );
        }
        break;
    case SS_TYPE:
        if( item.mod != NO_MOD ) return( SR_NONE );
        switch( item.type ) {
        case ST_TYPE:
        case ST_STRUCT_TAG:
        case ST_CLASS_TAG:
        case ST_UNION_TAG:
        case ST_ENUM_TAG:
            return( SR_NONE );
        }
        return( SearchMbr( ii, (imp_type_handle *)source, &item, d ) );
    }
    if( sr == SR_NONE ) {
        switch( item.type ) {
        case ST_NONE:
        case ST_DESTRUCTOR:
        case ST_OPERATOR:
            sr = SearchGbl( ii, im, (imp_mod_handle)item.mod, &item, d );
            break;
        }
    }
    return( sr );
}

search_result DIPENTRY DIPImpLookupSym( imp_image_handle *ii, symbol_source ss,
                         void *source, lookup_item *li, void *d )
{
    return( DoLookupSym( ii, ss, source, li, NULL, d ) );
}

search_result DIPENTRY DIPImpLookupSymEx( imp_image_handle *ii, symbol_source ss,
                         void *source, lookup_item *li, location_context *lc, void *d )
{
    return( DoLookupSym( ii, ss, source, li, lc, d ) );
}

dip_status DIPENTRY DIPImpSymLocation( imp_image_handle *ii, imp_sym_handle *is,
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


dip_status DIPENTRY DIPImpSymValue( imp_image_handle *ii, imp_sym_handle *is,
                                location_context *lc, void *value )
{
    lc = lc;
    switch( is->type ) {
    case SH_CST:
        return( SymHdl2CstValue( ii, is, value ) );
    }
    return( DS_ERR|DS_BAD_PARM );
}


dip_status DIPENTRY DIPImpSymType( imp_image_handle *ii, imp_sym_handle *is,
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

dip_status DIPENTRY DIPImpSymInfo( imp_image_handle *ii, imp_sym_handle *is,
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

dip_status DIPENTRY DIPImpSymParmLocation( imp_image_handle *ii,
    imp_sym_handle *is, location_context *lc, location_list *ll, unsigned parm )
{
    if( is->type != SH_LCL ) return( DS_FAIL );
    return( SymHdl2LclParmLoc( ii, is, lc, ll, parm ) );
}

typedef struct {
    IMP_SYM_WKR         *walk;
    imp_sym_handle      *is;
    void                *d;
} sym_glue;

walk_result WalkMySyms( imp_image_handle *ii,
                        imp_mod_handle im, void *d )
{
    sym_glue    *wd = d;
    walk_result wr;

    WalkLclModSymList( ii, im, wd->walk, wd->is, wd->d, &wr );
    return( wr );
}

walk_result DoWalkSymList( imp_image_handle *ii, symbol_source ss,
                void *t, IMP_SYM_WKR *wk, imp_sym_handle *is, void *d )
{
    imp_mod_handle      im;
    sym_glue            glue;
    walk_result         wr;

    switch( ss ) {
    case SS_TYPE:
        return( WalkTypeSymList( ii, (imp_type_handle *)t, wk, is, d ) );
    case SS_SCOPED:
        return( WalkScopedSymList( ii, (address *)t, wk, is, d ) );
    case SS_BLOCK:
        return( WalkBlockSymList( ii, (scope_block *)t, wk, is, d ) );
    case SS_MODULE:
        im = *(imp_mod_handle *)t;
        if( im == (imp_mod_handle)NO_MOD ) {
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

walk_result DIPENTRY DIPImpWalkSymList( imp_image_handle *ii, symbol_source ss,
                void *source, IMP_SYM_WKR *wk, imp_sym_handle *is, void *d )
{
    return( DoWalkSymList( ii, ss, source, wk, is, d ) );
}

walk_result DIPENTRY DIPImpWalkSymListEx( imp_image_handle *ii, symbol_source ss,
                void *source, IMP_SYM_WKR *wk, imp_sym_handle *is,
                location_context *lc, void *d )
{
    lc=lc;
    return( DoWalkSymList( ii, ss, source, wk, is, d ) );
}

dip_status DIPENTRY DIPImpModDefault( imp_image_handle *ii, imp_mod_handle im,
                        default_kind dk, type_info *ti )
{
    ii = ii; im = im; dk = dk; ti = ti;
    return( DS_FAIL );
}

static int GblCmp( void *g1, void *g2 )
{
#if defined(__COMPACT__) || defined(__LARGE__) || defined( __HUGE__ )
    unsigned    s1;
    unsigned    s2;

    s1 = FP_SEG( g1 );
    s2 = FP_SEG( g2 );
    if( s1 != s2 ) return( s1 - s2 );
    return( FP_OFF( g1 ) - FP_OFF( g2 ) );
#else
    return( (char*)g1 - (char*)g2 );
#endif
}

int DIPENTRY DIPImpSymCmp( imp_image_handle *ii, imp_sym_handle *is1,
                        imp_sym_handle *is2 )
{
    ii = ii;
    if( is1->im != is2->im ) return( is1->im - is2->im );
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

dip_status DIPENTRY DIPImpSymAddRef( imp_image_handle *ii, imp_sym_handle *is )
{
    ii=ii;
    is=is;
    return(DS_OK);
}

dip_status DIPENTRY DIPImpSymRelease( imp_image_handle *ii, imp_sym_handle *is )
{
    ii=ii;
    is=is;
    return(DS_OK);
}

dip_status DIPENTRY DIPImpSymFreeAll( imp_image_handle *ii )
{
    ii=ii;
    return(DS_OK);
}
