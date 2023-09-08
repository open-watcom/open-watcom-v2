/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  CodeView symbol processing.
*
****************************************************************************/


#include "cvinfo.h"
#include "walloca.h"
#include "watcom.h"
#include "demangle.h"

#include "clibext.h"


#define SCOPE_CLASS_FLAG        0x80000000UL
#define SCOPE_UNIQUE_MASK       (~SCOPE_CLASS_FLAG)

dip_status SymFillIn( imp_image_handle *iih, imp_sym_handle *ish, virt_mem h )
{
    s_all               *ref;
    cv_directory_entry  *cde;

    ish->containing_type = 0;
    ish->adjustor_type = 0;
    ish->adjustor_offset = 0;
    ish->mfunc_idx = 0;
    ref = VMBlock( iih, h, sizeof( ref->procref ) );
    if( ref == NULL )
        return( DS_ERR | DS_FAIL );
    if( ref->common.code == S_ALIGN ) {
        h += ref->common.length + sizeof( ref->common.length );
        ref = VMBlock( iih, h, sizeof( ref->procref ) );
    }
    switch( ref->common.code ) {
    case S_PROCREF:
    case S_DATAREF:
        ish->imh = ref->procref.f.module;
        cde = FindDirEntry( iih, ish->imh, sstAlignSym );
        if( cde == NULL )
            return( DS_ERR | DS_FAIL );
        h = ref->procref.f.offset + cde->lfo;
        ref = VMBlock( iih, h, sizeof( s_common ) );
        if( ref == NULL ) {
            return( DS_ERR | DS_FAIL );
        }
    }
    ish->handle = h;
    ish->len = ref->common.length + sizeof( ref->common.length );
    return( DS_OK );
}

static dip_status SymGetName( imp_image_handle *iih, imp_sym_handle *ish,
                            const char **name_p, size_t *name_len_p, s_all **pp )
{
    s_all               *p;
    size_t              skip = 0;
    const char          *name;
    numeric_leaf        val;

    if( ish->containing_type != 0 ) {
        static const s_common dummy = { 0, S_LDATA16 };

        if( pp != NULL )
            *pp = (s_all *)&dummy;
        return( TypeSymGetName( iih, ish, name_p, name_len_p ) );
    }
    p = VMBlock( iih, ish->handle, ish->len );
    if( p == NULL )
        return( DS_FAIL );
    if( pp != NULL )
        *pp = p;
    if( p->common.code == S_ENTRYTHIS ) {
        p = (void *) (&p->common + 1 );
    }
    switch( p->common.code ) {
    case S_REGISTER:
        skip = sizeof( s_register );
        break;
    case S_CONSTANT:
        name = (const char *)p + sizeof( s_constant );
        skip = (unsigned_8 *)GetNumLeaf( name, &val ) - (unsigned_8 *)p;
        break;
    case S_UDT:
        skip = sizeof( s_udt );
        break;
    case S_COBOLUDT:
        skip = sizeof( s_coboludt );
        break;
    case S_MANYREG:
        skip = sizeof( s_manyreg ) + p->manyreg.f.count;
        break;
    case S_BPREL16:
        skip = sizeof( s_bprel16 );
        break;
    case S_LDATA16:
    case S_GDATA16:
    case S_PUB16:
        skip = sizeof( s_ldata16 );
        break;
    case S_LPROC16:
    case S_GPROC16:
        skip = sizeof( s_lproc16 );
        break;
    case S_BLOCK16:
        skip = sizeof( s_block16 );
        break;
    case S_LABEL16:
        skip = sizeof( s_label16 );
        break;
    case S_REGREL16:
        skip = sizeof( s_regrel16 );
        break;
    case S_BPREL32:
        skip = sizeof( s_bprel32 );
        break;
    case S_LDATA32:
    case S_GDATA32:
    case S_PUB32:
        skip = sizeof( s_ldata32 );
        break;
    case S_LPROC32:
    case S_GPROC32:
        skip = sizeof( s_lproc32 );
        break;
    case S_BLOCK32:
        skip = sizeof( s_block32 );
        break;
    case S_LABEL32:
        skip = sizeof( s_label32 );
        break;
    case S_REGREL32:
        skip = sizeof( s_regrel32 );
        break;
    case S_LTHREAD32:
    case S_GTHREAD32:
        skip = sizeof( s_lthread32 );
        break;
    default:
        Confused();
    }
    name = (const char *)p + skip;
    *name_len_p = *(unsigned_8 *)name;
    *name_p = &name[1];
    return( DS_OK );
}

#define DEFTYPE( t ) \
    if( idx == 0 ) \
        idx = t;

static unsigned SymTypeIdx( imp_image_handle *iih, s_all *p )
{
    unsigned    idx;

    switch( p->common.code ) {
    case S_REGISTER:
        idx = p->register_.f.type;
        DEFTYPE( T_DATA_LBL16 );
        break;
    case S_CONSTANT:
        idx = p->constant.f.type;
        DEFTYPE( T_DATA_LBL16 );
        break;
    case S_UDT:
        idx = p->udt.f.type;
        DEFTYPE( T_DATA_LBL16 );
        break;
    case S_COBOLUDT:
        idx = p->coboludt.f.type;
        DEFTYPE( T_DATA_LBL16 );
        break;
    case S_MANYREG:
        idx = p->manyreg.f.type;
        DEFTYPE( T_DATA_LBL16 );
        break;
    case S_BPREL16:
        idx = p->bprel16.f.type;
        DEFTYPE( T_DATA_LBL16 );
        break;
    case S_LDATA16:
    case S_GDATA16:
        idx = p->ldata16.f.type;
        DEFTYPE( T_DATA_LBL16 );
        break;
    case S_PUB16:
        idx = p->pub16.f.type;
        if( SegIsExecutable( iih, p->pub16.f.segment ) ) {
            DEFTYPE( T_CODE_LBL16 );
        } else {
            DEFTYPE( T_DATA_LBL16 );
        }
        break;
    case S_LPROC16:
    case S_GPROC16:
        idx = p->lproc16.f.proctype;
        DEFTYPE( T_CODE_LBL16 );
        break;
    case S_LABEL16:
        idx = T_CODE_LBL16;
        if( SegIsExecutable( iih, p->label16.f.segment ) ) {
            DEFTYPE( T_CODE_LBL16 );
        } else {
            DEFTYPE( T_DATA_LBL16 );
        }
        break;
    case S_BPREL32:
        idx = p->bprel32.f.type;
        DEFTYPE( T_DATA_LBL16 );
        break;
    case S_LDATA32:
    case S_GDATA32:
        idx = p->ldata32.f.type;
        DEFTYPE( T_DATA_LBL32 );
        break;
    case S_PUB32:
        idx = p->ldata32.f.type;
        if( SegIsExecutable( iih, p->pub32.f.segment ) ) {
            DEFTYPE( T_CODE_LBL32 );
        } else {
            DEFTYPE( T_DATA_LBL32 );
        }
        break;
    case S_LPROC32:
    case S_GPROC32:
        idx = p->lproc32.f.proctype;
        DEFTYPE( T_CODE_LBL32 );
        break;
    case S_LABEL32:
        idx = T_CODE_LBL32;
        if( SegIsExecutable( iih, p->label32.f.segment ) ) {
            DEFTYPE( T_CODE_LBL32 );
        } else {
            DEFTYPE( T_DATA_LBL32 );
        }
        break;
    case S_REGREL32:
        idx = p->regrel32.f.type;
        DEFTYPE( T_DATA_LBL32 );
        break;
    case S_LTHREAD32:
    case S_GTHREAD32:
        idx = p->lthread32.f.type;
        DEFTYPE( T_DATA_LBL32 );
        break;
    default:
        Confused();
        idx = 0;
    }
    return( idx );
}

dip_status ImpSymLocation( imp_image_handle *iih, imp_sym_handle *ish, location_context *lc, location_list *ll )
{
    dip_status          ds;
    address             addr;
    s_all               *p;
    signed_32           disp;
    location_list       tmp_ll;
    union {
        addr32_off      off32;
        addr48_off      off48;
    }                   tmp;

    /*
     * NYI: parameters when at the start of a routine.
     */
    if( ish->containing_type != 0 ) {
        return( TypeSymGetAddr( iih, ish, lc, ll ) );
    }
    p = VMBlock( iih, ish->handle, ish->len );
    if( p == NULL )
        return( DS_FAIL );
    switch( p->common.code ) {
    case S_REGISTER:
        return( LocationOneReg( iih, p->register_.f.reg, lc, ll ) );
    case S_CONSTANT:
    case S_UDT:
    case S_COBOLUDT:
        return( DS_FAIL );
    case S_MANYREG:
        return( LocationManyReg( iih, p->manyreg.f.count, (unsigned_8 *)&p->manyreg + 1, lc, ll ) );
    case S_BPREL16:
        ds = DCItemLocation( lc, CI_FRAME, ll );
        if( ds != DS_OK ) {
            DCStatus( ds );
            return( ds );
        }
        LocationAdd( ll, p->bprel16.f.offset * 8 );
        break;
    case S_LDATA16:
    case S_GDATA16:
    case S_PUB16:
        addr.mach.offset = p->ldata16.f.offset;
        addr.mach.segment = p->ldata16.f.segment;
        MapLogical( iih, &addr );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_LPROC16:
    case S_GPROC16:
        addr.mach.offset = p->lproc16.f.offset;
        addr.mach.segment = p->lproc16.f.segment;
        MapLogical( iih, &addr );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_BLOCK16:
        addr.mach.offset = p->block16.f.offset;
        addr.mach.segment = p->block16.f.segment;
        MapLogical( iih, &addr );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_LABEL16:
        addr.mach.offset = p->label16.f.offset;
        addr.mach.segment = p->label16.f.segment;
        MapLogical( iih, &addr );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_REGREL16:
        disp = p->regrel16.f.offset;
        ds = LocationOneReg( iih, p->regrel16.f.reg, lc, ll );
        if( ds != DS_OK )
            return( ds );
        LocationCreate( &tmp_ll, LT_INTERNAL, &tmp.off32 );
        ds = DCAssignLocation( ll, &tmp_ll, sizeof( addr32_off ) );
        if( ds != DS_OK )
            return( ds );
        ds = DCItemLocation( lc, CI_DEF_ADDR_SPACE, ll );
        if( ds != DS_OK )
            return( ds );
        ll->e[0].u.addr.mach.offset = tmp.off32 + disp;
        break;
    case S_BPREL32:
        ds = DCItemLocation( lc, CI_FRAME, ll );
        if( ds != DS_OK ) {
            DCStatus( ds );
            return( ds );
        }
        LocationAdd( ll, p->bprel32.f.offset * 8 );
        break;
    case S_LDATA32:
    case S_GDATA32:
    case S_PUB32:
        addr.mach.offset = p->ldata32.f.offset;
        addr.mach.segment = p->ldata32.f.segment;
        MapLogical( iih, &addr );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_LPROC32:
    case S_GPROC32:
        addr.mach.offset = p->lproc32.f.offset;
        addr.mach.segment = p->lproc32.f.segment;
        MapLogical( iih, &addr );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_BLOCK32:
        addr.mach.offset = p->block32.f.offset;
        addr.mach.segment = p->block32.f.segment;
        MapLogical( iih, &addr );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_LABEL32:
        addr.mach.offset = p->label32.f.offset;
        addr.mach.segment = p->label32.f.segment;
        MapLogical( iih, &addr );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_REGREL32:
        disp = p->regrel32.f.offset;
        ds = LocationOneReg( iih, p->regrel32.f.reg, lc, ll );
        if( ds != DS_OK )
            return( ds );
        LocationCreate( &tmp_ll, LT_INTERNAL, &tmp.off48 );
        ds = DCAssignLocation( ll, &tmp_ll, sizeof( addr48_off ) );
        if( ds != DS_OK )
            return( ds );
        ds = DCItemLocation( lc, CI_DEF_ADDR_SPACE, ll );
        if( ds != DS_OK )
            return( ds );
        ll->e[0].u.addr.mach.offset = tmp.off48 + disp;
        break;
    case S_LTHREAD32:
    case S_GTHREAD32:
        addr.mach.offset = p->lthread32.f.offset;
        addr.mach.segment = p->lthread32.f.segment;
        MapLogical( iih, &addr );
        LocationCreate( ll, LT_ADDR, &addr );
        break;
    default:
        Confused();
        return( DS_FAIL );
    }
    return( DS_OK );
}

typedef struct {
    cv_directory_entry  *cde;
    address             start;
    unsigned_32         len;
    virt_mem            scope;
    virt_mem            parent;
    virt_mem            end;
    virt_mem            next;
    unsigned            code;
} scope_info;

static dip_status ScopeFillIn( imp_image_handle *iih, virt_mem chk,
                                scope_info *scope, s_all **pp )
{
    s_all       *p;

    p = VMBlock( iih, chk, sizeof( s_gproc32 ) );
    if( pp != NULL )
        *pp = p;
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    scope->code = p->common.code;
    scope->scope = chk;
    scope->next = 0;
    switch( p->common.code ) {
    case S_LPROC16:
    case S_GPROC16:
        scope->start.mach.segment = p->lproc16.f.segment;
        scope->start.mach.offset  = p->lproc16.f.offset;
        scope->len = p->lproc16.f.proc_length;
        scope->parent = p->lproc16.f.pParent;
        scope->end = p->lproc16.f.pEnd;
        scope->next = p->lproc16.f.pNext;
        break;
    case S_THUNK16:
        scope->start.mach.segment = p->thunk16.f.segment;
        scope->start.mach.offset  = p->thunk16.f.offset;
        scope->len = p->thunk16.f.length;
        scope->parent = p->thunk16.f.pParent;
        scope->end = p->thunk16.f.pEnd;
        scope->next = p->lproc16.f.pNext;
        break;
    case S_BLOCK16:
        scope->start.mach.segment = p->block16.f.segment;
        scope->start.mach.offset  = p->block16.f.offset;
        scope->len = p->block16.f.length;
        scope->parent = p->block16.f.pParent;
        scope->end = p->block16.f.pEnd;
        break;
    case S_WITH16:
        scope->start.mach.segment = p->with16.f.segment;
        scope->start.mach.offset  = p->with16.f.offset;
        scope->len = p->with16.f.length;
        scope->parent = p->with16.f.pParent;
        scope->end = p->with16.f.pEnd;
        break;
    case S_LPROC32:
    case S_GPROC32:
        scope->start.mach.segment = p->lproc32.f.segment;
        scope->start.mach.offset  = p->lproc32.f.offset;
        scope->len = p->lproc32.f.proc_length;
        scope->parent = p->lproc32.f.pParent;
        scope->end = p->lproc32.f.pEnd;
        scope->next = p->lproc16.f.pNext;
        break;
    case S_THUNK32:
        scope->start.mach.segment = p->thunk32.f.segment;
        scope->start.mach.offset  = p->thunk32.f.offset;
        scope->len = p->thunk32.f.length;
        scope->parent = p->thunk32.f.pParent;
        scope->end = p->thunk32.f.pEnd;
        scope->next = p->lproc16.f.pNext;
        break;
    case S_BLOCK32:
        scope->start.mach.segment = p->block32.f.segment;
        scope->start.mach.offset  = p->block32.f.offset;
        scope->len = p->block32.f.length;
        scope->parent = p->block32.f.pParent;
        scope->end = p->block32.f.pEnd;
        break;
    case S_WITH32:
        scope->start.mach.segment = p->with32.f.segment;
        scope->start.mach.offset  = p->with32.f.offset;
        scope->len = p->with32.f.length;
        scope->parent = p->with32.f.pParent;
        scope->end = p->with32.f.pEnd;
        break;
    default:
        return( DS_FAIL );
    }
    if( scope->parent != 0 )
        scope->parent += scope->cde->lfo;
    if( scope->next != 0 )
        scope->next += scope->cde->lfo;
    scope->end += scope->cde->lfo;
    MapLogical( iih, &scope->start );
    return( DS_OK );
}

static dip_status ScopeFindFirst( imp_image_handle *iih, imp_mod_handle imh,
                                address addr, scope_info *scope )
{
    virt_mem            chk;
    unsigned long       len;
    s_all               *p;
    scope_info          new;
    dip_status          ds;

    scope->cde = FindDirEntry( iih, imh, sstAlignSym );
    if( scope->cde == NULL )
        return( DS_FAIL );
    chk = scope->cde->lfo + sizeof( unsigned_32 );
    len = scope->cde->cb - sizeof( unsigned_32 );
    for( ;; ) {
        if( len == 0 )
            return( DS_FAIL );
        p = VMBlock( iih, chk, sizeof( s_ssearch ) );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        if( p->common.code == S_SSEARCH ) {
            scope->start.mach.segment = p->ssearch.f.segment;
            scope->start.mach.offset  = 0;
            MapLogical( iih, &scope->start );
            if( DCSameAddrSpace( scope->start, addr ) == DS_OK ) {
                chk = p->ssearch.f.sym_off + scope->cde->lfo;
                break;
            }
        }
        chk += p->common.length + sizeof( p->common.length );
        len -= p->common.length + sizeof( p->common.length );
    }
    /*
     * found first scope block, now find correct offset
     */
    for( ;; ) {
        ds = ScopeFillIn( iih, chk, scope, NULL );
        if( ds != DS_OK )
            return( DS_ERR | ds );
        if( addr.mach.offset >= scope->start.mach.offset
          && addr.mach.offset < (scope->start.mach.offset + scope->len) ) {
            break;
        }
        if( scope->next == 0 )
            return( DS_FAIL );
        chk = scope->next;
    }
    /*
     * found enclosing scope block, now find smallest one
     */
    chk = scope->scope;
    new.cde = scope->cde;
    for( ;; ) {
        ds = ScopeFillIn( iih, chk, &new, NULL );
        if( ds & DS_ERR )
            return( ds );
        if( ds == DS_OK ) {
            /*
             * new scope
             */
            if( !(addr.mach.offset >= new.start.mach.offset
              && addr.mach.offset <  (new.start.mach.offset + new.len)) ) {
                /*
                 * out of range
                 */
                chk = new.end;
            } else if( !(new.start.mach.offset >= scope->start.mach.offset
                   && (new.start.mach.offset + new.len) <= (scope->start.mach.offset + scope->len)) ) {
                /*
                 * not smaller
                 */
                chk = new.end;
            } else {
                /*
                 * inner scope
                 */
                *scope = new;
            }
        } else if( p->common.code == S_END ) {
            /*
             * all done
             */
            return( DS_OK );
        }
        p = VMBlock( iih, chk, sizeof( p->common ) );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        chk += p->common.length + sizeof( p->common.length );
    }
}

static dip_status ScopeFindNext( imp_image_handle *iih, scope_info *scope )
{
    if( scope->parent == 0 )
        return( DS_FAIL );
    return( ScopeFillIn( iih, scope->parent, scope, NULL ) );
}


static walk_result ScopeOneSymbol( imp_image_handle *iih, cv_directory_entry *cde,
                    scope_info *scope, DIP_IMP_SYM_WALKER *wk, void *d,
                    virt_mem *currp )
{
    scope_info          new;
    dip_status          ds;
    walk_result         wr;
    imp_sym_handle      ish;
    s_all               *p;
    virt_mem            curr;

    curr = *currp;
    new.cde = cde;
    ds = ScopeFillIn( iih, curr, &new, &p );
    if( ds & DS_ERR )
        return( WR_FAIL );
    if( ds == DS_OK ) {
        *currp = new.end;
        p = VMBlock( iih, *currp, sizeof( p->common ) );
        if( p == NULL ) {
            return( WR_FAIL );
        }
    }
    *currp += p->common.length + sizeof( p->common.length );
    switch( new.code ) {
    case S_COMPILE:
    case S_SSEARCH:
    case S_END:
    case S_SKIP:
    case S_OBJNAME:
    case S_ENDARG:
    case S_RETURN:
    case S_CEXMODEL16:
    case S_VFTPATH16:
    case S_THUNK16:
    case S_BLOCK16:
    case S_WITH16:
    case S_CEXMODEL32:
    case S_VFTPATH32:
    case S_THUNK32:
    case S_BLOCK32:
    case S_WITH32:
        /*
         * not interested in these
         */
        break;
    case S_ENTRYTHIS:
        curr += sizeof( s_common );
        /* fall through */
    default:
        if( ds == DS_OK && scope != NULL ) {
            /*
             * starting a new scope and not doing file scope
             * symbols -- skip scope start symbol
             */
            break;
        }
        ish.imh = cde->iMod;
        if( SymFillIn( iih, &ish, curr ) != DS_OK )
            return( WR_FAIL );
        wr = wk( iih, SWI_SYMBOL, &ish, d );
        if( wr != WR_CONTINUE )
            return( wr );
        break;
    }
    return( WR_CONTINUE );
}

static walk_result ScopeWalkOne( imp_image_handle *iih, scope_info *scope,
                                DIP_IMP_SYM_WALKER *wk, void *d )
{
    walk_result         wr;
    s_all               *p;
    virt_mem            curr;

    curr = scope->scope;
    p = VMBlock( iih, curr, sizeof( p->common ) );
    if( p == NULL )
        return( WR_FAIL );
    /*
     * skip over first scope start symbol
     */
    curr += p->common.length + sizeof( p->common.length );
    for( ;; ) {
        p = VMBlock( iih, curr, sizeof( p->common ) );
        if( p == NULL )
            return( WR_FAIL );
        if( p->common.code == S_END )
            return( WR_CONTINUE );
        wr = ScopeOneSymbol( iih, scope->cde, scope, wk, d, &curr );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
}

static walk_result ScopeWalkClass( imp_image_handle *iih, scope_info *scope,
                                DIP_IMP_SYM_WALKER *wk, void *d )
{
    dip_status          ds;
    imp_type_handle     ith;
    s_all               *p;
    imp_sym_handle      ish;

    p = VMBlock( iih, scope->scope, sizeof( *p ) );
    if( p == NULL )
        return( WR_FAIL );
    ds = TypeIndexFillIn( iih, SymTypeIdx( iih, p ), &ith );
    if( ds & DS_ERR )
        return( WR_FAIL );
    if( ds != DS_OK )
        return( WR_CONTINUE );
    ds = TypeMemberFuncInfo( iih, &ith, &ith, NULL, NULL );
    if( ds & DS_ERR )
        return( WR_FAIL );
    if( ds != DS_OK )
        return( WR_CONTINUE );
    return( TypeSymWalkList( iih, &ith, wk, &ish, d ) );
}

static walk_result ScopeWalkAll( imp_image_handle *iih, imp_mod_handle imh,
                        address addr, DIP_IMP_SYM_WALKER *wk, void *d )
{
    dip_status          ds;
    walk_result         wr;
    scope_info          scope;

    ds = ScopeFindFirst( iih, imh, addr, &scope );
    if( ds & DS_ERR )
        return( WR_FAIL );
    if( ds != DS_OK )
        return( WR_CONTINUE );
    for( ;; ) {
        wr = ScopeWalkOne( iih, &scope, wk, d );
        if( wr != WR_CONTINUE )
            return( wr );
        switch( scope.code ) {
        case S_LPROC16:
        case S_GPROC16:
        case S_LPROC32:
        case S_GPROC32:
            wr = ScopeWalkClass( iih, &scope, wk, d );
            if( wr != WR_CONTINUE )
                return( wr );
            break;
        }
        ds = ScopeFindNext( iih, &scope );
        if( ds & DS_ERR )
            return( WR_FAIL );
        if( ds != DS_OK ) {
            return( WR_CONTINUE );
        }
    }
}

static walk_result ScopeWalkFile( imp_image_handle *iih, imp_mod_handle imh,
                        DIP_IMP_SYM_WALKER *wk, void *d )
{
    cv_directory_entry  *cde;
    virt_mem            curr;
    virt_mem            end;
    walk_result         wr;

    cde = FindDirEntry( iih, imh, sstAlignSym );
    if( cde == NULL )
        return( WR_CONTINUE );
    curr = cde->lfo;
    end = curr + cde->cb;
    curr += sizeof( unsigned_32 );
    for( ;; ) {
        if( curr >= end )
            return( WR_CONTINUE );
        wr = ScopeOneSymbol( iih, cde, NULL, wk, d, &curr );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
}

static search_result TableSearchForAddr( imp_image_handle *iih,
                        address a, imp_sym_handle *ish, addr_off *best_off, unsigned tbl_type )
{
    cv_directory_entry          *cde;
    cv_sst_global_pub_header    *hdr;
    unsigned                    num_segs;
    void                        *p;
    unsigned                    i;
    address                     chk;
    virt_mem                    hash_base;
    virt_mem                    base;
    unsigned long               offset_count;
    unsigned long               count;
    addr_off                    new_off;
    struct {
        virt_mem                base;
        addr_off                off;
    }                           curr, best;

    cde = FindDirEntry( iih, IMH_GBL, tbl_type );
    if( cde == NULL )
        return( SR_NONE );
    hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) );
    if( hdr == NULL )
        return( SR_FAIL );
    switch( hdr->addrhash ) {
    case 12:
        hash_base = cde->lfo + hdr->cbSymbol + hdr->cbSymHash + sizeof( *hdr );
        p = VMBlock( iih, hash_base, sizeof( unsigned_16 ) );
        if( p == NULL )
            return( SR_FAIL );
        num_segs = *(unsigned_16 *)p;
        hash_base += 2 * sizeof( unsigned_16 );
        best.base = 0;
        best.off = *best_off;
        for( i = 0; i < num_segs; ++i ) {
            chk.mach.segment = i + 1;
            chk.mach.offset = 0;
            MapLogical( iih, &chk );
            if( DCSameAddrSpace( chk, a ) != DS_OK )
                continue;
            p = VMBlock( iih, hash_base + i * sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
            if( p == NULL )
                return( SR_FAIL );
            curr.base = *(unsigned_32 *)p;
            base = hash_base + num_segs * sizeof( unsigned_32 );
            p = VMBlock( iih, base + i * sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
            if( p == NULL )
                return( SR_FAIL );
            offset_count = *(unsigned_32 *)p;
            if( offset_count == 0 )
                continue;
            curr.base += base + num_segs * sizeof( unsigned_32 );
            /*
             * NYI: offsets are sorted, so we can binary search this sucker
             */
            curr.off = 0;
            new_off = 0;
            for( count = 0; count < offset_count; ++count ) {
                p = VMBlock( iih, curr.base + sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
                if( p == NULL )
                    return( SR_FAIL );
                new_off = *(unsigned_32 *)p + chk.mach.offset;
                if( new_off >= a.mach.offset )
                    break;
                curr.off = new_off;
                curr.base += 2 * sizeof( unsigned_32 );
            }
            if( new_off == a.mach.offset ) {
                best.base = curr.base;
                best.off  = new_off;
                break;
            }
            if( count != 0
              && curr.off < a.mach.offset
              && (curr.off > best.off || best.base == 0 && best.off ==  0) ) {
                best.base = curr.base - 2 * sizeof( unsigned_32 );
                best.off = curr.off;
            }
        }
        if( best.base == 0 )
            return( SR_NONE );
        p = VMBlock( iih, best.base, sizeof( unsigned_32 ) );
        if( p == NULL )
            return( SR_FAIL );
        if( SymFillIn( iih, ish, *(unsigned_32 *)p + cde->lfo + sizeof( *hdr ) ) != DS_OK ) {
            return( SR_FAIL );
        }
        *best_off = best.off;
        return( (best.off == a.mach.offset) ? SR_EXACT : SR_CLOSEST );
    default:
        /*
         * NYI: what to do when don't have hash function?
         */
        return( SR_NONE );
    }
}

static unsigned long CalcHash( const char *name, size_t len )
{
    unsigned_32         end;
    unsigned_32         sum;
    int                 i;

#define B_toupper( b )  ((b) & 0xdf)
#define D_toupper( d )  ((d) & 0xdfdfdfdf)

    end = 0;
    for( i = len & 0x3; i > 0; --i ) {
        end |= B_toupper( name[len - 1] );
        end <<= 8;
        --len;
    }
    len /= 4;
    sum = 0;
    while( len-- > 0 ) {
        sum ^= D_toupper( *(unsigned_32 *)name );
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
        sum = _lrotl( sum, 4 );
#else
        sum = (sum << 4) | (sum >> 28);
#endif
        name += 4;
    }
    return( sum ^ end );
}

typedef search_result   SEARCH_CREATOR( imp_image_handle *, s_all *, imp_sym_handle *, void * );

static search_result TableSearchForName( imp_image_handle *iih,
                bool case_sensitive, const char *name, size_t name_len,
                unsigned long hash, imp_sym_handle *ish,
                SEARCH_CREATOR *create, void *d, unsigned tbl_type )
{
    cv_directory_entry          *cde;
    cv_sst_global_pub_header    *hdr;
    void                        *p;
    unsigned                    i;
    unsigned                    hash_buckets;
    virt_mem                    hash_base;
    virt_mem                    base;
    virt_mem                    sym_base;
    unsigned long               count;
    const char                  *curr;
    size_t                      curr_len;
    s_all                       *sp;
    search_result               sr;
    strcompn_fn                 *scompn;

    cde = FindDirEntry( iih, IMH_GBL, tbl_type );
    if( cde == NULL )
        return( SR_NONE );
    hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) );
    if( hdr == NULL )
        return( SR_FAIL );
    switch( hdr->symhash ) {
    case 10:
        hash_base = cde->lfo + hdr->cbSymbol + sizeof( *hdr );
        p = VMBlock( iih, hash_base, sizeof( unsigned_16 ) );
        if( p == NULL )
            return( SR_FAIL );
        hash_buckets = *(unsigned_16 *)p;
        i = hash % hash_buckets;
        hash_base += 2 * sizeof( unsigned_16 );
        p = VMBlock( iih, hash_base + i * sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
        if( p == NULL )
            return( SR_FAIL );
        sym_base = *(unsigned_32 *)p;
        base = hash_base + hash_buckets * sizeof( unsigned_32 );
        p = VMBlock( iih, base + i * sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
        if( p == NULL )
            return( SR_FAIL );
        sym_base += base + hash_buckets * sizeof( unsigned_32 );
        scompn = ( case_sensitive ) ? strncmp : strnicmp;
        sr = SR_NONE;
        for( count = *(unsigned_32 *)p; count != 0; sym_base += 2*sizeof(unsigned_32), --count ) {
            p = VMBlock( iih, sym_base, 2 * sizeof( unsigned_32 ) );
            if( p == NULL )
                return( SR_FAIL );
            if( ((unsigned_32 *)p)[1] != hash )
                continue;
            if( SymFillIn( iih, ish, *(unsigned_32 *)p + cde->lfo + sizeof( *hdr ) ) != DS_OK ) {
                return( SR_FAIL );
            }
            if( SymGetName( iih, ish, &curr, &curr_len, &sp ) != DS_OK ) {
                return( SR_FAIL );
            }
            if( curr_len != name_len )
                continue;
            if( scompn( name, curr, curr_len ) != 0 ) {
                continue;
            }
            /*
             * Got one!
             */
            switch( create( iih, sp, ish, d ) ) {
            case SR_FAIL:
                return( SR_FAIL );
            case SR_CLOSEST:
                /*
                 * means we found one, but keep on going
                 */
                sr = SR_EXACT;
                break;
            case SR_EXACT:
                return( SR_EXACT );
            }
        }
        return( sr );
    default:
        /*
         * NYI: What to do if don't have hash table
         */
        return( SR_NONE );
    }
}

struct match_data {
    unsigned    idx;
};

static search_result MatchSym( imp_image_handle *iih, s_all *p, imp_sym_handle *ish, void *d )
{
    struct match_data   *md = d;

    /* unused parameters */ (void)ish;

    if( md->idx != SymTypeIdx( iih, p ) )
        return( SR_NONE );
    return( SR_EXACT );
}

dip_status SymFindMatchingSym( imp_image_handle *iih, const char *name, size_t name_len,
                                                    unsigned idx, imp_sym_handle *ish )
{
    unsigned long       hash;
    search_result       sr;
    struct match_data   data;

    data.idx = idx;
    hash = CalcHash( name, name_len );
    sr = TableSearchForName( iih, true, name, name_len, hash, ish, MatchSym, &data, sstStaticSym );
    switch( sr ) {
    case SR_FAIL:
        return( DS_ERR | DS_FAIL );
    case SR_EXACT:
        return( DS_OK );
    }
    sr = TableSearchForName( iih, true, name, name_len, hash, ish, MatchSym, &data, sstGlobalSym );
    switch( sr ) {
    case SR_FAIL:
        return( DS_ERR | DS_FAIL );
    case SR_EXACT:
        return( DS_OK );
    }
    return( DS_FAIL );
}

static walk_result TableWalkSym( imp_image_handle *iih, imp_sym_handle *ish,
                        DIP_IMP_SYM_WALKER *wk, void *d, unsigned tbl_type )
{
    cv_directory_entry          *cde;
    cv_sst_global_pub_header    *hdr;
    virt_mem                    base;
    virt_mem                    end;
    unsigned                    skip;
    s_all                       *p;
    walk_result                 wr;
    search_result               sr;
    address                     addr;
    imp_sym_handle              dummy_ish;
    addr_off                    dummy_off;

    ish->imh = IMH_GBL;
    cde = FindDirEntry( iih, IMH_GBL, tbl_type );
    if( cde == NULL )
        return( WR_CONTINUE );
    hdr = VMBlock( iih, cde->lfo, sizeof( *hdr ) );
    if( hdr == NULL )
        return( WR_FAIL );
    base = cde->lfo + sizeof( *hdr );
    end = base + hdr->cbSymbol;
    while( base < end ) {
        p = VMRecord( iih, base );
        skip = p->common.length + sizeof( p->common.length );
        switch( p->common.code ) {
        case S_ALIGN:
        case S_PROCREF:
        case S_DATAREF:
            /*
             * not interested
             */
            break;
        case S_PUB16:
        case S_PUB32:
            if( p->common.code == S_PUB16 ) {
                addr.mach.offset = p->pub16.f.offset;
                addr.mach.segment = p->pub16.f.segment;
            } else {
                addr.mach.offset = p->pub32.f.offset;
                addr.mach.segment = p->pub32.f.segment;
            }
            MapLogical( iih, &addr );
            dummy_off = 0;
            sr = TableSearchForAddr( iih, addr, &dummy_ish, &dummy_off, sstGlobalSym );
            if( sr == SR_FAIL )
                return( WR_FAIL );
            if( sr == SR_EXACT )
                break;
            /* fall through */
        default:
            if( SymFillIn( iih, ish, base ) != DS_OK )
                return( WR_FAIL );
            wr = wk( iih, SWI_SYMBOL, ish, d );
            if( wr != WR_CONTINUE )
                return( wr );
            break;

        }
        base += skip;
    }
    return( WR_CONTINUE );
}

struct glue_info {
    DIP_IMP_SYM_WALKER  *wk;
    void                *d;
    imp_sym_handle      *ish;
};

static walk_result WalkGlue( imp_image_handle *iih, sym_walk_info swi,
                                imp_sym_handle *ish, void *d )
{
    struct glue_info *gd = d;

    if( ish == NULL )
        return( gd->wk( iih, swi, NULL, gd->d ) );
    *gd->ish = *ish;
    return( gd->wk( iih, swi, gd->ish, gd->d ) );
}

static walk_result WalkAModule( imp_image_handle *iih,
                                cv_directory_entry *cde, void *d )
{
    if( cde->subsection != sstModule )
        return( WR_CONTINUE );
    return( ScopeWalkFile( iih, cde->iMod, WalkGlue, d ) );
}

static walk_result DoWalkSymList( imp_image_handle *iih,
                symbol_source ss, void *source, DIP_IMP_SYM_WALKER *wk,
                imp_sym_handle *ish, void *d )
{
    struct glue_info    glue;
    imp_mod_handle      imh;
    scope_block         *sc_block;
    s_all               *p;
    scope_info          sc_info;
    dip_status          ds;
    walk_result         wr;
    imp_type_handle     ith;

    glue.wk = wk;
    glue.ish = ish;
    glue.d  = d;
    switch( ss ) {
    case SS_MODULE:
        imh = *(imp_mod_handle *)source;
        if( imh == IMH_NOMOD ) {
            wr = WalkDirList( iih, &WalkAModule, &glue );
            if( wr != WR_CONTINUE )
                return( wr );
            imh = IMH_GBL;
        }
        if( imh == IMH_GBL ) {
            wr = TableWalkSym( iih, ish, wk, d, sstGlobalSym );
            if( wr != WR_CONTINUE )
                return( wr );
            return( TableWalkSym( iih, ish, wk, d, sstGlobalPub ) );
        }
        return( ScopeWalkFile( iih, imh, WalkGlue, &glue ) );
    case SS_SCOPED:
        if( ImpAddrMod( iih, *(address *)source, &imh ) == SR_NONE ) {
            return( WR_CONTINUE );
        }
        return( ScopeWalkAll( iih, imh, *(address *)source, &WalkGlue, &glue ) );
    case SS_BLOCK:
        sc_block = source;
        if( ImpAddrMod( iih, sc_block->start, &imh ) == SR_NONE ) {
            return( WR_CONTINUE );
        }
        sc_info.cde = FindDirEntry( iih, imh, sstAlignSym );
        if( sc_info.cde == NULL )
            return( WR_FAIL );
        ds = ScopeFillIn( iih, sc_block->unique & SCOPE_UNIQUE_MASK, &sc_info, &p );
        if( ds & DS_ERR )
            return( WR_FAIL );
        if( ds != DS_OK )
            return( WR_CONTINUE );
        if( sc_block->unique & SCOPE_CLASS_FLAG ) {
            /*
             * Walk the member function class scope
             */
            ds = TypeIndexFillIn( iih, SymTypeIdx( iih, p ), &ith );
            if( ds & DS_ERR )
                return( WR_FAIL );
            if( ds != DS_OK )
                return( WR_CONTINUE );
            ds = TypeMemberFuncInfo( iih, &ith, &ith, NULL, NULL );
            if( ds & DS_ERR )
                return( WR_FAIL );
            if( ds != DS_OK )
                return( WR_CONTINUE );
            return( TypeSymWalkList( iih, &ith, wk, ish, d ) );
        } else {
            return( ScopeWalkOne( iih, &sc_info, WalkGlue, &glue ) );
        }
    case SS_TYPE:
        return( TypeSymWalkList( iih, (imp_type_handle *)source, wk, ish, d ) );
    }
    return( WR_FAIL );
}

walk_result DIPIMPENTRY( WalkSymList )( imp_image_handle *iih, symbol_source ss, void *source,
                                        DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish, void *d )
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
    /* unused parameters */ (void)iih;

    return( ish->imh );
}

static size_t ImpSymName( imp_image_handle *iih, imp_sym_handle *ish, location_context *lc,
                                            symbol_name_type snt, char *buff, size_t buff_size )
{
    const char          *name;
    size_t              len;
    location_list       ll;
    dip_status          ds;
    imp_sym_handle      global_ish;
    addr_off            dummy_off;
    search_result       sr;

    switch( snt ) {
    case SNT_EXPRESSION:
        return( 0 );
    case SNT_OBJECT:
    case SNT_DEMANGLED:
        ds = ImpSymLocation( iih, ish, lc, &ll );
        if( ds != DS_OK )
            break;
        if( ll.num != 1 )
            break;
        if( ll.e[0].type != LT_ADDR )
            break;
        dummy_off = 0;
        sr = TableSearchForAddr( iih, ll.e[0].u.addr, &global_ish, &dummy_off, sstGlobalPub );
        if( sr != SR_EXACT )
            break;
        if( SymGetName( iih, &global_ish, &name, &len, NULL ) != DS_OK )
            break;
        if( snt == SNT_OBJECT ) {
            return( NameCopy( buff, name, buff_size, len ) );
        }
        if( !__is_mangled( name, len ) )
            return( 0 );
        return( __demangle_l( name, len, buff, buff_size ) );
    }
    if( snt == SNT_DEMANGLED )
        return( 0 );
    /*
     * SNT_SOURCE:
     */
    if( SymGetName( iih, ish, &name, &len, NULL ) != DS_OK )
        return( 0 );
    return( NameCopy( buff, name, buff_size, len ) );
}

size_t DIPIMPENTRY( SymName )( imp_image_handle *iih,
                        imp_sym_handle *ish, location_context *lc,
                        symbol_name_type snt, char *buff, size_t buff_size )
{
    return( ImpSymName( iih, ish, lc, snt, buff, buff_size ) );
}

dip_status ImpSymType( imp_image_handle *iih, imp_sym_handle *ish, imp_type_handle *ith )
{
    s_all       *p;

    if( ish->containing_type != 0 ) {
        return( TypeSymGetType( iih, ish, ith ) );
    }
    p = VMBlock( iih, ish->handle, ish->len );
    if( p == NULL )
        return( DS_FAIL );
    return( TypeIndexFillIn( iih, SymTypeIdx( iih, p ), ith ) );
}

dip_status DIPIMPENTRY( SymType )( imp_image_handle *iih, imp_sym_handle *ish, imp_type_handle *ith )
{
    return( ImpSymType( iih, ish, ith ) );
}

dip_status DIPIMPENTRY( SymLocation )( imp_image_handle *iih, imp_sym_handle *ish,
                                        location_context *lc, location_list *ll )
{
    return( ImpSymLocation( iih, ish, lc, ll ) );
}

dip_status      ImpSymValue( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, void *buff )
{
    s_all               *p;
    numeric_leaf        val;
    dip_status          ds;
    imp_type_handle     ith;
    dig_type_info       ti;

    if( ish->containing_type != 0 ) {
        return( TypeSymGetValue( iih, ish, lc, buff ) );
    }
    p = VMBlock( iih, ish->handle, ish->len );
    if( p == NULL )
        return( DS_FAIL );
    switch( p->common.code ) {
    case S_CONSTANT:
        GetNumLeaf( (unsigned_8 *)p + sizeof( s_constant ), &val );
        memcpy( buff, val.valp, val.size );
        ds = TypeIndexFillIn( iih, SymTypeIdx( iih, p ), &ith );
        if( ds != DS_OK )
            return( ds );
        ds = ImpTypeInfo( iih, &ith, lc, &ti );
        if( ds != DS_OK )
            return( ds );
        memset( (unsigned_8 *)buff + val.size, 0, ti.size - val.size );
        return( DS_OK );
    }
    return( DS_FAIL );
}

dip_status DIPIMPENTRY( SymValue )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, void *buff )
{
    return( ImpSymValue( iih, ish, lc, buff ) );
}

dip_status DIPIMPENTRY( SymInfo )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, sym_info *si )
{
    s_all       *p;

    memset( si, 0, sizeof( *si ) );

    if( ish->containing_type != 0 ) {
        return( TypeSymGetInfo( iih, ish, lc, si ) );
    }
    p = VMBlock( iih, ish->handle, ish->len );
    if( p == NULL )
        return( DS_FAIL );
    switch( p->common.code ) {
    case S_PUB16:
        if( SegIsExecutable( iih, p->pub16.f.segment ) == DS_OK ) {
            si->kind = SK_CODE;
        } else {
            si->kind = SK_DATA;
        }
        si->is_global = 1;
        break;
    case S_PUB32:
        if( SegIsExecutable( iih, p->pub32.f.segment ) == DS_OK ) {
            si->kind = SK_CODE;
        } else {
            si->kind = SK_DATA;
        }
        si->is_global = 1;
        break;
    case S_GDATA16:
    case S_GDATA32:
    case S_GTHREAD32:
        si->is_global = 1;
        /* fall through */
    case S_REGISTER:
    case S_MANYREG:
    case S_BPREL16:
    case S_LDATA16:
    case S_REGREL16:
    case S_LDATA32:
    case S_BPREL32:
    case S_REGREL32:
    case S_LTHREAD32:
        si->kind = SK_DATA;
        break;
    case S_CONSTANT:
        si->kind = SK_CONST;
        break;
    case S_UDT:
    case S_COBOLUDT:
        si->kind = SK_TYPE;
        break;
    case S_GPROC16:
        si->is_global = 1;
        /* fall through */
    case S_LPROC16:
        si->kind = SK_PROCEDURE;
        si->rtn_far = p->lproc16.f.flags.f.far_ret;
        si->ret_addr_offset = sizeof( unsigned_16 );
        si->prolog_size = p->lproc16.f.debug_start;
        si->epilog_size = p->lproc16.f.proc_length - p->lproc16.f.debug_end;
        si->rtn_size = p->lproc16.f.proc_length;
        /*
         * NYI: fill in si->rtn_calloc
         * NYI: fill in si->rtn_modifier
         * NYI: fill in si->ret_size
         * NYI: fill in si->num_parms
         */
        break;
    case S_GPROC32:
        si->is_global = 1;
        /* fall through */
    case S_LPROC32:
        si->kind = SK_PROCEDURE;
        si->rtn_far = p->lproc32.f.flags.f.far_ret;
        if( iih->arch == DIG_ARCH_AXP ) {
            si->ret_addr_offset = 0;
        } else {
            si->ret_addr_offset = sizeof( unsigned_32 );
        }
        si->prolog_size = p->lproc32.f.debug_start;
        si->epilog_size = p->lproc32.f.proc_length - p->lproc32.f.debug_end;
        si->rtn_size = p->lproc32.f.proc_length;
        /*
         * NYI: fill in si->rtn_calloc
         * NYI: fill in si->rtn_modifier
         * NYI: fill in si->ret_size
         * NYI: fill in si->num_parms
         */
        break;
    case S_LABEL16:
    case S_LABEL32:
        si->kind = SK_CODE;
        break;
    default:
        Confused();
        return( DS_FAIL );
    }
    return( DS_OK );
}

static const unsigned_8 DXAXList[]      = { CV_X86_DX, CV_X86_AX };
static const unsigned_8 DXEAXList[]     = { CV_X86_DX, CV_X86_EAX };
static const unsigned_8 ST1ST0List[]    = { CV_X86_ST1, CV_X86_ST0 };

dip_status DIPIMPENTRY( SymParmLocation )( imp_image_handle *iih,
                    imp_sym_handle *ish, location_context *lc,
                    location_list *ll, unsigned n )
{
    s_all               *p;
    unsigned            type;
    int                 is32;
    unsigned            parm_count;
    cv_calls            call;
    dip_status          ds;
    unsigned_8          *reg_list;
    imp_type_handle     ith;
    dig_type_info       ti;

    p = VMBlock( iih, ish->handle, ish->len );
    switch( p->common.code ) {
    case S_LPROC16:
    case S_GPROC16:
        is32 = 0;
        type = p->lproc16.f.proctype;
        break;
    case S_LPROC32:
    case S_GPROC32:
        is32 = 1;
        type = p->lproc32.f.proctype;
        break;
    default:
        return( DS_ERR | DS_FAIL );
    }
    ds = TypeCallInfo( iih, type, &call, &parm_count );
    if( ds != DS_OK )
        return( ds );
    if( n > parm_count )
        return( DS_NO_PARM );
    if( n == 0 ) {
        /*
         * return value
         */
        p = VMRecord( iih, ish->handle + ish->len );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        /*
         * WARNING: assuming that S_RETURN directly follows func defn
         */
        if( p->common.code == S_RETURN ) {
            switch( p->return_.f.style ) {
            case CVRET_VOID:
                return( DS_NO_PARM );
            case CVRET_DIRECT:
                reg_list = (unsigned_8 *)(&p->return_ + 1);
                return( LocationManyReg( iih, reg_list[0], &reg_list[1], lc, ll ) );
            case CVRET_CALLOC_NEAR:
            case CVRET_CALLOC_FAR:
            case CVRET_RALLOC_NEAR:
            case CVRET_RALLOC_FAR:
                /*
                 * NYI: have to handle these suckers
                 */
                NYI();
                break;
            }
            return( DS_ERR | DS_BAD_LOCATION );
        }
        /*
         * find out about return type
         */
        ds = TypeIndexFillIn( iih, type, &ith );
        if( ds != DS_OK )
            return( ds );
        ds = ImpTypeBase( iih, &ith, &ith );
        if( ds != DS_OK )
            return( ds );
        ds = ImpTypeInfo( iih, &ith, lc, &ti );
        if( ds != DS_OK )
            return( ds );
        switch( ti.kind ) {
        case TK_VOID:
            return( DS_BAD_LOCATION );
        case TK_BOOL:
        case TK_ENUM:
        case TK_CHAR:
        case TK_INTEGER:
        case TK_POINTER:
            switch( iih->arch ) {
            case DIG_ARCH_X86:
                switch( ti.size ) {
                case 1:
                    return( LocationOneReg( iih, CV_X86_AL, lc, ll ) );
                case 2:
                    return( LocationOneReg( iih, CV_X86_AX, lc, ll ) );
                case 4:
                    if( is32 ) {
                        return( LocationOneReg( iih, CV_X86_EAX, lc, ll ) );
                    } else {
                        return( LocationManyReg( iih, sizeof( DXAXList ), DXAXList, lc, ll ) );
                    }
                case 6:
                    return( LocationManyReg( iih, sizeof( DXEAXList ), DXEAXList, lc, ll ) );
                }
                break;
            case DIG_ARCH_AXP:
                 return( LocationOneReg( iih, CV_AXP_r0, lc, ll ) );
            }
            return( DS_ERR | DS_FAIL );
        case TK_REAL:
            switch( iih->arch ) {
            case DIG_ARCH_X86:
                return( LocationOneReg( iih, CV_X86_ST0, lc, ll ) );
            case DIG_ARCH_AXP:
                return( LocationOneReg( iih, CV_AXP_f0, lc, ll ) );
            }
            return( DS_ERR | DS_FAIL );
        case TK_COMPLEX:
            switch( iih->arch ) {
            case DIG_ARCH_X86:
                return( LocationManyReg( iih, sizeof( ST1ST0List ), ST1ST0List, lc, ll ) );
            }
            return( DS_ERR | DS_FAIL );
        case TK_STRUCT:
        case TK_ARRAY:
            /*
             * NYI: have to handle these suckers
             */
            NYI();
            break;
        }
        return( DS_ERR | DS_FAIL );
    }
    switch( call ) {
    case CV_NEARC:
    case CV_FARC:
        /*
         * all on stack
         */
        return( DS_NO_PARM );
    case CV_NEARPASCAL:
    case CV_FARPASCAL:
    case CV_NEARFASTCALL:
    case CV_FARFASTCALL:
    case CV_NEARSTDCALL:
    case CV_FARSTDCALL:
    case CV_NEARSYSCALL:
    case CV_FARSYSCALL:
    case CV_THISCALL:
    case CV_MIPS:
    case CV_AXP:
    case CV_GENERIC:
        /*
         * NYI: have to handle all of these suckers
         */
        NYI();
        break;
    }
    return( DS_ERR | DS_NO_PARM );
}

dip_status DIPIMPENTRY( SymObjType )( imp_image_handle *iih,
                    imp_sym_handle *ish, imp_type_handle *ith, dig_type_info *ti )
{
    dip_status          ds;
    imp_type_handle     func_ith;
    imp_type_handle     this_ith;

    ds = ImpSymType( iih, ish, &func_ith );
    if( ds == DS_OK ) {
        ds = TypeMemberFuncInfo( iih, &func_ith, ith, &this_ith, NULL );
        if( ds == DS_OK && ti != NULL ) {
            ds = ImpTypeInfo( iih, &this_ith, NULL, ti );
        }
    }
    return( ds );
}

dip_status DIPIMPENTRY( SymObjLocation )( imp_image_handle *iih,
                                imp_sym_handle *ish, location_context *lc,
                                 location_list *ll )
{
    const char          *name;
    size_t              len;
    dip_status          ds;
    virt_mem            check;
    virt_mem            next;
    s_all               *p;
    imp_sym_handle      parm;
    imp_type_handle     ith;
    dig_type_info       ti;
    unsigned long       adjust;

    ds = ImpSymType( iih, ish, &ith );
    if( ds != DS_OK )
        return( ds );
    ds = TypeMemberFuncInfo( iih, &ith, NULL, &ith, &adjust );
    if( ds != DS_OK )
        return( ds );
    if( ith.idx == 0 )
        return( DS_FAIL );
    check = ish->handle + ish->len;
#define THIS_NAME       "this"
    for( ;; ) {
        p = VMRecord( iih, check );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        next = check + p->common.length + sizeof( p->common.length );
        switch( p->common.code ) {
        case S_END:
        case S_ENDARG:
            return( DS_FAIL );
        }
        ds = SymFillIn( iih, &parm, check );
        if( ds != DS_OK )
            return( ds );
        ds = SymGetName( iih, &parm, &name, &len, &p );
        if( ds != DS_OK )
            return( ds );
        if( p->common.code == S_ENTRYTHIS )
            break;
        if( len == ( sizeof( THIS_NAME ) - 1 ) && memcmp( name, THIS_NAME, sizeof( THIS_NAME ) - 1 ) == 0 )
            break;
        check = next;
    }
    /*
     * We have a 'this' pointer! Repeat, we have a this pointer!
     */
    ds = ImpSymLocation( iih, &parm, lc, ll );
    if( ds != DS_OK )
        return( ds );
    ds = ImpTypeInfo( iih, &ith, lc, &ti );
    if( ds != DS_OK )
        return( ds );
    ds = DoIndirection( iih, &ti, lc, ll );
    if( ds != DS_OK )
        return( ds );
    LocationAdd( ll, adjust * 8 );
    return( DS_OK );
}

search_result DIPIMPENTRY( AddrSym )( imp_image_handle *iih,
                            imp_mod_handle imh, address a, imp_sym_handle *ish )
{
    search_result       sr;
    search_result       prev_sr;
    addr_off            best_off;

    ish->imh = imh;
    best_off = 0;
    sr = TableSearchForAddr( iih, a, ish, &best_off, sstStaticSym );
    switch( sr ) {
    case SR_EXACT:
    case SR_FAIL:
        return( sr );
    }
    prev_sr = sr;
    sr = TableSearchForAddr( iih, a, ish, &best_off, sstGlobalSym );
    switch( sr ) {
    case SR_NONE:
        sr = prev_sr;
        break;
    case SR_EXACT:
    case SR_FAIL:
        return( sr );
    }
    prev_sr = sr;
    sr = TableSearchForAddr( iih, a, ish, &best_off, sstGlobalPub );
    if( sr == SR_NONE )
        sr = prev_sr;
    return( sr );
}

struct search_data {
    lookup_item         li;
    void                *d;
    unsigned            found : 1;
};

static walk_result SymFind( imp_image_handle *iih, sym_walk_info swi,
                                imp_sym_handle *ish, void *d )
{
    struct search_data  *sd = d;
    lookup_item         *li;
    const char          *name;
    size_t              len;
    imp_sym_handle      *new_ish;
    s_all               *p;
    strcompn_fn         *scompn;

    if( swi != SWI_SYMBOL )
        return( WR_CONTINUE );
    if( SymGetName( iih, ish, &name, &len, &p ) != DS_OK )
        return( WR_FAIL );
    li = &sd->li;
    switch( p->common.code ) {
    case S_UDT:
    case S_COBOLUDT:
        if( li->type != ST_TYPE )
            return( WR_CONTINUE );
        break;
    default:
        if( li->type == ST_TYPE )
            return( WR_CONTINUE );
        break;
    }
    if( len != li->name.len )
        return( WR_CONTINUE );
    scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
    if( scompn( li->name.start, name, len ) != 0 ) {
        return( WR_CONTINUE );
    }
    /*
     * Got one!
     */
    new_ish = DCSymCreate( iih, sd->d );
    if( new_ish == NULL )
        return( WR_FAIL );
    *new_ish = *ish;
    sd->found = 1;
    return( WR_CONTINUE );
}

static search_result SearchFileScope( imp_image_handle *iih, imp_mod_handle imh,
                struct search_data *d )
{
    if( MH2IMH( d->li.mod ) != imh && MH2IMH( d->li.mod ) != IMH_NOMOD ) {
        return( SR_NONE );
    }
    switch( d->li.type ) {
    case ST_TYPE:
    case ST_NONE:
        break;
    default:
        return( SR_NONE );
    }
    if( ScopeWalkFile( iih, imh, SymFind, d ) == WR_FAIL ) {
        return( SR_FAIL );
    }
    return( d->found ? SR_EXACT : SR_NONE );
}

static search_result SymCreate( imp_image_handle *iih, s_all *p,
                        imp_sym_handle *ish, void *d )
{
    imp_sym_handle      *new_ish;

    /* unused parameters */ (void)p;

    new_ish = DCSymCreate( iih, d );
    if( new_ish == NULL )
        return( SR_FAIL );
    *new_ish = *ish;
    /*
     * keep on looking for more
     */
    return( SR_CLOSEST );
}

static search_result    DoLookupSym( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li, void *d )
{
    search_result       sr;
    imp_sym_handle      ish;
    unsigned long       hash;
    struct search_data  data;
    imp_sym_handle      *scope_ish;
    size_t              len;

    data.d = d;
    data.found = 0;
    data.li = *li;
    if( ss == SS_SCOPESYM ) {
        char    *scope_name;
        scope_ish = source;
        len = ImpSymName( iih, scope_ish, NULL, SNT_SOURCE, NULL, 0 );
        scope_name = walloca( len + 1 );
        ImpSymName( iih, scope_ish, NULL, SNT_SOURCE, scope_name, len + 1 );
        data.li.scope.start = scope_name;
        data.li.scope.len = len;
        ss = SS_MODULE;
        data.li.mod = IMH2MH( scope_ish->imh );
        source = &data.li.mod;
    }
    switch( li->type ) {
    case ST_STRUCT_TAG:
    case ST_CLASS_TAG:
    case ST_UNION_TAG:
    case ST_ENUM_TAG:
        return( TypeSearchTagName( iih, &data.li, d ) );
    }
    ish.imh = IMH_NOMOD;
    switch( ss ) {
    case SS_MODULE:
        ish.imh = *(imp_mod_handle *)source;
        sr = SearchFileScope( iih, ish.imh, &data );
        if( sr != SR_NONE )
            return( sr );
        break;
    case SS_SCOPED:
        if( ImpAddrMod( iih, *(address *)source, &ish.imh ) == SR_NONE ) {
            /*
             * just check the global symbols
             */
            break;
        }
        if( MH2IMH( data.li.mod ) != ish.imh && MH2IMH( data.li.mod ) != IMH_NOMOD ) {
            return( SR_NONE );
        }
        switch( data.li.type ) {
        case ST_NONE:
        case ST_TYPE:
            if( ScopeWalkAll( iih, ish.imh, *(address *)source, SymFind, &data ) == WR_FAIL ) {
                return( SR_FAIL );
            }
            if( data.found )
                return( SR_EXACT );
            break;
        }
        sr = SearchFileScope( iih, ish.imh, &data );
        if( sr != SR_NONE )
            return( sr );
        break;
    case SS_TYPE:
        return( TypeSearchNestedSym( iih, (imp_type_handle *)source, &data.li, d ) );
    }
    switch( data.li.type ) {
    case ST_NONE:
    case ST_TYPE:
        break;
    default:
        return( SR_NONE );
    }
    if( MH2IMH( data.li.mod ) != ish.imh && MH2IMH( data.li.mod ) != IMH_NOMOD ) {
        return( SR_NONE );
    }
    hash = CalcHash( data.li.name.start, data.li.name.len );
    sr = TableSearchForName( iih, data.li.case_sensitive,
        data.li.name.start, data.li.name.len, hash, &ish, SymCreate, d, sstStaticSym );
    if( sr != SR_NONE )
        return( sr );
    sr = TableSearchForName( iih, data.li.case_sensitive,
        data.li.name.start, data.li.name.len, hash, &ish, SymCreate, d, sstGlobalSym );
    if( sr != SR_NONE )
        return( sr );
    sr = TableSearchForName( iih, data.li.case_sensitive,
        data.li.name.start, data.li.name.len, hash, &ish, SymCreate, d, sstGlobalPub );
    return( sr );
}

#define OPERATOR_TOKEN          "operator"
#define DESTRUCTOR_TOKEN        "~"

static search_result   DoImpLookupSym( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
    lookup_token        save_name;
    symbol_type         save_type;
    search_result       sr;
    char                *new;
    size_t              new_len;

    /* unused parameters */ (void)lc;

    save_type = li->type;
    save_name = li->name;
    new_len = 0;
    if( li->scope.start != NULL ) {
        new_len = li->scope.len + SCOPE_TOKEN_LEN;
    }
    switch( save_type ) {
    case ST_OPERATOR:
        new_len += sizeof( OPERATOR_TOKEN ) - 1;
        li->type = ST_NONE;
        break;
    case ST_DESTRUCTOR:
        new_len += sizeof( DESTRUCTOR_TOKEN ) - 1;
        li->type = ST_NONE;
        break;
    }
    if( new_len > 0 ) {
        li->name.len += new_len;
        new = walloca( li->name.len );
        li->name.start = new;
        if( li->scope.start != NULL ) {
            memcpy( new, li->scope.start, li->scope.len );
            new += li->scope.len;
            memcpy( new, SCOPE_TOKEN, SCOPE_TOKEN_LEN );
            new += SCOPE_TOKEN_LEN;
        }
        switch( save_type ) {
        case ST_OPERATOR:
            memcpy( new, OPERATOR_TOKEN, sizeof( OPERATOR_TOKEN ) - 1 );
            new += sizeof( OPERATOR_TOKEN ) - 1;
            break;
        case ST_DESTRUCTOR:
            memcpy( new, DESTRUCTOR_TOKEN, sizeof( DESTRUCTOR_TOKEN ) - 1 );
            new += sizeof( DESTRUCTOR_TOKEN ) - 1;
            break;
        }
        memcpy( new, save_name.start, save_name.len );
    }
    sr = DoLookupSym( iih, ss, source, li, d );
    li->name = save_name;
    li->type = save_type;
    return( sr );
}

search_result DIPIMPENTRY( LookupSym )( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li, void *d )
{
    return( DoImpLookupSym( iih, ss, source, li, NULL, d ) );
}

search_result DIPIMPENTRY( LookupSymEx )( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
    return( DoImpLookupSym( iih, ss, source, li, lc, d ) );
}

search_result DIPIMPENTRY( AddrScope )( imp_image_handle *iih,
                imp_mod_handle imh, address addr, scope_block *scope )
{
    scope_info  sc_info;
    dip_status  ds;

    scope->unique = 0;
    ds = ScopeFindFirst( iih, imh, addr, &sc_info );
    if( ds & DS_ERR )
        return( SR_FAIL );
    if( ds != DS_OK )
        return( SR_NONE );
    scope->start = sc_info.start;
    scope->len   = sc_info.len;
    scope->unique= sc_info.scope;
    return( sc_info.start.mach.offset == addr.mach.offset ? SR_EXACT : SR_CLOSEST );
}

search_result DIPIMPENTRY( ScopeOuter )( imp_image_handle *iih,
                imp_mod_handle imh, scope_block *in, scope_block *out )
{
    scope_info  sc_info;
    dip_status  ds;

    if( in->unique == 0 )
        return( SR_NONE );
    sc_info.cde = FindDirEntry( iih, imh, sstAlignSym );
    if( sc_info.cde == NULL )
        return( SR_FAIL );
    ds = ScopeFillIn( iih, in->unique & SCOPE_UNIQUE_MASK, &sc_info, NULL );
    if( ds != DS_OK )
        return( SR_FAIL );
    if( !(in->unique & SCOPE_CLASS_FLAG) ) {
        switch( sc_info.code ) {
        case S_GPROC16:
        case S_LPROC16:
        case S_GPROC32:
        case S_LPROC32:
            /*
             * Might be a member function. We'll let WalkSym figure it out.
             */
            *out = *in;
            out->unique |= SCOPE_CLASS_FLAG;
            return( SR_CLOSEST );
        }
    }
    ds = ScopeFindNext( iih, &sc_info );
    if( ds & DS_ERR )
        return( SR_FAIL );
    if( ds != DS_OK )
        return( SR_NONE );
    out->start = sc_info.start;
    out->len   = sc_info.len;
    out->unique= sc_info.scope;
    return( SR_CLOSEST );
}

int DIPIMPENTRY( SymCmp )( imp_image_handle *iih, imp_sym_handle *ish1, imp_sym_handle *ish2 )
{
    /* unused parameters */ (void)iih;

    if( ish1->handle < ish2->handle )
        return( -1 );
    if( ish1->handle > ish2->handle )
        return( 1 );
    return( 0 );
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
