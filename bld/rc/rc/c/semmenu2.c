/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 Menu related semantic actions.
*
****************************************************************************/


/*

The data structure for OS/2 MENU resources is as follows:

struct MenuHeader {
    ULONG  cbSize;          // Size of menu/submenu including this field
    USHORT idCodePage;      // Menu codepage
    USHORT idClass;         // Window class ID (always WC_MENU?)
    USHORT cItems;          // Number of menu items that follow
}

struct MenuItem {
    USHORT flStyle;         // Menu style flags
    USHORT fAttributes;     // Menu attribute flags
    USHORT cmd;             // Menu item ID
    CHAR   szItemName[];    // Null-terminated menu text (none for separator)
}

*/


#include "global.h"
#include "rcerrors.h"
#include "semantic.h"
#include "semantc2.h"
#include "layer0.h"
#include "rcrtns.h"
#include "reserr.h"
#include "rccore.h"


static bool ResOS2WriteMenuHeader( MenuHeaderOS2 *currhead, FILE *fp )
/********************************************************************/
{
    bool        error;

    error = ResWriteUint32( fp, currhead->Size );
    if( !error ) {
        error = ResWriteUint16( fp, currhead->Codepage );
    }
    if( !error ) {
        error = ResWriteUint16( fp, currhead->Class );
    }
    if( !error ) {
        error = ResWriteUint16( fp, currhead->NumItems );
    }
    return( error );
}

static bool ResOS2WriteMenuItemNormal( const MenuItemOS2 *item, FILE *fp )
/************************************************************************/
{
    bool        error;

    error = ResWriteUint16( fp, item->ItemStyle );
    if( !error ) {
        error = ResWriteUint16( fp, item->ItemAttrs );
    }
    if( !error ) {
        error = ResWriteUint16( fp, item->ItemCmd );
    }
    if( !error ) {
        if( (item->ItemStyle & OS2_MIS_SEPARATOR) == 0
          && item->ItemText != NULL ) {
            error = ResWriteString( item->ItemText, fp );
        }
    }

    return( error );
}

MenuFlags SemOS2AddMenuOption( MenuFlags oldflags, YYTOKENTYPE token )
/********************************************************************/
{
    switch( token ) {
    case Y_BITMAP:
        oldflags |= MENU_BITMAP;
        break;
    }

    return( oldflags );
}

FullMenuOS2 *SemOS2AddMenuItem( FullMenuOS2 *menu, FullMenuItemOS2 menuitem )
/***************************************************************************/
{
    FullMenuItemOS2     *new_menuitem;

    if( menu == NULL ) {
        menu = MemAllocSafe( sizeof( FullMenuOS2 ) );
        menu->head = NULL;
        menu->tail = NULL;
    }

    new_menuitem = MemAllocSafe( sizeof( FullMenuItemOS2 ) );

    *new_menuitem = menuitem;

    ResAddLLItemAtEnd( (void **)&(menu->head), (void **)&(menu->tail), new_menuitem );

    return( menu );
}

static bool SemOS2WriteMenuItem( FullMenuItemOS2 *menuitem, int *err_code )
/*********************************************************************/
{
    bool    error;

    error = ResOS2WriteMenuItemNormal( &(menuitem->item), CurrResFile.fp );
    *err_code = LastWresErr();
    return( error );
}

static size_t SemOS2CalcSubMenuSize( FullMenuOS2 *menu, unsigned *count )
/***********************************************************************/
{
    FullMenuItemOS2 *menuitem;
    size_t          size;
    unsigned        dummycount;

    size = MenuHeaderOS2_FILESIZE;

    if( menu == NULL ) {
        return( size );
    }

    for( menuitem = menu->head; menuitem != NULL; menuitem = menuitem->next ) {
        *count += 1;
        size += 3 * sizeof( uint_16 );
        if( (menuitem->item.ItemStyle & OS2_MIS_SEPARATOR) == 0
          && menuitem->item.ItemText != NULL )
            size += strlen( menuitem->item.ItemText ) + 1;
        if( menuitem->item.ItemStyle & OS2_MIS_SUBMENU ) {
            size += SemOS2CalcSubMenuSize( menuitem->submenu, &dummycount );
        }
    }
    return( size );
}


static bool SemOS2WriteSubMenu( FullMenuOS2 *menu, int *err_code, uint_32 codepage )
/**********************************************************************************/
{
    unsigned        count = 0;
    bool            error;
    FullMenuItemOS2 *menuitem;
    MenuHeaderOS2   head;

    if( ErrorHasOccured ) {
        return( false );
    }

    head.Size = SemOS2CalcSubMenuSize( menu, &count );
    head.Codepage = codepage;
    head.Class    = 4;
    head.NumItems = count;

    error = ResOS2WriteMenuHeader( &head, CurrResFile.fp );
    if( error ) {
        ErrorHasOccured = true;
        return( error );
    }

    if( menu == NULL ) {
        return( error );
    }

    error = false;
    for( menuitem = menu->head; menuitem != NULL && !error; menuitem = menuitem->next ) {
        if( !ErrorHasOccured ) {
            error = SemOS2WriteMenuItem( menuitem, err_code );
            if( !error
              && (menuitem->item.ItemStyle & OS2_MIS_SUBMENU) ) {
                error = SemOS2WriteSubMenu( menuitem->submenu, err_code, codepage );
            }
        }
    }

    if( error ) {
        ErrorHasOccured = true;
    }
    return( error );
}

static void SemOS2FreeSubMenu( FullMenuOS2 *menu )
/************************************************/
{
    FullMenuItemOS2   *menuitem;

    if( menu != NULL ) {
        while( (menuitem = menu->head) != NULL ) {
            menu->head = menuitem->next;
            if( menuitem->submenu != NULL )
                SemOS2FreeSubMenu( menuitem->submenu );
            if( menuitem->item.ItemText != NULL )
                MemFree( menuitem->item.ItemText );
            MemFree( menuitem );
        }
        MemFree( menu );
    }
}

void SemOS2WriteMenu( WResID *res_id, ResMemFlags res_flags, FullMenuOS2 *menu,
                   YYTOKENTYPE tokentype, uint_32 codepage )
/*****************************************************************************/
{
    ResLocation     loc;
    bool            error;
    int             err_code;

    /* unused parameters */ (void)tokentype;

    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();
        error = SemOS2WriteSubMenu( menu, &err_code, codepage );
        if( error ) {
            err_code = LastWresErr();
            RcError( RCMSG_WRITTING_RES, CurrResFile.filename, strerror( err_code ) );
            ErrorHasOccured = true;
        } else {
            loc.len = SemEndResource( loc.start );
            SemAddResourceAndFree( res_id, WResIDFromNum( OS2_RT_MENU ), res_flags, loc );
        }
    } else {
        MemFree( res_id );
    }
    SemOS2FreeSubMenu( menu );
}
