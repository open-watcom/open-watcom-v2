/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024-2025 The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include <stddef.h>
#include "cfloati.h"
#include "i64.h"


#define _HiBitOn( x )   (((x) & 0x80000000) != 0)

#define I16DIGITS       5
#define I32DIGITS       10
//#define I64DIGITS       21

static struct STRUCT_cfloat( 4 )    MaxNegI8 = {
    3,              /* exponent ten */
    3,              /* mantissa length ten */
    0,              /* allocation length */
    -1,             /* negative */
    { '1','2','8',0 }
};

static struct STRUCT_cfloat( 4 )    MaxU8 = {
    3,              /* exponent ten */
    3,              /* mantissa length ten */
    0,              /* allocation length */
    1,              /* positive */
    { '2','5','5',0 }
};

static struct STRUCT_cfloat( 6 )    MaxNegI16 = {
    5,              /* exponent ten */
    5,              /* mantissa length ten */
    0,              /* allocation length */
    -1,             /* negative */
    { '3','2','7','6','8',0 }
};

static struct STRUCT_cfloat( 6 )    MaxU16 = {
    5,              /* exponent ten */
    5,              /* mantissa length ten */
    0,              /* allocation length */
    1,              /* positive */
    { '6','5','5','3','5',0 }
};

static struct STRUCT_cfloat( 11 )   MaxNegI32 = {
    10,             /* exponent ten */
    10,             /* mantissa length ten */
    0,              /* allocation length */
    -1,             /* negative */
    { '2','1','4','7','4','8','3','6','4','8',0 }
};

static struct STRUCT_cfloat( 11 )   MaxU32 = {
    10,             /* exponent ten */
    10,             /* mantissa length ten */
    0,              /* allocation length */
    1,              /* positive */
    { '4','2','9','4','9','6','7','2','9','5',0 }
};

static struct STRUCT_cfloat( 20 )   MaxNegI64 = {
    19,             /* exponent ten */
    19,             /* mantissa length ten */
    0,              /* allocation length */
    -1,             /* negative */
    { '9','2','2','3','3','7','2','9','3','6','8','5','4','7','7','5','8','0','7',0 }
};

static struct STRUCT_cfloat( 21 )   MaxU64 = {
    20,             /* exponent ten */
    20,             /* mantissa length ten */
    0,              /* allocation length */
    1,              /* positive */
    { '1','8','4','4','6','7','4','4','0','7','3','7','0','9','5','5','1','6','1','5',0 }
};

/*
 * this is MaxU32 + 1 - just don't want to calc it at runtime
 */
static struct STRUCT_cfloat( 11 )   High64Mult = {
    10,             /* exponent ten */
    10,             /* mantissa length ten */
    0,              /* allocation length */
    1,              /* positive */
    { '4','2','9','4','9','6','7','2','9','6',0 }
};

/*
 * constants are extracted from ISO C standard 2.2.4.2.2
 */

/*
 * MaxPosFloat 3.40282347e+38
 */
static struct STRUCT_cfloat( 10 )   MaxPosFloat = {
    39,         /* exponent ten */
    9,          /* mantissa length ten */
    0,          /* allocation length */
    1,          /* positive */
    { '3','4','0','2','8','2','3','4','7',0 }
};

/*
 * MinPosFloat 1.17549435e-38
 */
static struct STRUCT_cfloat( 10 )   MinPosFloat = {
    -37,        /* exponent ten */
    9,          /* mantissa length ten */
    0,          /* allocation length */
    1,          /* positive */
    { '1','1','7','5','4','9','4','3','5',0 }
};

/*
 * MaxPosDouble 1.7976931348623157e+308
 */
static struct STRUCT_cfloat( 18 )   MaxPosDouble = {
    309,        /* exponent ten */
    17,         /* mantissa length ten */
    0,          /* allocation length */
    1,          /* positive */
    { '1','7','9','7','6','9','3','1','3','4','8','6','2','3','1','5','7',0 }
};

/*
 * MinPosDouble 2.2250738585072014e-308
 */
static struct STRUCT_cfloat( 18 )   MinPosDouble = {
    -307,       /* exponent ten */
    17,         /* mantissa length ten */
    0,          /* allocation length */
    1,          /* positive */
    { '2','2','2','5','0','7','3','8','5','8','5','0','7','2','0','1','4',0 }
};

static signed_64    CFGetDec64( const char *str )
/***********************************************/
{
    signed_64   number;
    signed_64   ten;
    signed_64   temp;

    Set64ValZero( number );
    Set64ValU32( ten, 10 );
    while( _IsDigit( *str ) ) {
        U64MulEq( &number, &ten );
        Set64ValU32( temp, *str++ - '0' );
        U64AddEq( &number, &temp );
    }
    return( number );
}

static signed_32    CFGetDec32( const char *str )
/***********************************************/
{
    signed_32   number;

    number = 0;
    while( _IsDigit( *str ) ) {
        number = number * 10 + *str++ - '0';
    }
    return( number );
}

static signed_16    CFGetDec16( const char *str )
/***********************************************/
{
    signed_16   number;

    number = 0;
    while( _IsDigit( *str ) ) {
        number = number * 10 + *str++ - '0';
    }
    return( number );
}

char    *CFCnvFS( cfloat *f, char *buffer, int maxlen )
/*****************************************************/
{
    int         len;

    len = f->len - 1;
    if( len + 5 + I16DIGITS > maxlen ) {
        len = maxlen - 5 - I16DIGITS;
    }
    if( f->sign == -1 ) {
        *buffer++ = '-';
    }
    *buffer++ = f->mant[0];
    *buffer++ = '.';                        /* don't forget decimal point! */
    memcpy( buffer, f->mant + 1, len );     /* copy mantissa */
    buffer += len;
    *buffer++ = 'E';
    if( f->exp > 0 ) {
        len = f->exp - 1;
    } else {
        *buffer++ = '-';
        len = 1 - f->exp;
    }
    buffer[2] = len % 10 + '0';
    len /= 10;
    buffer[1] = len % 10 + '0';
    len /= 10;
    buffer[0] = (char)len + '0';
    buffer[3] = NULLCHAR;
    buffer += 3;
    return( buffer );
}

static void     DoConvert( cfloat *f, const char *str )
/******************************************************
 * Syntax accepted by this converter:
 *
 * {WS}*[[-|+]]  {0-9}+[.{0-9}*] or .{0-9}+  [[E|e][[-|+]]{0-9}+]
 */
{
    int         len;
    signed char sgn;
    int         expon;

    while( *str == ' ' ) {
        str++;
    }
    sgn = 1;
    if( *str  == '-' ) {
        sgn = -1;
        str++;
    } else if( *str == '+' ) {
        str++;
    }
    expon = 0;
    len = 0;
    /*
     * scan before decimal point
     */
    while( _IsDigit( *str ) ) {
        f->mant[len++] = *str++;
        expon++;
    }
    if( *str == '.' ) {
        str++;
        /*
         * scan after decimal point
         */
        while( _IsDigit( *str ) ) {
            f->mant[len++] = *str++;
        }
    }
    if( len == 0 ) {
        /* return, no mantisa digit */
        return;
    }
    /*
     * terminate mantisa digits
     */
    f->mant[len] = NULLCHAR;
    /*
     * scan exponent if exists
     */
    if( *str == 'E'
      || *str == 'e' ) {
        str++;
        if( *str == '-' ) {
            str++;
            expon -= CFGetDec16( str );
        } else {
            if( *str == '+' ) {
                str++;
            }
            expon += CFGetDec16( str );
        }
    }
    /*
     * update info in cfloat structure
     */
    f->sign = sgn;
    f->len = len;
    f->exp = expon;
    /*
     * normalize f
     */
    CFClean( f );
}


cfloat  *CFCnvSF( cfhandle h, const char *str )
/*********************************************/
{
    cfloat      *result;

    result = CFAlloc( h, strlen( str ) );
    DoConvert( result, str );
    return( result );
}

cfloat  *CFCopy( cfhandle h, cfloat *f )
/**************************************/
{
    cfloat      *result;

    result = CFAlloc( h, f->len );
    memcpy( result, f, CFLOAT_SIZE + f->len );
    return( result );
}

cfloat  *CFTrunc( cfhandle h, cfloat *f )
/***************************************/
{
    cfloat      *result;
    int         len;

    if( f->exp <= 0 )
        return( CFAlloc( h, 1 ) );
    len = f->exp;
    result = CFCopy( h, f );
    if( result->len <= len )
        return( result );
    result->len = len;
    result->mant[len] = NULLCHAR;
    return( result );
}

cfloat  *CFRound( cfhandle h, cfloat *f )
/***************************************/
{
    cfloat      *trim;
    cfloat      *addto;
    cfloat      *result;
    int         len;

    if( f->exp < 0 )
        return( CFAlloc( h, 1 ) );
    len = f->exp;
    if( f->len <= len )
        return( CFCopy( h, f ) );
    trim = CFTrunc( h, f );
    if( f->mant[len] < '5' )
        return( trim );
    if( f->sign < 0
      && f->len == ( len + 1 ) )
        return( trim );
    addto = CFAlloc( h, 1 );
    addto->sign = f->sign;
    addto->mant[0] = '1';
    result = CFAdd( h, trim, addto );
    CFFree( h, trim );
    CFFree( h, addto );
    return( result );
}

static cfloat   *CFCnvLongToF( cfhandle h, signed_32 data, bool is_signed )
/*************************************************************************/
{
    cfloat              *result;
    int                 len;
    signed_8            sign;
    char                *digit;
    unsigned_32         dividend;
    char                mant[I32DIGITS + 1];

    if( data == 0 )
        return( CFAlloc( h, 0 ) );
    if( is_signed
      && -data == data ) {
        /*
         * Aha! It's  -MaxNegI32
         */
        result = CFCopy( h, (cfloat *)&MaxNegI32 );
        return( result );
    }
    sign = 1;
    if( data < 0
      && is_signed ) {
        data = -data;
        sign = -1;
    }
    len = 0;
    digit = &mant[I32DIGITS];
    *digit = NULLCHAR;
    dividend = data;
    while( dividend != 0 ) {
        *--digit = (char)U32ModDiv( &dividend, 10 ) + '0';
        ++len;
    }
    if( len > I16DIGITS ) {
        result = CFAlloc( h, I32DIGITS );
    } else {
        result = CFAlloc( h, I16DIGITS );
    }
    memcpy( result->mant, digit, len + 1 );
    result->sign = sign;
    result->exp = len;
    result->len = len;
    /*
     * normalize result
     */
    CFClean( result );
    return( result );
}

cfloat  *CFCnvI32F( cfhandle h, signed_32 data )
/**********************************************/
{
    return( CFCnvLongToF( h, data, true ) );
}

cfloat  *CFCnvU32F( cfhandle h, unsigned_32 data )
/************************************************/
{
    return( CFCnvLongToF( h, data, false ) );
}

cfloat  *CFCnvU64F( cfhandle h, unsigned_32 lo32, unsigned_32 hi32 )
/******************************************************************/
{
    cfloat      *temp;
    cfloat      *lo;
    cfloat      *hi;
    cfloat      *result;

    lo = CFCnvU32F( h, lo32 );
    hi = CFCnvU32F( h, hi32 );
    temp = CFMul( h, hi, (cfloat *)&High64Mult );
    result = CFAdd( h, temp, lo );
    CFFree( h, lo );
    CFFree( h, hi );
    CFFree( h, temp );
    return( result );
}

cfloat  *CFCnvI64F( cfhandle h, unsigned_32 lo32, unsigned_32 hi32 )
/******************************************************************/
{
    bool        neg;
    cfloat      *result;

    neg = false;
    if( _HiBitOn( hi32 ) ) {
        /*
         * take the two's complement
         */
        neg = true;
        lo32 = ~lo32;
        hi32 = ~hi32;
        lo32++;
        if( lo32 == 0 ) {
            hi32++;
        }
    }
    result = CFCnvU64F( h, lo32, hi32 );
    if( neg ) {
        result->sign = -1;
    }
    return( result );
}

cfloat  *CFCnvIF( cfhandle h, int data )
/**************************************/
{
    return( CFCnvLongToF( h, data, true ) );
}


cfloat  *CFCnvUF( cfhandle h, uint data )
/***************************************/
{
    return( CFCnvLongToF( h, data, false ) );
}

static bool     CFIsIntType( cfloat *f, cfloat *maxval )
/*******************************************************
 * Assume 2-complement
 * if signed maxval is magnitude of smallest negative number
 */
{
    int         ord;

    if( f->exp <= 0                 /* < 1 or > maxval */
      || f->exp > maxval->exp )
        return( false );
    if( f->sign == -1               /* signedness */
      && maxval->sign == 1 )
        return( false );
    if( f->exp < f->len )           /* has decimal pt */
        return( false );
    ord = CFOrder( f, maxval);      /* compare mag */
    if( ord > 0 )                   /* | f | > | maxval |  too big */
        return( false );
    /*
     * can't have pos number equal to maxval if signed
     */
    if( ord == 0                    /* | f | = | maxval | */
      && f->sign == 1
      && maxval->sign == -1 ) {
        return( false );
    }
    return( true );
}

bool CFIsI8( cfloat *f )
/**********************/
{
    return( CFIsIntType( f, (cfloat *)&MaxNegI8 ) );
}

bool CFIsI16( cfloat *f )
/***********************/
{
    return( CFIsIntType( f, (cfloat *)&MaxNegI16 ) );
}

bool CFIsI32( cfloat *f )
/***********************/
{
    return( CFIsIntType( f, (cfloat *)&MaxNegI32 ) );
}

bool CFIsI64( cfloat *f )
/***********************/
{
    return( CFIsIntType( f, (cfloat *)&MaxNegI64 ) );
}

bool CFIsU8( cfloat *f )
/**********************/
{
    return( CFIsIntType( f, (cfloat *)&MaxU8 ) );
}

bool CFIsU16( cfloat *f )
/***********************/
{
    return( CFIsIntType( f, (cfloat *)&MaxU16 ) );
}

bool CFIsU32( cfloat *f )
/***********************/
{
    return( CFIsIntType( f, (cfloat *)&MaxU32 ) );
}

bool CFIsU64( cfloat *f )
/***********************/
{
    return( CFIsIntType( f, (cfloat *)&MaxU64 ) );
}

bool CFIs32( cfloat *f )
/**********************/
{
    if( CFIsI32( f ) )
        return( true );
    if( CFIsU32( f ) )
        return( true );
    return( false );
}

bool CFIs64( cfloat *f )
/**********************/
{
    if( CFIsI64( f ) )
        return( true );
    if( CFIsU64( f ) )
        return( true );
    return( false );
}

bool CFIsSize( cfloat *f, uint size )
/***********************************/
{
    switch( size ) {
    case 1:
        if( CFIsU8( f ) )
            return( true );
        if( CFIsI8( f ) )
            return( true );
        break;
    case 2:
        if( CFIsU16( f ) )
            return( true );
        if( CFIsI16( f ) )
            return( true );
        break;
    case 4:
        if( CFIsU32( f ) )
            return( true );
        if( CFIsI32( f ) )
            return( true );
        break;
    case 8:
        if( CFIsU64( f ) )
            return( true );
        if( CFIsI64( f ) )
            return( true );
        break;
    }
    return( false );
}

bool CFSignedSize( cfloat *f, uint size )
/***************************************/
{
    switch( size ) {
    case 1:
        if( CFIsI8( f ) )
            return( true );
        break;
    case 2:
        if( CFIsI16( f ) )
            return( true );
        break;
    case 4:
        if( CFIsI32( f ) )
            return( true );
        break;
    case 8:
        if( CFIsI64( f ) )
            return( true );
        break;
    }
    return( false );
}

bool CFUnSignedSize( cfloat *f, uint size )
/*****************************************/
{
    switch( size ) {
    case 1:
        if( CFIsU8( f ) )
            return( true );
        break;
    case 2:
        if( CFIsU16( f ) )
            return( true );
        break;
    case 4:
        if( CFIsU32( f ) )
            return( true );
        break;
    case 8:
        if( CFIsU64( f ) )
            return( true );
        break;
    }
    return( false );
}

bool CFIsFloat( cfloat *f )
/*************************/
{
    int     sign;
    bool    ok;

    sign = f->sign;
    f->sign = 1;
    ok = true;
    if( CFCompare( f, (cfloat *)&MaxPosFloat ) > 0
      || CFCompare( f, (cfloat *)&MinPosFloat ) < 0 ) {
        ok = false;
    }
    f->sign = sign;
    return( ok );
}

bool CFIsDouble( cfloat *f )
/**************************/
{
    int     sign;
    bool    ok;

    sign = f->sign;
    f->sign = 1;
    ok = true;
    if( CFCompare( f, (cfloat *)&MaxPosDouble ) > 0
      || CFCompare( f, (cfloat *)&MinPosDouble ) < 0 ) {
        ok = false;
    }
    f->sign = sign;
    return( ok );
}

signed_16       CFCnvF16( cfloat *f )
/***********************************/
{
    signed_16   data;
    signed_16   exp;

    data = 0;
    if( CFIsI16( f )
      || CFIsU16( f ) ) {
        data = CFGetDec16( f->mant );
        exp = (signed_16)( f->exp - f->len );
        while( exp > 0 ) {
            data *= 10;
            exp--;
        }
        while( exp < 0 ) {
            data /= 10;
            exp++;
        }
        if( f->sign == -1 ) {
            data = -data;
        }
    }
    return( data );
}

signed_32       CFCnvF32( cfloat *f )
/***********************************/
{
    signed_32   data;
    int         exp;

    data = 0;
    if( CFIsU32( f )
      || CFIsI32( f ) ) {
        data = CFGetDec32( f->mant );
        exp = f->exp - f->len;
        while( exp > 0 ) {
            data *= 10;
            exp--;
        }
        while( exp < 0 ) {
            data /= 10;
            exp++;
        }
        if( f->sign == -1 ) {
            data = -data;
        }
    }
    return( data );
}

signed_64       CFCnvF64( cfloat *f )
/***********************************/
{
    signed_64           data;
    signed_64           ten;
    signed_64           rem;
    signed_32           exp;

    Set64ValZero( data );
    if( CFIsI64( f )
      || CFIsU64( f ) ) {
        Set64ValU32( ten, 10 );
        data = CFGetDec64( f->mant );
        exp = f->exp - f->len;
        while( exp > 0 ) {
            U64MulEq( &data, &ten );
            exp--;
        }
        while( exp < 0 ) {
            U64Div( &data, &ten, &data, &rem );
            exp++;
        }
        if( f->sign == -1 ) {
            U64NegEq( &data );
        }
    }
    return( data );
}
