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


extern void             DlgSetLong( gui_window *gui, unsigned id, long value );
extern bool             DlgGetLong( gui_window *gui, unsigned id, long *value );
extern void             TabIntervalSet( int );
extern void             AsmChangeOptions( void );
extern void             VarChangeOptions( void );
extern void             GlobChangeOptions( void );
extern void             ModChangeOptions( void );
extern void             FuncChangeOptions( void );
extern unsigned         NewCurrRadix( unsigned rad );


static void GetDlgStatus( gui_window *gui )
{
    long        tab;
    unsigned    old;

    old = NewCurrRadix( 10 );
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
    NewCurrRadix( old );
}


static void SetDlgStatus( gui_window *gui )
{
    unsigned    old;

    old = NewCurrRadix( 10 );
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
    NewCurrRadix( old );
}


OVL_EXTERN bool WndSetEvent( gui_window * gui, gui_event gui_ev, void * param )
{
    unsigned    id;
    dlg_window_set      *wndset;

    wndset = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        GUISetFocus( gui, CTL_WIND_ASM_SOURCE );
        SetDlgStatus( gui );
        return( TRUE );
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_WIND_OK:
            wndset->cancel = FALSE;
            GetDlgStatus( gui );
            GUICloseDialog( gui );
            break;
        case CTL_WIND_CANCEL:
            wndset->cancel = TRUE;
            GUICloseDialog( gui );
            break;
        case CTL_WIND_DEFAULTS:
            SetDlgStatus( gui );
            break;
        }
        return( TRUE );
    }
    return( FALSE );
}


extern  bool    DlgWndSet( void )
{
    dlg_window_set      wndset;

    wndset.cancel = TRUE;
    ResDlgOpen( &WndSetEvent, &wndset, DIALOG_WIND );
    if( wndset.cancel ) return( FALSE );
    return( TRUE );
}
