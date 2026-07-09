/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
#include <limits.h>
#include "layer0.h"
#include "util.h"
#include "reserr.h"
#include "wresrtns.h"


WResIDName *WResIDNameFromStr( const char *str )
/**********************************************/
{
    WResIDName      *name_id;
    size_t          len;

    len = strlen( str );
#if !defined( _M_I86 )
    if( len > USHRT_MAX ) {
        /* truncate the str if it is more that USHRT_MAX in length */
        len = USHRT_MAX;
    }
#endif
    name_id = AllocWResIDName( 0, len );
    if( name_id == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        name_id->NumChars = len;
        /* don't copy the '\0' */
        memcpy( name_id->Name, str, len );
    }

    return( name_id );
}
