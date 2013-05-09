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
* Description:  DIP module loader for 16-bit OS/2.
*
****************************************************************************/


#include <string.h>
#define INCL_DOSMODULEMGR
#include <os2.h>
#include "dip.h"
#include "dipimp.h"
#include "dipsys.h"

dip_status DIPSysLoad( char *path, dip_client_routines *cli, dip_imp_routines **imp, dip_sys_handle *sys_hdl )
{
    HMODULE             dll;
    dip_imp_routines    *(*init_func)( dip_status *, dip_client_routines * );
    dip_status          status;

    if( DosLoadModule( NULL, 0, path, &dll ) != 0 ) {
        return( DS_ERR|DS_FOPEN_FAILED );
    }
    if( DosGetProcAddr( dll, "DIPLOAD", (PFN FAR *)&init_func ) != 0 ) {
        DosFreeModule( dll );
        return( DS_ERR|DS_INVALID_DIP );
    }
    *imp = init_func( &status, cli );
    if( *imp == NULL ) {
        DosFreeModule( dll );
        return( status );
    }
    *sys_hdl = (dip_sys_handle)dll;
    return( DS_OK );
}

void DIPSysUnload( dip_sys_handle sys_hdl )
{
    DosFreeModule( (HMODULE)sys_hdl );
}
