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


#include "guiwind.h"
#include <malloc.h>
#include <stdio.h>
#include "trmem.h"
#ifdef TRMEM
#include <io.h>
extern _trmem_hdl   TRMemHandle;
static FILE *       TRFileHandle;   /* stream to put output on */
#endif

extern void TRPrintLine( int * handle, const char * buff, size_t len )
/********************************************************************/
{
    handle = handle;
#ifdef TRMEM
    len = len;
    if( TRFileHandle != NULL ) {
        fprintf( TRFileHandle, "%s", buff );
    }
#else
    buff = buff;
    len = len;
#endif
}

extern void TRMemOpen( char * file )
/**********************************/
{
#ifdef TRMEM
    TRFileHandle = fopen( file, "w+t" );
    TRMemHandle = _trmem_open( malloc, free, realloc, _expand,
            &TRFileHandle, TRPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 | _TRMEM_REALLOC_NULL |
            _TRMEM_FREE_NULL | _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#else
    file = file;
#endif
}

extern void TRMemClose( void )
/****************************/
{
#ifdef TRMEM
    int ret;
    _trmem_prt_usage( TRMemHandle );
    if( TRFileHandle != NULL ) {
        ret = fclose( TRFileHandle );
    }
    _trmem_close( TRMemHandle );
#endif
}
