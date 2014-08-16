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
* Description:  Resource compiler memory management routines.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "rctypes.h"
#include "errors.h"
#include "rcmem.h"
#include "rcalloc0.h"
#include "rcalloc1.h"

#ifdef RC_USE_TRMEM
    #include "wio.h"
    #include "trmem.h"
    _trmem_hdl RcMemHandle = NULL;

    static void RcPutLine( void *dummy, const char *buf, size_t len )
    /***************************************************************/
    {
        dummy = dummy;
        write( STDOUT_FILENO, buf, len );
    }
#endif

void RcMemInit( void )
/********************/
{
#ifdef RC_USE_TRMEM
    RcMemHandle = _trmem_open( malloc, free, realloc, _TRMEM_NO_REALLOC,
                        NULL, RcPutLine,
                        _TRMEM_ALLOC_SIZE_0 | _TRMEM_FREE_NULL |
                        _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#else
    RCMemLayer1Init();
#endif
}

void RcMemShutdown( void )
/************************/
{
#ifdef RC_USE_TRMEM
     RcPrintList();
    _trmem_close( RcMemHandle );
#else
     RCMemLayer1ShutDown();
#endif
}

void *RcMemMalloc( size_t size )
/******************************/
{
    void *  ptr;

#ifdef RC_USE_TRMEM
    ptr = _trmem_alloc( size, _trmem_guess_who(), RcMemHandle );
#else
    ptr = RCMemLayer1Malloc( size );
#endif

    if( ptr == NULL ) {
        RcFatalError( ERR_OUT_OF_MEMORY );
    }

    return( ptr );
}

void RcMemFree( void * ptr )
/**************************/
{
#ifdef RC_USE_TRMEM
    _trmem_free( ptr, _trmem_guess_who(), RcMemHandle );
#else
    RCMemLayer1Free( ptr );
#endif
}

void * RcMemRealloc( void * old_ptr, size_t newsize )
/***************************************************/
{
    void *  ptr;

#ifdef RC_USE_TRMEM
    ptr = _trmem_realloc( old_ptr, newsize, _trmem_guess_who(), RcMemHandle );
#else
    ptr = RCMemLayer1Realloc( old_ptr, newsize );
#endif

    if( ptr == NULL && newsize != 0 ) {
        RcFatalError( ERR_OUT_OF_MEMORY );
    }

    return( ptr );
}

#ifdef RC_USE_TRMEM
void RcPrintUsage( void )
/***********************/
{
    if( RcMemHandle != NULL ) {
        _trmem_prt_usage( RcMemHandle );
    }
}

void RcPrintList( void )
/**********************/
{
    if( RcMemHandle != NULL ) {
        _trmem_prt_list( RcMemHandle );
    }
}

int RcMemValidate( void * ptr )
/*****************************/
{
    if( RcMemHandle != NULL ) {
        return( _trmem_validate( ptr, _trmem_guess_who(), RcMemHandle ) );
    } else {
        return( false );
    }
}

int RcMemChkRange( void * start, size_t len )
/*******************************************/
{
    if( RcMemHandle != NULL ) {
        return( _trmem_chk_range( start, len,
                        _trmem_guess_who(), RcMemHandle ) );
    } else {
        return( false );
    }
}
#endif
