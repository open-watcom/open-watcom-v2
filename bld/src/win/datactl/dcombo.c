#include <windows.h>
#include <string.h>
#include "ctltype.h"

extern WORD ctl_combo_add_msg( HWND, int );
extern WORD ctl_combo_sel_msg( HWND, int );
extern WORD ctl_combo_get_msg( HWND, int );

BOOL ctl_dcombo_start( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/*********************************************************************/
/* start a dynamic combo list box */

{
    char                *str;
    int                 i;
    int                 value;

    inst = inst;

    value = _value_int( ptr, elt ) - elt->info.dcombo.origin;

    for( i = 0;; ++i ) {
        str = (elt->info.dcombo.fetch)( i );
        if( str == NULL ) {
            break;
        }

        SendDlgItemMessage( dlg, elt->control,
                        ctl_combo_add_msg( dlg, elt->control ) , 0,
                                                    (DWORD)(LPSTR) str );
    }

    if( value >= i ) {
        value = i - 1;
    }

    SendDlgItemMessage( dlg, elt->control, ctl_combo_sel_msg( dlg,
                                            elt->control ), value, 0 );

    return( TRUE );
}

BOOL ctl_dcombo_finish( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/**********************************************************************/
/* finish a dynamic combo list box */

{
    inst = inst;

    _value_int( ptr, elt ) = elt->info.dcombo.origin +
            SendDlgItemMessage( dlg, elt->control,
                        ctl_combo_get_msg( dlg, elt->control ), 0, 0 );

    return( TRUE );
}
