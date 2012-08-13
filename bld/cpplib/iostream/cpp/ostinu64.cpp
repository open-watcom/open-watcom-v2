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
* Description:  Implementation of ostream::operator << for unsigned
*               64-bit integers.
*
****************************************************************************/


#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#endif
#include "ioutil.h"
#include "lock.h"
#include "osthdr.h"

ostream &ostream::operator << ( unsigned __int64 i ) {
/****************************************************/
// Write an unsigned 64bit integer to the stream.
// A signed 64bit integer displayed in base 8 or base 16 also uses
// this routine.

    int         base;
    char        buffer[LONGEST_INT64 + 1];
    int         size;
    int         digit_offset;

    __lock_it( __i_lock );
    base = __FlagsToBase( flags() );

    // Sign:
    if( i > 0  &&  (flags() & ios::showpos)  &&  base == 10 ) {
        buffer[0] = '+';
        size      = 1;
    } else {
        size      = 0;
    }

    // Prefix:
    __AddPrefix( i == 0, buffer, size, base, flags() );

    // Digits:
    digit_offset = size;
    ulltoa( i, buffer + digit_offset, base );
    if( flags() & ios::uppercase ) {
        strupr( buffer );
    }
    size = ::strlen( buffer );

    // Write the number:
    if( opfx() ) {
        setstate( __WATCOM_ios::writeitem( *this, buffer, size, digit_offset ) );
        osfx();
    }
    return( *this );
}
