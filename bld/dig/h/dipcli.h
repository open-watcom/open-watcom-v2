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


#ifndef _DIPCLI_H_INCLUDED
#define _DIPCLI_H_INCLUDED
#include "digpck.h"
#include "diptypes.h"
#include "digcli.h"

void            DIGCLIENT DIPCliImageUnload( mod_handle );

void            DIGCLIENT DIPCliMapAddr( addr_ptr *, void * );
sym_handle      *DIGCLIENT DIPCliSymCreate( void * );
dip_status      DIGCLIENT DIPCliItemLocation( location_context *, context_item, location_list * );
dip_status      DIGCLIENT DIPCliAssignLocation( location_list *, location_list *, unsigned long );
dip_status      DIGCLIENT DIPCliSameAddrSpace( address, address );
void            DIGCLIENT DIPCliAddrSection( address * );

void            DIGCLIENT DIPCliStatus( dip_status );

mad_handle      DIGCLIENT DIPCliCurrMAD(void);
#include "digunpck.h"
#endif
