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
#include "trpimp.h"
#include "trperr.h"
#include "dosdebug.h"
#include "os2trap.h"
#include "bsexcpt.h"

extern USHORT           TaskFS;

ULONG MakeItFlatNumberOne( USHORT seg, ULONG offset );
extern dos_debug        Buff;


extern unsigned short   ThisDLLModHandle;
extern ULONG            ExceptNum;

extern unsigned short pascal far Dos16SelToFlat();

extern long CallDosSelToFlat( long );
#pragma aux CallDosSelToFlat = \
 0x66 0x92                              /* xchg    eax,edx */ \
 0x66 0xc1 0xe0 0x10                    /* shl     eax,10H */ \
 0x92                                   /* xchg    ax,dx */ \
 0x9a offset Dos16SelToFlat seg Dos16SelToFlat /* call ... */ \
 0x66 0x8b 0xd0                         /* mov     edx,eax */ \
 0x66 0xc1 0xea 0x10                    /* shr     edx,10H */ \
 parm[dx ax] value [dx ax];

ULONG MakeLocalPtrFlat( void far *ptr );

USHORT  (APIENTRY *DebugFunc)( PVOID );
USHORT  FlatCS,FlatDS;
static ULONG    _retaddr;
extern void far *DoReturn();

extern USHORT   DoCall( void far*, ULONG, ULONG );
#pragma aux DoCall parm [dx ax] [cx bx] [di si] modify [ax bx cx dx si di es];

extern void bp( void );
#pragma aux bp = 0xcc;

#define LOCATOR     "OS2V2HLP.EXE"

unsigned int Call32BitDosDebug( dos_debug far *buff )
{
    return( DoCall( DebugFunc, MakeLocalPtrFlat( buff ), _retaddr ) );
}

/*
 * get the address of Dos32Debug, and get the flat selectors, too.
 */
int GetDos32Debug( char far *err )
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
        StrCopy( TRP_OS2_no_dll, err );
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
    p = StrCopy( LOCATOR, start );
    if( DosMakePipe( &inh, &outh, sizeof( data ) ) ) {
        StrCopy( TRP_OS2_no_pipe, err );
        return( FALSE );
    }
    *++p = outh + '0';
    *++p = '\0';
    *++p = '\0';
    rc = DosExecPgm( NULL, 0, EXEC_ASYNC, buff, NULL, &resc, buff );
    DosClose( outh );
    if( rc )  {
        DosClose( inh );
        StrCopy( TRP_OS2_no_help, err );
        return( FALSE );
    }
    rc = DosRead( inh, &data, sizeof( data ), &dummy );
    DosClose( inh );
    if( rc ) {
        StrCopy( TRP_OS2_no_help, err );
        return( FALSE );
    }
    DebugFunc = (void far *) data.dos_debug;
    FlatCS = (USHORT) data.cs;
    FlatDS = (USHORT) data.ds;

    _retaddr = MakeLocalPtrFlat( (void far *) DoReturn );
    return( TRUE );
}

/*
 * MakeSegmentedPointer - create a 16:16 ptr from a 0:32 ptr
 */
void far *MakeSegmentedPointer( ULONG val )
{
    dos_debug   buff;

    buff.Pid = Buff.Pid;
    buff.Cmd = DBG_C_LinToSel;
    buff.Addr = val;
    CallDosDebug( &buff );
    return( MK_FP( (USHORT) buff.Value, (USHORT) buff.Index ) );

} /* MakeSegmentedPointer */

/*
 * MakeLocalPtrFlat - create a 0:32 ptr from a 16:16 ptr
 */
ULONG MakeLocalPtrFlat( void far *ptr )
{
    return( CallDosSelToFlat( (long) ptr ) );

} /* MakeLocalPtrFlat */

/*
 * IsFlatSeg - check for flat segment
 */
int IsFlatSeg( USHORT seg )
{
    if( seg == FlatCS || seg == FlatDS ) return( TRUE );
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

    if( IsFlatSeg( seg ) ) return( offset );
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
void far * MakeItSegmentedNumberOne( USHORT seg, ULONG offset )
{
    if( !IsFlatSeg( seg ) ) return( MK_FP( seg, (USHORT) offset ) );
    return( MakeSegmentedPointer( offset ) );

} /* MakeItSegmentedNumberOne */


/*
 * GetExceptionText - return text for last exception
 */
char far *GetExceptionText( void )
{
    char        far *str;

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
