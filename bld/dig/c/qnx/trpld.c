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


#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <signal.h>
#include "trpimp.h"
#include "tcerr.h"
#include "dipcli.h"
#include "trpqimp.h"

#include "../dsx/ldimp.h"

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

static imp_header               *TrapCode;
static const trap_requests      *TrapFuncs;

extern trap_version     TrapVer;
extern unsigned         (TRAPENTRY *ReqFunc)( unsigned, mx_entry *,
                                        unsigned, mx_entry * );


extern  int      PathOpen(char *,unsigned, char *);

#define TRAPSIG 0x50415254UL

char *LoadTrap( char *trapbuff, char *buff, trap_version *trap_ver )
{
    char                init_error[256];
    int                 filehndl;
    char                *ptr;
    char                *parm;
    const trap_requests *(*ld_func)( const trap_callbacks * );

    if( trapbuff == NULL ) trapbuff = "std";
    for( ptr = trapbuff; *ptr != '\0' && *ptr != ';'; ++ptr ) ;
    parm = (*ptr != '\0') ? ptr + 1 : ptr;
    filehndl = PathOpen( trapbuff, ptr - trapbuff, "trp" );
    if( filehndl <= 0 ) {
        strcpy( buff, TC_ERR_CANT_LOAD_TRAP );
        return( buff );
    }
    TrapCode = ReadInImp( filehndl );
    DIGCliClose( filehndl );
    if( TrapCode == NULL || TrapCode->sig != TRAPSIG ) {
        strcpy( buff, TC_ERR_BAD_TRAP_FILE );
        return( buff );
    }
    ld_func = (void *)TrapCode->init_rtn;
    TrapFuncs = ld_func( &TrapCallbacks );
    if( TrapFuncs == NULL ) {
        strcpy( buff, TC_ERR_CANT_LOAD_TRAP );
        return( buff );
    }
    *trap_ver = TrapFuncs->init_func( parm, init_error, trap_ver->remote );
    if( init_error[0] != '\0' ) {
        strcpy( buff, init_error );
        return( buff );
    }
    if( !TrapVersionOK( *trap_ver ) ) {
        KillTrap();
        return( buff );
    }
    TrapVer = *trap_ver;
    ReqFunc = TrapFuncs->req_func;
    return( NULL );
}


void KillTrap()
{
    TrapFuncs->fini_func();
    ReqFunc = NULL;
    DIGCliFree( TrapCode );
}
