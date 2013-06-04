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
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "trpimp.h"
#include "tcerr.h"
#include "trpld.h"

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

char *LoadTrap( char *trapbuff, char *buff, trap_version *trap_ver )
{
    char                trpfile[256];
    char                *ptr;
    char                *parm;
    char                *dst;
    char                have_ext;
    char                chr;
    trap_init_func      *init_func;

    if( trapbuff == NULL )
        trapbuff = "std";
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
            have_ext = 0;
            break;
        case '.':
            have_ext = 1;
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
    TrapFile = RdosLoadDll( trpfile );
    if( TrapFile == NULL ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trpfile );
        return( buff );
    }
    init_func = RdosGetModuleProc( TrapFile, "TrapInit_" );
    FiniFunc = RdosGetModuleProc( TrapFile, "TrapFini_" );
    ReqFunc  = RdosGetModuleProc( TrapFile, "TrapRequest_" );
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    if( init_func == NULL || FiniFunc == NULL || ReqFunc == NULL
        /* || LibListFunc == NULL */ ) {
        KillTrap();
        return( buff );
    }
    *trap_ver = init_func( parm, trpfile, trap_ver->remote );
    if( trpfile[0] != '\0' ) {
        strcpy( buff, (char *)trpfile );
        KillTrap();
        return( buff );
    }
    if( !TrapVersionOK( *trap_ver ) ) {
        KillTrap();
        return( buff );
    }
    TrapVer = *trap_ver;
    return( NULL );
}
