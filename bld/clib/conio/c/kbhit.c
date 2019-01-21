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
* Description:  Implementation of kbhit() for DOS.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <dos.h>
#include <conio.h>
#include "dosfuncx.h"
#include "rtdata.h"
#include "defwin.h"


#ifndef DEFAULT_WINDOWING
    extern unsigned char    _dos( unsigned char );
    #pragma aux _dos = \
            "int 21h"      \
        __parm caller   [__ah] \
        __value         [__al]
#endif

_WCRTLINK int kbhit( void )
{
    if( _RWD_cbyte != 0 )
        return( 1 );
#ifdef DEFAULT_WINDOWING
    if( _WindowsKbhit != NULL ) {
        LPWDATA res;
        res = _WindowsIsWindowedHandle( STDIN_FILENO );
        return( _WindowsKbhit( res ) );
    } else {
        return( 0 );
    }
#else
    return( _dos( DOS_INPUT_STATUS ) != 0 );
#endif
}
