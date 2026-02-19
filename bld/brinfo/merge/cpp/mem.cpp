/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


/*
    Copyright (c) WATCOM Systems Inc., 1992. All rights reserved.

    Modified    By              Reason
    ========    ==              ======
    9-nov-92    Jim Randall     initial implementation
*/

#include <malloc.h>
//#include <dr.h>       //include only for the browser
#ifdef TRMEM
    #include "trmem.h"
    #include "io.h"
    #include "fcntl.h"
  #ifdef __WINDOWS__
    #define TRMEM_NO_STDOUT
    #define STRICT
    #include <windows.h>
  #endif
  #ifdef __OS2__
    #define TRMEM_NO_STDOUT
    #define INCL_PM
    #include <os2.h>
  #endif
#endif


#ifdef TRMEM

#pragma initialize  40;

static unsigned     NumMessages = 0;
static _trmem_hdl   TrHdl = _TRMEM_HDL_NONE;
static FILE         *TrFileHandle = NULL;

struct Memory
{
        Memory();
        ~Memory();
};

static Memory bogus;    // just need to get the ctor's called

void PrintLine( void *parm, const char *buf, size_t len )
{
    /* unused parameters */ (void)parm; (void)len;

    if( TrFileHandle != NULL ) {
        fprintf( TrFileHandle, "%s\n", buf );
    }
    NumMessages++;
}

#endif /* TRMEM */

void *operator new( size_t size )
/*******************************/
{
    void *p;

//  for(;;) {
#ifdef TRMEM
        p = _trmem_alloc( size, _trmem_guess_who(), TrHdl );
#else
        p = malloc( size );
#endif
//      if( p != NULL || !DRSwap() ) break;     //only for browser
//    }
    return p;
}

extern "C" {

void * WBRAlloc( size_t size )
/****************************/
// note: code directly cloned from above since we need to be able to trace
// calling functions when the memory tracker is in.
{
    void *p;

//    for(;;) {
#ifdef TRMEM
        p = _trmem_alloc( size, _trmem_guess_who(), TrHdl );
#else
        p = malloc( size );
#endif
//      if( p != NULL || !DRSwap() ) break;
//    }
    return p;
}

void * WBRRealloc( void * p, size_t size )
/****************************************/
// note: code cloned from above since we need to be able to trace
// calling functions when the memory tracker is in.
{
//    for(;;) {
#ifdef TRMEM
        p = _trmem_realloc( p, size, _trmem_guess_who(), TrHdl );
#else
        p = realloc( p, size );
#endif
//      if( p != NULL || !DRSwap() ) break;
//    }
    return p;
}

void WBRFree( void *p )
/*********************/
{
    if( p == NULL )
        return;
#ifdef TRMEM
    _trmem_free( p, _trmem_guess_who(), TrHdl );
#else
    free( p );
#endif
}

};

void operator delete( void *p )
/*****************************/
{
    if( p == NULL )
        return;
#ifdef TRMEM
    _trmem_free( p, _trmem_guess_who(), TrHdl );
#else
    free( p );
#endif
}

#ifdef TRMEM
Memory::Memory()
/**************/
{
    TrHdl = _trmem_open( malloc, free, realloc, _TRMEM_NO_STRDUP,
                            NULL, PrintLine, _TRMEM_DEF );
#ifdef TRMEM_NO_STDOUT
    TrFileHandle = fopen( "tracker.txt", "w" );
#else
    TrFileHandle = stdout;
#endif
}

Memory::~Memory()
{
    if( TrHdl != _TRMEM_HDL_NONE ) {
        if( _trmem_get_current_usage( TrHdl ) ) {
            _trmem_prt_usage( TrHdl );
        }
        _trmem_prt_list( TrHdl );
        _trmem_close( TrHdl );
        TrHdl = _TRMEM_HDL_NONE;
    }
#ifdef TRMEM_NO_STDOUT
    fclose( TrFileHandle );
    if( NumMessages > 1 ) {
#ifdef __WINDOWS__
        MessageBox ( NULL, "memory problems detected", "Memory Tracker",
                     MB_ICONINFORMATION | MB_OK | MB_TASKMODAL );
#elif defined( __OS2__ )
        WinMessageBox ( HWND_DESKTOP, NULL, "memory problems detected",
                    "Memory Tracker", 0,
                     MB_ICONASTERISK | MB_OK | MB_APPLMODAL );
#endif
    }
#endif
    TrFileHandle = NULL;
}
#endif
