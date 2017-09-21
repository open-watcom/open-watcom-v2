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
* Description:  Load resources from file. 
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "wresall.h"
#include "wresset2.h"
#include "wresall.h"
#include "reserr.h"
#include "wresrtns.h"
#include "wresdefn.h"
#include "layer2.h"
#include "wresset2.h"
#include "seekres.h"


int WResLoadResourceX( PHANDLE_INFO hinfo, lpcstr idType, lpcstr idResource,
                                    lpstr *lpszBuffer, size_t *bufferSize )
/*************************************************************************/
{
    WResID              *resource_type;
    WResID              *resource_id;
    int                 rc;

    if( IS_INTRESOURCE( idResource ) ) {
        resource_id = WResIDFromNum( (uint_16)RESOURCE2INT( idResource ) );
    } else {
        resource_id = WResIDFromStrF( idResource );
    }
    if( IS_INTRESOURCE( idType ) ) {
        resource_type = WResIDFromNum( (uint_16)RESOURCE2INT( idType ) );
    } else {
        resource_type = WResIDFromStrF( idType );
    }
    rc = WResLoadResource2( MainDir, hinfo, resource_type, resource_id, lpszBuffer, bufferSize );
    if( resource_type != NULL )
        WResIDFree( resource_type );
    if( resource_id != NULL )
        WResIDFree( resource_id );
    return( rc );
}
