/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "layer0.h"
#include "util.h"
#include "reserr.h"
#include "wresrtns.h"
#include "wresset2.h"
#include "seekres.h"


WResID *WResIDFromFarStr( lpcstr str )
/*************************************
 * allocate an ID and fill it in
 */
{
    WResID          *id;
    size_t          len;

#if defined( _M_I86 )
    len = _fstrlen( str );
#else
    len = strlen( str );
    /* check the size of the string:  can it fit in two bytes? */
    if( len > 0xffff ) {
        WRES_ERROR( WRS_BAD_PARAMETER );
        return( NULL );
    }
#endif

    /* allocate the new ID */
    id = AllocWResIDName( offsetof( WResID, ID.Name ), len );
    if( id == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        id->IsName = true;
        id->ID.Name.NumChars = len;
#if defined( _M_I86 )
        _fmemcpy( id->ID.Name.Name, str, len );
#else
        memcpy( id->ID.Name.Name, str, len );
#endif
    }
    return( id );
} /* WResIDFromFarStr */
