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


#ifndef RESMENU_INCLUDED
#define RESMENU_INCLUDED

typedef uint_16             MenuFlags;
#define MENU_GRAYED         0x0001
#define MENU_INACTIVE       0x0002
#define MENU_BITMAP         0x0004
#define MENU_CHECKED        0x0008
#define MENU_POPUP          0x0010
#define MENU_MENUBARBREAK   0x0020
#define MENU_MENUBREAK      0x0040
#define MENU_ENDMENU        0x0080
#define MENU_OWNERDRAWN     0x0100
#define MENU_SEPARATOR      0x0800
#define MENU_HELP           0x4000

#define MENUEX_POPUP        0x0001
#define MENUEX_SEPARATOR    0x0800

#define MENUEX_VERSION_SIG  0x0001

typedef struct MenuHeader {
    uint_16         Version;        /* currently 0 */
    uint_16         Size;           /* currently 0 */
} MenuHeader;

typedef struct MenuItemPopup {      /* if (ItemFlags & MENU_POPUP) */
    MenuFlags       ItemFlags;
    char            *ItemText;
} MenuItemPopup;

typedef struct MenuExItemPopup {
    uint_32         ItemId;
    uint_32         ItemType;
    uint_32         ItemState;
    uint_32         HelpId;
} MenuExItemPopup;

typedef struct MenuItemNormal {     /* if !(ItemFlags & MENU_POPUP) */
    MenuFlags       ItemFlags;
    uint_32         ItemID;         /* Note! This was changed from uint_16!! */
    char            *ItemText;
} MenuItemNormal;

typedef struct MenuExItemNormal {
    uint_32         ItemType;
    uint_32         ItemState;
} MenuExItemNormal;

typedef struct MenuItem {
    uint_8          IsPopup;
    union {
        MenuItemNormal  Normal;
        MenuItemPopup   Popup;
    } Item;
} MenuItem;

typedef struct MenuExItemNormalData {
    MenuItemNormal      Normal;
    MenuExItemNormal    ExData;
} MenuExItemNormalData;

typedef struct MenuExItemPopupData {
    MenuItemPopup       Popup;
    MenuExItemPopup     ExData;
} MenuExItemPopupData;

typedef struct MenuExItem {
    uint_8        IsPopup;
    union {
        MenuExItemNormalData    ExNormal;
        MenuExItemPopupData     ExPopup;
    } Item;
} MenuExItem;

/* reswmenu.c */

extern bool     ResWriteMenuHeader( MenuHeader *currhead, FILE *fp );
extern bool     ResWriteMenuItemNormal( const MenuItemNormal *curritem, bool use_unicode, FILE *fp );
extern bool     ResWriteMenuItemPopup( const MenuItemPopup *curritem, bool use_unicode, FILE *fp );
extern bool     ResWriteMenuItem( const MenuItem *curritem, bool use_unicode, FILE *fp );
extern bool     ResWriteMenuExHeader( MenuHeader *currhead, FILE *fp, uint_8 *data );
extern bool     ResWriteMenuExItemPopup( const MenuItemPopup *curritem, const MenuExItemPopup *exdata, bool use_unicode, FILE *fp );
extern bool     ResWriteMenuExItemNormal( const MenuItemNormal *, const MenuExItemNormal *, bool use_unicode, FILE *fp );

/* resrmenu.c */

extern bool     ResReadMenuHeader( MenuHeader *currhead, FILE *fp );
extern bool     ResReadMenuItem( MenuItem *curritem, FILE *fp );
extern bool     ResReadMenuItem32( MenuItem *curritem, FILE *fp );
extern bool     ResReadMenuExItem( MenuExItem *curritem, FILE *fp );
extern bool     ResReadMenuExtraBytes( MenuHeader *header, FILE *fp, char *buf );
extern bool     ResIsMenuEx( FILE *fp );
extern bool     ResIsHeaderMenuEx( MenuHeader *hdr );
extern MenuItem *ResNewMenuItem( void );
extern void     ResFreeMenuItem( MenuItem *olditem );

#endif
