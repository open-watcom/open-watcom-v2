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


#include "variety.h"
#define INCL_WIN
#define INCL_DOSFILEMGR
#define INCL_DOSMODULEMGR
#include <os2.h>
#include <stdlib.h>
#include <string.h>

#define message \
    "This Dynamic Link Library is already in use.\n\n" \
    "The Watcom OS/2 32bit runtime library does not support "\
    "attaching more than one process to a Dynamic Link Library "\
    "that does not have a separate data space for each attached process.\n"

static char dllname[_MAX_PATH];
static char buf[sizeof(message)+sizeof(dllname)+6];

static char *my_strcat( char *p, char *msg ) {
    while( *msg ) {
        *p = *msg;
        if( *p == '\n' ) {
            *p = '\r';
            p++;
            *p = '\n';
        }
        p++;
        msg++;
    }
    return( p );
}

int __disallow_single_dgroup( unsigned hmod ) {

    int         use_pm = 0;
    HMQ         hMessageQueue = 0;
    HAB         AnchorBlock = 0;

    DosQueryModuleName( hmod, sizeof( dllname ), dllname );

    AnchorBlock = WinInitialize( 0 );
    if( AnchorBlock != 0 ) {
        hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 );
        if( hMessageQueue != 0 ) {
            use_pm = 1;
        } else {
            int rc;
            rc = WinGetLastError( AnchorBlock );
            if( (rc & 0xffff) == PMERR_MSG_QUEUE_ALREADY_EXISTS ) {
                use_pm = 1;
            }
        }
    }
    if( use_pm ) {
        WinMessageBox( HWND_DESKTOP, 0, message, dllname, 0, MB_NOICON | MB_OK );
    } else {
        HFILE file;
        ULONG written;
        ULONG action;
        char *p = buf;
        p = my_strcat( p, dllname );
        p = my_strcat( p, "\n" );
        p = my_strcat( p, message );
        *p = 0;
        DosOpen( "CON", &file, &action, 0, 0, 0, OPEN_ACCESS_WRITEONLY, 0 );
        if( file != -1 ) {
            DosWrite( file, buf, strlen( buf ), &written );
            DosClose( file );
        }
    }
    if( hMessageQueue != 0 ) {
        WinDestroyMsgQueue( hMessageQueue );
    }
    if( AnchorBlock != 0 ) {
        WinTerminate( AnchorBlock );
    }
    return( 1 );
}
