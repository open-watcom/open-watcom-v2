#include <windows.h>
#include "ctltype.h"

BOOL ctl_check_start( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/********************************************************************/
/* start check field */

{
    inst = inst;

    CheckDlgButton( dlg, elt->control, _value_bool( ptr, elt ) );

    return( TRUE );
}

BOOL ctl_check_finish( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/*********************************************************************/
/* end check field */

{
    inst = inst;

    _value_bool( ptr, elt ) = IsDlgButtonChecked( dlg, elt->control );

    return( TRUE );
}

BOOL ctl_check_modified( ctl_elt *elt, UINT wparam, LONG lparam )
/***************************************************************/

{
    WORD        cmd;
    WORD        id;


    lparam = lparam;
    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );
    if( id == elt->control &&
        ( cmd == BN_CLICKED || cmd == BN_DOUBLECLICKED ) ) {
        return( TRUE );
    }

    return( FALSE );
}
