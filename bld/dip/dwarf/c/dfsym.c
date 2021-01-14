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


imp_mod_handle DIPIMPENTRY( SymMod )( imp_image_handle *iih, imp_sym_handle *ish )
/********************************************************************************/
{
    /* unused parameters */ (void)iih;

    /* Return the module that the implementation symbol handle comes from. */
    return( ish->imh );
}


size_t DIPIMPENTRY( SymName )( imp_image_handle *iih, imp_sym_handle *ish,
    location_context *lc, symbol_name_type snt, char *buff, size_t buff_size )
/****************************************************************************/
{
    /*
        SNT_SOURCE:
                The name of the symbol as it appears in the source code.

        SNT_OBJECT:
                The name of the symbol as it appeared to the linker.

        SNT_DEMANGLED:
                C++ names, with full typing (essentially it looks like
                a function prototype). If the symbol is not a C++ symbol
                (not mangled), return zero for the length.

        SNT_EXPRESSION:
                Return whatever character string is necessary such that
                when scanned in an expression, the symbol handle can
                be reconstructed.

        SNT_SCOPED:
                The scoped name of the symbol as it appears in the source code.
    */
    char        *name;
    size_t      demangled_len;
    size_t      len = 0;

    /* unused parameters */ (void)lc;

//TODO: what's lc for?
    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    switch( snt ) {
    case SNT_SOURCE:
    case SNT_OBJECT:
        len = DRGetNameBuff( ish->sym, buff, buff_size );
        if( len == 0 ) {
            DCStatus( DS_FAIL );
            return( 0 );
        }
        --len;
        if( buff_size != 0 && len > buff_size ) {
            buff[buff_size - 1] = '\0';
        }
        break;
    case SNT_SCOPED:
        len =  DRGetScopedNameBuff( ish->sym, buff, buff_size );
        if( len == 0 ) {
            DCStatus( DS_FAIL );
            return( 0 );
        }
        --len;
        if( buff_size != 0 && len > buff_size ) {
           buff[buff_size - 1] = '\0';
        }
        break;
    case SNT_DEMANGLED:
        if( IMH2MODI( iih, ish->imh )->lang == DR_LANG_CPLUSPLUS ) {
            name = DRDecoratedName( ish->sym, 0 );
            if( name == NULL ) {
                DCStatus( DS_FAIL );
                return( 0 );
            }
            len = NameCopy( buff, name, buff_size, 0 );
            DCFree( name );
        } else if( buff == NULL ) {
            return( 0 );
        } else {
            len = DRGetNameBuff( ish->sym, buff, buff_size );
            if( __is_mangled( buff, len ) ) {
                demangled_len = __demangle_l( buff, len, NULL, 0 );
                if( demangled_len > buff_size ) {
                    demangled_len = buff_size;
                }
                name = DCAlloc( demangled_len );
                __demangle_l( buff, len, name, demangled_len );
                strncpy( buff, name, buff_size );
                DCFree( name );
                return( demangled_len );
            } else {
                return( 0 );
            }
        }
        break;
    case SNT_EXPRESSION:
        return( 0 );
    }
    return( len );
}


dip_status DIPIMPENTRY( SymType )( imp_image_handle *iih,
                    imp_sym_handle *ish, imp_type_handle *ith )
/*************************************************************/
{
    /* Get the implementation type handle for the type of the given symbol. */
    dip_status  ds;

    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    if( ish->state == DF_NOT ) {
        ish->stype = DRGetTagType( ish->sym );
    }
    switch( ish->stype ) {
    case DR_TAG_FUNCTION:
    case DR_TAG_CLASS:
    case DR_TAG_ENUM:
        ith->type = ish->sym;
        break;
    default:
        ith->type = DRGetTypeAT( ish->sym );
        break;
    }
    ds = DS_OK;
    ith->imh = ish->imh;
    if( ith->type != DRMEM_HDL_NULL ) {
        ith->state = DF_NOT;
    } else {
        ith->state = DF_SET;         // default the type
        ith->sub_array = false;
        ith->typeinfo.kind = DR_TYPEK_DATA;
        ith->typeinfo.size = 0;
        ith->typeinfo.mclass = DR_MOD_NONE;
        if( ish->stype == DR_TAG_LABEL ) {
            ith->typeinfo.kind = DR_TYPEK_CODE;
        }
    }
    return( ds );
}


struct mod_wlk {
    imp_image_handle    *iih;
    seg_list            *addr_sym;
    imp_mod_handle      imh;
    address             a;
    drmem_hdl           sym;
    search_result       ret;
};

static bool AMod( drmem_hdl sym, void *_d, dr_search_context *cont )
/******************************************************************/
{
//TODO: no segments, better TAG_label
    struct mod_wlk  *d = _d;
    uint_32         offset;
    addr_seg        seg;
//    bool            ret;
    addrsym_info    info;
    bool            is_segment;

    /* unused parameters */ (void)cont;

//    ret = true;
    is_segment = IMH2MODI( d->iih, d->imh )->is_segment;
    if( DRGetLowPc( sym, &offset) ) {
        if( !is_segment ) {
            seg = SEG_FLAT; // if flat hoke segment
        } else {
            EvalSeg( d->iih, sym, &seg );
        }
    } else {
        if( !is_segment ) {
            seg = SEG_FLAT; // if flat hoke segment
        } else {
            EvalSeg( d->iih, sym, &seg );
        }
//        ret = EvalSymOffset( d->iih, sym, &offset );
        EvalSymOffset( d->iih, sym, &offset );
    }
    info.map_offset = offset;
    info.sym = sym;
    info.map_seg = seg;
    AddAddrSym( d->addr_sym, &info );
    return( true );
}


seg_list *DFLoadAddrSym( imp_image_handle *iih, imp_mod_handle imh )
/******************************************************************/
{
    struct mod_wlk  d;
    seg_list        *addr_sym;
    mod_info        *modinfo;

    modinfo = IMH2MODI( iih, imh );
    addr_sym = modinfo->addr_sym;
    if( addr_sym->head == NULL ) {  /* no cache */
        FiniAddrSym( addr_sym );    /* kill cache */
        DRSetDebug( iih->dwarf->handle ); /* must do at each interface */
        d.addr_sym = addr_sym;
        d.iih = iih;
        d.imh = imh;
        d.ret = SR_NONE;
        DRWalkModFunc( modinfo->cu_tag, false, AMod, &d );   /* load cache */
        SortAddrSym( addr_sym );
    }
    return( addr_sym );
}


typedef struct {
    drmem_hdl   inh;    /* member class handle */
    char        *name;  /* member name         */
    drmem_hdl   match;  /* handle that matches */
} mem_func_wlk;

static bool AMemFuncSym( void *_df, addrsym_info *info )
/******************************************************/
{
    mem_func_wlk    *df = _df;
    bool            cont;
    char            buff[256];
//    size_t          len;
    drmem_hdl       contain;

    cont = true;
    contain  = DRGetContaining( info->sym );
    if( contain != DRMEM_HDL_NULL ) {
        contain = DRSkipTypeChain( contain );   /* PCH typedef link */
        if( contain == df->inh ) {
//            len =  DRGetNameBuff( info->sym, buff, sizeof( buff ) );
            DRGetNameBuff( info->sym, buff, sizeof( buff ) );
            if( strcmp( buff, df->name ) == 0 ) {
                cont = false;
                df->match = info->sym;
            }
        }
    }
    return( cont );
}


static drmem_hdl MemFuncLookUp( imp_image_handle *iih,
                                drmem_hdl sym,
                                drmem_hdl inh,
                                imp_mod_handle imh )
/***************************************************/
{
    mem_func_wlk    df;
    char            buff[256];
    seg_list        *addr_sym;

    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    addr_sym = DFLoadAddrSym( iih, imh );
    DRGetNameBuff( sym, buff, sizeof( buff ) );
    df.name = buff;
    df.inh  = inh;
    df.match = 0;
    WlkAddrSyms( addr_sym, AMemFuncSym, &df );
    return( df.match );
}


static  addr_seg GetCodeSeg( imp_image_handle *iih )
/**************************************************/
{
    addr_ptr mach;
    mach.segment = SEG_CODE;
    mach.offset = 0;
    DCMapAddr( &mach, iih->dcmap );
    return( mach.segment );
}


dip_status DIPIMPENTRY( SymLocation )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, location_list *ll )
/****************************************************************************/
{
    /* Get the location of the given symbol. */
    dip_status       ds;
    address          base; /* base segment & offset */
    addr_seg         seg;
    drmem_hdl        sym;
    bool             is_segment;

    ds = DS_FAIL;
    base = NilAddr;
    is_segment = IMH2MODI( iih, ish->imh )->is_segment;
    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    if( DRGetLowPc( ish->sym, &base.mach.offset) ) {
        if( !is_segment ) {
            seg = SEG_CODE; // if flat hoke segment
        } else {
            EvalSeg( iih, ish->sym, &seg );
        }
        base.mach.segment = seg;
        DCMapAddr( &base.mach, iih->dcmap );
        LocationCreate( ll, LT_ADDR, &base );
        ds = DS_OK;
    } else {
        switch( ish->sclass ) {
        case SYM_MEM:
        case SYM_VIRTF:
        {
            dr_bitfield info;
            int         b_strt;

            ds = SafeDCItemLocation( lc, CI_OBJECT, ll );
            if( ds != DS_OK ) {
                DCStatus( ds );
                return( ds );
            }
            if( ish->f.minfo.inh != DRMEM_HDL_NULL ) {
                DFBaseAdjust( iih, ish->f.minfo.root,
                      DRGetTypeAT( ish->f.minfo.inh ),lc, &ll->e[0].u.addr );
            }
            ds = EvalLocAdj( iih, lc, ish->sym, &ll->e[0].u.addr  );
            if( ish->sclass == SYM_VIRTF ) {
                ll->e[0].u.addr.mach.segment = GetCodeSeg( iih );
            } else if( DRGetBitFieldInfo( ish->sym, &info ) ) {
                b_strt = 8 * info.byte_size.val.s -
                    (info.bit_offset.val.s + info.bit_size.val.s);
                LocationAdd( ll, b_strt );
                LocationTrunc( ll, info.bit_size.val.s );
            }
            break;
        }
        case SYM_MEMF:
            if( ish->f.minfo.inh == DRMEM_HDL_NULL ) {
                sym = ish->f.minfo.root;
            } else {
                sym = DRGetTypeAT( ish->f.minfo.inh );   /* get inherited type */
            }
            sym = DRSkipTypeChain( sym );   /* PCH typedef link */
            sym = MemFuncLookUp( iih, ish->sym, sym, ish->imh );
            if( sym == DRMEM_HDL_NULL ) {
                base = NilAddr; /* for now say it's NULL */
                LocationCreate( ll, LT_ADDR, &base );
                ds = DS_OK;
                break;
            } else {
                ish->sym = sym;
            }
            DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
            if( DRGetLowPc( ish->sym, &base.mach.offset) ) {
                if( !is_segment ) {
                    seg = SEG_CODE; // if flat hoke segment
                } else {
                    EvalSeg( iih, ish->sym, &seg );
                }
                base.mach.segment = seg;
                DCMapAddr( &base.mach, iih->dcmap );
                LocationCreate( ll, LT_ADDR, &base );
                ds = DS_OK;
            }
            break;
        case SYM_MEMVAR:
        case SYM_VAR:
            if( !is_segment ) {
                if( DRIsFunc( ish->sym ) ) {
                    seg = SEG_CODE; // if flat hoke segment
                } else {
                    seg = SEG_DATA; // if flat hoke segment
                }
            } else {
                EvalSeg( iih, ish->sym, &seg );
            }
            ds = EvalLocation( iih, lc, ish->sym, seg, ll );
            break;
        }
    }
    return( ds );
}


dip_status DIPIMPENTRY( SymValue )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *ic, void *buff )
/*********************************************************************/
{
    uint_32     value;

    /* unused parameters */ (void)ic;

    /* Copy the value of a constant symbol into 'buff'. You can get the
     * size required by doing a SymType followed by a TypeInfo.
     */
    if( ish->sclass != SYM_ENUM ) {
        return( DS_FAIL );
    }
    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    if( !DRConstValAT( ish->sym, &value ) ) {
        return( DS_FAIL );
    }
    switch( ish->f.einfo.size ) {
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


dip_status DIPIMPENTRY( SymInfo )( imp_image_handle *iih,
                imp_sym_handle *ish, location_context *lc, sym_info *si )
/***********************************************************************/
{
    /* Get some generic information about a symbol. */
    uint_32         num1;
    uint_32         num2;
    dr_ptr          addr_class;

    /* unused parameters */ (void)lc;

    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    if( ish->state == DF_NOT ) {
        DRSetDebug( iih->dwarf->handle ); /* must do at each interface */
        ish->stype = DRGetTagType( ish->sym );
        ish->acc   = DRGetAccess( ish->sym );
        ish->isstatic = DRIsStatic( ish->sym );
        ish->isartificial = DRIsArtificial( ish->sym );
        ish->state = DF_SET;
    }
    switch( ish->stype ) {
    case DR_TAG_FUNCTION:
        if( ish->sclass == SYM_MEMF || ish->sclass == SYM_VIRTF ) {
            si->kind = SK_CODE;     /* member functions */
        } else {
            si->kind = SK_PROCEDURE;
            if( DRStartScopeAT( ish->sym, &num1 ) ) {
                si->prolog_size  = num1;
            } else {
                si->prolog_size  = 0;
            }
            if( !DRGetLowPc( ish->sym, &num1 ) ) {
                num1 = 0;
            }
            if( !DRGetHighPc( ish->sym, &num2 ) ) {
                num2 = num1;
            }
            si->rtn_size = num2 - num1;
            si->num_parms = GetParmCount( iih, ish->sym );
            si->ret_size = 0;
            si->ret_modifier = TM_NONE;
            si->epilog_size = 0;
            si->rtn_calloc  = 0;
            if( EvalOffset( iih, ish->sym, &num1 ) ) {
                si->ret_addr_offset = num1;
            } else {
                si->ret_addr_offset = (addr_off)-1L;
            }
            addr_class =  DRGetAddrClass( ish->sym );
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
    switch( ish->acc ) {
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
    si->compiler = ish->isartificial;
    if( ish->isstatic ) {
        si->is_global = 0;
    } else {
        si->is_global = 1;
    }
    switch( ish->sclass ) {
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


#define SYMBOL_dot_return       ".return"

static bool ARet( drmem_hdl var, int index, void *_var_ptr )
/**********************************************************/
{
    drmem_hdl   *var_ptr = _var_ptr;
    char        name[sizeof( SYMBOL_dot_return )];
    bool        cont;
    size_t      len;

    /* unused parameters */ (void)index;

    cont = true;
    if( DRIsArtificial( var ) ) {
        len = DRGetNameBuff( var, name, sizeof( name ) );
        if( len == sizeof( name ) ) {
            if( strcmp( name, SYMBOL_dot_return ) == 0 ) {
                *var_ptr = var;
                cont = false;
            }
        }
    }
    return( cont );
}


drmem_hdl GetRet( imp_image_handle *iih, drmem_hdl proc )
/*******************************************************/
{
    /* Find handle of Watcom return symbol. */
    drmem_hdl ret = DRMEM_HDL_NULL;

    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    if( DRWalkBlock( proc, DR_SRCH_var, ARet, (void *)&ret ) ) {
        ret = DRMEM_HDL_NULL;
    }
    return( ret );
}


dip_status DIPIMPENTRY( SymParmLocation )( imp_image_handle *iih,
                    imp_sym_handle *ish, location_context *lc,
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
    drmem_hdl       parm;
    dip_status      ds;

    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    ds = DS_FAIL;
    if( n > 0 ) {
        parm = GetParmN( iih, ish->sym, n );
        if( parm != DRMEM_HDL_NULL ) {
            ds = EvalParmLocation( iih, lc, parm, ll );
        }
    } else if( n == 0 ) {
        //TODO: get ret location
        parm = GetRet( iih, ish->sym );
        if( parm != DRMEM_HDL_NULL ) {
            ds = EvalRetLocation( iih, lc, parm, ll );
        }
    }
    return( ds );
}


#define SYMBOL_this     "this"

static bool AThis( drmem_hdl var, int index, void *_var_ptr )
/***********************************************************/
{
    drmem_hdl   *var_ptr = _var_ptr;
    char        name[sizeof( SYMBOL_this )];
    bool        ret;
    size_t      len;

    /* unused parameters */ (void)index;

    ret = true;
    len =  DRGetNameBuff( var, name, sizeof( name ) );
    if( len == sizeof( name ) ) {
        if( strcmp( name, SYMBOL_this ) == 0 ) {
            *var_ptr = var;
            ret = false;
        }
    }
    return( ret );
}


static drmem_hdl GetThis( imp_image_handle *iih, drmem_hdl proc )
/***************************************************************/
{
    /* Return handle of the this parmeter. */
    drmem_hdl   ret = DRMEM_HDL_NULL;

    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    if( DRWalkBlock( proc, DR_SRCH_parm, AThis, (void *)&ret ) ) {
        ret = DRMEM_HDL_NULL;
    }
    return( ret );
}


dip_status DIPIMPENTRY( SymObjType )( imp_image_handle *iih,
                    imp_sym_handle *ish, imp_type_handle *ith, dig_type_info *ti )
/********************************************************************************/
{
    /* Fill in the imp_type_handle with the type of the 'this' object
     * for a C++ member function.
     * If 'ti' is not NULL, fill in the dig_type_info with the kind of 'this'
     * pointer that the routine is expecting (near/far, 16/32). If the
     * routine is a static member, set ti->kind to TK_NONE.
     */
    drmem_hdl   dr_this;
    drmem_hdl   dr_type;
    dr_typeinfo typeinfo;
    dip_status  ds;

    dr_this = GetThis( iih, ish->sym );
    if( dr_this != DRMEM_HDL_NULL ) {
        DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
        dr_type =  DRGetTypeAT( dr_this );
        if( dr_type != DRMEM_HDL_NULL ) {
            DRGetTypeInfo( dr_type, &typeinfo );
            MapImpTypeInfo( &typeinfo, ti );
            dr_type = DRSkipTypeChain( dr_type );
            ith->type = DRGetTypeAT( dr_type );
            ith->state = DF_NOT;
            ith->imh = ish->imh;
            ds = DS_OK;
        } else {
            ds = DS_FAIL;
        }
    } else {
        if( ti != NULL ) {
            ti->kind = TK_NONE;
            ti->size = 0;
            ti->modifier = TM_NONE;
            ti->deref = false;
        }
        ds = DS_FAIL;
    }
    return( ds );
}


dip_status DIPIMPENTRY( SymObjLocation )( imp_image_handle *iih,
                                imp_sym_handle *ish, location_context *lc,
                                location_list *ll )
/***********************************************************************/
{
    /* Fill in the location list with the location of the '*this' object
     * for a C++ member function. Return DS_FAIL if it's a static member
     * function.
     */
    drmem_hdl       dr_this;
    drmem_hdl       dr_type;
    dr_typeinfo     typeinfo;
    addr_seg        seg;
    address         base;   /* base segment & offset */
    location_list   tmp;
    dip_status      ds;
    union{
        addr32_off  n16;
        addr48_off  n32;
        addr32_ptr  f16;
        addr48_ptr  f32;
    } obj_ptr;

    dr_this = GetThis( iih, ish->sym );
    if( dr_this != DRMEM_HDL_NULL ) {
        if( !IMH2MODI( iih, ish->imh )->is_segment ) {
            seg = SEG_DATA; // if flat hoke segment
        } else {
            EvalSeg( iih, ish->sym, &seg );
        }
        ds = EvalLocation( iih, lc, dr_this, seg, &tmp );
        if( ds == DS_OK ) {
            dr_type =  DRGetTypeAT( dr_this );
            if( dr_type != DRMEM_HDL_NULL ) {
                DRGetTypeInfo( dr_type, &typeinfo );
                LocationCreate( ll, LT_INTERNAL, &obj_ptr );
                base = NilAddr;
                ds = DCAssignLocation( ll, &tmp, typeinfo.size );
                if( ds == DS_OK ) {
                    switch( typeinfo.modifier.ptr ) {   /* set segment */
                    case DR_PTR_none:
                    case DR_PTR_near16:
                    case DR_PTR_near32:
                        ds = SafeDCItemLocation( lc, CI_DEF_ADDR_SPACE, ll );
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
        ds = DS_FAIL;
    }
    return( ds );
}


search_result DIPIMPENTRY( AddrSym )( imp_image_handle *iih,
                        imp_mod_handle imh, address a, imp_sym_handle *ish )
/**************************************************************************/
{
    /* Search the given module for a symbol who's address is greater than
     * or equal to 'addr'. If none is found return SR_NONE. If you find
     * a symbol at that address exactly, fill in '*ish' and return SR_EXACT.
     * Otherwise, fill in '*ish' and return SR_CLOSEST.
     */
    addrsym_info    info;
    search_result   ret;
    seg_list        *addr_sym;

    if( imh == IMH_NOMOD ) {
        DCStatus( DS_FAIL );
        return( SR_NONE );
    }
    addr_sym = DFLoadAddrSym( iih, imh );
    Real2Map( iih->addr_map, &a );
    if( FindAddrSym( addr_sym, &a.mach, &info ) >= 0 ) {
        ish->sclass = SYM_VAR;
        ish->imh = imh;
        ish->sym = info.sym;
        ish->state = DF_NOT;
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

static bool InBlk( drmem_hdl blk, int depth, void *_ctl )
/*******************************************************/
{
    scope_ctl       *ctl = _ctl;
    uint_32         lo;
    uint_32         hi;
    scope_node      *new;

    /* unused parameters */ (void)depth;

    lo = 0; //quick fix for labels
    hi = 0;
    DRGetLowPc( blk, &lo );
    DRGetHighPc( blk, &hi );
    new = AddScope( ctl->edge, lo, hi, blk );
    if( ctl->root == NULL ) {
        ctl->root = new;
    }
    ctl->edge = new;
    return( true );
}


static bool IsInScope( scope_ctl  *ctl, address addr )
/****************************************************/
{
    bool            ret;
    scope_node      *root;

    ret = false;
    root = ctl->root;
    if( root != NULL ) {
        if( ctl->base.mach.segment == addr.mach.segment ) {
            if( root->start <= addr.mach.offset && addr.mach.offset < root->end ) {
                ret = true;
           }
        }
    }
    return( ret );
}


static search_result  DFAddrScope( imp_image_handle *iih,
        imp_mod_handle imh, address addr, scope_block *scope )
/************************************************************/
{
    scope_ctl       *ctl;
    scope_node      *node;
    search_result   ret;

    ret = SR_NONE;
    Real2Map( iih->addr_map, &addr );
    ctl =  &iih->scope;
    if( !IsInScope( ctl, addr ) ) {
        addrsym_info        info;
        seg_list            *addr_sym;

        addr_sym = DFLoadAddrSym( iih, imh );
        if( FindAddrSym( addr_sym, &addr.mach, &info ) >= 0 ) {
            DRSetDebug( iih->dwarf->handle );    /* must do at each interface */
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
            DCMapAddr( &scope->start.mach, iih->dcmap );
            scope->unique = node->what - IMH2MODI( iih, imh )->cu_tag;   /* make relative */
            ret = SR_CLOSEST;
        }
    }
    return( ret );
}


search_result DIPIMPENTRY( AddrScope )( imp_image_handle *iih,
                imp_mod_handle imh, address addr, scope_block *scope )
/********************************************************************/
{
    /* Find the range of the lexical scope block enclosing 'addr' in
     * module 'imh'. If there is no such scope, return SR_NONE. Otherwise
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
    return( DFAddrScope( iih, imh, addr, scope ) );
}


static search_result   DFScopeOuter( imp_image_handle *iih,
                imp_mod_handle imh, scope_block *in, scope_block *out )
/*********************************************************************/
{
    /* Given the scope_block pointed to by 'in' in the module 'imh', find
     * the parent lexical block of it and fill in the scope_block pointed
     * to by 'out' with the information. Return SR_EXACT/SR_CLOSEST as
     * appropriate. Return SR_NONE if there is no parent block.
     * Make sure that the case where 'in' and 'out' point to the same
     * address is handled.
     */
    drmem_hdl           what;
    search_result       ret;
    address             addr;
    scope_ctl           *ctl;
    scope_node          *node;
    drmem_hdl           cu_tag;

    ret = SR_NONE;
    addr =  in->start;
    Real2Map( iih->addr_map, &addr );
    ctl = &iih->scope;
    if( !IsInScope( ctl, addr ) ) {
        addrsym_info        info;
        seg_list            *addr_sym;

        addr_sym = DFLoadAddrSym( iih, imh );
        if( FindAddrSym( addr_sym, &addr.mach, &info ) >= 0 ) {
            DRSetDebug( iih->dwarf->handle );    /* must do at each interface */
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
        cu_tag = IMH2MODI(iih, imh )->cu_tag;
        node = FindScope( ctl->root, addr.mach.offset );
        what = cu_tag + in->unique;    /* make absolute */
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
            DCMapAddr( &out->start.mach, iih->dcmap );
            out->unique = node->what - cu_tag; /* make relative */
            ret = SR_EXACT;
        }
    }
    return( ret );
}


static drmem_hdl GetContainingClass( drmem_hdl curr )
/***************************************************/
{
    dr_tag_type     sc;

    curr = DRGetContaining( curr );
    if( curr != DRMEM_HDL_NULL ) {
        curr = DRSkipTypeChain( curr ); /* PCH typedef link */
        sc = DRGetTagType( curr );
        if( sc != DR_TAG_CLASS ) {
            curr = DRMEM_HDL_NULL;
        }
    }
    return( curr );
}


search_result DIPIMPENTRY( ScopeOuter )( imp_image_handle *iih,
                imp_mod_handle imh, scope_block *in, scope_block *out )
/*********************************************************************/
{
    search_result   ret;
    drmem_hdl       curr;
    dr_tag_type     sc;
    drmem_hdl       cu_tag;

    DRSetDebug( iih->dwarf->handle );    /* must do at each interface */
    cu_tag = IMH2MODI( iih, imh )->cu_tag;
    curr = cu_tag + in->unique;         /* make absolute */
    sc = DRGetTagType( curr );
    switch( sc ) {
    case DR_TAG_CLASS:
        DCStatus( DS_FAIL );
        ret = SR_NONE;
        break;
    case DR_TAG_FUNCTION:
        curr = GetContainingClass( curr );
        if( curr != DRMEM_HDL_NULL ) {
            *out = *in;
            out->unique = curr - cu_tag;    /* make relatine */
            ret = SR_EXACT;
        } else {
            DCStatus( DS_FAIL );
            ret = SR_NONE;
        }
        break;
    default:
        ret = DFScopeOuter( iih, imh, in, out );
    }
    return( ret );
}

/**********************************************/
/* Walk or Search various scopes for symbols  */
/**********************************************/

typedef struct {
    imp_mod_handle      imh;
    imp_image_handle    *iih;
    void                *d;
    drmem_hdl           containing;
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
    DIP_IMP_SYM_WALKER  *wk;
    imp_sym_handle      *ish;
} blk_wlk_wlk;

typedef struct {
    blk_wlk_com         com;
    strcompn_fn         *scompn;
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


static bool ASym( drmem_hdl var, int index, void *_df )
/*****************************************************/
{
    blk_wlk_wlk     *df = _df;
    bool            cont;
    imp_sym_handle  *ish;
    dr_dbg_handle   saved;

    /* unused parameters */ (void)index;

    ish = df->ish;
    ish->sclass = SYM_VAR;
    ish->imh = df->com.imh;
    ish->sym = var;
    ish->state = DF_NOT;
    saved = DRGetDebug();
    df->wr = df->wk( df->com.iih, SWI_SYMBOL, ish, df->com.d );
    DRSetDebug( saved );
    cont = true;
    if( df->wr != WR_CONTINUE ) {
        cont = false;
    }
    df->com.cont = cont;
    return( cont );
}


static bool ASymCont( drmem_hdl var, int index, void *_df )
/*********************************************************/
{
    blk_wlk_wlk     *df = _df;
    drmem_hdl       contain;
    bool            cont;

    cont = true;
    contain = DRGetContaining( var );
    if( contain != DRMEM_HDL_NULL ) {
        contain = DRSkipTypeChain( contain );   /* PCH typedef link */
    }
    if( df->com.containing == contain ) {
        cont = ASym( var, index, df );
    }
    return( cont );
}


static bool AModSym( drmem_hdl var, int index, void *_df )
/********************************************************/
{
    blk_wlk_wlk     *df = _df;
    bool            cont;
    imp_sym_handle  *ish;
    dr_tag_type     sc;
    dr_dbg_handle   saved;

    /* unused parameters */ (void)index;

    sc = DRGetTagType( var );
    if( sc == DR_TAG_NAMESPACE ) {
        DRWalkBlock( var, df->com.what, AModSym, (void *)df );
        cont = true;
    } else {
        ish = df->ish;
        ish->sclass = SYM_VAR;
        ish->imh = df->com.imh;
        ish->sym = var;
        ish->state = DF_NOT;
        saved = DRGetDebug();
        df->wr = df->wk( df->com.iih, SWI_SYMBOL, ish, df->com.d );
        DRSetDebug( saved );
        cont = true;
    }
    if( df->wr != WR_CONTINUE ) {
        cont = false;
    }
    df->com.cont = cont;
    return( cont );
}


static bool ASymLookup( drmem_hdl var, int index, void *_df )
/***********************************************************/
{
    blk_wlk_lookup  *df = _df;
    imp_sym_handle  *ish;
    size_t          len;

    /* unused parameters */ (void)index;

    len = DRGetNameBuff( var, df->buff, df->len );
    if( len == df->len
      && df->scompn( df->buff, df->li->name.start, df->li->name.len ) == 0 ) {
        /* Found symbol by name */
        if( !DRIsFunc( var ) && !DRIsStatic( var ) && !DRIsSymDefined( var ) ) {
            /* If symbol is a global variable declaration, ignore it; it
             * won't have location information and will likely be found in
             * another module.
             */
        } else {
            ish = DCSymCreate( df->com.iih, df->com.d );
            ish->sclass = SYM_VAR;
            ish->imh = df->com.imh;
            ish->sym = var;
            ish->state = DF_NOT;
            df->sr = SR_EXACT;
            df->com.cont = false;
        }
    }
    return( true );
}


static bool ASymContLookup( drmem_hdl var, int index, void *_df )
/***************************************************************/
{
    blk_wlk_lookup  *df = _df;
    drmem_hdl       contain;
    bool            cont;

    cont = true;
    contain = DRGetContaining( var );
    if( contain != DRMEM_HDL_NULL ) {
        contain = DRSkipTypeChain( contain );   /* PCH typedef link */
    }
    if( df->com.containing == contain ) {
        cont = ASymLookup( var, index, df );
    }
    return( cont );
}


static bool WalkOneBlock( blk_wlk *df, DRWLKBLK fn, drmem_hdl blk )
/*****************************************************************/
{
    DRSetDebug( df->com.iih->dwarf->handle );    /* must do at each call into DWARF */
    DRWalkBlock( blk, df->com.what, fn, (void *)df );
    return( df->com.cont );
}


static bool WalkModSymList( blk_wlk *df, DRWLKBLK fn, imp_mod_handle imh )
/************************************************************************/
{
    bool            cont;
    mod_info        *modinfo;

    df->com.imh = imh;
    modinfo = IMH2MODI( df->com.iih, imh );
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
    imp_image_handle    *iih;
    scope_block         scope;
    drmem_hdl           cu_tag;
    drmem_hdl           dbg_pch;
    dr_tag_type         sc;
    imp_mod_handle      imh;
    drmem_hdl           curr;
    bool                cont;
    mod_info            *modinfo;

    iih = df->com.iih;
    cont = true;
    if( DFAddrMod( iih, *addr, &imh ) != SR_NONE ) {
        modinfo = IMH2MODI( iih, imh );
        if( DFAddrScope( iih, imh, *addr, &scope ) != SR_NONE ) {
            df->com.imh = imh;
            cu_tag = modinfo->cu_tag;
            for( ;; ) {
                curr = cu_tag + scope.unique;   /* make absolute */
                cont = WalkOneBlock( df, fn, curr );
                if( !cont )
                    break;
                if( DFScopeOuter( iih, imh, &scope, &scope ) == SR_NONE ) {
                    cont = true;
                    break;
                }
            }
            if( cont ) {
                sc = DRGetTagType( curr );
                if( sc == DR_TAG_FUNCTION ) {
                    imp_type_handle     ith;

                    curr = GetContainingClass( curr );
                    if( curr != DRMEM_HDL_NULL ) {
                        ith.state = DF_NOT;
                        ith.type = curr;
                        ith.imh = imh;
                        if( df->com.kind == WLK_WLK ) {
                            df->wlk.wr = WalkTypeSymList( iih, &ith,
                                 df->wlk.wk, df->wlk.ish, df->com.d );
                            if( df->wlk.wr != WR_CONTINUE ) {
                                cont = false;
                            }
                        } else {
                            df->lookup.sr = SearchMbr( iih, &ith, df->lookup.li, df->com.d );
                            if( df->lookup.sr == SR_EXACT ) {
                                cont = false;
                            }
                        }
                    }
                }
            }
        }
        if( cont ) {
            cont = WalkModSymList( df, fn, imh );
        }
        dbg_pch = modinfo->dbg_pch;
        if( cont && dbg_pch != 0 ) {
            imh = CuTag2Mod( iih, dbg_pch );
            cont = WalkModSymList( df, fn, imh );
        }
    }
    return( cont );
}


static bool WalkBlockSymList( blk_wlk  *df, DRWLKBLK fn, scope_block *scope )
/***************************************************************************/
{
    imp_image_handle    *iih;
    drmem_hdl           cu_tag;
    imp_mod_handle      imh;
    drmem_hdl           blk;
    dr_tag_type         sc;
    bool                cont;

    cont = true;
    iih = df->com.iih;
    if( DFAddrMod( iih, scope->start, &imh ) != SR_NONE ) {
        DRSetDebug( df->com.iih->dwarf->handle );    /* must do at each call into DWARF */
        df->com.imh = imh;
        cu_tag = IMH2MODI( iih, imh )->cu_tag;
        blk = cu_tag + scope->unique;               /* make absolute */
        sc = DRGetTagType( blk );
        if( sc == DR_TAG_CLASS ) {
            imp_type_handle     ith;

            ith.state = DF_NOT;
            ith.type = blk;
            ith.imh = imh;
            if( df->com.kind == WLK_WLK ) {
                df->wlk.wr = WalkTypeSymList( iih, &ith, df->wlk.wk, df->wlk.ish, df->com.d );
            } else {
                df->lookup.sr = SearchMbr( iih, &ith, df->lookup.li, df->com.d );
            }
        } else {
            cont = WalkOneBlock( df, fn, blk );
        }
    }
    return( cont );
}


static bool WalkSymSymList( blk_wlk *df, DRWLKBLK fn, imp_sym_handle *ish )
/*************************************************************************/
{
    bool                cont;

    df->com.imh = ish->imh;
    if( df->com.what == DR_SRCH_ctypes && IMH2MODI( df->com.iih, ish->imh )->lang == DR_LANG_CPLUSPLUS ) {
        df->com.what = DR_SRCH_cpptypes;
    }
    cont = WalkOneBlock( df, fn, ish->sym );
    return( cont );
}


static walk_result  WalkMyLDSyms( imp_image_handle *iih, imp_mod_handle imh, void *_df )
/**************************************************************************************/
{
    blk_wlk     *df = _df;

    /* unused parameters */ (void)iih;

    df->wlk.com.imh = imh;
    WalkModSymList( df, &ASym, imh );
    return( df->wlk.wr );
}


static walk_result DFWalkSymList( imp_image_handle *iih,
                         symbol_source      ss,
                         void               *source,
                         DIP_IMP_SYM_WALKER *wk,
                         imp_sym_handle     *ish,
                         void               *d )
/******************************************************/
{
    imp_mod_handle      imh;
    walk_result         wr = 0;
    blk_wlk             df;
    bool                cont;

    df.com.iih = iih;
    df.com.d = d;
    df.com.what = DR_SRCH_func_var;
    df.com.containing = DRMEM_HDL_NULL;
    df.com.cont = true;
    df.com.kind = WLK_WLK;
    df.wlk.wk = wk;
    df.wlk.ish = ish;
    df.wlk.wr = WR_CONTINUE;
    switch( ss ) {
    case SS_TYPE: /* special case */
        wr = WalkTypeSymList( iih, (imp_type_handle *)source, wk, ish, d );
        break;
    case SS_SCOPED:
        cont = WalkScopedSymList( &df, &ASym, (address *)source );
        wr = df.wlk.wr;
        break;
    case SS_SCOPESYM:
        ish = (imp_sym_handle *)source;
        cont = WalkSymSymList( &df, &ASym, ish );
        if( cont ) {
            df.com.containing = ish->sym;
            WalkModSymList( &df, &ASymCont, ish->imh );
        }
        wr = df.wlk.wr;
        break;
    case SS_BLOCK:
        WalkBlockSymList( &df, &ASym, (scope_block *)source );
        wr = df.wlk.wr;
        break;
    case SS_MODULE:
        imh = *(imp_mod_handle *)source;
        if( imh == IMH_NOMOD ) {
            wr = DFWalkModList( iih, WalkMyLDSyms, &df );
        } else {
            WalkModSymList( &df, &AModSym, imh );
            wr = df.wlk.wr;
        }
        break;
    }
    return( wr );
}


walk_result DIPIMPENTRY( WalkSymList )( imp_image_handle    *iih,
                                        symbol_source       ss,
                                        void                *source,
                                        DIP_IMP_SYM_WALKER  *wk,
                                        imp_sym_handle      *ish,
                                        void                *d )
/***************************************************************/
{
    return( DFWalkSymList( iih, ss, source, wk, ish, d ) );
}


walk_result DIPIMPENTRY( WalkSymListEx )( imp_image_handle      *iih,
                                          symbol_source         ss,
                                          void                  *source,
                                          DIP_IMP_SYM_WALKER    *wk,
                                          imp_sym_handle        *ish,
                                          location_context      *lc,
                                           void                 *d )
/******************************************************************/
{
    /* unused parameters */ (void)lc;

    return( DFWalkSymList( iih, ss, source, wk, ish, d ) );
}


#define SH_ESCAPE       0xf0

static void CollectSymHdl( const char *ep, imp_sym_handle *ish )
/**************************************************************/
{
    byte        *sp;
    byte        curr;
    static byte escapes[] = { SH_ESCAPE, '\0', '`' };

    ++ep;
    sp = (byte *)ish;
    ++ish;
    while( sp < (byte *)ish ) {
        curr = *ep++;
        if( curr == SH_ESCAPE ) {
            curr = *ep++;
            curr = escapes[curr - 1];
        }
        *sp++ = curr;
    }
}


typedef struct {
    imp_image_handle    *iih;
    void                *d;
    strcomp_fn          *scomp;
    const char          *name;
    drmem_hdl           sym;
} hash_look_data;

static bool AHashItem( void *_find, drmem_hdl dr_sym, const char *name )
/**********************************************************************/
{
    hash_look_data  *find = _find;
    imp_sym_handle  *ish;

    if( find->scomp( name, find->name ) == 0 ) {
        find->sym = dr_sym;
        ish = DCSymCreate( find->iih, find->d );
        ish->imh = DwarfMod( find->iih, dr_sym );
        ish->sclass = SYM_VAR;
        ish->sym = dr_sym;
        ish->state = DF_NOT;
    }
    return( true );
}

typedef struct {
    const char  *p;
    size_t      len;
} strvi;

typedef struct {
    char        *p;
    size_t      len;
} strvo;

static unsigned StrVCopy( strvo *dst, strvi *src )
/************************************************/
{
    /* Copy source until null char or source runs out
     * to dest until it runs out, return total length of source
     */
    unsigned    total;

    total = src->len;
    for( ; src->len > 0; --src->len ) {
        if( *src->p == '\0' )
            break;
        if( dst->len > 0 ) {
            *dst->p = *src->p;
            ++dst->p;
            --dst->len;
        }
        ++src->p;
    }
    total -= src->len;
    return( total );
}


static unsigned QualifiedName( lookup_item  *li, char *buff, unsigned buff_size )
/******************************************************************************/
{
    unsigned    total;
    strvi       curr;
    strvo       dst;
    strvi       tmp;

    curr.p   = li->scope.start;
    curr.len = li->scope.len;
    dst.p   = buff;
    dst.len = buff_size;
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


static search_result HashSearchGbl( imp_image_handle *iih,
                                    lookup_item      *li, void *d )
/*****************************************************************/
{
    char                buff[256];
    size_t              len;
    search_result       sr;
    hash_look_data      data;
    name_wlk            wlk;

    sr = SR_NONE;
    data.iih  = iih;
    data.scomp = ( li->case_sensitive ) ? &strcmp : &stricmp;
    data.sym = DRMEM_HDL_NULL;
    len = QualifiedName( li, buff, sizeof( buff ) );
    if( len <= sizeof( buff ) ) {
        data.name = buff;
    } else {
        char    *str;
        str = DCAlloc( len + 1 );
        len = QualifiedName( li, str, len + 1 );
        data.name = str;
    }
    data.d  = d;
    wlk.fn = AHashItem;
    wlk.name = data.name;
    wlk.d = &data;
    DRSetDebug( iih->dwarf->handle );    /* must do at each call into DWARF */
    FindHashWalk( iih->name_map, &wlk );
    if( data.sym != DRMEM_HDL_NULL ) {
        sr = SR_EXACT;
    }
    if( data.name != buff ) {
        DCFree( (void *)data.name );
    }
    return( sr );
}


static search_result DoLookupSym( imp_image_handle *iih, symbol_source ss, void *source,
                                    lookup_item *li, location_context *lc, void *d )
/*************************************************************************************/
{
    imp_mod_handle      imh;
    imp_sym_handle      *ish;
    search_result       sr;
//    char                *src;
//    unsigned            len;
    blk_wlk             df;
    char                buff[256];
    bool                cont;

    /* unused parameters */ (void)lc;

    if( *(unsigned_8 *)li->name.start == SH_ESCAPE ) {
        CollectSymHdl( li->name.start, DCSymCreate( iih, d ) );
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
            sr = SearchMbr( iih, (imp_type_handle *)source, li, d  );
        }
        return( sr );
    }
    df.com.iih = iih;
    df.com.d = d;
    df.com.what = Dip2DwarfSrch( li->type );
    df.com.containing = DRMEM_HDL_NULL;
    df.com.cont = true;
    df.com.kind = WLK_LOOKUP;
    df.lookup.scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
    df.lookup.li = li;
    df.lookup.len =  li->name.len + 1;
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
            cont = true;
        }
        sr = df.lookup.sr;
        if( cont ) {
            if( DR_SRCH_func_var == df.com.what ) {
                sr = HashSearchGbl( iih, li, d );
           }
        }
        break;
    case SS_MODULE:
        imh = *(imp_mod_handle *)source;
        if( imh == IMH_NOMOD ) {
            if( DR_SRCH_func_var == df.com.what ) {
                sr = HashSearchGbl( iih, li, d );
            }
        } else {
           WalkModSymList( &df, ASymLookup, imh );
           sr = df.lookup.sr;
        }
        break;
    case SS_SCOPESYM:
        ish = (imp_sym_handle *)source;
        cont = WalkSymSymList( &df, &ASymLookup, ish );
        if( cont ) {
            df.com.containing = ish->sym;    //check for out of line defn
            WalkModSymList( &df, ASymContLookup, ish->imh );
        }
        sr = df.lookup.sr;
        break;
    }
    if( df.lookup.buff != buff ) {
        DCFree( df.lookup.buff );
    }
    return( sr );
}


search_result DIPIMPENTRY( LookupSym )( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li, void *d )
/************************************************************************/
{
    return( DoLookupSym( iih, ss, source, li, NULL, d ) );
}


search_result DIPIMPENTRY( LookupSymEx )( imp_image_handle *iih,
                symbol_source ss, void *source, lookup_item *li,
                location_context *lc, void *d )
/**************************************************************/
{
    return( DoLookupSym( iih, ss, source, li, lc, d ) );
}


int DIPIMPENTRY( SymCmp )( imp_image_handle *iih, imp_sym_handle *ish1, imp_sym_handle *ish2 )
/********************************************************************************************/
{
    /* unused parameters */ (void)iih;

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
    if( ish1->sym < ish2->sym )
        return( -1 );
    if( ish1->sym > ish2->sym )
        return( 1 );
    return( 0 );
}


dip_status DIPIMPENTRY( SymAddRef )( imp_image_handle *iih, imp_sym_handle *ish )
/*******************************************************************************/
{
    /* unused parameters */ (void)iih; (void)ish;

    return( DS_OK );
}


dip_status DIPIMPENTRY( SymRelease )( imp_image_handle *iih, imp_sym_handle *ish )
/********************************************************************************/
{
    /* unused parameters */ (void)iih; (void)ish;

    return( DS_OK );
}


dip_status DIPIMPENTRY( SymFreeAll )( imp_image_handle *iih )
/***********************************************************/
{
    /* unused parameters */ (void)iih;

    return( DS_OK );
}
