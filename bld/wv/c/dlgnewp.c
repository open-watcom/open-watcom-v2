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
* Description:  New Program dialog processing.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgwind.h"
#include "guidlg.h"
#include "dbgerr.h"
#include "dbgtoggl.h"
#include "dlgnewp.h"
#include "string.h"

extern char             *TxtBuff;

extern unsigned         GetProgName( char *where, unsigned len );
extern unsigned         GetProgArgs( char *where, unsigned len );
extern void             DoInput( void );

extern void             LoadNewProg( char *cmd, char *parms );
extern bool             ExeBrowse( void );

static char     prog[UTIL_LEN];
static char     args[UTIL_LEN];


OVL_EXTERN bool ProgEvent( gui_window * gui, gui_event gui_ev, void * param )
{
    unsigned            id;
    dlg_new_prog        *dlg;

    dlg = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUILimitEditText( gui, CTL_NEWP_PROG, UTIL_LEN - 1 );
        GUILimitEditText( gui, CTL_NEWP_ARGS, UTIL_LEN - 1 );
        GUISetText( gui, CTL_NEWP_PROG, dlg->prog );
        GUISetText( gui, CTL_NEWP_ARGS, dlg->args );
        GUISetFocus( gui, CTL_NEWP_PROG );
        return( TRUE );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_NEWP_BROWSE:
            GUIDlgBuffGetText( gui, CTL_NEWP_PROG, TxtBuff, TXT_LEN );
            if( !ExeBrowse() ) return( TRUE );
            GUISetText( gui, CTL_NEWP_PROG, TxtBuff );
            GUISetFocus( gui, CTL_NEWP_PROG );
            return( TRUE );
        case CTL_NEWP_OK:
            GUIDlgBuffGetText( gui, CTL_NEWP_PROG, prog, UTIL_LEN );
            GUIDlgBuffGetText( gui, CTL_NEWP_ARGS, args, UTIL_LEN );
            dlg->cancel = FALSE;
            GUICloseDialog( gui );
            return( TRUE );
        case CTL_NEWP_CANCEL:
            GUICloseDialog( gui );
            return( TRUE );
        }
        return( FALSE );
    case GUI_DESTROY:
        return( TRUE );
    }

    return( FALSE );
}

static void DoDlgNewProg( dlg_new_prog  *pdlg )
{
    GetProgName( prog, sizeof( prog ) );
    GetProgArgs( args, sizeof( args ) );
    pdlg->prog = prog;
    pdlg->args = args;
    pdlg->cancel = TRUE;
    ResDlgOpen( &ProgEvent, pdlg, DIALOG_NEW_PROG );
}

extern  void    DlgNewProg( void )
{
    dlg_new_prog        dlg;

    for( ;; ) {
        DoDlgNewProg( &dlg );
        if( dlg.cancel ) break;
        LoadNewProg( dlg.prog, dlg.args );
        if( _IsOn( SW_HAVE_TASK ) ) break;
    }
    DoInput();
}
