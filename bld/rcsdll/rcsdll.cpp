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
* Description:  SCM interface library implementation.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined( __WINDOWS__ ) || defined( __NT__ )
    #include <windows.h>
    static      HINSTANCE   hInstance = 0;
#elif defined( __OS2__ )
    #define INCL_WINSHELLDATA
    extern "C" {
    #include <os2.h>
    }
#endif
#ifndef TRUE
    #define TRUE 1
    #define FALSE 0
#endif
#include "rcsdll.hpp"
#include "inifile.hpp"

mksRcsSystem    MksRcs;
pvcsSystem      Pvcs;
genericRcs      Generic;
p4System        Perforce;
wprojRcs        Wproj;

extern "C" {
/* common functions */
extern RCSGetVersionFn  RCSGetVersion;
extern RCSSetSystemFn   RCSSetSystem;
extern RCSQuerySystemFn RCSQuerySystem;
extern RCSRegBatchCbFn  RCSRegisterBatchCallback;
extern RCSRegMsgBoxCbFn RCSRegisterMessageBoxCallback;
/* system specific functions -- mapped to function for appropriate system */
extern RCSInitFn        RCSInit;
extern RCSCheckoutFn    RCSCheckout;
extern RCSCheckinFn     RCSCheckin;
extern RCSHasShellFn    RCSHasShell;
extern RCSRunShellFn    RCSRunShell;
extern RCSFiniFn        RCSFini;
extern RCSSetPauseFn    RCSSetPause;

static char *rcs_type_strings[] = {
    "no_rcs",
    "mks_rcs",
    "mks_si",
    "pvcs",
    "generic",
    "o_cycle",
    "perforce",
    "wproj" // hidden
};

static char *pause_strings[] = {
    "no_pause",
    "pause"
};


static rcsSystem *rcs_systems[] = {
    NULL,
    &MksRcs,
#if defined( __WINDOWS__ ) || defined( __NT__ )
    &MksSI,
#else
    NULL,
#endif
    &Pvcs,
    &Generic,
#if defined( __NT__ ) && !defined( __AXP__ )
    &ObjCycle,
#else
    NULL,
#endif
    &Perforce,
    &Wproj // hidden
};

rcsdata RCSAPI RCSInit( unsigned long window, char *cfg_dir )
{
    userData *data;

    data = new userData( window, cfg_dir );
    data->setSystem( RCSQuerySystem( data ) ); /* sets var. & calls init */
    return( (rcsdata)data );
}

void RCSAPI RCSFini( rcsdata data )
{
    userData *d = (userData*)data;
    if( d==NULL ) return;
    if( d->getSystem() != NULL ) {
        d->getSystem()->fini();
    }
    delete( d );
}

int RCSAPI RCSCheckout( rcsdata data, rcsstring name, rcsstring pj, rcsstring tgt )
{
    userData *d = (userData*)data;
    if( d==NULL || d->getSystem() == NULL ) return( 0 );
    return( d->getSystem()->checkout( d, name, pj, tgt ) );
}
int RCSAPI RCSCheckin( rcsdata data, rcsstring name, rcsstring pj, rcsstring tgt )
{
    userData *d = (userData*)data;
    if( d==NULL || d->getSystem() == NULL ) return( 0 );
    return( d->getSystem()->checkin( d, name, pj, tgt ) );
}
int RCSAPI RCSHasShell( rcsdata data )
{
    userData *d = (userData*)data;
    if( d==NULL || d->getSystem() == NULL ) return( 0 );
    return( d->getSystem()->hasShell() );
}
int RCSAPI RCSRunShell( rcsdata data )
{
    userData *d = (userData*)data;
    if( d==NULL || d->getSystem() == NULL ) return( 0 );
    return( d->getSystem()->runShell() );
}
void RCSAPI RCSSetPause( rcsdata data, int p )
{
    userData *d = (userData*)data;
    if( d==NULL || d->getSystem() == NULL ) return;
    d->setPause( p );
}

/* common functions */

int RCSAPI RCSGetVersion() { return( RCS_DLL_VER ); }

int RCSAPI RCSSetSystem( rcsdata data, int rcs_type )
{
    userData *d = (userData*)data;
    if( d==NULL ) return( 0 );
    if( !d->setSystem( rcs_type ) ) return( FALSE );
    if( rcs_type > MAX_RCS_TYPE ) return( FALSE );
    MyWriteProfileString( (char*)d->getCfgDir(), RCS_CFG, RCS_SECTION,
                          RCS_KEY, rcs_type_strings[rcs_type] );
    return( TRUE );

}

int RCSAPI RCSQuerySystem( rcsdata data )
{
    char buffer[MAX_RCS_STRING_LEN];
    int i;

    userData *d = (userData*)data;
    if( d==NULL ) return( 0 );
    MyGetProfileString( (char*)d->getCfgDir(), RCS_CFG, RCS_SECTION, RCS_KEY,
                        RCS_DEFAULT, buffer, MAX_RCS_STRING_LEN );
    for( i=1; i <= MAX_RCS_TYPE; i++ ) {
        if( strnicmp( buffer, rcs_type_strings[i],
                        strlen( rcs_type_strings[i] ) ) == 0 ) return( i );
    }
    return( 0 );
}

int RCSAPI RCSRegisterBatchCallback( rcsdata data,
                                     BatchCallbackFP fp, void *cookie )
{
    userData *d = (userData*)data;
    if( d==NULL ) return( 0 );
    return( d->regBatcher( fp, cookie ) );
}

int RCSAPI RCSRegisterMessageBoxCallback( rcsdata data,
                                  MessageBoxCallbackFP fp, void *cookie )
{
    userData *d = (userData*)data;
    if( d==NULL ) return( 0 );
    return( d->regMessager( fp, cookie ) );
}

#ifdef __NT__

int WINAPI LibMain( HINSTANCE hDll, DWORD reason, LPVOID res )
{
    res = res;
    reason = reason;

    hInstance = hDll;
    return( 1 );
}

#elif defined( __WINDOWS__ )

int WINAPI LibMain( HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine )
{
    wDataSeg = wDataSeg;
    wHeapSize = wHeapSize;
    lpszCmdLine = lpszCmdLine;

    hInstance = hInst;

    return( 1 );
}

int CALLBACK WEP( int q )
{
    q = q;
    return( 1);
}

#elif defined( __OS2__ )

int     __dll_initialize( void )
{
    return( 1 );
}

int     __dll_terminate( void )
{
    return( 1 );
}

#endif

}  // extern "C"

int userData::setSystem( int rcs_type )
{
    if( currentSystem != NULL ) {
        currentSystem->fini();
    }
    currentSystem = rcs_systems[rcs_type];
    if( currentSystem != NULL ) {
        if( !currentSystem->init( this ) ) return( FALSE );
    }
    return( TRUE );
}

int rcsSystem::checkout( userData *d, rcsstring name,
                         rcsstring pj, rcsstring tgt )
{
    char Buffer[BUFLEN];
    if( d == NULL ) return( 0 );
    if( d->batcher ) {
        sprintf( Buffer, "%s %s %s %s %s",
            checkout_name, pause_strings[d->getPause()], name,
                pj != NULL ? pj : "", tgt != NULL ? tgt : "" );
        d->batcher( Buffer, d->batch_cookie );
    }
    return( 1 );
}

int rcsSystem::checkin( userData *d, rcsstring name,
                        rcsstring pj, rcsstring tgt )
{
    char MsgBuf[BUFLEN];
    char Buffer[BUFLEN];
    char path[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    int i=0;
    FILE *fp;

    *MsgBuf = '\0';
    if( d == NULL ) return( 0 );
    if( d->msgBox ) {
        sprintf( Buffer, "Checkin %s", name );
        if( !d->msgBox( "Enter Message", Buffer, MsgBuf, BUFLEN, d->msg_cookie ) ) return( TRUE );

    }
    _splitpath( name, drive, dir, NULL, NULL );
    _makepath( path, drive, dir, "temp", "___" );
    for( i = 0; i < 10; i++ ) {
        path[strlen(path)-1] = (char)(i + '0');
        if( access( path, W_OK ) ) {
            break;
        }
    }
    if( i >= 10 ) return( FALSE );

    fp = fopen( path, "w" );
    fputs( MsgBuf, fp );
//  fputs( ".", fp ); // for MKS
    fclose( fp );

    if( d->batcher ) {
        sprintf( Buffer, "%s %s %s %s %s ",
            checkin_name, name, path, pj, tgt );
        d->batcher( Buffer, d->batch_cookie );
    }
    remove( path );
    return( 1 );
}
