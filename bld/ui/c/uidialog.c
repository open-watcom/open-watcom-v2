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

static bool exit_field( a_dialog *ui_dlg_info, VFIELD *field )
{
    bool                flag;
    an_edit_control     *edit;
    a_list              *list;
    a_combo_box         *combo;

    flag = false;
    if( field != NULL ) {
        switch( field->typ ) {
        case FLD_EDIT :
        case FLD_INVISIBLE_EDIT:
            if( ui_dlg_info->edit_data != NULL ) {
                edit = field->u.edit;
                if( uieditisdirty() ) {
                    flag = true;
                }
                uiedittrim( ui_dlg_info->edit_data->edit_buffer );
                edit->buffer = ui_dlg_info->edit_data->edit_buffer;
                edit->length = ui_dlg_info->edit_data->edit_eline.length;
                uiendedit();
                ui_dlg_info->edit_data = NULL;
            }
            uinocursor( ui_dlg_info->vs );
            break;
        case FLD_COMBOBOX :
            combo = field->u.combo;
            list = &combo->list;
            edit = &combo->edit;

            if( !combo->perm ) {
                uiendlistbox( list );           // Shut down listbox
            }
            if( ui_dlg_info->edit_data != NULL ) {
                if( uieditisdirty() ) {
                    flag = true;
                }
                uiedittrim( ui_dlg_info->edit_data->edit_buffer );
                edit->buffer = ui_dlg_info->edit_data->edit_buffer;
                edit->length = ui_dlg_info->edit_data->edit_eline.length;
                uiendedit();
                ui_dlg_info->edit_data = NULL;
            }
            uinocursor( ui_dlg_info->vs );
            break;
        case FLD_PULLDOWN :
            list = field->u.list;
            uiendlistbox( list );
            break;
        case FLD_LISTBOX :
            list = field->u.list;
            list->box->attr = ATTR_EDIT;
            uipaintlistbox( list );
            break;
        case FLD_EDIT_MLE :
            list = field->u.list;
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
        r = fld->u.radio;
        if( r->value != r->group->value ) {
            return( true );
        }
    }
    return( false );
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

static void print_field( VSCREEN *vs, VFIELD *field, bool current )
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
    an_edit_control     *edit;
    VSCREEN             *c_vs;
    SAREA               c_area;
    bool                use_hottext;
    char                hotkey;

    if( field == NULL )
        return;
    area = &field->area;
    str = NULL;
    use_hottext = false;
    memset( ctrlbuf, '\0', CTRL_BUF_LEN+1 );

    attr = UIData->attrs[( current ) ? ATTR_CURR_EDIT : ATTR_EDIT];

    switch( field->typ ) {
    case FLD_HOT :
        if( current ) {
            field->u.hs->flags |= HOT_CURRENT ;
        } else {
            field->u.hs->flags &= (~HOT_CURRENT);
        }
        uidisplayhotspot( vs, field );
        return;
    case FLD_TEXT :
        attr = UIData->attrs[ ATTR_NORMAL ];
        str  = field->u.str;
        break;
    case FLD_LABEL :
        attr = UIData->attrs[ ATTR_NORMAL ];
        strcpy( ctrlbuf, field->u.str );
        strcat( ctrlbuf, ":" );
        length = area->width;
        break;
    case FLD_FRAME :
        uidrawbox( vs, area, UIData->attrs[ ATTR_NORMAL ], field->u.str );
        return;
    case FLD_EDIT :
    case FLD_INVISIBLE_EDIT :
        edit = field->u.edit;
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
        combo = field->u.combo;
        edit  = &combo->edit;
        list  = &combo->list;

        if( !combo->perm ) {
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
        if( list->box == NULL && combo->perm ) {
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
        list = field->u.list;
        ctrlbuf[0] = UiGChar[ UI_ARROW_DOWN ];  /* JBS was 25 */
        uivtextput( vs, area->row, area->col + area->width,
                    UIData->attrs[ ATTR_SCROLL_ICON ], ctrlbuf, 1 );
        ctrlbuf[0] = '\0';
        if( list->get == NULL ) {
            list->get = uigetlistelement;
        }
        (*list->get)( list->data_handle, list->choice, ctrlbuf, area->width );
        length = area->width;
        break;
    case FLD_LISTBOX:
    case FLD_EDIT_MLE:
        list = field->u.list;
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
        attr = UIData->attrs[( current ) ? ATTR_CURR_EDIT : ATTR_NORMAL];
        check = field->u.check;

        ctrlbuf[0] = CHECKBOX_CHAR( LEFT );
        if( _checked( check ) ) {
            ctrlbuf[1] = CHECKBOX_CHAR( FULL );
        } else {
            ctrlbuf[1] = CHECKBOX_CHAR( EMPTY );
        }
        ctrlbuf[2] = CHECKBOX_CHAR( RIGHT );
        ctrlbuf[3] = ' ';

        strncat( ctrlbuf, check->str, CTRL_BUF_LEN - 4 );
        length = strlen( ctrlbuf );
        use_hottext = true;
        break;
    case FLD_RADIO:
        attr = UIData->attrs[( current ) ? ATTR_CURR_EDIT : ATTR_NORMAL];
        radio = field->u.radio;

        ctrlbuf[0] = RADIO_CHAR( LEFT );
        if( radio->value == radio->group->value ) {
            ctrlbuf[1] = RADIO_CHAR( FULL );
        } else {
            ctrlbuf[1] = RADIO_CHAR( EMPTY );
        }
        ctrlbuf[2] = RADIO_CHAR( RIGHT );
        ctrlbuf[3] = ' ';

        strncat( ctrlbuf, radio->str, CTRL_BUF_LEN - 4 );
        length = strlen( ctrlbuf );
        use_hottext = true;
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
        hotkey = uidrawhottext( vs, ctrlbuf, area, attr, hotattr, false, false, false );
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

void uiprintfield( a_dialog *ui_dlg_info, VFIELD *field )
{
    print_field( ui_dlg_info->vs, field, ( field == ui_dlg_info->curr ) );
}

static void *makevs( const char *title, int cols, int rows, int cpos, int rpos )
{
    SAREA               area;

    uiposition( &area, rows, cols, rpos, cpos, true );
    return( uiopen( &area, title, V_DIALOGUE ) );
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

void *uiinitdialog( const char *title, ATTR attr, char *lines[],
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

    uisetscreenarea( &area, true, false );
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
    vs = makevs( title, maxlen, depth + extra_rows, cpos, rpos );
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

static void enter_field( a_dialog *ui_dlg_info, VFIELD *field )
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
            combo = field->u.combo;
            edit  = &combo->edit;
        } else {
            edit = field->u.edit;
        }
        if( ui_dlg_info->edit_data == NULL ) {
            ui_dlg_info->edit_data = uibegedit( ui_dlg_info->vs, area.row, area.col, area.width,
                                  UIData->attrs[ ATTR_CURR_EDIT ],
                                  edit->buffer, edit->length, 0, 0, true, 0,
                                  field->typ == FLD_INVISIBLE_EDIT );
        }
        break;
    case FLD_LISTBOX :
        list = field->u.list;
        list->box->attr = ATTR_CURR_EDIT;
        uipaintlistbox( list );
        break;
    case FLD_EDIT_MLE :
        list = field->u.list;
        list->box->attr = ATTR_NORMAL;
        uipaintlistbox( list );
        break;
    }
}

void uireinitdialog( a_dialog *ui_dlg_info, VFIELD *fields )
{
    unsigned            i;

    uiposnhotspots( ui_dlg_info->vs, fields );
    ui_dlg_info->dirty = false;
    ui_dlg_info->first = NULL;
    ui_dlg_info->other = NULL;
    ui_dlg_info->fields = fields;

    ui_dlg_info->first = &fields[0];
    /* set first to be first field in tab sequence */
    while( notintab( ui_dlg_info->first ) ) {
        if( ui_dlg_info->first->typ == FLD_VOID ) {
            ui_dlg_info->first = NULL;
            break;
        }
        ++(ui_dlg_info->first);
    }
    ui_dlg_info->curr = ui_dlg_info->first;
    if( ui_dlg_info->first != NULL ) {
        /* set curr to first field in tab sequence not an unset radio button */
        while( radiooff( ui_dlg_info->curr ) ) {
            ui_dlg_info->curr = nextfield( ui_dlg_info->curr );
        }
    }

    for( i = 0 ; fields[i].typ != FLD_VOID ; ++i ) {
        print_field( ui_dlg_info->vs, &fields[i], ( &fields[i] == ui_dlg_info->curr ) );
    }
    enter_field( ui_dlg_info, ui_dlg_info->curr );
}

bool uigetdialogarea( a_dialog *ui_dlg_info, SAREA *area )
{
    VSCREEN     *vs;

    if( ui_dlg_info != NULL && area != NULL ) {
        vs = ui_dlg_info->vs;
        *area = vs->area;
        return( true );
    }
    return( false );
}

a_dialog *uibegdialog( const char *title, VFIELD *fields, ORD rows, ORD cols, int rpos, int cpos )
{
    char                *lines[1];
    a_dialog            *ui_dlg_info;

    lines[ 0 ] = NULL;
    ui_dlg_info = uimalloc( sizeof( a_dialog ) );
    if( ui_dlg_info == NULL ) {
        return( NULL );
    }
    ui_dlg_info->field = 0;
    ui_dlg_info->edit_data = NULL;
    ui_dlg_info->moving = false;
    ui_dlg_info->vs = uiinitdialog( title, UIData->attrs[ ATTR_NORMAL ],
                           lines, rows, cols, rpos, cpos );
    uireinitdialog( ui_dlg_info, fields );
    return( ui_dlg_info );
}

static void do_radio( a_dialog *ui_dlg_info, VFIELD *field )
{
    a_radio *cur_radio;
    VFIELD  *fields;
    a_radio *radio;
    int i;

    fields = ui_dlg_info->fields;
    // might want to use ui_dlg_info->first, kind of unsure
    cur_radio = field->u.radio;
    if( cur_radio->value == cur_radio->group->value )
        return;

    ui_dlg_info->dirty = true;

    for( i = 0 ; fields[i].typ != FLD_VOID ; ++i ) {
        if( fields[i].typ == FLD_RADIO ) {
            radio = fields[i].u.radio;
            if( radio->group == cur_radio->group  &&
                radio->value == radio->group->value ) {
                radio->group->value = cur_radio->value;
                print_field( ui_dlg_info->vs, &fields[i], false );
                break;
            }
        }
    }
    cur_radio->group->value = cur_radio->value;
    print_field( ui_dlg_info->vs, field, true );
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
        if( (*list->get)( list->data_handle, list->choice, str, CTRL_BUF_LEN ) ) {
            uifree( edit->buffer );
            edit->buffer = str;
            edit->length = strlen( str );
        } else {
            uifree( str );
        }
    }
}

void uiupdateedit( a_dialog *ui_dlg_info, VFIELD *field )
{
    an_edit_control     *edit;
    a_combo_box         *combo;

    if( ( field == NULL ) || ( ui_dlg_info == NULL ) || ( field != ui_dlg_info->curr ) ) {
        return;
    }
    edit = NULL;
    switch( field->typ ) {
    case FLD_COMBOBOX :
        combo = field->u.combo;
        edit = &combo->edit;
        break;
    case FLD_EDIT :
    case FLD_INVISIBLE_EDIT:
        edit = field->u.edit;
        break;
    }
    if( edit != NULL ) {
        if( ui_dlg_info->edit_data != NULL ) {
            uiendedit();
            ui_dlg_info->edit_data = NULL;
        }
        enter_field( ui_dlg_info, ui_dlg_info->curr );
    }
}

static  void setcombobuffer( a_dialog *ui_dlg_info, VFIELD *fld )
{
    a_combo_box         *combo;
    an_edit_control     *edit;

    combo = fld->u.combo;
    edit  = &combo->edit;
    uiupdatecombobox( combo );
    if( ui_dlg_info->edit_data != NULL ) {
        ui_dlg_info->edit_data->edit_buffer = edit->buffer;
        ui_dlg_info->edit_data->edit_eline.buffer = edit->buffer;
        ui_dlg_info->edit_data->edit_eline.length = edit->length;
        ui_dlg_info->edit_data->edit_eline.update = true;
        ui_dlg_info->edit_data->edit_eline.index = 0;
        ui_dlg_info->edit_data->edit_eline.scroll = 0;
    }
}

static EVENT   pulldownfilter( EVENT ev, a_dialog *ui_dlg_info )
{
    a_list          *list = NULL;
    a_combo_box     *combo;

    VSCREEN     *vs;
    SAREA       area;
    VFIELD      *fld;

    fld = ui_dlg_info->curr;

    if( fld->typ == FLD_PULLDOWN ) {
        list = fld->u.list;
    } else if( fld->typ == FLD_COMBOBOX ) {
        combo = fld->u.combo;
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

        uivmousepos( ui_dlg_info->vs, &row, &col );
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
        vs   = ui_dlg_info->vs;
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
            ui_dlg_info->dirty = true;
            setcombobuffer( ui_dlg_info, fld );
            print_field( ui_dlg_info->vs, fld, true );
        }
        break;
    }
    return( ev );
}

static void *forwardtab( a_dialog *ui_dlg_info )
{
    VFIELD             *fld;

    if( ui_dlg_info->curr == NULL ){
        fld = ui_dlg_info->first;
    } else {
        fld = nextfield( ui_dlg_info->curr );
        if( fld == NULL ) {
            fld = ui_dlg_info->first;
        }
    }
    while( radiooff( fld ) ) {
        fld = nextfield( fld );
    }
    return( fld );
}

static void *backwardtab( a_dialog *ui_dlg_info )
{
    VFIELD      *fld, *hold;

    hold = NULL;
    fld = ui_dlg_info->first;
    while( fld != ui_dlg_info->curr ) {
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

extern void uidialogexitcurr( a_dialog *ui_dlg_info )
{
    /* you must call this function to set ui_dlg_info->dirty or to do  */
    /* blank trimming on an edit control                        */
    if( exit_field( ui_dlg_info, ui_dlg_info->curr ) ) {
        ui_dlg_info->dirty = true;
    }
}

extern void uidialogsetcurr( a_dialog *ui_dlg_info, VFIELD *curr )
{
    VFIELD      *other;

    other = ui_dlg_info->curr;
    if( other != curr ) {
        ui_dlg_info->other = ui_dlg_info->curr;
        ui_dlg_info->curr  = curr;
        if( exit_field( ui_dlg_info, other ) ) {
            ui_dlg_info->dirty = true;
        }
        print_field( ui_dlg_info->vs, other, false );
        enter_field( ui_dlg_info, curr );
        print_field( ui_dlg_info->vs, curr, true );
        uidialogchangefield( ui_dlg_info );
    }
}

void uimovefield( a_dialog *ui_dlg_info, VFIELD *curr, int row_diff, int col_diff )
{
    a_combo_box *combo;
    a_list      *list;

    ui_dlg_info = ui_dlg_info;

    switch( curr->typ ) {
    case FLD_COMBOBOX :
        combo = curr->u.combo;
        if( combo->perm ) {
            uimovelistbox( &combo->list, row_diff, col_diff );
        } else {
            uiendlistbox( &combo->list );
        }
        break;
    case FLD_LISTBOX :
    case FLD_EDIT_MLE :
        list = curr->u.list;
        uimovelistbox( list, row_diff, col_diff );
        if( list->box != NULL ) {
            uivsetactive( list->box->vs );
        }
        break;
    case FLD_PULLDOWN :
        uiendlistbox( curr->u.list );
        break;
    }
}

void uiredrawdialog( a_dialog *ui_dlg_info )
{
    VSCREEN     *vs;
    int         i;

    vs = ui_dlg_info->vs;

    uivclose( vs );
    uivopen( vs );

    for( i = 0 ; ui_dlg_info->fields[i].typ != FLD_VOID ; ++i ) {
        print_field( ui_dlg_info->vs, &ui_dlg_info->fields[i],
                     ( &ui_dlg_info->fields[i] == ui_dlg_info->curr ) );
    }
}

bool uiresizedialog( a_dialog *ui_dlg_info, SAREA *new_area )
{
    int         row_diff;
    int         col_diff;
    VSCREEN     *vs;
    VFIELD      *curr;
    int         i;
    bool        resize;

    vs = ui_dlg_info->vs;

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
        uivmove( ui_dlg_info->vs, new_area->row, new_area->col );
    }
    /* close all open pull down boxes */
    for( curr = ui_dlg_info->fields; curr->typ != FLD_VOID; curr++ ) {
        uimovefield( ui_dlg_info, curr, row_diff, col_diff );
    }
    if( resize ) {
        for( i = 0 ; ui_dlg_info->fields[i].typ != FLD_VOID ; ++i ) {
            print_field( ui_dlg_info->vs, &ui_dlg_info->fields[i],
                         ( &ui_dlg_info->fields[i] == ui_dlg_info->curr ) );
        }
    }
    return( true );
}

static  ORD     PrevRow = 0;
static  ORD     PrevCol = 0;

static EVENT uicheckmove( EVENT ev, a_dialog *ui_dlg_info )
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
        if( uivmousepos( NULL, &row, &col ) == ui_dlg_info->vs ) {
            vs = ui_dlg_info->vs;
            if( ( row + 1 ) == vs->area.row ) { /* because it's framed */
                PrevRow = row;
                PrevCol = col;
                ui_dlg_info->moving = true;
                new_ev = EV_NO_EVENT;
            }
        }
        break;
    case EV_MOUSE_DRAG :
        if( ui_dlg_info->moving ) {
            uivmousepos( NULL, &row, &col );
            if( ( row != PrevRow ) || ( col != PrevCol ) ) {
                vs = ui_dlg_info->vs;
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
                uiresizedialog( ui_dlg_info, &new_area );
                /* new_area may have been adjusted */
                PrevRow += new_area.row - prev_row;
                PrevCol += new_area.col - prev_col;
            }
            new_ev = EV_NO_EVENT;
        }
        break;
    case EV_MOUSE_RELEASE :
        if( ui_dlg_info->moving ) {
            ui_dlg_info->moving = false;
            new_ev = EV_NO_EVENT;
        }
    }
    return( new_ev );
}

static EVENT uitabkey( EVENT ev, a_dialog *ui_dlg_info )
{
    VFIELD          *curr, *fld;
    ORD             row, col;
    SAREA           area;
    EVENT           newev;

    if( ui_dlg_info->first == NULL ) return( false );
    curr = ui_dlg_info->curr;
    newev = ev;
    switch( ev ){
    case EV_MOUSE_DCLICK:
    case EV_MOUSE_PRESS:
    {
        a_combo_box         *combo;
        a_list              *list;
        VSCREEN             *mousevs;

        mousevs = uivmousepos( ui_dlg_info->vs, &row, &col );
        fld = ui_dlg_info->first;
        while( fld != NULL ) {
            list = NULL;
            area = fld->area;
            if( fld->typ == FLD_PULLDOWN ) {
                list = fld->u.list;
                area.height = 1;
                area.width += 1;    /* pulldown button */
            } else if( fld->typ == FLD_LISTBOX ) {
                list = fld->u.list;
            } else if( fld->typ == FLD_EDIT_MLE ) {
                list = fld->u.list;
            } else if( fld->typ == FLD_COMBOBOX ) {
                area.height = 1;
                area.width += 2;    /* pulldown button */
                combo = fld->u.combo;
                list = &combo->list;
            }
            if( mousevs != ui_dlg_info->vs ) {
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
        curr = forwardtab( ui_dlg_info );
        break;
    case EV_TAB_BACKWARD :
        curr = backwardtab( ui_dlg_info );
        break;
    case EV_CURSOR_RIGHT :
    case EV_CURSOR_DOWN :
    {
        a_radio             *r1, *r2;

        if( curr!= NULL && curr->typ == FLD_RADIO ) {
            fld = nextfield( curr );
            if( fld != NULL ) {
                r1 = curr->u.radio;
                r2 = fld->u.radio;
                if( r1->group == r2->group ) {
                    curr = fld;
                    do_radio( ui_dlg_info, fld );
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
            if( curr != ui_dlg_info->first ) {
                fld = curr - 1;
                r1 = curr->u.radio;
                r2 = fld->u.radio;
                if( r1->group == r2->group ) {
                    curr = fld;
                    do_radio( ui_dlg_info, fld );
                    newev = EV_CHECK_BOX_CLICK;
                }
            }
        }
        break;
    }
    }
    if( ui_dlg_info->curr != curr ) {
        uidialogsetcurr( ui_dlg_info, curr );
    }
    return( newev );
}

EVENT uihotkeyfilter( a_dialog *ui_dlg_info, EVENT ev )
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
        for( vf = ui_dlg_info->fields ; vf->typ != FLD_VOID; ++vf ){
            switch( vf->typ ) {
            case FLD_HOT:
                hotkey = vf->u.hs->flags;
                break;
            case FLD_CHECK:
                hotkey = vf->u.check->hotkey;
                break;
            case FLD_RADIO:
                hotkey = vf->u.radio->hotkey;
                break;
            default:
                hotkey = '\0';
                break;
            }
            if( hotkey == ch ) {
                break;
            }
        }

        /* make sure the new field is hilighted */
        if( ui_dlg_info->curr != vf && vf->typ != FLD_VOID ) {
            uidialogsetcurr( ui_dlg_info, vf );
        }

        if( hotkey == ch ) {
            switch( vf->typ ) {
                case FLD_HOT:
                    ev = vf->u.hs->event;
                    break;
                case FLD_CHECK:
                    ui_dlg_info->dirty = true;
                    vf->u.check->val = !vf->u.check->val;
                    print_field( ui_dlg_info->vs, vf, true );
                    ev = EV_CHECK_BOX_CLICK;
                    break;
                case FLD_RADIO:
                    do_radio( ui_dlg_info, vf );
                    ev = EV_CHECK_BOX_CLICK;
                    break;
            }
        }
    }

    return( ev );
}

EVENT uiprocessdialogevent( EVENT ev, a_dialog *ui_dlg_info )
{
    VFIELD              *field;
    a_check             *check;
    an_edit_control     *edit;
    unsigned            choice;
    a_combo_box         *combo;
    a_list              *list;

    ev = uicheckmove( ev, ui_dlg_info );
    ev = uitabkey( ev, ui_dlg_info );
    field = ui_dlg_info->curr;
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
                    do_radio( ui_dlg_info, field );
                    break;
                }
                if( field->typ == FLD_CHECK ) {
                    ui_dlg_info->dirty = true;
                    check = field->u.check;
                    check->val = !check->val;
                    print_field( ui_dlg_info->vs, field, true );
                    break;
                }
            }
            break;
        case FLD_EDIT:
        case FLD_INVISIBLE_EDIT:
            ev = uiledit( ev );
            edit = field->u.edit;
            if( ui_dlg_info->edit_data != NULL ) {
                edit->buffer = ui_dlg_info->edit_data->edit_buffer;
                edit->length = ui_dlg_info->edit_data->edit_eline.length;
            }
            break;
        case FLD_LISTBOX:
        case FLD_EDIT_MLE:
            list = field->u.list;
            ev = uilistbox( ev, list, true );
            break;
        case FLD_PULLDOWN:
            list = field->u.list;
            if( list->box == NULL ) {
                ev = pulldownfilter( ev, ui_dlg_info );
            } else {
                choice = list->choice;
                ev = uilistbox( ev, list, false );
                if( choice != list->choice ) {
                    ui_dlg_info->dirty = true;
                    print_field( ui_dlg_info->vs, field, true );
                }
            }
            break;
        case FLD_COMBOBOX:
            combo = field->u.combo;
            list  = &combo->list;
            edit  = &combo->edit;
            if( list->box == NULL ) {
                ev = pulldownfilter( ev, ui_dlg_info );
            } else {
                choice = list->choice;
                ev = uilistbox( ev, list, combo->perm );
                if( choice != list->choice ) {
                    ui_dlg_info->dirty = true;
                    setcombobuffer( ui_dlg_info, field );
                    print_field( ui_dlg_info->vs, field, true );
                }
            }
            ev = uiledit( ev );
            if( ui_dlg_info->edit_data != NULL ) {
                edit->buffer = ui_dlg_info->edit_data->edit_buffer;
                edit->length = ui_dlg_info->edit_data->edit_eline.length;
            }
            break;
        }
    }
    ev = uihotkeyfilter( ui_dlg_info, ev );
    ev = uihotspotfilter( ui_dlg_info->vs, ui_dlg_info->fields, ev );
    return( ev );
}

an_event uidialog( a_dialog *ui_dlg_info )
{
    static EVENT    dialog_events[] = {
        'a',            'z',
        'A',            'Z',
        EV_ALT_Q,       EV_ALT_M,
        EV_NO_EVENT,
        EV_ALT_CURSOR_DOWN,
        EV_CURSOR_UP,
        EV_CURSOR_DOWN,
        EV_CURSOR_LEFT,
        EV_CURSOR_RIGHT,
        EV_TAB_FORWARD,
        EV_TAB_BACKWARD,
        EV_MOUSE_PRESS,
        EV_MOUSE_RELEASE,
        EV_MOUSE_DRAG,
        EV_MOUSE_REPEAT,
        EV_MOUSE_DCLICK,
        EV_ENTER,
        ' ',
        EV_NO_EVENT
    };
    an_event    ev;
    VFIELD      *field;

    ev = EV_NO_EVENT;
    enter_field( ui_dlg_info, ui_dlg_info->curr );

    while( ev == EV_NO_EVENT || !uiinlists( ev ) ) {
        field = ui_dlg_info->curr;
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
                    uiboxpushlist();
                    break;
            }
        }
        uipushlist( dialog_events );
        ev = uidialogevent( ui_dlg_info->vs );
        ev = uidialogcallback( ui_dlg_info, ev );
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
        ev = uiprocessdialogevent( ev, ui_dlg_info );
    }
    /* This code will make sure to exit the current fields before returning
     * a default hot spot event. This is for consistency with windows.
     */
    if( uiisdefaulthotspot( ui_dlg_info->fields, ev ) ) {
        if( exit_field( ui_dlg_info, ui_dlg_info->curr ) ) {
            ui_dlg_info->dirty = true;
        }
    }
    ui_dlg_info->field = ui_dlg_info->curr - ui_dlg_info->fields;
    return( ev );
}

void uifreedialog( a_dialog *ui_dlg_info )
{
    VFIELD              *fields;  // pointer to array of fields in dialog box
    a_list              *list;
    short unsigned      i;
    a_combo_box         *combo;
//  an_edit_control     *edit;

    fields = ui_dlg_info->fields;

    exit_field( ui_dlg_info, ui_dlg_info->curr );
    for( i = 0 ; fields[i].typ != FLD_VOID ; ++i ) {
        switch( fields[i].typ ) {
            case FLD_LISTBOX:
            case FLD_PULLDOWN:
            case FLD_EDIT_MLE:
                list = fields[i].u.list;
                uiendlistbox( list );
                break;
//          case FLD_INVISIBLE_EDIT:
//          case FLD_EDIT:
//              edit = fields[i].u.edit;
//              uifree( edit->buffer );
//              edit->buffer = NULL;            //  Need this null for next
//              break;                          //  time around
            case FLD_COMBOBOX:
                combo = fields[i].u.combo;
                list = &combo->list;
//              edit = &combo->edit;
//              uifree( edit->buffer );
//              edit->buffer = NULL;
                uiendlistbox( list );       // Shut down listbox
                break;
        }
    }
}

void uienddialog( a_dialog *ui_dlg_info )
{
    uifreedialog( ui_dlg_info );
    uifinidialog( ui_dlg_info->vs );
    uifree( ui_dlg_info );
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
            edit = fields[i].u.edit;
            uifree( edit->buffer );
            edit->buffer = NULL;                //  Need this null for next
            break;                              //  time around
        case FLD_COMBOBOX:
            combo = fields[i].u.combo;
            edit = &combo->edit;
            uifree( edit->buffer );
            edit->buffer = NULL;
            break;
        }
    }
}
#endif

bool uidialogisdirty( a_dialog *ui_dlg_info )
{
    uidialogexitcurr( ui_dlg_info );
    return( ui_dlg_info->dirty );
}

void uidialogsetdirty( a_dialog *ui_dlg_info, bool dirty )
{
    ui_dlg_info->dirty = dirty;
}
