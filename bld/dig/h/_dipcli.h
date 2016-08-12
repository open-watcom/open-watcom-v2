/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DIP client callback function prototypes.
*
****************************************************************************/


pick( void,             ImageUnload,    ( mod_handle ) )
pick( void,             MapAddr,        ( addr_ptr *, void * ) )
pick( imp_sym_handle *, SymCreate,      ( imp_image_handle *, void * ) )
pick( dip_status,       ItemLocation,   ( location_context *, context_item, location_list * ) )
pick( dip_status,       AssignLocation, ( location_list *, location_list *, unsigned long ) )
pick( dip_status,       SameAddrSpace,  ( address, address ) )
pick( void,             AddrSection,    ( address * ) )
pick( void,             Status,         ( dip_status ) )
pick( dig_mad,          CurrMAD,        ( void ) )
