/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include "doschk.h"
#include "doschkx.h"


#define CHECK_FILE          "___CHK.MEM"

#define TINY_HANDLE_NULL    ((tiny_handle_t)-1)

static char                 *fullName = NULL;

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
        *f_buff++ = TinyGetCurrDrive() + 'A';
        *f_buff++ = ':';
        *f_buff++ = '\\';
        rc = TinyFarGetCWDir( f_buff, 0 );
        if( TINY_OK( rc ) ) {
            while( *f_buff != 0 )
                ++f_buff;
            if( f_buff[-1] == '\\' ) {
                --f_buff;
            } else {
                *f_buff++ = '\\';
            }
            memcpy( f_buff, CHECK_FILE, sizeof( CHECK_FILE ) );
            rc = TinyCreate( fullName, TIO_NORMAL );
            if( TINY_OK( rc ) ) {
                filehandle = TINY_INFO( rc );
            }
        }
        if( filehandle == TINY_HANDLE_NULL ) {
            fullName = NULL;
        }
    } else {
        if( fullName != NULL ) {
            rc = TinyOpen( fullName, TIO_READ );
            if( TINY_OK( rc ) ) {
                filehandle = TINY_INFO( rc );
            }
        }
    }
    return( filehandle );
}
