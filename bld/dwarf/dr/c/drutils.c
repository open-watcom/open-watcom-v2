/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DWARF reader utility functions.
*
****************************************************************************/


#include "bool.h"
#include "demangle.h"
#include "drpriv.h"
#include "drutils.h"


const dw_tagnum FunctionTags[] = {
    DW_TAG_subprogram, 0
};

const dw_tagnum ClassTags[] = {
    DW_TAG_class_type, DW_TAG_union_type, DW_TAG_structure_type, 0
};

const dw_tagnum TypedefTags[] = {
    DW_TAG_typedef, 0
};

const dw_tagnum EnumTags[] = {
    DW_TAG_enumeration_type, 0
};

const dw_tagnum LabelTags[] = {
    DW_TAG_label, 0
};

const dw_tagnum VariableTags[] = {
    DW_TAG_common_block, DW_TAG_variable,
    DW_TAG_formal_parameter, DW_TAG_member, 0
};

const dw_tagnum MacroTags[] = { 0 };

const dw_tagnum *const SearchTags[] = {
    FunctionTags, ClassTags, EnumTags,
    TypedefTags, VariableTags,
    MacroTags, LabelTags
};

const dw_tagnum SearchSymbolTags[] = {
    DW_TAG_class_type, DW_TAG_common_block, DW_TAG_enumeration_type,
    DW_TAG_subprogram, DW_TAG_variable, DW_TAG_member, DW_TAG_structure_type,
    DW_TAG_typedef, DW_TAG_union_type, DW_TAG_label, 0
};

const dw_tagnum SearchFunctionTags[] = {
    DW_TAG_subprogram, 0
};

const dw_tagnum SearchClassTags[] = {
    DW_TAG_class_type, DW_TAG_union_type, DW_TAG_structure_type, 0
};

const dw_tagnum SearchTypeTags[] = {
    DW_TAG_typedef, DW_TAG_enumeration_type, 0
};

const dw_tagnum SearchVariableTags[] = {
    DW_TAG_common_block, DW_TAG_variable, DW_TAG_member, 0
};

const dw_tagnum SearchFriendTags[] = {
    DW_TAG_friend, 0
};

const dw_tagnum SearchBaseTags[] = {
    DW_TAG_inheritance, 0
};

const dw_tagnum ScanKidsTags[] = {
    DW_TAG_compile_unit,  DW_TAG_lexical_block, 0
};

const dw_tagnum DeclarationTags[] = {
    DW_TAG_array_type, DW_TAG_class_type, DW_TAG_common_block, DW_TAG_constant,
    DW_TAG_enumeration_type, DW_TAG_member, DW_TAG_structure_type,
    DW_TAG_subprogram, DW_TAG_union_type, DW_TAG_variable, 0
};

const dw_tagnum *const SearchTypes[] = {
    SearchSymbolTags, SearchFunctionTags, SearchClassTags,
    SearchTypeTags, SearchVariableTags, SearchFriendTags, SearchBaseTags
};


long DR_InfoLength( drmem_hdl mod )
/****************************************/
// return length of dbg_info for mod
{
    drmem_hdl           finish;

    finish = mod + DR_VMReadDWord( mod );
    mod += sizeof( compuhdr_prologue );
    return( (long)( finish - mod ) );
}


bool DR_ScanCompileUnit( dr_search_context *ctxt,
                           DR_CUWLK fn,
                           const dw_tagnum *tagarray, dr_depth depth, void *data)
/*********************************************************************************/
/* note this modifies the start and stack fields of the context passed in */
{
    drmem_hdl           mod;
    dr_abbrev_idx       abbrev_idx;
    mod_scan_info       info;
    dw_children         haschild;
    stack_op            op;
    drmem_hdl           abbrev;
    bool                skipped;

    mod = ctxt->start;
    info.context = ctxt;

    if( ctxt->stack.free == 0 ) { // start out in an unnamed chain
        DR_ContextPush( &ctxt->stack, mod );
        DR_ContextPushOP( &ctxt->stack, DO_NOTHING );
    }

    while( mod < ctxt->end ) {
        info.handle = mod;
        abbrev_idx = DR_VMReadULEB128( &mod );
        if( abbrev_idx == 0 ) {
            op = DR_ContextPopOP( &ctxt->stack );
            switch( op ) {
            case SET_CLASS:
                ctxt->classhdl = DR_ContextPop( &ctxt->stack );
                break;
            case SET_FUNCTION:
                ctxt->functionhdl = DR_ContextPop( &ctxt->stack );
                break;
            case DO_NOTHING:
                DR_ContextPop( &ctxt->stack );
                break;
            }
            if( ctxt->stack.free < 0 ) {
                DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
            }
        } else {
            abbrev = ctxt->compunit->abbrevs[abbrev_idx];
            info.tag = DR_VMReadULEB128( &abbrev );
            haschild = DR_VMReadByte( abbrev );
            abbrev++;
            if( DR_SearchArray( tagarray, info.tag ) ) {
                info.context->start = info.handle;
                if( !fn( abbrev, mod, &info, data ) ) {
                    return( false );    // false == quit
                }
            }
            skipped = false;
            if( haschild == DW_CHILDREN_yes ) {
                if( DR_SearchArray( SearchClassTags, info.tag ) ) {
                    if( !(depth & DR_DEPTH_CLASSES) ) {
                        DR_SkipChildren( &abbrev, &mod );
                        skipped = true;
                    } else {
                        DR_ContextPush( &ctxt->stack, ctxt->classhdl );
                        DR_ContextPushOP( &ctxt->stack, SET_CLASS );
                        ctxt->classhdl = info.handle;
                    }
                } else {
                    if( DR_SearchArray( SearchFunctionTags, info.tag ) ) {
                        if( !(depth & DR_DEPTH_FUNCTIONS) ) {
                            DR_SkipChildren( &abbrev, &mod );
                            skipped = true;
                        } else {
                            DR_ContextPush( &ctxt->stack, ctxt->functionhdl );
                            DR_ContextPushOP( &ctxt->stack, SET_FUNCTION );
                            ctxt->functionhdl = info.handle;
                        }
                    } else {
                        DR_ContextPush( &ctxt->stack, info.handle );
                        DR_ContextPushOP( &ctxt->stack, DO_NOTHING );
                    }
                }
            }
            if( !skipped ) {
                DR_SkipAttribs( abbrev, &mod );
            }
        }
    }

    return( true );
}

void DR_SkipChildren( drmem_hdl *abbrev, drmem_hdl *mod )
/**************************************************************/
{
    drmem_hdl       handle;

    if( DR_ScanForAttrib( abbrev, mod, DW_AT_sibling ) ) {
        *mod = DR_ReadReference( *abbrev, *mod );
    } else {    // we have to manually skip everything
        while( !DR_ReadTagEnd( mod, &handle, NULL ) ) { // skip tag
            handle++;                                   // skip child flag
            DR_SkipAttribs( handle, mod );              // skip attribs
        }
    }
}

void DR_SkipAttribs( drmem_hdl abbrev, drmem_hdl *mod )
/************************************************************/
// skip the attributes.
{
    dw_formnum  form;
    dw_atnum    attrib;

    for( ;; ) {
        attrib = DR_VMReadULEB128( &abbrev );   // read attribute
        form = DR_VMReadULEB128( &abbrev );     // skip form
        if( attrib == 0 )
            break;
        DR_SkipForm( mod, form );
    }
}

void DR_SkipRest( drmem_hdl abbrev, drmem_hdl *mod )
/*********************************************************/
// skip the rest of the attributes.  This is designed to come after a
// DR_ScanForAttrib, which leaves abbrev pointing at the form.
{
    dw_formnum  form;
    dw_atnum    attrib;

    attrib = 1;         // arbitrary non-zero
    for( ;; ) {
        form = DR_VMReadULEB128( &abbrev );
        if( attrib == 0 )
            break;
        DR_SkipForm( mod, form );
        attrib = DR_VMReadULEB128( &abbrev );
    }
}

void DR_AllChildren( drmem_hdl mod, DR_CHILDCB fn, void *data )
/*************************************************************/
{
    drmem_hdl       abbrev;

    while( !DR_ReadTagEnd( &mod, &abbrev, NULL ) ) {
        abbrev++;   /* skip child flag */
        if( !fn( abbrev, mod, data ) )
            break;
        DR_SkipAttribs( abbrev, &mod );
    }
}

bool DR_SearchArray( const dw_tagnum *array, dw_tagnum value )
/************************************************************/
{
    while( *array != 0 ) {
        if( *array == value )
            return( true );
        array++;
    }
    return( false );
}

unsigned DR_GetAddrSize( drmem_hdl mod )
/**************************************/
/* returns the size of the address for the compile unit */
{
    return( DR_VMReadByte( mod + 10 ) );
}


void DR_SkipForm( drmem_hdl *addr, dw_formnum form )
/**************************************************/
{
    unsigned_32 value;

    switch( form ) {
    case DW_FORM_addr:
        if( DR_CurrNode->addr_size == 0 ) {
            *addr += DR_GetAddrSize( DR_FindCompileUnit( *addr ) );
        } else {
            *addr += DR_CurrNode->addr_size;
        }
        break;
    case DW_FORM_block1:
        *addr += DR_VMReadByte( *addr ) + sizeof(unsigned_8);
        break;
    case DW_FORM_block2:
        *addr += DR_VMReadWord( *addr ) + sizeof(unsigned_16);
        break;
    case DW_FORM_block4:
        *addr += DR_VMReadDWord( *addr ) + sizeof(unsigned_32);
        break;
    case DW_FORM_block:
        value = DR_VMReadULEB128( addr );
        *addr += value;
        break;
    case DW_FORM_flag:
    case DW_FORM_data1:
    case DW_FORM_ref1:
        *addr += 1;
        break;
    case DW_FORM_data2:
    case DW_FORM_ref2:
        *addr += 2;
        break;
    case DW_FORM_ref_addr:      // NYI: non-standard behaviour for form_ref
    case DW_FORM_data4:
    case DW_FORM_ref4:
        *addr += 4;
        break;
    case DW_FORM_data8:
        *addr += 8;
        break;
    case DW_FORM_sdata:
    case DW_FORM_udata:
    case DW_FORM_ref_udata:
        DR_VMSkipLEB128( addr );
        break;
    case DW_FORM_string:
        *addr += DR_VMStrLen( *addr ) + 1;
        break;
    case DW_FORM_strp:
        *addr += 4;
        break;
    case DW_FORM_indirect:
        value = DR_VMReadULEB128( addr );
        DR_SkipForm( addr, value );
        break;
    default:
        DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
    }
}

dr_formcl DR_FormClass( dw_formnum form )
/***************************************/
// class a form
{
    dr_formcl   formcl;

    switch( form ) {
    case DW_FORM_addr:
        formcl = DR_FORMCL_address;
        break;
    case DW_FORM_block1:
    case DW_FORM_block2:
    case DW_FORM_block4:
    case DW_FORM_block:
        formcl = DR_FORMCL_block;
        break;
    case DW_FORM_flag:
        formcl = DR_FORMCL_flag;
        break;
    case DW_FORM_data1:
    case DW_FORM_data2:
    case DW_FORM_data4:
    case DW_FORM_data8:
    case DW_FORM_sdata:
    case DW_FORM_udata:
        formcl = DR_FORMCL_data;
        break;
    case DW_FORM_ref1:
    case DW_FORM_ref2:
    case DW_FORM_ref4:
    case DW_FORM_ref_udata:
        formcl = DR_FORMCL_ref;
        break;
    case DW_FORM_ref_addr:
        formcl = DR_FORMCL_ref_addr;
        break;
    case DW_FORM_string:
        formcl = DR_FORMCL_string;
        break;
    case DW_FORM_indirect:
        formcl = DR_FORMCL_indirect;
        break;
    default:
        DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
        formcl = 0;
    }
    return( formcl );
}

unsigned_32 ReadConst( dw_formnum form, drmem_hdl info )
/******************************************************/
{
    unsigned_32 retval;
    dw_formnum  form1;

    switch( form ) {
    case DW_FORM_data1:
    case DW_FORM_ref1:
    case DW_FORM_flag:
        retval = DR_VMReadByte( info );
        break;
    case DW_FORM_data2:
    case DW_FORM_ref2:
        retval = DR_VMReadWord( info );
        break;
    case DW_FORM_ref_addr:      // NYI: non-standard behaviour for DW_FORM_ref
    case DW_FORM_data4:
    case DW_FORM_ref4:
        retval = DR_VMReadDWord( info );
        break;
    case DW_FORM_sdata:
        retval = DR_VMReadSLEB128( &info );
        break;
    case DW_FORM_udata:
    case DW_FORM_ref_udata:
        retval = DR_VMReadULEB128( &info );
        break;
    case DW_FORM_indirect:
        form1 = DR_VMReadULEB128( &info );
        retval = ReadConst( form1, info );
        break;
    default:
        DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
        retval = 0;
    }
    return( retval );
}

unsigned_32 DR_ReadConstant( drmem_hdl abbrev, drmem_hdl info )
/********************************************************************/
{
    dw_formnum  form;

    form = DR_VMReadULEB128( &abbrev );
    return( ReadConst( form, info ) );
}

drmem_hdl DR_ReadReference( drmem_hdl abbrev, drmem_hdl info )
/************************************************************/
// references are just a constant with the start of the compile unit added on.
{
    drmem_hdl       handle;
    dw_formnum      form;
    unsigned_32     offset;

    handle = DRMEM_HDL_NULL;
    form = DR_VMReadULEB128( &abbrev );
    offset = ReadConst( form, info );
    if( offset != 0 ) {     // if not NULL relocate
        if( form != DW_FORM_ref_addr ) {
            handle = DR_FindCompileUnit( info );
        } else {
            if( DR_CurrNode->wat_producer_ver == VER_V1 ) { // handle Watcom 10.x DWARF
                handle = DR_FindCompileUnit( info );
            } else {
                handle = DR_CurrNode->sections[DR_DEBUG_INFO].base;
            }
        }
    }
    return( handle + offset );
}

unsigned_32 DR_ReadAddr( drmem_hdl abbrev, drmem_hdl info )
/*********************************************************/
// address size dependent on CCU info
{
    unsigned     addr_size;
    dw_formnum   form;

    form = DR_VMReadULEB128( &abbrev );
    if( form != DW_FORM_addr ) {
        DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    if( DR_CurrNode->addr_size == 0 ) {
        addr_size = DR_GetAddrSize( DR_FindCompileUnit( info ) );
    } else {
        addr_size = DR_CurrNode->addr_size;
    }
    return( DR_ReadInt( info, addr_size ) );
}

char *DR_ReadString( drmem_hdl abbrev, drmem_hdl info )
/*****************************************************/
{
    dw_formnum  form;
    unsigned_32 offset;

    form = DR_VMReadULEB128( &abbrev );
    switch( form ) {
    case DW_FORM_string:
        return( DR_VMCopyString( &info ) );
    case DW_FORM_strp:
        offset = ReadConst( DW_FORM_data4, info );
        return( DR_CopyDbgSecString( &info, offset ) );
    default:
        DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    return( NULL );
}

unsigned_32 DR_ReadInt( drmem_hdl where, unsigned size )
/******************************************************/
//Read an int
{
    unsigned_32 ret;

    switch( size ) {
    case 1:
        ret = DR_VMReadByte( where );
        break;
    case 2:
        ret = DR_VMReadWord( where );
        break;
    case 4:
        ret = DR_VMReadDWord( where );
        break;
    default:
        DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
        ret = 0;
        break;
    }
    return( ret );
}

int DR_ReadFlag( drmem_hdl abbrev, drmem_hdl info )
/*************************************************/
{
    dw_formnum  form;

    form = DR_VMReadULEB128( &abbrev );
    if( form == DW_FORM_data1 || form == DW_FORM_flag ) {
        return( DR_VMReadByte( info ) );
    } else {
        DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    return( 0 );
}

dw_tagnum DR_ReadTag( drmem_hdl *entry, drmem_hdl *abbrev )
/*********************************************************/
{
    dw_tagnum       tag;
    dr_abbrev_idx   abbrev_idx;
    dr_cu_handle    cu;

    abbrev_idx = DR_VMReadULEB128( entry );
    cu = DR_FindCompileInfo( *entry );
    if( abbrev_idx >= cu->numabbrevs ) {
        DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
        *abbrev = DRMEM_HDL_NULL;
        tag = 0;
    } else {
        *abbrev = cu->abbrevs[abbrev_idx];
        tag = DR_VMReadULEB128( abbrev );
    }
    return( tag );
}

dw_tagnum DR_GetTag( drmem_hdl entry )
/************************************/
{
    drmem_hdl       tmp_abbrev;

    return( DR_ReadTag( &entry, &tmp_abbrev ) );
}

drmem_hdl DR_SkipTag( drmem_hdl *entry )
/**************************************/
{
    drmem_hdl       abbrev;

    DR_ReadTag( entry, &abbrev );
    return( abbrev );
}

bool DR_ReadTagEnd( drmem_hdl *entry, drmem_hdl *pabbrev, dw_tagnum *ptag )
/***************************************************************************/
{
    dr_abbrev_idx   abbrev_idx;
    dr_cu_handle    cu;
    drmem_hdl       abbrev;
    dw_tagnum       tag;

    abbrev = DRMEM_HDL_NULL;
    tag = 0;
    abbrev_idx = DR_VMReadULEB128( entry );
    if( abbrev_idx != 0 ) {
        cu = DR_FindCompileInfo( *entry );
        if( abbrev_idx >= cu->numabbrevs ) {
            DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
        } else {
            abbrev = cu->abbrevs[abbrev_idx];
            tag = DR_VMReadULEB128( &abbrev );
        }
    }
    *pabbrev = abbrev;
    if( ptag != NULL ) {
        *ptag = tag;
    }
    return( abbrev_idx == 0 );
}

char * DR_CopyDbgSecString( drmem_hdl *info, unsigned_32 offset )
/***************************************************************/
{
    drmem_hdl   dbgsec_str;
    char        *str;

    /* unused parameters */ (void)info;

    dbgsec_str = DR_CurrNode->sections[DR_DEBUG_STR].base + offset;
    str = DR_VMCopyString( &dbgsec_str );
    return( str );
}

bool DR_ScanForAttrib( drmem_hdl *abbrev, drmem_hdl *info, dw_atnum at )
/**********************************************************************/
/* look for a specific attribute in the list of attributes */
{
    dw_atnum    attrib;
    dw_formnum  form;
    bool        found;

    found = true;
    for( ;; ) {
        attrib = DR_VMReadULEB128( abbrev );
        if( attrib == at )
            break;
        form = DR_VMReadULEB128( abbrev );
        if( attrib == 0 ) {
            found = false;
            break;
        }
        DR_SkipForm( info, form );
    }
    return( found );
}

static const dw_tagnum CompUnitTag[] = { DW_TAG_compile_unit, 0 };

void DR_GetCompileUnitHdr( drmem_hdl mod, DR_CUWLK fn, void *data )
/*****************************************************************/
{
    dr_search_context   ctxt;
    dr_cu_handle        compunit;

    compunit = DR_FindCompileInfo( mod );
    ctxt.compunit = compunit;

    ctxt.start = compunit->start;
    ctxt.end = compunit->start + DR_VMReadDWord( compunit->start );
    ctxt.start += sizeof( compuhdr_prologue );
    ctxt.stack.size = 0;
    ctxt.stack.free = 0;
    ctxt.stack.stack = NULL;

    DR_ScanCompileUnit( &ctxt, fn, CompUnitTag, 0, data );
    DR_FreeContextStack( &ctxt.stack );
}

drmem_hdl DRENTRY DRGetCompileUnitTag( drmem_hdl comp_unit )
/***********************************************************
 * given the start of the compilation unit header
 * return the start of the DW_TAG_compile_unit
 * assume it starts after header
 */
{
    comp_unit += sizeof( compuhdr_prologue );
    return( comp_unit );
}

#define DEMANGLE_BUF_SIZE 256

char * DR_GetName( drmem_hdl abbrev, drmem_hdl entry )
/****************************************************/
{
    char    *name;
    char    buffer[DEMANGLE_BUF_SIZE];
    size_t  len;
    size_t  base_len;

    name = NULL;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_name ) ) {
        name = DR_ReadString( abbrev, entry );
        len = strlen( name );
        if( __is_mangled( name, len ) ) {
            base_len = __demangled_basename( name, len, buffer, DEMANGLE_BUF_SIZE );
            DR_REALLOC( name, base_len + 1 );
            strncpy( name, buffer, base_len + 1 );
        }
    }
    return( name );
}

void DRENTRY DRIterateCompileUnits( void *data, DRITERCUCB callback )
/*******************************************************************/
{
    dr_cu_handle    compunit;

    compunit = &DR_CurrNode->compunit;
    do {
        if( !callback( data, compunit->start ) ) {
            break;    // false == quit
        }
        compunit = compunit->next;
    } while( compunit != NULL );
}

bool DR_ScanAllCompileUnits( dr_search_context *startingCtxt, DR_CUWLK fn,
                        const dw_tagnum *tagarray, dr_depth depth, void *data )
/*****************************************************************************/
{
    bool                cont;
    dr_search_context   ctxt;
    int                 i;

    if( startingCtxt == NULL ) {
        ctxt.compunit = &DR_CurrNode->compunit;
        ctxt.start = ((dr_cu_handle)ctxt.compunit)->start;
        ctxt.end = ctxt.start + DR_VMReadDWord( ctxt.start );
        ctxt.start += sizeof( compuhdr_prologue );
        ctxt.classhdl = DRMEM_HDL_NULL;
        ctxt.functionhdl = DRMEM_HDL_NULL;
        ctxt.stack.size = 0;
        ctxt.stack.free = 0;
        ctxt.stack.stack = NULL;
    } else {
        ctxt = *startingCtxt;   /* structure copy */

        /* but allocate and copy own stack */
        ctxt.stack.stack = DR_ALLOC( ctxt.stack.size * sizeof( uint_32 ) );
        for( i = 0; i < ctxt.stack.free; i += 1 ) {
            ctxt.stack.stack[i] = startingCtxt->stack.stack[i];
        }
    }

    do {
        cont = DR_ScanCompileUnit( &ctxt, fn, tagarray, depth, data );

        ctxt.compunit = ((dr_cu_handle)ctxt.compunit)->next;
        if( ctxt.compunit ) {
            ctxt.start = ((dr_cu_handle)ctxt.compunit)->start;
            ctxt.end = ctxt.start + DR_VMReadDWord( ctxt.start );
            ctxt.start += sizeof( compuhdr_prologue );
        }
    } while( cont && ctxt.compunit != NULL );

    DR_FreeContextStack( &ctxt.stack );

    return( cont );     /* false if more symbols, true if at end of info */
}

bool DR_WalkCompileUnit( drmem_hdl mod, DR_CUWLK fn,
                        const dw_tagnum *tagarray, dr_depth depth, void *data )
/*******************************************************************************/
{
    bool                cont;
    dr_search_context   ctxt;
    dr_cu_handle        compunit;

    compunit = DR_FindCompileInfo( mod );
    ctxt.compunit = compunit;
    ctxt.start = mod;
    ctxt.end = compunit->start + DR_VMReadDWord( compunit->start );
    ctxt.classhdl = DRMEM_HDL_NULL;
    ctxt.functionhdl = DRMEM_HDL_NULL;
    ctxt.stack.size = 0;
    ctxt.stack.free = 0;
    ctxt.stack.stack = NULL;

    cont = DR_ScanCompileUnit( &ctxt, fn, tagarray, depth, data );


    DR_FreeContextStack( &ctxt.stack );

    return( cont );     /* false if more symbols, true if at end of info */
}

bool DR_WalkChildren( drmem_hdl mod, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d )
/*****************************************************************************************/
// takes an array of tags and wlks and calls wlk on tag match
// default func called if the 0 tag at end of array has a non NULL func
{
    drmem_hdl       abbrev;
    dr_abbrev_idx   abbrev_idx;
    drmem_hdl       curr;
    dw_tagnum       tag;
    dw_children     haschild;
    int             index;
    DRWLKBLK        wlk;
    dr_cu_handle    cu;

    cu = DR_FindCompileInfo( mod );
    abbrev_idx = DR_VMReadULEB128( &mod );
    if( abbrev_idx == 0 ) {
        DR_EXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }
    abbrev = cu->abbrevs[abbrev_idx];
    tag = DR_VMReadULEB128( &abbrev );
    haschild = DR_VMReadByte( abbrev );

    abbrev += sizeof( unsigned_8 );         /* skip child byte */
    DR_SkipAttribs( abbrev, &mod );
    if( haschild == DW_CHILDREN_yes ) {
        curr = mod;
        for( ;; ) {
            mod = curr;
            abbrev_idx = DR_VMReadULEB128( &curr );
            if( abbrev_idx == 0 )
                break;
            abbrev = cu->abbrevs[abbrev_idx];
            tag = DR_VMReadULEB128( &abbrev );
            haschild = DR_VMReadByte( abbrev );
            abbrev += sizeof( unsigned_8 );
            index = 0;
            while( tags[index] != 0 ) {
                if( tags[index] == tag )
                    break;
                ++index;
            }
            wlk = wlks[index];
            if( wlk != NULL ) {
                if( !wlk( mod, index, d ) ) {
                    return( false );    // false == quit
                }
            }
            if( haschild == DW_CHILDREN_yes ) {
                DR_SkipChildren( &abbrev, &curr );
            } else {
                DR_SkipAttribs( abbrev, &curr );
            }
        }
    }
    return( true );
}

bool DR_WalkContaining( drmem_hdl mod, drmem_hdl target, DRWLKBLK wlk, void *d )
/******************************************************************************/
// Walk into tags that enclose target
// The final call shoud be target else a goose chase
{
    drmem_hdl       abbrev;
    drmem_hdl       curr;
    dw_children     haschild;

    if( DR_ReadTagEnd( &mod, &abbrev, NULL ) )  /* skip tag */
        DR_EXCEPT( DREXCEP_DWARF_LIB_FAIL );
    haschild = DR_VMReadByte( abbrev );
    abbrev++;
    DR_SkipAttribs( abbrev, &mod );
    if( haschild == DW_CHILDREN_yes ) {
        curr = mod;
        for( ;; ) {
            drmem_hdl   old_abbrev;
            drmem_hdl   old_curr;

            if( curr > target )
                break;
            mod = curr;
            if( DR_ReadTagEnd( &curr, &abbrev, NULL ) ) /* skip tag */
                break;
            haschild = DR_VMReadByte( abbrev );
            abbrev++;
            old_abbrev = abbrev;
            old_curr = curr;
            if( haschild == DW_CHILDREN_yes ) {
                DR_SkipChildren( &abbrev, &curr );
            } else {
                DR_SkipAttribs( abbrev, &curr );
            }
            if( curr > target ) {           // mod < target < curr - encloses
                if( !wlk( mod, 0, d ) ){
                    return( false );        // false == quit
                }
                abbrev = old_abbrev;        // rest cause we are going in
                curr = old_curr;
                DR_SkipAttribs( abbrev, &curr );    // skip current tags stuff
            }
        }
    }
    return( true );
}

bool DR_WalkSiblings( drmem_hdl curr, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d )
/******************************************************************************************/
// takes an array of tags and wlks and calls wlk on tag match
// default func called if the 0 tag at end of array has a non NULL func
// positions curr at next tag return true if end of list
{
    drmem_hdl       abbrev;
    drmem_hdl       start;
    dw_tagnum       tag;
    dw_children     haschild;
    int             index;
    bool            cont;
    DRWLKBLK        wlk;

    cont = true;
    start = curr;
    while( !DR_ReadTagEnd( &curr, &abbrev, &tag ) ) {
        haschild = DR_VMReadByte( abbrev );
        abbrev++;
        index = 0;
        while( tags[index] != 0 ) {
            if( tags[index] == tag )
                break;
            ++index;
        }
        wlk = wlks[index];
        if( wlk != NULL ) {
            cont = wlk( start, index, d );
            if( !cont ) {
                break;
            }
        }
        if( haschild == DW_CHILDREN_yes ) {
            DR_SkipChildren( &abbrev, &curr );
        } else {
            DR_SkipAttribs( abbrev, &curr );
        }
        start = curr;
    }
    return( cont );
}

bool DR_WalkScope( drmem_hdl mod, const dw_tagnum *tags, DRWLKBLK wlk, void *d )
/********************************************************************************/
// walk a scope starting at mod
// if a block go into it, if a tag we are interested in call user
{
    drmem_hdl       abbrev;
    drmem_hdl       curr;
    dw_tagnum       tag;
    dw_children     haschild;
    int             index;

    if( !wlk( mod, 0, d ) ) // call with parent
        return( false );    // false == quit
    if( DR_ReadTagEnd( &mod, &abbrev, &tag ) ) {
        DR_EXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }
    haschild = DR_VMReadByte( abbrev );
    abbrev++;
    DR_SkipAttribs( abbrev, &mod );
    if( haschild == DW_CHILDREN_yes ) {
        int         depth;
//        int         skip;

        depth = 1;
//        skip  = true;
        curr = mod;
        for( ;; ) {
            mod = curr;
            if( DR_ReadTagEnd( &curr, &abbrev, &tag ) ) {
                if( --depth == 0 ) {
                    break;
                }
            } else {
                haschild = DR_VMReadByte( abbrev );
                abbrev++;
                index = 0;
                while( tags[index] != 0 ) {
                    if( tags[index] == tag ) {
                        if( !wlk( mod, depth, d ) )
                            return( false );    // false == quit
                        break;
                    }
                    ++index;
                }
                if( tag == DW_TAG_lexical_block ) {
                    ++depth;                    // go into block
                     DR_SkipAttribs( abbrev, &curr );
                } else {
                    if( haschild == DW_CHILDREN_yes ) { // skip fuzz
                        DR_SkipChildren( &abbrev, &curr );
                    } else {
                        DR_SkipAttribs( abbrev, &curr );
                    }
                }
            }
        }
    }
    return( true );
}

static dr_cu_handle FindCompileInfo( dr_cu_handle compunit, drmem_hdl addr )
/**************************************************************************/
{
    for( ;; ) {
        if( (addr >= compunit->start) && (addr <= compunit->end) )
            break;
        compunit = compunit->next;
        if( compunit == NULL ) {
            break;
        }
    }
    if( compunit == NULL ) {
        DR_EXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }
    return( compunit );
}

dr_cu_handle DR_FindCompileInfo( drmem_hdl addr )
/***********************************************/
/* gets the drmem_hdl of the module that addr is in */
{
    dr_cu_handle    compunit;

    compunit = DR_CurrNode->last_ccu;
    if( addr < compunit->start ) {  // start at begining
        compunit = &DR_CurrNode->compunit;
    }
    compunit = FindCompileInfo( compunit, addr );
    DR_CurrNode->last_ccu = compunit;
    return( compunit );
}

drmem_hdl DR_FindCompileUnit( drmem_hdl addr )
/********************************************/
{
    dr_cu_handle    compunit;

    compunit = DR_FindCompileInfo( addr );
    return( compunit->start );
}

#define CONTEXT_GUESS 0x10

void DR_ContextPush( dr_context_stack *stack, drmem_hdl val )
/***********************************************************/
{
    if( stack->stack == NULL ) {
        stack->stack = DR_ALLOC( CONTEXT_GUESS * sizeof( uint_32 ) );
        stack->free = 0;
        stack->size = CONTEXT_GUESS;
    }
    if( stack->free >= stack->size ) {
        stack->size += CONTEXT_GUESS;
        stack->stack = DR_REALLOC( stack->stack, stack->size * sizeof( drmem_hdl ) );
    }

    stack->stack[stack->free] = val;
    stack->free += 1;
}

drmem_hdl DR_ContextPop( dr_context_stack *stack )
/************************************************/
{
    if( stack->free <= 0 ) {
        DR_EXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }

    stack->free -= 1;
    return( stack->stack[stack->free] );
}

drmem_hdl DR_Context( dr_context_stack *stack, int up )
/*****************************************************/
{
    int free;

    free = stack->free;
    free -= up+1;
    if( free <= 0 ) {
        return( 0 );
    }
    return( stack->stack[free] );
}

void DR_FreeContextStack( dr_context_stack *stack )
/*************************************************/
{
    DR_FREE( stack->stack );
    stack->free = 0;
    stack->size = 0;
}
