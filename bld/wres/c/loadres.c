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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "wresall.h"
#include "walloca.h"
#include "wresset2.h"
#include "loadstr.h"
#include "wresrtns.h"

extern WResDir    MainDir;

static int GetResource( WResLangInfo *res, PHANDLE_INFO hInstance, char *res_buffer )
/***********************************************************************************/
{
    if( WRESSEEK( hInstance->handle, res->Offset, SEEK_SET ) == -1 )
        return( -1 );
    WRESREAD( hInstance->handle, res_buffer, res->Length );
    return( 0 );
}

int WResLoadResource2( WResDir dir, PHANDLE_INFO hInstance, UINT idType,
                       UINT idResource, LPSTR *lpszBuffer, int *bufferSize )
/**************************************************************************/
{
    int                 retcode;
    WResID              resource_type;
    WResID              resource_id;
    WResDirWindow       wind;
    WResLangInfo        *res;
    WResLangType        lang;
    char                *res_buffer;

    if( ( lpszBuffer == NULL ) || ( bufferSize == NULL ) ) {
        return( -1 );
    }

    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    WResInitIDFromNum( idResource, &resource_id );
    WResInitIDFromNum( (long)idType, &resource_type );

    wind = WResFindResource( &resource_type, &resource_id, dir, &lang );

    if( WResIsEmptyWindow( wind ) ) {
        retcode = -1;
    } else {
        res = WResGetLangInfo( wind );
        // lets make sure we dont perturb malloc into apoplectic fits
        if( res->Length >= INT_MAX ) {
            return( -1 );
        }
        res_buffer  = WRESALLOC( res->Length );
        *lpszBuffer = res_buffer;
        if( *lpszBuffer == NULL ) {
            return( -1 );
        }
        *bufferSize = (int)res->Length;
        retcode = GetResource( res, hInstance, res_buffer );
    }

    return( retcode );
}

int WResLoadResource( PHANDLE_INFO hInstance, UINT idType, UINT idResource,
                                    LPSTR *lpszBuffer, int *bufferSize )
/*************************************************************************/
{
    return( WResLoadResource2( MainDir, hInstance, idType, idResource, lpszBuffer, bufferSize ) );
}
