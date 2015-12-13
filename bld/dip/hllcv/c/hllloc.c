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
* Description:  HLL/CV location support.
*
****************************************************************************/


#include "hllinfo.h"

/* FIXME: kick out these! They are all WRONG! */
#include "cv4w.h"

enum {
#define _CVREG( name, num )     CV_X86_##name = num,
#include "cv4intl.h"
#undef _CVREG
#define _CVREG( name, num )     CV_AXP_##name = num,
#include "cv4axp.h"
CV_LAST_REG
};


extern address          NilAddr;

void hllLocationCreate( location_list *ll, location_type lt, void *d )
{
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

void hllLocationAdd( location_list *ll, long sbits )
{
    location_entry      *le;
    unsigned long       add;
    unsigned            num;
    unsigned long       bits;

    bits = sbits;
    if( sbits < 0 ) {
        bits = -bits;
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

void hllLocationTrunc( location_list *ll, unsigned bits )
{
    byte    i;

    if( bits != 0 ) {
        for( i = 0; i < ll->num; ++i ) {
            if( ll->e[i].bit_length == 0  || ll->e[i].bit_length > bits ) {
                ll->e[i].bit_length = (word)bits;
                break;
            }
            bits -= ll->e[i].bit_length;
        }
    }
}

typedef struct {
    unsigned    ci      : 10;
    unsigned    start   : 2;
    unsigned    len     : 4;
} reg_entry;

static const reg_entry X86_CPURegTable[] = {
    { CI_EAX, 0, 1 },
    { CI_ECX, 0, 1 },
    { CI_EDX, 0, 1 },
    { CI_EBX, 0, 1 },
    { CI_EAX, 1, 1 },
    { CI_ECX, 1, 1 },
    { CI_EDX, 1, 1 },
    { CI_EBX, 1, 1 },

    { CI_EAX, 0, 2 },
    { CI_ECX, 0, 2 },
    { CI_EDX, 0, 2 },
    { CI_EBX, 0, 2 },
    { CI_ESP, 0, 2 },
    { CI_EBP, 0, 2 },
    { CI_ESI, 0, 2 },
    { CI_EDI, 0, 2 },

    { CI_EAX, 0, 4 },
    { CI_ECX, 0, 4 },
    { CI_EDX, 0, 4 },
    { CI_EBX, 0, 4 },
    { CI_ESP, 0, 4 },
    { CI_EBP, 0, 4 },
    { CI_ESI, 0, 4 },
    { CI_EDI, 0, 4 },

    { CI_ES,  0, 2 },
    { CI_CS,  0, 2 },
    { CI_SS,  0, 2 },
    { CI_DS,  0, 2 },
    { CI_FS,  0, 2 },
    { CI_GS,  0, 2 },

    { CI_EIP, 0, 2 },
    { CI_EFL, 0, 2 },
    { CI_EIP, 0, 4 },
    { CI_EFL, 0, 4 },
};

static const reg_entry X86_FPURegTable[] = {
    { CI_ST0, 0, 10 },
    { CI_ST1, 0, 10 },
    { CI_ST2, 0, 10 },
    { CI_ST3, 0, 10 },
    { CI_ST4, 0, 10 },
    { CI_ST5, 0, 10 },
    { CI_ST6, 0, 10 },
    { CI_ST7, 0, 10 },
    { CI_SW,  0, 2 },
    { CI_CW,  0, 2 },
};

#undef _CVREG
#define _CVREG( name, num ) { CI_AXP_##name, 0, 8 },

#define CI_AXP_fpcr     CI_LAST
#define CI_AXP_psr      CI_LAST
#define CI_AXP_fltfsr   CI_LAST

static const reg_entry AXP_RegTable[] = {
    #include "cv4axp.h"
};

dip_status hllLocationManyReg( imp_image_handle *ii, unsigned count,
                               const unsigned_8 *reg_list,
                               location_context *lc, location_list *ll )
{
    int                 i;
    int                 j;
    unsigned            idx;
    const reg_entry     *reg;
    location_list       reg_ll;
    dip_status          ds;

    j = 0;
    for( i = count-1; i >= 0; --i ) {
        idx = reg_list[i];
        switch( ii->mad ) {
        case MAD_X86:
            if( idx >= CV_X86_AL && idx <= CV_X86_EFLAGS ) {
                reg = &X86_CPURegTable[idx-CV_X86_AL];
            } else if( idx >= CV_X86_ST0 && idx <= CV_X86_STATUS ) {
                reg = &X86_FPURegTable[idx-CV_X86_ST0];
            } else {
                DCStatus( DS_ERR|DS_FAIL );
                return( DS_ERR|DS_FAIL );
            }
            break;
        case MAD_AXP:
            if( !(idx >= CV_AXP_f0 && idx <= CV_AXP_fltfsr) ) {
                DCStatus( DS_ERR|DS_FAIL );
                return( DS_ERR|DS_FAIL );
            }
            reg = &AXP_RegTable[idx-CV_AXP_f0];
            if( reg->ci == CI_LAST ) {
                DCStatus( DS_ERR|DS_FAIL );
                return( DS_ERR|DS_FAIL );
            }
            break;
        default:
            DCStatus( DS_ERR|DS_FAIL );
            return( DS_ERR|DS_FAIL );
        }
        ds = DCItemLocation( lc, reg->ci, &reg_ll );
        if( ds != DS_OK ) {
            DCStatus( ds );
            return( ds );
        }
        memcpy( &ll->e[j], &reg_ll.e[0], reg_ll.num * sizeof( reg_ll.e[0] ) );
        ll->e[j].bit_start += reg->start * BITS_PER_BYTE;
        ll->e[j].bit_length = reg->len * BITS_PER_BYTE;
        j += reg_ll.num;
        ll->flags |= reg_ll.flags;
    }
    ll->num = j;
    return( DS_OK );
}

dip_status hllLocationOneReg( imp_image_handle *ii, unsigned reg,
                              location_context *lc, location_list *ll )
{
    unsigned_8  reg_list;

    reg_list = reg;
    return( hllLocationManyReg( ii, 1, &reg_list, lc, ll ) );
}
