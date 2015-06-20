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
* Description:  Win32 executable entry point.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <windows.h>
#include "ntext.h"
#include "sigtab.h"
#include "initfini.h"
#include "initarg.h"
#include "thread.h"
#include "mthread.h"
#include "osmain.h"
#include "cmain.h"
#include "cominit.h"


#ifdef __SW_BR
    _WCRTLINK extern    void    (*__process_fini)( unsigned, unsigned );
    extern      int     wmain( int, wchar_t ** );
    extern      int     main( int, char ** );
#else
    extern      void            __NTMainInit( void *, void * );
#endif

void __F_NAME(__NTMain,__wNTMain)( void )
/***************************************/
{
#if defined(__SW_BR)
  #if defined(_M_IX86)
    REGISTRATION_RECORD rr;

    __NewExceptionFilter( &rr );
  #endif
    __process_fini = &__FiniRtns;
    __InitRtns( 255 );
    __CommonInit();
    exit( __F_NAME(main( ___Argc, ___Argv ),wmain( ___wArgc, ___wArgv )) );
#else
    REGISTRATION_RECORD     rr;
    thread_data             *tdata;

    __InitRtns( INIT_PRIORITY_THREAD );
    tdata = __alloca( __ThreadDataSize );
    memset( tdata, 0, __ThreadDataSize );
    // tdata->__allocated = 0;
    tdata->__data_size = __ThreadDataSize;

    __InitThreadData( tdata );
    __NTMainInit( &rr, tdata );
    __F_NAME(__CMain,__wCMain)();
#endif
}
