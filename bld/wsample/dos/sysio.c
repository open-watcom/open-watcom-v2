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


#include <malloc.h>
#include "tinyio.h"


void SysInit()
{
}

int SysCreate( char *name )
{
    tiny_ret_t  ret;

    ret = TinyCreate( name, TIO_NORMAL );
    if( ret < 0 ) return( -1 );
    return( ret );
}

int SysDelete( char *name )
{
    tiny_ret_t  ret;

    ret = TinyDelete( name );
    if( ret < 0 ) return( -1 );
    return( 0 );
}

unsigned SysWrite( int handle, void FAR_PTR *buff, unsigned len )
{
    tiny_ret_t  ret;

    ret = TinyFarWrite( handle, buff, len );
    if( ret < 0 ) return( (unsigned)-1 );
    return( ret );
}

int SysSeek( int handle, unsigned long loc )
{
    if( TinySeek( handle, loc, TIO_SEEK_START ) != loc ) return( -1 );
    return( 0 );
}

int SysClose( int handle )
{
    if( TinyClose( handle ) < 0 ) return( -1 );
    return( 0 );
}

void FAR_PTR * alloc( int size )
{
    return( _fmalloc( size ) );
}

void mfree( void FAR_PTR *chunk )
{
    _ffree( chunk );
}
