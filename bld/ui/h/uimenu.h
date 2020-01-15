/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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

#define MAX_MENUS               15

#define MENU_TAB_OFFSET         0x00ffU
#define MENU_HAS_TAB            0x0100U
#define MENU_HAS_CHECK          0x0200U
#define MENU_HAS_POPUP          0x0400U

#define MENU_GET_TAB_OFFSET( desc )     ((desc)->flags & MENU_TAB_OFFSET)
#define MENU_SET_TAB_OFFSET( desc, o )  (desc)->flags |= (o) & MENU_TAB_OFFSET
#define MENU_GET_ROW( desc )            ((desc)->area.row - 1)

typedef struct describemenu {
    SAREA           area;           /* area of menu         */
    uisize          titlecol;       /* column of title      */
    uisize          titlewidth;     /* width of title       */
    unsigned short  flags;
} DESCMENU;

#define ITEM_CHAR_OFFSET        0x00ff
#define ITEM_GRAYED             0x0100
#define ITEM_CHECKED            0x0200
#define ITEM_SEPARATOR          0x0400

#define CHAROFFSET( menu )      ((menu).flags & ITEM_CHAR_OFFSET)
#define MENUGRAYED( menu )      (((menu).flags & ITEM_GRAYED) != 0)
#define MENUSEPARATOR( menu )   (((menu).flags & ITEM_SEPARATOR) || ((menu).name[0] == '\0'))
#define MENUENDMARKER( menu )   (((menu).name == NULL) && ((menu).flags & ITEM_SEPARATOR) == 0)

typedef struct menuitem {
    char            *name;              /* name of item         */
    ui_event        event;              /* item event           */
    unsigned short  flags;              /* char offset, grayed  */
    struct menuitem *popup;             /* popup from this menu */
} UIMENUITEM;

/* the titles and items fields must be initialized by the       */
/* application - all other fields are for looking at only       */

typedef struct vbarmenu {
    UIMENUITEM      *titles;            /* titles for pull down menus        */
    ui_event        event;              /* current menu item event           */
    int             currmenu;           /* current menu index (base 0)       */
    boolbit         inlist       :1;    /* selection will lead to the event  */
    boolbit         active       :1;    /* the user is browsing the menus    */
    boolbit         draginmenu   :1;    /* drag operation began in menus     */
    boolbit         indicators   :1;    /* keyboard indicators               */
    boolbit         scroll       :1;    /* scroll indicator                  */
    boolbit         caps         :1;    /* caps indicator                    */
    boolbit         num          :1;    /* num indicator                     */
    boolbit         altpressed   :1;    /* alt key has been pressed not rel  */
    boolbit         movedmenu    :1;    /* have dragged to diff item or menu */
    boolbit         ignorealt    :1;    /* ignore 1 alt press                */
    boolbit         popuppending :1;    /* need to open popup                */
    boolbit         disabled     :1;    /* menu has been disabled            */
} VBARMENU;

#ifdef __cplusplus
    extern "C" {
#endif

extern char         UIAPI uialtchar( ui_event );
extern void         UIAPI uisetmenudesc( void );
extern VBARMENU     * UIAPI uimenubar( VBARMENU * );
extern int          UIAPI uimenuitemscount( UIMENUITEM *menuitems );
extern void         UIAPI uimenuindicators( bool );
extern void         UIAPI uimenus( UIMENUITEM *menuitems, UIMENUITEM **popupitems, ui_event );
extern bool         UIAPI uimenuson( void );
extern void         UIAPI uimenudisable( bool );
extern bool         UIAPI uimenuisdisabled( void );
extern bool         UIAPI uimenugetaltpressed ( void );
extern void         UIAPI uimenusetaltpressed ( bool );
extern void         UIAPI uinomenus( void );
extern bool         UIAPI uienablepopupitem( int menuitem, int popupitem, bool );
extern void         UIAPI uidescmenu( UIMENUITEM *menuitems, DESCMENU *desc );
extern void         UIAPI uidrawmenu( UIMENUITEM *menuitems, DESCMENU *desc, int curritem );
extern void         UIAPI uidisplaymenuitem( UIMENUITEM *menuitem, DESCMENU *desc, int item, bool iscurritem );
extern void         UIAPI uiclosepopup( UI_WINDOW * );
extern void         UIAPI uiopenpopup( DESCMENU *, UI_WINDOW * );
extern bool         UIAPI uiposfloatingpopup( UIMENUITEM *menuitems, DESCMENU *desc, ORD row, ORD col, SAREA *keep_inside, SAREA *keep_visible );
extern ui_event     UIAPI uicreatepopup( ORD row, ORD col, UIMENUITEM *menuitems, bool left, bool right, ui_event curritem_event );
extern ui_event     UIAPI uicreatepopupdesc( UIMENUITEM *menuitems, DESCMENU *desc, bool left, bool right, ui_event curritem_event, bool issubmenu );
extern ui_event     UIAPI uicreatepopupinarea( UIMENUITEM *menuitems, DESCMENU *desc, bool left, bool right, ui_event curritem_event, SAREA *keep_inside, bool issubmenu );
extern ui_event     UIAPI uicreatesubpopup( UIMENUITEM *menuitems, DESCMENU *desc, bool left, bool right, ui_event curritem_event, SAREA *keep_inside, DESCMENU *parentdesc, int item );
extern ui_event     UIAPI uicreatesubpopupinarea( UIMENUITEM *menuitems, DESCMENU *desc, bool left, bool right, ui_event curritem_event, SAREA *keep_inside, SAREA *return_inside, SAREA *return_exclude );
extern void         UIAPI uisetbetweentitles( unsigned );
extern void         UICALLBACK uimenucurr( UIMENUITEM *menuitem );
extern void         UIAPI uimenutitlebar( void );
extern bool         UIAPI uigetcurrentmenu( UIMENUITEM *currmenuitem );
extern unsigned     UIAPI uimenuheight( void );

#ifdef __cplusplus
}
#endif

#endif
