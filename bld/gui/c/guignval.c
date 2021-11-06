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


#include "guiwind.h"
#include "guidlg.h"
#include "guistr.h"
#include <string.h>

#define MIN_LENGTH      10
#define MAX_LENGTH      25
#define BUTTON_WIDTH    7
#define TEXT_ROW        0
#define BUTTON_ROW      2
#define NUM_ROWS        4
#define START_STATIC    5
#define START_EQUAL     7
#define START_EDIT      9

/* all 0 values are set in the code */

#define DLGNEW_CTLS() \
    pick_p4(   STATIC,  GUI_CTL_STRING,     DLG_SET_RECT_CHARCOORD_1,   NULL,   START_STATIC, TEXT_ROW,   0 ) \
    pick_p4(   EQUAL,   GUI_CTL_STRING,     DLG_SET_RECT_CHARCOORD_1,   "=",    0,            TEXT_ROW,   1 ) \
    pick_p4id( EDIT,    GUI_CTL_EDIT,       DLG_SET_RECT_CHARCOORD_1,   NULL,   0,            TEXT_ROW,   0 ) \
    pick_p4id( CANCEL,  GUI_CTL_BUTTON,     DLG_SET_RECT_CHARCOORD_1,   NULL,   0,            BUTTON_ROW, BUTTON_WIDTH ) \
    pick_p4id( OK,      GUI_CTL_DEFBUTTON,  DLG_SET_RECT_CHARCOORD_1,   NULL,   0,            BUTTON_ROW, BUTTON_WIDTH )

enum {
    DUMMY_ID = 100,
    #define pick_p4(id,m,s,p1,p2,p3,p4)   CTL_ ## id,
    #define pick_p4id(id,m,s,p1,p2,p3,p4) CTL_ ## id,
    DLGNEW_CTLS()
    #undef pick_p4id
    #undef pick_p4
};

enum {
    #define pick_p4(id,m,s,p1,p2,p3,p4)   id ## _IDX,
    #define pick_p4id(id,m,s,p1,p2,p3,p4) id ## _IDX,
    DLGNEW_CTLS()
    #undef pick_p4id
    #undef pick_p4
};

static gui_control_info GetNew[] = {
    #define pick_p4(id,m,s,p1,p2,p3,p4)   m(p1,p2,p3,p4),
    #define pick_p4id(id,m,s,p1,p2,p3,p4) m(p1,CTL_ ## id,p2,p3,p4),
    DLGNEW_CTLS()
    #undef pick_p4id
    #undef pick_p4
};

typedef struct ret_info {
    char                *text;
    gui_message_return  ret_val;
} ret_info;

/*
 * GetNewValGUIEventProc - call back routine for the GetNewVal dialog
 */

static bool GetNewValGUIEventProc( gui_window *wnd, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    ret_info    *info;

    info = GUIGetExtra( wnd );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        info->ret_val = GUI_RET_CANCEL;
        return( true );
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_CANCEL:
            GUICloseDialog( wnd );
            info->ret_val = GUI_RET_CANCEL;
            return( true );
        case CTL_OK:
            info->text = GUIGetText( wnd, CTL_EDIT );
            GUICloseDialog( wnd );
            info->ret_val = GUI_RET_OK;
            return( true );
        default:
            break;
        }
        break;
    default:
        break;
    }
    return( false );
}

/*
 * GUIGetNewVal --
 */

gui_message_return GUIAPI GUIGetNewVal( const char *title, const char *old, char **new_val )
{
    size_t          length;
    size_t          disp_length;
    gui_text_ord    cols;
    ret_info        info;

    info.ret_val = GUI_RET_ABORT;
    info.text = NULL;
    length = 0;
    if( old != NULL ) {
        length = strlen( old );
    }
    disp_length = length;
    if( disp_length < MIN_LENGTH ) {
        disp_length = MIN_LENGTH;
    }
    if( disp_length > MAX_LENGTH ) {
        disp_length = MAX_LENGTH;
    }

    /* reset dialog character coordinates */
    #define pick_p4(id,m,s,p1,p2,p3,p4)   s(GetNew[id ## _IDX],p2,p3,p4);
    #define pick_p4id(id,m,s,p1,p2,p3,p4) s(GetNew[id ## _IDX],p2,p3,p4);
    DLGNEW_CTLS()
    #undef pick_p4id
    #undef pick_p4

    cols = START_EDIT + disp_length * 2 + START_STATIC;

    GetNew[STATIC_IDX].rect.width = disp_length;
    GetNew[STATIC_IDX].text = old;

    GetNew[EQUAL_IDX].rect.x = START_EQUAL + disp_length;

    GetNew[EDIT_IDX].style |= GUI_STYLE_CONTROL_FOCUS;
    GetNew[EDIT_IDX].rect.x = START_EDIT + disp_length;
    GetNew[EDIT_IDX].rect.width = disp_length;
    GetNew[EDIT_IDX].text = old;

    GetNew[CANCEL_IDX].text = LIT( Cancel );
    GetNew[CANCEL_IDX].rect.x = cols - ( ( cols / 2 - BUTTON_WIDTH ) / 2 ) - BUTTON_WIDTH;

    GetNew[OK_IDX].text = LIT( OK );
    GetNew[OK_IDX].rect.x = ( cols / 2 ) - ( ( cols / 2 - BUTTON_WIDTH ) / 2 ) - BUTTON_WIDTH;

    GUIDlgOpen( title, NUM_ROWS, cols, GetNew, GUI_ARRAY_SIZE( GetNew ), &GetNewValGUIEventProc, &info );
    *new_val = info.text;
    return( info.ret_val );
}
