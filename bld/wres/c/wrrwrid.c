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
#include <stddef.h>
#include "layer0.h"
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"


void *ResReadWResID( unsigned offs, FILE *fp, uint_16 ver )
/*********************************************************/
{
    WResID          *id;
    uint_8          isname;
    char            *ptr;
    size_t          numread;
    bool            error;

    if( (numread = WRESREAD( fp, &isname, sizeof( isname ) )) != sizeof( isname ) ) {
        WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
        return( NULL );
    }
    if( isname ) {
        isname = true;  /* normalize value to boolean type */
        ptr = ResReadWResIDName( offs + offsetof( WResID, ID ), fp, ver );
        if( ptr == NULL ) {
            return( NULL );
        }
    } else {
        ptr = WRESALLOC( offs + sizeof( WResID ) - 1 );
        if( ptr == NULL ) {
            WRES_ERROR( WRS_MALLOC_FAILED );
            return( NULL );
        }
    }
    id = (WResID *)( ptr + offs );
    if( !isname ) {
        error = false;
        id->ID.Num = ResReadUint16( &error, fp );
        if( error ) {
            WRESFREE( ptr );
            return( NULL );
        }
    }
    id->IsName = isname;
    return( ptr );
}

WResID *WResReadWResID( FILE *fp )
/********************************/
{
    return( ResReadWResID( 0, fp, WRESVERSION ) );
}

