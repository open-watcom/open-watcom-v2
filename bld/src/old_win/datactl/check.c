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

BOOL ctl_check_modified( ctl_elt *elt, int parm1, LONG parm2 )
/************************************************************/

{
    if( parm1 == elt->control && ( HIWORD( parm2 ) == BN_CLICKED ||
                                    HIWORD( parm2 ) == BN_DOUBLECLICKED ) ) {
        return( TRUE );
    }

    return( FALSE );
}
