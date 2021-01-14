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
* Description:  HLL/CV debugging type support.
*
****************************************************************************/


#include "hllinfo.h"
#include "walloca.h"

/* FIXME: kick out these! */
#include "cv4w.h"

enum {
#define _CVREG( name, num )     CV_X86_##name = num,
#include "cv4intl.h"
#undef _CVREG
#define _CVREG( name, num )     CV_AXP_##name = num,
#include "cv4axp.h"
CV_LAST_REG
};


#define UNKNOWN_TYPE_IDX        ((unsigned short)-1)
#define PT_REALLY_CHAR          0x0070

static dip_status hllTypeArrayInfo( imp_image_handle *iih,
                        imp_type_handle *array_ith, location_context *lc,
                        array_info *ai, imp_type_handle *index_ith );

static dip_status TypeVMGetName( imp_image_handle *iih, virt_mem base,
                        const char **name_p, size_t *name_len_p, lf_all **pp )
{
    lf_all              *p;
    unsigned            skip;
    const char          *name;
    numeric_leaf        dummy;

    if( base == 0 ) {
        *name_p = NULL;
        *name_len_p = 0;
        return( DS_OK );
    }
    /*
       The "+ sizeof( unsigned_32 )" is to make sure that the GetNumLeaf's
       have enough stuff mapped in to work.
    */
    p = VMBlock( iih, base, sizeof( *p ) + sizeof( unsigned_32 ) );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    switch( p->common.code ) {
    case LF_ARRAY:
        skip = sizeof( lf_array );
        break;
    case LF_CLASS:
    case LF_STRUCTURE:
        name = hllGetNumLeaf( &p->class_ + 1, &dummy );
        skip = name - (char *)p;
        break;
    case LF_UNION:
        name = hllGetNumLeaf( &p->union_ + 1, &dummy );
        skip = name - (char *)p;
        break;
    case LF_ENUM:
        skip = sizeof( lf_enum );
        break;
    case LF_ENUMERATE:
        name = hllGetNumLeaf( &p->enumerate + 1, &dummy );
        skip = name - (char *)p;
        break;
    case LF_FRIENDFCN:
        skip = sizeof( lf_friendfcn );
        break;
    case LF_MEMBER:
        name = hllGetNumLeaf( &p->member + 1, &dummy );
        skip = name - (char *)p;
        break;
    case LF_STMEMBER:
        skip = sizeof( lf_stmember );
        break;
    case LF_METHOD:
        skip = sizeof( lf_method );
        break;
    case LF_NESTEDTYPE:
        skip = sizeof( lf_nestedtype );
        break;
    case LF_ONEMETHOD:
        skip = sizeof( lf_onemethod );
        switch( p->onemethod.f.attr.f.mprop ) {
        case CV_VIRTUAL:
        case CV_INTROVIRT:
        case CV_PUREVIRT:
        case CV_PUREINTROVIRT:
            skip += sizeof( unsigned_32 );
            break;
        }
        break;
    default:
        *name_p = NULL;
        *name_len_p = 0;
        return( DS_OK );
    }
    /* A name can't be longer than 255 bytes */
    p = VMBlock( iih, base, 256 + skip );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    name = (char *)p + skip;
    *name_len_p = *(unsigned_8 *)name;
    *name_p = &name[1];
    if( pp != NULL )
        *pp = p;
    return( DS_OK );
}

static virt_mem TypeIndexVM( imp_image_handle *iih, unsigned idx )
{
    cv_directory_entry  *cde;
    unsigned_32         *p;

    if( idx < CV_FIRST_USER_TYPE )
        return( 0 );
    cde = (cv_directory_entry *)hllFindDirEntry( iih, IMH_GBL, sstGlobalTypes );
    if( cde == NULL )
        return( 0 );
    p = VMBlock( iih,
        cde->lfo
           + (unsigned long)(idx - CV_FIRST_USER_TYPE) * sizeof( *p )
           + offsetof( cv_sst_global_types_header, offType ),
        sizeof( *p ) );
    if( p == NULL )
        return( 0 );
    return( *p + iih->types_base );
}

dip_status hllTypeIndexFillIn( imp_image_handle *iih, unsigned idx, imp_type_handle *ith )
{
    ith->array_dim = 0;
    ith->idx = idx;
    if( idx < CV_FIRST_USER_TYPE ) {
        ith->handle = 0;
        return( DS_OK );
    }
    ith->handle = TypeIndexVM( iih, idx );
    if( ith->handle == 0 )
        return( DS_ERR | DS_FAIL );
    ith->handle += sizeof( unsigned_16 );
    return( DS_OK );
}

static dip_status TypeReal( imp_image_handle *iih,
                                imp_type_handle *in_ith, imp_type_handle *out_ith,
                                lf_all **pp )
{
    lf_all      *p;

    *out_ith = *in_ith;
    p = NULL;
    for( ;; ) {
        if( out_ith->handle == 0 )
            goto done;
        p = VMBlock( iih, out_ith->handle, sizeof( *p ) );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        switch( p->common.code ) {
        case LF_MODIFIER:
            if( hllTypeIndexFillIn( iih, p->modifier.f.index, out_ith ) != DS_OK ) {
                return( DS_ERR | DS_FAIL );
            }
            break;
        default:
            goto done;
        }
    }
done:
    if( pp != NULL )
        *pp = p;
    return( DS_OK );
}

#if 0
dip_status hllTypeCallInfo( imp_image_handle *iih, unsigned idx,
                            cv_calls *call_type, unsigned *parms )
{
    lf_all              *p;
    dip_status          ds;
    imp_type_handle     ith;

    ds = hllTypeIndexFillIn( iih, idx, &ith );
    if( ds != DS_OK )
        return( ds );
    ds = TypeReal( iih, &ith, &ith, &p );
    if( ds != DS_OK )
        return( ds );
    switch( p->common.code ) {
    case LF_PROCEDURE:
        *call_type = p->procedure.f.call;
        *parms = p->procedure.f.parms;
        break;
    case LF_MFUNCTION:
        *call_type = p->mfunction.f.call;
        *parms = p->mfunction.f.parms;
        break;
    default:
        return( DS_FAIL );
    }
    return( DS_OK );
}
#endif

dip_status hllTypeMemberFuncInfo( imp_image_handle *iih, imp_type_handle *func_ith,
                                  imp_type_handle *class_ith, imp_type_handle *this_ith,
                                  unsigned long *adjustp )
{
    lf_all      *p;
    dip_status  ds;
    unsigned    class_idx;

    p = VMBlock( iih, func_ith->handle, sizeof( *p ) );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    if( p->common.code != LF_MFUNCTION )
        return( DS_FAIL );
    if( adjustp != NULL ) {
        *adjustp = p->mfunction.f.thisadjust;
    }
    class_idx = p->mfunction.f.class_idx;
    if( this_ith != NULL ) {
        ds = hllTypeIndexFillIn( iih, p->mfunction.f.thisptr, this_ith );
        if( ds != DS_OK ) {
            return( ds );
        }
    }
    if( class_ith != NULL ) {
        ds = hllTypeIndexFillIn( iih, class_idx, class_ith );
        p = VMBlock( iih, class_ith->handle, sizeof( *p ) );
        if( p->common.code == LF_MODIFIER ) { //NON standard need it to handle forward refs
            ds = hllTypeIndexFillIn( iih, p->modifier.f.index, class_ith );
        }
        if( ds != DS_OK ) {
            return( ds );
        }
    }
    return( DS_OK );
}

dip_status hllTypeSymGetName( imp_image_handle *iih, imp_sym_handle *ish,
                              const char **name_p, size_t *name_len_p )
{
    return( TypeVMGetName( iih, ish->handle, name_p, name_len_p, NULL ) );
}


typedef struct pending_type_list pending_type_list;
struct pending_type_list {
    pending_type_list   *prev;
    virt_mem            curr;
    virt_mem            end;
    virt_mem            containing_type;
    unsigned            len;
    unsigned            code;
};

typedef walk_result (DIP_LEAF_WALKER)( imp_image_handle *, sym_walk_info, pending_type_list *, lf_all *, void * );

static walk_result TypeListWalk( imp_image_handle *iih, virt_mem head,
                        virt_mem containing_type, DIP_LEAF_WALKER *wk, void *d )
{
    lf_all              *p;
    unsigned_16         *len_p;
    unsigned            len;
    walk_result         wr;
    numeric_leaf        val;
    void                *ptr;
    pending_type_list   *list;
    pending_type_list   *free_list;
    pending_type_list   *tmp;
    unsigned            base_idx;

    free_list = NULL;
    list = walloca( sizeof( *list ) );
    list->prev = NULL;
    list->curr = head;
    list->containing_type = containing_type;
new_list:
    if( list->curr == 0 )
        return( WR_FAIL );
    len_p = VMBlock( iih, list->curr, sizeof( *len_p ) );
    if( len_p == NULL )
        return( WR_FAIL );
    list->curr += sizeof( *len_p );
    list->end = list->curr + *len_p;
    list->curr += sizeof( p->common.code ); /* skip the list introducer */
    for( ;; ) {
        while( list->curr >= list->end ) {
            tmp = list;
            list = list->prev;
            if( list == NULL )
                return( WR_CONTINUE );
            wr = wk( iih, SWI_INHERIT_END, list, NULL, d );
            if( wr != WR_CONTINUE )
                return( wr );
            tmp->prev = free_list;
            free_list = tmp;
            list->curr += list->len;
        }
        p = VMBlock( iih, list->curr, sizeof( *p ) + sizeof( unsigned_32 ) );
        if( p == NULL )
            return( WR_FAIL );
        if( *(unsigned_8 *)p > LF_PAD0 ) {
            list->curr += *(unsigned_8 *)p & 0x0f;
        } else {
            list->code = p->common.code;
            switch( p->common.code ) {
            case LF_INDEX:
                list->curr = TypeIndexVM( iih, p->index.f.index );
                goto new_list;
            case LF_BCLASS:
                len = (unsigned_8 *)hllGetNumLeaf( &p->bclass + 1, &val ) - (unsigned_8 *)p;
                base_idx = p->bclass.f.type;
                goto inherit_class;
            case LF_VBCLASS:
            case LF_IVBCLASS:
                len = (unsigned_8 *)hllGetNumLeaf( &p->vbclass + 1, &val ) - (unsigned_8 *)p;
                ptr = VMBlock( iih, list->curr + len, sizeof( unsigned_32 ) );
                len += (unsigned_8 *)hllGetNumLeaf( ptr, &val ) - (unsigned_8 *)ptr;
                if( list->prev != NULL ) {
                    /*
                       If we're processing an inherited class, we want to
                       ignore any virtual base classes. They'll get handled
                       by the most derived class.
                    */
                    list->curr += len;
                    continue;
                }
                base_idx = p->vbclass.f.btype;
inherit_class:
                list->len = len;
                wr = wk( iih, SWI_INHERIT_START, list, NULL, d );
                if( wr == WR_STOP ) {
                    list->curr += list->len;
                    continue;
                }
                if( free_list == NULL ) {
                    free_list = walloca( sizeof( *list ) );
                    free_list->prev = NULL;
                }
                tmp = free_list;
                free_list = free_list->prev;
                tmp->prev = list;
                list = tmp;
                list->containing_type = TypeIndexVM( iih, base_idx );
                if( list->containing_type == 0 )
                    return( WR_FAIL );
                p = VMBlock( iih, list->containing_type+sizeof( unsigned_16 ), sizeof( *p ) );
                if( p == NULL )
                    return( WR_FAIL );
                switch( p->common.code ) {
                case LF_CLASS:
                case LF_STRUCTURE:
                    list->curr = TypeIndexVM( iih, p->class_.f.field );
                    break;
                default:
                    hllConfused();
                }
                goto new_list;
            case LF_ENUMERATE:
                len = (unsigned_8 *)hllGetNumLeaf( &p->enumerate + 1, &val ) - (unsigned_8 *)p;
                ptr = VMBlock( iih, list->curr + len, 1 );
                len += *(unsigned_8 *)ptr + 1;
                break;
            case LF_FRIENDFCN:
                len = *(unsigned_8 *)(&p->friendfcn + 1) + sizeof( p->friendfcn ) + 1;
                break;
            case LF_MEMBER:
                len = (unsigned_8 *)hllGetNumLeaf( &p->member + 1, &val ) - (unsigned_8 *)p;
                ptr = VMBlock( iih, list->curr + len, 1 );
                len += *(unsigned_8 *)ptr + 1;
                break;
            case LF_STMEMBER:
                len = *(unsigned_8 *)(&p->stmember + 1) + sizeof( p->stmember ) + 1;
                break;
            case LF_METHOD:
                len = *(unsigned_8 *)(&p->method + 1) + sizeof( p->method ) + 1;
                break;
            case LF_NESTEDTYPE:
                len = *(unsigned_8 *)(&p->nestedtype + 1) + sizeof( p->nestedtype ) + 1;
                break;
            case LF_VFUNCTAB:
                len = sizeof( p->vfunctab );
                break;
            case LF_FRIENDCLS:
                len = sizeof( p->friendcls );
                break;
            case LF_ONEMETHOD:
                len = *(unsigned_8 *)(&p->onemethod + 1) + sizeof( p->onemethod ) + 1;
                switch( p->onemethod.f.attr.f.mprop ) {
                case CV_VIRTUAL:
                case CV_INTROVIRT:
                case CV_PUREVIRT:
                case CV_PUREINTROVIRT:
                    len += sizeof( unsigned_32 );
                    break;
                }
                break;
            case LF_VFUNCOFF:
                len = sizeof( p->vfuncoff );
                break;
            default:
                hllConfused();
                return( WR_FAIL );
            }
            p = VMBlock( iih, list->curr, len );
            if( p == NULL )
                return( WR_FAIL );
            list->len = len;
            wr = wk( iih, SWI_SYMBOL, list, p, d );
            if( wr != WR_CONTINUE )
                return( wr );
            list->curr += len;
        }
    }
}

static void TypePrimitiveInfo( unsigned idx, dig_type_info *ti )
{
    static const unsigned_8     RealSizes[] = { 4, 8, 10, 16, 6 };
    cv_primitive                prim;

    prim.s = idx;
    switch( prim.f.mode ) {
    case CV_DIRECT:
        switch( prim.f.type ) {
        case CV_SPECIAL:
            switch( prim.f.size ) {
            case 0x1:
                ti->kind = TK_ADDRESS;
                ti->size = sizeof( addr48_ptr );
                break;
            case 0x2:
                ti->size = sizeof( addr_seg );
                ti->kind = TK_DATA;
                break;
            case 0x3:
                ti->size = 0;
                ti->kind = TK_VOID;
                break;
            case 0x4:
                ti->size = 8;
                ti->kind = TK_DATA;
                break;
            case 0x5:
            case 0x6:
                /* Basic string: what size? */
                ti->size = 0;
                ti->kind = TK_STRING;
                break;
            }
            break;
        case CV_SIGNED:
            ti->modifier = TM_SIGNED;
            ti->kind = TK_INTEGER;
            ti->size = 1 << prim.f.size;
            if( ti->size == 1 )
                ti->kind = TK_CHAR;
            break;
        case CV_UNSIGNED:
            ti->modifier = TM_UNSIGNED;
            ti->kind = TK_INTEGER;
            ti->size = 1 << prim.f.size;
            if( ti->size == 1 )
                ti->kind = TK_CHAR;
            break;
        case CV_BOOL:
            ti->kind = TK_BOOL;
            ti->size = 1 << prim.f.size;
            break;
        case CV_REAL:
            ti->kind = TK_REAL;
            ti->size = RealSizes[prim.f.size];
            break;
        case CV_COMPLEX:
            ti->kind = TK_COMPLEX;
            ti->size = 2 * RealSizes[prim.f.size];
            break;
        case CV_SPECIAL2:
            ti->kind = TK_DATA;
            ti->size = 1;
            break;
        case CV_REALLYINT:
            switch( prim.f.size ) {
            case 0x0:
                ti->kind = TK_CHAR;
                ti->size = 1;
                break;
            case 0x1:
                ti->kind = TK_CHAR;
                ti->size = 2;
                break;
            case 0x2:
                ti->modifier = TM_SIGNED;
                ti->size = 2;
                ti->kind = TK_INTEGER;
                break;
            case 0x3:
                ti->modifier = TM_UNSIGNED;
                ti->size = 2;
                ti->kind = TK_INTEGER;
                break;
            case 0x4:
                ti->modifier = TM_SIGNED;
                ti->size = 4;
                ti->kind = TK_INTEGER;
                break;
            case 0x5:
                ti->modifier = TM_UNSIGNED;
                ti->size = 4;
                ti->kind = TK_INTEGER;
                break;
            case 0x6:
                ti->modifier = TM_SIGNED;
                ti->size = 8;
                ti->kind = TK_INTEGER;
                break;
            case 0x7:
                ti->modifier = TM_UNSIGNED;
                ti->size = 8;
                ti->kind = TK_INTEGER;
                break;
            }
            break;
        case 0x0f:
            /* my own invention: T_CODE_LBL*, T_DATA_LBL* */
            ti->kind = (prim.s & 0x08) ? TK_DATA : TK_CODE;
            ti->size = 1 << prim.f.size;
            break;
        }
        break;
    case CV_NEARP:
        ti->size = sizeof( addr32_off );
        ti->modifier = TM_NEAR;
        ti->kind = TK_POINTER;
        break;
    case CV_FARP:
        ti->size = sizeof( addr32_ptr );
        ti->modifier = TM_FAR;
        ti->kind = TK_POINTER;
        break;
    case CV_HUGEP:
        ti->size = sizeof( addr32_ptr );
        ti->modifier = TM_HUGE;
        ti->kind = TK_POINTER;
        break;
    case CV_NEAR32P:
        ti->size = sizeof( addr48_off );
        ti->modifier = TM_NEAR;
        ti->kind = TK_POINTER;
        break;
    case CV_FAR32P:
        ti->size = sizeof( addr48_ptr );
        ti->modifier = TM_FAR;
        ti->kind = TK_POINTER;
        break;
    case CV_NEAR64P:
        ti->size = 8;
        ti->modifier = TM_NEAR;
        ti->kind = TK_POINTER;
        break;
    }
}

typedef struct {
    cv_fldattr  attr;
    unsigned_16 type;
    unsigned_32 vtab_off; /* optional: only if virtual function */
} method_info;

static method_info *GetMethodInfo( imp_image_handle *iih, unsigned ml_idx,
                                unsigned mf_idx )
{
    virt_mem    mlist;
    method_info *p;

    mlist = TypeIndexVM( iih, ml_idx );
    if( mlist == 0 )
        return( NULL );
    mlist += sizeof( unsigned_16 ) * 2;
    for( ;; ) {
        p = VMBlock( iih, mlist, sizeof( *p ) );
        if( p == NULL )
            return( NULL );
        if( mf_idx == 0 )
            return( p );
        mlist += sizeof( method_info );
        switch( p->attr.f.mprop ) {
        case CV_INTROVIRT:
        case CV_PUREINTROVIRT:
            break;
        default:
            mlist -= sizeof( p->vtab_off );
            break;
        }
        --mf_idx;
    }
}

static dip_status GetVirtBaseDisp( imp_image_handle *iih, virt_mem adj,
                location_context *lc, location_list ll, unsigned long *disp )
{
    lf_all              *p;
    const void          *ptr;
    numeric_leaf        val;
    unsigned            elt_type;
    unsigned            idx;
    imp_type_handle     vbp_ith;
    dip_status          ds;
    dig_type_info       ti;
    union {
        unsigned_8      u8;
        unsigned_16     u16;
        unsigned_32     u32;
    }                   tmp;
    location_list       dst;

    p = VMBlock( iih, adj, sizeof( *p ) + sizeof( unsigned_32 ) * 4 );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    ptr = hllGetNumLeaf( &p->vbclass + 1, &val );
    hllLocationAdd( &ll, val.int_val * 8 );
    hllGetNumLeaf( ptr, &val );
    idx = val.int_val;
    ds = hllTypeIndexFillIn( iih, p->vbclass.f.vtype, &vbp );
    if( ds != DS_OK )
        return( ds );
    ds = hllTypeInfo( iih, &vbp, lc, &ti );
    if( ds != DS_OK )
        return( ds );
    if( ti.kind != TK_POINTER ) {
        hllConfused();
    }
    p = VMBlock( iih, vbp.handle, sizeof( *p ) );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    if( p->pointer.f.type >= CV_FIRST_USER_TYPE ) {
        ds = hllTypeIndexFillIn( iih, p->pointer.f.type, &vbp );
        if( ds != DS_OK )
            return( ds );
        p = VMBlock( iih, vbp.handle, sizeof( *p ) );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        if( p->common.code != LF_ARRAY || p->array.f.elemtype >= CV_FIRST_USER_TYPE ) {
            hllConfused();
        }
        elt_type = p->array.f.elemtype;
    } else {
        elt_type = p->pointer.f.type;
    }
    ds = hllDoIndirection( iih, &ti, lc, &ll );
    TypePrimitiveInfo( elt_type, &ti );
    ll.e[0].u.addr.mach.offset += ti.size * idx;
    hllLocationCreate( &dst, LT_INTERNAL, &tmp );
    ds = DCAssignLocation( &dst, &ll, ti.size );
    if( ds != DS_OK )
        return( ds );
    switch( ti.size ) {
    case 1:
        *disp = tmp.u8;
        break;
    case 2:
        *disp = tmp.u16;
        break;
    case 4:
        *disp = tmp.u32;
        break;
    default:
        hllConfused();
        return( DS_ERR | DS_FAIL );
    }
    return( DS_OK );
}

struct vtab_data {
    unsigned            ptr_idx;
    unsigned long       disp;
};

static walk_result FindVTab( imp_image_handle *iih, sym_walk_info swi,
            pending_type_list *list, lf_all *p, void *d )
{
    struct vtab_data    *vd = d;

    iih = iih; list = list;
    switch( swi ) {
    case SWI_INHERIT_START:
    case SWI_INHERIT_END:
        return( WR_STOP );
    }
    /* SWI_SYMBOL: */
    switch( p->common.code ) {
    case LF_VFUNCTAB:
        vd->disp = 0;
        vd->ptr_idx = p->vfunctab.f.type;
        return( WR_STOP );
    case LF_VFUNCOFF:
        vd->disp = p->vfuncoff.f.offset;
        vd->ptr_idx = p->vfuncoff.f.type;
        return( WR_STOP );
    }
    return( WR_CONTINUE );
}

static dip_status VFuncLocation( imp_image_handle *iih, imp_sym_handle *ish,
            unsigned_32 vfunc_off, location_context *lc, location_list *ll )
{
    lf_all              *p;
    virt_mem            base;
    struct vtab_data    data;
    imp_type_handle     ith;
    dig_type_info       ti;
    unsigned_8          *vfsp;
    unsigned            vfshape;
    addr_off            save;
    dip_status          ds;

    p = VMBlock( iih, ish->containing_type, sizeof( *p ) );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    base = TypeIndexVM( iih, p->class_.f.field );
    if( base == 0 )
        return( DS_ERR | DS_FAIL );
    switch( TypeListWalk( iih, base, ish->containing_type, FindVTab, &data ) ) {
    case WR_STOP:
        break;
    default:
        return( DS_ERR | DS_FAIL );
    }
    ds = hllTypeIndexFillIn( iih, data.ptr_idx, &ith );
    if( ds != DS_OK )
        return( ds );
    ds = hllTypeInfo( iih, &ith, lc, &ti );
    if( ds != DS_OK )
        return( ds );
    ds = DCItemLocation( lc, CI_OBJECT, ll );
    if( ds != DS_OK )
        return( ds );
    hllLocationAdd( ll, data.disp * 8 );
    ds = hllDoIndirection( iih, &ti, lc, ll );
    if( ds != DS_OK )
        return( ds );
    hllLocationAdd( ll, vfunc_off * 8 );
    ds = hllTypeBase( iih, &ith, &ith );
    if( ds != DS_OK )
        return( ds );
    vfsp = VMBlock( iih, ith.handle + sizeof( unsigned_16 ) * 2 + ish->mfunc_idx / 2, sizeof( *vfsp ) );
    if( vfsp == NULL )
        return( DS_ERR | DS_FAIL );
    if( (ish->mfunc_idx % 2) != 0 ) {
        vfshape = *vfsp >> 4;
    } else {
        vfshape = *vfsp & 0x0f;
    }
    switch( vfshape ) {
    case CV_VTNEAR:
        ti.modifier = TM_NEAR;
        ti.size = sizeof( addr32_off );
        break;
    case CV_VTFAR:
        ti.modifier = TM_FAR;
        ti.size = sizeof( addr32_ptr );
        break;
    case CV_VTNEAR32:
        ti.modifier = TM_NEAR;
        ti.size = sizeof( addr48_off );
        break;
    case CV_VTFAR32:
        ti.modifier = TM_FAR;
        ti.size = sizeof( addr48_ptr );
        break;
    default:
        hllConfused();
        return( DS_ERR | DS_FAIL );
    }
    ds = hllDoIndirection( iih, &ti, lc, ll );
    if( ds != DS_OK )
        return( ds );
    switch( vfshape ) {
    case CV_VTNEAR:
    case CV_VTNEAR32:
        /* since it's a code item, the default addr space is CI_EXECUTION */
        save = ll->e[0].u.addr.mach.offset;
        ds = DCItemLocation( lc, CI_EXECUTION, ll );
        if( ds != DS_OK )
            return( ds );
        ll->e[0].u.addr.mach.offset = save;

    }
    return( DS_OK );
}

static dip_status MatchSymLocation( imp_image_handle *iih, imp_sym_handle *ish,
                    unsigned idx, location_context *lc, location_list *ll )
{
    const char          *name;
    size_t              len;
    char                *buff;
    char                *start;
    imp_sym_handle      real;
    dip_status          ds;

    /*
        Have to lookup "<scope>::<name>" with given type index to get
        address. Ugh :-(.
    */
    ds = TypeVMGetName( iih, ish->handle, &name, &len, NULL );
    if( ds != DS_OK )
        return( ds );
    /* name can't be longer than 256 because of CV format */
    buff = walloca( len + (SCOPE_TOKEN_LEN + 256) );
    start = &buff[SCOPE_TOKEN_LEN+256];
    buff = &start[len];
    memcpy( start, name, len );
    start -= SCOPE_TOKEN_LEN;
    memcpy( start, SCOPE_TOKEN, SCOPE_TOKEN_LEN );
    ds = TypeVMGetName( iih, ish->containing_type, &name, &len, NULL );
    if( ds != DS_OK )
        return( ds );
    start -= len;
    memcpy( start, name, len );
    ds = hllSymFindMatchingSym( iih, start, buff - start, idx, &real );
    if( ds != DS_OK )
        return( ds );
    return( hllSymLocation( iih, &real, lc, ll ) );
}

dip_status hllTypeSymGetAddr( imp_image_handle *iih, imp_sym_handle *ish,
                              location_context *lc, location_list *ll )
{
    lf_all              *p;
    numeric_leaf        val;
    dip_status          ds;
    unsigned            idx;
    imp_type_handle     base_ith;
    unsigned long       disp;
    method_info         *minfo;

    p = VMBlock( iih, ish->handle, sizeof( *p ) + sizeof( unsigned_32 ) * 2 );
    switch( p->common.code ) {
    case LF_MEMBER:
        /* save type index from scurges of VM system */
        idx = p->member.f.type;
        hllGetNumLeaf( &p->member + 1, &val );
        ds = DCItemLocation( lc, CI_OBJECT, ll );
        if( ds != DS_OK )
            return( ds );
        if( ish->adjustor_type != 0 ) {
            /* have to fish displacement out of virtual base table */
            ds = GetVirtBaseDisp( iih, ish->adjustor_type, lc, *ll, &disp );
            if( ds != DS_OK )
                return( ds );
            hllLocationAdd( ll, disp * 8 );
        }
        hllLocationAdd( ll, (ish->adjustor_offset + val.int_val) * 8 );
        if( idx >= CV_FIRST_USER_TYPE ) {
            /* have to check type in case it's a bit field */
            ds = hllTypeIndexFillIn( iih, idx, &base_ith );
            if( ds != DS_OK )
                return( ds );
            p = VMBlock( iih, base_ith.handle, sizeof( p->bitfield ) );
            if( p == NULL )
                return( DS_ERR | DS_FAIL );
            if( p->common.code == LF_BITFIELD ) {
                hllLocationAdd( ll, p->bitfield.f.position );
                hllLocationTrunc( ll, p->bitfield.f.length );
            }
        }
        break;
    case LF_STMEMBER:
        return( MatchSymLocation( iih, ish, p->stmember.f.type, lc, ll ) );
    case LF_ONEMETHOD:
        minfo = (method_info *)&p->onemethod.f.attr;
        goto method_addr;
    case LF_METHOD:
        minfo = GetMethodInfo( iih, p->method.f.mList, ish->mfunc_idx );
        if( minfo == NULL )
            return( DS_ERR | DS_FAIL );
method_addr:
        switch( minfo->attr.f.mprop ) {
        case CV_VIRTUAL:
        case CV_PUREVIRT:
             return( DS_FAIL );
        case CV_INTROVIRT:
        case CV_PUREINTROVIRT:
            return( VFuncLocation( iih, ish, minfo->vtab_off, lc, ll ) );
        }
        return( MatchSymLocation( iih, ish, minfo->type, lc, ll ) );
    case LF_ENUMERATE:
        return( DS_FAIL );
    default:
        hllConfused();
        return( DS_FAIL );
    }
    return( DS_OK );
}

dip_status hllTypeSymGetType( imp_image_handle *iih, imp_sym_handle *ish,
                              imp_type_handle *ith )
{
    lf_all              *p;
    dip_status          ds;
    unsigned            idx;
    method_info         *minfo;

    p = VMBlock( iih, ish->handle, sizeof( *p ) );
    switch( p->common.code ) {
    case LF_STMEMBER:
    case LF_MEMBER:
        idx = p->member.f.type;
        if( idx >= CV_FIRST_USER_TYPE ) {
            /* have to check type in case it's a bit field */
            ds = hllTypeIndexFillIn( iih, idx, ith );
            if( ds != DS_OK )
                return( ds );
            p = VMBlock( iih, ith->handle, sizeof( p->bitfield ) );
            if( p == NULL )
                return( DS_ERR | DS_FAIL );
            if( p->common.code == LF_BITFIELD ) {
                idx = p->bitfield.f.type;
            }
        }
        break;
    case LF_ONEMETHOD:
        idx = p->onemethod.f.type;
        break;
    case LF_METHOD:
        minfo = GetMethodInfo( iih, p->method.f.mList, ish->mfunc_idx );
        if( minfo == NULL )
            return( DS_ERR | DS_FAIL );
        idx = minfo->type;
        break;
    case LF_ENUMERATE:
        ith->handle = ish->containing_type;
        ith->idx = UNKNOWN_TYPE_IDX;
        ith->array_dim = 0;
        return( DS_OK );
    default:
        /* type name */
        ith->handle = ish->handle;
        ith->idx = UNKNOWN_TYPE_IDX;
        ith->array_dim = 0;
        return( DS_OK );
    }
    return( hllTypeIndexFillIn( iih, idx, ith ) );
}

dip_status hllTypeSymGetValue( imp_image_handle *iih, imp_sym_handle *ish,
                               location_context *lc, void *buff )
{
    lf_all              *p;
    numeric_leaf        val;

    lc = lc;
    /*
       The "+ sizeof( unsigned_32 )" is to make sure that the hllGetNumLeaf
       has enough stuff mapped in to work.
    */
    p = VMBlock( iih, ish->handle, sizeof( *p ) + sizeof( unsigned_32 ) );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    switch( p->common.code ) {
    case LF_ENUMERATE:
        hllGetNumLeaf( &p->enumerate + 1, &val );
        /* make sure everything is mapped in */
        p = VMBlock( iih, ish->handle,
                val.size + sizeof( *p ) + sizeof( unsigned_32 ) );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        /* VM might have shifted things around */
        hllGetNumLeaf( &p->enumerate + 1, &val );
        memcpy( buff, val.valp, val.size );
        return( DS_OK );
    }
    return( DS_FAIL );
}

dip_status hllTypeSymGetInfo( imp_image_handle *iih, imp_sym_handle *ish,
                              location_context *lc, sym_info *si )
{
    lf_all              *p;
    method_info         *minfo;

    lc = lc;
    p = VMBlock( iih, ish->handle, sizeof( *p ) );
    switch( p->common.code ) {
    case LF_STMEMBER:
    case LF_MEMBER:
        si->kind = SK_DATA;
        si->is_member = 1;
        switch( p->member.f.attr.f.access ) {
        case CV_PRIVATE:
            si->is_private = 1;
            break;
        case CV_PROTECTED:
            si->is_protected = 1;
            break;
        case CV_PUBLIC:
            si->is_public = 1;
            break;
        }
        break;
    case LF_ONEMETHOD:
        si->kind = SK_CODE;
        si->is_member = 1;
        switch( p->onemethod.f.attr.f.access ) {
        case CV_PRIVATE:
            si->is_private = 1;
            break;
        case CV_PROTECTED:
            si->is_protected = 1;
            break;
        case CV_PUBLIC:
            si->is_public = 1;
            break;
        }
        break;
    case LF_METHOD:
        si->kind = SK_CODE;
        si->is_member = 1;
        minfo = GetMethodInfo( iih, p->method.f.mList, ish->mfunc_idx );
        if( minfo == NULL )
            return( DS_ERR | DS_FAIL );
        switch( minfo->attr.f.access ) {
        case CV_PRIVATE:
            si->is_private = 1;
            break;
        case CV_PROTECTED:
            si->is_protected = 1;
            break;
        case CV_PUBLIC:
            si->is_public = 1;
            break;
        }
        break;
    case LF_ENUMERATE:
        si->kind = SK_CONST;
        break;
    default:
        si->kind = SK_TYPE;
        break;
    }
    return( DS_OK );
}

struct walk_glue {
    DIP_IMP_SYM_WALKER  *wk;
    void                *d;
    imp_sym_handle      *ish;
};

static walk_result WalkGlue( imp_image_handle *iih, sym_walk_info swi,
            pending_type_list *list, lf_all *p, void *d )
{
    struct walk_glue    *gd = d;
    numeric_leaf        val;
    walk_result         wr;
    unsigned            count;

    switch( swi ) {
    case SWI_INHERIT_START:
        wr = gd->wk( iih, SWI_INHERIT_START, NULL, gd->d );
        if( wr != WR_CONTINUE )
            return( wr );
        switch( list->code ) {
        case LF_BCLASS:
            p = VMBlock( iih, list->curr, list->len );
            if( p == NULL )
                return( WR_FAIL );
            hllGetNumLeaf( &p->bclass + 1, &val );
            gd->ish->adjustor_offset += val.int_val;
            break;
        default:
            gd->ish->adjustor_type = list->curr;
            break;
        }
        return( WR_CONTINUE );
    case SWI_INHERIT_END:
        wr = gd->wk( iih, SWI_INHERIT_END, NULL, gd->d );
        if( wr != WR_CONTINUE )
            return( wr );
        switch( list->code ) {
        case LF_BCLASS:
            p = VMBlock( iih, list->curr, list->len );
            if( p == NULL )
                return( WR_FAIL );
            hllGetNumLeaf( &p->bclass + 1, &val );
            gd->ish->adjustor_offset -= val.int_val;
            break;
        default:
            gd->ish->adjustor_type = 0;
            break;
        }
        return( WR_CONTINUE );
    }
    /* SWI_SYMBOL: */
    switch( p->common.code ) {
    case LF_MEMBER:
    case LF_STMEMBER:
    case LF_ENUMERATE:
    case LF_ONEMETHOD:
        count = 1;
        break;
    case LF_METHOD:
        count = p->method.f.count;
        break;
    default:
        return( WR_CONTINUE );
    }
    gd->ish->mfunc_idx = 0;
    gd->ish->containing_type = list->containing_type;
    gd->ish->handle = list->curr;
    do {
        wr = gd->wk( iih, SWI_SYMBOL, gd->ish, gd->d );
        if( wr != WR_CONTINUE )
            return( wr );
        gd->ish->mfunc_idx++;
    } while( --count != 0 );
    return( WR_CONTINUE );
}

walk_result hllTypeSymWalkList( imp_image_handle *iih, imp_type_handle *ith,
                                DIP_IMP_SYM_WALKER* wk, imp_sym_handle *ish, void *d )
{
    struct walk_glue    glue;
    lf_all              *p;
    unsigned            list_idx;
    virt_mem            base;
    imp_type_handle     real_ith;
    dip_status          ds;

    glue.wk = wk;
    glue.d  = d;
    glue.ish = ish;

    ds = TypeReal( iih, ith, &real, &p );
    if( ds != DS_OK )
        return( WR_FAIL );
    if( real.idx < CV_FIRST_USER_TYPE )
        return( WR_CONTINUE );
    switch( p->common.code ) {
    case LF_CLASS:
    case LF_STRUCTURE:
        list_idx = p->class_.f.field;
        break;
    case LF_UNION:
        list_idx = p->union_.f.field;
        break;
    case LF_ENUM:
        list_idx = p->enum_.f.fList;
        break;
    default:
        return( SR_NONE );
    }
    base = TypeIndexVM( iih, list_idx );
    if( base == 0 )
        return( WR_FAIL );
    ish->adjustor_type = 0;
    ish->adjustor_offset = 0;
    return( TypeListWalk( iih, base, ith->handle, WalkGlue, &glue ) );
}

search_result hllTypeSearchTagName( imp_image_handle *iih, lookup_item *li, void *d )
{
    unsigned            code;
    unsigned long       count;
    cv_directory_entry  *cde;
    lf_all              *p;
    virt_mem            type;
    virt_mem            array_vm;
    unsigned_32         *array_p;
    const char          *name;
    size_t              len;
    const char          *lookup_name;
    size_t              lookup_len;
    strcompn_fn         *scompn;
    imp_sym_handle      *ish;

    if( MH2IMH( li->mod ) != IMH_NOMOD && MH2IMH( li->mod ) != IMH_GBL ) {
        return( SR_NONE );
    }
    switch( li->type ) {
    case ST_STRUCT_TAG:
        code = LF_STRUCTURE;
        break;
    case ST_CLASS_TAG:
        code = LF_CLASS;
        break;
    case ST_UNION_TAG:
        code = LF_UNION;
        break;
    case ST_ENUM_TAG:
        code = LF_ENUM;
        break;
    default:
        return( SR_NONE );
    }
    cde = (cv_directory_entry *)hllFindDirEntry( iih, IMH_GBL, sstGlobalTypes );
    if( cde == NULL )
        return( SR_NONE );
    array_vm = cde->lfo + sizeof( unsigned_32 );
    array_p = VMBlock( iih, array_vm, sizeof( *array_p ) );
    if( array_p == NULL )
        return( SR_FAIL );
    scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
    lookup_name = li->name.start;
    lookup_len = li->name.len;
    count = *array_p;
    for( ;; ) {
        if( count == 0 )
            return( SR_NONE );
        array_vm += sizeof( *array_p );
        array_p = VMBlock( iih, array_vm, sizeof( *array_p ) );
        if( array_p == NULL )
            return( SR_FAIL );
        type = iih->types_base + *array_p + sizeof( unsigned_16 );
        if( TypeVMGetName( iih, type, &name, &len, &p ) != DS_OK ) {
            return( SR_FAIL );
        }
        if( name != NULL
         && p->common.code == code
         && len == lookup_len
         && scompn( name, lookup_name, len ) == 0 ) {
            ish = DCSymCreate( iih, d );
            if( ish == NULL )
                return( SR_FAIL );
            ish->mfunc_idx = 0;
            ish->handle = type;
            ish->containing_type = type;
            ish->adjustor_type = 0;
            ish->adjustor_offset = 0;
            return( SR_EXACT );
        }
        --count;
    }
}

struct search_data {
    imp_type_handle     *ith;
    lookup_item         *li;
    void                *d;
    unsigned long       adj_offset;
    virt_mem            adj_type;
    search_result       sr;
};

static walk_result SymSearch( imp_image_handle *iih, sym_walk_info swi,
                        pending_type_list *list, lf_all *p, void *d )
{
    struct search_data  *sd = d;
    const char          *name;
    size_t              name_len;
    imp_sym_handle      *ish;
    search_result       sr;
    numeric_leaf        val;
    unsigned            count;
    unsigned            idx;
    strcompn_fn         *scompn;

    switch( swi ) {
    case SWI_INHERIT_START:
        switch( list->code ) {
        case LF_BCLASS:
            p = VMBlock( iih, list->curr, list->len );
            if( p == NULL )
                return( WR_FAIL );
            hllGetNumLeaf( &p->bclass + 1, &val );
            sd->adj_offset += val.int_val;
            break;
        default:
            sd->adj_type = list->curr;
            break;
        }
        return( WR_CONTINUE );
    case SWI_INHERIT_END:
        switch( list->code ) {
        case LF_BCLASS:
            p = VMBlock( iih, list->curr, list->len );
            if( p == NULL )
                return( WR_FAIL );
            hllGetNumLeaf( &p->bclass + 1, &val );
            sd->adj_offset -= val.int_val;
            break;
        default:
            sd->adj_type = 0;
            break;
        }
        return( WR_CONTINUE );
    }
    /* SWI_SYMBOL */
    switch( p->common.code ) {
    case LF_MEMBER:
    case LF_STMEMBER:
    case LF_ENUMERATE:
    case LF_ONEMETHOD:
        count = 1;
        break;
    case LF_METHOD:
        count = p->method.f.count;
        break;
    default:
        return( WR_CONTINUE );
    }
    if( TypeVMGetName( iih, list->curr, &name, &name_len, NULL ) != DS_OK ) {
        sd->sr = SR_FAIL;
        return( WR_FAIL );
    }
    if( sd->li->name.len != name_len )
        return( WR_CONTINUE );
    sr = SR_NONE;
    scompn = ( sd->li->case_sensitive ) ? strncmp : strnicmp;
    if( scompn( name, sd->li->name.start, name_len ) == 0 ) {
        sr = SR_EXACT;
    }
    if( sr != SR_NONE ) {
        sd->sr = SR_EXACT;
        for( idx = 0; idx < count; ++idx ) {
            ish = DCSymCreate( iih, sd->d );
            if( ish == NULL )
                return( WR_FAIL );
            ish->mfunc_idx = idx;
            ish->handle = list->curr;
            ish->containing_type = list->containing_type;
            ish->adjustor_type = sd->adj_type;
            ish->adjustor_offset = sd->adj_offset;
        }
    }
    return( WR_CONTINUE );
}

search_result hllTypeSearchNestedSym( imp_image_handle *iih, imp_type_handle *ith, lookup_item *li, void *d )
{
    lf_all              *p;
    struct search_data  data;
    unsigned            list_idx;
    virt_mem            base;

    if( ith->handle == 0 )
        return( SR_NONE );
    p = VMBlock( iih, ith->handle, sizeof( *p ) );
    if( p == NULL )
        return( SR_FAIL );
    switch( p->common.code ) {
    case LF_CLASS:
    case LF_STRUCTURE:
        list_idx = p->class_.f.field;
        break;
    case LF_UNION:
        list_idx = p->union_.f.field;
        break;
    case LF_ENUM:
        list_idx = p->enum_.f.fList;
        break;
    default:
        return( SR_NONE );
    }
    base = TypeIndexVM( iih, list_idx );
    if( base == 0 )
        return( SR_FAIL );
    data.ith = ith;
    data.li = li;
    data.d  = d;
    data.sr = SR_NONE;
    data.adj_offset = 0;
    data.adj_type = 0;
    if( TypeListWalk( iih, base, ith->handle, SymSearch, &data ) == WR_FAIL ) {
        return( SR_FAIL );
    }
    return( data.sr );
}


walk_result DIPIMPENTRY( WalkTypeList )( imp_image_handle *iih,
                                         imp_mod_handle imh, DIP_IMP_TYPE_WALKER *wk,
                                         imp_type_handle *ith, void *d )
{
    unsigned long       count;
    cv_directory_entry  *cde;
    virt_mem            array_vm;
    unsigned_32         *array_p;
    walk_result         wr;

    wr = WR_CONTINUE;
    if( im == IMH_GBL ) {
        cde = (cv_directory_entry *)hllFindDirEntry( iih, IMH_GBL, sstGlobalTypes );
        if( cde != NULL ) {
            array_vm = cde->lfo + sizeof( unsigned_32 );
            array_p = VMBlock( iih, array_vm, sizeof( *array_p ) );
            if( array_p == NULL ) {
                wr = WR_FAIL;
            } else {
                ith->array_dim = 0;
                ith->idx = CV_FIRST_USER_TYPE;
                for( count = *array_p; count > 0; count-- ) {
                    array_vm += sizeof( *array_p );
                    array_p = VMBlock( iih, array_vm, sizeof( *array_p ) );
                    if( array_p == NULL ) {
                        wr = WR_FAIL;
                        break;
                    }
                    ith->handle = iih->types_base + *array_p + sizeof( unsigned_16 );
                    wr = wk( iih, ith, d );
                    if( wr != WR_CONTINUE )
                        break;
                    ith->idx++;
                }
            }
        }
    }
    return( wr );
}

imp_mod_handle DIPIMPENTRY( TypeMod )( imp_image_handle *iih, imp_type_handle *ith )
{
    iih = iih;
    ith = ith;
    return( IMH_GBL );
}

static int IsFortranModule( imp_image_handle *iih, location_context *lc )
{
    /* No fortran support */
    iih = iih;
    lc = lc;
    return( 0 );
}

dip_status hllTypeInfo( imp_image_handle *iih, imp_type_handle *ith,
                        location_context *lc, dig_type_info *ti )
{
    imp_type_handle             real_ith;
    dip_status                  ds;
    lf_all                      *p;
    numeric_leaf                val;
    array_info                  ai;
    int                         maybe_string;

    ti->kind = TK_NONE;
    ti->size = 0;
    ti->modifier = TM_NONE;
    ti->deref = false;
    ds = TypeReal( iih, ith, &real, NULL );
    if( ds != DS_OK )
        return( ds );
    if( real.idx < CV_FIRST_USER_TYPE ) {
        TypePrimitiveInfo( real.idx, ti );
        return( DS_OK );
    }
    /* map in enough to get integer values from numeric leaves */
    p = VMBlock( iih, real.handle, sizeof( *p ) + sizeof( unsigned_32 ) * 2 );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    maybe_string = 0;
    switch( p->common.code ) {
    case LF_POINTER:
        ti->kind = TK_POINTER;
        switch( p->pointer.f.attr.f.type ) {
        case CV_NEAR:
            ti->modifier = TM_NEAR;
            ti->size = sizeof( addr32_off );
            break;
        case CV_FAR:
            ti->modifier = TM_FAR;
            ti->size = sizeof( addr32_ptr );
            break;
        case CV_HUGE:
            ti->modifier = TM_HUGE;
            ti->size = sizeof( addr32_ptr );
            break;
        case CV_BASESEG:
        case CV_BASEVAL:
        case CV_BASESEGVAL:
        case CV_BASESYM:
        case CV_BASESEGSYM:
        case CV_BASETYPE:
        case CV_BASESELF:
            ti->modifier = TM_NEAR;
            ti->size = p->pointer.f.attr.f.isflat32 ?
                                sizeof( addr48_off ) : sizeof( addr32_off );
            break;
        case CV_NEAR32:
            ti->modifier = TM_NEAR;
            ti->size = sizeof( addr48_off );
            break;
        case CV_FAR32:
            ti->modifier = TM_FAR;
            ti->size = sizeof( addr48_ptr );
            break;
        }
        if( p->pointer.f.attr.f.mode == CV_REF ) {
            ti->deref = true;
        }
        break;
    case LF_ARRAY:
        ti->kind = TK_ARRAY;
        if( p->array.f.elemtype == PT_REALLY_CHAR ) {
            maybe_string = 1;
        }
        hllGetNumLeaf( &p->array + 1, &val );
        ti->size = val.int_val;
        break;
    case LF_CLASS:
    case LF_STRUCTURE:
        ti->kind = TK_STRUCT;
        hllGetNumLeaf( &p->class_ + 1, &val );
        ti->size = val.int_val;
        break;
    case LF_UNION:
        ti->kind = TK_STRUCT;
        hllGetNumLeaf( &p->union_ + 1, &val );
        ti->size = val.int_val;
        break;
    case LF_ENUM:
        ds = hllTypeIndexFillIn( iih, p->enum_.f.type, &real );
        if( ds != DS_OK )
            return( ds );
        ds = hllTypeInfo( iih, &real, lc, ti );
        if( ds != DS_OK )
            return( ds );
        ti->kind = TK_ENUM;
        break;
    case LF_PROCEDURE:
    case LF_MFUNCTION:
        ti->kind = TK_FUNCTION;
        break;
    case LF_COBOL0:
    case LF_COBOL1:
    case LF_BARRAY:
        break;
    case LF_LABEL:
        ti->kind = TK_ADDRESS;
        if( p->label.f.mode ) {
            ti->modifier = TM_FAR;
            ti->size = sizeof( addr_seg );
        } else {
            ti->modifier = TM_NEAR;
        }
        //NYI: 16/32 bit?
        ti->size += sizeof( addr32_off );
        break;
    case LF_NULL:
    case LF_NOTTRANS:
        break;
    case LF_DIMARRAY:
        ti->kind = TK_ARRAY;
        if( p->dimarray.f.utype == PT_REALLY_CHAR ) {
            maybe_string = 1;
        }
        ds = hllTypeArrayInfo( iih, ith, lc, &ai, NULL );
        if( ds != DS_OK )
            return( ds );
        if( ai.column_major && ai.num_dims > 1 ) {
            real = *ith;
            real.array_dim += ai.num_dims - 1;
            ds = hllTypeArrayInfo( iih, ith, lc, &ai, NULL );
            if( ds != DS_OK ) {
                return( ds );
            }
        }
        ti->size = ai.num_elts * ai.stride;
        break;
    case LF_PRECOMP:
    case LF_ENDPRECOMP:
    case LF_OEM:
    case LF_ARGLIST:
    case LF_DEFARG:
    case LF_LIST:
    case LF_FIELDLIST:
        break;
    case LF_BITFIELD:
        ds = hllTypeIndexFillIn( iih, p->bitfield.f.type, &real );
        if( ds != DS_OK )
            return( ds );
        return( hllTypeInfo( iih, &real, lc, ti ) );
    case LF_REFSYM:
        break;
    default:
        hllConfused();
        return( DS_ERR | DS_FAIL );
    }
    if( maybe_string && IsFortranModule( iih, lc ) ) {
        ti->kind = TK_STRING;
    }
    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeInfo )( imp_image_handle *iih, imp_type_handle *ith,
                                    location_context *lc, dig_type_info *ti )
{
    return( hllTypeInfo( iih, ith, lc, ti ) );
}

dip_status hllTypeBase( imp_image_handle *iih, imp_type_handle *ith, imp_type_handle *base_ith )
{
    dip_status                  ds;
    cv_primitive                prim;
    lf_all                      *p;
    imp_type_handle             dim_ith;
    unsigned                    save_idx;

    ds = TypeReal( iih, ith, base_ith, NULL );
    if( ds != DS_OK )
        return( ds );
    if( base_ith->idx < CV_FIRST_USER_TYPE ) {
        prim.s = base_ith->idx;
        prim.f.mode = CV_DIRECT;
        base_ith->idx = prim.s;
        return( DS_OK );
    }
    p = VMBlock( iih, base_ith->handle, sizeof( *p ) );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    switch( p->common.code ) {
    case LF_POINTER:
        return( hllTypeIndexFillIn( iih, p->pointer.f.type, base_ith ) );
    case LF_ARRAY:
        return( hllTypeIndexFillIn( iih, p->array.f.elemtype, base_ith ) );
    case LF_CLASS:
    case LF_STRUCTURE:
    case LF_UNION:
        return( DS_ERR | DS_BAD_PARM );
    case LF_ENUM:
        return( hllTypeIndexFillIn( iih, p->enum_.f.type, base_ith ) );
    case LF_PROCEDURE:
        return( hllTypeIndexFillIn( iih, p->procedure.f.rvtype, base_ith ) );
    case LF_MFUNCTION:
        return( hllTypeIndexFillIn( iih, p->mfunction.f.rvtype, base_ith ) );
    case LF_COBOL0:
    case LF_COBOL1:
    case LF_BARRAY:
    case LF_LABEL:
    case LF_NULL:
    case LF_NOTTRANS:
        return( DS_ERR | DS_BAD_PARM );
    case LF_DIMARRAY:
        save_idx = p->dimarray.f.utype;
        ds = hllTypeIndexFillIn( iih, p->dimarray.f.diminfo, &dim_ith );
        if( ds != DS_OK )
            return( ds );
        p = VMBlock( iih, dim_ith.handle, sizeof( *p ) );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        base_ith->array_dim++;
        if( base_ith->array_dim < p->dimconu.f.rank ) {
            return( DS_OK );
        }
        return( hllTypeIndexFillIn( iih, save_idx, base_ith ) );
    case LF_PRECOMP:
    case LF_ENDPRECOMP:
    case LF_OEM:
    case LF_ARGLIST:
    case LF_DEFARG:
    case LF_LIST:
    case LF_FIELDLIST:
        return( DS_ERR | DS_BAD_PARM );
    case LF_BITFIELD:
        ds = hllTypeIndexFillIn( iih, p->bitfield.f.type, base_ith );
        if( ds != DS_OK )
            return( ds );
        return( hllTypeBase( iih, base_ith, base_ith ) );
    case LF_REFSYM:
        break;
    default:
        hllConfused();
        return( DS_ERR | DS_FAIL );
    }
    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeBase )( imp_image_handle *iih, imp_type_handle *ith,
                                    imp_type_handle *base_ith,
                                    location_context *lc, location_list *ll )
{
    /* unused parameters */ (void)lc; (void)ll;

    return( hllTypeBase( iih, ith, base_ith ) );
}

typedef union {
    unsigned_16 idx[2];
    signed_8    s8[2];
    signed_16   s16[2];
    signed_32   s32[2];
} bound_data;

static dip_status GetBound( imp_image_handle *iih, virt_mem vm, unsigned size,
                                location_context *lc, long *bound )
{
    /* unused parameters */ (void)iih; (void)vm; (void)size; (void)lc; (void)bound;

#if 0 /* FIXME */
    dip_status          ds;
    imp_sym_handle      bnd;
    bound_data          data;
    location_list       src;
    location_list       dst;

    ds = hllSymFillIn( iih, &bnd, vm + sizeof( lf_common ) );
    if( ds != DS_OK )
        return( ds );
    ds = hllSymLocation( iih, &bnd, lc, &src );
    if( ds == DS_OK ) {
        hllLocationCreate( &dst, LT_INTERNAL, &data );
        ds = DCAssignLocation( &dst, &src, size );
    } else {
        ds = hllSymValue( iih, &bnd, lc, &data );
    }
    if( ds != DS_OK )
        return( ds );
    switch( size ) {
    case 1:
        *bound = data.s8[0];
        break;
    case 2:
        *bound = data.s16[0];
        break;
    case 4:
        *bound = data.s32[0];
        break;
    default:
        hllConfused();
        return( DS_ERR | DS_FAIL );
    }
    return( DS_OK );
#else
    return( DS_ERR | DS_FAIL );
#endif
}

static dip_status GetArrayRange( imp_image_handle *iih, location_context *lc,
                unsigned code, virt_mem dim_hdl, unsigned idx, unsigned i,
                array_info *ai )
{
    bound_data          *di;
    dip_status          ds;
    imp_type_handle     real_ith;
    dig_type_info       ti;
    long                hi_bound;

    ds = hllTypeIndexFillIn( iih, idx, &real );
    if( ds != DS_OK )
        return( ds );
    ds = hllTypeInfo( iih, &real, lc, &ti );
    if( ds != DS_OK )
        return( ds );
    switch( code ) {
    case LF_DIMCONU:
        ai->low_bound = ai->column_major;
        di = VMBlock( iih, dim_hdl + sizeof( lf_dimconu ) + ti.size * i, ti.size );
        if( di == NULL )
            return( DS_ERR | DS_FAIL );
        switch( ti.size ) {
        case 1:
            hi_bound = di->s8[0];
            break;
        case 2:
            hi_bound = di->s16[0];
            break;
        case 4:
            hi_bound = di->s32[0];
            break;
        default:
            hllConfused();
            return( DS_ERR | DS_FAIL );
        }
        break;
    case LF_DIMCONLU:
        di = VMBlock( iih, dim_hdl + sizeof( lf_dimconlu ) + 2 * ti.size * i, ti.size * 2 );
        if( di == NULL )
            return( DS_ERR | DS_FAIL );
        switch( ti.size ) {
        case 1:
            ai->low_bound = di->s8[0];
            hi_bound = di->s8[1];
            break;
        case 2:
            ai->low_bound = di->s16[0];
            hi_bound = di->s16[1];
            break;
        case 4:
            ai->low_bound = di->s32[0];
            hi_bound = di->s32[1];
            break;
        default:
            hllConfused();
            return( DS_ERR | DS_FAIL );
        }
        break;
    case LF_DIMVARU:
        ai->low_bound = ai->column_major;
        di = VMBlock( iih, dim_hdl + sizeof( lf_dimvaru ) + sizeof( di->idx[0] ) * i, sizeof( di->idx[0] ) );
        if( di == NULL )
            return( DS_ERR | DS_FAIL );
        ds = hllTypeIndexFillIn( iih, di->idx[0], &real );
        if( ds != DS_OK )
            return( ds );
        ds = GetBound( iih, real.handle, ti.size, lc, &hi_bound );
        if( ds != DS_OK )
            return( ds );
        break;
    case LF_DIMVARLU:
        di = VMBlock( iih, dim_hdl + sizeof( lf_dimvaru ) + sizeof( di->idx ) * i, sizeof( di->idx ) );
        if( di == NULL )
            return( DS_ERR | DS_FAIL );
        code = di->idx[1];      /* save from VM system */
        ds = hllTypeIndexFillIn( iih, di->idx[0], &real );
        if( ds != DS_OK )
            return( ds );
        ds = GetBound( iih, real.handle, ti.size, lc, &ai->low_bound );
        if( ds != DS_OK )
            return( ds );
        ds = hllTypeIndexFillIn( iih, code, &real );
        if( ds != DS_OK )
            return( ds );
        ds = GetBound( iih, real.handle, ti.size, lc, &hi_bound );
        if( ds != DS_OK )
            return( ds );
        break;
    default:
        hllConfused();
        return( DS_ERR | DS_FAIL );
    }
    ai->num_elts = hi_bound - ai->low_bound + 1;
    return( DS_OK );
}

static dip_status hllTypeArrayInfo( imp_image_handle *iih,
                        imp_type_handle *array_ith, location_context *lc,
                        array_info *ai, imp_type_handle *index_ith )
{
    lf_all              *p;
    numeric_leaf        val;
    imp_type_handle     real_ith;
    unsigned            idx;
    dig_type_info       ti;
    dip_status          ds;
    unsigned            code;
    virt_mem            dim_hdl;
    unsigned            utype;
    unsigned            i;

    ds = TypeReal( iih, array_ith, &real, NULL );
    if( ds != DS_OK )
        return( ds );
    ai->low_bound = 0;
    ai->num_elts = 0;
    ai->stride = 0;
    ai->num_dims = 0;
    ai->column_major = IsFortranModule( iih, lc );
    /* map in enough to get integer values from numeric leaves */
    p = VMBlock( iih, real.handle, sizeof( *p ) + sizeof( unsigned_32 ) * 2 );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    switch( p->common.code ) {
    case LF_ARRAY:
        idx = p->array.f.idxtype;
        ai->num_dims = 1;
        hllGetNumLeaf( &p->array + 1, &val );
        ds = hllTypeIndexFillIn( iih, p->array.f.elemtype, &real );
        if( ds != DS_OK )
            return( ds );
        ds = hllTypeInfo( iih, &real, lc, &ti );
        if( ds != DS_OK )
            return( ds );
        ai->stride = ti.size;
        ai->num_elts = val.int_val / ti.size;
        break;
    case LF_DIMARRAY:
        utype = p->dimarray.f.utype;
        ds = hllTypeIndexFillIn( iih, p->dimarray.f.diminfo, &real );
        if( ds != DS_OK )
            return( ds );
        p = VMBlock( iih, real.handle, sizeof( *p ) + sizeof( unsigned_32 ) * 2 );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        idx = p->dimconu.f.index;
        code = p->common.code;
        dim_hdl = real.handle;
        ai->num_dims = p->dimconu.f.rank - array_ith->array_dim;
        if( ai->column_major ) {
            ds = hllTypeIndexFillIn( iih, utype, &real );
            if( ds != DS_OK )
                return( ds );
            ds = hllTypeInfo( iih, &real, lc, &ti );
            if( ds != DS_OK )
                return( ds );
            ai->stride = ti.size;
            for( i = 0; i < array_ith->array_dim; ++i ) {
                ds = GetArrayRange( iih, lc, code, dim_hdl, idx, i, ai );
                if( ds != DS_OK )
                    return( ds );
                ai->stride *= ai->num_elts;
            }
        } else {
            ds = hllTypeBase( iih, array_ith, &real );
            if( ds != DS_OK )
                return( ds );
            ds = hllTypeInfo( iih, &real, lc, &ti );
            if( ds != DS_OK )
                return( ds );
            ai->stride = ti.size;
        }
        ds = GetArrayRange( iih, lc, code, dim_hdl, idx, array_ith->array_dim, ai );
        if( ds != DS_OK )
            return( ds );
        break;
    default:
        return( DS_FAIL );
    }
    if( index_ith != NULL ) {
        return( hllTypeIndexFillIn( iih, idx, index_ith ) );
    }
    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeArrayInfo )( imp_image_handle *iih,
                        imp_type_handle *array_ith, location_context *lc,
                        array_info *ai, imp_type_handle *index_ith )
{
    return( hllTypeArrayInfo( iih, array_ith, lc, ai, index_ith ) );
}

dip_status DIPIMPENTRY( TypeProcInfo )( imp_image_handle *iih,
                imp_type_handle *proc_ith, imp_type_handle *parm_ith, unsigned n )
{
    lf_all              *p;
    dip_status          ds;
    unsigned_16         *arg_types;
    unsigned            rv_idx;
    unsigned            al_idx;
    unsigned            parm_count;

    ds = TypeReal( iih, proc_ith, parm_ith, &p );
    if( ds != DS_OK )
        return( ds );
    if( parm_ith->idx < CV_FIRST_USER_TYPE )
        return( DS_FAIL );
    switch( p->common.code ) {
    case LF_PROCEDURE:
        rv_idx = p->procedure.f.rvtype;
        al_idx = p->procedure.f.arglist;
        parm_count = p->procedure.f.parms;
        break;
    case LF_MFUNCTION:
        rv_idx = p->mfunction.f.rvtype;
        al_idx = p->mfunction.f.arglist;
        parm_count = p->mfunction.f.parms;
        break;
    default:
        return( DS_FAIL );
    }
    if( n == 0 ) {
        return( hllTypeIndexFillIn( iih, rv_idx, parm_ith ) );
    }
    if( n <= parm_count ) {
        ds = hllTypeIndexFillIn( iih, al_idx, parm_ith );
        if( ds != DS_OK )
            return( ds );
        p = VMRecord( iih, parm_ith->handle - sizeof( unsigned_16 ), NULL, NULL );
        if( p == NULL )
            return( DS_ERR | DS_FAIL );
        p = (lf_all *)((unsigned_16 *)p + 1);
        if( p->common.code != LF_ARGLIST )
            return( DS_ERR | DS_FAIL );
        arg_types = (void *)(&p->arglist + 1);
        return( hllTypeIndexFillIn( iih, arg_types[n - 1], parm_ith ) );
    }
    return( DS_NO_PARM );
}

dip_status DIPIMPENTRY( TypePtrAddrSpace )( imp_image_handle *iih,
                    imp_type_handle *ith, location_context *lc, address *a )
{
    /* unised parameters */ (void)iih; (void)ith; (void)lc; (void)a;

#if 0
    lf_all              *p;
    imp_type_handle     real_ith;
    dip_status          ds;
    imp_sym_handle      ish;
    location_list       ll;
    dig_type_info       ti;
    unsigned            ptype;
    union {
        addr32_ptr      addr32;
        addr48_ptr      addr48;
        addr_seg        seg;
    }                   tmp;
    location_list       tmp_ll;

    ds = TypeReal( iih, ith, &real, NULL );
    if( ds != DS_OK )
        return( ds );
    if( real.idx < CV_FIRST_USER_TYPE )
        return( DS_FAIL );
    p = VMBlock( iih, real.handle, sizeof( p->pointer ) + sizeof( s_common ) );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    if( p->common.code != LF_POINTER )
        return( DS_FAIL );
    ptype = p->pointer.f.attr.f.type;
    switch( ptype ) {
    case CV_BASESEG:
        a->mach.segment = *(unsigned_16 *)(&p->pointer + 1);
        a->mach.offset  = 0;
        hllMapLogical( iih, a );
        return( DS_OK );
    case CV_BASEVAL:
    case CV_BASESEGVAL:
    case CV_BASESYM:
    case CV_BASESEGSYM:
        ds = hllSymFillIn( iih, &ish, real.handle + sizeof( p->pointer ) );
        if( ds != DS_OK )
            return( ds );
        ds = hllSymLocation( iih, &ish, lc, &ll );
        if( ds != DS_OK )
            return( ds );
        *a = ll.e[0].u.addr;
        switch( ptype ) {
        case CV_BASESEGSYM:
            a->mach.offset = 0;
            /* fall through */
        case CV_BASESYM:
            return( DS_OK );
        }
        ds = hllSymType( iih, &ish, &real );
        if( ds != DS_OK )
            return( ds );
        ds = hllTypeInfo( iih, &real, lc, &ti );
        if( ds != DS_OK )
            return( ds );
        hllLocationCreate( &tmp_ll, LT_INTERNAL, &tmp );
        ds = DCAssignLocation( &ll, &tmp_ll, ti.size );
        if( ds != DS_OK ) {
            DCStatus( ds );
            return( ds );
        }
        if( ti.kind != TK_POINTER ) {
            a->mach.segment = tmp.seg;
            a->mach.offset  = 0;
        } else if( ti.size == sizeof( tmp.addr32 ) ) {
            a->mach.segment = tmp.addr32.segment;
            a->mach.offset  = tmp.addr32.offset;
        } else {
            a->mach.segment = tmp.addr48.segment;
            a->mach.offset  = tmp.addr48.offset;
        }
        if( ptype == CV_BASESEGVAL )
            a->mach.offset = 0;
        return( DS_OK );
    case CV_BASETYPE:
        //NYI: ????
        /* fall through */
    case CV_BASESELF:
        return( DS_OK );
    }
#endif
    return( DS_FAIL );
}

struct thunk_data {
    unsigned    adj_offset;
    virt_mem    adj_type;
    unsigned    derived_vm;
    unsigned    found : 1;
};

static walk_result ThunkSearch( imp_image_handle *iih, sym_walk_info swi,
                        pending_type_list *list, lf_all *p, void *d )
{
    struct thunk_data   *td = d;
    numeric_leaf        val;
    unsigned            struct_idx;

    if( td->found )
        return( WR_STOP );
    switch( swi ) {
    case SWI_INHERIT_END:
        switch( list->code ) {
        case LF_BCLASS:
            p = VMBlock( iih, list->curr, list->len );
            if( p == NULL )
                return( WR_FAIL );
            hllGetNumLeaf( &p->bclass + 1, &val );
            td->adj_offset -= val.int_val;
            break;
        default:
            td->adj_type = 0;
            break;
        }
        return( WR_CONTINUE );
    case SWI_SYMBOL:
        return( WR_CONTINUE );
    }
    /* SWI_INHERIT_START */
    p = VMBlock( iih, list->curr, list->len );
    if( p == NULL )
        return( WR_FAIL );
    switch( list->code ) {
    case LF_BCLASS:
        hllGetNumLeaf( &p->bclass + 1, &val );
        td->adj_offset += val.int_val;
        struct_idx = p->bclass.f.type;
        break;
    default:
        td->adj_type = list->curr;
        struct_idx = p->vbclass.f.btype;
        break;
    }
    if( TypeIndexVM( iih, struct_idx ) == td->derived_vm ) {
        td->found = 1;
        return( WR_STOP );
    }
    return( WR_CONTINUE );
}

dip_status DIPIMPENTRY( TypeThunkAdjust )( imp_image_handle *iih,
                        imp_type_handle *base_ith, imp_type_handle *derived_ith,
                        location_context *lc, address *addr )
{
    lf_all              *p;
    struct thunk_data   data;
    unsigned            list_idx;
    virt_mem            list_vm;
    location_list       ll;
    unsigned long       disp;
    dip_status          ds;

    if( base_ith->idx < CV_FIRST_USER_TYPE || derived_ith->idx < CV_FIRST_USER_TYPE ) {
        return( DS_FAIL );
    }
    if( base_ith->handle == derived_ith->handle )
        return( DS_FAIL );
    p = VMBlock( iih, derived_ith->handle, sizeof( *p ) );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    switch( p->common.code ) {
    case LF_CLASS:
    case LF_STRUCTURE:
        break;
    default:
        return( DS_FAIL );
    }

    p = VMBlock( iih, base_ith->handle, sizeof( *p ) );
    if( p == NULL )
        return( DS_ERR | DS_FAIL );
    switch( p->common.code ) {
    case LF_CLASS:
    case LF_STRUCTURE:
        list_idx = p->class_.f.field;
        break;
    default:
        return( DS_FAIL );
    }
    list_vm = TypeIndexVM( iih, list_idx );
    if( list_vm == 0 )
        return( DS_ERR | DS_FAIL );
    data.derived_vm = derived_ith->handle - sizeof( unsigned_16 );
    data.adj_offset = 0;
    data.adj_type = 0;
    data.found = 0;
    switch( TypeListWalk( iih, list_vm, base_ith->handle, ThunkSearch, &data ) ) {
    case WR_FAIL:
        return( DS_ERR | DS_FAIL );
    }
    if( data.found ) {
        if( data.adj_type != 0 ) {
            hllLocationCreate( &ll, LT_ADDR, addr );
            ds = GetVirtBaseDisp( iih, data.adj_type, lc, ll, &disp );
            if( ds != DS_OK )
                return( ds );
            addr->mach.offset += disp;
        }
        addr->mach.offset += data.adj_offset;
        return( DS_OK );
    }
    return( DS_FAIL );
}

int DIPIMPENTRY( TypeCmp )( imp_image_handle *iih, imp_type_handle *ith1, imp_type_handle *ith2 )
{
    /* unised parameters */ (void)iih;

    if( ith2->idx < ith1->idx )
        return( -1 );
    if( ith2->idx > ith1->idx )
        return( 1 );
    return( 0 );
}

size_t DIPIMPENTRY( TypeName )( imp_image_handle *iih, imp_type_handle *ith,
                unsigned num, symbol_type *tag, char *buff, size_t buff_size )
{
    lf_all              *p;
    const char          *name;
    size_t              len;
    imp_type_handle     real_ith;

    if( num != 0 )
        return( 0 );
    if( TypeReal( iih, ith, &real_ith, &p ) != DS_OK ) {
        return( 0 );
    }
    if( real_ith.idx != 0 )
        return( 0 );
    switch( p->common.code ) {
    case LF_CLASS:
        *tag = ST_CLASS_TAG;
        break;
    case LF_STRUCTURE:
        *tag = ST_STRUCT_TAG;
        break;
    case LF_UNION:
        *tag = ST_UNION_TAG;
        break;
    case LF_ENUM:
        *tag = ST_ENUM_TAG;
        break;
    default:
        *tag = ST_NONE;
        break;
    }
    if( TypeVMGetName( iih, real_ith.handle, &name, &len, NULL ) != DS_OK ) {
        return( 0 );
    }
    return( hllNameCopy( buff, name, buff_size, len ) );
}

dip_status DIPIMPENTRY( TypeAddRef )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unised parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeRelease )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unised parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeFreeAll )( imp_image_handle *iih )
{
    /* unised parameters */ (void)iih;

    return( DS_OK );
}
