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


#include "cpplib.h"

static uint_8 mask_on[]         // bits mask for byte (set,test)
    = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

static uint_8 mask_off[]        // bits mask for byte (reset)
    = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };


extern "C"
uint_8 CPPLIB( bit_test )(      // TEST IF BIT IS ON
    uint_8* bits,               // - bits
    offset_t bit_no )           // - bit_no
{
    size_t offset;              // - offset within "bits"

    offset = bit_no / 8;
    bit_no -= ( offset * 8 );
    return (uint_8)( mask_on[ bit_no ] & bits[ offset ] );
}


extern "C"
void CPPLIB( bit_off )(         // TURN BIT OFF
    uint_8* bits,               // - bits
    offset_t bit_no )           // - bit_no
{
    size_t offset;              // - offset within "bits"

    offset = bit_no / 8;
    bit_no -= ( offset * 8 );
    bits[ offset ] &= mask_off[ bit_no ];
}
