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
* Description:  FORTRAN 77 run-time DLL entry
*
****************************************************************************/


#include "ftnstd.h"
#include <stdlib.h>
#include "fdll.h"


#if defined( __NT__ )

// Note: Suppose the user creates a DLL that contains C/C++ and FORTRAN
// object files and LibMain() is written in C/C++ (i.e. it's his own or the
// default one in the C library).  Suppose he then calls a FORTRAN subroutine
// from his DLL and the FORTRAN subroutine does some FORTRAN I/O.  Since the
// FORTRAN 77 library version of LibMain() is not called, he must call
// __FInitDLL() and __FTermDLL() explicitly from his C or C++ version of
// LibMain().  Also, he must call __FInitThrd() and __FTermThrd() on thread
// initialization and termination.


int   APIENTRY LibMain( HANDLE hdll, ULONG reason, LPVOID reserved ) {
//====================================================================

    unsigned    rc;

    hdll = hdll; reserved = reserved;
    switch( reason ) {
    case DLL_PROCESS_ATTACH:
        if( __FInitDLL() == 0 ) return( 0 );
        return( __fdll_initialize() );
    case DLL_PROCESS_DETACH:
        rc = __fdll_terminate();
        __FTermDLL();
        return( rc );
    case DLL_THREAD_ATTACH:
        if( __FInitThrd() == 0 ) return( 0 );
        return( __fthrd_initialize() );
    case DLL_THREAD_DETACH:
        rc = __fthrd_terminate();
        __FTermThrd();
        return( rc );
    default:
        return( 1 );
    }
}

#endif


#if defined( __OS2__ )

static void     f_dll_terminate(void) {
//=====================================

    __fdll_terminate();
    __FTermDLL();
}

int             __dll_initialize(void) {
//======================================

    if( __FInitDLL() == 0 ) return( 0 );
#if defined( __386__ )
    atexit( f_dll_terminate );
#endif
    return( __fdll_initialize() );
}

#endif
