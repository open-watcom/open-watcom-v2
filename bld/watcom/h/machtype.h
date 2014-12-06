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
* Description:  Definition of widely used fundamental machine types.
*
****************************************************************************/


#ifndef MACH_TYPES_INCLUDED
#define MACH_TYPES_INCLUDED
#include "watcom.h"

#include "pushpck1.h"

#define BITS_PER_BYTE   8U

typedef unsigned_32     dword;
typedef unsigned_16     word;
typedef unsigned_8      byte;

typedef word            addr32_off;
typedef dword           addr48_off;
typedef word            addr_seg;

typedef struct {
    byte        mantissa[2];
    word        xmant   : 7;
    word        exp     : 8;
    word        sign    : 1;
} sreal_fields;

typedef struct {
    byte        mantissa[6];
    word        xmant   : 4;
    word        exp     : 11;
    word        sign    : 1;
} lreal_fields;

typedef struct {
    byte        mantissa[8];
    word        exp     : 15;
    word        sign    : 1;
} xreal_fields;

typedef union {
    sreal_fields        f;
    float               r;
} sreal;

typedef union {
    lreal_fields        f;
    double              r;
} lreal;

typedef union {
    xreal_fields        f;
    long double         r;      /* for when compiler really supports it */
} xreal;

typedef struct {
    float       re;
    float       im;
} scomplex;

typedef struct {
    double      re;
    double      im;
} lcomplex;

typedef struct {
    xreal       re;
    xreal       im;
} xcomplex;

typedef struct {
    addr32_off  offset;
    addr_seg    segment;
} addr32_ptr;

typedef struct {
    addr48_off  offset;
    addr_seg    segment;
} addr48_ptr;

typedef addr48_ptr  addr_ptr;
typedef addr48_off  addr_off;

typedef struct {
    addr_ptr    mach;
    word        sect_id: 15;
    word        indirect: 1;
} address;

typedef struct {
    address         start;
    unsigned long   len;
} mem_block;

#define SET_NIL_ADDR( addr )    \
        { addr.indirect = 0; addr.sect_id = 0; addr.mach.segment = 0; addr.mach.offset = 0; }

#define IS_NIL_ADDR( addr )     ( (addr.indirect == 0)          \
                                && (addr.mach.segment == 0)     \
                                && (addr.mach.offset == 0) )

#define ConvAddr32ToAddr48( addr32, addr48 ) \
\
        (addr48).segment = (addr32).segment; \
        (addr48).offset  = (addr32).offset;

#define ConvAddr48ToAddr32( addr48, addr32 ) \
\
        (addr32).segment = (addr48).segment; \
        (addr32).offset  = (addr48).offset;

#include "poppck.h"

#endif
