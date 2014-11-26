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
#include "trpimp.h"
#include "tcerr.h"
#include "trpld.h"

void KillTrap( void )
{
    TrapFini();
}

char *LoadTrap( const char *trap_parms, char *buff, trap_version *trap_ver )
{
    const char  *ptr;

    if( trap_parms == NULL || *trap_parms == '\0' )
        trap_parms = "std";
    for( ptr = trap_parms; *ptr != '\0' && *ptr != TRAP_PARM_SEPARATOR; ++ptr )
        ;
    ptr = (*ptr != '\0') ? ptr + 1 : ptr;
    *trap_ver = TrapInit( ptr, buff, trap_ver->remote );
    if( buff[0] == '\0' ) {
        if( TrapVersionOK( *trap_ver ) ) {
            TrapVer = *trap_ver;
            ReqFunc = TrapRequest;
            return( NULL );
        }
        strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    }
    KillTrap();
    return( buff );
}
