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
* Description:  Configuration of variables display.
*
****************************************************************************/


#include "_srcmgt.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "guidlg.h"
#include "dbgerr.h"
#include "dlgwind.h"
#include "dbgscan.h"
#include "dlgexpr.h"
#include "dbgwass.h"
#include "dbgwfunc.h"
#include "dbgwglob.h"
#include "dbgwmod.h"
#include "dbgwset.h"
#include "dbgwvar.h"


typedef struct dlg_window_set {
    bool            cancel : 1;
} dlg_window_set;

static void GetDlgStatus( gui_window *gui )
{
    long        tab;
    mad_radix   old_radix;

    old_radix = NewCurrRadix( 10 );
    _SwitchSet( SW_ASM_SOURCE, GUIIsChecked( gui, CTL_WIND_ASM_SOURCE ) );
    _SwitchSet( SW_ASM_HEX, GUIIsChecked( gui, CTL_WIND_ASM_HEX ) );
    _SwitchSet( SW_VAR_WHOLE_EXPR, GUIIsChecked( gui, CTL_WIND_VAR_EXPR ) );
    _SwitchSet( SW_VAR_SHOW_CODE, GUIIsChecked( gui, CTL_WIND_VAR_CODE ) );
    _SwitchSet( SW_VAR_SHOW_INHERIT, GUIIsChecked( gui, CTL_WIND_VAR_INHERIT ) );
    _SwitchSet( SW_VAR_SHOW_COMPILER, GUIIsChecked( gui, CTL_WIND_VAR_COMPILER ) );
    _SwitchSet( SW_VAR_SHOW_MEMBERS, GUIIsChecked( gui, CTL_WIND_VAR_MEMBERS ) );
    _SwitchSet( SW_VAR_SHOW_PRIVATE, GUIIsChecked( gui, CTL_WIND_VAR_PRIVATE ) );
    _SwitchSet( SW_VAR_SHOW_PROTECTED, GUIIsChecked( gui, CTL_WIND_VAR_PROTECTED ) );
    _SwitchSet( SW_VAR_SHOW_STATIC, GUIIsChecked( gui, CTL_WIND_VAR_STATIC ) );
    _SwitchSet( SW_FUNC_D2_ONLY, GUIIsChecked( gui, CTL_WIND_FUNC_TYPED ) );
    _SwitchSet( SW_GLOB_D2_ONLY, GUIIsChecked( gui, CTL_WIND_GLOB_TYPED ) );
    _SwitchSet( SW_MOD_ALL_MODULES, GUIIsChecked( gui, CTL_WIND_MOD_ALL ) );
    if( DlgGetLong( gui, CTL_WIND_FILE_TAB, &tab ) ) {
        TabIntervalSet( tab );
    }
    AsmChangeOptions();
    VarChangeOptions();
    FuncChangeOptions();
    GlobChangeOptions();
    ModChangeOptions();
    NewCurrRadix( old_radix );
}


static void SetDlgStatus( gui_window *gui )
{
    mad_radix   old_radix;

    old_radix = NewCurrRadix( 10 );
    GUISetChecked( gui, CTL_WIND_ASM_SOURCE, _IsOn( SW_ASM_SOURCE ) );
    GUISetChecked( gui, CTL_WIND_ASM_HEX, _IsOn( SW_ASM_HEX ) );
    GUISetChecked( gui, CTL_WIND_VAR_EXPR, _IsOn( SW_VAR_WHOLE_EXPR ) );
    GUISetChecked( gui, CTL_WIND_VAR_CODE, _IsOn( SW_VAR_SHOW_CODE ) );
    GUISetChecked( gui, CTL_WIND_VAR_INHERIT, _IsOn( SW_VAR_SHOW_INHERIT ) );
    GUISetChecked( gui, CTL_WIND_VAR_COMPILER, _IsOn( SW_VAR_SHOW_COMPILER ) );
    GUISetChecked( gui, CTL_WIND_VAR_MEMBERS, _IsOn( SW_VAR_SHOW_MEMBERS ) );
    GUISetChecked( gui, CTL_WIND_VAR_PRIVATE, _IsOn( SW_VAR_SHOW_PRIVATE ) );
    GUISetChecked( gui, CTL_WIND_VAR_PROTECTED, _IsOn( SW_VAR_SHOW_PROTECTED ) );
    GUISetChecked( gui, CTL_WIND_VAR_STATIC, _IsOn( SW_VAR_SHOW_STATIC ) );
    GUISetChecked( gui, CTL_WIND_FUNC_TYPED, _IsOn( SW_FUNC_D2_ONLY ) );
    GUISetChecked( gui, CTL_WIND_GLOB_TYPED, _IsOn( SW_GLOB_D2_ONLY ) );
    GUISetChecked( gui, CTL_WIND_MOD_ALL, _IsOn( SW_MOD_ALL_MODULES ) );
    DlgSetLong( gui, CTL_WIND_FILE_TAB, TabIntervalGet() );
    NewCurrRadix( old_radix );
}


OVL_EXTERN bool WndSetEvent( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id      id;
    dlg_window_set  *wndset;

    wndset = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUISetFocus( gui, CTL_WIND_ASM_SOURCE );
        SetDlgStatus( gui );
        return( true );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_WIND_OK:
            wndset->cancel = false;
            GetDlgStatus( gui );
            GUICloseDialog( gui );
            break;
        case CTL_WIND_CANCEL:
            wndset->cancel = true;
            GUICloseDialog( gui );
            break;
        case CTL_WIND_DEFAULTS:
            SetDlgStatus( gui );
            break;
        }
        return( true );
    }
    return( false );
}


bool    DlgWndSet( void )
{
    dlg_window_set      wndset;

    wndset.cancel = true;
    ResDlgOpen( &WndSetEvent, &wndset, GUI_MAKEINTRESOURCE( DIALOG_WIND ) );
    return( !wndset.cancel );
}
