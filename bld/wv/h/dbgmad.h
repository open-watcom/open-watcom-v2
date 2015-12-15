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
* Description:  Debugger lexical scanner.
*
****************************************************************************/


#include "mad.h"

extern mad_type_handle  ScanType( mad_type_kind tk, mad_type_kind *tkr );
extern mad_string       ScanCall( void );
extern unsigned         GetMADMaxFormatWidth( mad_type_handle th );
extern void             RegFindData( mad_type_kind kind, const mad_reg_set_data **pdata );
#ifdef DEAD_CODE
extern void             CnvAddrToItem( address *a, item_mach *item, mad_type_info *mti );
#endif
extern char             *AddrTypeToString( address *a, mad_type_handle th, char *buff, size_t buff_len );
extern char             *AddrToString( address *a, mad_address_format af, char *buff, size_t buff_len );
