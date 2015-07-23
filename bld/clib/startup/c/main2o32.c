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
* Description:  OS/2 32-bit executable entry point.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#define INCL_DOSPROCESS
#include "initfini.h"
#include "thread.h"
#include "initsig.h"
#include "initarg.h"
#include "mthread.h"
#include "osmain.h"
#include "osmainin.h"
#include "cmain.h"
#include "cominit.h"
#include "procfini.h"

#ifdef __SW_BR
    extern      int     main( int, char ** );
    extern      int     wmain( int, wchar_t ** );
#endif

void __F_NAME(__OS2Main,__wOS2Main)( unsigned hmod, unsigned reserved,
                                     char *env, char *cmd )
/********************************************************************/
{
    EXCEPTIONREGISTRATIONRECORD     xcpt;

#ifdef __SW_BR
    __hmodule = hmod;
    env = env;
    cmd = cmd;
    // Even though the exception handler and all that is
    // in the runtime DLL, it must be registered from here since
    // the registration record needs to live on stack
    __XCPTHANDLER = &xcpt;
    __process_fini = __FiniRtns;
    __InitRtns( 255 );
    __sig_init_rtn();
    __CommonInit();
    exit( __F_NAME(main( ___Argc, ___Argv ),wmain( ___wArgc, ___wArgv )) );
#else
    thread_data     *tdata;

    __InitRtns( INIT_PRIORITY_THREAD );

    tdata = __alloca( __ThreadDataSize );
    memset( tdata, 0, __ThreadDataSize );
    // tdata->__allocated = 0;
    tdata->__data_size = __ThreadDataSize;
    __InitThreadData( tdata );
    __OS2MainInit( &xcpt, tdata, hmod, env, cmd );
    __F_NAME(__CMain,__wCMain)();
#endif
    reserved = reserved;
}
