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


#define FILE_BLOCK_SIZE         0x8000
#define FILE_BLOCK_SIZE_PARA    (FILE_BLOCK_SIZE >> 4)
#define MEM_BLOCK_SIZE          0x2000
#define MEM_BLOCK_SIZE_PARA     (MEM_BLOCK_SIZE >> 4)

#define SPAWN_FILE_NAME     "SWXXXXXX"

static char             *fullName = NULL;
static int              fileHandle = -1;

#if defined( USE_XMS ) || defined( USE_EMS )

static unsigned short   *xSize = NULL;
static long             *xHandle = NULL;
static unsigned short   curr_blk = 0;
static int              chkSwapSize = 0;

static void memGiveBack( void (*rtn)( long ) )
{
    int i;

    for( i = 0; i < chkSwapSize; i++ ) {
        rtn( xHandle[i] );
    }
}

static void memBlockWrite( void (*rtn)(long, void __far *, unsigned), __segment blk, unsigned *blk_size )
{
    unsigned    bytes;

    if( *blk_size > MEM_BLOCK_SIZE_PARA ) {
        *blk_size = MEM_BLOCK_SIZE_PARA;
    }
    bytes = *blk_size << 4;
    rtn( xHandle[curr_blk], MK_FP( blk, 0 ), bytes );
    xSize[curr_blk] = bytes;
    curr_blk++;
}

static bool memBlockRead( void (*rtn)(long, void __far *, unsigned), __segment *blk )
{
    rtn( xHandle[curr_blk], MK_FP( *blk, 0 ), xSize[curr_blk] );
    *blk += MEM_BLOCK_SIZE_PARA;
    if( xSize[curr_blk] < MEM_BLOCK_SIZE ) {
        return( false );
    }
    curr_blk++;
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
        curr_blk = 0;
        break;
#endif
#if defined( USE_XMS )
    case IN_XMS:
        curr_blk = 0;
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

bool XchkWrite( where_parm where, __segment blk, unsigned *blk_size )
{
    tiny_ret_t      rc;
    unsigned        bytes;

    switch( where ) {
    case ON_DISK:
        if( *blk_size > FILE_BLOCK_SIZE_PARA ) {
            *blk_size = FILE_BLOCK_SIZE_PARA;
        }
        bytes = *blk_size << 4;
        rc = TinyFarWrite( fileHandle, MK_FP( blk, 0 ), bytes );
        return( TINY_OK( rc ) && TINY_INFO( rc ) == bytes );
#if defined( USE_EMS )
    case IN_EMS:
        memBlockWrite( EMSBlockWrite, blk, blk_size );
        break;
#endif
#if defined( USE_XMS )
    case IN_XMS:
        memBlockWrite( XMSBlockWrite, blk, blk_size );
        break;
#endif
    }
    return( true );
}

bool XchkRead( where_parm where, __segment *blk )
{
    tiny_ret_t      rc;

    switch( where ) {
    case ON_DISK:
        rc = TinyFarRead( fileHandle, MK_FP( *blk, 0 ), FILE_BLOCK_SIZE );
        if( TINY_ERROR( rc ) || TINY_INFO( rc ) != FILE_BLOCK_SIZE ) {
            return( false );
        }
        *blk += FILE_BLOCK_SIZE_PARA;
        break;
#if defined( USE_EMS )
    case IN_EMS:
        return( memBlockRead( EMSBlockRead, blk ) );
#endif
#if defined( USE_XMS )
    case IN_XMS:
        return( memBlockRead( XMSBlockRead, blk ) );
#endif
    }
    return( true );
}
