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
#include <rdos.h>
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#include "dipsys.h"

void DIPSysUnload( dip_sys_handle *sys_hdl )
{
    RdosFreeDll( *sys_hdl );
}

dip_status DIPSysLoad( char *path, dip_client_routines *cli, dip_imp_routines **imp, dip_sys_handle *sys_hdl )
{
    int                 dll;
    dip_init_func       *init_func;
    char                newpath[256];
    dip_status          status;

    strcpy( newpath, path );
    strcat( newpath, ".dll" );
    dll = RdosLoadDll( newpath );
    if( dll == NULL_SYSHDL ) {
        return( DS_ERR|DS_FOPEN_FAILED );
    }
    status = DS_ERR|DS_INVALID_DIP;
    init_func = (dip_init_func *)RdosGetModuleProc( dll, "DIPLOAD" );
    if( init_func != NULL && (*imp = init_func( &status, cli )) != NULL ) {
        *sys_hdl = dll;
        return( DS_OK );
    }
    RdosFreeDll( dll );
    return( status );
}
