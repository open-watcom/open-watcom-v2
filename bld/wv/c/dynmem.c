/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Dynamic memory management routines for debugger.
*
****************************************************************************/


#include "dbglit.h"
#include <stdlib.h>
#ifdef _M_IX86
    #include <i86.h>
#endif
#ifdef __WATCOMC__
    /* it's important that <malloc> is included up here */
    #define __fmemneed foo
    #define __nmemneed bar
    #include <malloc.h>
    #undef __nmemneed
    #undef __fmemneed
#endif
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbgio.h"
#include "dui.h"
#include "dip.h"
#include "strutil.h"
#include "dbginit.h"
#ifndef __NOUI__
    #include "aui.h"
    #include "guimem.h"
    #include "wresmem.h"
    #ifdef GUI_IS_GUI
        #include "cguimem.h"
        #include "wpimem.h"
        #ifdef __OS2__
            #include "os2mem.h"
        #endif
    #else
        #include "stdui.h"
        #include "helpmem.h"
    #endif
#endif
#ifdef TRMEM
    #include "trmem.h"
#endif


#define _60kB   (60UL * 1024UL)
#define _1MB    (1024UL * 1024UL)
#define _4MB    (4UL * 1024UL * 1024UL)

#ifdef __DOS__
    #define MEM_NEAR_PTR(x)     (void *)_FP_OFF( x )
#else
    #define MEM_NEAR_PTR(x)     x
#endif

#ifdef __DOS__
extern int _d16ReserveExt( int );
#pragma aux _d16ReserveExt = \
        "mov cx,ax" \
        "shr eax,16" \
        "mov bx,ax" \
        "mov dx,1400H" \
        "mov ax,0ff00H" \
        "int 21H" \
        "ror eax,16" \
        "mov ax,dx" \
        "ror eax,16" \
    __parm      [__eax] \
    __value     [__eax] \
    __modify    [__ebx __ecx __edx]
#endif

#ifdef TRMEM
static FILE             *TrackFile = NULL;   /* stream to put output on */
static _trmem_hdl       DbgMemHandle;
#endif

#ifdef __WATCOMC__
  #ifdef _M_I86
    #define xmemneed    __fmemneed
  #else
    #define xmemneed    __nmemneed
  #endif
extern int __saveregs   xmemneed( size_t size );

int __saveregs  xmemneed( size_t size )
{
    if( DIPMoreMem( size ) == DS_OK )
        return( true );
    if( DUIInfoRelease() )
        return( true );
    return( false );
}
#endif


/*
 * Dynamic Memory management routines
 */

void *DbgAlloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

void *DbgMustAlloc( size_t size )
{
    void        *ptr;

#ifdef TRMEM
    ptr = _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle );
#else
    ptr = malloc( size );
#endif
    if( ptr == NULL ) {
        Error( ERR_NONE, LIT_ENG( ERR_NO_MEMORY ) );
    }
    return( ptr );
}

void *DbgRealloc( void *chunk, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( chunk, size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( realloc( chunk, size ) );
#endif
}

void DbgFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), DbgMemHandle );
#else
    free( ptr );
#endif
}

void *DbgChkAlloc( size_t size, char *error )
{
    void *ret;

#ifdef TRMEM
    ret = _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle );
#else
    ret = malloc( size );
#endif
    if( ret == NULL )
        Error( ERR_NONE, error );
    return( ret );
}

#if defined( __DOS__ ) || defined( __NOUI__ )

#if defined( _M_I86 )
    #define MAX_BLOCK   _60kB
#elif defined( __DOS__ )
    #define MAX_BLOCK   _4MB
#else
    #define MAX_BLOCK   _1MB
#endif

static void MemExpand( void )
{
    unsigned long   size;
    void            **link;
    void            **p;
    size_t          alloced;

    if( MemSize == ~0 )
        return;
    link = NULL;
    alloced = MAX_BLOCK;
    for( size = MemSize; size > 0; size -= alloced ) {
        if( size < MAX_BLOCK )
            alloced = size;
#ifdef TRMEM
        p = _trmem_alloc( alloced, _trmem_guess_who(), DbgMemHandle );
#else
        p = malloc( alloced );
#endif
        if( p != NULL ) {
            *p = link;
            link = p;
        }
    }
    while( link != NULL ) {
        p = *link;
#ifdef TRMEM
        _trmem_free( link, _trmem_guess_who(), DbgMemHandle );
#else
        free( link );
#endif
        link = p;
    }
}

#endif  /* defined( __DOS__ ) || defined( __NOUI__ ) */

void SysSetMemLimit( void )
{
#ifdef  __DOS__
    _d16ReserveExt( MemSize + _1MB );
    MemExpand();
    if( _IsOff( SW_REMOTE_LINK ) && _IsOff( SW_KEEP_HEAP_ENABLED ) ) {
        _heapenable( 0 );
    }
#endif
}

#ifdef __NOUI__

#ifdef TRMEM

static const char   UnFreed[] = { "Memory UnFreed" };
static const char   TrackErr[] = { "Memory Tracker Errors Detected" };
static bool         Closing = false;

static void DbgMemPrintLine( void *parm, const char *buff, size_t len )
/*********************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( !Closing )
        PopErrBox( buff );
    if( TrackFile != NULL ) {
        fprintf( TrackFile, "%s\n", buff );
    }
}

static unsigned DbgMemPrtList( void )
/***********************************/
{
    return( _trmem_prt_list( DbgMemHandle ) );
}

static void DbgMemOpen( void )
/****************************/
{
    DbgMemHandle = _trmem_open( malloc, free, realloc, NULL,
            NULL, DbgMemPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
}

static void DbgMemClose( void )
/*****************************/
{
    _trmem_prt_list( DbgMemHandle );
    _trmem_close( DbgMemHandle );
}

static void MemTrackInit( void )
{
    char        name[FILENAME_MAX];

    TrackFile = stderr;
    if( DUIEnvLkup( "TRMEMFILE", name, sizeof( name ) ) ) {
        TrackFile = fopen( name, "w" );
    }
    DbgMemOpen();
}

static void MemTrackFini( void )
{
    Closing = true;
    if( TrackFile != stderr ) {
        fseek( TrackFile, 0, SEEK_END );
        if( ftell( TrackFile ) != 0 ) {
            PopErrBox( TrackErr );
        } else if( DbgMemPrtList() != 0 ) {
            PopErrBox( UnFreed );
        }
        fclose( TrackFile );
        TrackFile = NULL;
    }
    DbgMemClose();
}

#endif  /* TRMEM */

void MemInit( void )
{
#ifdef TRMEM
    MemTrackInit();
#endif
    MemExpand();
}

void MemFini( void )
{
#ifdef TRMEM
    MemTrackFini();
#elif defined( __WATCOMC__ )
    static const char   Heap_Corupt[] = { "ERROR - Heap is corrupted - %s" };
    struct _heapinfo    h_info;
    int                 status;
    char                buf[50];
    char                *end;

    if( getenv( "TRMEMFILE" ) == NULL )
        return;
    h_info._pentry = NULL;
    while( (status = _heapwalk( &h_info )) == _HEAPOK ) {
  #ifndef NDEBUG
        if( h_info._useflag == _USEDENTRY ) {
            end = Format( buf, "%s block",
                h_info._useflag == _USEDENTRY ? "Used" : "Free" );
            WriteText( STD_OUT, buf, end - buf );
        }
  #endif
    }
    switch( status ) {
    case _HEAPBADBEGIN:
        end = Format( buf, Heap_Corupt, "bad header info" );
        WriteText( STD_OUT, buf, end - buf );
        break;
    case _HEAPBADPTR:
        end = Format( buf, Heap_Corupt, "bad pointer" );
        WriteText( STD_OUT, buf, end - buf );
        break;
    case _HEAPBADNODE:
        end = Format( buf, Heap_Corupt, "bad node" );
        WriteText( STD_OUT, buf, end - buf );
        break;
    default:
        break;
    }
#endif
}

#else   /* !defined( __NOUI__ ) */

#ifdef TRMEM

static int  GUIMemOpened = 0;

static void GUIMemPrintLine( void *parm, const char *buff, size_t len )
/*********************************************************************/
{
    /* unused parameters */ (void)parm;

    if( TrackFile != NULL && len > 0 ) {
        fprintf( TrackFile, "%s\n", buff );
    }
}

#endif  /* TRMEM */

void WndNoMemory( void )
{
    Error( ERR_NONE, LIT_ENG( ERR_NO_MEMORY_FOR_WINDOW ) );
}

void GUIMemPrtUsage( void )
/*************************/
{
#ifdef TRMEM
    _trmem_prt_usage( DbgMemHandle );
#endif
}

void GUIMemRedirect( FILE *fp )
/*****************************/
{
#ifdef TRMEM
    TrackFile = fp;
#else
    /* unused parameters */ (void)fp;
#endif
}

void GUIMemOpen( void )
/*********************/
{
#ifdef TRMEM
    char * tmpdir;

    if( !GUIMemOpened ) {
        TrackFile = stderr;
        DbgMemHandle = _trmem_open( malloc, free, realloc, NULL,
            NULL, GUIMemPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );

        tmpdir = getenv( "TRMEMFILE" );
        if( tmpdir != NULL ) {
            TrackFile = fopen( tmpdir, "w" );
        }
        GUIMemOpened = 1;
    }
#endif
}
#ifndef GUI_IS_GUI
void UIAPI UIMemOpen( void ) {}
#endif

void GUIMemClose( void )
/**********************/
{
#ifdef TRMEM
    _trmem_prt_list( DbgMemHandle );
    _trmem_close( DbgMemHandle );
    if( TrackFile != stderr ) {
        fclose( TrackFile );
        TrackFile = NULL;
    }
#endif
}
#ifndef GUI_IS_GUI
void UIAPI UIMemClose( void ) {}
#endif


/*
 * Alloc functions
 */

void *GUIMemAlloc( size_t size )
/******************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
void *WndAlloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

#ifdef GUI_IS_GUI

void *MemAlloc( size_t size )
{
    void        *ptr;

#ifdef TRMEM
    ptr = _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle );
#else
    ptr = malloc( size );
#endif
    memset( ptr, 0, size );
    return( ptr );
}

#ifdef __OS2__

void * _wpi_malloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
void *PMmalloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

#endif  /* __OS2__ */

#else   /* GUI_IS_GUI */

void * UIAPI uimalloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
LP_VOID UIAPI uifaralloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
void *HelpMemAlloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( malloc( size ) );
#endif
}
void *wres_alloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

#endif  /* ! GUI_IS_GUI */


/*
 * Free functions
 */

void GUIMemFree( void *ptr )
/**************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), DbgMemHandle );
#else
    free( ptr );
#endif
}
void WndFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), DbgMemHandle );
#else
    free( ptr );
#endif
}

#ifdef GUI_IS_GUI

void MemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), DbgMemHandle );
#else
    free( ptr );
#endif
}

#ifdef __OS2__

void _wpi_free( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), DbgMemHandle );
#else
    free( ptr );
#endif
}
void PMfree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), DbgMemHandle );
#else
    free( ptr );
#endif
}

#endif  /* __OS2__ */

#else   /* GUI_IS_GUI */

void UIAPI uifree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), DbgMemHandle );
#else
    free( ptr );
#endif
}
void UIAPI uifarfree( LP_VOID ptr )
{
    if( ptr != NULL ) {
#ifdef TRMEM
        _trmem_free( MEM_NEAR_PTR( ptr ), _trmem_guess_who(), DbgMemHandle );
#else
        free( MEM_NEAR_PTR( ptr ) );
#endif
    }
}
void HelpMemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), DbgMemHandle );
#else
    free( ptr );
#endif
}
void wres_free( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), DbgMemHandle );
#else
    free( ptr );
#endif
}

#endif  /* ! GUI_IS_GUI */


/*
 * Realloc functions
 */

void *GUIMemRealloc( void *ptr, size_t size )
/*******************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}
void *WndRealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

#ifdef GUI_IS_GUI

void *MemRealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

#ifdef __OS2__

void * _wpi_realloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}
void *PMrealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

#endif  /* __OS2__ */

#else   /* GUI_IS_GUI */

void * UIAPI uirealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}
void *HelpMemRealloc( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), DbgMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

#endif  /* ! GUI_IS_GUI */

#endif  /* ! __NOUI__ */
