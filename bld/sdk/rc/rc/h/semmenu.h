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
* Description:  Semantic actions for processing menu resources.
*
****************************************************************************/


#ifndef SEMMENU_INCLUDED
#define SEMMENU_INCLUDED

#include "scan.h"

#include "os2res.h"

/**** Constant Definitions ****/

#define RES_HEADER_VERSION     1
#define RES_HEADER_SIZE        4

/**** Semantic structures ****/

typedef enum MenuItemType {
    MT_SEPARATOR,
    MT_MENU,
    MT_MENUEX,
    MT_MENUEX_NO_ID,
    MT_EITHER
} MenuItemType;

typedef struct MenuItemNormalData {
    MenuItemType        type;
    MenuItemNormal      menuData;
    MenuExItemNormal    menuExData;
} MenuItemNormalData;

typedef struct MenuItemPopupData {
    MenuItemType        type;
    MenuItemPopup       menuData;
    MenuExItemPopup     menuExData;
} MenuItemPopupData;

typedef struct FullMenu {
    struct FullMenuItem *       head;
    struct FullMenuItem *       tail;
} FullMenu;

typedef FullMenu * FullMenuPtr;

typedef struct FullMenuItemPopup {
    MenuItemPopupData       item;
    FullMenuPtr             submenu;
} FullMenuItemPopup;

typedef struct FullMenuItem {
    struct FullMenuItem *       next;
    struct FullMenuItem *       prev;
    int                         UseUnicode;
    int                         IsPopup;
    union {
        FullMenuItemPopup       popup;
        MenuItemNormalData      normal;
    } item;
} FullMenuItem;

typedef struct FullMenuOS2 {
    struct FullMenuItemOS2 *       head;
    struct FullMenuItemOS2 *       tail;
} FullMenuOS2;

typedef FullMenuOS2 * FullMenuPtrOS2;

typedef struct FullMenuItemOS2 {
    struct FullMenuItemOS2 *    next;
    struct FullMenuItemOS2 *    prev;
    MenuItemType                type;
    MenuItemOS2                 item;
    FullMenuPtrOS2              submenu;
} FullMenuItemOS2;

/**** Semantic routines ****/

MenuFlags SemAddFirstMenuOption( uint_8 token );
MenuFlags SemAddMenuOption( MenuFlags oldflags, uint_8 token );
FullMenu * SemNewMenu( FullMenuItem firstitem );
FullMenu * SemAddMenuItem( FullMenu * currmenu, FullMenuItem curritem );
void SemWriteMenu( WResID * name, ResMemFlags flags, FullMenu * menu, uint_16 );

MenuFlags SemOS2AddFirstMenuOption( uint_8 token );
MenuFlags SemOS2AddMenuOption( MenuFlags oldflags, uint_8 token );
FullMenuOS2 * SemOS2NewMenu( FullMenuItemOS2 firstitem );
FullMenuOS2 * SemOS2AddMenuItem( FullMenuOS2 * currmenu, FullMenuItemOS2 curritem );
void SemOS2WriteMenu( WResID * name, ResMemFlags flags, FullMenuOS2 * menu, uint_16, uint_32 codepage );

#endif
