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
* Description:  MAD client callback function prototypes.
*
****************************************************************************/


#ifndef MADCLI_H_INCLUDED
#define MADCLI_H_INCLUDED

#include "madtypes.h"
#include "digcli.h"

unsigned        DIGCLIENT MADCliReadMem( address, unsigned size, void *buff );
unsigned        DIGCLIENT MADCliWriteMem( address, unsigned size, const void *buff );

unsigned        DIGCLIENT MADCliString( mad_string, unsigned max, char *buff );
mad_status      DIGCLIENT MADCliAddString( mad_string, const char * );
unsigned        DIGCLIENT MADCliRadixPrefix( unsigned radix, unsigned max, char *buff );

void            DIGCLIENT MADCliNotify( mad_notify_type, const void * );


mad_status      DIGCLIENT MADCliAddrToString( address, mad_type_handle, mad_label_kind, unsigned max, char *buff );
mad_status      DIGCLIENT MADCliMemExpr( const char *start, unsigned len, unsigned radix, address * );

void            DIGCLIENT MADCliAddrSection( address * );
mad_status      DIGCLIENT MADCliAddrOvlReturn( address * );

system_config   *DIGCLIENT MADCliSystemConfig( void );

#endif
