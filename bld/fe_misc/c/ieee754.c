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


#include header

#include <string.h>
#include <stdio.h>
#include "watcom.h"
#include "cfloat.h"
#include "ieee754.h"
#include "xfloat.h"

cfloat *PosInf = NULL;
cfloat *NegInf = NULL;
cfloat *NaN = NULL;
cfloat *MaxPosIEEE32 = NULL;
cfloat *MinPosIEEE32 = NULL;
cfloat *MaxNegIEEE32 = NULL;
cfloat *MinNegIEEE32 = NULL;

cfloat *MaxPosIEEE64 = NULL;
cfloat *MinPosIEEE64 = NULL;
cfloat *MaxNegIEEE64 = NULL;
cfloat *MinNegIEEE64 = NULL;

#define MAX_TRIES 4

// Float.MAX_VALUE = 3.40282347e+38f
// mant: 340282347
// exp: 39 (ie. think of this as 0.340282347 exp 39
static struct STRUCT_cfloat( 18 )     MaxIEEE32 = {
                        39,             /* exponent base ten */
                        9,              /* mantissa length base ten */
                        0,              /* allocation length */
                        1,              /* sign: positive */
                        {'3','4','0','2','8','2','3','4','7',0 }
                        };

// public static final float MIN_VALUE = 1.40129846e-45f;
static struct STRUCT_cfloat( 19 )     MinIEEE32 = {
                        -44,            /* exponent base ten */
                        9,              /* mantissa length base ten */
                        0,              /* allocation length */
                        1,              /* sign: positive */
                        {'1','4','0','1','2','9','8','4','6', 0 }
                        };

// Double.MAX_VALUE = 1.79769313486231570e+308
static struct STRUCT_cfloat( 18 )     MaxIEEE64 = {
                        309,            /* exponent base ten */
                        17,             /* mantissa length base ten */
                        0,              /* allocation length */
                        1,              /* sign: positive */
                        { '1','7','9','7','6','9','3','1','3','4','8','6','2','3','1','5','7',0 }
                        };

//  public static final double MIN_VALUE = 4.94065645841246544e-324;
static struct STRUCT_cfloat( 19 )     MinIEEE64 = {
                        -323,           /* exponent base ten */
                        18,             /* mantissa length base ten */
                        0,              /* allocation length */
                        1,              /* sign: positive */
                        { '4','9','4','0','6','5','6','4','5','8','4','1','2','4','6','5','4','4',0}
                        };

cfloat *IEEECFToCFNotSpecial
/**************************/
    ( cfloat *cf
    , CF_PRECISION prec )
{
    // could be too much precision
    cfloat *new_cf;
    if( prec == CF_32 ) {
        char buffer[64];
        volatile float f = CFToF( cf );
        sprintf( buffer, "%.15e", f );
        new_cf = CFCnvSF( (char *)buffer, (char *)(buffer + strlen(buffer)) );
    } else {
        int num_tries;
        double d, d2;
        unsigned __int64 u, u2;

        char buffer[64];
        d = CFToF( cf ); // precision: infinite->64 bits
        sprintf( buffer, "%.15e", d );
        new_cf = CFCnvSF( (char *)buffer, (char *)(buffer + strlen(buffer)) );

        // check if it converts back to what we wanted
        d2 = CFToF( new_cf );

        u = *(unsigned __int64 *)&d;
        u2 = *(unsigned __int64 *)&d2;

        for( num_tries = 0;
             ( u != u2 ) &&
             ( num_tries < MAX_TRIES ) &&
             ( cf->len > new_cf->len);
             num_tries++ ) {
            // didn't convert back just right
            cfloat      *newer;

            // add another digit
            newer = CFAlloc( new_cf->len + 1 );
            memcpy( newer, new_cf, offsetof( cfloat, mant ) + new_cf->len + 1 );
            CFDeposit( newer, new_cf->len -1, CFAccess( cf, new_cf->len-1 ) );
            CFDeposit( newer, new_cf->len, CFAccess( cf, new_cf->len ) );
            newer->len++;
            newer->mant[newer->len] = '\0';
            CFFree( new_cf );
            new_cf = newer;
            newer = NULL;

            d2 = CFToF( new_cf );
            u2 = *(unsigned __int64 *)&d2;
        }
    }
    if( CFCompare( new_cf, cf ) != 0 ) {
        if( ( cf->len == ( new_cf->len + 1 ) ) && ( prec == CF_64 ) ) {
            // if we just shaved off one digit, don't bother taking the
            // new one for doubles (need this extra digit for proper
            // arithmetic in outer ranges of doubles
            CFFree( new_cf );
        } else {
            CFFree( cf );
            cf = new_cf;
        }
    } else {
        // they are the same, so no truncation was really needed
        CFFree( new_cf );
    }
    return( cf );
}

#if 0
// need special code to get value for MinPosIEEE64
// it's hard to get anything other than zero for this
cfloat *getMinPos()
{
    char buffer[64];
    unsigned_64 u64;
    double d;

    u64.u._64[0] = 0x80000000L;
    d = *(double *)&u64;
    sprintf( buffer, "%.15e", d );
    return( CFCnvSF( (char *)buffer, (char *)(buffer + strlen(buffer)) ) );
}
#endif

void IEEEInit
/***********/
    ( void )
{
    PosInf = CFCnvI32F( 1 );    // so it has correct sign
    NegInf = CFCnvI32F( -1 );   // so it has correct sign
    NaN = CFCnvI32F( 0 );

    // if we do IEEECFToCFNotSpecial on MaxPosIEEE32, we get back too many
    // digits of precision
    MaxPosIEEE32 = CFCopy( (cfloat *)&MaxIEEE32 );
    MinPosIEEE32 = IEEECFToCFNotSpecial( CFCopy( (cfloat *)&MinIEEE32 ), CF_32 );
    MaxPosIEEE64 = IEEECFToCFNotSpecial( CFCopy( (cfloat *)&MaxIEEE64 ), CF_64 );

    MinPosIEEE64 = CFCopy( (cfloat *)&MinIEEE64 );
#if 0
    MinPosIEEE64 = IEEECFToCFNotSpecial( getMinPos(), CF_64 );
#endif


    MaxNegIEEE32 = IEEECFToCFNotSpecial( CFCopyNegate( MaxPosIEEE32 ), CF_32 );
    MinNegIEEE32 = IEEECFToCFNotSpecial( CFCopyNegate( MinPosIEEE32 ), CF_32 );
    MaxNegIEEE64 = IEEECFToCFNotSpecial( CFCopyNegate( MaxPosIEEE64 ), CF_64 );
    MinNegIEEE64 = IEEECFToCFNotSpecial( CFCopyNegate( MinPosIEEE64 ), CF_64 );

}

void IEEEFini
/***********/
    ( void )
{
    CFFree( PosInf );
    CFFree( NegInf );
    CFFree( NaN );

    CFFree( MaxPosIEEE32 );
    CFFree( MaxPosIEEE64 );
    CFFree( MinPosIEEE32 );
    CFFree( MinPosIEEE64 );

    CFFree( MaxNegIEEE32 );
    CFFree( MaxNegIEEE64 );
    CFFree( MinNegIEEE32 );
    CFFree( MinNegIEEE64 );

    PosInf = NULL;
    NegInf = NULL;
    NaN = NULL;
}

cfloat *IEEEGetPosInf
/******************/
    (void)
{
    return PosInf;
}

cfloat *IEEEGetNegInf
/*******************/
    (void)
{
    return NegInf;
}

cfloat *IEEEGetNaN
/****************/
    (void)
{
    return NaN;
}

cf_bool IEEEIsSpecialValue
/************************/
    (cfloat *f)
{
    if( (f == NegInf) || (f == PosInf) || (f == NaN) ) {
        return( CF_TRUE );
    } else {
        return( CF_FALSE );
    }
}

static cf_bool IEEEIsInf
/************************/
    (cfloat *f)
{
    if( (f == NegInf) || (f == PosInf) ) {
        return( CF_TRUE );
    } else {
        return( CF_FALSE );
    }
}

void IEEECFFree                 // FREE CFLOAT (UNLESS SPECIAL VALUE)
/**************/
    ( cfloat * cf )
{
    if( IEEEIsSpecialValue( cf ) != CF_TRUE ) {
//      CFFree( cf );
    }
}

cfloat *IEEECheckRange
/********************/
    ( cfloat *cf
    , CF_PRECISION prec )
{
    cfloat *minpos;
    cfloat *minneg;
    cfloat *maxpos;
    cfloat *maxneg;

    if( prec == CF_32 ) {
        maxpos = MaxPosIEEE32;
        maxneg = MaxNegIEEE32;
        minpos = MinPosIEEE32;
        minneg = MinNegIEEE32;
    } else {
        maxpos = MaxPosIEEE64;
        maxneg = MaxNegIEEE64;
        minpos = MinPosIEEE64;
        minneg = MinNegIEEE64;
    }
    if( CFGetSign( cf ) > 0 ) { // cf positive
        if( CFCompare( cf, maxpos ) > 0 ) { // cf > maxpos
            IEEECFFree( cf );
            return( IEEEGetPosInf() );
        }
        if( CFCompare( cf, minpos ) < 0 ) { // cf < minpos
            IEEECFFree( cf );
            return( CFCnvI32F( 0 ) );       // positive zero
        }
    } else if( CFGetSign( cf ) < 0 ) {    // cf is negative
        if( CFCompare( cf, maxneg ) < 0 ) { // cf < maxneg
            IEEECFFree( cf );
            return( IEEEGetNegInf() );
        }
        if( CFCompare( cf, minneg ) > 0 ) { // cf > minneg
            IEEECFFree( cf );
            return( CFNegate( CFCnvI32F( 0 ) ) );       // negative zero
        }
    }
    return( cf );
}

// assumtions: cf will not be NaN, but may be greater than
// infinity or less than negative infinity of the given precision
cfloat *IEEECFToCF
/****************/
    ( cfloat *cf
    , CF_PRECISION prec )
{
    cfloat *new_cf = IEEECheckRange( cf, prec );
    if( new_cf == cf ) {
        new_cf = IEEECFToCFNotSpecial( cf, prec );
    }
    return( new_cf );
}

cfloat * IEEECFDiv
/****************/
    ( cfloat *left
    , cfloat *right
    , CF_PRECISION prec )
{
    cfloat *new_val;

    if( IEEEIsSpecialValue( left  ) || IEEEIsSpecialValue( right ) ) {
        // at least one is Nan, PosInf, or NegInf
        if( left == IEEEGetNaN() || right == IEEEGetNaN() ) {
            return IEEEGetNaN();        // NaN / float = float / NaN = NaN
        } else {
            // if neither was NaN, at least one must be inf
            cfloat *inf;
            cfloat *other;
            if( IEEEIsInf( left ) ) {
                inf = left;
                other = right;
            } else {
                DbgAssert( IEEEIsInf( right ) );
                inf = right;
                other = left;
            }
            if( IEEEIsInf( other ) ) {
                return IEEEGetNaN();    // inf / inf = NaN
            }
            // only one infinity
            if( left == inf ) {
                // left is infinity, right is not
                if( CFGetPosNeg( left ) * CFGetPosNeg( right ) < 0 ) {
                    return( IEEEGetNegInf() ); // inf / -ve or -inf / +ve = -inf
                } else {
                    return( IEEEGetPosInf() ); // inf / +ve or -inf / -ve = +inf
                }
            } else {
                DbgAssert( right == inf );
                // right is infinity
                if( CFGetPosNeg( left ) * CFGetPosNeg( right ) < 0 ) {
                    // -ve/inf or +ve/-inf = -0
                    return( CFNegate( CFCnvI32F( 0 ) ) );
                } else {
                    return( CFCnvI32F( 0 ) );   // +ve/inf or -ve/-inf = 0
                }
            }
        }
    } else if( CFTest( right ) ) {
        // right is not zero
        if( CFTest( left ) ) {
            // left is not zero
            new_val = IEEECFToCF( CFDiv( left, right ), prec );
        } else {
            // left is zero (-0.0 or 0.0)
            new_val = IEEECFToCF( CFCopy( left ), prec );
        }
    } else {
        // right is zero
        if( CFTest( left ) ) {
            // left not zero
            if( ( CFTest( left ) * CFGetZeroSign( right ) ) > 0 ) {
                // zero or two positives: positive infinity
                new_val = IEEEGetPosInf();
            } else {
                // one negative: negative infinity
                new_val = IEEEGetNegInf();
            }
        } else {
            // both are zero: NaN
            new_val = IEEEGetNaN();
        }
    }
    return( new_val );
}

cfloat * IEEECFMod              // IEEE MODULUS
/****************/
    ( cfloat *left
    , cfloat *right
    , CF_PRECISION prec )

{
    cfloat *retn = NULL;

    if( IEEEIsSpecialValue( left  ) || IEEEIsSpecialValue( right ) ) {
        if( IEEEIsSpecialValue( left )  // NaN%float=inf%float=-inf%float=NaN
         || right == IEEEGetNaN()       // float % NaN = Nan
         || CFTest( right ) == 0 ) {    // float % 0 = Nan
            retn = IEEEGetNaN();
        } else if( IEEEIsInf( right ) ) {
            // finite / inf = finite
            retn = left;
        }
    } else if( CFTest( left ) == 0 ) {
        retn = left;    // 0 % finite = 0
    } else if( CFTest( right ) == 0 ) {
        // x % 0 = NaN
        retn = IEEEGetNaN();
    } else {
        // left / right = result;
        // left % right = ( left - (right * result ) );
        cfloat *result;
        result = CFTrunc( CFDiv( left, right ) );
        retn = IEEECFToCF( CFSub( left, CFMul( right, result ) ), prec );
    }
    DbgAssert( retn != NULL );
    return( retn );
}

cfloat * IEEECFMul                      // IEEE MULTIPLY
/****************/
    ( cfloat *left
    , cfloat *right
    , CF_PRECISION prec )
{
    if( IEEEIsSpecialValue( left  ) || IEEEIsSpecialValue( right ) ) {
        // JLS 15.16.1
        if( left == IEEEGetNaN() || right == IEEEGetNaN() ) {
            return IEEEGetNaN();        // NaN * float = float* NaN = NaN
        } else {
            // if neither was NaN, at least one must be inf
            cfloat *inf;
            cfloat *other;
            if( IEEEIsInf( left ) ) {
                inf = left;
                other = right;
            } else {
                DbgAssert( IEEEIsInf( right ) );
                inf = right;
                other = left;
            }
            if( CFGetSign( other ) == 0 ) {
                return( IEEEGetNaN() ); // inf * zero = zero * inf = NaN
            } else {
                // num * inf = inf * num = inf
                // sign of result is determined by signs of inf and num
                if( ( CFGetSign( inf ) * CFGetSign( other ) ) > 0 ) {
                    return IEEEGetPosInf();
                } else {
                    return IEEEGetNegInf();
                }
            }
        }
    } else {
        return( IEEECFToCF( CFMul( left, right ), prec ) );
    }
}

cfloat * IEEECFNegate
/*******************/
    ( cfloat *f
    , CF_PRECISION prec )
{
    if( f == PosInf ) {
        return NegInf;
    } else if( f == NegInf ) {
        return PosInf;
    } else if( f == NaN ) {
        return f;
    } else {
        // CFNegate doesn't make a copy so we have to call CFCopyNegate
        // as upper layers expect to be able to discard operands after
        // result of operation is returned
        return( CFCopyNegate( f ) );
    }
}

cfloat * IEEECFAdd              // IEEE ADDITION
/****************/
    ( cfloat *left
    , cfloat *right
    , CF_PRECISION prec )
{
    cfloat *retn = NULL;

    if( IEEEIsSpecialValue( left  ) || IEEEIsSpecialValue( right ) ) {
        // JLS 15.17.2
        if( left == IEEEGetNaN() || right == IEEEGetNaN() ) {
            retn = IEEEGetNaN();        // NaN + float = float + NaN = NaN
        } else {
            cfloat *inf;
            cfloat *other;
            // one or both are infinity;
            if( IEEEIsInf( left ) ) {
                inf = left;
                other = right;
            } else {
                inf = right;
                other = left;
            }
            if( IEEEIsInf( other ) ) {
                // both are infinite
                if( CFGetPosNeg( inf ) * CFGetPosNeg( other ) < 0 ) {
                    // inf + -inf = -inf + inf = NaN
                    retn = IEEEGetNaN();
                } else {
                    // both inf, same sign:
                    // -inf + -inf = -inf
                    // inf + inf = inf
                    retn = left;
                }
            } else {
                // one infinite, one finite
                // float + inf = inf
                retn = inf;
            }
        }
    } else {
        // not a special value
        if( CFTest( left ) * CFTest( right ) == 0 ) {
            // one or both is zero
            if( CFTest( left ) != 0 ) {
                retn = left;    // float + 0 = float
            } else if( CFTest( right ) != 0 ) {
                retn = right;   // 0 + float = float
            } else {
                //both zero
                if( CFGetZeroSign( left ) * CFGetZeroSign( right ) < 0 ) {
                    // opposite sign, return +ve zero
                    if( CFGetZeroSign( left ) > 0 ) {
                        retn = left;
                    } else {
                        retn = right;
                    }
                } else {
                    // same sign, return either
                    retn = left;
                }
            }
        } else {
            // neither is zero
            retn = CFAdd( left, right );
            if( CFTest( retn ) == 0 ) {
                // a zero result should be +ve zero
                CFSetZeroSign( retn, 1 );
            } else {
                retn = IEEECFToCF( retn, prec );
            }
        }
    }
    DbgAssert( retn != NULL );
    return retn;
}
//---------------------- comparison functions ---------------------

static int compareInf
    ( cfloat *left
    , cfloat *right )
{
    if( left == right ) {       //  inf cmp  inf or
        return 0;               // -inf cmp -inf
    } else {
        if( left == IEEEGetNegInf() ) { // -inf cmp num  or
            return( -1 );               // -inf cmp inf
        } else if( right == IEEEGetPosInf() ) { //  num cmp inf or
            return( -1 );                       // -inf cmp inf
        } else {
            return( 1 );        // inf cmp num
                                // num cmp -inf
        }
    }
}

cf_bool IEEECFCmp               // IEEE COMPARISON
/***************/
    ( cfloat *left
    , cfloat *right
    , CF_COMPARE cmp )
{
    cf_bool retn = -2;
    int result;
    if( IEEEIsSpecialValue( left  ) || IEEEIsSpecialValue( right ) ) {
        if( left == IEEEGetNaN() || right == IEEEGetNaN() ) {
            if( cmp == CF_CMP_NE ) {     // val != NaN = val != NaN = true
                return( CF_TRUE );       // n.b. NaN != NaN = true
            } else {
                // at least one NaN
                return( CF_FALSE );
            }
        } else {
            // at least one must be infinite
            result = compareInf( left, right );
        }
    } else {
        result = CFCompare( left, right );
    }
    DbgAssert( result == -1 || result == 1 || result == 0 );
    switch( cmp  ) {
    case CF_CMP_NONE :
        DbgNever();
        break;
    case CF_CMP_LT :
        retn = ( result < 0 );
        break;
    case CF_CMP_LE :
        retn = ( result <= 0 );
        break;
    case CF_CMP_GT :
        retn = ( result > 0 );
        break;
    case CF_CMP_GE :
        retn = ( result >= 0 );
        break;
    case CF_CMP_EQ :
        retn = ( result == 0 );
        break;
    case CF_CMP_NE :
        retn = ( result != 0 );
        break;
    }
    return( retn );
}

