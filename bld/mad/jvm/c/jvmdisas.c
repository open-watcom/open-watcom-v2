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


#include <string.h>
#include <stdlib.h>
#include "walloca.h"
#include "jvm.h"
#include "jvmtypes.h"
#include "madregs.h"
#include "jvmops.h"

static dis_handle DH;

mad_status DisasmInit()
{
    if( DisInit( DISCPU_jvm, &DH ) != DR_OK ) {
        return( MS_ERR | MS_FAIL );
    }
    return( MS_OK );
}

void DisasmFini()
{
    DisFini( &DH );
}

unsigned DisCliGetAlign( void *d, unsigned off, unsigned align )
{
    mad_disasm_data     *dd = d;
    addr_off            start;
    unsigned            mod;

    start = dd->addr.mach.offset + off;
    mod = start % align;
    return( off + ((align - mod) % align) );
}

dis_return DisCliGetData( void *d, unsigned off, unsigned int size, void *data )
{
    mad_disasm_data     *dd = d;
    address             addr;

    addr = dd->addr;
    addr.mach.offset += off;
    if( MCReadMem( addr, size, data ) == 0 ) return( DR_FAIL );
    //NYI: this may break in future virgins of Java
    // (maybe turn on a QUICK bit in the disassembler flags?)
#define opc_invokevirtual_q    203
#define opc_invokeinterface_q  204
#define opc_invokestatic_q     213
#define opc_getfield_q         205
#define opc_putfield_q         206
#define opc_getfieldw_q        207
#define opc_putfieldw_q        208
#define opc_invokevirtual_s    209
#define opc_invokeinterface_s  210
#define opc_getfield_s         211
#define opc_putfield_s         212
    if( off == 0 ) {
        switch( *(char*)data ) {
        case opc_invokevirtual_q: *(char*)data = opc_invokevirtual; break;
        case opc_invokeinterface_q: *(char*)data = opc_invokeinterface; break;
        case opc_invokestatic_q: *(char*)data = opc_invokestatic; break;
        case opc_getfield_q: *(char*)data = opc_getfield; break;
        case opc_putfield_q: *(char*)data = opc_putfield; break;
        case opc_getfieldw_q: *(char*)data = opc_getfield; break;
        case opc_putfieldw_q: *(char*)data = opc_putfield; break;
        case opc_invokevirtual_s: *(char*)data = opc_invokevirtual; break;
        case opc_invokeinterface_s: *(char*)data = opc_invokeinterface; break;
        case opc_getfield_s: *(char*)data = opc_getfield; break;
        case opc_putfield_s: *(char*)data = opc_putfield; break;
        }
    }
    return( DR_OK );
}

size_t DisCliValueString( void *d, dis_dec_ins *ins, unsigned op, char *buff, size_t buff_size )
{
    mad_disasm_data     *dd = d;
    mad_type_info       mti;
    address             val;

    buff[0] = '\0';
    val = dd->addr;
    switch( ins->op[op].type & DO_MASK ) {
    case DO_RELATIVE:
        val.mach.offset += ins->op[op].value;
        MCAddrToString( val, JVMT_N32_PTR, MLK_CODE, buff, buff_size );
        break;
    case DO_IMMED:
    case DO_ABSOLUTE:
    case DO_MEMORY_ABS:
        MCTypeInfoForHost( MTK_INTEGER, SIGNTYPE_SIZE( sizeof( ins->op[0].value ) ), &mti );
        MCTypeToString( dd->radix, &mti, &ins->op[op].value, buff, &buff_size );
        break;
    }
    return( strlen( buff ) );
}

/*
        Return the size of the disassembler constrol structure.
*/
unsigned                DIGENTRY MIDisasmDataSize( void )
{
    return( sizeof( mad_disasm_data ) );
}

/*
        Return the length of the longest instruction name.
*/
unsigned                DIGENTRY MIDisasmNameMax( void )
{
    return( DisInsNameMax( &DH ) );
}

mad_status DisasmOne( mad_disasm_data *dd, address *a, int adj )
{
    addr_off    new;

    dd->addr = *a;
    //NYI: doesn't handle variable sized instructions
    new = dd->addr.mach.offset + adj * (int)sizeof( unsigned_32 );
    if( (adj < 0 && new > dd->addr.mach.offset)
     || (adj > 0 && new < dd->addr.mach.offset) ) {
        return( MS_FAIL );
    }
    dd->addr.mach.offset = new;
    DisDecodeInit( &DH, &dd->ins );
    if( DisDecode( &DH, dd, &dd->ins ) != DR_OK ) {
        return( MS_ERR | MS_FAIL );
    }
    a->mach.offset = dd->addr.mach.offset + dd->ins.size;
    return( MS_OK );
}

/*
        Disassemble one instruction. On input, 'a' indicates the address
        to be disassembled, as modified by 'adj'. The 'adj' parameter
        indicates the number of instructions forward (for positive values) or
        backwards (for negative values) to be disassembled. For example,

                0       - disassemble the instruction at 'a'
                1       - disassemble the instruction following the one
                          at 'a'
                -1      - disassemble the instruction preceeding the one
                          at 'a'

        On exit, 'a' is adjusted to indicate the starting address of the
        instruction following the one that has been disassembled.
*/
mad_status              DIGENTRY MIDisasm( mad_disasm_data *dd, address *a, int adj )
{
    return( DisasmOne( dd, a, adj ) );
}

/*
        Convert a disassembled instruction/operands into strings.
*/
unsigned                DIGENTRY MIDisasmFormat( mad_disasm_data *dd, mad_disasm_piece dp, mad_radix radix, char *buff, unsigned buff_size )
{
    char                nbuff[20];
    char                obuff[256];
    char                *np;
    char                *op;
    unsigned            nlen;
    unsigned            olen;
    unsigned            len;
    dis_format_flags    ff;

    nbuff[0] = '\0';
    obuff[0] = '\0';
    np = (dp & MDP_INSTRUCTION) ? nbuff : NULL;
    op = (dp & MDP_OPERANDS)    ? obuff : NULL;
    ff = DFF_NONE;
    if( MADState->disasm_state & DT_PSUEDO_OPS ) ff |= DFF_PSEUDO;
    if( MADState->disasm_state & DT_UPPER ) ff |= DFF_INS_UP | DFF_REG_UP;
    dd->radix = radix;
    if( DisFormat( &DH, dd, &dd->ins, ff, np, op ) != DR_OK ) {
        return( 0 );
    }
    olen = strlen( obuff );
    nlen = strlen( nbuff );
    if( dp == MDP_ALL ) nbuff[ nlen++ ] = ' ';
    len = nlen + olen;
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        if( nlen > buff_size )
            nlen = buff_size;
        memcpy( buff, nbuff, nlen );
        buff += nlen;
        buff_size -= nlen;
        if( olen > buff_size )
            olen = buff_size;
        memcpy( buff, obuff, olen );
        buff[buff_size] = '\0';
    }
    return( len );
}

/*
        Return the number of bytes in the instruction.
*/
unsigned                DIGENTRY MIDisasmInsSize( mad_disasm_data *dd )
{
    return( dd->ins.size );
}

/*
        Return MS_OK if the instruction's effects can be completely backed
        out of by restoring the register and memory contents to their
        original values, MS_FAIL if not.
*/
mad_status              DIGENTRY MIDisasmInsUndoable( mad_disasm_data *dd )
{
    return( MS_FAIL|MS_ERR );
}

/*
        Return the general classification of the instruction:

            MDC_OPER    - an operation: add, sub, etc.
            MDC_JUMP    - a jump instruction of some kind
            MDC_CALL    - a call instruction of some kind
            MDC_SYSCALL - a system (interrupt) call of some kind
            MDC_RET     - a return instruction
            MDC_SYSRET  - a system call return (iret).

            or'd with:

            MDC_TAKEN_NOT       - for a jump/call/ret, execution continues
                                  with the next sequential instruction rather
                                  than taking the control transfer, for an
                                  operation, any program state modification
                                  is suppressed
            MDC_TAKEN_BACK      - execution continues at an address before
                                  the current location
            MDC_TAKEN_FORWARD   - execution continues at an address after
                                  the current location
            MDC_TAKEN           - execution continues at some other address
                                  for jmp/call/ret, for an operation, any
                                  program state modification is performed


*/
mad_disasm_control      DIGENTRY MIDisasmControl( mad_disasm_data *dd, const mad_registers *mr )
{
    //NYI:
    return( MDC_OPER | MDC_TAKEN );
}

mad_status      DIGENTRY MIDisasmInsNext( mad_disasm_data *dd, const mad_registers *mr, address *next )
{
    //NYI:
    return( MS_ERR|MS_UNSUPPORTED );
}

/*
        Call back to a client routine describing all the memory references
        made by this instruction.
*/
walk_result             DIGENTRY MIDisasmMemRefWalk( mad_disasm_data *dd, MI_MEMREF_WALKER *wk, const mad_registers *mr, void *d )
{
    //NYI:
    return( WR_CONTINUE );
}

/*
        Return the list of toggle strings for the disassembler.
*/
const mad_toggle_strings        *DIGENTRY MIDisasmToggleList( void )
{
    //NYI:
    static const mad_toggle_strings list[] = {
        { MAD_MSTR_NIL, MAD_MSTR_NIL, MAD_MSTR_NIL }
    };
    return( list );
}

/*
        Get/Set the disassembler toggle bits. For a particular bit, the
        following algorithm is used:

                on      off     result
                --      ---     ------
                0       0       bit left unchanged
                1       0       bit set to 1
                0       1       bit set to 0
                1       1       bit toggled from previous value

        (it operates the same as a JK flip-flop).
        Return the new toggle state.
*/
unsigned                DIGENTRY MIDisasmToggle( unsigned on, unsigned off )
{
    unsigned    toggle;

    toggle = (on & off);
    MADState->disasm_state ^= toggle;
    MADState->disasm_state |= on & ~toggle;
    MADState->disasm_state &= ~off | toggle;
    return( MADState->disasm_state );
}

/*
        Given a string, convert that to an address to be examined.
*/
mad_status              DIGENTRY MIDisasmInspectAddr(const char *start, unsigned len, mad_radix radix, const mad_registers *mr, address *a)
{
    //NYI:
    return( MS_FAIL );
}
