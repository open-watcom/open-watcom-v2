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


/* C implementation of floating point arithmetic, conversion, and comparison
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "mp.h"
#include "fp.h"

/* c implementation of dadd that may be used in special cases for the non-c
 * version of dadd */
void c_dadd( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    int diff;
    int emax, emin;
    mpnum num1, num2, zero;
    int isNeg1 = d_isNeg( src1 );
    int isNeg2 = d_isNeg( src2 );
    int isNeg = FALSE;
    int newexp;

    /* special cases */
    if( d_isNan( src1 ) || d_isNan( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isPosInf( src1 ) && d_isNegInf( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isNegInf( src1 ) && d_isPosInf( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isPosInf( src1 ) || d_isPosInf( src2 ) ) {
        *(uint64*)dst = DOUBLE_POS_INF;
    } else if( d_isNegInf( src1 ) || d_isNegInf( src2 ) ) {
        *(uint64*)dst = DOUBLE_NEG_INF;
    } else if( d_isPosZero( src1 ) && d_isNegZero( src2 ) ) {
        *(uint64*)dst = DOUBLE_POS_ZERO;
    } else if( d_isNegZero( src1 ) && d_isPosZero( src2 ) ) {
        *(uint64*)dst = DOUBLE_POS_ZERO;
    } else if( d_isPosZero( src1 ) && d_isPosZero( src2 ) ) {
        *(uint64*)dst = DOUBLE_POS_ZERO;
    } else if( d_isNegZero( src1 ) && d_isNegZero( src2 ) ) {
        *(uint64*)dst = DOUBLE_NEG_ZERO;
    } else if( d_isZero( src1 ) ) {
        *(uint64*)dst = *(uint64*)src2;
    } else if( d_isZero( src2 ) ) {
        *(uint64*)dst = *(uint64*)src1;
    } else {
        parseDouble( *(double*)src1, &f1, &e1 );
        parseDouble( *(double*)src2, &f2, &e2 );
        diff = abs( (e1 - e2) );
        emax = max( e1, e2 );
        emin = min( e1, e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_init( &zero, 0 );
        if( e1 > e2 ) {
            mp_shiftleft( &num1, &num1, (uint64)diff );
        } else if( e2 > e1 ) {
            mp_shiftleft( &num2, &num2, (uint64)diff );
        }
        if( (isNeg1 && isNeg2) || (!isNeg1 && !isNeg2 ) ) {
            mp_add( &num1, &num1, &num2 );
            if( isNeg1 ) isNeg = TRUE;
        } else {
            if( mp_gt( &num1, &num2 ) ) {
                mp_sub( &num1, &num1, &num2 );
                if( isNeg1 ) isNeg = TRUE;
            } else {
                mp_sub( &num1, &num2, &num1 );
                if( isNeg2 ) isNeg = TRUE;
            }
        }
        if( mp_eq( &num1, &zero ) ) {
            *(uint64*)dst = DOUBLE_POS_ZERO;
        } else {
            newexp = emax - diff - 53 + mp_bitsize( &num1 ) + double_bias;
            if( newexp <= 0 && newexp > -53 ) {
                /* denormalized result */
                uint64 temp;
                mp_binround( &num1, &num1, -(emin + double_bias) + 1 );
                mp_shiftright( &num1, &num1, -(emin + double_bias) + 1 );
                mp_touint64( &temp, &num1 );
                makeDouble( dst, temp, 0 - double_bias );
            } else {
                mp_todouble( dst, &num1 );
                changeDoubleExponent( dst, dst, emax - diff - 52 );
            }
            if( isNeg ) dneg( dst, dst );
        }
        mp_free( &num1 );
        mp_free( &num2 );
        mp_free( &zero );
    }
}

void c_dsub( void *dst, void *src1, void *src2 )
{
    double nsrc2;
    dneg( &nsrc2, src2 );
    dadd( dst, src1, &nsrc2 );
}

#ifdef C_IMPL
void dadd( void *dst, void *src1, void *src2 ) {
    c_dadd( dst, src1, src2 );
}

void dsub( void *dst, void *src1, void *src2 ) {
    c_dsub( dst, src1, src2 );
}

void dmul( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    mpnum num1, num2;
    int isNeg1 = d_isNeg( src1 );
    int isNeg2 = d_isNeg( src2 );
    int isDstNeg = (isNeg1 && !isNeg2) || (!isNeg1 && isNeg2);
    int newexp;

    /* special cases */
    if( d_isNan( src1 ) || d_isNan( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isInf( src1 ) && d_isZero( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isZero( src1 ) && d_isInf( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isInf( src1 ) ) {
        *(uint64*)dst = DOUBLE_POS_INF;
        if( isDstNeg ) dneg( dst, dst );
    } else if( d_isInf( src2 ) ) {
        *(uint64*)dst = DOUBLE_POS_INF;
        if( isDstNeg ) dneg( dst, dst );
    } else if( d_isZero( src1 ) || d_isZero( src2 ) ) {
        *(uint64*)dst = DOUBLE_POS_ZERO;
        if( isDstNeg ) dneg( dst, dst );
    } else {
        parseDouble( *(double*)src1, &f1, &e1 );
        parseDouble( *(double*)src2, &f2, &e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_mul( &num1, &num1, &num2 );
        newexp = e1 + e2 - 105 + mp_bitsize( &num1 ) + double_bias;
        if( newexp <= 0 && newexp > -53 ) {
            /* denormalized result */
            uint64 temp;
            mp_binround( &num1, &num1, -newexp + 53 );
            mp_shiftright( &num1, &num1, -newexp + 53 );
            mp_touint64( &temp, &num1 );
            makeDouble( dst, temp, 0 - double_bias );
        } else {
            mp_todouble( dst, &num1 );
            changeDoubleExponent( dst, dst, e1 + e2 - 104 );
        }
        if( isDstNeg ) dneg( dst, dst );
        mp_free( &num1 );
        mp_free( &num2 );
    }
}

void ddiv( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    mpnum num1, num2, rem;
    int isNeg1 = d_isNeg( src1 );
    int isNeg2 = d_isNeg( src2 );
    int isDstNeg = (isNeg1 && !isNeg2) || (!isNeg1 && isNeg2);
    int newexp, rc;
    int adjust = 0;

    /* special cases */
    if( d_isNan( src1 ) || d_isNan( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isInf( src1 ) && d_isInf( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isInf( src1 ) ) {
        *(uint64*)dst = DOUBLE_POS_INF;
        if( isDstNeg ) dneg( dst, dst );
    } else if( d_isInf( src2 ) ) {
        *(uint64*)dst = DOUBLE_POS_ZERO;
        if( isDstNeg ) dneg( dst, dst );
    } else if( d_isZero( src1 ) && d_isZero( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isZero( src1 ) ) {
        *(uint64*)dst = DOUBLE_POS_ZERO;
        if( isDstNeg ) dneg( dst, dst );
    } else if( d_isZero( src2 ) ) {
        *(uint64*)dst = DOUBLE_POS_INF;
        if( isDstNeg ) dneg( dst, dst );
    } else {
        parseDouble( *(double*)src1, &f1, &e1 );
        parseDouble( *(double*)src2, &f2, &e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_init( &rem, 0 );
        if( mp_lt( &num1, &num2 ) ) {
            adjust = 1;
            e1--;
        }
        newexp = e1 - e2 + double_bias;
        if( newexp <= 0 && newexp > -52 ) {
            mp_shiftleft( &num1, &num1, 51 + newexp + adjust );
        } else {
            mp_shiftleft( &num1, &num1, 52 + adjust );
        }
        mp_div( &num1, &rem, &num1, &num2 );
        mp_mulsc( &rem, &rem, 2 );
        rc = mp_cmp( &rem, &num2 );
        if( (rc == 0 && (rem.num[0] & 2)) || rc == -1 ) {
            /* round up */
            mp_addsc( &num1, &num1, 1 );
        }
        if( newexp <= 0 && newexp > -52 ) {
            /* denormalized result */
            uint64 temp;
            mp_touint64( &temp, &num1 );
            makeDouble( dst, temp, 0 - double_bias );
        } else {
            mp_todouble( dst, &num1 );
            changeDoubleExponent( dst, dst, e1 - e2 - 52 );
        }
        if( isDstNeg ) dneg( dst, dst );
        mp_free( &num1 );
        mp_free( &num2 );
        mp_free( &rem );
    }
}

void drem( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    mpnum num1, num2, zero;
    int emin;
    int newexp;

    /* special cases */
    if( d_isNan( src1 ) || d_isNan( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isInf( src1 ) || d_isZero( src2 ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( d_isZero( src1 ) || d_isInf( src2 ) ) {
        *(uint64*)dst = *(uint64*)src1;
    } else {
        parseDouble( *(double*)src1, &f1, &e1 );
        parseDouble( *(double*)src2, &f2, &e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_init( &zero, 0 );
        emin = min( e1, e2 );
        if( e2 > e1 ) {
            mp_shiftleft( &num2, &num2, e2 - e1 );
        } else {
            mp_shiftleft( &num1, &num1, e1 - e2 );
        }
        mp_div( &num1, &num2, &num1, &num2 ); /* num2 will get the remainder */
        if( mp_eq( &num2, &zero ) ) {
            *(uint64*)dst = DOUBLE_POS_ZERO;
        } else {
            newexp = emin - 53 + mp_bitsize( &num2 ) + double_bias;
            if( newexp <= 0 && newexp > -53 ) {
                /* denormalized result */
                uint64 temp;
                mp_binround( &num2, &num2, -newexp + 1 );
                mp_shiftright( &num2, &num2, -newexp + 1 );
                mp_touint64( &temp, &num2 );
                makeDouble( dst, temp, 0 - double_bias );
            } else {
                mp_todouble( dst, &num2 );
                changeDoubleExponent( dst, dst, emin - 52 );
            }
        }
        if( d_isNeg( src1 ) ) dneg( dst, dst );
        mp_free( &num1 );
        mp_free( &num2 );
        mp_free( &zero );
    }
}

void dneg( void *dst, void *src ) {
    memcpy( dst, src, 8 );
    if( !d_isNan( dst ) ) {
        ((char*)dst)[7] ^= 128;
    }
}

void fadd( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    int diff;
    int emax, emin;
    mpnum num1, num2, zero;
    int isNeg1 = f_isNeg( src1 );
    int isNeg2 = f_isNeg( src2 );
    int isNeg = FALSE;
    int newexp;

    /* special cases */
    if( f_isNan( src1 ) || f_isNan( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isPosInf( src1 ) && f_isNegInf( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isNegInf( src1 ) && f_isPosInf( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isPosInf( src1 ) || f_isPosInf( src2 ) ) {
        *(uint32*)dst = FLOAT_POS_INF;
    } else if( f_isNegInf( src1 ) || f_isNegInf( src2 ) ) {
        *(uint32*)dst = FLOAT_NEG_INF;
    } else if( f_isPosZero( src1 ) && f_isNegZero( src2 ) ) {
        *(uint32*)dst = FLOAT_POS_ZERO;
    } else if( f_isNegZero( src1 ) && f_isPosZero( src2 ) ) {
        *(uint32*)dst = FLOAT_POS_ZERO;
    } else if( f_isPosZero( src1 ) && f_isPosZero( src2 ) ) {
        *(uint32*)dst = FLOAT_POS_ZERO;
    } else if( f_isNegZero( src1 ) && f_isNegZero( src2 ) ) {
        *(uint32*)dst = FLOAT_NEG_ZERO;
    } else if( f_isZero( src1 ) ) {
        *(uint32*)dst = *(uint32*)src2;
    } else if( f_isZero( src2 ) ) {
        *(uint32*)dst = *(uint32*)src1;
    } else {
        parseFloat( *(float*)src1, &f1, &e1 );
        parseFloat( *(float*)src2, &f2, &e2 );
        diff = abs( (e1 - e2) );
        emax = max( e1, e2 );
        emin = min( e1, e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_init( &zero, 0 );
        if( e1 > e2 ) {
            mp_shiftleft( &num1, &num1, (uint64)diff );
        } else if( e2 > e1 ) {
            mp_shiftleft( &num2, &num2, (uint64)diff );
        }
        if( (isNeg1 && isNeg2) || (!isNeg1 && !isNeg2 ) ) {
            mp_add( &num1, &num1, &num2 );
            if( isNeg1 ) isNeg = TRUE;
        } else {
            if( mp_gt( &num1, &num2 ) ) {
                mp_sub( &num1, &num1, &num2 );
                if( isNeg1 ) isNeg = TRUE;
            } else {
                mp_sub( &num1, &num2, &num1 );
                if( isNeg2 ) isNeg = TRUE;
            }
        }
        if( mp_eq( &num1, &zero ) ) {
            *(uint32*)dst = FLOAT_POS_ZERO;
        } else {
            newexp = emax - diff - 24 + mp_bitsize( &num1 ) + float_bias;
            if( newexp <= 0 && newexp > -24 ) {
                /* denormalized result */
                uint64 temp;
                mp_binround( &num1, &num1, -(emin + float_bias) + 1 );
                mp_shiftright( &num1, &num1, -(emin + float_bias) + 1 );
                mp_touint64( &temp, &num1 );
                makeFloat( dst, temp, 0 - float_bias );
            } else {
                mp_tofloat( dst, &num1 );
                changeFloatExponent( dst, dst, emax - diff - 23 );
            }
            if( isNeg ) fneg( dst, dst );
        }
        mp_free( &num1 );
        mp_free( &num2 );
        mp_free( &zero );
    }
}

void fsub( void *dst, void *src1, void *src2 ) {
    float nsrc2;
    fneg( &nsrc2, src2 );
    fadd( dst, src1, &nsrc2 );
}

void fmul( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    mpnum num1, num2;
    int isNeg1 = f_isNeg( src1 );
    int isNeg2 = f_isNeg( src2 );
    int isDstNeg = (isNeg1 && !isNeg2) || (!isNeg1 && isNeg2);
    int newexp;

    /* special cases */
    if( f_isNan( src1 ) || f_isNan( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isInf( src1 ) && f_isZero( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isZero( src1 ) && f_isInf( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isInf( src1 ) ) {
        *(uint32*)dst = FLOAT_POS_INF;
        if( isDstNeg ) fneg( dst, dst );
    } else if( f_isInf( src2 ) ) {
        *(uint32*)dst = FLOAT_POS_INF;
        if( isDstNeg ) fneg( dst, dst );
    } else if( f_isZero( src1 ) || f_isZero( src2 ) ) {
        *(uint32*)dst = FLOAT_POS_ZERO;
        if( isDstNeg ) fneg( dst, dst );
    } else {
        parseFloat( *(float*)src1, &f1, &e1 );
        parseFloat( *(float*)src2, &f2, &e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_mul( &num1, &num1, &num2 );
        newexp = e1 + e2 - 47 + mp_bitsize( &num1 ) + float_bias;
        if( newexp <= 0 && newexp > -24 ) {
            /* denormalized result */
            uint64 temp;
            mp_binround( &num1, &num1, -newexp + 24 );
            mp_shiftright( &num1, &num1, -newexp + 24 );
            mp_touint64( &temp, &num1 );
            makeFloat( dst, temp, 0 - float_bias );
        } else {
            mp_tofloat( dst, &num1 );
            changeFloatExponent( dst, dst, e1 + e2 - 46 );
        }
        if( isDstNeg ) fneg( dst, dst );
        mp_free( &num1 );
        mp_free( &num2 );
    }
}

void fdiv( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    mpnum num1, num2, rem;
    int isNeg1 = f_isNeg( src1 );
    int isNeg2 = f_isNeg( src2 );
    int isDstNeg = (isNeg1 && !isNeg2) || (!isNeg1 && isNeg2);
    int newexp, rc;
    int adjust = 0;

    /* special cases */
    if( f_isNan( src1 ) || f_isNan( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isInf( src1 ) && f_isInf( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isInf( src1 ) ) {
        *(uint32*)dst = FLOAT_POS_INF;
        if( isDstNeg ) fneg( dst, dst );
    } else if( f_isInf( src2 ) ) {
        *(uint32*)dst = FLOAT_POS_ZERO;
        if( isDstNeg ) fneg( dst, dst );
    } else if( f_isZero( src1 ) && f_isZero( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isZero( src1 ) ) {
        *(uint32*)dst = FLOAT_POS_ZERO;
        if( isDstNeg ) fneg( dst, dst );
    } else if( f_isZero( src2 ) ) {
        *(uint32*)dst = FLOAT_POS_INF;
        if( isDstNeg ) fneg( dst, dst );
    } else {
        parseFloat( *(float*)src1, &f1, &e1 );
        parseFloat( *(float*)src2, &f2, &e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_init( &rem, 0 );
        if( mp_lt( &num1, &num2 ) ) {
            adjust = 1;
            e1--;
        }
        newexp = e1 - e2 + float_bias;
        if( newexp <= 0 && newexp > -23 ) {
            mp_shiftleft( &num1, &num1, 22 + newexp + adjust );
        } else {
            mp_shiftleft( &num1, &num1, 23 + adjust );
        }
        mp_div( &num1, &rem, &num1, &num2 );
        mp_mulsc( &rem, &rem, 2 );
        rc = mp_cmp( &rem, &num2 );
        if( (rc == 0 && (rem.num[0] & 2)) || rc == -1 ) {
            /* round up */
            mp_addsc( &num1, &num1, 1 );
        }
        if( newexp <= 0 && newexp > -23 ) {
            /* denormalized result */
            uint64 temp;
            mp_touint64( &temp, &num1 );
            makeFloat( dst, temp, 0 - float_bias );
        } else {
            mp_tofloat( dst, &num1 );
            changeFloatExponent( dst, dst, e1 - e2 - 23 );
        }
        if( isDstNeg ) fneg( dst, dst );
        mp_free( &num1 );
        mp_free( &num2 );
        mp_free( &rem );
    }
}

void frem( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    mpnum num1, num2, zero;
    int emin;
    int newexp;

    /* special cases */
    if( f_isNan( src1 ) || f_isNan( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isInf( src1 ) || f_isZero( src2 ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( f_isZero( src1 ) || f_isInf( src2 ) ) {
        *(uint32*)dst = *(uint32*)src1;
    } else {
        parseFloat( *(float*)src1, &f1, &e1 );
        parseFloat( *(float*)src2, &f2, &e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_init( &zero, 0 );
        emin = min( e1, e2 );
        if( e2 > e1 ) {
            mp_shiftleft( &num2, &num2, e2 - e1 );
        } else {
            mp_shiftleft( &num1, &num1, e1 - e2 );
        }
        mp_div( &num1, &num2, &num1, &num2 ); /* num2 will get the remainder */
        if( mp_eq( &num2, &zero ) ) {
            *(uint32*)dst = FLOAT_POS_ZERO;
        } else {
            newexp = emin - 24 + mp_bitsize( &num2 ) + float_bias;
            if( newexp <= 0 && newexp > -24 ) {
                /* denormalized result */
                uint64 temp;
                mp_binround( &num2, &num2, -newexp + 1 );
                mp_shiftright( &num2, &num2, -newexp + 1 );
                mp_touint64( &temp, &num2 );
                makeFloat( dst, temp, 0 - float_bias );
            } else {
                mp_tofloat( dst, &num2 );
                changeFloatExponent( dst, dst, emin - 23 );
            }
        }
        if( f_isNeg( src1 ) ) fneg( dst, dst );
        mp_free( &num1 );
        mp_free( &num2 );
        mp_free( &zero );
    }
}

void fneg( void *dst, void *src ) {
    memcpy( dst, src, 4 );
    if( !d_isNan( dst ) ) {
        ((char*)dst)[3] ^= 128;
    }
}

void eadd( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    int diff;
    int emax, emin;
    mpnum num1, num2, zero;
    int isNeg1 = e_isNeg( src1 );
    int isNeg2 = e_isNeg( src2 );
    int isNeg;
    int newexp;

    /* special cases */
    if( e_isNan( src1 ) || e_isNan( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isPosInf( src1 ) && e_isNegInf( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isNegInf( src1 ) && e_isPosInf( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isPosInf( src1 ) || e_isPosInf( src2 ) ) {
        makeExtPosInf( dst );
    } else if( e_isNegInf( src1 ) || e_isNegInf( src2 ) ) {
        makeExtNegInf( dst );
    } else if( e_isPosZero( src1 ) && e_isNegZero( src2 ) ) {
        makeExtPosZero( dst );
    } else if( e_isNegZero( src1 ) && e_isPosZero( src2 ) ) {
        makeExtPosZero( dst );
    } else if( e_isPosZero( src1 ) && e_isPosZero( src2 ) ) {
        makeExtPosZero( dst );
    } else if( e_isNegZero( src1 ) && e_isNegZero( src2 ) ) {
        makeExtNegZero( dst );
    } else if( e_isZero( src1 ) ) {
        memcpy( dst, src2, 10 );
    } else if( e_isZero( src2 ) ) {
        memcpy( dst, src1, 10 );
    } else {
        parseExtended( (char*)src1, &f1, &e1 );
        parseExtended( (char*)src2, &f2, &e2 );
        diff = abs( (e1 - e2) );
        emax = max( e1, e2 );
        emin = min( e1, e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_init( &zero, 0 );
        if( e1 > e2 ) {
            mp_shiftleft( &num1, &num1, (uint64)diff );
        } else if( e2 > e1 ) {
            mp_shiftleft( &num2, &num2, (uint64)diff );
        }
        if( (isNeg1 && isNeg2) || (!isNeg1 && !isNeg2 ) ) {
            mp_add( &num1, &num1, &num2 );
            if( isNeg1 ) isNeg = TRUE;
        } else {
            if( mp_gt( &num1, &num2 ) ) {
                mp_sub( &num1, &num1, &num2 );
                if( isNeg1 ) isNeg = TRUE;
            } else {
                mp_sub( &num1, &num2, &num1 );
                if( isNeg2 ) isNeg = TRUE;
            }
        }
        if( mp_eq( &num1, &zero ) ) {
            makeExtPosZero( dst );
        } else {
            newexp = emax - diff - 64 + mp_bitsize( &num1 ) + extended_bias;
            if( newexp <= 0 && newexp > -64 ) {
                /* denormalized result */
                uint64 temp;
                mp_binround( &num1, &num1, -(emin + extended_bias) + 1 );
                mp_shiftright( &num1, &num1, -(emin + extended_bias) + 1 );
                mp_touint64( &temp, &num1 );
                makeExtended( dst, temp, 0 - extended_bias );
            } else {
                mp_toextended( dst, &num1 );
                changeExtendedExponent( dst, dst, emax - diff - 63 );
            }
            if( isNeg ) eneg( dst, dst );
        }
        mp_free( &num1 );
        mp_free( &num2 );
        mp_free( &zero );
    }
}

void esub( void *dst, void *src1, void *src2 ) {
    char nsrc2[10];
    eneg( &nsrc2, src2 );
    eadd( dst, src1, &nsrc2 );
}

void emul( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    mpnum num1, num2;
    int isNeg1 = e_isNeg( src1 );
    int isNeg2 = e_isNeg( src2 );
    int isDstNeg = (isNeg1 && !isNeg2) || (!isNeg1 && isNeg2);
    int newexp;

    /* special cases */
    if( e_isNan( src1 ) || e_isNan( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isInf( src1 ) && e_isZero( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isZero( src1 ) && e_isInf( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isInf( src1 ) ) {
        makeExtPosInf( dst );
        if( isDstNeg ) eneg( dst, dst );
    } else if( e_isInf( src2 ) ) {
        makeExtPosInf( dst );
        if( isDstNeg ) eneg( dst, dst );
    } else if( e_isZero( src1 ) || e_isZero( src2 ) ) {
        makeExtPosZero( dst );
        if( isDstNeg ) eneg( dst, dst );
    } else {
        parseExtended( (char*)src1, &f1, &e1 );
        parseExtended( (char*)src2, &f2, &e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_mul( &num1, &num1, &num2 );
        newexp = e1 + e2 - 127 + mp_bitsize( &num1 ) + extended_bias;
        if( newexp <= 0 && newexp > -64 ) {
            /* denormalized result */
            uint64 temp;
            mp_binround( &num1, &num1, -newexp + 64 );
            mp_shiftright( &num1, &num1, -newexp + 64 );
            mp_touint64( &temp, &num1 );
            makeExtended( dst, temp, 0 - extended_bias );
        } else {
            mp_toextended( dst, &num1 );
            changeExtendedExponent( dst, dst, e1 + e2 - 126 );
        }
        if( isDstNeg ) eneg( dst, dst );
        mp_free( &num1 );
        mp_free( &num2 );
    }
}

void ediv( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    mpnum num1, num2, rem;
    int isNeg1 = e_isNeg( src1 );
    int isNeg2 = e_isNeg( src2 );
    int isDstNeg = (isNeg1 && !isNeg2) || (!isNeg1 && isNeg2);
    int newexp, rc;
    int adjust = 0;

    /* special cases */
    if( e_isNan( src1 ) || e_isNan( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isInf( src1 ) && e_isInf( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isInf( src1 ) ) {
        makeExtPosInf( dst );
        if( isDstNeg ) eneg( dst, dst );
    } else if( e_isInf( src2 ) ) {
        makeExtPosZero( dst );
        if( isDstNeg ) eneg( dst, dst );
    } else if( e_isZero( src1 ) && e_isZero( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isZero( src1 ) ) {
        makeExtPosZero( dst );
        if( isDstNeg ) eneg( dst, dst );
    } else if( e_isZero( src2 ) ) {
        makeExtPosInf( dst );
        if( isDstNeg ) eneg( dst, dst );
    } else {
        parseExtended( (char*)src1, &f1, &e1 );
        parseExtended( (char*)src2, &f2, &e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_init( &rem, 0 );
        if( mp_lt( &num1, &num2 ) ) {
            adjust = 1;
            e1--;
        }
        newexp = e1 - e2 + extended_bias;
        if( newexp <= 0 && newexp > -63 ) {
            mp_shiftleft( &num1, &num1, 62 + newexp + adjust );
        } else {
            mp_shiftleft( &num1, &num1, 63 + adjust );
        }
        mp_div( &num1, &rem, &num1, &num2 );
        mp_mulsc( &rem, &rem, 2 );
        rc = mp_cmp( &rem, &num2 );
        if( (rc == 0 && (rem.num[0] & 2)) || rc == -1 ) {
            /* round up */
            mp_addsc( &num1, &num1, 1 );
        }
        if( newexp <= 0 && newexp > -63 ) {
            /* denormalized result */
            uint64 temp;
            mp_touint64( &temp, &num1 );
            makeExtended( dst, temp, 0 - extended_bias );
        } else {
            mp_toextended( dst, &num1 );
            changeExtendedExponent( dst, dst, e1 - e2 - 63 );
        }
        if( isDstNeg ) eneg( dst, dst );
        mp_free( &num1 );
        mp_free( &num2 );
        mp_free( &rem );
    }
}

void erem( void *dst, void *src1, void *src2 ) {
    uint64 f1, f2;
    int e1, e2;
    mpnum num1, num2, zero;
    int emin;
    int newexp;

    /* special cases */
    if( e_isNan( src1 ) || e_isNan( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isInf( src1 ) || e_isZero( src2 ) ) {
        makeExtNan( dst );
    } else if( e_isZero( src1 ) || e_isInf( src2 ) ) {
        memcpy( dst, src1, 10 );
    } else {
        parseExtended( (char*)src1, &f1, &e1 );
        parseExtended( (char*)src2, &f2, &e2 );
        mp_init( &num1, f1 );
        mp_init( &num2, f2 );
        mp_init( &zero, 0 );
        emin = min( e1, e2 );
        if( e2 > e1 ) {
            mp_shiftleft( &num2, &num2, e2 - e1 );
        } else {
            mp_shiftleft( &num1, &num1, e1 - e2 );
        }
        mp_div( &num1, &num2, &num1, &num2 ); /* num2 will get the remainder */
        if( mp_eq( &num2, &zero ) ) {
            makeExtPosZero( dst );
        } else {
            newexp = emin - 64 + mp_bitsize( &num2 ) + extended_bias;
            if( newexp <= 0 && newexp >= -64 ) {
                /* denormalized result */
                uint64 temp;
                mp_binround( &num2, &num2, -newexp + 1 );
                mp_shiftright( &num2, &num2, -newexp + 1 );
                mp_touint64( &temp, &num2 );
                makeExtended( dst, temp, 0 - extended_bias );
            } else {
                mp_toextended( dst, &num2 );
                changeExtendedExponent( dst, dst, emin - 63 );
            }
        }
        if( e_isNeg( src1 ) ) eneg( dst, dst );
        mp_free( &num1 );
        mp_free( &num2 );
        mp_free( &zero );
    }
}

void eneg( void *dst, void *src ) {
    memcpy( dst, src, 10 );
    if( !d_isNan( dst ) ) {
        ((char*)dst)[9] ^= 128;
    }
}

void f2d( void *dst, void *src )
{
    uint64 f;
    int e;

    if( f_isNan( src ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( f_isPosInf( src ) ) {
        *(uint64*)dst = DOUBLE_POS_INF;
    } else if( f_isNegInf( src ) ) {
        *(uint64*)dst = DOUBLE_NEG_INF;
    } else if( f_isPosZero( src ) ) {
        *(uint64*)dst = DOUBLE_POS_ZERO;
    } else if( f_isNegZero( src ) ) {
        *(uint64*)dst = DOUBLE_NEG_ZERO;
    } else {
        parseFloat( *(float*)src, &f, &e );
        f <<= 29;
        makeDouble( (double*)dst, f, e );
        if( f_isNeg( src ) ) {
            dneg( dst, dst );
        }
    }
}

void d2f( void *dst, void *src ) {
    uint64 f;
    int e;
    mpnum temp;

    if( d_isNan( src ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( d_isPosInf( src ) ) {
        *(uint32*)dst = FLOAT_POS_INF;
    } else if( d_isNegInf( src ) ) {
        *(uint32*)dst = FLOAT_NEG_INF;
    } else if( d_isPosZero( src ) ) {
        *(uint32*)dst = FLOAT_POS_ZERO;
    } else if( d_isNegZero( src ) ) {
        *(uint32*)dst = FLOAT_NEG_ZERO;
    } else {
        parseDouble( *(double*)src, &f, &e );
        mp_init( &temp, f );
        if( e + float_bias <= 0 && e + float_bias > -24 ) {
            mp_binround( &temp, &temp, 29 - (e + float_bias) + 1 );
            mp_shiftright( &temp, &temp, 29 - (e + float_bias) + 1 );
        } else {
            mp_binround( &temp, &temp, 29 );
            mp_shiftright( &temp, &temp, 29 );
        }
        mp_touint64( &f, &temp );
        makeFloat( (float*)dst, f, e );
        mp_free( &temp );
        if( d_isNeg( src ) ) {
            fneg( dst, dst );
        }
    }
}

void e2d( void *dst, void *src ) {
    uint64 f;
    int e;
    mpnum temp;

    if( e_isNan( src ) ) {
        *(uint64*)dst = DOUBLE_NAN;
    } else if( e_isPosInf( src ) ) {
        *(uint64*)dst = DOUBLE_POS_INF;
    } else if( e_isNegInf( src ) ) {
        *(uint64*)dst = DOUBLE_NEG_INF;
    } else if( e_isPosZero( src ) ) {
        *(uint64*)dst = DOUBLE_POS_ZERO;
    } else if( e_isNegZero( src ) ) {
        *(uint64*)dst = DOUBLE_NEG_ZERO;
    } else {
        parseExtended( (char*)src, &f, &e );
        mp_init( &temp, f );
        if( e + double_bias <= 0 && e + double_bias > -53 ) {
            mp_binround( &temp, &temp, 11 - (e + double_bias) + 1 );
            mp_shiftright( &temp, &temp, 11 - (e + double_bias) + 1 );
        } else {
            mp_binround( &temp, &temp, 11 );
            mp_shiftright( &temp, &temp, 11 );
        }
        mp_touint64( &f, &temp );
        makeDouble( (double*)dst, f, e );
        mp_free( &temp );
        if( e_isNeg( src ) ) {
            dneg( dst, dst );
        }
    }
}

void d2e( void *dst, void *src ) {
    uint64 f;
    int e;

    if( d_isNan( src ) ) {
        makeExtNan( dst );
    } else if( d_isPosInf( src ) ) {
        makeExtPosInf( dst );
    } else if( d_isNegInf( src ) ) {
        makeExtNegInf( dst );
    } else if( d_isPosZero( src ) ) {
        makeExtPosZero( dst );
    } else if( d_isNegZero( src ) ) {
        makeExtNegZero( dst );
    } else {
        parseDouble( *(double*)src, &f, &e );
        f <<= 11;
        makeExtended( (char*)dst, f, e );
        if( d_isNeg( src ) ) {
            eneg( dst, dst );
        }
    }
}

void e2f( void *dst, void *src ) {
    uint64 f;
    int e;
    mpnum temp;

    if( e_isNan( src ) ) {
        *(uint32*)dst = FLOAT_NAN;
    } else if( e_isPosInf( src ) ) {
        *(uint32*)dst = FLOAT_POS_INF;
    } else if( e_isNegInf( src ) ) {
        *(uint32*)dst = FLOAT_NEG_INF;
    } else if( e_isPosZero( src ) ) {
        *(uint32*)dst = FLOAT_POS_ZERO;
    } else if( e_isNegZero( src ) ) {
        *(uint32*)dst = FLOAT_NEG_ZERO;
    } else {
        parseExtended( (char*)src, &f, &e );
        mp_init( &temp, f );
        if( e + float_bias <= 0 && e + float_bias > -24 ) {
            mp_binround( &temp, &temp, 40 - (e + float_bias) + 1 );
            mp_shiftright( &temp, &temp, 40 - (e + float_bias) + 1 );
        } else {
            mp_binround( &temp, &temp, 40 );
            mp_shiftright( &temp, &temp, 40 );
        }
        mp_touint64( &f, &temp );
        makeFloat( (float*)dst, f, e );
        mp_free( &temp );
        if( e_isNeg( src ) ) {
            fneg( dst, dst );
        }
    }
}

void f2e( void *dst, void *src ) {
    uint64 f;
    int e;

    if( f_isNan( src ) ) {
        makeExtNan( dst );
    } else if( f_isPosInf( src ) ) {
        makeExtPosInf( dst );
    } else if( f_isNegInf( src ) ) {
        makeExtNegInf( dst );
    } else if( f_isPosZero( src ) ) {
        makeExtPosZero( dst );
    } else if( f_isNegZero( src ) ) {
        makeExtNegZero( dst );
    } else {
        parseFloat( *(float*)src, &f, &e );
        f <<= 40;
        makeExtended( (char*)dst, f, e );
        if( f_isNeg( src ) ) {
            eneg( dst, dst );
        }
    }
}

void f2i( void *dst, void *src ) {
    uint64 f;
    int e;
    mpnum num;

    if( f_isNan( src ) ) {
        *(int32*)dst = 0;
        return;
    }
    parseFloat( *(float*)src, &f, &e );
    mp_init( &num, f );
    e -= 23;
    if( e > 0 ) {
        mp_shiftleft( &num, &num, e );
    } else if( e < 0 ) {
        mp_shiftright( &num, &num, -e );
    }
    mp_touint64( &f, &num );
    if( f > LONG_MAX ) f = LONG_MAX;
    if( f_isNeg( src ) ) {
        *(int32*)dst = -f;
    } else {
        *(int32*)dst = f;
    }
    mp_free( &num );
}

void i2f( void *dst, void *src ) {
    int32 num = *(int32*)src;
    mpnum n;
    int isNeg = FALSE;
    if( num < 0 ) {
        isNeg = TRUE;
        num = -num;
    }
    mp_init( &n, (uint64) num );
    mp_tofloat( dst, &n );
    if( isNeg ) {
        fneg( dst, dst );
    }
    mp_free( &n );
}

void f2l( void *dst, void *src ) {
    uint64 f;
    int e;
    mpnum num;

    if( f_isNan( src ) ) {
        *(int64*)dst = 0;
        return;
    }
    parseFloat( *(float*)src, &f, &e );
    mp_init( &num, f );
    e -= 23;
    if( e > 0 ) {
        mp_shiftleft( &num, &num, e );
    } else if( e < 0 ) {
        mp_shiftright( &num, &num, -e );
    }
    mp_touint64( &f, &num );
    if( f > LONGLONG_MAX ) f = LONGLONG_MAX;
    if( f_isNeg( src ) ) {
        *(int64*)dst = -f;
    } else {
        *(int64*)dst = f;
    }
    mp_free( &num );
}

void l2f( void *dst, void *src ) {
    int64 num = *(int64*)src;
    mpnum n;
    int isNeg = FALSE;
    if( num < 0 ) {
        isNeg = TRUE;
        num = -num;
    }
    mp_init( &n, (uint64) num );
    mp_tofloat( dst, &n );
    if( isNeg ) {
        fneg( dst, dst );
    }
    mp_free( &n );
}

void d2i( void *dst, void *src ) {
    uint64 f;
    int e;
    mpnum num;

    if( d_isNan( src ) ) {
        *(int32*)dst = 0;
        return;
    }
    parseDouble( *(double*)src, &f, &e );
    mp_init( &num, f );
    e -= 52;
    if( e > 0 ) {
        mp_shiftleft( &num, &num, e );
    } else if( e < 0 ) {
        mp_shiftright( &num, &num, -e );
    }
    mp_touint64( &f, &num );
    if( f > LONG_MAX ) f = LONG_MAX;
    if( d_isNeg( src ) ) {
        *(int32*)dst = -f;
    } else {
        *(int32*)dst = f;
    }
    mp_free( &num );
}

void i2d( void *dst, void *src ) {
    int32 num = *(int32*)src;
    mpnum n;
    int isNeg = FALSE;
    if( num < 0 ) {
        isNeg = TRUE;
        num = -num;
    }
    mp_init( &n, (uint64) num );
    mp_todouble( dst, &n );
    if( isNeg ) {
        dneg( dst, dst );
    }
    mp_free( &n );
}

void d2l( void *dst, void *src ) {
    uint64 f;
    int e;
    mpnum num;

    if( d_isNan( src ) ) {
        *(int64*)dst = 0;
        return;
    }
    parseDouble( *(double*)src, &f, &e );
    mp_init( &num, f );
    e -= 52;
    if( e > 0 ) {
        mp_shiftleft( &num, &num, e );
    } else if( e < 0 ) {
        mp_shiftright( &num, &num, -e );
    }
    mp_touint64( &f, &num );
    if( f > LONGLONG_MAX ) f = LONGLONG_MAX;
    if( d_isNeg( src ) ) {
        *(int64*)dst = -f;
    } else {
        *(int64*)dst = f;
    }
    mp_free( &num );
}

void l2d( void *dst, void *src ) {
    int64 num = *(int64*)src;
    mpnum n;
    int isNeg = FALSE;
    if( num < 0 ) {
        isNeg = TRUE;
        num = -num;
    }
    mp_init( &n, (uint64) num );
    mp_todouble( dst, &n );
    if( isNeg ) {
        dneg( dst, dst );
    }
    mp_free( &n );
}

void e2i( void *dst, void *src ) {
    uint64 f;
    int e;
    mpnum num;

    if( e_isNan( src ) ) {
        *(int32*)dst = 0;
        return;
    }
    parseExtended( (char*)src, &f, &e );
    mp_init( &num, f );
    e -= 63;
    if( e > 0 ) {
        mp_shiftleft( &num, &num, e );
    } else if( e < 0 ) {
        mp_shiftright( &num, &num, -e );
    }
    mp_touint64( &f, &num );
    if( f > LONG_MAX ) f = LONG_MAX;
    if( e_isNeg( src ) ) {
        *(int32*)dst = -f;
    } else {
        *(int32*)dst = f;
    }
    mp_free( &num );
}

void i2e( void *dst, void *src ) {
    int32 num = *(int32*)src;
    mpnum n;
    int isNeg = FALSE;
    if( num < 0 ) {
        isNeg = TRUE;
        num = -num;
    }
    mp_init( &n, (uint64) num );
    mp_toextended( dst, &n );
    if( isNeg ) {
        eneg( dst, dst );
    }
    mp_free( &n );
}

void e2l( void *dst, void *src ) {
    uint64 f;
    int e;
    mpnum num;

    if( e_isNan( src ) ) {
        *(int64*)dst = 0;
        return;
    }
    parseExtended( (char*)src, &f, &e );
    mp_init( &num, f );
    e -= 63;
    if( e > 0 ) {
        mp_shiftleft( &num, &num, e );
    } else if( e < 0 ) {
        mp_shiftright( &num, &num, -e );
    }
    mp_touint64( &f, &num );
    if( f > LONGLONG_MAX ) f = LONGLONG_MAX;
    if( e_isNeg( src ) ) {
        *(int64*)dst = -f;
    } else {
        *(int64*)dst = f;
    }
    mp_free( &num );
}

void l2e( void *dst, void *src ) {
    int64 num = *(int64*)src;
    mpnum n;
    int isNeg = FALSE;
    if( num < 0 ) {
        isNeg = TRUE;
        num = -num;
    }
    mp_init( &n, (uint64) num );
    mp_toextended( dst, &n );
    if( isNeg ) {
        eneg( dst, dst );
    }
    mp_free( &n );
}

int fcmp( void *arg1, void *arg2 )
{
    uint64 f1, f2;
    int e1, e2;
    int isNeg1 = f_isNeg( arg1 );
    int isNeg2 = f_isNeg( arg2 );
    int result = 0;

    /* check for Nan */
    if( f_isNan( arg1 ) || f_isNan( arg2 ) ) {
        return -2;  /* unordered */
    }

    /* check +/-0 == +/-0 */
    if( f_isZero( arg1 ) && f_isZero( arg2 ) ) {
        return 0;
    } else if( f_isZero( arg1 ) ) {
        if( isNeg2 ) return -1;
        else return 1;
    } else if( f_isZero( arg2 ) ) {
        if( isNeg1 ) return 1;
        else return -1;
    }

    /* check sign */
    if( isNeg1 && !isNeg2 ) {
        return 1;
    } else if( !isNeg1 && isNeg2 ) {
        return -1;
    }

    parseFloat( *(float*)arg1, &f1, &e1 );
    parseFloat( *(float*)arg2, &f2, &e2 );

    /* check exponent */
    if( e1 > e2 ) {
        result = -1;
    } else if( e1 < e2 ) {
        result = 1;
    } else {
        /* check mantissa */
        if( f1 > f2 ) {
            result = -1;
        } else if( f1 < f2 ) {
            result = 1;
        } else {
            result = 0;
        }
    }

    /* if values are both negative, switch result */
    if( isNeg1 ) result = -result;

    return result;
}

int dcmp( void *arg1, void *arg2 )
{
    uint64 f1, f2;
    int e1, e2;
    int isNeg1 = d_isNeg( arg1 );
    int isNeg2 = d_isNeg( arg2 );
    int result = 0;

    /* check for Nan */
    if( d_isNan( arg1 ) || d_isNan( arg2 ) ) {
        return -2;  /* unordered */
    }

    /* check +/-0 == +/-0 */
    if( d_isZero( arg1 ) && d_isZero( arg2 ) ) {
        return 0;
    } else if( d_isZero( arg1 ) ) {
        if( isNeg2 ) return -1;
        else return 1;
    } else if( d_isZero( arg2 ) ) {
        if( isNeg1 ) return 1;
        else return -1;
    }

    /* check sign */
    if( isNeg1 && !isNeg2 ) {
        return 1;
    } else if( !isNeg1 && isNeg2 ) {
        return -1;
    }

    parseDouble( *(double*)arg1, &f1, &e1 );
    parseDouble( *(double*)arg2, &f2, &e2 );

    /* check exponent */
    if( e1 > e2 ) {
        result = -1;
    } else if( e1 < e2 ) {
        result = 1;
    } else {
        /* check mantissa */
        if( f1 > f2 ) {
            result = -1;
        } else if( f1 < f2 ) {
            result = 1;
        } else {
            result = 0;
        }
    }

    /* if values are both negative, switch result */
    if( isNeg1 ) result = -result;

    return result;
}
#endif

#ifdef TEST

void OutputFloatInHex( void *fl )
{
    char *flt = (char*)fl;
    int i;
    printf( "0x" );
    for( i = 3; i >= 0; i-- ) {
        printf( "%02x", flt[i] );
    }
    printf( "\n" );
}

void OutputDoubleInHex( void *db )
{
    char *dbl = (char*)db;
    int i;
    printf( "0x" );
    for( i = 7; i >= 0; i-- ) {
        printf( "%02x", dbl[i] );
    }
    printf( "\n" );
}

main()
{
}

#endif /* ifdef TEST */
