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


/* This module contains functions for doing multiple precision, positive
 * integer arithmetic.  Allocations for these numbers increase automatically
 * as required when doing the arithmetic.  The size of a number can grow until
 * there is no more heap space.
 */
#ifndef TEST
#include header
#else
#include <malloc.h>
#define _MemoryAllocate( size )         malloc( size )
#define _MemoryFree( ptr )              free( ptr )
#endif
#include "mp.h"
#include <limits.h>
#include <string.h>
#include "fp.h"

#define two_to_the_31           0x80000000
#define two_to_the_32           (uint64)0x100000000

#define MP_GROW( num, size ) \
        if( mp_grow( num, size ) == MP_OUT_OF_MEMORY ) { \
            return MP_OUT_OF_MEMORY; \
        }
#define MP_COPY( dst, src ) \
        if( mp_copy( dst, src ) == MP_OUT_OF_MEMORY ) { \
            return MP_OUT_OF_MEMORY; \
        }
#define MP_INIT( num, val ) \
        if( mp_init( num, val ) == MP_OUT_OF_MEMORY ) { \
            return MP_OUT_OF_MEMORY; \
        }

/* initialize an mpnum */
int mp_init( mpnum *mp, uint64 value )
{
    mp->num = (uint32*)_MemoryAllocate( sizeof(uint32) * 2 );
    if( mp->num == NULL ) return MP_OUT_OF_MEMORY;
    memset( mp->num, 0, sizeof(uint32) * 2 );
    mp->allocated = 2;
    if( value >= two_to_the_32 ) {
        mp->num[0] = (uint32)value;
        mp->num[1] = value>>32;
        mp->len = 2;
    } else {
        mp->num[0] = value;
        mp->len = 1;
    }
    return MP_NO_ERR;
}

/* extend the number of uint32s allocated to src by size */
int mp_grow( mpnum *src, uint32 size )
{
    uint32 *temp;
    size = max( 3, size );
    temp = (uint32*)_MemoryAllocate( sizeof(uint32) * (src->allocated + size) );
    if( temp == NULL ) return MP_OUT_OF_MEMORY;
    memcpy( temp, src->num, src->allocated * sizeof(uint32) );
    memset( &temp[src->allocated], 0, size * sizeof(uint32) );
    _MemoryFree( src->num );
    src->num = temp;
    src->allocated += size;
    return MP_NO_ERR;
}

/* free the array of uint32s */
int mp_free( mpnum *num )
{
    if( num->allocated > 0 ) {
        _MemoryFree( num->num );
    }
    num->num = NULL;
    num->allocated = 0;
    num->len = 0;
    return MP_NO_ERR;
}

/* set num to zero */
int mp_zero( mpnum *num )
{
    memset( num->num, 0, num->allocated * sizeof(uint32) );
    num->len = 0;
    return MP_NO_ERR;
}

/* copy the src number to the destination number; dst must have already been
 * initialized */
int mp_copy( mpnum *dst, mpnum *src )
{
    if( dst->allocated < src->len ) {
        MP_GROW( dst, src->len - dst->allocated );
    }
    memcpy( dst->num, src->num, src->len * sizeof(uint32) );
    dst->len = src->len;
    return MP_NO_ERR;
}

/* compare num1 and num2
 * returns:  -1 if num1 > num2
 *            0 if num1 = num2
 *            1 if num1 < num2
 */
int mp_cmp( mpnum *num1, mpnum *num2 )
{
    uint32 i;
    uint32 minlen = min( num1->len, num2->len );

    if( num1->len > num2->len ) {
        for( i = num1->len - 1; i >= minlen; i-- ) {
            if( num1->num[i] > 0 ) return -1;
            if( i == 0 ) break;
        }
    } else if( num1->len < num2->len ) {
        for( i = num2->len - 1; i >= minlen; i-- ) {
            if( num2->num[i] > 0 ) return 1;
            if( i == 0 ) break;
        }
    }
    if( minlen == 0 ) return 0;
    for( i = minlen - 1; ; i-- ) {
        if( num1->num[i] > num2->num[i] ) {
            return -1;
        } else if( num1->num[i] < num2->num[i] ) {
            return 1;
        }
        if( i == 0 ) break;
    }
    return 0;
}

int mp_gt( mpnum *num1, mpnum *num2 )
{
    if( mp_cmp( num1, num2 ) < 0 ) return TRUE;
    return FALSE;
}

int mp_gte( mpnum *num1, mpnum *num2 )
{
    if( mp_cmp( num1, num2 ) <= 0 ) return TRUE;
    return FALSE;
}

int mp_lt( mpnum *num1, mpnum *num2 )
{
    if( mp_cmp( num1, num2 ) > 0 ) return TRUE;
    return FALSE;
}

int mp_lte( mpnum *num1, mpnum *num2 )
{
    if( mp_cmp( num1, num2 ) >= 0 ) return TRUE;
    return FALSE;
}

int mp_eq( mpnum *num1, mpnum *num2 )
{
    if( mp_cmp( num1, num2 ) == 0 ) return TRUE;
    return FALSE;
}

int mp_ne( mpnum *num1, mpnum *num2 )
{
    if( mp_cmp( num1, num2 ) == 0 ) return FALSE;
    return TRUE;
}

/* shift src 'bits' number of bits to the left */
int mp_shiftleft( mpnum *dst, mpnum *src, uint32 bits )
{
    uint32 newsize = src->len + ((bits-1) / 32) + 1;
    uint32 cursrc = src->len - 1;
    uint32 curdst = newsize - 1;
    uint32 temp1, temp2;
    uint8  shift = ((bits-1) % 32) + 1;
    uint32 i;

    if( bits == 0 || src->len == 0 ) {
        if( dst != src ) MP_COPY( dst, src );
        return MP_NO_ERR;
    }

    if( dst->allocated < newsize ) {
        MP_GROW( dst, newsize - dst->allocated );
    }
    for( i = dst->allocated-1; i >= newsize - 1; i-- ) {
        dst->num[i] = 0;
    }
    for(;;cursrc--,curdst--) {
        temp1 = src->num[cursrc] >> (32-shift);
        if( shift == 32 ) {
            temp2 = 0;
        } else {
            temp2 = src->num[cursrc] << shift;
        }
        dst->num[curdst] |= temp1;
        dst->num[curdst-1] = temp2;
        if( cursrc == 0 ) break;
    }
    for( curdst--;; curdst-- ) {
        if( curdst == 0 ) break;
        dst->num[curdst-1] = 0;
    }
    dst->len = newsize;
    if( dst->num[newsize-1] == 0 ) {
        dst->len -= 1;
    }
    return MP_NO_ERR;
}

/* shift src 'bits' number of bits to the right */
int mp_shiftright( mpnum *dst, mpnum *src, uint32 bits )
{
    uint32 newsize;
    int temp = src->len - bits / 32;
    uint32 cursrc = bits / 32;
    uint32 curdst = 0;
    uint32 temp1, temp2;
    uint8  shift = bits % 32;

    if( temp <= 0 ) {
        mp_zero( dst );
        return MP_NO_ERR;
    }
    newsize = (uint32)temp;

    if( bits == 0 || src->len == 0 ) {
        if( dst != src ) MP_COPY( dst, src );
        return MP_NO_ERR;
    }

    if( dst->allocated < newsize ) {
        MP_GROW( dst, newsize - dst->allocated );
    }
    for(;;cursrc++,curdst++) {
        if( shift == 0 ) {
            temp1 = 0;
        } else {
            temp1 = src->num[cursrc] << (32-shift);
        }
        temp2 = src->num[cursrc] >> shift;
        if( curdst > 0 ) dst->num[curdst-1] |= temp1;
        dst->num[curdst] = temp2;
        if( cursrc == src->len - 1 ) break;
    }
    for( curdst++;; curdst++ ) {
        if( curdst >= newsize ) break;
        dst->num[curdst] = 0;
    }
    dst->len = newsize;
    if( dst->num[newsize-1] == 0 ) {
        dst->len -= 1;
    }

    return MP_NO_ERR;
}

/* add a scalar value to src */
int mp_addsc( mpnum *dst, mpnum *src, uint32 scalar )
{
    uint32 maxlen = src->len;
    uint64 sum = 0;
    uint64 carry = scalar;
    uint32 i;

    if( dst->allocated < maxlen + 1 ) {
        MP_GROW( dst, maxlen + 1 - dst->allocated );
    }
    for( i = 0; ; i++ ) {
        if( i >= src->len ) {
            if( carry > 0 ) {
                dst->num[i] = carry;
            }
            break;
        } else {
            sum = (uint64)src->num[i] + carry;
        }
        if( sum >= two_to_the_32 ) {
            dst->num[i] = sum - two_to_the_32;
            carry = 1;
        } else {
            dst->num[i] = sum;
            carry = 0;
            break;
        }
    }
    if( dst->num[maxlen] == 0 ) {
        dst->len = maxlen;
    } else {
        dst->len = maxlen + 1;
    }
    return MP_NO_ERR;
}

/* add two mpnums */
int mp_add( mpnum *dst, mpnum *src1, mpnum *src2 )
{
    uint32 maxlen = max( src1->len, src2->len );
    uint64 sum = 0;
    uint64 carry = 0;
    uint32 i;

    if( dst->allocated < maxlen + 1 ) {
        MP_GROW( dst, maxlen + 1 - dst->allocated );
    }
    for( i = 0; ; i++ ) {
        if( i >= src1->len && i >= src2->len ) {
            if( carry > 0 ) {
                dst->num[i] = carry;
                dst->len = maxlen + 1;
            } else {
                dst->len = maxlen;
            }
            break;
        } else if( i >= src1->len ) {
            sum = (uint64)src2->num[i] + carry;
        } else if( i >= src2->len ) {
            sum = (uint64)src1->num[i] + carry;
        } else {
            sum = (uint64)src1->num[i] + (uint64)src2->num[i] + carry;
        }
        if( sum >= two_to_the_32 ) {
            dst->num[i] = sum - two_to_the_32;
            carry = 1;
        } else {
            dst->num[i] = sum;
            carry = 0;
        }
    }
    return MP_NO_ERR;
}

/* subtract two npnums; returns MP_NEGATIVE_RESULT if src1 < src2 */
int mp_sub( mpnum *dst, mpnum *src1, mpnum *src2 )
{
    uint32 maxlen = max( src1->len, src2->len );
    uint32 carry = 0;
    uint32 i;
    uint32 temp;

    if( mp_lt( src1, src2 ) ) {
        /* src1 < src2 => error */
        return MP_NEGATIVE_RESULT;
    }
    /* ASSERT num1 >= num2 => don't need to look for errors */
    if( dst->allocated < maxlen ) {
        MP_GROW( dst, maxlen - dst->allocated );
    }
    for( i = 0; ; i++ ) {
        if( i >= src1->len && i >= src2->len ) {
            break;
        } else if( i >= src2->len ) {
            temp = 0;
        } else {
            temp = src2->num[i];
        }
        if( src1->num[i] >= temp + carry ) {
            dst->num[i] = src1->num[i] - temp - carry;
            carry = 0;
        } else {
            dst->num[i] = (uint64)src1->num[i] + two_to_the_32 - temp - carry;
            carry = 1;
        }
    }
    for( i = maxlen - 1; ; i-- ) {
        if( dst->num[i] > 0 ) {
            dst->len = i + 1;
            break;
        }
        if( i == 0 ) {
            dst->len = 0;
            break;
        }
    }
    return MP_NO_ERR;
}

/* multiply an mpnum by a scalar value */
int mp_mulsc( mpnum *dst, mpnum *src, uint32 scalar )
{
    uint64 product = 0;
    uint64 carry = 0;
    uint32 i;

    if( dst->allocated < src->len + 1 ) {
        MP_GROW( dst, src->len + 1 - dst->allocated );
    }
    for( i = 0; ; i++ ) {
        if( i >= src->len ) {
            if( carry > 0 ) {
                dst->num[i] = carry;
                dst->len = src->len + 1;
            } else {
                dst->len = src->len;
            }
            break;
        } else {
            product = (uint64)src->num[i] * (uint64)scalar + carry;
        }
        if( product >= two_to_the_32 ) {
            dst->num[i] = (uint32)product;
            carry = product>>32;
        } else {
            dst->num[i] = product;
            carry = 0;
        }
    }
    return MP_NO_ERR;
}

/* Multiply two mpnums together using the intuitive pen-and-paper method for
 * multilpication.
 */
int mp_mul( mpnum *dst, mpnum *src1, mpnum *src2 )
{
    uint32 cur;
    uint32 sc;
    mpnum temp;
    mpnum result;
    int rc;

    if( dst->allocated < src1->len + src2->len ) {
        MP_GROW( dst, src1->len + src2->len - dst->allocated );
    }

    MP_INIT( &temp, 0 );
    MP_INIT( &result, 0 );

    for( cur = 0; cur < src2->len; cur++ ) {
        sc = src2->num[cur];
        if( sc != 1 ) {
            rc = mp_mulsc( &temp, src1, sc );
            if( rc != 0 ) goto done;
            rc = mp_shiftleft( &temp, &temp, cur * 32 );
        } else {
            rc = mp_shiftleft( &temp, src1, cur * 32 );
        }
        if( rc != 0 ) goto done;
        rc = mp_add( &result, &result, &temp );
        if( rc != 0 ) goto done;
    }

    MP_COPY( dst, &result );
    rc = MP_NO_ERR;

done:
    mp_free( &temp );
    mp_free( &result );

    return rc;
}

/* divide an mpnum by a scalar value; mode indicates ceiling or floor */
int mp_divsc( mpnum *dst, mpnum *src, uint32 scalar, int mode )
{
    uint64 dividend = 0;
    uint32 remainder = 0;
    uint32 i;

    if( scalar == 0 ) return MP_DIVIDE_BY_ZERO;
    if( scalar == 1 || src->len == 0 ) {
        if( dst != src ) MP_COPY( dst, src );
        return MP_NO_ERR;
    }
    if( dst->allocated < src->len ) {
        MP_GROW( dst, src->len - dst->allocated );
    }
    for( i = src->len - 1; ; i-- ) {
        dividend = (((uint64)remainder) << 32) | (uint64)src->num[i];
        dst->num[i] = (uint32)(dividend / scalar);
        remainder = (uint32)(dividend % scalar);
        if( i == 0 ) break;
    }
    dst->len = src->len;
    if( remainder > 0 && mode == CEILING ) {
        mp_addsc( dst, dst, 1 );
    }
    if( dst->num[src->len-1] == 0 ) dst->len--;
    return MP_NO_ERR;
}

/* This division algorithm is essentially the pen-and-paper long division
 * method.
 */
int mp_div( mpnum *qdst, mpnum *rdst, mpnum *x, mpnum *y )
{
    mpnum temp, temp2, q, r, mptmp1, mptmp2, mptmp3;
    uint32      n = x->len;
    uint32      t = y->len;
    uint32      i;
    int         rc = -1;
    uint64      tmp;
    uint32      shift = 0;

    MP_INIT( &q, 0 );
    if( q.allocated < x->len ) MP_GROW( &q, x->len - q.allocated );
    q.len = x->len;
    MP_INIT( &r, 0 );
    MP_INIT( &temp, 0 );
    MP_INIT( &temp2, 0 );
    i = 0;

    /* copy x into r */
    mp_copy( &r, x );

    /* check division by zero */
    if( mp_eq( &q, y ) ) {
        rc = MP_DIVIDE_BY_ZERO;
        goto done;
    }

    /* check if dividend is zero or x is less than y */
    if( mp_eq( &q, x ) || mp_lt( x, y ) ) {
        rc = MP_NO_ERR;
        goto done;
    }

    /* make sure n and t are true lengths */
    while( x->num[n-1] == 0 ) {
        n--;
        if( n == 0 ) goto done;
    }
    while( y->num[t-1] == 0 ) {
        t--;
        if( t == 0 ) goto done;
    }

    /* make sure first digit of divisor is >= base / 2 to ensure we get a
       good guess */
    shift = 31 - ((mp_bitsize( y ) - 1) % 32);
    if( ((mp_bitsize( &r ) + 1) % 32 - 1) + shift > 32 ) n++;
    mp_shiftleft( y, y, shift );
    mp_shiftleft( &r, &r, shift );

    /* step 2 */
    if( temp.allocated < n - t +1 ) MP_GROW( &temp, n - t - temp.allocated + 1 );
    temp.num[n-t] = 1;
    temp.len = n - t + 1;
    rc = mp_mul( &temp, &temp, y );
    if( rc != MP_NO_ERR ) goto done;
    for(;;) {
        if( mp_lt( &r, &temp ) ) break;
        q.num[n-t]++;
        mp_sub( &r, &r, &temp );
    }

    /* step 3 */
    for( i = n; i >= t + 1; i-- ) {
        /* 3.1 - make guess using two digits */
        if( r.num[i-1] == y->num[t-1] ) {
            q.num[i-t-1] = 0xFFFFFFFF;
        } else {
            tmp = ((uint64)(r.num[i-1])<<32  | (uint64)r.num[i-2]) / y->num[t-1];
            q.num[i-t-1] = (uint32) tmp;
        }
        /* 3.2 - improve guess using three digits */
        if( t > 1 ) {
            tmp = ((uint64)(r.num[i-1])<<32) | (uint64)r.num[i-2];
            MP_INIT( &mptmp1, tmp );
            MP_INIT( &mptmp3, 0 );
            mp_shiftleft( &mptmp1, &mptmp1, 32 );
            mp_addsc( &mptmp1, &mptmp1, r.num[i-3] );
            tmp = ((uint64)(y->num[t-1])<<32) | (uint64)y->num[t-2];
            MP_INIT( &mptmp2, tmp );
            mp_mulsc( &mptmp3, &mptmp2, q.num[i-t-1] );
            for(;;) {
                if( mp_lte( &mptmp3, &mptmp1 ) ) break;
                q.num[i-t-1]--;
                mp_sub( &mptmp3, &mptmp3, &mptmp2 );
            }
            mp_free( &mptmp1 );
            mp_free( &mptmp2 );
            mp_free( &mptmp3 );
        }
        /* 3.3 + 3.4 */
        mp_zero( &temp );
        mp_zero( &temp2 );
        temp.len = i - t;
        temp.num[i-t-1] = 1;
        rc = mp_mul( &temp, &temp, y );
        if( rc != MP_NO_ERR ) goto done;
        rc = mp_mulsc( &temp2, &temp, q.num[i-t-1] );
        if( rc != MP_NO_ERR ) goto done;
        if( mp_lt( &r, &temp2 ) ) {
            /* the guess was off by one */
            rc = mp_sub( &temp2, &temp2, &temp );
            if( rc != MP_NO_ERR ) goto done;
            q.num[i-t-1]--;
        }
        rc = mp_sub( &r, &r, &temp2 );
        if( rc != MP_NO_ERR ) goto done;
    }

    rc = MP_NO_ERR;
done:
    if( rc== MP_NO_ERR ) {
        mp_shiftright( y, y, shift );
        MP_COPY( qdst, &q );
        MP_COPY( rdst, &r );
        mp_shiftright( rdst, rdst, shift );
    }
    mp_free( &temp );
    mp_free( &temp2 );
    mp_free( &q );
    mp_free( &r );
    return rc;
}

/* sqaure src */
int mp_sqr( mpnum *dst, mpnum *src )
{
    return mp_mul( dst, src, src );
}

/* calculate src to the exp power using the binary method */
int mp_pow( mpnum *dst, mpnum *src, uint32 exp )
{
    mpnum temp;
    uint32 bit = 1;
    int rc;

    MP_INIT( &temp, 0 );
    MP_COPY( &temp, src );
    mp_free( dst );
    MP_INIT( dst, 1 );

    for( ;; ) {
        if( exp & bit ) {
            rc = mp_mul( dst, dst, &temp );
            if( rc != MP_NO_ERR ) goto done;
        }
        bit <<= 1;
        if( bit > exp ) break;
        rc = mp_sqr( &temp, &temp );
        if( rc != MP_NO_ERR ) goto done;
    }

    rc = MP_NO_ERR;

done:
    mp_free( &temp );

    return rc;
}

/* Do a binary rounding at the 'bit'th bit;
 * Round to nearest rounding mode is used and if there are two value equally
 * close to the number, it rounds to the even one. */
int mp_binround( mpnum *dst, mpnum *src, uint64 bit )
{
    uint32 bitval = two_to_the_31;
    uint32 unit = bit / 32;
    int isFirstBit = TRUE;
    int i;
    int round = 0;  /* -1 => down, 0 => even, 1 => up */
    uint32 valoffirstbit;
    uint32 firstunit = unit;
    bit = bit % 32;

    if( dst->allocated < src->len + 1 ) {
        MP_GROW( dst, src->len + 1 - dst->allocated );
    }

    MP_COPY( dst, src );

    /* handle special case where bit is 0 */
    if( unit == 0 && bit == 0 ) {
        return MP_NO_ERR;
    }

    for( i = 31; i > bit; i-- ) {
        bitval >>= 1;
    }
    valoffirstbit = bitval;

    /* determine rounding */
    for(;;) {
        /* decrement bit */
        if( bitval == 1 ) {
            if( unit == 0 ) break;
            unit--;
            bitval = two_to_the_31;
        } else {
            bitval >>= 1;
        }

        if( isFirstBit ) {
            if( !(src->num[unit] & bitval) ) {
                round = -1;  /* round down */
                break;
            }
            isFirstBit = FALSE;
        } else {
            if( src->num[unit] & bitval ) {
                round = 1; /* round up */
                break;
            }
        }
    }

    /* add 1 if necessary */
    if( (round == 1) || (round == 0 && (src->num[firstunit] & valoffirstbit) ) ) {
        mpnum temp;
        MP_INIT( &temp, 0 );
        if( firstunit >= temp.allocated ) {
            MP_GROW( &temp, firstunit - temp.allocated + 1);
        }
        mp_zero( &temp );
        temp.num[firstunit] = valoffirstbit;
        temp.len = firstunit + 1;
        mp_add( dst, dst, &temp );
        mp_free( &temp );
    }

    /* truncate */
    dst->num[firstunit] = dst->num[firstunit] & (ULONG_MAX - (valoffirstbit - 1));
    for( unit = firstunit;; unit--) {
        if( unit == 0 ) break;
        dst->num[unit-1] = 0;
    }

    return MP_NO_ERR;
}

/* Convert src to a real 4 byte number using mp_binround to round to the
 * nearest value. */
int mp_tofloat( uint8 *dst, mpnum *src )
{
    uint32 firstunit = src->len - 1;
    uint8  firstbit = 31;
    uint32 bit = two_to_the_31;
    uint64 exp;
    uint8 *temp;
    uint64 temp2;
    uint64 sig = 0;
    uint32 left = 0;
    uint8 i;
    mpnum rounded_num;

    memset( dst, 0, 4 );

    /* find first non-zero bit */
    if( src->len == 0 ) {
        return MP_NO_ERR;
    }
    for( ;; firstunit-- ) {
        if( src->num[firstunit] > 0 ) {
            break;
        }
        if( firstunit == 0 ) return MP_NO_ERR; /* src = 0 */
    }
    for( ;; firstbit-- ) {
        if( src->num[firstunit] & bit ) {
            break;
        }
        bit >>= 1;
    }

    /* round this number so that we have 24 significant bits */
    MP_INIT( &rounded_num, 0 );
    MP_COPY( &rounded_num, src );
    if( firstunit * 32 + firstbit > 23 ) {
        mp_binround( &rounded_num, &rounded_num, firstunit * 32 + firstbit - 23 );
        /* if it rounded up, the first bit may be up one */
        if( firstbit == 31 ) {
            if( rounded_num.num[firstunit+1] > 0 ) {
                firstunit++;
                firstbit = 0;
            }
        } else {
            if( rounded_num.num[firstunit] & (bit * 2) ) {
                firstbit++;
            }
        }
    }

    /* generate exponent */
    exp = (uint64)firstunit * 32 + firstbit + float_bias;
    if( exp > 255 ) exp = 255;
    temp = (uint8*)&exp;
    dst[3] = temp[0]>>1;
    dst[2] = temp[0]<<7;
    if( exp == 255 ) {
        /* too large => use +infinity */
        mp_free( &rounded_num );
        return MP_LOSS_OF_PRECISION;
    }

    /* generate significand */
    if( firstbit > 22 ) {
        left = 0;
        sig = (uint64)rounded_num.num[firstunit] >> (firstbit - 23);
    } else {
        left = 23 - firstbit;
        sig = (uint64)rounded_num.num[firstunit] << left;
    }
    if( firstunit > 0 && left > 0 ) {
        temp2 = (uint64)rounded_num.num[firstunit-1] >> (32-left);
        sig = sig | temp2;
        left = 0;
    }
    temp = (uint8*)&sig;
    dst[2] = dst[2] | (temp[2] & 127);
    for( i = 0; i < 2; i++ ) {
        dst[i] = temp[i];
    }

    mp_free( &rounded_num );
    return MP_NO_ERR;
}

/* Convert src to a real 8 byte number using mp_binround to round to the
 * nearest value. */
int mp_todouble( uint8 *dst, mpnum *src )
{
    uint32 firstunit = src->len - 1;
    uint8  firstbit = 31;
    uint32 bit = two_to_the_31;
    uint64 exp;
    uint8 *temp;
    uint64 temp2;
    uint64 sig = 0;
    uint32 left = 0;
    uint8 i;
    mpnum rounded_num;

    memset( dst, 0, 8 );
    if( src->len == 0 ) {
        return MP_NO_ERR;
    }
    for( ;; firstunit-- ) {
        if( src->num[firstunit] > 0 ) {
            break;
        }
        if( firstunit == 0 ) return MP_NO_ERR; /* src = 0 */
    }
    for( ;; firstbit-- ) {
        if( src->num[firstunit] & bit ) {
            break;
        }
        bit >>= 1;
    }

    /* round this number so that we have 53 significant bits */
    MP_INIT( &rounded_num, 0 );
    MP_COPY( &rounded_num, src );
    if( firstunit * 32 + firstbit > 52 ) {
        mp_binround( &rounded_num, &rounded_num, firstunit * 32 + firstbit - 52 );
        /* if it rounded up, the first bit may be up one */
        if( firstbit == 31 ) {
            if( rounded_num.num[firstunit+1] > 0 ) {
                firstunit++;
                firstbit = 0;
            }
        } else {
            if( rounded_num.num[firstunit] & (bit * 2) ) {
                firstbit++;
            }
        }
    }

    /* generate exponent */
    exp = (uint64)firstunit * 32 + firstbit + double_bias;
    if( exp > 2047 ) exp = 2047;
    temp = (uint8*)&exp;
    dst[7] = temp[1] << 4;
    dst[7] = dst[7] | ((temp[0] & 240) >> 4);
    dst[6] = (temp[0] & 15) << 4;
    if( exp == 2047 ) {
        /* too large => use +infinity */
        mp_free( &rounded_num );
        return MP_LOSS_OF_PRECISION;
    }

    /* generate significand */
    left = 52 - firstbit;
    sig = (uint64)rounded_num.num[firstunit] << left;
    if( firstunit > 0 ) {
        if( left > 32 ) {
            left -= 32;
            temp2 = (uint64)rounded_num.num[firstunit-1] << left;
            sig = sig | temp2;
        } else if( left < 32 ) {
            temp2 = (uint64)rounded_num.num[firstunit-1] >> (32-left);
            sig = sig | temp2;
            left = 0;
        } else {
            temp2 = (uint64)rounded_num.num[firstunit-1];
            sig = sig | temp2;
            left = 0;
        }
    }
    if( left > 0 && firstunit > 1 ) {
        temp2 = (uint64)rounded_num.num[firstunit-2] >> (32-left);
        sig = sig | temp2;
    }
    temp = (uint8*)&sig;
    for( i = 0; i < 6; i++ ) {
        dst[i] = temp[i];
    }
    dst[6] = dst[6] | (temp[6] & 15);

    mp_free( &rounded_num );
    return MP_NO_ERR;
}

/* Convert src to a real 10 byte number using mp_binround to round to the
 * nearest value. */
int mp_toextended( uint8 *dst, mpnum *src )
{
    uint32 firstunit = src->len - 1;
    uint8  firstbit = 31;
    uint32 bit = two_to_the_31;
    uint64 exp;
    uint8 *temp;
    uint64 temp2;
    uint64 sig = 0;
    uint32 left = 0;
    uint8 i;
    mpnum rounded_num;

    memset( dst, 0, 10 );

    /* find first non-zero bit */
    if( src->len == 0 ) {
        return MP_NO_ERR;
    }
    for( ;; firstunit-- ) {
        if( src->num[firstunit] > 0 ) {
            break;
        }
        if( firstunit == 0 ) return MP_NO_ERR; /* src = 0 */
    }
    for( ;; firstbit-- ) {
        if( src->num[firstunit] & bit ) {
            break;
        }
        bit >>= 1;
    }

    /* round this number so that we have 64 significant bits */
    MP_INIT( &rounded_num, 0 );
    MP_COPY( &rounded_num, src );
    if( firstunit * 32 + firstbit > 63 ) {
        mp_binround( &rounded_num, &rounded_num, firstunit * 32 + firstbit - 63 );
        /* if it rounded up, the first bit may be up one */
        if( firstbit == 31 ) {
            if( rounded_num.num[firstunit+1] > 0 ) {
                firstunit++;
                firstbit = 0;
            }
        } else {
            if( rounded_num.num[firstunit] & (bit * 2) ) {
                firstbit++;
            }
        }
    }

    /* generate exponent */
    exp = (uint64)firstunit * 32 + firstbit + extended_bias;
    if( exp > 32767 ) exp = 32767;
    temp = (uint8*)&exp;
    dst[9] = temp[1];
    dst[8] = temp[0];
    if( exp == 32767 ) {
        /* too large => use +infinity */
        mp_free( &rounded_num );
        return MP_LOSS_OF_PRECISION;
    }

    /* generate significand */
    left = 63 - firstbit;
    sig = (uint64)rounded_num.num[firstunit] << left;
    if( firstunit > 0 ) {
        if( left > 32 ) {
            left -= 32;
            temp2 = (uint64)rounded_num.num[firstunit-1] << left;
            sig = sig | temp2;
        } else if( left < 32 ) {
            temp2 = (uint64)rounded_num.num[firstunit-1] >> (32-left);
            sig = sig | temp2;
            left = 0;
        } else {
            temp2 = (uint64)rounded_num.num[firstunit-1];
            sig = sig | temp2;
            left = 0;
        }
    }
    if( left > 0 && firstunit > 1 ) {
        temp2 = (uint64)rounded_num.num[firstunit-2] >> (32-left);
        sig = sig | temp2;
    }
    temp = (uint8*)&sig;
    for( i = 0; i < 8; i++ ) {
        dst[i] = temp[i];
    }

    mp_free( &rounded_num );
    return MP_NO_ERR;
}

int mp_touint64( uint64 *dst, mpnum *src )
{
    mpnum limit;
    uint32 *temp = (uint32*)dst;

    MP_INIT( &limit, (uint64)0xffffffffffffffff );
    if( mp_gt( src, &limit ) ) {
        temp[0] = limit.num[0];
        temp[1] = limit.num[1];
        mp_free( &limit );
        return MP_TOO_LARGE;
    }
    temp[0] = src->num[0];
    if( src->len > 1 ) {
        temp[1] = src->num[1];
    } else {
        temp[1] = 0;
    }
    mp_free( &limit );
    return MP_NO_ERR;
}

/* returns size of num in bits */
int mp_bitsize( mpnum *num )
{
    uint32 i = num->len - 1;
    uint32 j = 32;
    uint32 bit = two_to_the_31;

    /* find first non-zero bit */
    if( num->len == 0 ) {
        return 0;
    }
    for( ;; i-- ) {
        if( num->num[i] > 0 ) {
            break;
        }
        if( i == 0 ) return 0; /* src = 0 */
    }
    for( ;; j-- ) {
        if( num->num[i] & bit ) {
            break;
        }
        bit >>= 1;
    }
    return i * 32 + j;
}

