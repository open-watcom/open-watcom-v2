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


#ifndef CANADDR_H
#define CANADDR_H   1
typedef struct addr_info    *addr_handle;

#include <stddef.h>
#include <watcom.h>
#include "segment.h"
#include "fixup.h"

#define CANA_NULL   ((addr_handle)0)

typedef struct addr_info {
    size_t      data_len;
    uint_8      data[6];            /* up to 48 bits of the original data */
    fixup       *fixup;             /* fixup data */
} addr_info;

void        CanAInit( void );
void        CanAFini( void );
addr_handle CanACreateHdl(
                seghdr *seg,
                uint_32 offset,
                size_t len
            );
void        CanADestroyHdl( addr_handle hdl );
addr_info   *CanAFind( addr_handle hdl );
#define     CanAFind( hdl )     ( (addr_info *)( hdl ) )

#endif
