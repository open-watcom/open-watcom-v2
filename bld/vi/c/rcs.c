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
* Description:  RCS DLL loading and function import for vi.
*
****************************************************************************/


#include "vi.h"
#include "rcscli.h"
#include "rcs.h"

#ifndef TRUE
    #define TRUE    1
    #define FALSE   0
#endif

#if defined( __WINDOWS__ ) || defined( __NT__ ) || defined( __OS2__ )
/* function pointers */
extern RCSGetVersionFn          *RCSGetVersion = NULL;
extern RCSInitFn                *RCSInit = NULL;
extern RCSCheckoutFn            *RCSCheckout = NULL;
extern RCSCheckinFn             *RCSCheckin = NULL;
extern RCSHasShellFn            *RCSHasShell = NULL;
extern RCSRunShellFn            *RCSRunShell = NULL;
extern RCSSetSystemFn           *RCSSetSystem = NULL;
extern RCSQuerySystemFn         *RCSQuerySystem = NULL;
extern RCSRegBatchCbFn          *RCSRegisterBatchCallback = NULL;
extern RCSRegMsgBoxCbFn         *RCSRegisterMessageBoxCallback = NULL;
extern RCSSetPauseFn            *RCSSetPause = NULL;
extern RCSFiniFn                *RCSFini = NULL;
#endif

#if defined( __WINDOWS__ ) || defined( __NT__ )

#define GET_ADDR( inst, name, proc, type ) proc = (type *)GetProcAddress( inst, name )
#if defined( __WINDOWS__ )
static HINSTANCE LibHandle = (HINSTANCE)0;
#else
static HINSTANCE LibHandle = NULL;
#endif
static void getFunctionPtrs( void );

bool ViRCSInit( void )
{
    UINT    uErrMode;

    /* Use SetErrorMode to prevent annoying error popups. */
    uErrMode = SetErrorMode( SEM_NOOPENFILEERRORBOX );
    LibHandle = LoadLibrary( RCS_DLLNAME );
    SetErrorMode( uErrMode );
#if defined( __WINDOWS__ )
    if( LibHandle < (HINSTANCE)32 ) {
        LibHandle = (HINSTANCE)0;
#else
    if( LibHandle == NULL ) {
#endif
        return( FALSE );
    }
    getFunctionPtrs();
    return( TRUE );
}

bool ViRCSFini( void )
{
#if defined( __WINDOWS__ )
    if( LibHandle != (HINSTANCE)0 )
#else
    if( LibHandle != NULL )
#endif
        FreeLibrary( LibHandle );
    return( TRUE );
}

#elif defined( __OS2__ ) && defined( __386__ )

static HMODULE LibHandle;
APIRET APIENTRY  DosLoadModule( PSZ pszName, ULONG cbName, PSZ pszModname, PHMODULE phmod );
APIRET APIENTRY  DosFreeModule( HMODULE hmod );
APIRET APIENTRY  DosQueryProcAddr( HMODULE hmod, ULONG ordinal, PSZ pszName, PFN *ppfn );
#define GET_ADDR( inst, name, proc, type ) DosQueryProcAddr( inst, 0, name, (PFN *)(&proc) )
static void getFunctionPtrs( void );

bool ViRCSInit( void )
{
    #define BUFF_LEN 128
    char    fail_name[BUFF_LEN];
    int     rc;

    rc = DosLoadModule( fail_name, BUFF_LEN, RCS_DLLNAME, &LibHandle );
    if( rc != 0 ) {
        return( FALSE );
    } else {
        getFunctionPtrs();
        return( ( LibHandle != 0 ) );
    }
}

bool ViRCSFini( void )
{
    DosFreeModule( LibHandle );
    return( TRUE );
}

#else

bool ViRCSInit( void )
{
    return( TRUE );
}

bool ViRCSFini( void )
{
    return( TRUE );
}

#endif

#if defined( __WINDOWS__ ) || defined( __NT__ ) || (defined( __OS2__ ) && defined( __386__ ))
static void getFunctionPtrs( void )
{
    GET_ADDR( LibHandle, GETVER_FN_NAME,        RCSGetVersion,                 RCSGetVersionFn );
    GET_ADDR( LibHandle, INIT_FN_NAME,          RCSInit,                       RCSInitFn );
    GET_ADDR( LibHandle, CHECKOUT_FN_NAME,      RCSCheckout,                   RCSCheckoutFn );
    GET_ADDR( LibHandle, CHECKIN_FN_NAME,       RCSCheckin,                    RCSCheckinFn );
    GET_ADDR( LibHandle, HAS_SHELL_FN_NAME,     RCSHasShell,                   RCSHasShellFn );
    GET_ADDR( LibHandle, RUNSHELL_FN_NAME,      RCSRunShell,                   RCSRunShellFn );
    GET_ADDR( LibHandle, SETSYS_FN_NAME,        RCSSetSystem,                  RCSSetSystemFn );
    GET_ADDR( LibHandle, GETSYS_FN_NAME,        RCSQuerySystem,                RCSQuerySystemFn );
    GET_ADDR( LibHandle, REG_BAT_CB_FN_NAME,    RCSRegisterBatchCallback,      RCSRegBatchCbFn );
    GET_ADDR( LibHandle, REG_MSGBOX_CB_FN_NAME, RCSRegisterMessageBoxCallback, RCSRegMsgBoxCbFn );
    GET_ADDR( LibHandle, SET_PAUSE_FN_NAME,     RCSSetPause,                   RCSSetPauseFn );
    GET_ADDR( LibHandle, FINI_FN_NAME,          RCSFini,                       RCSFiniFn );
}
#endif
