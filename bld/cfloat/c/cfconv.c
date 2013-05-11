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


#include <string.h>
#include <stddef.h>
#include "watcom.h"
#include "cfloat.h"
#include "machine.h"
#include "i64.h"

#define NULLCHAR        '\0'

extern  unsigned_32     U32ModDiv(unsigned_32*,unsigned_32);

#define _IsDigit( ch ) ( ch >= '0' && ch <= '9' )

extern  signed_64       CFGetDec64( char *bstart ) {
/**************************************************/

    signed_64   number;
    signed_64   ten;
    signed_64   temp;

    I32ToI64( 0, &number );
    I32ToI64( 10, &ten );
    while( _IsDigit( *bstart ) ) {
        U64Mul( &number, &ten, &number );
        I32ToI64( *bstart++ - '0', &temp );
        U64Add( &number, &temp, &number );
    }
    return( number );
}

extern  signed_32       CFGetDec32( char *bstart ) {
/**************************************************/

    signed_32   number;

    number = 0;
    while( _IsDigit( *bstart ) ) {
        number = number * 10 + *bstart++ - '0';
    }
    return( number );
}

static  signed_16       CFGetDec16( char *bstart ) {
/**************************************************/

    signed_16   number;

    number = 0;
    while( _IsDigit( *bstart ) ) {
        number = number * 10 + *bstart++ - '0';
    }
    return( number );
}

extern  char    *CFCnvFS( cfloat *f, char *buffer, unsigned maxlen ) {
/********************************************************************/

    unsigned    len;

    len = f->len - 1;
    if( len + 5 + I16DIGITS > maxlen ) {
        len = maxlen - 5 - I16DIGITS;
    }
    if( f->sign == -1 ) {
        *buffer++ = '-';
    }
    *buffer++ = f->mant[0];
    *buffer++ = '.';                            /* don't forget decimal point!*/
    memcpy( buffer, &f->mant[1], len );         /* copy mantissa*/
    buffer += len;
    *buffer++ = 'E';
    if( f->exp > 0 ) {
        len = f->exp - 1;
    } else {
        *buffer++ = '-';
        len = 1 - f->exp;
    }
    buffer[ 2 ] = len % 10 + '0';
    len /= 10;
    buffer[ 1 ] = len % 10 + '0';
    len /= 10;
    buffer[ 0 ] = len + '0';
    buffer[ 3 ] = NULLCHAR;
    buffer += 3;
    return( buffer );
}

static  void            DoConvert( cfloat *number, char *bstart ) {
/*****************************************************************/


    int         len;
    char        *fptr;
    int         sgn;
    int         expon;

    for(;;) {
        if( *bstart != ' ' ) break;
        bstart++;
    }
    sgn = 1;
    if( *bstart  == '-' ) {
        sgn = -1;
        bstart++;
    } else if( *bstart == '+' ) {
        bstart++;
    }
    expon = 0;
    len = 0;
    fptr = number->mant;
    if( _IsDigit( *bstart ) ) {
        for(;;) {                /* scan before decimal point*/
            *fptr++ = *bstart++;
            len++;
            expon++;
            if( !_IsDigit( *bstart ) ) break;
        }
        if( *bstart == '.' ) {
            bstart++;
            for(;;) {            /* scan after decimal point*/
                if( !_IsDigit( *bstart ) ) break;
                *fptr++ = *bstart++;
                len++;
            }
        }
    } else {
        if( *bstart != '.' ) return;
        ++bstart;
        if( !_IsDigit( *bstart ) ) return;
        for(;;) {                /* scan after decimal point*/
            *fptr++ = *bstart++;
            len++;
            if( !_IsDigit( *bstart ) ) break;
        }
    }
    if( *bstart == 'E' || *bstart == 'e' ) {
        if( *++bstart == '-' ) {
            expon -= CFGetDec16( ++bstart );
        } else {
            if( *bstart == '+' ) {
                bstart++;
            }
            expon += CFGetDec16( bstart );
        }
    }
    number->sign = sgn;
    number->len = len;             /* fill in length*/
    number->exp = expon;
    CFClean( number );
}


extern  cfloat  *CFCnvSF( char *bstart, char *bend ) {
/****************************************************/

/* Syntax accepted by this converter:*/
/**/
/* { at least 0 blanks }  < - | + >*/
/*               { at least 1 digit } < . { at least 0 digits } >*/
/*       or      . { at least 1 digit }*/
/*               < E | e < - | + > { at least 1 digit } >*/

    cfloat      *number;
    char        saved;

    saved = *bend;
    if( saved != NULLCHAR ) {
        *bend = NULLCHAR;                  /* KLUGE!!!*/
    }
    number = CFAlloc( bend - bstart );
    DoConvert( number, bstart );
    if( saved != NULLCHAR ) {
        *bend = saved;                     /* un-KLUGE*/
    }
    return( number );
}

extern  cfloat  *CFCopy( cfloat *old ) {
/***************************************/

    cfloat      *new;

    new = CFAlloc( old->len );
    memcpy( new, old, offsetof( cfloat, mant ) + old->len + 1 );
    return( new );
}

extern  cfloat  *CFTrunc( cfloat *f ) {
/*************************************/

    cfloat      *new;
    unsigned    len;

    if( f->exp <= 0 )
        return( CFAlloc( 1 ) );
    len = f->exp;
    new = CFCopy( f );
    if( new->len <= len )
        return( new );
    new->len = len;
    new->mant[ len ] = NULLCHAR;
    return( new );
}

extern  cfloat  *CFRound( cfloat *f ) {
/*************************************/

    cfloat      *trim;
    cfloat      *addto;
    cfloat      *new;
    unsigned    len;

    if( f->exp < 0 ) return( CFAlloc( 1 ) );
    len = f->exp;
    if( f->len <= len ) return( CFCopy( f ) );
    trim = CFTrunc( f );
    if( f->mant[ len ] < '5' ) return( trim );
    if( f->sign < 0 && f->len == ( len + 1 ) ) return( trim );
    addto = CFAlloc( 1 );
    addto->sign = f->sign;
    addto->mant[0] = '1';
    new = CFAdd( trim, addto );
    CFFree( trim );
    CFFree( addto );
    return( new );
}

static  cfloat  *CFCnvLongToF( signed_32 data, cf_bool is_signed ) {
/******************************************************************/

    cfloat              *new;
    unsigned            len;
    signed_8            sign;
    char                *digit;
    unsigned_32         dividend;
    char                mant[I32DIGITS+1];

    if( data == 0 ) return( CFAlloc( 0 ) );
    if( is_signed != CF_FALSE && -data == data ) {
        /* Aha! It's  -MaxNegI32 */
        new = CFCopy( (cfloat *)&MaxNegI32 );
        new->sign = -1;
        return( new );
    }
    sign = 1;
    if( data < 0 && is_signed ) {
        data = -data;
        sign = -1;
    }
    len = 0;
    digit = &mant[ I32DIGITS ];
    *digit = NULLCHAR;
    dividend = data;
    while( dividend != 0 ) {
        *--digit = U32ModDiv( &dividend, 10 ) + '0';
        ++len;
    }
    if( len > I16DIGITS ) {
        new = CFAlloc( I32DIGITS );
    } else {
        new = CFAlloc( I16DIGITS );
    }
    memcpy( new->mant, digit, len + 1 );
    new->sign = sign;
    new->exp = len;
    new->len = len;
    CFClean( new );
    return( new );
}

extern  cfloat  *CFCnvI32F( signed_32 data ) {
/********************************************/

    return( CFCnvLongToF( data, CF_TRUE ) );
}

extern  cfloat  *CFCnvU32F( unsigned_32 data ) {
/**********************************************/

    return( CFCnvLongToF( data, CF_FALSE ) );
}

extern  cfloat  *CFCnvU64F( unsigned_32 low, unsigned_32 high ) {
/***************************************************************/

    cfloat      *temp;
    cfloat      *lo;
    cfloat      *hi;
    cfloat      *result;

    lo = CFCnvU32F( low );
    hi = CFCnvU32F( high );
    temp = CFMul( hi, (cfloat *)&High64Mult );
    result = CFAdd( temp, lo );
    CFFree( lo );
    CFFree( hi );
    CFFree( temp );
    return( result );
}

#define _HiBitOn( x )   ( ( (x) & 0x80000000 ) != 0 )

extern  cfloat  *CFCnvI64F( unsigned_32 lo, unsigned_32 hi ) {
/************************************************************/

    cf_bool     neg;
    cfloat      *result;

    neg = CF_FALSE;
    if( _HiBitOn( hi ) ) {
        // take the two's complement
        neg = CF_TRUE;
        lo = ~lo;
        hi = ~hi;
        lo++;
        if( lo == 0 ) {
            hi++;
        }
    }
    result = CFCnvU64F( lo, hi );
    if( neg ) {
        result->sign = -1;
    }
    return( result );
}

extern  cfloat  *CFCnvIF( int data ) {
/*************************************/

    return( CFCnvLongToF( data, CF_TRUE ) );
}


extern  cfloat  *CFCnvUF( uint data ) {
/*************************************/

    return( CFCnvLongToF( data, CF_FALSE ) );
}

static  cf_bool CFIsType( cfloat *f, cfloat *maxval ) {
/*****************************************************/
// Assume 2-complement
// if signed maxval is magnitude of smallest negative number
    int         ord;

    if( f->exp <= 0 || f->exp > maxval->exp ) return( CF_FALSE );/* < 1 or > maxval*/
    if( f->sign == -1 && maxval->sign == 1 ) return( CF_FALSE ); /* signedness*/
    if( f->exp < f->len ) return( CF_FALSE );                 /* has decimal pt*/
    ord = CFOrder( f, maxval);                                /* compare mag*/
    if( ord == 1 ) return( CF_FALSE );                        /* too big*/
    /* can't have pos number equal to maxval if signed */
    if( ord == 0 && f->sign == 1 && maxval->sign == -1 ) return( CF_FALSE );
    return( CF_TRUE );
}

extern  cf_bool CFIsI8( cfloat *f ) {
/***********************************/

    return( CFIsType( f, (cfloat *)&MaxI8 ) );
}

extern  cf_bool CFIsI16( cfloat *f ) {
/***********************************/

    return( CFIsType( f, (cfloat *)&MaxNegI16 ) );
}

extern  cf_bool CFIsI32( cfloat *f ) {
/***********************************/

    return( CFIsType( f, (cfloat *)&MaxNegI32 ) );
}

extern  cf_bool CFIsI64( cfloat *f ) {
/************************************/

    return( CFIsType( f, (cfloat *)&MaxNegI64 ) );
}

extern  cf_bool CFIsU8( cfloat *f ) {
/***********************************/

    return( CFIsType( f, (cfloat *)&MaxU8 ) );
}

extern  cf_bool CFIsU16( cfloat *f ) {
/***********************************/

    return( CFIsType( f, (cfloat *)&MaxU16 ) );
}

extern  cf_bool CFIsU32( cfloat *f ) {
/***********************************/

    return( CFIsType( f, (cfloat *)&MaxU32 ) );
}

extern  cf_bool CFIsU64( cfloat *f ) {
/************************************/

    return( CFIsType( f, (cfloat *)&MaxU64 ) );
}

extern  cf_bool CFIs32( cfloat * cf ) {
/*************************************/

    if( CFIsI32( cf ) ) return( CF_TRUE );
    if( CFIsU32( cf ) ) return( CF_TRUE );
    return( CF_FALSE );
}

extern  cf_bool CFIs64( cfloat * cf ) {
/*************************************/

    if( CFIsI64( cf ) ) return( CF_TRUE );
    if( CFIsU64( cf ) ) return( CF_TRUE );
    return( CF_FALSE );
}

extern  cf_bool CFIsSize( cfloat *f, uint size ) {
/************************************************/

    switch( size ) {
    case 1:
        if( CFIsU8( f ) ) return( CF_TRUE );
        if( CFIsI8( f ) ) return( CF_TRUE );
        break;
    case 2:
        if( CFIsU16( f ) ) return( CF_TRUE );
        if( CFIsI16( f ) ) return( CF_TRUE );
        break;
    case 4:
        if( CFIsU32( f ) ) return( CF_TRUE );
        if( CFIsI32( f ) ) return( CF_TRUE );
        break;
    case 8:
        if( CFIsU64( f ) ) return( CF_TRUE );
        if( CFIsI64( f ) ) return( CF_TRUE );
    }
    return( CF_FALSE );
}


extern  cf_bool CFSignedSize( cfloat *f, uint size ) {
/****************************************************/

    switch( size ) {
    case 1:
        if( CFIsI8( f ) ) return( CF_TRUE );
        break;
    case 2:
        if( CFIsI16( f ) ) return( CF_TRUE );
        break;
    case 4:
        if( CFIsI32( f ) ) return( CF_TRUE );
        break;
    case 8:
        if( CFIsI64( f ) ) return( CF_TRUE );
    }
    return( CF_FALSE );
}

extern  cf_bool CFUnSignedSize( cfloat *f, uint size ) {
/************************************************/

    switch( size ) {
    case 1:
        if( CFIsU8( f ) ) return( CF_TRUE );
        break;
    case 2:
        if( CFIsU16( f ) ) return( CF_TRUE );
        break;
    case 4:
        if( CFIsU32( f ) ) return( CF_TRUE );
        break;
    case 8:
        if( CFIsU64( f ) ) return( CF_TRUE );
        break;
    }
    return( CF_FALSE );
}


extern  signed_16       CFCnvF16( cfloat *f ) {
/*********************************************/

    signed_16   data;
    signed_16   exp;

    data = 0;
    if( CFIsI16( f ) || CFIsU16( f ) ) {
        data = CFGetDec16( f->mant );
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

extern  signed_32       CFCnvF32( cfloat *f ) {
/*********************************************/

    signed_32   data;
    int         exp;

    data = 0;
    if( CFIsU32( f ) || CFIsI32( f ) ) {
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

extern  signed_64       CFCnvF64( cfloat *f ) {
/*********************************************/

    signed_64           data;
    signed_64           ten;
    signed_64           rem;
    signed_32           exp;

    I32ToI64( 0, &data );
    if( CFIsI64( f ) || CFIsU64( f ) ) {
        I32ToI64( 10, &ten );
        data = CFGetDec64( f->mant );
        exp = f->exp - f->len;
        while( exp > 0 ) {
            U64Mul( &data, &ten, &data );
            exp--;
        }
        while( exp < 0 ) {
            U64Div( &data, &ten, &data, &rem );
            exp++;
        }
        if( f->sign == -1 ) {
            U64Neg( &data, &data );
        }
    }
    return( data );
}
