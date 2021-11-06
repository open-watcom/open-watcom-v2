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
* Description:  Stuff dealing with symbol handles.
*
****************************************************************************/


#include "msym.h"
#if defined( _M_I86 )
#include <i86.h>
#endif

#include "clibext.h"


static walk_result     DoWalkSymList( imp_image_handle *iih,
                symbol_source ss, void *source, DIP_IMP_SYM_WALKER *wk,
                imp_sym_handle *ish, void *d )
{
    walk_result         wr;

    /* unused parameters */ (void)source;

    if( ss != SS_MODULE )
        return( WR_CONTINUE );
    wr = WR_CONTINUE;
    for( ish->p = iih->gbl; ish->p != NULL; ish->p = ish->p->next ) {
        wr = wk( iih, SWI_SYMBOL, ish, d );
        if( wr != WR_CONTINUE )
            break;
    }
    return( wr );
}

walk_result DIPIMPENTRY( WalkSymList )( imp_image_handle *iih,
                symbol_source ss, void *source, DIP_IMP_SYM_WALKER *wk,
                imp_sym_handle *ish, void *d )
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


imp_mod_handle DIPIMPENTRY( SymMod )( imp_image_handle *iih, imp_sym_handle *ish )
{
    /* unused parameters */ (void)iih; (void)ish;

    return( IMH_MAP );
}

size_t DIPIMPENTRY( SymName )( imp_image_handle *iih, imp_sym_handle *ish,
    location_context *lc, symbol_name_type snt, char *buff, size_t buff_size )
{
    /* unused parameters */ (void)iih; (void)lc;

    if( snt == SNT_DEMANGLED )
        return( 0 );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > ish->p->len )
            buff_size = ish->p->len;
        memcpy( buff, ish->p->name, buff_size );
        buff[buff_size] = '\0';
    }
    return( ish->p->len );
}

dip_status DIPIMPENTRY( SymType )( imp_image_handle *iih,
                imp_sym_handle *ish, imp_type_handle *ith )
{
    msym_block  *b;

    b = FindAddrBlock( iih, ish->p->addr );
    if( b != NULL ) {
        ith->code = b->code;
    } else {
        ith->code = 0;
    }
    return( DS_OK );
}

dip_status DIPIMPENTRY( SymLocation )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, location_list *ll )
{
    /* unused parameters */ (void)iih; (void)lc;

    ll->num = 1;
    ll->flags = 0;
    ll->e[0].bit_start = 0;
    ll->e[0].bit_length = 0;
    ll->e[0].type = LT_ADDR;
    ll->e[0].u.addr.mach = ish->p->addr;
    DCAddrSection( &ll->e[0].u.addr );
    return( DS_OK );
}

dip_status DIPIMPENTRY( SymValue )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, void *buff )
{
    /* unused parameters */ (void)iih; (void)ish; (void)lc; (void)buff;

    return( DS_FAIL );
}

dip_status DIPIMPENTRY( SymInfo )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, sym_info *si )
{
    msym_block  *b;

    /* unused parameters */ (void)lc;

    memset( si, 0, sizeof( *si ) );
    b = FindAddrBlock( iih, ish->p->addr );
    if( b != NULL && b->code ) {
        si->kind = SK_CODE;
    } else {
        si->kind = SK_DATA;
    }
    si->is_global = 1;
    si->is_static = 1;
    return( DS_OK );
}

dip_status DIPIMPENTRY( SymParmLocation )( imp_image_handle *iih,
                    imp_sym_handle *ish, location_context *lc,
                    location_list *ll, unsigned n )
{
    /* unused parameters */ (void)iih; (void)ish; (void)lc; (void)ll; (void)n;

    return( DS_FAIL );
}

dip_status DIPIMPENTRY( SymObjType )( imp_image_handle *iih,
                    imp_sym_handle *ish, imp_type_handle *ith, dig_type_info *ti )
{
    /* unused parameters */ (void)iih; (void)ish; (void)ith; (void)ti;

    return( DS_FAIL );
}

dip_status DIPIMPENTRY( SymObjLocation )( imp_image_handle *iih,
                                imp_sym_handle *ish, location_context *lc,
                                 location_list *ll )
{
    /* unused parameters */ (void)iih; (void)ish; (void)lc; (void)ll;

    return( DS_FAIL );
}

search_result DIPIMPENTRY( AddrSym )( imp_image_handle *iih,
                            imp_mod_handle imh, address a, imp_sym_handle *ish )
{
    msym_sym    *s;

    /* unused parameters */ (void)imh;

    ish->p = NULL;
    for( s = iih->gbl; s != NULL; s = s->next ) {
        if( SameAddrSpace( s->addr, a.mach ) && s->addr.offset <= a.mach.offset ) {
              if( ish->p == NULL || ish->p->addr.offset < s->addr.offset ) {
                  ish->p = s;
              }
              if( ish->p->addr.offset == a.mach.offset ) {
                  return( SR_EXACT );
              }
        }
    }
    if( ish->p != NULL )
        return( SR_CLOSEST );
    return( SR_NONE );
}


static search_result DoLookupSym( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
    strcompn_fn *scompn;
    msym_sym    *s;
    const char  *name;
    size_t      len;

    /* unused parameters */ (void)lc; (void)source;

    switch( ss ) {
    case SS_MODULE:
    case SS_SCOPED:
        break;
    default:
        return( SR_NONE );
    }
    if( li->type != ST_NONE )
        return( SR_NONE );
    if( li->scope.start != NULL )
        return( SR_NONE );
    scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
    name = li->name.start;
    len = li->name.len;
    for( s = iih->gbl; s != NULL; s = s->next ) {
        if( s->len == len && scompn( s->name, name, s->len ) == 0 ) {
            DCSymCreate( iih, d )->p = s;
            return( SR_EXACT );
        }
    }
    return( SR_NONE );
}

search_result DIPIMPENTRY( LookupSym )( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li, void *d )
{
    return( DoLookupSym( iih, ss, source, li, NULL, d ) );
}

search_result DIPIMPENTRY( LookupSymEx )( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
    return( DoLookupSym( iih, ss, source, li, lc, d ) );
}

search_result DIPIMPENTRY( AddrScope )( imp_image_handle *iih,
                imp_mod_handle imh, address addr, scope_block *scope )
{
    /* unused parameters */ (void)iih; (void)imh; (void)addr; (void)scope;

    return( SR_NONE );
}

search_result DIPIMPENTRY( ScopeOuter )( imp_image_handle *iih,
                imp_mod_handle imh, scope_block *in, scope_block *out )
{
    /* unused parameters */ (void)iih; (void)imh; (void)in; (void)out;

    return( SR_NONE );
}

int DIPIMPENTRY( SymCmp )( imp_image_handle *iih, imp_sym_handle *ish1, imp_sym_handle *ish2 )
{
    void        *g1;
    void        *g2;

    /* unused parameters */ (void)iih;

    g1 = ish1->p;
    g2 = ish2->p;
#if defined( __COMPACT__ ) || defined( __LARGE__ ) || defined( __HUGE__ )
    {
        unsigned        s1;
        unsigned        s2;

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
    }
#else
    if( (char *)g1 < (char *)g2 )
        return( -1 );
    if( (char *)g1 > (char *)g2 )
        return( 1 );
    return( 0 );
#endif
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
