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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "dlgnew.h"
#include <string.h>

#define R0 0
#define R1 2
#define C0 1
#define W 48
#define BW 12
#define B1 BUTTON_POS( 1, 2, W, BW )
#define B2 BUTTON_POS( 2, 2, W, BW )


#define DLG_NEW_ROWS    4
#define DLG_NEW_COLS    W
#define DLG_MAX_COLS    70


static gui_control_info Controls[] = {

    DLG_EDIT( "", CTL_NEW_EDIT,                 C0, R0, W-1 ),

    DLG_DEFBUTTON( "", CTL_NEW_OK,                      B1, R1, B1+BW ),
    DLG_BUTTON( "", CTL_NEW_CANCEL,             B2, R1, B2+BW ),
};


typedef struct {
    char        *buff;
    unsigned    buff_len;
    bool        cancel;
} dlg_new;

extern bool DlgNewEvent( gui_window * gui, gui_event event, void * param )
{
    unsigned    id;
    dlg_new     *dlgnew;

    dlgnew = GUIGetExtra( gui );
    switch( event ) {
    case GUI_INIT_DIALOG:
        GUISetText( gui, CTL_NEW_EDIT, dlgnew->buff);
        GUISetFocus( gui, CTL_NEW_EDIT );
        dlgnew->buff[0] = '\0';
        return( TRUE );
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        dlgnew->buff[0] = '\0';
        switch( id ) {
        case CTL_NEW_OK:
            GUIDlgBuffGetText( gui, CTL_NEW_EDIT, dlgnew->buff, dlgnew->buff_len );
            dlgnew->cancel = FALSE;
            /* fall thru */
        case CTL_NEW_CANCEL:
            GUICloseDialog( gui );
            return( TRUE );
        default:
            return( FALSE );
        }
    case GUI_DESTROY:
        return( TRUE );
    default:
        return( FALSE );
    }
}


extern  bool    DlgNewWithCtl( const char *title, char *buff, unsigned buff_len,
                               gui_control_info *controls, int num_controls,
                               GUICALLBACK *callback, int rows,
                               int cols, int max_cols )
{
    dlg_new     dlgnew;

    int len = strlen( title );
    if( len < cols )
        len = cols;
    if( len > max_cols )
        len = max_cols;
    dlgnew.buff = buff;
    dlgnew.buff_len = buff_len;
    dlgnew.cancel = TRUE;

    DlgOpen( title, rows, len, controls, num_controls, callback, &dlgnew );
    return( !dlgnew.cancel );
}


extern  bool    DlgNew( const char *title, char *buff, unsigned buff_len )
{
    bool        rc;

    Controls[1].text = WndLoadString( LITERAL_New_OK );
    Controls[2].text = WndLoadString( LITERAL_New_Cancel );
    rc = DlgNewWithCtl( title, buff, buff_len,
                   Controls, ArraySize( Controls ), &DlgNewEvent,
                   DLG_NEW_ROWS, DLG_NEW_COLS, DLG_MAX_COLS );
    WndFree( (void *)Controls[1].text );
    WndFree( (void *)Controls[2].text );
    return( rc );
}
