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
* Description:  MAD module loader for QNX.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __LINUX__
    #include <sys/mman.h>
#endif
#ifdef __WATCOMC__
    #include "watcom.h"
    #include "exephar.h"
#else
    #include <dlfcn.h>
#endif
#include "digld.h"
#include "mad.h"
#include "madimp.h"
#include "madcli.h"
#include "madsys.h"
#include "roundmac.h"


#ifdef __WATCOMC__

#define DEFEXT      ".mad"
#define MODSIGN     MADSIGN
#include "../ldrrex.c"      /* PharLap REX format loader */

#else

#define DEFEXT      ".so"
#define MODINIT     "MADLOAD"
#include "../ldrso.c"       /* Shared library format loader */

#endif

void MADSysUnload( mad_sys_handle *sys_hdl )
{
    if( *sys_hdl != NULL_SYSHDL ) {
        loader_unload_image( *sys_hdl );
        *sys_hdl = NULL_SYSHDL;
    }
}

mad_status MADSysLoad( const char *base_name, mad_client_routines *cli,
                                mad_imp_routines **imp, mad_sys_handle *sys_hdl )
{
    FILE                *fp;
    module              modhdl;
    mad_init_func       *init_func;
    mad_status          status;
    char                filename[_MAX_PATH];
    digld_error         err;

    *sys_hdl = NULL_SYSHDL;
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, 0, DEFEXT, filename, sizeof( filename ) ) == 0 ) {
        return( MS_ERR | MS_FOPEN_FAILED );
    }
    fp = DIGLoader( Open )( filename );
    if( fp == NULL ) {
        return( MS_ERR | MS_FOPEN_FAILED );
    }
    err = loader_load_image( fp, filename, &modhdl, (void **)&init_func );
    DIGLoader( Close )( fp );
    if( err == DIGS_ERR_CANT_LOAD_MODULE )
        return( MS_ERR | MS_FREAD_FAILED );
    if( err == DIGS_ERR_OUT_OF_MEMORY
      || err == DIGS_ERR_OUT_OF_DOS_MEMORY )
        return( MS_ERR | MS_NO_MEM );
    status = MS_ERR | MS_INVALID_MAD;
    if( err != DIGS_OK ) {
        return( status );
    }
    if( init_func != NULL && (*imp = init_func( &status, cli )) != NULL ) {
        *sys_hdl = modhdl;
        return( MS_OK );
    }
    loader_unload_image( modhdl );
    return( status );
}
