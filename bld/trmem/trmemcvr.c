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


#ifdef __QNX__
#include <unistd.h>
#else
#include <io.h>
#endif
#include <malloc.h>
#include "trmem.h"

#ifdef TRMEM
static _trmem_hdl   TRMemHandle;
static int          TRFileHandle;   /* stream to put output on */
static void TRPrintLine( int *, const char * buff, size_t len );
#endif

#ifdef NLM
/* There is no equivalent expand function in NetWare. */
#define _expand NULL
#endif

extern void TRMemRedirect( int handle )
/*************************************/
{
    handle=handle;
#ifdef TRMEM
    TRFileHandle = handle;
#endif
}

extern void TRMemOpen( void )
/***************************/
{
#ifdef TRMEM
    #ifdef NLM
        TRFileHandle = STDERR_HANDLE;
    #else
        TRFileHandle = STDERR_FILENO;
    #endif
    TRMemHandle = _trmem_open( malloc, free, realloc, _expand,
            &TRFileHandle, TRPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}

extern void TRMemClose( void )
/****************************/
{
#ifdef TRMEM
    _trmem_close( TRMemHandle );
#endif
}

extern void * TRMemAlloc( size_t size )
/*************************************/
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

extern void TRMemFree( void * ptr )
/*********************************/
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
#else
    free( ptr );
#endif
}

extern void * TRMemRealloc( void * ptr, size_t size )
/***************************************************/
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

#ifdef TRMEM

extern void TRMemPrtUsage( void )
/*******************************/
{
    _trmem_prt_usage( TRMemHandle );
}

extern unsigned TRMemPrtList( void )
/******************************/
{
    return( _trmem_prt_list( TRMemHandle ) );
}

extern int TRMemValidate( void * ptr )
/************************************/
{
    return( _trmem_validate( ptr, _trmem_guess_who(), TRMemHandle ) );
}

extern int TRMemChkRange( void * start, size_t len )
/**************************************************/
{
    return( _trmem_chk_range( start, len, _trmem_guess_who(), TRMemHandle ) );
}

/* extern to avoid problems with taking address and overlays */
extern void TRPrintLine( int * handle, const char * buff, size_t len )
/********************************************************************/
{
    write( *handle, buff, len );
}

#endif
