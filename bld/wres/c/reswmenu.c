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
/***************************************************/
{
    if( ResWriteUint16( fp, head->Version ) )
        return( true );
    if( ResWriteUint16( fp, head->Size ) )
        return( true );
    if( IS_MENUEX( *head ) ) {
        if( ResWriteUint32( fp, head->ExHelpId ) ) {
            return( true );
        }
    }
    return( false );
}

bool ResWriteMenuItem( const MenuItem *item, bool use_unicode, FILE *fp )
/***********************************************************************/
{
    if( item->IsOldWin ) {
        if( ResWriteUint8( fp, item->u.ItemFlags ) ) {
            return( true );
        }
    } else {
        if( ResWriteUint16( fp, item->u.ItemFlags ) ) {
            return( true );
        }
    }
    if( (item->u.ItemFlags & MENU_POPUP) == 0 ) {
        if( ResWriteUint16( fp, item->ItemId ) ) {
            return( true );
        }
    }
    return( ResWriteString( item->ItemText, use_unicode, fp ) );
}

bool ResWriteMenuItemEx( const MenuItem *item, FILE *fp )
/*******************************************************/
{
    if( ResWriteUint32( fp, item->ExType ) )
        return( true );
    if( ResWriteUint32( fp, item->ExState ) )
        return( true );
    if( ResWriteUint32( fp, item->ItemId ) )
        return( true );
    if( ResWriteUint16( fp, item->u.ExFlags ) )
        return( true );
    if( ResWriteString( item->ItemText, true, fp ) )
        return( true );
    if( ResWritePadDWord( fp ) )
        return( true );
    if( item->u.ExFlags & MENUEX_POPUP ) {
        if( ResWriteUint32( fp, item->ExHelpId ) ) {
            return( true );
        }
    }
    return( false );
}
