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


#ifndef _FLOATLIM_H
#define _FLOATLIM_H

#include "cfloat.h"

// constants are extracted from ISO C standard 2.2.4.2.2

static struct STRUCT_cfloat( 10 )      MaxPosFloat = { // 3.40282347e+38
    39,         /* exponent ten */
    9,          /* mantissa length ten */
    0,          /* allocation length */
    1,          /* positive */
    { '3','4','0','2','8','2','3','4','7',0 }
};
static struct STRUCT_cfloat( 10 )      MinPosFloat = { // 1.17549435e-38
    -37,        /* exponent ten */
    9,          /* mantissa length ten */
    0,          /* allocation length */
    1,          /* positive */
    { '1','1','7','5','4','9','4','3','5',0 }
};
static struct STRUCT_cfloat( 18 )      MaxPosDbl = { // 1.7976931348623157e+308
    309,        /* exponent ten */
    17,         /* mantissa length ten */
    0,          /* allocation length */
    1,          /* positive */
    { '1','7','9','7','6','9','3','1','3','4','8','6','2','3','1','5','7',0 }
};
static struct STRUCT_cfloat( 18 )      MinPosDbl = { // 2.2250738585072014e-308
    -307,       /* exponent ten */
    17,         /* mantissa length ten */
    0,          /* allocation length */
    1,          /* positive */
    { '2','2','2','5','0','7','3','8','5','8','5','0','7','2','0','1','4',0 }
};

static struct STRUCT_cfloat( 10 )      MaxNegFloat = { // -3.40282347e+38
    39,         /* exponent ten */
    9,          /* mantissa length ten */
    0,          /* allocation length */
    -1,         /* negative */
    { '3','4','0','2','8','2','3','4','7',0 }
};
static struct STRUCT_cfloat( 10 )      MinNegFloat = { // -1.17549435e-38
    -37,        /* exponent ten */
    9,          /* mantissa length ten */
    0,          /* allocation length */
    -1,         /* negative */
    { '1','1','7','5','4','9','4','3','5',0 }
};
static struct STRUCT_cfloat( 18 )      MaxNegDbl = { // -1.7976931348623157e+308
    309,        /* exponent ten */
    17,         /* mantissa length ten */
    0,          /* allocation length */
    -1,         /* negative */
    { '1','7','9','7','6','9','3','1','3','4','8','6','2','3','1','5','7',0 }
};
static struct STRUCT_cfloat( 18 )      MinNegDbl = { // -2.2250738585072014e-308
    -307,       /* exponent ten */
    17,         /* mantissa length ten */
    0,          /* allocation length */
    -1,         /* negative */
    { '2','2','2','5','0','7','3','8','5','8','5','0','7','2','0','1','4',0 }
};

#endif
