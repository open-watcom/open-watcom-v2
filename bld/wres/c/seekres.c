/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Find resource and seek to begining.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "wresall.h"
#include "wresset2.h"
#include "wresall.h"
#include "reserr.h"
#include "wresrtns.h"
#include "wresdefn.h"
#include "layer2.h"
#include "seekres.h"


bool WResSeekResource2( WResDir dir, PHANDLE_INFO hinfo, WResID *type_id, WResID *res_id )
/****************************************************************************************/
{
    WResDirWindow       wind;
    WResLangInfo        *res;
    WResLangType        lang;

    if( ( type_id != NULL ) && ( res_id != NULL ) ) {
        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;

        wind = WResFindResource( type_id, res_id, dir, &lang );

        if( !WResIsEmptyWindow( wind ) ) {
            res = WResGetLangInfo( wind );
            if( !WRESSEEK( hinfo->fp, res->Offset, SEEK_SET ) ) {
                return( true );
            }
        }
    }
    return( false );
}

bool WResSeekResource( PHANDLE_INFO hinfo, unsigned int idType, unsigned int idResource )
/***************************************************************************************/
{
    WResID              type_id;
    WResID              res_id;

    WResInitIDFromNum( idResource, &res_id );
    WResInitIDFromNum( idType, &type_id );

    return( WResSeekResource2( MainDir, hinfo, &type_id, &res_id ) );
}

bool WResSeekResourceX( PHANDLE_INFO hinfo, lpcstr idType, lpcstr idResource )
/****************************************************************************/
{
    WResID              *type_id;
    WResID              *res_id;
    bool                rc;

    if( IS_INTRESOURCE( idResource ) ) {
        res_id = WResIDFromNum( (uint_16)RESOURCE2INT( idResource ) );
    } else {
        res_id = WResIDFromFarStr( idResource );
    }
    if( IS_INTRESOURCE( idType ) ) {
        type_id = WResIDFromNum( (uint_16)RESOURCE2INT( idType ) );
    } else {
        type_id = WResIDFromFarStr( idType );
    }
    rc = WResSeekResource2( MainDir, hinfo, type_id, res_id );
    if( type_id != NULL )
        WResIDFree( type_id );
    if( res_id != NULL )
        WResIDFree( res_id );
    return( rc );
}
