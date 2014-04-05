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


#include "dbgdefn.h"
#undef AddrMod
#undef AddrSym
#undef AddrScope
#undef AddrCue
#include "dbglit.h"
#include "dbgmem.h"
#include "dbginfo.h"
#include "dbgio.h"
#include "dipcli.h"
#include "dipimp.h"
#include "dipwv.h"
#include "dbgreg.h"
#include "mad.h"
#include <string.h>
#include <stddef.h>


extern unsigned         CueFile( cue_handle *ch, char *file, unsigned max );
extern unsigned long    CueLine( cue_handle *ch );
extern int              SectIsLoaded( unsigned, int );
extern void             AddrSection( address *, unsigned );
extern bool             SameAddrSpace( address, address );
extern void             LocationCreate( location_list *, location_type, void * );
extern dip_status       LocationAssign( location_list *, location_list *, unsigned long, bool );
extern void             LocationSet( location_list *, unsigned, unsigned );
extern mad_reg_info     *LookupRegName( mad_reg_info *, lookup_item * );
extern wv_sym_entry     *LookupInternalName( lookup_item * );
extern wv_sym_entry     *LookupUserName( lookup_item * );
extern void             InternalValue( unsigned, void * );
extern void             PurgeUserNames(void);
extern void             StartupErr( char * );
extern void             FreeImage( image_entry * );
extern void             MapAddrForImage( image_entry *, addr_ptr * );
extern image_entry      *ImageEntry( mod_handle );
extern char             *Format( char *buff, char *fmt, ... );
extern address          DefAddrSpaceForAddr( address );
extern int              AddrComp( address, address );
extern void             DeAlias( addr_ptr * );
extern char             *StrCopy( char *, char *);

extern address          NilAddr;
extern dip_status       DIPStatus;
extern char             *DipFiles[];
extern char             *TxtBuff;
extern system_config    SysConfig;

/*
 * Client support routines
 */

void DIGCLIENT DIPCliImageUnload( mod_handle mh )
{
    image_entry         *image;

    image = ImageEntry( mh );
    if( image != NULL && !image->nofree ) {
        FreeImage( image );
    }
}

void DIGCLIENT DIPCliMapAddr( addr_ptr *addr, void *d )
{
    image_entry         *id = d;

    MapAddrForImage( id, addr );
}

sym_handle *DIGCLIENT DIPCliSymCreate( void *d )
{
    sym_list    **sl_head = d;
    sym_list    *new;

    _ChkAlloc( new, sizeof(sym_list)-sizeof(byte) + sym_SIZE, LIT( ERR_NO_MEMORY_FOR_DEBUG ) );//
    new->next = *sl_head;
    *sl_head = new;
    return( SL2SH( new ) );
}

void MadTypeToDipTypeInfo( mad_type_handle mt, dip_type_info *ti )
{
    mad_type_info       mti;

    MADTypeInfo( mt, &mti );
    ti->size = mti.b.bits / BITS_PER_BYTE;
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

dip_status RegLocation( machine_state *regs, mad_reg_info const *ri, location_list *ll )
{

    if( regs == NULL || ri == NULL ) return( DS_ERR|DS_CONTEXT_ITEM_INVALID );
    LocationCreate( ll, LT_INTERNAL, &regs->mr );
    LocationSet( ll, ri->bit_start, ri->bit_size );
    ll->flags = ri->flags << LLF_REG_FLAGS_SHIFT;
    return( DS_OK );
}

dip_status DIGCLIENT DIPCliItemLocation( location_context *lc, context_item ci,
                         location_list *ll )
{
    sym_info            info;
    DIPHDL( sym, sh );

    if( lc == NULL ) return( DS_ERR|DS_NO_CONTEXT );
    switch( ci ) {
    case CI_FRAME:
        if( lc->maybe_have_frame ) {
            lc->maybe_have_frame = FALSE;
            if( DeAliasAddrSym( NO_MOD, lc->execution, sh ) == SR_NONE ) {
                /* nothing to do */
            } else if( SymInfo( sh, NULL, &info ) != DS_OK ) {
                /* nothing to do */
            } else if( info.kind != SK_PROCEDURE ) {
                /* nothing to do */
            } else if( SymLocation( sh, NULL, ll ) != DS_OK ) {
                /* nothing to do */
            } else if( lc->execution.mach.offset
                 < ll->e[0].u.addr.mach.offset + info.prolog_size ) {
                /* nothing to do */
            } else if( lc->execution.mach.offset
                 <= ll->e[0].u.addr.mach.offset+info.rtn_size-info.epilog_size ) {
                lc->have_frame = TRUE;
            }
        }
        if( Context.maybe_have_frame
            && AddrComp( Context.frame, lc->frame ) == 0
            && AddrComp( Context.execution, lc->execution ) == 0 ) {
            /* cache result in global context item */
            Context.have_frame = lc->have_frame;
            Context.maybe_have_frame = FALSE;
        }
        if( !lc->have_frame ) return( DS_ERR|DS_CONTEXT_ITEM_INVALID );
        LocationCreate( ll, LT_ADDR, &lc->frame );
        return( DS_OK );
    case CI_STACK:
        if( !lc->have_stack ) return( DS_ERR|DS_CONTEXT_ITEM_INVALID );
        LocationCreate( ll, LT_ADDR, &lc->stack );
        return( DS_OK );
    case CI_EXECUTION:
        LocationCreate( ll, LT_ADDR, &lc->execution );
        return( DS_OK );
    case CI_OBJECT:
        if( lc->maybe_have_object ) {
            lc->maybe_have_object = FALSE;
            if( DeAliasAddrSym( NO_MOD, lc->execution, sh ) == SR_NONE ) {
                /* nothing to do */
            } else if( SymInfo( sh, NULL, &info ) != DS_OK ) {
                /* nothing to do */
            } else if( info.kind != SK_PROCEDURE ) {
                /* nothing to do */
            } else if( SymObjLocation( sh, lc, &lc->object ) == DS_OK ) {
                lc->have_object = TRUE;
            }
        }
        if( !lc->have_object ) return( DS_ERR|DS_CONTEXT_ITEM_INVALID );
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

dip_status DIGCLIENT DIPCliAssignLocation( location_list *dst,
                        location_list *src, unsigned long size )
{
    return( LocationAssign( dst, src, size, FALSE ) );
}

dip_status DIGCLIENT DIPCliSameAddrSpace( address a, address b )
{
    return( SameAddrSpace( a, b ) ? DS_OK : DS_FAIL );
}

void DIGCLIENT DIPCliAddrSection( address *addr )
{
    AddrSection( addr, OVL_MAP_CURR );
}


void DIGCLIENT DIPCliStatus( dip_status status )
{
    DIPStatus = status;
}

mad_handle DIGCLIENT DIPCliCurrMAD( void )
{
    return( SysConfig.mad );
}

/*
 * Dealiasing cover routines
 */

search_result DeAliasAddrMod( address a, mod_handle *mh )
{
    DeAlias( &a.mach );
    return( AddrMod( a, mh ) );
}

search_result DeAliasAddrSym( mod_handle mh, address a, sym_handle *sh )
{
    DeAlias( &a.mach );
    return( AddrSym( mh, a, sh ) );
}

search_result DeAliasAddrScope( mod_handle mh, address a, scope_block *sb )
{
    DeAlias( &a.mach );
    return( AddrScope( mh, a, sb ) );
}

search_result DeAliasAddrCue( mod_handle mh, address a, cue_handle *ch )
{
    DeAlias( &a.mach );
    return( AddrCue( mh, a, ch ) );
}

/*
 * Internal symbol table interface routines
 */

static char     WVName[] = "Debugger Internal";

static unsigned DIGREGISTER WVHandleSize( handle_kind hk )
{
    static const unsigned_8 Sizes[] = {
        0, sizeof( imp_type_handle ), 0, sizeof( imp_sym_handle )
    };
    return( Sizes[hk] );
}

static dip_status DIGREGISTER WVMoreMem( unsigned amount )
{
    amount = amount;
    return( DS_FAIL );
}

#if 0
static dip_status DIGREGISTER WVStartup()
{
    return( DS_OK );
}
#endif

static void DIGREGISTER WVShutdown( void )
{
}

static void DIGREGISTER WVCancel( void )
{
}

static dip_status DIGREGISTER WVLoadInfo( dig_fhandle f, imp_image_handle *ii )
{
    f = f;
    ii = ii;
    return( DS_FAIL );
}

static void DIGREGISTER WVMapInfo( imp_image_handle *ii, void *d )
{
    ii = ii; d = d;
}

static void DIGREGISTER WVUnloadInfo( imp_image_handle *ii )
{
    ii = ii;
}

static walk_result DIGREGISTER WVWalkModList( imp_image_handle *ii, IMP_MOD_WKR *wk, void *d )
{
    return( wk( ii, WV_INT_MH, d ) );
}

static const char InternalName[] = "_dbg";

static unsigned DIGREGISTER WVModName( imp_image_handle *ii, imp_mod_handle im,
                                char *name, unsigned max )
{
    unsigned len;

    ii = ii; im = im;
    len = sizeof( InternalName ) - 1;
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        memcpy( name, InternalName, max );
        name[max] = '\0';
    }
    return( len );
}


static char *DIGREGISTER WVModSrcLang( imp_image_handle *ii, imp_mod_handle im )
{
    ii = ii; im = im;
    return( LIT( Empty ) );
}

static dip_status DIGREGISTER WVModInfo( imp_image_handle *ii, imp_mod_handle im,
                        handle_kind hk )
{
    static const dip_status Kinds[] = { DS_FAIL, DS_OK, DS_FAIL, DS_OK };

    ii = ii; im = im;
    return( Kinds[hk] );
}

static dip_status DIGREGISTER WVModDefault( imp_image_handle *ii, imp_mod_handle im,
                        handle_kind dk, dip_type_info *ti )
{
    /* never called */
    ii = ii; im = im; dk = dk, ti = ti;
    return( DS_FAIL );
}

static search_result DIGREGISTER WVAddrMod( imp_image_handle *ii, address a, imp_mod_handle *imp )
{
    ii = ii; a = a; imp = imp;
    return( SR_NONE );
}

static address DIGREGISTER WVModAddr( imp_image_handle *ii, imp_mod_handle im )
{
    ii = ii; im = im;
    return( NilAddr );
}


static walk_result DIGREGISTER WVWalkTypeList( imp_image_handle *ii, imp_mod_handle im,
                        IMP_TYPE_WKR *wk, imp_type_handle * it, void *d )
{
    ii = ii; im = im; wk = wk, it=it, d = d;
    return( WR_CONTINUE );
}

static imp_mod_handle DIGREGISTER WVTypeMod( imp_image_handle *ii, imp_type_handle *it )
{
    ii = ii; it = it;
    return( WV_INT_MH );
}

static dip_status DIGREGISTER WVTypeInfo( imp_image_handle *ii, imp_type_handle *it,
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

static dip_status DIGREGISTER WVTypeBase( imp_image_handle *ii, imp_type_handle *src,
                                imp_type_handle *dst )
{
    ii = ii;
    *dst = *src;
    return( DS_FAIL );
}

static dip_status DIGREGISTER WVTypeBaseLocation( imp_image_handle *ii, imp_type_handle *src,
                    imp_type_handle *dst, location_context *lc, location_list *ll )
{
    ii = ii; lc = lc; ll = ll;
    *dst = *src;
    return( DS_FAIL );
}

static dip_status DIGREGISTER WVTypeArrayInfo( imp_image_handle *ii, imp_type_handle *it,
                location_context *lc, array_info *ai, imp_type_handle *index )
{
    /* will never get called */
    ii = ii; it = it; lc = lc; ai = ai; index = index;
    return( DS_FAIL );
}

static dip_status DIGREGISTER WVTypeProcInfo( imp_image_handle *ii, imp_type_handle *it,
                imp_type_handle *parm_it, unsigned parm )
{
    /* will never get called */
    ii = ii; it = it; parm_it = parm_it; parm = parm;
    return( DS_FAIL );
}

static dip_status DIGREGISTER WVTypePtrAddrSpace( imp_image_handle *ii, imp_type_handle *it,
                        location_context *lc, address *addr )
{
    /* will never get called */
    ii = ii; it = it; lc = lc; addr = addr;
    return( DS_FAIL );
}

static dip_status DIGREGISTER WVTypeThunkAdjust( imp_image_handle *ii, imp_type_handle *obj,
                imp_type_handle *member, location_context *lc, address *a )
{
    /* will never get called */
    ii = ii; obj = obj; member = member; lc = lc; a = a;
    return( DS_FAIL );
}

static walk_result DIGREGISTER WVWalkSymList( imp_image_handle *ii, symbol_source ss,
                    void *src, IMP_SYM_WKR *wk, imp_sym_handle *is, void *d )
{
    ii = ii; ss = ss; src = src; wk = wk; is = is; d = d;
    return( WR_CONTINUE );
}

static walk_result DIGREGISTER WVWalkSymListEx( imp_image_handle *ii, symbol_source ss,
                    void *src, IMP_SYM_WKR *wk, imp_sym_handle *is,
                    location_context *lc, void *d )
{
    ii = ii; ss = ss; src = src; wk = wk; is = is; lc = lc; d = d;
    return( WR_CONTINUE );
}

static imp_mod_handle DIGREGISTER WVSymMod( imp_image_handle *ii, imp_sym_handle *is )
{
    ii = ii; is = is;
    return( WV_INT_MH );
}

static unsigned DIGREGISTER WVSymName( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc,
                        symbol_name sn, char *name, unsigned max )
{
    unsigned    len;
    char  const *p;

    ii = ii; lc = lc;
    if( sn == SN_DEMANGLED ) return( 0 );
    if( is->ri != NULL ) {
        p   = is->ri->name;
        len = strlen( p );
    } else {
        p   = &is->p->name[1];
        len =  is->p->name[0];
    }
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        memcpy( name, p, max );
        name[max] = '\0';
    }
    return( len );
}

static dip_status DIGREGISTER WVSymType( imp_image_handle *ii, imp_sym_handle *is,
                        imp_type_handle *it )
{
    ii = ii;
    if( is->ri != NULL ) {
        it->t.k = TK_NONE;
        it->ri = is->ri;
    } else {
        it->t  = is->p->t;
        it->ri = NULL;
    }
    return( DS_OK );
}

static dip_status DIGREGISTER WVSymLocation( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc, location_list *ll )
{
    wv_sym_entry        *se;
    void                *d;

    ii = ii;
    se = is->p;
    if( is->ri != NULL ) {
        return( RegLocation( lc->regs, is->ri, ll ) );
    }
    switch( se->sc ) {
    case SC_USER:
        if( se->t.k == TK_STRING ) {
            d = se->v.string;
        } else {
            d = &se->v;
        }
        LocationCreate( ll, LT_INTERNAL, d );
        break;
    default:
        return( DS_ERR|DS_BAD_PARM );
    }
    return( DS_OK );
}

static dip_status DIGREGISTER WVSymValue( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc, void *d )
{
    ii = ii; lc = lc;
    if( is->ri != NULL || is->p->sc != SC_INTERNAL ) return( DS_ERR|DS_BAD_PARM );
    InternalValue( is->p->v.internal, d );
    return( DS_OK );
}

static dip_status DIGREGISTER WVSymInfo( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc, sym_info *si )
{
    memset( si, 0, sizeof( *si ) );
    ii = ii; lc = lc;
    si->global = TRUE;
    if( is->ri != NULL ) {
        si->kind = SK_DATA;
        return( DS_OK );
    }
    switch( is->p->sc ) {
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

static dip_status DIGREGISTER WVSymParmLocation( imp_image_handle *ii, imp_sym_handle *proc,
                    location_context *lc, location_list *ll, unsigned parm )
{
    /* will never get called */
    ii = ii; proc = proc; lc = lc; ll = ll, parm = parm;
    return( DS_FAIL );
}

static dip_status DIGREGISTER WVSymObjType( imp_image_handle *ii, imp_sym_handle *proc,
                    imp_type_handle *it, dip_type_info *ti )
{
    /* will never get called */
    ii = ii; proc = proc; it = it; ti = ti;
    return( DS_FAIL );
}

static dip_status DIGREGISTER WVSymObjLocation( imp_image_handle *ii, imp_sym_handle *proc,
                    location_context *lc, location_list *ll )
{
    /* will never get called */
    ii = ii; proc = proc; lc = lc; ll = ll;
    return( DS_FAIL );
}

static search_result DIGREGISTER WVAddrSym( imp_image_handle *ii, imp_mod_handle im,
                        address addr, imp_sym_handle *is )
{
    ii = ii; im = im; addr = addr; is = is;
    return( SR_NONE );
}

static search_result DoLookupSym( imp_image_handle *ii, symbol_source ss,
                                  void *src, lookup_item *li,
                                  location_context *lc, void *d )
{
    imp_type_handle     *it;
    imp_sym_handle      *is;
    wv_sym_entry        *se;
    mad_reg_info        *ri;

    if( li->type != ST_NONE ) return( SR_NONE );
    if( li->scope.start != NULL ) return( SR_NONE );
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
        if( se != NULL ) break;
        ri = LookupRegName( NULL, li );
        if( ri != NULL ) break;
        se = LookupUserName( li );
        break;
    }
    if( se == NULL && ri == NULL ) return( SR_NONE );
    is = DCSymCreate( ii, d );
    is->p  = se;
    is->ri = ri;
    return( SR_EXACT );
}

static search_result DIGREGISTER WVLookupSym( imp_image_handle *ii,
                symbol_source ss, void *src, lookup_item *li, void *d )
{
    return( DoLookupSym( ii, ss, src, li, NULL, d ) );
}

static search_result DIGREGISTER WVLookupSymEx( imp_image_handle *ii,
                symbol_source ss, void *src, lookup_item *li,
                location_context *lc, void *d )
{
    return( DoLookupSym( ii, ss, src, li, lc, d ) );
}

static search_result DIGREGISTER WVAddrScope( imp_image_handle *ii, imp_mod_handle im,
                        address pos, scope_block *scope )
{
    /* never called */
    ii = ii; im = im; pos = pos; scope = scope;
    return( SR_NONE );
}

static search_result DIGREGISTER WVScopeOuter( imp_image_handle *ii, imp_mod_handle im,
                        scope_block *in, scope_block *out )
{
    /* never called */
    ii = ii; im = im; in = in; out = out;
    return( SR_NONE );
}


static walk_result DIGREGISTER WVWalkFileList( imp_image_handle *ii, imp_mod_handle im,
            IMP_CUE_WKR *wk, imp_cue_handle *ic, void *d )
{
    ii = ii; im = im; wk = wk; ic = ic; d = d;
    return( WR_CONTINUE );
}

static imp_mod_handle DIGREGISTER WVCueMod( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( WV_INT_MH );
}

static unsigned DIGREGISTER WVCueFile( imp_image_handle *ii, imp_cue_handle *ic,
                        char *name, unsigned max )
{
    /* will never get called */
    ii = ii; ic = ic; name = name; max = max;
    return( 0 );
}


static cue_file_id DIGREGISTER WVCueFileId( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( 0 );
}

static dip_status DIGREGISTER WVCueAdjust( imp_image_handle *ii, imp_cue_handle *orig_ic,
                        int adj, imp_cue_handle *adj_ic )
{
    /* will never get called */
    ii = ii; orig_ic = orig_ic; adj = adj; adj_ic = adj_ic;
    return( DS_FAIL );
}

static unsigned long DIGREGISTER WVCueLine( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( 0 );
}

static unsigned DIGREGISTER WVCueColumn( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( 0 );
}

static address DIGREGISTER WVCueAddr( imp_image_handle *ii, imp_cue_handle *ic )
{
    /* will never get called */
    ii = ii; ic = ic;
    return( NilAddr );
}


static search_result DIGREGISTER WVLineCue( imp_image_handle *ii,imp_mod_handle im, cue_file_id file,
                    unsigned long line, unsigned column, imp_cue_handle *ic )
{
    ii = ii; im=im; file = file; line = line; column = column; ic = ic;
    return( SR_NONE );
}

static search_result DIGREGISTER WVAddrCue( imp_image_handle *ii, imp_mod_handle im,
                                address a, imp_cue_handle *ic )
{
    ii = ii; im = im; a = a; ic = ic;
    return( SR_NONE );
}


static int DIGREGISTER WVTypeCmp( imp_image_handle *ii, imp_type_handle *it1,
                        imp_type_handle *it2 )
{
    /* never called */
    ii = ii; it1 = it1; it2 = it2;
    return( 0 );
}


static int DIGREGISTER WVSymCmp( imp_image_handle *ii, imp_sym_handle *is1,
                        imp_sym_handle *is2 )
{
    /* never called */
    ii = ii; is1 = is1; is2 = is2;
    return( 0 );
}


static int DIGREGISTER WVCueCmp( imp_image_handle *ii, imp_cue_handle *ic1,
                        imp_cue_handle *ic2 )
{
    /* never called */
    ii = ii; ic1 = ic1; ic2 = ic2;
    return( 0 );
}


static unsigned DIGREGISTER WVTypeName( imp_image_handle *ii, imp_type_handle *it,
                unsigned num, symbol_type *tag, char *buff, unsigned max )
{
    ii = ii; it = it; num = num; tag = tag; buff = buff; max = max;
    return( 0 );
}


dip_status DIGREGISTER WVTypeAddRef( imp_image_handle *ii, imp_type_handle *it )
{
    ii=ii;
    it=it;
    return(DS_OK);
}

dip_status DIGREGISTER WVTypeRelease( imp_image_handle *ii, imp_type_handle *it )
{
    ii=ii;
    it=it;
    return(DS_OK);
}

dip_status DIGREGISTER WVTypeFreeAll( imp_image_handle *ii )
{
    ii=ii;
    return(DS_OK);
}

dip_status DIGREGISTER WVSymAddRef( imp_image_handle *ii, imp_sym_handle *it )
{
    ii=ii;
    it=it;
    return(DS_OK);
}

dip_status DIGREGISTER WVSymRelease( imp_image_handle *ii, imp_sym_handle *it )
{
    ii=ii;
    it=it;
    return(DS_OK);
}

dip_status DIGREGISTER WVSymFreeAll( imp_image_handle *ii )
{
    ii=ii;
    return(DS_OK);
}

static dip_imp_routines InternalInterface = {
    DIP_MAJOR,
    DIP_MINOR,
    DP_EXPORTS,
    WVName,

    WVHandleSize,
    WVMoreMem,
    WVShutdown,
    WVCancel,

    WVLoadInfo,
    WVMapInfo,
    WVUnloadInfo,

    WVWalkModList,
    WVModName,
    WVModSrcLang,
    WVModInfo,
    WVModDefault,
    WVAddrMod,
    WVModAddr,

    WVWalkTypeList,
    WVTypeMod,
    WVTypeInfo,
    WVTypeBase,
    WVTypeArrayInfo,
    WVTypeProcInfo,
    WVTypePtrAddrSpace,
    WVTypeThunkAdjust,
    WVTypeCmp,
    WVTypeName,

    WVWalkSymList,
    WVSymMod,
    WVSymName,
    WVSymType,
    WVSymLocation,
    WVSymValue,
    WVSymInfo,
    WVSymParmLocation,
    WVSymObjType,
    WVSymObjLocation,
    WVAddrSym,
    WVLookupSym,
    WVAddrScope,
    WVScopeOuter,
    WVSymCmp,

    WVWalkFileList,
    WVCueMod,
    WVCueFile,
    WVCueFileId,
    WVCueAdjust,
    WVCueLine,
    WVCueColumn,
    WVCueAddr,
    WVLineCue,
    WVAddrCue,
    WVCueCmp,

    WVTypeBaseLocation,

    WVTypeAddRef,
    WVTypeRelease,
    WVTypeFreeAll,

    WVSymAddRef,
    WVSymRelease,
    WVSymFreeAll,

    WVWalkSymListEx,
    WVLookupSymEx,
};

static char **DIPErrTxt[] = {
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( LDS_TOO_MANY_DIPS ),
    LITREF( LDS_INVALID_DIP_VERSION ),
    LITREF( LDS_NO_MEM ),
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( LDS_FOPEN_FAILED ),
    LITREF( LDS_FREAD_FAILED ),
    LITREF( LDS_FWRITE_FAILED ),
    LITREF( LDS_FSEEK_FAILED ),
    LITREF( LDS_INVALID_DIP ),
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( LDS_INFO_INVALID ),
    LITREF( LDS_INFO_BAD_VERSION ),
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( Empty ),
    LITREF( Empty ),
};

char *DIPMsgText( dip_status status )
{
    status &= ~DS_ERR;
    if( status > DS_INVALID_OPERATOR ) status = DS_FAIL;
    return( *DIPErrTxt[ status ] );
}

static bool CheckDIPLoad( char *dip, bool defaults )
{
    dip_status  ret;

    ret = DIPLoad( dip );
    if( ret != DS_OK ) {
        if( defaults && (ret == (DS_ERR|DS_FOPEN_FAILED)) ) return( FALSE );
        DIPFini();
        Format( TxtBuff, LIT( DIP_load_failed ), dip, DIPMsgText( ret ) );
        StartupErr( TxtBuff );
    }
    return( TRUE );
}

void InitDbgInfo()
{
    char        **dip;
    char        *p;
    char        *d;
    unsigned    dip_count;

    if( DIPInit() != DS_OK ) {
        StartupErr( LIT( STARTUP_DIP_Not_Init ) );
    }
    if( DIPRegister( &InternalInterface ) != DS_OK ) {
        DIPFini();
        StartupErr( LIT( STARTUP_DIP_Not_Register ) );
    }
    dip = DipFiles;
    if( *dip == NULL ) {
        dip_count = 0;
        p = DIPDefaults;
        for( ;; ) {
            if( *p == NULLCHAR ) break;
            if( CheckDIPLoad( p, TRUE ) ) ++dip_count;
            p += strlen( p ) + 1;
        }
        if( dip_count == 0 ) {
            DIPFini();
            d = StrCopy( LIT( No_DIPs_Found ), TxtBuff );
            *d++ = ' ';
            *d++ = '(';
            p = DIPDefaults;
            for( ;; ) {
                if( *p == NULLCHAR ) break;
                d = StrCopy( p, d );
                *d++ = ',';
                p += strlen( p ) + 1;
            }
            --d;
            *d++ = ')';
            *d = NULLCHAR;
            StartupErr( TxtBuff );
        }
    } else {
        do {
            CheckDIPLoad( *dip, FALSE );
            _Free( *dip );
            *dip = NULL;
        } while( *++dip );
    }
}

void FiniDbgInfo()
{
    DIPFini();
    PurgeUserNames();
}

bool IsInternalMod( mod_handle mod )
{
    return( ImageDIP( mod ) == WVName );
}

bool IsInternalModName( char *start, unsigned len )
{
    if( len != sizeof( InternalName ) - 1 ) return( FALSE );
    if( memicmp( start, InternalName, len ) != 0 ) return( FALSE );
    return( TRUE );
}
