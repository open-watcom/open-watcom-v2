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
* Description:  Watcom debug information location descriptors.
*
****************************************************************************/


#ifndef DBGLOC_INCLUDED
#define DBGLOC_INCLUDED

/* locations */

#define CLASS_MASK      0xf0
#define SUBCLASS_MASK   0x0f

#define LOC_EXPR_IND    0x80

#define NOLOCATION      0x00

#define BP_OFFSET       0x10
#define BP_OFF_BYTE     0x00
#define BP_OFF_WORD     0x01
#define BP_OFF_DWORD    0x02

#define CONSTANT        0x20
#define ADDR286         0x00
#define ADDR386         0x01
#define INT_1           0x02
#define INT_2           0x03
#define INT_4           0x04

#define MULTI_REG       0x30

#define REG             0x40

#define IND_REG         0x50
#define IR_CALLOC_NEAR  0x00
#define IR_CALLOC_FAR   0x01
#define IR_RALLOC_NEAR  0x02
#define IR_RALLOC_FAR   0x03

#define OPERATOR        0x60
#define LOP_IND_2       0x00
#define LOP_IND_4       0x01
#define LOP_IND_ADDR286 0x02
#define LOP_IND_ADDR386 0x03
#define LOP_ZEB         0x04
#define LOP_ZEW         0x05
#define LOP_MK_FP       0x06
#define LOP_POP         0x07
#define LOP_XCHG        0x08
#define LOP_ADD         0x09
#define LOP_DUP         0x0a
#define LOP_NOP         0x0b

#pragma pack( push, 1 )

/* no location, class 0x */

typedef struct {
    unsigned char       class;
} loc_void;

/* bp offset, class 1x */

typedef struct {
    unsigned char       class;
    unsigned long       offset;
} _WCUNALIGNED loc_bp_off;

/* memory, class 2x */

typedef struct {
    unsigned char       class;
    address             addr;
} _WCUNALIGNED loc_mem;

/* constant, class 2x */

typedef struct {
    unsigned char       class;
    unsigned long       val;
} _WCUNALIGNED loc_const;

/* multiple registers, class 3x */
/* register, class 4x */

#define MAX_MULTI_REGS  4
#define EMPTY_MULTI_REG ((char)-1)
typedef struct {
    unsigned char       class;
    unsigned char       numregs;
    unsigned char       regs[MAX_MULTI_REGS];
} loc_mreg;

/* ind register, class 5x */

typedef struct {
    unsigned char       class;
    unsigned char       off_reg;
} loc_ireg_near;

typedef struct {
    unsigned char       class;
    unsigned char       seg_reg;
    unsigned char       off_reg;
} loc_ireg_far;

typedef struct {
    unsigned char       class;
    unsigned char       stk;
} loc_operator;


typedef union {
    loc_void            locvoid;
    loc_bp_off          bp_offset;
    loc_mem             memory;
    loc_const           constant;
    loc_mreg            multi_reg;
    loc_ireg_near       ind_reg_near;
    loc_ireg_far        ind_reg_far;
    loc_operator        op;
} loc_entry;

#pragma pack( pop )

typedef byte            *loc_expr;

extern void             LocationCreate( location_list *ll, location_type lt, void *d );
extern void             LocationAdd( location_list *ll, long sbits );
extern void             LocationTrunc( location_list *ll, unsigned bits );
extern byte             *SkipLocation( loc_expr e );
extern location_info    InfoLocation( loc_expr e );
extern unsigned         RegSize( unsigned idx );
extern void             PushBaseLocation( location_list *ll );
extern dip_status       EvalLocation( imp_image_handle *ii, location_context *lc,
                                loc_expr e, location_list *ll );

#endif
