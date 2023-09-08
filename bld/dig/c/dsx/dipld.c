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
* Description:  Module to load DIP libraries in PharLap REX format.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "digld.h"
#include "dip.h"
#include "dipimp.h"
#include "dipsys.h"
#include "dbgmod.h"
#include "ldimp.h"


#define DIPSIG  0x00504944UL    // "DIP"

void DIPSysUnload( dip_sys_handle *sys_hdl )
{
    /*
     * We should unload the symbols here but it's not worth the trouble
     */
    if( *sys_hdl != NULL_SYSHDL ) {
        DIGCli( Free )( *sys_hdl );
        *sys_hdl = NULL_SYSHDL;
    }
}

dip_status DIPSysLoad( const char *base_name, dip_client_routines *cli, dip_imp_routines **imp, dip_sys_handle *sys_hdl )
{
    FILE                *fp;
    imp_header          *mod_hdl;
    dip_init_func       *init_func;
    dip_status          ds;
    char                filename[_MAX_PATH];

    *sys_hdl = NULL_SYSHDL;
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, 0, ".dip", filename, sizeof( filename ) ) == 0 ) {
        return( DS_ERR | DS_FOPEN_FAILED );
    }
    fp = DIGLoader( Open )( filename );
    if( fp == NULL ) {
        return( DS_ERR | DS_FOPEN_FAILED );
    }
    mod_hdl = ReadInImp( fp );
    DIGLoader( Close )( fp );
    ds = DS_ERR | DS_INVALID_DIP;
    if( mod_hdl != NULL ) {
#ifdef __WATCOMC__
        if( mod_hdl->sig == DIPSIG ) {
#endif
#ifdef WATCOM_DEBUG_SYMBOLS
            /*
             * Look for symbols in separate .sym files, not the .dip itself
             */
            strcpy( filename + strlen( filename ) - 4, ".sym" );
            DebuggerLoadUserModule( filename, GetCS(), (unsigned long)mod_hdl );
#endif
            init_func = (dip_init_func *)mod_hdl->init_rtn;
            if( init_func != NULL && (*imp = init_func( &ds, cli )) != NULL ) {
                *sys_hdl = mod_hdl;
                return( DS_OK );
            }
#ifdef WATCOM_DEBUG_SYMBOLS
            DebuggerUnloadUserModule( filename );
#endif
#ifdef __WATCOMC__
        }
#endif
        DIPSysUnload( (dip_sys_handle *)&mod_hdl );
    }
    return( ds );
}
