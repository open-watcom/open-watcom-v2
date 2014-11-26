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
* Description:  HLL/CV debugging symbol support.
*
****************************************************************************/


#include "hllinfo.h"
#include "walloca.h"
#include "demangle.h"


#define SCOPE_CLASS_FLAG        0x80000000UL
#define SCOPE_UNIQUE_MASK       (~SCOPE_CLASS_FLAG)

/*
 * Fill in a symbol handle for a symbol scope record.
 */
dip_status hllSymFillIn( imp_image_handle *ii, imp_sym_handle *is, unsigned_16 seg,
                         virt_mem h, unsigned len )
{
    is->type = HLL_SYM_TYPE_HLL_SSR;
    is->segment = seg;
    is->containing_type = 0;
    is->adjustor_type = 0;
    is->adjustor_offset = 0;
    is->mfunc_idx = 0;
    is->handle = h;
    is->len = len;
    return( DS_OK );
}

/*
 * Fill in a symbol handle for a symbol scope record.
 */
dip_status cv3SymFillIn( imp_image_handle *ii, imp_sym_handle *is, virt_mem h, unsigned len )
{
    is->type = HLL_SYM_TYPE_CV3_SSR;
    is->segment = 0; //FIXME
    is->containing_type = 0;
    is->adjustor_type = 0;
    is->adjustor_offset = 0;
    is->mfunc_idx = 0;
    is->handle = h;
    is->len = len;
    return( DS_OK );
}

/*
 * Get the name in a SSR record.
 */
static dip_status hllSymGetName( imp_image_handle *ii, imp_sym_handle *is,
                                 const char **name_p, unsigned *name_len_p )
{
    hll_ssr_all        *ssr;

    if( is->containing_type != 0 ) {
        return( hllTypeSymGetName( ii, is, name_p, name_len_p ) );
    }

    ssr = VMBlock( ii, is->handle, is->len );
    if( ssr == NULL ) {
        return( DS_FAIL );
    }
    switch( ssr->common.code ) {
    case HLL_SSR_BEGIN:
        *name_len_p = ssr->begin.name_len;
        *name_p = ssr->begin.name;
        break;
    case HLL_SSR_PROC:
    case HLL_SSR_ENTRY:
        *name_len_p = ssr->proc.name_len;
        *name_p = ssr->proc.name;
        break;
    case HLL_SSR_PROC2:
    case HLL_SSR_MEM_FUNC:
        *name_len_p = ssr->proc.name_len;
        *name_p = ssr->proc.name;
        if( *name_len_p & 0x80 ) {
            *name_len_p = ((*name_len_p & 0x7f) << 8) | *(unsigned_8 *)name_p++;
        }
        break;
    case HLL_SSR_CODE_LABEL:
        *name_len_p = ssr->code_lable.name_len;
        *name_p = ssr->code_lable.name;
        break;
    case HLL_SSR_PUBLIC:
        *name_len_p = ssr->public_.name_len;
        *name_p = ssr->public_.name;
        break;
    case HLL_SSR_STATIC:
        *name_len_p = ssr->static_.name_len;
        *name_p = ssr->static_.name;
        break;
    case HLL_SSR_STATIC2:
        *name_len_p = ssr->static2.name_len;
        *name_p = ssr->static2.name;
        if( *name_len_p & 0x80 ) {
            *name_len_p = ((*name_len_p & 0x7f) << 8) | *(unsigned_8 *)name_p++;
        }
        break;
    case HLL_SSR_STATIC_SCOPED:
        *name_len_p = ssr->static_scoped.name_len;
        *name_p = ssr->static_scoped.name;
        break;
    case HLL_SSR_TLS:
        *name_len_p = ssr->tls.name_len;
        *name_p = ssr->tls.name;
        break;
    case HLL_SSR_AUTO:
        *name_len_p = ssr->auto_.name_len;
        *name_p = ssr->auto_.name;
        break;
    case HLL_SSR_AUTO_SCOPED:
        *name_len_p = ssr->auto_scoped.name_len;
        *name_p = ssr->auto_scoped.name;
        break;
    case HLL_SSR_REG:
        *name_len_p = ssr->reg.name_len;
        *name_p = ssr->reg.name;
        break;
    case HLL_SSR_REG_RELATIVE:
        *name_len_p = ssr->reg_relative.name_len;
        *name_p = ssr->reg_relative.name;
        break;
    case HLL_SSR_TAG:
        *name_len_p = ssr->tag.name_len;
        *name_p = ssr->tag.name;
        break;
    case HLL_SSR_TAG2:
        *name_len_p = ssr->tag2.name_len;
        *name_p = ssr->tag2.name;
        if( *name_len_p & 0x80 ) {
            *name_len_p = ((*name_len_p & 0x7f) << 8) | *(unsigned_8 *)name_p++;
        }
        break;
    case HLL_SSR_TYPEDEF:
        *name_len_p = ssr->typedef_.name_len;
        *name_p = ssr->typedef_.name;
        break;
    case HLL_SSR_MEMBER:
        *name_len_p = ssr->member.name_len;
        *name_p = ssr->member.name;
        break;
    case HLL_SSR_BASED:
        *name_len_p = ssr->based.name_len;
        *name_p = ssr->based.name;
        break;
    case HLL_SSR_TABLE:
        *name_len_p = ssr->table.name_len;
        *name_p = ssr->table.name;
        break;
    case HLL_SSR_MAP:
        *name_len_p = ssr->map.name_len;
        *name_p = ssr->map.name;
        break;
    case HLL_SSR_BASED_MEMBER:
        *name_len_p = ssr->based_member.name_len;
        *name_p = ssr->based_member.name;
        break;

    case HLL_SSR_ARRAY_SYM:
    case HLL_SSR_CONSTANT:
    case HLL_SSR_WITH:
    case HLL_SSR_SKIP:
    case HLL_SSR_CHANGE_SEG:
    case HLL_SSR_CU_INFO:
    case HLL_SSR_CU_FUNC_NUM:
    case HLL_SSR_END:
        *name_len_p = 0;
        *name_p = "";
        break;
    default:
        hllConfused();
        return( DS_FAIL );
    }
    return( DS_OK );
}

/*
 * Gets the symbol index.
 */
static unsigned hllSymTypeIdx( imp_image_handle *ii, hll_ssr_all *p )
{
    unsigned    idx;

#define DEFTYPE( t ) if( idx == 0 ) idx = t;

    switch( p->common.code ) {
    case HLL_SSR_BEGIN:
    case HLL_SSR_PROC:
    case HLL_SSR_END:
    case HLL_SSR_AUTO:
    case HLL_SSR_STATIC:
    case HLL_SSR_TLS:
    case HLL_SSR_CODE_LABEL:
    case HLL_SSR_WITH:
    case HLL_SSR_REG:
    case HLL_SSR_CONSTANT:
    case HLL_SSR_ENTRY:
    case HLL_SSR_SKIP:
    case HLL_SSR_CHANGE_SEG:
    case HLL_SSR_TYPEDEF:
    case HLL_SSR_PUBLIC:
    case HLL_SSR_MEMBER:
    case HLL_SSR_BASED:
    case HLL_SSR_TAG:
    case HLL_SSR_TABLE:
    case HLL_SSR_MAP:
    case HLL_SSR_TAG2:
    case HLL_SSR_MEM_FUNC:
    case HLL_SSR_AUTO_SCOPED:
    case HLL_SSR_STATIC_SCOPED:
    case HLL_SSR_PROC2:
    case HLL_SSR_STATIC2:
    case HLL_SSR_REG_RELATIVE:
    case HLL_SSR_BASED_MEMBER:
    case HLL_SSR_ARRAY_SYM:
    case HLL_SSR_CU_INFO:
    case HLL_SSR_CU_FUNC_NUM:
    default:
        idx = 0;
        break;
    }

#if 0
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
        if( hllIsSegExecutable( ii, p->pub16.f.segment ) ) {
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
        if( hllIsSegExecutable( ii, p->label16.f.segment ) ) {
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
        if( hllIsSegExecutable( ii, p->pub32.f.segment ) ) {
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
        if( hllIsSegExecutable( ii, p->label32.f.segment ) ) {
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
        hllConfused();
        idx = 0;
    }
#endif
#undef DEFTYPE

    return( idx );
}

/*
 * Get the location of the given symbol.
 */
dip_status hllSymLocation( imp_image_handle *ii, imp_sym_handle *is,
                           location_context *lc, location_list *ll )
{
    address     addr = { 0 };
    void        *p;
    dip_status  ds;

    p = VMBlock( ii, is->handle, is->len );
    if( p == NULL ) {
        return( DS_FAIL );
    }

    if( is->type == HLL_SYM_TYPE_PUB ) {
        hll_public_all *pub = p;

        if( HLL_IS_LVL_32BIT( ii->format_lvl ) ) {
            addr.mach.segment = pub->hll.seg;
            addr.mach.offset  = pub->hll.offset;
        } else {
            addr.mach.segment = pub->cv3.seg;
            addr.mach.offset  = pub->cv3.offset;
        }

    } else if( is->type == HLL_SYM_TYPE_HLL_SSR ) {
        hll_ssr_all *ssr = p;

        switch( ssr->common.code ) {
        case HLL_SSR_BEGIN:
            addr.mach.offset = ssr->begin.offset;
            addr.mach.segment = is->segment;
            break;

        case HLL_SSR_PROC:
        case HLL_SSR_PROC2:
        case HLL_SSR_MEM_FUNC:
        case HLL_SSR_ENTRY:
            addr.mach.offset = ssr->proc.offset;
            addr.mach.segment = is->segment;
            break;

        case HLL_SSR_CODE_LABEL:
            addr.mach.offset = ssr->code_lable.offset;
            addr.mach.segment = is->segment;
            break;

        case HLL_SSR_PUBLIC:
            addr.mach.offset = ssr->public_.offset;
            addr.mach.segment = ssr->public_.seg;
            break;

        case HLL_SSR_STATIC:
        case HLL_SSR_STATIC2:
            addr.mach.offset = ssr->static_.offset;
            addr.mach.segment = ssr->static_.seg;
            break;

        case HLL_SSR_STATIC_SCOPED:
            addr.mach.offset = ssr->static_scoped.offset;
            addr.mach.segment = ssr->static_scoped.seg;
            break;

        case HLL_SSR_TABLE:
            addr.mach.offset = ssr->table.offset;
            addr.mach.segment = ssr->table.seg;
            break;

        case HLL_SSR_AUTO:
        case HLL_SSR_AUTO_SCOPED:
            ds = DCItemLocation( lc, CI_FRAME, ll );
            if( ds == DS_OK ) {
                hllLocationAdd( ll,
                                ssr->common.code == HLL_SSR_AUTO
                                ? ssr->auto_.offset * 8
                                : ssr->auto_scoped.offset * 8 );
            } else {
                DCStatus( ds );
            }
            return( ds );


        case HLL_SSR_REG:
        case HLL_SSR_REG_RELATIVE:
            {
                signed_32       offset = 0;
                unsigned_8      reg;

                if( ssr->common.code == HLL_SSR_REG ) {
                    offset = 0;
                    reg = ssr->reg.reg;
                } else {
                    offset = ssr->reg_relative.offset;
                    reg = ssr->reg_relative.reg;
                }

                ds = hllLocationOneReg( ii, reg, lc, ll );
                if( ds == DS_OK && offset != 0 ) {
                    location_list   tmp_ll;
                    addr48_off      off48;
                    hllLocationCreate( &tmp_ll, LT_INTERNAL, &off48 );
                    ds = DCAssignLocation( ll, &tmp_ll, sizeof( off48 ) );
                    if( ds == DS_OK ) {
                        ds = DCItemLocation( lc, CI_DEF_ADDR_SPACE, ll );
                    }
                    if( ds == DS_OK ) {
                        ll->e[0].u.addr.mach.offset = off48 + offset;
                    }
                }
            }
            return( ds );

        case HLL_SSR_TLS:
            hllConfused(); //Need example!
            return( DS_FAIL );

        default:
            hllConfused();
        case HLL_SSR_CONSTANT:
        case HLL_SSR_MEMBER:
        case HLL_SSR_BASED:
        case HLL_SSR_BASED_MEMBER:
        case HLL_SSR_ARRAY_SYM:
        case HLL_SSR_SKIP:
        case HLL_SSR_MAP:
        case HLL_SSR_TAG:
        case HLL_SSR_TAG2:
        case HLL_SSR_TYPEDEF:
        case HLL_SSR_WITH:
        case HLL_SSR_END:
        case HLL_SSR_CHANGE_SEG:
        case HLL_SSR_CU_INFO:
        case HLL_SSR_CU_FUNC_NUM:
            return( DS_FAIL );
        }

    } else {
        /* FIXME: CV3 */
        return( DS_FAIL );
    }

    /* (addresses only, the others return) */
    hllMapLogical( ii, &addr );
    hllLocationCreate( ll, LT_ADDR, &addr );
    return( DS_OK );


#if 0
    address             addr;
    s_all               *p;
    signed_32           disp;
    location_list       tmp_ll;
    union {
        addr32_off      off32;
        addr48_off      off48;
    }                   tmp;

    //NYI: parameters when at the start of a routine.
    if( is->containing_type != 0 ) {
        return( TypeSymGetAddr( ii, is, lc, ll ) );
    }
    p = VMBlock( ii, is->handle, is->len );
    if( p == NULL ) return( DS_FAIL );
    switch( p->common.code ) {
    case S_REGISTER:
        return( hllLocationOneReg( ii, p->register_.f.reg, lc, ll ) );
    case S_CONSTANT:
    case S_UDT:
    case S_COBOLUDT:
        return( SR_FAIL );
    case S_MANYREG:
        return( hllLocationManyReg( ii, p->manyreg.f.count, (unsigned_8 *)&p->manyreg + 1, lc, ll ) );
    case S_BPREL16:
        ds = DCItemLocation( lc, CI_FRAME, ll );
        if( ds != DS_OK ) {
            DCStatus( ds );
            return( ds );
        }
        hllLocationAdd( ll, p->bprel16.f.offset * 8 );
        break;
    case S_LDATA16:
    case S_GDATA16:
    case S_PUB16:
        addr.mach.offset = p->ldata16.f.offset;
        addr.mach.segment = p->ldata16.f.segment;
        hllMapLogical( ii, &addr );
        hllLocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_LPROC16:
    case S_GPROC16:
        addr.mach.offset = p->lproc16.f.offset;
        addr.mach.segment = p->lproc16.f.segment;
        hllMapLogical( ii, &addr );
        hllLocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_BLOCK16:
        addr.mach.offset = p->block16.f.offset;
        addr.mach.segment = p->block16.f.segment;
        hllMapLogical( ii, &addr );
        hllLocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_LABEL16:
        addr.mach.offset = p->label16.f.offset;
        addr.mach.segment = p->label16.f.segment;
        hllMapLogical( ii, &addr );
        hllLocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_REGREL16:
        disp = p->regrel16.f.offset;
        ds = hllLocationOneReg( ii, p->regrel16.f.reg, lc, ll );
        if( ds != DS_OK ) return( ds );
        hllLocationCreate( &tmp_ll, LT_INTERNAL, &tmp.off32 );
        ds = DCAssignLocation( ll, &tmp_ll, sizeof( addr32_off ) );
        if( ds != DS_OK ) return( ds );
        ds = DCItemLocation( lc, CI_DEF_ADDR_SPACE, ll );
        if( ds != DS_OK ) return( ds );
        ll->e[0].u.addr.mach.offset = tmp.off32 + disp;
        break;
    case S_BPREL32:
        ds = DCItemLocation( lc, CI_FRAME, ll );
        if( ds != DS_OK ) {
            DCStatus( ds );
            return( ds );
        }
        hllLocationAdd( ll, p->bprel32.f.offset * 8 );
        break;
    case S_LDATA32:
    case S_GDATA32:
    case S_PUB32:
        addr.mach.offset = p->ldata32.f.offset;
        addr.mach.segment = p->ldata32.f.segment;
        hllMapLogical( ii, &addr );
        hllLocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_LPROC32:
    case S_GPROC32:
        addr.mach.offset = p->lproc32.f.offset;
        addr.mach.segment = p->lproc32.f.segment;
        hllMapLogical( ii, &addr );
        hllLocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_BLOCK32:
        addr.mach.offset = p->block32.f.offset;
        addr.mach.segment = p->block32.f.segment;
        hllMapLogical( ii, &addr );
        hllLocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_LABEL32:
        addr.mach.offset = p->label32.f.offset;
        addr.mach.segment = p->label32.f.segment;
        hllMapLogical( ii, &addr );
        hllLocationCreate( ll, LT_ADDR, &addr );
        break;
    case S_REGREL32:
        disp = p->regrel32.f.offset;
        ds = hllLocationOneReg( ii, p->regrel32.f.reg, lc, ll );
        if( ds != DS_OK ) return( ds );
        hllLocationCreate( &tmp_ll, LT_INTERNAL, &tmp.off48 );
        ds = DCAssignLocation( ll, &tmp_ll, sizeof( addr48_off ) );
        if( ds != DS_OK ) return( ds );
        ds = DCItemLocation( lc, CI_DEF_ADDR_SPACE, ll );
        if( ds != DS_OK ) return( ds );
        ll->e[0].u.addr.mach.offset = tmp.off48 + disp;
        break;
    case S_LTHREAD32:
    case S_GTHREAD32:
        addr.mach.offset = p->lthread32.f.offset;
        addr.mach.segment = p->lthread32.f.segment;
        hllMapLogical( ii, &addr );
        hllLocationCreate( ll, LT_ADDR, &addr );
        break;
    default:
        hllConfused();
        return( DS_FAIL );
    }
    return( DS_OK );
#endif
    return( DS_FAIL );
}

typedef struct {
    hll_dir_entry       *hde;   /* The dir entry for the SSR sub-section. */
    hll_ssr             code;   /* The symbol scope record type.  */
    unsigned_32         scope;  /* The position of the hll_ssr_common, relative to hde. */
    address             start;  /* The start address of the symbol if applicable. */
    unsigned_16         len;    /* The length of the sSR. */
/*
    unsigned_32         parent;
    unsigned_32         end;
    unsigned_32         next; */
} scope_info;

static dip_status hllScopeFillIn( imp_image_handle *ii, hll_dir_entry *hde,
                                  unsigned_32 pos, unsigned_16 seg,
                                  hll_ssr_all *ssr, scope_info *scope )
{
    scope->hde   = hde;
    scope->code  = ssr->common.code;
    scope->scope = pos;
    scope->start = NilAddr;
    scope->len   = 0;
    //scope->next = scope->parent = scope->end = 0;

    switch( ssr->common.code ) {
    case HLL_SSR_PROC:
    case HLL_SSR_PROC2:
    case HLL_SSR_MEM_FUNC:
        scope->len = ssr->proc.len;
        scope->start.mach.offset = ssr->proc.offset;
        scope->start.mach.segment = seg;
        break;
    case HLL_SSR_BEGIN:
        scope->len = ssr->begin.len;
        scope->start.mach.offset = ssr->begin.offset;
        scope->start.mach.segment = seg;
        break;
    //case HLL_SSR_WITH:
    //    break;
    default:
        return( DS_FAIL );
    }
    hllMapLogical( ii, &scope->start );
    return( DS_OK );
}

#if 0
static dip_status ScopeFindFirst( imp_image_handle *ii, imp_mod_handle im,
                                address addr, scope_info *scope )
{
    virt_mem            chk;
    unsigned long       len;
    s_all               *p;
    scope_info          new;
    dip_status          ds;

    scope->cde = hllFindDirEntry( ii, im, sstAlignSym );
    if( scope->cde == NULL ) return( DS_FAIL );
    chk = scope->cde->lfo + sizeof( unsigned_32 );
    len = scope->cde->cb - sizeof( unsigned_32 );
    for( ;; ) {
        if( len == 0 ) return( DS_FAIL );
        p = VMBlock( ii, chk, sizeof( s_ssearch ) );
        if( p == NULL ) return( DS_ERR|DS_FAIL );
        if( p->common.code == S_SSEARCH ) {
            scope->start.mach.segment = p->ssearch.f.segment;
            scope->start.mach.offset  = 0;
            hllMapLogical( ii, &scope->start );
            if( DCSameAddrSpace( scope->start, addr ) == DS_OK ) {
                chk = p->ssearch.f.sym_off + scope->cde->lfo;
                break;
            }
        }
        chk += p->common.length + sizeof( p->common.length );
        len -= p->common.length + sizeof( p->common.length );
    }
    /* found first scope block, now find correct offset */
    for( ;; ) {
        ds = hllScopeFillIn( ii, chk, scope, NULL );
        if( ds != DS_OK ) return( DS_ERR|ds );
        if( addr.mach.offset >= scope->start.mach.offset
         && addr.mach.offset < (scope->start.mach.offset + scope->len) ) {
            break;
        }
        if( scope->next == 0 ) return( DS_FAIL );
        chk = scope->next;
    }
    /* found enclosing scope block, now find smallest one */
    chk = scope->scope;
    new.cde = scope->cde;
    for( ;; ) {
        ds = hllScopeFillIn( ii, chk, &new, NULL );
        if( ds & DS_ERR ) return( ds );
        if( ds == DS_OK ) {
            /* new scope */
            if( !(addr.mach.offset >= new.start.mach.offset
             && addr.mach.offset <  (new.start.mach.offset + new.len)) ) {
                /* out of range */
                chk = new.end;
            } else if( !(new.start.mach.offset >= scope->start.mach.offset
                   && (new.start.mach.offset+new.len) <= (scope->start.mach.offset+scope->len)) ) {
                /* not smaller */
                chk = new.end;
            } else {
                /* inner scope */
                *scope = new;
            }
        } else if( p->common.code == S_END ) {
            /* all done */
            return( DS_OK );
        }
        p = VMBlock( ii, chk, sizeof( p->common ) );
        if( p == NULL ) return( DS_ERR|DS_FAIL );
        chk += p->common.length + sizeof( p->common.length );
    }
}

static dip_status ScopeFindNext( imp_image_handle *ii, scope_info *scope )
{
    if( scope->parent == 0 ) return( DS_FAIL );
    return( hllScopeFillIn( ii, scope->parent, scope, NULL ) );
}
#endif


/*
 * Process one global symbol.
 * If 'scope' is a non-zero, symbols starting a scope will be skipped.
 */
static walk_result hllScopeOneModSymbol( imp_image_handle *ii, hll_dir_entry *hde,
                                         scope_info *scope, IMP_SYM_WKR *wk,
                                         void *d, unsigned_32 *posp,
                                         unsigned_16 *depthp, unsigned_16 *segp )
{
    walk_result         wr = WR_CONTINUE;
    hll_ssr_all         *ssr;
    unsigned_32         pos = *posp;
    unsigned_16         len;
    scope_info          new_scope;
    dip_status          ds;
    unsigned_16         depth = *depthp;

    ssr = VMSsRecord( ii, hde, pos, posp, &len );
    if( !ssr ) {
        return( WR_FAIL );
    }
    ds = hllScopeFillIn( ii, hde, pos, *segp, ssr, &new_scope );

    switch( ssr->common.code ) {
    case HLL_SSR_CHANGE_SEG:
        *segp = ssr->change_seg.seg;
        break;

    case HLL_SSR_PROC:
    case HLL_SSR_PROC2:
    case HLL_SSR_MEM_FUNC:
    case HLL_SSR_ENTRY:
        ++*depthp;
    case HLL_SSR_REG:
    case HLL_SSR_REG_RELATIVE:
    case HLL_SSR_TABLE:
    case HLL_SSR_CONSTANT:
    case HLL_SSR_TLS:
    case HLL_SSR_CODE_LABEL:
    case HLL_SSR_STATIC2:
    case HLL_SSR_STATIC:
    case HLL_SSR_STATIC_SCOPED:
        if( depth > 0 ) {
            break;
        }
    case HLL_SSR_PUBLIC:
        //don't know about the next ones...
        if( ds == DS_OK && scope != NULL ) {
            /*
                starting a new scope and not doing file scope
                symbols -- skip scope start symbol
            */
            wr = WR_CONTINUE;
        } else {
            imp_sym_handle is;

            is.im = hde->iMod;
            ds = hllSymFillIn( ii, &is, *segp,
                               *posp - len + hde->lfo, len );
            if( ds != DS_OK ) {
                return( WR_FAIL );
            }
            wr = wk( ii, SWI_SYMBOL, &is, d );
        }
        break;

    case HLL_SSR_WITH:
    case HLL_SSR_BEGIN:
        ++*depthp;
        break;
    case HLL_SSR_END:
        if( *depthp > 0) {
            --*depthp;
        }
        break;

    default:
        hllConfused();
    case HLL_SSR_AUTO:
    case HLL_SSR_AUTO_SCOPED:
    case HLL_SSR_TAG:
    case HLL_SSR_TAG2:
    case HLL_SSR_TYPEDEF:
    case HLL_SSR_SKIP:
    case HLL_SSR_MEMBER: //??
    case HLL_SSR_BASED: //??
    case HLL_SSR_MAP:
    case HLL_SSR_BASED_MEMBER: //??
    case HLL_SSR_ARRAY_SYM: //??
    case HLL_SSR_CU_INFO:
    case HLL_SSR_CU_FUNC_NUM:
        /* not interested in these */
        break;
    }
    return( wr );
}

#if 0
static walk_result ScopeWalkOne( imp_image_handle *ii, scope_info *scope,
                                IMP_SYM_WKR *wk, void *d )
{
    walk_result         wr;
    s_all               *p;
    virt_mem            curr;

    curr = scope->scope;
    p = VMBlock( ii, curr, sizeof( p->common ) );
    if( p == NULL ) return( WR_FAIL );
    /* skip over first scope start symbol */
    curr += p->common.length + sizeof( p->common.length );
    for( ;; ) {
        p = VMBlock( ii, curr, sizeof( p->common ) );
        if( p == NULL ) return( WR_FAIL );
        if( p->common.code == S_END ) return( WR_CONTINUE );
        wr = hllScopeOneSymbol( ii, scope->cde, scope, wk, d, &curr );
        if( wr != WR_CONTINUE ) return( wr );
    }
}

static walk_result ScopeWalkClass( imp_image_handle *ii, scope_info *scope,
                                IMP_SYM_WKR *wk, void *d )
{
    dip_status          ds;
    imp_type_handle     it;
    s_all               *p;
    imp_sym_handle      is;

    p = VMBlock( ii, scope->scope, sizeof( *p ) );
    if( p == NULL ) return( WR_FAIL );
    ds = hllTypeIndexFillIn( ii, hllSymTypeIdx( ii, p ), &it );
    if( ds & DS_ERR ) return( WR_FAIL );
    if( ds != DS_OK ) return( WR_CONTINUE );
    ds = TypeMemberFuncInfo( ii, &it, &it, NULL, NULL );
    if( ds & DS_ERR ) return( WR_FAIL );
    if( ds != DS_OK ) return( WR_CONTINUE );
    return( TypeSymWalkList( ii, &it, wk, &is, d ) );
}

static walk_result ScopeWalkAll( imp_image_handle *ii, imp_mod_handle im,
                        address addr, IMP_SYM_WKR *wk, void *d )
{
    dip_status          ds;
    walk_result         wr;
    scope_info          scope;

    ds = ScopeFindFirst( ii, im, addr, &scope );
    if( ds & DS_ERR ) return( WR_FAIL );
    if( ds != DS_OK ) return( WR_CONTINUE );
    for( ;; ) {
        wr = ScopeWalkOne( ii, &scope, wk, d );
        if( wr != WR_CONTINUE ) return( wr );
        switch( scope.code ) {
        case S_LPROC16:
        case S_GPROC16:
        case S_LPROC32:
        case S_GPROC32:
            wr = ScopeWalkClass( ii, &scope, wk, d );
            if( wr != WR_CONTINUE ) return( wr );
            break;
        }
        ds = ScopeFindNext( ii, &scope );
        if( ds & DS_ERR ) return( WR_FAIL );
        if( ds != DS_OK ) return( WR_CONTINUE );
    }
}
#endif

static walk_result hllScopeWalkFile( imp_image_handle *ii,
                                     hll_dir_entry *hde,
                                     IMP_SYM_WKR *wk, void *d )
{
    unsigned_32     pos = 0;
    unsigned_16     seg = 0;
    unsigned_16     depth = 0;
    walk_result     wr = WR_CONTINUE;

    if( hllGetModStyle( ii, hde->iMod ) >= HLL_STYLE_HL01 ) {
        while( pos < hde->cb && wr == WR_CONTINUE ) {
            wr = hllScopeOneModSymbol( ii, hde, NULL, wk, d, &pos, &depth, &seg );
        }
    } else {
        /* FIXME: CV3 */
    }

    return( wr );
}

#if 0
static search_result TableSearchForAddr( imp_image_handle *ii,
                        address a, imp_sym_handle *is, addr_off *best_off, unsigned tbl_type )
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

    cde = hllFindDirEntry( ii, IMH_GBL, tbl_type );
    if( cde == NULL ) return( SR_NONE );
    hdr = VMBlock( ii, cde->lfo, sizeof( *hdr ) );
    if( hdr == NULL ) return( SR_FAIL );
    switch( hdr->addrhash ) {
    case 12:
        hash_base = cde->lfo + hdr->cbSymbol + hdr->cbSymHash + sizeof( *hdr );
        p = VMBlock( ii, hash_base, sizeof( unsigned_16 ) );
        if( p == NULL ) return( SR_FAIL );
        num_segs = *(unsigned_16 *)p;
        hash_base += 2 * sizeof( unsigned_16 );
        best.base = 0;
        best.off = *best_off;
        i = 0;
        for( ;; ) {
            if( i >= num_segs ) break;
            chk.mach.segment = i + 1;
            chk.mach.offset = 0;
            hllMapLogical( ii, &chk );
            if( DCSameAddrSpace( chk, a ) != DS_OK ) goto next_seg;
            p = VMBlock( ii, hash_base + i * sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
            if( p == NULL ) return( SR_FAIL );
            curr.base = *(unsigned_32 *)p;
            base = hash_base + num_segs * sizeof( unsigned_32 );
            p = VMBlock( ii, base + i * sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
            if( p == NULL ) return( SR_FAIL );
            offset_count = *(unsigned_32 *)p;
            if( offset_count == 0 ) goto next_seg;
            curr.base += base + num_segs * sizeof( unsigned_32 );
            //NYI: offsets are sorted, so we can binary search this sucker
            count = 0;
            for( ;; ) {
                if( count >= offset_count ) break;
                p = VMBlock( ii, curr.base + sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
                if( p == NULL ) return( SR_FAIL );
                new_off = *(unsigned_32 *)p + chk.mach.offset;
                if( new_off >= a.mach.offset ) break;
                curr.off = new_off;
                curr.base += 2 * sizeof( unsigned_32 );
                ++count;
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
next_seg:
            ++i;
        }
        if( best.base == 0 ) return( SR_NONE );
        p = VMBlock( ii, best.base, sizeof( unsigned_32 ) );
        if( p == NULL ) return( SR_FAIL );
        if( hllSymFillIn( ii, is, *(unsigned_32 *)p + cde->lfo + sizeof( *hdr ) ) != DS_OK ) {
            return( SR_FAIL );
        }
        *best_off = best.off;
        return( (best.off == a.mach.offset) ? SR_EXACT : SR_CLOSEST );
    default:
        //NYI: what to do when don't have hash function? */
        return( SR_NONE );
    }
}

static unsigned long CalcHash( const char *name, unsigned len )
{
    unsigned_32         end;
    unsigned_32         sum;
    unsigned            i;

#define B_toupper( b )  ((b) & 0xdf)
#define D_toupper( d )  ((d) & 0xdfdfdfdf)

    end = 0;
    for( i = len & 0x3; i != 0; --i ) {
        end |= B_toupper( name[ len - 1 ] );
        end <<= 8;
        len -= 1;
    }
    len /= 4;
    sum = 0;
    for( i = 0; i < len; ++i ) {
        sum ^= D_toupper( *(unsigned_32 *)name );
        sum = _lrotl( sum, 4 );
        name += 4;
    }
    return( sum ^ end );
}

typedef search_result   SEARCH_CREATOR( imp_image_handle *, s_all *, imp_sym_handle *, void * );

static search_result TableSearchForName( imp_image_handle *ii,
                int case_sense, const char *name, unsigned name_len,
                unsigned long hash, imp_sym_handle *is,
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
    unsigned                    curr_len;
    s_all                       *sp;
    search_result               sr;

    cde = hllFindDirEntry( ii, IMH_GBL, tbl_type );
    if( cde == NULL ) return( SR_NONE );
    hdr = VMBlock( ii, cde->lfo, sizeof( *hdr ) );
    if( hdr == NULL ) return( SR_FAIL );
    switch( hdr->symhash ) {
    case 10:
        hash_base = cde->lfo + hdr->cbSymbol + sizeof( *hdr );
        p = VMBlock( ii, hash_base, sizeof( unsigned_16 ) );
        if( p == NULL ) return( SR_FAIL );
        hash_buckets = *(unsigned_16 *)p;
        i = hash % hash_buckets;
        hash_base += 2 * sizeof( unsigned_16 );
        p = VMBlock( ii, hash_base + i * sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
        if( p == NULL ) return( SR_FAIL );
        sym_base = *(unsigned_32 *)p;
        base = hash_base + hash_buckets * sizeof( unsigned_32 );
        p = VMBlock( ii, base + i * sizeof( unsigned_32 ), sizeof( unsigned_32 ) );
        if( p == NULL ) return( SR_FAIL );
        sym_base += base + hash_buckets * sizeof( unsigned_32 );
        sr = SR_NONE;
        for( count = *(unsigned_32 *)p; count != 0; sym_base += 2*sizeof(unsigned_32), --count ) {
            p = VMBlock( ii, sym_base, 2 * sizeof( unsigned_32 ) );
            if( p == NULL ) return( SR_FAIL );
            if( ((unsigned_32 *)p)[1] != hash ) continue;
            if( hllSymFillIn( ii, is, *(unsigned_32 *)p + cde->lfo + sizeof( *hdr ) ) != DS_OK ) {
                return( SR_FAIL );
            }
            if( hllSymGetName( ii, is, &curr, &curr_len, &sp ) != DS_OK ) {
                return( SR_FAIL );
            }
            if( curr_len != name_len ) continue;
            if( case_sense ) {
                if( memcmp( name, curr, curr_len ) != 0 ) continue;
            } else {
                if( memicmp( name, curr, curr_len ) != 0 ) continue;
            }
            /* Got one! */
            switch( create( ii, sp, is, d ) ) {
            case SR_FAIL:
                return( SR_FAIL );
            case SR_CLOSEST:
                /* means we found one, but keep on going */
                sr = SR_EXACT;
                break;
            case SR_EXACT:
                return( SR_EXACT );
            }
        }
        return( sr );
    default:
        //NYI: What to do if don't have hash table
        return( SR_NONE );
    }
}

struct match_data {
    unsigned    idx;
};

static search_result MatchSym( imp_image_handle *ii, s_all *p,
                        imp_sym_handle *is, void *d )
{
    struct match_data   *md = d;

    is = is;
    if( md->idx != hllSymTypeIdx( ii, p ) ) return( SR_NONE );
    return( SR_EXACT );
}

#endif


dip_status hllSymFindMatchingSym( imp_image_handle *ii,
                                  const char *name, unsigned len, unsigned idx,
                                  imp_sym_handle *is )
{
#if 0
    unsigned long       hash;
    search_result       sr;
    struct match_data   data;

    data.idx = idx;
    hash = CalcHash( name, len );
    sr = TableSearchForName( ii, 1, name, len, hash, is, MatchSym, &data,
                sstStaticSym );
    switch( sr ) {
    case SR_FAIL:       return( DS_ERR|DS_FAIL );
    case SR_EXACT:      return( DS_OK );
    }
    sr = TableSearchForName( ii, 1, name, len, hash, is, MatchSym, &data,
                sstGlobalSym );
    switch( sr ) {
    case SR_FAIL:       return( DS_ERR|DS_FAIL );
    case SR_EXACT:      return( DS_OK );
    }
#endif
    return( DS_FAIL );
}

/*
 * Walks the publics subsection for a module.
 */
static walk_result hllWalkModulePublics( imp_image_handle *ii,
                                         hll_dir_entry *hde,  IMP_SYM_WKR *wk,
                                         imp_sym_handle *is, void *d)
{
    unsigned        off_name_len;
    unsigned_32     pos;

    /*
     * Iterate the publics.
     */
    if( HLL_IS_LVL_32BIT( ii->format_lvl ) ) {
        off_name_len = offsetof( hll_public, name_len );
    } else {
        off_name_len = offsetof( cv3_public_16, name_len );
    }
    pos = 0;
    while( pos < hde->cb ) {
        hll_public_all  *pub;
        unsigned_8      name_len;
        unsigned_16     len;
        walk_result     wr;

        /* get the record length and make sure it's all valid. */
        if( !VMSsGetU8( ii, hde, pos + off_name_len, &name_len ) ) {
            return( WR_FAIL );
        }
        len = off_name_len + name_len + 1;
        pub = VMSsBlock( ii, hde, pos, len );
        if( pub == NULL ) {
            return( WR_FAIL );
        }

        /* construct a symbol handle and feed it to the walker function. */
        is->type = HLL_SYM_TYPE_PUB;
        is->handle = pos + hde->lfo;
        is->len = len;
        is->im = hde->iMod;
        is->containing_type = 0;
        is->adjustor_type = 0;
        is->adjustor_offset = 0;
        is->mfunc_idx = 0;

        wr = wk( ii, SWI_SYMBOL, is, d );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }

        /* next */
        pos += len;
    }
    return( WR_CONTINUE );
}

struct glue_info {
    IMP_SYM_WKR         *wk;
    void                *d;
    imp_sym_handle      *is;
};

static walk_result WalkGlue( imp_image_handle *ii, sym_walk_info swi,
                                imp_sym_handle *is, void *d )
{
    struct glue_info *gd = d;

    if( is == NULL ) {
        return( gd->wk( ii, swi, NULL, gd->d ) );
    }
    *gd->is = *is;
    return( gd->wk( ii, swi, gd->is, gd->d ) );
}

/*
 * Module walker callback which will in turn walk the global
 * symbols of a module.
 */
static walk_result hllWalkModuleGlobals( imp_image_handle *ii,
                                         hll_dir_entry *hde, void *d )
{
    struct glue_info    *gd = d;
    walk_result         wr = WR_CONTINUE;
    hll_dir_entry       *hde_symbols = hllFindDirEntry( ii, hde->iMod, hll_sstSymbols );

    if( hde_symbols ) {
        wr = hllScopeWalkFile( ii, hde_symbols, WalkGlue, gd );
    } else {
        hde_symbols = hllFindDirEntry( ii, hde->iMod, hll_sstPublics );
        if( hde_symbols ) {
            wr = hllWalkModulePublics( ii, hde_symbols, gd->wk, gd->is, gd->d );
        }
    }
    return( wr );
}


/*
 * Walk the list of symbols.
 *
 * There can be a number of sources of symbol lists. What kind you're
 * walking is determined by the 'ss' parameter. It can take on the
 * following values:
 *
 *  SS_MODULE:
 *          The 'source' is a pointer to an imp_mod_handle. If
 *          the *(imp_mod_handle *)source is IMH_NOMOD, The list is
 *          all the module scope/global symbols in the image, otherwise
 *          it is the list of module scope/global symbols in the
 *          indicated module.
 *
 *  SS_SCOPED:
 *          The 'source' is a pointer at an address. Considering the
 *          point of execution to be *(address *)source, the list
 *          is all the lexically scoped symbols visible from that
 *          point.
 *
 *  SS_TYPE:
 *          The 'source' is a pointer to an imp_type_handle. If
 *          *(imp_type_handle *)source represents an enumerated
 *          type, the list is all the constant symbols of the
 *          enumeration. If the type handle represents a structure
 *          type, the list is all the field names in the structure.
 *
 *          When walking structures with a inherited classes, there is
 *          a small trick. Just before starting to walk the fields
 *          of an inherited class, the DIP should do a:
 *
 *                  wk( ii, SWI_INHERIT_START, NULL, d )
 *
 *          This indicates to client that an inherited field list
 *          is about to be started. If the client wishes the DIP to
 *          actually walk the inherited list, it will return WR_CONTINUE.
 *          If it wants you to skip the inherited fields it will return
 *          WR_STOP. You should continue with the reminder of the fields
 *          in the current structure that you're walking.
 *          When you come to the end of the list of members for an
 *          inherited class do a:
 *
 *                  wk( ii, SWI_INHERIT_END, NULL, d )
 *
 *  SS_BLOCK:
 *          The 'source' is a pointer to a scope_block structure.
 *          Walk all the symbols in that lexical block _only_.
 *
 *  SS_SCOPESYM:
 *          The 'source' is a pointer to an imp_sym_handle.
 *          This is a request to walk all the symbols in the
 *          scope identified by the imp_sym_handle. For example, if
 *          the imp_sym_handle is of type SK_NAMESPACE, walk all the
 *          symbols contained in that namespace.
 *
 *  PSEUDO-CODE:
 *
 *  for( each symbol in the list ) {
 *      if( starting new inherited base class ) {
 *          if( wk( ii, SWI_INHERIT_START, NULL, d ) != WR_CONTINUE ) {
 *              skip it and continue with next field in current class
 *          }
 *      } else if( ending list of inherited base class ) {
 *           wk( ii, SWI_INHERIT_END, NULL, d );
 *      } else {
 *          *is = fill in symbol handle information;
 *          wr = wk( ii, SWI_SYMBOL, is, d );
 *          if( wr != WR_CONTINUE ) return( wr );
 *      }
 *  }
 */
walk_result hllWalkSymList( imp_image_handle *ii, symbol_source ss,
                            void *source, IMP_SYM_WKR *wk,
                            imp_sym_handle *is, void *d )
{
    struct glue_info    glue;
    imp_mod_handle      im;
    //scope_block         *sc_block;
    //s_all               *p;
    //scope_info          sc_info;
    //dip_status          ds;
    walk_result         wr;
    //imp_type_handle     it;

    glue.wk = wk;
    glue.is = is;
    glue.d  = d;
    switch( ss ) {
        /*
         * The 'source' is a pointer to an imp_mod_handle. If the
         * '*(imp_mod_handle *)source' is IMH_NOMOD, the list is all the module
         * scope & global symbols in the image, otherwise it is the list of
         * module scope & global symbols in the indicated module.
         */
    case SS_MODULE:
        im = *(imp_mod_handle *)source;
        if( im == IMH_NOMOD ) {
            wr = hllWalkDirList( ii, hll_sstModule, &hllWalkModuleGlobals, &glue );
        } else  {
            hll_dir_entry *hde = hllFindDirEntry( ii, im, hll_sstModule );
            if( hde ) {
                wr = hllWalkModuleGlobals( ii, hde, &glue );
            } else {
                wr = WR_CONTINUE;
            }
        }
        return( wr );

#if 0
    case SS_SCOPED:
        if( ImpAddrMod( ii, *(address *)source, &im ) == SR_NONE ) {
            return( WR_CONTINUE );
        }
        return( ScopeWalkAll( ii, im, *(address *)source, &WalkGlue, &glue ) );
    case SS_BLOCK:
        sc_block = source;
        if( ImpAddrMod( ii, sc_block->start, &im ) == SR_NONE ) {
            return( WR_CONTINUE );
        }
        sc_info.cde = hllFindDirEntry( ii, im, sstAlignSym );
        if( sc_info.cde == NULL ) return( WR_FAIL );
        ds = hllScopeFillIn( ii, sc_block->unique & SCOPE_UNIQUE_MASK,
                                &sc_info, &p );
        if( ds & DS_ERR ) return( WR_FAIL );
        if( ds != DS_OK ) return( WR_CONTINUE );
        if( sc_block->unique & SCOPE_CLASS_FLAG ) {
            /* Walk the member function class scope */
            ds = hllTypeIndexFillIn( ii, hllSymTypeIdx( ii, p ), &it );
            if( ds & DS_ERR ) return( WR_FAIL );
            if( ds != DS_OK ) return( WR_CONTINUE );
            ds = TypeMemberFuncInfo( ii, &it, &it, NULL, NULL );
            if( ds & DS_ERR ) return( WR_FAIL );
            if( ds != DS_OK ) return( WR_CONTINUE );
            return( TypeSymWalkList( ii, &it, wk, is, d ) );
        } else {
            return( ScopeWalkOne( ii, &sc_info, WalkGlue, &glue ) );
        }
    case SS_TYPE:
        return( TypeSymWalkList( ii, (imp_type_handle *)source, wk, is, d ) );
#endif
    }
    return( WR_FAIL );
}

/*
 * Walk a symbol list.
 */
walk_result DIGENTRY DIPImpWalkSymList( imp_image_handle *ii, symbol_source ss,
                                        void *source, IMP_SYM_WKR *wk,
                                        imp_sym_handle *is, void *d )
{
    return( hllWalkSymList( ii, ss, source, wk, is, d ) );
}

/*
 * Walk a symbol list, new api.
 */
walk_result DIGENTRY DIPImpWalkSymListEx( imp_image_handle *ii,
                                          symbol_source ss, void *source,
                                          IMP_SYM_WKR *wk, imp_sym_handle *is,
                                          location_context *lc, void *d )
{
    lc=lc;
    return( hllWalkSymList( ii, ss, source, wk, is, d ) );
}

/*
 * Get the module of a symbol.
 */
imp_mod_handle DIGENTRY DIPImpSymMod( imp_image_handle *ii, imp_sym_handle *is )
{
    ii = ii;
    return( is->im );
}

/*
 * Given the imp_sym_handle, copy the name of the symbol into 'buff'.
 *
 * Do not copy more than 'buff_size' - 1 characters into the buffer and
 * append a trailing '\0' character. Return the real length
 * of the symbol name (not including the trailing '\0' character) even
 * if you had to truncate it to fit it into the buffer. If something
 * went wrong and you can't get the symbol name, call DCStatus and
 * return zero. NOTE: the client might pass in zero for 'buff_size'. In that
 * case, just return the length of the symbol name and do not attempt
 * to put anything into the buffer.
 * The 'sn' parameter indicates what type of symbol name the client
 * is interested in. It can have the following values:
 *
 * SN_SOURCE:
 *         The name of the symbol as it appears in the source code.
 *
 * SN_OBJECT:
 *         The name of the symbol as it appeared to the linker.
 *
 * SN_DEMANGLED:
 *         C++ names, with full typing (essentially it looks like
 *         a function prototype). If the symbol is not a C++ symbol
 *         (not mangled), return zero for the length.
 *
 * SN_EXPRESSION:
 *         Return whatever character string is necessary such that
 *         when scanned in an expression, the symbol handle can
 *         be reconstructed. Deprecated - never used.
 */
static unsigned hllSymName( imp_image_handle *ii, imp_sym_handle *is,
                            location_context *lc, symbol_name sn,
                            char *buff, unsigned buff_size )
{
    const char  *name = NULL;
    unsigned    name_len;

    if( is->type == HLL_SYM_TYPE_PUB ) {
        unsigned off_name;

        if( sn != SN_OBJECT && sn != SN_DEMANGLED ) {
            return( 0 );
        }

        /* get the name */
        if( HLL_IS_LVL_32BIT( ii->format_lvl ) ) {
            off_name = offsetof( hll_public, name_len ) + 1;
        } else {
            off_name = offsetof( cv3_public_16, name_len ) + 1;
        }
        name_len = is->len - off_name;
        name = VMBlock( ii, is->handle + off_name, name_len);

        /* demangle... */
        if( sn == SN_DEMANGLED && name != NULL ) {
            if( __is_mangled( name, name_len ) ) {
                return( __demangle_l( name, name_len, buff, buff_size ) );
            }
            return( 0 );
        }
    } else if( is->type == HLL_SYM_TYPE_HLL_SSR ) {
        //location_list       ll;
        //dip_status          ds;
        //imp_sym_handle      global_ish;
        //addr_off            dummy_off;
        //search_result       sr;

        switch( sn ) {
        case SN_EXPRESSION:
            return( 0 );
/*
        case SN_OBJECT:
        case SN_DEMANGLED:
            ds = hllSymLocation( ii, is, lc, &ll );
            if( ds != DS_OK ) break;
            if( ll.num != 1 ) break;
            if( ll.e[0].type != LT_ADDR ) break;
            dummy_off = 0;
            sr = TableSearchForAddr( ii, ll.e[0].u.addr, &global_ish,
                                &dummy_off, sstGlobalPub );
            if( sr != SR_EXACT ) break;
            if( hllSymGetName( ii, &global_ish, &name, &len, NULL ) != DS_OK ) break;
            if( sn == SN_OBJECT ) {
                return( hllNameCopy( buff, name, buff_size, len ) );
            }
            if( !__is_mangled( name, len ) ) return( 0 );
            return( __demangle_l( name, len, buff, buff_size ) );
*/
        case SN_SOURCE:
            if( hllSymGetName( ii, is, &name, &name_len ) != DS_OK ) {
                return( 0 );
            }
        default:
            //hllConfused();
            break;
        }

        if( sn == SN_DEMANGLED ) {
            return( 0 );
        }
    }

    if( name == NULL ) {
        return( 0 );
    }
    return( hllNameCopy( buff, name, buff_size, name_len ) );
}

/*
 * Get the symbol name.
 */
unsigned DIGENTRY DIPImpSymName( imp_image_handle *ii, imp_sym_handle *is,
                                 location_context *lc, symbol_name sn,
                                 char *buff, unsigned buff_size )
{
    return( hllSymName( ii, is, lc, sn, buff, buff_size ) );
}

/*
 * Get the type of the given symbol.
 */
dip_status hllSymType( imp_image_handle *ii, imp_sym_handle *is, imp_type_handle *it )
{
    unsigned    type_idx;
    void        *p;

    if( is->containing_type != 0 ) {
        return( hllTypeSymGetType( ii, is, it ) );
    }

    /*
     * Load the record, get the type index and create a type handle from that
     */
    p = VMBlock( ii, is->handle, is->len );
    if( p == NULL ) {
        return( DS_FAIL );
    }
    if( is->type == HLL_SYM_TYPE_PUB ) {
        hll_public_all *pub = p;
        if( HLL_IS_LVL_32BIT( ii->format_lvl ) ) {
            type_idx = pub->hll.type;
        } else {
            type_idx = pub->cv3.type;
        }

    } else if( is->type == HLL_SYM_TYPE_HLL_SSR ) {
        type_idx = hllSymTypeIdx( ii, p );
    } else {
        type_idx = 0; //TODO hllSymTypeIdxCV3( ii, p );
    }

    return( hllTypeIndexFillIn( ii, type_idx, it ) );
}

/*
 * Get the type of the given symbol.
 */
dip_status DIGENTRY DIPImpSymType( imp_image_handle *ii, imp_sym_handle *is,
                                   imp_type_handle *it )
{
    return( hllSymType( ii, is, it ) );
}

/*
 * Get the location of the given symbol.
 */
dip_status DIGENTRY DIPImpSymLocation( imp_image_handle *ii, imp_sym_handle *is,
                                       location_context *lc, location_list *ll )
{
    return( hllSymLocation( ii, is, lc, ll ) );
}


/*
 * Copy the value of a constant symbol into 'buff'.
 */
dip_status hllSymValue( imp_image_handle *ii, imp_sym_handle *is,
                        location_context *lc, void *buff )
{
    void    *p;

    /* Doesn't apply to publics. */
    if( is->type == HLL_SYM_TYPE_PUB ) {
        return( DS_FAIL );
    }

    p = VMBlock( ii, is->handle, is->len );
    if( p != NULL ) {
        if( is->type == HLL_SYM_TYPE_HLL_SSR ) {
            hll_ssr_all *ssr = p;
            switch( ssr->common.code ) {
            case HLL_SSR_CONSTANT:
                /* FIXME */
                break;

            }
        } else {
            /* FIXME: CV3 */
        }
    }

    return( DS_FAIL );


#if 0
    s_all               *p;
    numeric_leaf        val;
    dip_status          ds;
    imp_type_handle     it;
    dip_type_info       ti;

    if( is->containing_type != 0 ) {
        return( TypeSymGetValue( ii, is, lc, buff ) );
    }
    p = VMBlock( ii, is->handle, is->len );
    if( p == NULL ) return( DS_FAIL );
    switch( p->common.code ) {
    case S_CONSTANT:
        GetNumLeaf( (unsigned_8 *)p + sizeof( s_constant ), &val );
        memcpy( buff, val.valp, val.size );
        ds = hllTypeIndexFillIn( ii, hllSymTypeIdx( ii, p ), &it );
        if( ds != DS_OK ) return( ds );
        ds = hllTypeInfo( ii, &it, lc, &ti );
        if( ds != DS_OK ) return( ds );
        memset( (unsigned_8 *)buff + val.size, 0, ti.size - val.size );
        return( DS_OK );
    }
    return( DS_FAIL );
#endif
}

/*
 * Copy the value of a constant symbol into 'buff'. You can get the
 * size required by doing a SymType followed by a TypeInfo.
 */
dip_status DIGENTRY DIPImpSymValue( imp_image_handle *ii, imp_sym_handle *is,
                                    location_context *lc, void *buff )
{
    return( hllSymValue( ii, is, lc, buff ) );
}

/*
 * Get some generic information about a symbol.
 */
dip_status DIGENTRY DIPImpSymInfo( imp_image_handle *ii, imp_sym_handle *is,
                                   location_context *lc, sym_info *si )
{
    void    *p;

    memset( si, 0, sizeof( *si ) );

    if( is->containing_type != 0 ) {
        return( hllTypeSymGetInfo( ii, is, lc, si ) );
    }

    /* get the record. */
    p = VMBlock( ii, is->handle, is->len );
    if( p == NULL ) {
        return( DS_FAIL );
    }

    if( is->type == HLL_SYM_TYPE_PUB ) {
        hll_public_all  *pub = p;
        unsigned_16     seg;

        seg = HLL_IS_LVL_32BIT( ii->format_lvl )
            ? pub->hll.seg : pub->cv3.seg;
        si->kind = hllIsSegExecutable( ii, seg )
                 ? SK_CODE : SK_DATA;
        si->is_global = 1;
    } else if( is->type == HLL_SYM_TYPE_HLL_SSR ) {
        hll_ssr_all     *ssr = p;

        switch( ssr->common.code ) {
        case HLL_SSR_PUBLIC: /* never seen this.. */
            si->kind = hllIsSegExecutable( ii, ssr->public_.type )
                     ? SK_CODE : SK_DATA;
            si->is_global = 1;
            break;

        case HLL_SSR_ENTRY: //??
        case HLL_SSR_CODE_LABEL:
            si->kind = SK_CODE;
            break;

        case HLL_SSR_TAG:
        case HLL_SSR_TAG2:
        case HLL_SSR_TYPEDEF:
            si->kind = SK_TYPE;
            break;

        case HLL_SSR_PROC2:
        case HLL_SSR_PROC:
        case HLL_SSR_MEM_FUNC:
            si->kind = SK_PROCEDURE;
            si->rtn_far = !!(ssr->proc.flags & HLL_SSR_PROC_FAR);
            si->ret_addr_offset = ssr->proc.flags & HLL_SSR_PROC_32BIT
                                ? sizeof( unsigned_32 ) : sizeof( unsigned_16 );
            si->prolog_size = ssr->proc.prologue_len;
            si->epilog_size = ssr->proc.len - ssr->proc.prologue_body_len;
            si->rtn_size = ssr->proc.len;
            /* FIXME:
            si->num_parms
            si->rtn_calloc
            si->ret_modifier
            si->ret_size
            si->is_static */
            break;

        case HLL_SSR_CONSTANT:
            si->kind = SK_CONST;
            break;

        case HLL_SSR_STATIC:
        case HLL_SSR_STATIC2:
            si->is_global = 1; //??
            /* fall thru */
        case HLL_SSR_STATIC_SCOPED:
            si->kind = SK_DATA;
            si->is_static = 1;
            break;

        case HLL_SSR_MEMBER:
        case HLL_SSR_TLS:
        case HLL_SSR_REG:
        case HLL_SSR_AUTO:
        case HLL_SSR_AUTO_SCOPED:
        case HLL_SSR_TABLE:
            si->kind = SK_DATA;
            break;

        case HLL_SSR_BEGIN:
        case HLL_SSR_END:
        case HLL_SSR_WITH:
        case HLL_SSR_SKIP: //??
        case HLL_SSR_CHANGE_SEG:
        case HLL_SSR_BASED: //??
        case HLL_SSR_MAP:
        case HLL_SSR_REG_RELATIVE: //??
        case HLL_SSR_BASED_MEMBER: //??
        case HLL_SSR_ARRAY_SYM: //??
        case HLL_SSR_CU_INFO:
        case HLL_SSR_CU_FUNC_NUM:
            break;
        default:
            hllConfused();
            return( DS_FAIL );
        }
    }
    return( DS_OK );

#if 0
    s_all       *p;


    if( is->containing_type != 0 ) {
        return( hllTypeSymGetInfo( ii, is, lc, si ) );
    }
    p = VMBlock( ii, is->handle, is->len );
    if( p == NULL ) return( DS_FAIL );
    switch( p->common.code ) {
    case S_PUB16:
        if( hllIsSegExecutable( ii, p->pub16.f.segment ) ) {
            si->kind = SK_CODE;
        } else {
            si->kind = SK_DATA;
        }
        si->is_global = 1;
        break;
    case S_PUB32:
        if( hllIsSegExecutable( ii, p->pub32.f.segment ) ) {
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
        //NYI: fill in si->rtn_calloc
        //NYI: fill in si->rtn_modifier
        //NYI: fill in si->ret_size
        //NYI: fill in si->num_parms
        break;
    case S_GPROC32:
        si->is_global = 1;
        /* fall through */
    case S_LPROC32:
        si->kind = SK_PROCEDURE;
        si->rtn_far = p->lproc32.f.flags.f.far_ret;
        if( ii->mad == MAD_AXP ) {
            si->ret_addr_offset = 0;
        } else {
            si->ret_addr_offset = sizeof( unsigned_32 );
        }
        si->prolog_size = p->lproc32.f.debug_start;
        si->epilog_size = p->lproc32.f.proc_length - p->lproc32.f.debug_end;
        si->rtn_size = p->lproc32.f.proc_length;
        //NYI: fill in si->rtn_calloc
        //NYI: fill in si->rtn_modifier
        //NYI: fill in si->ret_size
        //NYI: fill in si->num_parms
        break;
    case S_LABEL16:
    case S_LABEL32:
        si->kind = SK_CODE;
        break;
    default:
        hllConfused();
        return( DS_FAIL );
    }
    return( DS_OK );
#endif
}

#if 0
static const unsigned_8 DXAXList[]      = { CV_X86_DX, CV_X86_AX };
static const unsigned_8 DXEAXList[]     = { CV_X86_DX, CV_X86_EAX };
static const unsigned_8 ST1ST0List[]    = { CV_X86_ST1, CV_X86_ST0 };
#endif

dip_status      DIGENTRY DIPImpSymParmLocation( imp_image_handle *ii,
                    imp_sym_handle *is, location_context *lc,
                    location_list *ll, unsigned n )
{
#if 0
    s_all               *p;
    unsigned            type;
    int                 is32;
    unsigned            parm_count;
    cv_calls            call;
    dip_status          ds;
    unsigned_8          *reg_list;
    imp_type_handle     it;
    dip_type_info       ti;

    p = VMBlock( ii, is->handle, is->len );
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
        return( DS_ERR|DS_FAIL );
    }
    ds = hllTypeCallInfo( ii, type, &call, &parm_count );
    if( ds != DS_OK ) return( ds );
    if( n > parm_count ) return( DS_NO_PARM );
    if( n == 0 ) {
        /* return value */
        p = VMRecord( ii, is->handle + is->len, NULL, 0 );
        if( p == NULL ) return( DS_ERR|DS_FAIL );
        /* WARNING: assuming that S_RETURN directly follows func defn */
        if( p->common.code == S_RETURN ) {
            switch( p->return_.f.style ) {
            case CVRET_VOID:
                return( DS_NO_PARM );
            case CVRET_DIRECT:
                reg_list = (unsigned_8 *)(&p->return_ + 1);
                return( hllLocationManyReg( ii, reg_list[0], &reg_list[1], lc, ll ) );
            case CVRET_CALLOC_NEAR:
            case CVRET_CALLOC_FAR:
            case CVRET_RALLOC_NEAR:
            case CVRET_RALLOC_FAR:
                //NYI: have to handle these suckers
                NYI();
                break;
            }
            return( DS_ERR|DS_BAD_LOCATION );
        }
        /* find out about return type */
        ds = hllTypeIndexFillIn( ii, type, &it );
        if( ds != DS_OK ) return( ds );
        ds = hllTypeBase( ii, &it, &it );
        if( ds != DS_OK ) return( ds );
        ds = hllTypeInfo( ii, &it, lc, &ti );
        if( ds != DS_OK ) return( ds );
        switch( ti.kind ) {
        case TK_VOID:
            return( DS_BAD_LOCATION );
        case TK_BOOL:
        case TK_ENUM:
        case TK_CHAR:
        case TK_INTEGER:
        case TK_POINTER:
            switch( ii->mad ) {
            case MAD_X86:
                switch( ti.size ) {
                case 1:
                    return( hllLocationOneReg( ii, CV_X86_AL, lc, ll ) );
                case 2:
                    return( hllLocationOneReg( ii, CV_X86_AX, lc, ll ) );
                case 4:
                    if( is32 ) {
                        return( hllLocationOneReg( ii, CV_X86_EAX, lc, ll ) );
                    } else {
                        return( hllLocationManyReg( ii, sizeof( DXAXList ), DXAXList, lc, ll ) );
                    }
                case 6:
                    return( hllLocationManyReg( ii, sizeof( DXEAXList ), DXEAXList, lc, ll ) );
                }
                break;
            case MAD_AXP:
                 return( hllLocationOneReg( ii, CV_AXP_r0, lc, ll ) );
            }
            return( DS_ERR|DS_FAIL );
        case TK_REAL:
            switch( ii->mad ) {
            case MAD_X86:
                return( hllLocationOneReg( ii, CV_X86_ST0, lc, ll ) );
            case MAD_AXP:
                return( hllLocationOneReg( ii, CV_AXP_f0, lc, ll ) );
            }
            return( DS_ERR|DS_FAIL );
        case TK_COMPLEX:
            switch( ii->mad ) {
            case MAD_X86:
                return( hllLocationManyReg( ii, sizeof( ST1ST0List ), ST1ST0List, lc, ll ) );
            }
            return( DS_ERR|DS_FAIL );
        case TK_STRUCT:
        case TK_ARRAY:
            //NYI: have to handle these suckers
            NYI();
            break;
        }
        return( DS_ERR|DS_FAIL );
    }
    switch( call ) {
    case CV_NEARC:
    case CV_FARC:
        /* all on stack */
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
        //NYI: have to handle all of these suckers
        NYI();
        break;
    }
    return( DS_ERR|DS_NO_PARM );
#else
    return( DS_ERR|DS_BAD_LOCATION );
#endif
}

dip_status      DIGENTRY DIPImpSymObjType( imp_image_handle *ii,
                    imp_sym_handle *is, imp_type_handle *it, dip_type_info *ti )
{
#if 0
    dip_status          ds;
    imp_type_handle     func_it;
    imp_type_handle     this_it;

    ds = ImpSymType( ii, is, &func_it );
    if( ds != DS_OK ) return( ds );
    ds = TypeMemberFuncInfo( ii, &func_it, it, &this_it, NULL );
    if( ds != DS_OK ) return( ds );
    if( ti != NULL ) {
        ds = hllTypeInfo( ii, &this_it, NULL, ti );
        if( ds != DS_OK ) return( ds );
    }
    return( DS_OK );
#else
    return( DS_FAIL );
#endif
}

dip_status      DIGENTRY DIPImpSymObjLocation( imp_image_handle *ii,
                                imp_sym_handle *is, location_context *lc,
                                 location_list *ll )
{
#if 0
    const char          *name;
    unsigned            len;
    dip_status          ds;
    virt_mem            check;
    virt_mem            next;
    s_all               *p;
    imp_sym_handle      parm;
    imp_type_handle     it;
    dip_type_info       ti;
    unsigned long       adjust;

    ds = ImpSymType( ii, is, &it );
    if( ds != DS_OK ) return( ds );
    ds = TypeMemberFuncInfo( ii, &it, NULL, &it, &adjust );
    if( ds != DS_OK ) return( ds );
    if( it.idx == 0 ) return( DS_FAIL );
    check = is->handle + is->len;
#define THIS_NAME       "this"
    for( ;; ) {
        p = VMRecord( ii, check, &next, NULL );
        if( p == NULL ) return( DS_ERR|DS_FAIL );
        switch( p->common.code ) {
        case S_END:
        case S_ENDARG:
            return( DS_FAIL );
        }
        ds = hllSymFillIn( ii, &parm, check );
        if( ds != DS_OK ) return( ds );
        ds = hllSymGetName( ii, &parm, &name, &len, &p );
        if( ds != DS_OK ) return( ds );
        if( p->common.code == S_ENTRYTHIS ) break;
        if( len == (sizeof( THIS_NAME ) - 1) &&
            memcmp( name, THIS_NAME, sizeof( THIS_NAME ) - 1 ) == 0 ) break;
        check = next;
    }
    /* We have a 'this' pointer! Repeat, we have a this pointer! */
    ds = hllSymLocation( ii, &parm, lc, ll );
    if( ds != DS_OK ) return( ds );
    ds = hllTypeInfo( ii, &it, lc, &ti );
    if( ds != DS_OK ) return( ds );
    ds = hllDoIndirection( ii, &ti, lc, ll );
    if( ds != DS_OK ) return( ds );
    hllLocationAdd( ll, adjust * 8 );
    return( DS_OK );
#else
    return( SR_NONE );
#endif
}

search_result   DIGENTRY DIPImpAddrSym( imp_image_handle *ii,
                            imp_mod_handle im, address a, imp_sym_handle *is )
{
#if 0
    search_result       sr;
    search_result       prev_sr;
    addr_off            best_off;

    is->im = im;
    best_off = 0;
    sr = TableSearchForAddr( ii, a, is, &best_off, sstStaticSym );
    switch( sr ) {
    case SR_EXACT:
    case SR_FAIL:
        return( sr );
    }
    prev_sr = sr;
    sr = TableSearchForAddr( ii, a, is, &best_off, sstGlobalSym );
    switch( sr ) {
    case SR_NONE:
        sr = prev_sr;
        break;
    case SR_EXACT:
    case SR_FAIL:
        return( sr );
    }
    prev_sr = sr;
    sr = TableSearchForAddr( ii, a, is, &best_off, sstGlobalPub );
    if( sr == SR_NONE ) sr = prev_sr;
    return( sr );
#else
    return( SR_NONE );
#endif
}

#if 0
struct search_data {
    lookup_item         li;
    void                *d;
    unsigned            found : 1;
};

static walk_result SymFind( imp_image_handle *ii, sym_walk_info swi,
                                imp_sym_handle *is, void *d )
{
    struct search_data  *sd = d;
    lookup_item         *li;
    const char          *name;
    unsigned            len;
    imp_sym_handle      *new;
    s_all               *p;

    if( swi != SWI_SYMBOL ) return( WR_CONTINUE );
    if( hllSymGetName( ii, is, &name, &len, &p ) != DS_OK ) return( WR_FAIL );
    li = &sd->li;
    switch( p->common.code ) {
    case S_UDT:
    case S_COBOLUDT:
        if( li->type != ST_TYPE ) return( WR_CONTINUE );
        break;
    default:
        if( li->type == ST_TYPE ) return( WR_CONTINUE );
        break;
    }
    if( len != li->name.len ) return( WR_CONTINUE );
    if( li->case_sensitive ) {
        if( memcmp( li->name.start, name, len ) != 0 ) return( WR_CONTINUE );
    } else {
        if( memicmp( li->name.start, name, len ) != 0 ) return( WR_CONTINUE );
    }
    /* Got one! */
    new = DCSymCreate( ii, sd->d );
    if( new == NULL ) return( WR_FAIL );
    *new = *is;
    sd->found = 1;
    return( WR_CONTINUE );
}

static search_result SearchFileScope( imp_image_handle *ii, imp_mod_handle im,
                struct search_data *d )
{
    if( MH2IMH( d->li.mod ) != im && MH2IMH( d->li.mod ) != IMH_NOMOD ) {
        return( SR_NONE );
    }
    switch( d->li.type ) {
    case ST_TYPE:
    case ST_NONE:
        break;
    default:
        return( SR_NONE );
    }
    if( ScopeWalkFile( ii, im, SymFind, d ) == WR_FAIL ) {
        return( SR_FAIL );
    }
    return( d->found ? SR_EXACT : SR_NONE );
}

static search_result SymCreate( imp_image_handle *ii, s_all *p,
                        imp_sym_handle *is, void *d )
{
    imp_sym_handle      *new;

    p = p;
    new = DCSymCreate( ii, d );
    if( new == NULL ) return( SR_FAIL );
    *new = *is;
    /* keep on looking for more */
    return( SR_CLOSEST );
}

static search_result    DoLookupSym( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li, void *d )
{
    search_result       sr;
    imp_sym_handle      is;
    unsigned long       hash;
    struct search_data  data;
    imp_sym_handle      *scope_is;
    unsigned            len;

    data.d = d;
    data.found = 0;
    data.li = *li;
    if( ss == SS_SCOPESYM ) {
        char    *scope_name;
        scope_is = source;
        len = hllSymName( ii, scope_is, NULL, SN_SOURCE, NULL, 0 );
        scope_name = __alloca( len + 1 );
        hllSymName( ii, scope_is, NULL, SN_SOURCE, scope_name, len + 1 );
        data.li.scope.start = scope_name;
        data.li.scope.len = len;
        ss = SS_MODULE;
        data.li.mod = IMH2MH( scope_is->im );
        source = &data.li.mod;
    }
    switch( li->type ) {
    case ST_STRUCT_TAG:
    case ST_CLASS_TAG:
    case ST_UNION_TAG:
    case ST_ENUM_TAG:
        return( TypeSearchTagName( ii, &data.li, d ) );
    }
    switch( ss ) {
    case SS_MODULE:
        is.im = *(imp_mod_handle *)source;
        sr = SearchFileScope( ii, is.im, &data );
        if( sr != SR_NONE ) return( sr );
        break;
    case SS_SCOPED:
        if( ImpAddrMod( ii, *(address *)source, &is.im ) == SR_NONE ) {
            /* just check the global symbols */
            break;
        }
        if( MH2IMH( data.li.mod ) != is.im && MH2IMH( data.li.mod ) != IMH_NOMOD ) {
            return( SR_NONE );
        }
        switch( data.li.type ) {
        case ST_NONE:
        case ST_TYPE:
            if( ScopeWalkAll( ii, is.im, *(address *)source, SymFind, &data ) == WR_FAIL ) {
                return( SR_FAIL );
            }
            if( data.found ) return( SR_EXACT );
            break;
        }
        sr = SearchFileScope( ii, is.im, &data );
        if( sr != SR_NONE ) return( sr );
        break;
    case SS_TYPE:
        return( TypeSearchNestedSym( ii, (imp_type_handle *)source, &data.li, d ) );
    }
    switch( data.li.type ) {
    case ST_NONE:
    case ST_TYPE:
        break;
    default:
        return( SR_NONE );
    }
    if( MH2IMH( data.li.mod ) != is.im && MH2IMH( data.li.mod ) != IMH_NOMOD ) {
        return( SR_NONE );
    }
    hash = CalcHash( data.li.name.start, data.li.name.len );
    sr = TableSearchForName( ii, data.li.case_sensitive,
        data.li.name.start, data.li.name.len, hash, &is, SymCreate, d, sstStaticSym );
    if( sr != SR_NONE ) return( sr );
    sr = TableSearchForName( ii, data.li.case_sensitive,
        data.li.name.start, data.li.name.len, hash, &is, SymCreate, d, sstGlobalSym );
    if( sr != SR_NONE ) return( sr );
    sr = TableSearchForName( ii, data.li.case_sensitive,
        data.li.name.start, data.li.name.len, hash, &is, SymCreate, d, sstGlobalPub );
    return( sr );
}
#endif

#define OPERATOR_TOKEN          "operator"
#define DESTRUCTOR_TOKEN        "~"

search_result   DoImpLookupSym( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
#if 0
    lookup_token        save_name;
    symbol_type         save_type;
    search_result       sr;
    char                *new;
    unsigned            new_len;

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
        new = __alloca( li->name.len );
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
    sr = DoLookupSym( ii, ss, source, li, d );
    li->name = save_name;
    li->type = save_type;
    return( sr );
#else
    return( SR_NONE );
#endif
}

search_result   DIGENTRY DIPImpLookupSym( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li, void *d )
{
#if 0
    return( DoImpLookupSym( ii, ss, source, li, NULL, d ) );
#else
    return( SR_NONE );
#endif
}

search_result   DIGENTRY DIPImpLookupSymEx( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
{
#if 0
    return( DoImpLookupSym( ii, ss, source, li, lc, d ) );
#else
    return( SR_NONE );
#endif
}

search_result   DIGENTRY DIPImpAddrScope( imp_image_handle *ii,
                imp_mod_handle im, address addr, scope_block *scope )
{
#if 0
    scope_info  sc_info;
    dip_status  ds;

    scope->unique = 0;
    ds = ScopeFindFirst( ii, im, addr, &sc_info );
    if( ds & DS_ERR ) return( SR_FAIL );
    if( ds != DS_OK ) return( SR_NONE );
    scope->start = sc_info.start;
    scope->len   = sc_info.len;
    scope->unique= sc_info.scope;
    return( sc_info.start.mach.offset == addr.mach.offset ? SR_EXACT : SR_CLOSEST );
#else
    return( SR_NONE );
#endif
}

search_result   DIGENTRY DIPImpScopeOuter( imp_image_handle *ii,
                imp_mod_handle im, scope_block *in, scope_block *out )
{
#if 0
    scope_info  sc_info;
    dip_status  ds;

    if( in->unique == 0 ) return( SR_NONE );
    sc_info.cde = hllFindDirEntry( ii, im, sstAlignSym );
    if( sc_info.cde == NULL ) return( SR_FAIL );
    ds = hllScopeFillIn( ii, in->unique & SCOPE_UNIQUE_MASK, &sc_info, NULL );
    if( ds != DS_OK ) return( SR_FAIL );
    if( !(in->unique & SCOPE_CLASS_FLAG) ) {
        switch( sc_info.code ) {
        case S_GPROC16:
        case S_LPROC16:
        case S_GPROC32:
        case S_LPROC32:
            /* Might be a member function. We'll let WalkSym figure it out. */
            *out = *in;
            out->unique |= SCOPE_CLASS_FLAG;
            return( SR_CLOSEST );
        }
    }
    ds = ScopeFindNext( ii, &sc_info );
    if( ds & DS_ERR ) return( SR_FAIL );
    if( ds != DS_OK ) return( SR_NONE );
    out->start = sc_info.start;
    out->len   = sc_info.len;
    out->unique= sc_info.scope;
    return( SR_CLOSEST );
#else
    return( SR_NONE );
#endif
}

/*
 * Compare two sym handles and return 0 if they refer to the same
 * information. If they refer to differnt things return either a
 * positive or negative value to impose an 'order' on the information.
 *
 * The value should obey the following constraints.
 * Given three handles H1, H2, H3:
 *          - if H1 < H2 then H1 is always < H2
 *          - if H1 < H2 and H2 < H3 then H1 is < H3
 * The reason for the constraints is so that a client can sort a
 * list of handles and binary search them.
 */
int DIGENTRY DIPImpSymCmp( imp_image_handle *ii,
                           imp_sym_handle *is1, imp_sym_handle *is2 )
{
    ii = ii;
    if( is1->handle < is2->handle ) {
        return( -1 );
    }
    if( is1->handle > is2->handle ) {
        return( 1 );
    }
    return( 0 );
}

dip_status DIGENTRY DIPImpSymAddRef( imp_image_handle *ii, imp_sym_handle *is )
{
    ii=ii;
    is=is;
    return( DS_OK );
}

dip_status DIGENTRY DIPImpSymRelease( imp_image_handle *ii, imp_sym_handle *is )
{
    ii=ii;
    is=is;
    return( DS_OK );
}

dip_status DIGENTRY DIPImpSymFreeAll( imp_image_handle *ii )
{
    ii=ii;
    return( DS_OK );
}
