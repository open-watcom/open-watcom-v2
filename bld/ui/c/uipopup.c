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


#include <string.h>
#include <ctype.h>
#include "uidef.h"
#include "uimenu.h"


static int      ScrollPos       = NO_SELECT;
static int      PrevScrollPos   = NO_SELECT;

static ui_event PopupEvents[] = {
    EV_FIRST_EDIT_CHAR, EV_LAST_EDIT_CHAR,
    EV_ALT_Q,           EV_ALT_M,           // JD - handle alt keys
    __rend__,
    EV_ALT_PRESS,
    EV_ALT_RELEASE,
    EV_CURSOR_UP,
    EV_CURSOR_DOWN,
    EV_CURSOR_RIGHT,
    EV_ESCAPE,
    EV_ENTER,
    EV_MOUSE_MOVE,
    EV_MOUSE_DCLICK,
    EV_MOUSE_DCLICK_R,
    EV_MENU_ACTIVE,
    __end__
};

static ui_event ListToClose[] = {
    __rend__,
    EV_CURSOR_LEFT,
    __end__,
};

static ui_event LeftMouseEvents[] = {
    __rend__,
    EV_MOUSE_PRESS,
    EV_MOUSE_DRAG,
    EV_MOUSE_RELEASE,
    __end__
};

static ui_event RightMouseEvents[] = {
    __rend__,
    EV_MOUSE_PRESS_R,
    EV_MOUSE_DRAG_R,
    EV_MOUSE_RELEASE_R,
    __end__
};

static bool InArea( ORD row, ORD col, SAREA *area )
{
    return( ( ( row >= area->row ) && ( row < ( area->row + area->height ) ) &&
              ( col >= area->col ) && ( col < ( area->col+ area->width   ) ) ) );
}

/*
 * DrawMenuText -- display the line of menu text
 */

static void DrawMenuText( int item, UIMENUITEM *menuitems, DESCMENU *desc, bool iscurritem )
{
    uidisplaymenuitem( &menuitems[item], desc, item, iscurritem );
    uimenucurr( &menuitems[item] );
}

static bool okvert( SAREA *area, SAREA *keep_inside )
{
    return( ( area->row + area->height ) <= ( keep_inside->row + keep_inside->height ) );
}

static bool okhorz( SAREA *area, SAREA *keep_inside )
{
    return( ( area->col + area->width ) <= ( keep_inside->col + keep_inside->width ) );
}

/*
 * RepositionBox -- reposition the box as needed fit on screen
 */

static bool RepositionBox( SAREA *area, SAREA *keep_inside, SAREA *keep_visible )
{
    bool        horz_ok;
    bool        vert_ok;
    ORD         row;
    ORD         col;

    if( area->width > keep_inside->width ) {
        return( false );
    }
    if( area->height > keep_inside->height ) {
        return( false );
    }
    horz_ok = false;
    vert_ok = false;
    if( keep_visible != NULL ) {
        row = area->row;
        area->row = keep_visible->row - 1;
        vert_ok = okvert( area, keep_inside );
        if( !vert_ok ) {
            area->row = keep_visible->row - area->height + 1;
            vert_ok = okvert( area, keep_inside );
            if( !vert_ok ) {
                area->row = row;
            }
        }

        col = area->col;
        area->col = keep_visible->col + keep_visible->width;
        horz_ok = okhorz( area, keep_inside );
        if( !horz_ok ) {
            area->col = keep_visible->col - area->width + 1;
            horz_ok = okhorz( area, keep_inside );
            if( !horz_ok ) {
                area->col = col;
            }
        }
    }
    horz_ok = okhorz( area, keep_inside );
    vert_ok = okvert( area, keep_inside );
    if( !horz_ok ) { /* too far to right, move left as far as needed */
        area->col = keep_inside->col + keep_inside->width - area->width;
    }
    if( !vert_ok ) { /* too close to bottom, move above point */
        if( area->row <= area->height + 1 ) {
            /* no room to go completely above */
            area->row = keep_inside->row + keep_inside->height -
                        area->height;
        } else {
            /* go completely above */
            area->row -= area->height + 1;
        }
    }
    return( true );
}

/*
 * SkipSeparators -- calculate new position, skipping separators
 */

static int SkipSeparators( int diff, int num_items, UIMENUITEM *menuitems )
{
    int pos;

    pos = GetNewPos( ScrollPos + diff, num_items );
    while( MENUSEPARATOR( menuitems[pos] ) ) {
        pos = GetNewPos( pos + diff, num_items );
    }
    return( pos );
}

static void ChangePos( int new_pos, UIMENUITEM *menuitems, DESCMENU *desc )
{
    ScrollPos = new_pos;
    if( PrevScrollPos != NO_SELECT ) {
        DrawMenuText( PrevScrollPos, menuitems, desc, false );
    }
    PrevScrollPos = ScrollPos;
    DrawMenuText( ScrollPos, menuitems, desc, true );
}

/*
 * Scroll -- Scroll which item is selected in the floating popup
 *           Draw old selection normally, hightlight new selection
 */

static void Scroll( int pos, int num_items, UIMENUITEM *menuitems, DESCMENU *desc )
{
    ChangePos( GetNewPos( pos, num_items ), menuitems, desc );
}

static void DoEnd( UI_WINDOW *wptr )
{
    uiclosepopup( wptr );
}

/*
 * SendMenuEvent -- calculate event to return, return whether or not to end
 *                  popup menu
 */

static bool SendMenuEvent( int num_items, UIMENUITEM *menuitems, int item, UI_WINDOW *wptr, ui_event *ui_ev )
{
    *ui_ev = EV_NO_EVENT;
    if( ( item < num_items ) && ( item >= 0 ) ) {
        DoEnd( wptr );
        if( !MENUGRAYED( menuitems[item] ) ) {
            *ui_ev = menuitems[item].event;
            return( true );
        }
        return( true );
    } else {
        return( false );
    }
}

/*
 * KeyboardSelect -- See if the pressed key selects one of the menu items
 */

static bool KeyboardSelect( ui_event ui_ev, int num_items, UIMENUITEM *menuitems, DESCMENU *desc )

{
    int         i;
    char        up;
    char        alt_char;
    int         offset;

    // JD - don't check uimenugetaltpressed.  The menu code may not have seen
    //      the alt key go down.
    alt_char = uialtchar( ui_ev );
    if( alt_char ) {
        up = toupper ( alt_char );
    } else {
        up = toupper( ui_ev );
    }
    for( i = 0; i < num_items; i++ ) {
       if( !MENUSEPARATOR( menuitems[i] ) && !MENUGRAYED( menuitems[i] ) ) {
           offset = CHAROFFSET( menuitems[i] );
           if( ( offset < strlen( menuitems[i].name ) ) &&
               ( toupper( menuitems[i].name[offset] ) == up ) ) {
               ChangePos( i, menuitems, desc );
               return( true );
           }
       }
    }
    return( false );
}

ui_event UIAPI uicreatepopupdesc( UIMENUITEM *menuitems, DESCMENU *desc, bool left,
                                bool right, ui_event curritem_event, bool issubmenu )
{
    SAREA       keep_inside;

    keep_inside.row = 0;
    keep_inside.col = 0;
    keep_inside.width = UIData->width;
    keep_inside.height = UIData->height;

    return( uicreatepopupinarea( menuitems, desc, left, right, curritem_event, &keep_inside, issubmenu ) );

}

static bool createsubpopup( UIMENUITEM *parentitems, bool left, bool right,
                            SAREA *keep_inside, ui_event *new_ui_ev, UI_WINDOW *wptr,
                            DESCMENU *parentdesc, bool set_default )
{
    SAREA       keep_visible;
    int         this_scroll_pos;
    int         this_prev_scroll_pos;
    ui_event    ui_ev;
    ORD         row;
    ORD         col;
    int         curr_row;
    DESCMENU    desc;
    int         num_items;
    ui_event    default_event;
    UIMENUITEM  *menuitems;

    if( MENUGRAYED( parentitems[ScrollPos] ) ) {
        menuitems = NULL;
    } else {
        menuitems = parentitems[ScrollPos].popup;
    }
    if( menuitems != NULL ) {
        row = parentdesc->area.row + ScrollPos;
        col = parentdesc->area.col + parentdesc->area.width - 2;
        keep_visible.row = row + 1;
        keep_visible.col = parentdesc->area.col;
        keep_visible.width = parentdesc->area.width - 2;
        keep_visible.height = 1;
        uiposfloatingpopup( menuitems, &desc, row, col, keep_inside, &keep_visible );
        this_scroll_pos = ScrollPos;
        this_prev_scroll_pos = PrevScrollPos;
        if( set_default && ( menuitems != NULL ) ) {
            default_event = menuitems[0].event;
        } else {
            default_event = EV_NO_EVENT;
        }
        ui_ev = uicreatesubpopup( menuitems, &desc, left, right, default_event, keep_inside, parentdesc, ScrollPos );
        ScrollPos = this_scroll_pos;
        PrevScrollPos = this_prev_scroll_pos;
        switch( ui_ev ) {
        case EV_MOUSE_DRAG :
        case EV_MOUSE_DRAG_R :
        case EV_MOUSE_PRESS:
        case EV_MOUSE_PRESS_R:
        case EV_MOUSE_RELEASE:
        case EV_MOUSE_RELEASE_R:
            uivmousepos( NULL, &row, &col );
            if( ( col > parentdesc->area.col ) && ( col < ( parentdesc->area.col + parentdesc->area.width - 1 ) ) ) {
                curr_row = row - parentdesc->area.row - 1;
                num_items = parentdesc->area.height - 2;
                if( ( curr_row >= 0 ) && ( curr_row < num_items ) ) {
                    if( curr_row != ScrollPos ) {
                        Scroll( curr_row, num_items, parentitems, parentdesc );
                        *new_ui_ev = ui_ev; // JD - send event back up to parent
                    }
                }
            }
            break;
        case EV_CURSOR_LEFT :
        case EV_CURSOR_RIGHT :
            break;
        default :
            if( ui_ev != EV_NO_EVENT ) {
                DoEnd( wptr );
                *new_ui_ev = ui_ev;
                return( true );
            }
        }
        uidrawmenu( parentitems, parentdesc, ScrollPos );
    }
    return( false );
}

/*
 *  uiposfloatingpopup
 *
 */

bool UIAPI uiposfloatingpopup( UIMENUITEM *menuitems, DESCMENU *desc, ORD row, ORD col,
                         SAREA *keep_inside, SAREA *keep_visible )
{
    desc->area.row = row;
    desc->area.col = col;
    desc->titlecol = 0;
    desc->titlewidth = 0;
    desc->flags = 0;
    uidescmenu( menuitems, desc );
    if( !RepositionBox( &desc->area, keep_inside, keep_visible ) ) {
        return( false );
    }
    return( true );
}

static ui_event createpopupinarea( UIMENUITEM *menuitems, DESCMENU *desc,
                                bool left, bool right,
                                ui_event curritem_event, SAREA *keep_inside,
                                SAREA *return_inside, SAREA *return_exclude,
                                bool issubmenu )
{
    ui_event    ui_ev;
    ui_event    new_ui_ev;
    int         curr_row;
    bool        done;
    bool        no_select;
//    bool        select_default;
    bool        no_move;
    int         new;
    UI_WINDOW   window;
    int         num_items;
    int         i;
    ORD         row;
    ORD         col;
    bool        disabled;

    num_items = desc->area.height - 2;
    if( num_items <= 0 ) {
        return( EV_NO_EVENT );
    }
    uiopenpopup( desc, &window );
    ScrollPos = NO_SELECT;
    if( curritem_event != EV_NO_EVENT ) {
        for( i = 0; i < num_items; i++ ) {
            if( !MENUSEPARATOR( menuitems[i] ) && !MENUGRAYED( menuitems[i] ) && ( menuitems[i].event == curritem_event ) ) {
                ScrollPos = i;
                break;
            }
        }
    }
    uidrawmenu( menuitems, desc, ScrollPos );
    if( ScrollPos != NO_SELECT ) {
        uimenucurr( &menuitems[ScrollPos] );
    }

    uipushlist( ListToClose );
    uipushlist( PopupEvents );
    disabled = uimenuisdisabled(); // JD - keep menus from intercepting alt keys
    uimenudisable( true );
    if( left ) {
        uipushlist( LeftMouseEvents );
    }
    if( right ) {
        uipushlist( RightMouseEvents );
    }

    PrevScrollPos = ScrollPos;
    no_move = true;
    done = false;
    new_ui_ev = EV_NO_EVENT;
    while( !done ) {
//        select_default = false;
        ui_ev = uivgetevent( NULL );

        switch( ui_ev ) {
        case EV_CURSOR_LEFT :
            if( !issubmenu ) {
                break;
            }
            /* fall through */
        case EV_ALT_PRESS :
            new_ui_ev = ui_ev;
            done = true;
            DoEnd( &window );
            break;
        case EV_ALT_RELEASE :
            uimenusetaltpressed( false );
            break;
        case EV_CURSOR_UP :
            new = SkipSeparators( -1, num_items, menuitems );
            Scroll( new, num_items, menuitems, desc );
            break;
        case EV_CURSOR_DOWN :
            new = SkipSeparators( 1, num_items, menuitems );
            Scroll( new, num_items, menuitems, desc );
            break;
        case EV_MENU_ACTIVE :
            new_ui_ev = ui_ev;
            /* fall through */
        case EV_ESCAPE :
            DoEnd( &window );
            done = true;
            break;
        case EV_MOUSE_MOVE :
            no_move = false;
            break;
        case EV_MOUSE_DCLICK :
        case EV_MOUSE_DCLICK_R :
            if( no_move ) {
                new_ui_ev = ui_ev;
                DoEnd( &window );
                done = true;
            }
            break;
        case EV_MOUSE_DRAG :
        case EV_MOUSE_DRAG_R :
            no_move = false;   /* break intentionally left out */
            /* fall through */
        case EV_MOUSE_PRESS:
        case EV_MOUSE_PRESS_R:
        case EV_MOUSE_RELEASE:
        case EV_MOUSE_RELEASE_R:
            for( ;; ) { // JD - loop to get subsequent popup created if
                        //      we went directly from one cascaded menu to another
                no_select = true;
                uivmousepos( NULL, &row, &col );
                if( ( return_inside != NULL ) && ( return_exclude != NULL ) ) {
                    if( !InArea( row, col, &desc->area ) &&
                        InArea( row, col, return_inside ) &&
                        !InArea( row, col, return_exclude ) ) {
                        done = true;
                        new_ui_ev = ui_ev;
                        DoEnd( &window );
                    }
                }
                if( !done ) {
                    if( ( col > desc->area.col ) && ( col < ( desc->area.col + desc->area.width - 1 ) ) ) {
                        curr_row = row - desc->area.row - 1;
                        if( ( curr_row >= 0 ) && ( curr_row < num_items ) ) {
                            no_select = false;
                            if( curr_row != ScrollPos ) {
                                Scroll( curr_row, num_items, menuitems, desc );
                            }
                            if( ( ui_ev == EV_MOUSE_RELEASE ) || ( ui_ev == EV_MOUSE_RELEASE_R ) ) {
                                done = SendMenuEvent( num_items, menuitems, row - desc->area.row - 1, &window, &new_ui_ev );
                            } else {
                                new_ui_ev = EV_NO_EVENT; // JD - break loop if no popup created
                                done = createsubpopup( menuitems, left, right, keep_inside, &new_ui_ev, &window, desc, false );
                                if( !done && new_ui_ev != EV_NO_EVENT ) {
                                    continue; // JD - see if we need to create another popup
                                }
                            }
                        }
                    }
                }
                if( ( ui_ev == EV_MOUSE_RELEASE ) || ( ui_ev == EV_MOUSE_RELEASE_R ) ) {
                    if( no_move ) {     /* mouse up and down on same spot */
                        no_select = false;
                        Scroll( 0, num_items, menuitems, desc );
                    } else {
                        if( !done ) {
                            done = true;
                            DoEnd( &window );
                        }
                    }
                }
                if( no_select && !done ) {  /* no item is selected */
                    if( ScrollPos != NO_SELECT ) {
                        DrawMenuText( ScrollPos, menuitems, desc, false );
                        uimenucurr( NULL );
                    }
                    ScrollPos = NO_SELECT;
                }
                break;
            }
            break;
        case EV_ENTER :
            if( ScrollPos != NO_SELECT && menuitems[ScrollPos].popup != NULL ) { // JD
                done = createsubpopup( menuitems, left, right, keep_inside, &new_ui_ev, &window, desc, true );
            } else {
                done = SendMenuEvent( num_items, menuitems, ScrollPos, &window, &new_ui_ev );
            }
            break;
        case EV_CURSOR_RIGHT :
            if( ScrollPos != NO_SELECT && menuitems[ScrollPos].popup != NULL ) { // JD
                done = createsubpopup( menuitems, left, right, keep_inside, &new_ui_ev, &window, desc, true );
            } else {
                if( issubmenu ) {
                    new_ui_ev = ui_ev;
                    done = true;
                    DoEnd( &window );
                }
            }
            break;
        case EV_KILL_UI:
            new_ui_ev = ui_ev;
            DoEnd( &window );
            done = true;
            break;
        default :
            if( iskeyboardchar( ui_ev ) ) {
                if( KeyboardSelect( ui_ev, num_items, menuitems, desc ) ) {
                    if( ScrollPos != NO_SELECT && menuitems[ScrollPos].popup != NULL ) { // JD
                        done = createsubpopup( menuitems, left, right, keep_inside, &new_ui_ev, &window, desc, true );
                    } else {
                        done = SendMenuEvent( num_items, menuitems, ScrollPos, &window, &new_ui_ev );
                    }
                }
            }
            break;
        }
    }
    if( left ) {
        uipoplist( /* LeftMouseEvents*/ );
    }
    if( right ) {
        uipoplist( /* RightMouseEvents*/ );
    }
    uimenudisable( disabled ); // JD
    uipoplist( /* PopupEvents */ );
    uipoplist( /* ListToClose */ );
    return( new_ui_ev );
}

ui_event UIAPI uicreatepopupinarea( UIMENUITEM *menuitems, DESCMENU *desc, bool left,
                                  bool right, ui_event curritem_event,
                                  SAREA *keep_inside, bool issubmenu )
{
    return( createpopupinarea( menuitems, desc, left, right, curritem_event, keep_inside, NULL, NULL, issubmenu ) );
}

ui_event UIAPI uicreatesubpopup( UIMENUITEM *menuitems, DESCMENU *desc, bool left,
                               bool right, ui_event curritem_event, SAREA *keep_inside,
                               DESCMENU *parentdesc, int item )
{
    SAREA       return_exclude;

    return_exclude.row = parentdesc->area.row + 1 + item;
    return_exclude.col = parentdesc->area.col;
    return_exclude.width = parentdesc->area.width;
    return_exclude.height = 1;

    return( uicreatesubpopupinarea( menuitems, desc, left, right, curritem_event,
                                    keep_inside, &parentdesc->area, &return_exclude ) );
}

ui_event UIAPI uicreatesubpopupinarea( UIMENUITEM *menuitems, DESCMENU *desc, bool left,
                                     bool right, ui_event curritem_event, SAREA *keep_inside,
                                     SAREA *return_inside, SAREA *return_exclude )
{
    return( createpopupinarea( menuitems, desc, left, right, curritem_event, keep_inside,
                               return_inside, return_exclude, true ) );
}

ui_event UIAPI uicreatepopup( ORD row, ORD col, UIMENUITEM *menuitems, bool left, bool right, ui_event curritem_event )
{
    DESCMENU    desc;
    SAREA       keep_inside;

    keep_inside.row = 0;
    keep_inside.col = 0;
    keep_inside.width = UIData->width;
    keep_inside.height = UIData->height;
    if( uiposfloatingpopup( menuitems, &desc, row, col, &keep_inside, NULL ) ) {
        return( uicreatepopupdesc( menuitems, &desc, left, right, curritem_event, false ) );
    }
    return( EV_NO_EVENT );
}
