#include <windows.h>
#include <stdio.h>
#include "ctltype.h"

extern BOOL ctl_float_finish( ctl_elt *, HANDLE, HWND, void * );

BOOL ctl_rfloat_finish( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/**********************************************************************/
/* end an float field */

{
    float               value;
    char                str[100];
    BOOL                any_max;

    if( !ctl_float_finish( elt, inst, dlg, ptr ) ) {
        return( FALSE );
    }

    value = _value_float( ptr, elt );

    any_max = elt->info.rfloat.max >= elt->info.rfloat.min;
    if( value < elt->info.rfloat.min || ( any_max &&
                                        value > elt->info.rfloat.max ) ) {
        SetFocus( GetDlgItem( dlg, elt->control ) );
        if( any_max ) {
            sprintf( str, "Value must be in the range %.2f to %.2f",
                            elt->info.rfloat.min, elt->info.rfloat.max );
        } else {
            sprintf( str, "Value must be greater than or equal to %.2f",
                                                    elt->info.rfloat.min );
        }

        MessageBox( dlg, str, NULL, MB_APPLMODAL | MB_ICONHAND | MB_OK );

        return( FALSE );
    }

    return( TRUE );
}
