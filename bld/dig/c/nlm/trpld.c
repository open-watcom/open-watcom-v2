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
#include "trptypes.h"
#include "trpld.h"
#include "trpcomm.h"
#include "tcerr.h"


void KillTrap( void )
{
    TrapFini();
}

char *LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    const char  *ptr;

    if( parms == NULL || *parms == '\0' )
        parms = "std";
    for( ptr = parms; *ptr != '\0' && *ptr != TRAP_PARM_SEPARATOR; ++ptr )
        ;
    parms =ptr;
    if( *parms != '\0' )
        ++parms;
    *trap_ver = TrapInit( parms, buff, trap_ver->remote );
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
