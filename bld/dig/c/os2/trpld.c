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
#include <stddef.h>
#include <stdio.h>
#include "trpimp.h"
#include "tcerr.h"

static trap_version     (TRAPENTRY *InitFunc)(char *,char *, bool);
static void             (TRAPENTRY *FiniFunc)(void);
static unsigned_16      (TRAPENTRY *InfoFunc)( void *, void * );
static char             (TRAPENTRY *HardFunc)( char );

extern trap_version     TrapVer;
extern unsigned         (TRAPENTRY *ReqFunc)( unsigned, mx_entry *,
                                        unsigned, mx_entry * );

bool IsTrapFilePumpingMessageQueue()
{
    return( InfoFunc != NULL );
}

void TellHandles( void far *hab, void far *hwnd )
{
    if( InfoFunc == NULL ) return;
    InfoFunc( hab, hwnd );
}


char TellHardMode( char hard )
{
    if( HardFunc == NULL ) return( 0 );
    return( HardFunc( hard ) );
}

void KillTrap( void )
{
    FiniFunc();
    ReqFunc = NULL;
    InitFunc = NULL;
    FiniFunc = NULL;
    InfoFunc = NULL;
    HardFunc = NULL;
}

char *LoadTrap( char *trapbuff, char *buff, trap_version *trap_ver )
{
    char                trpfile[256];
    int                 len;
    char                *ptr;
    char                *parm;
    unsigned            dll;
    unsigned            rc;

    if( trapbuff == NULL ) trapbuff = "std";
    ptr = trapbuff;
    while( *ptr != '\0' && *ptr != ';' ) {
        ++ptr;
    }
    parm = (*ptr != '\0') ? ptr + 1 : ptr;
    len = ptr - trapbuff;
    memcpy( trpfile, trapbuff, len );
    trpfile[len] = '\0';
    if( !stricmp( trpfile, "std" ) ) {
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
        trpfile[len] = 0;
    }
    rc = DosLoadModule( NULL, 0, trpfile, (PHMODULE)&dll );
    if( rc != 0 ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trpfile );
        return( buff );
    }
    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
    if( DosGetProcAddr( dll, "#1", (PFN FAR *)&InitFunc ) != 0
     || DosGetProcAddr( dll, "#2", (PFN FAR *)&FiniFunc ) != 0
     || DosGetProcAddr( dll, "#3", (PFN FAR *)&ReqFunc ) != 0 ) {
        return( buff );
    }
    if( DosGetProcAddr( dll, "#4", (PFN FAR *)&InfoFunc ) != 0 ) {
        InfoFunc = NULL;
    }
    if( DosGetProcAddr( dll, "#5", (PFN FAR *)&HardFunc ) != 0 ) {
        HardFunc = NULL;
    }
    *trap_ver = InitFunc( parm, trpfile, trap_ver->remote );
    if( trpfile[0] != '\0' ) {
        strcpy( buff, (char *)trpfile );
        return( buff );
    }
    if( !TrapVersionOK( *trap_ver ) ) {
        KillTrap();
        return( buff );
    }
    TrapVer = *trap_ver;
    return( NULL );
}
