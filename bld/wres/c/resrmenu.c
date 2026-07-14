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

bool ResReadMenuHeader( MenuHeader *head, FILE *fp )
/**************************************************/
{
    bool            error;

    error = false;
    head->Version = ResReadUint16( &error, fp );
    if( !error )
        head->Size = ResReadUint16( &error, fp );
    return( error );
}

bool ResReadMenuExtraBytes( MenuHeader *header, char *buf, FILE *fp )
/*******************************************************************/
{
    unsigned        size;

    size = header->Size;
    if( buf != NULL ) {
        if( WRESREAD( fp, buf, size ) != size ) {
            return( true );
        }
    } else {
        WRESSEEK( fp, size, SEEK_CUR );
    }
    return( false );
}

// NB: Anyone using this function will have to manually seek back after
// calling ResIsMenuEx() (just as in ResIsDialogBoxEx()).
// If you've already read the header, just call ResIsHeaderMenuEx().
bool ResIsMenuEx( FILE *fp )
/**************************/
{
    MenuHeader               header;
    bool                     ret;

    ret = ResReadMenuHeader( &header, fp );
    if( !ret ) {
        if( header.Version == MENUEX_VERSION_SIG ) {
            return( true );
        }
    }

    return( false );
}

bool ResIsHeaderMenuEx( MenuHeader *hdr )
/***************************************/
{
    return( hdr->Version == MENUEX_VERSION_SIG );
}

bool ResReadMenuExItem( MenuExItem *item, FILE *fp )
/**************************************************/
{
    bool               error;
    uint_32            type;
    uint_32            state;
    uint_32            id;
    uint_16            resInfo;

    state = 0;
    id = 0;
    resInfo = 0;

    // Store first structure members in temporary variables until
    // we know whether or not the item is a MenuExItemNormal or a
    // MenuExItemPopup

    error = false;
    type = ResReadUint32( &error, fp );
    if( !error ) {
        state = ResReadUint32( &error, fp );
    }
    if( !error ) {
        id = ResReadUint32( &error, fp );
    }
    if( !error ) {
        resInfo = ResReadUint16( &error, fp );
    }

    // Determine if this is a normal menu item or a popup menu item

    if( resInfo & MENUEX_POPUP ) {
        item->IsPopup = true;
        item->Item.ExPopup.Popup.ItemFlags = resInfo;
        item->Item.ExPopup.ExData.ItemId = id;
        item->Item.ExPopup.ExData.ItemType = type;
        item->Item.ExPopup.ExData.ItemState = state;
        item->Item.ExPopup.Popup.ItemText = ResRead32String( fp, NULL );

        // Careful! The string is DWORD aligned.
        ResReadPadDWord( fp );
        item->Item.ExPopup.ExData.HelpId = ResReadUint32( &error, fp );
    } else {
        item->IsPopup = false;
        item->Item.ExNormal.Normal.ItemFlags = resInfo;
        item->Item.ExNormal.Normal.ItemID = id;
        item->Item.ExNormal.Normal.ItemText = ResRead32String( fp, NULL );

        // Careful! The string is DWORD aligned.
        ResReadPadDWord( fp );
        item->Item.ExNormal.ExData.ItemType = type;
        item->Item.ExNormal.ExData.ItemState = state;
    }

    return( error );
}

bool ResReadMenuItem( MenuItem *item, FILE *fp )
/**********************************************/
{
    bool    error;

    error = false;
    item->Item.Popup.ItemFlags = ResReadUint16( &error, fp );
    if( !error ) {
        if( item->Item.Popup.ItemFlags & MENU_POPUP ) {
            item->IsPopup = true;
            item->Item.Popup.ItemText = ResReadString( fp, NULL );
            error = (item->Item.Popup.ItemText == NULL);
        } else {
            item->IsPopup = false;
            item->Item.Normal.ItemID = ResReadUint16( &error, fp );
            if( !error ) {
                item->Item.Normal.ItemText = ResReadString( fp, NULL );
                error = (item->Item.Normal.ItemText == NULL);
            }
        }
    }

    return( error );
}

bool ResReadMenuItem32( MenuItem *item, FILE *fp )
/************************************************/
{
    bool    error;

    error = false;
    item->Item.Popup.ItemFlags = ResReadUint16( &error, fp );
    if( !error ) {
        if( item->Item.Popup.ItemFlags & MENU_POPUP ) {
            item->IsPopup = true;
            item->Item.Popup.ItemText = ResRead32String( fp, NULL );
            error = (item->Item.Popup.ItemText == NULL);
        } else {
            item->IsPopup = false;
            item->Item.Normal.ItemID = ResReadUint16( &error, fp );
            if( !error ) {
                item->Item.Normal.ItemText = ResRead32String( fp, NULL );
                error = (item->Item.Normal.ItemText == NULL);
            }
        }
    }

    return( error );
}

MenuItem * ResNewMenuItem( void )
/*******************************/
{
    MenuItem        *menuitem;

    menuitem = WRESALLOC( sizeof( MenuItem ) );
    if( menuitem == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        menuitem->IsPopup = false;
        menuitem->Item.Normal.ItemFlags = 0;
        menuitem->Item.Normal.ItemID = 0;
        menuitem->Item.Normal.ItemText = NULL;
    }

    return( menuitem );
}

void ResFreeMenuItem( MenuItem *menuitem )
/****************************************/
{
    if( menuitem->IsPopup ) {
        if( menuitem->Item.Popup.ItemText != NULL ) {
            WRESFREE( menuitem->Item.Popup.ItemText );
        }
    } else {
        if( menuitem->Item.Normal.ItemText != NULL ) {
            WRESFREE( menuitem->Item.Normal.ItemText );
        }
    }

    WRESFREE( menuitem );
}
