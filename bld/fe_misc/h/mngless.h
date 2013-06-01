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
* Description:  Interface to meaningless comparison analysis routine.
*
****************************************************************************/


#ifndef __ANALMEAN_H__
#define __ANALMEAN_H__

#include "watcom.h"
#include "i64.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CMPMAXBITSIZE   64

#define SIGN_BIT        (0x80)
#define NumSign( a )    ((a) & SIGN_BIT)
#define NumBits( a )    ((a) & (SIGN_BIT-1))

#ifdef fe_cfg
#    include "fe_cfg.h"
#endif

typedef enum{
    CMP_VOID    = 0,    // comparison fine
    CMP_FALSE   = 1,    // always false
    CMP_TRUE    = 2,    // always true
    CMP_COMPLEX = 3,    // could be simplified
} cmp_result;

typedef enum {
    REL_EQ,    // x == c
    REL_LT,    // x < c
    REL_LE,    // x <= c
    REL_SIZE
} rel_op;

cmp_result CheckMeaninglessCompare(
    rel_op rel,
    int op1_size,
    int result_size,
    int isBitField,
    signed_64 val,
    signed_64 *low,
    signed_64 *high
);

#ifdef __cplusplus
}
#endif

#endif // __ANALMEAN_H_
