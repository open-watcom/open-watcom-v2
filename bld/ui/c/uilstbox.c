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
#include "uivedit.h"
#include "uiledit.h"
#include "uidialog.h"

static bool SelStart =  FALSE;    // user has moused over a valid choice
static bool Dclick =    FALSE;    // TRUE between DCLICK and MOUSE_RELEASE

enum    { R_UP, R_DOWN, R_UNS, R_SEL };

bool uigetlistelement( char **p, unsigned i, char *s, unsigned l )
{
    if( p == NULL ) {
        return( FALSE );
    }
    if( *( p + i ) != NULL && *p != NULL ) {
//    if( *( p + i ) != NULL ) {
        if( l > 0 ) {
            strncpy( s, *( p + i ), l );
        }
        return( TRUE );
    } else {
        if( l > 0 ) {
            *s = '\0';
        }
        return( FALSE );
    }
}

void uipaintlistbox( a_list *list )
{
    ORD         i;
    char        *buf;
    bool        ok;
    int         length;
    ATTR        attr;

    if( list->box == NULL ) {
        return;
    }

    length = list->box->area.width + 1;
    buf = ( char * )uimalloc( length );

    for( i = 0 ; i < list->box->area.height ; ++i ) {
        attr = ATTR_NORMAL;
        if( list->box->row == i + list->box->line ) {
            attr = list->box->attr;
        }
        ok = (*list->get)( list->data, list->box->line + i, buf, length );
        if( ok ) {
            uitextfield( list->box->vs, list->box->area.row + i,
                         list->box->area.col, list->box->area.width,
                         UIData->attrs[ attr ], buf, strlen( buf ) );
        } else {
            break;
        }
    }
    for( ; i < list->box->area.height; ++i ) {
        uitextfield( list->box->vs, list->box->area.row + i,
                     list->box->area.col, list->box->area.width,
                     UIData->attrs[ ATTR_NORMAL ], "", 0 );
    }
    uifree( buf );
}

static void setstartline( a_list *list )
{
    int             max;
    a_list_info     *box;

    max = uilistsize( list );
    box = list->box;

    if( box->line + box->area.height > max ) {
        if( max > box->area.height ) {
            box->line = max - box->area.height;
        } else {
            box->line = 0;
        }
    }
}

static bool selectoutofrange( a_list_info *box )
{
    return( box->row < box->line || box->row >= box->line + box->area.height );
}

bool checkitem( a_list *list, char typed, int index )
{
    char        first;

    if( (*list->get)( list->data, index, &first, 1 ) ) {
        if( isupper( first ) ) {
            first = tolower( first );
        }
        if( first == typed ) {
            list->choice = index;
            list->box->row = index;
            if( selectoutofrange( list->box ) ) {
                list->box->line = list->box->row;
                setstartline( list );
            }
            return( TRUE );
        }
    }
    return( FALSE );
}

void uiupdatelistbox( a_list *list )
{
    int     max;

    if( list->box == NULL ) {
        return;
    }
    max = uilistsize( list );
    list->box->row = list->choice;
    list->box->gadget.total_size = max( max, (int)list->box->area.height );
    uisetgadget( &list->box->gadget, list->choice );
    setstartline( list );
    if( selectoutofrange( list->box ) ) {
        list->box->line = list->box->row;
    }
    setstartline( list );
    uidrawgadget( &list->box->gadget );
    uipaintlistbox( list );
}

void uiboxpushlist( void )
{
    static EVENT    listboxevents[] = {
        'a',            'z',
        'A',            'Z',
        EV_NO_EVENT,
        EV_SCROLL_LINE_UP, EV_SCROLL_LINE_DOWN,
        EV_SCROLL_PAGE_UP, EV_SCROLL_PAGE_DOWN,
        EV_PAGE_UP, EV_PAGE_DOWN,
        EV_CURSOR_UP,   EV_CURSOR_DOWN,
        EV_MOUSE_PRESS, EV_MOUSE_RELEASE,
        EV_MOUSE_REPEAT, EV_MOUSE_DRAG,
        EV_ALT_CURSOR_UP, EV_NO_EVENT
    };

    uipushlist( listboxevents );
}

void uiboxpoplist( void )
{
    uipoplist();
}

static unsigned getlistsize( void *data, bool (*get)( void *, unsigned, char *,
                            unsigned ) )
{
    unsigned    i;

    for( i = 0; (*get)( data, i, NULL, 0 ) != FALSE; i++ );

    return( i );
}

unsigned uilistsize( a_list *list )
{
    if( list->data == NULL ) {
        return( 0 );
    }
    return( getlistsize( list->data, list->get ) );
}

void uimovelistbox( a_list *list, int row_diff, int col_diff )
{
    if( list ->box != NULL ) {
        uivmove( list->box->vs, list->box->vs->area.row + row_diff,
                 list->box->vs->area.col + col_diff );
    }
}

a_list_info *uibeglistbox( VSCREEN *vs, SAREA *area, a_list *list )
{
    a_list_info     *box;
    unsigned        max;

    box = uicalloc( 1, sizeof( a_list_info ) );
    if( box == NULL ) {
        return( NULL );
    }
    if( list->get == NULL ) {
        list->get = ( bool (*) ( void *, unsigned, char *, unsigned ) )
                        uigetlistelement;
    }

    box->vs     = vs;
    box->area   = *area;
    box->line   = list->choice;
    box->row    = list->choice;
    box->attr   = ATTR_EDIT;

    max = uilistsize( list );

    box->gadget.win = box->vs;          // Initialize gadget
    box->gadget.dir = VERTICAL;
    box->gadget.anchor = box->area.col + box->area.width + 1;
    box->gadget.start = box->area.row + 1;
    box->gadget.end = box->area.row + box->area.height;
    box->gadget.forward = EV_SCROLL_LINE_DOWN;
    box->gadget.backward = EV_SCROLL_LINE_UP;
    box->gadget.slider = EV_SCROLL_VERTICAL,
    box->gadget.pageforward = EV_SCROLL_PAGE_DOWN;
    box->gadget.pagebackward = EV_SCROLL_PAGE_UP;
    box->gadget.total_size = max( (int)max, (int)box->area.height );
    box->gadget.page_size = box->area.height;
    box->gadget.pos = 0;
    box->gadget.flags = GADGET_NONE;
    uiinitgadget( &box->gadget );

    list->box = box;
    setstartline( list );
    uipaintlistbox( list );

    return( box );
}

unsigned uiendlistbox( a_list *list )
{
    unsigned    k;

    if( list->box == NULL ) return( 0 );
    uiclose( list->box->vs );   // Shut down VSCREEN
    k = list->box->line;
    uifinigadget( &list->box->gadget );
    uifree( list->box );
    list->box = NULL;
    return( k );
}

static int getmouseregion( a_list *list, int *row, int *col )
{
    a_list_info     *box;

    box  = list->box;

    uimousepos( box->vs, row, col );

    if( *row - box->area.row >= (int)uilistsize( list ) ) {
        return( R_UNS );
    }
    if( *row >= box->area.height + box->area.row ) {
        return( R_DOWN );
    }
    if( *row < box->area.row ) {
        return( R_UP );
    }
    if( ( *col >= box->area.col + box->area.width ) ||
        ( *col <  box->area.col ) ) {
        return( R_UNS );
    }
    return( R_SEL );
}


EVENT charselect( EVENT ev, a_list *list )
{
    int         num;
    int         i;
    char        typed;

    typed = (char)ev;
    if( isupper( typed ) ) {
        typed = tolower( typed );
    }
    num = uilistsize( list );
    for( i = list->choice + 1; i < num; i++ ) {
        if( checkitem( list, typed, i ) ) {
            return( EV_LIST_BOX_CHANGED );
        }
    }
    for( i = 0; i < list->choice; i++ ) {
        if( checkitem( list, typed, i ) ) {
            return( EV_LIST_BOX_CHANGED );
        }
    }
    return( EV_NO_EVENT );
}

EVENT uilistbox( EVENT ev, a_list *list, bool permanent )
{
    int             listsize;
    int             maxline;
    int             newevent;
    a_list_info     *box;
    int             old_line;
    ORD             old_row;
    bool            close;

    if( Dclick ) {
        switch( ev ) {
        case EV_MOUSE_PRESS :
            Dclick = FALSE; /* must have gotten dlick without release */
            /* fall through */
        case EV_SCROLL_LINE_UP :
        case EV_SCROLL_LINE_DOWN :
        case EV_SCROLL_PAGE_UP :
        case EV_SCROLL_PAGE_DOWN :
        case EV_PAGE_UP :
        case EV_PAGE_DOWN :
        case EV_CURSOR_UP :
        case EV_CURSOR_DOWN :
            SelStart = FALSE;
            break;
        case EV_MOUSE_RELEASE :
            Dclick = FALSE;
            /* fall through */
        default :
            return( ev );
        }
    }
    close = FALSE;
    box = list->box;
    old_line = box->line;
    old_row = box->row;
    listsize = uilistsize( list );

    if( listsize > box->area.height ) {
        maxline = listsize - box->area.height;
    } else {
        maxline = 0;
    }

    if( maxline > 0 ) {
        ev = uigadgetfilter( ev, &box->gadget );
    }

    newevent = EV_NO_EVENT;
    switch( ev ) {
        case EV_MOUSE_DCLICK:
            Dclick = TRUE;
        case EV_MOUSE_PRESS:
        case EV_MOUSE_RELEASE:
        case EV_MOUSE_REPEAT:
        case EV_MOUSE_DRAG:
            {
                int         row, col, mpos;

                mpos = getmouseregion( list, &row, &col );
                newevent = ev;
                if( mpos == R_SEL ) {
                    SelStart = TRUE;
                    box->row  = (ORD) row - box->area.row;
                    box->row += box->line;
                }
                if( ev == EV_MOUSE_RELEASE ) {
                    if( mpos == R_SEL ) {
                        list->choice = list->box->row;
                        newevent = EV_LIST_BOX_CHANGED;
                    }
                    if( SelStart ) {
                        close = TRUE;
                        SelStart = FALSE;
                    }
                } else if( ev == EV_MOUSE_PRESS || ev == EV_MOUSE_DCLICK ) {
                    if( mpos == R_SEL ) {
                        if( ev == EV_MOUSE_DCLICK ) {
                            newevent = EV_LIST_BOX_DCLICK;
                        }
                    } else {
                        close = TRUE;
                    }
                } else if( mpos == R_UP  &&  box->line > 0  &&  SelStart ) {
                    box->line--;
                    box->row--;
                } else if( mpos == R_DOWN  &&  box->line < maxline ) {
                    box->line++;
                    box->row++;
                }
            }
            break;
        case EV_CURSOR_UP :
            if( box->row > 0 ) {
                if( box->row == box->line ) {
                    box->line--;
                }
                box->row--;
                list->choice = box->row;
                newevent = EV_LIST_BOX_CHANGED;
            }
            if( selectoutofrange( box ) ) {
                box->line = box->row;
                setstartline( list );
            }
            break;
        case EV_SCROLL_LINE_UP :
            if( box->line > 0 ) {
                box->line--;
            }
            break;
        case EV_CURSOR_DOWN :
            if( box->row < listsize - 1 ) {
                if( box->row - box->line == box->area.height - 1
                    && box->line < maxline ) {
                        ++box->line;
                }
                ++box->row;
                list->choice = box->row;
                newevent = EV_LIST_BOX_CHANGED;
            }
            if( selectoutofrange( box ) ) {
                box->line = box->row;
                setstartline( list );
            }
            break;
        case EV_SCROLL_LINE_DOWN :
            if( box->line < maxline ) {
                box->line++;
            }
            break;
        case EV_PAGE_UP :
            if( box->row == box->line ) {
                if( box->line < ( box->area.height - 1 ) ) {
                    box->line = 0;
                } else {
                    box->line -= ( box->area.height - 1 );
                }
                box->row -= old_line - box->line;
            } else {
                box->row = box->line;
            }
            if( box->row != old_row ) {
                list->choice = box->row;
                newevent = EV_LIST_BOX_CHANGED;
            }
            break;
        case EV_SCROLL_PAGE_UP :
            if( box->line < box->area.height ) {
                box->line = 0;
            } else {
                box->line -= box->area.height;
            }
            break;
        case EV_PAGE_DOWN :
            if( box->row == ( box->line + box->area.height - 1 ) ) {
                box->line += box->area.height - 1;
                if( box->line > maxline ) {
                    box->line = maxline;
                }
                box->row += ( box->line - old_line );
            } else {
                box->row = box->line + box->area.height - 1;
            }
            if( box->row != old_row ) {
                list->choice = box->row;
                newevent = EV_LIST_BOX_CHANGED;
            }
            break;
        case EV_SCROLL_PAGE_DOWN :
            box->line += box->area.height;
            if( box->line > maxline ) {
                box->line = maxline;
            }
            break;
        case EV_SCROLL_VERTICAL :
            box->line = box->gadget.pos;
            break;
        case EV_ALT_CURSOR_UP :
            close = TRUE;
            break;
        default :
            if( isalpha( ev ) ) {
                newevent = charselect( ev, list );
            } else {
                newevent = ev;
            }
            break;
    }
    if( ( old_line != box->line ) && ( maxline > 0 ) ) {
        uisetgadget( &box->gadget, box->line );
    }
    if( box->line != old_line || box->row != old_row ) {
        uipaintlistbox( list );
    }

    if( !permanent && close ) {
        uiendlistbox( list );
        newevent = EV_LIST_BOX_CLOSED;
    }
    return( newevent );
}
