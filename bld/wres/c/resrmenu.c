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
    long            offset;

    error = false;
    head->Version = ResReadUint16( &error, fp );
    if( error )
        return( true );
    offset = head->Size = ResReadUint16( &error, fp );
    if( error )
        return( true );
    if( IS_MENUEX( *head ) ) {
        head->ExHelpId = ResReadUint32( &error, fp );
        if( error ) {
            return( true );
        }
        offset -= sizeof( uint_32 );
    }
    if( offset ) {
        if( WRESSEEK( fp, offset, SEEK_CUR ) ) {
            return( true );
        }
    }
    return( false );
}

bool ResReadMenuItemEx( MenuItem *item, FILE *fp )
/************************************************/
{
    bool               error;

    error = false;
    item->ExType = ResReadUint32( &error, fp );
    if( error )
        return( true );
    item->ExState = ResReadUint32( &error, fp );
    if( error )
        return( true );
    item->ItemId = ResReadUint32( &error, fp );
    if( error )
        return( true );
    item->u.ExFlags = ResReadUint16( &error, fp );
    if( error )
        return( true );
    item->ItemText = ResRead32String( fp, NULL );
    if( error )
        return( true );
    if( ResReadPadDWord( fp ) )
        return( true );
    if( item->u.ExFlags & MENUEX_POPUP ) {
        item->ExHelpId = ResReadUint32( &error, fp );
        if( error ) {
            return( true );
        }
    }
    return( false );
}

bool ResReadMenuItem( MenuItem *item, bool use_unicode, FILE *fp )
/****************************************************************/
{
    bool    error;

    error = false;
    if( item->IsOldWin ) {
        item->u.ItemFlags = ResReadUint8( &error, fp );
    } else {
        item->u.ItemFlags = ResReadUint16( &error, fp );
    }
    if( error )
        return( true );
    if( (item->u.ItemFlags & MENU_POPUP) == 0 ) {
        item->ItemId = ResReadUint16( &error, fp );
    }
    if( use_unicode ) {
        item->ItemText = ResRead32String( fp, NULL );
    } else {
        item->ItemText = ResReadString( fp, NULL );
    }
    return( item->ItemText == NULL );
}

MenuItem *ResNewMenuItem( void )
/******************************/
{
    MenuItem        *menuitem;

    menuitem = WRESALLOC( sizeof( MenuItem ) );
    if( menuitem == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        memset( menuitem, 0, sizeof( MenuItem ) );
    }
    return( menuitem );
}

void ResFreeMenuItem( MenuItem *menuitem )
/****************************************/
{
    if( menuitem->IsPopup ) {
        if( menuitem->ItemText != NULL ) {
            WRESFREE( menuitem->ItemText );
        }
    } else {
        if( menuitem->ItemText != NULL ) {
            WRESFREE( menuitem->ItemText );
        }
    }

    WRESFREE( menuitem );
}

