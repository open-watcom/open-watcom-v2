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


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

extern  void    __CommonInit( void );
#if !defined(__OSI__)
extern  unsigned        __ASTACKSIZ;    /* alternate stack size */
extern  char            *__ASTACKPTR;   /* alternate stack pointer */
#if defined(_M_IX86)
 #pragma        aux     __ASTACKPTR "*"
 #pragma        aux     __ASTACKSIZ "*"
#endif
#endif

#ifdef __WIDECHAR__
    _WCRTLINK extern    int          ___wArgc;  /* argument count */
    _WCRTLINK extern    wchar_t    **___wArgv;  /* argument vector */
    extern      int     wmain( int, wchar_t ** );
    #if defined(_M_IX86)
        #pragma aux     __wCMain  "*";
    #endif
    void __wCMain()
    {
        #if !defined(__OSI__)
            /* allocate alternate stack for F77 */
            __ASTACKPTR = (char *)alloca( __ASTACKSIZ ) + __ASTACKSIZ;
        #endif
        __CommonInit();
        exit( wmain( ___wArgc, ___wArgv ) );
    }
#else
    _WCRTLINK extern    int       ___Argc;      /* argument count */
    _WCRTLINK extern    char    **___Argv;      /* argument vector */
    extern      int     main( int, char ** );
    #if defined(_M_IX86)
        #pragma aux     __CMain  "*";
    #endif
    void __CMain()
    {
        #if !defined(__OSI__)
            /* allocate alternate stack for F77 */
            __ASTACKPTR = (char *)alloca( __ASTACKSIZ ) + __ASTACKSIZ;
        #endif
        __CommonInit();
        exit( main( ___Argc, ___Argv ) );
    }
#endif
