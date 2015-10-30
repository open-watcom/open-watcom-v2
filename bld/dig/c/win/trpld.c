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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <windows.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "trptypes.h"
#include "tcerr.h"
#include "trpld.h"
#include "trpsys.h"

int TrapHardModeRequired;

static void             (TRAPENTRY*HookFunc)(LPVOID);
static void             (TRAPENTRY*InfoFunction)(HWND);
static void             (TRAPENTRY*UnLockInput)(void);
static void             (TRAPENTRY*SetHardMode)(char);
static int              (TRAPENTRY*HardModeCheck)(void);
static int              (TRAPENTRY*GetHwndFunc)(void);

static trap_fini_func   *FiniFunc = NULL;
static HINSTANCE        TrapFile = 0;
static HINSTANCE        toolhelp = 0;

void KillTrap( void )
{
    ReqFunc = NULL;
    HookFunc = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    InfoFunction = NULL;
    HardModeCheck = NULL;
    SetHardMode = NULL;
    UnLockInput = NULL;
    GetHwndFunc = NULL;
    if( TrapFile != 0 ) {
        FreeLibrary( TrapFile );
        TrapFile = 0;
    }
    if( toolhelp != 0 ) {
        FreeLibrary( toolhelp );
        toolhelp = 0;
    }
}

char *LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    char                trpfile[256];
    const char          *ptr;
    char                *dst;
    bool                have_ext;
    char                chr;
    UINT                prev;
    trap_init_func      *init_func;

    if( parms == NULL || *parms == '\0' )
        parms = "std";
    have_ext = FALSE;
    dst = trpfile;
    for( ptr = parms; *ptr != '\0' && *ptr != TRAP_PARM_SEPARATOR; ++ptr ) {
        chr = *ptr;
        switch( chr ) {
        case ':':
        case '/':
        case '\\':
            have_ext = FALSE;
            break;
        case '.':
            have_ext = TRUE;
            break;
        }
        *dst++ = chr;
    }
    if( !have_ext ) {
        *dst++ = '.';
        *dst++ = 'd';
        *dst++ = 'l';
        *dst++ = 'l';
    }
    *dst = '\0';
    /*
     * load toolhelp since windows can't seem to handle having a static
     * reference to a dll inside a dynamically loaded dll
     */
    toolhelp = LoadLibrary( "toolhelp.dll" );
    if( (UINT)toolhelp < 32 ) {
        toolhelp = 0;
    }
    prev = SetErrorMode( SEM_NOOPENFILEERRORBOX );
    TrapFile = LoadLibrary( trpfile );
    SetErrorMode( prev );
    if( (UINT)TrapFile < 32 ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trpfile );
        TrapFile = 0;
        return( buff );
    }
    init_func = (trap_init_func *)GetProcAddress( TrapFile, (LPSTR)2 );
    FiniFunc = (trap_fini_func *)GetProcAddress( TrapFile, (LPSTR)3 );
    ReqFunc  = (trap_req_func *)GetProcAddress( TrapFile, (LPSTR)4 );
    HookFunc = (void(TRAPENTRY*)(LPVOID)) GetProcAddress( TrapFile, (LPSTR)5 );
    InfoFunction = (void(TRAPENTRY*)(HWND)) GetProcAddress( TrapFile, (LPSTR)6 );
    HardModeCheck = (int(TRAPENTRY*)(void)) GetProcAddress( TrapFile, (LPSTR)7 );
    SetHardMode = (void(TRAPENTRY*)(char)) GetProcAddress( TrapFile, (LPSTR)12 );
    UnLockInput = (void(TRAPENTRY*)(void)) GetProcAddress( TrapFile, (LPSTR)13 );
    GetHwndFunc = (int(TRAPENTRY*)(void)) GetProcAddress( TrapFile, (LPSTR)8 );
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    if( init_func != NULL && FiniFunc != NULL && ReqFunc != NULL
      && HookFunc != NULL && InfoFunction != NULL && HardModeCheck != NULL
      && GetHwndFunc != NULL && SetHardMode != NULL && UnLockInput != NULL ) {
        parms = ptr;
        if( *parms != '\0' )
            ++parms;
        *trap_ver = init_func( parms, buff, trap_ver->remote );
        if( buff[0] == '\0' ) {
            if( TrapVersionOK( *trap_ver ) ) {
                TrapVer = *trap_ver;
                return( NULL );
            }
            strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
        }
    }
    KillTrap();
    return( buff );
}

void TrapHardModeCheck( void )
{
    TrapHardModeRequired = HardModeCheck();
}

void TrapTellHWND( HWND hwnd )
{
    InfoFunction( hwnd );
}

void TrapInputHook( hook_fn *hookfn )
{
    HookFunc( hookfn );
}

void TrapUnLockInput( void )
{
    UnLockInput();
}

void  TrapSetHardMode( char state )
{
    SetHardMode( state );
}
