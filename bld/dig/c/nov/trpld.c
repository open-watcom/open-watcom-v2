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
* Description:  Trap module loader for Novell Netware.
*
****************************************************************************/


#include <string.h>
#include "trpld.h"
#include "trpcomm.h"


void UnLoadTrap( void )
{
    TrapFini();
}

digld_error LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    const char  *trpname;
    size_t      len;
    digld_error err;

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
    trpname = parms;
    len = 0;
    for( ; *parms != '\0'; parms++ ) {
        if( *parms == TRAP_PARM_SEPARATOR ) {
            parms++;
            break;
        }
        len++;
    }
    *trap_ver = TrapInit( parms, buff, trap_ver->remote );
    err = DIGS_ERR_BUF;
    if( buff[0] == '\0' ) {
        if( TrapVersionOK( *trap_ver ) ) {
            TrapVer = *trap_ver;
            ReqFunc = TrapRequest;
            return( DIGS_OK );
        }
        err = DIGS_ERR_WRONG_MODULE_VERSION;
    }
    UnLoadTrap();
    return( err );
}
