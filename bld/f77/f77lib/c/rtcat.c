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


//
// RTCAT        : character concatenation into temporary
//

#include "ftnstd.h"
#include "rtenv.h"

#include <string.h>


void    TCat( uint num_args, string *dest, ... ) {
//===============================================

// Perform character concatenation.

    uint        dest_len;
    uint        src_len;
    char        *dest_ptr;
    string      *src;
    va_list     parminfo;

    dest_len = 0;
    dest_ptr = dest->strptr;
    va_start( parminfo, dest );
    for( ; num_args > 0; --num_args ) {
        src = va_arg( parminfo, string * );
        src_len = src->len;
        memcpy( dest_ptr, src->strptr, src_len );
        dest_len += src_len;
        dest_ptr += src_len;
    }
    dest->len = dest_len;
    va_end( parminfo );
}
