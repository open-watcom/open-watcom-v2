/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <ctype.h>
#include "uidef.h"
#include "uimenu.h"
#include "uidialog.h"
#include "uigchar.h"

#include "clibext.h"


#define CTRL_BUF_LEN    80

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
            list->box->uiattr = ATTR_EDIT;
            uipaintlistbox( list );
            break;
        case FLD_EDIT_MLE :
            list = field->u.list;
            list->box->uiattr = ATTR_NORMAL;
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
        if( fld->typ == FLD_NONE ) {
            fld = NULL;
            break;
        }
    }
    return( fld );
}

static void print_field( VSCREEN *vs, VFIELD *field, bool current )
{
    SAREA               *area;
    ATTR                attr;
    ATTR                hotattr;
    char                ctrlbuf[CTRL_BUF_LEN + 1];
    size_t              ctrlbuf_len;
    a_check             *check = NULL;
    a_radio             *radio = NULL;
    a_list              *list;
    a_combo_box         *combo;
    an_edit_control     *edit;
    VSCREEN             *c_vs;
    SAREA               c_area;
    bool                use_hottext;
    char                hotkey;
    UIPICKGETTEXT       *fn_get;

    if( field == NULL )
        return;
    area = &field->area;
    use_hottext = false;
    memset( ctrlbuf, '\0', CTRL_BUF_LEN + 1 );
    ctrlbuf_len = 0;

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
    case FLD_LABEL :
        attr = UIData->attrs[ATTR_NORMAL];
        if( field->u.str != NULL ) {
            strncpy( ctrlbuf, field->u.str, CTRL_BUF_LEN );
            ctrlbuf[CTRL_BUF_LEN] = '\0';
            ctrlbuf_len = strlen( ctrlbuf );
        }
        if( field->typ == FLD_LABEL ) {
            if( ctrlbuf_len < CTRL_BUF_LEN ) {
                ctrlbuf[ctrlbuf_len++] = ':';
                ctrlbuf[ctrlbuf_len] = '\0';
            }
        }
        break;
    case FLD_FRAME :
        uidrawbox( vs, area, UIData->attrs[ATTR_NORMAL], field->u.str );
        return;
    case FLD_EDIT :
    case FLD_INVISIBLE_EDIT :
        edit = field->u.edit;
        if( edit->buffer != NULL ) {
            ctrlbuf_len = edit->length;
            if( ctrlbuf_len > CTRL_BUF_LEN )
                ctrlbuf_len = CTRL_BUF_LEN;
            if( field->typ == FLD_INVISIBLE_EDIT ) {
                memset( ctrlbuf, '*', ctrlbuf_len );
            } else {
                strncpy( ctrlbuf, edit->buffer, ctrlbuf_len );
            }
        }
        ctrlbuf[ctrlbuf_len] = '\0';
        break;
    case FLD_COMBOBOX :
        combo = field->u.combo;
        edit  = &combo->edit;
        list  = &combo->list;

        if( !combo->perm ) {
            ctrlbuf[0] = UiGChar[UI_ARROW_DOWN];
            uivtextput( vs, area->row, area->col + area->width + 1,
                        UIData->attrs[ATTR_SCROLL_ICON], ctrlbuf, 1 );
        }
        if( edit->buffer != NULL ) {
            ctrlbuf_len = edit->length;
            if( ctrlbuf_len > CTRL_BUF_LEN )
                ctrlbuf_len = CTRL_BUF_LEN;
            strncpy( ctrlbuf, edit->buffer, ctrlbuf_len );
        }
        ctrlbuf[ctrlbuf_len] = '\0';
        if( list->box == NULL && combo->perm ) {
            c_area = *area;
            c_area.row += vs->area.row + 2;
            c_area.col += vs->area.col + 1;
            c_vs = uiopen( &c_area, NULL, V_DIALOGUE | V_LISTBOX );
            if( c_vs != NULL ) {
                c_area.row = 0;
                c_area.col = 0;
                list->box = uibeglistbox( c_vs, &c_area, list );
            }
        }
        break;
    case FLD_PULLDOWN :
        list = field->u.list;
        ctrlbuf[0] = UiGChar[UI_ARROW_DOWN];
        uivtextput( vs, area->row, area->col + area->width,
                    UIData->attrs[ATTR_SCROLL_ICON], ctrlbuf, 1 );
        fn_get = list->get;
        if( fn_get == NULL )
            fn_get = uigetlistelement;
        (*fn_get)( list->data_handle, list->choice, ctrlbuf, CTRL_BUF_LEN );
        /* ctrlbuf does not have to be null terminated */
        /* terminate it at maximum length */
        ctrlbuf[CTRL_BUF_LEN] = '\0';
        ctrlbuf_len = strlen( ctrlbuf );
        break;
    case FLD_LISTBOX:
    case FLD_EDIT_MLE:
        list = field->u.list;
        if( list->box == NULL ) {
            c_area = *area;
            c_area.row += vs->area.row;
            c_area.col += vs->area.col;
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
        /* ctrlbuf must be null terminated for this case */
        use_hottext = true;
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

        strncpy( ctrlbuf + 4, check->str, CTRL_BUF_LEN - 4 );
        ctrlbuf[CTRL_BUF_LEN] = '\0';
        break;
    case FLD_RADIO:
        /* ctrlbuf must be null terminated for this case */
        use_hottext = true;
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

        strncpy( ctrlbuf + 4, radio->str, CTRL_BUF_LEN - 4 );
        ctrlbuf[CTRL_BUF_LEN] = '\0';
        break;
    }
    if( use_hottext ) {
        if( current ) {
            hotattr = attr;
        } else {
            hotattr = UIData->attrs[ATTR_RADIO_HOTSPOT];
        }
        hotkey = uidrawhottext( vs, ctrlbuf, area, attr, hotattr, false, false, false );
        if( field->typ == FLD_CHECK ) {
            check->hotkey = hotkey;
        } else if( field->typ == FLD_RADIO ) {
            radio->hotkey = hotkey;
        }
    } else {
        uitextfield( vs, area->row, area->col, area->width, attr, ctrlbuf, ctrlbuf_len );
    }
}

void uiprintfield( a_dialog *ui_dlg_info, VFIELD *field )
{
    print_field( ui_dlg_info->vs, field, ( field == ui_dlg_info->curr ) );
}

static VSCREEN *makevs( const char *title, uisize cols, uisize rows, int cpos, int rpos )
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
    if( len < max )
        max = len;
    for( delim = 0, cr = 0, i = 0; i < max; ++i ) {
        ch = str[i];
        if( ch == '\n' ) {
            break;
        } else if( ch == '\r' ) {
            str[i] = ' ';
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
        str[cr] = '\r';
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

VSCREEN *uiinitdialog( const char *title, ATTR attr, char *lines[],
                uisize extra_rows, uisize maxlen, int rpos, int cpos )
{
    VSCREEN             *vs;
    uisize              width;
    int                 i;
    uisize              depth;
    uisize              linelen;
    char                *s, *t, *line;
    SAREA               area;

    uisetscreenarea( &area, true, false );
    width = UIData->width - 2;
    depth = 0;
    for( i = 0; lines[i] != NULL; ++i ) {
        s = lines[i];
        for( t = s + strlen( s ); s < t; ) {
            depth++;
            linelen = ui_split_line( &s, t, width );
            if( maxlen < linelen + 2 ) {
                maxlen = linelen + 2;
            }
        }
    }
    if( depth > 0 ) {
        /* never put text on the first line of the dialog */
        depth++;
    }
    if( maxlen > width )
        maxlen = width;
    vs = makevs( title, maxlen, depth + extra_rows, cpos, rpos );
    uisetarea( &area, vs );
    width = area.width;
    area.row = 1;
    area.height = 1;
    for( i = 0; lines[i] != NULL; ++i ) {
        s = lines[i];
        for( t = s + strlen( s ); s < t; ) {
            line = s;
            linelen = ui_split_line( &s, t, width );
            uicntrtext( vs, &area, attr, linelen, line );
            area.row += 1;
        }
    }
    return( vs );
}

void uifinidialog( VSCREEN *vs )
{
    uiclose( vs );
}

static void enter_field( a_dialog *ui_dlg_info, VFIELD *field )
{
    an_edit_control     *edit;
    a_combo_box         *combo;
    a_list              *list;
    SAREA               area;

    if( field == NULL )
        return;

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
                                  UIData->attrs[ATTR_CURR_EDIT],
                                  edit->buffer, edit->length, 0, 0, true, 0,
                                  field->typ == FLD_INVISIBLE_EDIT );
        }
        break;
    case FLD_LISTBOX :
        list = field->u.list;
        list->box->uiattr = ATTR_CURR_EDIT;
        uipaintlistbox( list );
        break;
    case FLD_EDIT_MLE :
        list = field->u.list;
        list->box->uiattr = ATTR_NORMAL;
        uipaintlistbox( list );
        break;
    }
}

void uireinitdialog( a_dialog *ui_dlg_info, VFIELD *fields )
{
    uiposnhotspots( ui_dlg_info->vs, fields );
    ui_dlg_info->dirty = false;
    ui_dlg_info->first = NULL;
    ui_dlg_info->other = NULL;
    ui_dlg_info->fields = fields;

    /* set first to be first field in tab sequence */
    for( ui_dlg_info->first = fields; notintab( ui_dlg_info->first ); ui_dlg_info->first++ ) {
        if( ui_dlg_info->first->typ == FLD_NONE ) {
            ui_dlg_info->first = NULL;
            break;
        }
    }
    ui_dlg_info->curr = ui_dlg_info->first;
    if( ui_dlg_info->first != NULL ) {
        /* set curr to first field in tab sequence not an unset radio button */
        while( radiooff( ui_dlg_info->curr ) ) {
            ui_dlg_info->curr = nextfield( ui_dlg_info->curr );
        }
    }

    for( ; fields->typ != FLD_NONE; fields++ ) {
        print_field( ui_dlg_info->vs, fields, ( fields == ui_dlg_info->curr ) );
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

a_dialog *uibegdialog( const char *title, VFIELD *fields, unsigned rows, unsigned cols, int rpos, int cpos )
{
    char                *lines[1];
    a_dialog            *ui_dlg_info;

    lines[0] = NULL;
    ui_dlg_info = uimalloc( sizeof( a_dialog ) );
    if( ui_dlg_info == NULL ) {
        return( NULL );
    }
    ui_dlg_info->field = 0;
    ui_dlg_info->edit_data = NULL;
    ui_dlg_info->moving = false;
    ui_dlg_info->vs = uiinitdialog( title, UIData->attrs[ATTR_NORMAL], lines, rows, cols, rpos, cpos );
    uireinitdialog( ui_dlg_info, fields );
    return( ui_dlg_info );
}

static void do_radio( a_dialog *ui_dlg_info, VFIELD *field )
{
    a_radio *cur_radio;
    VFIELD  *fields;
    a_radio *radio;

    // might want to use ui_dlg_info->first, kind of unsure
    cur_radio = field->u.radio;
    if( cur_radio->value == cur_radio->group->value )
        return;

    ui_dlg_info->dirty = true;

    for( fields = ui_dlg_info->fields; fields->typ != FLD_NONE; fields++ ) {
        if( fields->typ == FLD_RADIO ) {
            radio = fields->u.radio;
            if( radio->group == cur_radio->group && radio->value == radio->group->value ) {
                radio->group->value = cur_radio->value;
                print_field( ui_dlg_info->vs, fields, false );
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
    UIPICKGETTEXT       *fn_get;

    edit  = &combo->edit;
    list  = &combo->list;
    str = (char *)uimalloc( CTRL_BUF_LEN + 1 );
    if( str != NULL ) {
        fn_get = list->get;
        if( fn_get == NULL )
            fn_get = uigetlistelement;
        if( (*fn_get)( list->data_handle, list->choice, str, CTRL_BUF_LEN ) ) {
            /* str does not have to be null terminated */
            /* terminate it at maximum length */
            str[CTRL_BUF_LEN] = '\0';
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

static ui_event pulldownfilter( ui_event ui_ev, a_dialog *ui_dlg_info )
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
    switch( ui_ev ) {
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
        if( row < area.row || row >= area.row + area.height )
            break;
        if( col < area.col || col >= area.col + area.width ) {
            break;
        }
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
        if( ui_ev != EV_MOUSE_PRESS && ui_ev != EV_MOUSE_DCLICK
                        && fld->typ == FLD_COMBOBOX ) {
            ui_dlg_info->dirty = true;
            setcombobuffer( ui_dlg_info, fld );
            print_field( ui_dlg_info->vs, fld, true );
        }
        break;
    }
    return( ui_ev );
}

static void *forwardtab( a_dialog *ui_dlg_info )
{
    VFIELD             *fld;

    if( ui_dlg_info->curr == NULL ) {
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
    for( fld = ui_dlg_info->first; fld != ui_dlg_info->curr; fld = nextfield( fld ) ) {
        if( !radiooff( fld ) ) {
            hold = fld;
        }
    }
    if( hold == NULL ) {        /* wrap */
        for( ; fld != NULL; fld = nextfield( fld ) ) {
            if( !radiooff( fld ) ) {
                hold = fld;
            }
        }
    }
    return( hold );
}

void uidialogexitcurr( a_dialog *ui_dlg_info )
{
    /* you must call this function to set ui_dlg_info->dirty or to do  */
    /* blank trimming on an edit control                        */
    if( exit_field( ui_dlg_info, ui_dlg_info->curr ) ) {
        ui_dlg_info->dirty = true;
    }
}

void uidialogsetcurr( a_dialog *ui_dlg_info, VFIELD *curr )
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

    /* unused parameters */ (void)ui_dlg_info;

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
    VFIELD      *fields;

    vs = ui_dlg_info->vs;

    uivclose( vs );
    uivopen( vs );

    for( fields = ui_dlg_info->fields; fields->typ != FLD_NONE; fields++ ) {
        print_field( ui_dlg_info->vs, fields, ( fields == ui_dlg_info->curr ) );
    }
}

bool uiresizedialog( a_dialog *ui_dlg_info, SAREA *new_area )
{
    int         row_diff;
    int         col_diff;
    VSCREEN     *vs;
    VFIELD      *fields;
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
    for( fields = ui_dlg_info->fields; fields->typ != FLD_NONE; fields++ ) {
        uimovefield( ui_dlg_info, fields, row_diff, col_diff );
    }
    if( resize ) {
        for( fields = ui_dlg_info->fields; fields->typ != FLD_NONE; fields++ ) {
            print_field( ui_dlg_info->vs, fields, ( fields == ui_dlg_info->curr ) );
        }
    }
    return( true );
}

static  ORD     PrevRow = 0;
static  ORD     PrevCol = 0;

static ui_event uicheckmove( ui_event ui_ev, a_dialog *ui_dlg_info )
{
    ui_event    new_ui_ev;
    ORD         row;
    ORD         col;
    SAREA       new_area;
    VSCREEN     *vs;
    int         prev_row;
    int         prev_col;

    new_ui_ev = ui_ev;
    switch( ui_ev ) {
    case EV_MOUSE_PRESS :
        if( uivmousepos( NULL, &row, &col ) == ui_dlg_info->vs ) {
            vs = ui_dlg_info->vs;
            if( ( row + 1 ) == vs->area.row ) { /* because it's framed */
                PrevRow = row;
                PrevCol = col;
                ui_dlg_info->moving = true;
                new_ui_ev = EV_NO_EVENT;
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
            new_ui_ev = EV_NO_EVENT;
        }
        break;
    case EV_MOUSE_RELEASE :
        if( ui_dlg_info->moving ) {
            ui_dlg_info->moving = false;
            new_ui_ev = EV_NO_EVENT;
        }
    }
    return( new_ui_ev );
}

static ui_event uitabkey( ui_event ui_ev, a_dialog *ui_dlg_info )
{
    VFIELD          *curr, *fld;
    ORD             row, col;
    SAREA           area;
    ui_event        new_ui_ev;

    if( ui_dlg_info->first == NULL )
        return( ui_ev );
    curr = ui_dlg_info->curr;
    new_ui_ev = ui_ev;
    switch( ui_ev ) {
    case EV_MOUSE_DCLICK:
    case EV_MOUSE_PRESS:
        {
            a_combo_box         *combo;
            a_list              *list;
            VSCREEN             *mouse_vs;

            mouse_vs = uivmousepos( ui_dlg_info->vs, &row, &col );
            for( fld = ui_dlg_info->first; fld != NULL; fld = nextfield( fld ) ) {
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
                if( mouse_vs != ui_dlg_info->vs ) {
                    if( list != NULL && list->box != NULL ) {
                        if( list->box->vs == mouse_vs ) {
                            break;
                        }
                    }
                } else if( row >= area.row && row < area.row + area.height &&
                            col >= area.col && col < area.col + area.width ) {
                    break;
                }
            }
            /* check boxes don't get mouse events unless mouse over them */
            if( fld == NULL && curr != NULL && curr->typ == FLD_CHECK ) {
                new_ui_ev = EV_NO_EVENT;
            } else {
                curr = fld;
            }
        }
        break;
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
                        new_ui_ev = EV_CHECK_BOX_CLICK;
                    }
                }
            }
        }
        break;
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
                        new_ui_ev = EV_CHECK_BOX_CLICK;
                    }
                }
            }
        }
        break;
    }
    if( ui_dlg_info->curr != curr ) {
        uidialogsetcurr( ui_dlg_info, curr );
    }
    return( new_ui_ev );
}

ui_event uihotkeyfilter( a_dialog *ui_dlg_info, ui_event ui_ev )
{
    char        ch, hotkey;
    VFIELD      *fields;

    /* is the event a key press or alt-key press */
    if( iseditchar( ui_ev ) && isalpha( (unsigned char)ui_ev ) ) {
        ch = tolower( (unsigned char)ui_ev );
    } else {
        ch = uialtchar( ui_ev );
    }

    if( ch ) {
        hotkey = '\0';
        for( fields = ui_dlg_info->fields; fields->typ != FLD_NONE; fields++ ) {
            switch( fields->typ ) {
            case FLD_HOT:
                hotkey = fields->u.hs->flags;
                break;
            case FLD_CHECK:
                hotkey = fields->u.check->hotkey;
                break;
            case FLD_RADIO:
                hotkey = fields->u.radio->hotkey;
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
        if( ui_dlg_info->curr != fields && fields->typ != FLD_NONE ) {
            uidialogsetcurr( ui_dlg_info, fields );
        }

        if( hotkey == ch ) {
            switch( fields->typ ) {
            case FLD_HOT:
                ui_ev = fields->u.hs->event;
                break;
            case FLD_CHECK:
                ui_dlg_info->dirty = true;
                fields->u.check->val = !fields->u.check->val;
                print_field( ui_dlg_info->vs, fields, true );
                ui_ev = EV_CHECK_BOX_CLICK;
                break;
            case FLD_RADIO:
                do_radio( ui_dlg_info, fields );
                ui_ev = EV_CHECK_BOX_CLICK;
                break;
            }
        }
    }

    return( ui_ev );
}

ui_event uiprocessdialogevent( ui_event ui_ev, a_dialog *ui_dlg_info )
{
    VFIELD              *field;
    a_check             *check;
    an_edit_control     *edit;
    unsigned            choice;
    a_combo_box         *combo;
    a_list              *list;

    ui_ev = uicheckmove( ui_ev, ui_dlg_info );
    ui_ev = uitabkey( ui_ev, ui_dlg_info );
    field = ui_dlg_info->curr;
    if( field != NULL ) {
        switch( field->typ ) {
        case FLD_CHECK:
        case FLD_RADIO:
            switch( ui_ev ) {
            case EV_MOUSE_DCLICK:
            case EV_MOUSE_PRESS:
            case ' ' :
                ui_ev = EV_CHECK_BOX_CLICK;
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
            ui_ev = uiledit( ui_ev );
            edit = field->u.edit;
            if( ui_dlg_info->edit_data != NULL ) {
                edit->buffer = ui_dlg_info->edit_data->edit_buffer;
                edit->length = ui_dlg_info->edit_data->edit_eline.length;
            }
            break;
        case FLD_LISTBOX:
        case FLD_EDIT_MLE:
            list = field->u.list;
            ui_ev = uilistbox( ui_ev, list, true );
            break;
        case FLD_PULLDOWN:
            list = field->u.list;
            if( list->box == NULL ) {
                ui_ev = pulldownfilter( ui_ev, ui_dlg_info );
            } else {
                choice = list->choice;
                ui_ev = uilistbox( ui_ev, list, false );
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
                ui_ev = pulldownfilter( ui_ev, ui_dlg_info );
            } else {
                choice = list->choice;
                ui_ev = uilistbox( ui_ev, list, combo->perm );
                if( choice != list->choice ) {
                    ui_dlg_info->dirty = true;
                    setcombobuffer( ui_dlg_info, field );
                    print_field( ui_dlg_info->vs, field, true );
                }
            }
            ui_ev = uiledit( ui_ev );
            if( ui_dlg_info->edit_data != NULL ) {
                edit->buffer = ui_dlg_info->edit_data->edit_buffer;
                edit->length = ui_dlg_info->edit_data->edit_eline.length;
            }
            break;
        }
    }
    ui_ev = uihotkeyfilter( ui_dlg_info, ui_ev );
    ui_ev = uihotspotfilter( ui_dlg_info->vs, ui_dlg_info->fields, ui_ev );
    return( ui_ev );
}

ui_event uidialog( a_dialog *ui_dlg_info )
{
    static ui_event dialog_events[] = {
        'a',            'z',
        'A',            'Z',
        EV_ALT_Q,       EV_ALT_M,
        __rend__,
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
        __end__
    };
    ui_event    ui_ev;
    VFIELD      *field;

    ui_ev = EV_NO_EVENT;
    enter_field( ui_dlg_info, ui_dlg_info->curr );

    while( ui_ev == EV_NO_EVENT || !uiinlists( ui_ev ) ) {
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
        ui_ev = uidialogevent( ui_dlg_info->vs );
        ui_ev = uidialogcallback( ui_dlg_info, ui_ev );
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
        ui_ev = uiprocessdialogevent( ui_ev, ui_dlg_info );
    }
    /* This code will make sure to exit the current fields before returning
     * a default hot spot event. This is for consistency with windows.
     */
    if( uiisdefaulthotspot( ui_dlg_info->fields, ui_ev ) ) {
        if( exit_field( ui_dlg_info, ui_dlg_info->curr ) ) {
            ui_dlg_info->dirty = true;
        }
    }
    ui_dlg_info->field = ui_dlg_info->curr - ui_dlg_info->fields;
    return( ui_ev );
}

void uifreedialog( a_dialog *ui_dlg_info )
{
    VFIELD              *fields;  // pointer to array of fields in dialog box
    a_list              *list;
    a_combo_box         *combo;
//  an_edit_control     *edit;

    exit_field( ui_dlg_info, ui_dlg_info->curr );

    for( fields = ui_dlg_info->fields; fields->typ != FLD_NONE; fields++ ) {
        switch( fields->typ ) {
        case FLD_LISTBOX:
        case FLD_PULLDOWN:
        case FLD_EDIT_MLE:
            list = fields->u.list;
            uiendlistbox( list );
            break;
//        case FLD_INVISIBLE_EDIT:
//        case FLD_EDIT:
//            edit = fields->u.edit;
//            uifree( edit->buffer );
//            edit->buffer = NULL;        //  Need this null for next
//            break;                      //  time around
        case FLD_COMBOBOX:
            combo = fields->u.combo;
            list = &combo->list;
//            edit = &combo->edit;
//            uifree( edit->buffer );
//            edit->buffer = NULL;
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

    for( ; fields->typ != FLD_NONE; fields++ ) {
        switch( fields->typ ) {
        case FLD_INVISIBLE_EDIT:
        case FLD_EDIT:
            edit = fields->u.edit;
            uifree( edit->buffer );
            edit->buffer = NULL;                //  Need this null for next
            break;                              //  time around
        case FLD_COMBOBOX:
            combo = fields->u.combo;
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
