/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "filefmt.h"
#include "write.h"
#include "read.h"
#include "resmenu.h"
#include "reserr.h"
#include "wresrtns.h"

bool ResWriteMenuHeader( MenuHeader *currhead, FILE *fp )
/*******************************************************/
{
    if( ResWriteUint16( currhead->Version, fp ) )
        return( true );
    if( ResWriteUint16( currhead->Size, fp ) )
        return( true );
    return( false );
}

bool ResWriteMenuExHeader( MenuHeader *currhead, FILE *fp, uint_8 *headerdata )
/*****************************************************************************/
{
    if( ResWriteMenuHeader( currhead, fp ) )
        return( true );
    if( headerdata != NULL ) {
        if( WRESWRITE( fp, headerdata, currhead->Size ) != currhead->Size ) {
            return( WRES_ERROR( WRS_WRITE_FAILED ) );
        }
    }
    return( false );
}

bool ResWriteMenuItemPopup( const MenuItemPopup *curritem, bool use_unicode, FILE *fp )
/*************************************************************************************/
{
    if( curritem->ItemFlags & MENU_POPUP ) {
        if( ResWriteUint16( curritem->ItemFlags, fp ) )
            return( true );
        return( ResWriteString( curritem->ItemText, use_unicode, fp ) );
    }
    return( WRES_ERROR( WRS_BAD_PARAMETER ) );
}

bool ResWriteMenuExItemPopup( const MenuItemPopup *curritem, const MenuExItemPopup *exdata,
                             bool use_unicode, FILE *fp )
/*****************************************************************************************/
{
    bool        error;

    if( curritem->ItemFlags & MENUEX_POPUP ) {
        error = ResWriteUint32( exdata->ItemType, fp );
        if( !error ) {
            error = ResWriteUint32( exdata->ItemState, fp );
        }
        if( !error ) {
            error = ResWriteUint32( exdata->ItemId, fp );
        }
        if( !error ) {
            error = ResWriteUint16( curritem->ItemFlags, fp );
        }
        if( !error ) {
            error = ResWriteString( curritem->ItemText, use_unicode, fp );
        }
        if( !error ) {
            error = ResWritePadDWord( fp );
        }
        if( !error ) {
            error = ResWriteUint32( exdata->HelpId, fp );
        }
        return( error );
    }
    return( WRES_ERROR( WRS_BAD_PARAMETER ) );
}

bool ResWriteMenuItemNormal( const MenuItemNormal *curritem, bool use_unicode, FILE *fp )
/***************************************************************************************/
{
    bool        error;

    if( curritem->ItemFlags & MENU_POPUP )
        return( WRES_ERROR( WRS_BAD_PARAMETER ) );
    error = ResWriteUint16( curritem->ItemFlags, fp );
    if( !error )
        error = ResWriteUint16( (uint_16)curritem->ItemID, fp );
    if( !error )
        error = ResWriteString( curritem->ItemText, use_unicode, fp );
    return( error );
}

bool ResWriteMenuExItemNormal( const MenuItemNormal *curritem, const MenuExItemNormal *exdata,
                              bool use_unicode, FILE *fp )
/*******************************************************************************************/
{
    bool        error;

    if( curritem->ItemFlags & MENUEX_POPUP )
        return( WRES_ERROR( WRS_BAD_PARAMETER ) );
    error = ResWriteUint32( exdata->ItemType, fp );
    if( !error ) {
        error = ResWriteUint32( exdata->ItemState, fp );
    }
    if( !error ) {
        error = ResWriteUint32( curritem->ItemID, fp );
    }
    if( !error ) {
        error = ResWriteUint16( curritem->ItemFlags, fp );
    }
    if( !error ) {
        error = ResWriteString( curritem->ItemText, use_unicode, fp );
    }
    if( !error ) {
        error = ResWritePadDWord( fp );
    }
    return( error );
}

bool ResWriteMenuItem( const MenuItem *curritem, bool use_unicode, FILE *fp )
/***************************************************************************/
{
    bool    error;

    if( curritem->IsPopup ) {
        error = ResWriteMenuItemPopup( &(curritem->Item.Popup), use_unicode, fp );
    } else {
        error = ResWriteMenuItemNormal( &(curritem->Item.Normal), use_unicode, fp );
    }

    return( error );
}
