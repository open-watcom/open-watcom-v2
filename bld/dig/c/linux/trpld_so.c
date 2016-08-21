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
* Description:  Linux module to load debugger trap files.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <dlfcn.h>
#include "trptypes.h"
#include "trpld.h"
#include "tcerr.h"
#include "digio.h"

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

void KillTrap( void )
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
    dig_lhandle         lfh;
    const char          *ptr;
    trap_load_func      *ld_func;
    char                trap_name[_MAX_PATH];
    const trap_requests *trap_funcs;
#if !defined( BUILTIN_TRAP_FILE ) && defined( USE_FILENAME_VERSION )
    char                *p;
#endif

    if( parms == NULL || *parms == '\0' )
        parms = "std";
#if !defined( BUILTIN_TRAP_FILE ) && defined( USE_FILENAME_VERSION )
    for( ptr = parms, p = trap_name; *ptr != '\0' && *ptr != TRAP_PARM_SEPARATOR; ++ptr ) {
        *p++ = *ptr;
    }
#else
    for( ptr = parms; *ptr != '\0' && *ptr != TRAP_PARM_SEPARATOR; ++ptr )
        ;
#endif
#if !defined( BUILTIN_TRAP_FILE )
  #ifdef USE_FILENAME_VERSION
    *p++ = ( USE_FILENAME_VERSION / 10 ) + '0';
    *p++ = ( USE_FILENAME_VERSION % 10 ) + '0';
    *p = '\0';
    lfh = DIGLoadOpen( buff, p - trap_name, "so", trap_name, sizeof( trap_name ) );
#else
    lfh = DIGLoadOpen( parms, ptr - parms, "so", trap_name, sizeof( trap_name ) );
#endif
    if( lfh == DIG_NIL_LHANDLE ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, parms );
        return( buff );
    }
    TrapFile = dlopen( trap_name, RTLD_NOW );
    DIGLoadClose( lfh );
    if( TrapFile == NULL ) {
        puts( dlerror() );
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trap_name );
        return( buff );
    }
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    ld_func = (trap_load_func *)dlsym( TrapFile, "TrapLoad" );
    if( ld_func != NULL ) {
#else
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    ld_func = TrapLoad;
#endif
        trap_funcs = ld_func( &TrapCallbacks );
        if( trap_funcs != NULL ) {
            parms = ptr;
            if( *parms != '\0' )
                ++parms;
            *trap_ver = trap_funcs->init_func( parms, buff, trap_ver->remote );
            FiniFunc = trap_funcs->fini_func;
            if( buff[0] == '\0' ) {
                if( TrapVersionOK( *trap_ver ) ) {
                    TrapVer = *trap_ver;
                    ReqFunc = trap_funcs->req_func;
                    return( NULL );
                }
                strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
            }
        }
#if !defined( BUILTIN_TRAP_FILE )
    }
#endif
    KillTrap();
    return( buff );
}
