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
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define INCL_DOSPROCESS
#define INCL_DOSEXCEPTIONS
#include <wos2.h>
#include "sigtab.h"
#include "initfini.h"
#include "thread.h"

#ifdef __SW_BR
    _WCRTLINK extern    unsigned    __hmodule;
    _WCRTLINK extern    void        (*__process_fini)(unsigned,unsigned);
    _WCRTLINK extern    int         ___Argc;    /* argument count */
    _WCRTLINK extern    char        **___Argv;  /* argument vector */
    _WCRTLINK extern    int         ___wArgc;   /* argument count */
    _WCRTLINK extern    wchar_t     **___wArgv; /* argument vector */

    extern      void    __CommonInit( void );
    extern      int     main( int, char ** );
    extern      int     wmain( int, wchar_t ** );
#else
    extern      void    __OS2MainInit( EXCEPTIONREGISTRATIONRECORD *,
                                       void *, unsigned, char *,
                                       char * );
    #ifdef __WIDECHAR__
        extern  void    __wCMain( void );
        #if defined(_M_IX86)
            #pragma aux __wCMain   "*"
        #endif
    #else
        extern  void    __CMain( void );
        #if defined(_M_IX86)
            #pragma aux __CMain   "*"
        #endif
    #endif
    extern      unsigned        __ThreadDataSize;
#endif

void __F_NAME(__OS2Main,__wOS2Main)( unsigned hmod, unsigned reserved,
/********************************************************************/
    char *env, char *cmd )
{
    EXCEPTIONREGISTRATIONRECORD xcpt;
    reserved = reserved;
    #ifdef __SW_BR
        __hmodule = hmod;
        env = env;
        cmd = cmd;
        __XCPTHANDLER = &xcpt;
        __process_fini = &__FiniRtns;
        __InitRtns( 255 );
        __CommonInit();
        #ifdef __WIDECHAR__
            exit( wmain( ___wArgc, ___wArgv ) );
        #else
            exit( main( ___Argc, ___Argv ) );
        #endif
    #else
    {
        thread_data     *tdata;

        __InitRtns( 1 );

        tdata = __alloca( __ThreadDataSize );
        memset( tdata, 0, __ThreadDataSize );
        // tdata->__allocated = 0;
        tdata->__data_size = __ThreadDataSize;

        __OS2MainInit( &xcpt, tdata, hmod, env, cmd );
        __F_NAME(__CMain,__wCMain)();
    }
    #endif
}
#if defined(_M_IX86)
    #ifdef __WIDECHAR__
        #pragma aux __wOS2Main "*" parm caller []
    #else
        #pragma aux __OS2Main "*" parm caller []
    #endif
#endif
