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


#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "cfloati.h"


static void efInit( char ue[] )
{
    int     i;

    for( i = 0; i < CF_MAX_PREC; i++ ) {
        ue[i] = 0;
    }
}

static int efGet( cfloat *u, char ue[], int i )
{
    if( i >= u->len ) {
        return( ue[i - u->len] );
    } else {
        return( u->mant[i] - '0' );
    }
}

static void efSet( cfloat *u, char ue[], int i, int val )
{
    if( i >= u->len ) {
        ue[i - u->len] = (char)val;
    } else {
        u->mant[i] = (char)val + '0';
    }
}

static cfloat *scalarMultiply( cfloat *f, int s )
{
    cfloat      *res;
    div_t       d;
    int         i;

    res = CFAlloc( f->len + 1 );

    res->len  = f->len + 1;
    res->exp  = f->exp;
    res->sign = f->sign;

    d.quot = 0;

    for( i = f->len; i > 0; i-- ) {
        d = div( s * CFAccess( f, i - 1 ) + d.quot, 10 );
        CFDeposit( res, i, d.rem );
    }
    CFDeposit( res, 0, d.quot );
    return( res );
}

static void expandCF( cfloat **f, int scale )
{
    cfloat      *new;
    int         new_len;
    int         old_len;

    old_len = (*f)->len;
    new_len = old_len + scale;
    new = CFAlloc( new_len );
    memcpy( new, *f, offsetof( cfloat, mant ) + old_len );
    while( old_len < new_len ) {
        new->mant[old_len++] = '0';
    }
    new->mant[old_len] = NULLCHAR;
    new->len  = new_len;

    CFFree( *f );

    *f = new;
}

static void roundupCF( cfloat *f )
{
    int     i;

    for( i = f->len - 1; i >= 0; i-- ) {
        if( f->mant[i] == '9' ) {
            f->mant[i] = '0';
        } else {
            f->mant[i] += 1;
            return;
        }
    }

    f->mant[0] = '1';
    f->exp += 1;
}

/*
 * CFDiv:  Computes  op1 / op2
 */
cfloat  *CFDiv( cfloat *op1, cfloat *op2 )
{
    cfloat         *result;
    cfloat         *u, *v;
    int             i, qa, ua, va, v1, cy, scale;
    int             j;
    div_t           d;
    char            ue[CF_MAX_PREC];

    if( ! op2->sign ) {                         // Attempt to divide by zero.
        result = CFAlloc( 1 );
        result->mant[0] = '1';
        result->sign    = 1;
        result->exp     = CF_ERR_EXP;           // Return error-type.
        return( result );
    }

    efInit( ue );                               // Initialize extended float.

    result  = CFAlloc( CF_MAX_PREC );           // Allocate mem. for result.

    result->sign = op1->sign * op2->sign;       // Set sign of result.
    result->exp  = op1->exp - op2->exp + 1;     // Set exponent of result.
    result->len  = 0;

    if( CFAccess( op2, 0 ) < 5 ) {
        scale = 10 / (CFAccess( op2, 0 ) + 1);
    } else {
        scale = 1;
    }

    u = scalarMultiply( op1, scale );           // Extra digit added.
    v = scalarMultiply( op2, scale );           // Extra digit added.

    if( v->len < 3 ) {                          // Divisor must have at least
        expandCF( &v, 1 );                      // two digits (ignore leading 0)
    }
    if( v->len >= u->len ) {                    // Dividend must have more
        expandCF( &u, v->len - u->len + 1 );    // digits than divisor.
    }

    /*
     * We now use the classical division algorithm described in Knuth,
     * _Seminumerical_Algorithms_, section 4.3.1.
     *
     * The following implementation uses base 10, and the initial approximation
     * is made by taking qa = floor( uj.uj1.uj2 / v1.v2 ) instead of simply
     * floor( uj.uj1 / v1 ).  According to Knuth, this initial approximation
     * is always greater than the real quotient digit, and off by at most two.
     */

    v1 = CFAccess( v, 1 );
    va = 10 * v1 + CFAccess( v, 2 );

    for( j = 0; j <= CF_MAX_PREC; j++ ) {

        // Make initial approximation of the quotient digit.

        if( v1 == efGet( u, ue, j ) ) {
            qa = 9;
        } else {
            ua = efGet(u,ue,j) * 100 + efGet(u,ue,j+1) * 10 + efGet(u,ue,j+2);
            qa = ua / va;
        }

        /*
         * Replace (uj.uj1...ujn) with (uj.uj1..ujn) - qa * (v1.v2..vn)
         */

        cy = 0;
        for( i = v->len - 1; i >= 0; i-- ) {
            d = div( efGet( u, ue, i + j ) - qa * CFAccess( v, i ) + cy, 10 );
            if( d.rem < 0 ) {
                d.rem  += 10;
                d.quot -= 1;
            }
            cy = d.quot;
            efSet( u, ue, i + j, d.rem );
        }

        /*
         * The above subtraction resulted in a negative number.  So qa was
         * in fact off by one.  Correct that, and add back to correct
         * (uj.uj1...ujn).
         */

        if( cy ) {
            qa--;
            cy = 0;
            for( i = v->len - 1; i >= 0; i-- ) {
                d = div( efGet( u, ue, i + j ) + CFAccess( v, i ) + cy, 10 );
                efSet( u, ue, i + j, d.rem );
                cy = d.quot;
            }
        }

        /*
         * Set the real quotient digit, and do some rounding when maximum
         * precision is reached.  [Rounding is unbiased.]
         */

        if( j < CF_MAX_PREC ) {
            CFDeposit( result, j, qa );
            result->len++;
        } else {
            if( qa > 5 ) {
                roundupCF( result );
            } else if( qa == 5 ) {
                cy = 0;
                for( i = v->len - 1; i >= 0; i-- ) {
                    if( efGet( u, ue, i + j ) != 0 ) {
                        cy = 1;
                        break;
                    }
                }
                if( cy ) {
                    roundupCF( result );
                } else if( CFAccess( result, CF_MAX_PREC - 1 ) % 2 != 0 ) {
                    roundupCF( result );
                }
            }
        }
    }

    CFFree( u );                            // Clean up the mess we made.
    CFFree( v );
    CFClean( result );                      // Clean up the number we made.

    return( result );
}
