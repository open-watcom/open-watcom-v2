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


#include "rcs.h"
#include <stdlib.h>
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#if defined( __WINDOWS__ ) || defined( __NT__ ) || defined( __OS2__ )
/* function pointers */
extern RCSGetVerFn              RCSGetVersion = NULL;
extern RCSInitFn                RCSInit = NULL;
extern RCSCheckoutFn            RCSCheckout = NULL;
extern RCSCheckinFn             RCSCheckin = NULL;
extern RCSHasShellFn            RCSHasShell = NULL;
extern RCSRunShellFn            RCSRunShell = NULL;
extern RCSSetSystemFn           RCSSetSystem = NULL;
extern RCSQuerySystemFn         RCSQuerySystem = NULL;
extern RCSRegBatchCbFn          RCSRegisterBatchCallback = NULL;
extern RCSRegMsgBoxCbFn         RCSRegisterMessageBoxCallback = NULL;
extern RCSSetPauseFn            RCSSetPause = NULL;
extern RCSFiniFn                RCSFini = NULL;
#endif

#if defined( __WINDOWS__ ) || defined( __NT__ )
    #include <windows.h>

    #define GET_ADDR( inst, name, proc ) proc = (void*)GetProcAddress( inst, name )
    static HINSTANCE LibHandle;
    int ViRCSInit()
    {
        LibHandle = LoadLibrary( RCS_DLLNAME );
        if( LibHandle < (HINSTANCE)32 ) {
            return( FALSE );
        }
        getFunctionPtrs();
        return( TRUE );
    }
    int ViRCSFini()
    {
        FreeLibrary( LibHandle );
        return( TRUE );
    }
#elif defined( __OS2__ )
    #include <os2.h>
    static HMODULE LibHandle;
    APIRET APIENTRY  DosLoadModule(PSZ pszName, ULONG cbName, PSZ pszModname, PHMODULE phmod);
    APIRET APIENTRY  DosFreeModule(HMODULE hmod);
    APIRET APIENTRY  DosQueryProcAddr(HMODULE hmod, ULONG ordinal, PSZ pszName,PFN* ppfn);
    #define GET_ADDR( inst, name, proc ) DosQueryProcAddr( inst, 0, name, (PFN*)(&proc) )

    int ViRCSInit()
    {
        #define BUFF_LEN 128
        char fail_name[BUFF_LEN];
        int rc;
        rc = DosLoadModule( fail_name, BUFF_LEN, RCS_DLLNAME, &LibHandle );
        if( rc != 0 ) {
            return( FALSE );
        } else {
            getFunctionPtrs();
            return( LibHandle );
        }
    }
    int ViRCSFini()
    {
        DosFreeModule( LibHandle );
        return( TRUE );
    }
#else
    int ViRCSInit() { return( TRUE ); }
    int ViRCSFini() { return( TRUE ); }
#endif

#if defined( __WINDOWS__ ) || defined( __NT__ ) || defined( __OS2__ )
static void getFunctionPtrs()
{
    GET_ADDR( LibHandle, GETVER_FN_NAME,        RCSGetVersion );
    GET_ADDR( LibHandle, INIT_FN_NAME,          RCSInit );
    GET_ADDR( LibHandle, CHECKOUT_FN_NAME,      RCSCheckout );
    GET_ADDR( LibHandle, CHECKIN_FN_NAME,       RCSCheckin );
    GET_ADDR( LibHandle, HAS_SHELL_FN_NAME,     RCSHasShell );
    GET_ADDR( LibHandle, RUNSHELL_FN_NAME,      RCSRunShell );
    GET_ADDR( LibHandle, SETSYS_FN_NAME,        RCSSetSystem );
    GET_ADDR( LibHandle, GETSYS_FN_NAME,        RCSQuerySystem );
    GET_ADDR( LibHandle, REG_BAT_CB_FN_NAME,    RCSRegisterBatchCallback );
    GET_ADDR( LibHandle, REG_MSGBOX_CB_FN_NAME, RCSRegisterMessageBoxCallback );
    GET_ADDR( LibHandle, SET_PAUSE_FN_NAME,     RCSSetPause );
    GET_ADDR( LibHandle, FINI_FN_NAME,          RCSFini );
}
#endif
