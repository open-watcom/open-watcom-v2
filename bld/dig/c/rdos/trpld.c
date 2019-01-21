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
* Description:  RDOS trap file loading.
*
****************************************************************************/


#include <stdio.h>
#include <rdos.h>
#include <string.h>
#include <stdlib.h>
#include "trptypes.h"
#include "trpld.h"
#include "tcerr.h"


static int              TrapFile = 0;
static trap_fini_func   *FiniFunc = NULL;

void KillTrap( void )
{
    ReqFunc = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( TrapFile != 0 ) {
        RdosFreeDll( TrapFile );
        TrapFile = 0;
    }
}

char *LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    char                trpfile[256];
    char                *p;
    char                chr;
    bool                have_ext;
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
    TrapFile = RdosLoadDll( trpfile );
    if( TrapFile == NULL ) {
        sprintf( buff, "%s '%s'", TC_ERR_CANT_LOAD_TRAP, trpfile );
        return( buff );
    }
    init_func = RdosGetModuleProc( TrapFile, "TrapInit_" );
    FiniFunc = RdosGetModuleProc( TrapFile, "TrapFini_" );
    ReqFunc  = RdosGetModuleProc( TrapFile, "TrapRequest_" );
//    LibListFunc = RdosGetModuleProc( TrapFile, "TrapLibList_" );
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    if( init_func != NULL && FiniFunc != NULL && ReqFunc != NULL /* && LibListFunc != NULL */ ) {
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
