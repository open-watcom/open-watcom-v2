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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _UIMENU_H_
#define _UIMENU_H_
#define         MAX_MENUS               15

#define MENU_TAB_OFFSET 0x00ffU
#define MENU_HAS_TAB    0x0100U
#define MENU_HAS_CHECK  0x0200U
#define MENU_HAS_POPUP  0x0400U

#define TAB_OFFSET( desc )      ((desc)->flags & MENU_TAB_OFFSET)
#define MENU_GET_ROW( desc )    ((desc)->area.row - 1)
#define MENU_SET_ROW( desc, r )

typedef struct describemenu {
        SAREA           area;           /* area of menu         */
        ORD             titlecol;       /* column of title      */
        ORD             titlewidth;     /* width of title       */
        unsigned short  flags;
} DESCMENU;

#define ITEM_CHAR_OFFSET            0x00ff
#define ITEM_GRAYED                 0x0100
#define ITEM_CHECKED                0x0200
#define ITEM_SEPARATOR              0x0400

#define CHAROFFSET( menu )    ( (menu).flags & ITEM_CHAR_OFFSET )
#define MENUGRAYED( menu )    ( (menu).flags & ITEM_GRAYED )
#define MENUSEPARATOR( menu ) ( ( (menu).flags & ITEM_SEPARATOR ) || (*((menu).name) == '\0' ) )
#define MENUENDMARKER( menu ) ( ( (menu).name == NULL ) && !( (menu).flags & ITEM_SEPARATOR ) )

typedef struct menuitem {
        char*           name;           /* name of item         */
        EVENT           event;          /* item event           */
        unsigned short  flags;          /* char offset, grayed  */
        struct menuitem *popup;         /* popup from this menu */
} MENUITEM;

/* the titles and items fields must be initialized by the       */
/* application - all other fields are for looking at only       */

typedef struct vbarmenu {
        MENUITEM*       titles;         /* titles for pull down menus       */
        EVENT           event;          /* current menu item event          */
        unsigned        inlist:1;       /* selection will lead to the event */
        unsigned        active:1;       /* the user is browsing the menus   */
        unsigned        draginmenu:1;   /* drag operation began in menus    */
        unsigned        indicators:1;   /* keyboard indicators              */
        unsigned        scroll:1;       /* scroll indicator                 */
        unsigned        caps:1;         /* caps indicator                   */
        unsigned        num:1;          /* num indicator                    */
        unsigned        altpressed:1;   /* alt key has been pressed not rel */
        unsigned        movedmenu:1;    /* have dragged to diff item or menu*/
        unsigned        ignorealt:1;    /* ignore 1 alt press               */
        unsigned        popuppending:1; /* need to open popup               */
        unsigned        disabled:1;     /* menu has been disabled           */
        signed char     menu;           /* current menu number (base 1)     */
} VBARMENU;

#ifdef __cplusplus
    extern "C" {
#endif

extern          char            uialtchar( EVENT );
extern          void            uisetmenudesc( void );
extern          VBARMENU*       uimenubar( VBARMENU * );
extern          void            uimenuindicators( bool );
extern          void            uimenus( MENUITEM *, MENUITEM **, EVENT );
extern          bool            uimenuson( void );
extern          void            uimenudisable( bool );
extern          bool            uimenuisdisabled( void );
extern          bool            uimenugetaltpressed ( void );
extern          void            uimenusetaltpressed ( bool );
extern          void            uinomenus( void );
extern          void            uiactivatemenus( void );
extern          bool            uienablemenuitem( unsigned, unsigned, bool );
extern          void            uidescmenu( MENUITEM *, DESCMENU * );
extern          void            uidrawmenu( MENUITEM *, DESCMENU *, int );
extern          void            uidisplayitem( MENUITEM *, DESCMENU *, int, bool );
extern          void            uiclosepopup( UI_WINDOW* );
extern          void            uiopenpopup( DESCMENU*, UI_WINDOW* );
extern          bool            uiposfloatingpopup( MENUITEM *menu, DESCMENU *desc, ORD row, ORD col, SAREA *keep_inside, SAREA *keep_visible );
extern          EVENT           uicreatepopup( ORD row, ORD col, MENUITEM *menu, bool left, bool right, EVENT curr_item );
extern          EVENT           uicreatepopupdesc( MENUITEM *menu, DESCMENU *desc, bool left, bool right, EVENT curr_item, bool sub );
extern          EVENT           uicreatepopupinarea( MENUITEM *menu, DESCMENU *desc, bool left, bool right, EVENT curr_item, SAREA *keep_inside, bool sub );
extern          EVENT           uicreatesubpopup( MENUITEM *menu, DESCMENU *desc, bool left, bool right, EVENT curr_item, SAREA *keep_inside, DESCMENU *parent_menu, int index );
extern          EVENT           uicreatesubpopupinarea( MENUITEM *menu, DESCMENU *desc, bool left, bool right, EVENT curr_item, SAREA *keep_inside, SAREA *return_inside, SAREA *return_exclude );
extern          void            uisetbetweentitles( int );
extern          void            uimenucurr( MENUITEM * );
extern          void            uimenutitlebar( void );
extern          int             uigetcurrentmenu( MENUITEM *menu );
extern          unsigned        uimenuheight( void );

#ifdef __cplusplus
}
#endif

#endif
