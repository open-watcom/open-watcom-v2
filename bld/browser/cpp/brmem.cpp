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
 * this code needs to be compiled with C++ compiler -of+ option
 *
 * appropriate C code use #pragma aux ... __frame to do same
 * for some unknown reason this doesn't work for C++ compiler now
 */
#include <stdlib.h>
#include <string.h>
#ifndef STANDALONE_MERGER
    #include <dr.h>
#endif
#include "death.h"
#include "brmem.h"
#include "debuglog.h"
#ifdef TRMEM
    #include "trmem.h"
#endif


#ifdef TRMEM

#ifdef __WATCOMC__
#pragma initialize 40;
#endif

#ifndef MINALLOC
#define MINALLOC 0
#endif

#ifndef STANDALONE_MERGER
#include <wmsgdlg.hpp>
#endif

_trmem_hdl TrHdl;

class Memory : public DebuggingLog
{
    public:
        Memory();
        ~Memory();

        unsigned    _numMessages;
};

static Memory bogus;        // just need to get the ctor's called

void PrintLine( void *parm, const char *buf, size_t len )
{
    /* unused parameters */ (void)parm; (void)len;

    bogus.printf( "%s\n", buf );
    bogus._numMessages++;
}

#endif  // TRMEM

void *operator new( size_t size )
//-------------------------------
{
    void *p;

#ifndef STANDALONE_MERGER
    for(;;) {
#endif
#ifdef TRMEM
        p = _trmem_alloc( size, _trmem_guess_who(), TrHdl );
#else
        p = malloc( size );
#endif
#ifndef STANDALONE_MERGER
        if( p != NULL || size == 0 )    // successful allocation
            break;
        if( !DRSwap() ) {               // dwarf managed to swap out, so try again
            break;
        }
    }
#endif
    if( p == NULL && size != 0 )
        throw DEATH_BY_OUT_OF_MEMORY;
    return p;
}

extern "C" {

void * WBRAlloc( size_t size )
//----------------------------
// note: code directly cloned from above since we need to be able to trace
// calling functions when the memory tracker is in.
{
    void *p;

#ifndef STANDALONE_MERGER
    for(;;) {
#endif
#ifdef TRMEM
        p = _trmem_alloc( size, _trmem_guess_who(), TrHdl );
#else
        p = malloc( size );
#endif
#ifndef STANDALONE_MERGER
        if( p != NULL || size == 0 )    // successful allocation
            break;
        if( !DRSwap() ) {               // dwarf managed to swap out, so try again
            break;
        }
    }
#endif
    if( p == NULL && size != 0 )
        throw DEATH_BY_OUT_OF_MEMORY;
    return p;
}

char * WBRStrdup( const char *str )
//---------------------------------
// note: code directly cloned from above since we need to be able to trace
// calling functions when the memory tracker is in.
{
    char *p;

#ifndef STANDALONE_MERGER
    for(;;) {
#endif
#ifdef TRMEM
        p = _trmem_strdup( str, _trmem_guess_who(), TrHdl );
#else
        p = strdup( str );
#endif
#ifndef STANDALONE_MERGER
        if( p != NULL )             // successful allocation
            break;
        if( !DRSwap() ) {           // dwarf managed to swap out, so try again
            break;
        }
    }
#endif
    if( p == NULL )
        throw DEATH_BY_OUT_OF_MEMORY;
    return p;
}

void * WBRRealloc( void * p, size_t size )
//----------------------------------------
// note: code cloned from above since we need to be able to trace
// calling functions when the memory tracker is in.
{
#ifndef STANDALONE_MERGER
    for(;;) {
#endif
#ifdef TRMEM
        p = _trmem_realloc( p, size, _trmem_guess_who(), TrHdl );
#else
        p = realloc( p, size );
#endif
#ifndef STANDALONE_MERGER
        if( p != NULL || size == 0 )    // successful allocation
            break;
        if( !DRSwap() ) {               // dwarf managed to swap out, so try again
            break;
        }
    }
#endif
    if( p == NULL && size != 0 )
        throw DEATH_BY_OUT_OF_MEMORY;
    return p;
}

void WBRFree( void *p )
//---------------------
{
    if( p == NULL )
        return;
#ifdef TRMEM
    _trmem_free( p, _trmem_guess_who(), TrHdl );
#else
    free( p );
#endif
}

#if 1
#ifndef STANDALONE_MERGER
void *GUIMemAlloc( unsigned a )
//--------------------------
{
    return WBRAlloc( a );
}

void *GUIMemStrdup( const char *a )
//--------------------------
{
    return strdup( a );
}

void *GUIMemRealloc( void *ptr, unsigned size )
//------------------------------------------
{
    return WBRRealloc( ptr, size );
}

void GUIMemFree( void *p )
//---------------------
{
    WBRFree( p );
}

void *MemAlloc( unsigned a )
//--------------------------
{
    return WBRAlloc( a );
}

void *MemAllocSafe( unsigned a )
//------------------------------
{
    return WBRAlloc( a );
}

char *MemStrdup( const char *a )
//--------------------------
{
    return strdup( a );
}

void *MemRealloc( void *ptr, unsigned size )
//------------------------------------------
{
    return WBRRealloc( ptr, size );
}

void MemFree( void *p )
//---------------------
{
    WBRFree( p );
}

void *_wpi_malloc( unsigned a )
//----------------------------
{
    return WBRAlloc( a );
}

void _wpi_free( void *p )
//-----------------------
{
    WBRFree( p );
}


void GUIMemOpen( void )
{
}

void GUIMemClose( void )
{
}

void GUIMemPrtUsage( void )
{
}
#endif  // STANDALONE_MERGER
#endif  // 0

}; /* extern "C" */


void operator delete( void *p )
//-----------------------------
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

static void GetOffset()
//-------------------------
{
    GetOffset();
    // get an offset of a function to match to map file, and make sure it's referenced
}

Memory::Memory()
        : DebuggingLog( "memory" )
        , _numMessages( 0 )
//--------------------------------
{
    DebuggingLog::printf( "Address of GetOffset is %#p\n\n", &GetOffset );
    TrHdl = _trmem_open( malloc, free, realloc, strdup,
            NULL, PrintLine, _TRMEM_DEF );

    _trmem_set_min_alloc( MINALLOC, TrHdl );

#ifdef __WINDOWS__
    _trmem_prt_use_seg_num( TrHdl, true );
#endif
}

Memory::~Memory()
//---------------
{
    _trmem_prt_list( TrHdl );
    _trmem_close( TrHdl );
    if( _numMessages > 1 ) {
#ifdef __WINDOWS__
        WMessageDialog::message( NULL, MsgWarning, MsgOk,
                         "Memory problems detected", "Browser Memory Tracker" );
#else
        fprintf( stderr, "Memory problems detected" );
#endif
    }
}

#endif /* TRMEM */
