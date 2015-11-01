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
* Description:  Trap module loader for 32-bit OS/2.
*
****************************************************************************/


#define  INCL_DOSMODULEMGR
#define  INCL_DOSMISC
#include <os2.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trptypes.h"
#include "trpld.h"
#include "trpsys.h"
#include "tcerr.h"


static trap_fini_func   *FiniFunc = NULL;
static HMODULE          TrapFile = 0;

static TRAPENTRY_FUNC_PTR( TellHandles );
static TRAPENTRY_FUNC_PTR( TellHardMode );

bool IsTrapFilePumpingMessageQueue( void )
{
    return( TRAPENTRY_PTR_NAME( TellHandles ) != NULL );
}

bool TrapTellHandles( HAB hab, HWND hwnd )
{
    if( TRAPENTRY_PTR_NAME( TellHandles ) == NULL )
        return( false );
    TRAPENTRY_PTR_NAME( TellHandles )( hab, hwnd );
    return( true );
}

char TrapTellHardMode( char hard )
{
    if( TRAPENTRY_PTR_NAME( TellHardMode ) == NULL )
        return( 0 );

    return( TRAPENTRY_PTR_NAME( TellHardMode )( hard ) );
}

void KillTrap( void )
{
    ReqFunc = NULL;
    TRAPENTRY_PTR_NAME( TellHandles ) = NULL;
    TRAPENTRY_PTR_NAME( TellHardMode ) = NULL;
    if( FiniFunc != NULL ) {
        FiniFunc();
        FiniFunc = NULL;
    }
    if( TrapFile != 0 ) {
        DosFreeModule( TrapFile );
        TrapFile = 0;
    }
}

char *LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    char                trpfile[CCHMAXPATH];
    unsigned            len;
    const char          *ptr;
    APIRET              rc;
    char                trpname[CCHMAXPATH] = "";
    char                trppath[CCHMAXPATH] = "";
    trap_init_func      *init_func;

    if( parms == NULL || *parms == '\0' )
        parms = "std";
    for( ptr = parms; *ptr != '\0' && *ptr != TRAP_PARM_SEPARATOR; ++ptr )
        ;
    len = ptr - parms;
    memcpy( trpfile, parms, len );
    trpfile[len] = '\0';

    /* To prevent conflicts with the 16-bit DIP DLLs, the 32-bit versions have the "D32"
     * extension. We will search for them along the PATH (not in LIBPATH);
     */
    strcpy( trpname, trpfile );
    strcat( trpname, ".D32" );
    _searchenv( trpname, "PATH", trppath );
    if( trppath[0] == '\0' ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trpname );
        return( buff );
    }
    rc = DosLoadModule( NULL, 0, trppath, &TrapFile );
    if( rc != 0 ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trppath );
        return( buff );
    }
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    if( DosQueryProcAddr( TrapFile, 1, NULL, (PFN*)&init_func ) == 0
      && DosQueryProcAddr( TrapFile, 2, NULL, (PFN*)&FiniFunc ) == 0
      && DosQueryProcAddr( TrapFile, 3, NULL, (PFN*)&ReqFunc ) == 0 ) {
        if( DosQueryProcAddr( TrapFile, 4, NULL, (PFN*)&TRAPENTRY_PTR_NAME( TellHandles ) ) != 0 ) {
            TRAPENTRY_PTR_NAME( TellHandles ) = NULL;
        }
        if( DosQueryProcAddr( TrapFile, 5, NULL, (PFN*)&TRAPENTRY_PTR_NAME( TellHardMode ) ) != 0 ) {
            TRAPENTRY_PTR_NAME( TellHardMode ) = NULL;
        }
        parms = ptr;
        if( *parms != '\0' )
            ++parms;
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
