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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stddef.h>
#include "dbgdefn.h"
#undef DIPAddrMod
#undef DIPAddrSym
#undef DIPAddrScope
#undef DIPAddrCue
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "dipimp.h"
#include "dipcli.h"
#include "mad.h"
#include "strutil.h"
#include "dbgloc.h"
#include "dbgovl.h"
#include "dbg_dbg.h"
#include "dbgprog.h"
#include "dipinter.h"
#include "addarith.h"
#include "dbgerr.h"

#include "clibext.h"

#define WVImp(n)            WVImp ## n
#define WVIMPENTRY(n)       DIGREGISTER WVImp( n )

/*
 * Client support routines
 */

void DIPCLIENTRY( ImageUnload )( mod_handle mh )
{
    image_entry         *image;

    image = ImageEntry( mh );
    if( image != NULL && !image->nofree ) {
        FreeImage( image );
    }
}

void DIPCLIENTRY( MapAddr )( addr_ptr *addr, void *d )
{
    image_entry         *id = d;

    MapAddrForImage( id, addr );
}

static imp_sym_handle *DoSymCreate( imp_image_handle *iih, sym_list **sl_head )
{
    sym_list        *new_sl;

    _ChkAlloc( new_sl, sizeof( sym_list ) - sizeof( byte ) + sym_SIZE, LIT_ENG( ERR_NO_MEMORY_FOR_DEBUG ) );
    DIPSymInit( SL2SH( new_sl ), IIH2IH( iih ) );
    new_sl->next = *sl_head;
    *sl_head = new_sl;
    return( SL2ISH( new_sl ) );
}

imp_sym_handle *DIPCLIENTRY( SymCreate )( imp_image_handle *iih, void *d )
{
    return( DoSymCreate( iih, (sym_list **)d ) );
}

void MadTypeToDipTypeInfo( mad_type_handle mth, dig_type_info *ti )
{
    mad_type_info       mti;

    MADTypeInfo( mth, &mti );
    ti->size = BITS2BYTES( mti.b.bits );
    ti->modifier = TM_NONE;
    ti->deref = false;
    switch( mti.b.kind ) {
    case MTK_INTEGER:
        if( mti.i.nr == MNR_UNSIGNED ) {
            ti->modifier = TM_UNSIGNED;
        } else {
            ti->modifier = TM_SIGNED;
        }
        ti->kind = TK_INTEGER;
        break;
    case MTK_ADDRESS:
        if( mti.a.seg.bits != 0 ) {
            ti->modifier = TM_FAR;
        } else {
            ti->modifier = TM_NEAR;
        }
        ti->kind = TK_ADDRESS;
        break;
    case MTK_FLOAT:
        ti->kind = TK_REAL;
        break;
    }
}

dip_status RegLocation( machine_state *regs, const mad_reg_info *ri, location_list *ll )
{

    if( regs == NULL || ri == NULL )
        return( DS_ERR | DS_CONTEXT_ITEM_INVALID );
    LocationCreate( ll, LT_INTERNAL, &regs->mr );
    LocationSet( ll, ri->bit_start, ri->bit_size );
    ll->flags = ri->flags << LLF_REG_FLAGS_SHIFT;
    return( DS_OK );
}

dip_status DIPCLIENTRY( ItemLocation )( location_context *lc, context_item ci,
                         location_list *ll )
{
    sym_info            info;
    DIPHDL( sym, sh );

    if( lc == NULL )
        return( DS_ERR | DS_NO_CONTEXT );
    switch( ci ) {
    case CI_FRAME:
        if( lc->maybe_have_frame ) {
            lc->maybe_have_frame = false;
            if( DeAliasAddrSym( NO_MOD, lc->execution, sh ) == SR_NONE ) {
                /* nothing to do */
            } else if( DIPSymInfo( sh, NULL, &info ) != DS_OK ) {
                /* nothing to do */
            } else if( info.kind != SK_PROCEDURE ) {
                /* nothing to do */
            } else if( DIPSymLocation( sh, NULL, ll ) != DS_OK ) {
                /* nothing to do */
            } else if( lc->execution.mach.offset
                 < ll->e[0].u.addr.mach.offset + info.prolog_size ) {
                /* nothing to do */
            } else if( lc->execution.mach.offset
                 <= ll->e[0].u.addr.mach.offset+info.rtn_size-info.epilog_size ) {
                lc->have_frame = true;
            }
        }
        if( Context.maybe_have_frame
          && AddrComp( Context.frame, lc->frame ) == 0
          && AddrComp( Context.execution, lc->execution ) == 0 ) {
            /* cache result in global context item */
            Context.have_frame = lc->have_frame;
            Context.maybe_have_frame = false;
        }
        if( !lc->have_frame )
            return( DS_ERR|DS_CONTEXT_ITEM_INVALID );
        LocationCreate( ll, LT_ADDR, &lc->frame );
        return( DS_OK );
    case CI_STACK:
        if( !lc->have_stack )
            return( DS_ERR|DS_CONTEXT_ITEM_INVALID );
        LocationCreate( ll, LT_ADDR, &lc->stack );
        return( DS_OK );
    case CI_EXECUTION:
        LocationCreate( ll, LT_ADDR, &lc->execution );
        return( DS_OK );
    case CI_OBJECT:
        if( lc->maybe_have_object ) {
            lc->maybe_have_object = false;
            if( DeAliasAddrSym( NO_MOD, lc->execution, sh ) == SR_NONE ) {
                /* nothing to do */
            } else if( DIPSymInfo( sh, NULL, &info ) != DS_OK ) {
                /* nothing to do */
            } else if( info.kind != SK_PROCEDURE ) {
                /* nothing to do */
            } else if( DIPSymObjLocation( sh, lc, &lc->object ) == DS_OK ) {
                lc->have_object = true;
            }
        }
        if( !lc->have_object )
            return( DS_ERR|DS_CONTEXT_ITEM_INVALID );
        *ll = lc->object;
        return( DS_OK );
    case CI_DEF_ADDR_SPACE:
        {
            address     addr;

            addr = DefAddrSpaceForAddr( lc->execution );
            LocationCreate( ll, LT_ADDR, &addr );
        }
        return( DS_OK );
    }
    return( RegLocation( lc->regs, MADRegFromContextItem( ci ), ll ) );
}

dip_status DIPCLIENTRY( AssignLocation )( location_list *dst,
                        const location_list *src, unsigned long size )
{
    return( LocationAssign( dst, src, size, false ) );
}

dip_status DIPCLIENTRY( SameAddrSpace )( address a, address b )
{
    return( SameAddrSpace( a, b ) ? DS_OK : DS_FAIL );
}

void DIPCLIENTRY( AddrSection )( address *addr )
{
    AddrSection( addr, OVL_MAP_CURR );
}


void DIPCLIENTRY( Status )( dip_status ds )
{
    DIPStatus = ds;
}

dig_arch DIPCLIENTRY( CurrArch )( void )
{
    return( SysConfig.arch );
}

/*
 * Dealiasing cover routines
 */

search_result DeAliasAddrMod( address a, mod_handle *mh )
{
    DeAlias( &a.mach );
    return( DIPAddrMod( a, mh ) );
}

search_result DeAliasAddrSym( mod_handle mh, address a, sym_handle *sh )
{
    DeAlias( &a.mach );
    return( DIPAddrSym( mh, a, sh ) );
}

search_result DeAliasAddrScope( mod_handle mh, address a, scope_block *sb )
{
    DeAlias( &a.mach );
    return( DIPAddrScope( mh, a, sb ) );
}

search_result DeAliasAddrCue( mod_handle mh, address a, cue_handle *cueh )
{
    DeAlias( &a.mach );
    return( DIPAddrCue( mh, a, cueh ) );
}

/*
 * Internal symbol table interface routines
 */

static char     WVImp( Name )[] = "Debugger Internal";

static unsigned WVIMPENTRY( HandleSize )( handle_kind hk )
{
    static const unsigned_8 Sizes[] = {
        #define pick(e,hdl,imphdl,wvimphdl) wvimphdl,
        #include "diphndls.h"
        #undef pick
    };
    return( Sizes[hk] );
}

static dip_status WVIMPENTRY( MoreMem )( size_t amount )
{
    /* unused parameters */ (void)amount;

    return( DS_FAIL );
}

#if 0
static dip_status WVImp( Startup )( void )
{
    return( DS_OK );
}
#endif

static void WVIMPENTRY( Shutdown )( void )
{
}

static void WVIMPENTRY( Cancel )( void )
{
}

static dip_status WVIMPENTRY( LoadInfo )( FILE *fp, imp_image_handle *iih )
{
    /* unused parameters */ (void)fp; (void)iih;

    return( DS_FAIL );
}

static void WVIMPENTRY( MapInfo )( imp_image_handle *iih, void *d )
{
    /* unused parameters */ (void)iih; (void)d;
}

static void WVIMPENTRY( UnloadInfo )( imp_image_handle *iih )
{
    /* unused parameters */ (void)iih;
}

static walk_result WVIMPENTRY( WalkModList )( imp_image_handle *iih, DIP_IMP_MOD_WALKER *wk, void *d )
{
    return( wk( iih, IMH_WV, d ) );
}

static const char InternalName[] = "_dbg";

static size_t WVIMPENTRY( ModName )( imp_image_handle *iih, imp_mod_handle imh, char *name, size_t max )
{
    size_t  len;

    /* unused parameters */ (void)iih; (void)imh;

    len = sizeof( InternalName ) - 1;
    if( max > 0 ) {
        --max;
        if( max > len )
            max = len;
        memcpy( name, InternalName, max );
        name[max] = NULLCHAR;
    }
    return( len );
}


static char *WVIMPENTRY( ModSrcLang )( imp_image_handle *iih, imp_mod_handle imh )
{
    /* unused parameters */ (void)iih; (void)imh;

    return( LIT_ENG( Empty ) );
}

static dip_status WVIMPENTRY( ModInfo )( imp_image_handle *iih, imp_mod_handle imh, handle_kind hk )
{
    static const dip_status Kinds[] = {
        DS_FAIL,
        DS_OK,
        DS_FAIL,
        DS_OK
    };

    /* unused parameters */ (void)iih; (void)imh;

    return( Kinds[hk] );
}

static dip_status WVIMPENTRY( ModDefault )( imp_image_handle *iih, imp_mod_handle imh,
                        default_kind dk, dig_type_info *ti )
{
    /* unused parameters */ (void)iih; (void)imh; (void)dk; (void)ti;

    /* never called */
    return( DS_FAIL );
}

static search_result WVIMPENTRY( AddrMod )( imp_image_handle *iih, address a, imp_mod_handle *imh )
{
    /* unused parameters */ (void)iih; (void)a; (void)imh;

    return( SR_NONE );
}

static address WVIMPENTRY( ModAddr )( imp_image_handle *iih, imp_mod_handle imh )
{
    /* unused parameters */ (void)iih; (void)imh;

    return( NilAddr );
}


static walk_result WVIMPENTRY( WalkTypeList )( imp_image_handle *iih, imp_mod_handle imh,
                        DIP_IMP_TYPE_WALKER *wk, imp_type_handle *ith, void *d )
{
    /* unused parameters */ (void)iih; (void)imh; (void)wk; (void)ith; (void)d;

    return( WR_CONTINUE );
}

static imp_mod_handle WVIMPENTRY( TypeMod )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( IMH_WV );
}

static dip_status WVIMPENTRY( TypeInfo )( imp_image_handle *iih, imp_type_handle *ith,
                        location_context *lc, dig_type_info *ti )
{
    /* unused parameters */ (void)iih; (void)lc;

    if( ith->ri != NULL ) {
        MadTypeToDipTypeInfo( ith->ri->mth, ti );
    } else {
        *ti = ith->ti;
    }
    return( DS_OK );
}

static dip_status WVIMPENTRY( TypeBase )( imp_image_handle *iih, imp_type_handle *src_ith, imp_type_handle *dst_ith )
{
    /* unused parameters */ (void)iih;

    *dst_ith = *src_ith;
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypeBaseLocation )( imp_image_handle *iih, imp_type_handle *src_ith,
                    imp_type_handle *dst_ith, location_context *lc, location_list *ll )
{
    /* unused parameters */ (void)iih; (void)lc; (void)ll;

    *dst_ith = *src_ith;
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypeArrayInfo )( imp_image_handle *iih, imp_type_handle *ith,
                location_context *lc, array_info *ai, imp_type_handle *index_ith )
{
    /* unused parameters */ (void)iih; (void)ith; (void)lc; (void)ai; (void)index_ith;

    /* will never get called */
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypeProcInfo )( imp_image_handle *iih, imp_type_handle *ith,
                imp_type_handle *parm_ith, unsigned parm )
{
    /* unused parameters */ (void)iih; (void)ith; (void)parm_ith; (void)parm;

    /* will never get called */
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypePtrAddrSpace )( imp_image_handle *iih, imp_type_handle *ith,
                        location_context *lc, address *addr )
{
    /* unused parameters */ (void)iih; (void)ith; (void)lc; (void)addr;

    /* will never get called */
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypeThunkAdjust )( imp_image_handle *iih, imp_type_handle *obj_ith,
                imp_type_handle *member_ith, location_context *lc, address *a )
{
    /* unused parameters */ (void)iih; (void)obj_ith; (void)member_ith; (void)lc; (void)a;

    /* will never get called */
    return( DS_FAIL );
}

static walk_result WVIMPENTRY( WalkSymList )( imp_image_handle *iih, symbol_source ss,
                    void *src, DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish, void *d )
{
    /* unused parameters */ (void)iih; (void)ss; (void)src; (void)wk; (void)ish; (void)d;

    return( WR_CONTINUE );
}

static walk_result WVIMPENTRY( WalkSymListEx )( imp_image_handle *iih, symbol_source ss, void *src,
                        DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish, location_context *lc, void *d )
{
    /* unused parameters */ (void)iih; (void)ss; (void)src; (void)wk; (void)ish; (void)lc; (void)d;

    return( WR_CONTINUE );
}

static imp_mod_handle WVIMPENTRY( SymMod )( imp_image_handle *iih, imp_sym_handle *ish )
{
    /* unused parameters */ (void)iih; (void)ish;

    return( IMH_WV );
}

static size_t WVIMPENTRY( SymName )( imp_image_handle *iih, imp_sym_handle *ish,
    location_context *lc, symbol_name_type snt, char *name, size_t max )
{
    size_t      len;
    char  const *p;

    /* unused parameters */ (void)iih; (void)lc;

    if( snt == SNT_DEMANGLED )
        return( 0 );
    if( ish->ri != NULL ) {
        p   = ish->ri->name;
        len = strlen( p );
    } else {
        p   = SYM_NAME_NAME( ish->p->name );
        len =  SYM_NAME_LEN( ish->p->name );
    }
    if( max > 0 ) {
        --max;
        if( max > len )
            max = len;
        memcpy( name, p, max );
        name[max] = NULLCHAR;
    }
    return( len );
}

static dip_status WVIMPENTRY( SymType )( imp_image_handle *iih, imp_sym_handle *ish, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih;

    if( ish->ri != NULL ) {
        ith->ti.kind = TK_NONE;
        ith->ri = ish->ri;
    } else {
        ith->ti = ish->p->info.ti;
        ith->ri = NULL;
    }
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymLocation )( imp_image_handle *iih, imp_sym_handle *ish,
                                                location_context *lc, location_list *ll )
{
    const wv_sym_entry  *se;
    void                *d;

    /* unused parameters */ (void)iih;

    se = ish->p;
    if( ish->ri != NULL ) {
        return( RegLocation( lc->regs, ish->ri, ll ) );
    }
    switch( se->info.sc ) {
    case SC_USER:
        if( se->info.ti.kind == TK_STRING ) {
            d = se->info.v.string;
        } else {
            d = (void *)&se->info.v;
        }
        LocationCreate( ll, LT_INTERNAL, d );
        break;
    default:
        return( DS_ERR | DS_BAD_PARM );
    }
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymValue )( imp_image_handle *iih, imp_sym_handle *ish,
                                                        location_context *lc, void *d )
{
    /* unused parameters */ (void)iih; (void)lc;

    if( ish->ri != NULL || ish->p->info.sc != SC_INTERNAL )
        return( DS_ERR | DS_BAD_PARM );
    InternalValue( ish->p->info.v.internal, d );
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymInfo )( imp_image_handle *iih, imp_sym_handle *ish,
                                                location_context *lc, sym_info *si )
{
    /* unused parameters */ (void)iih; (void)lc;

    memset( si, 0, sizeof( *si ) );
    si->is_global = true;
    if( ish->ri != NULL ) {
        si->kind = SK_DATA;
        return( DS_OK );
    }
    switch( ish->p->info.sc ) {
    case SC_USER:
        si->kind = SK_DATA;
        break;
    case SC_INTERNAL:
        si->kind = SK_CONST;
        break;
    case SC_TYPE:
        si->kind = SK_TYPE;
        break;
    }
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymParmLocation )( imp_image_handle *iih, imp_sym_handle *proc_ish,
                    location_context *lc, location_list *ll, unsigned parm )
{
    /* unused parameters */ (void)iih; (void)proc_ish; (void)lc; (void)ll; (void)parm;

    /* will never get called */
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( SymObjType )( imp_image_handle *iih, imp_sym_handle *proc_ish,
                    imp_type_handle *ith, dig_type_info *ti )
{
    /* unused parameters */ (void)iih; (void)proc_ish; (void)ith; (void)ti;

    /* will never get called */
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( SymObjLocation )( imp_image_handle *iih, imp_sym_handle *proc_ish,
                    location_context *lc, location_list *ll )
{
    /* unused parameters */ (void)iih; (void)proc_ish; (void)lc; (void)ll;

    /* will never get called */
    return( DS_FAIL );
}

static search_result WVIMPENTRY( AddrSym )( imp_image_handle *iih, imp_mod_handle imh,
                        address addr, imp_sym_handle *ish )
{
    /* unused parameters */ (void)iih; (void)imh; (void)addr; (void)ish;

    return( SR_NONE );
}

static search_result DoLookupSym( imp_image_handle *iih, symbol_source ss, void *src, lookup_item *li,
                                                        location_context *lc, sym_list **sl_head )
{
    imp_type_handle     *ith;
    imp_sym_handle      *ish;
    const wv_sym_entry  *se;
    const mad_reg_info  *ri;

    /* unused parameters */ (void)lc;

    if( li->type != ST_NONE )
        return( SR_NONE );
    if( li->scope.start != NULL )
        return( SR_NONE );
    se = NULL;
    ri = NULL;
    switch( ss ) {
    case SS_TYPE:
        ith = src;
        ri = LookupRegName( ith->ri, li );
        break;
    case SS_MODULE:
    case SS_SCOPED:
        se = LookupInternalName( li );
        if( se != NULL )
            break;
        ri = LookupRegName( NULL, li );
        if( ri != NULL )
            break;
        se = LookupUserName( li );
        break;
    }
    if( se == NULL && ri == NULL )
        return( SR_NONE );
    ish = DoSymCreate( iih, sl_head );
    ish->p  = se;
    ish->ri = ri;
    return( SR_EXACT );
}

static search_result WVIMPENTRY( LookupSym )( imp_image_handle *iih, symbol_source ss, void *src,
                                                                        lookup_item *li, void *d )
{
    return( DoLookupSym( iih, ss, src, li, NULL, (sym_list **)d ) );
}

static search_result WVIMPENTRY( LookupSymEx )( imp_image_handle *iih, symbol_source ss, void *src,
                                                    lookup_item *li, location_context *lc, void *d )
{
    return( DoLookupSym( iih, ss, src, li, lc, (sym_list **)d ) );
}

static search_result WVIMPENTRY( AddrScope )( imp_image_handle *iih, imp_mod_handle imh,
                        address pos, scope_block *scope )
{
    /* unused parameters */ (void)iih; (void)imh; (void)pos; (void)scope;

    /* never called */
    return( SR_NONE );
}

static search_result WVIMPENTRY( ScopeOuter )( imp_image_handle *iih, imp_mod_handle imh,
                        scope_block *in, scope_block *out )
{
    /* unused parameters */ (void)iih; (void)imh; (void)in; (void)out;

    /* never called */
    return( SR_NONE );
}


static walk_result WVIMPENTRY( WalkFileList )( imp_image_handle *iih, imp_mod_handle imh,
            DIP_IMP_CUE_WALKER *wk, imp_cue_handle *icueh, void *d )
{
    /* unused parameters */ (void)iih; (void)imh; (void)wk; (void)icueh; (void)d;

    return( WR_CONTINUE );
}

static imp_mod_handle WVIMPENTRY( CueMod )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    /* will never get called */
    return( IMH_WV );
}

static size_t WVIMPENTRY( CueFile )( imp_image_handle *iih, imp_cue_handle *icueh, char *name, size_t max )
{
    /* unused parameters */ (void)iih; (void)icueh; (void)name; (void)max;

    /* will never get called */
    return( 0 );
}


static cue_fileid   WVIMPENTRY( CueFileId )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    /* will never get called */
    return( 0 );
}

static dip_status WVIMPENTRY( CueAdjust )( imp_image_handle *iih, imp_cue_handle *src_icueh,
                        int adj, imp_cue_handle *dst_icueh )
{
    /* unused parameters */ (void)iih; (void)src_icueh; (void)adj; (void)dst_icueh;

    /* will never get called */
    return( DS_FAIL );
}

static unsigned long WVIMPENTRY( CueLine )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    /* will never get called */
    return( 0 );
}

static unsigned WVIMPENTRY( CueColumn )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    /* will never get called */
    return( 0 );
}

static address WVIMPENTRY( CueAddr )( imp_image_handle *iih, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)icueh;

    /* will never get called */
    return( NilAddr );
}


static search_result WVIMPENTRY( LineCue )( imp_image_handle *iih,imp_mod_handle imh, cue_fileid file,
                    unsigned long line, unsigned column, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)imh; (void)file; (void)line; (void)column; (void)icueh;

    return( SR_NONE );
}

static search_result WVIMPENTRY( AddrCue )( imp_image_handle *iih, imp_mod_handle imh,
                                address a, imp_cue_handle *icueh )
{
    /* unused parameters */ (void)iih; (void)imh; (void)a; (void)icueh;

    return( SR_NONE );
}


static int WVIMPENTRY( TypeCmp )( imp_image_handle *iih, imp_type_handle *ith1, imp_type_handle *ith2 )
{
    /* unused parameters */ (void)iih; (void)ith1; (void)ith2;

    /* never called */
    return( 0 );
}


static int WVIMPENTRY( SymCmp )( imp_image_handle *iih, imp_sym_handle *ish1, imp_sym_handle *ish2 )
{
    /* unused parameters */ (void)iih; (void)ish1; (void)ish2;

    /* never called */
    return( 0 );
}


static int WVIMPENTRY( CueCmp )( imp_image_handle *iih, imp_cue_handle *icueh1, imp_cue_handle *icueh2 )
{
    /* unused parameters */ (void)iih; (void)icueh1; (void)icueh2;

    /* never called */
    return( 0 );
}


static size_t WVIMPENTRY( TypeName )( imp_image_handle *iih, imp_type_handle *ith,
                unsigned num, symbol_type *tag, char *buff, size_t max )
{
    /* unused parameters */ (void)iih; (void)ith; (void)num; (void)tag; (void)buff; (void)max;

    return( 0 );
}


static dip_status WVIMPENTRY( TypeAddRef )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

static dip_status WVIMPENTRY( TypeRelease )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

static dip_status WVIMPENTRY( TypeFreeAll )( imp_image_handle *iih )
{
    /* unused parameters */ (void)iih;

    return( DS_OK );
}

static dip_status WVIMPENTRY( SymAddRef )( imp_image_handle *iih, imp_sym_handle *ish )
{
    /* unused parameters */ (void)iih; (void)ish;

    return( DS_OK );
}

static dip_status WVIMPENTRY( SymRelease )( imp_image_handle *iih, imp_sym_handle *ish )
{
    /* unused parameters */ (void)iih; (void)ish;

    return( DS_OK );
}

static dip_status WVIMPENTRY( SymFreeAll )( imp_image_handle *iih )
{
    /* unused parameters */ (void)iih;

    return( DS_OK );
}

static dip_imp_routines InternalInterface = {
    DIP_MAJOR,
    DIP_MINOR,
    DIP_PRIOR_EXPORTS,
    WVImp( Name ),

    WVImp( HandleSize ),
    WVImp( MoreMem ),
    WVImp( Shutdown ),
    WVImp( Cancel ),

    WVImp( LoadInfo ),
    WVImp( MapInfo ),
    WVImp( UnloadInfo ),

    WVImp( WalkModList ),
    WVImp( ModName ),
    WVImp( ModSrcLang ),
    WVImp( ModInfo ),
    WVImp( ModDefault ),
    WVImp( AddrMod ),
    WVImp( ModAddr ),

    WVImp( WalkTypeList ),
    WVImp( TypeMod ),
    WVImp( TypeInfo ),
    WVImp( TypeBase ),
    WVImp( TypeArrayInfo ),
    WVImp( TypeProcInfo ),
    WVImp( TypePtrAddrSpace ),
    WVImp( TypeThunkAdjust ),
    WVImp( TypeCmp ),
    WVImp( TypeName ),

    WVImp( WalkSymList ),
    WVImp( SymMod ),
    WVImp( SymName ),
    WVImp( SymType ),
    WVImp( SymLocation ),
    WVImp( SymValue ),
    WVImp( SymInfo ),
    WVImp( SymParmLocation ),
    WVImp( SymObjType ),
    WVImp( SymObjLocation ),
    WVImp( AddrSym ),
    WVImp( LookupSym ),
    WVImp( AddrScope ),
    WVImp( ScopeOuter ),
    WVImp( SymCmp ),

    WVImp( WalkFileList ),
    WVImp( CueMod ),
    WVImp( CueFile ),
    WVImp( CueFileId ),
    WVImp( CueAdjust ),
    WVImp( CueLine ),
    WVImp( CueColumn ),
    WVImp( CueAddr ),
    WVImp( LineCue ),
    WVImp( AddrCue ),
    WVImp( CueCmp ),

    WVImp( TypeBaseLocation ),

    WVImp( TypeAddRef ),
    WVImp( TypeRelease ),
    WVImp( TypeFreeAll ),

    WVImp( SymAddRef ),
    WVImp( SymRelease ),
    WVImp( SymFreeAll ),

    WVImp( WalkSymListEx ),
    WVImp( LookupSymEx ),
};

static char **DIPErrTxt[] = {
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( LDS_TOO_MANY_DIPS ),
    LITREF_ENG( LDS_INVALID_DIP_VERSION ),
    LITREF_ENG( LDS_NO_MEM ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( LDS_FOPEN_FAILED ),
    LITREF_ENG( LDS_FREAD_FAILED ),
    LITREF_ENG( LDS_FWRITE_FAILED ),
    LITREF_ENG( LDS_FSEEK_FAILED ),
    LITREF_ENG( LDS_INVALID_DIP ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( LDS_INFO_INVALID ),
    LITREF_ENG( LDS_INFO_BAD_VERSION ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
    LITREF_ENG( Empty ),
};

char *DIPMsgText( dip_status ds )
{
    ds &= ~DS_ERR;
    if( ds > DS_INVALID_OPERATOR )
        ds = DS_FAIL;
    return( *DIPErrTxt[ds] );
}

static bool CheckDIPLoad( char *dip, bool defaults )
{
    dip_status  ds;

    ds = DIPLoad( dip );
    if( ds != DS_OK ) {
        if( defaults && ( ds == (DS_ERR | DS_FOPEN_FAILED) ) )
            return( false );
        DIPFini();
        Format( TxtBuff, LIT_ENG( DIP_load_failed ), dip, DIPMsgText( ds ) );
        StartupErr( TxtBuff );
    }
    return( true );
}

void InitDbgInfo( void )
{
    char        **dip;
    char        *p;
    char        *d;
    unsigned    dip_count;

    if( DIPInit() != DS_OK ) {
        StartupErr( LIT_ENG( STARTUP_DIP_Not_Init ) );
    }
    if( DIPRegister( &InternalInterface ) != DS_OK ) {
        DIPFini();
        StartupErr( LIT_ENG( STARTUP_DIP_Not_Register ) );
    }
    dip = DipFiles;
    if( *dip == NULL ) {
        dip_count = 0;
        for( p = DIPDefaults; *p != NULLCHAR; p += strlen( p ) + 1 ) {
            if( CheckDIPLoad( p, true ) ) {
                ++dip_count;
            }
        }
        if( dip_count == 0 ) {
            DIPFini();
            d = StrCopy( LIT_ENG( No_DIPs_Found ), TxtBuff );
            *d++ = ' ';
            *d++ = '(';
            for( p = DIPDefaults; *p != NULLCHAR; p += strlen( p ) + 1 ) {
                d = StrCopy( p, d );
                *d++ = ',';
            }
            --d;
            *d++ = ')';
            *d = NULLCHAR;
            StartupErr( TxtBuff );
        }
    } else {
        do {
            CheckDIPLoad( *dip, false );
            _Free( *dip );
            *dip = NULL;
        } while( *++dip != NULLCHAR );
    }
}

void FiniDbgInfo( void )
{
    DIPFini();
    PurgeUserNames();
}

bool IsInternalMod( mod_handle mod )
{
    return( DIPImageName( mod ) == WVImp( Name ) );
}

bool IsInternalModName( const char *start, size_t len )
{
    if( len != sizeof( InternalName ) - 1 )
        return( false );
    if( strnicmp( start, InternalName, len ) != 0 )
        return( false );
    return( true );
}
