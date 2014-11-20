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
* Description:  DWARF implementation of the DIP symbol handling interface.
*
****************************************************************************/


#include "dfdip.h"
#include "dfld.h"
#include "dfmod.h"
#include "dfmodinf.h"
#include "dfaddr.h"
#include "dfaddsym.h"
#include "dftype.h"
#include "dfloc.h"
#include "dfscope.h"
#include "dfhash.h"
#include "demangle.h"
#include "dfsym.h"
#include "clibext.h"


imp_mod_handle  DIGENTRY DIPImpSymMod( imp_image_handle *ii, imp_sym_handle *is )
/*******************************************************************************/
{
    /* Return the module that the implementation symbol handle comes from. */
    ii = ii;
    return( is->im );
}


unsigned        DIGENTRY DIPImpSymName( imp_image_handle *ii,
                        imp_sym_handle *is, location_context *lc,
                        symbol_name sn, char *buff, unsigned max )
/****************************************************************/
{
    /*
        SN_SOURCE:
                The name of the symbol as it appears in the source code.

        SN_OBJECT:
                The name of the symbol as it appeared to the linker.

        SN_DEMANGLED:
                C++ names, with full typing (essentially it looks like
                a function prototype). If the symbol is not a C++ symbol
                (not mangled), return zero for the length.

        SN_EXPRESSION:
                Return whatever character string is necessary such that
                when scanned in an expression, the symbol handle can
                be reconstructed.

        SS_BLOCK:
                Not possible. Will never happen.
    */
    char        *name;
    unsigned    demangled_len;
    unsigned    len = 0;

    lc = lc;
//TODO: what's lc for?
    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    switch( sn ) {
    case SN_SOURCE:
    case SN_OBJECT:
        len = DRGetNameBuff( is->sym, buff, max );
        if( len == 0 ) {
            DCStatus( DS_FAIL );
            return( 0 );
        }
        --len;
        if( max != 0 && len > max ) {
           buff[max-1] = '\0';
        }
        break;
    case SN_SCOPED:
        len =  DRGetScopedNameBuff( is->sym, buff, max );
        if( len == 0 ) {
            DCStatus( DS_FAIL );
            return( 0 );
        }
        --len;
        if( max != 0 && len > max ) {
           buff[max-1] = '\0';
        }
        break;
    case SN_DEMANGLED:
        if( IM2MODI( ii, is->im )->lang == DR_LANG_CPLUSPLUS ) {
            name = DRDecoratedName( is->sym, 0 );
            if( name == NULL ) {
                DCStatus( DS_FAIL );
                return( 0 );
            }
            len = NameCopy( buff, name, max );
            DCFree( name );
        } else if( buff == NULL ) {
            return( 0 );
        } else {
            len = DRGetNameBuff( is->sym, buff, max );
            if( __is_mangled( buff, len ) ) {
                demangled_len = __demangle_l( buff, len, NULL, 0 );
                if( demangled_len > max ) {
                    demangled_len = max;
                }
                name = DCAlloc( demangled_len );
                __demangle_l( buff, len, name, demangled_len );
                strncpy( buff, name, max );
                DCFree( name );
                return( demangled_len );
            } else {
                return( 0 );
            }
        }
        break;
    case SN_EXPRESSION:
        return( 0 );
    }
    return( len );
}


dip_status      DIGENTRY DIPImpSymType( imp_image_handle *ii,
                    imp_sym_handle *is, imp_type_handle *it )
/***********************************************************/
{
    /* Get the implementation type handle for the type of the given symbol. */
    dip_status  ret;

    DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
    if( is->state == DF_NOT ) {
        is->stype = DRGetTagType( is->sym );
    }
    switch( is->stype ) {
    case DR_TAG_FUNCTION:
    case DR_TAG_CLASS:
    case DR_TAG_ENUM:
        it->type = is->sym;
        break;
    default:
        it->type = DRGetTypeAT( is->sym );
        break;
    }
    ret = DS_OK;
    it->im = is->im;
    if( it->type ) {
        it->state = DF_NOT;
    } else {
        it->state = DF_SET;         // default the type
        it->sub_array = FALSE;
        it->typeinfo.kind = DR_TYPEK_DATA;
        it->typeinfo.size = 0;
        it->typeinfo.mclass = DR_MOD_NONE;
        if( is->stype == DR_TAG_LABEL ) {
            it->typeinfo.kind = DR_TYPEK_CODE;
        }
    }
    return( ret );
}


struct mod_wlk {
    imp_image_handle    *ii;
    seg_list            *addr_sym;
    imp_mod_handle      im;
    address             a;
    dr_handle           sym;
    search_result       ret;
};

static bool AMod( dr_handle sym, void *_d, dr_search_context *cont )
/******************************************************************/
{
//TODO: no segments, better TAG_label
    struct mod_wlk  *d = _d;
    uint_32         offset;
    uint_32         seg;
//    bool            ret;
    addrsym_info    info;
    bool            is_segment;

    cont = cont;
//    ret = TRUE;
    is_segment = IM2MODI( d->ii, d->im )->is_segment;
    if( DRGetLowPc( sym, &offset) ) {
        if( !is_segment ) {
            seg = SEG_FLAT; // if flat hoke segment
        } else {
            EvalSeg( d->ii, sym, &seg );
        }
    } else {
        if( !is_segment ) {
            seg = SEG_FLAT; // if flat hoke segment
        } else {
            EvalSeg( d->ii, sym, &seg );
        }
//        ret = EvalSymOffset( d->ii, sym, &offset );
        EvalSymOffset( d->ii, sym, &offset );
    }
    info.map_offset = offset;
    info.sym = sym;
    info.map_seg = seg;
    AddAddrSym( d->addr_sym, &info );
    return( TRUE );
}


seg_list *DFLoadAddrSym( imp_image_handle *ii, imp_mod_handle im )
/****************************************************************/
{
    struct mod_wlk  d;
    seg_list        *addr_sym;
    mod_info        *modinfo;

    modinfo = IM2MODI( ii, im );
    addr_sym = modinfo->addr_sym;
    if( addr_sym->head == NULL ) {  /* no cache */
        FiniAddrSym( addr_sym );    /* kill cache */
        DRSetDebug( ii->dwarf->handle ); /* must do at each interface */
        d.addr_sym = addr_sym;
        d.ii = ii;
        d.im = im;
        d.ret = SR_NONE;
        DRWalkModFunc( modinfo->cu_tag, FALSE, AMod, &d );   /* load cache */
        SortAddrSym( addr_sym );
    }
    return( addr_sym );
}


typedef struct {
    dr_handle   inh;    /* member class handle */
    char        *name;  /* member name         */
    dr_handle   match;  /* handle that matches */
} mem_func_wlk;

static bool AMemFuncSym( void *_df, addrsym_info *info )
/******************************************************/
{
    mem_func_wlk    *df = _df;
    bool            cont;
    char            buff[256];
//    unsigned        len;
    dr_handle       contain;

    cont = TRUE;
    contain  = DRGetContaining( info->sym );
    if( contain ) {
        contain = DRSkipTypeChain( contain );   /* PCH typedef link */
        if( contain == df->inh ) {
//            len =  DRGetNameBuff( info->sym, buff, sizeof( buff ) );
            DRGetNameBuff( info->sym, buff, sizeof( buff ) );
            if( strcmp( buff, df->name ) == 0 ) {
                cont = FALSE;
                df->match = info->sym;
            }
        }
    }
    return( cont );
}


static dr_handle MemFuncLookUp( imp_image_handle *ii,
                                dr_handle sym,
                                dr_handle inh,
                                imp_mod_handle im )
/***************************************************/
{
    mem_func_wlk    df;
    char            buff[256];
    seg_list        *addr_sym;

    DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
    addr_sym = DFLoadAddrSym( ii, im );
    DRGetNameBuff( sym, buff, sizeof( buff ) );
    df.name = buff;
    df.inh  = inh;
    df.match = 0;
    WlkAddrSyms( addr_sym, AMemFuncSym, &df );
    return( df.match );
}


static  addr_seg GetCodeSeg( imp_image_handle *ii )
/*************************************************/
{
    addr_ptr mach;
    mach.segment = SEG_CODE;
    mach.offset = 0;
    DCMapAddr( &mach, ii->dcmap );
    return( mach.segment );
}


dip_status      DIGENTRY DIPImpSymLocation( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, location_list *ll )
/***************************************************************************/
{
    /* Get the location of the given symbol. */
    dip_status       ret;
    address          base; /* base segment & offset */
    uint_32          seg;
    dr_handle        sym;
    bool             is_segment;

    ret = DS_FAIL;
    base = NilAddr;
    is_segment = IM2MODI( ii, is->im )->is_segment;
    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    if( DRGetLowPc( is->sym, &base.mach.offset) ) {
        if( !is_segment ) {
            seg = SEG_CODE; // if flat hoke segment
        } else {
            EvalSeg( ii, is->sym, &seg );
        }
        base.mach.segment = seg;
        DCMapAddr( &base.mach, ii->dcmap );
        LocationCreate( ll, LT_ADDR, &base );
        ret = DS_OK;
    } else {
        switch( is->sclass ) {
        case SYM_MEM:
        case SYM_VIRTF:
        {
            dr_bitfield info;
            int         b_strt;

            ret = SafeDCItemLocation( lc, CI_OBJECT, ll );
            if( ret != DS_OK ) {
                DCStatus( ret );
                return( ret );
            }
            if( is->f.minfo.inh ) {
                DFBaseAdjust( ii, is->f.minfo.root,
                      DRGetTypeAT( is->f.minfo.inh ),lc, &ll->e[0].u.addr );
            }
            ret = EvalLocAdj( ii, lc, is->sym, &ll->e[0].u.addr  );
            if( is->sclass == SYM_VIRTF ) {
                ll->e[0].u.addr.mach.segment = GetCodeSeg( ii );
            } else if( DRGetBitFieldInfo( is->sym, &info ) ) {
                b_strt = 8 * info.byte_size.val.s -
                    (info.bit_offset.val.s + info.bit_size.val.s);
                LocationAdd( ll, b_strt );
                LocationTrunc( ll, info.bit_size.val.s );
            }
            break;
        }
        case SYM_MEMF:
            if( is->f.minfo.inh == 0 ) {
                sym = is->f.minfo.root;
            } else {
                sym = DRGetTypeAT( is->f.minfo.inh );   /* get inherited type */
            }
            sym = DRSkipTypeChain( sym );   /* PCH typedef link */
            sym =  MemFuncLookUp( ii, is->sym, sym, is->im );
            if( sym == 0 ) {
                base = NilAddr; /* for now say it's NULL */
                LocationCreate( ll, LT_ADDR, &base );
                ret = DS_OK;
                break;
            } else {
                is->sym = sym;
            }
            DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
            if( DRGetLowPc( is->sym, &base.mach.offset) ) {
                if( !is_segment ) {
                    seg = SEG_CODE; // if flat hoke segment
                } else {
                    EvalSeg( ii, is->sym, &seg );
                }
                base.mach.segment = seg;
                DCMapAddr( &base.mach, ii->dcmap );
                LocationCreate( ll, LT_ADDR, &base );
                ret = DS_OK;
            }
            break;
        case SYM_MEMVAR:
        case SYM_VAR:
            if( !is_segment ) {
                if( DRIsFunc( is->sym ) ) {
                    seg = SEG_CODE; // if flat hoke segment
                } else {
                    seg = SEG_DATA; // if flat hoke segment
                }
            } else {
                EvalSeg( ii, is->sym, &seg );
            }
            ret = EvalLocation( ii, lc, is->sym, seg, ll );
            break;
        }
    }
    return( ret );
}


dip_status      DIGENTRY DIPImpSymValue( imp_image_handle *ii,
                imp_sym_handle *is, location_context *ic, void *buff )
/********************************************************************/
{
    uint_32     value;
    /* Copy the value of a constant symbol into 'buff'. You can get the
     * size required by doing a SymType followed by a TypeInfo.
     */
    ii = ii;
    ic = ic;
    if( is->sclass != SYM_ENUM ) {
        return( DS_FAIL );
    }
    DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
    if( !DRConstValAT( is->sym, &value ) ) {
        return( DS_FAIL );
    }
    switch( is->f.einfo.size ) {
    case 1:
        *(uint_8 *)buff = value;
        break;
    case 2:
        *(uint_16 *)buff = value;
        break;
    case 4:
        *(uint_32 *)buff = value;
        break;
    default:
        return( DS_FAIL );
    }
    return( DS_OK );
}


dip_status      DIGENTRY DIPImpSymInfo( imp_image_handle *ii,
                imp_sym_handle *is, location_context *lc, sym_info *si )
/**********************************************************************/
{
    /* Get some generic information about a symbol. */
    uint_32         num1;
    uint_32         num2;
    dr_ptr          addr_class;

    lc = lc;
    DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
    if( is->state == DF_NOT ) {
        DRSetDebug( ii->dwarf->handle ); /* must do at each interface */
        is->stype = DRGetTagType( is->sym );
        is->acc   = DRGetAccess( is->sym );
        is->isstatic = DRIsStatic( is->sym );
        is->isartificial = DRIsArtificial( is->sym );
        is->state = DF_SET;
    }
    switch( is->stype ) {
    case DR_TAG_FUNCTION:
        if( is->sclass == SYM_MEMF || is->sclass == SYM_VIRTF ) {
            si->kind = SK_CODE;     /* member functions */
        } else {
            si->kind = SK_PROCEDURE;
            if( DRStartScopeAT( is->sym, &num1 ) ) {
                si->prolog_size  = num1;
            } else {
                si->prolog_size  = 0;
            }
            if( !DRGetLowPc( is->sym, &num1 ) ) {
                num1 = 0;
            }
            if( !DRGetHighPc( is->sym, &num2 ) ) {
                num2 = num1;
            }
            si->rtn_size = num2 - num1;
            si->num_parms = GetParmCount( ii, is->sym );
            si->ret_size = 0;
            si->ret_modifier = TM_NONE;
            si->epilog_size = 0;
            si->rtn_calloc  = 0;
            if( EvalOffset( ii, is->sym, &num1 ) )
                si->ret_addr_offset = num1;
            else
                si->ret_addr_offset = ~0;
            addr_class =  DRGetAddrClass( is->sym );
            switch( addr_class ) {
            case DR_PTR_far32:
            case DR_PTR_far16:
                si->rtn_far = 1;
                break;
            default:
                si->rtn_far = 0;
            }
        }
        break;
    case DR_TAG_CLASS:
    case DR_TAG_ENUM:
    case DR_TAG_TYPEDEF:
        si->kind = SK_TYPE;
        break;
    case DR_TAG_VARIABLE:
        si->kind = SK_DATA;
        break;
    case DR_TAG_LABEL:
        si->kind = SK_CODE;
        break;
    case DR_TAG_NAMESPACE:
        si->kind = SK_NAMESPACE;
        break;
    default:
        si->kind = SK_NONE;
        break;
    }
    si->is_private = 0;
    si->is_protected = 0;
    si->is_public = 0;
    switch( is->acc ) {
    case DR_ACCESS_PUBLIC:
        si->is_public = 1;
        break;
    case DR_ACCESS_PROTECTED:
        si->is_protected = 1;
        break;
    case DR_ACCESS_PRIVATE:
        si->is_private = 1;
        break;
    }
    si->compiler = is->isartificial;
    if( is->isstatic ) {
        si->is_global = 0;
    } else {
        si->is_global = 1;
    }
    switch( is->sclass ) {
    case SYM_MEMVAR:
        si->is_member = 1;
        si->is_static = 1;
        break;
    case SYM_MEM:
    case SYM_MEMF:
    case SYM_VIRTF:
        si->is_member = 1;
        si->is_static = 0;
        break;
    default:
        si->is_member = 0;
        si->is_static = 0;
    }
    return( DS_OK );
}


static bool ARet( dr_handle var, int index, void *_var_ptr )
/**********************************************************/
{
    dr_handle   *var_ptr = _var_ptr;
    char        name[sizeof( ".return" )];
    bool        cont;
    int         len;

    index = index;
    cont = TRUE;
    if( DRIsArtificial( var ) ) {
        len = DRGetNameBuff( var, name, sizeof( ".return" ) );
        if( len == sizeof( ".return" ) ) {
            if( strcmp( name, ".return" ) == 0 ) {
                *var_ptr = var;
                cont = FALSE;
            }
        }
    }
    return( cont );
}


extern dr_handle GetRet(  imp_image_handle *ii, dr_handle proc )
/**************************************************************/
{
    /* Find handle of Watcom return symbol. */
    dr_handle ret = 0;

    DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
    if( DRWalkBlock( proc, DR_SRCH_var, ARet, &ret ) ) {
        ret = 0;
    }
    return( ret );
}


dip_status      DIGENTRY DIPImpSymParmLocation( imp_image_handle *ii,
                    imp_sym_handle *is, location_context *lc,
                    location_list *ll, unsigned n )
/*******************************************************************/
{
    /* Get information about where a routine's parameters/return value
     * are located.
     * If the 'n' parameter is zero, fill in the location list structure
     * pointed at by 'll' with the information on the location of the
     * function's return value. Otherwise fill it in with the location
     * of the n'th parameter.
     */
//TODO: brian only wants regs for now
    dr_handle       parm;
    dip_status      ret;

    DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
    ret = DS_FAIL;
    if( n > 0 ) {
        parm = GetParmN( ii, is->sym, n );
        if( parm ) {
            ret = EvalParmLocation( ii, lc, parm, ll );
        }
    } else if( n == 0 ) {
        //TODO: get ret location
        parm = GetRet( ii, is->sym );
        if( parm ) {
            ret = EvalRetLocation( ii, lc, parm, ll );
        }
    }
    return( ret );
}



static bool AThis( dr_handle var, int index, void *_var_ptr )
/***********************************************************/
{
    dr_handle   *var_ptr = _var_ptr;
    char        name[sizeof( "this" )];
    bool        ret;
    unsigned    len;

    index = index;
    ret = TRUE;
    len =  DRGetNameBuff( var, name, sizeof( "this" ) );
    if( len == sizeof( "this" ) ) {
        if( strcmp( name, "this" ) == 0 ) {
            *var_ptr = var;
            ret = FALSE;
        }
    }
    return( ret );
}


static dr_handle GetThis( imp_image_handle *ii, dr_handle proc )
/**************************************************************/
{
    /* Return handle of the this parmeter. */
    dr_handle   ret = 0;

    DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
    if( DRWalkBlock( proc, DR_SRCH_parm, AThis, &ret ) ) {
        ret = 0;
    }
    return( ret );
}


dip_status      DIGENTRY DIPImpSymObjType( imp_image_handle *ii,
                    imp_sym_handle *is, imp_type_handle *it, dip_type_info *ti )
/******************************************************************************/
{
    /* Fill in the imp_type_handle with the type of the 'this' object
     * for a C++ member function.
     * If 'ti' is not NULL, fill in the dip_type_info with the kind of 'this'
     * pointer that the routine is expecting (near/far, 16/32). If the
     * routine is a static member, set ti->kind to TK_NONE.
     */
    dr_handle   dr_this;
    dr_handle   dr_type;
    dr_typeinfo typeinfo;
    dip_status  ret;

    dr_this = GetThis( ii, is->sym );
    if( dr_this ) {
        DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
        dr_type =  DRGetTypeAT( dr_this );
        if( dr_type ) {
            DRGetTypeInfo( dr_type, &typeinfo );
            MapImpTypeInfo( &typeinfo, ti );
            dr_type =  DRSkipTypeChain( dr_type );
            it->type = DRGetTypeAT( dr_type );
            it->state = DF_NOT;
            it->im = is->im;
            ret = DS_OK;
        } else {
            ret = DS_FAIL;
        }
    } else {
        ti->kind = TK_NONE;
        ret = DS_FAIL;
    }
    return( ret );
}


dip_status      DIGENTRY DIPImpSymObjLocation( imp_image_handle *ii,
                                imp_sym_handle *is, location_context *lc,
                                location_list *ll )
/***********************************************************************/
{
    /* Fill in the location list with the location of the '*this' object
     * for a C++ member function. Return DS_FAIL if it's a static member
     * function.
     */
    dr_handle       dr_this;
    dr_handle       dr_type;
    dr_typeinfo     typeinfo;
    uint_32         seg;
    address         base;   /* base segment & offset */
    location_list   tmp;
    dip_status      ret;
    union{
        addr32_off  n16;
        addr48_off  n32;
        addr32_ptr  f16;
        addr48_ptr  f32;
    } obj_ptr;

    dr_this = GetThis( ii, is->sym );
    if( dr_this ) {
        if( !IM2MODI( ii, is->im )->is_segment ) {
            seg = SEG_DATA; // if flat hoke segment
        } else {
            EvalSeg( ii, is->sym, &seg );
        }
        ret = EvalLocation( ii, lc, dr_this, seg, &tmp );
        if( ret == DS_OK ) {
            dr_type =  DRGetTypeAT( dr_this );
            if( dr_type ) {
                DRGetTypeInfo( dr_type, &typeinfo );
                LocationCreate( ll, LT_INTERNAL, &obj_ptr );
                ret = DCAssignLocation( ll, &tmp, typeinfo.size );
                if( ret == DS_OK ) {
                    base = NilAddr;
                    switch( typeinfo.modifier.ptr ) {   /* set segment */
                    case DR_PTR_none:
                    case DR_PTR_near16:
                    case DR_PTR_near32:
                        ret = SafeDCItemLocation( lc, CI_DEF_ADDR_SPACE, ll );
                        base = ll->e[0].u.addr; /* set base */
                        break;
                    }
                    switch( typeinfo.modifier.ptr ) {
                    case DR_PTR_none:
                        if( typeinfo.size == 4 ) {
                            base.mach.offset  = obj_ptr.n32;
                        } else {
                            base.mach.offset  = obj_ptr.n16;
                        }
                        break;
                    case DR_PTR_near16:
                        base.mach.offset  = obj_ptr.n16;
                        break;
                    case DR_PTR_near32:
                        base.mach.offset  = obj_ptr.n32;
                        break;
                    case DR_PTR_far16:
                        base.mach.segment = obj_ptr.f16.segment;
                        base.mach.offset  = obj_ptr.f16.offset;
                        break;
                    case DR_PTR_far32:
                        base.mach.segment = obj_ptr.f32.segment;
                        base.mach.offset  = obj_ptr.f32.offset;
                        break;
                    case DR_PTR_huge16:
                        base.mach.segment = obj_ptr.f16.segment;
                        base.mach.offset  = obj_ptr.f16.offset;
                        break;
                    }
                }
                LocationCreate( ll, LT_ADDR, &base );
            }
        }
    } else {
        ret = DS_FAIL;
    }
    return( ret );
}


search_result   DIGENTRY DIPImpAddrSym( imp_image_handle *ii,
                        imp_mod_handle im, address a, imp_sym_handle *is )
/************************************************************************/
{
    /* Search the given module for a symbol who's address is greater than
     * or equal to 'addr'. If none is found return SR_NONE. If you find
     * a symbol at that address exactly, fill in '*is' and return SR_EXACT.
     * Otherwise, fill in '*is' and return SR_CLOSEST.
     */
    addrsym_info    info;
    search_result   ret;
    seg_list        *addr_sym;

    if( im == IMH_NOMOD ) {
        DCStatus( DS_FAIL );
        return( SR_NONE );
    }
    addr_sym = DFLoadAddrSym( ii, im );
    Real2Map( ii->addr_map, &a );
    if( FindAddrSym( addr_sym, &a.mach, &info ) >= 0 ) {
        is->sclass = SYM_VAR;
        is->im = im;
        is->sym = info.sym;
        is->state = DF_NOT;
        if( info.map_offset == a.mach.offset ) {
            ret = SR_EXACT;
        } else {
            ret = SR_CLOSEST;
        }
    } else {
        ret = SR_NONE;
    }
    return( ret );
}

/**********************************/
/* Walk inner and outer blocks   */
/*********************************/

static bool InBlk( dr_handle blk, int depth, void *_ctl )
/*******************************************************/
{
    scope_ctl       *ctl = _ctl;
    uint_32         lo;
    uint_32         hi;
    scope_node      *new;

    depth = depth;
    lo = 0; //quick fix for labels
    hi = 0;
    DRGetLowPc( blk, &lo );
    DRGetHighPc( blk, &hi );
    new = AddScope( ctl->edge, lo, hi, blk );
    if( ctl->root == NULL ) {
        ctl->root = new;
    }
    ctl->edge = new;
    return( TRUE );
}


static bool IsInScope( scope_ctl  *ctl, address addr )
/****************************************************/
{
    bool            ret;
    scope_node      *root;

    ret = FALSE;
    root = ctl->root;
    if( root != NULL ) {
        if( ctl->base.mach.segment == addr.mach.segment ) {
            if( root->start <= addr.mach.offset
              && addr.mach.offset <  root->end ) {
                ret = TRUE;
           }
        }
    }
    return( ret );
}


static search_result  DFAddrScope( imp_image_handle *ii,
        imp_mod_handle im, address addr, scope_block *scope )
/************************************************************/
{
    scope_ctl       *ctl;
    scope_node      *node;
    search_result   ret;

    ret = SR_NONE;
    Real2Map( ii->addr_map, &addr );
    ctl =  &ii->scope;
    if( !IsInScope( ctl, addr ) ) {
        addrsym_info        info;
        seg_list            *addr_sym;

        addr_sym = DFLoadAddrSym( ii, im );
        if( FindAddrSym( addr_sym, &addr.mach, &info ) >= 0 ) {
            DRSetDebug( ii->dwarf->handle );    /* must do at each interface */
            if( DRIsFunc( info.sym ) ) {
                FiniScope( ctl );
                ctl->base = addr;
                ctl->base.mach.offset  = info.map_offset;
                ctl->base.mach.segment = info.map_seg;
                DRWalkScope( info.sym, InBlk, ctl );
            }
        }
    }
    if( ctl->root != NULL ) {
        node = FindScope( ctl->root, addr.mach.offset );
        if( node != NULL ) {
            scope->start = addr;
            scope->start.mach.offset = node->start;
            scope->len = node->end - node->start;
            DCMapAddr( &scope->start.mach, ii->dcmap );
            scope->unique = node->what;
            scope->unique -= IM2MODI( ii, im )->cu_tag;   /* make relative */
            ret = SR_CLOSEST;
        }
    }
    return( ret );
}


search_result   DIGENTRY DIPImpAddrScope( imp_image_handle *ii,
                imp_mod_handle im, address addr, scope_block *scope )
/*******************************************************************/
{
    /* Find the range of the lexical scope block enclosing 'addr' in
     * module 'im'. If there is no such scope, return SR_NONE. Otherwise
     * fill in scope->start with the address of the start of the lexical
     * block and scope->len with the size of the block. Fill in
     * scope->unique with something that uniquely identifies the lexical
     * block in question. This is used to disamibiguate between blocks
     * that start at the same address and have the same length. The value
     * should be chosen so that
     *         1. It remains valid and consistant across a DIPUnloadInfo
     *            and DIPLoadInfo of the same information.
     *         2. It remains the same whether the scope_block was obtained
     *            by DIPImpAddrScope or DIPImpScopeOuter.
     * Then return SR_EXACT/SR_CLOSEST as appropriate.
     */
    return( DFAddrScope( ii, im, addr, scope ) );
}


static search_result   DFScopeOuter( imp_image_handle *ii,
                imp_mod_handle im, scope_block *in, scope_block *out )
/*************************************************************/
{
    /* Given the scope_block pointed to by 'in' in the module 'im', find
     * the parent lexical block of it and fill in the scope_block pointed
     * to by 'out' with the information. Return SR_EXACT/SR_CLOSEST as
     * appropriate. Return SR_NONE if there is no parent block.
     * Make sure that the case where 'in' and 'out' point to the same
     * address is handled.
     */
    dr_handle           what;
    search_result       ret;
    address             addr;
    scope_ctl           *ctl;
    scope_node          *node;
    dr_handle           cu_tag;

    ret = SR_NONE;
    addr =  in->start;
    Real2Map( ii->addr_map, &addr );
    ctl = &ii->scope;
    if( !IsInScope( ctl, addr ) ) {
        addrsym_info        info;
        seg_list            *addr_sym;

        addr_sym = DFLoadAddrSym( ii, im );
        if( FindAddrSym( addr_sym, &addr.mach, &info ) >= 0 ) {
            DRSetDebug( ii->dwarf->handle );    /* must do at each interface */
            if( DRIsFunc( info.sym ) ) {
                FiniScope( ctl );
                ctl->base = addr;
                ctl->base.mach.offset = info.map_offset;
                ctl->base.mach.segment = info.map_seg;
                DRWalkScope( info.sym, InBlk, ctl );
            }
        }
    }
    if( ctl->root != NULL ) {
        ret = SR_NONE;
        cu_tag = IM2MODI(ii, im )->cu_tag;
        node = FindScope( ctl->root, addr.mach.offset );
        what = in->unique;
        what += cu_tag;    /* make absolute */
        while( node != NULL ) {
            if( node->what == what ) {
                node = node->down;
                break;
            }
            node = node->down;
        }
        if( node != NULL ) {
            out->start = addr;
            out->start.mach.offset = node->start;
            out->len = node->end - node->start;
            DCMapAddr( &out->start.mach, ii->dcmap );
            what = node->what;
            what -= cu_tag;
            out->unique = what; /* make relative */
            ret = SR_EXACT;
        }
    }
    return( ret );
}


static dr_handle GetContainingClass( dr_handle curr )
/***************************************************/
{
    dr_tag_type     sc;

    curr = DRGetContaining( curr );
    if( curr ) {
        curr = DRSkipTypeChain( curr ); /* PCH typedef link */
        sc = DRGetTagType( curr );
        if( sc != DR_TAG_CLASS ) {
            curr = 0;
        }
    }
    return( curr );
}


search_result   DIGENTRY DIPImpScopeOuter( imp_image_handle *ii,
                imp_mod_handle im, scope_block *in, scope_block *out )
/********************************************************************/
{
    search_result   ret;
    dr_handle       curr;
    dr_tag_type     sc;
    dr_handle       cu_tag;

    DRSetDebug( ii->dwarf->handle );    /* must do at each interface */
    cu_tag = IM2MODI( ii, im )->cu_tag;
    curr = in->unique + cu_tag;
    sc = DRGetTagType( curr );
    switch( sc ) {
    case DR_TAG_CLASS:
        DCStatus( DS_FAIL );
        ret = SR_NONE;
        break;
    case DR_TAG_FUNCTION:
        curr = GetContainingClass( curr );
        if( curr ) {
            *out = *in;
            out->unique = curr - cu_tag;
            ret = SR_EXACT;
        } else {
            DCStatus( DS_FAIL );
            ret = SR_NONE;
        }
        break;
    default:
        ret = DFScopeOuter( ii, im, in, out );
    }
    return( ret );
}

/**********************************************/
/* Walk or Search various scopes for symbols  */
/**********************************************/

typedef struct {
    imp_mod_handle      im;
    imp_image_handle    *ii;
    void                *d;
    dr_handle           containing;
    dr_srch             what;
    bool                cont;   //continue to next scope
    enum {
        WLK_LOOKUP,
        WLK_WLK,
    }                   kind;
} blk_wlk_com;

typedef struct {
    blk_wlk_com         com;
    walk_result         wr;
    IMP_SYM_WKR         *wk;
    imp_sym_handle      *is;
} blk_wlk_wlk;

typedef struct {
    blk_wlk_com         com;
    int                 (*comp)(const void *, const void *, size_t);
    lookup_item         *li;
    search_result       sr;
    char                *buff;
    unsigned            len;
} blk_wlk_lookup;

typedef union {
    blk_wlk_com         com;
    blk_wlk_wlk         wlk;
    blk_wlk_lookup      lookup;
} blk_wlk;

static dr_srch Dip2DwarfSrch( symbol_type dip )
/*********************************************/
{
    dr_srch     ret;

    ret = DR_SRCH_func_var;
    switch( dip ) {
    case ST_NONE:
    case ST_OPERATOR:
    case ST_DESTRUCTOR:
        ret = DR_SRCH_func_var;
        break;
    case ST_TYPE:
        ret = DR_SRCH_ctypes;
        break;
    case ST_STRUCT_TAG:
    case ST_CLASS_TAG:
    case ST_UNION_TAG:
        ret = DR_SRCH_class;
        break;
    case ST_ENUM_TAG:
        ret = DR_SRCH_enum;
        break;
    }
    return( ret );
}


static bool ASym( dr_handle var, int index, void *_df )
/*****************************************************/
{
    blk_wlk_wlk     *df = _df;
    bool            cont;
    imp_sym_handle  *is;
    dr_dbg_handle   saved;

    index = index;
    is = df->is;
    is->sclass = SYM_VAR;
    is->im = df->com.im;
    is->sym = var;
    is->state = DF_NOT;
    saved = DRGetDebug();
    df->wr = df->wk( df->com.ii, SWI_SYMBOL, is, df->com.d );
    DRSetDebug( saved );
    cont = TRUE;
    if( df->wr != WR_CONTINUE ) {
        cont = FALSE;
    }
    df->com.cont = cont;
    return( cont );
}


static bool ASymCont( dr_handle var, int index, void *_df )
/*********************************************************/
{
    blk_wlk_wlk     *df = _df;
    dr_handle       contain;
    bool            cont;

    cont = TRUE;
    contain = DRGetContaining( var );
    if( contain ) {
        contain = DRSkipTypeChain( contain );   /* PCH typedef link */
    }
    if( df->com.containing == contain ) {
        cont = ASym( var, index, df );
    }
    return( cont );
}


static bool AModSym( dr_handle var, int index, void *_df )
/********************************************************/
{
    blk_wlk_wlk     *df = _df;
    bool            cont;
    imp_sym_handle  *is;
    dr_tag_type     sc;
    dr_dbg_handle   saved;

    index = index;
    sc = DRGetTagType( var );
    if( sc == DR_TAG_NAMESPACE ) {
        DRWalkBlock( var, df->com.what, AModSym, df );
        cont = TRUE;
    } else {
        is = df->is;
        is->sclass = SYM_VAR;
        is->im = df->com.im;
        is->sym = var;
        is->state = DF_NOT;
        saved = DRGetDebug();
        df->wr = df->wk( df->com.ii, SWI_SYMBOL, is, df->com.d );
        DRSetDebug( saved );
        cont = TRUE;
    }
    if( df->wr != WR_CONTINUE ) {
        cont = FALSE;
    }
    df->com.cont = cont;
    return( cont );
}


static bool ASymLookup( dr_handle var, int index, void *_df )
/***********************************************************/
{
    blk_wlk_lookup  *df = _df;
    imp_sym_handle  *is;
    unsigned        len;

    index = index;
    len = DRGetNameBuff( var, df->buff, df->len );
    if( len == df->len
      && df->comp( df->buff, df->li->name.start, df->li->name.len ) == 0 ) {
        /* Found symbol by name */
        if( !DRIsFunc( var ) && !DRIsStatic( var ) && !DRIsSymDefined( var ) ) {
            /* If symbol is a global variable declaration, ignore it; it
             * won't have location information and will likely be found in
             * another module.
             */
        } else {
            is = DCSymCreate( df->com.ii, df->com.d );
            is->sclass = SYM_VAR;
            is->im = df->com.im;
            is->sym = var;
            is->state = DF_NOT;
            df->sr = SR_EXACT;
            df->com.cont = FALSE;
        }
    }
    return( TRUE );
}


static bool ASymContLookup( dr_handle var, int index, void *_df )
/***************************************************************/
{
    blk_wlk_lookup  *df = _df;
    dr_handle       contain;
    bool            cont;

    cont = TRUE;
    contain = DRGetContaining( var );
    if( contain ) {
        contain = DRSkipTypeChain( contain );   /* PCH typedef link */
    }
    if( df->com.containing == contain ) {
        cont = ASymLookup( var, index, df );
    }
    return( cont );
}


static bool WalkOneBlock( blk_wlk *df, DRWLKBLK fn, dr_handle blk )
/*****************************************************************/
{
    DRSetDebug( df->com.ii->dwarf->handle );    /* must do at each call into DWARF */
    DRWalkBlock( blk, df->com.what, fn, df );
    return( df->com.cont );
}


static bool WalkModSymList( blk_wlk *df, DRWLKBLK fn, imp_mod_handle im )
/***********************************************************************/
{
    bool            cont;
    mod_info        *modinfo;

    df->com.im = im;
    modinfo = IM2MODI( df->com.ii, im );
    if( df->com.what == DR_SRCH_ctypes && modinfo->lang == DR_LANG_CPLUSPLUS ) {
        df->com.what = DR_SRCH_cpptypes;
    }
    cont = WalkOneBlock( df, fn, modinfo->cu_tag );
    return( cont );
}


static bool WalkScopedSymList( blk_wlk *df, DRWLKBLK fn, address *addr )
/**********************************************************************/
{
    /* Walk inner to outer function scopes, then containing class
     * if present, then module scope.
     */
    imp_image_handle    *ii;
    scope_block         scope;
    dr_handle           cu_tag;
    dr_handle           dbg_pch;
    dr_tag_type         sc;
    imp_mod_handle      im;
    dr_handle           curr;
    bool                cont;
    mod_info            *modinfo;

    ii = df->com.ii;
    cont = TRUE;
    if( DFAddrMod( ii, *addr, &im ) != SR_NONE ) {
        modinfo = IM2MODI( ii, im );
        if( DFAddrScope( ii, im, *addr, &scope ) != SR_NONE ) {
            df->com.im = im;
            cu_tag = modinfo->cu_tag;
            for( ;; ) {
                curr = scope.unique + cu_tag;
                cont = WalkOneBlock( df, fn, curr );
                if( !cont )
                    break;
                if( DFScopeOuter( ii, im, &scope, &scope ) == SR_NONE ) {
                    cont = TRUE;
                    break;
                }
            }
            if( cont ) {
                sc = DRGetTagType( curr );
                if( sc == DR_TAG_FUNCTION ) {
                    imp_type_handle     it;

                    curr = GetContainingClass( curr );
                    if( curr ) {
                        it.state = DF_NOT;
                        it.type = curr;
                        it.im = im;
                        if( df->com.kind == WLK_WLK ) {
                            df->wlk.wr = WalkTypeSymList( ii, &it,
                                 df->wlk.wk, df->wlk.is, df->com.d );
                            if( df->wlk.wr != WR_CONTINUE ) {
                                cont = FALSE;
                            }
                        } else {
                            df->lookup.sr = SearchMbr( ii, &it,
                                          df->lookup.li, df->com.d );
                            if( df->lookup.sr == SR_EXACT ) {
                                cont = FALSE;
                            }
                        }
                    }
                }
            }
        }
        if( cont ) {
            cont = WalkModSymList( df, fn, im );
        }
        dbg_pch = modinfo->dbg_pch;
        if( cont && dbg_pch != 0 ) {
            im = CuTag2Mod( ii, dbg_pch );
            cont = WalkModSymList( df, fn, im );
        }
    }
    return( cont );
}


static bool WalkBlockSymList( blk_wlk  *df, DRWLKBLK fn, scope_block *scope )
/***************************************************************************/
{
    imp_image_handle    *ii;
    dr_handle           cu_tag;
    imp_mod_handle      im;
    dr_handle           blk;
    dr_tag_type         sc;
    bool                cont;

    ii = df->com.ii;
    if( DFAddrMod( ii, scope->start, &im ) != SR_NONE ) {
        DRSetDebug( df->com.ii->dwarf->handle );    /* must do at each call into DWARF */
        df->com.im = im;
        cu_tag = IM2MODI( ii, im )->cu_tag;
        blk = scope->unique + cu_tag;
        sc = DRGetTagType( blk );
        if( sc == DR_TAG_CLASS ) {
            imp_type_handle     it;

            it.state = DF_NOT;
            it.type = blk;
            it.im = im;
            if( df->com.kind == WLK_WLK ) {
                df->wlk.wr = WalkTypeSymList( ii, &it, df->wlk.wk, df->wlk.is, df->com.d );
            } else {
                df->lookup.sr = SearchMbr( ii, &it, df->lookup.li, df->com.d );
            }
        } else {
            cont = WalkOneBlock( df, fn, blk );
        }
    } else {
        cont = TRUE;
    }
    return( cont );
}


static bool WalkSymSymList( blk_wlk *df, DRWLKBLK fn, imp_sym_handle *is )
/************************************************************************/
{
    bool                cont;

    df->com.im = is->im;
    if( df->com.what == DR_SRCH_ctypes && IM2MODI( df->com.ii, is->im )->lang == DR_LANG_CPLUSPLUS ) {
        df->com.what = DR_SRCH_cpptypes;
    }
    cont = WalkOneBlock( df, fn, is->sym );
    return( cont );
}


static walk_result  WalkMyLDSyms( imp_image_handle *ii, imp_mod_handle im, void *_df )
/************************************************************************************/
{
    blk_wlk     *df = _df;

    df->wlk.com.im = im;
    WalkModSymList( df, &ASym, im );
    return( df->wlk.wr );
}


static walk_result DFWalkSymList( imp_image_handle *ii,
                         symbol_source     ss,
                         void             *source,
                         IMP_SYM_WKR      *wk,
                         imp_sym_handle   *is,
                         void             *d )
/******************************************************/
{
    imp_mod_handle      im;
    walk_result         wr = 0;
    blk_wlk             df;
    int                 cont = 0;

    df.com.ii = ii;
    df.com.d = d;
    df.com.what = DR_SRCH_func_var;
    df.com.containing = 0;
    df.com.cont = TRUE;
    df.com.kind = WLK_WLK;
    df.wlk.wk = wk;
    df.wlk.is = is;
    df.wlk.wr = WR_CONTINUE;
    switch( ss ) {
    case SS_TYPE: /* special case */
        wr = WalkTypeSymList( ii, (imp_type_handle *)source, wk, is, d );
        break;
    case SS_SCOPED:
        cont = WalkScopedSymList( &df, &ASym, (address *)source );
        wr = df.wlk.wr;
        break;
    case SS_SCOPESYM:
        is = (imp_sym_handle *)source;
        WalkSymSymList( &df, &ASym, is );
        if( cont ) {
            df.com.containing = is->sym;
            WalkModSymList( &df, &ASymCont, is->im );
        }
        wr = df.wlk.wr;
        break;
    case SS_BLOCK:
        WalkBlockSymList( &df, &ASym, (scope_block *)source );
        wr = df.wlk.wr;
        break;
    case SS_MODULE:
        im = *(imp_mod_handle *)source;
        if( im == IMH_NOMOD ) {
            wr = DFWalkModList( ii, WalkMyLDSyms, &df );
        } else {
            WalkModSymList( &df, &AModSym, im );
            wr = df.wlk.wr;
        }
        break;
    }
    return( wr );
}


walk_result DIGENTRY DIPImpWalkSymList( imp_image_handle *ii,
                                        symbol_source     ss,
                                        void             *source,
                                        IMP_SYM_WKR      *wk,
                                        imp_sym_handle   *is,
                                        void              *d )
/***************************************************************/
{
    return( DFWalkSymList( ii, ss, source, wk, is, d ) );
}


walk_result DIGENTRY DIPImpWalkSymListEx( imp_image_handle *ii,
                                          symbol_source     ss,
                                          void             *source,
                                          IMP_SYM_WKR      *wk,
                                          imp_sym_handle   *is,
                                          location_context *lc,
                                           void             *d )
/*****************************************************************/
{
    lc = lc;
    return( DFWalkSymList( ii, ss, source, wk, is, d ) );
}


#define SH_ESCAPE       0xf0

static void CollectSymHdl( byte *ep, imp_sym_handle *is )
/*******************************************************/
{
    byte        *sp;
    byte        curr;
    static byte escapes[] = { SH_ESCAPE, '\0', '`' };

    ++ep;
    sp = (byte *)is;
    ++is;
    while( sp < (byte *)is ) {
        curr = *ep++;
        if( curr == SH_ESCAPE )
            curr = escapes[ *ep++ - 1 ];
        *sp++ = curr;
    }
}


typedef struct {
    imp_image_handle    *ii;
    void                *d;
    int                 (*compare)(char const *, char const *);
    char                *name;
    dr_handle           sym;
} hash_look_data;

static bool AHashItem( void *_find, dr_handle sym, char *name )
/*************************************************************/
{
    hash_look_data  *find = _find;
    imp_sym_handle  *is;

    if( find->compare( name, find->name ) == 0 ) {
        find->sym = sym;
        is = DCSymCreate( find->ii, find->d );
        is->im = DwarfMod( find->ii, sym );
        is->sclass = SYM_VAR;
        is->sym = sym;
        is->state = DF_NOT;
    }
    return( TRUE );
}

typedef struct {
    const char  *p;
    int         len;
} strvi;

typedef struct {
    char        *p;
    int         len;
} strvo;

static int StrVCopy( strvo *dst, strvi *src )
/*****************************************/
{
    /* Copy source until null char or source runs out
     * to dest until it runs out, return total length of source
     */
    int     total;

    total = src->len;
    for( ;; ) {
        if( src->len == 0 ) break;
        if( *src->p == '\0' ) break;
        if( dst->len > 0 ) {
            *dst->p = *src->p;
            ++dst->p;
            --dst->len;
        }
        ++src->p;
        --src->len;
    }
    total -= src->len;
    return( total );
}


static int QualifiedName( char *buff, lookup_item  *li, int max )
/***************************************************************/
{
    int     total;
    strvi   curr;
    strvo   dst;
    strvi   tmp;

    curr.p   = li->scope.start;
    curr.len = li->scope.len;
    dst.p   = buff;
    dst.len = max;
    total = 0;
    while( curr.len > 0 ) {
        total += StrVCopy( &dst, &curr );
        if( *curr.p == '\0' ) {
            ++curr.p;
            --curr.len;
        }
        tmp.p = "::";
        tmp.len = 2;
        total += StrVCopy( &dst, &tmp );
    }
    curr.p = li->name.start;
    curr.len = li->name.len;
    total += StrVCopy( &dst, &curr );
    if( dst.len > 0 ) {
        *dst.p = '\0';
    }
    total += 1;
    return( total );
}


static search_result HashSearchGbl( imp_image_handle *ii,
                                    lookup_item      *li, void *d )
/*****************************************************************/
{
    char                buff[256];
    unsigned            len;
    search_result       sr;
    hash_look_data      data;
    name_wlk            wlk;

    sr = SR_NONE;
    data.ii  = ii;
    data.compare = li->case_sensitive ? &strcmp : &stricmp;
    data.sym = 0;
    len = QualifiedName(  buff, li, sizeof( buff ) );
    if( len <= sizeof( buff ) ) {
        data.name = buff;
    } else {
        data.name = DCAlloc( len + 1 );
        len = QualifiedName(  data.name, li, len + 1 );
    }
    data.d  = d;
    wlk.fn = AHashItem;
    wlk.name = data.name;
    wlk.d = &data;
    DRSetDebug( ii->dwarf->handle );    /* must do at each call into DWARF */
    FindHashWalk( ii->name_map, &wlk );
    if( data.sym ) {
        sr = SR_EXACT;
    }
    if( data.name != buff ) {
        DCFree( data.name );
    }
    return( sr );
}


extern search_result   DoLookupSym( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
/**************************************************************/
{
    imp_mod_handle      im;
    imp_sym_handle      *is;
    search_result       sr;
//    char                *src;
//    unsigned            len;
    blk_wlk             df;
    char                buff[256];
    bool                cont;

    lc = lc;
    if( *(unsigned_8 *)li->name.start == SH_ESCAPE ) {
        CollectSymHdl( (unsigned_8 *)li->name.start, DCSymCreate( ii, d ) );
        return( SR_EXACT );
    }
    if( li->type == ST_OPERATOR ) {
//TODO this operator
//        src = li->name.start;
//        len = li->name.len;
        return( SR_NONE );
    }
    if( ss ==  SS_TYPE ) {
        if( MH2IMH( li->mod ) != IMH_NOMOD ) {
            sr =  SR_NONE;
        } else {
            sr = SearchMbr( ii, (imp_type_handle *)source, li, d  );
        }
        return( sr );
    }
    df.com.ii = ii;
    df.com.d = d;
    df.com.what = Dip2DwarfSrch( li->type );
    df.com.containing = 0;
    df.com.cont = TRUE;
    df.com.kind = WLK_LOOKUP;
    df.lookup.comp = li->case_sensitive ? memcmp : memicmp;
    df.lookup.li = li;
    df.lookup.len =  li->name.len+1;
    if( df.lookup.len <= sizeof( buff ) ) {
        df.lookup.buff = buff;
    } else {
        df.lookup.buff = DCAlloc( df.lookup.len );
    }
    df.lookup.sr = SR_NONE;
    sr =  SR_NONE;
    switch( ss ) {
    case SS_SCOPED:
        if( li->scope.len == 0 ) {
            cont = WalkScopedSymList( &df, ASymLookup, (address *)source );
        } else {
            cont = TRUE;
        }
        sr = df.lookup.sr;
        if( cont ) {
            if( DR_SRCH_func_var == df.com.what ) {
                sr = HashSearchGbl( ii, li, d );
           }
        }
        break;
    case SS_MODULE:
        im = *(imp_mod_handle *)source;
        if( im == IMH_NOMOD ) {
            if( DR_SRCH_func_var == df.com.what ) {
                sr = HashSearchGbl( ii, li, d );
            }
        } else {
           WalkModSymList( &df, ASymLookup, im );
           sr = df.lookup.sr;
        }
        break;
    case SS_SCOPESYM:
        is = (imp_sym_handle *)source;
        WalkSymSymList( &df, &ASymLookup, is );
        df.com.containing = is->sym;    //check for out of line defn
        WalkModSymList( &df, ASymContLookup, is->im );
        sr = df.lookup.sr;
        break;
    }
    if( df.lookup.buff != buff ) {
        DCFree( df.lookup.buff );
    }
    return( sr );
}


extern search_result   DIGENTRY DIPImpLookupSym( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li, void *d )
/************************************************************************/
{
    return( DoLookupSym( ii, ss, source, li, NULL, d ) );
}


extern search_result   DIGENTRY DIPImpLookupSymEx( imp_image_handle *ii,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
/**********************************************************************/
{
    return( DoLookupSym( ii, ss, source, li, lc, d ) );
}


int DIGENTRY DIPImpSymCmp( imp_image_handle *ii, imp_sym_handle *is1,
                                imp_sym_handle *is2 )
/*******************************************************************/
{
    /* Compare two sym handles and return 0 if they refer to the same
     * information. If they refer to differnt things return either a
     * positive or negative value to impose an 'order' on the information.
     * The value should obey the following constraints.
     * Given three handles H1, H2, H3:
     *         - if H1 < H2 then H1 is always < H2
     *         - if H1 < H2 and H2 < H3 then H1 is < H3
     * The reason for the constraints is so that a client can sort a
     * list of handles and binary search them.
     */
    long    diff;

    ii = ii;
    diff = is1->sym - is2->sym;
    return( diff );
}


dip_status DIGENTRY DIPImpSymAddRef( imp_image_handle *ii, imp_sym_handle *is )
/*****************************************************************************/
{
    ii = ii;
    is = is;
    return( DS_OK );
}


dip_status DIGENTRY DIPImpSymRelease( imp_image_handle *ii, imp_sym_handle *is )
/******************************************************************************/
{
    ii = ii;
    is = is;
    return( DS_OK );
}


dip_status DIGENTRY DIPImpSymFreeAll( imp_image_handle *ii )
/**********************************************************/
{
    ii = ii;
    return( DS_OK );
}
