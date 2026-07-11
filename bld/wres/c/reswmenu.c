/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2026 The Open Watcom Contributors. All Rights Reserved.
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

bool ResWriteMenuHeader( MenuHeader *head, FILE *fp )
/*******************************************************/
{
    if( ResWriteUint16( head->Version, fp ) )
        return( true );
    if( ResWriteUint16( head->Size, fp ) )
        return( true );
    return( false );
}

bool ResWriteMenuExHeader( MenuHeader *head, FILE *fp, uint_8 *headerdata )
/*************************************************************************/
{
    if( ResWriteMenuHeader( head, fp ) )
        return( true );
    if( headerdata != NULL ) {
        if( WRESWRITE( fp, headerdata, head->Size ) != head->Size ) {
            return( WRES_ERROR( WRS_WRITE_FAILED ) );
        }
    }
    return( false );
}

bool ResWriteMenuItemPopup( const MenuItemPopup *item, bool use_unicode, FILE *fp )
/*********************************************************************************/
{
    if( item->ItemFlags & MENU_POPUP ) {
        if( ResWriteUint16( item->ItemFlags, fp ) )
            return( true );
        return( ResWriteString( item->ItemText, use_unicode, fp ) );
    }
    return( WRES_ERROR( WRS_BAD_PARAMETER ) );
}

bool ResWriteMenuItemPopupOldWin( const MenuItemPopup *item, bool use_unicode, FILE *fp )
/***************************************************************************************/
{
    if( item->ItemFlags & MENU_POPUP ) {
        if( ResWriteUint8( item->ItemFlags, fp ) )
            return( true );
        return( ResWriteString( item->ItemText, use_unicode, fp ) );
    }
    return( WRES_ERROR( WRS_BAD_PARAMETER ) );
}

bool ResWriteMenuExItemPopup( const MenuItemPopup *item, const MenuExItemPopup *exdata,
                             bool use_unicode, FILE *fp )
/*****************************************************************************************/
{
    bool        error;

    if( item->ItemFlags & MENUEX_POPUP ) {
        error = ResWriteUint32( exdata->ItemType, fp );
        if( !error ) {
            error = ResWriteUint32( exdata->ItemState, fp );
        }
        if( !error ) {
            error = ResWriteUint32( exdata->ItemId, fp );
        }
        if( !error ) {
            error = ResWriteUint16( item->ItemFlags, fp );
        }
        if( !error ) {
            error = ResWriteString( item->ItemText, use_unicode, fp );
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

bool ResWriteMenuItemNormal( const MenuItemNormal *item, bool use_unicode, FILE *fp )
/***********************************************************************************/
{
    bool        error;

    if( item->ItemFlags & MENU_POPUP )
        return( WRES_ERROR( WRS_BAD_PARAMETER ) );
    error = ResWriteUint16( item->ItemFlags, fp );
    if( !error )
        error = ResWriteUint16( (uint_16)item->ItemID, fp );
    if( !error )
        error = ResWriteString( item->ItemText, use_unicode, fp );
    return( error );
}

bool ResWriteMenuItemNormalOldWin( const MenuItemNormal *item, bool use_unicode, FILE *fp )
/*****************************************************************************************/
{
    bool        error;

    if( item->ItemFlags & MENU_POPUP )
        return( WRES_ERROR( WRS_BAD_PARAMETER ) );
    error = ResWriteUint8( item->ItemFlags, fp );
    if( !error )
        error = ResWriteUint16( (uint_16)item->ItemID, fp );
    if( !error )
        error = ResWriteString( item->ItemText, use_unicode, fp );
    return( error );
}

bool ResWriteMenuExItemNormal( const MenuItemNormal *item, const MenuExItemNormal *exdata,
                              bool use_unicode, FILE *fp )
/*******************************************************************************************/
{
    bool        error;

    if( item->ItemFlags & MENUEX_POPUP )
        return( WRES_ERROR( WRS_BAD_PARAMETER ) );
    error = ResWriteUint32( exdata->ItemType, fp );
    if( !error ) {
        error = ResWriteUint32( exdata->ItemState, fp );
    }
    if( !error ) {
        error = ResWriteUint32( item->ItemID, fp );
    }
    if( !error ) {
        error = ResWriteUint16( item->ItemFlags, fp );
    }
    if( !error ) {
        error = ResWriteString( item->ItemText, use_unicode, fp );
    }
    if( !error ) {
        error = ResWritePadDWord( fp );
    }
    return( error );
}

bool ResWriteMenuItem( const MenuItem *item, bool use_unicode, FILE *fp )
/***********************************************************************/
{
    bool    error;

    if( item->IsPopup ) {
        error = ResWriteMenuItemPopup( &(item->Item.Popup), use_unicode, fp );
    } else {
        error = ResWriteMenuItemNormal( &(item->Item.Normal), use_unicode, fp );
    }

    return( error );
}

bool ResWriteMenuItemOldWin( const MenuItem *item, bool use_unicode, FILE *fp )
/*****************************************************************************/
{
    bool    error;

    if( item->IsPopup ) {
        error = ResWriteMenuItemPopupOldWin( &(item->Item.Popup), use_unicode, fp );
    } else {
        error = ResWriteMenuItemNormalOldWin( &(item->Item.Normal), use_unicode, fp );
    }

    return( error );
}
