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


// UNCOMMENT_TOKEN Change this to reflect the proper size of a long double
//      - change EXP_LEN to 4
//      - change MAX_XP to 20

// maximum precision (number of digits) achievable for exponent when
// converting a binary floating point number to a string
#define EXP_LEN 3

// maximum precision (number of digits) achievable when converting a binary
// floating-point number to a string
#define MAX_SP  7       // single precision
#define MAX_DP  16      // double precision
#define MAX_XP  16      // extended precision

// number of digits required to accurately convert a floating-point string
// to its binary representation
#define CONVERSION_DIGITS 20

//                         s   0   .  <CONVERSION_DIGITS> E   s  <EXP_LEN>
//
#define CONVERSION_BUFFER (1 + 1 + 1 + CONVERSION_DIGITS+ 1 + 1 + EXP_LEN)
