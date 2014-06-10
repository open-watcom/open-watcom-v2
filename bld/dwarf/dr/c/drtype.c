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
* Description:  DWARF type information processing.
*
****************************************************************************/


#include "drpriv.h"
#include "drutils.h"

static bool   DWRGetConstAT( dr_handle abbrev, dr_handle info,
                                               unsigned at,
                                               unsigned_32  *where )
/******************************************************************/
/* look for a specific attribute in the list of attributes */
/* if found read in   */
{
    unsigned    attrib;
    unsigned    form;
    bool        ret;

    ret = FALSE;
    for( ;; ) {
        attrib = DWRVMReadULEB128( &abbrev );
        if( attrib == at ) break;
        form = DWRVMReadULEB128( &abbrev );
        if( attrib == 0 ) break;
        DWRSkipForm( &info, form );
    }
    if( attrib != 0 ) {
        *where = DWRReadConstant( abbrev, info );
        ret = TRUE;
    }
    return( ret );
}
static int DWRGetAT( dr_handle abbrev, dr_handle  info,
                     dr_val32  *vals, uint_16 const  *at )
/********************************************************/
/* look for a specific attribute in the list of attributes */
/* if found read in   */
{
    unsigned    attrib;
    unsigned    form;
    uint_16     index;
    dwr_formcl  formcl;
    uint_32     value;
    int         count;
    int         max;

    count = 0;
    for( index = 0; at[index] != 0; ++index ) {
        vals[index].val_class = DR_VAL_NOT;
    }
    max = index;
    for( ;; ) {
        attrib = DWRVMReadULEB128( &abbrev );
        if( attrib == 0 ) break;
        form = DWRVMReadULEB128( &abbrev );
        for( index = 0; index < max; ++index ) {
            if( attrib == at[index] ) {
                ++count;
                if( form == DW_FORM_indirect ) {
                    form = DWRVMReadULEB128( &info );
                }
                formcl = DWRFormClass( form );
                value = ReadConst(  form, info );
                if( formcl == DWR_FORMCL_data ) {
                    vals[index].val_class = DR_VAL_INT;
                    vals[index].val.s = value;
                } else {
                    value += DWRFindCompileUnit( info );
                    vals[index].val_class = DR_VAL_REF;
                    vals[index].val.ref = value;
                }
            }
        }
        DWRSkipForm( &info, form );
    }
    return( count );
}


static unsigned_16 const SubATList[] = {
    DW_AT_lower_bound,
    DW_AT_upper_bound,
    DW_AT_count,
    0,
};


extern void DRGetSubrangeInfo( dr_handle sub, dr_subinfo *info )
/**************************************************************/
{
    dr_handle   abbrev;
    dr_val32    vals[3];

    abbrev = DWRVMReadULEB128( &sub );
    abbrev = DWRLookupAbbrev( sub, abbrev );
    DWRVMReadULEB128( &abbrev );    /* skip tag */
    ++abbrev;                       /* skip child flag */
    DWRGetAT( abbrev, sub, vals, SubATList );
    info->low = vals[0];
    info->high = vals[1];
    info->count = vals[2];
}

static unsigned_16 const BitATList[] = {
    DW_AT_byte_size,
    DW_AT_bit_offset,
    DW_AT_bit_size,
    0,
};

extern int DRGetBitFieldInfo( dr_handle mem, dr_bitfield *info )
/**************************************************************/
{
    dr_handle   abbrev;
    dr_val32    vals[3];
    int         count;

    abbrev = DWRVMReadULEB128( &mem );
    abbrev = DWRLookupAbbrev( mem, abbrev );
    DWRVMReadULEB128( &abbrev );  /* skip tag */
    ++abbrev;                     /* skip child flag */
    count =  DWRGetAT( abbrev, mem, vals, BitATList );
    info->byte_size = vals[0];
    info->bit_offset = vals[1];
    info->bit_size = vals[2];
    return( count );
}


extern bool DRGetTypeInfo( dr_handle entry,  dr_typeinfo *info )
/**************************************************************/
// Assume entry is pointing at start of a type
{
    dr_handle   curr_ab;
    dr_handle   abbrev;
    dr_handle   curr_ent;
    dw_tagnum   tag;
    uint_32     value;
    dr_typek    kind;

    info->acc = DR_STORE_NONE;
    info->mclass = DR_MOD_NONE;

    kind = 0;
    for( ;; ) {
        if( entry == DR_HANDLE_VOID ) {
            info->kind = DR_TYPEK_VOID;
            info->mclass = DR_MOD_BASE;
            info->size = 0;
            info->modifier.sign = FALSE;
            return( TRUE );
        }
        abbrev = DWRVMReadULEB128( &entry );
        abbrev = DWRLookupAbbrev( entry, abbrev );
        tag = DWRVMReadULEB128( &abbrev );
        ++abbrev; /* skip child flag */
        switch( tag ) {
        case DW_TAG_array_type:
            kind = DR_TYPEK_ARRAY;
            goto end_loop;
        case DW_TAG_enumeration_type:
            kind = DR_TYPEK_ENUM;
            info->mclass = DR_MOD_BASE;
            goto end_loop;
        case DW_TAG_pointer_type:
            kind = DR_TYPEK_POINTER;
            info->mclass = DR_MOD_ADDR;
            goto end_loop;
        case DW_TAG_string_type:
            kind = DR_TYPEK_STRING;
            goto end_loop;
        case DW_TAG_structure_type:
            kind = DR_TYPEK_STRUCT;
            goto end_loop;
        case DW_TAG_union_type:
            kind = DR_TYPEK_UNION;
            goto end_loop;
        case DW_TAG_class_type:
            kind = DR_TYPEK_CLASS;
            goto end_loop;
        case DW_TAG_subprogram:
        case DW_TAG_subroutine_type:
            kind = DR_TYPEK_FUNCTION;
            goto end_loop;
        case DW_TAG_reference_type:
            kind = DR_TYPEK_REF;
            info->mclass = DR_MOD_ADDR;
            goto end_loop;
        case DW_TAG_ptr_to_member_type:
            kind = DR_TYPEK_ADDRESS;
            goto end_loop;
        case DW_TAG_set_type:
            kind = DR_TYPEK_DATA;
            goto end_loop;
        case DW_TAG_subrange_type:
            kind = DR_TYPEK_DATA;
            info->mclass = DR_MOD_BASE;
            goto end_loop;
            break;
        case DW_TAG_base_type:
            info->mclass = DR_MOD_BASE;
            goto end_loop;
        case DW_TAG_file_type:
            kind = DR_TYPEK_DATA;
            goto end_loop;
        case DW_TAG_thrown_type:
            kind = DR_TYPEK_CODE;
            goto end_loop;
        /*** goes for loop ***/
        case DW_TAG_const_type:
            info->acc |= DR_STORE_CONST;
            break;
        case DW_TAG_volatile_type:
            info->acc |= DR_STORE_VOLATILE;
            break;
        case DW_TAG_packed_type:
            info->acc |= DR_STORE_PACKED;
            break;
        case DW_TAG_typedef:
            break;
        default:
            goto error;
        }
        curr_ab = abbrev;
        curr_ent = entry;
        if( DWRScanForAttrib( &curr_ab, &curr_ent, DW_AT_type ) ) {
            entry = DWRReadReference( curr_ab, curr_ent );
        } else {
            goto error;
        }
    }end_loop:;
    info->kind = kind;
    if( info->mclass != DR_MOD_ADDR ) {
        if( DWRGetConstAT( abbrev, entry, DW_AT_byte_size, &value ) ) {
            info->size =  value;
        } else {
            info->size = 0;
        }
    }
    switch( info->mclass ) {
    case DR_MOD_BASE:
        if( DWRGetConstAT( abbrev, entry, DW_AT_encoding, &value ) ) {
            switch( value ) {
            case DW_ATE_address:
                info->kind = DR_TYPEK_ADDRESS;
                info->modifier.sign = FALSE;
                break;
            case DW_ATE_boolean:
                info->kind = DR_TYPEK_BOOL;
                info->modifier.sign = FALSE;
                break;
            case DW_ATE_complex_float:
                info->kind = DR_TYPEK_COMPLEX;
                break;
            case DW_ATE_float:
                info->kind = DR_TYPEK_REAL;
                break;
            case DW_ATE_signed:
                info->kind = DR_TYPEK_INTEGER;
                info->modifier.sign = TRUE;
                break;
            case DW_ATE_signed_char:
                info->kind  = DR_TYPEK_CHAR;
                info->modifier.sign = TRUE;
                break;
            case DW_ATE_unsigned:
                info->kind = DR_TYPEK_INTEGER;
                info->modifier.sign = FALSE;
                break;
            case DW_ATE_unsigned_char:
                info->kind  = DR_TYPEK_CHAR;
                info->modifier.sign = FALSE;
                break;
            default:
                goto error;
            }
        } else {
            info->modifier.sign = FALSE;
        }
        break;
    case DR_MOD_ADDR:
        if( !DWRGetConstAT( abbrev, entry, DW_AT_address_class, &value ) ) {
            value = DW_ADDR_none;
        }
        switch( value ) {
        case DW_ADDR_none:
            info->size = DWRGetAddrSize( DWRFindCompileUnit( entry ) );
            info->modifier.ptr = DR_PTR_none;
            break;
        case DW_ADDR_near16:
            info->size = 2;
            info->modifier.ptr = DR_PTR_near16;
            break;
        case DW_ADDR_far16:
            info->size = 4;
            info->modifier.ptr = DR_PTR_far16;
            break;
        case DW_ADDR_huge16:
            info->size = 4;
            info->modifier.ptr = DR_PTR_huge16;
            break;
        case DW_ADDR_near32:
            info->size = 4;
            info->modifier.ptr = DR_PTR_near32;
            break;
        case DW_ADDR_far32:
            info->size = 6;
            info->modifier.ptr = DR_PTR_far32;
            break;
        default:
            goto error;
        }
        break;
    }
    return( TRUE );
error:
    return( FALSE );
}

extern dr_ptr DRGetAddrClass( dr_handle entry )
/*********************************************/
{
    dr_handle   abbrev;
    dr_ptr      ret;
    int         value;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_address_class ) ) {
        value = DWRReadConstant( abbrev, entry );
    } else {
        value = DW_ADDR_none;
    }
    switch( value ) {
    case DW_ADDR_none:
        ret = DR_PTR_none;
        break;
    case DW_ADDR_near16:
        ret = DR_PTR_near16;
        break;
    case DW_ADDR_far16:
        ret = DR_PTR_far16;
        break;
    case DW_ADDR_huge16:
        ret = DR_PTR_huge16;
        break;
    case DW_ADDR_near32:
        ret = DR_PTR_near32;
        break;
    case DW_ADDR_far32:
        ret = DR_PTR_far32;
        break;
    default:
        ret = 0;
        break;
    }
    return( ret );
}

extern dr_handle DRGetTypeAT( dr_handle entry )
/*********************************************/
{
    dr_handle   abbrev;
    dr_handle   type;

    abbrev = DWRGetAbbrev( &entry );
    type = 0;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_type ) ) {
        type = DWRReadReference( abbrev, entry );
    }
    return( type );
}

extern dr_array_stat DRGetArrayInfo( dr_handle entry, dr_array_info *info )
/*************************************************************************/
{
    dr_handle     abbrev;
    dr_array_stat stat;
    uint_32       value;
    unsigned_8    haschild;

    stat = DR_ARRAY_NONE;
    abbrev = DWRVMReadULEB128( &entry );
    abbrev = DWRLookupAbbrev( entry, abbrev );
    DWRVMReadULEB128( &abbrev );    /* skip tag */
    haschild = DWRVMReadByte( abbrev );
    ++abbrev;                       /* skip child flag */
    if( DWRGetConstAT( abbrev, entry, DW_AT_ordering, &value ) ) {
        info->ordering = value;
        stat |= DR_ARRAY_ORDERING;
    }
    if( DWRGetConstAT( abbrev, entry, DW_AT_byte_size, &value ) ) {
        info->byte_size = value;
        stat |= DR_ARRAY_BYTE_SIZE;
    }
    if( DWRGetConstAT( abbrev, entry, DW_AT_stride_size, &value ) ) {
        info->stride_size = value;
        stat |= DR_ARRAY_STRIDE_SIZE;
    }
    if( DWRGetConstAT( abbrev, entry, DW_AT_count, &value ) ) {
        info->count = value;
        stat |= DR_ARRAY_COUNT;
    }
    if( haschild ) {
        DWRSkipAttribs( abbrev, &entry );
        info->child = entry;
    } else {
        info->child = 0;
    }
    return( stat );
}

extern dr_handle DRSkipTypeChain( dr_handle tref )
/************************************************/
// skip modifiers and typedefs
{
    dr_handle   abbrev;
    dr_handle   entry;
    uint_32     tag;

    for( ;; ) {
        entry = tref;
        abbrev = DWRVMReadULEB128( &entry );
        abbrev = DWRLookupAbbrev( entry, abbrev );
        tag = DWRVMReadULEB128( &abbrev );
        ++abbrev; /* skip child flag */
        switch( tag ) {
        case DW_TAG_const_type:
        case DW_TAG_volatile_type:
        case DW_TAG_packed_type:
        case DW_TAG_typedef:
            break;
        default:
            goto end_loop;
        }
        if( DWRScanForAttrib( &abbrev, &entry, DW_AT_type ) ) {
            entry = DWRReadReference( abbrev, entry );
            tref = entry;
        } else {
            tref = 0;
            goto end_loop;
        }
    } end_loop:;
    return( tref );
}

static unsigned_16 const MemTag[DR_WLKBLK_STRUCT] = {
    DW_TAG_member, DW_TAG_inheritance, DW_TAG_variable, DW_TAG_subprogram, 0
};

bool DRWalkStruct( dr_handle mod,  DRWLKBLK *wlks, void *d )
/**********************************************************/
// wlks[0] == member func, wlks[1] inherit func, wlks[2] default
{
    return( DWRWalkChildren( mod, MemTag, wlks, d ) );
}

static unsigned_16 const ArrayTag[DR_WLKBLK_ARRSIB] = {
    DW_TAG_subrange_type, DW_TAG_enumerator, 0
};

bool DRWalkArraySibs( dr_handle mod,  DRWLKBLK *wlks, void *d )
/*************************************************************/
// wlks[0] == subrange [1] = enumerator , 0 = Null
{
    return( DWRWalkSiblings( mod, ArrayTag, wlks, d ) );
}

static unsigned_16 const EnumTag[DR_WLKBLK_ENUMS] = {
    DW_TAG_enumerator, 0
};

bool DRWalkEnum( dr_handle mod,  DRWLKBLK wlk, void *d )
/******************************************************/
// wlks[0] == Enum  func, [1] Null
{
    DRWLKBLK    wlks[2];

    wlks[0] = wlk;
    wlks[1] = NULL;
    return( DWRWalkChildren( mod, EnumTag, wlks, d ) );
}

bool DRConstValAT( dr_handle var, uint_32 *ret )
/**********************************************/
{
    dr_handle   abbrev;
    unsigned    form;
    uint_32     val;
    dwr_formcl  formcl;

    abbrev = DWRGetAbbrev( &var );
    if( DWRScanForAttrib( &abbrev, &var, DW_AT_const_value ) ) {
        form = DWRVMReadULEB128( &abbrev );
        for( ;; ) {
            formcl = DWRFormClass( form );
            switch( formcl ) {
            case DWR_FORMCL_indirect:
                form = DWRVMReadULEB128( &var );
                break;
            case DWR_FORMCL_data:
                val = ReadConst( form, var );
                *ret = val;
                goto found;
            default:
                goto not_found;
            }
        }
    }
not_found:
    return( FALSE );
found:
    return( TRUE );
}
