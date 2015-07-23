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
* Description:  Win32 runtime main entrypoint.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <process.h>
#include <windows.h>
#include "rtstack.h"
#include "ntext.h"
#include "initfini.h"
#include "initarg.h"
#include "thread.h"
#include "mthread.h"
#include "cominit.h"
#include "winmain.h"
#include "osmainin.h"
#include "procfini.h"


void __F_NAME(__WinMain,__wWinMain)( void )
{
#ifdef __SW_BR
    {
    #ifdef _M_IX86
        REGISTRATION_RECORD rr;
        __NewExceptionFilter( &rr );
    #endif
        __process_fini = __FiniRtns;
        __InitRtns( 255 );
    }
#else
    {
        REGISTRATION_RECORD     rr;
        thread_data             *tdata;
        __InitRtns( INIT_PRIORITY_THREAD );
        tdata = __alloca( __ThreadDataSize );
        memset( tdata, 0, __ThreadDataSize );
        // tdata->__allocated = 0;
        tdata->__data_size = __ThreadDataSize;
        __InitThreadData( tdata );
        __NTMainInit( &rr, tdata );
        /* allocate alternate stack for F77 */
        __ASTACKPTR = (char *)alloca( __ASTACKSIZ ) + __ASTACKSIZ;
    }
#endif
    __CommonInit();
    exit( __F_NAME(WinMain,wWinMain)(
                GetModuleHandle( NULL ),
                0,
                __F_NAME(_LpCmdLine,_LpwCmdLine),
                SW_SHOWDEFAULT ) );
}
