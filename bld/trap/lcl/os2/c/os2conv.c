/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#include <string.h>
#include <i86.h>
#include <env.h>
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#define INCL_WINSYS
#include <os2.h>
#include <os2dbg.h>
#include "os2v2acc.h"
#include "trperr.h"
#include "bsexcpt.h"
#include "dbgthrd.h"
#include "os2path.h"


#define LOCATOR     "OS2V2HLP.EXE"

extern long APIENTRY Dos16SelToFlat( long );
extern ULONG CallDosSelToFlat( PVOID );
#pragma aux CallDosSelToFlat = \
        ".386"          \
        "xchg eax,edx"  \
        "shl  eax,16"   \
        "xchg ax,dx"    \
        "call far ptr Dos16SelToFlat" \
        "mov  edx,eax"  \
        "shr  edx,16"   \
    __parm  [__dx __ax] \
    __value [__dx __ax]

extern USHORT   DoCall( PVOID, ULONG, ULONG );
#pragma aux DoCall \
    __parm      [__dx __ax] [__cx __bx] [__di __si] \
    __modify    [__ax __bx __cx __dx __si __di __es]

extern PVOID __far DoReturn( void );

extern HMODULE          ThisDLLModHandle;

USHORT          (APIENTRY *DebugFunc)( PVOID );
USHORT          FlatCS;
USHORT          FlatDS;

static ULONG    _retaddr;

/*
 * MakeLocalPtrFlat - create a 0:32 ptr from a 16:16 ptr
 */
ULONG MakeLocalPtrFlat( PVOID ptr )
{
    return( CallDosSelToFlat( ptr ) );

} /* MakeLocalPtrFlat */

unsigned Call32BitDosDebug( dos_debug __far *buff )
{
    return( DoCall( DebugFunc, MakeLocalPtrFlat( buff ), _retaddr ) );
}

/*
 * get the address of Dos32Debug, and get the flat selectors, too.
 */
int GetDos32Debug( PCHAR err )
{
    char        buff[256];
    RESULTCODES resc;
    USHORT      dummy;
    PSZ         start;
    PSZ         p;
    HFILE       inh;
    HFILE       outh;
    USHORT      rc;
    struct {
        ULONG       dos_debug;
        USHORT      cs;
        USHORT      ds;
        USHORT      ss;
    }           data;

    rc = DosGetModName( ThisDLLModHandle, sizeof( buff ), buff );
    if( rc ) {
        StrCopyDst( TRP_OS2_no_dll, err );
        return( FALSE );
    }
    start = buff;
    for( p = buff; *p != '\0'; ++p ) {
        switch( *p ) {
        case ':':
        case '\\':
        case '/':
           start = p + 1;
           break;
        }
    }
    p = StrCopyDst( LOCATOR, start );
    if( DosMakePipe( &inh, &outh, sizeof( data ) ) ) {
        StrCopyDst( TRP_OS2_no_pipe, err );
        return( FALSE );
    }
    *++p = outh + '0';
    *++p = '\0';
    *++p = '\0';
    rc = DosExecPgm( NULL, 0, EXEC_ASYNC, buff, NULL, &resc, buff );
    DosClose( outh );
    if( rc )  {
        DosClose( inh );
        StrCopyDst( TRP_OS2_no_help, err );
        return( FALSE );
    }
    rc = DosRead( inh, &data, sizeof( data ), &dummy );
    DosClose( inh );
    if( rc ) {
        StrCopyDst( TRP_OS2_no_help, err );
        return( FALSE );
    }
    DebugFunc = (PVOID)data.dos_debug;
    FlatCS = (USHORT)data.cs;
    FlatDS = (USHORT)data.ds;

    _retaddr = MakeLocalPtrFlat( (PVOID)DoReturn );
    return( TRUE );
}

/*
 * MakeSegmentedPointer - create a 16:16 ptr from a 0:32 ptr
 */
PVOID MakeSegmentedPointer( ULONG val )
{
    dos_debug   buff;

    buff.Pid = Buff.Pid;
    buff.Cmd = DBG_C_LinToSel;
    buff.Addr = val;
    CallDosDebug( &buff );
    return( _MK_FP( (USHORT)buff.Value, (USHORT)buff.Index ) );

} /* MakeSegmentedPointer */

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
 * IsUnknownGDTSeg - tell if someone is NOT a flat segment but IS a GDT seg
 */
int IsUnknownGDTSeg( USHORT seg )
{
    if( seg == FlatCS || seg == FlatDS ) {
        return( FALSE );
    }
#if 0
    if( !(seg & 0x04) ) {
        return( TRUE );
    }
#else
    if( seg == TaskFS ) {
        return( TRUE );
    }
#endif
    return( FALSE );

} /* IsUnknownGDTSeg */


/*
 * MakeItFlatNumberOne - make a (sel,offset) into a flat pointer
 */
ULONG MakeItFlatNumberOne( USHORT seg, ULONG offset )
{
    dos_debug   buff;

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
 * MakeItSegmentedNumberOne - make a (sel,offset) into a 16:16 pointer
 */
PVOID MakeItSegmentedNumberOne( USHORT seg, ULONG offset )
{
    if( !IsFlatSeg( seg ) )
        return( _MK_FP( seg, (USHORT) offset ) );
    return( MakeSegmentedPointer( offset ) );

} /* MakeItSegmentedNumberOne */


/*
 * GetExceptionText - return text for last exception
 */
PCHAR GetExceptionText( void )
{
    PCHAR   str;

    switch( ExceptNum ) {
    case XCPT_DATATYPE_MISALIGNMENT:
        str = TRP_EXC_data_type_misalignment;
        break;
    case XCPT_ACCESS_VIOLATION:
        str = TRP_EXC_general_protection_fault;
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
    default:
        str = TRP_EXC_unknown;
        break;
    }
    return( str );

} /* GetExceptionText */
