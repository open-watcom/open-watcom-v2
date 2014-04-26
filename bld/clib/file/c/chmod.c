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
* Description:  DOS implementation of chmod().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <unistd.h>
#include <dos.h>


_WCRTLINK int __F_NAME(chmod,_wchmod)( const CHAR_TYPE *pathname, int pmode )
/***************************************************************************/
{
#ifdef __WIDECHAR__
    char        mbPath[MB_CUR_MAX * _MAX_PATH];

    if( wcstombs( mbPath, pathname, sizeof( mbPath ) ) == -1 ) {
        mbPath[0] = '\0';
    }
    return( chmod( mbPath, pmode ) );
#else
    unsigned    attr;

    if( _dos_getfileattr( __F_NAME(pathname,mbPath), &attr ) ) {
        return( -1 );
    }
    attr &= ~_A_RDONLY;
    if( !( pmode & S_IWRITE ) ) {
        attr |= _A_RDONLY;
    }
    if( _dos_setfileattr( __F_NAME(pathname,mbPath), attr ) ) {
        return( -1 );
    }
    return( 0 );
#endif
}
