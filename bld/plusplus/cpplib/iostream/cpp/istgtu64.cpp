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

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <ctype.h>
#include <iostream>
#include <streambu>
#endif
#include "ioutil.h"
#include "isthdr.h"

std::ios::iostate __getunsignedint64( std::streambuf *sb,
                                      unsigned __int64 &value,
                                      unsigned __int64 maxval,
                                      signed __int64 minval,
                                      std::ios::fmtflags format )
{

    unsigned __int64  number;
    std::ios::iostate state;
    char           sign;
    int            base;
    int            offset;
    int            ch;

    state = std::ios::goodbit;
    offset = 0;
    format &= std::ios::basefield;
    if( format ) {
        base = __FlagsToBase( format );
    } else {
        base = 0;
    }
    state = __getsign( sb, sign );
    if( !state && sign ) {
        ch = sb->sbumpc();
        int low_char = sb->speekc();
        if( (base == 10 || base == 0) && isdigit( low_char ) ) {
            offset++;
        } else if( base == 16 && isxdigit( low_char ) ) {
            offset++;
        } else if( base == 8 && isdigit( low_char ) && low_char < '8' ) {
            offset++;
        } else {
            // this will catch (low_char == EOF) too
            sb->sputbackc( (char)ch );
            state |= std::ios::failbit;
        }
    }
    if( !state && !base ) {
        state = __getbase( sb, base, offset );
    }
    if( !state ) {
        state = __getnumberint64( sb, number, base, offset );
    }
    if( !state && offset > 0 ) {
        if( number <= maxval ) {
            if( number == 0 || sign != '-' ) {
                value = number;
            } else {
                if( minval == 0 || -number >= minval ) {
                    value = -number;
                } else {
                    state |= std::ios::failbit;
                }
            }
        } else {
            state |= std::ios::failbit;
        }
    }
    if( offset == 0 ) {
        state |= std::ios::failbit;
    }
    return( state );
}
