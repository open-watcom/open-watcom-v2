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
* Description:  Trap module loader for QNX.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "digcli.h"
#include "trpld.h"
#include "trpimp.h"
#include "exephar.h"
#include "roundmac.h"


#define DEFEXT      ".trp"
//#define MODINIT     "TrapLoad"
#define MODSIGN     TRAPSIGN

#include "../ldrrex.c"       /* PharLap REX format loader */

extern void             *_slib_func[2];

const static trap_callbacks TrapCallbacks = {
    sizeof( trap_callbacks ),

    &environ,
    _slib_func,

    malloc,
    realloc,
    free,
    getenv,
    signal,
};

static module           modhdl = NULL;
static trap_fini_func   *FiniFunc = NULL;

void UnLoadTrap( void )
{
    ReqFunc = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( modhdl != NULL ) {
        loader_unload_image( modhdl );
        modhdl = NULL;
    }
}

digld_error LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    FILE                *fp;
    trap_load_func      *ld_func;
    const trap_requests *trap_funcs;
    char                filename[_MAX_PATH];
    const char          *base_name;
    size_t              len;
    digld_error         err;

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
    base_name = parms;
    len = 0;
    for( ; *parms != '\0'; parms++ ) {
        if( *parms == TRAP_PARM_SEPARATOR ) {
            parms++;
            break;
        }
        len++;
    }
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, len, DEFEXT, filename, sizeof( filename ) ) == 0 ) {
        return( DIGS_ERR_CANT_FIND_MODULE );
    }
    fp = DIGLoader( Open )( filename );
    if( fp == NULL ) {
        return( DIGS_ERR_CANT_LOAD_MODULE );
    }
    err = loader_load_image( fp, filename, &modhdl, (void **)&ld_func );
    DIGLoader( Close )( fp );
    if( err != DIGS_OK ) {
        return( err );
    }
    err = DIGS_ERR_BAD_MODULE_FILE;
    trap_funcs = (( ld_func != NULL ) ? ld_func( &TrapCallbacks ) : NULL);
    if( trap_funcs != NULL ) {
        *trap_ver = trap_funcs->init_func( parms, buff, trap_ver->remote );
        FiniFunc = trap_funcs->fini_func;
        ReqFunc = trap_funcs->req_func;
        err = DIGS_ERR_BUF;
        if( buff[0] == '\0' ) {
            if( TrapVersionOK( *trap_ver ) ) {
                TrapVer = *trap_ver;
                return( DIGS_OK );
            }
            err = DIGS_ERR_WRONG_MODULE_VERSION;
        }
    }
    loader_unload_image( modhdl );
    modhdl = NULL;
    return( err );
}
