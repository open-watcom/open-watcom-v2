#include <windows.h>
#include "ctltype.h"

BOOL ctl_radio_start( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/********************************************************************/
/* start a radio button */

{
    int                 value;

    inst = inst;

    value = _value_int( ptr, elt );

    if( value != 0 ) {
        CheckRadioButton( dlg, elt->control, elt->info.radio.end_control,
                                elt->control + _value_int( ptr, elt ) - 1 );
    }

    return( TRUE );
}

BOOL ctl_radio_finish( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/*********************************************************************/
/* finish a radio button */

{
    int                 control;

    inst = inst;

    for( control = elt->info.radio.end_control; control >= elt->control;
                                                            --control ) {
        if( IsDlgButtonChecked( dlg, control ) ) {
            _value_int( ptr, elt ) = control - elt->control + 1;

            break;
        }
    }

    return( TRUE );
}

BOOL ctl_radio_modified( ctl_elt *elt, UINT wparam, LONG lparam )
/************************************************************/

{
    WORD        id;
    WORD        cmd;

    lparam = lparam;
    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD(wparam, lparam );
    if( id >= elt->control && id <= elt->info.radio.end_control &&
                ( cmd == BN_CLICKED || cmd == BN_DOUBLECLICKED ) ) {
        return( TRUE );
    }

    return( FALSE );
}
