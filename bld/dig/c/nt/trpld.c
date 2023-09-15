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
* Description:  Trap module loader for Win32.
*
****************************************************************************/


#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "trpld.h"
#include "trpsys.h"


#define pick(n,r,p,ar,ap)   typedef r TRAPENTRY (*TRAP_EXTFUNC_TYPE(n)) ## p;
#include "_trpextf.h"
#undef pick

#define pick(n,r,p,ar,ap)   static TRAP_EXTFUNC_TYPE(n) TRAP_EXTFUNC_PTR(n);
#include "_trpextf.h"
#undef pick

static HANDLE           mod_hdl = 0;
static trap_fini_func   *FiniFunc = NULL;

bool TRAP_EXTFUNC( InfoFunction )( HWND hwnd )
{
    if( TRAP_EXTFUNC_PTR( InfoFunction ) != NULL ) {
        TRAP_EXTFUNC_PTR( InfoFunction )( hwnd );
        return( true );
    }
    return( false );
}

bool TRAP_EXTFUNC( InterruptProgram )( void )
{
    if( TRAP_EXTFUNC_PTR( InterruptProgram ) != NULL ) {
        TRAP_EXTFUNC_PTR( InterruptProgram )();
        return( true );
    }
    return( false );
}

bool TRAP_EXTFUNC( Terminate )( void )
{
    if( TRAP_EXTFUNC_PTR( Terminate ) != NULL ) {
        TRAP_EXTFUNC_PTR( Terminate )();
        return( true );
    }
    return( false );
}

void UnLoadTrap( void )
{
    ReqFunc = NULL;
    TRAP_EXTFUNC_PTR( InfoFunction ) = NULL;
    TRAP_EXTFUNC_PTR( InterruptProgram ) = NULL;
    TRAP_EXTFUNC_PTR( Terminate ) = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( mod_hdl != 0 ) {
        FreeLibrary( mod_hdl );
        mod_hdl = 0;
    }
}

digld_error LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    char                filename[256];
    const char          *base_name;
    size_t              len;
    trap_init_func      *init_func;
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
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, len, ".dll", filename, sizeof( filename ) ) == 0 ) {
        return( DIGS_ERR_CANT_FIND_MODULE );
    }
    mod_hdl = LoadLibrary( filename );
    if( mod_hdl == NULL ) {
        return( DIGS_ERR_CANT_LOAD_MODULE );
    }
    err = DIGS_ERR_BAD_MODULE_FILE;
    init_func = (trap_init_func *)GetProcAddress( mod_hdl, (LPSTR)1 );
    FiniFunc = (trap_fini_func *)GetProcAddress( mod_hdl, (LPSTR)2 );
    ReqFunc = (trap_req_func *)GetProcAddress( mod_hdl, (LPSTR)3 );
    TRAP_EXTFUNC_PTR( InfoFunction ) = (TRAP_EXTFUNC_TYPE( InfoFunction ))GetProcAddress( mod_hdl, (LPSTR)4 );
    TRAP_EXTFUNC_PTR( InterruptProgram ) = (TRAP_EXTFUNC_TYPE( InterruptProgram ))GetProcAddress( mod_hdl, (LPSTR)5 );
    TRAP_EXTFUNC_PTR( Terminate ) = (TRAP_EXTFUNC_TYPE( Terminate ))GetProcAddress( mod_hdl, (LPSTR)6 );
    if( init_func != NULL
      && FiniFunc != NULL
      && ReqFunc != NULL ) {
        *trap_ver = init_func( parms, buff, trap_ver->remote );
        err = DIGS_ERR_BUF;
        if( buff[0] == '\0' ) {
            if( TrapVersionOK( *trap_ver ) ) {
                TrapVer = *trap_ver;
                return( DIGS_OK );
            }
            err = DIGS_ERR_WRONG_MODULE_VERSION;
        }
    }
    UnLoadTrap();
    return( err );
}
