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
* Description:  integer and floating point conversion routines
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextfun.h"
#include "intcnv.h"
#include "fltcnv.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <float.h>
#include "fmtcnvt.h"


static void AddDig( canon_form *canon, char ch )
// Add a significant digit to mantissa.
{
    int         col;

    col = canon->col;
    canon->col++;
    if( col <= CONVERSION_DIGITS ) {
        _CanonDigits( canon )[col] = ch;
    }
}

static int  Digits( canon_form *canon, char *field, char *stop, int decimals, bool blanks, byte flag )
// Collect digits to the left or right of the decimal point. Take blanks
// into consideration. Set "canon->exp" accordingly.
{
    int         count;
    char        ch;

    count = 0;
    for( ;; ) {
        for( ;; ) {
            if( field == stop )
                break;
            ch = *field;
            if( ch != ' ' )
                break;
            if( blanks ) {
                ch = '0';
            } else {
                if( flag == LEFT_DIGITS ) {
                    canon->blanks++;
                }
                ++field;
                ++count;
            }
            if( ch == '0' ) {
                break;
            }
        }
        if( field == stop )
            break;
        if( isdigit( ch ) == 0 )
            break;
        if( canon->col == 0 ) {
            if( ch == '0' ) {
                if( canon->flags & DECIMAL ) {
                    canon->exp--;
                }
            } else {
                if( (canon->flags & DECIMAL) == 0 ) {
                     canon->exp = stop - field - decimals;
                }
                AddDig( canon, ch );
            }
        } else {
            AddDig( canon, ch );
        }
        count++;
        field++;
    }
    if( count > 0 ) {
        canon->flags |= flag;
    }
    return( count );
}

int     FmtS2I( char *str, uint len, bool blanks, intstar4 *value, bool stop_ok, uint *width )
// Convert a FORTRAN I format string to an integer.
{
    char        ch;
    bool        sign;
    char        *strend;
    bool        minus;
    int         status;
    uint        wid;
    intstar4    result;
    intstar4    tmp;

    status = INT_OK;
    strend = str + len;
    while( *str == ' ' ) {
        str++;
        if( str == strend ) {
            break;
        }
    }
    minus = false;
    sign = false;
    if( str != strend ) {
        ch = *str;
        if( (ch == '+') || (ch == '-') ) {
            if( ch == '-' ) {
                minus = true;
            }
            str++;
            sign = true;
        }
    }
    result = 0;
    for( ; str != strend; str++ ) {
        ch = *str;
        if( ch == ' ' ) {
            if( !blanks )
                continue;
            ch = '0';
        }
        if( isdigit( ch ) == 0 )
            break;
        tmp = 10;
        if( MulIOFlo( &result, &tmp ) ) {
            status = INT_OVERFLOW;
        }
        tmp = ch - '0';
        if( minus ) {
            tmp = -tmp;
        }
        if( AddIOFlo( &result, &tmp ) ) {
            status = INT_OVERFLOW;
        }
        sign = false;
    }
    if( sign ) {
        str = NULL;
    } else {
        *value = result;
    }
    wid = len;
    if( str != strend ) {
        if( stop_ok && !sign ) {
            wid -= ( strend - str );
        } else {
            status = INT_INVALID;
        }
    }
    if( width != NULL ) {
        *width = wid;
    }
    return( status );
}

int FmtS2F( char *field, uint width, int decimals, bool blanks,
                int scale, int prec, extended *result, bool stop_ok,
                uint *new_width, bool extend_flt ) {
// Format a string to floating point representation.

    char        *stop;
    char        ch;
    char        *start;
    canon_form  canon;
    intstar4    exp;

    // initialize the canonical form of the number to be collected

    canon.exp = 0;
    canon.neg = false;
    canon.col = 0;
    canon.flags = 0;
    canon.blanks = 0;

    // scan over blanks

    stop = field + width;
    while( *field == ' ' ) {
        field++;
        if( field == stop ) {
            break;
        }
    }
    start = field;

    // process optional sign

    if( field != stop ) {
        ch = *field;
        if( (ch == '-') || (ch == '+') ) {
            canon.flags |= FOUND_SIGN;
            field++;
            start++;
            if( ch != '+' ) {
                canon.neg = true;
            }
        }
    }

    // collect digits on left of decimal

    memset( _CanonDigits( &canon ), '0', CONVERSION_DIGITS );
    _CanonDigits( &canon )[CONVERSION_DIGITS] = NULLCHAR;
    field += Digits( &canon, field, stop, decimals, blanks, LEFT_DIGITS );

    // collect optional decimal point

    if( (field != stop) && (*field == '.') ) {
        canon.exp = canon.col;
        field++;
        canon.flags |= DECIMAL;
    }

    // collect digits on right of decimal

    field += Digits( &canon, field, stop, decimals, blanks, RIGHT_DIGITS );

    // collect optional exponent

    if( field != stop ) {
        ch = tolower( *field );
        if( (ch == 'e') || (ch == 'd') || (ch == 'q') || (ch == '+') || (ch == '-') ) {
            if( ( ch == 'e' ) && extend_flt ) {
                canon.flags |= DOUBLE;
            } else if( ch == 'd' ) {
                if( extend_flt ) {
                    canon.flags |= LONGDOUBLE;
                } else {
                    canon.flags |= DOUBLE;
                }
            } else if( ch == 'q' ) {
                canon.flags |= LONGDOUBLE;
            }
            // if E|D found but no decimal, use the specified # of decimals
            if( (canon.flags & DECIMAL) == 0 ) {
                canon.exp = canon.col - decimals + canon.blanks;
            }
            if( ch != '-' ) {
                field++;
            }
            canon.flags |= EXPONENT;
            exp = 0;
            if( (stop == field) || (FmtS2I( field, stop - field, blanks, &exp, false, NULL ) != INT_OK) ) {
                canon.flags |= BAD_EXPONENT;
            }
            canon.exp += exp;
            field = stop;
        }
    }

    // if exponent was found, override the scale factor

    if( canon.flags & EXPONENT ) {
        canon.exp += scale;
    }
    if( new_width != NULL ) {
        *new_width = width - ( stop - field );
    }
    if( !stop_ok && ( field != stop ) )
        return( FLT_INVALID );
    if( canon.flags & BAD_EXPONENT )
        return( FLT_INVALID );
    if( (canon.flags & (FOUND_SIGN | DECIMAL | EXPONENT)) && (canon.flags & (LEFT_DIGITS | RIGHT_DIGITS)) == 0 )
        return( FLT_INVALID );
    canon.exp -= scale; // adjust for kP specifier
    if( (canon.flags & DECIMAL) == 0 ) { // if no '.' found
        // adjust for BN specifier
        canon.exp -= canon.blanks;
    }

    // Convert the string to floating-point

    if( canon.neg ) {
        *_CanonSign( &canon ) = '-';
    } else {
        *_CanonSign( &canon ) = ' ';
    }
    *_CanonDecimal( &canon ) = '.';
    sprintf( _CanonExponent( &canon ), "E%d", canon.exp );
    errno = 0;
    *result = strtod( _CanonNumber( &canon ), NULL );
    if( errno != 0 )
        return( FLT_RANGE_EXCEEDED );
    if( prec == PRECISION_SINGLE ) {
        if( (canon.flags & DOUBLE) == 0 && (canon.flags & LONGDOUBLE) == 0 ) {

            single      volatile sresult;

            if( *result > FLT_MAX )
                return( FLT_RANGE_EXCEEDED );

            sresult = *result;
            *result = sresult;
        }
    } else if( prec == PRECISION_DOUBLE ) {
        if( (canon.flags & LONGDOUBLE) == 0 ) {

            double      volatile sresult;

            if( *result > DBL_MAX )
                return( FLT_RANGE_EXCEEDED );

            sresult = *result;
            *result = sresult;
        }
    }
    return( FLT_OK );
}
