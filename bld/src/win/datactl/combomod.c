#include <windows.h>
#include "ctltype.h"
#include <string.h>

BOOL ctl_combo_modified( ctl_elt *elt, UINT wparam, LONG lparam )
/***************************************************************/

{
    WORD        id;
    WORD        cmd;

    lparam = lparam;
    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );
    if( id == elt->control &&
        ( cmd == CBN_SELCHANGE || cmd == LBN_SELCHANGE ) ) {
        return( TRUE );
    }

    return( FALSE );
}

static BOOL is_listbox( HWND dlg, int ctl )
/*****************************************/

{
    char                buf[100];

    GetClassName( GetDlgItem( dlg, ctl ), buf, 100 );
    if( 0 == stricmp( buf, "listbox" ) ) {
        return( TRUE );
    }

    return( FALSE );
}

WORD ctl_combo_add_msg( HWND dlg, int ctl )
/*****************************************/

{
    if( is_listbox( dlg, ctl ) ) {
        return( LB_ADDSTRING );
    } else {
        return( CB_ADDSTRING );
    }
}

WORD ctl_combo_sel_msg( HWND dlg, int ctl )
/*****************************************/

{
    if( is_listbox( dlg, ctl ) ) {
        return( LB_SETCURSEL );
    } else {
        return( CB_SETCURSEL );
    }
}

WORD ctl_combo_get_msg( HWND dlg, int ctl )
/*****************************************/

{
    if( is_listbox( dlg, ctl ) ) {
        return( LB_GETCURSEL );
    } else {
        return( CB_GETCURSEL );
    }
}

