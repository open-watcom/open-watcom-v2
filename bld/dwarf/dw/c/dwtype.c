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
* Description:  Manage type information.
*
****************************************************************************/


#include "dwpriv.h"
#include "dwutils.h"
#include "dwinfo.h"
#include "dwhandle.h"
#include "dwloc.h"
#include "dwtype.h"
#include "dwrefer.h"

void EmitTypeRef(
    dw_client                   cli,
    dw_handle                   hdl )
{
    HandleReference( cli, hdl, DW_DEBUG_INFO );
}

dw_handle DWENTRY DWHandle(
    dw_client                   cli,
    uint                        kind )
{
    dw_handle                   new_hdl;

    kind = kind;
    new_hdl = NewHandle( cli );
    return( new_hdl );
}


void DWENTRY DWHandleSet(
    dw_client                   cli,
    dw_handle                   set_hdl )
{

    _Assert( cli->defset == 0 );
    cli->defset = set_hdl;
}

dw_handle DWENTRY DWFundamental(
    dw_client                   cli,
    char const                 *name,
    unsigned                    fund_idx,
    unsigned                    size )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    _Validate( fund_idx >= DW_FT_MIN && fund_idx < DW_FT_MAX );
    new_hdl = GetHandle( cli );
    abbrev = AB_BASE_TYPE;
    if( name ) abbrev |= AB_NAME;
    StartDIE( cli, abbrev );
    if( name ) {
        InfoString( cli, name );
    }
    Info8( cli, fund_idx );
    Info8( cli, size );
    EndDIE( cli );
    return( new_hdl );
}


dw_handle DWENTRY DWModifier(
    dw_client                   cli,
    dw_handle                   hdl,
    uint                        modifiers )
{
    dw_handle                   new_hdl = 0;
    abbrev_code                 abbrev;

    _Validate( hdl != 0 );
    while( modifiers != 0 ) {
        if( modifiers & DW_MOD_CONSTANT ) {
            abbrev = AB_CONST_TYPE;
            modifiers &= ~DW_MOD_CONSTANT;
        } else if( modifiers & DW_MOD_VOLATILE ) {
            abbrev = AB_VOLATILE_TYPE;
            modifiers &= ~DW_MOD_VOLATILE;
        } else {
            abbrev = AB_ADDR_CLASS_TYPE;
        }
        new_hdl = GetHandle( cli );
        StartDIE( cli, abbrev );
        HandleWriteOffset( cli, hdl, DW_DEBUG_INFO );
        if( abbrev == AB_ADDR_CLASS_TYPE ) {
            Info8( cli, modifiers );
            modifiers = 0;
        }
        EndDIE( cli );
        hdl = new_hdl;
    }
    return( new_hdl );
}


void EmitAccessFlags(
    dw_client                   cli,
    uint                        access_flags )
{
    access_flags &= DW_FLAG_ACCESS_MASK;
    switch( access_flags ) {
    case DW_FLAG_PRIVATE:
        Info8( cli, DW_ACCESS_private );
        break;
    case DW_FLAG_PROTECTED:
        Info8( cli, DW_ACCESS_protected );
        break;
    default:
        Info8( cli, DW_ACCESS_public );
    }
}


static void emitCommonTypeInfo(
    dw_client                   cli,
    abbrev_code                 abbrev,
    const char *                name,
    uint                        access_flags )
{
    if( name ) abbrev |= AB_NAME;
    StartDIE( cli, abbrev );
    EmitAccessFlags( cli, access_flags );
    if( name != NULL ) {
        /* AT_name */
        InfoString( cli, name );
    }
}


dw_handle DWENTRY DWTypedef(
    dw_client                   cli,
    dw_handle                   base_type,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        flags )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    start_scope = start_scope;
    _Validate( name != NULL );
    _Validate( base_type != 0 );
    new_hdl = GetHandle( cli );
    abbrev = AB_TYPEDEF_TYPE | AB_START_REF;
    emitCommonTypeInfo( cli, abbrev , name, flags );
    EmitTypeRef( cli, base_type );
    EndDIE( cli );
    EndRef( cli );
    return( new_hdl );
}

dw_handle DWENTRY DWRefPCH(
    dw_client                   cli,
    uint_32                     ref )
{
    dw_handle                   new_hdl;

    _Validate( ref != 0 );
    _Validate( cli->dbg_pch != 0 );
    new_hdl = GetHandle( cli );
    StartDIE( cli, AB_INDIRECT_TYPE );
    CLIReloc4( DW_DEBUG_INFO, DW_W_EXT_REF, cli->dbg_pch, ref  );
    EndDIE( cli );
    return( new_hdl );
}

void WriteAddressClass(
    dw_client                   cli,
    uint                        flags )
{
    flags &= DW_PTR_TYPE_MASK;
    if( flags ) {
        Info8( cli, flags >> DW_PTR_TYPE_SHIFT );
    }
}


dw_handle DWENTRY DWPointer(
    dw_client                   cli,
    dw_handle                   base_type,
    uint                        flags )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    _Validate( base_type != 0 );
    new_hdl = GetHandle( cli );
    abbrev = AB_POINTER_TYPE;
    if( flags & DW_FLAG_REFERENCE ) abbrev = AB_REFERENCE_TYPE;
    flags &= ~DW_FLAG_REFERENCE;
    if( flags ) abbrev |= AB_ADDRESS_CLASS;
    StartDIE( cli, abbrev );
    WriteAddressClass( cli, flags );
    HandleWriteOffset( cli, base_type, DW_DEBUG_INFO );
    EndDIE( cli );
    return( new_hdl );
}


dw_handle DWENTRY DWBasedPointer(
    dw_client                   cli,
    dw_handle                   base_type,
    dw_loc_handle               seg,
    uint                        flags )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    _Validate( base_type != 0 );
    new_hdl = GetHandle( cli );
    abbrev = AB_POINTER_TYPE;
    if( seg ) abbrev |= AB_SEGMENT;
    if( flags ) abbrev |= AB_ADDRESS_CLASS;
    StartDIE( cli, abbrev );
    if( seg ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8), seg );
    }
    WriteAddressClass( cli, flags );
    HandleWriteOffset( cli, base_type, DW_DEBUG_INFO );
    EndDIE( cli );
    return( new_hdl );
}

dw_handle DWENTRY DWSimpleArray(
    dw_client                   cli,
    dw_handle                   elt_type,
    int                         elt_count )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    new_hdl = GetHandle( cli );
    _Validate( elt_type != 0 );
    abbrev = AB_SIMPLE_ARRAY_TYPE;
    StartDIE( cli, abbrev );
    InfoULEB128( cli, elt_count );
    EmitTypeRef( cli, elt_type );
    EndDIE( cli );
    return( new_hdl );
}

dw_handle DWENTRY DWBeginArray(
    dw_client                   cli,
    dw_handle                   elt_type,
    uint                        stride_size,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        access_flags )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    start_scope = start_scope;
    new_hdl = GetHandle( cli );
    _Validate( elt_type != 0 );
    abbrev = AB_SIBLING;
    if( stride_size != 0 ) {
        abbrev |= AB_ARRAY_TYPE_WITH_STRIDE;
    } else {
        abbrev |= AB_ARRAY_TYPE;
    }
    emitCommonTypeInfo( cli, abbrev, name, access_flags );
    /* AT_stride_size */
    if( stride_size ) {
        InfoULEB128( cli, stride_size );
    }
    EmitTypeRef( cli, elt_type );
    EndDIE( cli );
    StartChildren( cli );
    return( new_hdl );
}


void DWENTRY DWArrayDimension(
    dw_client                   cli,
    const dw_dim_info *         dim_info )
{
    abbrev_code abbrev;

    abbrev = AB_ARRAY_BOUND;
    if( dim_info->index_type != 0 ) {
        abbrev |= AB_TYPE;
    }
    StartDIE( cli, abbrev );
    if( dim_info->index_type != 0 ) {
        EmitTypeRef( cli, dim_info->index_type );
    }
    /* AT_lower_bound */
    InfoULEB128( cli, dim_info->lo_data );
    /* AT_upper_bound */
    InfoULEB128( cli, dim_info->hi_data );
    EndDIE( cli );
}

void DWENTRY DWArrayVarDim(
    dw_client                   cli,
    const dw_vardim_info *      dim_info )
{
    abbrev_code abbrev;

    abbrev = AB_ARRAY_VARDIM;
    if( dim_info->index_type != 0 ) {
        abbrev |= AB_TYPE;
    }
    StartDIE( cli, abbrev );
    if( dim_info->index_type != 0 ) {
        EmitTypeRef( cli, dim_info->index_type );
    }
    /* AT_lower_bound */
    EmitTypeRef( cli, dim_info->lo_data );
    /* AT_count */
    EmitTypeRef( cli, dim_info->count_data );
    EndDIE( cli );
}

void DWENTRY DWEndArray(
    dw_client                   cli )
{
    EndChildren( cli );
}

dw_handle DWENTRY DWStruct(
    dw_client                   cli,
    uint                        kind )
{
    dw_handle                   new_hdl;

    new_hdl = NewHandle( cli );
    CreateExtra( cli, new_hdl )->structure.kind = kind;
    return( new_hdl );
}


void DWENTRY DWBeginStruct(
    dw_client                   cli,
    dw_handle                   struct_hdl,
    dw_size_t                   size,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        flags )
{
    abbrev_code                 abbrev;
    dw_st                       kind;

    start_scope = start_scope;
    SetHandleLocation( cli, struct_hdl );
    kind = GetExtra( cli, struct_hdl )->structure.kind;
    abbrev = 0;
    switch( kind ){
    case DW_ST_CLASS:
        abbrev = AB_CLASS_TYPE;
        break;
    case DW_ST_STRUCT:
        abbrev = AB_STRUCT_TYPE;
        break;
    case DW_ST_UNION:
        abbrev = AB_UNION_TYPE;
        break;
    }
    DestroyExtra( cli, struct_hdl );
    abbrev |= AB_SIBLING | AB_START_REF;
    emitCommonTypeInfo( cli, abbrev , name, flags );
    /* AT_byte_size */
    InfoULEB128( cli, size );
    EndDIE( cli );
    StartChildren( cli );
}


void DWENTRY DWAddFriend(
    dw_client                   cli,
    dw_handle                   friend )
{
    StartDIE( cli, AB_FRIEND );
    HandleReference( cli, friend, DW_DEBUG_INFO );
    EndDIE( cli );
}


dw_handle DWENTRY DWAddInheritance(
    dw_client                   cli,
    dw_handle                   ancestor_hdl,
    dw_loc_handle               loc,
    uint                        flags )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    new_hdl = LabelNewHandle( cli );
    abbrev = AB_INHERITANCE;
    StartDIE( cli, abbrev );
    EmitAccessFlags( cli, flags );
    /* AT_location */
    if( loc ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8), loc );
    }else{
        EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8) );
    }
    /* AT_virtual */
    if( flags & DW_FLAG_VIRTUAL ) {
        Info8( cli, DW_VIRTUALITY_virtual );
    } else {
        Info8( cli, DW_VIRTUALITY_none );
    }
    /* AT_type */
    HandleReference( cli, ancestor_hdl, DW_DEBUG_INFO );
    EndDIE( cli );
    return( new_hdl );
}



dw_handle DWENTRY DWAddField(
    dw_client                   cli,
    dw_handle                   field_hdl,
    dw_loc_handle               loc,
    const char *                name,
    uint                        flags )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    _Validate( field_hdl );
    _Validate( !(flags & DW_FLAG_STATIC) ); // use DWVariable dummy
    new_hdl = LabelNewHandle( cli );
    abbrev = AB_FIELD;
    if( (flags & DW_FLAG_ACCESS_MASK) != DW_FLAG_PUBLIC ){ // not the default
        abbrev |= AB_OPACCESS;
    }
    if( flags & DW_FLAG_ARTIFICIAL ){
        abbrev |= AB_ARTIFICIAL;
    }
    StartDIE( cli, abbrev );
    if( abbrev & AB_OPACCESS ){
        /* AT_accessibility */
        EmitAccessFlags( cli, flags );
    }
    /* AT_artificial */
    if( flags & DW_FLAG_ARTIFICIAL ){
        Info8( cli, 1 );
    }
    if( name == NULL ) {
        name  = "";
    }
    /* AT_name */
    InfoString( cli, name );
    /* AT_data_member_location */
    if( loc ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8), loc );
    }else{
        EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8) );
    }
    /* AT_type */
    EmitTypeRef( cli, field_hdl );
    EndDIE( cli );
    return( new_hdl );
}


dw_handle DWENTRY DWAddBitField(
    dw_client                   cli,
    dw_handle                   field_hdl,
    dw_loc_handle               loc,
    dw_size_t                   byte_size,
    uint                        bit_offset,
    uint                        bit_size,
    const char *                name,
    uint                        flags )
{
    abbrev_code                 abbrev;
    dw_handle                   new_hdl;

    _Validate( field_hdl );
    new_hdl = LabelNewHandle( cli );
    abbrev = AB_BITFIELD;
    if( byte_size ) abbrev |= AB_BYTE_SIZE;
    StartDIE( cli, abbrev );
    /* AT_accessibility */
    EmitAccessFlags( cli, flags );
    if( byte_size ) {
        /* AT_byte_size */
        InfoULEB128( cli, byte_size );
    }
    /* AT_bit_offset */
    InfoULEB128( cli, bit_offset );
    /* AT_bit_size  */
    InfoULEB128( cli, bit_size );
    /* AT_artificial */
    Info8( cli, (flags & DW_FLAG_ARTIFICIAL) != 0 );
    if( name == NULL ) {
        name  = "";
    }
    /* AT_name */
    InfoString( cli, name );
    /* AT_data_member_location */
    if( loc ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8), loc );
    }else{
        EmitLocExprNull( cli, DW_DEBUG_INFO, sizeof( uint_8) );
    }
    /* AT_type  */
    EmitTypeRef( cli, field_hdl );
    EndDIE( cli );
    return( new_hdl );
}


void DWENTRY DWEndStruct(
    dw_client                   cli )
{
    EndChildren( cli );
    EndRef( cli );
}


dw_handle DWENTRY DWBeginEnumeration(
    dw_client                   cli,
    dw_size_t                   byte_size,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        flags )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    start_scope = start_scope;
    new_hdl = GetHandle( cli );
    abbrev = AB_ENUMERATION | AB_SIBLING;
    emitCommonTypeInfo( cli, abbrev, name, flags );
    /* AT_byte_size */
    InfoULEB128( cli, byte_size );
    EndDIE( cli );
    StartChildren( cli );
    return( new_hdl );
}


void DWENTRY DWAddConstant(
    dw_client                   cli,
    dw_uconst                   value,
    const char *                name )
{
    StartDIE( cli, AB_ENUMERATOR );
    InfoULEB128( cli, value );
    InfoString( cli, name );
    EndDIE( cli );
}


void DWENTRY DWEndEnumeration(
    dw_client                   cli )
{
    EndChildren( cli );
}


dw_handle DWENTRY DWBeginSubroutineType(
    dw_client                   cli,
    dw_handle                   return_type,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        flags )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    start_scope = start_scope;
    _Validate( return_type != 0 );
    new_hdl = GetHandle( cli );
    abbrev = AB_SUBROUTINE_TYPE | AB_SIBLING;
    if( flags & DW_PTR_TYPE_MASK ) abbrev |= AB_ADDRESS_CLASS;
    emitCommonTypeInfo( cli, abbrev,
                        name, flags );
    if( abbrev & AB_ADDRESS_CLASS ) {
        WriteAddressClass( cli, flags );
    }
    /* AT_prototyped */
    Info8( cli, ( flags & DW_FLAG_PROTOTYPED ) != 0 );
    EmitTypeRef( cli, return_type );
    EndDIE( cli );
    StartChildren( cli );
    return( new_hdl );
}


dw_handle DWENTRY DWAddParmToSubroutineType(
    dw_client                   cli,
    dw_handle                   parm_type,
    dw_loc_handle               loc,
    dw_loc_handle               seg,
    const char *                name )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;
//TODO: change interface
    loc = loc;
    seg = seg;
    _Validate( parm_type != 0 );
    new_hdl = LabelNewHandle( cli );
    abbrev = AB_FORMAL_PARM_TYPE;
    if( name ) abbrev |= AB_NAME;
    StartDIE( cli, abbrev );
    /* AT_name */
    if( name ) {
        InfoString( cli, name );
    }
    EmitTypeRef( cli, parm_type );
    EndDIE( cli );
    return( new_hdl );
}


dw_handle DWENTRY DWAddEllipsisToSubroutineType(
    dw_client                   cli )
{
    dw_handle                   new_hdl;

    new_hdl = LabelNewHandle( cli );
    StartDIE( cli, AB_ELLIPSIS );
    EndDIE( cli );
    return( new_hdl );
}


void DWENTRY DWEndSubroutineType(
    dw_client                   cli )
{
    EndChildren( cli );
}


dw_handle DWENTRY DWString(
    dw_client                   cli,
    dw_loc_handle               string_length,
    dw_size_t                   byte_size,
    const char *                name,
    dw_addr_offset              start_scope,
    uint                        flags )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    start_scope = start_scope;
    new_hdl = GetHandle( cli );
    abbrev = string_length ? AB_STRING_WITH_LOC : AB_STRING;
    if( byte_size ) abbrev |= AB_BYTE_SIZE;
    emitCommonTypeInfo( cli, abbrev, name, flags );
    /* AT_byte_size */
    if( byte_size ) {
        InfoULEB128( cli, byte_size );
    }
    /* AT_string_length */
    if( string_length ) {
        EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_8), string_length );
    }
    EndDIE( cli );
    return( new_hdl );
}


dw_handle DWENTRY DWMemberPointer(
    dw_client                   cli,
    dw_handle                   struct_type,
    dw_loc_handle               loc,
    dw_handle                   base_type,
    const char *                name,
    unsigned                    flags )
{
    abbrev_code                 abbrev;
    dw_handle                   new_hdl;

    new_hdl = LabelNewHandle( cli );
    abbrev = AB_MEMBER_POINTER;
    if( name ) abbrev |= AB_NAME;
    if( flags & DW_PTR_TYPE_MASK ) abbrev |= AB_ADDRESS_CLASS;
    StartDIE( cli, abbrev );
    /* AT_name */
    if( name ) {
        InfoString( cli, name );
    }
    WriteAddressClass( cli, flags );
    /* AT_type */
    EmitTypeRef( cli, base_type );
    /* AT_containing_type */
    HandleReference( cli, struct_type, DW_DEBUG_INFO );
    /* AT_use_location */
    EmitLocExpr( cli, DW_DEBUG_INFO, sizeof( uint_16), loc );
    EndDIE( cli );
    return( new_hdl );
}

dw_handle DWENTRY DWBeginNameSpace(
    dw_client                   cli,
    const char *                name )
{
    dw_handle                   new_hdl;
    abbrev_code                 abbrev;

    new_hdl = LabelNewHandle( cli );
    abbrev = AB_NAMESPACE | AB_SIBLING | AB_START_REF;
    if( name ) abbrev |= AB_NAME;
    StartDIE( cli, abbrev );
    if( name ) {
    /* AT_name */
        InfoString( cli, name );
    }
    EndDIE( cli );
    StartChildren( cli );
    return( new_hdl );
}

void DWENTRY DWEndNameSpace(
    dw_client                   cli )
{
    EndChildren( cli );
    EndRef( cli );
}
