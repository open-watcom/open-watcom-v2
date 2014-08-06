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
#include "layer0.h"
#include "read.h"
#include "reserr.h"

WResResInfo *WResReadResRecord( WResFileID handle )
/*************************************************/
/* reads in the fields of a res info record from the current position in */
/* the file identified by fp */
{
    WResResInfo     newres;
    WResResInfo     *newptr;
    WResFileSSize   numread;
    int             numcharsleft;
    int             error;

    error = WResReadFixedResRecord( &newres, handle );
    if (error) {
        return( NULL );
    }

    if (newres.ResName.IsName) {
        numcharsleft = newres.ResName.ID.Name.NumChars - 1;
    } else {
        numcharsleft = 0;
    }
    newptr = WRESALLOC( sizeof(WResResInfo) + numcharsleft );
    if( newptr == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        memcpy( newptr, &newres, sizeof(WResResInfo) );
        if( numcharsleft != 0 ) {
            numread = WRESREAD( handle, &(newptr->ResName.ID.Name.Name[1]), numcharsleft );
            if( numread != numcharsleft ) {
                WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
                WRESFREE( newptr );
                newptr = NULL;
            }
        }
    }

    return( newptr );
} /* WResReadResRecord */
