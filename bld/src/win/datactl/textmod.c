#include <windows.h>
#include "ctltype.h"

BOOL ctl_text_modified( ctl_elt *elt, UINT wparam , LONG lparam )
/***********************************************************/

{
    WORD        id;
    WORD        cmd;

    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );

    if( id == elt->control && cmd == EN_CHANGE ) {
        return( TRUE );
    }

    return( FALSE );
}
