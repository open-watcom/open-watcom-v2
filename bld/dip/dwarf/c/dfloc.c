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
* Description:  DWARF location processing for the DIP interface.
*
****************************************************************************/


#include "dfdip.h"
#include "dfld.h"
#include "dfmod.h"
#include "dfaddr.h"
#include "dfaddsym.h"
#include "dfsym.h"
#include "dftype.h"
#include "dfloc.h"


dip_status SafeDCItemLocation( location_context *lc, context_item ci, location_list *ll ){
    dr_dbg_handle   old_handle;
    dip_status  rc;

    old_handle = DRGetDebug();
    rc = DCItemLocation( lc, ci, ll );
    DRSetDebug( old_handle );
    return( rc );
}

static void LocationInit( location_list *ll ){
    ll->num = 0;
    ll->flags = 0;
}

static void LocationLast( location_list *ll )
{
    int                 num;

    num = ll->num;
    if( --num >= 0 ){
        ll->e[num].bit_length = 0;
    }
}

extern void LocationCreate( location_list *ll, location_type lt, void *d ){

    ll->num = 1;
    ll->flags = 0;
    ll->e[0].bit_start = 0;
    ll->e[0].bit_length = 0;
    ll->e[0].type = lt;
    if( lt == LT_ADDR ) {
        ll->e[0].u.addr = *(address *)d;
    } else {
        ll->e[0].u.p = d;
    }
}

static void LocationJoin( location_list *to, location_list *from )
{
    int next;

    next = to->num;
    memcpy( &to->e[next], &from->e[0], from->num * sizeof( from->e[0] ) );
    to->flags |= from->flags;
    to->num += from->num;
}

extern void LocationAdd( location_list *ll, long sbits )
{
    location_entry      *le;
    unsigned long       add;
    unsigned            num;
    unsigned long       bits;

    bits = sbits;
    if( sbits < 0 ) {
        bits = -sbits;
        add = (bits + 7) / 8;
        if( ll->e[0].type == LT_ADDR ) {
            ll->e[0].u.addr.mach.offset -= add;
        } else {
            ll->e[0].u.p = (byte *)ll->e[0].u.p - add;
        }
        bits = 8 - (bits % 8);
        bits %= 8;
    }
    num = 0;
    le = &ll->e[0];
    for( ;; ) {
        if( le->bit_length == 0 ) break;
        if( le->bit_length > bits ) break;
        bits -= le->bit_length;
        ++num;
    }
    if( num != 0 ) {
        ll->num -= num;
        memcpy( &ll->e[0], le, ll->num * sizeof( ll->e[0] ) );
    }
    add = bits / 8;
    bits = bits % 8;
    ll->e[0].bit_start += bits;
    if( ll->e[0].bit_length != 0 ) ll->e[0].bit_length -= bits;
    if( ll->e[0].type == LT_ADDR ) {
        ll->e[0].u.addr.mach.offset += add;
    } else {
        ll->e[0].u.p = (byte *)ll->e[0].u.p + add;
    }
}

extern void LocationTrunc( location_list *ll, unsigned bits )
{
    unsigned    i;

    if( bits == 0 ) return;
    i = 0;
    for( ;; ) {
        if( i >= ll->num ) return;
        if( ll->e[i].bit_length == 0 ) break;
        if( ll->e[i].bit_length > bits ) break;
        bits -= ll->e[i].bit_length;
        ++i;
    }
    ll->e[i].bit_length = bits;
}

typedef enum {
    #define DW_REG( __n  )   DW_X86_##__n,
    #include "dwreginf.h"
    DW_REG( MAX )
    #undef DW_REG
}dw_X86_regs;

typedef struct {
    unsigned    ci      : 5;
    unsigned    start   : 4;
    unsigned    len     : 7;
} reg_entry;

/* WARNING: Must be kept in sync with dwreginf.h included above */

static reg_entry const CLRegX86[DW_X86_MAX] = {
    { CI_EAX, 0, 32 },       //eax
    { CI_ECX, 0, 32 },       //ecx
    { CI_EDX, 0, 32 },       //edx
    { CI_EBX, 0, 32 },       //ebx
    { CI_ESP, 0, 32 },       //esp
    { CI_EBP, 0, 32 },       //ebp
    { CI_ESI, 0, 32 },       //esi
    { CI_EDI, 0, 32 },       //edi
    { CI_EIP, 0, 32 },       //eip
    { CI_EFL, 0, 32 },       //eflags
    { 0     , 0, 0 },        //trapno
    { CI_ST0, 0, 80 },       //st0
    { CI_ST1, 0, 80 },       //st1
    { CI_ST2, 0, 80 },       //st2
    { CI_ST3, 0, 80 },       //st3
    { CI_ST4, 0, 80 },       //st4
    { CI_ST5, 0, 80 },       //st5
    { CI_ST6, 0, 80 },       //st6
    { CI_ST7, 0, 80 },       //st7
    { CI_EAX, 0, 8 },        //al
    { CI_EAX, 8, 8 },        //ah
    { CI_EBX, 0, 8 },        //bl
    { CI_EBX, 8, 8 },        //bh
    { CI_ECX, 0, 8 },        //cl
    { CI_ECX, 8, 8 },        //ch
    { CI_EDX, 0, 8 },        //dl
    { CI_EDX, 8, 8 },        //dh
    { CI_EAX, 0, 16 },       //ax
    { CI_EBX, 0, 16 },       //bx
    { CI_ECX, 0, 16 },       //cx
    { CI_EDX, 0, 16 },       //dx
    { CI_ESI, 0, 16 },       //si
    { CI_EDI, 0, 16 },       //di
    { CI_EBP, 0, 16 },       //bp
    { CI_ESP, 0, 16 },       //sp
    { CI_CS,  0, 16 },       //cs
    { CI_SS,  0, 16 },       //ss
    { CI_DS,  0, 16 },       //ds
    { CI_ES,  0, 16 },       //es
    { CI_FS,  0, 16 },       //fs
    { CI_GS,  0, 16 },       //gs
};

typedef enum {
    #define DW_REG( __n  )   DW_AXP_##__n,
    #include "dwregaxp.h"
    DW_REG( MAX )
    #undef DW_REG
}dw_axp_regs;

/* WARNING: Must be kept in sync with dwregaxp.h included above */

static uint_16 const CLRegAXP[DW_AXP_MAX] = {
        /* Alpha architecture */
        CI_AXP_r0,          //DW_AXP_r0
        CI_AXP_r1,          //DW_AXP_r1
        CI_AXP_r2,          //DW_AXP_r2
        CI_AXP_r3,          //DW_AXP_r3
        CI_AXP_r4,          //DW_AXP_r4
        CI_AXP_r5,          //DW_AXP_r5
        CI_AXP_r6,          //DW_AXP_r6
        CI_AXP_r7,          //DW_AXP_r7
        CI_AXP_r8,          //DW_AXP_r8
        CI_AXP_r9,          //DW_AXP_r9
        CI_AXP_r10,         //DW_AXP_r10
        CI_AXP_r11,         //DW_AXP_r11
        CI_AXP_r12,         //DW_AXP_r12
        CI_AXP_r13,         //DW_AXP_r13
        CI_AXP_r14,         //DW_AXP_r14
        CI_AXP_r15,         //DW_AXP_r15
        CI_AXP_r16,         //DW_AXP_r16
        CI_AXP_r17,         //DW_AXP_r17
        CI_AXP_r18,         //DW_AXP_r18
        CI_AXP_r19,         //DW_AXP_r19
        CI_AXP_r20,         //DW_AXP_r20
        CI_AXP_r21,         //DW_AXP_r21
        CI_AXP_r22,         //DW_AXP_r22
        CI_AXP_r23,         //DW_AXP_r23
        CI_AXP_r24,         //DW_AXP_r24
        CI_AXP_r25,         //DW_AXP_r25
        CI_AXP_r26,         //DW_AXP_r26
        CI_AXP_r27,         //DW_AXP_r27
        CI_AXP_r28,         //DW_AXP_r28
        CI_AXP_r29,         //DW_AXP_r29
        CI_AXP_r30,         //DW_AXP_r30
        CI_AXP_r31,         //DW_AXP_r31
        CI_AXP_f0,          //DW_AXP_f0
        CI_AXP_f1,          //DW_AXP_f1
        CI_AXP_f2,          //DW_AXP_f2
        CI_AXP_f3,          //DW_AXP_f3
        CI_AXP_f4,          //DW_AXP_f4
        CI_AXP_f5,          //DW_AXP_f5
        CI_AXP_f6,          //DW_AXP_f6
        CI_AXP_f7,          //DW_AXP_f7
        CI_AXP_f8,          //DW_AXP_f8
        CI_AXP_f9,          //DW_AXP_f9
        CI_AXP_f10,         //DW_AXP_f10
        CI_AXP_f11,         //DW_AXP_f11
        CI_AXP_f12,         //DW_AXP_f12
        CI_AXP_f13,         //DW_AXP_f13
        CI_AXP_f14,         //DW_AXP_f14
        CI_AXP_f15,         //DW_AXP_f15
        CI_AXP_f16,         //DW_AXP_f16
        CI_AXP_f17,         //DW_AXP_f17
        CI_AXP_f18,         //DW_AXP_f18
        CI_AXP_f19,         //DW_AXP_f19
        CI_AXP_f20,         //DW_AXP_f20
        CI_AXP_f21,         //DW_AXP_f21
        CI_AXP_f22,         //DW_AXP_f22
        CI_AXP_f23,         //DW_AXP_f23
        CI_AXP_f24,         //DW_AXP_f24
        CI_AXP_f25,         //DW_AXP_f25
        CI_AXP_f26,         //DW_AXP_f26
        CI_AXP_f27,         //DW_AXP_f27
        CI_AXP_f28,         //DW_AXP_f28
        CI_AXP_f29,         //DW_AXP_f29
        CI_AXP_f30,         //DW_AXP_f30
        CI_AXP_f31,         //DW_AXP_f31
};

typedef enum {
    #define DW_REG( __n  )   DW_PPC_##__n,
    #include "dwregppc.h"
    DW_REG( MAX )
    #undef DW_REG
} dw_ppc_regs;

typedef struct {
    unsigned    ci      : 8;
    unsigned    start   : 8;
    unsigned    len     : 8;
} ppcreg_entry;

/* WARNING: Must be kept in sync with dwregppc.h included above */

static ppcreg_entry const CLRegPPC[DW_PPC_MAX] = {
    /* PowerPC architecture */
    { CI_PPC_r0,  0, 32 },  //DW_PPC_r0
    { CI_PPC_r1,  0, 32 },  //DW_PPC_r1
    { CI_PPC_r2,  0, 32 },  //DW_PPC_r2
    { CI_PPC_r3,  0, 32 },  //DW_PPC_r3
    { CI_PPC_r4,  0, 32 },  //DW_PPC_r4
    { CI_PPC_r5,  0, 32 },  //DW_PPC_r5
    { CI_PPC_r6,  0, 32 },  //DW_PPC_r6
    { CI_PPC_r7,  0, 32 },  //DW_PPC_r7
    { CI_PPC_r8,  0, 32 },  //DW_PPC_r8
    { CI_PPC_r9,  0, 32 },  //DW_PPC_r9
    { CI_PPC_r10, 0, 32 },  //DW_PPC_r10
    { CI_PPC_r11, 0, 32 },  //DW_PPC_r11
    { CI_PPC_r12, 0, 32 },  //DW_PPC_r12
    { CI_PPC_r13, 0, 32 },  //DW_PPC_r13
    { CI_PPC_r14, 0, 32 },  //DW_PPC_r14
    { CI_PPC_r15, 0, 32 },  //DW_PPC_r15
    { CI_PPC_r16, 0, 32 },  //DW_PPC_r16
    { CI_PPC_r17, 0, 32 },  //DW_PPC_r17
    { CI_PPC_r18, 0, 32 },  //DW_PPC_r18
    { CI_PPC_r19, 0, 32 },  //DW_PPC_r19
    { CI_PPC_r20, 0, 32 },  //DW_PPC_r20
    { CI_PPC_r21, 0, 32 },  //DW_PPC_r21
    { CI_PPC_r22, 0, 32 },  //DW_PPC_r22
    { CI_PPC_r23, 0, 32 },  //DW_PPC_r23
    { CI_PPC_r24, 0, 32 },  //DW_PPC_r24
    { CI_PPC_r25, 0, 32 },  //DW_PPC_r25
    { CI_PPC_r26, 0, 32 },  //DW_PPC_r26
    { CI_PPC_r27, 0, 32 },  //DW_PPC_r27
    { CI_PPC_r28, 0, 32 },  //DW_PPC_r28
    { CI_PPC_r29, 0, 32 },  //DW_PPC_r29
    { CI_PPC_r30, 0, 32 },  //DW_PPC_r30
    { CI_PPC_r31, 0, 32 },  //DW_PPC_r31
    { CI_PPC_f0,  0, 64 },  //DW_PPC_f0
    { CI_PPC_f1,  0, 64 },  //DW_PPC_f1
    { CI_PPC_f2,  0, 64 },  //DW_PPC_f2
    { CI_PPC_f3,  0, 64 },  //DW_PPC_f3
    { CI_PPC_f4,  0, 64 },  //DW_PPC_f4
    { CI_PPC_f5,  0, 64 },  //DW_PPC_f5
    { CI_PPC_f6,  0, 64 },  //DW_PPC_f6
    { CI_PPC_f7,  0, 64 },  //DW_PPC_f7
    { CI_PPC_f8,  0, 64 },  //DW_PPC_f8
    { CI_PPC_f9,  0, 64 },  //DW_PPC_f9
    { CI_PPC_f10, 0, 64 },  //DW_PPC_f10
    { CI_PPC_f11, 0, 64 },  //DW_PPC_f21
    { CI_PPC_f12, 0, 64 },  //DW_PPC_f22
    { CI_PPC_f13, 0, 64 },  //DW_PPC_f23
    { CI_PPC_f14, 0, 64 },  //DW_PPC_f24
    { CI_PPC_f15, 0, 64 },  //DW_PPC_f25
    { CI_PPC_f16, 0, 64 },  //DW_PPC_f26
    { CI_PPC_f17, 0, 64 },  //DW_PPC_f27
    { CI_PPC_f18, 0, 64 },  //DW_PPC_f28
    { CI_PPC_f19, 0, 64 },  //DW_PPC_f29
    { CI_PPC_f20, 0, 64 },  //DW_PPC_f20
    { CI_PPC_f21, 0, 64 },  //DW_PPC_f21
    { CI_PPC_f22, 0, 64 },  //DW_PPC_f22
    { CI_PPC_f23, 0, 64 },  //DW_PPC_f23
    { CI_PPC_f24, 0, 64 },  //DW_PPC_f24
    { CI_PPC_f25, 0, 64 },  //DW_PPC_f25
    { CI_PPC_f26, 0, 64 },  //DW_PPC_f26
    { CI_PPC_f27, 0, 64 },  //DW_PPC_f27
    { CI_PPC_f28, 0, 64 },  //DW_PPC_f28
    { CI_PPC_f29, 0, 64 },  //DW_PPC_f29
    { CI_PPC_f30, 0, 64 },  //DW_PPC_f30
    { CI_PPC_f31, 0, 64 },  //DW_PPC_f31
};

typedef enum {
    #define DW_REG( __n  )   DW_MIPS_##__n,
    #include "dwregmps.h"
    DW_REG( MAX )
    #undef DW_REG
} dw_mips_regs;

typedef struct {
    unsigned    ci      : 8;
    unsigned    start   : 8;
    unsigned    len     : 8;
} mipsreg_entry;

/* WARNING: Must be kept in sync with dwregmips.h included above */

static mipsreg_entry const CLRegMIPS[DW_MIPS_MAX] = {
    /* MIPS architecture */
    { CI_MIPS_r0,  0, 32 }, //DW_MIPS_r0
    { CI_MIPS_r1,  0, 32 }, //DW_MIPS_r1
    { CI_MIPS_r2,  0, 32 }, //DW_MIPS_r2
    { CI_MIPS_r3,  0, 32 }, //DW_MIPS_r3
    { CI_MIPS_r4,  0, 32 }, //DW_MIPS_r4
    { CI_MIPS_r5,  0, 32 }, //DW_MIPS_r5
    { CI_MIPS_r6,  0, 32 }, //DW_MIPS_r6
    { CI_MIPS_r7,  0, 32 }, //DW_MIPS_r7
    { CI_MIPS_r8,  0, 32 }, //DW_MIPS_r8
    { CI_MIPS_r9,  0, 32 }, //DW_MIPS_r9
    { CI_MIPS_r10, 0, 32 }, //DW_MIPS_r10
    { CI_MIPS_r11, 0, 32 }, //DW_MIPS_r11
    { CI_MIPS_r12, 0, 32 }, //DW_MIPS_r12
    { CI_MIPS_r13, 0, 32 }, //DW_MIPS_r13
    { CI_MIPS_r14, 0, 32 }, //DW_MIPS_r14
    { CI_MIPS_r15, 0, 32 }, //DW_MIPS_r15
    { CI_MIPS_r16, 0, 32 }, //DW_MIPS_r16
    { CI_MIPS_r17, 0, 32 }, //DW_MIPS_r17
    { CI_MIPS_r18, 0, 32 }, //DW_MIPS_r18
    { CI_MIPS_r19, 0, 32 }, //DW_MIPS_r19
    { CI_MIPS_r20, 0, 32 }, //DW_MIPS_r20
    { CI_MIPS_r21, 0, 32 }, //DW_MIPS_r21
    { CI_MIPS_r22, 0, 32 }, //DW_MIPS_r22
    { CI_MIPS_r23, 0, 32 }, //DW_MIPS_r23
    { CI_MIPS_r24, 0, 32 }, //DW_MIPS_r24
    { CI_MIPS_r25, 0, 32 }, //DW_MIPS_r25
    { CI_MIPS_r26, 0, 32 }, //DW_MIPS_r26
    { CI_MIPS_r27, 0, 32 }, //DW_MIPS_r27
    { CI_MIPS_r28, 0, 32 }, //DW_MIPS_r28
    { CI_MIPS_r29, 0, 32 }, //DW_MIPS_r29
    { CI_MIPS_r30, 0, 32 }, //DW_MIPS_r30
    { CI_MIPS_r31, 0, 32 }, //DW_MIPS_r31
    { CI_MIPS_f0,  0, 64 }, //DW_MIPS_f0
    { CI_MIPS_f1,  0, 64 }, //DW_MIPS_f1
    { CI_MIPS_f2,  0, 64 }, //DW_MIPS_f2
    { CI_MIPS_f3,  0, 64 }, //DW_MIPS_f3
    { CI_MIPS_f4,  0, 64 }, //DW_MIPS_f4
    { CI_MIPS_f5,  0, 64 }, //DW_MIPS_f5
    { CI_MIPS_f6,  0, 64 }, //DW_MIPS_f6
    { CI_MIPS_f7,  0, 64 }, //DW_MIPS_f7
    { CI_MIPS_f8,  0, 64 }, //DW_MIPS_f8
    { CI_MIPS_f9,  0, 64 }, //DW_MIPS_f9
    { CI_MIPS_f10, 0, 64 }, //DW_MIPS_f10
    { CI_MIPS_f11, 0, 64 }, //DW_MIPS_f21
    { CI_MIPS_f12, 0, 64 }, //DW_MIPS_f22
    { CI_MIPS_f13, 0, 64 }, //DW_MIPS_f23
    { CI_MIPS_f14, 0, 64 }, //DW_MIPS_f24
    { CI_MIPS_f15, 0, 64 }, //DW_MIPS_f25
    { CI_MIPS_f16, 0, 64 }, //DW_MIPS_f26
    { CI_MIPS_f17, 0, 64 }, //DW_MIPS_f27
    { CI_MIPS_f18, 0, 64 }, //DW_MIPS_f28
    { CI_MIPS_f19, 0, 64 }, //DW_MIPS_f29
    { CI_MIPS_f20, 0, 64 }, //DW_MIPS_f20
    { CI_MIPS_f21, 0, 64 }, //DW_MIPS_f21
    { CI_MIPS_f22, 0, 64 }, //DW_MIPS_f22
    { CI_MIPS_f23, 0, 64 }, //DW_MIPS_f23
    { CI_MIPS_f24, 0, 64 }, //DW_MIPS_f24
    { CI_MIPS_f25, 0, 64 }, //DW_MIPS_f25
    { CI_MIPS_f26, 0, 64 }, //DW_MIPS_f26
    { CI_MIPS_f27, 0, 64 }, //DW_MIPS_f27
    { CI_MIPS_f28, 0, 64 }, //DW_MIPS_f28
    { CI_MIPS_f29, 0, 64 }, //DW_MIPS_f29
    { CI_MIPS_f30, 0, 64 }, //DW_MIPS_f30
    { CI_MIPS_f31, 0, 64 }, //DW_MIPS_f31
};

/*
    Stuff dealing with evaluating dwarf location expr's
*/

typedef struct {
    imp_image_handle *ii;
    location_context *lc;  /* addr context          */
    location_list    *ll;  /* where to store addr   */
    address          base; /* base segment & offset */
    dip_status       ret;
    dr_loc_kind      init; /* how to get stack going */
    uint_32          value;/* value on top of stack  */
    int              val_count;
    uint_32          seg;
} loc_handle;

static dr_loc_kind Init( void *_d, uint_32 *where )
{
    loc_handle  *d = _d;

// Set location expr initial value
    switch( d->init ){
    case DR_LOC_NONE:
        break;
    case DR_LOC_REG:
        break;
    case DR_LOC_ADDR:
       *where = 0;
        break;
    }
    return( d->init );
}


static bool Ref( void *_d, uint_32 offset, uint_32 size, dr_loc_kind kind )
{
// Collect a reference from location expr and stuff in ret ll
// Assume d->ll init num == 0, flags == 0
    loc_handle  *d = _d;
    location_list tmp, *ll;

    if( kind == DR_LOC_ADDR ){
        LocationCreate( &tmp, LT_ADDR, &d->base );
        tmp.e[0].u.addr.mach.offset = offset;
        tmp.e[0].bit_length = size*8;
    }else{
        mad_handle    kind;
        int           areg;
        int           start;

        kind =  DCCurrMAD();
        switch( kind ){
        case MAD_X86:
            areg = CLRegX86[offset].ci;
            start = CLRegX86[offset].start;
            break;
        case MAD_AXP:
            areg = CLRegAXP[offset];
            start = 0;
            break;
        case MAD_PPC:
            areg  = CLRegPPC[offset].ci;
            // This should really be dynamic; anyway the registers are really
            // stored as 64-bit values, so if we want to get at the lower 32
            // bits only, we need to start 32 bits into the storage.
#if defined( __BIG_ENDIAN__ )
            if( CLRegPPC[offset].len == 32 )
                start = 32;
            else
                start = 0;
#else
            start = CLRegPPC[offset].start;
#endif
            break;
        case MAD_MIPS:
            areg  = CLRegMIPS[offset].ci;
            // See PowerPC comments above
#if defined( __BIG_ENDIAN__ )
            if( CLRegMIPS[offset].len == 32 )
                start = 32;
            else
                start = 0;
#else
            start = CLRegMIPS[offset].start;
#endif
            break;
        case MAD_NIL:
        default:
            DCStatus( DS_ERR | DS_BAD_LOCATION );
            return( FALSE );
        }
        d->ret = SafeDCItemLocation( d->lc, areg, &tmp );
        if( d->ret != DS_OK ){
            DCStatus( d->ret );
            return( FALSE );
        }
        LocationAdd( &tmp, start );
        LocationTrunc( &tmp, size*8 );
    }
    ll = d->ll;
    LocationJoin( ll, &tmp );
    return( TRUE );
}


static bool DRef( void *_d, uint_32 *where, uint_32 offset, uint_32 size ){
// Dereference a value use default address space
// The offset on top of the stack is relative to it
    loc_handle  *d = _d;
    address     a;
    location_list tmp;
    location_list ll;
#if 1
    d->ret = SafeDCItemLocation( d->lc, CI_DEF_ADDR_SPACE, &ll );
    if( d->ret != DS_OK ){
        DCStatus( d->ret );
        return( FALSE );
    }
    a = ll.e[0].u.addr;
#else
    a = d->base;
#endif
    a.mach.offset = offset;
    LocationCreate( &ll, LT_ADDR, &a );
    ll.e[0].bit_length = size*8;
    LocationCreate( &tmp, LT_INTERNAL, where );
    d->ret = DCAssignLocation( &tmp, &ll, sizeof( *where ) );
    if( d->ret != DS_OK ){
        DCStatus( d->ret );
        return( FALSE );
    }
    return( TRUE );
}

static bool DRefX( void *_d, uint_32 *where, uint_32 offset, uint_32 seg, uint_16 size )
{
// Dereference an extended address
    loc_handle  *d = _d;
    address a;
    location_list tmp;
    location_list ll;

    a = NilAddr;
    a.mach.segment = seg;
    a.mach.offset = offset;
//  DCMapAddr( &a.mach, d->ii->dcmap );
    LocationCreate( &ll, LT_ADDR, &a );
    ll.e[0].bit_length = size*8;
    LocationCreate( &tmp, LT_INTERNAL, where );
    d->ret = DCAssignLocation( &tmp, &ll, sizeof( *where ) );
    if( d->ret != DS_OK ){
        DCStatus( d->ret );
        return( FALSE );
    }
    return( TRUE );
}

static bool Frame( void *_d, uint_32 *where )
{
    loc_handle  *d = _d;
    location_list ll;
//    mad_handle  kind;

// Get frame location
//    kind =  DCCurrMAD();
    DCCurrMAD();
    d->ret = SafeDCItemLocation( d->lc, CI_FRAME, &ll );
    if( d->ret != DS_OK ){
        DCStatus( d->ret );
        return( FALSE );
    }
    d->base = ll.e[0].u.addr; /* set base */
    *where = ll.e[0].u.addr.mach.offset;
    return( TRUE );
}

static bool Reg( void *_d, uint_32 *where, uint_16 reg )
{
// get value of reg
    loc_handle  *d = _d;
    location_list ll;
    location_list tmp;
    mad_handle  kind;
    int         areg;
    int         start;
    int         size;

    kind =  DCCurrMAD();
    switch( kind ){
    case MAD_X86:
        areg = CLRegX86[reg].ci;
        start = CLRegX86[reg].start;
        size = CLRegX86[reg].len;
        break;
    case MAD_AXP:
        areg = CLRegAXP[reg];
        start = 0;
        /* a massive kludge here */
        if( areg >= CI_AXP_f0 && areg <= CI_AXP_f31 ) {
            size = 64;
        } else {
            size = 32;
        }
        break;
    case MAD_PPC:
        areg  = CLRegPPC[reg].ci;
        /* yep, another and even worse kludge */
#if defined( __BIG_ENDIAN__ )
        if( CLRegPPC[reg].len == 32 )
            start = 32;
        else
            start = 0;
#else
        start = CLRegPPC[reg].start;
#endif
        size  = CLRegPPC[reg].len;
        break;
    case MAD_MIPS:
        areg  = CLRegMIPS[reg].ci;
        /* just as bad as PPC */
#if defined( __BIG_ENDIAN__ )
        if( CLRegMIPS[reg].len == 32 )
            start = 32;
        else
            start = 0;
#else
        start = CLRegMIPS[reg].start;
#endif
        size  = CLRegMIPS[reg].len;
        break;
    case MAD_NIL:
    default:
        DCStatus( DS_ERR | DS_BAD_LOCATION );
        return( FALSE );
    }
    d->ret = SafeDCItemLocation( d->lc, areg, &ll );
    if( d->ret != DS_OK ){
        DCStatus( d->ret );
        return( FALSE );
    }
    LocationAdd( &ll, start );
    LocationTrunc( &ll, size);
//    ll.e[0].bit_start  = start;
//    ll.e[0].bit_length = size;
    LocationCreate( &tmp, LT_INTERNAL, where );
    d->ret = DCAssignLocation( &tmp, &ll, sizeof( *where ) );
    if( d->ret != DS_OK ){
        DCStatus( d->ret );
        return( FALSE );
    }
    if( kind == MAD_X86 && (reg == DW_X86_esp || reg == DW_X86_sp) ){ /* kludge for now */
        d->ret = SafeDCItemLocation( d->lc, CI_STACK, &ll );
        if( d->ret != DS_OK ){
            DCStatus( d->ret );
            return( FALSE );
        }
        d->base = ll.e[0].u.addr;    /* set base */
        d->base.mach.offset = 0;
    }else{
        d->ret = SafeDCItemLocation( d->lc, CI_DEF_ADDR_SPACE, &ll );
        if( d->ret != DS_OK ){
            DCStatus( d->ret );
            return( FALSE );
        }
        d->base = ll.e[0].u.addr;    /* set base */
        d->base.mach.offset = 0;
    }
    return( TRUE );
}

static bool ACon( void *_d, uint_32 *where, bool isfar )
{
// relocate a map address constant
    loc_handle  *d = _d;

    d->base.mach.offset = where[0];
    if( isfar ){ /* assume next in stack is a seg and an xdref is coming */
        d->base.mach.segment = where[1];
    }else{
        d->base.mach.segment = d->seg;
    }
    DCMapAddr( &d->base.mach, d->ii->dcmap );
    where[0] = d->base.mach.offset;
    if( isfar ){ /* assume next in stack is a seg and an xdref is coming */
        where[1] = d->base.mach.segment;
    }
    return( TRUE );
}

static bool Live( void *_d, uint_32 *where )
{
// find the appropriate live range
    loc_handle  *d = _d;
    location_list ll;
//    dip_status    ret;

// Get execution location
//    ret = SafeDCItemLocation( d->lc, CI_EXECUTION, &ll );
    SafeDCItemLocation( d->lc, CI_EXECUTION, &ll );
    d->ret = DS_OK;
    if( d->ret != DS_OK ){
        DCStatus( d->ret );
        return( FALSE );
    }
    if( !Real2Map( d->ii->addr_map, &ll.e[0].u.addr ) ){
        d->ret = DS_ERR|DS_BAD_LOCATION;
        DCStatus( DS_ERR|DS_BAD_LOCATION );
    }
    *where  =  ll.e[0].u.addr.mach.offset;
    return( TRUE );
}

static dr_loc_callbck_def const CallBck = {
    Init,
    Ref,
    DRef,
    DRefX,
    Frame,
    Reg,
    ACon,
    Live
};

static bool IsEntry( imp_image_handle *ii, location_context *lc ){
    /*
        Determine if we are at function entry
    */
    location_list   ll;
    addrsym_info    info;
    dip_status      ret;
    seg_list        *addr_sym;
    imp_mod_handle  im;

// Get execution location
    ret = SafeDCItemLocation( lc, CI_EXECUTION, &ll );
    if( ret != DS_OK ){
        return( FALSE );
    }
    if( DFAddrMod( ii, ll.e[0].u.addr, &im ) == SR_NONE ){
        return( FALSE );
    }
    if( !Real2Map( ii->addr_map, &ll.e[0].u.addr ) ){
        return( FALSE );
    }
    addr_sym = DFLoadAddrSym( ii, im );
    if( FindAddrSym( addr_sym, &ll.e[0].u.addr.mach, &info ) >= 0 ){
        if( info.map_offset == ll.e[0].u.addr.mach.offset ){
            return( TRUE );
        }
    }
    return( FALSE );
}

extern dip_status EvalLocation( imp_image_handle *ii,
                                location_context *lc,
                                dr_handle         sym,
                                word              seg,
                                location_list    *ll ){
    loc_handle d;

    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    d.ii = ii;
    d.lc = lc;
    d.ll = ll;
    d.ret = DS_OK;
    d.base = NilAddr;
    d.val_count = 0;
    d.base.mach.segment = 0;
    d.base.mach.offset = 0;
    d.seg = seg;
    d.init = DR_LOC_NONE;
    LocationInit( ll ); /* set to 0 */
    if( !DRLocationAT( sym, &CallBck, &d ) ){
        if( d.ret == DS_OK ){
            d.ret = DS_FAIL;
        }
    }
    if( d.ret != DS_OK ){
        if( DRIsParm( sym ) ){
            if( IsEntry( ii, lc ) ){
                d.val_count = 0;
                d.base.mach.segment = 0;
                d.base.mach.offset = 0;
                d.init = DR_LOC_NONE;
                LocationInit( ll ); /* set to 0 */
                if( !DRParmEntryAT( sym, &CallBck, &d ) ){
                   if( d.ret == DS_OK ){
                       d.ret = DS_FAIL;
                   }
                }
            }
        }
    }
    LocationLast( ll );
    return( d.ret );
}

static bool NoFrame( void *_d, uint_32 *where )
{
//For EvalParmLocation frame not valid
    loc_handle  *d = _d;

    d->ret = DS_FAIL;
    *where = 0;
    return( FALSE  );
}

static bool FakeLive( void *_d, uint_32 *where )
{
// force loc to take first addr in live range hopefully the parm on entry
// Get execution location
    loc_handle  *d = _d;

    d->ret = DS_OK;
    *where  =  d->base.mach.offset;
    return( TRUE );
}

static bool RegOnlyRef( void *_d, uint_32 offset, uint_32 size, dr_loc_kind kind )
{
// Collect a reference from location expr and stuff in ret ll
// Assume d->ll init num == 0, flags == 0
// if not reg return FALSE parmloc requirement
    loc_handle  *d = _d;
    location_list tmp, *ll;

    if( kind == DR_LOC_ADDR ){
        d->ret = DS_FAIL;
        return( FALSE );
    }else{
        reg_entry  clreg;

        clreg = CLRegX86[offset];
        d->ret = SafeDCItemLocation( d->lc, clreg.ci, &tmp );
        if( d->ret != DS_OK ){
            DCStatus( d->ret );
            return( FALSE );
        }
        LocationAdd( &tmp, clreg.start );
        LocationTrunc( &tmp, size*8 );
    }
    ll = d->ll;
    LocationJoin( ll, &tmp );
    return( TRUE );
}
static dr_loc_callbck_def const ParmBck = {
    Init,
    RegOnlyRef,
    DRef,
    DRefX,
    NoFrame,
    Reg,
    ACon,
    FakeLive
};

extern dip_status EvalParmLocation( imp_image_handle *ii,
                                    location_context *lc,
                                    dr_handle         sym,
                                    location_list    *ll ){
    loc_handle d;

    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    d.ii = ii;
    d.lc = lc;
    d.ll = ll;
    d.ret = DS_OK;
    d.base = NilAddr;
    d.val_count = 0;
    d.seg = SEG_DATA;
    d.init = DR_LOC_NONE;
    LocationInit( ll ); /* set to 0 */
    if( !DRParmEntryAT( sym, &ParmBck, &d ) ){
       if( d.ret == DS_OK ){
           d.ret = DS_FAIL;
       }
    }
//  LocationLast( ll );
    return( d.ret );

}

extern dip_status EvalRetLocation( imp_image_handle *ii,
                                    location_context *lc,
                                    dr_handle         sym,
                                    location_list    *ll ){
    loc_handle d;

    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    d.ii = ii;
    d.lc = lc;
    d.ll = ll;
    d.ret = DS_OK;
    d.base = NilAddr;
    d.val_count = 0;
    d.seg = SEG_DATA;
    d.init = DR_LOC_NONE;
    LocationInit( ll ); /* set to 0 */
    if( !DRLocationAT( sym, &ParmBck, &d ) ){
       if( d.ret == DS_OK ){
           d.ret = DS_FAIL;
       }
    }
//  LocationLast( ll );
    return( d.ret );

}

static dr_loc_kind AdjInit( void *_d, uint_32 *where ){
// Set location expr initial value
    loc_handle  *d = _d;

    switch( d->init ){
    case DR_LOC_NONE:
        break;
    case DR_LOC_REG:
        break;
    case DR_LOC_ADDR:
       *where = d->base.mach.offset;
        break;
    }
    return( d->init );
}
static dr_loc_callbck_def const AdjBck = {
    AdjInit,
    Ref,
    DRef,
    DRefX,
    Frame,
    Reg,
    ACon,
    Live
};

extern dip_status EvalLocAdj( imp_image_handle *ii,
                               location_context *lc,
                               dr_handle         sym,
                               address          *addr ){
// locations are relative to the object
    loc_handle d;
    location_list ll;

    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    d.ii = ii;
    d.lc = lc;
    d.ll = &ll;
    d.ret = DS_OK;
    d.base = *addr;
    d.val_count = 0;
    d.init = DR_LOC_ADDR;
    LocationInit( &ll ); /* set to 0 */
    if( !DRLocBasedAT( sym, &AdjBck, &d ) ){
        if( d.ret == DS_OK ){
            d.ret = DS_FAIL;
        }
    }
    /* DWARF V2 spec is unclear at best, but DWARF V3 spells out that
     * DW_AT_data_member_location may be a location expression, in which
     * case the address of the "base" needs to be pushed on the stack and
     * the expression is expected to include a plus operator to add to it.
     * Alternatively, a constant may be specified for DW_AT_data_member_location
     * and in that case it represents an offset from the base and must be
     * explicitly added. Since I don't feel like ripping up the location
     * expression processing right now, this is the easiest place to take
     * care of it - if the location "stack" contains two entries, it means
     * we have a base (which we pushed on the stack ourselves) and an
     * offset (attribute value from DIE) that we must add to the base.
     * Note: Open Watcom as well as GCC on x86 and PowerPC never seem to
     * use constants, always location expressions. However GCC on MIPS
     * uses constants.
     */
    if( (ll.num == 2) && (ll.e[0].type == LT_ADDR) && (ll.e[1].type == LT_ADDR) ) {
        ll.e[0].u.addr.mach.offset += ll.e[1].u.addr.mach.offset;
        ll.num = 1;
    }
    *addr = ll.e[0].u.addr;
    return( d.ret );
}

static bool Val( void *_d, uint_32 offset, uint_32 size, dr_loc_kind kind )
{
// Assume top of stack is value to get
    loc_handle  *d = _d;
    size = size;
    if( kind == DR_LOC_ADDR ){
        if( ++d->val_count == 1 ){
            d->base.mach.offset = offset;
        }else{
            d->base.mach.segment = offset;
        }
    }else{
        d->ret = DS_ERR|DS_BAD_LOCATION;
        DCStatus( d->ret );
        return( FALSE );
    }
    return( TRUE );
}

static dr_loc_callbck_def const ValBck = {
    Init,
    Val,
    DRef,
    DRefX,
    Frame,
    Reg,
    ACon,
    Live
};

extern dip_status EvalBasedPtr( imp_image_handle *ii,
                                location_context *lc,
                                dr_handle         sym,
                                address          *addr ) {
    loc_handle d;
    location_list ll;

    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    if( SafeDCItemLocation( lc, CI_OBJECT, &ll ) == DS_OK ){
        d.base = ll.e[0].u.addr;
    }else{
        d.base = NilAddr;
    }
    d.ii = ii;
    d.lc = lc;
    d.ll = NULL;
    d.ret = DS_OK;
    d.seg = 0;
    d.val_count = 0;
    d.init = DR_LOC_NONE;
    if( DRSegLocation( sym, &ValBck, &d ) ){
        *addr = d.base;
    }else if( d.ret == DS_OK ){
        d.ret = DS_FAIL;
    }
    return( d.ret );

}

typedef struct {
    uint_32     offset;
    uint_32     size;
    dr_loc_kind kind;
    uint_32     live_value;
    int init  :1;
    int ref   :1;
    int dref  :1;
    int drefx :1;
    int frame :1;
    int base  :1;
    int reg   :1;
    int acon  :1;
    int live  :1;
} nop_loc_handle;

static dr_loc_kind NOPInit( void *d, uint_32 *where )
{
// Set location expr initial value
    d = d;
    where = where;
    return( DR_LOC_NONE );
}


static bool NOPRef( void *_d, uint_32 offset, uint_32 size, dr_loc_kind kind )
{
    nop_loc_handle  *d = _d;

    d->offset = offset;
    d->size = size;
    d->kind = kind;
    return( d->ref );
}

static bool NOPDRef( void *_d, uint_32 *where, uint_32 offset, uint_32 size )
{
// Dereference a value use default address space
    nop_loc_handle  *d = _d;

    offset = offset;
    size = size;
    *where = 0;
    return( d->dref );
}

static bool NOPDRefX( void *_d, uint_32 *where, uint_32 offset, uint_32 seg, uint_16 size )
{
// Dereference an extended address
    nop_loc_handle  *d = _d;

    where = where;
    offset = offset;
    seg = seg;
    size = size;
    return( d->drefx );
}

static bool NOPFrame( void *_d, uint_32 *where )
{
    nop_loc_handle  *d = _d;

    *where = 0;
    return( d->frame );
}


static bool NOPReg( void *_d, uint_32 *where, uint_16 reg )
{
    nop_loc_handle  *d = _d;

    reg = reg;
    *where = 0;
    return( d->reg );
}

static bool NOPACon( void *_d, uint_32 *where, bool isfar )
{
// relocate a map address constant
    nop_loc_handle  *d = _d;

    where = where;
    isfar = isfar;
    return( d->acon );
}

static bool NOPLive( void *_d, uint_32 *where )
{
// find the appropriate live range
    nop_loc_handle  *d = _d;

    *where = d->live_value;
    return( d->live );
}

static dr_loc_callbck_def const NOPCallBck = {
    NOPInit,
    NOPRef,
    NOPDRef,
    NOPDRefX,
    NOPFrame,
    NOPReg,
    NOPACon,
    NOPLive
};

extern bool EvalOffset( imp_image_handle *ii,
                        dr_handle         sym,
                        uint_32          *val )
//Evaluate location expr to an offset off frame
{
    nop_loc_handle  d;
    bool            ret;

    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    d.init  = TRUE;
    d.ref   = TRUE;
    d.dref  = TRUE;
    d.drefx = TRUE;
    d.frame = TRUE;
    d.base  = TRUE;
    d.reg   = TRUE;
    d.acon  = TRUE;
    d.live  = TRUE;
    d.offset = 0;
    ret = DRRetAddrLocation( sym, &NOPCallBck, &d );
    *val = d.offset;
    return( ret );

}

extern bool EvalSeg( imp_image_handle *ii,
                     dr_handle         sym,
                     uint_32          *val ){
//Evaluate location expr to an offset off frame
    nop_loc_handle d;
    bool      ret;

    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    d.init  = TRUE;
    d.ref   = TRUE;
    d.dref  = TRUE;
    d.drefx = TRUE;
    d.frame = TRUE;
    d.base  = TRUE;
    d.reg   = TRUE;
    d.acon  = TRUE;
    d.live  = TRUE;
    d.offset = 0;
    ret = DRSegLocation( sym, &NOPCallBck, &d );
    *val = d.offset;
    return( ret );

}

extern bool EvalSymOffset( imp_image_handle *ii,
                           dr_handle         sym,
                           uint_32          *val ){
//Evaluate sym's map offset
    nop_loc_handle d;
    bool      ret;

    DRSetDebug( ii->dwarf->handle ); /* must do at each call into dwarf */
    d.init  = TRUE;
    d.ref   = TRUE;
    d.dref  = FALSE;
    d.drefx = FALSE;
    d.frame = FALSE;
    d.base  = FALSE;
    d.reg   = FALSE;
    d.acon  = TRUE;
    d.live  = FALSE;
    d.offset = 0;
    ret = DRLocationAT( sym, &NOPCallBck, &d  );
    *val = d.offset;
    return( ret );

}
