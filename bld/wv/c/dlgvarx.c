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
#include "dbgwind.h"
#include "guidlg.h"
#include "dlgvarx.h"
#include "dbgerr.h"
#include "dbgtoggl.h"
#include "string.h"

extern char             *TxtBuff;

extern void             DlgSetLong( gui_window *gui, unsigned id, long value );
extern bool             DlgGetLong( gui_window *gui, unsigned id, long *value );

static bool GetDlgStatus( dlg_var_expand *varx, gui_window *gui )
{
    long        start,end;

    if( DlgGetLong( gui, CTL_VARX_START, &start ) &&
        DlgGetLong( gui, CTL_VARX_END, &end ) && end >= start ) {
        varx->start = start;
        varx->end = end;
        return( TRUE );
    }
    _SwitchOn( SW_ERROR_RETURNS );
    Error( ERR_NONE, LIT( ERR_BAD_ARRAY_INDEX ) );
    _SwitchOff( SW_ERROR_RETURNS );
    GUISetFocus( gui, CTL_VARX_START );
    return( FALSE );
}


static void SetDlgStatus( dlg_var_expand *varx, gui_window *gui )
{
    DlgSetLong( gui, CTL_VARX_START, varx->start );
    DlgSetLong( gui, CTL_VARX_END, varx->end );
}


OVL_EXTERN bool VarEvent( gui_window * gui, gui_event gui_ev, void * param )
{
    unsigned    id;
    dlg_var_expand      *varx;

    varx = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        SetDlgStatus( varx, gui );
        GUISetFocus( gui, CTL_VARX_START );
        return( TRUE );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_VARX_OK:
            if( GetDlgStatus( varx, gui ) ) {
                varx->cancel = FALSE;
                GUICloseDialog( gui );
            }
            break;
        case CTL_VARX_CANCEL:
            varx->cancel = TRUE;
            GUICloseDialog( gui );
            break;
        case CTL_VARX_DEFAULTS:
            SetDlgStatus( varx, gui );
            break;
        }
        return( TRUE );
    }
    return( FALSE );
}


extern  bool    DlgVarExpand( dlg_var_expand *pvarx )
{
    dlg_var_expand      varx;

    varx.start = pvarx->start;
    varx.end = pvarx->end;
    varx.cancel = TRUE;
    ResDlgOpen( &VarEvent, &varx, DIALOG_VARX );
    if( varx.cancel ) return( FALSE );
    pvarx->start = varx.start;
    pvarx->end = varx.end;
    return( TRUE );
}
