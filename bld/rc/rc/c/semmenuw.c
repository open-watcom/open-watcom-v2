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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*

The data structure for MENUEX's is as follows:


struct MenuExHeader {
    WORD   wVersion;          // Possibly number of different control
    WORD   wBytesToFollow;    // This number is 04 00 for Menus
    WCHAR  data[];
}

struct NormalMenuExItem {
    DWORD  dwType;
    DWORD  dwState;
    DWORD  dwId;
    WORD   fItemFlags;
    WCHAR  szItemText[];    // DWORD Aligned
}

struct PopupMenuExItem {
    DWORD  dwType;
    DWORD  dwState;
    DWORD  dwId;
    WORD   fItemFlags;
    WCHAR  szItemText[];   // DWORD Aligned
    DWORD  dwHelpId;
}

*/


#include "global.h"
#include "rcerrors.h"
#include "semantic.h"
#include "semantcw.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "rccore.h"


MenuFlags SemWINAddMenuOption( MenuFlags oldflags, YYTOKENTYPE token )
/********************************************************************/
{
    switch( token ) {
    case Y_GRAYED:
        oldflags |= MENU_GRAYED;
        break;
    case Y_INACTIVE:
        oldflags |= MENU_INACTIVE;
        break;
    case Y_BITMAP:
        oldflags |= MENU_BITMAP;
        break;
    case Y_CHECKED:
        oldflags |= MENU_CHECKED;
        break;
    case Y_MENUBARBREAK:
        oldflags |= MENU_MENUBARBREAK;
        break;
    case Y_MENUBREAK:
        oldflags |= MENU_MENUBREAK;
        break;
    case Y_OWNERDRAW:
        if( CmdLineParms.winver > 20 ) {
            oldflags |= MENU_OWNERDRAWN;
        }
        break;
    case Y_HELP:
        oldflags |= MENU_HELP;
        break;
    }

    return( oldflags );
}

FullMenuWIN *SemWINAddMenuItem( FullMenuWIN *menu, FullMenuItemWIN menuitem )
/***************************************************************************/
{
    FullMenuItemWIN *new_menuitem;

    if( menu == NULL ) {
        menu = MemAllocSafe( sizeof( FullMenuWIN ) );
        menu->head = NULL;
        menu->tail = NULL;
    }

    new_menuitem = MemAllocSafe( sizeof( FullMenuItemWIN ) );

    *new_menuitem = menuitem;

    ResAddLLItemAtEnd( (void **)&(menu->head), (void **)&(menu->tail), new_menuitem );

    return( menu );
}

static bool SemWINWriteMenuItem( FullMenuItemWIN *menuitem, int islastitem, int *err_code, bool is_menuex )
/*********************************************************************************************************/
{
    bool    error;

    error = false;
    if( islastitem ) {
        if( is_menuex ) {
            menuitem->item.u.ExFlags |= MENU_LAST_ITEM;
        } else {
            menuitem->item.u.ItemFlags |= MENU_LAST_ITEM;
        }
    }
    if( is_menuex ) {
        error = ResWriteMenuItemEx( &(menuitem->item), CurrResFile.fp );
    } else {
        error = ResWriteMenuItem( &(menuitem->item), menuitem->iswin32, CurrResFile.fp );
    }
    *err_code = LastWresErr();
    return( error );
}

static bool SemWINWriteSubMenu( FullMenuWIN *menu, int *err_code, bool is_menuex )
/********************************************************************************/
{
    bool            error;
    FullMenuItemWIN *menuitem;

    error = false;

    if( ErrorHasOccured ) {
        return( false );
    }

    for( menuitem = menu->head; menuitem != NULL && !error; menuitem = menuitem->next ) {
        if( !ErrorHasOccured ) {
            error = SemWINWriteMenuItem( menuitem, ( menuitem == menu->tail ), err_code, is_menuex );
            if( !error
              && menuitem->submenu != NULL ) {
                error = SemWINWriteSubMenu( menuitem->submenu, err_code, is_menuex );
            }
        }
    }

    if( error ) {
        ErrorHasOccured = true;
    }
    return( error );
}

static void SemWarnIfSubmenus( FullMenuWIN *menu )
/*************************************************
 * Windows 2.x does not support submenus, though submenus are parsed
 * correctly.  So it is valid to have top level MF_POPUP, but MF_POPUP
 * within the menus is ignored.
 */
{
    FullMenuItemWIN *menuitem;
    FullMenuItemWIN *submenuitem;

    for( menuitem = menu->head; menuitem != NULL; menuitem = menuitem->next ) { // top level menu bar
        if( menuitem->submenu != NULL ) {
            for( submenuitem = menuitem->submenu->head; submenuitem != NULL; submenuitem = submenuitem->next ) { // menu appearing below menu bar
                if( submenuitem->submenu != NULL ) {
                    RcWarning( RCMSG_SUBMEN_WIN2X );
                }
            }
        }
    }
}

static void SemWINFreeSubMenu( FullMenuWIN *menu )
/************************************************/
{
    FullMenuItemWIN *menuitem;

    while( (menuitem = menu->head) != NULL ) {
        menu->head = menuitem->next;
        if( menuitem->submenu != NULL )
            SemWINFreeSubMenu( menuitem->submenu );
        if( menuitem->item.ItemText != NULL )
            MemFree( menuitem->item.ItemText );
        MemFree( menuitem );
    }
    MemFree( menu );
}

void SemWINWriteMenu( WResID *res_id, ResMemFlags res_flags, FullMenuWIN *menu, bool is_menuex )
/**********************************************************************************************/
{
    MenuHeader      head;
    ResLocation     loc;
    bool            error;
    int             err_code;

    error = false;
    if( !ErrorHasOccured ) {
        memset( &head, 0, sizeof( head ) );
        if( is_menuex ) {
            head.Version = MENUEX_VERSION_SIG;
            head.Size = sizeof( head.ExHelpId );
            ResWritePadDWord( CurrResFile.fp );
        }
        loc.start = SemStartResource();
        /* Windows 2.x menus do not have a header */
        if( CmdLineParms.winver > 20 ) {
            error = ResWriteMenuHeader( &head, CurrResFile.fp );
        }
        if( error ) {
            err_code = LastWresErr();
        } else {
            if( CmdLineParms.winver < 30 ) {
                SemWarnIfSubmenus( menu );
            }
            error = SemWINWriteSubMenu( menu, &err_code, is_menuex );
        }
        if( !error
          && CmdLineParms.MSResFormat
          && CmdLineParms.iswin32 ) {
            error = ResWritePadDWord( CurrResFile.fp );
        }
        if( error ) {
            RcError( RCMSG_WRITTING_RES, CurrResFile.filename, strerror( err_code ) );
            ErrorHasOccured = true;
        } else {
            loc.len = SemEndResource( loc.start );
            SemAddResourceAndFree( res_id, WResIDFromNum( RESOURCE2INT( RT_MENU ) ), res_flags, loc );
        }
    } else {
        MemFree( res_id );
    }
    SemWINFreeSubMenu( menu );
}

