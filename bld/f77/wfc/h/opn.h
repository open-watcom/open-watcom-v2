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


#ifndef _OPN_H_INCLUDED
#define _OPN_H_INCLUDED

//
// Flags for 'opn' field of 'itnode'
//

// DownScan codes:
// ================

#ifdef pick
#undef pick
#endif
#define pick(tok_id,dsopn_id,opn_proc) dsopn_id,

typedef enum {
#include "tokdsopn.h"
} DSOPN;

// UpScan codes:
// =============

typedef enum {
    USOPN_NONE,        // no operand
    USOPN_NNL,         // - name, no argument list
    USOPN_NWL,         // - name, with argument list
    USOPN_ARR,         // - array name, without subscript list
    USOPN_STN,         // - statement number
    USOPN_CON,         // - constant
    USOPN_ASS,         // - array name subscripted and substrung
    USOPN_SSR,         // - high bound in a(n:)

    USOPN_FLD  = 0x08, // field name of structure

    USOPN_SAFE = 0x10, // - value in a register
    USOPN_TMP  = 0x20, // - value in a temporary
    USOPN_VAL  = 0x30, // - static temporary SCB

    USOPN_ASY  = 0x40, // symbol table entry is alternate
    USOPN_SS1  = 0x80, // chr(i:i)
} USOPN;

#define WHAT_SHIFT      0       // bits to shift to make OPN_WHAT origin 0
#define USOPN_WHAT      0x07    // what type of operand is it?

#define WHERE_SHIFT     4       // bits to shift to make OPN_WHERE origin 0
#define USOPN_WHERE     0x30    // where is the operand?

#endif
