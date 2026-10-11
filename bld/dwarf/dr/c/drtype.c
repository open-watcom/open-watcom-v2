/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


static bool   DR_GetConstAT( drmem_hdl abbrev, drmem_hdl info,
                                               dw_atnum at,
                                               unsigned_32  *where )
/******************************************************************/
/* look for a specific attribute in the list of attributes */
/* if found read in   */
{
    dw_atnum    attrib;
    dw_formnum  form;
    bool        ret;

    ret = false;
    for( ;; ) {
        attrib = DR_VMReadULEB128( &abbrev );
        if( attrib == at )
            break;
        form = DR_VMReadULEB128( &abbrev );
        if( attrib == 0 )
            break;
        DR_SkipForm( &info, form );
    }
    if( attrib != 0 ) {
        *where = DR_ReadConstant( abbrev, info );
        ret = true;
    }
    return( ret );
}

static int DR_GetAT( drmem_hdl abbrev, drmem_hdl  info,
                     dr_val32  *vals, const dw_atnum *at )
/********************************************************/
/* look for a specific attribute in the list of attributes */
/* if found read in   */
{
    dw_atnum    attrib;
    dw_formnum  form;
    uint_16     index;
    dr_formcl   formcl;
    uint_32     value;
    int         count;
    int         max;

    count = 0;
    for( index = 0; at[index] != 0; ++index ) {
        vals[index].val_class = DR_VAL_NOT;
    }
    max = index;
    for( ;; ) {
        attrib = DR_VMReadULEB128( &abbrev );
        if( attrib == 0 )
            break;
        form = DR_VMReadULEB128( &abbrev );
        for( index = 0; index < max; ++index ) {
            if( attrib == at[index] ) {
                ++count;
                if( form == DW_FORM_indirect ) {
                    form = DR_VMReadULEB128( &info );
                }
                formcl = DR_FormClass( form );
                value = ReadConst( form, info );
                if( formcl == DR_FORMCL_data ) {
                    vals[index].val_class = DR_VAL_INT;
                    vals[index].val.s = value;
                } else {
                    vals[index].val_class = DR_VAL_REF;
                    vals[index].val.ref = DR_FindCompileUnit( info ) + value;
                }
            }
        }
        DR_SkipForm( &info, form );
    }
    return( count );
}


static const dw_atnum SubATList[] = {
    DW_AT_lower_bound,
    DW_AT_upper_bound,
    DW_AT_count,
    0,
};


void DRENTRY DRGetSubrangeInfo( drmem_hdl sub, dr_subinfo *info )
/***************************************************************/
{
    drmem_hdl       abbrev;
    dr_val32        vals[3];

    abbrev = DR_SkipTag( &sub ) + 1;
    DR_GetAT( abbrev, sub, vals, SubATList );
    info->low = vals[0];
    info->high = vals[1];
    info->count = vals[2];
}

static const dw_atnum BitATList[] = {
    DW_AT_byte_size,
    DW_AT_bit_offset,
    DW_AT_bit_size,
    0,
};

int DRENTRY DRGetBitFieldInfo( drmem_hdl mem, dr_bitfield *info )
/***************************************************************/
{
    drmem_hdl       abbrev;
    dr_val32        vals[3];
    int             count;

    abbrev = DR_SkipTag( &mem ) + 1;
    count =  DR_GetAT( abbrev, mem, vals, BitATList );
    info->byte_size = vals[0];
    info->bit_offset = vals[1];
    info->bit_size = vals[2];
    return( count );
}


bool DRENTRY DRGetTypeInfo( drmem_hdl entry, dr_typeinfo *info )
/**************************************************************/
// Assume entry is pointing at start of a type
{
    drmem_hdl       curr_ab;
    drmem_hdl       abbrev;
    drmem_hdl       curr_ent;
    dw_tagnum       tag;
    uint_32         value;
    dr_typek        kind;

    info->acc = DR_STORE_NONE;
    info->mclass = DR_MOD_NONE;

    kind = 0;
    for( ;; ) {
        if( entry == DRMEM_HDL_VOID ) {
            info->kind = DR_TYPEK_VOID;
            info->mclass = DR_MOD_BASE;
            info->size = 0;
            info->modifier.sign = false;
            return( true );
        }
        tag = DR_ReadTag( &entry, &abbrev );
        abbrev++;   /* skip child flag */
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
        if( DR_ScanForAttrib( &curr_ab, &curr_ent, DW_AT_type ) ) {
            entry = DR_ReadReference( curr_ab, curr_ent );
        } else {
            goto error;
        }
    }end_loop:;
    info->kind = kind;
    if( info->mclass != DR_MOD_ADDR ) {
        if( DR_GetConstAT( abbrev, entry, DW_AT_byte_size, &value ) ) {
            info->size =  value;
        } else {
            info->size = 0;
        }
    }
    switch( info->mclass ) {
    case DR_MOD_BASE:
        if( DR_GetConstAT( abbrev, entry, DW_AT_encoding, &value ) ) {
            switch( value ) {
            case DW_ATE_address:
                info->kind = DR_TYPEK_ADDRESS;
                info->modifier.sign = false;
                break;
            case DW_ATE_boolean:
                info->kind = DR_TYPEK_BOOL;
                info->modifier.sign = false;
                break;
            case DW_ATE_complex_float:
                info->kind = DR_TYPEK_COMPLEX;
                break;
            case DW_ATE_float:
                info->kind = DR_TYPEK_REAL;
                break;
            case DW_ATE_signed:
                info->kind = DR_TYPEK_INTEGER;
                info->modifier.sign = true;
                break;
            case DW_ATE_signed_char:
                info->kind  = DR_TYPEK_CHAR;
                info->modifier.sign = true;
                break;
            case DW_ATE_unsigned:
                info->kind = DR_TYPEK_INTEGER;
                info->modifier.sign = false;
                break;
            case DW_ATE_unsigned_char:
                info->kind  = DR_TYPEK_CHAR;
                info->modifier.sign = false;
                break;
            default:
                goto error;
            }
        } else {
            info->modifier.sign = false;
        }
        break;
    case DR_MOD_ADDR:
        if( !DR_GetConstAT( abbrev, entry, DW_AT_address_class, &value ) ) {
            value = DW_ADDR_none;
        }
        switch( value ) {
        case DW_ADDR_none:
            info->size = DR_GetAddrSize( DR_FindCompileUnit( entry ) );
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
    return( true );
error:
    return( false );
}

dr_ptr DRENTRY DRGetAddrClass( drmem_hdl entry )
/**********************************************/
{
    drmem_hdl   abbrev;
    dr_ptr      ret;
    dw_addr     value;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_address_class ) ) {
        value = (dw_addr)DR_ReadConstant( abbrev, entry );
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

drmem_hdl DRENTRY DRGetTypeAT( drmem_hdl entry )
/**********************************************/
{
    drmem_hdl   abbrev;
    drmem_hdl   type;

    abbrev = DR_SkipTag( &entry ) + 1;
    type = DRMEM_HDL_NULL;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_type ) ) {
        type = DR_ReadReference( abbrev, entry );
    }
    return( type );
}

dr_array_stat DRENTRY DRGetArrayInfo( drmem_hdl entry, dr_array_info *info )
/**************************************************************************/
{
    drmem_hdl       abbrev;
    dr_array_stat   stat;
    uint_32         value;
    dw_children     haschild;

    stat = DR_ARRAY_NONE;
    abbrev = DR_SkipTag( &entry );  /* skip tag */
    haschild = DR_VMReadByte( abbrev );
    abbrev++;
    if( DR_GetConstAT( abbrev, entry, DW_AT_ordering, &value ) ) {
        info->ordering = value;
        stat |= DR_ARRAY_ORDERING;
    }
    if( DR_GetConstAT( abbrev, entry, DW_AT_byte_size, &value ) ) {
        info->byte_size = value;
        stat |= DR_ARRAY_BYTE_SIZE;
    }
    if( DR_GetConstAT( abbrev, entry, DW_AT_stride_size, &value ) ) {
        info->stride_size = value;
        stat |= DR_ARRAY_STRIDE_SIZE;
    }
    if( DR_GetConstAT( abbrev, entry, DW_AT_count, &value ) ) {
        info->count = value;
        stat |= DR_ARRAY_COUNT;
    }
    if( haschild == DW_CHILDREN_yes ) {
        DR_SkipAttribs( abbrev, &entry );
        info->child = entry;
    } else {
        info->child = DRMEM_HDL_NULL;
    }
    return( stat );
}

drmem_hdl DRENTRY DRSkipTypeChain( drmem_hdl tref )
/**************************************************
 * skip modifiers and typedefs
 */
{
    drmem_hdl       abbrev;
    drmem_hdl       entry;
    dw_tagnum       tag;

    for( ;; ) {
        entry = tref;
        tag = DR_ReadTag( &entry, &abbrev );
        abbrev++;   /* skip child flag */
        switch( tag ) {
        case DW_TAG_const_type:
        case DW_TAG_volatile_type:
        case DW_TAG_packed_type:
        case DW_TAG_typedef:
            break;
        default:
            goto end_loop;
        }
        if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_type ) ) {
            entry = DR_ReadReference( abbrev, entry );
            tref = entry;
        } else {
            tref = DRMEM_HDL_NULL;
            break;
        }
    } end_loop:;
    return( tref );
}

static const dw_tagnum MemTag[DR_WLKBLK_STRUCT] = {
    DW_TAG_member,
    DW_TAG_inheritance,
    DW_TAG_variable,
    DW_TAG_subprogram,
    0
};

bool DRENTRY DRWalkStruct( drmem_hdl mod, const DRWLKBLK *wlks, void *d )
/************************************************************************
 * wlks[0] == member func, wlks[1] inherit func, wlks[2] default
 */
{
    return( DR_WalkChildren( mod, MemTag, wlks, d ) );
}

static const dw_tagnum ArrayTag[DR_WLKBLK_ARRSIB] = {
    DW_TAG_subrange_type,
    DW_TAG_enumerator,
    0
};

bool DRENTRY DRWalkArraySibs( drmem_hdl mod, const DRWLKBLK *wlks, void *d )
/***************************************************************************
 * wlks[0] == subrange [1] = enumerator , 0 = Null
 */
{
    return( DR_WalkSiblings( mod, ArrayTag, wlks, d ) );
}

static const dw_tagnum EnumTag[DR_WLKBLK_ENUMS] = {
    DW_TAG_enumerator,
    0
};

bool DRENTRY DRWalkEnum( drmem_hdl mod,  DRWLKBLK wlk, void *d )
/***************************************************************
 * wlks[0] == Enum  func, [1] Null
 */
{
    DRWLKBLK    wlks[2];

    wlks[0] = wlk;
    wlks[1] = NULL;
    return( DR_WalkChildren( mod, EnumTag, wlks, d ) );
}

bool DRENTRY DRConstValAT( drmem_hdl var, uint_32 *ret )
/******************************************************/
{
    drmem_hdl   abbrev;
    dw_formnum  form;
    uint_32     val;
    dr_formcl   formcl;

    abbrev = DR_SkipTag( &var ) + 1;
    if( DR_ScanForAttrib( &abbrev, &var, DW_AT_const_value ) ) {
        form = DR_VMReadULEB128( &abbrev );
        for( ;; ) {
            formcl = DR_FormClass( form );
            switch( formcl ) {
            case DR_FORMCL_indirect:
                form = DR_VMReadULEB128( &var );
                break;
            case DR_FORMCL_data:
                val = ReadConst( form, var );
                *ret = val;
                goto found;
            default:
                goto not_found;
            }
        }
    }
not_found:
    return( false );
found:
    return( true );
}
