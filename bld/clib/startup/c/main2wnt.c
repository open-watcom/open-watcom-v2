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
#include <windows.h>
#include "ntex.h"
#include "sigtab.h"
#include "initfini.h"

#ifdef __SW_BR
    _WCRTLINK extern    void    (*__process_fini)(unsigned,unsigned);
    _WCRTLINK extern    int     ___Argc;        /* argument count */
    _WCRTLINK extern    int     ___wArgc;       /* argument count */
    _WCRTLINK extern    char    **___Argv;      /* argument vector */
    _WCRTLINK extern    wchar_t **___wArgv;     /* argument vector */
    extern      void    __CommonInit( void );
    extern      int     wmain( int, wchar_t ** );
    extern      int     main( int, char ** );
#else
    extern      void            __NTMainInit( void *, void * );
    #ifdef __WIDECHAR__
        extern  void            __wCMain( void );
        #if defined(_M_IX86)
            #pragma aux __wCMain  "*"
        #endif
    #else
        extern  void            __CMain( void );
        #if defined(_M_IX86)
            #pragma aux __CMain  "*"
        #endif
    #endif
    extern      unsigned        __ThreadDataSize;
#endif

void __F_NAME(__NTMain,__wNTMain)( void )
/***************************************/
{

    #if defined(__SW_BR)
    {
        #if defined(_M_IX86)
            REGISTRATION_RECORD rr;
            __NewExceptionHandler( &rr, 1 );
        #endif
        __process_fini = &__FiniRtns;
        __InitRtns( 255 );
        __CommonInit();
        #ifdef __WIDECHAR__
            exit( wmain( ___Argc, ___wArgv ) );
        #else
            exit( main( ___Argc, ___Argv ) );
        #endif
    }
    #else
    {
        REGISTRATION_RECORD     rr;
        thread_data             *tdata;
        __InitRtns( 1 );
        tdata = __alloca( __ThreadDataSize );
        memset( tdata, 0, __ThreadDataSize );
        // tdata->__allocated = 0;
        tdata->__data_size = __ThreadDataSize;

        __NTMainInit( &rr, tdata );
        __F_NAME(__CMain,__wCMain)();
    }
    #endif
}
#ifdef __WIDECHAR__
    #if defined(_M_IX86)
        #pragma aux __wNTMain "*"
    #endif
#else
    #if defined(_M_IX86)
        #pragma aux __NTMain "*"
    #endif
#endif
