/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#include "dipwat.h"
#include "watloc.h"
#include "watgbl.h"
#include "watlcl.h"


void LocationCreate( location_list *ll, location_type lt, void *d )
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

void LocationAdd( location_list *ll, long sbits )
{
    location_entry      *le;
    unsigned long       add;
    byte                num;
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
    le = ll->e;
    for( ;; ) {
        if( le->bit_length == 0 )
            break;
        if( le->bit_length > bits )
            break;
        bits -= le->bit_length;
        ++num;
    }
    if( num != 0 ) {
        ll->num -= num;
        memcpy( ll->e, le, ll->num * sizeof( ll->e[0] ) );
    }
    add = bits / 8;
    bits = bits % 8;
    ll->e[0].bit_start += bits;
    if( ll->e[0].bit_length != 0 )
        ll->e[0].bit_length -= bits;
    if( ll->e[0].type == LT_ADDR ) {
        ll->e[0].u.addr.mach.offset += add;
    } else {
        ll->e[0].u.p = (byte *)ll->e[0].u.p + add;
    }
}

void LocationTrunc( location_list *ll, unsigned bits )
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


static unsigned SizeLocation( const char *e )
{
    unsigned    subclass;

    if( MGET_U8( e ) & LOC_EXPR_IND ) {
        return( MGET_U8( e ) & ~LOC_EXPR_IND );
    }
    subclass = MGET_U8( e ) & SUBCLASS_MASK;
    switch( MGET_U8( e ) & CLASS_MASK ) {
    case NOLOCATION:
    case REG:
        return( 1 );
    case BP_OFFSET:
        subclass += (INT_1 - BP_OFF_BYTE);
        /* fall through */
    case CONSTANT:
        switch( subclass ) {
        case INT_1: /* also BP_OFF_BYTE */
            return( 2 );
        case INT_2: /* also BP_OFF_WORD */
            return( 3 );
        case INT_4: /* also BP_OFF_DWORD */
        case ADDR286:
            return( 5 );
        case ADDR386:
            return( 7 );
        }
        break;
    case MULTI_REG:
        return( subclass + 2 );
    case IND_REG:
       switch( subclass ) {
       case IR_CALLOC_NEAR:
       case IR_RALLOC_NEAR:
           return( 2 );
       case IR_CALLOC_FAR:
       case IR_RALLOC_FAR:
           return( 3 );
       }
       break;
    case OPERATOR:
        switch( subclass ) {
        case LOP_XCHG:
            return( 2 );
        default:
            return( 1 );
        }
    }
    /*
     * should never get here
     */
    return( 1 );
}


const char *SkipLocation( const char *e )
{
    return( e + SizeLocation( e ) );
}


location_info InfoLocation( const char *e )
{
    static signed char OpNeed[] = { 1, 1, 1, 1, 1, 1,  2,  1, 1,  2, 1, 0 };
    static signed char OpAdj[]  = { 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, 1, 0 };
    unsigned        size;
    unsigned        item;
    int         depth;
    location_info       info;
    unsigned    subclass;

    size = SizeLocation( e );
    if( MGET_U8( e ) & LOC_EXPR_IND ) {
        --size;
        ++e;
    }
    info = NEED_NOTHING | EMPTY_EXPR;
    depth = 0;
    while( size != 0 ) {
        switch( MGET_U8( e ) & CLASS_MASK ) {
        case BP_OFFSET:
        case MULTI_REG:
        case REG:
        case IND_REG:
            info |= NEED_REGISTERS;
            /* fall through */
        case CONSTANT:
            info &= ~EMPTY_EXPR;
            ++depth;
            break;
        case OPERATOR:
            subclass = MGET_U8( e ) & SUBCLASS_MASK;
            if( subclass != LOP_NOP )
                info &= ~EMPTY_EXPR;
            if( depth < OpNeed[subclass] )
                info |= NEED_BASE;
            depth += OpAdj[subclass & SUBCLASS_MASK];
            break;
        }
        item = SizeLocation( e );
        e += item;
        size -= item;
    }
    return( info );
}


static const char *ParseLocEntry( imp_image_handle *iih, const char *ptr,
                        loc_entry *location )
{
    byte    numregs;

    location->bp_offset.class = MGET_U8( ptr );
    ptr++;
    switch( location->bp_offset.class & CLASS_MASK ) {
    case BP_OFFSET :
        switch( location->bp_offset.class ) {
        case BP_OFFSET | BP_OFF_BYTE:
            location->bp_offset.offset = MGET_S8( ptr );
            ptr += 1;
            break;
        case BP_OFFSET | BP_OFF_WORD:
            location->bp_offset.offset = MGET_S16( ptr );
            ptr += 2;
            break;
        case BP_OFFSET | BP_OFF_DWORD:
            location->bp_offset.offset = MGET_S32( ptr );
            ptr += 4;
            break;
        }
        location->bp_offset.class = BP_OFFSET;
        break;
    case CONSTANT:
        switch( location->memory.class ) {
        case CONSTANT | ADDR386:
            ptr = GetAddress( iih, ptr, &location->memory.addr, 1 );
            break;
        case CONSTANT | ADDR286:
            ptr = GetAddress( iih, ptr, &location->memory.addr, 0 );
            location->constant.class = CONSTANT | ADDR386;
            break;
        case CONSTANT | INT_1:
            location->constant.val = MGET_S8( ptr );
            ptr += 1;
            location->constant.class = CONSTANT | INT_4;
            break;
        case CONSTANT | INT_2:
            location->constant.val = MGET_S16( ptr );
            ptr += 2;
            location->constant.class = CONSTANT | INT_4;
            break;
        case CONSTANT | INT_4:
            location->constant.val = MGET_S32( ptr );
            ptr += 4;
            location->constant.class = CONSTANT | INT_4;
            break;
        }
        break;
    case MULTI_REG :
        numregs = (location->multi_reg.class & SUBCLASS_MASK) + 1;
        location->multi_reg.class &= CLASS_MASK;
        location->multi_reg.numregs = numregs;
        do {
            location->multi_reg.regs[location->multi_reg.numregs - numregs] = *ptr++;
        } while( --numregs != 0 );
        break;
    case REG :
        location->multi_reg.regs[0] = location->multi_reg.class & SUBCLASS_MASK;
        location->multi_reg.numregs = 1;
        location->multi_reg.class = MULTI_REG;
        break;
    case IND_REG :
        switch( location->ind_reg_far.class & SUBCLASS_MASK ) {
        case IR_CALLOC_NEAR:
        case IR_RALLOC_NEAR:
            location->ind_reg_near.off_reg = MGET_U8( ptr );
            ptr++;
            break;
        default:
            location->ind_reg_far.off_reg = MGET_U8( ptr );
            ptr++;
            location->ind_reg_far.seg_reg = MGET_U8( ptr );
            ptr++;
            break;
        }
        break;
    case OPERATOR:
        switch( location->op.class ) {
        case OPERATOR | LOP_XCHG:
            location->op.stk = MGET_U8( ptr );
            ptr++;
            break;
        }
        break;
    case NOLOCATION :
    default:
        location->locvoid.class = NOLOCATION;
        break;
    }
    return( ptr );
}

typedef struct {
    unsigned    ci      : 5;
    unsigned    start   : 4;
    unsigned    len     : 7;
} reg_entry;

static const reg_entry RegTable[] = {
    #define pick(name,ci,start,len) { ci, start, len },
    #include "watdbreg.h"
    #undef pick
};

unsigned RegSize( unsigned idx )
{
    return( RegTable[idx].len / 8 );
}

struct loc_stack_entry {
    union {
        unsigned_32     num;
        location_list   ll;
    }           u;
    enum {
        LS_NUM,
        LS_ADDR,
    }           type;
};

#define MAX_LOC_STACK   10

static struct loc_stack_entry LocStack[MAX_LOC_STACK];
static int LocStkPtr = 0;

void PushBaseLocation( location_list *ll )
{
    LocStack[LocStkPtr].u.ll = *ll;
    LocStack[LocStkPtr].type = LS_ADDR;
    ++LocStkPtr;
}


dip_status EvalLocation( imp_image_handle *iih, location_context *lc, const char *e,
                        location_list *ll )
{
    const char                  *end;
    struct loc_stack_entry      *sp, *op1, *start;
    int                         i;
    int                         j;
    loc_entry                   loc;
    union {
      struct loc_stack_entry    lse;
      location_list             ll;
      unsigned_32               num;
      address                   addr;
    }                           tmp;
    union {
        unsigned_16             so;
        unsigned_32             lo;
        addr32_ptr              sa;
        addr48_ptr              la;
    }                           item;
    unsigned                    item_size;
    reg_entry                   const *reg;
    int                         item_addr;
    dip_status                  ds;

    end = SkipLocation( e );
    if( (unsigned char)*e & LOC_EXPR_IND ) {
        ++e;
    }
    ds = DS_OK;
    sp = LocStack + LocStkPtr - 1;
    start = sp;
    while( e < end ) {
        e = ParseLocEntry( iih, e, &loc );
        switch( loc.locvoid.class ) {
        case BP_OFFSET:
            ++sp;
            sp->type = LS_ADDR;
            ds = DCItemLocation( lc, CI_FRAME, &sp->u.ll );
            if( ds != DS_OK ) {
                DCStatus( ds );
                goto done;
            }
            LocationAdd( &sp->u.ll, loc.bp_offset.offset * 8 );
            break;
        case CONSTANT | ADDR386:
            ++sp;
            sp->type = LS_ADDR;
            LocationCreate( &sp->u.ll, LT_ADDR, &loc.memory.addr );
            break;
        case CONSTANT | INT_4:
            ++sp;
            sp->type = LS_NUM;
            sp->u.num = loc.constant.val;
            break;
        case MULTI_REG:
            ++sp;
            sp->type = LS_ADDR;
            sp->u.ll.flags = 0;
            j = 0;
            for( i = 0; i < loc.multi_reg.numregs; ++i ) {
                reg = RegTable + loc.multi_reg.regs[i];
                ds = DCItemLocation( lc, reg->ci, &tmp.ll );
                if( ds != DS_OK ) {
                    DCStatus( ds );
                    goto done;
                }
                memcpy( sp->u.ll.e + j, tmp.ll.e,
                        tmp.ll.num * sizeof( tmp.ll.e[0] ) );
                sp->u.ll.e[j].bit_start += reg->start;
                sp->u.ll.e[j].bit_length = reg->len;
                j += tmp.ll.num;
                sp->u.ll.flags |= tmp.ll.flags;
            }
            sp->u.ll.num = j;
            break;
        case OPERATOR | LOP_IND_2:
            item_addr = 0;
            item_size = 2;
            goto do_ind;
        case OPERATOR | LOP_IND_4:
            item_addr = 0;
            item_size = 4;
            goto do_ind;
        case OPERATOR | LOP_IND_ADDR286:
            item_addr = 1;
            item_size = 4;
            goto do_ind;
        case OPERATOR | LOP_IND_ADDR386:
            item_addr = 1;
            item_size = 6;
do_ind:
            if( sp->type == LS_NUM ) {
                tmp.num = sp->u.num;
                ds = DCItemLocation( lc, CI_DEF_ADDR_SPACE, &sp->u.ll );
                if( ds != DS_OK )
                    goto done;
                sp->u.ll.e[0].u.addr.mach.offset = tmp.num;
            }
            LocationCreate( &tmp.ll, LT_INTERNAL, &item );
            ds = DCAssignLocation( &tmp.ll, &sp->u.ll, item_size );
            if( ds != DS_OK ) {
                DCStatus( ds );
                goto done;
            }
            if( item_addr ) {
                if( item_size == 4 ) {
                    ConvAddr32ToAddr48( item.sa, tmp.addr.mach );
                } else {
                    tmp.addr.mach = item.la;
                }
                DCAddrSection( &tmp.addr );
                LocationCreate( &sp->u.ll, LT_ADDR, &tmp.addr );
                sp->type = LS_ADDR;
            } else {
                if( item_size == 2 ) {
                    sp->u.num = item.so;
                } else {
                    sp->u.num = item.lo;
                }
                sp->type = LS_NUM;
            }
            break;
        case OPERATOR | LOP_ZEB:
            sp->u.num &= (unsigned_32) 0xff;
            break;
        case OPERATOR | LOP_ZEW:
            sp->u.num &= (unsigned_32) 0xffff;
            break;
        case OPERATOR | LOP_MK_FP:
            op1 = sp;
            --sp;
            if( sp->type == LS_NUM ) {
                tmp.addr = NilAddr;
                tmp.addr.mach.segment = sp->u.num;
            } else {
                if( sp->u.ll.num != 1 || sp->u.ll.e[0].type != LT_ADDR ) {
                    ds = DS_ERR | DS_BAD_PARM;
                    goto done;
                }
                tmp.addr = sp->u.ll.e[0].u.addr;
            }
            if( op1->type == LS_NUM ) {
                tmp.addr.mach.offset = op1->u.num;
            } else {
                if( op1->u.ll.num != 1 || op1->u.ll.e[0].type != LT_ADDR ) {
                    ds = DS_ERR | DS_BAD_LOCATION;
                    goto done;
                }
                tmp.addr.mach.offset = op1->u.ll.e[0].u.addr.mach.offset;
            }
            if( sp->type == LS_NUM ) {
                DCAddrSection( &tmp.addr );
            }
            LocationCreate( &sp->u.ll, LT_ADDR, &tmp.addr );
            sp->type = LS_ADDR;
            break;
        case OPERATOR | LOP_POP:
            --sp;
            break;
        case OPERATOR | LOP_XCHG:
            tmp.lse = sp[0];
            sp[0] = sp[-loc.op.stk];
            sp[-loc.op.stk] = tmp.lse;
            break;
        case OPERATOR | LOP_ADD:
            op1 = sp;
            --sp;
            if( op1->type == LS_ADDR ) {
                /*
                 * get the address into sp
                 */
                tmp.lse = *sp;
                *sp = *op1;
                *op1 = tmp.lse;
            }
            if( sp->type == LS_ADDR ) {
                if( op1->type == LS_ADDR ) {
                    tmp.num = op1->u.ll.e[0].u.addr.mach.offset;
                } else {
                    tmp.num = op1->u.num;
                }
                LocationAdd( &sp->u.ll, tmp.num * 8 );
                sp->type = LS_ADDR;
            } else {
                sp->u.num += op1->u.num;
                sp->type = LS_NUM;
            }
            break;
        case OPERATOR | LOP_DUP:
            sp[1] = sp[0];
            ++sp;
            break;
        case OPERATOR | LOP_NOP:
            /*
             * well, what did you expect?
             */
            break;
        }
    }
    if( LocStkPtr == 0 && sp == start ) {
        /*
         * empty location
         */
        ds = DS_ERR | DS_BAD_LOCATION;
        goto done;
    }
    if( sp->type == LS_ADDR ) {
        *ll = sp->u.ll;
    } else {
        DCItemLocation( lc, CI_DEF_ADDR_SPACE, ll );
        ll->e[0].u.addr.mach.offset = sp->u.num;
    }
done:
    LocStkPtr = 0;
    return( ds );
}
