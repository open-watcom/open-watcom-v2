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
* Description:  Stack and call analysis routines
*
****************************************************************************/


#include <string.h>
#include "x86.h"
#include "madregs.h"

#define OP_1 0
#define OP_2 1
#define OP_3 2

mad_string              DIGENTRY MICallStackGrowsUp( void )
{
    return( MS_FAIL );
}

const mad_string        *DIGENTRY MICallTypeList( void )
{
    static const mad_string list[] = { MAD_MSTR_NEAR, MAD_MSTR_FAR, MAD_MSTR_INTERRUPT, MAD_MSTR_NIL };

    return( list );
}


mad_status      DIGENTRY MICallBuildFrame( mad_string call, address ret, address rtn, const mad_registers *in, mad_registers *out )
{
    unsigned    dec;
    unsigned_32 value;
    address     sp;

    dec = BIG_SEG( rtn ) ? 4 : 2;
    if( call == MAD_MSTR_NIL ) {
        call = (dec == 2) ? MAD_MSTR_FAR : MAD_MSTR_NEAR;
    }
    out->x86 = in->x86;
    sp = GetRegSP( out );
    switch( call ) {
    case MAD_MSTR_INTERRUPT:
        sp.mach.offset -= dec;
        value = out->x86.cpu.efl;
        MCWriteMem( sp, dec, &value );
        /* fall through */
    case MAD_MSTR_FAR:
        sp.mach.offset -= dec;
        value = ret.mach.segment;
        MCWriteMem( sp, dec, &value );
        /* fall through */
    case MAD_MSTR_NEAR:
        sp.mach.offset -= dec;
        value = ret.mach.offset;
        MCWriteMem( sp, dec, &value );
        break;
    }
    out->x86.cpu.esp = sp.mach.offset;
    out->x86.cpu.cs  = rtn.mach.segment;
    out->x86.cpu.eip = rtn.mach.offset;
    return( MS_OK );
}

const mad_reg_info      *DIGENTRY MICallReturnReg( mad_string call, address rtn )
{
    call = call;
    if( BIG_SEG( rtn ) ) {
        return( &CPU_eax.info );
    } else {
        return( &CPU_ax.info );
    }
}

const mad_reg_info      **DIGENTRY MICallParmRegList( mad_string call, address rtn )
{
    static const mad_reg_info *list32[] =
        { &CPU_eax.info, &CPU_edx.info, &CPU_ebx.info, &CPU_ecx.info, NULL };
    static const mad_reg_info *list16[] =
        { &CPU_ax.info,  &CPU_dx.info,  &CPU_bx.info,  &CPU_cx.info, NULL };

    call = call;
    if( BIG_SEG( rtn ) ) {
        return( list32 );
    } else {
        return( list16 );
    }
}

/*
    WHAT'S a halfways normal prolog look like? Here are some snippets

----------------------------------------------------------------------
            PUSH ESP
            PUSH c
            PUSH c
            CALL __TNK/__TNC
----------------------------------------------------------------------
            CALL __GETDS
----------------------------------------------------------------------
            PUSH c
            CALL __GRO
----------------------------------------------------------------------
            PUSH c
            CALL __CHK
----------------------------------------------------------------------
            CALL __PRO
----------------------------------------------------------------------
            INC     BP
            PUSH    BP
            MOV     BP,SP
            SUB     SP,n
----------------------------------------------------------------------
            {
                PUSH    DS
                POP     AX
                NOP
            }
            {
                NOP
                NOP
                NOP
            }
            {
                MOV     AX,DGROUP
            }
            INC     BP
            PUSH    BP
            MOV     BP,SP
            PUSH    DS
            MOV     DS,AX
            SUB SP,n
----------------------------------------------------------------------
            PUSH        BP
            MOV BP,SP
            SUB SP,n
----------------------------------------------------------------------
            ENTER X,0
----------------------------------------------------------------------
            SUB BP,n
----------------------------------------------------------------------
            SUB sp,c
----------------------------------------------------------------------
            AND sp,-8
----------------------------------------------------------------------
            { PUSH AX }
            MOV AX,c
            CALL __CHK
            { POP AX }
----------------------------------------------------------------------
            MOV [esp],al
----------------------------------------------------------------------
            PLUS a liberal sprinkling of pushes and pops anywhere

This boils down to the following set of recognizable stuff

        Jackpot instructions

            MOV         BP,SP
            ENTER       X,0

        These fiddle the stack and must be counted!

            PUSH        REG
            POP         REG
            SUB         SP,n
            SUB         BP,n


        Indicates a far routine

            INC         BP
        Things we may skip over

            PUSH        const           // popped by __xxx
            PUSH        ESP             // popped by __TNC
            MOV         AX,c
            MOV         [esp],al
            MOV         DS,AX
            NOP
            CALL        __xxx

        Anything else stops us in our tracks


*/

#define IsSPReg( op ) ( (op).type == DO_REG && \
                        ( (op).base == DR_X86_sp || (op).base == DR_X86_esp ) )
#define IsCSReg( op ) ( (op).type == DO_REG && (op).base == DR_X86_cs )
#define IsBPReg( op ) ( (op).type == DO_REG && \
                        ( (op).base == DR_X86_bp || (op).base == DR_X86_ebp ) )
#define ConstOp( op ) ( (op).type == DO_IMMED )

typedef struct {
    const char  *name;
    int         pop;
} prolog_call;

static const prolog_call PrologCalls[] =
{
    { "__GETDS", 0 },
    { "__PRO", 0 },
    { "__CHK", 4 },
    { "__GRO", 4 },
    { "__TNK", 12 },
    { "__TNC", 12 },
    { "__STK", 0 },
    { NULL, 0 }
};

static int      Is32BitSegment;

static int IdentifyFunc( char *name, long *sp_adjust )
{
    const prolog_call   *p;

    for( p = PrologCalls; p->name != NULL; ++p ) {
        if( strncmp( name, p->name, strlen( p->name ) ) == 0 ) {
            *sp_adjust += p->pop;
            return( 1 );
        }
    }
    return( 0 );
}

static addr_off GetAnOffset( address *where )
{
    DbgAddr = *where;
    if( Is32BitSegment ) {
        return( GetDataLong() );
    } else {
        return( (unsigned short)GetDataWord() );
    }
}


static int GetBPFromStack( address *where, address *value )
{
    long        offset;

    *value = *where;
    offset = GetAnOffset( where );
    value->mach.offset = offset & ~1;
    return( ( offset & 1 ) != 0 );
}


static address GetFarAddr( address *return_location )
{
    address addr;

    addr.sect_id = 0;
    addr.indirect = 0;
    addr.mach.offset = GetAnOffset( return_location );
    addr.mach.segment = GetDataWord();
    if( Is32BitSegment ) {
        GetDataWord();
    }
    return( addr );
}

static void DisAsm( mad_disasm_data *dd )
{
    DoCode( dd, Is32BitSegment );
    DbgAddr.mach.offset += dd->ins.size;
}

static int FindCall( address *ip_value, address *return_addr_location )
{
    address         prev_ins;
    address         return_addr;
    mad_disasm_data dd;

    return_addr = *ip_value;
    return_addr.mach.offset = GetAnOffset( return_addr_location );
    prev_ins = return_addr;
    if( GetDisasmPrev( &prev_ins ) != MS_OK )
        return( 0 );
    DbgAddr = prev_ins;
    DisAsm( &dd );
    if( dd.ins.type == DI_X86_call ) {
        DbgAddr = prev_ins;
        if( GetDisasmPrev( &DbgAddr ) != MS_OK )
            return( 0 );
        DisAsm( &dd );
        if( dd.ins.type == DI_X86_push3 && IsCSReg( dd.ins.op[ OP_1 ] ) ) {
            *ip_value = GetFarAddr( return_addr_location );
        } else {
            ip_value->mach.offset = GetAnOffset( return_addr_location );
        }
        return( 1 );
    }
    return_addr = GetFarAddr( return_addr_location );
    MCAddrOvlReturn( &return_addr );
    DbgAddr = return_addr;
    if( GetDisasmPrev( &DbgAddr ) != MS_OK )
        return( 0 );
    DisAsm( &dd );
    if( dd.ins.type == DI_X86_call3 ) {
        *ip_value = GetFarAddr( return_addr_location );
        return( 1 );
    } else if( dd.ins.type == DI_X86_call ) {
        DbgAddr = prev_ins;
        if( GetDisasmPrev( &DbgAddr ) != MS_OK )
            return( 0 );
        DisAsm( &dd );
        if( dd.ins.type == DI_X86_push3 && IsCSReg( dd.ins.op[ OP_1 ] ) ) {
            *ip_value = GetFarAddr( return_addr_location );
            return( 0 );
        }
    }
    return( 1 );
}

static int HeuristicTraceBack(
    address *p_prev_sp,
    address *start,
    address *execution,
    address *frame,
    address *stack )
{
    mad_disasm_data dd;
    int             word_size;
    long            sp_adjust;
    long            bp_adjust;
    long            saved_bp_loc = 0;
    long            bp_to_ra_offset = 0;
//    int             found_inc_bp;
    int             found_mov_bp_sp;
    int             found_push_bp;
    char            *jmplabel;
    address         return_addr_location;
    address         bp_value;
    address         sp_value;
    address         saved_return_location;
    int             found_call;
    int             i;

    InitCache( *start, 100 );
    sp_value = *stack;
    bp_value = *frame;

    DbgAddr = *execution;
    DisAsm( &dd );
    if( dd.ins.type == DI_X86_retf || dd.ins.type == DI_X86_retf2 ) {
        *execution = GetFarAddr( &sp_value );
        found_call = 1;
    } else if( dd.ins.type == DI_X86_ret || dd.ins.type == DI_X86_ret2 ) {
        execution->mach.offset = GetAnOffset( &sp_value );
        found_call = 1;
    } else {
        // Check for ADD SP,n right after current ip and adjust SP if its there
        // because it must be popping parms
        if( dd.ins.type == DI_X86_add3 && ConstOp( dd.ins.op[OP_2] ) && IsSPReg( dd.ins.op[OP_1] ) ){
            sp_value.mach.offset += dd.ins.op[ OP_2 ].value;
        }
        // Run through code from the known symbol until and collect prolog info
        word_size = Is32BitSegment ? 4 : 2;
        sp_adjust = 0;
        bp_adjust = 0;
//        found_inc_bp = 0;
        found_mov_bp_sp = 0;
        found_push_bp = 0;
        DbgAddr = *start;
        while( DbgAddr.mach.offset != execution->mach.offset ) {
            DisAsm( &dd );
            switch( dd.ins.type ) {
            case DI_INVALID:
                return( 0 );
            case DI_X86_call3:
                jmplabel = ToSegStr( dd.ins.op[ OP_1 ].value, dd.ins.op[ OP_1 ].extra, 0 );
                if( IdentifyFunc( jmplabel, &sp_adjust ) )
                    continue;
                break;
            case DI_X86_call:
                jmplabel = JmpLabel( dd.ins.op[ OP_1 ].value, 0 );
                if( IdentifyFunc( jmplabel, &sp_adjust ) )
                    continue;
                break;
            case DI_X86_enter:
                sp_adjust -= word_size; // push bp
                found_push_bp = 1;
                bp_to_ra_offset = sp_adjust; // mov bp,sp
                found_mov_bp_sp = 1;
                saved_bp_loc = 0; // 0[bp]
                sp_adjust -= dd.ins.op[ OP_1 ].value; // sub sp,n
                break;
            case DI_X86_inc2:
                if( IsBPReg( dd.ins.op[ OP_1 ] ) ) {
//                    found_inc_bp = 1;
                    continue;
                }
                break;
            case DI_X86_mov:
                if( IsBPReg( dd.ins.op[ OP_1 ] ) && IsSPReg( dd.ins.op[ OP_2 ] ) ) {
                    found_mov_bp_sp = 1;
                    bp_to_ra_offset = sp_adjust;
                    saved_bp_loc -= sp_adjust;
                }
                continue;
            case DI_X86_nop:
                continue;
            case DI_X86_pop:
            case DI_X86_pop2:
            case DI_X86_pop3d:
            case DI_X86_pop3e:
            case DI_X86_pop3s:
            case DI_X86_pop4f:
            case DI_X86_pop4g:
                sp_adjust += word_size;
                continue;
            case DI_X86_push:
            case DI_X86_push2:
            case DI_X86_push3:
            case DI_X86_push4f:
            case DI_X86_push4g:
            case DI_X86_push5:
                sp_adjust -= word_size;
                if( IsBPReg( dd.ins.op[ OP_1 ] ) ) {
                    saved_bp_loc = sp_adjust;
                    found_push_bp = 1;
                }
                continue;
            case DI_X86_sub:
                dd.ins.op[ OP_2 ].value = -dd.ins.op[ OP_2 ].value;
            case DI_X86_add:
                if( !ConstOp( dd.ins.op[ OP_2 ] ) )
                    break;
                if( IsSPReg( dd.ins.op[ OP_1 ] ) ) {
                    sp_adjust += dd.ins.op[ OP_2 ].value;
                    continue;
                } else if( IsBPReg( dd.ins.op[ OP_1 ] ) ) {
                    bp_adjust += dd.ins.op[ OP_2 ].value;
                    continue;
                }
                break;
            default:
                break;
            }
            break;
        }

        // find the address of the return address (return_addr_location)
        if( found_mov_bp_sp ) {
            return_addr_location = bp_value;
            return_addr_location.mach.offset -= bp_adjust;
            GetBPFromStack( &return_addr_location, &bp_value );
            return_addr_location.mach.offset -= bp_to_ra_offset;
        } else {
            if( found_push_bp ) {
                return_addr_location = sp_value;
                return_addr_location.mach.offset += saved_bp_loc - sp_adjust;
                GetBPFromStack( &return_addr_location, &bp_value );
            }
            return_addr_location = sp_value;
            return_addr_location.mach.offset -= sp_adjust;
        }

        found_call = 0;
        if( found_mov_bp_sp ) {
            found_call = FindCall( execution, &return_addr_location );
            if( !found_call ) {
                return_addr_location = sp_value;
                return_addr_location.mach.offset -= sp_adjust;
            }
        }
        if( !found_call ) {
            saved_return_location = return_addr_location;
            // limit the search to 512*word_size (W2K can cause us to search 4Gb!)
            for( i = 0; return_addr_location.mach.offset >= p_prev_sp->mach.offset && i < 512; ++i ) {
                found_call = FindCall( execution, &return_addr_location );
                if( found_call )
                    break;
                return_addr_location.mach.offset -= word_size;
            }
            if( !found_call ) {
                return_addr_location = saved_return_location;
                for( i = 0; i < 10; ++i ) {
                    return_addr_location.mach.offset += word_size;
                    found_call = FindCall( execution, &return_addr_location );
                    if( found_call ) {
                        break;
                    }
                }
            }
        }
    }
    *stack = DbgAddr;
    *frame = bp_value;
    return( found_call );
}



static void SymbolicTraceBack(
    address  *start,
    unsigned characteristics,
    long     bp_disp,
    address  *execution,
    address  *frame,
    address  *stack )
{
    address     where;

    if( execution->mach.offset == start->mach.offset ) {
        /* return address is top item on the stack */
        where = *stack;
    } else {
        where = *frame;
        GetBPFromStack( &where, frame );
        where.mach.offset += bp_disp;
    }
    execution->mach.offset = GetAnOffset( &where );
    if( characteristics ) {
        execution->mach.segment = (unsigned short) GetDataWord();
    }
    *stack = DbgAddr;
}


static int BPTraceBack( address *execution, address *frame,
                        address *stack )
{
    address     where;
    int         is_far;

    where = *frame;
    is_far = GetBPFromStack( &where, frame );
    if( frame->mach.offset == 0 ) return( 0 );
    execution->mach.offset = (unsigned short) GetDataWord();
    if( is_far ) {
        execution->mach.segment = (unsigned short) GetDataWord();
    }
    *stack = DbgAddr;
    return( 1 );
}

unsigned        DIGENTRY MICallUpStackSize( void )
{
    return( sizeof( mad_call_up_data ) );
}

mad_status      DIGENTRY MICallUpStackInit( mad_call_up_data *cud, const mad_registers *mr )
{
    cud = cud;
    mr = mr;
    return( MS_OK );
}

mad_status      DIGENTRY MICallUpStackLevel( mad_call_up_data *cud,
                                const address *startp,
                                unsigned rtn_characteristics,
                                long return_disp,
                                const mad_registers *in,
                                address *execution,
                                address *frame,
                                address *stack,
                                mad_registers **out )
{
    address             prev_sp_value;
    address             start;


    cud = cud;
    in = in;
    *out = NULL;
    Is32BitSegment = BIG_SEG( *execution );
    start = *startp;
    if( MCSystemConfig()->os == MAD_OS_WINDOWS && !Is32BitSegment ) {
        memset( &start, 0, sizeof( start ) );
    }
    prev_sp_value = *stack;
    if( start.mach.segment == 0 && start.mach.offset == 0 ) {
        if( Is32BitSegment ) return( MS_FAIL );
        if( !BPTraceBack( execution, frame, stack ) ) return( MS_FAIL );
    } else {
        if( return_disp != -1L ) {
            SymbolicTraceBack( &start, rtn_characteristics, return_disp,
                        execution, frame, stack );
        } else {
            if( !HeuristicTraceBack( &prev_sp_value, &start,
                                execution, frame, stack ) ) return( MS_FAIL );
        }
    }
    if( stack->mach.offset <= prev_sp_value.mach.offset ) return( MS_FAIL );
    return( MS_OK );
}
