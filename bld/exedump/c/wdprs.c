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
* Description:  Executable dumper utility routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <ctype.h>

#include "wdglb.h"
#include "wdfunc.h"


static const char   hexchar[] = "0123456789ABCDEF";

/*
 * Put a hex number
 */
void Puthex( unsigned_32 num, unsigned_16 width )
/***********************************************/
{
    while( width-- ) {
        Wdputc( hexchar[( num >> ( 4 * width ) ) & 0x000f] );
    }
}

/*
 * Put a hex number 64-bit
 */
void Puthex64( long long num, unsigned_16 width )
/*********************************************/
{
    while( width-- ) {
        Wdputc( hexchar[( num >> ( 4 * width ) ) & 0x000f] );
    }
}

/*
 * Put a decimal number
 */
void Putdec( unsigned_32 num )
/****************************/
{
    if( num >= 10 )
        Putdec( num / 10 );
    Wdputc( num % 10 + '0' );
}

/*
 * Put a signed decimal number
 */
void Putdecs( signed_32 num )
/***************************/
{
    if( num < 0 ) {
        Wdputc( '-' );
        Putdec( -num );
    } else {
        Putdec( num );
    }
}

/*
 * Put a decimal number 64-bit
 */
void Putdec64( long long num )
/****************************/
{
    if( num >= 10 )
        Putdec64( num / 10 );
    Wdputc( num % 10 + '0' );
}

/*
 * put a decimal number, always printing 'len' characters.
 * this will print 0 if num == 0.
*/

void Putdecl( unsigned_32 num, unsigned_16 len )
/**********************************************/
{
    Putdecbz( num /10, len - 1 );
    Wdputc( num % 10 + '0' );
}

void Putdecl64( long long num, unsigned_16 len )
/**********************************************/
{
    Putdecbz( (unsigned_32)( num / 10 ), len - 1 );
    Wdputc( num % 10 + '0' );
}

static void DecBZRecurse( unsigned_32 num, unsigned_16 len, char minus )
/*********************************************************************/
{
    if( num > 0 ) {
        if( len > 1 ) {
            DecBZRecurse( num / 10, len - 1, minus );
        } else {
            DecBZRecurse( num / 10, 0, minus );
        }
    }
    if( num == 0 ) {        /* we are to the left of the 1st digit */
        while( len-- > 0 ) {
            Wdputc( ' ' );
        }
    } else {
        if( num < 10 && minus )
            Wdputc( '-' );
        Wdputc( num % 10 + '0' );
    }
}

/*
 * put a decimal number, always printing 'len' characters.
 * this will print only spaces if num == 0.
 * (i.e. PUT DECimal Blank when Zero)
*/

void Putdecbz( unsigned_32 num, unsigned_16 len )
/***********************************************/
{
    if( num == 0 ) {
        while( len-- > 1 ) {
            Wdputc( ' ' );
        }
        Wdputc( ' ' );
    } else {
        DecBZRecurse( num, len, 0 );
    }
}

static void DecBZRecurse64( long long num, unsigned_16 len, char minus )
/**********************************************************************/
{
    if( num > 0 ) {
        if( len > 1 ) {
            DecBZRecurse64( num / 10, len - 1, minus );
        } else {
            DecBZRecurse64( num / 10, 0, minus );
        }
    }
    if( num == 0 ) {        /* we are to the left of the 1st digit */
        while( len-- > 0 ) {
            Wdputc( ' ' );
        }
    } else {
        if( num < 10 && minus )
            Wdputc( '-' );
        Wdputc( num % 10 + '0' );
    }
}

/*
 * put a decimal number, always printing 'len' characters.
 * this will print only spaces if num == 0.
 * (i.e. PUT DECimal Blank when Zero)
*/

void Putdecbz64( long long num, unsigned_16 len )
/***********************************************/
{
    if( num == 0 ) {
        while( len-- > 1 ) {
            Wdputc( ' ' );
        }
        Wdputc( '0' );
    } else {
        DecBZRecurse64( num, len, 0 );
    }
}

void Putdecsbz( signed_32 num, unsigned_16 len )
/**********************************************/
{
    if( num == 0 ) {
        while( len-- > 1 ) {
            Wdputc( ' ' );
        }
        Wdputc( '0' );
    } else if( num < 0 ) {
        DecBZRecurse( -num, len - 1, 1 );
    } else {
        DecBZRecurse( num, len, 0 );
    }
}

/* 16/32-bit byte swapping routines */

unsigned_32 get_u32( unsigned_32 *src )
/*************************************/
{
    if( Byte_swap ) {
        return( SWAPNC_32( *src ) );
    } else {
        return( *src );
    }
}

signed_32 get_s32( signed_32 *src )
/*********************************/
{
    if( Byte_swap ) {
        return( SWAPNC_32( *src ) );
    } else {
        return( *src );
    }
}

unsigned_16 get_u16( unsigned_16 *src )
/*************************************/
{
    if( Byte_swap ) {
        return( SWAPNC_16( *src ) );
    } else {
        return( *src );
    }
}

signed_16 get_s16( signed_16 *src )
/*********************************/
{
    if( Byte_swap ) {
        return( SWAPNC_16( *src ) );
    } else {
        return( *src );
    }
}
