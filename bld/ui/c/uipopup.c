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


#include <string.h>
#include <ctype.h>
#include "uidef.h"
#include "uimenu.h"

#define NO_SELECT       -1

static  int     ScrollPos       = NO_SELECT;
static  int     PrevScrollPos   = NO_SELECT;

static EVENT PopupEvents[] = {
    EV_FIRST_EDIT_CHAR, EV_LAST_EDIT_CHAR,
    EV_ALT_Q, EV_ALT_M, // JD - handle alt keys
    EV_NO_EVENT,
    EV_ALT_PRESS,
    EV_ALT_RELEASE,
    EV_CURSOR_UP,
    EV_CURSOR_DOWN,
    EV_CURSOR_RIGHT,
    EV_ESCAPE,
    EV_RETURN,
    EV_MOUSE_MOVE,
    EV_MOUSE_DCLICK,
    EV_MOUSE_DCLICK_R,
    EV_MENU_ACTIVE,
    EV_NO_EVENT
};

static EVENT ListToClose[] = {
    EV_NO_EVENT,
    EV_CURSOR_LEFT,
    EV_NO_EVENT,
};

static EVENT LeftMouseEvents[] = {
    EV_NO_EVENT,
    EV_MOUSE_PRESS,
    EV_MOUSE_DRAG,
    EV_MOUSE_RELEASE
};

static EVENT RightMouseEvents[] = {
    EV_NO_EVENT,
    EV_MOUSE_PRESS_R,
    EV_MOUSE_DRAG_R,
    EV_MOUSE_RELEASE_R
};

static bool InArea( ORD row, ORD col, SAREA *area )
{
    return( ( ( row >= area->row ) && ( row < ( area->row + area->height ) ) &&
              ( col >= area->col ) && ( col < ( area->col+ area->width   ) ) ) );
}

/*
 * DrawMenuText -- display the line of menu text
 */

static void DrawMenuText( int index, MENUITEM *menu, bool curr, DESCMENU *desc )
{
    uidisplayitem( &menu[index], desc, index + 1, curr );
    uimenucurr( &menu[index] );
}

static bool okvert( SAREA *area, SAREA *keep_inside )
{
    return( ( area->row + area->height ) <=
            ( keep_inside->row + keep_inside->height ) );
}

static bool okhorz( SAREA *area, SAREA *keep_inside )
{
    return( ( area->col + area->width ) <=
            ( keep_inside->col + keep_inside->width ) );
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
        return( FALSE );
    }
    if( area->height > keep_inside->height ) {
        return( FALSE );
    }
    horz_ok = FALSE;
    vert_ok = FALSE;
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
    return( TRUE );
}

/*
 * GetNewPos -- calculate new position based on circular menu
 */

static int GetNewPos( int pos, int num )
{
    if( pos >= num ) {
        return( 0 );
    } else if( pos < 0 ) {
        return( num - 1 );
    } else {
        return( pos );
    }
}

/*
 * SkipSeparators -- calculate new position, skipping separators
 */

static int SkipSeparators( int diff, int num, MENUITEM *menu )
{
    int pos;

    pos = GetNewPos( ScrollPos + diff, num );
    while( MENUSEPARATOR( menu[pos] ) ) {
        pos = GetNewPos( pos + diff, num );
    }
    return( pos );
}

static void ChangePos( int new_pos, MENUITEM *menu, DESCMENU *desc )
{
    ScrollPos = new_pos;
    if( PrevScrollPos != NO_SELECT ) {
        DrawMenuText( PrevScrollPos, menu, FALSE, desc );
    }
    PrevScrollPos = ScrollPos;
    DrawMenuText( ScrollPos, menu, TRUE, desc );
}

/*
 * Scroll -- Scroll which item is selected in the floating popup
 *           Draw old selection normally, hightlight new selection
 */

static void Scroll( int pos, int num, MENUITEM *menu, DESCMENU *desc )
{
    ChangePos( GetNewPos( pos, num ), menu, desc );
}

static void DoEnd( UI_WINDOW *window )
{
    uiclosepopup( window );
}

/*
 * SendEvent -- calculate event to return, return whether or not to end
 *              popup menu
 */

static bool SendEvent( int num, MENUITEM *menu, int index,
                       UI_WINDOW *window, EVENT *ev )
{

    *ev = EV_NO_EVENT;
    if( ( index < num ) && ( index >= 0 ) ) {
        DoEnd( window );
        if( !( MENUGRAYED( menu[index] ) ) ) {
            *ev = menu[index].event;
            return( TRUE );
        }
        return( TRUE );
    } else {
        return( FALSE );
    }
}

/*
 * KeyboardSelect -- See if the pressed key selects one of the menu items
 */

static bool KeyboardSelect( EVENT ev, int num, MENUITEM *menu, DESCMENU *desc )

{
    int         i;
    char        up, alt_char;
    int         offset;

    // JD - don't check uimenugetaltpressed.  The menu code may not have seen
    //      the alt key go down.
    alt_char = uialtchar( ev );
    if ( alt_char ) {
        up = toupper ( alt_char );
    } else {
        up = toupper( ev );
    }
    for( i = 0; i < num; i++ ) {
       if( !MENUSEPARATOR( menu[i] ) && !MENUGRAYED( menu[i] ) ) {
           offset = CHAROFFSET( menu[i] );
           if( ( offset < strlen( menu[i].name ) ) &&
               ( toupper( menu[i].name[offset] ) == up ) ) {
               ChangePos( i, menu, desc );
               return( TRUE );
           }
       }
    }
    return( FALSE );
}

global EVENT uicreatepopupdesc( MENUITEM *menu, DESCMENU *desc, bool left,
                                bool right, EVENT curr_item, bool sub )
{
    SAREA       keep_inside;

    keep_inside.row = 0;
    keep_inside.col = 0;
    keep_inside.width = UIData->width;
    keep_inside.height = UIData->height;

    return( uicreatepopupinarea( menu, desc, left, right, curr_item,
                                 &keep_inside, sub ) );

}

static bool createsubpopup( MENUITEM *menu, bool left, bool right,
                            SAREA *keep_inside, EVENT *new_ev, UI_WINDOW *window,
                            DESCMENU *desc, bool set_default )
{
    SAREA       keep_visible;
    int         this_scroll_pos;
    int         this_prev_scroll_pos;
    EVENT       ev;
    ORD         row;
    ORD         col;
    int         curr_row;
    DESCMENU    sub_desc;
    int         num;
    EVENT       default_event;
    MENUITEM    *curr_popup;

    if ( MENUGRAYED(menu[ScrollPos]) ) {
        curr_popup = NULL;
    } else {
        curr_popup = menu[ScrollPos].popup;
    }
    if( curr_popup != NULL ) {
        row = desc->area.row + ScrollPos;
        col = desc->area.col + desc->area.width - 2;
        keep_visible.row = row + 1;
        keep_visible.col = desc->area.col;
        keep_visible.width = desc->area.width - 2;
        keep_visible.height = 1;
        uiposfloatingpopup( curr_popup, &sub_desc, row, col,
                            keep_inside, &keep_visible );
        this_scroll_pos = ScrollPos;
        this_prev_scroll_pos = PrevScrollPos;
        if( set_default && ( curr_popup != NULL ) ) {
            default_event = curr_popup[0].event;
        } else {
            default_event = EV_NO_EVENT;
        }
        ev = uicreatesubpopup( curr_popup, &sub_desc, left, right,
                               default_event, keep_inside, desc, ScrollPos );
        ScrollPos = this_scroll_pos;
        PrevScrollPos = this_prev_scroll_pos;
        switch( ev ) {
        case EV_MOUSE_DRAG :
        case EV_MOUSE_DRAG_R :
        case EV_MOUSE_PRESS:
        case EV_MOUSE_PRESS_R:
        case EV_MOUSE_RELEASE:
        case EV_MOUSE_RELEASE_R:
            uivmousepos( NULL, &row, &col );
            if( ( col > desc->area.col ) &&
                ( col < ( desc->area.col + desc->area.width - 1 ) ) ) {
                curr_row = row - desc->area.row - 1;
                num = desc->area.height - 2;
                if( ( curr_row >= 0 ) && ( curr_row < num ) ) {
                    if( curr_row != ScrollPos ) {
                        Scroll( curr_row, num, menu, desc );
                        *new_ev = ev; // JD - send event back up to parent
                    }
                }
            }
            break;
        case EV_CURSOR_LEFT :
        case EV_CURSOR_RIGHT :
            break;
        default :
            if( ev != EV_NO_EVENT ) {
                DoEnd( window );
                *new_ev = ev;
                return( TRUE );
            }
        }
        uidrawmenu( menu, desc, ScrollPos + 1 );
    }
    return( FALSE );
}

/*
 *  uiposfloatingpopup
 *
 */

bool uiposfloatingpopup( MENUITEM *menu, DESCMENU *desc, ORD row, ORD col,
                         SAREA *keep_inside, SAREA *keep_visible )
{
    desc->area.row = row;
    desc->area.col = col;
    desc->titlecol = 0;
    desc->titlewidth = 0;
    desc->flags = 0;
    uidescmenu( menu, desc );
    if( !RepositionBox( &desc->area, keep_inside, keep_visible ) ) {
        return( FALSE );
    }
    return( TRUE );
}

static EVENT createpopupinarea( MENUITEM *menu, DESCMENU *desc,
                                bool left, bool right,
                                EVENT curr_item, SAREA *keep_inside,
                                SAREA *return_inside, SAREA *return_exclude,
                                bool sub )
{
    EVENT       ev;
    EVENT       new_ev;
    int         curr_row;
    bool        done;
    bool        no_select;
    bool        select_default;
    bool        no_move;
    int         new;
    UI_WINDOW   window;
    int         num;
    int         i;
    ORD         row;
    ORD         col;
    bool        disabled;

    num = desc->area.height - 2;
    if( num <= 0 ) {
        return( EV_NO_EVENT );
    }
    uiopenpopup( desc, &window );
    ScrollPos = NO_SELECT;
    if( curr_item != 0 ) {
        for( i=0; i< num; i++ ) {
            if( ( !MENUSEPARATOR( menu[i] ) ) && ( !MENUGRAYED( menu[i] ) ) &&
                ( menu[i].event == curr_item ) ) {
                ScrollPos = i;
                break;
            }
        }
    }
    uidrawmenu( menu, desc, ScrollPos + 1 );
    if( ScrollPos != NO_SELECT ) {
        uimenucurr( &menu[ScrollPos] );
    }

    uipushlist( ListToClose );
    uipushlist( PopupEvents );
    disabled = uimenuisdisabled(); // JD - keep menus from intercepting alt keys
    uimenudisable( TRUE );
    if( left ) {
        uipushlist( LeftMouseEvents );
    }
    if( right ) {
        uipushlist( RightMouseEvents );
    }

    PrevScrollPos = ScrollPos;
    no_move = TRUE;
    done = FALSE;
    new_ev = EV_NO_EVENT;
    while( !done ) {
        select_default = FALSE;
        ev = uivgetevent( NULL );

        switch( ev ) {
        case EV_CURSOR_LEFT :
            if( !sub ) {
                break;
            }
            /* fall through */
        case EV_ALT_PRESS :
            new_ev = ev;
            done = TRUE;
            DoEnd( &window );
            break;
        case EV_ALT_RELEASE :
            uimenusetaltpressed( FALSE );
            break;
        case EV_CURSOR_UP :
            new = SkipSeparators( -1, num, menu );
            Scroll( new, num, menu, desc );
            break;
        case EV_CURSOR_DOWN :
            new = SkipSeparators( 1, num, menu );
            Scroll( new, num, menu, desc );
            break;
        case EV_MENU_ACTIVE :
            new_ev = ev;
            /* fall through */
        case EV_ESCAPE :
            DoEnd( &window );
            done = TRUE;
            break;
        case EV_MOUSE_MOVE :
            no_move = FALSE;
            break;
        case EV_MOUSE_DCLICK :
        case EV_MOUSE_DCLICK_R :
            if( no_move ) {
                new_ev = ev;
                DoEnd( &window );
                done = TRUE;
            }
            break;
        case EV_MOUSE_DRAG :
        case EV_MOUSE_DRAG_R :
            no_move = FALSE;   /* break intentionally left out */
        case EV_MOUSE_PRESS:
        case EV_MOUSE_PRESS_R:
        case EV_MOUSE_RELEASE:
        case EV_MOUSE_RELEASE_R:
            for( ;; ) { // JD - loop to get subsequent popup created if
                        //      we went directly from one cascaded menu to another
                no_select = TRUE;
                uivmousepos( NULL, &row, &col );
                if( ( return_inside != NULL ) && ( return_exclude != NULL ) ) {
                    if( !InArea( row, col, &desc->area ) &&
                        InArea( row, col, return_inside ) &&
                        !InArea( row, col, return_exclude ) ) {
                        done = TRUE;
                        new_ev = ev;
                        DoEnd( &window );
                    }
                }
                if( !done ) {
                    if( ( col > desc->area.col ) &&
                        ( col < ( desc->area.col + desc->area.width - 1 ) ) ) {
                        curr_row = row - desc->area.row - 1;
                        if( ( curr_row >= 0 ) && ( curr_row < num ) ) {
                            no_select = FALSE;
                            if( curr_row != ScrollPos ) {
                                Scroll( curr_row, num, menu, desc );
                            }
                            if( ( ev == EV_MOUSE_RELEASE ) || ( ev == EV_MOUSE_RELEASE_R ) ) {
                                done = SendEvent( num, menu, row - desc->area.row - 1, &window, &new_ev );
                            } else {
                                new_ev = EV_NO_EVENT; // JD - break loop if no popup created
                                done = createsubpopup( menu, left, right, keep_inside,
                                                       &new_ev, &window, desc, FALSE );
                                if( !done && new_ev != EV_NO_EVENT ) {
                                    continue; // JD - see if we need to create another popup
                                }
                            }
                        }
                    }
                }
                if( ( ev == EV_MOUSE_RELEASE ) || ( ev == EV_MOUSE_RELEASE_R ) ) {
                    if( no_move ) {     /* mouse up and down on same spot */
                        no_select = FALSE;
                        Scroll( 0, num, menu, desc );
                    } else {
                        if( !done ) {
                            done = TRUE;
                            DoEnd( &window );
                        }
                    }
                }
                if( no_select && !done ) {  /* no item is selected */
                    if( ScrollPos != NO_SELECT ) {
                        DrawMenuText( ScrollPos, menu, FALSE, desc );
                        uimenucurr( NULL );
                    }
                    ScrollPos = NO_SELECT;
                }
                break;
            }
            break;
        case EV_RETURN :
            if( ScrollPos != NO_SELECT && menu[ScrollPos].popup != NULL ) { // JD
                done = createsubpopup( menu, left, right, keep_inside,
                                       &new_ev, &window, desc, TRUE );
            } else {
                done = SendEvent( num, menu, ScrollPos, &window, &new_ev );
            }
            break;
        case EV_CURSOR_RIGHT :
            if( ScrollPos != NO_SELECT && menu[ScrollPos].popup != NULL ) { // JD
                done = createsubpopup( menu, left, right, keep_inside,
                                       &new_ev, &window, desc, TRUE );
            } else {
                if( sub ) {
                    new_ev = ev;
                    done = TRUE;
                    DoEnd( &window );
                }
            }
            break;
        case EV_KILL_UI:
            new_ev = ev;
            DoEnd( &window );
            done = TRUE;
            break;
        default :
            if( ev <= EV_LAST_KEYBOARD ) {
                if( KeyboardSelect( ev, num, menu, desc ) ) {
                    if( ScrollPos != NO_SELECT && menu[ScrollPos].popup != NULL ) { // JD
                        done = createsubpopup( menu, left, right, keep_inside,
                                               &new_ev, &window, desc, TRUE );
                    } else {
                        done = SendEvent( num, menu, ScrollPos, &window,
                                          &new_ev );
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
    return( new_ev );
}

global EVENT uicreatepopupinarea( MENUITEM *menu, DESCMENU *desc, bool left,
                                  bool right, EVENT curr_item,
                                  SAREA *keep_inside, bool sub )
{
    return( createpopupinarea( menu, desc, left, right, curr_item,
                               keep_inside, NULL, NULL, sub ) );
}

global EVENT uicreatesubpopup( MENUITEM *menu, DESCMENU *desc, bool left,
                               bool right, EVENT curr_item, SAREA *keep_inside,
                               DESCMENU *parent_menu, int index )
{
    SAREA       return_exclude;

    return_exclude.row = parent_menu->area.row + index + 1;
    return_exclude.col = parent_menu->area.col;
    return_exclude.width = parent_menu->area.width;
    return_exclude.height = 1;

    return( uicreatesubpopupinarea( menu, desc, left, right, curr_item,
                                    keep_inside, &parent_menu->area,
                                    &return_exclude ) );
}

global EVENT uicreatesubpopupinarea( MENUITEM *menu, DESCMENU *desc, bool left,
                                     bool right, EVENT curr_item, SAREA *keep_inside,
                                     SAREA *return_inside, SAREA *return_exclude )
{
    return( createpopupinarea( menu, desc, left, right, curr_item, keep_inside,
                               return_inside, return_exclude, TRUE ) );
}

global EVENT uicreatepopup( ORD row, ORD col, MENUITEM *menu, bool left, bool right,
                            EVENT curr_item )
{
    DESCMENU    desc;
    SAREA       keep_inside;

    keep_inside.row = 0;
    keep_inside.col = 0;
    keep_inside.width = UIData->width;
    keep_inside.height = UIData->height;
    if( uiposfloatingpopup( menu, &desc, row, col, &keep_inside, NULL ) ) {
        return( uicreatepopupdesc( menu, &desc, left, right, curr_item, FALSE ) );
    }
    return( EV_NO_EVENT );
}
