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
#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include "initfini.h"
#include "libwin32.h"

extern int APIENTRY LibMain( HANDLE, DWORD, LPVOID );
extern void __CommonInit( void );
extern BOOL __disallow_single_dgroup( HANDLE );

#ifdef __SW_BR
    extern int          __Is_DLL;       /* TRUE => DLL, else not a DLL */
    extern char *       _LpDllName;
    extern wchar_t *    _LpwDllName;
#else
    extern int  __NTInit( int, void *, HANDLE );
    extern void __NTFini( void );
    extern BOOL __NTThreadInit( void );
    extern void __InitMultipleThread( void );
    extern BOOL __NTAddThread(void *);
    extern void __NTRemoveThread( int );
#endif

_WCRTLINK extern void (*__sig_init_rtn)(void);


int APIENTRY _LibMain( HANDLE hdll, DWORD reason, LPVOID reserved )
{
    int rc;
    static int processes;

    switch( reason ) {
    case DLL_THREAD_ATTACH:
        #ifndef __SW_BR
            if( !__NTAddThread( NULL ) ) {
                return( FALSE );
            }
        #endif
        rc = LibMain( hdll, reason, reserved );
        break;
    case DLL_PROCESS_ATTACH:
        ++processes;
        if( processes > 1 ) {
            if( __disallow_single_dgroup( hdll ) ) {
                rc = FALSE;
                break;
            }
        }
        #ifdef __SW_BR
            __Is_DLL = 1;
            __InitRtns( 15 );
        #else
            // The following initializers are called: (in the CLIB run-time DLL):
            //      nothing is called
            __InitRtns( 1 );
            // allocate some thread data storage and initialize run-time variables
            if( !__NTInit( TRUE, NULL, hdll ) ) {
                rc = FALSE;
                break;
            }
            // set up TLSIndex thingee
            if( !__NTThreadInit() ) {   // safe to call multiple times
                rc = FALSE;
                break;
            }
            // The following initializers are called: (in the CLIB run-time DLL):
            //      __chk8087
            //      __verify_pentium_fdiv_bug
            //      __Init_Argv
            //      __imthread_fn (which calls _NTThreadInit and __InitMultipleThread)
            __InitRtns( 15 );
            // sets up semaphores and starts linked list of thread data storage
            __InitMultipleThread();     // now safe to call multiple times
        #endif
        if( _pRawDllMain != NULL ) {
            if( !_pRawDllMain( hdll, reason, reserved ) ) {
                __FiniRtns( 0, FINI_PRIORITY_EXIT-1 );
                rc = FALSE;
                break;
            }
        }
        // The following initializers are called: (in the CLIB run-time DLL):
        //      profilog_init
        //      __InitWinLinesSem
        //      dbgdata@do_it
        //      __setenvp
        //      __mbInitOnStartup
        //      __sig_init
        //      __is_nonIBM
        //      (??) in STK
        //      __InitFiles
        //      __clock_init
        __InitRtns( 255 );
        #ifdef __SW_BR
        {
            static char    fn[_MAX_PATH];
            GetModuleFileNameA( hdll, fn, sizeof( fn ) );
            _LpDllName = fn;
        }
        {
            static wchar_t wfn[_MAX_PATH];
            __lib_GetModuleFileNameW( hdll, wfn, sizeof( wfn ) );
            _LpwDllName = wfn;
        }
        #endif
        __CommonInit();
        __sig_init_rtn();
        rc = LibMain( hdll, reason, reserved );
        if( !rc ) {
            __FiniRtns( 0, 255 );
        }
        break;
    case DLL_THREAD_DETACH:
        rc = LibMain( hdll, reason, reserved );
        #ifndef __SW_BR
            __NTRemoveThread( TRUE );
        #endif
        break;
    case DLL_PROCESS_DETACH:
        rc = LibMain( hdll, reason, reserved );
        __FiniRtns( FINI_PRIORITY_EXIT, 255 );
        if( _pRawDllMain != NULL ) {
            _pRawDllMain( hdll, reason, reserved );
        }
        #ifndef __SW_BR
            __NTFini(); // must be done before following finalizers get called
        #endif
        __FiniRtns( 0, FINI_PRIORITY_EXIT-1 );
        #ifndef __SW_BR
            __NTRemoveThread( TRUE );
        #endif
        --processes;
    }
    return( rc );
}
