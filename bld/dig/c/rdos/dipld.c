/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#include <string.h>
#include <rdos.h>
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#include "dipsys.h"


void DIPSysUnload( dip_sys_handle *sys_hdl )
{
    if( *sys_hdl != NULL_SYSHDL ) {
        RdosFreeDll( *sys_hdl );
        *sys_hdl = NULL_SYSHDL;
    }
}

dip_status DIPSysLoad( const char *path, dip_client_routines *cli, dip_imp_routines **imp, dip_sys_handle *sys_hdl )
{
    dip_sys_handle      dip_dll;
    dip_init_func       *init_func;
    char                newpath[256];
    dip_status          ds;

    *sys_hdl = NULL_SYSHDL;
    strcpy( newpath, path );
    strcat( newpath, ".dll" );
    dip_dll = RdosLoadDll( newpath );
    if( dip_dll == NULL_SYSHDL ) {
        return( DS_ERR | DS_FOPEN_FAILED );
    }
    ds = DS_ERR | DS_INVALID_DIP;
    init_func = (dip_init_func *)RdosGetModuleProc( dip_dll, "DIPLOAD" );
    if( init_func != NULL && (*imp = init_func( &ds, cli )) != NULL ) {
        *sys_hdl = dip_dll;
        return( DS_OK );
    }
    RdosFreeDll( dip_dll );
    return( ds );
}
