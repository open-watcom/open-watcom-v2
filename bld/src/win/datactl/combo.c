#include <windows.h>
#include <string.h>
#include "ctltype.h"

extern WORD ctl_combo_add_msg( HWND, int );
extern WORD ctl_combo_sel_msg( HWND, int );
extern WORD ctl_combo_get_msg( HWND, int );

BOOL ctl_combo_start( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/********************************************************************/
/* start a combo list box */

{
    char                value[50];
    WORD                id;
    int                 choose;
    int                 max;

    choose = _value_int( ptr, elt ) - elt->info.combo.origin;

    if( choose < 0 ) {
        choose = 0;
    } else {
        max = elt->info.combo.end_id - elt->info.combo.start_id;
        if( choose > max ) {
            choose = max;
        }
    }

    for( id = elt->info.combo.start_id; id <= elt->info.combo.end_id; ++id ) {

        LoadString( inst, id, value, sizeof( value ) );
        value[49] = '\0';
        SendDlgItemMessage( dlg, elt->control,
                        ctl_combo_add_msg( dlg, elt->control ) , 0,
                                                    (DWORD)(LPSTR) value );
    }
    SendDlgItemMessage( dlg, elt->control, ctl_combo_sel_msg( dlg,
                                            elt->control ), choose, 0 );

    return( TRUE );
}

BOOL ctl_combo_finish( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/*********************************************************************/
/* finish a combo list box */

{
    inst = inst;

    _value_int( ptr, elt ) = elt->info.combo.origin +
            SendDlgItemMessage( dlg, elt->control,
                        ctl_combo_get_msg( dlg, elt->control ), 0, 0 );

    return( TRUE );
}
