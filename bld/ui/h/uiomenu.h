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


#ifndef _UIOMENU_H_
#define _UIOMENU_H_
#define         MAX_MENUS               10

typedef struct describemenu {
        SAREA           area;           /* area of menu         */
        ORD             titlecol;       /* column of title      */
        ORD             titlewidth;     /* width of title       */
} DESCMENU;

typedef struct menuitem {
        char*           name;           /* name of item         */
        EVENT           event;          /* item event           */
} MENUITEM;

/* the titles and items fields must be initialized by the       */
/* application - all other fields are for looking at only       */

typedef struct vbarmenu {
        MENUITEM*       titles;         /* titles for pull down menus       */
        MENUITEM**      items;          /* pull down menus                  */
        EVENT           event;          /* current menu item event          */
        unsigned        inlist:1;       /* selection will lead to the event */
        unsigned        newitem:1;      /* current menu item changed        */
        unsigned        active:1;       /* the user is browsing the menus   */
        unsigned        indicators:1;   /* keyboard indicators              */
        unsigned        scroll:1;       /* scroll indicator                 */
        unsigned        caps:1;         /* caps indicator                   */
        unsigned        num:1;          /* num indicator                    */
        signed char     menu;           /* current menu number (base 1)     */
        signed char     item;           /* current item number (base 1)     */
} VBARMENU;

void            extern          uimenubar( VBARMENU * );
void            extern          uimenuindicators( int );
void            extern          uimenus( MENUITEM *, MENUITEM **, EVENT );
void            extern          uinomenus( void );
#endif
