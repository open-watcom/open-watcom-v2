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
* Description:  Stuff dealing with symbol handles.
*
****************************************************************************/


#include "msym.h"
#if defined( _M_I86 )
#include <i86.h>
#endif


walk_result     DoWalkSymList( imp_image_handle *ii,
                symbol_source ss, void *source, IMP_SYM_WKR *wk,
                imp_sym_handle *is, void *d )
{
    walk_result         wr;

    source = source;
    if( ss != SS_MODULE )
        return( WR_CONTINUE );
    wr = WR_CONTINUE;
    for( is->p = ii->gbl; is->p != NULL; is->p = is->p->next ) {
        wr = wk( ii, SWI_SYMBOL, is, d );
        if( wr != WR_CONTINUE )
            break;
    }
    return( wr );
}

walk_result     DIPENTRY DIPImpWalkSymList( imp_image_handle *ii,
                symbol_source ss, void *source, IMP_SYM_WKR *wk,
                imp_sym_handle *is, void *d )
{
    return( DoWalkSymList( ii, ss, source, wk, is, d ) );
}

walk_result DIPENTRY DIPImpWalkSymListEx( imp_image_handle *ii, symbol_source ss,
                void *source, IMP_SYM_WKR *wk, imp_sym_handle *is,
                location_context *lc, void *d )
{
    lc = lc;
    return( DoWalkSymList( ii, ss, source, wk, is, d ) );
}


imp_mod_handle DIPENTRY DIPImpSymMod( imp_image_handle *ii,
                        imp_sym_handle *is )
{
    ii = ii; is = is;
    return( MY_MOD_ID );
}

unsigned DIPENTRY DIPImpSymName( imp_image_handle *ii,
                        imp_sym_handle *is, location_context *lc,
                        symbol_name sn, char *buff, unsigned max )
{
    ii = ii; lc = lc;
    if( sn == SN_DEMANGLED ) return( 0 );
    if( max > 0 ) {
        --max;
        if( max > is->p->len ) max = is->p->len;
        memcpy( buff, is->p->name, max );
        buff[ max ] = '\0';
    }
    return( is->p->len );
}

dip_status DIPENTRY DIPImpSymType( imp_image_handle *ii,
                imp_sym_handle *is, imp_type_handle *it )
{
    msym_block  *b;

    b = FindAddrBlock( ii, is->p->addr );
    if( b != NULL ) {
        it->code = b->code;
    } else {
        it->code = 0;
    }
    return( DS_OK );
}

dip_status DIPENTRY DIPImpSymLocation( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, location_list *ll )
{
    ii = ii; lc = lc;

    ll->num = 1;
    ll->flags = 0;
    ll->e[0].bit_start = 0;
    ll->e[0].bit_length = 0;
    ll->e[0].type = LT_ADDR;
    ll->e[0].u.addr.mach = is->p->addr;
    DCAddrSection( &ll->e[0].u.addr );
    return( DS_OK );
}

dip_status DIPENTRY DIPImpSymValue( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, void *buff )
{
    ii = ii; is = is; lc = lc; buff = buff;
    return( DS_FAIL );
}

dip_status DIPENTRY DIPImpSymInfo( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, sym_info *si )
{
    msym_block  *b;

    memset( si, 0, sizeof( *si ) );
    lc = lc;
    b = FindAddrBlock( ii, is->p->addr );
    if( b != NULL && b->code ) {
        si->kind = SK_CODE;
    } else {
        si->kind = SK_DATA;
    }
    si->global = 1;
    si->is_static = 1;
    return( DS_OK );
}

dip_status DIPENTRY DIPImpSymParmLocation( imp_image_handle *ii,
                    imp_sym_handle *is, location_context *lc,
                    location_list *ll, unsigned n )
{
    ii = ii; is = is; lc = lc; ll = ll; n = n;
    return( DS_FAIL );
}

dip_status DIPENTRY DIPImpSymObjType( imp_image_handle *ii,
                    imp_sym_handle *is, imp_type_handle *it, dip_type_info *ti )
{
    ii = ii; is = is; it = it; ti = ti;
    return( DS_FAIL );
}

dip_status DIPENTRY DIPImpSymObjLocation( imp_image_handle *ii,
                                imp_sym_handle *is, location_context *lc,
                                 location_list *ll )
{
    ii = ii; is = is; lc = lc; ll = ll;
     return( DS_FAIL );
}

search_result DIPENTRY DIPImpAddrSym( imp_image_handle *ii,
                            imp_mod_handle im, address a, imp_sym_handle *is )
{
    msym_sym    *s;

    im = im;
    is->p = NULL;
    s = ii->gbl;
    for( ;; ) {
        if( s == NULL ) break;
        if( SameAddrSpace( s->addr, a.mach ) && s->addr.offset <= a.mach.offset ) {
              if( is->p == NULL || is->p->addr.offset < s->addr.offset ) {
                  is->p = s;
              }
              if( is->p->addr.offset == a.mach.offset ) return( SR_EXACT );
        }
        s = s->next;
    }
    if( is->p != NULL ) return( SR_CLOSEST );
    return( SR_NONE );
}


search_result DoLookupSym( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
    int         (*cmp)(const void *, const void *, size_t);
    msym_sym    *s;

    source = source;
    switch( ss ) {
    case SS_MODULE:
    case SS_SCOPED:
        break;
    default:
        return( SR_NONE );
    }
    if( li->type != ST_NONE ) return( SR_NONE );
    if( li->scope.start != NULL ) return( SR_NONE );
    if( li->case_sensitive ) {
        cmp = memcmp;
    } else {
        cmp = memicmp;
    }
    for( s = ii->gbl; s != NULL; s = s->next ) {
        if( s->len == li->name.len && cmp( s->name, li->name.start, s->len ) == 0 ) {
            DCSymCreate( ii, d )->p = s;
            return( SR_EXACT );
        }
    }
    return( SR_NONE );
}

search_result DIPENTRY DIPImpLookupSym( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li, void *d )
{
    return( DoLookupSym( ii, ss, source, li, NULL, d ) );
}

search_result DIPENTRY DIPImpLookupSymEx( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
    return( DoLookupSym( ii, ss, source, li, lc, d ) );
}

search_result DIPENTRY DIPImpAddrScope( imp_image_handle *ii,
                imp_mod_handle im, address addr, scope_block *scope )
{
    ii = ii; im = im; addr = addr; scope = scope;
    return( SR_NONE );
}

search_result DIPENTRY DIPImpScopeOuter( imp_image_handle *ii,
                imp_mod_handle im, scope_block *in, scope_block *out )
{
    ii = ii; im = im; in = in; out = out;
    return( SR_NONE );
}

int DIPENTRY DIPImpSymCmp( imp_image_handle *ii, imp_sym_handle *is1,
                                imp_sym_handle *is2 )
{
    void        *g1;
    void        *g2;

    ii = ii;
    g1 = is1->p;
    g2 = is2->p;
#if defined( __COMPACT__ ) || defined( __LARGE__ ) || defined( __HUGE__ )
    {
        unsigned        s1;
        unsigned        s2;

        s1 = FP_SEG( g1 );
        s2 = FP_SEG( g2 );
        if( s1 != s2 ) return( s1 - s2 );
        return( FP_OFF( g1 ) - FP_OFF( g2 ) );
    }
#else
    return( (char*)g1 - (char*)g2 );
#endif
}

dip_status DIPENTRY DIPImpSymAddRef( imp_image_handle *ii, imp_sym_handle *is )
{
    ii = ii; is = is;
    return( DS_OK );
}

dip_status DIPENTRY DIPImpSymRelease( imp_image_handle *ii, imp_sym_handle *is )
{
    ii = ii; is = is;
    return( DS_OK );
}

dip_status DIPENTRY DIPImpSymFreeAll( imp_image_handle *ii )
{
    ii = ii;
    return( DS_OK );
}
