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


/* This module defines functions for converting between floating point numbers
 * and ascii strings.  It uses multiply precions integers to get accurate
 * results.
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "mp.h"
#include "fp.h"

/* real number types */
#define FLOAT                   1
#define DOUBLE                  2
#define EXTENDED                3

#define max_exp                 32768

#define ASCII_POS_ZERO          "0"
#define ASCII_NEG_ZERO          "0"
#define ASCII_POS_INF           "Infinity"
#define ASCII_NEG_INF           "-Infinity"
#define ASCII_NAN               "NaN"

/* Remove trailing zeroes at the end of the mantissa part of the number to
 * speed up the calculations. */
int RemoveTrailingZeros( char *ascii )
{
    int i;

    for( i = 0; ; i++ ) {
        if( ascii[i] == 'e' || ascii[i] == 'E' || ascii[i] == '\0' ) break;
    }
    for( i--; i > 0; i-- ) {
        if( ascii[i] == '0' ) {
            ascii[i] = '_';
        } else if( ascii[i] != '.' ) {
            break;
        }
    }
    return 0;
}

/* parse ascii string into the mantissa f and the exponent e */
/* note: this function assumes a positive value */
int ParseAscii( char *ascii, mpnum *f, long *e )
{
    int readingExponent = FALSE;
    int expIsNeg = FALSE;
    int behindDecimal = FALSE;
    uint32 digitsBehindDecimal = 0;
    int rc;
    int sigexp = 0;
    *e = 0;
    mp_zero( f );

    RemoveTrailingZeros( ascii );

    for( ; *ascii != '\0'; ascii++ ) {
        if( *ascii == '.' ) {
            /* decimal point */
            behindDecimal = TRUE;
        } else if( *ascii == 'e' || *ascii == 'E' ) {
            /* exponent */
            if( ascii[1] == '-' ) {
                expIsNeg = TRUE;
                ascii++;
            } else if( ascii[1] == '+' ) {
                ascii++;
            }
            readingExponent = TRUE;
        } else if( *ascii >= '0' && *ascii <= '9' ) {
            if( readingExponent ) {
                if( *e <= max_exp ) {
                    *e = *e * 10 + (long)(*ascii - '0');
                }
            } else {
                /* number */
                rc = mp_mulsc( f, f, 10 );
                if( rc != MP_NO_ERR ) return rc;
                rc = mp_addsc( f, f, (uint32)(*ascii - '0') );
                if( rc != MP_NO_ERR ) return rc;
                if( behindDecimal ) {
                    digitsBehindDecimal++;
                }
            }
        } else if( *ascii == '_' ) {
            *ascii = '0';
            if( !behindDecimal ) {
                sigexp++;
            }
        } else {
            /* unrecognized character */
        }
    }
    if( expIsNeg ) *e = -*e;
    *e -= digitsBehindDecimal;
    *e += sigexp;

    return 0;
}

/* convert ascii string to a real number of type type */
int a2r( void *dst, char *ascii, int type )
{
    int isNeg = FALSE;
    char exp_real[10];
    mpnum f;
    mpnum exp;
    long e = 0;
    int rc;
    mpnum zero;
    int isZero;

    if( type != FLOAT && type != DOUBLE && type != EXTENDED ) return -1;

    /* check for sign */
    if( *ascii == '-' ) {
        isNeg = TRUE;
        ascii++;
    } else if( *ascii == '+' ) {
        ascii++;
    }

    rc = mp_init( &f, 0 );
    if( rc != MP_NO_ERR ) return rc;
    rc = mp_init( &exp, 10 );
    if( rc != MP_NO_ERR ) return rc;
    rc = mp_init( &zero, 0 );
    if( rc != MP_NO_ERR ) return rc;

    /* parse ascii into significand and exponent */
    ParseAscii( ascii, &f, &e );
    isZero = mp_eq( &f, &zero );
    mp_free( &zero );

    /* calculate 10^abs(e) */
    if( e > 0 ) {
        rc = mp_pow( &exp, &exp, (uint32)e );
    } else if( e < 0 ) {
        rc = mp_pow( &exp, &exp, (uint32)(-e) );
    }
    if( rc != MP_NO_ERR ) return rc;

    /* combine significand and exponent as reals */
    if( type == FLOAT ) {
        if( e > 0 ) {
            mp_mul( &f, &f, &exp );
            mp_tofloat( dst, &f );
        } else if( e < 0 ) {
            char temp[10];
            mp_toextended( temp, &f );
            mp_toextended( exp_real, &exp );
            ediv( temp, temp, exp_real );
            e2f( dst, temp );
        } else {
            mp_tofloat( dst, &f );
        }
    } else if( type == DOUBLE ) {
        if( e > 0 ) {
            mp_mul( &f, &f, &exp );
            mp_todouble( dst, &f );
        } else if( e < 0 ) {
            char temp[10];
            mp_toextended( temp, &f );
            mp_toextended( exp_real, &exp );
            ediv( temp, temp, exp_real );
            e2d( dst, temp );
        } else {
            mp_todouble( dst, &f );
        }
    } else {
        mp_toextended( dst, &f );
        mp_toextended( exp_real, &exp );
        if( e > 0 ) {
            emul( dst, dst, exp_real );
        } else if( e < 0 ) {
            ediv( dst, dst, exp_real );
        }
    }

    /* add sign */
    if( isNeg ) {
        if( type == FLOAT ) fneg( dst, dst );
        else if( type == DOUBLE ) dneg( dst, dst );
        else eneg( dst, dst );
    }
    mp_free( &f );
    mp_free( &exp );

    /* check if numbers are too large or too small */
    if( type == FLOAT ) {
        if( !isZero && ( f_isPosZero(dst) || f_isNegZero(dst) ) ) {
            return VALUE_TOO_SMALL;
        }
        if( f_isPosInf(dst) || f_isNegInf(dst) ) {
            return VALUE_TOO_LARGE;
        }
    } else if( type == DOUBLE ) {
        if( !isZero && ( d_isPosZero(dst) || d_isNegZero(dst) ) ) {
            return VALUE_TOO_SMALL;
        }
        if( d_isPosInf(dst) || d_isNegInf(dst) ) {
            return VALUE_TOO_LARGE;
        }
    }

    return 0;
}

int a2e( char *ext, char *ascii )
{
    return( a2r( ext, ascii, EXTENDED ) );
}

int a2d( double *dbl, char *ascii )
{
    return( a2r( dbl, ascii, DOUBLE ) );
}

int a2f( float *flt, char *ascii )
{
    return( a2r( flt, ascii, FLOAT ) );
}

/* Generate String
 *
 * This function uses an algorithm found in an essay entitled "How to Print
 * Floating Point Numbers Accurately" by Guy L. Steele Jr and Jon L White.
 * It takes as input a binary floating point number and generates a string
 * of decimal digits which represent the most significant bits of a decimal
 * representation of the number.  H represents the power of ten of the most
 * significant digit, and N represents the power of ten of the least signi-
 * ficant digit.  For example, for string "123456", H = 1, N = -4, the
 * decimal number is 12.3456.
 *
 * The resulting decimal number is not the closest possible representation
 * of the binary number.  Instead, it is the number of minimal length such
 * that converting the number to a binary floating point yields the original
 * binary value.  One advantage of this is that the algorithm will return
 * "1.3" for 1.3 instead of something like "1.299999952316284". (1.3 cannot
 * be stored exactly in binary form.)
 *
 * Note: D is assumed to be large enough to hold the whole string.  In the
 * worst case for a double, it will require 18 bytes.
 */
int GenerateString( char *D, void *src, int type, int *N, int *H )
{
    uint64 temp;
    int e;
    mpnum f, R, S, Mp, Mn, U, mptemp, mptemp2;
    int k = 0, p;
    int low, high;
    int index = 0;

    if( type != FLOAT && type != DOUBLE && type != EXTENDED ) return -1;

    if( type == FLOAT ) {
        parseFloat( *(float*)src, &temp, &e );
        p = float_precision;
    } else if( type == DOUBLE ) {
        parseDouble( *(double*)src, &temp, &e );
        p = double_precision;
    } else {
        parseExtended( src, &temp, &e );
        p = extended_precision;
    }
    e++;
    mp_init( &f, temp );
    mp_init( &R, 0 );
    mp_init( &S, 1 );
    mp_init( &Mp, 0 );
    mp_init( &Mn, 1 );
    mp_init( &U, 0 );
    mp_init( &mptemp, 1 );
    mp_init( &mptemp2, 0 );
    /* f * 2^(e-p) = src */

    mp_shiftleft( &R, &f, max( e-p, 0 ) );
    mp_shiftleft( &S, &S, max( 0, -(e-p) ) );
    /* assert R/S = f * 2^(e-p) = src */
    mp_shiftleft( &Mn, &Mn, max( e-p, 0 ) );
    mp_copy( &Mp, &Mn );

    mp_shiftleft( &mptemp, &mptemp, p - 1 );
    if( mp_eq( &f, &mptemp ) ) {
        /* border case: src is a power of 2 */
        mp_shiftleft( &Mp, &Mp, 1 );
        mp_shiftleft( &R, &R, 1 );
        mp_shiftleft( &S, &S, 1 );
    }
    for(;;) {
        mp_divsc( &mptemp, &S, 10, CEILING );
        if( mp_gte( &R, &mptemp ) ) break;
        k--;
        mp_mulsc( &R, &R, 10 );
        mp_mulsc( &Mn, &Mn, 10 );
        mp_mulsc( &Mp, &Mp, 10 );
    }
    /* assert  k = min( 0, 1 + floor( logv ) ) */
    mp_mulsc( &mptemp, &R, 2 );
    mp_add( &mptemp, &mptemp, &Mp );
    for(;;) {
        mp_mulsc( &mptemp2, &S, 2 );
        if( mp_lt( &mptemp, &mptemp2 ) ) break;
        mp_mulsc( &S, &S, 10 );
        k++;
    }
    *H = k - 1;
    for(;;) {
        k--;
        mp_mulsc( &R, &R, 10 );
        mp_div( &U, &R, &R, &S );
        mp_mulsc( &Mn, &Mn, 10 );
        mp_mulsc( &Mp, &Mp, 10 );
        mp_mulsc( &mptemp, &R, 2 );
        mp_mulsc( &mptemp2, &S, 2 );
        mp_sub( &mptemp2, &mptemp2, &Mp );
        low = mp_lt( &mptemp, &Mn );
        high = mp_gt( &mptemp, &mptemp2 );
        if( high || low ) break;
        D[index++] = U.num[0] + '0';
    }
    if( low && high ) {
        mp_mulsc( &mptemp, &R, 2 );
        if( mp_lte( &mptemp, &S ) ) {
            D[index++] = U.num[0] + '0';
        } else {
            D[index++] = U.num[0] + '1';
        }
    } else if( low ) {
        D[index++] = U.num[0] + '0';
    } else if( high ) {
        D[index++] = U.num[0] + '1';
    }
    D[index] = '\0';
    *N = k;
    mp_free( &f );
    mp_free( &R );
    mp_free( &S );
    mp_free( &Mn );
    mp_free( &Mp );
    mp_free( &U );
    mp_free( &mptemp );
    mp_free( &mptemp2 );
    return 0;
}

/* This function will round the decimal string at the given location and then
 * set the character after the location to NULL.
 * Return 1 when a number like "99999" rounds to "1". (Most significant bit
 * is in a new location.)
 */
int RoundString( char *str, int loc )
{
    int cur = loc + 1;
    int round = 0;

    if( str[cur] > '5' ) {
        round = 1;  /* round up */
    } else if( str[cur] < '5' ) {
        round = -1; /* round down */
    } else {
        cur++;
        while( str[cur] != '\0' ) {
            if( str[cur] > '0' ) {
                round = 1; /* round up */
                break;
            }
            cur++;
        }
        if( round == 0 ) {
            if( (str[loc] - '0') % 2 == 0 ) {
                round = -1; /* round down */
            } else {
                round = 1; /* round up */
            }
        }
    }
    cur = loc;
    if( round == 1 ) {
        while( cur >= 0 && str[cur] == '9' ) {
            str[cur] = '0';
            cur--;
        }
        if( cur == -1 ) {
            str[0] = '1';
            str[1] = '\0';
            return 1;
        }
        str[cur] = str[cur] + 1;
    }
    str[cur+1] = '\0';

    return 0;
}

/* This function formats a string generated by the GenerateString function.
 * The formated string will use E notation of the number is between 1e+10
 * and 1e-4 and will just output the decimal number in normal notation
 * otherwise.  If there isn't enough room to fit the full mantissa and the
 * exponent, it rounds the mantissa.
 * Returns -1 if maxlen is too small to display a rounded mantissa and
 * exponent in E notation.
 */
int FormatString( char *dst, char *src, int least, int most, int maxlen )
{
    int i, j, src_index = 0, dst_index = 0, exp_size;

    if( most > 9 || most < -3 || most - least >= maxlen ) {
        /* use E notation */
        char buf[10];

        /* determine size of exponent part */
        if( most > 99 || most < -99 ) {
            exp_size = 5;
        } else if( most > 9 || most < -9 ) {
            exp_size = 4;
        } else {
            exp_size = 3;
        }
        if( maxlen < exp_size + 3 ) return -1; /* not enough room */

        if( most - least + exp_size >= maxlen ) {
            /* need to round */
            if( RoundString( src, maxlen - exp_size - 3 ) == 1 ) {
                most++;
            }
        }

        dst[dst_index++] = src[src_index++];
        if( most - least > 0 ) dst[dst_index++] = '.';
        for( i = most - 1; i >= least; i-- ) {
            if( src[src_index] == '\0' ) break;
            dst[dst_index++] = src[src_index++];
        }
        dst[dst_index++] = 'E';
        if( most > 0 ) dst[dst_index++] = '+';
        itoa( most, buf, 10 );
        i = 0;
        while( buf[i] != '\0' ) {
            dst[dst_index++] = buf[i++];
        }
    } else {
        i = most;
        j = least;
        if( i < 0 ) i = 0;
        if( j > 0 ) j = 0;
        for( ; i >= j; i-- ) {
            if( i > most ) {
                if( i == -1 ) dst[dst_index++] = '.';
                dst[dst_index++] = '0';
            } else if( i < least ) {
                dst[dst_index++] = '0';
            } else {
                if( i == -1 ) dst[dst_index++] = '.';
                dst[dst_index++] = src[src_index++];
            }
        }
    }
    dst[dst_index] = '\0';
    return 0;
}

/* Convert a binary floating-point number to a string */
int r2a( char *dst, void *src, int type, int maxlen )
{
    int rc;
    int N, H;
    char str[20];

    rc = GenerateString( str, src, type, &N, &H );
    rc = FormatString( dst, str, N, H, maxlen );

    return rc;
}

int e2a( char *ascii, char *ext, int maxlen )
{
    /* NYI */
    return( r2a( ascii, ext, EXTENDED, maxlen ) );
}

int d2a( char *ascii, double *dbl, int maxlen )
{
    /* first check special cases */
    if( *(uint64*)dbl == DOUBLE_POS_ZERO ) {
        strcpy( ascii, ASCII_POS_ZERO );
    } else if( *(uint64*)dbl == DOUBLE_NEG_ZERO ) {
        strcpy( ascii, ASCII_NEG_ZERO );
    } else if( *(uint64*)dbl == DOUBLE_POS_INF ) {
        strcpy( ascii, ASCII_POS_INF );
    } else if( *(uint64*)dbl == DOUBLE_NEG_INF ) {
        strcpy( ascii, ASCII_NEG_INF );
    } else if( d_isNan( dbl ) ) {
        strcpy( ascii, ASCII_NAN );
    } else {
        /* handle negative sign */
        if( d_isNeg( dbl ) ) {
            *ascii = '-';
            ascii++;
            dneg( dbl, dbl );
            maxlen--;
        }
        return( r2a( ascii, dbl, DOUBLE, maxlen ) );
    }
    return 0;
}

int f2a( char *ascii, float *flt, int maxlen )
{
    /* first check special cases */
    if( *(uint32*)flt == FLOAT_POS_ZERO ) {
        strcpy( ascii, ASCII_POS_ZERO );
    } else if( *(uint32*)flt == FLOAT_NEG_ZERO ) {
        strcpy( ascii, ASCII_NEG_ZERO );
    } else if( *(uint32*)flt == FLOAT_POS_INF ) {
        strcpy( ascii, ASCII_POS_INF );
    } else if( *(uint32*)flt == FLOAT_NEG_INF ) {
        strcpy( ascii, ASCII_NEG_INF );
    } else if( f_isNan( flt ) ) {
        strcpy( ascii, ASCII_NAN );
    } else {
        /* handle negative sign */
        if( f_isNeg( flt ) ) {
            *ascii = '-';
            ascii++;
            fneg( flt, flt );
            maxlen--;
        }
        return( r2a( ascii, flt, FLOAT, maxlen ) );
    }
    return 0;
}

