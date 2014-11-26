/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <string.h>
#define INCL_DOSMODULEMGR
#include <os2.h>
#include "mad.h"
#include "madimp.h"
#include "madsys.h"

void MADSysUnload( mad_sys_handle *sys_hdl )
{
    DosFreeModule( *sys_hdl );
}

mad_status MADSysLoad( const char *path, mad_client_routines *cli,
                                mad_imp_routines **imp, mad_sys_handle *sys_hdl )
{
    HMODULE             dll;
    mad_init_func       *init_func;
    mad_status          status;

    if( DosLoadModule( NULL, 0, path, &dll ) != 0 ) {
        return( MS_ERR|MS_FOPEN_FAILED );
    }
    status = MS_ERR|MS_INVALID_MAD;
    if( DosGetProcAddr( dll, "MADLOAD", (PFN FAR *)&init_func ) == 0
      && (*imp = init_func( &status, cli )) != NULL ) {
        *sys_hdl = dll;
        return( MS_OK );
    }
    DosFreeModule( dll );
    return( status );
}
