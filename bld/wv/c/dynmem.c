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
* Description:  Dynamic memory management routines for debugger.
*
****************************************************************************/


#include "dbglit.h"
#ifdef __WATCOMC__
 /* it's important that <malloc> is included up here */
 #define __fmemneed foo
 #define __nmemneed bar
 #include <malloc.h>
 #undef __nmemneed
 #undef __fmemneed
#endif
#include "dbgdefn.h"
#include "dbgerr.h"
#ifdef TRMEM
 #include "trmem.h"
 #include "dui.h"
 #include <unistd.h>
 #include <fcntl.h>
#else
 #include "dbgio.h"
 #include "dui.h"
 #define TRMemAlloc(x)          malloc(x)
 #define TRMemRealloc(p,x)      realloc(p,x)
 #define TRMemFree(p)           free(p)
#endif
#include <stdlib.h>
#include <stdio.h>
#include <dip.h>



extern bool     VarInfoRelease( void );
extern bool     DlgInfoRelease( void );
extern char     *Format( char *, char *, ... );
extern int      DUIEnvLkup( char *, char *, int );
extern void     PopErrBox( char * );


#ifdef TRMEM
static  int             TrackFile;
static _trmem_hdl       TRMemHandle;


/* extern to avoid problems with taking address and overlays */
static bool Closing = FALSE;

static void TRPrintLine( int * handle, const char * buff, size_t len )
/********************************************************************/
{
    handle = handle;
    len = len;
    if( !Closing ) PopErrBox( (void*)buff );
    write( TrackFile, buff, len );
}

static void TRMemOpen( void )
/***************************/
{
    TRMemHandle = _trmem_open( malloc, free, realloc, _expand,
            NULL, TRPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
}

static void TRMemClose( void )
/****************************/
{
    _trmem_close( TRMemHandle );
}

static void * TRMemAlloc( size_t size )
/*************************************/
{
    return( _trmem_alloc( size, _trmem_guess_who(), TRMemHandle ) );
}

static void TRMemFree( void * ptr )
/*********************************/
{
    _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
}

static void * TRMemRealloc( void * ptr, size_t size )
/***************************************************/
{
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), TRMemHandle ) );
}


extern void TRMemPrtUsage( void )
/*******************************/
{
    _trmem_prt_usage( TRMemHandle );
}

static unsigned TRMemPrtList( void )
/**********************************/
{
    return( _trmem_prt_list( TRMemHandle ) );
}

extern int TRMemValidate( void * ptr )
/************************************/
{
    return( _trmem_validate( ptr, _trmem_guess_who(), TRMemHandle ) );
}

extern void TRMemCheck( void )
/****************************/
{
    _trmem_validate_all( TRMemHandle );
}

extern int TRMemChkRange( void * start, size_t len )
/**************************************************/
{
    return( _trmem_chk_range( start, len, _trmem_guess_who(), TRMemHandle ) );
}

static void MemTrackInit( void )
{
    char        name[FILENAME_MAX];

    TrackFile = STDERR_FILENO;
    if( DUIEnvLkup( "TRMEMFILE", name, sizeof( name ) ) ) {
        TrackFile = open( name, O_CREAT+O_RDWR+O_TEXT+O_TRUNC );
    }
    TRMemOpen();
}

static char UnFreed[] = { "Memory UnFreed" };
static char TrackErr[] = { "Memory Tracker Errors Detected" };

static void MemTrackFini( void )
{
    Closing = TRUE;
    if( TrackFile != STDERR_FILENO ) {
        if( lseek( TrackFile, 0, SEEK_END ) != 0 ) {
            PopErrBox( TrackErr );
        } else if( TRMemPrtList() != 0 ) {
            PopErrBox( UnFreed );
        }
        close( TrackFile );
    }
    TRMemClose();
}
#endif


/*
 * Dynamic Memory management routines
 */

#ifdef __WATCOMC__
#ifdef __386__
#define __fmemneed __nmemneed
#endif

int __saveregs __fmemneed( size_t size )
{
    if( DIPMoreMem( size ) == DS_OK ) return( TRUE );
    if( DUIInfoRelease() ) return( TRUE );
    return( FALSE );
}
#endif

void *DbgAlloc( unsigned size )
{
    return( TRMemAlloc( size ) );
}


void *DbgMustAlloc( unsigned size )
{
    void        *ptr;

    ptr = DbgAlloc( size );
    if( ptr == NULL ) {
        Error( ERR_NONE, LIT( ERR_NO_MEMORY ) );
    }
    return( ptr );
}

void *DbgRealloc( void *chunk, unsigned size )
{
    return( TRMemRealloc( chunk, size ) );
}


void DbgFree( void *ptr )
{
    if( ptr != NULL ) TRMemFree( ptr );
}

void *ChkAlloc( unsigned size, char *error )
{
    void *ret;

    ret = TRMemAlloc( size );
    if( ret == NULL ) Error( ERR_NONE, error );
    return( ret );
}

#define Heap_Corupt     "ERROR - Heap is corrupted - %s"

void MemFini( void )
{
#ifdef TRMEM
    MemTrackFini();
#elif defined( __WATCOMC__ )

    struct _heapinfo    h_info;
    int                 status;
    char                buf[50];
    char                *end;

    if( getenv( "TRMEMFILE" ) == NULL ) return;
    h_info._pentry = NULL;
    for( ;; ) {
        status = _heapwalk( &h_info );
        if( status != _HEAPOK ) break;
#ifdef DBG_DBG
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

#ifndef _OVERLAYED_

extern unsigned long MemSize;
#if defined( _M_I86 )
#define MAX_BLOCK (60U * 1024)
#elif defined( __DOS__ )
#define MAX_BLOCK (4U*1024*1024)
#else
#define MAX_BLOCK (1U*1024*1024)
#endif


void MemExpand( void )
{
    unsigned long   size;
    void            **link;
    void            **p;
    unsigned        alloced;

    if( MemSize == ~0 ) return;
    link = NULL;
    size = MemSize;
    for( ;; ) {
        if( size == 0 ) break;
        alloced = min( MAX_BLOCK, size );
        p = TRMemAlloc( alloced );
        if( p != NULL ) {
            *p = link;
            link = p;
        }
        size -= alloced;
    }
    while( link != NULL ) {
        p = *link;
        TRMemFree( link );
        link = p;
    }
}

void MemInit( void )
{
#ifdef TRMEM
    MemTrackInit();
#endif
    MemExpand();
}

void *ExtraAlloc( unsigned size )
{
    return( TRMemAlloc( size ) );
}


void *ExtraRealloc( void *p, unsigned size )
{
    return( TRMemRealloc( p, size ) );
}


void ExtraFree( void *ptr )
{
    if( ptr != NULL ) TRMemFree( ptr );
}
#endif
