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


#include <malloc.h>
//#include <dr.h>       //include only for the browser
#ifdef TRACKER
extern "C" {
#include "trmem.h"
}
#include "io.h"
#include "fcntl.h"
#ifdef __WINDOWS__
#define TRMEM_NO_STDOUT
#include <windows.h>
#endif
#ifdef __OS2__
#define TRMEM_NO_STDOUT
#define INCL_PM
#include <os2.h>
#endif

static _trmem_hdl TrHdl;
int TrFileHandle;
unsigned NumMessages = 0;

#pragma initialize 40;

struct Memory
{
        Memory();
        ~Memory();
};

static Memory bogus;    // just need to get the ctor's called

void PrintLine( void *parm, const char *buf, size_t len )
{
    parm = parm;
    write( TrFileHandle, (void *) buf, (unsigned int) len );
    NumMessages++;
}
#endif

void *operator new( size_t size )
/*******************************/
{
    void *p;
#ifdef TRACKER
    _trmem_who  caller;

    caller = _trmem_guess_who();
#endif

//  for(;;) {
#ifdef TRACKER
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
#ifdef TRACKER
    _trmem_who  caller;

    caller = _trmem_guess_who();
#endif

//    for(;;) {
#ifdef TRACKER
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
#ifdef TRACKER
    _trmem_who  caller;

    caller = _trmem_guess_who();
#endif

//    for(;;) {
#ifdef TRACKER
        p = _trmem_realloc( p, size, caller, TrHdl );
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
    if( p == NULL ) return;
#ifdef TRACKER
    _trmem_free( p, _trmem_guess_who(), TrHdl );
#else
    free( p );
#endif
}

};

void operator delete( void *p )
/*****************************/
{
    if( p == NULL ) return;
#ifdef TRACKER
    _trmem_free( p, _trmem_guess_who(), TrHdl );
#else
    free( p );
#endif
}

#ifdef TRACKER
Memory::Memory()
/**************/
{
    TrHdl = _trmem_open( malloc, free, realloc, NULL, NULL, PrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 | _TRMEM_REALLOC_NULL |
            _TRMEM_FREE_NULL | _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#ifdef TRMEM_NO_STDOUT
    TrFileHandle = open( "c:\\tmp\\tracker.txt",
                                O_RDWR | O_CREAT | O_TRUNC | O_TEXT, 0 );
#else
    TrFileHandle = STDOUT_FILENO;
#endif
}

Memory::~Memory()
{
    _trmem_prt_list( TrHdl );
    _trmem_close( TrHdl );
#ifdef __WINDOWS__
    close( TrFileHandle );
    if( NumMessages > 1 ) {
        MessageBox ( NULL, "memory problems detected", "Memory Tracker",
                     MB_ICONINFORMATION | MB_OK | MB_TASKMODAL );
    }
#endif
#ifdef __OS2__
    close( TrFileHandle );
    if( NumMessages > 1 ) {
        WinMessageBox ( HWND_DESKTOP, NULL, "memory problems detected",
                    "Memory Tracker", 0,
                     MB_ICONASTERISK | MB_OK | MB_APPLMODAL );
    }
#endif
}
#endif
