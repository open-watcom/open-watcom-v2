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


#ifndef __ANALMEAN_H__
#define __ANALMEAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SIGN_BIT        (0x80)
#define NumSign( a )   ((a)& SIGN_BIT )

#ifdef fe_cfg
#    include "fe_cfg.h"
#endif

#ifdef FE_I64_MEANINGLESS
#define LARGEST_TYPE    __int64
#else
#define LARGEST_TYPE    long
#endif

typedef enum {
    CMP_VOID    = 0,
    CMP_FALSE   = 1,
    CMP_TRUE    = 2,
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
    LARGEST_TYPE val,
    LARGEST_TYPE *low,
    LARGEST_TYPE *high
);

#ifdef __cplusplus
};
#endif

#endif // __ANALMEAN_H_
