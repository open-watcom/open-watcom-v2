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
* Description:  Float to string conversion routines.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "xfloat.h"


/*
NOTES ON USAGE
==============

Parameter explanation :
        1. userbuffer@ : char  - Pointer points to where user
                                 wants the result string to go
        2. relnum@ : reallong - Pointer to the floating number
                                 to be converted
        3. digs : int - If format='G' or format='E' this number
                        represents the number of significant
                        digits the user wants.  If format='F'
                        this number represent the number of
                        decimal places the user wants
        4. plus : bool - Bool indicating if a plus sign is wanted
                         for a positive number.
        5. maxwidth : int - This is the maximum width allowable
                            for the resulting string.
        6. scale : int - See the FORTRAN standard for a detailed
                         explanation of what it is used for. For
                         most applications, this parm should be 0
        7. expwidth : int - This specifies the width of the
                            exponent field ( not including the
                            sign or the E char )
        8. expchar : char - This represent the character the user
                            wants to represent the exponent char.
                            For most applications, it will be 'E'

General notes : If the resulting string will not fit in the
                max width specified by the user, the entire field
                will be filled with *'s. If the field is wider
                than necessary, the string is right justified and
                padded on the left with spaces.
                The expwidth and expchar parms will not be used
                if F formatting is done.
*/

extern  double  _Scale10V( double, int );
extern  char   *__cvt( double, int, int *, int *, int, char * );
extern  int     __Nan_Inf( double, char * );

#define SIG_DIGITS              17

static int MaxPrec = {
    SIG_DIGITS              /* maximum allowed precision */
};


void _SetMaxPrec( int value )
/***************************/
{
    MaxPrec = value;
}

/**************************************************************************/
/***                                                                    ***/
/***    DoEformat - format number into E-format                         ***/
/***                                                                    ***/
/**************************************************************************/

static int DoEFormat( char *bufptr, int exponent, int expwidth, int width_left )
/******************************************************************************/
{
    int     n;
    int     divisor;
    int     width;
    char    sign;

    if( exponent < 0 ) {
        exponent = -exponent;
        sign = '-';
    } else {
        sign = '+';
    }
    /* assume 3 digits required for exponent */
    n = 3;
    divisor = 100;
    if( exponent < 100 ) {
        n = 2;
        divisor = 10;
        if( exponent < 10 ) {
            n = 1;
            divisor = 1;
        }
    }
    if( expwidth == 0 ) {
        expwidth = 2;
        if( n == 3 ) expwidth = 3;
    }
    width = expwidth + 1;
    if( expwidth + 1 <= width_left ) {
        if( n <= expwidth ) {
            *bufptr++ = sign;
            while( expwidth > n ) {
                /* put in leading zeroes for exponent */
                *bufptr++ = '0';
                --expwidth;
            }
            do {
                *bufptr++ = exponent / divisor + '0';
                exponent = exponent % divisor;
                divisor = divisor / 10;
            } while( divisor != 0 );
        } else {
            width = n + 1;
        }
    }
    return( width );
}


/**************************************************************************/
/***                                                                    ***/
/***    DoFFormat - format number into F-format                         ***/
/***                                                                    ***/
/**************************************************************************/

static  char *DoFFormat( char *bufptr, char *p, int left, int sigdigits )
/***********************************************************************/
{
    if( sigdigits == 0 && left <= 0 ) {
        *bufptr++ = '0';
        *bufptr++ = '.';
    } else {
        for( ;; ) {
            if( left <= 0 ) break;
            if( *p == '\0' ) break;
            *bufptr++ = *p++;
            --left;
        }
        if( left > 0 ) {
            do {
                *bufptr++ = '0';
            } while( --left > 0 );
        }
        *bufptr++ = '.';
/*
 note:  Only leading zeroes may be printed if the decimal accuracy
        specified in the  f-format specifier is not large enough
               ie f4.2 for .0000566
        Once we decide that some significant digits get printed, we
        can print all the digits returned by CanForm since it takes
        into account the number of leading zero's necessary when it
        decides what number of digits to round to. CanForm always
        returns at least 1 digit, so we must check for the above
        case before we print the result returned by CanForm.
*/
        if( sigdigits > 0 ) {
            while( left != 0 ) {
                *bufptr++ = '0';
                ++left;
                if( --sigdigits == 0 ) break;
            }
        }
        if( sigdigits > 0 ) {
            while( *p != '\0' ) {
                *bufptr++ = *p++;
                if( --sigdigits == 0 ) break;
            }
            while( sigdigits != 0 ) {
                *bufptr++ = '0';
                --sigdigits;
            }
        }
    }
    return( bufptr );
}

static  void AdjField( char *userbuf, int actual_width, int width )
/*****************************************************************/
{
    char    second_char;

    if( actual_width != width ) {
        second_char = userbuf[1];
        do {
            --actual_width;
            --width;
            userbuf[ width ] = userbuf[ actual_width ];
        } while( actual_width != 0 );
        if( userbuf[ 0 ] == '.' ) {
            --width;
            userbuf[ width ] = '0';
        } else {
            if( ( userbuf[ 0 ] == '+' || userbuf[ 0 ] == '-' ) &&
                ( second_char == '.' ) ) {
                userbuf[ width ] = '0';
                --width;
                userbuf[ width ] = userbuf[ 0 ];
            }
        }
        memset( userbuf, ' ', width );
    }
}


void _FtoS( char *userbuffer, double *realnum, int digs,
            char plus, int maxwidth, int scale, int expwidth,
            char expchar, char user_format )
/************************************************************/
{
    int     sigdigits;
    char    *bufptr;
    int     len;
    char    format;
    int     exp;
    int     dec;            /* decimal place */
    int     sign;           /* 0 => +ve, -1 => -ve */
    double  x;
    char    buf[ __FPCVT_BUFFERLEN + 1 ];

    if( __Nan_Inf( *realnum, buf ) ) {              /* 05-mar-91 */
        for( len = 0; buf[len]; len++ ) {
            if( len < maxwidth )  userbuffer[len] = buf[len];
        }
        goto check_field_width;
    }
    format = user_format;
    if( format == 'G' ) {
        x = fabs( *realnum );                       /* 02-may-91 */
        if( x != 0.0 ) {
            x = log10( x );
            exp = floor( x );
            if( exp < -4  ||  exp >= digs ) {
                format = 'E';
            } else {
                if( x >= 0.0 )  ++exp;              /* 19-nov-91 */
                digs -= exp;                        /* 21-jul-88 */
                scale = 0;
                format = 'F';
            }
        } else {
            exp = 0;
            scale = 0;
            format = 'F';
        }
    }
    if( format == 'E' && (scale <= -digs || scale >= digs + 2) ) {
        memset( userbuffer, '*', maxwidth );
    } else {
        sigdigits = digs;
        /* 27-oct-88, format changed to user_format */
        if( user_format == 'E' ) {
            if( scale > 0 ) {
                ++sigdigits;
            } else if( scale < 0 ) {
                sigdigits += scale;
            }
        }
        x = *realnum;
        if( x != 0.0 ) {
            if( format != 'E' && scale != 0 ) {
                x = _Scale10V( x, scale );
            }
        }
        __cvt( x, sigdigits, &dec, &sign, format, &buf );
        bufptr = userbuffer;
        if( sign != 0 ) {
            *bufptr++ = '-';
        } else {
            if( plus ) *bufptr++ = '+';
        }
        if( format == 'E' ) {
            bufptr = DoFFormat( bufptr, &buf, scale, sigdigits - scale );
            len = bufptr - userbuffer;
            if( expchar != '\0' ) {
                if( len < maxwidth ) {
                    *bufptr++ = expchar;
                    ++len;
                }
            }
            if( x != 0.0 ) {
                dec -= scale;
            }
            len += DoEFormat( bufptr, dec, expwidth, maxwidth - len );
        } else {
            len = bufptr - userbuffer;
            if( maxwidth < dec + 1 + sigdigits + len ) {
                len = maxwidth + 1; /* indicate # too big for field */
            } else {
                bufptr = DoFFormat( bufptr, &buf, dec, sigdigits );
                len = bufptr - userbuffer;
            }
        }
check_field_width:
        if( len > maxwidth ) {
            memset( userbuffer, '*', maxwidth );
        } else {
            AdjField( userbuffer, len, maxwidth );
        }
    }
}
