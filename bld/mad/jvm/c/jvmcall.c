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
#include "jvm.h"
#include "madregs.h"

/*
        Return MS_OK if a stack push increments the stack pointer, MS_FAIL
        if it decrements it.
*/
mad_string              DIGENTRY MICallStackGrowsUp( void )
{
    return( MS_FAIL );
}

/*
        Return an array of strings, terminated by a MSTR_NIL, that
        lists out the different types of calls the architecture supports
        (e.g. near/far/interrupt).
*/
const mad_string        *DIGENTRY MICallTypeList( void )
{
    static const mad_string list[] = { MSTR_NIL };

    return( list );
}

/*
        Build a call frame to call the routine at address 'rtn' and return
        to address 'ret'. Use the register set 'in' as the starting point
        and place the register set required for the call in 'out'. The kind
        of call to make will be in 'call' (it'll be one of the strings from
        the MICallTypeList). Return MS_OK if the call frame was build,
        otherwise return the error condition.
*/
mad_status      DIGENTRY MICallBuildFrame( mad_string call, address ret, address rtn, const mad_registers *in, mad_registers *out )
{
    return( MS_FAIL );
}

/*
    Return a pointer to mad_reg_info describing the return register for a
    routine.
*/
const mad_reg_info      *DIGENTRY MICallReturnReg( mad_string call, address rtn )
{
    return( NULL );
}

/*
        Return a pointer to an array of register descriptions describing
        the registers used to pass parameters to a routine. The list is
        terminated by a NULL entry.
*/
const mad_reg_info      **DIGENTRY MICallParmRegList( mad_string call, address rtn )
{
    static const mad_reg_info *list[] = {
        NULL };

    return( list );
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


/*
        Walk up one level in the stack frame.
*/
mad_status      DIGENTRY MICallUpStackLevel( mad_call_up_data *cud,
                                const address *start,
                                unsigned rtn_characteristics,
                                long return_disp,
                                const mad_registers *in,
                                address *execution,
                                address *frame,
                                address *stack,
                                mad_registers **out )
{
    address fake_addr;
    addr48_ptr new_execution;
    addr48_ptr old_execution;
    cud = cud;
    fake_addr.mach.segment = JVM_MAD_UPSTACK_SELECTOR;
    fake_addr.mach.offset = 0;
    old_execution.offset = execution->mach.offset;
    old_execution.segment = execution->mach.segment;
    if( MCWriteMem( fake_addr, sizeof( old_execution ), &old_execution ) != sizeof( old_execution ) ) return( MS_FAIL );
    if( MCReadMem( fake_addr, sizeof( new_execution ), &new_execution ) != sizeof( new_execution ) ) return( MS_FAIL );
    execution->mach.offset = new_execution.offset;
    execution->mach.segment = new_execution.segment;
    return( MS_OK );
}
