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
* Description:  Single DGROUP in a DLL sanity check for 32-bit OS/2.
*
****************************************************************************/


/* Note: All the complex junk is here to make sure the clib does not
 * statically link against Presentation Manager DLLs, so that we can load
 * without PM installed.
 */

#include "variety.h"
#define INCL_WIN
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSPROCESS
#define INCL_DOSMODULEMGR
#define INCL_ORDINALS
#include <os2.h>
#include <stdlib.h>
#include <string.h>

#define message \
    "The DLL is already in use.\n\n" \
    "The Open Watcom OS/2 32-bit runtime library does not support "\
    "attaching multiple processes to a DLL with shared DGROUP.\n"

static char dllname[_MAX_PATH];
static char buf[sizeof( message ) + sizeof( dllname ) + 6];

static char *my_strcat( char *p, char *msg )
{
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

static HAB     (APIENTRY *pfnWinInitialize)( ULONG );
static HMQ     (APIENTRY *pfnWinCreateMsgQueue)( HAB, ULONG );
static ULONG   (APIENTRY *pfnWinGetLastError)( HAB );
static ERRORID (APIENTRY *pfnWinGetLastError)( HAB );
static BOOL    (APIENTRY *pfnWinDestroyMsgQueue)( HMQ );
static BOOL    (APIENTRY *pfnWinTerminate)( HAB );
static BOOL    (APIENTRY *pfnWinTerminate)( HAB );
static ULONG   (APIENTRY *pfnWinMessageBox)( HWND, HWND, PCSZ, PCSZ, ULONG, ULONG );

int __disallow_single_dgroup( unsigned hmod )
{
    int         use_pm = 0;
    HMQ         hMessageQueue = 0;
    HAB         AnchorBlock = 0;
    PTIB        ptib;
    PPIB        ppib;
    HMODULE     hmodPMWIN;

    DosQueryModuleName( hmod, sizeof( dllname ), dllname );

    DosGetInfoBlocks( &ptib, &ppib );
    /* If this isn't a PM process, don't even try to initialize PM */
    if( ppib->pib_ultype == PT_PM ) {
        if( DosLoadModule( NULL, 0, "PMWIN", &hmodPMWIN) == NO_ERROR
            && DosQueryProcAddr( hmodPMWIN, ORD_WIN32INITIALIZE, NULL, (PFN*)&pfnWinInitialize ) == NO_ERROR
            && DosQueryProcAddr( hmodPMWIN, ORD_WIN32CREATEMSGQUEUE, NULL, (PFN*)&pfnWinCreateMsgQueue ) == NO_ERROR
            && DosQueryProcAddr( hmodPMWIN, ORD_WIN32GETLASTERROR, NULL, (PFN*)&pfnWinGetLastError ) == NO_ERROR
            && DosQueryProcAddr( hmodPMWIN, ORD_WIN32DESTROYMSGQUEUE, NULL, (PFN*)&pfnWinDestroyMsgQueue ) == NO_ERROR
            && DosQueryProcAddr( hmodPMWIN, ORD_WIN32TERMINATE, NULL, (PFN*)&pfnWinTerminate ) == NO_ERROR
            && DosQueryProcAddr( hmodPMWIN, ORD_WIN32MESSAGEBOX, NULL, (PFN*)&pfnWinMessageBox ) == NO_ERROR
            ) {
            AnchorBlock = pfnWinInitialize( 0 );
            if( AnchorBlock != 0 ) {
                hMessageQueue = pfnWinCreateMsgQueue( AnchorBlock, 0 );
                if( hMessageQueue != 0 ) {
                    use_pm = 1;
                } else {
                    int rc;
                    rc = pfnWinGetLastError( AnchorBlock );
                    if( (rc & 0xffff) == PMERR_MSG_QUEUE_ALREADY_EXISTS ) {
                        use_pm = 1;
                    }
                }
            }
        }
    }
    if( use_pm ) {
        pfnWinMessageBox( HWND_DESKTOP, 0, message, dllname, 0, MB_NOICON | MB_OK );
    } else {
        HFILE   file;
        ULONG   written;
        ULONG   action;
        char    *p = buf;

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
        pfnWinDestroyMsgQueue( hMessageQueue );
    }
    if( AnchorBlock != 0 ) {
        pfnWinTerminate( AnchorBlock );
    }
    return( 1 );
}
