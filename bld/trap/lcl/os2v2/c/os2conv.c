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
* Description:  OS/2 address conversion and other support routines.
*
****************************************************************************/


#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#define INCL_WINSYS
#include <os2.h>
#include <os2dbg.h>
#include "trpimp.h"
#include "trperr.h"
#include "dosdebug.h"
#include "os2trap.h"
#include "bsexcpt.h"

extern uDB_t            Buff;

extern USHORT           TaskFS;

/* Hardcoded selector values - extremely unlikely to change. */
USHORT FlatCS = 0x5B, FlatDS = 0x53;

extern ULONG            ExceptNum;


/*
 * IsFlatSeg - check for flat segment
 */
int IsFlatSeg( USHORT seg )
{
    if( seg == FlatCS || seg == FlatDS )
        return( TRUE );
    return( FALSE );
} /* IsFlatSeg */


/*
 * IsUnknownGDTSeg - tell if someone is NOT a flat segment but IS a GDT seg.
 * This is useful for FS segment access.
 */
int IsUnknownGDTSeg( USHORT seg )
{
    if( seg == FlatCS || seg == FlatDS ) {
        return( FALSE );
    }
    if( seg == TaskFS ) {
        return( TRUE );
    }
    return( FALSE );
} /* IsUnknownGDTSeg */


/*
 * MakeItFlatNumberOne - make a (sel,offset) into a flat pointer
 */
ULONG MakeItFlatNumberOne( USHORT seg, ULONG offset )
{
    uDB_t       buff;

    if( IsFlatSeg( seg ) )
        return( offset );
    buff.Pid = Buff.Pid;
    buff.Cmd = DBG_C_SelToLin;
    buff.Value = seg;
    buff.Index = offset;
    CallDosDebug( &buff );
    return( buff.Addr );
} /* MakeItFlatNumberOne */


/*
 * GetExceptionText - return text for last exception
 */
char *GetExceptionText( void )
{
    char       *str;

    switch( ExceptNum ) {
        case XCPT_DATATYPE_MISALIGNMENT:
            str = TRP_EXC_data_type_misalignment;
            break;
        case XCPT_ACCESS_VIOLATION:
            str = TRP_EXC_access_violation;
            break;
        case XCPT_ILLEGAL_INSTRUCTION:
            str = TRP_EXC_illegal_instruction;
            break;
        case XCPT_INTEGER_DIVIDE_BY_ZERO:
            str = TRP_EXC_integer_divide_by_zero;
            break;
        case XCPT_INTEGER_OVERFLOW:
            str = TRP_EXC_integer_overflow;
           break;
        case XCPT_PRIVILEGED_INSTRUCTION:
            str = TRP_EXC_privileged_instruction;
            break;
        case XCPT_FLOAT_DENORMAL_OPERAND:
            str = TRP_EXC_floating_point_denormal_operand;
            break;
        case XCPT_FLOAT_DIVIDE_BY_ZERO:
            str = TRP_EXC_floating_point_divide_by_zero;
            break;
        case XCPT_FLOAT_INEXACT_RESULT:
            str = TRP_EXC_floating_point_inexact_result;
            break;
        case XCPT_FLOAT_INVALID_OPERATION:
            str = TRP_EXC_floating_point_invalid_operation;
            break;
        case XCPT_FLOAT_OVERFLOW:
            str = TRP_EXC_floating_point_overflow;
            break;
        case XCPT_FLOAT_STACK_CHECK:
            str = TRP_EXC_floating_point_stack_check;
            break;
        case XCPT_FLOAT_UNDERFLOW:
            str = TRP_EXC_floating_point_underflow;
            break;
        case XCPT_PROCESS_TERMINATE:
            str = TRP_EXC_process_terminate;
            break;
        case XCPT_ASYNC_PROCESS_TERMINATE:
            str = TRP_EXC_async_process_terminate;
            break;
        case XCPT_SIGNAL:
            str = TRP_EXC_signal;
            break;
        default:
            str = TRP_EXC_unknown;
            break;
    }
    return( str );
} /* GetExceptionText */
