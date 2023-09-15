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
* Description:  Trap module loader for Linux.
*
****************************************************************************/


//#define REX

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#ifdef __LINUX__
    #include <sys/mman.h>
#endif
#ifdef __WATCOMC__
#ifdef REX
    #include "watcom.h"
    #include "exephar.h"
#else
    #include "watcom.h"
    #include "exepe.h"
    #include "exedos.h"
#endif
#else
    #include <dlfcn.h>
#endif
#include "digcli.h"
#include "trpld.h"
#include "roundmac.h"

#include "clibext.h"


#ifndef __WATCOMC__
extern char             **environ;
#endif

static trap_fini_func   *FiniFunc = NULL;

const static trap_callbacks TrapCallbacks = {
    sizeof( trap_callbacks ),

    &environ,
    NULL,

    malloc,
    realloc,
    free,
    getenv,
    signal,
};

#if !defined( BUILTIN_TRAP_FILE )

#ifdef __WATCOMC__

#ifdef REX

#define DEFEXT      ".trp"
#define MODSIGN     TRAPSIGN
#include "../ldrrex.c"      /* PharLap REX format loader */

#else

#define DEFEXT      ".trp"
#define MODINIT     "TrapLoad_"
#include "../ldrpe.c"       /* PE DLL format loader */

#endif

#else

#define DEFEXT      ".so"
#define MODINIT     "TrapLoad"
#include "../ldrso.c"       /* Shared library format loader */

#endif

static module       modhdl = NULL;

#endif

void UnLoadTrap( void )
{
    ReqFunc = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
#if !defined( BUILTIN_TRAP_FILE )
    if( modhdl != NULL ) {
        loader_unload_image( modhdl );
        modhdl = NULL;
    }
#endif
}

digld_error LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    FILE                *fp;
    trap_load_func      *ld_func;
    const trap_requests *trap_funcs;
    digld_error         err;
#if !defined( BUILTIN_TRAP_FILE )
    char                filename[_MAX_PATH];
    const char          *base_name;
    size_t              len;
#endif

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
#if !defined( BUILTIN_TRAP_FILE )
    modhdl = NULL;
    base_name = parms;
    len = 0;
#endif
    for( ; *parms != '\0'; parms++ ) {
        if( *parms == TRAP_PARM_SEPARATOR ) {
            parms++;
            break;
        }
#if !defined( BUILTIN_TRAP_FILE )
        len++;
#endif
    }
#if !defined( BUILTIN_TRAP_FILE )
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
    trap_funcs = ((ld_func != NULL) ? ld_func( &TrapCallbacks ) : NULL);
#else
    trap_funcs = TrapLoad( &TrapCallbacks );
#endif
    err = DIGS_ERR_BAD_MODULE_FILE;
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
#if !defined( BUILTIN_TRAP_FILE )
    loader_unload_image( modhdl );
    modhdl = NULL;
#endif
    return( err );
}
