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
* Description:  DOS memory Swap handling
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <i86.h>
#include "bool.h"
#include "tinyio.h"
#include "vi.h"
#include "fcbmem.h"
#include "tempio.h"
#include "doschkx.h"


#define SPAWN_FILE_NAME     "SWXXXXXX"

#define TINY_HANDLE_NULL    ((tiny_handle_t)-1)

static char                 *fullName = NULL;

#if defined( USE_XMEM )

#define MAX_IO_BUFFER   0x2000  // 8 kB EMS/XMS page

static unsigned short   *xSize = NULL;
static xhandle          *xHandle = NULL;
static unsigned short   currMem = 0;
static int              chkSwapSize = 0;

#endif

void XchkDeleteFile( void )
{
    TinyDelete( fullName );
    fullName = NULL;
}

tiny_handle_t XchkOpenFile( char *f_buff )
{
    tiny_ret_t      rc;
    tiny_handle_t   filehandle;

    filehandle = TINY_HANDLE_NULL;
    if( f_buff != NULL ) {
        fullName = f_buff;
        MakeTmpPath( f_buff, SPAWN_FILE_NAME );
        filehandle = mkstemp( f_buff );
        if( filehandle == TINY_HANDLE_NULL ) {
            fullName = NULL;
        }
    } else {
        filehandle = TINY_HANDLE_NULL;
        if( fullName != NULL ) {
            rc = TinyOpen( fullName, TIO_READ );
            if( TINY_OK( rc ) ) {
                filehandle = TINY_INFO( rc );
            }
        }
    }
    return( filehandle );
}

#if defined( USE_XMEM )

static void memGiveBack( void (*rtn)(xhandle) )
{
    int i;

    for( i = 0; i < chkSwapSize; i++ ) {
        rtn( xHandle[i] );
    }
}

static bool memBlockWrite( void (*rtn)(xhandle, void*, unsigned), __segment buff, unsigned *size )
{
    unsigned    bytes;

    if( *size > MAX_IO_BUFFER >> 4 ) {
        *size = MAX_IO_BUFFER >> 4;
    }
    bytes = *size << 4;
    rtn( xHandle[currMem], _MK_FP( buff, 0 ), bytes );
    xSize[currMem] = bytes;
    currMem++;
    return( true );
}

static bool memBlockRead( void (*rtn)(xhandle, void*, unsigned), __segment *buff )
{
    rtn( xHandle[currMem], _MK_FP( *buff, 0 ), xSize[currMem] );
    *buff += MAX_IO_BUFFER >> 4;
    if( xSize[currMem] < MAX_IO_BUFFER ) {
        return( false );
    }
    currMem++;
    return( true );
}

void XSwapInit( int count, xhandle *handles, unsigned short *sizes )
{
    chkSwapSize = count;
    xHandle = handles;
    xSize = sizes;
}

void XMemCleanUp( where_parm where )
{
  #if defined( USE_XMS )
    if( where == IN_XMS ) {
        memGiveBack( &GiveBackXMSBlock );
    }
  #endif
  #if defined( USE_EMS )
    if( where == IN_EMS ) {
        memGiveBack( &GiveBackEMSBlock );
    }
  #endif
}

bool XMemChkOpen( where_parm where )
{
  #if defined( USE_EMS )
    if( where == IN_XMS ) {
        currMem = 0;
    }
  #endif
  #if defined( USE_XMS )
    if( where == IN_EMS ) {
        currMem = 0;
    }
  #endif
    return( true );
}

bool XMemChkWrite( where_parm where, __segment buff, unsigned *size )
{
  #if defined( USE_EMS )
    if( where == IN_XMS ) {
        return( memBlockWrite( EMSBlockWrite, buff, size ) );
    }
  #endif
  #if defined( USE_XMS )
    if( where == IN_EMS ) {
        return( memBlockWrite( XMSBlockWrite, buff, size ) );
    }
  #endif
    return( true );
}

bool XMemChkRead( where_parm where, __segment *buff )
{
  #if defined( USE_EMS )
    if( where == IN_XMS ) {
        return( memBlockRead( EMSBlockRead, buff ) );
    }
  #endif
  #if defined( USE_XMS )
    if( where == IN_EMS ) {
        return( memBlockRead( XMSBlockRead, buff ) );
    }
  #endif
    return( true );
}

#endif
