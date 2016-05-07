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
* Description:  Various DWARF information helper functions.
*
****************************************************************************/


#include "drpriv.h"
#include "drutils.h"
#include "drgettab.h"
#include "drscope.h"


static dr_language GetLanguage( drmem_hdl abbrev, drmem_hdl mod )
/***************************************************************/
{
    dr_language result;
    dw_langnum  lang;

    result = DR_LANG_UNKNOWN;
    if( DWRScanForAttrib( &abbrev, &mod, DW_AT_language ) ) {
        lang = (dw_langnum)DWRReadConstant( abbrev, mod );
        switch( lang ) {
        case DW_LANG_C89:
        case DW_LANG_C:
            result = DR_LANG_C;
            break;
        case DW_LANG_C_plus_plus:
            result = DR_LANG_CPLUSPLUS;
            break;
        case DW_LANG_Fortran77:
        case DW_LANG_Fortran90:
            result = DR_LANG_FORTRAN;
            break;
        }
    }
    return( result );
}

static bool CheckLanguage( drmem_hdl abbrev, drmem_hdl mod, mod_scan_info *x, void *data )
/****************************************************************************************/
{
    x = x;
    *(dr_language *)data = GetLanguage( abbrev, mod );
    return( false );        // do not continue processing
}

dr_language DRGetLanguageAT( drmem_hdl entry )
/********************************************/
{
    drmem_hdl   abbrev;
    dr_language result;

    abbrev = DWRSkipTag( &entry ) + 1;
    result = GetLanguage( abbrev, entry );
    return( result );
}

dr_model DRGetMemModelAT( drmem_hdl entry )
/*****************************************/
{
    drmem_hdl   abbrev;
    dr_model    retval;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_WATCOM_memory_model ) ) {
        retval = (dr_model)DWRReadConstant( abbrev, entry );
    } else {
        retval = DR_MODEL_NONE;
    }
    return( retval );
}

char *DRGetProducer( drmem_hdl entry )
/************************************/
{
    drmem_hdl   abbrev;
    char       *name;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_producer ) ) {
        name = DWRReadString( abbrev, entry );
    } else {
        name = NULL;
    }
    return( name );
}

dr_language DRGetLanguage( void )
/*******************************/
{
    dr_language result;
    drmem_hdl   start;

    start = DWRCurrNode->sections[DR_DEBUG_INFO].base;
    DWRGetCompileUnitHdr( start, CheckLanguage, &result );
    return( result );
}

static size_t GetNameBuffAttr( drmem_hdl entry, char *buff, size_t length, dw_atnum attrib )
/******************************************************************************************/
{
    drmem_hdl   abbrev;
    dw_formnum  form;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, attrib ) ) {
        form = DWRVMReadULEB128( &abbrev );
        switch( form ) {
        case DW_FORM_string:
            length = DWRVMGetStrBuff( entry, buff, length );
            break;
        case DW_FORM_strp:
            {
                unsigned_32 offset;
                drmem_hdl   dbgsec_str;
    
                offset = ReadConst( DW_FORM_data4, entry );
                dbgsec_str = DWRCurrNode->sections[DR_DEBUG_STR].base + offset;
                length = DWRVMGetStrBuff( dbgsec_str, buff, length );
            }
            break;
        default:
            DWREXCEPT( DREXCEP_BAD_DBG_INFO );
            length = 0;
        }
    } else {
        length = 0;
    }
    return( length );
}

size_t DRGetCompDirBuff( drmem_hdl entry, char *buff, size_t length )
/*******************************************************************/
{
    return( GetNameBuffAttr( entry, buff, length, DW_AT_comp_dir ) );
}

char *DRGetName( drmem_hdl entry )
/********************************/
{
    drmem_hdl   abbrev;

    abbrev = DWRSkipTag( &entry ) + 1;
    return( DWRGetName( abbrev, entry ) );
}

size_t DRGetNameBuff( drmem_hdl entry, char *buff, size_t length )
/****************************************************************/
{
    return( GetNameBuffAttr( entry, buff, length, DW_AT_name ) );
}

size_t DRGetScopedNameBuff( drmem_hdl entry, char *buff, size_t max )
/*******************************************************************/
{
    drmem_hdl       of;
    dr_scope_trail  container;
    dr_scope_entry  *curr;
    size_t          total;
    size_t          length;

    of = DRGetContaining( entry );
    if( of == DR_HANDLE_NUL ) {
        of = entry;
    }
    DRGetScopeList( &container, of );
    curr = container.head;
    if( curr != NULL  ) {
        dr_scope_entry  *outside;

        if( of == entry ) {         //bump from list
            curr = curr->next;
        }
        outside = NULL;
        while( curr != NULL ) {     //reverse list to outer scope first
            dr_scope_entry  *next;
            dw_tagnum       tag;

            next = curr->next;
            tag = DWRGetTag( curr->handle );
            switch( tag ){
            case DW_TAG_class_type:
            case DW_TAG_union_type:
            case DW_TAG_structure_type:
            case DW_TAG_WATCOM_namespace:
                curr->next = outside;
                outside = curr;
                break;
            default:
                goto done_loop;
            }
            curr = next;
        }
done_loop:
        curr = outside;
    }
    total = 0;
    while( curr != NULL ) {

        length = DRGetNameBuff(  curr->handle, buff, max );
        if( length > 0 ) {  //nullchar
            --length;
        }
        total += length;
        if( length + 2 < max ) {
            buff  += length;
            max   -= length;
            buff[0] = ':';
            buff[1] = ':';
            buff += 2;
        } else {
            max = 0;
        }
        total += 2;
        curr = curr->next;
    }
    length = DRGetNameBuff( entry, buff, max );
    total += length;
    DREndScopeList( &container );
    return( total );
}

long DRGetColumn( drmem_hdl entry )
/*********************************/
// NYI: this is not going to work for macros.
{
    long        retval;
    drmem_hdl   abbrev;

    retval = -1;        // signifies no column available
    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_decl_column ) ) {
        retval = DWRReadConstant( abbrev, entry );
    }
    return( retval );
}

long DRGetLine( drmem_hdl entry )
/**************************************/
// NYI: this is not going to work for macros.
{
    long        retval;
    drmem_hdl   abbrev;

    retval = -1;        // signifies no column available
    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_decl_line ) ) {
        retval = DWRReadConstant( abbrev, entry );
    }
    return( retval );
}

extern char *DRGetFileName( drmem_hdl entry )
/*******************************************/
{
    drmem_hdl           abbrev;
    char *              name;
    dr_fileidx          fileidx;

    name = NULL;
    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_decl_file ) ) {
        fileidx = (dr_fileidx)DWRReadConstant( abbrev, entry );
        name = DWRFindFileName( fileidx, entry );
    }
    return( name );
}

void DRGetFileNameList( DRFNAMECB callback, void *data )
/******************************************************/
{
    compunit_info       *compunit;
    dr_fileidx          fileidx;
    filetab_idx         ftidx;
    char                *name;

    compunit = &DWRCurrNode->compunit;
    do {
        fileidx = compunit->filetab.len;
        while( fileidx > 0 ) {
            fileidx--;
            ftidx = DWRIndexFile( fileidx, &compunit->filetab );
            name = DWRIndexFileName( ftidx, &FileNameTable.fnametab );
            if( !callback( name, data ) ) {
                return;
            }
        }
        compunit = compunit->next;
    } while( compunit != NULL );
}

char *DRIndexFileName( drmem_hdl mod, dr_fileidx fileidx  )
/********************************************************/
{
    compunit_info       *compunit;
    char                *name;
    filetab_idx         ftidx;

    compunit = DWRFindCompileInfo( mod );
    ftidx = DWRIndexFile( fileidx - 1, &compunit->filetab );
    name = DWRIndexFileName( ftidx, &FileNameTable.fnametab );
    return( name );
}

dr_access DRGetAccess( drmem_hdl entry )
/**************************************/
{
    drmem_hdl   abbrev;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_accessibility ) ) {
        return( (dr_access)DWRReadConstant( abbrev, entry ) );
    }
    return( DR_ACCESS_PUBLIC );
}

bool DRIsStatic( drmem_hdl entry )
/********************************/
{
    drmem_hdl   abbrev;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_external ) ) {
        return( DWRReadFlag( abbrev, entry ) == 0 );
    }
    return( false );
}

bool DRIsArtificial( drmem_hdl entry )
/************************************/
{
    drmem_hdl   abbrev;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_artificial ) ) {
        return( DWRReadFlag( abbrev, entry ) != 0 );
    }
    return( false );
}

bool DRIsSymDefined( drmem_hdl entry )
/************************************/
{
    drmem_hdl   abbrev;

    abbrev = DWRSkipTag( &entry ) + 1;
    return( !DWRScanForAttrib( &abbrev, &entry, DW_AT_declaration ) );
}

bool DRIsMemberStatic( drmem_hdl entry )
/**************************************/
{
    dw_tagnum       tag;

    tag = DWRGetTag( entry );
    return( tag == DW_TAG_variable );
}

bool DRIsFunc( drmem_hdl entry )
/******************************/
{
    dw_tagnum       tag;

    tag = DWRGetTag( entry );
    return( tag == DW_TAG_subprogram );
}

bool DRIsParm( drmem_hdl entry )
/******************************/
{
    dw_tagnum       tag;

    tag = DWRGetTag( entry );
    return( tag == DW_TAG_formal_parameter );
}

dr_virtuality DRGetVirtuality( drmem_hdl entry )
/**********************************************/
{
    drmem_hdl   abbrev;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_virtuality ) ) {
        return( (dr_virtuality)DWRReadConstant( abbrev, entry ) );
    }
    return( DR_VIRTUALITY_NONE );
}

unsigned DRGetByteSize( drmem_hdl entry )
/***************************************/
{
    drmem_hdl   abbrev;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_byte_size ) ) {
        return( DWRReadConstant( abbrev, entry ) );
    }
    return( 0 );
}

bool DRGetLowPc( drmem_hdl entry, uint_32 *num )
/**********************************************/
{
    drmem_hdl   abbrev;
    uint_32     offset;
    bool        ret;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_low_pc ) ) {
        offset = DWRReadAddr( abbrev, entry );
        *num = offset;
        ret = true;
    } else {
        ret = false;
    }
    return( ret );
}

bool DRGetHighPc( drmem_hdl entry, uint_32 *num )
/***********************************************/
{
    drmem_hdl   abbrev;
    uint_32     offset;
    bool        ret;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_high_pc ) ) {
        offset = DWRReadAddr( abbrev, entry );
        *num = offset;
        ret = true;
    } else {
        ret = false;
    }
    return( ret );
}

drmem_hdl DRGetContaining( drmem_hdl entry )
/******************************************/
{
    drmem_hdl   abbrev;
    drmem_hdl   ret;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_containing_type ) ) {
        ret = DWRReadReference( abbrev, entry );
    } else {
        ret = DR_HANDLE_NUL;
    }
    return( ret );
}

drmem_hdl DRWalkParent( dr_search_context * context )
/**********************************************************/
// get past relative
{
    drmem_hdl   prev;
    stack_op    op;

    op = DWRContextOP( &context->stack, 0 );
    switch( op ) {
    case DO_NOTHING:
        prev = DWRContext( &context->stack, 1 );
        break;
    case SET_CLASS:
        prev = context->classhdl;
        break;
    case SET_FUNCTION:
        prev = context->functionhdl;
        break;
    default:
        prev = DR_HANDLE_NUL;
        break;
    }
    return( prev );
}

struct wlk_wlk {
    DRWLKMODF   wlk;
    void        *d;
};

static bool CheckAFunc( drmem_hdl abbrev, drmem_hdl mod, mod_scan_info *x, void *_d )
/***********************************************************************************/
{
    struct  wlk_wlk     *d = _d;

    abbrev = abbrev;

    mod = x->handle;
    return( d->wlk( mod, d->d, x->context ) );
}

static const dw_tagnum BlockTags[] = {
    DW_TAG_subprogram, DW_TAG_lexical_block, 0
};

static const dw_tagnum EntryTags[] = {
    DW_TAG_subprogram, DW_TAG_label, DW_TAG_variable, 0
};

bool DRWalkModFunc( drmem_hdl mod, bool blocks, DRWLKMODF wlk, void *d )
/**********************************************************************/
{
    bool            ret;
    struct wlk_wlk  dat;

    dat.wlk = wlk;
    dat.d = d;
    if( blocks ) {
        ret = DWRWalkCompileUnit( mod, CheckAFunc, BlockTags, DR_DEPTH_FUNCTIONS, &dat );
    } else {
        ret = DWRWalkCompileUnit( mod, CheckAFunc, EntryTags, 0, &dat );
    }
    return( ret );
}

static const dw_tagnum TypeTags[] = { // any type
    DW_TAG_array_type,
    DW_TAG_enumeration_type,
    DW_TAG_pointer_type,
    DW_TAG_string_type,
    DW_TAG_structure_type,
    DW_TAG_union_type,
    DW_TAG_class_type,
    DW_TAG_subprogram,
    DW_TAG_subroutine_type,
    DW_TAG_reference_type,
    DW_TAG_ptr_to_member_type,
    DW_TAG_set_type,
    DW_TAG_subrange_type,
    DW_TAG_base_type,
    DW_TAG_file_type,
    DW_TAG_thrown_type,
    DW_TAG_const_type,
    DW_TAG_volatile_type,
    DW_TAG_packed_type,
    DW_TAG_typedef,
};

bool DRWalkModTypes( drmem_hdl mod, DRWLKMODF wlk, void *d )
/**********************************************************/
{
    struct wlk_wlk  dat;

    dat.wlk = wlk;
    dat.d = d;
    return( DWRWalkCompileUnit( mod, CheckAFunc, TypeTags, DR_DEPTH_FUNCTIONS | DR_DEPTH_CLASSES, &dat ) );
}

bool DRWalkScope( drmem_hdl mod, DRWLKBLK wlk, void *d )
/******************************************************/
{
    return( DWRWalkScope( mod, &BlockTags[1], wlk, d ) );
}

static const dw_tagnum CodeDataTags[] = {
    DW_TAG_subprogram, DW_TAG_variable, DW_TAG_label,
    DW_TAG_WATCOM_namespace, DW_TAG_formal_parameter, 0
};

static const dw_tagnum ParmTags[] = {
    DW_TAG_formal_parameter, 0
};

static const dw_tagnum CTypeTags[] = {    // visible c type names
    DW_TAG_typedef, 0
};

static const dw_tagnum CPPTypeTags[] = {  // visible c++ type names
    DW_TAG_base_type, DW_TAG_typedef,
    DW_TAG_enumeration_type, DW_TAG_class_type,
    DW_TAG_union_type, DW_TAG_structure_type, 0
};

static const dw_tagnum * const SrchTags[DR_SRCH_LAST] = {
    CodeDataTags,
    FunctionTags,
    ClassTags,
    EnumTags,
    TypedefTags,
    CTypeTags,
    CPPTypeTags,
    VariableTags,
    ParmTags,
    LabelTags,
};

#define MAX_TAG_WLK   8   //check max size of tag search array
bool DRWalkBlock( drmem_hdl mod, dr_srch what, DRWLKBLK wlk, void *d )
/********************************************************************/
{
    const dw_tagnum     *tags;
    DRWLKBLK            wlks[MAX_TAG_WLK];
    int                 index;

    tags = SrchTags[what];
    index = 0;
    while( tags[index] != 0 ) {
        wlks[index] = wlk;
        ++index;
    }
    wlks[index] = NULL;
    return( DWRWalkChildren( mod, tags, wlks, d ) );
}

bool DRStartScopeAT( drmem_hdl entry, uint_32 *num )
/**************************************************/
{
    drmem_hdl   abbrev;
    uint_32     offset;
    bool        ret;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_start_scope ) ) {
        offset =  DWRReadConstant( abbrev, entry );
        *num = offset;
        ret = true;
    } else {
        ret = false;
    }
    return( ret );
}

unsigned DRGetAddrSize( drmem_hdl mod )
/*************************************/
// returns the size of the address for the compile unit
{
    return( DWRVMReadByte( mod + 10 ) );
}

drmem_hdl DRDebugPCHDef( drmem_hdl entry )
/****************************************/
{
    drmem_hdl   abbrev;
    drmem_hdl   ret;

    abbrev = DWRSkipTag( &entry ) + 1;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_base_types ) ) {
        ret = DWRReadReference( abbrev, entry );
    } else {
        ret = DR_HANDLE_NUL;
    }
    return( ret );
}

 dr_tag_type DRGetTagType( drmem_hdl entry )
/************************************************/
{
    dr_tag_type tagtype;
    dw_tagnum   tag;

    tag = DWRGetTag( entry );
    switch( tag ) {
    case DW_TAG_subprogram:
        tagtype =  DR_TAG_FUNCTION;
        break;
    case DW_TAG_class_type:
    case DW_TAG_union_type:
    case DW_TAG_structure_type:
        tagtype =  DR_TAG_CLASS;
        break;
    case DW_TAG_enumeration_type:
        tagtype =  DR_TAG_ENUM;
        break;
    case DW_TAG_typedef:
        tagtype =  DR_TAG_TYPEDEF;
        break;
    case DW_TAG_common_block:
    case DW_TAG_variable:
    case DW_TAG_formal_parameter:
    case DW_TAG_member:
        tagtype =  DR_TAG_VARIABLE;
        break;
    case  DW_TAG_label:
        tagtype =  DR_TAG_LABEL;
        break;
    case  DW_TAG_WATCOM_namespace:
        tagtype =  DR_TAG_NAMESPACE;
        break;
    default:
        tagtype =  DR_TAG_NONE;
        break;
    }
    return( tagtype );
}
