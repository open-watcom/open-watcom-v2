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

#include "clibext.h"


static walk_result     DoWalkSymList( imp_image_handle *ii,
                symbol_source ss, void *source, DIP_IMP_SYM_WALKER *wk,
                imp_sym_handle *is, void *d )
{
    walk_result         wr;

    /* unused parameters */ (void)source;

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

walk_result DIPIMPENTRY( WalkSymList )( imp_image_handle *ii,
                symbol_source ss, void *source, DIP_IMP_SYM_WALKER *wk,
                imp_sym_handle *is, void *d )
{
    return( DoWalkSymList( ii, ss, source, wk, is, d ) );
}

walk_result DIPIMPENTRY( WalkSymListEx )( imp_image_handle *ii, symbol_source ss,
                void *source, DIP_IMP_SYM_WALKER *wk, imp_sym_handle *is,
                location_context *lc, void *d )
{
    /* unused parameters */ (void)lc;

    return( DoWalkSymList( ii, ss, source, wk, is, d ) );
}


imp_mod_handle DIPIMPENTRY( SymMod )( imp_image_handle *ii,
                        imp_sym_handle *is )
{
    /* unused parameters */ (void)ii; (void)is;

    return( IMH_MAP );
}

size_t DIPIMPENTRY( SymName )( imp_image_handle *ii,
                        imp_sym_handle *is, location_context *lc,
                        symbol_name sn, char *buff, size_t buff_size )
{
    /* unused parameters */ (void)ii; (void)lc;

    if( sn == SN_DEMANGLED ) return( 0 );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > is->p->len )
            buff_size = is->p->len;
        memcpy( buff, is->p->name, buff_size );
        buff[buff_size] = '\0';
    }
    return( is->p->len );
}

dip_status DIPIMPENTRY( SymType )( imp_image_handle *ii,
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

dip_status DIPIMPENTRY( SymLocation )( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, location_list *ll )
{
    /* unused parameters */ (void)ii; (void)lc;

    ll->num = 1;
    ll->flags = 0;
    ll->e[0].bit_start = 0;
    ll->e[0].bit_length = 0;
    ll->e[0].type = LT_ADDR;
    ll->e[0].u.addr.mach = is->p->addr;
    DCAddrSection( &ll->e[0].u.addr );
    return( DS_OK );
}

dip_status DIPIMPENTRY( SymValue )( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, void *buff )
{
    /* unused parameters */ (void)ii; (void)is; (void)lc; (void)buff;

    return( DS_FAIL );
}

dip_status DIPIMPENTRY( SymInfo )( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, sym_info *si )
{
    msym_block  *b;

    /* unused parameters */ (void)lc;

    memset( si, 0, sizeof( *si ) );
    b = FindAddrBlock( ii, is->p->addr );
    if( b != NULL && b->code ) {
        si->kind = SK_CODE;
    } else {
        si->kind = SK_DATA;
    }
    si->is_global = 1;
    si->is_static = 1;
    return( DS_OK );
}

dip_status DIPIMPENTRY( SymParmLocation )( imp_image_handle *ii,
                    imp_sym_handle *is, location_context *lc,
                    location_list *ll, unsigned n )
{
    /* unused parameters */ (void)ii; (void)is; (void)lc; (void)ll; (void)n;

    return( DS_FAIL );
}

dip_status DIPIMPENTRY( SymObjType )( imp_image_handle *ii,
                    imp_sym_handle *is, imp_type_handle *it, dip_type_info *ti )
{
    /* unused parameters */ (void)ii; (void)is; (void)it; (void)ti;

    return( DS_FAIL );
}

dip_status DIPIMPENTRY( SymObjLocation )( imp_image_handle *ii,
                                imp_sym_handle *is, location_context *lc,
                                 location_list *ll )
{
    /* unused parameters */ (void)ii; (void)is; (void)lc; (void)ll;

    return( DS_FAIL );
}

search_result DIPIMPENTRY( AddrSym )( imp_image_handle *ii,
                            imp_mod_handle im, address a, imp_sym_handle *is )
{
    msym_sym    *s;

    /* unused parameters */ (void)im;

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


static search_result DoLookupSym( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
    int         (*cmp)(const void *, const void *, size_t);
    msym_sym    *s;

    /* unused parameters */ (void)lc; (void)source;

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

search_result DIPIMPENTRY( LookupSym )( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li, void *d )
{
    return( DoLookupSym( ii, ss, source, li, NULL, d ) );
}

search_result DIPIMPENTRY( LookupSymEx )( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
    return( DoLookupSym( ii, ss, source, li, lc, d ) );
}

search_result DIPIMPENTRY( AddrScope )( imp_image_handle *ii,
                imp_mod_handle im, address addr, scope_block *scope )
{
    /* unused parameters */ (void)ii; (void)im; (void)addr; (void)scope;

    return( SR_NONE );
}

search_result DIPIMPENTRY( ScopeOuter )( imp_image_handle *ii,
                imp_mod_handle im, scope_block *in, scope_block *out )
{
    /* unused parameters */ (void)ii; (void)im; (void)in; (void)out;

    return( SR_NONE );
}

int DIPIMPENTRY( SymCmp )( imp_image_handle *ii, imp_sym_handle *is1, imp_sym_handle *is2 )
{
    void        *g1;
    void        *g2;

    /* unused parameters */ (void)ii;

    g1 = is1->p;
    g2 = is2->p;
#if defined( __COMPACT__ ) || defined( __LARGE__ ) || defined( __HUGE__ )
    {
        unsigned        s1;
        unsigned        s2;

        s1 = FP_SEG( g1 );
        s2 = FP_SEG( g2 );
        if( s1 < s2 )
            return( -1 );
        if( s1 > s2 )
            return( 1 );
        if( FP_OFF( g1 ) < FP_OFF( g2 ) )
            return( -1 );
        if( FP_OFF( g1 ) > FP_OFF( g2 ) )
            return( 1 );
        return( 0 );
    }
#else
    if( (char*)g1 < (char*)g2 )
        return( -1 );
    if( (char*)g1 > (char*)g2 )
        return( 1 );
    return( 0 );
#endif
}

dip_status DIPIMPENTRY( SymAddRef )( imp_image_handle *ii, imp_sym_handle *is )
{
    /* unused parameters */ (void)ii; (void)is;

    return( DS_OK );
}

dip_status DIPIMPENTRY( SymRelease )( imp_image_handle *ii, imp_sym_handle *is )
{
    /* unused parameters */ (void)ii; (void)is;

    return( DS_OK );
}

dip_status DIPIMPENTRY( SymFreeAll )( imp_image_handle *ii )
{
    /* unused parameters */ (void)ii;

    return( DS_OK );
}
