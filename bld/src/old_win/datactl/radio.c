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

BOOL ctl_radio_modified( ctl_elt *elt, int parm1, LONG parm2 )
/************************************************************/

{
    if( parm1 >= elt->control && parm1 <= elt->info.radio.end_control &&
                                    ( HIWORD( parm2 ) == BN_CLICKED ||
                                    HIWORD( parm2 ) == BN_DOUBLECLICKED ) ) {
        return( TRUE );
    }

    return( FALSE );
}
