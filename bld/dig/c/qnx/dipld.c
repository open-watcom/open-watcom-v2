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
* Description:  DIP module loader for QNX.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "digld.h"
#include "dip.h"
#include "dipimp.h"
#include "dipsys.h"
#include "exephar.h"
#include "roundmac.h"


#define DEFEXT      ".dip"
//#define MODINIT     "DIPLOAD"
#define MODSIG      DIPSIGVAL

#include "../ldrrex.c"       /* PharLap REX format loader */

void DIPSysUnload( dip_sys_handle *sys_hdl )
{
    /*
     * We should unload the symbols here but it's not worth the trouble
     */
    if( *sys_hdl != NULL_SYSHDL ) {
        loader_unload_image( *sys_hdl );
        *sys_hdl = NULL_SYSHDL;
    }
}

dip_status DIPSysLoad( const char *base_name, dip_client_routines *cli, dip_imp_routines **imp, dip_sys_handle *sys_hdl )
{
    FILE                *fp;
    module              modhdl;
    dip_init_func       *init_func;
    dip_status          status;
    char                filename[_MAX_PATH];
    digld_error         err;

    *sys_hdl = NULL_SYSHDL;
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, 0, DEFEXT, filename, sizeof( filename ) ) == 0 ) {
        return( DS_ERR | DS_FOPEN_FAILED );
    }
    fp = DIGLoader( Open )( filename );
    if( fp == NULL ) {
        return( DS_ERR | DS_FOPEN_FAILED );
    }
    err = loader_load_image( fp, filename, &modhdl, (void **)&init_func );
    DIGLoader( Close )( fp );
    if( err == DIGS_ERR_CANT_LOAD_MODULE )
        return( DS_ERR | DS_FREAD_FAILED );
    if( err == DIGS_ERR_OUT_OF_DOS_MEMORY
      || err == DIGS_ERR_OUT_OF_DOS_MEMORY )
        return( DS_ERR | DS_NO_MEM );
    status = DS_ERR | DS_INVALID_DIP;
    if( err != DIGS_OK ) {
        return( status );
    }
    if( init_func != NULL && (*imp = init_func( &status, cli )) != NULL ) {
#ifdef WATCOM_DEBUG_SYMBOLS
        /*
         * Look for symbols in separate .sym files, not the .dip itself
         */
        strcpy( filename + strlen( filename ) - 4, ".sym" );
        DebuggerLoadUserModule( filename, GetCS(), (unsigned long)modhdl );
#endif
        *sys_hdl = modhdl;
        return( DS_OK );
    }
    loader_unload_image( modhdl );
    return( status );
}
