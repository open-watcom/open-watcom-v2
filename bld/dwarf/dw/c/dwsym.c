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
* Description:  Manage subroutines and lexical blocks.
*
****************************************************************************/


#include <stdarg.h>
#include "dwpriv.h"
#include "dwloc.h"
#include "dwtype.h"
#include "dwsym.h"
#include "dwhandle.h"
#include "dwrefer.h"


dw_handle DWENTRY DWBeginLexicalBlock(
    dw_client       cli,
    dw_loc_handle   segment,
    const char      *name )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    new = DW_LabelNewHandle( cli );
    abbrev = AB_LEXICAL_BLOCK | AB_SIBLING | AB_START_REF;
    if( name ) abbrev |= AB_NAME;
    if( segment ) abbrev |= AB_SEGMENT;
    DW_StartDIE( cli, abbrev );
    /* AT_name */
    if( name ) {
        DW_InfoString( cli, name );
    }
    if( segment ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), segment );
    }
    /* AT_low_pc */
    DW_InfoReloc( cli, DW_W_LOW_PC );
    /* AT_high_pc */
    DW_InfoReloc( cli, DW_W_HIGH_PC );
    DW_EndDIE( cli );
    DW_StartChildren( cli );
    return( new );
}

void DWENTRY DWEndLexicalBlock( dw_client cli )
{
    DW_EndChildren( cli );
    DW_EndRef( cli );
}


dw_handle DWENTRY DWBeginCommonBlock(
    dw_client       cli,
    dw_loc_handle   loc,
    dw_loc_handle   segment,
    const char      *name,
    dw_flags        flags )
{
    dw_handle       new;
    abbrev_code     abbrev;

    new = DW_LabelNewHandle( cli );
    abbrev = AB_COMMON_BLOCK | AB_SIBLING | AB_START_REF;
    if( name ) abbrev |= AB_NAME;
    if( segment ) abbrev |= AB_SEGMENT;
    if( flags & DW_FLAG_DECLARATION ) abbrev |= AB_DECLARATION;
    DW_StartDIE( cli, abbrev );
    if( flags & DW_FLAG_DECLARATION ) {
        DW_Info8( cli, 1 );
    }
    /* AT_name */
    if( name ) {
        DW_InfoString( cli, name );
    }
    /* AT_segment */
    if( segment ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), segment );
    }
    /* AT_location */
    DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), loc );
    DW_EndDIE( cli );
    DW_StartChildren( cli );
    return( new );
}


void DWENTRY DWEndCommonBlock( dw_client cli )
{
    DW_EndChildren( cli );
    DW_EndRef( cli );
}

dw_handle DWENTRY DWNameListBegin( dw_client cli, const char *name )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    new_hdl = DW_LabelNewHandle( cli );
    abbrev = AB_NAMELIST | AB_SIBLING | AB_START_REF;
    DW_StartDIE( cli, abbrev );
    /* AT_name */
    DW_InfoString( cli, name );
    DW_EndDIE( cli );
    DW_StartChildren( cli );
    return( new_hdl );
}

void DWENTRY DWNameListItem( dw_client cli, dw_handle ref )
{
    abbrev_code abbrev;

    abbrev = AB_NAMELIST_ITEM;
    DW_StartDIE( cli, abbrev );
    /* AT_namelist_item */
    DW_InfoHandleReference( cli, ref );
    DW_EndDIE( cli );
}

void DWENTRY DWEndNameList( dw_client cli )
{
    DW_EndChildren( cli );
    DW_EndRef( cli );
}

dw_handle DWENTRY DWBeginInlineSubroutine(
    dw_client       cli,
    dw_handle       subr,
    dw_loc_handle   ret_addr_loc,
    dw_loc_handle   segment )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    new = DW_GetHandle( cli );
    abbrev = AB_INLINED_SUBROUTINE | AB_SIBLING | AB_START_REF;
    if( segment ) abbrev |= AB_SEGMENT;
    if( ret_addr_loc ) abbrev |= AB_RETURN_ADDR;
    DW_StartDIE( cli, AB_INLINED_SUBROUTINE );
    if( ret_addr_loc ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), ret_addr_loc );
    }
    if( segment ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), segment );
    }
    DW_InfoHandleReference( cli, subr );
    /* AT_low_pc */
    DW_InfoReloc( cli, DW_W_LOW_PC );
    /* AT_high_pc */
    DW_InfoReloc( cli, DW_W_HIGH_PC );
    DW_EndDIE( cli );
    DW_StartChildren( cli );
    return( new );
}

//TODO bust up into a more saner bunch of routines
dw_handle DWENTRY DWBeginSubroutine(
    dw_client       cli,
    uint            call_type,
    dw_handle       return_type,
    dw_loc_handle   return_addr_loc,
    dw_loc_handle   frame_base_loc,
    dw_loc_handle   vtable_loc,
    dw_handle       member_hdl,
    dw_loc_handle   segment,
    const char      *name,
    dw_addr_offset  start_scope,
    dw_flags        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    /* unused parameters */ (void)call_type;

    _Validate( name != NULL );
    new = DW_GetHandle( cli );
    if( flags & DW_FLAG_DECLARATION ) {
        abbrev = AB_SUBROUTINE_DECL;
        if( vtable_loc )
            abbrev |= AB_VTABLE_LOC;
        if( member_hdl ) {
            abbrev |= AB_MEMBER;
        }
    } else {
        abbrev = AB_SUBROUTINE;
        if( vtable_loc )
            abbrev |= AB_VTABLE_LOC;
        if( member_hdl )
            abbrev |= AB_MEMBER;
        if( segment ) {
            abbrev |= AB_SEGMENT;
        }
    }
    if( return_type != 0 ){
        abbrev |= AB_TYPE;
    }
    DW_StartDIE( cli, abbrev | AB_SIBLING | AB_START_REF );
    /* AT_public, AT_protected, AT_private */
    DW_EmitAccessFlags( cli, flags );
    /* AT_type reference */
    if( abbrev & AB_TYPE ){
        DW_EmitTypeRef( cli, return_type );
    }
    /* AT_start_scope */
    if( (abbrev & AB_SUBROUTINE) == AB_SUBROUTINE ){
        DW_InfoULEB128( cli, start_scope );
    }
    /* AT_member */
    if( abbrev & AB_MEMBER ) {
        DW_InfoHandleReference( cli, member_hdl );
    }
    /* AT_segment */
    if( abbrev & AB_SEGMENT ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), segment );
    }
    /* AT_vtable_location */
    if( abbrev & AB_VTABLE_LOC  ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), vtable_loc );
    }
    /* AT_name */
    DW_InfoString( cli, name );
    /* AT_external */
    DW_Info8( cli, (flags & DW_FLAG_SUB_STATIC) == 0 );
    /* AT_inline */
    DW_Info8( cli, GET_FLAG_INLINE( flags ) );
    /* AT_calling_convention */
    if( flags & DW_FLAG_MAIN ) {
        DW_Info8( cli, DW_CC_program );
    } else {
        DW_Info8( cli, DW_CC_normal );
    }
    /* AT_prototyped */
    DW_Info8( cli, (flags & DW_FLAG_PROTOTYPED) != 0 );
    /* AT_virtuality */
    DW_Info8( cli, GET_FLAG_VIRTUAL( flags ) );
    /* AT_artificial */
    DW_Info8( cli, (flags & DW_FLAG_ARTIFICIAL) != 0 );
    if( flags & DW_FLAG_DECLARATION ) {
        /* AT_declaration */
        DW_Info8( cli, 1 );
    } else {
        /* AT_return_addr */
        if( return_addr_loc != NULL ) {
            DW_InfoEmitLocExpr( cli, sizeof( uint_8 ),return_addr_loc );
        } else {
            DW_InfoEmitLocExprNull( cli, sizeof( uint_8 ) );
        }
        /* AT_low_pc */
        DW_InfoReloc( cli, DW_W_LOW_PC );
        /* AT_high_pc */
        DW_InfoReloc( cli, DW_W_HIGH_PC );
    }
    /* AT_address_class */
    DW_Info8( cli, GET_FLAG_PTR_TYPE( flags ) );
    /* AT_frame_base */
    if( frame_base_loc != NULL ){
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ),frame_base_loc );
    }else{
        DW_InfoEmitLocExprNull( cli, sizeof( uint_8 ) );
    }
    DW_EndDIE( cli );
    DW_StartChildren( cli );
    return( new );
}

dw_handle DWENTRY DWBeginEntryPoint(
    dw_client       cli,
    dw_handle       return_type,
    dw_loc_handle   return_addr_loc,
    dw_loc_handle   segment,
    const char      *name,
    dw_addr_offset  start_scope,
    dw_flags        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    _Validate( name != NULL );
    new = DW_GetHandle( cli );
    abbrev = AB_ENTRY_POINT;
    if( segment )
        abbrev |= AB_SEGMENT;
    if( return_addr_loc )
        abbrev |= AB_RETURN_ADDR;
    if( return_type != 0 ){
        abbrev |= AB_TYPE;
    }
    DW_StartDIE( cli, abbrev | AB_SIBLING | AB_START_REF );
    /* AT_public, AT_protected, AT_private */
    DW_EmitAccessFlags( cli, flags );
    /* AT_type reference */
    if( abbrev & AB_TYPE ){
        DW_EmitTypeRef( cli, return_type );
    }
    /* AT_start_scope */
    DW_InfoULEB128( cli, start_scope );
    /* AT_return_addr */
    if( abbrev & AB_RETURN_ADDR ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), return_addr_loc );
    }
    /* AT_segment */
    if( abbrev & AB_SEGMENT ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), segment );
    }
    /* AT_low_pc */
    DW_InfoReloc( cli, DW_W_LOW_PC );
    /* AT_address_class */
    DW_Info8( cli, GET_FLAG_PTR_TYPE( flags ) );
    /* AT_name */
    DW_InfoString( cli, name );
    DW_EndDIE( cli );
    DW_StartChildren( cli );
    return( new );
}

static void MemFuncCommon(
    dw_client       cli,
    abbrev_code     abbrev,
    dw_handle       return_type,
    const char      *name,
    dw_flags        flags )
{
    DW_StartDIE( cli, abbrev );
    if( flags & DW_FLAG_ARTIFICIAL ){
        /* AT_artificial */
        DW_Info8( cli, 1 );
    }
    /* AT_accessibility =  AT_public, AT_protected, AT_private */
    DW_EmitAccessFlags( cli, flags );
    /* AT_type */
    DW_EmitTypeRef( cli, return_type );
    /* AT_name */
    DW_InfoString( cli, name );
    /* AT_declaration */
    DW_Info8( cli, (flags & DW_FLAG_DECLARATION) != 0 );
    /* AT_inline */
    DW_Info8( cli, GET_FLAG_INLINE( flags ) );
}

dw_handle DWENTRY DWBeginMemFuncDecl(
    dw_client       cli,
    dw_handle       return_type,
    dw_loc_handle   segment,
    dw_loc_handle   loc,
    const char      *name,
    dw_flags        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    /* unused parameters */ (void)segment; (void)loc;

    new = DW_GetHandle( cli );
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
//      DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), segment );
//  }
//  /* AT_loc       */
//  if( loc != NULL ){
//      DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), loc );
//  }else{
//      DW_InfoEmitLocExprNull( cli, sizeof( uint_8 ) );
//  }
    DW_EndDIE( cli );
    return( new );
}

dw_handle DWENTRY DWBeginVirtMemFuncDecl(
    dw_client       cli,
    dw_handle       return_type,
    dw_loc_handle   vtable_loc,
    const char      *name,
    dw_flags        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    new = DW_GetHandle( cli );
    abbrev = AB_VIRTMEMFUNCDECL;
    _Assert( (flags & DW_FLAG_ARTIFICIAL) == 0 );
    MemFuncCommon( cli, abbrev, return_type, name, flags );
    /* AT_virtuality  */
    DW_Info8( cli, 1 );
    /* AT_vtable_location */
    if( vtable_loc != NULL ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), vtable_loc );
    } else {
        DW_InfoEmitLocExprNull( cli, sizeof( uint_8 ) );
    }
    DW_EndDIE( cli );
    return( new );
}

void DWENTRY DWEndSubroutine( dw_client cli )
{
    DW_EndChildren( cli );
    DW_EndRef( cli );
}


dw_handle DWENTRY DWFormalParameter(
    dw_client       cli,
    dw_handle       parm_type,
    dw_loc_handle   parm_loc,
    dw_loc_handle   entry_loc,
    const char      *name,
    uint            default_value_type,
    ... )
{
    dw_handle                   new;
    abbrev_code                 abbrev;
    va_list                     args;
    const void *                value;
    size_t                      len;

    new = DW_LabelNewHandle( cli );
    _ValidateHandle( parm_type );
    va_start( args, default_value_type );
    abbrev = default_value_type == DW_DEFAULT_NONE ? AB_FORMAL_PARAMETER
        : AB_FORMAL_PARAMETER_WITH_DEFAULT;
    DW_StartDIE( cli, abbrev );
    switch( default_value_type ) {
    case DW_DEFAULT_NONE:
        break;
    case DW_DEFAULT_FUNCTION:
        DW_InfoULEB128( cli, DW_FORM_addr );
        DW_InfoReloc( cli, DW_W_DEFAULT_FUNCTION );
        break;
    case DW_DEFAULT_STRING:
        DW_InfoULEB128( cli, DW_FORM_string );
        DW_InfoString( cli, va_arg( args, const char * ) );
        break;
    case DW_DEFAULT_BLOCK:
        DW_InfoULEB128( cli, DW_FORM_block );
        value = va_arg( args, const void * );
        len = va_arg( args, size_t );
        DW_InfoULEB128( cli, (dw_uconst)len );
        DW_InfoBytes( cli, value, len );
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
    DW_InfoString( cli, name );
    /* AT_location */
    if( parm_loc ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), parm_loc );
    }else{
        DW_InfoEmitLocExprNull( cli, sizeof( uint_8 ) );
    }
    /* AT_WATCOM_parm_entry */
    if( entry_loc ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), entry_loc );
    }else{
        DW_InfoEmitLocExprNull( cli, sizeof( uint_8 ) );
    }
    /* AT_type */
    DW_EmitTypeRef( cli, parm_type );
    DW_EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWEllipsis( dw_client cli )
{
    dw_handle                   new;

    new = DW_LabelNewHandle( cli );
    DW_StartDIE( cli, AB_ELLIPSIS );
    DW_EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWLabel(
    dw_client       cli,
    dw_loc_handle   segment,
    const char      *name,
    dw_addr_offset  start_scope )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    new = DW_GetHandle( cli );
    abbrev = AB_LABEL;
    if( name ) abbrev |= AB_NAME;
    if( segment ) abbrev |= AB_SEGMENT;
    DW_StartDIE( cli, abbrev );
    /* AT_name */
    if( name ) {
        DW_InfoString( cli, name );
    }
    /* AT_start_scope */
    DW_InfoULEB128( cli, start_scope );
    /* AT_segment */
    if( segment ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), segment );
    }
    /* AT_low_pc */
    DW_InfoReloc( cli, DW_W_LABEL );
    DW_EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWVariable(
    dw_client       cli,
    dw_handle       type,
    dw_loc_handle   loc,
    dw_handle       member_of,
    dw_loc_handle   segment,
    const char      *name,
    dw_addr_offset  start_scope,
    dw_flags        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    /* unused parameters */ (void)start_scope;

    _ValidateHandle( type );
    _Validate( name != NULL );
    new = DW_LabelNewHandle( cli );
    abbrev = AB_VARIABLE;
    if( member_of )
        abbrev |= AB_MEMBER;
    if( segment )
        abbrev |= AB_SEGMENT;
    if( flags & DW_FLAG_DECLARATION )
        abbrev |= AB_DECLARATION;
    DW_StartDIE( cli, abbrev );
    if( flags & DW_FLAG_DECLARATION ) {
        DW_Info8( cli, 1 );
    }
    /* AT_private, etc. */
    DW_EmitAccessFlags( cli, flags );
    /* AT_member */
    if( member_of ) {
        DW_InfoHandleReference( cli, member_of );
    }
    /* AT_segment */
    if( segment ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), segment );
    }
    DW_Info8( cli, (flags & DW_FLAG_GLOBAL) != 0 );
    DW_Info8( cli, (flags & DW_FLAG_ARTIFICIAL) != 0 );
    /* AT_name */
    DW_InfoString( cli, name );
    /* AT_location */
    if( loc ) {
        DW_InfoEmitLocExpr( cli, sizeof( uint_8 ), loc );
    } else {
        DW_InfoEmitLocExprNull( cli, sizeof( uint_8 ) );
    }
    /* AT_type */
    DW_EmitTypeRef( cli, type );
    DW_EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWConstant(
    dw_client       cli,
    dw_handle       type,
    const void      *value,
    size_t          len,
    dw_handle       member_of,
    const char      *name,
    dw_addr_offset  start_scope,
    dw_flags        flags )
{
    dw_handle                   new;
    abbrev_code                 abbrev;

    /* unused parameters */ (void)start_scope;

    _ValidateHandle( type );
    _Validate( name != NULL );
    new = DW_LabelNewHandle( cli );
    abbrev = AB_CONSTANT;
    if( member_of )
        abbrev |= AB_MEMBER;
    DW_StartDIE( cli, abbrev );
    /* AT_private */
    DW_EmitAccessFlags( cli, flags );
    /* AT_member */
    if( member_of ) {
        DW_InfoHandleReference( cli, member_of );
    }
    /* AT_external */
    DW_Info8( cli, (flags & DW_FLAG_GLOBAL) != 0 );
    /* AT_const_value */
    if( len == 0 ) {    /* assume it's a string */
        DW_InfoULEB128( cli, DW_FORM_string );
        DW_InfoString( cli, value );
    } else {
        DW_InfoULEB128( cli, DW_FORM_block );
        DW_InfoULEB128( cli, (dw_uconst)len );
        DW_InfoBytes( cli, value, len );
    }
    /* AT_name */
    DW_InfoString( cli, name );
    /* AT_type */
    DW_EmitTypeRef( cli, type );
    DW_EndDIE( cli );
    return( new );
}


dw_handle DWENTRY DWIncludeCommonBlock( dw_client cli, dw_handle common_block )
{
    dw_handle                   new;

    new = DW_LabelNewHandle( cli );
    DW_StartDIE( cli, AB_COMMON_INCLUSION );
    /* AT_common_inclusion */
    DW_InfoHandleReference( cli, common_block );
    DW_EndDIE( cli );
    return( new );
}
