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


#include "tinyio.h"
#include "nmp.h"

int myopen( char *name )
{
    tiny_ret_t  rc;

    rc = TinyOpen( name, TIO_READ_WRITE );
    if( TINY_ERROR( rc ) ) {
        return( -1 );
    } else {
        return( TINY_INFO( rc ) );
    }
}

void myclose( int handle )
{
    TinyClose( handle );
}

int myread( int handle, char *buff, int len )
{
    tiny_ret_t  rc;

    rc = TinyRead( handle, buff, len );
    if( TINY_ERROR( rc ) ) {
        return( 0 );
    } else {
        return( TINY_INFO( rc ) );
    }
}


int mywrite( int handle, char *buff, int len )
{
    tiny_ret_t  rc;

    rc = TinyWrite( handle, buff, len );
    if( TINY_ERROR( rc ) ) {
        return( 0 );
    } else {
        return( TINY_INFO( rc ) );
    }
}

void mysnooze()
{
}
