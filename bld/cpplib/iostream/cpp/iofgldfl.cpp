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
#include <stdlib.h>
#include <iostream>
#include <streambu>
#endif
#include "ioutil.h"
#include "lock.h"
#include "iofhdr.h"

std::ios::iostate __getLDFsign( std::streambuf *sb, char *&bufptr, int is_exp ) {

    std::ios::iostate  state;
    int           c, sign;

    state = std::ios::goodbit;
    c = sb->speekc();
    if( c == EOF ) {
        state |= std::ios::eofbit;
        return( state );
    }
    if( !isdigit(c)  ) {
        if( c == '+'  ||  c == '-' ) {
            sign = sb->sbumpc();
            c = sb->speekc();
            if( !isdigit(c) ) {
                if( is_exp || c != '.' ) {
                    state |= std::ios::failbit;
                    sb->sputbackc( (char)sign );
                    return( state );
                }
            }
            *(bufptr++) = (char)sign;
            return( state );
        }
    }
    return( state );
}

std::ios::iostate __getLDFmantissa( std::streambuf *sb, char *&bufptr ) {

    std::ios::iostate  state;
    int           c, mant_digits;
    char          have_decimal;

    state = std::ios::goodbit;
    mant_digits  = 0;
    have_decimal = false;
    for( ;; ) {
        c = sb->speekc();
        if( c == EOF ) {
            break;
        }
        if( isdigit( c ) ) {
            mant_digits++;
            sb->sbumpc();
        } else if( c == '.' ) {
            if( have_decimal ) {
                if ( mant_digits == 0 ) {
                    bufptr--;
                    sb->sputbackc( '.' );
                    state |= std::ios::failbit;
                    return( state );
                } else {
                    break;
                }
            }
            have_decimal = true;
            sb->sbumpc();
        } else {
           break;
        }
        *(bufptr++) = (char)c;
    }
    if( mant_digits == 0 ) {
        state |= std::ios::failbit;
        if ( have_decimal ) {
            bufptr--;
            sb->sputbackc( '.' );
        }
    }
    return( state );
}

std::ios::iostate __getLDFexponent( std::streambuf *sb, char *&bufptr ) {

    std::ios::iostate  state;
    int           c, exp, exp_digits;

    state = std::ios::goodbit;
    exp_digits = 0;
    c = sb->speekc();
    if( tolower(c) != 'e' ) {
         return( state );
    }
    exp = sb->sbumpc();
    *(bufptr++) = (char)exp;  // keep the 'E'
    state |= __getLDFsign( sb, bufptr, true );
    if ( state & std::ios::eofbit ) {
        bufptr--;             // discard the 'E'
        sb->sputbackc( (char)exp );
        return( std::ios::goodbit );
    }
    if ( !state ) {
        for( ;; ) {
            c = sb->speekc();
            if( isdigit( c ) ) {
                exp_digits++;
                sb->sbumpc();
            } else {
                if( exp_digits == 0 ) {
                    bufptr--;         // discard the 'E'
                    sb->sputbackc( (char)exp );
                    return( state );
                } else {
                    break;
                }
            }
            *(bufptr++) = (char)c;
        }
    }
    return( state );
}


// Read the input stream into buffer as long as it is a prefix of a
// floating point number. Return ios::failbit if something goes wrong in
// the format.

std::ios::iostate __GetLDFloat( std::streambuf *sb, char *buffer ) {

    char         *bufptr;
    std::ios::iostate  state;

    state = std::ios::goodbit;
    bufptr = buffer;
    state |= __getLDFsign( sb, bufptr, false );
    if( state & std::ios::eofbit ) {
        state |= std::ios::failbit;
    }
    if( !state ) {
        state |= __getLDFmantissa( sb, bufptr );
    }
    if( !state ) {
        state |= __getLDFexponent( sb, bufptr );
    }
    if( state ) {
        buffer[0] = '\0';
    }
    *bufptr = '\0';
    return( state );
}
