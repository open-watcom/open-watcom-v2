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


#include <string.h>
#include "womp.h"
#include "canaddr.h"
#include "segment.h"
#include "myassert.h"
#include "memutil.h"

void CanAInit( void ) {
/*******************/
}

void CanAFini( void ) {
/*******************/
}

addr_handle CanACreateHdl( seghdr *seg, uint_32 offset, size_t len ) {
/******************************************************************/
    addr_handle new;

/**/myassert( seg != NULL );
/**/myassert( seg->data != NULL );
/**/myassert( offset + len < seg->alloc );
    new = MemAlloc( sizeof( struct addr_info ) );
    new->data_len = len;
    memcpy( new->data, seg->data + offset, len );
    new->fixup = FixDup( SegFindFix( seg, offset ) );
    return( new );
}

void CanADestroyHdl( addr_handle hdl ) {
/************************************/
    if( hdl == CANA_NULL ) {
        return;
    }
    if( hdl->fixup ) {
        FixKill( hdl->fixup );
    }
    MemFree( hdl );
}
