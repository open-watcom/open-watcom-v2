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
* Description:  Calculate the cost of arithmetic instructions.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "model.h"

extern  byte    OptForSize;

extern  int     MulCost( unsigned_32 num )
/****************************************/
{
    int         cost;

    /* When optimizing for size, MUL/IMUL is always preferred. */
    if( OptForSize > 50 ) return( 1 );
    /* Note that in the following, the cost given for multiplication 
     * is less than the number of cycles it takes to execute, to give slight
     * preference to multiply versus a shift/add sequence.
     */
    if( _CPULevel( CPU_686 ) ) {
        /* On the Pentium Pro and up, multiply is pretty cheap. */
        cost = 3;
    } else if( _CPULevel( CPU_586 ) ) {
        /* On the Pentium, multiply takes a fixed amount of time. */
        cost = 6;
    } else if( _CPULevel( CPU_386 ) ) {
        /* The 386 and 486 have an early-out algorithm. */
        cost = 4;
        while( num ) {
            cost++;
            num >>= 1;
        }
    } else if( _CPULevel( CPU_286 ) ) {
        /* The 286 can multiply reasonably quickly. */
        cost = 18;
    } else if( _CPULevel( CPU_186 ) ) {
        /* The 81086/188 is slower. */
        cost = 28;
    } else {
        /* The 8086/8088 is a real dog! */
        cost = 120;
    }
    return( cost );
}

extern  int     AddCost( void )
/***********************/
{
    int     cost;

    if( _CPULevel( CPU_486 ) ) {
        cost = 1;
    } else if( _CPULevel( CPU_286 ) ) {
        cost = 2;
    } else {
        cost = 3;
    }
    return( cost );
}

extern  int     SubCost( void )
/***********************/
{
    return( AddCost() );
}

extern  int     ShiftCost( int count )
/*************************/
{
    int     cost;

    if( _CPULevel( CPU_586 ) ) {
        cost = 1;
    } else if( _CPULevel( CPU_486 ) ) {
        cost = 2;
    } else if( _CPULevel( CPU_386 ) ) {
        cost = 3;
    } else if( _CPULevel( CPU_186 ) ) {
        /* On the older CPUs, shift by 1 is a lot cheaper than shift immediate. */
        if( count == 1 ) {
            cost = 2;
        } else {
            cost = 5 + count;
        }
    } else {
        if( count == 8 ) {
            cost = 5;   /* Implemented as mov/xor. */
        } else if( count < 3 ) {
            cost = 2 * count;
        } else {
            cost = 8 + 4 * count + 4;   /* Add 4 for mov cl,2. */
        }
    }
    return( cost );
}
