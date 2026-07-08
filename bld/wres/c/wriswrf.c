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


#include "layer0.h"
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"


WResResType WResReadResType( FILE *fp )
/*************************************/
{
    WResResType     res_type;
    uint_32         magic0;
    uint_32         magic1;
    bool            error;

    magic0 = 0;
    magic1 = 0;
    if( WRESSEEK( fp, 0, SEEK_SET ) ) {
        error = WRES_ERROR( WRS_SEEK_FAILED );
    } else {
        error = false;
        magic0 = ResReadUint32( &error, fp );
        if( !error ) {
            magic1 = ResReadUint32( &error, fp );
        }
        if( WRESSEEK( fp, 0, SEEK_SET ) ) {
            WRES_ERROR( WRS_SEEK_FAILED );
        }
    }

    res_type = RT_UNKNOWN;
    if( !error ) {
        if( magic0 == WRESMAGIC0
          && magic1 == WRESMAGIC1 ) {
            res_type = RT_WATCOM;
        } else if( magic0 == 0L ) {
            res_type = RT_WIN32;
        } else {
            res_type = RT_WIN16;
        }
    }
    return( res_type );
}
