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
* Description:  Trap module loader for Windows 3.x.
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
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

bool    TrapHardModeRequired = false;

static trap_fini_func   *FiniFunc = NULL;
static HINSTANCE        mod_hdl = 0;
static HINSTANCE        toolhelp = 0;

void UnLoadTrap( void )
{
    ReqFunc = NULL;
    TRAP_EXTFUNC_PTR( InputHook ) = NULL;
    TRAP_EXTFUNC_PTR( InfoFunction ) = NULL;
    TRAP_EXTFUNC_PTR( HardModeCheck ) = NULL;
    TRAP_EXTFUNC_PTR( SetHardMode ) = NULL;
    TRAP_EXTFUNC_PTR( UnLockInput ) = NULL;
    TRAP_EXTFUNC_PTR( GetHwndFunc ) = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( mod_hdl != 0 ) {
        FreeLibrary( mod_hdl );
        mod_hdl = 0;
    }
    if( toolhelp != 0 ) {
        FreeLibrary( toolhelp );
        toolhelp = 0;
    }
}

digld_error LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    char                filename[_MAX_PATH];
    const char          *base_name;
    size_t              len;
    UINT                prev;
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
    /*
     * load toolhelp since windows can't seem to handle having a static
     * reference to a dll inside a dynamically loaded dll
     */
    toolhelp = LoadLibrary( "toolhelp.dll" );
    if( (UINT)toolhelp < 32 ) {
        toolhelp = 0;
    }
    if( DIGLoader( Find )( DIG_FILETYPE_EXE, base_name, len, ".dll", filename, sizeof( filename ) ) == 0 ) {
        return( DIGS_ERR_CANT_FIND_MODULE );
    }
    prev = SetErrorMode( SEM_NOOPENFILEERRORBOX );
    mod_hdl = LoadLibrary( filename );
    SetErrorMode( prev );
    if( (UINT)mod_hdl < 32 ) {
        mod_hdl = 0;
        return( DIGS_ERR_CANT_LOAD_MODULE );
    }
    err = DIGS_ERR_BAD_MODULE_FILE;
    init_func = (trap_init_func *)GetProcAddress( mod_hdl, (LPSTR)2 );
    FiniFunc = (trap_fini_func *)GetProcAddress( mod_hdl, (LPSTR)3 );
    ReqFunc  = (trap_req_func *)GetProcAddress( mod_hdl, (LPSTR)4 );
    TRAP_EXTFUNC_PTR( InputHook ) = (TRAP_EXTFUNC_TYPE( InputHook ))GetProcAddress( mod_hdl, (LPSTR)5 );
    TRAP_EXTFUNC_PTR( InfoFunction ) = (TRAP_EXTFUNC_TYPE( InfoFunction ))GetProcAddress( mod_hdl, (LPSTR)6 );
    TRAP_EXTFUNC_PTR( HardModeCheck ) = (TRAP_EXTFUNC_TYPE( HardModeCheck ))GetProcAddress( mod_hdl, (LPSTR)7 );
    TRAP_EXTFUNC_PTR( GetHwndFunc ) = (TRAP_EXTFUNC_TYPE( GetHwndFunc ))GetProcAddress( mod_hdl, (LPSTR)8 );
    TRAP_EXTFUNC_PTR( SetHardMode ) = (TRAP_EXTFUNC_TYPE( SetHardMode ))GetProcAddress( mod_hdl, (LPSTR)12 );
    TRAP_EXTFUNC_PTR( UnLockInput ) = (TRAP_EXTFUNC_TYPE( UnLockInput ))GetProcAddress( mod_hdl, (LPSTR)13 );
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

void TRAP_EXTFUNC( HardModeCheck )( void )
{
    if( TRAP_EXTFUNC_PTR( HardModeCheck ) != NULL ) {
        TrapHardModeRequired = TRAP_EXTFUNC_PTR( HardModeCheck )();
    }
}

HWND TRAP_EXTFUNC( GetHwndFunc )( void )
{
    if( TRAP_EXTFUNC_PTR( GetHwndFunc ) != NULL ) {
        return( TRAP_EXTFUNC_PTR( GetHwndFunc )() );
    }
    return( (HWND)0 );
}

bool TRAP_EXTFUNC( InfoFunction )( HWND hwnd )
{
    if( TRAP_EXTFUNC_PTR( InfoFunction ) != NULL ) {
        TRAP_EXTFUNC_PTR( InfoFunction )( hwnd );
        return( true );
    }
    return( false );
}

void TRAP_EXTFUNC( InputHook )( event_hook_fn *hookfn )
{
    if( TRAP_EXTFUNC_PTR( InputHook ) != NULL ) {
        TRAP_EXTFUNC_PTR( InputHook )( hookfn );
    }
}

void TRAP_EXTFUNC( UnLockInput )( void )
{
    if( TRAP_EXTFUNC_PTR( UnLockInput ) != NULL ) {
        TRAP_EXTFUNC_PTR( UnLockInput )();
    }
}

void  TRAP_EXTFUNC( SetHardMode )( bool mode )
{
    if( TRAP_EXTFUNC_PTR( SetHardMode ) != NULL ) {
        TRAP_EXTFUNC_PTR( SetHardMode )( mode );
    }
}
