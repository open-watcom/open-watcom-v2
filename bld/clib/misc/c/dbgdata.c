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
#define _DEBUG                  /* so prototypes are included */
#include <crtdbg.h>
#include <stddef.h>
#ifdef __NT__
    #include <windows.h>
#else
    #ifdef __QNX__
        #include <unistd.h>
    #else
        #include <io.h>
    #endif
#endif
#ifdef __OS2__
    #define INCL_WIN
    #include <os2.h>
#endif
#include "dbgdata.h"
#include "rtinit.h"


int                     __DbgReportModes[3] = {
    _CRTDBG_MODE_FILE,          /* _CRT_WARN */
    _CRTDBG_MODE_WNDW,          /* _CRT_ERROR */
    _CRTDBG_MODE_WNDW           /* _CRT_ASSERT */
};

_HFILE                  __DbgReportFiles[3] = {
    _CRTDBG_INVALID_HFILE,      /* _CRT_WARN */
    _CRTDBG_INVALID_HFILE,      /* _CRT_ERROR */
    _CRTDBG_INVALID_HFILE       /* _CRT_ASSERT */
};

_CRT_REPORT_HOOK        __DbgReportHook = NULL;



#if defined(__NT__) || defined(__OS2__)

static int is_windowed_app( void )
/********************************/
{
    #ifdef __NT__
        return( GetActiveWindow() != NULL );
    #elif defined(__OS2__)
        int             retval = 0;
        int             rc;
        HMQ             hMessageQueue = 0;
        HAB             AnchorBlock = 0;

        AnchorBlock = WinInitialize( 0 );
        if( AnchorBlock != 0 ) {
            hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 );
            if( hMessageQueue == 0 ) {
                rc = WinGetLastError( AnchorBlock );
                if( (rc & 0xFFFF) == PMERR_MSG_QUEUE_ALREADY_EXISTS ) {
                    retval = 1;
                }
            }
        }
        if( hMessageQueue != 0 ) {
            WinDestroyMsgQueue( hMessageQueue );
        }
        if( AnchorBlock != 0 ) {
            WinTerminate( AnchorBlock );
        }
        return( retval );
    #endif
}

#endif  /* defined(__NT__) || defined(__OS2__) */


static void do_it( void )
/***********************/
{
    #ifdef __NT__
        if( is_windowed_app() ) {
            __DbgReportModes[_CRT_WARN] = _CRTDBG_MODE_DEBUG;
        } else {
            __DbgReportFiles[_CRT_WARN] = GetStdHandle( STD_ERROR_HANDLE );
        }
    #elif defined(__WINDOWS__)
        __DbgReportModes[_CRT_WARN] = _CRTDBG_MODE_DEBUG;
    #elif defined(__OS2__)
        if( is_windowed_app() ) {
            __DbgReportModes[_CRT_WARN] = _CRTDBG_MODE_DEBUG;
        } else {
            __DbgReportFiles[_CRT_WARN] = STDERR_FILENO;
        }
    #else
        __DbgReportFiles[_CRT_WARN] = STDERR_FILENO;
    #endif
}

AXI( do_it, INIT_PRIORITY_LIBRARY )
