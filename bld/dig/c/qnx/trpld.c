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


#include <string.h>
#include <stdio.h>      // only for sprintf()
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "trpimp.h"
#include "tcerr.h"
#include "dipcli.h"
#include "trpqimp.h"
#include "trpld.h"

#include "../dsx/ldimp.h"

#define TRAPSIG 0x50415254UL

extern void             *_slib_func[2];

extern int              PathOpen(char *,unsigned, char *);

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
        DIGCliFree( TrapCode );
        TrapCode = NULL;
    }
}

char *LoadTrap( char *trapbuff, char *buff, trap_version *trap_ver )
{
    char                init_error[256];
    int                 filehndl;
    char                *ptr;
    char                *parm;
    const trap_requests *(*ld_func)( const trap_callbacks * );
    const trap_requests *trap_funcs;

    if( trapbuff == NULL ) trapbuff = "std";
    for( ptr = trapbuff; *ptr != '\0' && *ptr != ';'; ++ptr ) ;
    parm = (*ptr != '\0') ? ptr + 1 : ptr;
    filehndl = PathOpen( trapbuff, ptr - trapbuff, "trp" );
    if( filehndl <= 0 ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trapbuff );
        return( buff );
    }
    TrapCode = ReadInImp( filehndl );
    DIGCliClose( filehndl );
    if( TrapCode == NULL || TrapCode->sig != TRAPSIG ) {
        strcpy( buff, TC_ERR_BAD_TRAP_FILE );
        KillTrap();
        return( buff );
    }
    ld_func = (void *)TrapCode->init_rtn;
    trap_funcs = ld_func( &TrapCallbacks );
    if( trap_funcs == NULL ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trapbuff );
        KillTrap();
        return( buff );
    }
    *trap_ver = trap_funcs->init_func( parm, init_error, trap_ver->remote );
    if( init_error[0] != '\0' ) {
        strcpy( buff, init_error );
        KillTrap();
        return( buff );
    }
    FiniFunc = trap_funcs->fini_func;
    if( !TrapVersionOK( *trap_ver ) ) {
        KillTrap();
        return( buff );
    }
    TrapVer = *trap_ver;
    ReqFunc = trap_funcs->req_func;
    return( NULL );
}
