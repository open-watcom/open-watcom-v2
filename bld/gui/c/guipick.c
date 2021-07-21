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
#include "guipick.h"
#include <string.h>


#define DLG_PICK_ROWS   14
#define DLG_PICK_COLS   30
#define DLG_MAX_COLS    75

#define DLGPICK_CTLS() \
    pick_p4id( LIST,     GUI_CTL_LIST_BOX,   1,  1,  29, 10 ) \
    pick_p3id( OK,       GUI_CTL_DEFBUTTON,  2,  12, 11 ) \
    pick_p3id( CANCEL,   GUI_CTL_BUTTON,     18, 12, 11 )

enum {
    #define pick_p3id(id,m,p1,p2,p3)    id ## _IDX,
    #define pick_p4id(id,m,p1,p2,p3,p4) id ## _IDX,
    DLGPICK_CTLS()
    #undef pick_p4id
    #undef pick_p3id
};

static gui_control_info Controls[] = {
    #define pick_p3id(id,m,p1,p2,p3)     m(NULL,CTL_PICK_ ## id,p1,p2,p3),
    #define pick_p4id(id,m,p1,p2,p3,p4)  m(NULL,CTL_PICK_ ## id,p1,p2,p3,p4),
    DLGPICK_CTLS()
    #undef pick_p4id
    #undef pick_p3id
};

bool GUIPickGUIEventProc( gui_window *wnd, gui_event gui_ev, void *param )
{
    gui_ctl_id          id;
    dlg_pick            *dlg;

    dlg = (dlg_pick*)GUIGetExtra( wnd );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUIClearList( wnd, CTL_PICK_LIST );
        (*dlg->func)( wnd, CTL_PICK_LIST );
        GUISetFocus( wnd, CTL_PICK_LIST );
        return( true );
    case GUI_CONTROL_DCLICKED:
        GUI_GETID( param, id );
        if( id == CTL_PICK_LIST ) {
            dlg->choice = -1;
            GUIGetCurrSelect( wnd, CTL_PICK_LIST, &dlg->choice );
            GUICloseDialog( wnd );
            return( true );
        }
        break;
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_PICK_OK:
            dlg->choice = -1;
            GUIGetCurrSelect( wnd, CTL_PICK_LIST, &dlg->choice );
            /* fall through */
        case CTL_PICK_CANCEL:
            GUICloseDialog( wnd );
            return( true );
        }
        break;
    default:
        break;
    }
    return( false );
}


bool GUIAPI GUIDlgPickWithRtn( const char *title, GUIPICKCALLBACK *pickinit, PICKDLGOPEN *openrtn, int *choice )
{
    dlg_pick    dlg;
    size_t      len;

    len = strlen( title );
    if( len < DLG_PICK_COLS )
        len = DLG_PICK_COLS;
    if( len > DLG_MAX_COLS )
        len = DLG_MAX_COLS;

    Controls[OK_IDX].text = LIT( OK );
    Controls[CANCEL_IDX].text = LIT( Cancel );
    dlg.func = pickinit;
    dlg.choice = -1;
    openrtn( title, DLG_PICK_ROWS, (gui_text_ord)len, Controls, GUI_ARRAY_SIZE( Controls ), &GUIPickGUIEventProc, &dlg );
    if( dlg.choice == -1 )
        return( false );
    *choice = dlg.choice;
    return( true );
}


bool GUIAPI GUIDlgPick( const char *title, GUIPICKCALLBACK *pickinit, int *choice )
{
    return( GUIDlgPickWithRtn( title, pickinit, GUIDlgOpen, choice ) );
}
