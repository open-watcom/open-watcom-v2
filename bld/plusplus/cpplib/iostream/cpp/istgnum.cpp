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
#include <limits.h>
#include <iostream>
#include <streambu>
#endif
#include "ioutil.h"
#include "lock.h"
#include "isthdr.h"

// Used by getnumber. Multiplication by 8 is done using a left-shift of
// three bits. Multiplication by 16 is done using a left-shift of four
// bits. Multiplication by 10 is done using a left-shift of three bits
// plus a left-shift of one bit. This table is used to determine if a
// shift will overflow. The number of bits to shift is used to index
// into the table. The table entry is anded with the unsigned long
// number and if any bits are on, then the shift will overflow.

static unsigned long const overFlowMasks[] = {
    0x00000000,
    0x80000000,
    0xC0000000,
    0xE0000000,
    0xF0000000
};

// Extract digits from the stream. Stop when a non-digit is found,
// leaving the non-digit in the stream. As digits are read, convert to
// an "unsigned long".

std::ios::iostate __getnumber( std::streambuf *sb,
                               unsigned long &number,
                               int base,
                               int &offset ) {

    unsigned long  result;
    unsigned long  overflow;
    std::ios::iostate   state;
    int            ch;
    int            is_digit;
    int            digit_value;
    int            shift1;
    int            shift2;

    if( base == 8 ) {
        shift1 = 3;     // *8
        shift2 = 0;
    } else if( base == 16 ) {
        shift1 = 4;     // *16
        shift2 = 0;
    } else {
        shift1 = 3;     // *8
        shift2 = 1;     // *2
    }
    state    = std::ios::goodbit;
    result   = 0;
    overflow = 0;
    is_digit = TRUE;
    __lock_it( sb->__b_lock );
    while( is_digit ) {
        ch = sb->speekc();
        if( ch == EOF ) {
            if( offset == 0 ) {
                state |= std::ios::eofbit;
            }
        }
        digit_value = ch - '0';
        if( base == 8 ) {
            is_digit = (ch >= '0'  &&  ch <= '7');
        } else {
            is_digit = isdigit( ch );
            if( base == 16 && !is_digit ) {
                char low_char;
                low_char = (char)tolower( ch );
                if( low_char >= 'a'  &&  low_char <= 'f' ) {
                    digit_value = low_char - 'a' + 10;
                    is_digit    = TRUE;
                }
            }
        }
        if( is_digit ) {
            unsigned long tmp_result = result;
            overflow  |= (result & overFlowMasks[shift1]);
            result   <<= shift1;
            if( shift2 > 0 ) {
                tmp_result <<= shift2;
                if( result > (ULONG_MAX - tmp_result) ) overflow = 1;
                result += tmp_result;
            }
            if( result > ULONG_MAX - digit_value ) overflow = 1;
            result += digit_value;
            sb->sbumpc();
            offset++;
        }
    }
    if( overflow != 0 ) {
        state |= std::ios::failbit;
    }
    number = result;
    return( state );
}
