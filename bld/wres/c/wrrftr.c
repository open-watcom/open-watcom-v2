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

bool WResReadFixedTypeRecord( WResTypeInfo *newtype, WResFileID handle )
/**********************************************************************/
/* read the fixed part of a Type info record */
{
    WResFileSSize   numread;

    numread = WRESREAD( handle, newtype, sizeof( WResTypeInfo ) );
    if( numread == sizeof( WResTypeInfo ) ) {
        return( false );
    } else {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
    }
    return( true );
} /* WResReadFixedTypeRecord */

bool WResReadFixedTypeRecord2( WResTypeInfo *newtype, WResFileID handle )
/***********************************************************************/
/* read the fixed part of a Type info record for version 2 or before */
{
    WResFileSSize   numread;
    WResTypeInfo2   info;

    numread = WRESREAD( handle, &info, sizeof( WResTypeInfo2 ) );
    if( numread == sizeof( WResTypeInfo2 ) ) {
        newtype->NumResources = info.NumResources;
        newtype->TypeName.IsName = info.TypeName.IsName;
        if( newtype->TypeName.IsName ) {
            newtype->TypeName.ID.Name.Name[0] = info.TypeName.ID.Name.Name[0];
            newtype->TypeName.ID.Name.NumChars
                                        = info.TypeName.ID.Name.NumChars;
        } else {
            newtype->TypeName.ID.Num = info.TypeName.ID.Num;
        }
        return( false );
    } else {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
    }
    return( true );
} /* WResReadFixedTypeRecord */
