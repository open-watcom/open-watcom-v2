#include <windows.h>
#include "ctltype.h"

BOOL ctl_text_modified( ctl_elt *elt, int parm1, LONG parm2 )
/***********************************************************/

{
    if( parm1 == elt->control && HIWORD( parm2 ) == EN_CHANGE ) {
        return( TRUE );
    }

    return( FALSE );
}
