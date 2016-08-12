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
* Description:  MAD client callback function prototypes.
*
****************************************************************************/


pick( size_t,          ReadMem,         ( address, size_t size, void *buff ) )
pick( size_t,          WriteMem,        ( address, size_t size, const void *buff ) )
pick( size_t,          String,          ( mad_string, char *buff, size_t buff_size ) )
pick( mad_status,      AddString,       ( mad_string, const char * ) )
pick( size_t,          RadixPrefix,     ( mad_radix radix, char *buff, size_t buff_size ) )
pick( void,            Notify,          ( mad_notify_type, const void * ) )
pick( mad_status,      AddrToString,    ( address, mad_type_handle, mad_label_kind, char *buff, size_t buff_size ) )
pick( mad_status,      MemExpr,         ( const char *expr, mad_radix radix, address * ) )
pick( void,            AddrSection,     ( address * ) )
pick( mad_status,      AddrOvlReturn,   ( address * ) )
pick( system_config *, SystemConfig,    ( void ) )
pick( mad_status,      TypeInfoForHost, ( mad_type_kind, int size, mad_type_info * ) )
pick( mad_status,      TypeConvert,     ( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg ) )
pick( mad_status,      TypeToString,    ( mad_radix radix, const mad_type_info *, const void *data, char *buff, size_t *buff_size_p ) )
