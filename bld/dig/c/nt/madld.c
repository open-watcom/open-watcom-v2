/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MAD module loader for Win32.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "digld.h"
#include "mad.h"
#include "madimp.h"
#include "madcli.h"
#include "madsys.h"


void MADSysUnload( mad_sys_handle *sys_hdl )
{
    if( *sys_hdl != NULL_SYSHDL ) {
        FreeLibrary( *sys_hdl );
        *sys_hdl = NULL_SYSHDL;
    }
}

mad_status MADSysLoad( const char *base_name, mad_client_routines *cli, mad_imp_routines **imp, mad_sys_handle *sys_hdl )
{
    mad_sys_handle      mod_hdl;
    mad_init_func       *init_func;
    char                filename[_MAX_PATH];
    mad_status          status;

    *sys_hdl = NULL_SYSHDL;
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, 0, ".dll", filename, sizeof( filename ) ) == 0 ) {
        return( MS_ERR | MS_FOPEN_FAILED );
    }
    mod_hdl = LoadLibrary( filename );
    if( mod_hdl == NULL ) {
        return( MS_ERR | MS_FOPEN_FAILED );
    }
    status = MS_ERR | MS_INVALID_MAD;
    init_func = (mad_init_func *)GetProcAddress( mod_hdl, "MADLOAD" );
    if( init_func != NULL && (*imp = init_func( &status, cli )) != NULL ) {
        *sys_hdl = mod_hdl;
        return( MS_OK );
    }
    MADSysUnload( &mod_hdl );
    return( status );
}
