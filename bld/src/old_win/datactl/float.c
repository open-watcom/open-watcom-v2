#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "ctltype.h"


BOOL ctl_float_start( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/********************************************************************/
/* start a float field */

{
    char                buf[50];
    char                *str;
    int                 dec;
    float               value;

    inst = inst;

    value = _value_float( ptr, elt );
    sprintf( buf, "%f", value );
    for( str = buf + strlen( buf ) - 1; *str == '0'; --str );
    for( dec = 0; *str != '.'; --str, ++dec );
    if( dec < 2 ) {
        dec = 2;
    }

    sprintf( buf, "%-.*f", dec, value );

    SetDlgItemText( dlg, elt->control, buf );

    return( TRUE );
}

BOOL ctl_float_finish( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/*********************************************************************/
/* end a float field */

{
    char                str[50];
    float               value;

    inst = inst;

    GetDlgItemText( dlg, elt->control, str, 50 );
    str[49] = '\0';

    if( 1 == sscanf( str, "%f", &value ) ) {
        _value_float( ptr, elt ) = value;
    } else {
        SetFocus( GetDlgItem( dlg, elt->control ) );
        MessageBox( dlg, "Invalid value: please re-enter it", NULL,
                                        MB_APPLMODAL | MB_ICONHAND | MB_OK );
        return( FALSE );
    }

    return( TRUE );
}
