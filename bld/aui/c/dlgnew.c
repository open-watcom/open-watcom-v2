/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#include "_aui.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include <string.h>


#define R0 0
#define R1 2
#define C0 1
#define W  48
#define BW 12
#define B1 BUTTON_POS( 1, 2, W, BW )
#define B2 BUTTON_POS( 2, 2, W, BW )

//                      ROWS    COLS    MAX_COLS
#define DLG_SIZE_DATA   4,      W,      70

#define DLGNEW_CTLS() \
    pick_p4id( EDIT,    GUI_CTL_EDIT,       "",   C0, R0, W - C0 ) \
    pick_p4id( OK,      GUI_CTL_DEFBUTTON,  NULL, B1, R1, BW + 1 ) \
    pick_p4id( CANCEL,  GUI_CTL_BUTTON,     NULL, B2, R1, BW + 1 )

enum {
    DUMMY_ID = 100,
    #define pick_p4id(id,m,p1,p2,p3,p4) CTL_ ## id,
    DLGNEW_CTLS()
    #undef pick_p4id
};

enum {
    #define pick_p4id(id,m,p1,p2,p3,p4) id ## _IDX,
    DLGNEW_CTLS()
    #undef pick_p4id
};

static gui_control_info Controls[] = {
    #define pick_p4id(id,m,p1,p2,p3,p4) m(p1,CTL_ ## id,p2,p3,p4),
    DLGNEW_CTLS()
    #undef pick_p4id
};

static bool GUICALLBACK dlgNewGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
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

bool    WNDAPI DlgNewWithCtl( const char *title, char *buff, size_t buff_len, gui_control_info *controls,
                    int num_controls, GUIEVCALLBACK *gui_call_back, gui_text_ord rows, gui_text_ord cols, gui_text_ord max_cols )
{
    dlgnew_ctl  dlgnew;
    int         len;

    len = strlen( title );
    if( cols < len )
        cols = len;
    if( cols > max_cols )
        cols = max_cols;
    dlgnew.buff = buff;
    dlgnew.buff_len = buff_len;
    dlgnew.cancel = true;
    DlgOpen( title, rows, cols, controls, num_controls, gui_call_back, &dlgnew );
    return( !dlgnew.cancel );
}

bool    WNDAPI DlgNew( const char *title, char *buff, size_t buff_len )
{
    bool        rc;

    Controls[OK_IDX].text = WndLoadString( LIT_AUI( New_OK ) );
    Controls[CANCEL_IDX].text = WndLoadString( LIT_AUI( New_Cancel ) );
    rc = DlgNewWithCtl( title, buff, buff_len, Controls, ArraySize( Controls ), dlgNewGUIEventProc, DLG_SIZE_DATA );
    WndFree( (void *)Controls[OK_IDX].text );
    WndFree( (void *)Controls[CANCEL_IDX].text );
    return( rc );
}
