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


#include "target.h"

#define FOUND_SIGN      0x01
#define LEFT_DIGITS     0x02
#define DECIMAL         0x04
#define RIGHT_DIGITS    0x08
#define EXPONENT        0x10
#define BAD_EXPONENT    0x20
#define DOUBLE          0x40
#define LONGDOUBLE      0x80

#define _SIGN           0
#define _DECIMAL        (_SIGN+1)
#define _DIGITS         (_DECIMAL+1)
#define _EXPONENT       (_DIGITS+CONVERSION_DIGITS)
#define _STRING_SIZE    (_EXPONENT+MAX_INT_SIZE)

#define _CanonSign( canon )     (&((canon)->string[_SIGN]))
#define _CanonDecimal( canon )  (&((canon)->string[_DECIMAL]))
#define _CanonDigits( canon )   (&((canon)->string[_DIGITS]))
#define _CanonExponent( canon ) (&((canon)->string[_EXPONENT]))

#define _CanonNumber( canon )   ((canon)->string)

typedef struct canon_form {
    int         exp;
    bool        neg;
    char        string[_STRING_SIZE+1];
    byte        col;
    byte        flags;
    byte        blanks;
} canon_form;

// Floating-point conversion return values:
// ========================================

#define FLT_OK                  0       //  conversion successful
#define FLT_RANGE_EXCEEDED      1       //  range exceeded during conversion
#define FLT_INVALID             2       //  not a valid floating-point number

#define PRECISION_SINGLE        0
#define PRECISION_DOUBLE        1
#define PRECISION_EXTENDED      2
