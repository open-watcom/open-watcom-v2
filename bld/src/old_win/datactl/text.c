#include <windows.h>
#include "ctltype.h"

BOOL ctl_text_start( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/*******************************************************************/
/* start a text field */

{
    inst = inst;

    SetDlgItemText( dlg, elt->control, _str_ptr( ptr, elt ) );

    return( TRUE );
}

BOOL ctl_text_finish( ctl_elt *elt, HANDLE inst, HWND dlg, void *ptr )
/********************************************************************/
/* end a text field */

{
    char                *str;

    inst = inst;

    str = _str_ptr( ptr, elt );

    GetDlgItemText( dlg, elt->control, str, elt->info.text.text_size );

    str[elt->info.text.text_size - 1]= '\0'; // in case of overflow

    return( TRUE );
}
