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


#include <stddef.h>
#include <string.h>
#include "xmp.h"
#include "madregs.h"

/*
        Return the size of the trace control structure.
*/
unsigned        DIGENTRY MITraceSize( void )
{
    return( sizeof( mad_trace_data ) );
}

/*
        Initialize for tracing.
*/
void            DIGENTRY MITraceInit( mad_trace_data *td, const mad_registers *mr )
{
    //NYI:
}

/*
        Return MS_OK if we have recursively entered a routine, based on
        the value of 'watch_stack'. Return MS_FAIL if not.
*/
mad_status      DIGENTRY MITraceHaveRecursed( address watch_stack, const mad_registers *mr )
{
    //NYI:
    return( MS_FAIL );
}

/*
        Return how to trace the current instruction.

            MTRH_STOP           - Stop single stepping
            MTRH_SIMULATE       - Use MITraceSimulate
            MTRH_STEP           - Tell the trap file to single step
            MTRH_STEPBREAK      - We want to single step, but for magical
                                  reasons we have to break at '*brk'
            MTRH_BREAK          - Set a break point at '*brk'

        The 'tk' parameter indicates the type of tracing required:

            MTRK_INTO           - Trace into function calls
            MTRK_OVER           - Trace over function calls
            MTRK_OUT            - We just traced into a function call, but
                                  have decided we don't want to be here:
                                  stop when the function returns
            MTRK_NEXT           - Stop at the next sequential instruction

*/
mad_trace_how   DIGENTRY MITraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, const mad_registers *mr, address *brk )
{
    //NYI:
    return( MTRH_STOP );
}

/*
        Starting with the register values in 'in', set the register set
        pointed to by 'out' as if the instruction indicated by 'td' and 'dd'
        had been executed. Return MS_OK if this could be done, or some
        error status if something goes wrong. Return MS_UNSUPPORTED if this
        code can't handle simulating this instruction.
*/
mad_status      DIGENTRY MITraceSimulate( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *in, mad_registers *out )
{
    //NYI:
    return( MS_UNSUPPORTED );
}

/*
        Finalize tracing.
*/
void            DIGENTRY MITraceFini( mad_trace_data *td )
{
    //NYI:
}

/*
        The client has run into a breakpoint that it didn't plant. Check
        to see if it's got the particular code pattern that the C library
        uses to indicate a breakpoint with a message (see enterdb.h in
        the WATCOM project). If so, fill in 'buff' with the text of the
        message and return MS_OK. If not, return MS_FAIL. On entry '*maxp'
        should contain the length of the buffer. On exit it will be set
        to the true length of the message, even if the message did not all
        fit into the buffer provided.
*/
mad_status              DIGENTRY MIUnexpectedBreak( mad_registers *mr, unsigned *maxp, char *buff )
{
    //NYI:
    return( MS_FAIL );
}
