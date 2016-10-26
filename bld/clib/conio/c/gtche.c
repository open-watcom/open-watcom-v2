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
#include <conio.h>
#include "rtdata.h"
#include "defwin.h"

#ifndef DEFAULT_WINDOWING
    #include "tinyio.h"
    extern      unsigned char _dos(char);

    #pragma aux _dos = _INT_21 parm caller [ah] value [al];
#endif

_WCRTLINK int getche( void )
{
    int         c;

    c = _RWD_cbyte;
    _RWD_cbyte = 0;
    if( c == 0 ) {
#ifdef DEFAULT_WINDOWING
        if( _WindowsGetche != 0 ) {
            LPWDATA     res;
            res = _WindowsIsWindowedHandle( STDOUT_FILENO );
            c = _WindowsGetche( res );
        }
#else
        c = _dos( 1 );
#endif
    }
    return( c );
}
