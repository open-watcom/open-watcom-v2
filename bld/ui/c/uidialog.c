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


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include "uidef.h"
#include "uimenu.h"
#include "uidialog.h"
#include "uigchar.h"
#include "clibext.h"

#define CTRL_BUF_LEN    80

typedef EVENT           an_event;

static bool exit_field( a_dialog *info, VFIELD *field )
{
    bool                flag;
    an_edit_control     *edit;
    a_list              *list;
    a_combo_box         *combo;

    flag = FALSE;
    if( field != NULL ) {
        switch( field->typ ) {
        case FLD_EDIT :
        case FLD_INVISIBLE_EDIT:
            if( info->edit_data != NULL ) {
                edit = field->ptr;
                if( uieditisdirty() ) {
                    flag = TRUE;
                }
                uiedittrim( info->edit_data->edit_buffer );
                edit->buffer = info->edit_data->edit_buffer;
                edit->length = info->edit_data->edit_eline.length;
                uiendedit();
                info->edit_data = NULL;
            }
            uinocursor( info->vs );
            break;
        case FLD_COMBOBOX :
            combo = field->ptr;
            list = &combo->list;
            edit = &combo->edit;

            if( !combo->perm ) {
                uiendlistbox( list );           // Shut down listbox
            }
            if( info->edit_data != NULL ) {
                if( uieditisdirty() ) {
                    flag = TRUE;
                }
                uiedittrim( info->edit_data->edit_buffer );
                edit->buffer = info->edit_data->edit_buffer;
                edit->length = info->edit_data->edit_eline.length;
                uiendedit();
                info->edit_data = NULL;
            }
            uinocursor( info->vs );
            break;
        case FLD_PULLDOWN :
            list = field->ptr;
            uiendlistbox( list );
            break;
        case FLD_LISTBOX :
            list = field->ptr;
            list->box->attr = ATTR_EDIT;
            uipaintlistbox( list );
            break;
        case FLD_EDIT_MLE :
            list = field->ptr;
            list->box->attr = ATTR_NORMAL;
            uipaintlistbox( list );
            break;
        }
    }
    return( flag );
}

static bool notintab( VFIELD *fld )
{
    return( fld->typ < FLD_HOT );
}

static bool radiooff( VFIELD *fld )
{
    a_radio             *r;

    if( fld->typ == FLD_RADIO ) {
        r = fld->ptr;
        if( r->value != r->group->value ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static VFIELD *nextfield( VFIELD *fld )
{
    while( notintab( ++fld ) ) {                // unselectable field types!
        if( fld->typ == FLD_VOID ) {
            fld = NULL;
            break;
        }
    }
    return( fld );
}

static void print_field( VSCREEN *vs, VFIELD *field, unsigned current )
{
    SAREA               *area;
    char                *str;
    ATTR                attr;
    ATTR                hotattr;
    char                ctrlbuf[CTRL_BUF_LEN+1];
    unsigned            length = 0;
    a_check             *check = NULL;
    a_radio             *radio = NULL;
    a_list              *list;
    a_combo_box         *combo;
    a_hot_spot          *hotspot;
    an_edit_control     *edit;
    VSCREEN             *c_vs;
    SAREA               c_area;
    bool                use_hottext;
    char                hotkey;

    if( field == NULL ) return;
    area = &field->area;
    str = NULL;
    use_hottext = FALSE;
    memset( ctrlbuf, '\0', CTRL_BUF_LEN+1 );

    attr = UIData->attrs[ current ? ATTR_CURR_EDIT : ATTR_EDIT ];

    switch( field->typ ) {
    case FLD_HOT :
        hotspot = field->ptr;
        if( current ) {
            hotspot->flags |= HOT_CURRENT ;
        } else {
            hotspot->flags &= (~HOT_CURRENT);
        }
        hotspot->flags |= uihotspot( vs, hotspot->str,
                            &field->area, hotspot->flags );
        return;                     // don't want to print anything
    case FLD_TEXT :
        attr = UIData->attrs[ ATTR_NORMAL ];
        str  = field->ptr;
        break;
    case FLD_LABEL :
        attr = UIData->attrs[ ATTR_NORMAL ];
        strcpy( ctrlbuf, (char *)field->ptr );
        strcat( ctrlbuf, ":" );
        length = area->width;
        break;
    case FLD_FRAME :
        uidrawbox( vs, area, UIData->attrs[ ATTR_NORMAL ], field->ptr );
        return;
    case FLD_EDIT :
    case FLD_INVISIBLE_EDIT :
        edit = field->ptr;
        if( edit->buffer != NULL ) {
            length = edit->length;
            if( length > CTRL_BUF_LEN )
                length = CTRL_BUF_LEN;
            if( length > area->width ) {
                length = area->width;
            }
            if( field->typ == FLD_INVISIBLE_EDIT ) {
                memset( ctrlbuf, '*', length );
            } else {
                strncpy( ctrlbuf, edit->buffer, length );
            }
        } else {
            length = 0;
        }
        break;
    case FLD_COMBOBOX :
        combo = field->ptr;
        edit  = &combo->edit;
        list  = &combo->list;

        if( combo->perm == FALSE ) {
            ctrlbuf[0] = UiGChar[ UI_ARROW_DOWN ];  /* JBS was 25 */
            uivtextput( vs, area->row, area->col + area->width + 1,
                        UIData->attrs[ ATTR_SCROLL_ICON ], ctrlbuf, 1 );
            ctrlbuf[0] = '\0';
        }
        if( edit->buffer != NULL ) {
            length = edit->length;
            if( length > CTRL_BUF_LEN )
                length = CTRL_BUF_LEN;
            strncpy( ctrlbuf, edit->buffer, length );
        } else {
            length = 0;
        }
        if( list->get == NULL ) {
            list->get = uigetlistelement;
        }
        if( list->box == NULL  &&  combo->perm == TRUE ) {
            c_area = *area;
            c_area.row += vs->area.row + 2;
            c_area.col += vs->area.col + 1;
            c_vs = uiopen( &c_area, NULL, V_DIALOGUE | V_LISTBOX );
            if( c_vs == NULL ) {
                break;
            }
            c_area.row = 0;
            c_area.col = 0;
            list->box = uibeglistbox( c_vs, &c_area, list );
        }
        break;
    case FLD_PULLDOWN :
        list = field->ptr;
        ctrlbuf[0] = UiGChar[ UI_ARROW_DOWN ];  /* JBS was 25 */
        uivtextput( vs, area->row, area->col + area->width,
                    UIData->attrs[ ATTR_SCROLL_ICON ], ctrlbuf, 1 );
        ctrlbuf[0] = '\0';
        if( list->get == NULL ) {
            list->get = uigetlistelement;
        }
        (*list->get)( list->data, list->choice, ctrlbuf, area->width );
        length = area->width;
        break;
    case FLD_LISTBOX:
    case FLD_EDIT_MLE:
        list = field->ptr;
        if( list->box == NULL ) {
            c_area = *area;
            c_area.row += ((VSCREEN *)vs)->area.row;
            c_area.col += ((VSCREEN *)vs)->area.col;
            c_vs = uiopen( &c_area, NULL, V_DIALOGUE | V_LISTBOX );
            if( c_vs == NULL ) {
                break;
            }
            c_area.row = 0;
            c_area.col = 0;
            list->box = uibeglistbox( c_vs, &c_area, list );
        }
        return;
    case FLD_CHECK:
        attr = UIData->attrs[ current ? ATTR_CURR_EDIT : ATTR_NORMAL ];
        check = field->ptr;

        ctrlbuf[0] = UiGChar[ UI_CHECKBOX_LEFT ];
        if( _checked( check ) ) {
            ctrlbuf[1] = UiGChar[ UI_CHECKBOX_FULL ];
        } else {
            ctrlbuf[1] = UiGChar[ UI_CHECKBOX_EMPTY ];
        }
        ctrlbuf[2] = UiGChar[ UI_CHECKBOX_RIGHT ];
        ctrlbuf[3] = ' ';

        strncat( ctrlbuf, check->str, CTRL_BUF_LEN - 4 );
        length = strlen( ctrlbuf );
        use_hottext = TRUE;
        break;
    case FLD_RADIO:
        attr = UIData->attrs[ current ? ATTR_CURR_EDIT : ATTR_NORMAL ];
        radio = field->ptr;

        ctrlbuf[0] = UiGChar[ UI_RADIO_LEFT ];
        if( radio->value == radio->group->value ) {
            ctrlbuf[1] = UiGChar[ UI_RADIO_FULL ];
        } else {
            ctrlbuf[1] = UiGChar[ UI_RADIO_EMPTY ];
        }
        ctrlbuf[2] = UiGChar[ UI_RADIO_RIGHT ];
        ctrlbuf[3] = ' ';

        strncat( ctrlbuf, radio->str, CTRL_BUF_LEN - 4 );
        length = strlen( ctrlbuf );
        use_hottext = TRUE;
        break;
    }
    if( str != NULL ) {
        uivtextput( vs, area->row, area->col, attr, str, area->width );
    } else if( use_hottext ) {
        if( current ) {
            hotattr = attr;
        } else {
            hotattr = UIData->attrs[ ATTR_RADIO_HOTSPOT ];
        }
        hotkey = uidrawhottext( vs, ctrlbuf, area, attr, hotattr, FALSE, FALSE, FALSE );
        if( field->typ == FLD_CHECK ) {
            check->hotkey = hotkey;
        } else if( field->typ == FLD_RADIO ) {
            radio->hotkey = hotkey;
        }
    } else {
        uitextfield( vs, area->row, area->col, area->width, attr,
                     ctrlbuf, length );
    }
}

void uiprintfield( a_dialog *dialog, VFIELD *field )
{
    print_field( dialog->vs, field, field == dialog->curr );
}

static void *makevs( char *heading, int cols, int rows, int cpos, int rpos )
{
    SAREA               area;

    uiposition( &area, rows, cols, rpos, cpos, TRUE );
    return( uiopen( &area, heading, V_DIALOGUE ) );
}

unsigned ui_split_line( char **sptr, char *t, unsigned max )
{
// Attempt to split the line at an appropriate place
//
//  \n forces the line to be split even when everything fits
//  \r will be a favoured place to split when things don't fit

    unsigned            delim, cr, i;
    char                *str;
    unsigned            len;
    unsigned            slen = 0;
    char                ch = '\0';

    str = *sptr;
    len = t - str;
    if( len < max ) max = len;
    for( delim = 0, cr = 0, i = 0 ; i < max ; ++i ) {
        ch = str[ i ];
        if( ch == '\n' ) {
            break;
        } else if( ch == '\r' ) {
            str[ i ] = ' ';
            cr = i;
        } else if( ch == ' ' || ch == ',' || ch == ')' ) {
            delim = i;
        }
    }
    if( ch == '\n' ) {            /* forced new line */
        max = i+1;
        slen = i;
    } else if( len == max ) {     /* everything fit */
        slen = max;
    } else if( cr > 0 ) {         /* split at the carriage return */
        str[ cr ] = '\r';
        max = cr+1;
        slen = cr;
    } else if( delim > 0 ) {      /* split at the space, comma, or paren */
        max = delim+1;
        slen = max;
    }
    str += max;
    *sptr = str;
    return( slen );
}

void *uiinitdialog( char *heading, ATTR attr, char *lines[],
            unsigned int extra_rows, int maxlen, int rpos, int cpos )
{
    VSCREEN             *vs;
    int                 len;
    unsigned            width;
    int                 i;
    unsigned            depth;
    unsigned            linelen;
    char                *s, *t, *line;
    SAREA               area;

    uisetscreenarea( &area, TRUE, FALSE );
    width = UIData->width - 2;
    for( depth = 0, i = 0 ; lines[ i ] != NULL ; ++i ) {
        s = lines[ i ];
        len = strlen( s );
        for( t = s + len ; s < t ; ) {
            ++ depth;
            linelen = ui_split_line( &s, t, width );
            if( maxlen < linelen + 2 ) {
                maxlen = linelen + 2;
            }
        }
    }
    if( depth > 0 ) {
        /* never put text on the first line of the dialog */
        ++depth;
    }
    if( maxlen > width )
        maxlen = width;
    vs = makevs( heading, maxlen, depth + extra_rows, cpos, rpos );
    uisetarea( &area, vs );
    width = area.width;
    area.row = 1;
    area.height = 1;
    for( i = 0 ; lines[ i ] != NULL ; ++i ) {
        s = lines[ i ];
        len = strlen( s );
        for( t = s + len ; s < t ; ) {
            line = s;
            linelen = ui_split_line( &s, t, width );
            uicntrtext( vs, &area, attr, linelen, line );
            area.row += 1;
        }
    }
    return( vs );
}

void uifinidialog( void *vs )
{
    uiclose( vs );
}

static void enter_field( a_dialog *info, VFIELD *field )
{
    an_edit_control     *edit;
    a_combo_box         *combo;
    a_list              *list;
    SAREA               area;

    if( field == NULL )  return;

    area = field->area;
    switch( field->typ ) {
    case FLD_EDIT :
    case FLD_INVISIBLE_EDIT :
    case FLD_COMBOBOX :
        if( field->typ == FLD_COMBOBOX ) {
            combo = field->ptr;
            edit  = &combo->edit;
        } else {
            edit = field->ptr;
        }
        if( info->edit_data == NULL ) {
            info->edit_data = uibegedit( info->vs, area.row, area.col, area.width,
                                  UIData->attrs[ ATTR_CURR_EDIT ],
                                  edit->buffer, edit->length, 0, 0, TRUE, 0,
                                  field->typ == FLD_INVISIBLE_EDIT );
        }
        break;
    case FLD_LISTBOX :
        list = field->ptr;
        list->box->attr = ATTR_CURR_EDIT;
        uipaintlistbox( list );
        break;
    case FLD_EDIT_MLE :
        list = field->ptr;
        list->box->attr = ATTR_NORMAL;
        uipaintlistbox( list );
        break;
    }
}

void uireinitdialog( a_dialog *info, VFIELD *fields )
{
    unsigned            i;

    uiposnhotspots( info->vs, fields );
    info->dirty = FALSE;
    info->first = NULL;
    info->other = NULL;
    info->fields = fields;

    info->first = &fields[0];
    /* set first to be first field in tab sequence */
    while( notintab( info->first ) ) {
        if( info->first->typ == FLD_VOID ) {
            info->first = NULL;
            break;
        }
        ++(info->first);
    }
    info->curr = info->first;
    if( info->first != NULL ) {
        /* set curr to first field in tab sequence not an unset radio button */
        while( radiooff( info->curr ) ) {
            info->curr = nextfield( info->curr );
        }
    }

    for( i = 0 ; fields[i].typ != FLD_VOID ; ++i ) {
        print_field( info->vs, &fields[i], &fields[i] == info->curr );
    }
    enter_field( info, info->curr );
}

bool uigetdialogarea( a_dialog *dialog, SAREA *area )
{
    VSCREEN     *vs;

    if( dialog != NULL && area != NULL ) {
        vs = (VSCREEN *)dialog->vs;
        *area = vs->area;
        return( TRUE );
    }
    return( FALSE );
}

void *uibegdialog( char *title, VFIELD *fields, ORD rows, ORD cols, int rpos, int cpos )
{
    char                *lines[1];
    a_dialog            *info;

    lines[ 0 ] = NULL;
    info = uicalloc( 1, sizeof( a_dialog ) );
    if( info == NULL ) {
        return( NULL );
    }
    info->vs = uiinitdialog( title, UIData->attrs[ ATTR_NORMAL ],
                           lines, rows, cols, rpos, cpos );
    uireinitdialog( info, fields );
    return( info );
}

static void do_radio( a_dialog *info, VFIELD *field )
{
    a_radio *cur_radio;
    VFIELD  *fields;
    a_radio *radio;
    int i;

    fields = info->fields;
    // might want to use info->first, kind of unsure
    cur_radio = field->ptr;
    if( cur_radio->value == cur_radio->group->value ) return;

    info->dirty = TRUE;

    for( i = 0 ; fields[i].typ != FLD_VOID ; ++i ) {
        if( fields[i].typ == FLD_RADIO ) {
            radio = fields[i].ptr;
            if( radio->group == cur_radio->group  &&
                radio->value == radio->group->value ) {
                radio->group->value = cur_radio->value;
                print_field( info->vs, &fields[i], FALSE );
                break;
            }
        }
    }
    cur_radio->group->value = cur_radio->value;
    print_field( info->vs, field, TRUE );
}

void uiupdatecombobox( a_combo_box *combo )
{
    char                *str;
    a_list              *list;
    an_edit_control     *edit;

    edit  = &combo->edit;
    list  = &combo->list;
    str = (char *)uimalloc( CTRL_BUF_LEN );
    if( str != NULL ) {
        if( (*list->get)( list->data, list->choice, str, CTRL_BUF_LEN ) ) {
            uifree( edit->buffer );
            edit->buffer = str;
            edit->length = strlen( str );
        } else {
            uifree( str );
        }
    }
}

void uiupdateedit( a_dialog *info, VFIELD *field )
{
    an_edit_control     *edit;
    a_combo_box         *combo;

    if( ( field == NULL ) || ( info == NULL ) || ( field != info->curr ) ) {
        return;
    }
    edit = NULL;
    switch( field->typ ) {
    case FLD_COMBOBOX :
        combo = field->ptr;
        edit = &combo->edit;
        break;
    case FLD_EDIT :
    case FLD_INVISIBLE_EDIT:
        edit = field->ptr;
        break;
    }
    if( edit != NULL ) {
        if( info->edit_data != NULL ) {
            uiendedit();
            info->edit_data = NULL;
        }
        enter_field( info, info->curr );
    }
}

static  void setcombobuffer( a_dialog *info, VFIELD *fld )
{
    a_combo_box         *combo;
    an_edit_control     *edit;

    combo = fld->ptr;
    edit  = &combo->edit;
    uiupdatecombobox( combo );
    if( info->edit_data != NULL ) {
        info->edit_data->edit_buffer = edit->buffer;
        info->edit_data->edit_eline.buffer = edit->buffer;
        info->edit_data->edit_eline.length = edit->length;
        info->edit_data->edit_eline.update = TRUE;
        info->edit_data->edit_eline.index = 0;
        info->edit_data->edit_eline.scroll = 0;
    }
}

EVENT   pulldownfilter( EVENT ev, a_dialog *info )
{
    a_list          *list = NULL;
    a_combo_box     *combo;

    VSCREEN     *vs;
    SAREA       area;
    VFIELD      *fld;

    fld = info->curr;

    if( fld->typ == FLD_PULLDOWN ) {
        list = fld->ptr;
    } else if( fld->typ == FLD_COMBOBOX ) {
        combo = fld->ptr;
        list = &combo->list;
    }
    if( list->get == NULL ) {
        list->get = uigetlistelement;       // set get_element function
    }
    switch( ev ) {
    case EV_MOUSE_DCLICK:
    case EV_MOUSE_PRESS:
    {
        ORD             row, col;

        uivmousepos( info->vs, &row, &col );
        area = fld->area;
        area.width += 1;    /* extra column for button */
        if( fld->typ == FLD_COMBOBOX ) {
            /* mouse press must be on little button at right */
            area.col += area.width;
            area.width = 1;
        }
        if( fld->typ == FLD_COMBOBOX || fld->typ == FLD_PULLDOWN ) {
            /* mouse press must be on top line */
            area.height = 1;
        }
        if( row < area.row  || row >= area.row + area.height ) break;
        if( col < area.col  || col >= area.col + area.width ) break;
        /* FALLS into next case */
    }
    case EV_CURSOR_DOWN:
    case EV_ALT_CURSOR_DOWN:
        /* list->box must be null */
        area = fld->area;
        vs   = info->vs;
        area.row += ( vs->area.row + 2 );
        area.col += vs->area.col;
        if( fld->typ == FLD_COMBOBOX ) {
            area.col++;
        }
        if( area.row + area.height >= UIData->height ) {
            area.row -= area.height + 3;
        }
        vs = uiopen( &area, NULL, V_DIALOGUE | V_LISTBOX );
        area.row = 0;
        area.col = 0;
        list->box = uibeglistbox( vs, &area, list );
        if( ev != EV_MOUSE_PRESS && ev != EV_MOUSE_DCLICK
                        && fld->typ == FLD_COMBOBOX ) {
            info->dirty = TRUE;
            setcombobuffer( info, fld );
            print_field( info->vs, fld, TRUE );
        }
        break;
    }
    return( ev );
}

static void *forwardtab( a_dialog *info )
{
    VFIELD             *fld;

    if( info->curr == NULL ){
        fld = info->first;
    } else {
        fld = nextfield( info->curr );
        if( fld == NULL ) {
            fld = info->first;
        }
    }
    while( radiooff( fld ) ) {
        fld = nextfield( fld );
    }
    return( fld );
}

static void *backwardtab( a_dialog *info )
{
    VFIELD      *fld, *hold;

    hold = NULL;
    fld = info->first;
    while( fld != info->curr ) {
        if( !radiooff( fld ) ) {
            hold = fld;
        }
        fld = nextfield( fld );
    }
    if( hold == NULL ) {        /* wrap */
        while( fld != NULL ) {
            if( !radiooff( fld ) ) {
                hold = fld;
            }
            fld = nextfield( fld );
        }
    }
    return( hold );
}

extern void uidialogexitcurr( a_dialog *info )
{
    /* you must call this function to set info->dirty or to do  */
    /* blank trimming on an edit control                        */
    if( exit_field( info, info->curr ) ) {
        info->dirty = TRUE;
    }
}

extern void uidialogsetcurr( a_dialog *info, VFIELD *curr )
{
    VFIELD      *other;

    other = info->curr;
    if( other != curr ) {
        info->other = info->curr;
        info->curr  = curr;
        if( exit_field( info, other ) ) {
            info->dirty = TRUE;
        }
        print_field( info->vs, other, FALSE );
        enter_field( info, curr );
        print_field( info->vs, curr, TRUE );
        uidialogchangefield( info );
    }
}

void uimovefield( a_dialog *info, VFIELD *curr, int row_diff, int col_diff )
{
    a_combo_box *combo;
    a_list      *list;

    info = info;

    switch( curr->typ ) {
    case FLD_COMBOBOX :
        combo = curr->ptr;
        if( combo->perm ) {
            uimovelistbox( &combo->list, row_diff, col_diff );
        } else {
            uiendlistbox( &combo->list );
        }
        break;
    case FLD_LISTBOX :
    case FLD_EDIT_MLE :
        list = (a_list *)curr->ptr;
        uimovelistbox( list, row_diff, col_diff );
        if( list->box != NULL ) {
            uivsetactive( list->box->vs );
        }
        break;
    case FLD_PULLDOWN :
        uiendlistbox( (a_list *)curr->ptr );
        break;
    }
}

void uiredrawdialog( a_dialog *info )
{
    VSCREEN     *vs;
    int         i;

    vs = (VSCREEN *)info->vs;

    uivclose( vs );
    uivopen( vs );

    for( i = 0 ; info->fields[i].typ != FLD_VOID ; ++i ) {
        print_field( info->vs, &info->fields[i],
                     &info->fields[i] == info->curr );
    }
}

bool uiresizedialog( a_dialog *info, SAREA *new_area )
{
    int         row_diff;
    int         col_diff;
    VSCREEN     *vs;
    VFIELD      *curr;
    int         i;
    bool        resize;

    vs = (VSCREEN *)info->vs;

    if( new_area->row < 2 ) {
        new_area->row = 2;
    }
    if( new_area->col < 1 ) {
        new_area->col = 1;
    }
    if( new_area->row + vs->area.height >= UIData->height ) {
        new_area->row = UIData->height - vs->area.height - 1;
    }
    if( new_area->col + vs->area.width >= UIData->width ) {
        new_area->col = UIData->width - vs->area.width - 1;
    }
    row_diff = new_area->row - vs->area.row;
    col_diff = new_area->col - vs->area.col;
    resize = ( new_area->width != vs->area.width ) ||
             ( new_area->height != vs->area.height );
    if( resize ) {
        uivclose( vs );
        vs->area = *new_area;
        uivopen( vs );
    } else {
        uivmove( info->vs, new_area->row, new_area->col );
    }
    /* close all open pull down boxes */
    for( curr = info->fields; curr->typ != FLD_VOID; curr++ ) {
        uimovefield( info, curr, row_diff, col_diff );
    }
    if( resize ) {
        for( i = 0 ; info->fields[i].typ != FLD_VOID ; ++i ) {
            print_field( info->vs, &info->fields[i],
                         &info->fields[i] == info->curr );
        }
    }
    return( TRUE );
}

static  ORD     PrevRow = 0;
static  ORD     PrevCol = 0;

static EVENT uicheckmove( EVENT ev, a_dialog *info )
{
    EVENT       new_ev;
    ORD         row;
    ORD         col;
    SAREA       new_area;
    VSCREEN     *vs;
    int         prev_row;
    int         prev_col;

    new_ev = ev;
    switch( ev ) {
    case EV_MOUSE_PRESS :
        if( uivmousepos( NULL, &row, &col ) == info->vs ) {
            vs = (VSCREEN *)info->vs;
            if( ( row + 1 ) == vs->area.row ) { /* because it's framed */
                PrevRow = row;
                PrevCol = col;
                info->moving = TRUE;
                new_ev = EV_NO_EVENT;
            }
        }
        break;
    case EV_MOUSE_DRAG :
        if( info->moving ) {
            uivmousepos( NULL, &row, &col );
            if( ( row != PrevRow ) || ( col != PrevCol ) ) {
                vs = (VSCREEN *)info->vs;
                if( vs->area.row + row < PrevRow ) {
                    new_area.row = 0;
                } else {
                    new_area.row = vs->area.row + row - PrevRow;
                }
                if( vs->area.col + col < PrevCol ) {
                    new_area.col = 0;
                } else {
                    new_area.col = vs->area.col + col - PrevCol;
                }
                new_area.width = vs->area.width;
                new_area.height = vs->area.height;
                prev_row = vs->area.row;
                prev_col = vs->area.col;
                uiresizedialog( info, &new_area );
                /* new_area may have been adjusted */
                PrevRow += new_area.row - prev_row;
                PrevCol += new_area.col - prev_col;
            }
            new_ev = EV_NO_EVENT;
        }
        break;
    case EV_MOUSE_RELEASE :
        if( info->moving ) {
            info->moving = FALSE;
            new_ev = EV_NO_EVENT;
        }
    }
    return( new_ev );
}

static EVENT uitabkey( EVENT ev, a_dialog *info )
{
    VFIELD          *curr, *fld;
    ORD             row, col;
    SAREA           area;
    EVENT           newev;

    if( info->first == NULL ) return( FALSE );
    curr = info->curr;
    newev = ev;
    switch( ev ){
    case EV_MOUSE_DCLICK:
    case EV_MOUSE_PRESS:
    {
        a_combo_box         *combo;
        a_list              *list;
        VSCREEN             *mousevs;

        mousevs = uivmousepos( info->vs, &row, &col );
        fld = info->first;
        while( fld != NULL ) {
            list = NULL;
            area = fld->area;
            if( fld->typ == FLD_PULLDOWN ) {
                list = fld->ptr;
                area.height = 1;
                area.width += 1;    /* pulldown button */
            } else if( fld->typ == FLD_LISTBOX ) {
                list = fld->ptr;
            } else if( fld->typ == FLD_EDIT_MLE ) {
                list = fld->ptr;
            } else if( fld->typ == FLD_COMBOBOX ) {
                area.height = 1;
                area.width += 2;    /* pulldown button */
                combo = fld->ptr;
                list = &combo->list;
            }
            if( mousevs != info->vs ) {
                if( list != NULL && list->box != NULL ) {
                    if( list->box->vs == mousevs ) {
                        break;
                    }
                }
            } else if( row >= area.row  && row < area.row + area.height &&
                        col >= area.col  && col < area.col + area.width ) {
                break;
            }
            fld = nextfield( fld );
        }
        /* check boxes don't get mouse events unless mouse over them */
        if( fld == NULL && curr != NULL && curr->typ == FLD_CHECK ) {
            newev = EV_NO_EVENT;
        } else {
            curr = fld;
        }
        break;
    }
    case EV_TAB_FORWARD :
        curr = forwardtab( info );
        break;
    case EV_TAB_BACKWARD :
        curr = backwardtab( info );
        break;
    case EV_CURSOR_RIGHT :
    case EV_CURSOR_DOWN :
    {
        a_radio             *r1, *r2;

        if( curr!= NULL && curr->typ == FLD_RADIO ) {
            fld = nextfield( curr );
            if( fld != NULL ) {
                r1 = curr->ptr;
                r2 = fld->ptr;
                if( r1->group == r2->group ) {
                    curr = fld;
                    do_radio( info, fld );
                    newev = EV_CHECK_BOX_CLICK;
                }
            }
        }
        break;
    }
    case EV_CURSOR_LEFT :
    case EV_CURSOR_UP :
    {
        a_radio             *r1, *r2;

        if( curr!= NULL && curr->typ == FLD_RADIO ) {
            if( curr != info->first ) {
                fld = curr - 1;
                r1 = curr->ptr;
                r2 = fld->ptr;
                if( r1->group == r2->group ) {
                    curr = fld;
                    do_radio( info, fld );
                    newev = EV_CHECK_BOX_CLICK;
                }
            }
        }
        break;
    }
    }
    if( info->curr != curr ) {
        uidialogsetcurr( info, curr );
    }
    return( newev );
}

EVENT uihotkeyfilter( a_dialog *info, EVENT ev )
{
    char        ch, hotkey;
    VFIELD      *vf;

    /* is the event a key press or alt-key press */
    if( ev < 0x100 && isalpha( ev ) ) {
        ch = tolower( ev );
    } else {
        ch = uialtchar( ev );
    }

    if( ch ) {
        hotkey = '\0';
        for( vf = info->fields ; vf->typ != FLD_VOID; ++vf ){
            switch( vf->typ ) {
                case FLD_HOT:
                    hotkey = (char)((a_hot_spot *)vf->ptr)->flags & HOT_CHAR;
                    break;
                case FLD_CHECK:
                    hotkey = (char)((a_check *)vf->ptr)->hotkey;
                    break;
                case FLD_RADIO:
                    hotkey = (char)((a_radio *)vf->ptr)->hotkey;
                    break;
                default:
                    hotkey = '\0';
            }
            if( hotkey == ch ) {
                break;
            }
        }

        /* make sure the new field is hilighted */
        if( info->curr != vf && vf->typ != FLD_VOID ) {
            uidialogsetcurr( info, vf );
        }

        if( hotkey == ch ) {
            switch( vf->typ ) {
                case FLD_HOT:
                    ev = ((a_hot_spot *)vf->ptr)->event;
                    break;
                case FLD_CHECK:
                    info->dirty = TRUE;
                    ((a_check *)vf->ptr)->val = !((a_check *)vf->ptr)->val;
                    print_field( info->vs, vf, TRUE );
                    ev = EV_CHECK_BOX_CLICK;
                    break;
                case FLD_RADIO:
                    do_radio( info, vf );
                    ev = EV_CHECK_BOX_CLICK;
                    break;
            }
        }
    }

    return( ev );
}

EVENT uiprocessdialogevent( EVENT ev, a_dialog *info )
{
    VFIELD              *field;
    a_check             *check;
    an_edit_control     *edit;
    unsigned            choice;
    a_combo_box         *combo;
    a_list              *list;

    ev = uicheckmove( ev, info );
    ev = uitabkey( ev, info );
    field = info->curr;
    if( field != NULL ) {
        switch( field->typ ) {
        case FLD_CHECK:
        case FLD_RADIO:
            switch( ev ) {
            case EV_MOUSE_DCLICK:
            case EV_MOUSE_PRESS:
            case ' ' :
                ev = EV_CHECK_BOX_CLICK;
                if( field->typ == FLD_RADIO ) {
                    do_radio( info, field );
                    break;
                }
                if( field->typ == FLD_CHECK ) {
                    info->dirty = TRUE;
                    check = field->ptr;
                    check->val = !check->val;
                    print_field( info->vs, field, TRUE );
                    break;
                }
            }
            break;
        case FLD_EDIT:
        case FLD_INVISIBLE_EDIT:
            ev = uiledit( ev );
            edit = field->ptr;
            if( info->edit_data != NULL ) {
                edit->buffer = info->edit_data->edit_buffer;
                edit->length = info->edit_data->edit_eline.length;
            }
            break;
        case FLD_LISTBOX:
        case FLD_EDIT_MLE:
            list = field->ptr;
            ev = uilistbox( ev, list, TRUE );
            break;
        case FLD_PULLDOWN:
            list = field->ptr;
            if( list->box == NULL ) {
                ev = pulldownfilter( ev, info );
            } else {
                choice = list->choice;
                ev = uilistbox( ev, list, FALSE );
                if( choice != list->choice ) {
                    info->dirty = TRUE;
                    print_field( info->vs, field, TRUE );
                }
            }
            break;
        case FLD_COMBOBOX:
            combo = field->ptr;
            list  = &combo->list;
            edit  = &combo->edit;
            if( list->box == NULL ) {
                ev = pulldownfilter( ev, info );
            } else {
                choice = list->choice;
                ev = uilistbox( ev, list, combo->perm );
                if( choice != list->choice ) {
                    info->dirty = TRUE;
                    setcombobuffer( info, field );
                    print_field( info->vs, field, TRUE );
                }
            }
            ev = uiledit( ev );
            if( info->edit_data != NULL ) {
                edit->buffer = info->edit_data->edit_buffer;
                edit->length = info->edit_data->edit_eline.length;
            }
            break;
        }
    }
    ev = uihotkeyfilter( info, ev );
    ev = uihotspotfilter( info->vs, info->fields, ev );
    return( ev );
}

an_event uidialog( a_dialog *info )
{
    static EVENT    dialog_events[] = {
        'a',            'z',
        'A',            'Z',
        EV_ALT_Q,       EV_ALT_M,
        EV_NO_EVENT,
        EV_ALT_CURSOR_DOWN,
        EV_CURSOR_UP,   EV_CURSOR_DOWN,
        EV_CURSOR_LEFT, EV_CURSOR_RIGHT,
        EV_TAB_FORWARD, EV_TAB_BACKWARD,
        EV_MOUSE_PRESS, EV_MOUSE_RELEASE,
        EV_MOUSE_DRAG,  EV_MOUSE_REPEAT,
        EV_MOUSE_DCLICK,
        EV_ENTER,       ' ',
        EV_NO_EVENT
    };
    a_list      *list;
    an_event    ev;
    VFIELD      *field;

    ev = EV_NO_EVENT;
    enter_field( info, info->curr );

    while( ev == EV_NO_EVENT || !uiinlist( ev ) ) {
        field = info->curr;
        if( field != NULL ) {
            switch( field->typ ) {
                case FLD_EDIT:
                case FLD_INVISIBLE_EDIT:
                    uieditpushlist();
                    break;
                case FLD_PULLDOWN:
                    uiboxpushlist( );
                    break;
                case FLD_COMBOBOX:
                    uiboxpushlist( );
                    uieditpushlist();
                    break;
                case FLD_LISTBOX:
                case FLD_EDIT_MLE:
                    list = field->ptr;
                    uiboxpushlist();
                    break;
            }
        }
        uipushlist( dialog_events );
        ev = uidialogevent( info->vs );
        ev = uidialogcallback( info, ev );
        uipoplist( /* dialog_events */ );

        if( field != NULL ) {
            switch( field->typ ) {
            case FLD_EDIT:
            case FLD_INVISIBLE_EDIT:
                uieditpoplist();
                break;
            case FLD_PULLDOWN:
                uiboxpoplist();
                break;
            case FLD_COMBOBOX:
                uiboxpoplist();
                uieditpoplist();
                break;
            case FLD_LISTBOX:
            case FLD_EDIT_MLE:
                uiboxpoplist();
                break;
            }
        }
        ev = uiprocessdialogevent( ev, info );
    }
    /* This code will make sure to exit the current fields before returning
     * a default hot spot event. This is for consistency with windows.
     */
    if( uiisdefaulthotspot( info->fields, ev ) ) {
        if( exit_field( info, info->curr ) ) {
            info->dirty = TRUE;
        }
    }
    info->field = (VFIELD *)info->curr - (VFIELD *)info->fields;
    return( ev );
}

void uifreedialog( a_dialog  *info )
{
    VFIELD              *fields;  // pointer to array of fields in dialog box
    a_list              *list;
    short unsigned      i;
    a_combo_box         *combo;
//  an_edit_control     *edit;

    fields = info->fields;

    exit_field( info, info->curr );
    for( i = 0 ; fields[i].typ != FLD_VOID ; ++i ) {
        switch( fields[i].typ ) {
            case FLD_LISTBOX:
            case FLD_PULLDOWN:
            case FLD_EDIT_MLE:
                list = fields[i].ptr;
                uiendlistbox( list );
                break;
//          case FLD_INVISIBLE_EDIT:
//          case FLD_EDIT:
//              edit = fields[i].ptr;
//              uifree( edit->buffer );
//              edit->buffer = NULL;            //  Need this null for next
//              break;                          //  time around
            case FLD_COMBOBOX:
                combo = fields[i].ptr;
                list = &combo->list;
//              edit = &combo->edit;
//              uifree( edit->buffer );
//              edit->buffer = NULL;
                uiendlistbox( list );       // Shut down listbox
                break;
        }
    }
}

void uienddialog( a_dialog  *info )
{
    uifreedialog( info );
    uifinidialog( info->vs );
    uifree( info );
}

#if 0
void uifreefields( VFIELD *fields )
{
    short unsigned      i;
    a_combo_box         *combo;
    an_edit_control     *edit;

    for( i = 0 ; fields[i].typ != FLD_VOID ; ++i ) {
        switch( fields[i].typ ) {
        case FLD_INVISIBLE_EDIT:
        case FLD_EDIT:
            edit = fields[i].ptr;
            uifree( edit->buffer );
            edit->buffer = NULL;                //  Need this null for next
            break;                              //  time around
        case FLD_COMBOBOX:
            combo = fields[i].ptr;
            edit = &combo->edit;
            uifree( edit->buffer );
            edit->buffer = NULL;
            break;
        }
    }
}
#endif

bool uidialogisdirty( a_dialog *info )
{
    uidialogexitcurr( info );
    return( info->dirty );
}

void uidialogsetdirty( a_dialog *info, bool dirty )
{
    info->dirty = dirty;
}
