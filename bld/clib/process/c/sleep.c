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
#include <i86.h>
#include <dos.h>


_WCRTLINK void sleep( unsigned seconds )
    {
#ifndef __NETWARE__
        unsigned char old_second;
        unsigned char old_hundredths;
        struct dostime_t  t;
#endif

#ifdef __NETWARE__
        delay( seconds * 1000 );
#else
        _dos_gettime( &t );
        old_hundredths = t.hsecond;
        if( old_hundredths > 90 )  old_hundredths = 90;
        for( ; seconds; --seconds ) {
            old_second = t.second;
            do {                        /* loop until second changes */
                _dos_gettime( &t );
            } while( t.second == old_second );
        }
        do {
            _dos_gettime( &t );
        } while( t.hsecond < old_hundredths );
#endif
    }
