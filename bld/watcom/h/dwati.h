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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


DWATI( sibling                  ,0x01 )
DWATI( location                 ,0x02 )
DWATI( name                     ,0x03 )
DWATI( ordering                 ,0x09 )
DWATI( byte_size                ,0x0b )
DWATI( bit_offset               ,0x0c )
DWATI( bit_size                 ,0x0d )
DWATI( stmt_list                ,0x10 )
DWATI( low_pc                   ,0x11 )
DWATI( high_pc                  ,0x12 )
DWATI( language                 ,0x13 )
DWATI( discr                    ,0x15 )
DWATI( discr_value              ,0x16 )
DWATI( visibility               ,0x17 )
DWATI( import                   ,0x18 )
DWATI( string_length            ,0x19 )
DWATI( common_reference         ,0x1a )
DWATI( comp_dir                 ,0x1b )
DWATI( const_value              ,0x1c )
DWATI( containing_type          ,0x1d )
DWATI( default_value            ,0x1e )
DWATI( inline                   ,0x20 )
DWATI( is_optional              ,0x21 )
DWATI( lower_bound              ,0x22 )
DWATI( producer                 ,0x25 )
DWATI( prototyped               ,0x27 )
DWATI( return_addr              ,0x2a )
DWATI( start_scope              ,0x2c )
DWATI( stride_size              ,0x2e )
DWATI( upper_bound              ,0x2f )
DWATI( abstract_origin          ,0x31 )
DWATI( accessibility            ,0x32 )
DWATI( address_class            ,0x33 )
DWATI( artificial               ,0x34 )
DWATI( base_types               ,0x35 )
DWATI( calling_convention       ,0x36 )
DWATI( count                    ,0x37 )
DWATI( data_member_location     ,0x38 )
DWATI( decl_column              ,0x39 )
DWATI( decl_file                ,0x3a )
DWATI( decl_line                ,0x3b )
DWATI( declaration              ,0x3c )
DWATI( discr_list               ,0x3d )
DWATI( encoding                 ,0x3e )
DWATI( external                 ,0x3f )
DWATI( frame_base               ,0x40 )
DWATI( friend                   ,0x41 )
DWATI( identifier_case          ,0x42 )
DWATI( macro_info               ,0x43 )
DWATI( namelist_item            ,0x44 )
DWATI( priority                 ,0x45 )
DWATI( segment                  ,0x46 )
DWATI( specification            ,0x47 )
DWATI( static_link              ,0x48 )
DWATI( type                     ,0x49 )
DWATI( use_location             ,0x4a )
DWATI( variable_parameter       ,0x4b )
DWATI( virtuality               ,0x4c )
DWATI( vtable_elem_location     ,0x4d )
DWATI( lo_user                  ,0x2000 )
DWATI( WATCOM_memory_model      ,0x2082 )
DWATI( WATCOM_references_start  ,0x2083 )
DWATI( WATCOM_parm_entry        ,0x2084 )
DWATI( hi_user                  ,0x3fff )
