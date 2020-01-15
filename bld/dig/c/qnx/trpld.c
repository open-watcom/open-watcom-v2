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
* Description:  Trap module loader for QNX.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "trptypes.h"
#include "trpld.h"
#include "tcerr.h"
#include "digcli.h"
#include "trpimp.h"
#include "digld.h"
#include "ldimp.h"


#define TRAPSIG 0x50415254UL    // "TRAP"

extern void             *_slib_func[2];

const static trap_callbacks TrapCallbacks = {
    sizeof( trap_callbacks ),

    &environ,
    _slib_func,

    malloc,
    realloc,
    free,
    getenv,
    signal,
};

static imp_header       *TrapCode = NULL;
static trap_fini_func   *FiniFunc = NULL;

void KillTrap( void )
{
    ReqFunc = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( TrapCode != NULL ) {
        DIGCli( Free )( TrapCode );
        TrapCode = NULL;
    }
}

char *LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    FILE                *fp;
    trap_load_func      *ld_func;
    const trap_requests *trap_funcs;
    char                filename[256];
    char                *p;
    char                chr;

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
    p = filename;
    for( ; (chr = *parms) != '\0'; parms++ ) {
        if( chr == TRAP_PARM_SEPARATOR ) {
            parms++;
            break;
        }
        *p++ = chr;
    }
#ifdef USE_FILENAME_VERSION
    *p++ = ( USE_FILENAME_VERSION / 10 ) + '0';
    *p++ = ( USE_FILENAME_VERSION % 10 ) + '0';
#endif
    *p = '\0';
    fp = DIGLoader( Open )( filename, p - filename, "trp", NULL, 0 );
    if( fp == NULL ) {
        sprintf( buff, "%s '%s'", TC_ERR_CANT_LOAD_TRAP, filename );
        return( buff );
    }
    TrapCode = ReadInImp( fp );
    DIGLoader( Close )( fp );
    sprintf( buff, "%s '%s'", TC_ERR_CANT_LOAD_TRAP, filename );
    if( TrapCode != NULL ) {
#ifdef __WATCOMC__
        if( TrapCode->sig == TRAPSIG ) {
#endif
            strcpy( buff, TC_ERR_BAD_TRAP_FILE );
            ld_func = (trap_load_func *)TrapCode->init_rtn;
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
                    strcpy( buff, TC_ERR_BAD_TRAP_FILE );
                }
            }
#ifdef __WATCOMC__
        }
#endif
        KillTrap();
    }
    return( buff );
}
