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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "app.h"


#define R0 0
#define R1 2
#define C0 1
#define W  48
#define BW 12
#define B1 BUTTON_POS( 1, 2, W, BW )
#define B2 BUTTON_POS( 2, 2, W, BW )

//                      ROWS    COLS    MAX_COLS
#define DLG_SIZE_DATA   4,      W,      70

#define DLGPASSWORD_CTLS() \
    pick_p4id( EDIT,    GUI_CTL_INVISIBLE_EDIT, "",         C0, R0, W - C0 ) \
    pick_p4id( OK,      GUI_CTL_DEFBUTTON,      "OK",       B1, R1, BW + 1 ) \
    pick_p4id( CANCEL,  GUI_CTL_BUTTON,         "Cancel",   B2, R1, BW + 1 )

enum {
    DUMMY_ID = 100,
    #define pick_p4id(id,m,p1,p2,p3,p4)     CTL_ ## id,
    DLGPASSWORD_CTLS()
    #undef pick_p4id
};

enum {
    #define pick_p4id(id,m,p1,p2,p3,p4)     id ## _IDX,
    DLGPASSWORD_CTLS()
    #undef pick_p4id
};

static gui_control_info Controls[] = {
    #define pick_p4id(id,m,p1,p2,p3,p4)     m(p1,CTL_ ## id,p2,p3,p4),
    DLGPASSWORD_CTLS()
    #undef pick_p4id
};

static bool passwordGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    dlgnew_ctl  *dlgnew;

    dlgnew = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUISetText( gui, CTL_EDIT, dlgnew->buff);
        GUISetFocus( gui, CTL_EDIT );
        dlgnew->buff[0] = '\0';
        return( true );
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        dlgnew->buff[0] = '\0';
        switch( id ) {
        case CTL_OK:
            GUIDlgBuffGetText( gui, CTL_EDIT, dlgnew->buff, dlgnew->buff_len );
            dlgnew->cancel = false;
            /* fall through */
        case CTL_CANCEL:
            GUICloseDialog( gui );
            return( true );
        default:
            break;
        }
        break;
    case GUI_DESTROY:
        return( true );
    default:
        break;
    }
    return( false );
}

void    Password( const char *title, char *buff, size_t buff_len )
{
    DlgNewWithCtl( title, buff, buff_len, Controls, ArraySize( Controls ), passwordGUIEventProc, DLG_SIZE_DATA );
}
