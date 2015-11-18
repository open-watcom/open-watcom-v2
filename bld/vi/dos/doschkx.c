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
* Description:  Check DOS memory blocks for consistency.
*
****************************************************************************/


#include <stdlib.h>
#include <i86.h>
#include "vi.h"
#include "tinyio.h"
#include "fcbmem.h"
#include "doschk.h"
#include "doschkx.h"


#define FILE_BUFFER_SIZE    0x8000

#define SPAWN_FILE_NAME    "SWXXXXXX"

static char             *fullName = NULL;
static int              fileHandle = -1;

#if defined( USE_XMS ) || defined( USE_EMS )

static unsigned short   *xSize = NULL;
static long             *xHandle = NULL;
static unsigned short   currMem = 0;
static int              chkSwapSize = 0;

static void memGiveBack( void (*rtn)( long ) )
{
    int i;

    for( i = 0; i < chkSwapSize; i++ ) {
        rtn( xHandle[i] );
    }
}

static void memBlockWrite( void (*rtn)(long, void*, unsigned), __segment buff, unsigned *size )
{
    unsigned    bytes;

    if( *size >= 0x0200 ) {
        *size = 0x0200;
    }
    bytes = *size << 4;
    rtn( xHandle[currMem], MK_FP( buff, 0 ), bytes );
    xSize[currMem] = bytes;
    currMem++;
}

static bool memBlockRead( void (*rtn)(long, void*, unsigned), __segment *buff )
{
    rtn( xHandle[currMem], MK_FP( *buff, 0 ), xSize[currMem] );
    *buff += 0x200;
    if( xSize[currMem] < MAX_IO_BUFFER ) {
        return( false );
    }
    currMem++;
    return( true );
}

void XSwapInit( int count, long *handles, unsigned short *sizes )
{
    chkSwapSize = count;
    xHandle = handles;
    xSize = sizes;
}

#endif

void XcleanUp( where_parm where )
{
    switch( where ) {
    case ON_DISK:
        TinyClose( fileHandle );
        fileHandle = -1;
        TinyDelete( fullName );
        fullName = NULL;
        break;
#if defined( USE_XMS )
    case IN_XMS:
        memGiveBack( &GiveBackXMSBlock );
        break;
#endif
#if defined( USE_EMS )
    case IN_EMS:
        memGiveBack( &GiveBackEMSBlock );
        break;
#endif
    }
}

bool XchkOpen( where_parm where, char *f_buff )
{
    tiny_ret_t      rc;

    switch( where ) {
    case ON_DISK:
        if( f_buff != NULL ) {
            fullName = f_buff;
            MakeTmpPath( f_buff, SPAWN_FILE_NAME );
            fileHandle = mkstemp( f_buff );
            if( fileHandle == -1 ) {
                fullName = NULL;
            }
        } else {
            fileHandle = -1;
            if( fullName != NULL ) {
                rc = TinyOpen( fullName, TIO_READ );
                if( TINY_OK( rc ) ) {
                    fileHandle = TINY_INFO( rc );
                }
            }
        }
        return( fileHandle != -1 );
#if defined( USE_EMS )
    case IN_EMS:
        currMem = 0;
        break;
#endif
#if defined( USE_XMS )
    case IN_XMS:
        currMem = 0;
        break;
#endif
    }
    return( true );
}

void XchkClose( where_parm where )
{
    switch( where ) {
    case ON_DISK:
        TinyClose( fileHandle );
        fileHandle = -1;
        break;
    }
}

bool XchkWrite( where_parm where, __segment buff, unsigned *size )
{
    tiny_ret_t      rc;
    unsigned        bytes;

    switch( where ) {
    case ON_DISK:
        if( *size >= 0x1000 ) {
            *size = FILE_BUFFER_SIZE >> 4;
        }
        bytes = *size << 4;
        rc = TinyFarWrite( fileHandle, MK_FP( buff, 0 ), bytes );
        return( TINY_OK( rc ) && TINY_INFO( rc ) == bytes );
#if defined( USE_EMS )
    case IN_EMS:
        memBlockWrite( EMSBlockWrite, buff, size );
        break;
#endif
#if defined( USE_XMS )
    case IN_XMS:
        memBlockWrite( XMSBlockWrite, buff, size );
        break;
#endif
    }
    return( true );
}

bool XchkRead( where_parm where, __segment *buff )
{
    tiny_ret_t      rc;

    switch( where ) {
    case ON_DISK:
        rc = TinyFarRead( fileHandle, MK_FP( *buff, 0 ), FILE_BUFFER_SIZE );
        if( TINY_ERROR( rc ) || TINY_INFO( rc ) != FILE_BUFFER_SIZE ) {
            return( false );
        }
        *buff += FILE_BUFFER_SIZE >> 4;
        break;
#if defined( USE_EMS )
    case IN_EMS:
        return( memBlockRead( EMSBlockRead, buff ) );
#endif
#if defined( USE_XMS )
    case IN_XMS:
        return( memBlockRead( XMSBlockRead, buff ) );
#endif
    }
    return( true );
}
