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


#include "mad.h"


extern void            InitMADInfo( void );
extern void            FiniMADInfo( void );
extern mad_type_handle GetMADTypeHandleDefaultAt( address a, mad_type_kind mtk );
extern void            GetMADTypeDefaultAt( address a, mad_type_kind mtk, mad_type_info *mti );
extern void            GetMADTypeDefault( mad_type_kind mtk, mad_type_info *mti );
extern void            ReportMADFailure( mad_status ms );
extern void            CheckMADChange( void );
extern size_t          GetMADNormalizedString( mad_string ms, char *buff, size_t buff_len );
extern size_t          GetMADTypeNameForCmd( mad_type_handle th, char *buff, size_t buff_len );
extern mad_type_handle FindMADTypeHandle( mad_type_kind tk, unsigned size );
extern dig_mad         FindMAD( const char *name, unsigned len );
