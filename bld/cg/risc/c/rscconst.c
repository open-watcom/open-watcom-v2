/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Integer factoring for RISC architectures. Designed for
*               Alpha AXP.
*
****************************************************************************/


#include "_cgstd.h"
#include "rscconst.h"

void FactorInt32( int_32 val, int_16 *high, int_16 *extra, int_16 *low )
/***********************************************************************
 * Factor a int_32 value into 16-bit constants such that the following sequence
 *  ldah rn,high(r31)
 *  ldah rn,extra(rn)
 *  lda  rn,low(rn)
 * results in value, properly sign-extended, being in rn.
 */
{
    int_16      h, l, e;
    int_32      tmp;

    e = 0;
    l = val & 0xffff;
    tmp = val - (int_32)l;
    h = (tmp >> 16) & 0xffff;
    if( val >= 0x7fff8000 ) {
        e = 0x4000;
        tmp -= 0x40000000;
        h = (tmp >> 16) & 0xffff;
    }
    *high = h;
    *extra = e;
    *low = l;
}
