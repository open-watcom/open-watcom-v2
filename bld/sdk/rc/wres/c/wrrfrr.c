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


#include "wresrtns.h"
#include "read.h"
#include "reserr.h"

/*
 * WResReadFixedResRecord - reads the fixed part of a Res info record for
 *                          the current version
 */
int WResReadFixedResRecord( WResResInfo * newres, WResFileID handle )
/*******************************************************************/
{
    int             numread;

    numread = (* WRESREAD) ( handle, newres, sizeof(WResResInfo) );
    if( numread != sizeof(WResResInfo) ) {
        WRES_ERROR( numread == -1 ? WRS_READ_FAILED:WRS_READ_INCOMPLETE );
        return( TRUE );
    } else {
        return( FALSE );
    }
} /* WResReadFixedResRecord */

/*
 * WResReadFixedResRecord1 - reads the fixed part of a Res info record for
 *                           versions 1 and below
 */
int WResReadFixedResRecord1( WResResInfo1 *newres, WResFileID handle )
/********************************************************************/
{
    int             numread;

    numread = (* WRESREAD) ( handle, newres, sizeof(WResResInfo1) );
    if( numread != sizeof(WResResInfo1) ) {
        WRES_ERROR( numread == -1 ? WRS_READ_FAILED:WRS_READ_INCOMPLETE );
        return( TRUE );
    } else {
        return( FALSE );
    }
}

/*
 * WResReadFixedResRecord2 - reads the fixed part of a Res info record for
 *                           version 2
 */
int WResReadFixedResRecord2( WResResInfo *newres, WResFileID handle )
/********************************************************************/
{
    int             numread;
    WResResInfo2    info;

    numread = (* WRESREAD) ( handle, &info, sizeof(WResResInfo2) );
    if( numread != sizeof( WResResInfo2 ) ) {
        WRES_ERROR( numread == -1 ? WRS_READ_FAILED:WRS_READ_INCOMPLETE );
        return( TRUE );
    } else {
        newres->NumResources = info.NumResources;
        newres->ResName.IsName = info.ResName.IsName;
        if( newres->ResName.IsName ) {
            newres->ResName.ID.Name.Name[0] = info.ResName.ID.Name.Name[0];
            newres->ResName.ID.Name.NumChars = info.ResName.ID.Name.NumChars;
        } else {
            newres->ResName.ID.Num = info.ResName.ID.Num;
        }
        return( FALSE );
    }
}
