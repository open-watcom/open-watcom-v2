#include <windows.h>
#include "ctltype.h"


BOOL ctl_int_start( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/******************************************************************/
/* start an integer field */

{
    inst = inst;

    SetDlgItemInt( dlg, elt->control, _value_int( ptr, elt ), TRUE );

    return( TRUE );
}

BOOL ctl_int_finish( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/*******************************************************************/
/* end an int field */

{
    BOOL                ok;

    inst = inst;

    _value_int( ptr, elt ) = GetDlgItemInt( dlg, elt->control, &ok, TRUE );

    if( !ok ) {
        SetFocus( GetDlgItem( dlg, elt->control ) );
        MessageBox( dlg, "Invalid integer: please re-enter it", NULL,
                                        MB_APPLMODAL | MB_ICONHAND | MB_OK );
        return( FALSE );
    }

    return( TRUE );
}
