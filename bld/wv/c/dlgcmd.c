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


#include "dbgdefn.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgerr.h"
#include "dbginp.h"
#include "dbgtoken.h"
#include "dbgmem.h"
#include "dbgwind.h"
#include "dbgio.h"
#include "guidlg.h"
#include "dlgcmd.h"
#include <string.h>

extern char             *ReScan();
extern void             PushInpStack(void *,bool (*)(),bool );
extern void             TypeInpStack(input_type);
extern void             WndDlgTxt(char*);
extern bool             TBreak();
extern void             DlgSetHistory( gui_window *gui, void *history, char *cmd,
                           int edit, int list );
extern bool             DlgHistoryKey( gui_window *gui, void *param, int edit, int list );
extern void             DlgClickHistory( gui_window *gui, int edit, int list );
extern void             SymComplete( gui_window *gui, int id );
extern void             DoCmd(char*);

extern char             *TxtBuff;
extern void             *CmdHistory;

static bool CmdEvent( gui_window * gui, gui_event gui_ev, void * param )
{
    dlg_command *dlg;
    char        *text;
    unsigned    id;

    dlg = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        DlgSetHistory( gui, CmdHistory, dlg->cmd, CTL_CMD_EDIT, CTL_CMD_LIST );
        GUISetFocus( gui, CTL_CMD_EDIT );
        return( TRUE );
    case GUI_KEY_CONTROL:
        return( DlgHistoryKey( gui, param, CTL_CMD_EDIT, CTL_CMD_LIST ) );
    case GUI_CONTROL_DCLICKED:
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_CMD_LIST:
            DlgClickHistory( gui, CTL_CMD_EDIT, CTL_CMD_LIST );
            if( gui_ev == GUI_CONTROL_CLICKED ) return( TRUE );
            /* fall through */
        case CTL_CMD_SYMBOL:
            SymComplete( gui, CTL_CMD_EDIT );
            return( TRUE );
        case CTL_CMD_OK:
            text = GUIGetText( gui, CTL_CMD_EDIT );
            if( text != NULL ) {
                if( text[0] != '\0' ) {
                    WndSaveToHistory( CmdHistory, text );
                }
                DoCmd( text );
            }
            break;
        }
        GUICloseDialog( gui );
        return( TRUE );
    case GUI_DESTROY:
        WndFree( dlg );
        return( TRUE );
    default:
        return( FALSE );
    }
}


extern  void    DlgCmd()
{
    dlg_command *dlg;

    dlg = WndMustAlloc( sizeof( dlg_command ) );
    ResDlgOpen( &CmdEvent, dlg, DIALOG_CMD );
}
