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
* Description:  Linux module to load debugger trap files.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <dlfcn.h>
#include "digld.h"
#include "trpld.h"
#include "tcerr.h"

#include "clibext.h"


#ifndef __WATCOMC__
extern char             **environ;
#endif

#if !defined( BUILTIN_TRAP_FILE )
static void             *TrapFile = NULL;
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

void UnLoadTrap( void )
{
    ReqFunc = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
#if !defined( BUILTIN_TRAP_FILE )
    if( TrapFile != NULL ) {
        dlclose( TrapFile );
        TrapFile = NULL;
    }
#endif
}

char *LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    FILE                *fp;
    trap_load_func      *ld_func;
    const trap_requests *trap_funcs;
#if !defined( BUILTIN_TRAP_FILE )
    char                filename[_MAX_PATH];
    const char          *base_name;
    size_t              len;
#endif

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
#if !defined( BUILTIN_TRAP_FILE )
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
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, len, "so", filename, sizeof( filename ) ) == 0 ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, base_name );
        return( buff );
    }
    sprintf( buff, TC_ERR_CANT_LOAD_TRAP, filename );
    fp = DIGLoader( Open )( filename );
    if( fp == NULL ) {
        return( buff );
    }
    TrapFile = dlopen( filename, RTLD_NOW );
    DIGLoader( Close )( fp );
    if( TrapFile == NULL ) {
        puts( dlerror() );
        return( buff );
    }
    buff[0] = '\0';
    ld_func = (trap_load_func *)dlsym( TrapFile, "TrapLoad" );
    if( ld_func != NULL ) {
#else
    buff[0] = '\0';
    ld_func = TrapLoad;
#endif
        trap_funcs = ld_func( &TrapCallbacks );
        if( trap_funcs != NULL ) {
            *trap_ver = trap_funcs->init_func( parms, buff, trap_ver->remote );
            FiniFunc = trap_funcs->fini_func;
            ReqFunc = trap_funcs->req_func;
            if( buff[0] == '\0' ) {
                if( TrapVersionOK( *trap_ver ) ) {
                    TrapVer = *trap_ver;
                    return( NULL );
                }
            }
        }
#if !defined( BUILTIN_TRAP_FILE )
    }
#endif
    if( buff[0] == '\0' )
        strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    UnLoadTrap();
    return( buff );
}
