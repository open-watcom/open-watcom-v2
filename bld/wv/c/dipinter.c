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


#include <stddef.h>
#include "dbgdefn.h"
#undef AddrMod
#undef AddrSym
#undef AddrScope
#undef AddrCue
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "dipimp.h"
#include "dipcli.h"
#include "dipwv.h"
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

void MadTypeToDipTypeInfo( mad_type_handle mt, dip_type_info *ti )
{
    mad_type_info       mti;

    MADTypeInfo( mt, &mti );
    ti->size = BITS2BYTES( mti.b.bits );
    ti->modifier = TM_NONE;
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
        return( DS_ERR|DS_CONTEXT_ITEM_INVALID );
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
        return( DS_ERR|DS_NO_CONTEXT );
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
                        location_list *src, unsigned long size )
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


void DIPCLIENTRY( Status )( dip_status status )
{
    DIPStatus = status;
}

dig_mad DIPCLIENTRY( CurrMAD )( void )
{
    return( SysConfig.mad );
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

search_result DeAliasAddrCue( mod_handle mh, address a, cue_handle *ch )
{
    DeAlias( &a.mach );
    return( DIPAddrCue( mh, a, ch ) );
}

/*
 * Internal symbol table interface routines
 */

static char     WVImp( Name )[] = "Debugger Internal";

static unsigned WVIMPENTRY( HandleSize )( handle_kind hk )
{
    static const unsigned_8 Sizes[] = {
        #define pick(e,h,ih,wih)    wih,
        #include "diphndls.h"
        #undef pick
    };
    return( Sizes[hk] );
}

static dip_status WVIMPENTRY( MoreMem )( unsigned amount )
{
    amount = amount;
    return( DS_FAIL );
}

#if 0
static dip_status WVIMPENTRY( Startup )( void )
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

static dip_status WVIMPENTRY( LoadInfo )( dig_fhandle dfh, imp_image_handle *ii )
{
    dfh = dfh;
    ii = ii;
    return( DS_FAIL );
}

static void WVIMPENTRY( MapInfo )( imp_image_handle *ii, void *d )
{
    ii = ii; d = d;
}

static void WVIMPENTRY( UnloadInfo )( imp_image_handle *ii )
{
    ii = ii;
}

static walk_result WVIMPENTRY( WalkModList )( imp_image_handle *ii, DIP_IMP_MOD_WALKER *wk, void *d )
{
    return( wk( ii, WV_INT_MH, d ) );
}

static const char InternalName[] = "_dbg";

static size_t WVIMPENTRY( ModName )( imp_image_handle *ii, imp_mod_handle im, char *name, size_t max )
{
    size_t  len;

    ii = ii; im = im;
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


static char *WVIMPENTRY( ModSrcLang )( imp_image_handle *ii, imp_mod_handle im )
{
    ii = ii; im = im;
    return( LIT_ENG( Empty ) );
}

static dip_status WVIMPENTRY( ModInfo )( imp_image_handle *ii, imp_mod_handle im,
                        handle_kind hk )
{
    static const dip_status Kinds[] = { DS_FAIL, DS_OK, DS_FAIL, DS_OK };

    ii = ii; im = im;
    return( Kinds[hk] );
}

static dip_status WVIMPENTRY( ModDefault )( imp_image_handle *ii, imp_mod_handle im,
                        handle_kind dk, dip_type_info *ti )
{
    /* never called */
    ii = ii; im = im; dk = dk, ti = ti;
    return( DS_FAIL );
}

static search_result WVIMPENTRY( AddrMod )( imp_image_handle *ii, address a, imp_mod_handle *imp )
{
    ii = ii; a = a; imp = imp;
    return( SR_NONE );
}

static address WVIMPENTRY( ModAddr )( imp_image_handle *ii, imp_mod_handle im )
{
    ii = ii; im = im;
    return( NilAddr );
}


static walk_result WVIMPENTRY( WalkTypeList )( imp_image_handle *ii, imp_mod_handle im,
                        DIP_IMP_TYPE_WALKER *wk, imp_type_handle *it, void *d )
{
    ii = ii; im = im; wk = wk, it=it, d = d;
    return( WR_CONTINUE );
}

static imp_mod_handle WVIMPENTRY( TypeMod )( imp_image_handle *ii, imp_type_handle *it )
{
    ii = ii; it = it;
    return( WV_INT_MH );
}

static dip_status WVIMPENTRY( TypeInfo )( imp_image_handle *ii, imp_type_handle *it,
                        location_context *lc, dip_type_info *ti )
{

    ii = ii; lc = lc;
    if( it->ri != NULL ) {
        MadTypeToDipTypeInfo( it->ri->type, ti );
    } else {
        ti->kind = it->t.k;
        ti->modifier = it->t.m;
        ti->size = it->t.s;
    }
    return( DS_OK );
}

static dip_status WVIMPENTRY( TypeBase )( imp_image_handle *ii, imp_type_handle *src_it,
                                imp_type_handle *dst_it )
{
    ii = ii;
    *dst_it = *src_it;
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypeBaseLocation )( imp_image_handle *ii, imp_type_handle *src_it,
                    imp_type_handle *dst_it, location_context *lc, location_list *ll )
{
    ii = ii; lc = lc; ll = ll;
    *dst_it = *src_it;
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypeArrayInfo )( imp_image_handle *ii, imp_type_handle *it,
                location_context *lc, array_info *ai, imp_type_handle *index_it )
{
    /* will never get called */
    ii = ii; it = it; lc = lc; ai = ai; index_it = index_it;
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypeProcInfo )( imp_image_handle *ii, imp_type_handle *it,
                imp_type_handle *parm_it, unsigned parm )
{
    /* will never get called */
    ii = ii; it = it; parm_it = parm_it; parm = parm;
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypePtrAddrSpace )( imp_image_handle *ii, imp_type_handle *it,
                        location_context *lc, address *addr )
{
    /* will never get called */
    ii = ii; it = it; lc = lc; addr = addr;
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( TypeThunkAdjust )( imp_image_handle *ii, imp_type_handle *obj_it,
                imp_type_handle *member_it, location_context *lc, address *a )
{
    /* will never get called */
    ii = ii; obj_it = obj_it; member_it = member_it; lc = lc; a = a;
    return( DS_FAIL );
}

static walk_result WVIMPENTRY( WalkSymList )( imp_image_handle *ii, symbol_source ss,
                    void *src, DIP_IMP_SYM_WALKER *wk, imp_sym_handle *is, void *d )
{
    ii = ii; ss = ss; src = src; wk = wk; is = is; d = d;
    return( WR_CONTINUE );
}

static walk_result WVIMPENTRY( WalkSymListEx )( imp_image_handle *ii, symbol_source ss,
                    void *src, DIP_IMP_SYM_WALKER *wk, imp_sym_handle *is,
                    location_context *lc, void *d )
{
    ii = ii; ss = ss; src = src; wk = wk; is = is; lc = lc; d = d;
    return( WR_CONTINUE );
}

static imp_mod_handle WVIMPENTRY( SymMod )( imp_image_handle *ii, imp_sym_handle *is )
{
    ii = ii; is = is;
    return( WV_INT_MH );
}

static size_t WVIMPENTRY( SymName )( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc,
                        symbol_name sn, char *name, size_t max )
{
    size_t      len;
    char  const *p;

    ii = ii; lc = lc;
    if( sn == SN_DEMANGLED )
        return( 0 );
    if( is->ri != NULL ) {
        p   = is->ri->name;
        len = strlen( p );
    } else {
        p   = SYM_NAME_NAME( is->p->name );
        len =  SYM_NAME_LEN( is->p->name );
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

static dip_status WVIMPENTRY( SymType )( imp_image_handle *ii, imp_sym_handle *is,
                        imp_type_handle *it )
{
    ii = ii;
    if( is->ri != NULL ) {
        it->t.k = TK_NONE;
        it->ri = is->ri;
    } else {
        it->t  = is->p->info.t;
        it->ri = NULL;
    }
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymLocation )( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc, location_list *ll )
{
    const wv_sym_entry  *se;
    void                *d;

    ii = ii;
    se = is->p;
    if( is->ri != NULL ) {
        return( RegLocation( lc->regs, is->ri, ll ) );
    }
    switch( se->info.sc ) {
    case SC_USER:
        if( se->info.t.k == TK_STRING ) {
            d = se->info.v.string;
        } else {
            d = (void *)&se->info.v;
        }
        LocationCreate( ll, LT_INTERNAL, d );
        break;
    default:
        return( DS_ERR|DS_BAD_PARM );
    }
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymValue )( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc, void *d )
{
    ii = ii; lc = lc;
    if( is->ri != NULL || is->p->info.sc != SC_INTERNAL )
        return( DS_ERR|DS_BAD_PARM );
    InternalValue( is->p->info.v.internal, d );
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymInfo )( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc, sym_info *si )
{
    memset( si, 0, sizeof( *si ) );
    ii = ii; lc = lc;
    si->is_global = true;
    if( is->ri != NULL ) {
        si->kind = SK_DATA;
        return( DS_OK );
    }
    switch( is->p->info.sc ) {
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

static dip_status WVIMPENTRY( SymParmLocation )( imp_image_handle *ii, imp_sym_handle *proc_is,
                    location_context *lc, location_list *ll, unsigned parm )
{
    /* will never get called */
    ii = ii; proc_is = proc_is; lc = lc; ll = ll, parm = parm;
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( SymObjType )( imp_image_handle *ii, imp_sym_handle *proc_is,
                    imp_type_handle *it, dip_type_info *ti )
{
    /* will never get called */
    ii = ii; proc_is = proc_is; it = it; ti = ti;
    return( DS_FAIL );
}

static dip_status WVIMPENTRY( SymObjLocation )( imp_image_handle *ii, imp_sym_handle *proc_is,
                    location_context *lc, location_list *ll )
{
    /* will never get called */
    ii = ii; proc_is = proc_is; lc = lc; ll = ll;
    return( DS_FAIL );
}

static search_result WVIMPENTRY( AddrSym )( imp_image_handle *ii, imp_mod_handle im,
                        address addr, imp_sym_handle *is )
{
    ii = ii; im = im; addr = addr; is = is;
    return( SR_NONE );
}

static search_result DoLookupSym( imp_image_handle *ii, symbol_source ss,
                                  void *src, lookup_item *li,
                                  location_context *lc, sym_list **sl_head )
{
    imp_type_handle     *it;
    imp_sym_handle      *is;
    const wv_sym_entry  *se;
    const mad_reg_info  *ri;

    if( li->type != ST_NONE )
        return( SR_NONE );
    if( li->scope.start != NULL )
        return( SR_NONE );
    se = NULL;
    ri = NULL;
    switch( ss ) {
    case SS_TYPE:
        it = src;
        ri = LookupRegName( it->ri, li );
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
    is = DoSymCreate( ii, sl_head );
    is->p  = se;
    is->ri = ri;
    return( SR_EXACT );
}

static search_result WVIMPENTRY( LookupSym )( imp_image_handle *ii,
                symbol_source ss, void *src, lookup_item *li, void *d )
{
    return( DoLookupSym( ii, ss, src, li, NULL, (sym_list **)d ) );
}

static search_result WVIMPENTRY( LookupSymEx )( imp_image_handle *ii,
                symbol_source ss, void *src, lookup_item *li,
                location_context *lc, void *d )
{
    return( DoLookupSym( ii, ss, src, li, lc, (sym_list **)d ) );
}

static search_result WVIMPENTRY( AddrScope )( imp_image_handle *ii, imp_mod_handle im,
                        address pos, scope_block *scope )
{
    /* never called */
    ii = ii; im = im; pos = pos; scope = scope;
    return( SR_NONE );
}

static search_result WVIMPENTRY( ScopeOuter )( imp_image_handle *ii, imp_mod_handle im,
                        scope_block *in, scope_block *out )
{
    /* never called */
    ii = ii; im = im; in = in; out = out;
    return( SR_NONE );
}


static walk_result WVIMPENTRY( WalkFileList )( imp_image_handle *ii, imp_mod_handle im,
            DIP_IMP_CUE_WALKER *wk, imp_cue_handle *ic, void *d )
{
    ii = ii; im = im; wk = wk; ic = ic; d = d;
    return( WR_CONTINUE );
}

static imp_mod_handle WVIMPENTRY( CueMod )( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( WV_INT_MH );
}

static size_t WVIMPENTRY( CueFile )( imp_image_handle *ii, imp_cue_handle *ic, char *name, size_t max )
{
    /* will never get called */
    ii = ii; ic = ic; name = name; max = max;
    return( 0 );
}


static cue_fileid   WVIMPENTRY( CueFileId )( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( 0 );
}

static dip_status WVIMPENTRY( CueAdjust )( imp_image_handle *ii, imp_cue_handle *orig_ic,
                        int adj, imp_cue_handle *adj_ic )
{
    /* will never get called */
    ii = ii; orig_ic = orig_ic; adj = adj; adj_ic = adj_ic;
    return( DS_FAIL );
}

static unsigned long WVIMPENTRY( CueLine )( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( 0 );
}

static unsigned WVIMPENTRY( CueColumn )( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( 0 );
}

static address WVIMPENTRY( CueAddr )( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( NilAddr );
}


static search_result WVIMPENTRY( LineCue )( imp_image_handle *ii,imp_mod_handle im, cue_fileid file,
                    unsigned long line, unsigned column, imp_cue_handle *ic )
{
    ii = ii; im=im; file = file; line = line; column = column; ic = ic;
    return( SR_NONE );
}

static search_result WVIMPENTRY( AddrCue )( imp_image_handle *ii, imp_mod_handle im,
                                address a, imp_cue_handle *ic )
{
    ii = ii; im = im; a = a; ic = ic;
    return( SR_NONE );
}


static int WVIMPENTRY( TypeCmp )( imp_image_handle *ii, imp_type_handle *it1,
                        imp_type_handle *it2 )
{
    /* never called */
    ii = ii; it1 = it1; it2 = it2;
    return( 0 );
}


static int WVIMPENTRY( SymCmp )( imp_image_handle *ii, imp_sym_handle *is1,
                        imp_sym_handle *is2 )
{
    /* never called */
    ii = ii; is1 = is1; is2 = is2;
    return( 0 );
}


static int WVIMPENTRY( CueCmp )( imp_image_handle *ii, imp_cue_handle *ic1,
                        imp_cue_handle *ic2 )
{
    /* never called */
    ii = ii; ic1 = ic1; ic2 = ic2;
    return( 0 );
}


static size_t WVIMPENTRY( TypeName )( imp_image_handle *ii, imp_type_handle *it,
                unsigned num, symbol_type *tag, char *buff, size_t max )
{
    ii = ii; it = it; num = num; tag = tag; buff = buff; max = max;
    return( 0 );
}


static dip_status WVIMPENTRY( TypeAddRef )( imp_image_handle *ii, imp_type_handle *it )
{
    ii=ii;
    it=it;
    return( DS_OK );
}

static dip_status WVIMPENTRY( TypeRelease )( imp_image_handle *ii, imp_type_handle *it )
{
    ii=ii;
    it=it;
    return( DS_OK );
}

static dip_status WVIMPENTRY( TypeFreeAll )( imp_image_handle *ii )
{
    ii=ii;
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymAddRef )( imp_image_handle *ii, imp_sym_handle *is )
{
    ii=ii;
    is=is;
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymRelease )( imp_image_handle *ii, imp_sym_handle *is )
{
    ii=ii;
    is=is;
    return( DS_OK );
}

static dip_status WVIMPENTRY( SymFreeAll )( imp_image_handle *ii )
{
    ii=ii;
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

char *DIPMsgText( dip_status status )
{
    status &= ~DS_ERR;
    if( status > DS_INVALID_OPERATOR )
        status = DS_FAIL;
    return( *DIPErrTxt[status] );
}

static bool CheckDIPLoad( char *dip, bool defaults )
{
    dip_status  ret;

    ret = DIPLoad( dip );
    if( ret != DS_OK ) {
        if( defaults && (ret == (DS_ERR|DS_FOPEN_FAILED)) )
            return( false );
        DIPFini();
        Format( TxtBuff, LIT_ENG( DIP_load_failed ), dip, DIPMsgText( ret ) );
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
    if( memicmp( start, InternalName, len ) != 0 )
        return( false );
    return( true );
}
