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


static void SemFreeSubMenu( FullMenuWIN *submenu );

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
    case Y_POPUP:
        oldflags |= MENU_POPUP;
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

FullMenuWIN *SemWINAddMenuItem( FullMenuWIN *currmenu, FullMenuItemWIN curritem )
/*******************************************************************************/
{
    FullMenuItemWIN *newitem;

    if( currmenu == NULL ) {
        currmenu = MemAllocSafe( sizeof( FullMenuWIN ) );
        currmenu->head = NULL;
        currmenu->tail = NULL;
    }

    newitem = MemAllocSafe( sizeof( FullMenuItemWIN ) );

    *newitem = curritem;

    ResAddLLItemAtEnd( (void **)&(currmenu->head), (void **)&(currmenu->tail), newitem );

    return( currmenu );
}

static void SemCheckMenuItemPopup( FullMenuItemWIN *item, bool is_menuex )
/************************************************************************/
{
    if( is_menuex ) {
        item->item.popup.item.menuData.ItemFlags = MENUEX_POPUP;
        if( item->item.popup.item.type == MT_MENU ) {
            RcError( ERR_MENU_POPUP_OPTIONS );
        }
    } else {
        if( item->item.popup.item.type == MT_MENUEX ) {
            RcError( ERR_MENUEX_POPUP_OPTIONS );
        }
    }
}

static void SemCheckMenuItemNormal( FullMenuItemWIN *item, bool is_menuex )
/*************************************************************************/
{
    if( is_menuex ) {
        if( item->item.normal.type == MT_MENU ) {
            RcError( ERR_MENU_NORMAL_OPTIONS );
        }
    } else {
        if( item->item.normal.type == MT_MENUEX ) {
            RcError( ERR_MENUEX_NORMAL_OPTIONS );
        } else if( item->item.normal.type == MT_MENUEX_NO_ID ) {
            RcError( ERR_MISSING_MENUITEM_ID );
        }

    }
}

static bool SemWriteMenuItem( FullMenuItemWIN *item, int islastitem,
                          int *err_code, bool is_menuex )
/******************************************************************/
{
    bool    error;

    error = false;
    if( item->IsPopup ) {
        SemCheckMenuItemPopup( item, is_menuex );
        if( islastitem ) {
            item->item.popup.item.menuData.ItemFlags |= MENU_LAST_ITEM;
        }
        if( is_menuex ) {
            error = ResWriteMenuExItemPopup( &(item->item.popup.item.menuData),
                      &(item->item.popup.item.menuExData), item->iswin32,
                      CurrResFile.fp );
        } else {
            if( CmdLineParms.winver < 30 ) {
                error = ResWriteMenuItemPopupOldWin( &(item->item.popup.item.menuData),
                            item->iswin32, CurrResFile.fp );
            } else {
                error = ResWriteMenuItemPopup( &(item->item.popup.item.menuData),
                            item->iswin32, CurrResFile.fp );
            }
        }
    } else {
        SemCheckMenuItemNormal( item, is_menuex );
        if( islastitem ) {
            item->item.normal.menuData.ItemFlags |= MENU_LAST_ITEM;
        }
        if( is_menuex ) {
            error = ResWriteMenuExItemNormal( &(item->item.normal.menuData),
                         &(item->item.normal.menuExData), item->iswin32,
                         CurrResFile.fp );
        } else {
            if( CmdLineParms.winver < 30 ) {
                error = ResWriteMenuItemNormalOldWin( &(item->item.normal.menuData),
                            item->iswin32, CurrResFile.fp );
            } else {
                error = ResWriteMenuItemNormal( &(item->item.normal.menuData),
                            item->iswin32, CurrResFile.fp );
            }
        }
    }
    *err_code = LastWresErr();
    return( error );
}

static bool SemWriteSubMenu( FullMenuWIN *submenu, int *err_code, bool is_menuex )
/********************************************************************************/
{
    bool            error;
    int             islastitem;
    FullMenuItemWIN *curritem;

    error = false;

    if( ErrorHasOccured ) {
        return( false );
    }

    for( curritem = submenu->head; curritem != NULL && !error; curritem = curritem->next ) {
        islastitem = (curritem == submenu->tail);
        if( !ErrorHasOccured ) {
            error = SemWriteMenuItem( curritem, islastitem, err_code, is_menuex );
            if( !error
              && curritem->IsPopup ) {
                error = SemWriteSubMenu( curritem->item.popup.submenu, err_code, is_menuex );
            }
        }
    }

    if( error ) {
        ErrorHasOccured = true;
    }
    return( error );
}

static void SemWarnIfSubmenus( FullMenuWIN *submenu )
/****************************************************
 * Windows 2.x does not support submenus, though submenus are parsed
 * correctly.  So it is valid to have top level MF_POPUP, but MF_POPUP
 * within the menus is ignored.
 */
{
    FullMenuItemWIN *curritem,*currsubitem;

    for( curritem = submenu->head; curritem != NULL; curritem = curritem->next ) { // top level menu bar
        if( curritem->IsPopup ) {
            for( currsubitem = curritem->item.popup.submenu->head; currsubitem != NULL; currsubitem = currsubitem->next ) { // menu appearing below menu bar
                if( currsubitem->IsPopup ) {
                    RcWarning( WARN_SUBMEN_WIN2X );
                }
            }
        }
    }
}

static void SemFreeMenuItem( FullMenuItemWIN *curritem )
/******************************************************/
{
    if( curritem->IsPopup ) {
        SemFreeSubMenu( curritem->item.popup.submenu );
        if( curritem->item.popup.item.menuData.ItemText != NULL ) {
            MemFree( curritem->item.popup.item.menuData.ItemText );
        }
    } else {
        if( curritem->item.normal.menuData.ItemText != NULL ) {
            MemFree( curritem->item.normal.menuData.ItemText );
        }
    }
}

static void SemFreeSubMenu( FullMenuWIN *submenu )
/************************************************/
{
    FullMenuItemWIN *curritem;
    FullMenuItemWIN *nextitem;

    for( curritem = submenu->head; curritem != NULL; curritem = nextitem ) {
        nextitem = curritem->next;
        SemFreeMenuItem( curritem );
        MemFree( curritem );
    }
    MemFree( submenu );
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
            head.Size = sizeof( head.ExHelpID );
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
            error = SemWriteSubMenu( menu, &err_code, is_menuex );
        }
        if( !error
          && CmdLineParms.MSResFormat
          && CmdLineParms.iswin32 ) {
            error = ResWritePadDWord( CurrResFile.fp );
        }
        if( error ) {
            RcError( ERR_WRITTING_RES, CurrResFile.filename, strerror( err_code ) );
            ErrorHasOccured = true;
        } else {
            loc.len = SemEndResource( loc.start );
            SemAddResourceAndFree( res_id, WResIDFromNum( RESOURCE2INT( RT_MENU ) ), res_flags, loc );
        }
    } else {
        MemFree( res_id );
    }
    SemFreeSubMenu( menu );
}

