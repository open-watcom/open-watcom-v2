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
#include "trperr.h"

unsigned ExceptionText( unsigned except, char *err_txt )
{
    static const char * const ExceptionMsgs[] = {
            TRP_EXC_divide_overflow
            "",
            TRP_EXC_non_maskable_interrupt
            "",
            TRP_EXC_integer_overflow
            TRP_EXC_bounds_check
            TRP_EXC_invalid_opcode
            TRP_EXC_coprocessor_not_available
            TRP_EXC_double_fault
            TRP_EXC_coprocessor_segment_overrun
            TRP_EXC_invalid_TSS
            TRP_EXC_segment_not_present
            TRP_EXC_stack_exception
            TRP_EXC_general_protection_fault
            TRP_EXC_page_fault
            "",
            TRP_EXC_coprocessor_error
    };
    if( except > ( (sizeof(ExceptionMsgs) / sizeof(char *) - 1) ) ) {
        strcpy( err_txt, TRP_EXC_unknown );
    } else {
        strcpy( err_txt, ExceptionMsgs[ except ] );
    }
    return( strlen( err_txt ) + 1 );
}
