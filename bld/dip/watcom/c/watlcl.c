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


#define NO_BASE ((unsigned_16)-1)

typedef struct lclinfo {
    section_info        *inf;
    const char          *start;
    const char          *end;
    address             code_base;
    unsigned_16         base_off;
    imp_mod_handle      imh;
} lclinfo;

const char *GetIndex( const char *ptr, unsigned *value )
{
    unsigned tmp;

    tmp = GETU8( ptr++ );
    if( tmp & 0x80 ) {
        tmp = (tmp & 0x7f) << 8;
        tmp += GETU8( ptr++ );
    }
    *value = tmp;
    return( ptr );
}

static const char *GetMachAddr( imp_image_handle *iih, const char *ptr, addr_ptr *addr, int is32 )
{
    if( is32 ) {
        *addr = *(addr48_ptr *)ptr;
        ptr += sizeof( addr48_ptr );
    } else {
        ConvAddr32ToAddr48( *(addr32_ptr *)ptr, *addr );
        ptr += sizeof( addr32_ptr );
    }
    AddressMap( iih, addr );
    return( ptr );
}


const char *GetAddress( imp_image_handle *iih, const char *ptr, address *addr, int is32 )
{
    ptr = GetMachAddr( iih, ptr, &addr->mach, is32 );
    DCAddrSection( addr );
    return( ptr );
}

static void LclCreate( imp_sym_handle *ish, const char *ptr, const char *name, lclinfo *local )
{
    ish->type = SH_LCL;
    ish->imh = local->imh;
    ish->u.lcl.base = local->base_off;
    ish->u.lcl.offset = ptr - local->start;
    ish->u.lcl.gbl_link = NULL;
    ish->name_off = (byte)( name - ptr );
}

/*
 * LoadLocalSyms -- load the local symbol information for the module
 */

static dip_status LoadLocalSyms( imp_image_handle *iih, imp_mod_handle imh, lclinfo *new )
{
    dip_status  ds;

    ds = DS_FAIL;
    if( imh != IMH_NOMOD ) {
        new->start = InfoLoad( iih, imh, DMND_LOCALS, 0, NULL );
        if( new->start != NULL ) {
            new->end = new->start + InfoSize( iih, imh, DMND_LOCALS, 0 );
            new->imh = imh;
            new->inf = FindInfo( iih, imh );
            new->base_off = NO_BASE;
            ds = DS_OK;
        }
    }
    return( ds );
}

static void PopLocal( lclinfo *local )
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
static const char *ProcBlock( imp_image_handle *iih, const char *ptr, lcl_defn *defn, lclinfo *local )
{
    if( local->base_off == NO_BASE ) {
        defn->b.start = FindModBase( iih, local->imh );
    } else {
        defn->b.start = local->code_base;
    }
    if( defn->i.class >= (CODE_SYMBOL | CODE_BLOCK386) ) {
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
static const char *ProcDefn( imp_image_handle *iih, const char *ptr, lcl_defn *defn, lclinfo *local )
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
        ptr = ProcBlock( iih, ptr, defn, local );
        break;
    case CODE_SYMBOL | CODE_NEAR_ROUT386:
    case CODE_SYMBOL | CODE_FAR_ROUT386:
    case CODE_SYMBOL | CODE_NEAR_ROUT :
    case CODE_SYMBOL | CODE_FAR_ROUT :
        ptr = ProcBlock( iih, ptr, defn, local );
        defn->r.pro_size = GETU8( ptr++ );
        defn->r.epi_size = GETU8( ptr++ );
        if( defn->i.class >= (CODE_SYMBOL | CODE_BLOCK386) ) {
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

static void NewBase( imp_image_handle *iih, const char *ptr, lclinfo *local )
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
            GetMachAddr( iih, ptr + 1, &local->code_base.mach, 1 );
            break;
        case SET_BASE:
            GetMachAddr( iih, ptr + 1, &local->code_base.mach, 0 );
            break;
        }
    }
}

static const char *FindBlockRout( imp_image_handle *iih, const char *ptr, lclinfo *local )
{
    byte        cls;

    while( ptr < local->end ) {
        cls = GETU8( ptr + 1 );
        if( (cls & CLASS_MASK) == CODE_SYMBOL && cls != (CODE_SYMBOL | CODE_MEMBER_SCOPE) )  {
            return( ptr );
        }
        NewBase( iih, ptr, local );
        ptr += GETU8( ptr );
    }
    return( NULL );
}

static const char *ModAddrLkupVar( imp_image_handle *iih, const char *ptr, lclinfo *local )
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
        NewBase( iih, ptr, local );
        ptr += GETU8( ptr );
    }
    return( NULL );
}

static const char *FindLclVar( imp_image_handle *iih, const char *ptr, lclinfo *local )
{
    while( ptr < local->end ) {
        if( (GETU8( ptr + 1 ) & CLASS_MASK) == CODE_SYMBOL )
            break;
        if( GETU8( ptr + 1 ) == (VAR_SYMBOL | VAR_LOCAL) )
            return( ptr );
        NewBase( iih, ptr, local );
        ptr += GETU8( ptr );
    }
    return( NULL );
}


static dip_status DefnLocation( imp_image_handle *iih, lcl_defn *defn, location_context *lc,
                                location_list *ll, lclinfo *local )
{
    address     addr;

    /* unused parameters */ (void)local;

    switch( defn->i.class ) {
    case VAR_SYMBOL | VAR_MODULE386:
        GetAddress( iih, defn->i.unparsed, &addr, 1 );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case VAR_SYMBOL | VAR_MODULE:
        GetAddress( iih, defn->i.unparsed, &addr, 0 );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case VAR_SYMBOL | VAR_LOCAL:
    case VAR_SYMBOL | VAR_MODULE_LOC:
        return( EvalLocation( iih, lc, defn->i.unparsed, ll ) );
    default:
        LocationCreate( ll, LT_ADDR, &defn->b.start );
        break;
    }
    return( DS_OK );
}


static address DefnAddr( imp_image_handle *iih, lcl_defn *defn, lclinfo *local )
{
    location_list       ll;

    if( DefnLocation( iih, defn, NULL, &ll, local ) == DS_OK
      && ll.num == 1 && ll.e[0].type == LT_ADDR ) {
        return( ll.e[0].u.addr );
    } else {
        return( NilAddr );
    }
}


search_result SearchLclMod( imp_image_handle *iih, imp_mod_handle imh, lookup_item *li, void *d )
{
    lcl_defn            defn;
    strcompn_fn         *scompn;
    const char          *curr;
    const char          *next;
    const char          *name;
    size_t              len;
    imp_sym_handle      *ish;
    search_result       sr;
    lclinfo             lclld;
    lclinfo             *local;


    sr = SR_NONE;
    if( li->scope.start == NULL ) {
        if( LoadLocalSyms( iih, imh, &lclld ) == DS_OK ) {
            local = &lclld;
            scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
            name = li->name.start;
            len = li->name.len;
            for( curr = local->start; (curr = ModAddrLkupVar( iih, curr, local )) != NULL; curr = next ) {
                next = ProcDefn( iih, curr, &defn, local );
                if( len == defn.i.namelen && scompn( name, defn.i.name, len ) == 0 ) {
                    ish = DCSymCreate( iih, d );
                    LclCreate( ish, curr, defn.i.name, local );
                    sr = SR_EXACT;
                }
            }
            PopLocal( local );
        }
    }
    return( sr );
}


static search_result DoLclScope( imp_image_handle *iih, imp_mod_handle imh,
                    address addr, lookup_item *li, void *d, lclinfo *local )
{
    lcl_defn            blk;
    lcl_defn            defn;
    const char          *curr;
    const char          *next;
    strcompn_fn         *scompn;
    const char          *name;
    size_t              len;
    search_result       sr;
    lookup_item         type_li;
    imp_type_handle     ith;
    imp_sym_handle      *ish;

    if( li->scope.start != NULL ) {
        type_li.name = li->scope;
        type_li.file_scope = li->file_scope;
        type_li.case_sensitive = li->case_sensitive;
        type_li.scope.start = NULL;
        type_li.type = ST_TYPE;
        sr = LookupTypeName( iih, imh, &type_li, &ith );
        if( sr == SR_NONE )
            return( sr );
        return( SearchMbr( iih, &ith, li, d ) );
    }
    scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
    name = li->name.start;
    len = li->name.len;
    sr = SR_NONE;
    for( curr = local->start; (curr = FindBlockRout( iih, curr, local )) != NULL; curr = next ) {
        next = ProcDefn( iih, curr, &blk, local );
        if( DCSameAddrSpace( blk.b.start, addr ) == DS_OK
          && ( blk.b.start.mach.offset <= addr.mach.offset )
          && ( blk.b.start.mach.offset + blk.b.size > addr.mach.offset ) ) {
            for( ;; ) {
                if( blk.i.class == (CODE_SYMBOL | CODE_MEMBER_SCOPE) ) {
                    if( FindTypeHandle( iih, imh, blk.i.type_index, &ith ) == DS_OK ) {
                        sr = SearchMbr( iih, &ith, li, d );
                    }
                } else {
                    for( ; (curr = FindLclVar( iih, curr, local )) != NULL; curr = next ) {
                        next = ProcDefn( iih, curr, &defn, local );
                        if( len == defn.i.namelen && scompn( name, defn.i.name, len ) == 0 ) {
                            ish = DCSymCreate( iih, d );
                            LclCreate( ish, curr, defn.i.name, local );
                            sr = SR_EXACT;
                        }
                    }
                }
                if( sr != SR_NONE || blk.b.parent_block == 0 )
                    break;
                curr = ProcDefn( iih, local->start + blk.b.parent_block, &blk, local );
            }
            break;
        }
    }
    return( sr );
}


search_result SearchLclScope( imp_image_handle *iih, imp_mod_handle imh,
                                address *addr, lookup_item *li, void *d )
{
    search_result       sr;
    lclinfo             lclld;
    lclinfo             *local;

    sr = SR_NONE;
    if( LoadLocalSyms( iih, imh, &lclld ) == DS_OK ) {
        local = &lclld;
        sr = DoLclScope( iih, imh, *addr, li, d, local );
        PopLocal( local );
    }
    return( sr );
}


search_result LookupLclAddr( imp_image_handle *iih, address addr, imp_sym_handle *ish )
{
    lcl_defn            defn;
    address             mod_addr;
    address             close_addr;
    const char          *curr;
    const char          *next;
    search_result       sr;
    lclinfo             lclld;
    lclinfo             *local;

    sr = SR_NONE;
    if( LoadLocalSyms( iih, ish->imh, &lclld ) == DS_OK ) {
        local = &lclld;
        close_addr.mach.offset = 0;
        for( curr = local->start; (curr = ModAddrLkupVar( iih, curr, local )) != NULL; curr = next ) {
            next = ProcDefn( iih, curr, &defn, local );
            mod_addr = DefnAddr( iih, &defn, local );
            if( DCSameAddrSpace( addr, mod_addr ) == DS_OK ) {
                if( addr.mach.offset >= mod_addr.mach.offset ) {
                    /* possible */
                    if( sr == SR_NONE || close_addr.mach.offset <= mod_addr.mach.offset ) {
                        LclCreate( ish, curr, defn.i.name, local );
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
        PopLocal( local );
    }
    return( sr );
}


unsigned SymHdl2LclName( imp_image_handle *iih, imp_sym_handle *ish,
                                char *buff, unsigned buff_size )
{
    const char  *ptr;
    unsigned    len;
    lclinfo     lclld;
    lclinfo     *local;

    len = 0;
    if( LoadLocalSyms( iih, ish->imh, &lclld ) == DS_OK ) {
        local = &lclld;
        ptr = local->start + ish->u.lcl.offset;
        len = GETU8( ptr ) - ish->name_off;
        if( buff_size > 0 ) {
            --buff_size;
            if( buff_size > len )
                buff_size = len;
            ptr += ish->name_off;
            memcpy( buff, ptr, buff_size );
            buff[buff_size] = '\0';
        }
        PopLocal( local );
    }
    return( len );
}

static void SetBase( imp_image_handle *iih, imp_sym_handle *ish, lclinfo *local )
{
    local->base_off = ish->u.lcl.base;
    if( ish->u.lcl.base != NO_BASE ) {
        NewBase( iih, local->start + ish->u.lcl.base, local );
    }
}

dip_status SymHdl2LclLoc( imp_image_handle *iih, imp_sym_handle *ish,
                        location_context *lc, location_list *ll )
{
    lcl_defn    defn;
    dip_status  ds;
    lclinfo     lclld;
    lclinfo     *local;

    ds = LoadLocalSyms( iih, ish->imh, &lclld );
    if( ds == DS_OK ) {
        local = &lclld;
        SetBase( iih, ish, local );
        ProcDefn( iih, local->start + ish->u.lcl.offset, &defn, local );
        ds = DefnLocation( iih, &defn, lc, ll, local );
        PopLocal( local );
    }
    return( ds );
}

dip_status SymHdl2LclType( imp_image_handle *iih, imp_sym_handle *ish, imp_type_handle *ith )
{
    lcl_defn    defn;
    dip_status  ds;
    lclinfo     lclld;
    lclinfo     *local;

    ds = LoadLocalSyms( iih, ish->imh, &lclld );
    if( ds == DS_OK ) {
        local = &lclld;
        local->base_off = 0;
        ProcDefn( iih, local->start + ish->u.lcl.offset, &defn, local );
        ds = FindTypeHandle( iih, ish->imh, defn.i.type_index, ith );
        PopLocal( local );
    }
    return( ds );
}

void SetGblLink( imp_sym_handle *ish, gbl_info *link )
{
    ish->u.lcl.gbl_link = link;
}

dip_status Lcl2GblHdl( imp_image_handle *iih,
                        imp_sym_handle *lcl_ish, imp_sym_handle *gbl_ish )
{
    lcl_defn            defn;
    dip_status          ds;
    location_list       ll;
    lclinfo             lclld;
    lclinfo             *local;

    if( lcl_ish->u.lcl.gbl_link != NULL ) {
        return( Link2GblHdl( iih, lcl_ish->u.lcl.gbl_link, gbl_ish ) );
    }
    ds = LoadLocalSyms( iih, lcl_ish->imh, &lclld );
    if( ds == DS_OK ) {
        local = &lclld;
        SetBase( iih, lcl_ish, local );
        ProcDefn( iih, local->start + lcl_ish->u.lcl.offset, &defn, local );
        ds = DefnLocation( iih, &defn, NULL, &ll, local );
        if( ds == DS_OK ) {
            ds = DS_FAIL;
            if( ll.num == 1 && ll.e[0].type == LT_ADDR ) {
                gbl_ish->imh = lcl_ish->imh;
                if( LookupGblAddr( iih, ll.e[0].u.addr, gbl_ish ) == SR_EXACT ) {
                    ds = DS_OK;
                }
            }
        }
        PopLocal( local );
    }
    return( ds );
}

dip_status SymHdl2LclInfo( imp_image_handle *iih, imp_sym_handle *ish, sym_info *si )
{
    lcl_defn            defn;
    const char          *p;
    dip_status          ds;
    lclinfo             lclld;
    lclinfo             *local;
    imp_sym_handle      gbl_ish;

    ds = LoadLocalSyms( iih, ish->imh, &lclld );
    if( ds == DS_OK ) {
        local = &lclld;
        if( Lcl2GblHdl( iih, ish, &gbl_ish ) == DS_OK ) {
            SymHdl2GblInfo( iih, &gbl_ish, si ); /* get the global bit set */
        }
        SetBase( iih, ish, local );
        ProcDefn( iih, local->start + ish->u.lcl.offset, &defn, local );
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
            case IND_REG | IR_RALLOC_NEAR:
                si->ret_modifier = TM_NEAR;
                break;
            case IND_REG | IR_RALLOC_FAR:
                si->ret_modifier = TM_FAR;
                break;
            case IND_REG | IR_CALLOC_NEAR:
                si->rtn_calloc = 1;
                si->ret_modifier = TM_NEAR;
                break;
            case IND_REG | IR_CALLOC_FAR:
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
        PopLocal( local );
    }
    return( ds );
}

dip_status SymHdl2LclParmLoc( imp_image_handle *iih, imp_sym_handle *ish,
                location_context *lc, location_list *ll, unsigned parm )
{
    lcl_defn    defn;
    const char  *p;
    dip_status  ds;
    lclinfo     lclld;
    lclinfo     *local;

    ds = LoadLocalSyms( iih, ish->imh, &lclld );
    if( ds == DS_OK ) {
        local = &lclld;
        local->base_off = 0;
        ProcDefn( iih, local->start + ish->u.lcl.offset, &defn, local );
        if( parm == 0 ) { /* return value */
            ds = EvalLocation( iih, lc, defn.i.unparsed, ll );
            if( ds == (DS_ERR | DS_BAD_LOCATION) ) {
                ds = DS_NO_PARM;
            }
        } else {
            p = SkipLocation( defn.i.unparsed );
            if( GETU8( p ) < parm ) {
                ds = DS_NO_PARM;
            } else {
                ++p;
                while( parm-- > 0 ) {
                    p = SkipLocation( p );
                }
                ds = EvalLocation( iih, lc, p, ll );
            }
        }
        PopLocal( local );
    }
    return( ds );
}

dip_status DIPIMPENTRY( SymObjType )( imp_image_handle *iih,
                imp_sym_handle *ish, imp_type_handle *ith, dig_type_info *ti )
{
    lcl_defn    defn;
    dip_status  ds;
    lclinfo     lclld;
    lclinfo     *local;

    ds = LoadLocalSyms( iih, ish->imh, &lclld );
    if( ds == DS_OK ) {
        local = &lclld;
        local->base_off = 0;
        ProcDefn( iih, local->start + ish->u.lcl.offset, &defn, local );
        ds = DS_FAIL;
        if( (defn.i.class & CLASS_MASK) == CODE_SYMBOL && defn.b.parent_block != 0 ) {
            ProcDefn( iih, local->start + defn.b.parent_block, &defn, local );
            if( defn.i.class == (CODE_SYMBOL | CODE_MEMBER_SCOPE) ) {
                ds = DS_OK;
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
            }
        }
        PopLocal( local );
    }
    if( ds == DS_OK )
        return( FindTypeHandle( iih, ish->imh, defn.i.type_index, ith ) );
    return( ds );
}

dip_status DIPIMPENTRY( SymObjLocation )( imp_image_handle *iih,
        imp_sym_handle *ish, location_context *lc, location_list *ll )
{
    lcl_defn    defn;
    dip_status  ds;
    lclinfo     lclld;
    lclinfo     *local;

    ds = LoadLocalSyms( iih, ish->imh, &lclld );
    if( ds == DS_OK ) {
        local = &lclld;
        local->base_off = 0;
        ProcDefn( iih, local->start + ish->u.lcl.offset, &defn, local );
        ds = DS_FAIL;
        if( (defn.i.class & CLASS_MASK) == CODE_SYMBOL && defn.b.parent_block != 0 ) {
            ProcDefn( iih, local->start + defn.b.parent_block, &defn, local );
            if( defn.i.class == (CODE_SYMBOL | CODE_MEMBER_SCOPE) && defn.i.unparsed != NULL ) {
                ds = EvalLocation( iih, lc, defn.i.unparsed + 1, ll );
            }
        }
        PopLocal( local );
    }
    return( ds );
}

static const char *FindBlockScope( imp_image_handle *iih, const char *curr, lcl_defn *blk, address *addr, lclinfo *local )
{
    const char  *next;

    for( ; (curr = FindBlockRout( iih, curr, local )) != NULL; curr = next ) {
        next = ProcDefn( iih, curr, blk, local );
        if( DCSameAddrSpace( blk->b.start, *addr ) == DS_OK
          && (blk->b.start.mach.offset <= addr->mach.offset)
          && (blk->b.start.mach.offset + blk->b.size > addr->mach.offset ) ) {
            break;
        }
    }
    return( curr );
}

static walk_result WalkOneBlock( imp_image_handle *iih, const char *ptr, lcl_defn *blk,
                    DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish, void *d, lclinfo *local )
{
    imp_type_handle             ith;
    const char                  *next;
    lcl_defn                    defn;
    walk_result                 wr;

    wr = WR_CONTINUE;
    switch( blk->i.class ) {
    case CODE_SYMBOL | CODE_MEMBER_SCOPE:
        /* process member list */
        if( FindTypeHandle( iih, local->imh, blk->i.type_index, &ith ) == DS_OK ) {
            wr = WalkTypeSymList( iih, &ith, wk, ish, d );
        }
        break;
    case CODE_SYMBOL | CODE_BLOCK:
    case CODE_SYMBOL | CODE_BLOCK386:
    case CODE_SYMBOL | CODE_NEAR_ROUT:
    case CODE_SYMBOL | CODE_NEAR_ROUT386:
    case CODE_SYMBOL | CODE_FAR_ROUT:
    case CODE_SYMBOL | CODE_FAR_ROUT386:
        /* process local scope */
        for( ; (ptr = FindLclVar( iih, ptr, local )) != NULL; ptr = next ) {
            next = ProcDefn( iih, ptr, &defn, local );
            LclCreate( ish, ptr, defn.i.name, local );
            wr = wk( iih, SWI_SYMBOL, ish, d );
            if( wr != WR_CONTINUE ) {
                break;
            }
        }
        break;
    }
    return( wr );
}

walk_result WalkScopedSymList( imp_image_handle *iih, address *addr, DIP_IMP_SYM_WALKER *wk,
                        imp_sym_handle *ish, void *d )
{
    imp_mod_handle      imh;
    const char          *ptr;
    lcl_defn            blk;
    lclinfo             lclld;
    lclinfo             *local;
    walk_result         wr;

    wr = WR_CONTINUE;
    if( ImpInterface.AddrMod( iih, *addr, &imh ) != SR_NONE ) {
        if( LoadLocalSyms( iih, imh, &lclld ) == DS_OK ) {
            local = &lclld;
            ptr = FindBlockScope( iih, local->start, &blk, addr, local );
            if( ptr != NULL ) {
                ptr += GETU8( ptr );
                for( ; (wr = WalkOneBlock( iih, ptr, &blk, wk, ish, d, local )) == WR_CONTINUE; ) {
                    if( blk.b.parent_block == 0 )
                        break;
                    ptr = local->start + blk.b.parent_block;
                    ptr = ProcDefn( iih, ptr, &blk, local );
                }
            }
            PopLocal( local );
        }
    }
    return( wr );
}

walk_result WalkBlockSymList( imp_image_handle *iih, scope_block *scope, DIP_IMP_SYM_WALKER *wk,
                        imp_sym_handle *ish, void *d )
{
    imp_mod_handle      imh;
    lclinfo             lclld;
    lclinfo             *local;
    unsigned_16         base;
    const char          *ptr;
    lcl_defn            blk;
    walk_result         wr;

    wr = WR_CONTINUE;
    if( ImpInterface.AddrMod( iih, scope->start, &imh ) != SR_NONE ) {
        if( LoadLocalSyms( iih, imh, &lclld ) == DS_OK ) {
            local = &lclld;
            base = scope->unique >> 16;
            local->base_off = base;
            if( base != NO_BASE ) {
                NewBase( iih, local->start + base, local );
            }
            ptr = local->start + (unsigned_16)scope->unique;
            ptr = ProcDefn( iih, ptr, &blk, local );
            wr = WalkOneBlock( iih, ptr, &blk, wk, ish, d, local );
            PopLocal( local );
        }
    }
    return( wr );
}

dip_status WalkLclModSymList( imp_image_handle *iih, imp_mod_handle imh,
                        DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish, void *d,
                        walk_result *last )
{
    const char          *curr;
    const char          *next;
    lcl_defn            defn;
    lclinfo             lclld;
    lclinfo             *local;
    dip_status          ds;
    walk_result         wr;

    wr = WR_CONTINUE;
    ds = LoadLocalSyms( iih, imh, &lclld );
    if( ds == DS_OK ) {
        local = &lclld;
        for( curr = local->start; (curr = ModAddrLkupVar( iih, curr, local )) != NULL; curr = next ) {
            next = ProcDefn( iih, curr, &defn, local );
            LclCreate( ish, curr, defn.i.name, local );
            wr = wk( iih, SWI_SYMBOL, ish, d );
            if( wr != WR_CONTINUE ) {
                break;
            }
        }
        PopLocal( local );
    }
    *last = wr;
    return( ds );
}


search_result DIPIMPENTRY( AddrScope )( imp_image_handle *iih,
                        imp_mod_handle imh, address addr, scope_block *scope )
{
    lcl_defn            blk;
    const char          *curr;
    lclinfo             lclld;
    lclinfo             *local;
    search_result       sr;

    sr = SR_NONE;
    if( LoadLocalSyms( iih, imh, &lclld ) == DS_OK ) {
        local = &lclld;
        curr = FindBlockScope( iih, local->start, &blk, &addr, local );
        if( curr != NULL ) {
            scope->start = blk.b.start;
            scope->len = blk.b.size;
            scope->unique = ( curr - local->start ) + ((unsigned_32)local->base_off << 16);
            sr = SR_CLOSEST;
        }
        PopLocal( local );
    }
    return( sr );
}


search_result DIPIMPENTRY( ScopeOuter )( imp_image_handle *iih,
                        imp_mod_handle imh, scope_block *in, scope_block *out )
{
    lcl_defn            blk;
    const char          *curr;
    lclinfo             lclld;
    lclinfo             *local;
    unsigned_16         base;
    search_result       sr;

    sr = SR_NONE;
    if( LoadLocalSyms( iih, imh, &lclld ) == DS_OK ) {
        local = &lclld;
        base = in->unique >> 16;
        local->base_off = base;
        if( base != NO_BASE ) {
            NewBase( iih, local->start + base, local );
        }
        curr = local->start + (unsigned_16)in->unique;
        ProcDefn( iih, curr, &blk, local );
        if( blk.b.parent_block != 0 ) {
            curr = local->start + blk.b.parent_block;
            ProcDefn( iih, curr, &blk, local );
            out->start = blk.b.start;
            out->len = blk.b.size;
            out->unique = ( curr - local->start ) + ((unsigned_32)local->base_off << 16);
            sr = SR_CLOSEST;
        }
        PopLocal( local );
    }
    return( sr );
}
