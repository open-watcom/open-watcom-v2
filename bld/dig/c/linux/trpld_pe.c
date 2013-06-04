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
#include <stdlib.h>
#include <stddef.h>
#include <signal.h>
#include "trpimp.h"
#include "tcerr.h"
#include "peloader.h"
#include "trpqimp.h"
#include "trpld.h"

#if defined( BUILTIN_TRAP_FILE )
extern const trap_requests *TrapLoad( const trap_callbacks *client );
#endif

#ifndef __WATCOMC__
extern char **environ;
#endif

extern  int      FullPathOpen( char const *name, char *ext, char *result, unsigned max_result );
extern  int      GetSystemHandle(int);

static PE_MODULE        *TrapFile = NULL;
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
    if( TrapFile != NULL ) {
        PE_freeLibrary( TrapFile );
        TrapFile = NULL;
    }
}

int PathOpenTrap( char const *name, unsigned len, char *ext, char *trap_name, int trap_name_len )
{
    char    path[_MAX_PATH];

    len = min(len,sizeof(path));
    memcpy( path, name, len );
    path[ len ] = '\0';
    return( FullPathOpen( path, ext, trap_name, trap_name_len ) );
}

char *LoadTrap( char *trapbuff, char *buff, trap_version *trap_ver )
{
    char                init_error[256];
    int                 filehndl;
    char                *ptr;
    char                *parm;
    const trap_requests *(*ld_func)( const trap_callbacks * );
    char                trap_name[_MAX_PATH];
    const trap_requests *trap_funcs;

    if( trapbuff == NULL ) trapbuff = "std";
    for( ptr = trapbuff; *ptr != '\0' && *ptr != ';'; ++ptr ) ;
    parm = (*ptr != '\0') ? ptr + 1 : ptr;
    filehndl = PathOpenTrap( trapbuff, ptr - trapbuff, "trp", trap_name, sizeof(trap_name) );

    parm = (*ptr != '\0') ? ptr + 1 : ptr;

#if !defined( BUILTIN_TRAP_FILE )
    TrapFile = PE_loadLibraryHandle( GetSystemHandle( filehndl ), trap_name );
    if( TrapFile == NULL ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trapbuff );
        return( buff );
    }
    ld_func = PE_getProcAddress( TrapFile, "TrapLoad_" );
#else
    ld_func = TrapLoad;
#endif
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    if( ld_func == NULL ) {
        KillTrap();
        return( buff );
    }
    trap_funcs = ld_func( &TrapCallbacks );
    if( trap_funcs == NULL ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trapbuff );
        KillTrap();
        return( buff );
    }
    *trap_ver = trap_funcs->init_func( parm, init_error, trap_ver->remote );
    FiniFunc = trap_funcs->fini_func;
    if( init_error[0] != '\0' ) {
        strcpy( buff, init_error );
        KillTrap();
        return( buff );
    }
    if( !TrapVersionOK( *trap_ver ) ) {
        KillTrap();
        return( buff );
    }
    TrapVer = *trap_ver;
    ReqFunc = trap_funcs->req_func;
    return( NULL );
}

