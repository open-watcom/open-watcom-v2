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


#include <windows.h>
#include <malloc.h>
#include "msjutil.h"
#ifdef TRMEM
    #include "trmem.h"
    #include "bool.h"
    #include <stdlib.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <dip.h>
#endif


#if defined( TRMEM ) && defined( _M_IX86 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef TRMEM

static FILE             *TrFile = NULL;
static _trmem_hdl       TrHdl = _TRMEM_HDL_NONE;

/* extern to avoid problems with taking address and overlays */
void PopErrBox( void *buff )
/*********************************/
{
    MessageBox( NULL, buff, "Memory Error", MB_SYSTEMMODAL+MB_OK );
}

static bool Closing = FALSE;
static void TRPrintLine( void *parm, const char *buff, size_t len )
/*****************************************************************/
{
    /* unused parameters */ (void)parm; (void)len;

    if( !Closing )
        PopErrBox( (void *)buff );
    if( TrFile != NULL ) {
        fprintf( TrFile, "%s\n", buff );
    }
}

static void TRMemOpen( void )
/***************************/
{
    TrHdl = _trmem_open( malloc, free, realloc, _TRMEM_NO_STRDUP,
            NULL, TRPrintLine, _TRMEM_DEF );
}

static void TRMemClose( void )
/****************************/
{
    _trmem_close( TrHdl );
}

TRMEMAPI( TRMemAlloc )
static void * TRMemAlloc( size_t size )
/*************************************/
{
    return( _trmem_alloc( size, _TRMEM_WHO( 1 ), TrHdl ) );
}

TRMEMAPI( TRMemFree )
static void TRMemFree( void * ptr )
/*********************************/
{
    _trmem_free( ptr, _TRMEM_WHO( 2 ), TrHdl );
}

TRMEMAPI( TRMemRealloc )
static void * TRMemRealloc( void * ptr, size_t size )
/***************************************************/
{
    return( _trmem_realloc( ptr, size, _TRMEM_WHO( 3 ), TrHdl ) );
}


extern void TRMemPrtUsage( void )
/*******************************/
{
    _trmem_prt_usage( TrHdl );
}

static unsigned TRMemPrtList( void )
/******************************/
{
    return( _trmem_prt_list( TrHdl ) );
}

TRMEMAPI( TRMemValidate )
extern int TRMemValidate( void * ptr )
/************************************/
{
    return( _trmem_validate( ptr, _TRMEM_WHO( 4 ), TrHdl ) );
}

extern void TRMemCheck()
/**********************/
{
    _trmem_validate_all( TrHdl );
}

TRMEMAPI( TRMemChkRange )
extern int TRMemChkRange( void * start, size_t len )
/**************************************************/
{
    return( _trmem_chk_range( start, len, _TRMEM_WHO( 5 ), TrHdl ) );
}

static void MemTrackInit()
{
    TrFile = stderr;
    TrFile = fopen( "track.fil", "wt" );
    TRMemOpen();
}

static char UnFreed[] = { "Memory UnFreed" };
static char TrackErr[] = { "Memory Tracker Errors Detected" };

static void MemTrackFini()
{
    Closing = TRUE;
    if( TrFile != stderr ) {
        if( fseek( TrFile, 0, SEEK_END ) != 0 ) {
            PopErrBox( TrackErr );
        } else if( TRMemPrtList() != 0 ) {
            PopErrBox( UnFreed );
        }
        fclose( TrFile );
        TrFile = NULL;
    }
    TRMemClose();
}


void MSJMemInit()
/**********/
{
    MemTrackInit();
}

void MSJMemFini()
/***************/
{
    MemTrackFini();
}

extern void * MSJAlloc( unsigned size )
/*************************************/
{
    return TRMemAlloc( size );
}

extern void * MSJRealloc( void *ptr, unsigned size )
/**************************************************/
{
    return TRMemRealloc( ptr, size );
}

extern void MSJFree( void *mem )
/******************************/
{
    TRMemFree( mem );
}

#else /* !TRMEM */

void MSJMemInit()
/**********/
{
}

void MSJMemFini()
/***************/
{
}

extern void * MSJAlloc( unsigned size )
/*************************************/
{
    return malloc( size );
}

extern void * MSJRealloc( void *ptr, unsigned size )
/**************************************************/
{
    return realloc( ptr, size );
}

extern void MSJFree( void *mem )
/******************************/
{
    free( mem );
}

#endif /* TRMEM */

extern char * UnicodeToASCII( wchar_t *unicode )
/**********************************************/
{
    char *      ascii;
    unsigned    len;

    len = WideCharToMultiByte( CP_ACP, 0, unicode, -1, NULL, 0, NULL, NULL );
    ascii = MSJAlloc( len );
    WideCharToMultiByte( CP_ACP, 0, unicode, -1, ascii, len, NULL, NULL );
    return ascii;
}
