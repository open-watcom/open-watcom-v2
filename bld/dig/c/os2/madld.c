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
* Description:  MAD module loader for OS/2.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC
#include <wos2.h>
#include "mad.h"
#include "madimp.h"
#include "madsys.h"


#ifdef _M_I86
#define GET_PROC_ADDRESS(m,s,f) (DosGetProcAddr( m, s, (PFN FAR *)&f ) == 0)
#define LOAD_MODULE(n,m)        (DosLoadModule( NULL, 0, (char *)n, &m ) != 0)
#else
#define GET_PROC_ADDRESS(m,s,f) (DosQueryProcAddr( m, 0, s, (PFN FAR *)&f ) == 0)
#define LOAD_MODULE(n,m)        (DosLoadModule( NULL, 0, n, &m ) != 0)
#endif

void MADSysUnload( mad_sys_handle *sys_hdl )
{
    if( *sys_hdl != NULL_SYSHDL ) {
        DosFreeModule( *sys_hdl );
        *sys_hdl = NULL_SYSHDL;
    }
}

mad_status MADSysLoad( const char *path, mad_client_routines *cli, mad_imp_routines **imp, mad_sys_handle *sys_hdl )
{
    HMODULE             dip_mod;
    mad_init_func       *init_func;
    mad_status          status;
#ifndef _M_I86
    char                madname[CCHMAXPATH];
    char                madpath[CCHMAXPATH];

    *sys_hdl = NULL_SYSHDL;
    /* To prevent conflicts with the 16-bit MAD DLLs, the 32-bit versions have the "D32"
     * extension. We will search for them along the PATH (not in LIBPATH);
     */
    strcpy( madname, path );
    strcat( madname, ".D32" );
    _searchenv( madname, "PATH", madpath );
    if( *madpath == '\0' )
        return( MS_ERR | MS_FOPEN_FAILED );
    path = madpath;
#endif
    if( LOAD_MODULE( path, dip_mod ) ) {
        return( MS_ERR | MS_FOPEN_FAILED );
    }
    status = MS_ERR | MS_INVALID_MAD;
    if( GET_PROC_ADDRESS( dip_mod, "MADLOAD", init_func ) && (*imp = init_func( &status, cli )) != NULL ) {
        *sys_hdl = dip_mod;
        return( MS_OK );
    }
    DosFreeModule( dip_mod );
    return( status );
}
