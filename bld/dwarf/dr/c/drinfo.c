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
    if( DR_ScanForAttrib( &abbrev, &mod, DW_AT_language ) ) {
        lang = (dw_langnum)DR_ReadConstant( abbrev, mod );
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
    /* unused parameters */ (void)x;

    *(dr_language *)data = GetLanguage( abbrev, mod );
    return( false );        // do not continue processing
}

dr_language DRENTRY DRGetLanguageAT( drmem_hdl entry )
/****************************************************/
{
    drmem_hdl   abbrev;
    dr_language result;

    abbrev = DR_SkipTag( &entry ) + 1;
    result = GetLanguage( abbrev, entry );
    return( result );
}

dw_mem_model DRENTRY DRGetMemModelAT( drmem_hdl entry )
/*****************************************************/
{
    drmem_hdl       abbrev;
    dw_mem_model    retval;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_WATCOM_memory_model ) ) {
        retval = (dw_mem_model)DR_ReadConstant( abbrev, entry );
    } else {
        retval = DW_MEM_MODEL_none;
    }
    return( retval );
}

char * DRENTRY DRGetProducer( drmem_hdl entry )
/*********************************************/
{
    drmem_hdl   abbrev;
    char       *name;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_producer ) ) {
        name = DR_ReadString( abbrev, entry );
    } else {
        name = NULL;
    }
    return( name );
}

dr_language DRENTRY DRGetLanguage( void )
/***************************************/
{
    dr_language result;
    drmem_hdl   start;

    start = DR_CurrNode->sections[DR_DEBUG_INFO].base;
    DR_GetCompileUnitHdr( start, CheckLanguage, &result );
    return( result );
}

static size_t GetNameBuffAttr( drmem_hdl entry, char *buff, size_t length, dw_atnum attrib )
/******************************************************************************************/
{
    drmem_hdl   abbrev;
    dw_formnum  form;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, attrib ) ) {
        form = DR_VMReadULEB128( &abbrev );
        switch( form ) {
        case DW_FORM_string:
            length = DR_VMGetStrBuff( entry, buff, length );
            break;
        case DW_FORM_strp:
            {
                unsigned_32 offset;
                drmem_hdl   dbgsec_str;

                offset = ReadConst( DW_FORM_data4, entry );
                dbgsec_str = DR_CurrNode->sections[DR_DEBUG_STR].base + offset;
                length = DR_VMGetStrBuff( dbgsec_str, buff, length );
            }
            break;
        default:
            DR_EXCEPT( DREXCEP_BAD_DBG_INFO );
            length = 0;
        }
    } else {
        length = 0;
    }
    return( length );
}

size_t DRENTRY DRGetCompDirBuff( drmem_hdl entry, char *buff, size_t length )
/***************************************************************************/
{
    return( GetNameBuffAttr( entry, buff, length, DW_AT_comp_dir ) );
}

char * DRENTRY DRGetName( drmem_hdl entry )
/*****************************************/
{
    drmem_hdl   abbrev;

    abbrev = DR_SkipTag( &entry ) + 1;
    return( DR_GetName( abbrev, entry ) );
}

size_t DRENTRY DRGetNameBuff( drmem_hdl entry, char *buff, size_t length )
/************************************************************************/
{
    return( GetNameBuffAttr( entry, buff, length, DW_AT_name ) );
}

size_t DRENTRY DRGetScopedNameBuff( drmem_hdl entry, char *buff, size_t max )
/***************************************************************************/
{
    drmem_hdl       of;
    dr_scope_trail  container;
    dr_scope_entry  *curr;
    size_t          total;
    size_t          length;

    of = DRGetContaining( entry );
    if( of == DRMEM_HDL_NULL ) {
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
            tag = DR_GetTag( curr->handle );
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

long DRENTRY DRGetColumn( drmem_hdl entry )
/******************************************
 * NYI: this is not going to work for macros.
 */
{
    long        retval;
    drmem_hdl   abbrev;

    retval = -1;        // signifies no column available
    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_decl_column ) ) {
        retval = DR_ReadConstant( abbrev, entry );
    }
    return( retval );
}

long DRENTRY DRGetLine( drmem_hdl entry )
/****************************************
 * NYI: this is not going to work for macros.
 */
{
    long        retval;
    drmem_hdl   abbrev;

    retval = -1;        // signifies no column available
    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_decl_line ) ) {
        retval = DR_ReadConstant( abbrev, entry );
    }
    return( retval );
}

char * DRENTRY DRGetFileName( drmem_hdl entry )
/*********************************************/
{
    drmem_hdl           abbrev;
    char *              name;
    dr_fileidx          fileidx;

    name = NULL;
    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_decl_file ) ) {
        fileidx = (dr_fileidx)DR_ReadConstant( abbrev, entry );
        name = DR_FindFileName( fileidx, entry );
    }
    return( name );
}

void DRENTRY DRGetFileNameList( DRFNAMECB callback, void *data )
/**************************************************************/
{
    dr_cu_handle        compunit;
    dr_fileidx          fileidx;
    filetab_idx         ftidx;
    char                *name;

    compunit = &DR_CurrNode->compunit;
    do {
        fileidx = compunit->filetab.len;
        while( fileidx > 0 ) {
            fileidx--;
            ftidx = DR_IndexFile( fileidx, &compunit->filetab );
            name = DR_IndexFileName( ftidx, &DR_FileNameTable.fnametab );
            if( !callback( name, data ) ) {
                return;
            }
        }
        compunit = compunit->next;
    } while( compunit != NULL );
}

char * DRENTRY DRIndexFileName( drmem_hdl mod, dr_fileidx fileidx  )
/******************************************************************/
{
    dr_cu_handle        compunit;
    char                *name;
    filetab_idx         ftidx;

    compunit = DR_FindCompileInfo( mod );
    ftidx = DR_IndexFile( fileidx - 1, &compunit->filetab );
    name = DR_IndexFileName( ftidx, &DR_FileNameTable.fnametab );
    return( name );
}

dr_access DRENTRY DRGetAccess( drmem_hdl entry )
/**********************************************/
{
    drmem_hdl   abbrev;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_accessibility ) ) {
        return( (dr_access)DR_ReadConstant( abbrev, entry ) );
    }
    return( DR_ACCESS_PUBLIC );
}

bool DRENTRY DRIsStatic( drmem_hdl entry )
/****************************************/
{
    drmem_hdl   abbrev;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_external ) ) {
        return( DR_ReadFlag( abbrev, entry ) == 0 );
    }
    return( false );
}

bool DRENTRY DRIsArtificial( drmem_hdl entry )
/********************************************/
{
    drmem_hdl   abbrev;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_artificial ) ) {
        return( DR_ReadFlag( abbrev, entry ) != 0 );
    }
    return( false );
}

bool DRENTRY DRIsSymDefined( drmem_hdl entry )
/********************************************/
{
    drmem_hdl   abbrev;

    abbrev = DR_SkipTag( &entry ) + 1;
    return( !DR_ScanForAttrib( &abbrev, &entry, DW_AT_declaration ) );
}

bool DRENTRY DRIsMemberStatic( drmem_hdl entry )
/**********************************************/
{
    dw_tagnum       tag;

    tag = DR_GetTag( entry );
    return( tag == DW_TAG_variable );
}

bool DRENTRY DRIsFunc( drmem_hdl entry )
/**************************************/
{
    dw_tagnum       tag;

    tag = DR_GetTag( entry );
    return( tag == DW_TAG_subprogram );
}

bool DRENTRY DRIsParm( drmem_hdl entry )
/**************************************/
{
    dw_tagnum       tag;

    tag = DR_GetTag( entry );
    return( tag == DW_TAG_formal_parameter );
}

dr_virtuality DRENTRY DRGetVirtuality( drmem_hdl entry )
/******************************************************/
{
    drmem_hdl   abbrev;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_virtuality ) ) {
        return( (dr_virtuality)DR_ReadConstant( abbrev, entry ) );
    }
    return( DR_VIRTUALITY_NONE );
}

unsigned DRENTRY DRGetByteSize( drmem_hdl entry )
/***********************************************/
{
    drmem_hdl   abbrev;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_byte_size ) ) {
        return( DR_ReadConstant( abbrev, entry ) );
    }
    return( 0 );
}

bool DRENTRY DRGetLowPc( drmem_hdl entry, uint_32 *num )
/******************************************************/
{
    drmem_hdl   abbrev;
    uint_32     offset;
    bool        ret;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_low_pc ) ) {
        offset = DR_ReadAddr( abbrev, entry );
        *num = offset;
        ret = true;
    } else {
        ret = false;
    }
    return( ret );
}

bool DRENTRY DRGetHighPc( drmem_hdl entry, uint_32 *num )
/*******************************************************/
{
    drmem_hdl   abbrev;
    uint_32     offset;
    bool        ret;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_high_pc ) ) {
        offset = DR_ReadAddr( abbrev, entry );
        *num = offset;
        ret = true;
    } else {
        ret = false;
    }
    return( ret );
}

drmem_hdl DRENTRY DRGetContaining( drmem_hdl entry )
/**************************************************/
{
    drmem_hdl   abbrev;
    drmem_hdl   ret;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_containing_type ) ) {
        ret = DR_ReadReference( abbrev, entry );
    } else {
        ret = DRMEM_HDL_NULL;
    }
    return( ret );
}

drmem_hdl DRENTRY DRWalkParent( dr_search_context *context )
/***********************************************************
 * get past relative
 */
{
    drmem_hdl   prev;
    stack_op    op;

    op = DR_ContextOP( &context->stack, 0 );
    switch( op ) {
    case DO_NOTHING:
        prev = DR_Context( &context->stack, 1 );
        break;
    case SET_CLASS:
        prev = context->classhdl;
        break;
    case SET_FUNCTION:
        prev = context->functionhdl;
        break;
    default:
        prev = DRMEM_HDL_NULL;
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

    /* unused parameters */ (void)abbrev;

    mod = x->handle;
    return( d->wlk( mod, d->d, x->context ) );
}

static const dw_tagnum BlockTags[] = {
    DW_TAG_subprogram, DW_TAG_lexical_block, 0
};

static const dw_tagnum EntryTags[] = {
    DW_TAG_subprogram, DW_TAG_label, DW_TAG_variable, 0
};

bool DRENTRY DRWalkModFunc( drmem_hdl mod, bool blocks, DRWLKMODF wlk, void *d )
/******************************************************************************/
{
    bool            ret;
    struct wlk_wlk  dat;

    dat.wlk = wlk;
    dat.d = d;
    if( blocks ) {
        ret = DR_WalkCompileUnit( mod, CheckAFunc, BlockTags, DR_DEPTH_FUNCTIONS, &dat );
    } else {
        ret = DR_WalkCompileUnit( mod, CheckAFunc, EntryTags, 0, &dat );
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

bool DRENTRY DRWalkModTypes( drmem_hdl mod, DRWLKMODF wlk, void *d )
/******************************************************************/
{
    struct wlk_wlk  dat;

    dat.wlk = wlk;
    dat.d = d;
    return( DR_WalkCompileUnit( mod, CheckAFunc, TypeTags, DR_DEPTH_FUNCTIONS | DR_DEPTH_CLASSES, &dat ) );
}

bool DRENTRY DRWalkScope( drmem_hdl mod, DRWLKBLK wlk, void *d )
/**************************************************************/
{
    return( DR_WalkScope( mod, &BlockTags[1], wlk, d ) );
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
bool DRENTRY DRWalkBlock( drmem_hdl mod, dr_srch what, DRWLKBLK wlk, void *d )
/****************************************************************************/
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
    return( DR_WalkChildren( mod, tags, wlks, d ) );
}

bool DRENTRY DRStartScopeAT( drmem_hdl entry, uint_32 *num )
/**********************************************************/
{
    drmem_hdl   abbrev;
    uint_32     offset;
    bool        ret;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_start_scope ) ) {
        offset =  DR_ReadConstant( abbrev, entry );
        *num = offset;
        ret = true;
    } else {
        ret = false;
    }
    return( ret );
}

unsigned DRENTRY DRGetAddrSize( drmem_hdl mod )
/**********************************************
 * returns the size of the address for the compile unit
 */
{
    return( DR_VMReadByte( mod + 10 ) );
}

drmem_hdl DRENTRY DRDebugPCHDef( drmem_hdl entry )
/************************************************/
{
    drmem_hdl   abbrev;
    drmem_hdl   ret;

    abbrev = DR_SkipTag( &entry ) + 1;
    if( DR_ScanForAttrib( &abbrev, &entry, DW_AT_base_types ) ) {
        ret = DR_ReadReference( abbrev, entry );
    } else {
        ret = DRMEM_HDL_NULL;
    }
    return( ret );
}

dr_tag_type DRENTRY DRGetTagType( drmem_hdl entry )
/*************************************************/
{
    dr_tag_type tagtype;
    dw_tagnum   tag;

    tag = DR_GetTag( entry );
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
