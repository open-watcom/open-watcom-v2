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
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "xmp.h"
#include "xmptypes.h"
#include "madregs.h"

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
    //NYI:
    return( 0 );
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
    //NYI:
    return( MS_FAIL );
}

/*
        Convert a disassembled instruction/operands into strings.
*/
unsigned                DIGENTRY MIDisasmFormat( mad_disasm_data *dd, mad_disasm_piece dp, unsigned radix, unsigned max, char *buff )
{
    //NYI:
    return( 0 );
}

/*
        Return the number of bytes in the instruction.
*/
unsigned                DIGENTRY MIDisasmInsSize( mad_disasm_data *dd )
{
    //NYI:
    return( 0 );
}

/*
        Return MS_OK if the instruction's effects can be completely backed
        out of by restoring the register and memory contents to their
        original values, MS_FAIL if not.
*/
mad_status              DIGENTRY MIDisasmInsUndoable( mad_disasm_data *dd )
{
    //NYI:
    return( MS_FAIL );
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

/*
        Return the address of the next instruction to be executed.
        This is normally just the next instruction sequentially, but for
        a taken control flow instruction, it's the target address of
        the instruction. Return MS_OK if the address of the next instruction
        could be determined (and fill in "*next"). If the next instruction's
        address couldn't be determined (e.g. "int 0x21" on an X86),
        return MS_FAIL.
*/
mad_status      DIGENTRY MIDisasmInsNext( mad_disasm_data *dd, const mad_registers *mr, address *next )
{
    //NYI:
    return( MS_FAIL );
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
        { MSTR_NIL, MSTR_NIL, MSTR_NIL }
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
mad_status              DIGENTRY MIDisasmInspectAddr( char *from, unsigned len, unsigned radix, const mad_registers *mr, address *a )
{
    //NYI:
    return( MS_FAIL );
}
