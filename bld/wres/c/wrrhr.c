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
#include "wresrtns.h"

bool WResReadHeaderRecord( WResHeader *header, WResFileID handle )
/****************************************************************/
{
    bool            error;
    WResFileSSize   numread;

    error = ( WRESSEEK( handle, 0, SEEK_SET ) == -1 );
    if( error ) {
        WRES_ERROR( WRS_SEEK_FAILED );
    } else {
        numread = WRESREAD( handle, header, sizeof( WResHeader ) );
        if( numread != sizeof( WResHeader ) ) {
            WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
            error = true;
        } else {
            error = ( WRESSEEK( handle, 0, SEEK_SET ) == -1 );
            if( error ) {
                WRES_ERROR( WRS_SEEK_FAILED );
            }
        }
    }
    return( error );
}

bool WResReadExtHeader( WResExtHeader *head, WResFileID handle )
/**************************************************************/
{
    WResFileSSize   numread;

    numread = WRESREAD( handle, head, sizeof(WResExtHeader) );
    if( numread != sizeof(WResExtHeader) ) {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
        return( true );
    } else {
        return( false );
    }
}
