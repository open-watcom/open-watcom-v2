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


static char     hexchar[] = "0123456789ABCDEF";

/*
 * Put a hex number
 */
void Puthex( unsigned_32 num, unsigned_16 width )
/***********************************************/
{
    while( width-- ) {
        Wdputc( hexchar[ ( num >> ( 4 * width ) ) & 0x000f ] );
    }
}

/*
 * Put a decimal number
 */
void Putdec( unsigned_16 num )
/****************************/
{
    if( num >= 10 )   Putdec( num / 10 );
    Wdputc( num % 10 + '0' );
}

/*
 * Put a signed decimal number
 */
void Putdecs( signed_16 num )
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
 * put a decimal number, always printing 'len' characters.
 * this will print 0 if num == 0.
*/

void Putdecl( unsigned_16 num, unsigned_16 len )
/**********************************************/
{
    Putdecbz( num/10, len - 1 );
    Wdputc( num % 10 + '0' );
}

static void DecBZRecurse( unsigned_16 num, unsigned_16 len )
/**********************************************************/
{
    if( len > 1 ) {
        DecBZRecurse( num / 10, len - 1 );
    }
    if( num == 0 ) {        /* we are to the left of the 1st digit */
        Wdputc( ' ' );
    } else {
        Wdputc( num % 10 + '0' );
    }
}

/*
 * put a decimal number, always printing 'len' characters.
 * this will print only spaces if num == 0.
 * (i.e. PUT DECimal Blank when Zero)
*/

void Putdecbz( unsigned_16 num, unsigned_16 len )
/***********************************************/
{
    if( num == 0 ) {
        while( len > 1 ) {
            Wdputc( ' ' );
            len--;
        }
        Wdputc( '0' );
    } else {
        DecBZRecurse( num, len );
    }
}

/* 16/32-bit byte swapping routines */

uint_32 get_u32( uint_32 *src )
/*****************************/
{
    if( Byte_swap ) {
        return( SWAPNC_32( *src ) );
    } else {
        return( *src );
    }
}

int_32 get_s32( int_32 *src )
/***************************/
{
    if( Byte_swap ) {
        return( SWAPNC_32( *src ) );
    } else {
        return( *src );
    }
}

uint_16 get_u16( uint_16 *src )
/*****************************/
{
    if( Byte_swap ) {
        return( SWAPNC_16( *src ) );
    } else {
        return( *src );
    }
}

int_16 get_s16( int_16 *src )
/***************************/
{
    if( Byte_swap ) {
        return( SWAPNC_16( *src ) );
    } else {
        return( *src );
    }
}
