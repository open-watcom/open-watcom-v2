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


#include "layer0.h"
#include "read.h"
#include "reserr.h"

WResIDName *WResReadWResIDName( WResFileID handle )
/*************************************************/
{
    WResIDName      newname;
    WResIDName      *newptr;
    WResFileSSize   numread;
    bool            error;

    /* read the size of the name in */
    error = ResReadUint8( &(newname.NumChars), handle );

    /* alloc the space for the new record */
    if( error ) {
        return( NULL );
    } else {
        /* -1 because one of the chars in the name is declared in the struct */
        newptr = WRESALLOC( sizeof( WResIDName ) + newname.NumChars - 1 );
    }

    /* read in the characters */
    if( newptr == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        newptr->NumChars = newname.NumChars;
        numread = WRESREAD( handle, newptr->Name, newptr->NumChars );
        if( numread != newptr->NumChars ) {
            WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
            WRESFREE( newptr );
            newptr = NULL;
        }
    }

    return( newptr );
} /* WResReadWResIDName */
