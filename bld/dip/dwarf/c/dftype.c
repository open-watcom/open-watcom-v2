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
* Description:  Type handle support for DWARF DIP.
*
****************************************************************************/


#include "dfdip.h"
#include <limits.h>
#include "dfld.h"
#include "dfmod.h"
#include "dfmodinf.h"
#include "dfloc.h"
#include "dfclean.h"
#include "dftype.h"

#include "clibext.h"


typedef struct {
    drmem_hdl curr;
    int       skip;
} array_wlk_skip; /* and jump */

static bool ArrayIndexSkip( drmem_hdl index, int pos, void *_df )
{
    array_wlk_skip *df = _df;

    /* unused parameters */ (void)pos;

    if( df->skip == 0 ) {
        df->curr = index;
        return( false );
    }
    df->skip--;
    return( true );
}

static DRWLKBLK ArrayWlkNext[DR_WLKBLK_ARRSIB] = {
    ArrayIndexSkip,
    ArrayIndexSkip,
    NULL
};

static drmem_hdl GetArrayDim( drmem_hdl index, int skip ) {
// Get current or next dim handle
    array_wlk_skip df;

    df.skip = skip;
    if( !DRWalkArraySibs( index, ArrayWlkNext, &df ) ) {
        index = df.curr;
    }else{
        index = DRMEM_HDL_NULL;
    }
    return( index );
}

static bool GetStrLen( imp_image_handle *iih,
                        drmem_hdl dr_sym,
                        location_context *lc,
                        dr_typeinfo  *ret ) {
//  Find value of scalar
    addr_seg        seg;
    location_list   src;
    location_list   dst;
    imp_mod_handle  imh;
    union {
        long  l;
        short s;
        char  c;
    } val;
    unsigned        idx_size;
    mod_info        *modinfo;

    imh = DwarfMod( iih, dr_sym );
    if( imh == IMH_NOMOD ) {
        return( false );
    }
    modinfo = IMH2MODI( iih, imh );
    if( modinfo->is_segment == false ) {
        seg = SEG_DATA; // if flat hoke segment
    }else{
        EvalSeg( iih, dr_sym, &seg );
    }
    if( EvalLocation( iih, lc, dr_sym, seg, &src ) != DS_OK ) {
        return( false );
    }
    idx_size = ret->size;
    if( idx_size == 0 ) {
        idx_size = modinfo->addr_size;
    }
    LocationCreate( &dst, LT_INTERNAL, &val );
    if( DCAssignLocation( &dst, &src, idx_size ) != DS_OK ) {
        return( false );
    }
    switch( idx_size ) {
    case 1:
        ret->size = val.c;
        break;
    case 2:
        ret->size = val.s;
        break;
    case 4:
        ret->size = val.l;
        break;
    }
    return( true );
}

/***********************/
/* Walk array dims     */
/***********************/
typedef struct {
    dig_type_bound      low;
    dig_type_size       count;
    imp_image_handle    *iih;
    imp_type_handle     *ith;
    location_context    *lc;
    uint_32             num_elts;
    int                 dim;
    bool                cont;
} array_wlk_wlk;

static bool ArraySubRange( drmem_hdl tsub, int index, void *df );
static bool ArrayEnumType( drmem_hdl tenu, int index, void *df );

static DRWLKBLK ArrayWlk[DR_WLKBLK_ARRSIB] = {
    ArraySubRange,
    ArrayEnumType,
    NULL
};

static void GetArraySize( imp_image_handle *iih,
                          imp_type_handle  *ith,
                          location_context *lc ) {
//Calculate size of array starting at ith->array.index;
    drmem_hdl     dim;
    array_wlk_wlk df;
    uint_32       base_stride;
    uint_32       n_el;

    df.iih = iih;
    df.ith = ith;
    df.lc = lc;
    df.count = 1;
    df.dim = 0;
    df.cont = false;
    DRWalkArraySibs( ith->array.index, ArrayWlk, &df );
    ith->array.num_elts = df.count;
    ith->array.low = df.low;
    df.cont = true;
    dim = GetArrayDim( ith->array.index, 1 );
    if( dim != DRMEM_HDL_NULL ) {
        DRWalkArraySibs( dim, ArrayWlk, &df );
    }
    ith->array.dims = df.dim;
    ith->typeinfo.size = df.count * ith->array.base_stride;
    if( !ith->array.column_major ) {
        base_stride = ith->typeinfo.size;
        n_el = ith->array.num_elts;
        base_stride = n_el ? base_stride / n_el : 0;
        ith->array.base_stride = base_stride;
    }
    ith->array.is_set = true;
    ith->array.is_based = false;
}

static void GetArraySubSize( imp_image_handle *iih,
                          imp_type_handle  *ith,
                          location_context *lc ) {
// Calc array size one in from previous dim
    array_wlk_wlk df;
    uint_32         new_size;
    uint_32         base_stride;
    uint_32         n_el;

    df.iih = iih;
    df.ith = ith;
    df.lc = lc;
    df.count = 1;
    df.dim = 0;
    df.cont = false;
    DRWalkArraySibs( ith->array.index, ArrayWlk, &df );
    new_size = ith->typeinfo.size;
    n_el = ith->array.num_elts;
    new_size = n_el ? new_size / n_el : 0;
    if( ith->array.column_major ) {
        base_stride = ith->array.base_stride;
        base_stride *= ith->array.num_elts;
        ith->array.base_stride = base_stride;
    } else {
        base_stride = ith->typeinfo.size;
        base_stride = df.count ? base_stride / df.count : 0;
        ith->array.base_stride = base_stride;
    }
    ith->typeinfo.size = new_size;
    ith->array.num_elts = df.count;
    ith->array.low = df.low;
    --ith->array.dims;
    ith->array.is_set = true;
    ith->array.is_based = false;
}

static void InitTypeHandle( imp_image_handle *iih,
                            imp_type_handle  *ith,
                            location_context *lc )
/***********************************************************************/
//Set type handle to the base state
//If array poise at first index
{
    imp_type_handle sub_ith;
    dr_array_info   info;
    drmem_hdl       btype;
    dr_array_stat   stat;
    uint_32         base_stride;
    uint_32         n_el;

    if( ith->state == DF_NOT ) {
        DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
        DRGetTypeInfo( ith->type, &ith->typeinfo );
        ith->state = DF_SET;
        ith->sub_array = false;
        if( ith->typeinfo.kind == DR_TYPEK_ARRAY ) {
            if( ith->typeinfo.size == 0 ) {
                btype =  DRSkipTypeChain( ith->type ); /* skip modifiers and typedefs */
                stat = DRGetArrayInfo( btype, &info );
                if( stat & DR_ARRAY_STRIDE_SIZE ) {
                    base_stride = info.stride_size/8;
                }else{
                    btype = DRGetTypeAT( btype );    /* get base type */
                    sub_ith.type = btype;
                    sub_ith.imh = ith->imh;
                    sub_ith.state = DF_NOT;
                    InitTypeHandle( iih, &sub_ith, lc );
                    base_stride = sub_ith.typeinfo.size;
                }
                ith->array.base_stride = base_stride;
                ith->array.column_major = 0; /* 1 for fortran */
                if( stat & DR_ARRAY_ORDERING ) {
                    if( info.ordering == DW_ORD_col_major ) {
                        ith->array.column_major = 1;
                    }
                }else if( IMH2MODI( iih, ith->imh )->lang == DR_LANG_FORTRAN ) {
                    ith->array.column_major = 1;
                }
                if( info.child == DRMEM_HDL_NULL ) { // set info now
                    ith->array.dims = 1;
                    ith->array.low = 0;
                    ith->array.index = DRMEM_HDL_NULL;
                    if( stat & DR_ARRAY_COUNT ) {
                        if( info.count == 0 ) { // ie  char (*x)[]
                            info.count = 1;
                        }
                        ith->typeinfo.size = info.count * ith->array.base_stride;
                        ith->array.num_elts= info.count;
                    }else{
                        ith->typeinfo.size = ith->array.base_stride;
                    }
                    if( !ith->array.column_major ) {
                        base_stride = ith->typeinfo.size;
                        n_el = ith->array.num_elts;
                        base_stride = n_el ? base_stride / n_el : 0;
                        ith->array.base_stride = base_stride;
                    }
                    ith->array.is_set = true;
                    ith->array.is_based = false;
                    ith->sub_array = false;
                }else{
                    ith->sub_array = true;
                    ith->array.is_set = false;
                    ith->array.index = GetArrayDim( info.child, 0 );
                }
            }
        }else if( ith->typeinfo.kind == DR_TYPEK_STRING ) {
            if( DRStringLengthAT( ith->type ) ) {
                if( !GetStrLen( iih, ith->type, lc, &ith->typeinfo ) ) {
                    ith->typeinfo.size = 1;
                }
            }
        }
    }
    if( ith->typeinfo.kind == DR_TYPEK_ARRAY ) {
        if( !ith->array.is_set ) {
            GetArraySize( iih, ith, lc );
        }else if( ith->array.is_based ) {
            GetArraySubSize( iih, ith, lc );
        }
    }
}

struct mod_type{
    imp_image_handle    *iih;
    imp_mod_handle      imh;
    DIP_IMP_TYPE_WALKER *wk;
    imp_type_handle     *ith;
    void                *d;
    walk_result         wr;
};

static bool AType( drmem_hdl type, void *_typ_wlk, dr_search_context *cont )
/**************************************************************************/
{
    struct mod_type *typ_wlk = _typ_wlk;
    bool            ret;
    imp_type_handle *ith;
    dr_dbg_handle   saved;

    /* unused parameters */ (void)cont;

    ret = true;
    ith = typ_wlk->ith;
    ith->imh = typ_wlk->imh;
    ith->state = DF_NOT;
    ith->type = type;
    saved = DRGetDebug();
    typ_wlk->wr = typ_wlk->wk( typ_wlk->iih, ith, typ_wlk->d );
    DRSetDebug( saved );
    if( typ_wlk->wr != WR_CONTINUE ) {
        ret = false;
    }
    return( ret );
}

walk_result DIPIMPENTRY( WalkTypeList )( imp_image_handle *iih, imp_mod_handle imh,
                         DIP_IMP_TYPE_WALKER *wk, imp_type_handle *ith, void *d )
{
    drmem_hdl       cu_tag;
    struct mod_type typ_wlk;

    DRSetDebug( iih->dwarf->handle ); /* must do at each interface */
    cu_tag = IMH2MODI( iih, imh )->cu_tag;
    typ_wlk.iih = iih;
    typ_wlk.imh = imh;
    typ_wlk.wk = wk;
    typ_wlk.ith = ith;
    typ_wlk.d   = d;
    DRWalkModTypes( cu_tag, AType, &typ_wlk );
    return( typ_wlk.wr );
}

imp_mod_handle DIPIMPENTRY( TypeMod )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih;

    /*
        Return the module that the type handle comes from.
    */
    return( ith->imh );
}

void MapImpTypeInfo( dr_typeinfo *typeinfo, dig_type_info *ti )
{
    /*
        Map dwarf info to dip imp
    */
    type_kind   kind;

    switch( typeinfo->kind ) {
    case DR_TYPEK_NONE:
        kind = TK_NONE;
        break;
    case DR_TYPEK_DATA:
        kind = TK_DATA;
        break;
    case DR_TYPEK_CODE:
        kind = TK_CODE;
        break;
    case DR_TYPEK_ADDRESS:
        kind = TK_ADDRESS;
        break;
    case DR_TYPEK_VOID:
        kind = TK_VOID;
        break;
    case DR_TYPEK_BOOL:
        kind = TK_BOOL;
        break;
    case DR_TYPEK_ENUM:
        kind = TK_ENUM;
        break;
    case DR_TYPEK_CHAR:
        kind = TK_CHAR;
        break;
    case DR_TYPEK_INTEGER:
        kind = TK_INTEGER;
        break;
    case DR_TYPEK_REAL:
        kind = TK_REAL;
        break;
    case DR_TYPEK_COMPLEX:
        kind = TK_COMPLEX;
        break;
    case DR_TYPEK_STRING:
        kind = TK_STRING;
        break;
    case DR_TYPEK_POINTER:
        kind = TK_POINTER;
        break;
    case DR_TYPEK_REF:
        kind = TK_POINTER;
        break;
    case DR_TYPEK_STRUCT:
    case DR_TYPEK_UNION:
    case DR_TYPEK_CLASS:
        kind = TK_STRUCT;
        break;
    case DR_TYPEK_ARRAY:
        kind = TK_ARRAY;
        break;
    case DR_TYPEK_FUNCTION:
        kind = TK_FUNCTION;
        break;
    default:
        kind = TK_NONE;
        break;
    }
    ti->kind = kind;
    ti->size = typeinfo->size;
    ti->modifier = TM_NONE;
    ti->deref = false;
    switch( typeinfo->mclass ) {
    case DR_MOD_BASE:
        if( (ti->kind == TK_INTEGER) || (ti->kind == TK_CHAR)) {
            if( typeinfo->modifier.sign ) {
                ti->modifier = TM_SIGNED;
            } else {
                ti->modifier = TM_UNSIGNED;
            }
        }
        break;
    case DR_MOD_ADDR:
        switch( typeinfo->modifier.ptr ) {
        case DR_PTR_none:
            ti->modifier = TM_NEAR;
            break;
        case DR_PTR_near16:
        case DR_PTR_near32:
            ti->modifier = TM_NEAR;
            break;
        case DR_PTR_far16:
        case DR_PTR_far32:
            ti->modifier = TM_FAR;
            break;
        case DR_PTR_huge16:
            ti->modifier = TM_HUGE;
            break;
        }
        if( typeinfo->kind == DR_TYPEK_REF ) {
            ti->deref = true;
        }
        break;
    }
}

dip_status DIPIMPENTRY( TypeInfo )( imp_image_handle *iih,
                imp_type_handle *ith, location_context *lc, dig_type_info *ti )
{
    /*
        Fill in the type information for the type handle. The location
        context is being passed in because it might be needed to calculate
        the size of the type (variable dimensioned arrays and the like).
    */

    InitTypeHandle( iih, ith, lc );
    MapImpTypeInfo( &ith->typeinfo, ti );
    if( ti->kind == TK_INTEGER ) {  // this can be removed when 10.5 gets updated
        char *name;

        name =  DRGetName( ith->type );
        if( name != NULL ) {
            if( strcmp( name, "char" ) == 0 || strcmp( name, "unsigned char" ) == 0 ) {
                ti->kind = TK_CHAR;
            }
            DCFree( name );
        }

    }
    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeBase )( imp_image_handle *iih,
                        imp_type_handle *ith, imp_type_handle *base_ith,
                        location_context *lc, location_list *ll )
{
    drmem_hdl  btype;

    /* unused parameters */ (void)lc; (void)ll;

    /*
        Given an implementation type handle, fill in 'base' with the
        base type of the handle.
     */
    if( base_ith != ith ) {
        *base_ith = *ith;
    }
    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    if( base_ith->state == DF_SET && base_ith->sub_array ) {
        base_ith->array.index = GetArrayDim( base_ith->array.index, 1 );
        if( base_ith->array.is_based ) {
            base_ith->array.is_set = false;
        }
        base_ith->array.is_based = true;
        if( base_ith->array.index != DRMEM_HDL_NULL ) {
            return( DS_OK );
        }
    }
    btype =  DRSkipTypeChain( base_ith->type ); /* skip modifiers and typedefs */
    base_ith->type = DRGetTypeAT( btype );      /* get base type */
    if( base_ith->type == DRMEM_HDL_NULL ) {
        base_ith->type = DRMEM_HDL_VOID;        /* no type means 'void' */
    }
    base_ith->state = DF_NOT;
    return( DS_OK );
}

typedef struct {
    int_32 low;
    int_32 high;
} enum_range;

static bool AEnum( drmem_hdl var, int index, void *_de )
{
    enum_range *de = _de;
    int_32  value;

    /* unused parameters */ (void)index;

    if( !DRConstValAT( var, (uint_32 *)&value ) ) {
        return( false );
    }
    if( value < de->low ) {
        de->low = value;
    } else if( value > de->high ) {
        de->high = value;
    }
    return( true );
}

static bool ArrayEnumType( drmem_hdl tenu, int index, void *_df )
/****************************************************************/
// Find low, high bounds of enum
{
//TODO:unsigned range
    array_wlk_wlk  *df = _df;
    enum_range     de;
    int_32         count;

    /* unused parameters */ (void)index;

    de.low  = _I32_MIN;
    de.high = _I32_MAX;
    if( !DRWalkEnum( tenu, AEnum, &de ) ) {
        return( false );
    }
    df->low = de.low;
    count = de.high-de.low+1;
    df->count *= count;

    df->dim++;
    return( df->cont );
}

static bool GetSymVal( imp_image_handle *iih,
                       drmem_hdl dr_sym, location_context *lc,
                       int_32 *ret ) {
//  Find value of scalar
    drmem_hdl       dr_type;
    dr_typeinfo     typeinfo[1];
    imp_mod_handle  imh;
    addr_seg        seg;
    location_list   src;
    location_list   dst;

    dr_type = DRGetTypeAT( dr_sym );
    if( dr_type == DRMEM_HDL_NULL ) {
        return( false );
    }
    DRGetTypeInfo( dr_type, typeinfo );
    if( typeinfo->size > sizeof( *ret ) ) {
        return( false );
    }
    imh = DwarfMod( iih, dr_sym );
    if( imh == IMH_NOMOD ) {
        return( false );
    }
    if( IMH2MODI( iih, imh )->is_segment == false ) {
        seg = SEG_DATA; // if flat hoke segment
    }else{
        EvalSeg( iih, dr_sym, &seg );
    }
    if( EvalLocation( iih, lc, dr_sym, seg, &src ) != DS_OK ) {
        return( false );
    }
    LocationCreate( &dst, LT_INTERNAL, ret );
    if( DCAssignLocation( &dst, &src, typeinfo->size ) != DS_OK ) {
        return( false );
    }
    return( true );
}

static bool GetDrVal( array_wlk_wlk *df, dr_val32 *val, int_32 *ret )
/*******************************************************************/
// extract the value from val
{
    switch( val->val_class ) {
    case DR_VAL_INT:
        *ret = val->val.s;
        return( true );
    case DR_VAL_REF:
        if( DRConstValAT( val->val.ref, (uint_32*)ret ) ) {
            return( true );
        } else if( GetSymVal( df->iih, val->val.ref, df->lc, ret ) ) {
            return( true );
        }
    }
    return( false );
}

static bool ArraySubRange( drmem_hdl tsub, int index, void *_df )
/****************************************************************/
{
    array_wlk_wlk *df = _df;
    dr_subinfo info;
    int_32     low;
    int_32     high;
    int_32     count;

    /* unused parameters */ (void)index;

    DRGetSubrangeInfo( tsub, &info );
    /* DWARF 2.0 specifies lower bound defaults for C/C++ (0) and FORTRAN (1) */
    if( info.low.val_class == DR_VAL_NOT ) {
        if( IMH2MODI( df->iih, df->ith->imh )->lang == DR_LANG_FORTRAN )
            low = 1;
        else
            low = 0;
    } else {
        GetDrVal( df, &info.low, &low );
    }
    if( info.count.val_class == DR_VAL_NOT ) {
        if( info.high.val_class == DR_VAL_NOT ) {
            return( false );
        }
        GetDrVal( df, &info.high, &high );
        count = high - low +1;
    }else{
        GetDrVal( df, &info.count, &count );
    }
    df->low = low;
    df->count *= count;
    df->dim++;
    return( df->cont );
}

dip_status DIPIMPENTRY( TypeArrayInfo )( imp_image_handle *iih,
                        imp_type_handle *array_ith, location_context *lc,
                        array_info *ai, imp_type_handle *index_ith )
{
    /*
        Given an implemenation type handle that represents an array type,
        get information about the array shape and index type. The location
        context is for variable dimensioned arrays again. The 'index'
        parameter is filled in with the type of variable used to subscript
        the array. It may be NULL, in which case no information is returned.
    */

    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    if( array_ith->state == DF_NOT ) {
        InitTypeHandle( iih, array_ith, lc );
    }
    if( array_ith->state == DF_NOT ) {
        DCStatus( DS_ERR | DS_BAD_PARM );
        return( DS_ERR | DS_BAD_PARM  );
    }
    if( array_ith->typeinfo.kind != DR_TYPEK_ARRAY ) {
        DCStatus( DS_ERR | DS_BAD_PARM );
        return( DS_ERR | DS_BAD_PARM  );
    }
    ai->low_bound = array_ith->array.low;
    ai->num_elts = array_ith->array.num_elts;
    ai->num_dims = array_ith->array.dims;
    ai->column_major = array_ith->array.column_major;
    ai->stride = array_ith->array.base_stride;
    if( index_ith != NULL ) {
        index_ith->imh = array_ith->imh;
        if( array_ith->array.index == DRMEM_HDL_NULL ) { //Fake a type up
            index_ith->state = DF_SET;
            index_ith->type  = DRMEM_HDL_NULL;
            index_ith->typeinfo.size = 0;
            index_ith->typeinfo.kind = DR_TYPEK_NONE;
            index_ith->typeinfo.mclass = DR_MOD_NONE;
        }else{
            index_ith->state = DF_NOT;
            index_ith->type = array_ith->array.index;
        }
    }
    return( DS_OK );
}

/*
    Given an implementation type handle that represents a procedure type,
    get information about the return and parameter types. If the 'n'
    parameter is zero, store the return type handle into the 'parm'
    variable. Otherwise store the handle for the n'th parameter in
    'parm'.
*/
typedef struct {
    int             count;
    int             last;
    drmem_hdl       var;
} parm_wlk;

static bool AParm( drmem_hdl var, int index, void *_df )
/******************************************************/
{
    parm_wlk *df = _df;

    /* unused parameters */ (void)index;

    ++df->count;
    if( df->count == df->last ) {
        df->var = var;
        return( false );
    }else{
        return( true );
    }
}

drmem_hdl GetParmN( imp_image_handle *iih, drmem_hdl proc, int count )
/********************************************************************/
// return handle of the n parm
{
    parm_wlk    df;
    drmem_hdl   ret;

    df.count = 0;
    df.last = count;
    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    if( DRWalkBlock( proc, DR_SRCH_parm, AParm, (void *)&df ) ) {
        ret = DRMEM_HDL_NULL;
    }else{
        ret = df.var;
    }
    return( ret );
}

int GetParmCount( imp_image_handle *iih, drmem_hdl proc )
/*******************************************************/
// return handle of the n parm
{
    parm_wlk df;

    df.count = 0;
    df.last = 0;
    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    DRWalkBlock( proc, DR_SRCH_parm, AParm, (void *)&df );
    return( df.count );
}

dip_status DIPIMPENTRY( TypeProcInfo )( imp_image_handle *iih,
                imp_type_handle *proc_ith, imp_type_handle *parm_ith, unsigned n )
{
    drmem_hdl       btype;
    drmem_hdl       parm_type = DRMEM_HDL_NULL;
    dip_status      ds;

    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    btype = DRSkipTypeChain( proc_ith->type ); /* skip modifiers and typedefs */
    if( n > 0 ) {
        btype = GetParmN( iih, btype, n );
    }// if n == 0 just fall through and get type of function
    if( btype != DRMEM_HDL_NULL ) {
        parm_type = DRGetTypeAT( btype );    /* get type */
    }
    if( parm_type != DRMEM_HDL_NULL ) {
        parm_ith->state = DF_NOT;
        parm_ith->type = parm_type;
        parm_ith->imh = proc_ith->imh;
        ds = DS_OK;
    }else{
        ds = DS_FAIL;
    }
    return( ds );
}

dip_status DIPIMPENTRY( TypePtrAddrSpace )( imp_image_handle *iih,
                    imp_type_handle *ith, location_context *lc, address *a )
{
    /*
        Given an implementation type handle that represents a pointer type,
        get information about any implied address space for that pointer
        (based pointer cruft). If there is an implied address space for
        the pointer, fill in *a with the information and return DS_OK.
        Otherwise return DS_FAIL.
    */
    dip_status ds;

    ds = EvalBasedPtr( iih, lc, ith->type, a );
    return( ds );
}


int DIPIMPENTRY( TypeCmp )( imp_image_handle *iih, imp_type_handle *ith1, imp_type_handle *ith2 )
{
    /* unused parameters */ (void)iih;

    if( ith1->type < ith2->type )
        return( -1 );
    if( ith1->type > ith2->type )
        return( 1 );
    return( 0 );
}
/*****************************/
/* Structure Enum Walks      */
/*****************************/
typedef struct inh_vbase {
    struct inh_vbase *next;
    drmem_hdl        base;
} inh_vbase;

typedef struct {
    imp_mod_handle      imh;
    imp_image_handle    *iih;
    sym_sclass          sclass;
    void                *d;
    drmem_hdl           root;
    drmem_hdl           inh;
    inh_vbase           *vbase;
    enum_einfo          einfo;
} type_wlk_com;


typedef struct {
    type_wlk_com        com;
    DIP_IMP_SYM_WALKER  *wk;
    imp_sym_handle      *ish;
    walk_result         wr;
}type_wlk_wlk;

typedef struct {
    type_wlk_com     com;
    strcompn_fn      *scompn;
    lookup_item      *li;
    search_result     sr;
}type_wlk_lookup;

typedef union {
    type_wlk_com    com;
    type_wlk_wlk    sym;
    type_wlk_lookup lookup;
}type_wlk;


static bool AddBase( drmem_hdl base, inh_vbase **lnk )
/****************************************************/
//if base not in list add return false
{
    inh_vbase   *cur;

    while( (cur = *lnk) != NULL ) {
        if( base <= cur->base ) {
            if( base == cur->base ) {
                return( false );
            }
            break;
        }
        lnk = &cur->next;
    }
    cur = DCAlloc( sizeof( *cur ) );
    cur->base = base;
    cur->next = *lnk;
    *lnk = cur;
    return( true );
}

static bool FreeBases( void *_lnk )
/*********************************/
//Free bases
{
    inh_vbase   **lnk = _lnk;
    inh_vbase   *cur;
    inh_vbase   *next;

    for( cur = *lnk; cur != NULL; cur = next ) {
        next = cur->next;
        DCFree( cur );
    }
    *lnk = NULL;
    return 0;
}

static bool AMem( drmem_hdl var, int index, void *d );
static bool AInherit( drmem_hdl inh, int index, void *d );

static DRWLKBLK StrucWlk[DR_WLKBLK_STRUCT] = {
    AMem,
    AInherit,
    AMem,
    AMem,
    NULL
};

static void SetSymHandle( type_wlk *d, imp_sym_handle *ish )
// Set is with info from com
{
    ish->sclass = d->com.sclass;
    ish->imh = d->com.imh;
    ish->state = DF_NOT;
    if( d->com.sclass == SYM_ENUM ) {
        ish->f.einfo = d->com.einfo;
    }else{
        ish->f.minfo.root = d->com.root;
        ish->f.minfo.inh = d->com.inh;
    }
}

static bool AMem( drmem_hdl var, int index, void *_d )
/****************************************************/
{
    type_wlk_wlk    *d = _d;
    bool            cont;
    imp_sym_handle  *ish;
    dr_dbg_handle   saved;

    cont = true;
    ish = d->ish;
    SetSymHandle( (type_wlk *)d, ish );
    ish->sym = var;
    switch( index ) {
    case 0:
        ish->sclass = SYM_MEM;
        break;
    case 2:
        ish->sclass = SYM_MEMVAR;      // static member
        break;
    case 3:
        if( DRGetVirtuality( var ) == DR_VIRTUALITY_VIRTUAL  ) {
            ish->sclass = SYM_VIRTF;   // virtual func
        }else if( !DRIsSymDefined( var ) ) {
            ish->sclass = SYM_MEMF;    // memfunc decl
        }else{
            ish->sclass = SYM_MEMVAR;   // inlined defn treat like a var
        }
        break;
    }
    saved = DRGetDebug();
    d->wr = d->wk( d->com.iih, SWI_SYMBOL, ish, d->com.d );
    DRSetDebug( saved );
    if( d->wr != WR_CONTINUE ) {
        cont = false;
    }
    return( cont );
}

static bool AInherit( drmem_hdl inh, int index, void *_d )
/********************************************************/
{
//TODO: Need to track virtual base as not to visit same place twice
    type_wlk_wlk    *d = _d;
    bool            cont;
    drmem_hdl       btype;
    drmem_hdl       old_inh;
    imp_sym_handle  *ish;
    dr_dbg_handle   saved;
    walk_result     wr;

    /* unused parameters */ (void)index;

    cont = true;

    btype = DRGetTypeAT( inh );
    btype =  DRSkipTypeChain( btype ); /* skip modifiers and typedefs */
    if( DRGetVirtuality( inh ) == DR_VIRTUALITY_VIRTUAL ) {
        if( !AddBase( btype, &d->com.vbase ) ) {
            return( cont );
        }
    }
    ish = d->ish;
    SetSymHandle( (type_wlk *)d, ish );
    ish->sym = inh;
    ish->sclass = SYM_MEM;     //  treat inherit like a var
    saved = DRGetDebug();
    wr = d->wk( d->com.iih, SWI_INHERIT_START, ish, d->com.d );
    DRSetDebug( saved );
    if( wr == WR_CONTINUE ) {
        old_inh = d->com.inh;
        d->com.inh = inh;
        DRWalkStruct( btype, StrucWlk, d );
        d->com.inh = old_inh;
        saved = DRGetDebug();
        d->wk( d->com.iih, SWI_INHERIT_END, NULL, d->com.d );
        DRSetDebug( saved );
        if( d->wr != WR_CONTINUE ) {
            cont = false;
        }
    }
    return( cont );
}


static bool AMemLookup( drmem_hdl var, int index, void *d );
static bool AInheritLookup( drmem_hdl inh, int index, void *d );

static DRWLKBLK StrucWlkLookup[DR_WLKBLK_STRUCT] = {
    AMemLookup,
    AInheritLookup,
    AMemLookup,
    AMemLookup,
    NULL
};

static bool AMemLookup( drmem_hdl var, int index, void *_d )
/**********************************************************/
{
    type_wlk_lookup *d = _d;
    imp_sym_handle  *ish;
    char            *name;
    size_t          len;

    name =  DRGetName( var );
    if( name == NULL ) {
        DCStatus( DS_FAIL );
        return( false );
    }
    len = strlen( name );
    if( len == d->li->name.len && d->scompn( name, d->li->name.start, len ) == 0 ) {
        ish = DCSymCreate( d->com.iih, d->com.d );
        SetSymHandle( (type_wlk *)d, ish );
        ish->sym = var;
        switch( index ) {
        case 0:
            ish->sclass = SYM_MEM;
            break;
        case 2:
            ish->sclass = SYM_MEMVAR;     // static member
            break;
        case 3:
            if( DRGetVirtuality( var ) == DR_VIRTUALITY_VIRTUAL ) {
                ish->sclass = SYM_VIRTF;   // virtual func
            }else if( !DRIsSymDefined( var ) ) {
                ish->sclass = SYM_MEMF;    // memfunc decl
            }else{
                ish->sclass = SYM_MEMVAR;   // inlined defn treat like a var
            }
            break;
        }
        d->sr = SR_EXACT;
    }
    DCFree( name );
    return( true );
}

static bool AInheritLookup( drmem_hdl inh, int index, void *_d )
/**************************************************************/
//Push inherit handle and search
{
    type_wlk_lookup *d = _d;
    drmem_hdl btype;
    drmem_hdl old_inh;

    /* unused parameters */ (void)index;

    btype = DRGetTypeAT( inh );
    btype = DRSkipTypeChain( btype ); /* skip modifiers and typedefs */
    if( DRGetVirtuality( inh ) == DR_VIRTUALITY_VIRTUAL ) {
        if( !AddBase( btype, &d->com.vbase ) ) {
            return( true );
        }
    }
    old_inh = d->com.inh;
    d->com.inh = inh;
    DRWalkStruct( btype, StrucWlkLookup, d );
    d->com.inh = old_inh;
    return( true );
}

static bool AEnumMem( drmem_hdl var, int index, void *_d )
/********************************************************/
{
    type_wlk_wlk    *d = _d;
    bool            cont;
    imp_sym_handle  *ish;
    dr_dbg_handle   saved;

    /* unused parameters */ (void)index;

    cont = true;
    ish = d->ish;
    SetSymHandle( (type_wlk *)d, ish );
    ish->sym = var;
    saved = DRGetDebug();
    d->wr = d->wk( d->com.iih, SWI_SYMBOL, ish, d->com.d );
    DRSetDebug( saved );
    if( d->wr != WR_CONTINUE ) {
        cont = false;
    }
    return( cont );
}

static bool AEnumMemLookup( drmem_hdl var, int index, void *_d )
/**************************************************************/
{
    type_wlk_lookup *d = _d;
    imp_sym_handle  *ish;
    char            *name;
    size_t          len;

    /* unused parameters */ (void)index;

    name =  DRGetName( var );
    if( name == NULL ) {
        DCStatus( DS_FAIL );
        return( false );
    }
    len = strlen( name );
    if( len == d->li->name.len && d->scompn( name, d->li->name.start, len ) == 0 ) {
        ish = DCSymCreate( d->com.iih, d->com.d );
        SetSymHandle( (type_wlk *)d, ish );
        ish->sym = var;
        d->sr = SR_EXACT;
    }
    DCFree( name );
    return( true );
}

walk_result WalkTypeSymList( imp_image_handle *iih, imp_type_handle *ith,
                 DIP_IMP_SYM_WALKER *wk, imp_sym_handle *ish, void *d )
{
    drmem_hdl       btype;
    type_wlk_wlk    df;
    df_cleaner      cleanup;

    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    if( ith->state == DF_NOT ) {
        if( DRGetTypeInfo( ith->type, &ith->typeinfo ) ) {
            ith->state = DF_SET;
        }
    }
    if( ith->state == DF_NOT ) {
        return( WR_STOP );
    }
    df.com.imh = ith->imh;
    df.com.iih = iih;
    df.com.d = d;
    df.com.root = ith->type;
    df.com.inh = DRMEM_HDL_NULL;
    df.com.vbase = NULL;
    cleanup.rtn = FreeBases;   //push cleanup
    cleanup.d = &df.com.vbase;
    cleanup.prev = Cleaners;
    Cleaners = &cleanup;
    btype = DRSkipTypeChain( ith->type );
    df.ish = ish;
    df.wk = wk;
    df.wr = WR_CONTINUE;
    switch( ith->typeinfo.kind ) {
    case DR_TYPEK_ENUM:
        df.com.sclass = SYM_ENUM;
        df.com.einfo.size = ith->typeinfo.size;
        df.com.einfo.sign = ith->typeinfo.modifier.sign;
        DRWalkEnum( btype, AEnumMem, &df );
        break;
    case DR_TYPEK_STRUCT:
    case DR_TYPEK_UNION:
    case DR_TYPEK_CLASS:
        df.com.sclass = SYM_MEM;
        DRWalkStruct( btype, StrucWlk, &df );
        break;
    default:
        DCStatus( DS_ERR | DS_BAD_PARM );
        df.wr = WR_STOP;
    }
    FreeBases( &df.com.vbase ); // free virtual base list
    Cleaners = cleanup.prev; // pop cleanup
    return( df.wr );
}

search_result SearchMbr( imp_image_handle *iih, imp_type_handle *ith, lookup_item *li, void *d )
//Search for matching lookup item
{
    drmem_hdl       btype;
    type_wlk_lookup df;
    df_cleaner      cleanup;

    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    if( ith->state == DF_NOT ) {
        if( DRGetTypeInfo( ith->type, &ith->typeinfo ) ) {
            ith->state = DF_SET;
        }
    }
    if( ith->state == DF_NOT ) {
        return( SR_NONE );
    }
    df.com.imh = ith->imh;
    df.com.iih = iih;
    df.com.d = d;
    df.com.root = ith->type;
    df.com.inh = DRMEM_HDL_NULL;
    df.com.vbase = NULL;
    cleanup.rtn = FreeBases;   //push cleanup
    cleanup.d = &df.com.vbase;
    cleanup.prev = Cleaners;
    Cleaners = &cleanup;
    btype = DRSkipTypeChain( ith->type );
    df.scompn = ( li->case_sensitive ) ? strncmp : strnicmp;
    df.li = li;
    df.sr = SR_NONE;
    switch( ith->typeinfo.kind ) {
    case DR_TYPEK_ENUM:
        df.com.sclass = SYM_ENUM;
        df.com.einfo.size = ith->typeinfo.size;
        df.com.einfo.sign = ith->typeinfo.modifier.sign;
        DRWalkEnum( btype, AEnumMemLookup, &df );
        break;
    case DR_TYPEK_STRUCT:
    case DR_TYPEK_UNION:
    case DR_TYPEK_CLASS:
        df.com.sclass = SYM_MEM;
        DRWalkStruct( btype, StrucWlkLookup, &df );
        break;
    default:
        DCStatus( DS_ERR | DS_BAD_PARM );
        df.sr = SR_FAIL;
    }
    FreeBases( &df.com.vbase ); // free virtual base list
    Cleaners = cleanup.prev; // pop cleanup
    return( df.sr );
}
/*********************************************/
/*  Search for a derived type then eval loc  */
/*********************************************/
typedef struct inh_path {
    struct inh_path *next;
    drmem_hdl        inh;
}inh_path;

typedef struct type_wlk_inherit {
    imp_image_handle *iih;
    drmem_hdl         dr_derived;
    location_context *lc;
    address          *addr;
    inh_path         *head;
    inh_path         **lnk;
    dip_status       ds;
    bool             cont;
} type_wlk_inherit;

static bool AInhFind( drmem_hdl inh, int index, void *df );

static DRWLKBLK InheritWlk[DR_WLKBLK_STRUCT] = {
    NULL,
    AInhFind,
    NULL,
    NULL,
    NULL,
};


static bool AInhFind( drmem_hdl inh, int index, void *_df )
/*********************************************************/
//Push inherit handle and search
{
    type_wlk_inherit *df = _df;
    drmem_hdl       dr_derived;
    inh_path        head, *curr, **old_lnk;

    /* unused parameters */ (void)index;

    head.inh = inh;
    head.next = NULL;
    old_lnk = df->lnk;
    *df->lnk  = &head;
    df->lnk = &head.next;
    dr_derived = DRGetTypeAT( inh );        /* get base type */
    if( dr_derived == df->dr_derived ) {
        for( curr = df->head; curr != NULL; curr = curr->next ) {
            df->ds = EvalLocAdj( df->iih, df->lc, curr->inh, df->addr );
            if( df->ds != DS_OK ) {
                break;
            }
        }
        df->cont = false;
    } else {
        dr_derived =  DRSkipTypeChain( dr_derived); /* skip modifiers and typedefs */
        DRWalkStruct( dr_derived, InheritWlk, df ); /* walk struct looking for inheritance */
    }
    df->lnk = old_lnk;
    return( df->cont );
}

dip_status  DFBaseAdjust( imp_image_handle *iih, drmem_hdl base, drmem_hdl derived,
                                            location_context *lc, address *addr )
{
    type_wlk_inherit df;
    drmem_hdl        dr_base;

    df.iih = iih;
    df.dr_derived = derived;
    df.lc =  lc;
    df.addr = addr;
    df.head = NULL;
    df.lnk  = &df.head;
    df.ds = DS_FAIL;
    df.cont = true;
    dr_base =  DRSkipTypeChain( base );   /* skip modifiers and typedefs */
    DRWalkStruct( dr_base, InheritWlk, &df ); /* walk struct looking for inheritance */
    return( df.ds );
}

dip_status DIPIMPENTRY( TypeThunkAdjust )( imp_image_handle *iih,
                        imp_type_handle *base_ith, imp_type_handle *derived_ith,
                        location_context *lc, address *addr )
{
    /*
        When you convert a pointer to a C++ class to a pointer at one
        of its derived classes you have to adjust the pointer so that
        it points at the start of the derived class. The 'derived' type
        may not actually be a derived type of 'base'. In that case, return
        DS_FAIL and nothing to 'addr'. If it is a derived type, let 'disp'
        be the displacement between the 'base' type and the 'derived' type.
        You need to do the following. "addr->mach.offset += disp;".
    */
    return( DFBaseAdjust( iih, base_ith->type, derived_ith->type, lc, addr ) );
}

size_t DIPIMPENTRY( TypeName )( imp_image_handle *iih, imp_type_handle *ith,
                unsigned num, symbol_type *tag, char *buff, size_t buff_size )
{
    /*
        Given the imp_type_handle, copy the name of the type into 'buff'.
        Do not copy more than 'buff_size' - 1 characters into the buffer and
        append a trailing '\0' character. Return the real length
        of the type name (not including the trailing '\0' character) even
        if you had to truncate it to fit it into the buffer. If something
        went wrong and you can't get the type name, call DCStatus and
        return zero. NOTE: the client might pass in zero for 'buff_size'. In that
        case, just return the length of the module name and do not attempt
        to put anything into the buffer.

        Since there can be a "string" of typedef names associated with
        a type_handle, the 'num' parm indicates which one of the names
        the client wants returned. Zero is the first type name, one is
        the second, etc. Fill in '*tag' with ST_ENUM_TAG, ST_UNION_TAG,
        ST_STRUCT_TAG, ST_CLASS_TAG if the name is a enum, union, struct,
        or class tag name respectively. If not, set '*tag' to ST_NONE.

        If the type does not have a name, return zero.
    */
    char        *name = NULL;
    drmem_hdl   dr_type;
    dr_typeinfo typeinfo;
    size_t      len;

    DRSetDebug( iih->dwarf->handle ); /* must do at each call into dwarf */
    ++num;
    len = 0;
    for( dr_type = ith->type; dr_type != DRMEM_HDL_NULL; dr_type = DRGetTypeAT( dr_type ) ) {
        name =  DRGetName( dr_type );
        if( name != NULL ) {
            if( --num == 0 )
                break;
            DCFree( name );
        }
    }
    if( num == 0 ) {
        DRGetTypeInfo( dr_type, &typeinfo );
        switch( typeinfo.kind ) {
        case DR_TYPEK_ENUM:
            *tag = ST_ENUM_TAG;
            break;
        case DR_TYPEK_STRUCT:
            *tag = ST_STRUCT_TAG;
            break;
        case DR_TYPEK_UNION:
            *tag = ST_UNION_TAG;
            break;
        case DR_TYPEK_CLASS:
            *tag = ST_CLASS_TAG;
            break;
        default:
            *tag = ST_NONE;
            break;
        }
        len = NameCopy( buff, name, buff_size, 0 );
        DCFree( name );
    }
    return( len );
}

dip_status DIPIMPENTRY( TypeAddRef )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeRelease )( imp_image_handle *iih, imp_type_handle *ith )
{
    /* unused parameters */ (void)iih; (void)ith;

    return( DS_OK );
}

dip_status DIPIMPENTRY( TypeFreeAll )( imp_image_handle *iih )
{
    /* unused parameters */ (void)iih;

    return( DS_OK );
}
