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


#include "variety.h"
#include <stddef.h>
#include <conio.h>
#include "dosfuncx.h"
#include "defwin.h"


#ifndef DEFAULT_WINDOWING
    #include "tinyio.h"
    extern unsigned char    _dos( unsigned char, int );
    #pragma aux _dos = \
            _INT_21 \
        __parm __caller [__ah] [__dx] \
        __value         [__al]
#endif

_WCRTLINK int putch( int c )
{
#ifdef DEFAULT_WINDOWING
    if( _WindowsPutch != NULL ) {
        LPWDATA res;
        res = _WindowsIsWindowedHandle( STDOUT_FILENO );
        _WindowsPutch( res, c );
    }
#else
    _dos( DOS_OUTPUT_CHAR, c );
#endif
    return( c );
}
