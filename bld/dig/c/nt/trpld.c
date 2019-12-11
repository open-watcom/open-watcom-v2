/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Win32 trap file loading.
*
****************************************************************************/


#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "trptypes.h"
#include "trpld.h"
#include "trpsys.h"
#include "tcerr.h"


#define pick(n,r,p,ar,ap)   typedef r TRAPENTRY (*TRAP_EXTFUNC_TYPE(n)) ## p;
#include "_trpextf.h"
#undef pick

#define pick(n,r,p,ar,ap)   static TRAP_EXTFUNC_TYPE(n) TRAP_EXTFUNC_PTR(n);
#include "_trpextf.h"
#undef pick

static HANDLE           TrapFile = 0;
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

void KillTrap( void )
{
    ReqFunc = NULL;
    TRAP_EXTFUNC_PTR( InfoFunction ) = NULL;
    TRAP_EXTFUNC_PTR( InterruptProgram ) = NULL;
    TRAP_EXTFUNC_PTR( Terminate ) = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( TrapFile != 0 ) {
        FreeLibrary( TrapFile );
        TrapFile = 0;
    }
}

char *LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    char                trpfile[256];
    char                *p;
    bool                have_ext;
    char                chr;
    trap_init_func      *init_func;

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
    have_ext = false;
    p = trpfile;
    for( ; (chr = *parms) != '\0'; parms++ ) {
        if( chr == TRAP_PARM_SEPARATOR ) {
            parms++;
            break;
        }
        switch( chr ) {
        case ':':
        case '/':
        case '\\':
            have_ext = false;
            break;
        case '.':
#ifdef USE_FILENAME_VERSION
            *p++ = ( USE_FILENAME_VERSION / 10 ) + '0';
            *p++ = ( USE_FILENAME_VERSION % 10 ) + '0';
#endif
            have_ext = true;
            break;
        }
        *p++ = chr;
    }
    if( !have_ext ) {
#ifdef USE_FILENAME_VERSION
        *p++ = ( USE_FILENAME_VERSION / 10 ) + '0';
        *p++ = ( USE_FILENAME_VERSION % 10 ) + '0';
#endif
        *p++ = '.';
        *p++ = 'd';
        *p++ = 'l';
        *p++ = 'l';
    }
    *p = '\0';
    TrapFile = LoadLibrary( trpfile );
    if( TrapFile == NULL ) {
        sprintf( buff, "%s '%s'", TC_ERR_CANT_LOAD_TRAP, trpfile );
        return( buff );
    }
    init_func = (trap_init_func *)GetProcAddress( TrapFile, (LPSTR)1 );
    FiniFunc = (trap_fini_func *)GetProcAddress( TrapFile, (LPSTR)2 );
    ReqFunc = (trap_req_func *)GetProcAddress( TrapFile, (LPSTR)3 );
    TRAP_EXTFUNC_PTR( InfoFunction ) = (TRAP_EXTFUNC_TYPE( InfoFunction ))GetProcAddress( TrapFile, (LPSTR)4 );
    TRAP_EXTFUNC_PTR( InterruptProgram ) = (TRAP_EXTFUNC_TYPE( InterruptProgram ))GetProcAddress( TrapFile, (LPSTR)5 );
    TRAP_EXTFUNC_PTR( Terminate ) = (TRAP_EXTFUNC_TYPE( Terminate ))GetProcAddress( TrapFile, (LPSTR)6 );
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    if( init_func != NULL && FiniFunc != NULL && ReqFunc != NULL ) {
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
