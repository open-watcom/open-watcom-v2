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
* Description:  Load a MAD which is a UNIX shared library.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include "mad.h"
#include "madimp.h"
#include "madcli.h"
#include "madsys.h"

#include "clibext.h"


void MADSysUnload( mad_sys_handle *sys_hdl )
{
    dlclose( *sys_hdl );
}

mad_status MADSysLoad( const char *path, mad_client_routines *cli,
                       mad_imp_routines **imp, mad_sys_handle *sys_hdl )
{
    void                *shlib;
    mad_init_func       *init_func;
    char                newpath[_MAX_PATH];
    char                full_path[_MAX_PATH];
    mad_status          status;

    strcpy( newpath, path );
    strcat( newpath, ".so" );
    shlib = dlopen( newpath, RTLD_NOW );
    if( shlib == NULL ) {
        _searchenv( newpath, "PATH", full_path );
        shlib = dlopen( full_path, RTLD_NOW );
        if( shlib == NULL ) {
            return( MS_ERR|MS_FOPEN_FAILED );
        }
    }
    status = MS_ERR|MS_INVALID_MAD;
    init_func = (mad_init_func *)dlsym( shlib, "MADLOAD" );
    if( init_func != NULL && (*imp = init_func( &status, cli )) != NULL ) {
        *sys_hdl = shlib;
        return( MS_OK );
    }
    dlclose( shlib );
    return( status );
}
