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
#include "wresrtns.h"

WResID *WResReadWResID( WResFileID handle )
/*****************************************/
{
    WResID          newid;
    WResID          *newidptr;
    int             numread;
    int             extrabytes;     /* chars to be read beyond the fixed size */
    int             error;

    /* read in the fixed part of the record */
    error = WResReadFixedWResID( &newid, handle );
    if (error) {
        return( NULL );
    }

    if (newid.IsName) {
        extrabytes = newid.ID.Name.NumChars - 1;
    } else {
        extrabytes = 0;
    }

    newidptr = WRESALLOC( sizeof(WResID) + extrabytes );
    if (newidptr == NULL) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        memcpy( newidptr, &newid, sizeof(WResID) );
        if (extrabytes != 0) {
            numread = WRESREAD( handle, &(newidptr->ID.Name.Name[1]), extrabytes );
            if (numread != extrabytes) {
                WRES_ERROR( numread == -1 ? WRS_READ_FAILED:WRS_READ_INCOMPLETE );
                WRESFREE( newidptr );
                newidptr = NULL;
            }
        }
    }

    return( newidptr );
} /* WResReadWResID */
