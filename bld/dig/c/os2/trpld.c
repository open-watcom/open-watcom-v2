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
#define INCL_DOSMODULEMGR
#define INCL_DOSMISC
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include "trptypes.h"
#include "trpld.h"
#include "trpsys.h"
#include "tcerr.h"


static HMODULE          TrapFile = 0;
static trap_fini_func   *FiniFunc = NULL;

static TRAPENTRY_FUNC_PTR( TellHandles );
static TRAPENTRY_FUNC_PTR( TellHardMode );

bool IsTrapFilePumpingMessageQueue( void )
{
    return( TRAPENTRY_PTR_NAME( TellHandles ) != NULL );
}

bool TrapTellHandles( void __far *hab, void __far *hwnd )
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
    char                trpfile[256];
    unsigned            len;
    const char          *ptr;
    unsigned            rc;
    trap_init_func      *init_func;

    if( parms == NULL || *parms == '\0' )
        parms = "std";
    for( ptr = parms; *ptr != '\0' && *ptr != TRAP_PARM_SEPARATOR; ++ptr )
        ;
    len = ptr - parms;
    memcpy( trpfile, parms, len );
    if( stricmp( trpfile, "std" ) == 0 ) {
        unsigned        version;
        char            os2ver;

        DosGetVersion( (PUSHORT)&version );
        os2ver = version >> 8;
        if( os2ver >= 20 ) {
            trpfile[len++] = '3';
            trpfile[len++] = '2';
        } else {
            trpfile[len++] = '1';
            trpfile[len++] = '6';
        }
    }
#ifdef USE_FILENAME_VERSION
    trpfile[len++] = ( USE_FILENAME_VERSION / 10 ) + '0';
    trpfile[len++] = ( USE_FILENAME_VERSION % 10 ) + '0';
#endif
    trpfile[len] = '\0';
    rc = DosLoadModule( NULL, 0, trpfile, &TrapFile );
    if( rc != 0 ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trpfile );
        return( buff );
    }
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    if( DosGetProcAddr( TrapFile, "#1", (PFN FAR *)&init_func ) == 0
      && DosGetProcAddr( TrapFile, "#2", (PFN FAR *)&FiniFunc ) == 0
      && DosGetProcAddr( TrapFile, "#3", (PFN FAR *)&ReqFunc ) == 0 ) {
        if( DosGetProcAddr( TrapFile, "#4", (PFN FAR *)&TRAPENTRY_PTR_NAME( TellHandles ) ) != 0 ) {
            TRAPENTRY_PTR_NAME( TellHandles ) = NULL;
        }
        if( DosGetProcAddr( TrapFile, "#5", (PFN FAR *)&TRAPENTRY_PTR_NAME( TellHardMode ) ) != 0 ) {
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
