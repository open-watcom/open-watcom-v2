#include <stddef.h>
#include "watcom.h"
#include "wres.h"

#include "clibext.h"


WResFileID res_handle = WRES_NIL_HANDLE;

int IsLoadResFile( WResFileID handle )
{
    return( handle == res_handle );
}
