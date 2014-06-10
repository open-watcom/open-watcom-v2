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

static dw_tagnum GetTag( dr_handle entry )
{
    dr_handle   abbrev;
    dw_tagnum   tag;

    abbrev = DWRVMReadULEB128( &entry );
    abbrev = DWRLookupAbbrev( entry, abbrev );
    tag = DWRVMReadULEB128( &abbrev );
    return( tag );
}


static dr_language GetLanguage( dr_handle abbrev, dr_handle mod )
/***************************************************************/
{
    dr_language result;
    unsigned    lang;

    result = DR_LANG_UNKNOWN;
    if( DWRScanForAttrib( &abbrev, &mod, DW_AT_language ) ) {
        lang = DWRReadConstant( abbrev, mod );
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

static bool CheckLanguage( dr_handle abbrev, dr_handle mod, mod_scan_info *x,
                           void *_data )
/***************************************************************************/
{
    dr_language *data = _data;

    x = x;
    *data = GetLanguage( abbrev, mod );
    return( FALSE );        // do not continue processing
}

dr_language DRGetLanguageAT( dr_handle entry )
/********************************************/
{
    dr_handle   abbrev;
    dr_language result;

    abbrev = DWRGetAbbrev( &entry );

    result = GetLanguage( abbrev, entry );
    return( result );
}

dr_model DRGetMemModelAT( dr_handle entry )
/*****************************************/
{
    dr_handle   abbrev;
    dr_model    retval;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_WATCOM_memory_model ) ) {
        retval = DWRReadConstant( abbrev, entry );
    } else {
        retval = DR_MODEL_NONE;
    }
    return( retval );
}

char *DRGetProducer( dr_handle entry )
/************************************/
{
    dr_handle   abbrev;
    char       *name;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_producer ) ) {
        name = DWRReadString( abbrev, entry );
    }else{
        name = NULL;
    }
    return( name );
}

dr_language DRGetLanguage( void )
/*******************************/
{
    dr_language result;
    dr_handle   start;

    start = DWRCurrNode->sections[DR_DEBUG_INFO].base;
    DWRGetCompileUnitHdr( start, CheckLanguage, &result );
    return( result );
}

static unsigned GetNameBuffAttr( dr_handle entry, char *buff, unsigned length, dw_atnum attrib )
/**********************************************************************************************/
{
    dr_handle   abbrev;
    unsigned    form;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, attrib ) ) {
        form = DWRVMReadULEB128( &abbrev );
        switch( form ) {
        case DW_FORM_string:
            length = DWRGetStrBuff( entry, buff, length );
            break;
        case DW_FORM_strp: {
            unsigned_32 offset;
            dr_handle   dbgsec_str;

            offset = ReadConst( DW_FORM_data4, entry );
            dbgsec_str = DWRCurrNode->sections[DR_DEBUG_STR].base + offset;
            length = DWRGetStrBuff( dbgsec_str, buff, length );
            break;
            }
        default:
            DWREXCEPT( DREXCEP_BAD_DBG_INFO );
            length = 0;
        }
    } else {
        length = 0;
    }
    return( length );
}

unsigned DRGetCompDirBuff( dr_handle entry, char *buff, unsigned length )
/***********************************************************************/
{
    return( GetNameBuffAttr( entry, buff, length, DW_AT_comp_dir ) );
}

char * DRGetName( dr_handle entry )
/*********************************/
{
    dr_handle   abbrev;

    abbrev = DWRGetAbbrev( &entry );
    return( DWRGetName( abbrev, entry ) );
}

unsigned DRGetNameBuff( dr_handle entry, char *buff, unsigned length )
/********************************************************************/
{
    return( GetNameBuffAttr( entry, buff, length, DW_AT_name ) );
}

unsigned DRGetScopedNameBuff( dr_handle entry,
                                     char     *buff,
                                     unsigned  max )
/***************************************************/
{
    dr_handle       of;
    scope_trail     container;
    scope_entry     *curr;
    unsigned        total;
    unsigned        length;

    of = DRGetContaining( entry );
    if( of == 0 ) {
        of = entry;
    }
    DRGetScopeList( &container, of );
    curr = container.head;
    if( curr != NULL  ) {
        scope_entry    *outside;

        if( of == entry ) {         //bump from list
            curr = curr->next;
        }
        outside = NULL;
        while( curr != NULL ) {     //reverse list to outer scope first
            scope_entry    *next;
            dw_tagnum     tag;

            next = curr->next;
            tag = GetTag( curr->handle );
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
        }done_loop:;
        curr = outside;
    }
    total = 0;
    while( curr != NULL ) {

        length = DRGetNameBuff(  curr->handle, buff, max );
        if( length > 0 ) {  //nullchar
            --length;
        }
        total += length;
        if( length+2 < max ) {
            buff  += length;
            max   -= length;
            buff[0] = ':';
            buff[1] = ':';
            buff+= 2;
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

long DRGetColumn( dr_handle entry )
/*********************************/
// NYI: this is not going to work for macros.
{
    unsigned long retval;
    dr_handle     abbrev;

    retval = -1;        // signifies no column available
    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_decl_column ) ) {
        retval = DWRReadConstant( abbrev, entry );
    }
    return( retval );
}

long DRGetLine( dr_handle entry )
/**************************************/
// NYI: this is not going to work for macros.
{
    unsigned long retval;
    dr_handle     abbrev;

    retval = -1;        // signifies no column available
    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_decl_line ) ) {
        retval = DWRReadConstant( abbrev, entry );
    }
    return( retval );
}

extern char *DRGetFileName( dr_handle entry )
/*******************************************/
{
    dr_handle           abbrev;
    char *              name;
    unsigned            fileno;

    name = NULL;
    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_decl_file ) ) {
        fileno = DWRReadConstant( abbrev, entry );
        name = DWRFindFileName( fileno, entry );
    }
    return( name );
}

void DRGetFileNameList( DRFNAMECB callback, void *data )
/******************************************************/
{
    compunit_info       *compunit;
    unsigned            num;
    unsigned            fileno;
    char                *name;

    compunit = &DWRCurrNode->compunit;
    do {
        num = compunit->filetab.len;
        while( num > 0 ) {
            num--;
            fileno = DWRIndexFile( num, &compunit->filetab );
            name = DWRIndexFileName( fileno, &FileNameTable.fnametab );
            if( !callback( name, data ) ) {
                return;
            }
        }
        compunit = compunit->next;
    } while( compunit != NULL );
}

char *DRIndexFileName( dr_handle mod, unsigned fileno  )
/******************************************************/
{
    compunit_info       *compunit;
    char                *name;

    compunit = DWRFindCompileInfo( mod );
    fileno = DWRIndexFile( fileno-1, &compunit->filetab );
    name = DWRIndexFileName( fileno, &FileNameTable.fnametab );
    return( name );
}

dr_access DRGetAccess( dr_handle entry )
/**************************************/
{
    dr_handle   abbrev;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_accessibility ) ) {
        return( DWRReadConstant( abbrev, entry ) );
    }
    return( DR_ACCESS_PUBLIC );
}

bool DRIsStatic( dr_handle entry )
/********************************/
{
    dr_handle   abbrev;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_external ) ) {
        return( DWRReadFlag( abbrev, entry ) == 0 );
    }
    return( FALSE );
}

bool DRIsArtificial( dr_handle entry )
/************************************/
{
    dr_handle   abbrev;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_artificial ) ) {
        return( DWRReadFlag( abbrev, entry ) != 0 );
    }
    return( FALSE );
}

bool DRIsSymDefined( dr_handle entry )
/************************************/
{
    dr_handle   abbrev;

    abbrev = DWRGetAbbrev( &entry );
    return( !DWRScanForAttrib( &abbrev, &entry, DW_AT_declaration ) );
}

bool DRIsMemberStatic( dr_handle entry )
/**************************************/
{
    dr_handle   abbrev;
    dw_tagnum   tag;

    abbrev = DWRVMReadULEB128( &entry );
    abbrev = DWRLookupAbbrev( entry, abbrev );
    tag = DWRVMReadULEB128( &abbrev );
    return( tag == DW_TAG_variable );
}

bool DRIsFunc( dr_handle entry )
/******************************/
{
    dr_handle   abbrev;
    dw_tagnum   tag;

    abbrev = DWRVMReadULEB128( &entry );
    abbrev = DWRLookupAbbrev( entry, abbrev );
    tag = DWRVMReadULEB128( &abbrev );
    return( tag == DW_TAG_subprogram );
}

bool DRIsParm( dr_handle entry )
/******************************/
{
    dr_handle   abbrev;
    dw_tagnum   tag;

    abbrev = DWRVMReadULEB128( &entry );
    abbrev = DWRLookupAbbrev( entry, abbrev );
    tag = DWRVMReadULEB128( &abbrev );
    return( tag == DW_TAG_formal_parameter );
}

dr_virtuality DRGetVirtuality( dr_handle entry )
/**********************************************/
{
    dr_handle   abbrev;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_virtuality ) ) {
        return( DWRReadConstant( abbrev, entry ) );
    }
    return( DR_VIRTUALITY_NONE );
}

unsigned DRGetByteSize( dr_handle entry )
/***************************************/
{
    dr_handle   abbrev;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_byte_size ) ) {
        return( DWRReadConstant( abbrev, entry ) );
    }
    return( 0 );
}

bool DRGetLowPc( dr_handle entry, uint_32 *num )
/**********************************************/
{
    dr_handle   abbrev;
    uint_32     offset;
    bool        ret;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_low_pc ) ) {
        offset = DWRReadAddr( abbrev, entry );
        *num = offset;
        ret = TRUE;
    } else {
        ret = FALSE;
    }
    return( ret );
}

bool DRGetHighPc( dr_handle entry, uint_32 *num )
/***********************************************/
{
    dr_handle   abbrev;
    uint_32     offset;
    bool        ret;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_high_pc ) ) {
        offset = DWRReadAddr( abbrev, entry );
        *num = offset;
        ret = TRUE;
    } else {
        ret = FALSE;
    }
    return( ret );
}

dr_handle DRGetContaining( dr_handle entry )
/******************************************/
{
    dr_handle   abbrev;
    dr_handle   ret;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_containing_type ) ) {
        ret = DWRReadReference( abbrev, entry );
    } else {
        ret = 0;
    }
    return( ret );
}

dr_handle DRWalkParent( dr_search_context * context )
/**********************************************************/
// get past relative
{
    dr_handle   prev;
    stack_op      op;

    op = DWRContext( &context->stack, 0 );
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
        prev = 0;
        break;
    }
    return( prev );
}

struct wlk_wlk {
    DRWLKMODF   wlk;
    void        *d;
};

static bool CheckAFunc( dr_handle abbrev, dr_handle mod, mod_scan_info *x, void *_d )
/***********************************************************************************/
{
    struct  wlk_wlk     *d = _d;

    abbrev = abbrev;

    mod = x->handle;
    return( d->wlk( mod, d->d, x->context ) );
}

static unsigned_16 const BlockTags[] = {
    DW_TAG_subprogram, DW_TAG_lexical_block, 0
};

static unsigned_16 const EntryTags[] = {
    DW_TAG_subprogram, DW_TAG_label, DW_TAG_variable, 0
};

bool DRWalkModFunc( dr_handle mod, bool blocks, DRWLKMODF wlk, void *d )
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

static unsigned_16 const TypeTags[] = { // any type
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

bool DRWalkModTypes( dr_handle mod, DRWLKMODF wlk, void *d )
/**********************************************************/
{
    struct wlk_wlk  dat;

    dat.wlk = wlk;
    dat.d = d;
    return( DWRWalkCompileUnit( mod, CheckAFunc, TypeTags, DR_DEPTH_FUNCTIONS | DR_DEPTH_CLASSES, &dat ) );
}

bool DRWalkScope( dr_handle mod, DRWLKBLK wlk, void *d )
/******************************************************/
{
    return( DWRWalkScope( mod, &BlockTags[1], wlk, d ) );
}

static unsigned_16 const CodeDataTags[] = {
    DW_TAG_subprogram, DW_TAG_variable, DW_TAG_label,
    DW_TAG_WATCOM_namespace, DW_TAG_formal_parameter, 0
};

static unsigned_16 const ParmTags[] = {
    DW_TAG_formal_parameter, 0
};

static unsigned_16 const CTypeTags[] = {    // visible c type names
    DW_TAG_typedef, 0
};

static unsigned_16 const CPPTypeTags[] = {  // visible c++ type names
    DW_TAG_base_type, DW_TAG_typedef,
    DW_TAG_enumeration_type, DW_TAG_class_type,
    DW_TAG_union_type, DW_TAG_structure_type, 0
};

static unsigned_16 const * const SrchTags[DR_SRCH_LAST] = {
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
bool DRWalkBlock( dr_handle mod, dr_srch what, DRWLKBLK wlk, void *d )
/********************************************************************/
{
    unsigned_16 const   *tags;
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

bool DRStartScopeAT( dr_handle entry, uint_32 *num )
/**************************************************/
{
    dr_handle   abbrev;
    uint_32     offset;
    bool        ret;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_start_scope ) ) {
        offset =  DWRReadConstant( abbrev, entry );
        *num = offset;
        ret = TRUE;
    } else {
        ret = FALSE;
    }
    return( ret );
}

unsigned DRGetAddrSize( dr_handle mod )
/*************************************/
// returns the size of the address for the compile unit
{
    return( DWRVMReadByte( mod + 10 ) );
}

dr_handle DRDebugPCHDef( dr_handle entry )
/****************************************/
{
    dr_handle   abbrev;
    dr_handle   ret;

    abbrev = DWRGetAbbrev( &entry );
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_base_types ) ) {
        ret = DWRReadReference( abbrev, entry );
    } else {
        ret = 0;
    }
    return( ret );
}

 dr_tag_type DRGetTagType( dr_handle entry )
/************************************************/
{
    dr_tag_type tagtype;
    dw_tagnum   tag;

    tag = GetTag( entry );
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
