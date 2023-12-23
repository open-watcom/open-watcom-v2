/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Intel x86 descriptor prototype.
*
****************************************************************************/


#ifndef _DESCRIPT_H_INCLUDED
#define _DESCRIPT_H_INCLUDED

/*
 * descriptor access right definitions
 */
#define DESC_ACCESS_DATA16      0x0093     /* 0000 0000 1001 0011 */
#define DESC_ACCESS_DATA16BIG   0x8093     /* 1000 0000 1001 0011 */
#define DESC_ACCESS_CODE32SMALL 0x409B     /* 0100 0000 1001 1011 */
#define DESC_ACCESS_CODE32BIG   0xC09B     /* 1100 0000 1001 1011 */
#define DESC_ACCESS_DATA32SMALL 0x4093     /* 0100 0000 1001 0011 */
#define DESC_ACCESS_DATA32BIG   0xC093     /* 1100 0000 1001 0011 */

#define DESC_ACCESS_CODE        1
#define DESC_ACCESS_DATA        2

typedef union {
    struct {
        unsigned char   accessed    : 1;
        unsigned char               : 2;
        unsigned char   execute     : 1;
        unsigned char   nonsystem   : 1;
        unsigned char   dpl         : 2;
        unsigned char   present     : 1;
    } flags;
    struct {
        unsigned char               : 1;
        unsigned char   writeable   : 1;
        unsigned char   expand_down : 1;
    } flags_data;
    struct {
        unsigned char               : 1;
        unsigned char   readable    : 1;
        unsigned char   conforming  : 1;
    } flags_exec;
    struct {
        unsigned char   type        : 2;
        unsigned char   gate        : 1;
        unsigned char   use32       : 1;
    } flags_sys;
    unsigned char   val;
} descriptor_type;

typedef union {
    struct {
        unsigned char   limit_19_16   : 4;
        unsigned char   available     : 1;
        unsigned char   use64         : 1;
        unsigned char   use32         : 1;
        unsigned char   page_granular : 1;
    } flags;
    unsigned char       val;
} descriptor_xtype;

typedef struct {
    unsigned short      limit_15_0;
    unsigned short      base_15_0;
    unsigned char       base_23_16;
    descriptor_type     u1;
    descriptor_xtype    u2;
    unsigned char       base_31_24;
} descriptor;

#define GET_DESC_BASE( desc ) \
    ( (unsigned long)(desc).base_15_0 \
    | ((unsigned long)(desc).base_23_16 << 16) \
    | ((unsigned long)(desc).base_31_24 << 24) \
    )

#define SET_DESC_BASE( desc, base ) \
    (desc).base_15_0 = (base); \
    (desc).base_23_16 = (unsigned long)(base) >> 16; \
    (desc).base_31_24 = ((unsigned long)(base) >> 24)

#define GET_DESC_LIMIT_NUM( desc ) \
    ((desc).limit_15_0 | ((unsigned long)(desc).u2.flags.limit_19_16 << 16))

#define GET_DESC_LIMIT_4K( desc ) \
    ((GET_DESC_LIMIT_NUM( desc ) << 12) | 0x0fffL)

#define GET_DESC_LIMIT( desc ) \
    ( (desc).u2.flags.page_granular \
    ? GET_DESC_LIMIT_4K( desc ) \
    : GET_DESC_LIMIT_NUM( desc ) \
    )

#define SET_DESC_LIMIT( desc, limit ) \
    (desc).limit_15_0 = (limit); \
    (desc).u2.flags.limit_19_16 = ((unsigned long)(limit) >> 16)

#endif /* _DESCRIPT_H_INCLUDED */
