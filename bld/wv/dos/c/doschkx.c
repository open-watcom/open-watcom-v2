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


#include <stdlib.h>
#include <i86.h>
#include "bool.h"
#include "tinyio.h"
#include "doschk.h"
#include "doschkx.h"


#define CHECK_FILE      "___CHK.MEM"

#define FILE_BLOCK_SIZE 0x8000

static char             *fullName = NULL;
static int              fileHandle = -1;

void XcleanUp( where_parm where )
{
    TinyClose( fileHandle );
    fileHandle = -1;
    TinyDelete( fullName );
    fullName = NULL;
}

bool XchkOpen( where_parm where, char *f_buff )
{
    tiny_ret_t      rc;

    fileHandle = -1;
    if( f_buff != NULL ) {
        fullName = f_buff;
        *f_buff++ = TinyGetCurrDrive() + 'A';
        *f_buff++ = ':';
        *f_buff++ = '\\';
        rc = TinyFarGetCWDir( f_buff, 0 );
        if( TINY_OK( rc ) ) {
            while( *f_buff != NULLCHAR )
                ++f_buff;
            if( f_buff[-1] == '\\' ) {
                --f_buff;
            } else {
                *f_buff++ = '\\';
            }
            memcpy( f_buff, CHECK_FILE, sizeof( CHECK_FILE ) );
            rc = TinyCreate( fullName, TIO_NORMAL );
            if( TINY_OK( rc ) ) {
                fileHandle = TINY_INFO( rc );
            }
        }
        if( fileHandle == -1 ) {
            fullName = NULL;
        }
    } else {
        if( fullName != NULL ) {
            rc = TinyOpen( fullName, TIO_READ );
            if( TINY_OK( rc ) ) {
                fileHandle = TINY_INFO( rc );
            }
        }
    }
    return( fileHandle != -1 );
}

void XchkClose( where_parm where )
{
    TinyClose( fileHandle );
    fileHandle = -1;
}

bool XchkWrite( where_parm where, __segment buff, unsigned *size )
{
    tiny_ret_t      rc;
    unsigned        bytes;

    if( *size >= 0x1000 ) {
        *size = FILE_BLOCK_SIZE >> 4;
    }
    bytes = *size << 4;
    rc = TinyFarWrite( fileHandle, MK_FP( buff, 0 ), bytes );
    return( TINY_OK( rc ) && TINY_INFO( rc ) == bytes );
}

bool XchkRead( where_parm where, __segment *buff )
{
    tiny_ret_t      rc;

    rc = TinyFarRead( fileHandle, MK_FP( *buff, 0 ), FILE_BLOCK_SIZE );
    if( TINY_ERROR( rc ) || TINY_INFO( rc ) != FILE_BLOCK_SIZE ) {
        return( false );
    }
    *buff += FILE_BLOCK_SIZE >> 4;
    return( true );
}
