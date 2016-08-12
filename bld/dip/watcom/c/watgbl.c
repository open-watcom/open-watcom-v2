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
#include <ctype.h>
#include "demangle.h"
#include "watldsym.h"
#include "watlcl.h"
#include "watgbl.h"
#include "watmod.h"

#include "clibext.h"


#define HL_END                  ((hash_link)-1)
#define MAKE_LP( la, off )      ((gbl_link *)((byte *)(la) + (off)))
#define MK_ADDR( a, ma, sect )  {a.mach=ma;a.sect_id=sect;a.indirect=true;}

#if defined(__DOS__) && !defined(__386__)
#define SYM_TAB_SIZE  128       /* for DOS */
#else
#define SYM_TAB_SIZE  256       /* for less memory constrained environs */
#endif

#define GBL_OFFS(x)             (GblNameOff?1+x:0+x)
#define GBL_KIND( gbl )         ((unsigned char)(GblNameOff?(gbl)->name[0]:0))
#define GBL_NAME( gbl )         ((gbl)->name + GBL_OFFS(1))
#define GBL_NAMELEN( gbl )      ((unsigned char)((gbl)->name[GBL_OFFS(0)]))
#define GBL_MOD( gbl )          (IDX2IMH((gbl)->mod))
/* I know -- this is evil. It's temporary and it's fast */
#define SET_GBLNAMEOFF( ctl )   (GblNameOff=!((ctl)->v2))

#define LINK( blk )     ((gbl_link_info *)blk->link)

typedef word            hash_link;

typedef struct gbl_link {
    gbl_info            *gbl;
    hash_link           hash_off;
    byte                src_off;
    byte                src_len;
    bool                dtor;
} gbl_link;

typedef struct gbl_link_info {
    gbl_link            *end;
    hash_link           hash[SYM_TAB_SIZE];
    gbl_link            link[1];
} gbl_link_info;

static bool             GblNameOff;

void GblSymFini( section_info *inf )
{
    info_block          *ptr;

    for( ptr = inf->gbl; ptr != NULL; ptr = ptr->next ) {
        DCFree( ptr->link );
        ptr->link = NULL;
    }
}

/*
 * GblNamHash -- hash a symbol name
 */

static unsigned GblNameHash( const char *name, size_t name_len )
{
    unsigned    rtrn;

    rtrn = name_len;
    rtrn += toupper( GETU8( name ) );
    rtrn += toupper( GETU8( name + name_len / 2 ) );
    rtrn += toupper( GETU8( name + name_len - 1 ) );
    return( rtrn & (SYM_TAB_SIZE - 1) );
}

static void GblCreate( imp_sym_handle *is, gbl_info *gbl )
{
    is->type = SH_GBL;
    is->im = GBL_MOD( gbl );
    is->name_off = (byte)( GBL_NAME( gbl ) - (char *)gbl - 1 );
    is->u.gbl = gbl;
}

static int source_name( const char *gstart, size_t glen, const char **rstart, size_t *rlen )
{
    int         type;

    type = __is_mangled_internal( gstart, glen );
    switch( type ) {
    case __NOT_MANGLED:
        *rstart = gstart;
        gstart = memchr( gstart, '@', glen );
        if( gstart != NULL ) {
            /* stupid MS stdcall with it's stupid trailing "@<num>" */
            glen = gstart - *rstart;
            type = __MANGLED;
        }
        *rlen = glen;
        break;
    case __MANGLED:
    case __MANGLED_INTERNAL:
        __unmangled_name( gstart, glen, rstart, rlen );
        break;
    case __MANGLED_CTOR:
    case __MANGLED_DTOR:
        __scope_name( gstart, glen, 0, rstart, rlen );
        break;
    }
    return( type );
}

/*
 * SearchGbl -- look up a global symbol name
 */
static search_result LkupGblName( section_info *inf, imp_mod_handle cim,
                        imp_mod_handle im, lookup_item *li, void *d )
{
    gbl_link            *lnk;
    gbl_link            *lnk_array;
    gbl_info            *gbl;
    hash_link           lnk_off;
    int                 (*compare)(void const *,void const *,size_t);
    const char          *gblname;
    size_t              gbllen;
    const char          *nam = NULL;
    size_t              namlen = 0;
    const char          *snam;
    size_t              snamlen;
    const char          *mangled_name;
    size_t              mangled_len;
    unsigned            entry;
    info_block          *blk;
    bool                lkup_dtor;
    bool                lkup_full;
    imp_sym_handle      *is;
    address             addr;
    search_result       sr;

    sr = SR_NONE;
    compare = li->case_sensitive ? memcmp : memicmp;

    lkup_dtor = (li->type == ST_DESTRUCTOR);
    lkup_full = false;
    /* only want to hash the source code portion of the name */
    switch( source_name( li->name.start, li->name.len, &snam, &snamlen ) ) {
    case __NOT_MANGLED:
        nam = snam;
        namlen = snamlen;
        break;
    case __MANGLED_DTOR:
        lkup_dtor = true;
        /* fall through */
    case __MANGLED_CTOR:
    case __MANGLED:
    case __MANGLED_INTERNAL:
        lkup_full = true;
        nam = li->name.start;
        namlen = li->name.len;
        break;
    }
    for( blk = inf->gbl; blk != NULL; blk = blk->next ) {
        lnk_array = LINK( blk )->link;
        for( lnk_off = LINK( blk )->hash[GblNameHash( snam, snamlen )]; lnk_off != HL_END; lnk_off = lnk->hash_off ) {
            lnk = MAKE_LP( lnk_array, lnk_off );
            gbl = lnk->gbl;
            if( lnk->dtor != lkup_dtor )
                continue;
            if( lkup_full ) {
                if( GBL_NAMELEN( gbl ) != namlen ) {
                    continue;
                }
            } else {
                if( lnk->src_len != namlen ) {
                    continue;
                }
            }
            if( im == IMH_NOMOD ) {
                if( (GBL_KIND( gbl ) & GBL_KIND_STATIC) && cim != GBL_MOD( gbl ) ) {
                    continue;
                }
            } else {
                if( im != GBL_MOD( gbl ) ) {
                    continue;
                }
            }
            mangled_name = GBL_NAME( gbl );
            gblname = mangled_name;
            if( !lkup_full )
                gblname += lnk->src_off;
            if( compare( gblname, nam, namlen ) != 0 )
                continue;
            if( li->scope.start != NULL ) {
                int     rc;

                mangled_len = GBL_NAMELEN( gbl );
                for( entry = 0;
                  (rc = __scope_name( mangled_name, mangled_len, entry, &gblname, &gbllen )) != 0;
                  ++entry ) {
                    if( li->scope.len == gbllen && compare( li->scope.start, gblname, gbllen ) == 0 ) {
                        break;
                    }
                }
                if( rc == 0 ) {
                    continue;
                }
            }
            is = DCSymCreate( inf->ctl, d );
            is->im = GBL_MOD( gbl );
            MK_ADDR( addr, gbl->addr, inf->sect_id );
            /* need to see if there's a local symbol at the right
                    address and use that instead */
            if( cim == is->im ) {
                /* We've already checked the local symbols. It ain't there. */
                GblCreate( is, gbl );
            } else if( LookupLclAddr( inf->ctl, addr, is ) == SR_EXACT ) {
                SetGblLink( is, gbl );
            } else {
                GblCreate( is, gbl );
            }
            sr = SR_EXACT;
        }
    }
    return( sr );
}

search_result SearchGbl( imp_image_handle *ii, imp_mod_handle cim,
                        imp_mod_handle im, lookup_item *li, void *d )
{
    section_info        *inf;
    section_info        *end;
    search_result       sr;

    SET_GBLNAMEOFF( ii );
    sr = SR_NONE;
    end = ii->sect + ii->num_sects;
    for( inf = ii->sect; inf < end; ++inf ) {
        if( LkupGblName( inf, cim, im, li, d ) != SR_NONE ) {
            sr = SR_EXACT;
        }
    }
    return( sr );
}

static int MachAddrComp( addr_ptr a, imp_mod_handle ima,
                         addr_ptr b, imp_mod_handle imb )
{
    if( a.segment < b.segment )
        return( -1 );
    if( a.segment > b.segment )
        return( 1 );
    if( a.offset < b.offset )
        return( -1 );
    if( a.offset > b.offset )
        return( 1 );
    if( ima < imb )
        return( -1 );
    if( ima > imb )
        return( 1 );
    return( 0 );
}

/*
 * LookupGblAddr -- look up a global address
 */
static search_result LkupGblAddr( info_block *inf, imp_sym_handle *is, addr_ptr addr )
{
    gbl_link            *low, *high;
    gbl_link            *nearest;
    gbl_link            *target;
    gbl_info            *gbl;
    int                 comp_rtn;
 
    low = LINK( inf )->link;
    nearest = NULL;
    high = LINK( inf )->end - 1;
    while( low <= high ) {
        /*
         * This is to allow the code to work when the size
         * of the link block exceeds 32K.
         */
        target = low + (unsigned)( (byte *)high - (byte *)low ) / ( 2 * sizeof( gbl_link ) );
        gbl = target->gbl;
        comp_rtn = MachAddrComp( addr, is->im, gbl->addr, GBL_MOD( gbl ) );
        if( comp_rtn < 0 ) {
            high = target - 1;
        } else if( comp_rtn > 0 ) {
            if( GBL_MOD( gbl ) == is->im && gbl->addr.segment == addr.segment ) {
                nearest = target;
            }
            low = target + 1;
        } else {
            is->u.gbl = gbl;
            return( SR_EXACT );
        }
    }
    if( nearest == NULL )
        return( SR_NONE );
    gbl = nearest->gbl;
    if( is->u.gbl != NULL
        && is->u.gbl->addr.offset > gbl->addr.offset ) {
        return( SR_NONE );
    }
    is->u.gbl = gbl;
    return( SR_CLOSEST );
}


search_result LookupGblAddr( imp_image_handle *ii, address addr, imp_sym_handle *is )
{
    section_info        *inf;
    info_block          *curr;
    search_result       sr = SR_NONE;

    is->u.gbl = NULL;
    SET_GBLNAMEOFF( ii );
    inf = FindInfo( ii, is->im );
    for( curr = inf->gbl; curr != NULL; curr = curr->next ) {
        if( addr.sect_id == 0 || addr.sect_id == inf->sect_id ) {
            sr = LkupGblAddr( curr, is, addr.mach );
            if( sr == SR_EXACT ) {
                break;
            }
        }
    }
    if( is->u.gbl == NULL )
        return( SR_NONE );
    GblCreate( is, is->u.gbl );
    return( ( sr == SR_EXACT ) ? SR_EXACT : SR_CLOSEST );
}


/*
 * Insert -- insert into hash table
 */

static void Insert( info_block *inf, gbl_link *new )
{
    hash_link           *owner;
    gbl_info            *gbl;
    const char          *name;
    const char          *mangled_name;
    size_t              name_len;

    gbl = new->gbl;
    /* only want to hash the source code portion of the name */
    mangled_name = GBL_NAME( gbl );
    new->dtor = ( source_name( mangled_name, GBL_NAMELEN( gbl ), &name, &name_len ) == __MANGLED_DTOR );
    new->src_off = name - mangled_name;
    new->src_len = name_len;
    owner = LINK( inf )->hash + GblNameHash( name, name_len );
    new->hash_off = *owner;
    *owner = (byte *)new - (byte *)LINK( inf )->link;
}


static int CmpInfo( const void *a, const void *b )
{
    gbl_info    *ga = ((gbl_link *)a)->gbl;
    gbl_info    *gb = ((gbl_link *)b)->gbl;

    return( MachAddrComp( ga->addr, GBL_MOD( ga ), gb->addr, GBL_MOD( gb ) ) );
}


/*
 * AdjustSyms -- adjust symbol table info according to start segment location
 */
void AdjustSyms( section_info *inf )
{
    info_block  *ginf;
    gbl_link    *lnk;
    gbl_info    *gbl;
    unsigned    count;

    SET_GBLNAMEOFF( inf->ctl );
    for( ginf = inf->gbl; ginf != NULL; ginf = ginf->next ) {
        count = 0;
        for( lnk = LINK( ginf )->link; lnk < LINK( ginf )->end; ++lnk ) {
            gbl = lnk->gbl;
            if( inf->ctl->v2 )
                gbl->mod = ModOff2Idx( inf, gbl->mod );
            gbl->mod += inf->mod_base_idx;
            AddressMap( inf->ctl, &gbl->addr );
            ++count;
        }
        qsort( LINK( ginf )->link, count, sizeof( gbl_link ), CmpInfo );
        for( lnk = LINK( ginf )->link; lnk < LINK( ginf )->end; ++lnk ) {
            Insert( ginf, lnk );
        }
    }
}

#define GBL_SIZE( p )   (sizeof( gbl_info ) + GBL_NAMELEN( p ) + GBL_OFFS( 0 ))

static dip_status DoMakeGblLst( info_block *inf, unsigned size )
{
    unsigned            num_syms;
    gbl_link            *lnk;
    hash_link           *tbl;
    byte                *ptr;
    byte                *start;
    byte                *end;
    gbl_link_info       *link_data;


    start = inf->info;
    end = start + size;
    num_syms = 0;
    for( ptr = start; ptr < end; ptr += GBL_SIZE( (gbl_info *)ptr ) ) {
        ++num_syms;
    }
    link_data = DCAlloc( sizeof( gbl_link_info ) + ( num_syms - 1 ) * sizeof( gbl_link ) );
    if( link_data == NULL ) {
        DCStatus( DS_ERR|DS_NO_MEM );
        return( DS_ERR|DS_NO_MEM );
    }
    inf->link = link_data;
    link_data->end = link_data->link + num_syms;
    tbl = link_data->hash + SYM_TAB_SIZE;
    do {
        --tbl;
        *tbl = HL_END;
    } while( tbl > link_data->hash );
    ptr = start;
    for( lnk = link_data->link; lnk < link_data->end; ++lnk ) {
        lnk->gbl = (gbl_info *)ptr;
        lnk->hash_off = HL_END;
        ptr += GBL_SIZE( (gbl_info *)ptr );
    }
    return( DS_OK );
}

dip_status MakeGblLst( section_info *inf )
{
    info_block          *gbl;
    dip_status          status;

    SET_GBLNAMEOFF( inf->ctl );

    for( gbl = inf->gbl; gbl != NULL; gbl = gbl->next ) {
        status = DoMakeGblLst( gbl, gbl->size );
        if( status != DS_OK ) {
            return( status );
        }
    }
    return( DS_OK );
}


unsigned GblSymSplit( info_block *gbl, section_info *inf )
{
    byte            *ptr;
    unsigned        size;
    unsigned        total;
    unsigned        next;

    SET_GBLNAMEOFF( inf->ctl );
    ptr = gbl->info;
    /* check if there is enough there to pick up the name length field */
    for( total = 0; ( total + sizeof( gbl_info ) + 1 ) <= gbl->size; total = next ) {
        size = GBL_SIZE( (gbl_info *)ptr );
        next = total + size;
        if( next > gbl->size )
            break;
        if( next <= total )
            break;
        ptr += size;
    }
    return( total );
}


dip_status SymHdl2GblLoc( imp_image_handle *ii, imp_sym_handle *is,
                        location_list *ll )
{
    gbl_info            *gbl;
    address             addr;

    gbl = is->u.gbl;
    MK_ADDR( addr, gbl->addr, FindInfo( ii, is->im )->sect_id );
    LocationCreate( ll, LT_ADDR, &addr );
    return( DS_OK );
}


dip_status SymHdl2GblInfo( imp_image_handle *ii, imp_sym_handle *is,
                        sym_info *si )
{
    gbl_info            *gbl;
    unsigned            kind;

    ii = ii;
    gbl = is->u.gbl;
    kind = GBL_KIND( gbl );
    if( kind & GBL_KIND_DATA ) {
        si->kind = SK_DATA;
    } else if( kind & GBL_KIND_CODE ) {
        si->kind = SK_CODE;
    } else {
        si->kind = SK_NONE;
    }
    si->is_global = ( (kind & GBL_KIND_STATIC) == 0 );
    return( DS_OK );
}

unsigned SymHdl2GblName( imp_image_handle *ii, imp_sym_handle *is,
                        char *buff, unsigned buff_size )
{
    size_t      len;
    const char  *gbl;

    ii = ii;
    gbl = (const char *)is->u.gbl + is->name_off;
    len = GETU8( gbl++ );
    __unmangled_name( gbl, len, &gbl, &len );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, gbl, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}

unsigned SymHdl2ObjGblName( imp_image_handle *ii, imp_sym_handle *is,
                        char *buff, unsigned buff_size )
{
    unsigned    len;
    const char  *gbl;

    ii = ii;
    gbl = (const char *)is->u.gbl + is->name_off;
    len = GETU8( gbl++ );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, gbl, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}

dip_status SymHdl2GblType( imp_image_handle *ii, imp_sym_handle *is,
                        imp_type_handle *it )
{
    SET_GBLNAMEOFF( ii );
    it->im = is->im;
    it->f.all = 0;
    it->f.s.gbl = 1;
    it->t.offset = GBL_KIND( is->u.gbl );
    return( DS_OK );
}

type_kind GblTypeClassify( unsigned kind )
{
    if( kind & GBL_KIND_CODE )
        return( TK_CODE );
    if( kind & GBL_KIND_DATA )
        return( TK_DATA );
    return( TK_NONE );
}


walk_result WalkGblModSymList( imp_image_handle *ii, imp_mod_handle im,
                        IMP_SYM_WKR *wk, imp_sym_handle *is, void *d )
{
    section_info        *inf;
    info_block          *blk;
    byte                *end;
    byte                *ptr;
    unsigned            i;
    walk_result         wr;
    bool                seen_module;

    SET_GBLNAMEOFF( ii );
    seen_module = false;
    inf = ii->sect;
    for( i = 0; i < ii->num_sects; ++i, ++inf ) {
        for( blk = inf->gbl; blk != NULL; blk = blk->next ) {
            end = blk->info + blk->size;
            for( ptr = blk->info; ptr < end; ptr += GBL_SIZE( (gbl_info *)ptr ) ) {
                is->im = GBL_MOD( (gbl_info *)ptr );
                if( im == IMH_NOMOD ) {
                    if( ImpInterface.ModInfo( ii, is->im, HK_SYM ) != DS_OK ) {
                        GblCreate( is, (gbl_info *)ptr );
                        wr = wk( ii, SWI_SYMBOL, is, d );
                        if( wr != WR_CONTINUE ) {
                            return( wr );
                        }
                    }
                } else if( is->im == im ) {
                    GblCreate( is, (gbl_info *)ptr );
                    wr = wk( ii, SWI_SYMBOL, is, d );
                    if( wr != WR_CONTINUE )
                        return( wr );
                    seen_module = true;
                } else if( seen_module ) {
                    /*
                        WARNING: Assuming that all the publics for a
                        particular module are right next to each other
                        in the info.
                    */
                    return( WR_CONTINUE );
                }
            }
        }
    }
    return( WR_CONTINUE );
}

dip_status Link2GblHdl( imp_image_handle *ii, gbl_info *gbl, imp_sym_handle *is )
{
    SET_GBLNAMEOFF( ii );
    GblCreate( is, gbl );
    return( DS_OK );
}
