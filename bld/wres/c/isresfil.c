#include <stddef.h>
#include "wio.h"
#include "watcom.h"
#include "wres.h"

#include "clibext.h"


WResFileID res_fid = WRES_NIL_HANDLE;

int IsLoadResFile( WResFileID fid )
{
    return( fid == res_fid );
}
