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


#include <windows.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "trpimp.h"
#include "tcerr.h"

static trap_version     TRAPENTRY (TRAPENTRY*InitFunc)();
static void             TRAPENTRY (TRAPENTRY*FiniFunc)();
void                    TRAPENTRY (TRAPENTRY*HookFunc)();
void                    TRAPENTRY (TRAPENTRY*InfoFunction)();
void                    TRAPENTRY (TRAPENTRY*UnLockInput)();
void                    TRAPENTRY (TRAPENTRY*SetHardMode)();
static int              TRAPENTRY (TRAPENTRY*HardModeCheck)();
static int              TRAPENTRY (TRAPENTRY*GetHwndFunc)();

extern trap_version     TrapVer;
extern unsigned         TRAPENTRY (TRAPENTRY*ReqFunc)( unsigned, mx_entry *,
                                        unsigned, mx_entry * );
static HANDLE           dll;

int HardModeRequired;

HANDLE toolhelp;

void KillTrap()
{
    ReqFunc = NULL;
    HookFunc = NULL;
    FiniFunc();
    InitFunc = NULL;
    FiniFunc = NULL;
    InfoFunction = NULL;
    HardModeCheck = NULL;
    SetHardMode = NULL;
    UnLockInput = NULL;
    GetHwndFunc = NULL;
    FreeLibrary( dll );
    if( toolhelp > 32 ) FreeLibrary( toolhelp );
}

char *LoadTrap( char *trapbuff, char *buff, trap_version *trap_ver )
{
    char                trpfile[256];
    char                *ptr;
    char                *parm;
    char                *dst;
    bool                have_ext;
    char                chr;
    UINT                prev;

    if( trapbuff == NULL ) trapbuff = "std";
    have_ext = FALSE;
    ptr = trapbuff;
    dst = (char *)trpfile;
    for( ;; ) {
        chr = *ptr;
        if( chr == '\0' || chr == ';' ) break;
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
        ++ptr;
    }
    if( !have_ext ) {
        *dst++ = '.';
        *dst++ = 'd';
        *dst++ = 'l';
        *dst++ = 'l';
    }
    *dst = '\0';
    parm = (*ptr != '\0') ? ptr + 1 : ptr;
    /*
     * load toolhelp since windows can't seem to handle having a static
     * reference to a dll inside a dynamically loaded dll
     */
    toolhelp = LoadLibrary( "toolhelp.dll" );
    prev = SetErrorMode( SEM_NOOPENFILEERRORBOX );
    dll = LoadLibrary( trpfile );
    SetErrorMode( prev );
    if( dll < 32 ) {
        strcpy( buff, TC_ERR_CANT_LOAD_TRAP );
        return( buff );
    }
    InitFunc = (LPVOID) GetProcAddress( dll, (LPSTR)2 );
    FiniFunc = GetProcAddress( dll, (LPSTR)3 );
    ReqFunc  = (LPVOID) GetProcAddress( dll, (LPSTR)4 );
    HookFunc = GetProcAddress( dll, (LPSTR)5 );
    InfoFunction = GetProcAddress( dll, (LPSTR)6 );
    HardModeCheck = GetProcAddress( dll, (LPSTR)7 );
    SetHardMode = GetProcAddress( dll, (LPSTR)12 );
    UnLockInput = GetProcAddress( dll, (LPSTR)13 );
    GetHwndFunc = GetProcAddress( dll, (LPSTR)8 );
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    if( InitFunc == NULL || FiniFunc == NULL || ReqFunc == NULL ||
        HookFunc == NULL || GetHwndFunc == NULL ||
        SetHardMode == NULL || UnLockInput == NULL ) {
        return( buff );
    }
    *trap_ver = InitFunc( parm, trpfile, trap_ver->remote );
    if( trpfile[0] != '\0' ) {
        strcpy( buff, (char *)trpfile );
        return( buff );
    }
    if( !TrapVersionOK( *trap_ver ) ) {
        KillTrap();
        return( buff );
    }
    TrapVer = *trap_ver;
    return( NULL );
}

void DoHardModeCheck()
{
    HardModeRequired = HardModeCheck();
}
