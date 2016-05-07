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
* Description:  DWARF reader utility functions.
*
****************************************************************************/


#include <string.h>
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


long DWRInfoLength( drmem_hdl mod )
/****************************************/
// return length of dbg_info for mod
{
    drmem_hdl           finish;

    finish = mod + DWRVMReadDWord( mod );
    mod += sizeof( compuhdr_prologue );
    return( (long)( finish - mod ) );
}


bool DWRScanCompileUnit( dr_search_context *ctxt,
                           DWRCUWLK fn,
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
        DWRContextPush( &ctxt->stack, mod );
        DWRContextPushOP( &ctxt->stack, DO_NOTHING );
    }

    while( mod < ctxt->end ) {
        info.handle = mod;
        abbrev_idx = DWRVMReadULEB128( &mod );
        if( abbrev_idx == 0 ) {
            op = DWRContextPopOP( &ctxt->stack );
            switch( op ) {
            case SET_CLASS:
                ctxt->classhdl = DWRContextPop( &ctxt->stack );
                break;
            case SET_FUNCTION:
                ctxt->functionhdl = DWRContextPop( &ctxt->stack );
                break;
            case DO_NOTHING:
                DWRContextPop( &ctxt->stack );
                break;
            }
            if( ctxt->stack.free < 0 ) {
                DWREXCEPT( DREXCEP_BAD_DBG_INFO );
            }
        } else {
            abbrev = ctxt->compunit->abbrevs[abbrev_idx];
            info.tag = DWRVMReadULEB128( &abbrev );
            haschild = DWRVMReadByte( abbrev );
            abbrev++;
            if( DWRSearchArray( tagarray, info.tag ) ) {
                info.context->start = info.handle;
                if( !fn( abbrev, mod, &info, data ) ) {
                    return( false );    // false == quit
                }
            }
            skipped = false;
            if( haschild == DW_CHILDREN_yes ) {
                if( DWRSearchArray( SearchClassTags, info.tag ) ) {
                    if( !(depth & DR_DEPTH_CLASSES) ) {
                        DWRSkipChildren( &abbrev, &mod );
                        skipped = true;
                    } else {
                        DWRContextPush( &ctxt->stack, ctxt->classhdl );
                        DWRContextPushOP( &ctxt->stack, SET_CLASS );
                        ctxt->classhdl = info.handle;
                    }
                } else {
                    if( DWRSearchArray( SearchFunctionTags, info.tag ) ) {
                        if( !(depth & DR_DEPTH_FUNCTIONS) ) {
                            DWRSkipChildren( &abbrev, &mod );
                            skipped = true;
                        } else {
                            DWRContextPush( &ctxt->stack, ctxt->functionhdl );
                            DWRContextPushOP( &ctxt->stack, SET_FUNCTION );
                            ctxt->functionhdl = info.handle;
                        }
                    } else {
                        DWRContextPush( &ctxt->stack, info.handle );
                        DWRContextPushOP( &ctxt->stack, DO_NOTHING );
                    }
                }
            }
            if( !skipped ) {
                DWRSkipAttribs( abbrev, &mod );
            }
        }
    }

    return( true );
}

void DWRSkipChildren( drmem_hdl *abbrev, drmem_hdl *mod )
/**************************************************************/
{
    drmem_hdl       handle;

    if( DWRScanForAttrib( abbrev, mod, DW_AT_sibling ) ) {
        *mod = DWRReadReference( *abbrev, *mod );
    } else {    // we have to manually skip everything
        while( !DWRReadTagEnd( mod, &handle, NULL ) ) { // skip tag
            handle++;                                   // skip child flag
            DWRSkipAttribs( handle, mod );              // skip attribs
        }
    }
}

void DWRSkipAttribs( drmem_hdl abbrev, drmem_hdl *mod )
/************************************************************/
// skip the attributes.
{
    dw_formnum  form;
    dw_atnum    attrib;

    for( ;; ) {
        attrib = DWRVMReadULEB128( &abbrev );   // read attribute
        form = DWRVMReadULEB128( &abbrev );     // skip form
        if( attrib == 0 )
            break;
        DWRSkipForm( mod, form );
    }
}

void DWRSkipRest( drmem_hdl abbrev, drmem_hdl *mod )
/*********************************************************/
// skip the rest of the attributes.  This is designed to come after a
// DWRScanForAttrib, which leaves abbrev pointing at the form.
{
    dw_formnum  form;
    dw_atnum    attrib;

    attrib = 1;         // arbitrary non-zero
    for( ;; ) {
        form = DWRVMReadULEB128( &abbrev );
        if( attrib == 0 )
            break;
        DWRSkipForm( mod, form );
        attrib = DWRVMReadULEB128( &abbrev );
    }
}

void DWRAllChildren( drmem_hdl mod, DWRCHILDCB fn, void *data )
/*************************************************************/
{
    drmem_hdl       abbrev;

    while( !DWRReadTagEnd( &mod, &abbrev, NULL ) ) {
        abbrev++;   /* skip child flag */
        if( !fn( abbrev, mod, data ) )
            break;
        DWRSkipAttribs( abbrev, &mod );
    }
}

bool DWRSearchArray( const dw_tagnum *array, dw_tagnum value )
/************************************************************/
{
    while( *array != 0 ) {
        if( *array == value )
            return( true );
        array++;
    }
    return( false );
}

unsigned DWRGetAddrSize( drmem_hdl mod )
/**************************************/
/* returns the size of the address for the compile unit */
{
    return( DWRVMReadByte( mod + 10 ) );
}


void DWRSkipForm( drmem_hdl *addr, dw_formnum form )
/**************************************************/
{
    unsigned_32 value;

    switch( form ) {
    case DW_FORM_addr:
        if( DWRCurrNode->addr_size == 0 ) {
            *addr += DWRGetAddrSize( DWRFindCompileUnit( *addr ) );
        } else {
            *addr += DWRCurrNode->addr_size;
        }
        break;
    case DW_FORM_block1:
        *addr += DWRVMReadByte( *addr ) + sizeof(unsigned_8);
        break;
    case DW_FORM_block2:
        *addr += DWRVMReadWord( *addr ) + sizeof(unsigned_16);
        break;
    case DW_FORM_block4:
        *addr += DWRVMReadDWord( *addr ) + sizeof(unsigned_32);
        break;
    case DW_FORM_block:
        value = DWRVMReadULEB128( addr );
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
        DWRVMSkipLEB128( addr );
        break;
    case DW_FORM_string:
        *addr += DWRVMStrLen( *addr ) + 1;
        break;
    case DW_FORM_strp:
        *addr += 4;
        break;
    case DW_FORM_indirect:
        value = DWRVMReadULEB128( addr );
        DWRSkipForm( addr, value );
        break;
    default:
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
    }
}

dwr_formcl DWRFormClass( dw_formnum form )
/****************************************/
// class a form
{
    dwr_formcl formcl;

    switch( form ) {
    case DW_FORM_addr:
        formcl = DWR_FORMCL_address;
        break;
    case DW_FORM_block1:
    case DW_FORM_block2:
    case DW_FORM_block4:
    case DW_FORM_block:
        formcl = DWR_FORMCL_block;
        break;
    case DW_FORM_flag:
        formcl = DWR_FORMCL_flag;
        break;
    case DW_FORM_data1:
    case DW_FORM_data2:
    case DW_FORM_data4:
    case DW_FORM_data8:
    case DW_FORM_sdata:
    case DW_FORM_udata:
        formcl = DWR_FORMCL_data;
        break;
    case DW_FORM_ref1:
    case DW_FORM_ref2:
    case DW_FORM_ref4:
    case DW_FORM_ref_udata:
        formcl = DWR_FORMCL_ref;
        break;
    case DW_FORM_ref_addr:
        formcl = DWR_FORMCL_ref_addr;
        break;
    case DW_FORM_string:
        formcl = DWR_FORMCL_string;
        break;
    case DW_FORM_indirect:
        formcl = DWR_FORMCL_indirect;
        break;
    default:
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
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
        retval = DWRVMReadByte( info );
        break;
    case DW_FORM_data2:
    case DW_FORM_ref2:
        retval = DWRVMReadWord( info );
        break;
    case DW_FORM_ref_addr:      // NYI: non-standard behaviour for DW_FORM_ref
    case DW_FORM_data4:
    case DW_FORM_ref4:
        retval = DWRVMReadDWord( info );
        break;
    case DW_FORM_sdata:
        retval = DWRVMReadSLEB128( &info );
        break;
    case DW_FORM_udata:
    case DW_FORM_ref_udata:
        retval = DWRVMReadULEB128( &info );
        break;
    case DW_FORM_indirect:
        form1 = DWRVMReadULEB128( &info );
        retval = ReadConst( form1, info );
        break;
    default:
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
        retval = 0;
    }
    return( retval );
}

unsigned_32 DWRReadConstant( drmem_hdl abbrev, drmem_hdl info )
/********************************************************************/
{
    dw_formnum  form;

    form = DWRVMReadULEB128( &abbrev );
    return( ReadConst( form, info ) );
}

drmem_hdl DWRReadReference( drmem_hdl abbrev, drmem_hdl info )
/************************************************************/
// references are just a constant with the start of the compile unit added on.
{
    drmem_hdl       handle;
    dw_formnum      form;
    unsigned_32     offset;

    handle = DR_HANDLE_NUL;
    form = DWRVMReadULEB128( &abbrev );
    offset = ReadConst( form, info );
    if( offset != 0 ) {     // if not NULL relocate
        if( form != DW_FORM_ref_addr ) {
            handle = DWRFindCompileUnit( info );
        } else {
            if( DWRCurrNode->wat_producer_ver == VER_V1 ) { // handle Watcom 10.x DWARF
                handle = DWRFindCompileUnit( info );
            } else {
                handle = DWRCurrNode->sections[DR_DEBUG_INFO].base;
            }
        }
    }
    return( handle + offset );
}

unsigned_32 DWRReadAddr( drmem_hdl abbrev, drmem_hdl info )
/*********************************************************/
// address size dependent on CCU info
{
    unsigned     addr_size;
    dw_formnum   form;

    form = DWRVMReadULEB128( &abbrev );
    if( form != DW_FORM_addr ) {
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    if( DWRCurrNode->addr_size == 0 ) {
        addr_size = DWRGetAddrSize( DWRFindCompileUnit( info ) );
    } else {
        addr_size = DWRCurrNode->addr_size;
    }
    return( DWRReadInt( info, addr_size ) );
}

char *DWRReadString( drmem_hdl abbrev, drmem_hdl info )
/*****************************************************/
{
    dw_formnum  form;
    unsigned_32 offset;

    form = DWRVMReadULEB128( &abbrev );
    switch( form ) {
    case DW_FORM_string:
        return( DWRVMCopyString( &info ) );
    case DW_FORM_strp:
        offset = ReadConst( DW_FORM_data4, info );
        return( DWRCopyDbgSecString( &info, offset ) );
    default:
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    return( NULL );
}

unsigned_32 DWRReadInt( drmem_hdl where, unsigned size )
/******************************************************/
//Read an int
{
    unsigned_32 ret;

    switch( size ) {
    case 1:
        ret = DWRVMReadByte( where );
        break;
    case 2:
        ret = DWRVMReadWord( where );
        break;
    case 4:
        ret = DWRVMReadDWord( where );
        break;
    default:
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
        ret = 0;
        break;
    }
    return( ret );
}

int DWRReadFlag( drmem_hdl abbrev, drmem_hdl info )
/*************************************************/
{
    dw_formnum  form;

    form = DWRVMReadULEB128( &abbrev );
    if( form == DW_FORM_data1 || form == DW_FORM_flag ) {
        return( DWRVMReadByte( info ) );
    } else {
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    return( 0 );
}

dw_tagnum DWRReadTag( drmem_hdl *entry, drmem_hdl *abbrev )
/*********************************************************/
{
    dw_tagnum       tag;
    dr_abbrev_idx   abbrev_idx;
    compunit_info   *cu;

    abbrev_idx = DWRVMReadULEB128( entry );
    cu = DWRFindCompileInfo( *entry );
    if( abbrev_idx >= cu->numabbrevs ) {
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
        *abbrev = DR_HANDLE_NUL;
        tag = 0;
    } else {
        *abbrev = cu->abbrevs[abbrev_idx];
        tag = DWRVMReadULEB128( abbrev );
    }
    return( tag );
}

dw_tagnum DWRGetTag( drmem_hdl entry )
/************************************/
{
    drmem_hdl       tmp_abbrev;

    return( DWRReadTag( &entry, &tmp_abbrev ) );
}

drmem_hdl DWRSkipTag( drmem_hdl *entry )
/**************************************/
{
    drmem_hdl       abbrev;

    DWRReadTag( entry, &abbrev );
    return( abbrev );
}

bool DWRReadTagEnd( drmem_hdl *entry, drmem_hdl *pabbrev, dw_tagnum *ptag )
/***************************************************************************/
{
    dr_abbrev_idx   abbrev_idx;
    compunit_info   *cu;
    drmem_hdl       abbrev;
    dw_tagnum       tag;

    abbrev = DR_HANDLE_NUL;
    tag = 0;
    abbrev_idx = DWRVMReadULEB128( entry );
    if( abbrev_idx != 0 ) {
        cu = DWRFindCompileInfo( *entry );
        if( abbrev_idx >= cu->numabbrevs ) {
            DWREXCEPT( DREXCEP_BAD_DBG_INFO );
        } else {
            abbrev = cu->abbrevs[abbrev_idx];
            tag = DWRVMReadULEB128( &abbrev );
        }
    }
    *pabbrev = abbrev;
    if( ptag != NULL ) {
        *ptag = tag;
    }
    return( abbrev_idx == 0 );
}

char * DWRCopyDbgSecString( drmem_hdl *info, unsigned_32 offset )
/***************************************************************/
{
    drmem_hdl   dbgsec_str;
    char        *str;

    info = info;
    dbgsec_str = DWRCurrNode->sections[DR_DEBUG_STR].base + offset;
    str = DWRVMCopyString( &dbgsec_str );
    return( str );
}

bool DWRScanForAttrib( drmem_hdl *abbrev, drmem_hdl *info, dw_atnum at )
/**********************************************************************/
/* look for a specific attribute in the list of attributes */
{
    dw_atnum    attrib;
    dw_formnum  form;
    bool        found;

    found = true;
    for( ;; ) {
        attrib = DWRVMReadULEB128( abbrev );
        if( attrib == at )
            break;
        form = DWRVMReadULEB128( abbrev );
        if( attrib == 0 ) {
            found = false;
            break;
        }
        DWRSkipForm( info, form );
    }
    return( found );
}

static const dw_tagnum CompUnitTag[] = { DW_TAG_compile_unit, 0 };

void DWRGetCompileUnitHdr( drmem_hdl mod, DWRCUWLK fn, void *data )
/*****************************************************************/
{
    dr_search_context   ctxt;
    compunit_info       *compunit;

    compunit = DWRFindCompileInfo( mod );
    ctxt.compunit = compunit;

    ctxt.start = compunit->start;
    ctxt.end = compunit->start + DWRVMReadDWord( compunit->start );
    ctxt.start += sizeof( compuhdr_prologue );
    ctxt.stack.size = 0;
    ctxt.stack.free = 0;
    ctxt.stack.stack = NULL;

    DWRScanCompileUnit( &ctxt, fn, CompUnitTag, 0, data );
    DWRFreeContextStack( &ctxt.stack );
}

drmem_hdl DRGetCompileUnitTag( drmem_hdl comp_unit )
/**************************************************/
{ // given the start of the compilation unit header
  // return the start of the DW_TAG_compile_unit
  // assume it starts after header

    comp_unit += sizeof( compuhdr_prologue );
    return( comp_unit );
}

#define DEMANGLE_BUF_SIZE 256

char * DWRGetName( drmem_hdl abbrev, drmem_hdl entry )
/****************************************************/
{
    char    *name;
    char    buffer[DEMANGLE_BUF_SIZE];
    size_t  len;
    size_t  base_len;

    name = NULL;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_name ) ) {
        name = DWRReadString( abbrev, entry );
        len = strlen( name );
        if( __is_mangled( name, len ) ) {
            base_len = __demangled_basename( name, len, buffer, DEMANGLE_BUF_SIZE );
            DWRREALLOC( name, base_len + 1 );
            strncpy( name, buffer, base_len + 1 );
        }
    }
    return( name );
}

void DRIterateCompileUnits( void *data, DRITERCUCB callback )
/***********************************************************/
{
    compunit_info   *compunit;

    compunit = &DWRCurrNode->compunit;
    do {
        if( !callback( data, compunit->start ) ) {
            break;    // false == quit
        }
        compunit = compunit->next;
    } while( compunit != NULL );
}

bool DWRScanAllCompileUnits( dr_search_context * startingCtxt, DWRCUWLK fn,
                        const dw_tagnum *tagarray, dr_depth depth, void *data )
/*******************************************************************************/
{
    bool                cont;
    dr_search_context   ctxt;
    int                 i;

    if( startingCtxt == NULL ) {
        ctxt.compunit = &DWRCurrNode->compunit;
        ctxt.start = ((compunit_info *)ctxt.compunit)->start;
        ctxt.end = ctxt.start + DWRVMReadDWord( ctxt.start );
        ctxt.start += sizeof( compuhdr_prologue );
        ctxt.classhdl = DR_HANDLE_NUL;
        ctxt.functionhdl = DR_HANDLE_NUL;
        ctxt.stack.size = 0;
        ctxt.stack.free = 0;
        ctxt.stack.stack = NULL;
    } else {
        ctxt = *startingCtxt;   /* structure copy */

        /* but allocate and copy own stack */
        ctxt.stack.stack = DWRALLOC( ctxt.stack.size * sizeof( uint_32 ) );
        for( i = 0; i < ctxt.stack.free; i += 1 ) {
            ctxt.stack.stack[i] = startingCtxt->stack.stack[i];
        }
    }

    do {
        cont = DWRScanCompileUnit( &ctxt, fn, tagarray, depth, data );

        ctxt.compunit = ((compunit_info *) ctxt.compunit)->next;
        if( ctxt.compunit ) {
            ctxt.start = ((compunit_info *) ctxt.compunit)->start;
            ctxt.end = ctxt.start + DWRVMReadDWord( ctxt.start );
            ctxt.start += sizeof( compuhdr_prologue );
        }
    } while( cont && ctxt.compunit != NULL );

    DWRFreeContextStack( &ctxt.stack );

    return( cont );     /* false if more symbols, true if at end of info */
}

bool DWRWalkCompileUnit( drmem_hdl mod, DWRCUWLK fn,
                        const dw_tagnum *tagarray, dr_depth depth, void *data )
/*******************************************************************************/
{
    bool                cont;
    dr_search_context   ctxt;
    compunit_info       *compunit;

    compunit = DWRFindCompileInfo( mod );
    ctxt.compunit = compunit;
    ctxt.start = mod;
    ctxt.end = compunit->start + DWRVMReadDWord( compunit->start );
    ctxt.classhdl = DR_HANDLE_NUL;
    ctxt.functionhdl = DR_HANDLE_NUL;
    ctxt.stack.size = 0;
    ctxt.stack.free = 0;
    ctxt.stack.stack = NULL;

    cont = DWRScanCompileUnit( &ctxt, fn, tagarray, depth, data );


    DWRFreeContextStack( &ctxt.stack );

    return( cont );     /* false if more symbols, true if at end of info */
}

bool DWRWalkChildren( drmem_hdl mod, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d )
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
    compunit_info   *cu;

    cu = DWRFindCompileInfo( mod );
    abbrev_idx = DWRVMReadULEB128( &mod );
    if( abbrev_idx == 0 ) {
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }
    abbrev = cu->abbrevs[abbrev_idx];
    tag = DWRVMReadULEB128( &abbrev );
    haschild = DWRVMReadByte( abbrev );

    abbrev += sizeof( unsigned_8 );         /* skip child byte */
    DWRSkipAttribs( abbrev, &mod );
    if( haschild == DW_CHILDREN_yes ) {
        curr = mod;
        for( ;; ) {
            mod = curr;
            abbrev_idx = DWRVMReadULEB128( &curr );
            if( abbrev_idx == 0 )
                break;
            abbrev = cu->abbrevs[abbrev_idx];
            tag = DWRVMReadULEB128( &abbrev );
            haschild = DWRVMReadByte( abbrev );
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
                DWRSkipChildren( &abbrev, &curr );
            } else {
                DWRSkipAttribs( abbrev, &curr );
            }
        }
    }
    return( true );
}

bool DWRWalkContaining( drmem_hdl mod, drmem_hdl target, DRWLKBLK wlk, void *d )
/******************************************************************************/
// Walk into tags that enclose target
// The final call shoud be target else a goose chase
{
    drmem_hdl       abbrev;
    drmem_hdl       curr;
    dw_children     haschild;

    if( DWRReadTagEnd( &mod, &abbrev, NULL ) )  /* skip tag */
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    haschild = DWRVMReadByte( abbrev );
    abbrev++;
    DWRSkipAttribs( abbrev, &mod );
    if( haschild == DW_CHILDREN_yes ) {
        curr = mod;
        for( ;; ) {
            drmem_hdl   old_abbrev;
            drmem_hdl   old_curr;

            if( curr > target )
                break;
            mod = curr;
            if( DWRReadTagEnd( &curr, &abbrev, NULL ) ) /* skip tag */
                break;
            haschild = DWRVMReadByte( abbrev );
            abbrev++;
            old_abbrev = abbrev;
            old_curr = curr;
            if( haschild == DW_CHILDREN_yes ) {
                DWRSkipChildren( &abbrev, &curr );
            } else {
                DWRSkipAttribs( abbrev, &curr );
            }
            if( curr > target ) {           // mod < target < curr - encloses
                if( !wlk( mod, 0, d ) ){
                    return( false );        // false == quit
                }
                abbrev = old_abbrev;        // rest cause we are going in
                curr = old_curr;
                DWRSkipAttribs( abbrev, &curr );    // skip current tags stuff
            }
        }
    }
    return( true );
}

bool DWRWalkSiblings( drmem_hdl curr, const dw_tagnum *tags, const DRWLKBLK *wlks, void *d )
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
    while( !DWRReadTagEnd( &curr, &abbrev, &tag ) ) {
        haschild = DWRVMReadByte( abbrev );
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
            DWRSkipChildren( &abbrev, &curr );
        } else {
            DWRSkipAttribs( abbrev, &curr );
        }
        start = curr;
    }
    return( cont );
}

bool DWRWalkScope( drmem_hdl mod, const dw_tagnum *tags, DRWLKBLK wlk, void *d )
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
    if( DWRReadTagEnd( &mod, &abbrev, &tag ) ) {
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }
    haschild = DWRVMReadByte( abbrev );
    abbrev++;
    DWRSkipAttribs( abbrev, &mod );
    if( haschild == DW_CHILDREN_yes ) {
        int         depth;
//        int         skip;

        depth = 1;
//        skip  = true;
        curr = mod;
        for( ;; ) {
            mod = curr;
            if( DWRReadTagEnd( &curr, &abbrev, &tag ) ) {
                if( --depth == 0 ) {
                    break;
                }
            } else {
                haschild = DWRVMReadByte( abbrev );
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
                     DWRSkipAttribs( abbrev, &curr );
                } else {
                    if( haschild == DW_CHILDREN_yes ) { // skip fuzz
                        DWRSkipChildren( &abbrev, &curr );
                    } else {
                        DWRSkipAttribs( abbrev, &curr );
                    }
                }
            }
        }
    }
    return( true );
}

static compunit_info *FindCompileInfo( compunit_info *compunit, drmem_hdl addr )
/******************************************************************************/
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
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }
    return( compunit );
}

compunit_info * DWRFindCompileInfo( drmem_hdl addr )
/**************************************************/
/* gets the drmem_hdl of the module that addr is in */
{
    compunit_info   *compunit;

    compunit = DWRCurrNode->last_ccu;
    if( addr < compunit->start ) {  // start at begining
        compunit = &DWRCurrNode->compunit;
    }
    compunit = FindCompileInfo( compunit, addr );
    DWRCurrNode->last_ccu = compunit;
    return( compunit );
}

drmem_hdl DWRFindCompileUnit( drmem_hdl addr )
/********************************************/
{
    compunit_info   *compunit;

    compunit = DWRFindCompileInfo( addr );
    return( compunit->start );
}

#define CONTEXT_GUESS 0x10

void DWRContextPush( dr_context_stack *stack, drmem_hdl val )
/***********************************************************/
{
    if( stack->stack == NULL ) {
        stack->stack = DWRALLOC( CONTEXT_GUESS * sizeof( uint_32 ) );
        stack->free = 0;
        stack->size = CONTEXT_GUESS;
    }
    if( stack->free >= stack->size ) {
        stack->size += CONTEXT_GUESS;
        stack->stack = DWRREALLOC( stack->stack, stack->size * sizeof( drmem_hdl ) );
    }

    stack->stack[stack->free] = val;
    stack->free += 1;
}

drmem_hdl DWRContextPop( dr_context_stack *stack )
/************************************************/
{
    if( stack->free <= 0 ) {
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }

    stack->free -= 1;
    return( stack->stack[stack->free] );
}

drmem_hdl DWRContext( dr_context_stack *stack, int up )
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

void DWRFreeContextStack( dr_context_stack *stack )
/********************************************************/
{
    DWRFREE( stack->stack );
    stack->free = 0;
    stack->size = 0;
}
