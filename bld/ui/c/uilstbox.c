/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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

#include "clibext.h"


static bool SelStart =  false;    // user has moused over a valid choice
static bool Dclick =    false;    // true between DCLICK and MOUSE_RELEASE

typedef enum {
    R_UP,
    R_DOWN,
    R_UNS,
    R_SEL
} maction;

bool uigetlistelement( const void *data_handle, unsigned item, char *buff, unsigned buff_len )
/* if buffer is shorter then necessary then data is not null terminated
 * caller must handle this if need C null terminated string
 */
{
    const char **p;

    if( data_handle == NULL ) {
        return( false );
    }
    p = (const char **)data_handle;
    if( p[item] != NULL && p[0] != NULL ) {
        if( buff_len > 0 ) {
            strncpy( buff, p[item], buff_len );
        }
        return( true );
    } else {
        if( buff_len > 0 ) {
            *buff = '\0';
        }
        return( false );
    }
}

void uipaintlistbox( a_list *list )
{
    unsigned        i;
    char            *buf;
    bool            ok;
    unsigned        length;
    UIATTR          uiattr;
    UIPICKGETTEXT   *fn_get;

    if( list->box == NULL ) {
        return;
    }

    length = list->box->area.width;
    buf = (char *)uimalloc( length + 1 );
    fn_get = list->get;
    if( fn_get == NULL )
        fn_get = uigetlistelement;
    for( i = 0; i < list->box->area.height; ++i ) {
        uiattr = ATTR_NORMAL;
        if( list->box->row == i + list->box->line ) {
            uiattr = list->box->uiattr;
        }
        ok = (*fn_get)( list->data_handle, list->box->line + i, buf, length );
        /* buf does not have to be null terminated */
        /* terminate it at maximum length */
        buf[length] = '\0';
        if( ok ) {
            uitextfield( list->box->vs, list->box->area.row + i,
                         list->box->area.col, list->box->area.width,
                         UIData->attrs[uiattr], buf, strlen( buf ) );
        } else {
            break;
        }
    }
    for( ; i < list->box->area.height; ++i ) {
        uitextfield( list->box->vs, list->box->area.row + i,
                     list->box->area.col, list->box->area.width,
                     UIData->attrs[ATTR_NORMAL], "", 0 );
    }
    uifree( buf );
}

static void setstartline( a_list *list )
{
    unsigned        maxline;
    a_list_info     *box;

    maxline = uilistsize( list );
    box = list->box;

    if( box->line + box->area.height > maxline ) {
        if( maxline > box->area.height ) {
            box->line = maxline - box->area.height;
        } else {
            box->line = 0;
        }
    }
}

static bool selectoutofrange( a_list_info *box )
{
    return( box->row < box->line || box->row >= box->line + box->area.height );
}

static bool checkitem( a_list *list, int typed, unsigned index )
{
    char            chr[1];
    int             first;
    UIPICKGETTEXT   *fn_get;

    fn_get = list->get;
    if( fn_get == NULL )
        fn_get = uigetlistelement;
    if( (*fn_get)( list->data_handle, index, chr, sizeof( chr ) ) ) {
        first = (unsigned char)chr[0];
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
            return( true );
        }
    }
    return( false );
}

void uiupdatelistbox( a_list *list )
{
    unsigned    maxline;

    if( list->box == NULL ) {
        return;
    }
    maxline = uilistsize( list );
    list->box->row = list->choice;
    list->box->gadget.total_size = maxline;
    if( list->box->gadget.total_size < list->box->area.height )
        list->box->gadget.total_size = list->box->area.height;
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
    static ui_event listboxevents[] = {
        'a',            'z',
        'A',            'Z',
        __rend__,
        EV_SCROLL_LINE_UP,
        EV_SCROLL_LINE_DOWN,
        EV_SCROLL_PAGE_UP,
        EV_SCROLL_PAGE_DOWN,
        EV_PAGE_UP,
        EV_PAGE_DOWN,
        EV_CURSOR_UP,
        EV_CURSOR_DOWN,
        EV_MOUSE_PRESS,
        EV_MOUSE_RELEASE,
        EV_MOUSE_REPEAT,
        EV_MOUSE_DRAG,
        EV_ALT_CURSOR_UP,
        __end__
    };

    uipushlist( listboxevents );
}

void uiboxpoplist( void )
{
    uipoplist( /* listboxevents */ );
}

static unsigned getlistsize( const void *data_handle, UIPICKGETTEXT *fn_get )
{
    unsigned    item;

    if( fn_get == NULL )
        fn_get = uigetlistelement;
    for( item = 0; (*fn_get)( data_handle, item, NULL, 0 ); item++ )
        ;
    return( item );
}

unsigned uilistsize( a_list *list )
{
    if( list->data_handle == NULL ) {
        return( 0 );
    }
    return( getlistsize( list->data_handle, list->get ) );
}

void uimovelistbox( a_list *list, int row_diff, int col_diff )
{
    if( list->box != NULL ) {
        uivmove( list->box->vs, list->box->vs->area.row + row_diff,
                 list->box->vs->area.col + col_diff );
    }
}

a_list_info *uibeglistbox( VSCREEN *vs, SAREA *area, a_list *list )
{
    a_list_info     *box;
    unsigned        maxline;

    box = uimalloc( sizeof( a_list_info ) );
    if( box == NULL ) {
        return( NULL );
    }
    box->vs     = vs;
    box->area   = *area;
    box->line   = list->choice;
    box->row    = list->choice;
    box->uiattr = ATTR_EDIT;

    maxline = uilistsize( list );

    box->gadget.vs = box->vs;          // Initialize gadget
    box->gadget.dir = VERTICAL;
    box->gadget.anchor = box->area.col + box->area.width + 1;
    box->gadget.start = box->area.row + 1;
    box->gadget.end = box->area.row + box->area.height;
    box->gadget.forward = EV_SCROLL_LINE_DOWN;
    box->gadget.backward = EV_SCROLL_LINE_UP;
    box->gadget.slider = EV_SCROLL_VERTICAL,
    box->gadget.pageforward = EV_SCROLL_PAGE_DOWN;
    box->gadget.pagebackward = EV_SCROLL_PAGE_UP;
    box->gadget.total_size = maxline;
    if( box->gadget.total_size < box->area.height )
        box->gadget.total_size = box->area.height;
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

    if( list->box == NULL )
        return( 0 );
    uiclose( list->box->vs );   // Shut down VSCREEN
    k = list->box->line;
    uifinigadget( &list->box->gadget );
    uifree( list->box );
    list->box = NULL;
    return( k );
}

static maction getmouseregion( a_list *list, int *row, int *col )
{
    a_list_info     *box;

    box  = list->box;

    uimousepos( box->vs, row, col );

    if( *row >= box->area.row + uilistsize( list ) ) {
        return( R_UNS );
    }
    if( *row >= box->area.height + box->area.row ) {
        return( R_DOWN );
    }
    if( *row < box->area.row ) {
        return( R_UP );
    }
    if( ( *col >= box->area.col + box->area.width ) || ( *col <  box->area.col ) ) {
        return( R_UNS );
    }
    return( R_SEL );
}


static ui_event charselect( int typed, a_list *list )
{
    unsigned    num;
    unsigned    i;

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

ui_event uilistbox( ui_event ui_ev, a_list *list, bool permanent )
{
    unsigned        listsize;
    unsigned        maxline;
    ui_event        newevent;
    a_list_info     *box;
    unsigned        old_line;
    ORD             old_row;
    bool            close;

    if( Dclick ) {
        switch( ui_ev ) {
        case EV_MOUSE_PRESS :
            Dclick = false; /* must have gotten dlick without release */
            /* fall through */
        case EV_SCROLL_LINE_UP :
        case EV_SCROLL_LINE_DOWN :
        case EV_SCROLL_PAGE_UP :
        case EV_SCROLL_PAGE_DOWN :
        case EV_PAGE_UP :
        case EV_PAGE_DOWN :
        case EV_CURSOR_UP :
        case EV_CURSOR_DOWN :
            SelStart = false;
            break;
        case EV_MOUSE_RELEASE :
            Dclick = false;
            /* fall through */
        default :
            return( ui_ev );
        }
    }
    close = false;
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
        ui_ev = uigadgetfilter( ui_ev, &box->gadget );
    }

    newevent = EV_NO_EVENT;
    switch( ui_ev ) {
    case EV_MOUSE_DCLICK:
        Dclick = true;
        /* fall through */
    case EV_MOUSE_PRESS:
    case EV_MOUSE_RELEASE:
    case EV_MOUSE_REPEAT:
    case EV_MOUSE_DRAG:
        {
            int     row, col;
            maction mpos;

            mpos = getmouseregion( list, &row, &col );
            newevent = ui_ev;
            if( mpos == R_SEL ) {
                SelStart = true;
                box->row  = (ORD)row - box->area.row;
                box->row += box->line;
            }
            if( ui_ev == EV_MOUSE_RELEASE ) {
                if( mpos == R_SEL ) {
                    list->choice = list->box->row;
                    newevent = EV_LIST_BOX_CHANGED;
                }
                if( SelStart ) {
                    close = true;
                    SelStart = false;
                }
            } else if( ui_ev == EV_MOUSE_PRESS || ui_ev == EV_MOUSE_DCLICK ) {
                if( mpos == R_SEL ) {
                    if( ui_ev == EV_MOUSE_DCLICK ) {
                        newevent = EV_LIST_BOX_DCLICK;
                    }
                } else {
                    close = true;
                }
            } else if( mpos == R_UP && box->line > 0 && SelStart ) {
                box->line--;
                box->row--;
            } else if( mpos == R_DOWN && box->line < maxline ) {
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
            if( box->row == box->line + box->area.height - 1 && box->line < maxline ) {
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
            if( box->line + 1 < box->area.height ) {
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
        close = true;
        break;
    default :
        if( iseditchar( ui_ev ) && isalpha( (unsigned char)ui_ev ) ) {
            newevent = charselect( (unsigned char)ui_ev, list );
        } else {
            newevent = ui_ev;
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
