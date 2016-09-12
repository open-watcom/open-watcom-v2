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
* Description:  Locals processing for Watcom style debugging information.
*
****************************************************************************/


#include "dipwat.h"
#include "wataddr.h"
#include "watldsym.h"
#include "watmod.h"
#include "wattype.h"
#include "watgbl.h"
#include "watlcl.h"

#include "clibext.h"


typedef struct lclinfo {
    section_info        *inf;
    const char          *start;
    const char          *end;
    unsigned            base_off;
    address             code_base;
    imp_mod_handle      im;
} lclinfo;

#define NO_BASE ((unsigned short)-1)

const char *GetIndex( const char *ptr, unsigned *value )
{
    unsigned tmp;

    tmp = GETU8( ptr++ );
    if ( tmp & 0x80 ) {
        tmp = (tmp & 0x7f) << 8;
        tmp += GETU8( ptr++ );
    }
    *value = tmp;
    return( ptr );
}

static const char *GetMachAddr( imp_image_handle *ii, const char *ptr, addr_ptr *addr, int is32 )
{
    if( is32 ) {
        *addr = *(addr48_ptr *)ptr;
        ptr += sizeof( addr48_ptr );
    } else {
        ConvAddr32ToAddr48( *(addr32_ptr *)ptr, *addr );
        ptr += sizeof( addr32_ptr );
    }
    AddressMap( ii, addr );
    return( ptr );
}


const char *GetAddress( imp_image_handle *ii, const char *ptr, address *addr, int is32 )
{
    ptr = GetMachAddr( ii, ptr, &addr->mach, is32 );
    DCAddrSection( addr );
    return( ptr );
}

static void LclCreate( imp_sym_handle *is, const char *ptr, const char *name, lclinfo *local )
{
    is->type = SH_LCL;
    is->im = local->im;
    is->u.lcl.base = local->base_off;
    is->u.lcl.offset = ptr - local->start;
    is->u.lcl.gbl_link = NULL;
    is->name_off = (byte)( name - ptr );
}

/*
 * LoadLocalSyms -- load the local symbol information for the module
 */

static dip_status LoadLocalSyms( imp_image_handle *ii, imp_mod_handle im, lclinfo *new )
{
    if( im == IMH_NOMOD )
        return( DS_FAIL );
    new->start = InfoLoad( ii, im, DMND_LOCALS, 0, NULL );
    if( new->start == NULL ) {
        return( DS_FAIL );
    }
    new->end = new->start + InfoSize( ii, im, DMND_LOCALS, 0 );
    new->im = im;
    new->inf = FindInfo( ii, im );
    new->base_off = NO_BASE;
    return( DS_OK );
}

static void PopLoad( lclinfo *local )
{
    if( local->start != NULL ) {
        InfoSpecUnlock( local->start );
    }
}

void KillLclLoadStack( void )
{
    // Nothing to do - not using globals anymore
}

/*
 * ProcBlock -- process a block definition
 */
static const char *ProcBlock( const char *ptr, lcl_defn *defn, lclinfo *local )
{
    if( local->base_off == NO_BASE ) {
        defn->b.start = FindModBase( local->inf->ctl, local->im );
    } else {
        defn->b.start = local->code_base;
    }
    if( defn->i.class >= (CODE_SYMBOL + CODE_BLOCK386) ) {
        defn->b.start.mach.offset += *((dword *)ptr);
        ptr += sizeof( dword );
        defn->b.size = *((dword *)ptr);
        ptr += sizeof( dword );
    } else {
        defn->b.start.mach.offset += *((word *)ptr);
        ptr += sizeof( word );
        defn->b.size = *((word *)ptr);
        ptr += sizeof( word );
    }
    defn->b.parent_block = *((word *)ptr);
    ptr += sizeof( word );
    return( ptr );
}


/*
 * ProcDefn -- process the next definition in the local symbol information
 */
static const char *ProcDefn( const char *ptr, lcl_defn *defn, lclinfo *local )
{
    const char  *end;
    unsigned    parms;

    end = ptr + GETU8( ptr );
    ptr++;
    defn->i.class = GETU8( ptr++ );
    defn->i.unparsed = ptr;
    switch( defn->i.class ) {
    case VAR_SYMBOL | VAR_MODULE386:
        ptr += sizeof( addr48_ptr );
        ptr = GetIndex( ptr, &defn->i.type_index );
        break;
    case VAR_SYMBOL | VAR_MODULE :
        ptr += sizeof( addr32_ptr );
        ptr = GetIndex( ptr, &defn->i.type_index );
        break;
    case VAR_SYMBOL | VAR_MODULE_LOC:
    case VAR_SYMBOL | VAR_LOCAL :
        ptr = SkipLocation( ptr );
        ptr = GetIndex( ptr, &defn->i.type_index );
        break;
    case CODE_SYMBOL | CODE_BLOCK386:
    case CODE_SYMBOL | CODE_BLOCK :
        ptr = ProcBlock( ptr, defn, local );
        break;
    case CODE_SYMBOL | CODE_NEAR_ROUT386:
    case CODE_SYMBOL | CODE_FAR_ROUT386:
    case CODE_SYMBOL | CODE_NEAR_ROUT :
    case CODE_SYMBOL | CODE_FAR_ROUT :
        ptr = ProcBlock( ptr, defn, local );
        defn->r.pro_size = GETU8( ptr++ );
        defn->r.epi_size = GETU8( ptr++ );
        if( defn->i.class >= (CODE_SYMBOL + CODE_BLOCK386) ) {
            defn->r.ret_addr_offset = *((dword *)ptr);
            ptr += sizeof( dword );
        } else {
            defn->r.ret_addr_offset = *((word *)ptr);
            ptr += sizeof( word );
        }
        ptr = GetIndex( ptr, &defn->i.type_index );
        defn->i.unparsed = ptr;
        ptr = SkipLocation( ptr );
        for( parms = GETU8( ptr++ ); parms != 0; --parms ) {
            ptr = SkipLocation( ptr );
        }
        break;
    case CODE_SYMBOL | CODE_MEMBER_SCOPE:
        defn->b.parent_block = *((word *)ptr);
        ptr += sizeof( word );
        ptr = GetIndex( ptr, &defn->i.type_index );
        if( ptr < end ) {
            defn->i.unparsed = ptr;
            /* skip the 'this' pointer type and the object loc expr */
            ptr = SkipLocation( ptr + 1 );
        } else {
            defn->i.unparsed = NULL;
        }
        break;
    }
    defn->i.name = ptr;
    defn->i.namelen = end - ptr;
    return( end );
}

static void NewBase( const char *ptr, lclinfo *local )
{
    ptr += 1;
    if( (GETU8( ptr ) & CLASS_MASK) == NEW_BASE ) {
        local->base_off = ptr - local->start - 1;
        local->code_base.sect_id = local->inf->sect_id;
        local->code_base.indirect = true;
        switch( GETU8( ptr ) & SUBCLASS_MASK ) {
        case ADD_PREV_SEG:
            ptr += 1;
            local->code_base.mach.segment += *(word *)ptr;
            local->code_base.mach.offset = 0;
            break;
        case SET_BASE386:
            GetMachAddr( local->inf->ctl, ptr + 1, &local->code_base.mach, 1 );
            break;
        case SET_BASE:
            GetMachAddr( local->inf->ctl, ptr + 1, &local->code_base.mach, 0 );
            break;
        }
    }
}

static const char *FindBlockRout( const char *ptr, lclinfo *local )
{
    byte        cls;

    while( ptr < local->end ) {
        cls = GETU8( ptr + 1 );
        if( (cls & CLASS_MASK) == CODE_SYMBOL
          && cls != (CODE_SYMBOL | CODE_MEMBER_SCOPE) )  {
            return( ptr );
        }
        NewBase( ptr, local );
        ptr += GETU8( ptr );
    }
    return( NULL );
}

static const char *ModAddrLkupVar( const char *ptr, lclinfo *local )
{
    while( ptr < local->end ) {
        switch( GETU8( ptr + 1 ) ) {
        case VAR_SYMBOL | VAR_MODULE:
        case VAR_SYMBOL | VAR_MODULE386:
        case CODE_SYMBOL | CODE_NEAR_ROUT:
        case CODE_SYMBOL | CODE_FAR_ROUT:
        case CODE_SYMBOL | CODE_NEAR_ROUT386:
        case CODE_SYMBOL | CODE_FAR_ROUT386:
            return( ptr );
        }
        NewBase( ptr, local );
        ptr += GETU8( ptr );
    }
    return( NULL );
}

static const char *FindLclVar( const char *ptr, lclinfo *local )
{
    while( ptr < local->end ) {
        if( (GETU8( ptr + 1 ) & CLASS_MASK) == CODE_SYMBOL )
            break;
        if( GETU8( ptr + 1 ) == (VAR_SYMBOL | VAR_LOCAL) )
            return( ptr );
        NewBase( ptr, local );
        ptr += GETU8( ptr );
    }
    return( NULL );
}


static dip_status DefnLocation( lcl_defn *defn, location_context *lc,
                                location_list *ll, lclinfo *local )
{
    address     addr;

    switch( defn->i.class ) {
    case VAR_SYMBOL | VAR_MODULE386:
        GetAddress( local->inf->ctl, defn->i.unparsed, &addr, 1 );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case VAR_SYMBOL | VAR_MODULE:
        GetAddress( local->inf->ctl, defn->i.unparsed, &addr, 0 );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case VAR_SYMBOL | VAR_LOCAL:
    case VAR_SYMBOL | VAR_MODULE_LOC:
        return( EvalLocation( local->inf->ctl, lc, defn->i.unparsed, ll ) );
    default:
        LocationCreate( ll, LT_ADDR, &defn->b.start );
        break;
    }
    return( DS_OK );
}


static address DefnAddr( lcl_defn *defn, lclinfo *local )
{
    location_list       ll;

    if( DefnLocation( defn, NULL, &ll, local ) == DS_OK
        && ll.num == 1
        && ll.e[0].type == LT_ADDR ) {
        return( ll.e[0].u.addr );
    } else {
        return( NilAddr );
    }
}


search_result SearchLclMod( imp_image_handle *ii, imp_mod_handle im,
                lookup_item *li, void *d )
{
    lcl_defn            defn;
    int                 (*compare)(void const*,void const*,size_t);
    const char          *ptr;
    const char          *next;
    const char          *name;
    size_t              len;
    imp_sym_handle      *is;
    search_result       sr;
    lclinfo             lclld;
    lclinfo             *local = &lclld;


    if( li->scope.start != NULL )
        return( SR_NONE );
    if( LoadLocalSyms( ii, im, &lclld ) != DS_OK )
        return( SR_NONE );
    if( li->case_sensitive ) {
        compare = memcmp;
    } else {
        compare = memicmp;
    }
    name = li->name.start;
    len = li->name.len;
    sr = SR_NONE;
    ptr = local->start;
    for( ;; ) {
        ptr = ModAddrLkupVar( ptr, local );
        if( ptr == NULL )
            break;
        next = ProcDefn( ptr, &defn, local );
        if( len == defn.i.namelen && compare( name, defn.i.name, len ) == 0 ) {
            is = DCSymCreate( ii, d );
            LclCreate( is, ptr, defn.i.name, local );
            sr = SR_EXACT;
        }
        ptr = next;
    }
    PopLoad( local );
    return( sr );
}


static search_result DoLclScope( imp_image_handle *ii, imp_mod_handle im,
                                address addr, lookup_item *li,
                                void *d, lclinfo *local )
{
    lcl_defn            blk;
    lcl_defn            defn;
    const char          *ptr;
    const char          *next;
    unsigned            parent;
    int                 (*compare)(void const*,void const*,size_t);
    const char          *name;
    size_t              len;
    search_result       sr;
    lookup_item         type_li;
    imp_type_handle     it;
    imp_sym_handle      *is;

    if( li->case_sensitive ) {
        compare = memcmp;
    } else {
        compare = memicmp;
    }
    name = li->name.start;
    len  = li->name.len;
    if( li->scope.start != NULL ) {
        type_li.name = li->scope;
        type_li.file_scope = li->file_scope;
        type_li.case_sensitive = li->case_sensitive;
        type_li.scope.start = NULL;
        type_li.type = ST_TYPE;
        sr = LookupTypeName( ii, im, &type_li, &it );
        if( sr == SR_NONE )
            return( SR_NONE );
        return( SearchMbr( ii, &it, li, d ) );
    }
    ptr = local->start;
    for( ;; ) {
        ptr = FindBlockRout( ptr, local );
        if( ptr == NULL )
            return( SR_NONE );
        ptr = ProcDefn( ptr, &blk, local );
        if( DCSameAddrSpace( blk.b.start, addr ) == DS_OK
          && (blk.b.start.mach.offset <= addr.mach.offset)
          && ( blk.b.start.mach.offset + blk.b.size > addr.mach.offset ) ) {
            break;
        }
    }
    sr = SR_NONE;
    for( ;; ) {
        if( blk.i.class == (CODE_SYMBOL | CODE_MEMBER_SCOPE) ) {
            if( FindTypeHandle( ii, im, blk.i.type_index, &it ) == DS_OK ) {
                sr = SearchMbr( ii, &it, li, d );
            }
        } else {
            for( ;; ) {
                ptr = FindLclVar( ptr, local );
                if( ptr == NULL )
                    break;
                next = ProcDefn( ptr, &defn, local );
                if( len == defn.i.namelen
                  && compare( name, defn.i.name, len ) == 0 ) {
                    is = DCSymCreate( ii, d );
                    LclCreate( is, ptr, defn.i.name, local );
                    sr = SR_EXACT;
                }
                ptr = next;
            }
        }
        if( sr != SR_NONE )
            break;
        parent = blk.b.parent_block;
        if( parent == 0 )
            break;
        ptr = ProcDefn( local->start + parent, &blk, local );
    }
    return( sr );
}


search_result SearchLclScope( imp_image_handle *ii, imp_mod_handle im,
                                address *addr, lookup_item *li, void *d )
{
    search_result       sr;
    lclinfo             lclld;
    lclinfo             *local = &lclld;

    if( LoadLocalSyms( ii, im, &lclld ) != DS_OK )
        return( SR_NONE );
    sr = DoLclScope( ii, im, *addr, li, d, local );
    PopLoad( local );
    return( sr );
}


search_result LookupLclAddr( imp_image_handle *ii, address addr,
                                imp_sym_handle *is )
{
    lcl_defn            defn;
    address             mod_addr;
    address             close_addr;
    const char          *ptr;
    const char          *next;
    search_result       sr;
    lclinfo             lclld;
    lclinfo             *local = &lclld;

    if( LoadLocalSyms( ii, is->im, &lclld ) != DS_OK )
        return( SR_NONE );
    sr = SR_NONE;
    next = local->start;
    close_addr.mach.offset = 0;
    for( ;; ) {
        ptr = ModAddrLkupVar( next, local );
        if( ptr == NULL )
            break;
        next = ProcDefn( ptr, &defn, local );
        mod_addr = DefnAddr( &defn, local );
        if( DCSameAddrSpace( addr, mod_addr ) == DS_OK ) {
            if( addr.mach.offset >= mod_addr.mach.offset ) {
                /* possible */
                if( sr == SR_NONE
                  || close_addr.mach.offset <= mod_addr.mach.offset ) {
                    LclCreate( is, ptr, defn.i.name, local );
                    close_addr = mod_addr;
                    if( addr.mach.offset == mod_addr.mach.offset ) {
                        sr = SR_EXACT;
                        break;
                    }
                    sr = SR_CLOSEST;
                }
            }
        }
    }
    PopLoad( local );
    return( sr );
}


unsigned SymHdl2LclName( imp_image_handle *ii, imp_sym_handle *is,
                                char *buff, unsigned buff_size )
{
    const char  *ptr;
    unsigned    len;
    lclinfo     lclld;
    lclinfo     *local = &lclld;

    if( LoadLocalSyms( ii, is->im, &lclld ) != DS_OK )
        return( 0 );
    ptr = local->start + is->u.lcl.offset;
    len = GETU8( ptr ) - is->name_off;
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        ptr += is->name_off;
        memcpy( buff, ptr, buff_size );
        buff[buff_size] = '\0';
    }
    PopLoad( local );
    return( len );
}

static void SetBase( imp_sym_handle *is, lclinfo *local )
{
    local->base_off = is->u.lcl.base;
    if( is->u.lcl.base != NO_BASE ) {
        NewBase( local->start + is->u.lcl.base, local );
    }
}

dip_status SymHdl2LclLoc( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc, location_list *ll )
{
    lcl_defn    defn;
    dip_status  ret;
    lclinfo     lclld;
    lclinfo     *local = &lclld;

    ret = LoadLocalSyms( ii, is->im, &lclld );
    if( ret != DS_OK )
        return( ret );
    SetBase( is, local );
    ProcDefn( local->start + is->u.lcl.offset, &defn, local );
    ret = DefnLocation( &defn, lc, ll, local );
    PopLoad( local );
    return( ret );
}

dip_status SymHdl2LclType( imp_image_handle *ii, imp_sym_handle *is,
                        imp_type_handle *it )
{
    lcl_defn    defn;
    dip_status  ret;
    lclinfo     lclld;
    lclinfo     *local = &lclld;

    ret = LoadLocalSyms( ii, is->im, &lclld );
    if( ret != DS_OK )
        return( ret );
    local->base_off = 0;
    ProcDefn( local->start + is->u.lcl.offset, &defn, local );
    ret = FindTypeHandle( ii, is->im, defn.i.type_index, it );
    PopLoad( local );
    return( ret );
}

void SetGblLink( imp_sym_handle *is, gbl_info *link )
{
    is->u.lcl.gbl_link = link;
}

dip_status Lcl2GblHdl( imp_image_handle *ii,
                        imp_sym_handle *lcl_is, imp_sym_handle *gbl_is )
{
    lcl_defn            defn;
    dip_status          ret;
    location_list       ll;
    lclinfo             lclld;
    lclinfo             *local = &lclld;

    if( lcl_is->u.lcl.gbl_link != NULL ) {
        return( Link2GblHdl( ii, lcl_is->u.lcl.gbl_link, gbl_is ) );
    }
    ret = LoadLocalSyms( ii, lcl_is->im, &lclld );
    if( ret != DS_OK )
        return( ret );
    SetBase( lcl_is, local );
    ProcDefn( local->start + lcl_is->u.lcl.offset, &defn, local );
    ret = DefnLocation( &defn, NULL, &ll, local );
    if( ret != DS_OK )
        goto done;
    ret = DS_FAIL;
    if( ll.num != 1 || ll.e[0].type != LT_ADDR )
        goto done;
    gbl_is->im = lcl_is->im;
    if( LookupGblAddr( ii, ll.e[0].u.addr, gbl_is ) != SR_EXACT )
        goto done;
    ret = DS_OK;
done:
    PopLoad( local );
    return( ret );
}

dip_status SymHdl2LclInfo( imp_image_handle *ii, imp_sym_handle *is,
                        sym_info *si )
{
    lcl_defn            defn;
    const char          *p;
    dip_status          ret;
    lclinfo             lclld;
    lclinfo             *local = &lclld;
    imp_sym_handle      gbl_is;

    ret = LoadLocalSyms( ii, is->im, &lclld );
    if( ret != DS_OK )
        return( ret );
    if( Lcl2GblHdl( ii, is, &gbl_is ) == DS_OK ) {
        SymHdl2GblInfo( ii, &gbl_is, si ); /* get the global bit set */
    }
    SetBase( is, local );
    ProcDefn( local->start + is->u.lcl.offset, &defn, local );
    switch( defn.i.class & CLASS_MASK ) {
    case VAR_SYMBOL:
        si->kind = SK_DATA;
        break;
    case CODE_SYMBOL:
        si->kind = SK_PROCEDURE; /* never get handled a block */
        switch( defn.i.class & SUBCLASS_MASK ) {
        case CODE_FAR_ROUT:
        case CODE_FAR_ROUT386:
            si->rtn_far = 1;
            break;
        default:
            si->rtn_far = 0;
            break;
        }
        si->rtn_calloc = 0;
        si->ret_modifier = TM_NONE;
        si->ret_size = 0;
        switch( *defn.i.unparsed ) {
        case IND_REG+IR_RALLOC_NEAR:
            si->ret_modifier = TM_NEAR;
            break;
        case IND_REG+IR_RALLOC_FAR:
            si->ret_modifier = TM_FAR;
            break;
        case IND_REG+IR_CALLOC_NEAR:
            si->rtn_calloc = 1;
            si->ret_modifier = TM_NEAR;
            break;
        case IND_REG+IR_CALLOC_FAR:
            si->rtn_calloc = 1;
            si->ret_modifier = TM_FAR;
            break;
        }
        switch( si->ret_modifier ) {
        case TM_FAR:
            si->ret_size = sizeof( addr_seg );
            /* fall through */
        case TM_NEAR:
            si->ret_size += RegSize( defn.i.unparsed[1] );
            break;
        }
        si->ret_addr_offset = defn.r.ret_addr_offset;
        p = SkipLocation( defn.i.unparsed );
        si->num_parms = GETU8( p );
        si->prolog_size = defn.r.pro_size;
        si->epilog_size = defn.r.epi_size;
        si->rtn_size = defn.b.size;
        break;
    }
    PopLoad( local );
    return( DS_OK );
}

dip_status SymHdl2LclParmLoc( imp_image_handle *ii, imp_sym_handle *is,
                location_context *lc, location_list *ll, unsigned parm )
{
    lcl_defn    defn;
    const char  *p;
    dip_status  ret;
    lclinfo     lclld;
    lclinfo     *local = &lclld;

    ret = LoadLocalSyms( ii, is->im, &lclld );
    if( ret != DS_OK )
        return( ret );
    local->base_off = 0;
    ProcDefn( local->start + is->u.lcl.offset, &defn, local );
    if( parm == 0 ) { /* return value */
        ret = EvalLocation( ii, lc, defn.i.unparsed, ll );
        if( ret == (DS_ERR|DS_BAD_LOCATION) ) {
            ret = DS_NO_PARM;
        }
    } else {
        p = SkipLocation( defn.i.unparsed );
        if( parm > GETU8( p ) ) {
            ret = DS_NO_PARM;
        } else {
            ++p;
            for( ;; ) {
                --parm;
                if( parm == 0 )
                    break;
                p = SkipLocation( p );
            }
            ret = EvalLocation( ii, lc, p, ll );
        }
    }
    PopLoad( local );
    return( ret );
}

dip_status DIPIMPENTRY( SymObjType )( imp_image_handle *ii,
                imp_sym_handle *is, imp_type_handle *it, dip_type_info *ti )
{
    lcl_defn    defn;
    dip_status  ret;
    lclinfo     lclld;
    lclinfo     *local = &lclld;

    ret = LoadLocalSyms( ii, is->im, &lclld );
    if( ret != DS_OK )
        return( ret );
    local->base_off = 0;
    ProcDefn( local->start + is->u.lcl.offset, &defn, local );
    if( (defn.i.class & CLASS_MASK) != CODE_SYMBOL )
        return( DS_FAIL );
    if( defn.b.parent_block == 0 )
        return( DS_FAIL );
    ProcDefn( local->start + defn.b.parent_block, &defn, local );
    if( defn.i.class != (CODE_SYMBOL+CODE_MEMBER_SCOPE) )
        return( DS_FAIL );
    if( ti != NULL ) {
        if( defn.i.unparsed == NULL ) {
            ti->kind = TK_NONE;
        } else {
            ti->kind = TK_POINTER;
            switch( *defn.i.unparsed ) {
            case 0:
               ti->size = sizeof( addr32_off );
               ti->modifier = TM_NEAR;
               break;
            case 1:
               ti->size = sizeof( addr32_ptr );
               ti->modifier = TM_FAR;
               break;
            case 6:
               ti->size = sizeof( addr48_off );
               ti->modifier = TM_NEAR;
               break;
            case 7:
               ti->size = sizeof( addr48_ptr );
               ti->modifier = TM_FAR;
               break;
            }
        }
    }
    PopLoad( local );
    return( FindTypeHandle( ii, is->im, defn.i.type_index, it ) );
}

dip_status DIPIMPENTRY( SymObjLocation )( imp_image_handle *ii,
        imp_sym_handle *is, location_context *lc, location_list *ll )
{
    lcl_defn    defn;
    dip_status  ret;
    lclinfo     lclld;
    lclinfo     *local = &lclld;

    ret = LoadLocalSyms( ii, is->im, &lclld );
    if( ret != DS_OK )
        return( ret );
    local->base_off = 0;
    ProcDefn( local->start + is->u.lcl.offset, &defn, local );
    if( (defn.i.class & CLASS_MASK) != CODE_SYMBOL )
        return( DS_FAIL );
    if( defn.b.parent_block == 0 )
        return( DS_FAIL );
    ProcDefn( local->start + defn.b.parent_block, &defn, local );
    if( defn.i.class != (CODE_SYMBOL+CODE_MEMBER_SCOPE) )
        return( DS_FAIL );
    if( defn.i.unparsed == NULL )
        return( DS_FAIL );
    ret = EvalLocation( ii, lc, defn.i.unparsed + 1, ll );
    PopLoad( local );
    return( ret );
}

static const char *FindBlockScope( const char *ptr, lcl_defn *blk, address *addr, lclinfo *local )
{
    const char  *blk_ptr;

    for( ;; ) {
        ptr = FindBlockRout( ptr, local );
        if( ptr == NULL )
            return( NULL );
        blk_ptr = ptr;
        ptr = ProcDefn( ptr, blk, local );
        if( DCSameAddrSpace( blk->b.start, *addr ) == DS_OK
            && (blk->b.start.mach.offset <= addr->mach.offset)
            && (blk->b.start.mach.offset + blk->b.size > addr->mach.offset ) ) {
            return( blk_ptr );
        }
    }
}

static walk_result WalkOneBlock( imp_image_handle *ii, const char *ptr, lcl_defn *blk,
                    DIP_IMP_SYM_WALKER *wk, imp_sym_handle *is, void *d, lclinfo *local )
{
    imp_type_handle             it;
    const char                  *next;
    lcl_defn                    defn;
    walk_result                 wr;

    switch( blk->i.class ) {
    case CODE_SYMBOL | CODE_MEMBER_SCOPE:
        /* process member list */
        if( FindTypeHandle( ii, local->im, blk->i.type_index, &it ) == DS_OK ) {
            wr = WalkTypeSymList( ii, &it, wk, is, d );
            if( wr != WR_CONTINUE ) {
                return( wr );
            }
        }
        break;
    case CODE_SYMBOL | CODE_BLOCK:
    case CODE_SYMBOL | CODE_BLOCK386:
    case CODE_SYMBOL | CODE_NEAR_ROUT:
    case CODE_SYMBOL | CODE_NEAR_ROUT386:
    case CODE_SYMBOL | CODE_FAR_ROUT:
    case CODE_SYMBOL | CODE_FAR_ROUT386:
        /* process local scope */
        for( ;; ) {
            ptr = FindLclVar( ptr, local );
            if( ptr == NULL )
                break;
            next = ProcDefn( ptr, &defn, local );
            LclCreate( is, ptr, defn.i.name, local );
            wr = wk( ii, SWI_SYMBOL, is, d );
            if( wr != WR_CONTINUE )
                return( wr );
            ptr = next;
        }
        break;
    }
    return( WR_CONTINUE );
}

walk_result WalkScopedSymList( imp_image_handle *ii, address *addr, DIP_IMP_SYM_WALKER *wk,
                        imp_sym_handle *is, void *d )
{
    imp_mod_handle      im;
    const char          *ptr;
    lcl_defn            blk;
    lclinfo             lclld;
    lclinfo             *local = &lclld;
    walk_result         wr;

    wr = WR_CONTINUE;
    if( ImpInterface.AddrMod( ii, *addr, &im ) != SR_NONE ) {
        if( LoadLocalSyms( ii, im, &lclld ) == DS_OK ) {
            ptr = FindBlockScope( local->start, &blk, addr, local );
            if( ptr != NULL ) {
                ptr += GETU8( ptr );
                for( ;; ) {
                    wr = WalkOneBlock( ii, ptr, &blk, wk, is, d, local );
                    if( wr != WR_CONTINUE )
                        break;
                    if( blk.b.parent_block == 0 )
                        break;
                    ptr = local->start + blk.b.parent_block;
                    ptr = ProcDefn( ptr, &blk, local );
                }
            }
            PopLoad( local );
        }
    }
    return( wr );
}

walk_result WalkBlockSymList( imp_image_handle *ii, scope_block *scope, DIP_IMP_SYM_WALKER *wk,
                        imp_sym_handle *is, void *d )
{
    imp_mod_handle      im;
    lclinfo             lclld;
    lclinfo             *local = &lclld;
    unsigned_16         base;
    const char          *ptr;
    lcl_defn            blk;
    walk_result         wr;

    wr = WR_CONTINUE;
    if( ImpInterface.AddrMod( ii, scope->start, &im ) != SR_NONE ) {
        if( LoadLocalSyms( ii, im, &lclld ) == DS_OK ) {
            base = scope->unique >> 16;
            local->base_off = base;
            if( base != NO_BASE ) {
                NewBase( local->start + base, local );
            }
            ptr = local->start + (unsigned_16)scope->unique;
            ptr = ProcDefn( ptr, &blk, local );
            wr = WalkOneBlock( ii, ptr, &blk, wk, is, d, local );
            PopLoad( local );
        }
    }
    return( wr );
}

dip_status WalkLclModSymList( imp_image_handle *ii, imp_mod_handle im,
                        DIP_IMP_SYM_WALKER *wk, imp_sym_handle *is, void *d,
                        walk_result *last )
{
    const char          *curr;
    const char          *next;
    lcl_defn            defn;
    lclinfo             lclld;
    lclinfo             *local = &lclld;
    dip_status          ret;

    *last = WR_CONTINUE;
    ret = LoadLocalSyms( ii, im, &lclld );
    if( ret != DS_OK )
        return( ret );
    curr = local->start;
    for( ;; ) {
        curr = ModAddrLkupVar( curr, local );
        if( curr == NULL )
            break;
        next = ProcDefn( curr, &defn, local );
        LclCreate( is, curr, defn.i.name, local );
        *last = wk( ii, SWI_SYMBOL, is, d );
        if( *last != WR_CONTINUE )
            break;
        curr = next;
    }
    PopLoad( local );
    return( DS_OK );
}


search_result DIPIMPENTRY( AddrScope )( imp_image_handle *ii,
                        imp_mod_handle im, address addr, scope_block *scope )
{
    lcl_defn            blk;
    const char          *curr;
    lclinfo             lclld;
    lclinfo             *local = &lclld;

    if( LoadLocalSyms( ii, im, &lclld ) != DS_OK )
        return( SR_NONE );
    curr = FindBlockScope( local->start, &blk, &addr, local );
    if( curr == NULL ) {
        PopLoad( local );
        return( SR_NONE );
    }
    scope->start = blk.b.start;
    scope->len = blk.b.size;
    scope->unique = (curr - local->start) + ((unsigned_32)local->base_off << 16);
    PopLoad( local );
    return( SR_CLOSEST );
}


search_result DIPIMPENTRY( ScopeOuter )( imp_image_handle *ii,
                        imp_mod_handle im, scope_block *in, scope_block *out )
{
    lcl_defn            blk;
    const char          *curr;
    lclinfo             lclld;
    lclinfo             *local = &lclld;
    unsigned_16         base;

    if( LoadLocalSyms( ii, im, &lclld ) != DS_OK )
        return( SR_NONE );
    base = in->unique >> 16;
    local->base_off = base;
    if( base != NO_BASE ) {
        NewBase( local->start + base, local );
    }
    curr = local->start + (unsigned_16)in->unique;
    ProcDefn( curr, &blk, local );
    if( blk.b.parent_block == 0 ) {
        return( SR_NONE );
    }
    curr = local->start + blk.b.parent_block;
    ProcDefn( curr, &blk, local );
    out->start = blk.b.start;
    out->len = blk.b.size;
    out->unique = (curr - local->start) + ((unsigned_32)local->base_off << 16);
    PopLoad( local );
    return( SR_CLOSEST );
}
