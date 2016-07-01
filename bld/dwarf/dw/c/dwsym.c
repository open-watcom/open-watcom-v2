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
* Description:  Manage subroutines and lexical blocks.
*
****************************************************************************/


#include <stdarg.h>
#include "dwpriv.h"
#include "dwutils.h"
#include "dwloc.h"
#include "dwtype.h"
#include "dwsym.h"
#include "dwhandle.h"
#include "dwrefer.h"


dw_handle DWENTRY DWBeginLexicalBlock(
    dw_client                   cli,
    dw_loc_handle               segment,
    const char *                name )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    new = LabelNewHandle( cli );
    abbrev = AB_LEXICAL_BLOCK | AB_SIBLING | AB_START_REF;
    if( name ) abbrev |= AB_NAME;
    if( segment ) abbrev |= AB_SEGMENT;
    StartDIE( cli, abbrev );
    /* AT_name */
    if( name ) {
        InfoString( cli, name );
    }
    if( segment ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), segment );
    }
    /* AT_low_pc */
    InfoReloc( cli, DW_W_LOW_PC );
    /* AT_high_pc */
    InfoReloc( cli, DW_W_HIGH_PC );
    EndDIE( cli );
    StartChildren( cli );
    return( new );
}

void DWENTRY DWEndLexicalBlock(
    dw_client                   cli )
{
    EndChildren( cli );
    EndRef( cli );
}


dw_handle DWENTRY DWBeginCommonBlock(
    dw_client                   cli,
    dw_loc_handle               loc,
    dw_loc_handle               segment,
    const char *                name,
    unsigned                    flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    new = LabelNewHandle( cli );
    abbrev = AB_COMMON_BLOCK | AB_SIBLING | AB_START_REF;
    if( name ) abbrev |= AB_NAME;
    if( segment ) abbrev |= AB_SEGMENT;
    if( flags & DW_FLAG_DECLARATION ) abbrev |= AB_DECLARATION;
    StartDIE( cli, abbrev );
    if( flags & DW_FLAG_DECLARATION ) {
        Info8( cli, 1 );
    }
    /* AT_name */
    if( name ) {
        InfoString( cli, name );
    }
    /* AT_segment */
    if( segment ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), segment );
    }
    /* AT_location */
    EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), loc );
    EndDIE( cli );
    StartChildren( cli );
    return( new );
}


void DWENTRY DWEndCommonBlock(
    dw_client                   cli )
{
    EndChildren( cli );
    EndRef( cli );
}

dw_handle DWENTRY DWNameListBegin(
    dw_client                   cli,
    const char *                name )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    new_hdl = LabelNewHandle( cli );
    abbrev = AB_NAMELIST | AB_SIBLING | AB_START_REF;
    StartDIE( cli, abbrev );
    /* AT_name */
    InfoString( cli, name );
    EndDIE( cli );
    StartChildren( cli );
    return( new_hdl );
}

void DWENTRY DWNameListItem(
    dw_client                   cli,
    dw_handle                    ref )
{
    abbrev_code abbrev;

    abbrev = AB_NAMELIST_ITEM;
    StartDIE( cli, abbrev );
    /* AT_namelist_item */
    HandleReference( cli, ref, DW_DEBUG_INFO );
    EndDIE( cli );
}

void DWENTRY DWEndNameList(
    dw_client                   cli )
{
    EndChildren( cli );
    EndRef( cli );
}

dw_handle DWENTRY DWBeginInlineSubroutine(
    dw_client                   cli,
    dw_handle                   subr,
    dw_loc_handle               ret_addr_loc,
    dw_loc_handle               segment )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    new = GetHandle( cli );
    abbrev = AB_INLINED_SUBROUTINE | AB_SIBLING | AB_START_REF;
    if( segment ) abbrev |= AB_SEGMENT;
    if( ret_addr_loc ) abbrev |= AB_RETURN_ADDR;
    StartDIE( cli, AB_INLINED_SUBROUTINE );
    if( ret_addr_loc ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), ret_addr_loc );
    }
    if( segment ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), segment );
    }
    HandleReference( cli, subr, DW_DEBUG_INFO );
    /* AT_low_pc */
    InfoReloc( cli, DW_W_LOW_PC );
    /* AT_high_pc */
    InfoReloc( cli, DW_W_HIGH_PC );
    EndDIE( cli );
    StartChildren( cli );
    return( new );
}

//TODO bust up into a more saner bunch of routines
dw_handle DWENTRY DWBeginSubroutine(
    dw_client                   cli,
    uint                        call_type,
    dw_handle                   return_type,
    dw_loc_handle               return_addr_loc,
    dw_loc_handle               frame_base_loc,
    dw_loc_handle               vtable_loc,
    dw_handle                   member_hdl,
    dw_loc_handle               segment,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    _Validate( name != NULL );
    call_type = call_type;
    new = GetHandle( cli );
    if( flags & DW_FLAG_DECLARATION ){
        abbrev = AB_SUBROUTINE_DECL;
        if( vtable_loc ) abbrev |= AB_VTABLE_LOC;
        if( member_hdl ) abbrev |= AB_MEMBER;
    } else {
        abbrev = AB_SUBROUTINE;
        if( vtable_loc ) abbrev |= AB_VTABLE_LOC;
        if( member_hdl ) abbrev |= AB_MEMBER;
        if( segment ) abbrev |= AB_SEGMENT;
    }
    if( return_type != 0 ){
        abbrev |= AB_TYPE;
    }
    StartDIE( cli, abbrev | AB_SIBLING | AB_START_REF );
    /* AT_public, AT_protected, AT_private */
    EmitAccessFlags( cli, flags );
    /* AT_type reference */
    if( abbrev & AB_TYPE ){
        EmitTypeRef( cli, return_type );
    }
    /* AT_start_scope */
    if( (abbrev & AB_SUBROUTINE) == AB_SUBROUTINE ){
        InfoULEB128( cli, start_scope );
    }
    /* AT_member */
    if( abbrev & AB_MEMBER ) {
        HandleReference( cli, member_hdl, DW_DEBUG_INFO );
    }
    /* AT_segment */
    if( abbrev & AB_SEGMENT ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), segment );
    }
    /* AT_vtable_location */
    if( abbrev & AB_VTABLE_LOC  ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), vtable_loc );
    }
    /* AT_name */
    InfoString( cli, name );
    /* AT_external */
    Info8( cli, (flags & DW_SUB_STATIC) == 0 );
    /* AT_inline */
    Info8( cli, (flags & DW_FLAG_INLINE_MASK) >> DW_FLAG_INLINE_SHIFT );
    /* AT_calling_convention */
    if( flags & DW_FLAG_MAIN ) {
        Info8( cli, DW_CC_program );
    } else {
        Info8( cli, DW_CC_normal );
    }
    /* AT_prototyped */
    Info8( cli, (flags & DW_FLAG_PROTOTYPED) != 0 );
    /* AT_virtuality */
    Info8( cli, (flags & DW_FLAG_VIRTUAL_MASK) >> DW_FLAG_VIRTUAL_SHIFT );
    /* AT_artificial */
    Info8( cli, (flags & DW_FLAG_ARTIFICIAL) != 0 );
    if( flags & DW_FLAG_DECLARATION ){
        /* AT_declaration */
        Info8( cli, 1 );
    }else{
        /* AT_return_addr */
        if( return_addr_loc != NULL ){
            EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ),return_addr_loc );
        }else{
            EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8 ) );
        }
        /* AT_low_pc */
        InfoReloc( cli, DW_W_LOW_PC );
        /* AT_high_pc */
        InfoReloc( cli, DW_W_HIGH_PC );
    }
    /* AT_address_class */
    Info8( cli, (flags & DW_PTR_TYPE_MASK) >> DW_PTR_TYPE_SHIFT );
    /* AT_frame_base */
    if( frame_base_loc != NULL ){
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ),frame_base_loc );
    }else{
        EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8 ) );
    }
    EndDIE( cli );
    StartChildren( cli );
    return( new );
}

dw_handle DWENTRY DWBeginEntryPoint(
    dw_client                   cli,
    dw_handle                   return_type,
    dw_loc_handle               return_addr_loc,
    dw_loc_handle               segment,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    _Validate( name != NULL );
    new = GetHandle( cli );
    abbrev = AB_ENTRY_POINT;
    if( segment ) abbrev |= AB_SEGMENT;
    if( return_addr_loc ) abbrev |= AB_RETURN_ADDR;
    if( return_type != 0 ){
        abbrev |= AB_TYPE;
    }
    StartDIE( cli, abbrev | AB_SIBLING | AB_START_REF );
    /* AT_public, AT_protected, AT_private */
    EmitAccessFlags( cli, flags );
    /* AT_type reference */
    if( abbrev & AB_TYPE ){
        EmitTypeRef( cli, return_type );
    }
    /* AT_start_scope */
    InfoULEB128( cli, start_scope );
    /* AT_return_addr */
    if( abbrev & AB_RETURN_ADDR ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), return_addr_loc );
    }
    /* AT_segment */
    if( abbrev & AB_SEGMENT ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), segment );
    }
    /* AT_low_pc */
        InfoReloc( cli, DW_W_LOW_PC );
    /* AT_address_class */
    Info8( cli, (flags & DW_PTR_TYPE_MASK) >> DW_PTR_TYPE_SHIFT );
    /* AT_name */
    InfoString( cli, name );
    EndDIE( cli );
    StartChildren( cli );
    return( new );
}

static void MemFuncCommon(
    dw_client                   cli,
    abbrev_code                 abbrev,
    dw_handle                   return_type,
    const char *                name,
    uint                        flags )
{
    StartDIE( cli, abbrev );
    if( flags & DW_FLAG_ARTIFICIAL ){
        /* AT_artificial */
        Info8( cli, 1 );
    }
    /* AT_accessibility =  AT_public, AT_protected, AT_private */
    EmitAccessFlags( cli, flags );
    /* AT_type */
    EmitTypeRef( cli, return_type );
    /* AT_name */
    InfoString( cli, name );
    /* AT_declaration */
    if( flags & DW_FLAG_DECLARATION ){
        Info8( cli, 1 );
    }else{
        Info8( cli, 0 );
    }
    /* AT_inline */
    Info8( cli, (flags & DW_FLAG_INLINE_MASK) >> DW_FLAG_INLINE_SHIFT );
}

dw_handle DWENTRY DWBeginMemFuncDecl(
    dw_client                   cli,
    dw_handle                   return_type,
    dw_loc_handle               segment,
    dw_loc_handle               loc,
    const char *                name,
    uint                        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    segment = segment;
    loc = loc;
    new = GetHandle( cli );
//  if( segment != NULL ){
//      abbrev = AB_MEMFUNCDECLSEG;
//  }else{
        abbrev = AB_MEMFUNCDECL;
//  }
    if( flags & DW_FLAG_ARTIFICIAL ){
        abbrev |= AB_ARTIFICIAL;
    }
    MemFuncCommon( cli, abbrev, return_type, name, flags );
    /* AT_segment    */
//  if( segment != NULL ){
//      EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), segment );
//  }
//  /* AT_loc       */
//  if( loc != NULL ){
//      EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), loc );
//  }else{
//      EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8 ) );
//  }
    EndDIE( cli );
    return( new );
}

dw_handle DWENTRY DWBeginVirtMemFuncDecl(
    dw_client                   cli,
    dw_handle                   return_type,
    dw_loc_handle               vtable_loc,
    const char *                name,
    uint                        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    new = GetHandle( cli );
    abbrev = AB_VIRTMEMFUNCDECL;
    _Assert( (flags & DW_FLAG_ARTIFICIAL) == 0 );
    MemFuncCommon( cli, abbrev, return_type, name, flags );
    /* AT_virtuality  */
    Info8( cli, 1 );
    /* AT_vtable_location */
    if( vtable_loc != NULL ){
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), vtable_loc );
    }else{
        EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8 ) );
    }
    EndDIE( cli );
    return( new );
}

void DWENTRY DWEndSubroutine(
    dw_client                   cli )
{
    EndChildren( cli );
    EndRef( cli );
}


dw_handle DWENTRY DWFormalParameter(
    dw_client                   cli,
    dw_handle                   parm_type,
    dw_loc_handle               parm_loc,
    dw_loc_handle               entry_loc,
    const char *                name,
    uint                        default_value_type,
    ... )
{
    dw_handle                   new;
    abbrev_code                 abbrev;
    va_list                     args;
    const void *                value;
    size_t                      len;

    new = LabelNewHandle( cli );
    _Validate( parm_type != 0 );
    va_start( args, default_value_type );
    abbrev = default_value_type == DW_DEFAULT_NONE ? AB_FORMAL_PARAMETER
        : AB_FORMAL_PARAMETER_WITH_DEFAULT;
    StartDIE( cli, abbrev );
    switch( default_value_type ) {
    case DW_DEFAULT_NONE:
        break;
    case DW_DEFAULT_FUNCTION:
        InfoULEB128( cli, DW_FORM_addr );
        InfoReloc( cli, DW_W_DEFAULT_FUNCTION );
        break;
    case DW_DEFAULT_STRING:
        InfoULEB128( cli, DW_FORM_string );
        InfoString( cli, va_arg( args, const char * ) );
        break;
    case DW_DEFAULT_BLOCK:
        InfoULEB128( cli, DW_FORM_block );
        value = va_arg( args, const void * );
        len = va_arg( args, size_t );
        InfoULEB128( cli, (dw_uconst)len );
        InfoBytes( cli, value, len );
        break;
    default:
        _Abort( ABORT_INVALID_DEFAULT_VALUE );
        break;
    }
    va_end( args );
    /* AT_name */
    if( name == NULL ) {
        name = "";
    }
    InfoString( cli, name );
    /* AT_location */
    if( parm_loc ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), parm_loc );
    }else{
        EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8 ) );
    }
    /* AT_WATCOM_parm_entry */
    if( entry_loc ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), entry_loc );
    }else{
        EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8 ) );
    }
    /* AT_type */
    EmitTypeRef( cli, parm_type );
    EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWEllipsis(
    dw_client                   cli )
{
    dw_handle                   new;

    new = LabelNewHandle( cli );
    StartDIE( cli, AB_ELLIPSIS );
    EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWLabel(
    dw_client                   cli,
    dw_loc_handle               segment,
    const char *                name,
    dw_addr_offset              start_scope )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    new = GetHandle( cli );
    abbrev = AB_LABEL;
    if( name ) abbrev |= AB_NAME;
    if( segment ) abbrev |= AB_SEGMENT;
    StartDIE( cli, abbrev );
    /* AT_name */
    if( name ) {
        InfoString( cli, name );
    }
    /* AT_start_scope */
    InfoULEB128( cli, start_scope );
    /* AT_segment */
    if( segment ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), segment );
    }
    /* AT_low_pc */
    InfoReloc( cli, DW_W_LABEL );
    EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWVariable(
    dw_client                   cli,
    dw_handle                   type,
    dw_loc_handle               loc,
    dw_handle                   member_of,
    dw_loc_handle               segment,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    start_scope = start_scope;
    _Validate( type != 0 );
    _Validate( name !=NULL );
    new = LabelNewHandle( cli );
    abbrev = AB_VARIABLE;
    if( member_of ) abbrev |= AB_MEMBER;
    if( segment ) abbrev |= AB_SEGMENT;
    if( flags & DW_FLAG_DECLARATION ) abbrev |= AB_DECLARATION;
    StartDIE( cli, abbrev );
    if( flags & DW_FLAG_DECLARATION ) {
        Info8( cli, 1 );
    }
    /* AT_private, etc. */
    EmitAccessFlags( cli, flags );
    /* AT_member */
    if( member_of ) {
        HandleReference( cli, member_of, DW_DEBUG_INFO );
    }
    /* AT_segment */
    if( segment ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), segment );
    }
    Info8( cli, (flags & DW_FLAG_GLOBAL) != 0 );
    Info8( cli, (flags & DW_FLAG_ARTIFICIAL) != 0 );
    /* AT_name */
    InfoString( cli, name );
    /* AT_location */
    if( loc ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8 ), loc );
    }else{
        EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8 ) );
    }
    /* AT_type */
    EmitTypeRef( cli, type );
    EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWConstant(
    dw_client                   cli,
    dw_handle                   type,
    const void *                value,
    size_t                      len,
    dw_handle                   member_of,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    start_scope = start_scope;
    _Validate( type != 0 );
    _Validate( name != NULL );
    new = LabelNewHandle( cli );
    abbrev = AB_CONSTANT;
    if( member_of )
        abbrev |= AB_MEMBER;
    StartDIE( cli, abbrev );
    /* AT_private */
    EmitAccessFlags( cli, flags );
    /* AT_member */
    if( member_of ) {
        HandleReference( cli, member_of, DW_DEBUG_INFO );
    }
    /* AT_external */
    Info8( cli, (flags & DW_FLAG_GLOBAL) != 0 );
    /* AT_const_value */
    if( len == 0 ) {    /* assume it's a string */
        InfoULEB128( cli, DW_FORM_string );
        InfoString( cli, value );
    } else {
        InfoULEB128( cli, DW_FORM_block );
        InfoULEB128( cli, (dw_uconst)len );
        InfoBytes( cli, value, len );
    }
    /* AT_name */
    InfoString( cli, name );
    /* AT_type */
    EmitTypeRef( cli, type );
    EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWIncludeCommonBlock(
    dw_client                   cli,
    dw_handle                   common_block )
{
    dw_handle                   new;

    new = LabelNewHandle( cli );
    StartDIE( cli, AB_COMMON_INCLUSION );
    /* AT_common_inclusion */
    HandleReference( cli, common_block, DW_DEBUG_INFO );
    EndDIE( cli );
    return( new );
}
