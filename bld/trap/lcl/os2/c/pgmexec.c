/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include <i86.h>
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#include <os2.h>
#include <os2dbg.h>
#include "os2v2acc.h"
#include "bsexcpt.h"
#include "brkptcpu.h"


#define LOAD_THIS_DLL_SIZE      6

extern PVOID my_alloca( unsigned short );
#pragma aux my_alloca = \
        "sub  sp,ax"    \
        "mov  ax,sp"    \
        "mov  dx,ss"    \
    __parm __caller [__ax] \
    __value         [__ax __dx] \
    __modify        [__sp]

typedef struct {
    USHORT      phmod[2];               /* offset-segment */
    USHORT      mod_name[2];            /* offset-segment */
    USHORT      fail_len;
    PSZ         fail_name;              /* offset-segment */
    USHORT      hmod;
    CHAR        load_name[2];
} loadstack_t;

typedef void (*excfn)();

extern void         DoReadWord( void );
extern void         DoWriteWord( void );

static char         stack[1024];

bool CausePgmToLoadThisDLL( ULONG startLinear )
{

    char        savecode[LOAD_THIS_DLL_SIZE];
    USHORT      codesize;
    USHORT      len;
    loadstack_t __far *loadstack;
    PVOID       ptr;
    USHORT      dll_name_len;
    USHORT      size;
    char        this_dll[BUFF_SIZE];
    bool        rc;

    /*
     * save a chunk of the program's code, and put in LoadThisDLL instead
     */
    if( DosGetModName( ThisDLLModHandle, BUFF_SIZE, this_dll ) != 0 ) {
        return( FALSE );
    }
    codesize = (char *)EndLoadThisDLL - (char *)LoadThisDLL;
    if( codesize > LOAD_THIS_DLL_SIZE )
        return( FALSE );
    ReadLinear( savecode, startLinear, codesize );
    if( Buff.Cmd != DBG_N_Success )
        return( FALSE );
    WriteLinear( (PBYTE)LoadThisDLL, startLinear, codesize );

    /*
     * set up the stack for the routine LoadThisDLL
     */
    dll_name_len = ( strlen( this_dll ) + 1 ) & ~1;
    size = sizeof( loadstack_t ) + dll_name_len;
    loadstack = my_alloca( size );
    Buff.ESP -= size;
    strcpy( loadstack->load_name, this_dll );
    loadstack->fail_name = NULL;
    loadstack->fail_len = 0;
    ptr = MakeItSegmentedNumberOne( Buff.SS, Buff.ESP + offsetof( loadstack_t, load_name ) );
    loadstack->mod_name[0] = _FP_OFF( ptr );
    loadstack->mod_name[1] = _FP_SEG( ptr );
    ptr = MakeItSegmentedNumberOne( Buff.SS, Buff.ESP + offsetof( loadstack_t, hmod ) );
    loadstack->phmod[0] = _FP_OFF( ptr );
    loadstack->phmod[1] = _FP_SEG( ptr );
    len = WriteBuffer( (PBYTE)loadstack, Buff.SS, Buff.ESP, size );
    if( len != size )
        return( FALSE );

    /*
     * set up 16:16 CS:IP, SS:SP for execution
     */
    ptr = MakeSegmentedPointer( startLinear );
    Buff.CS = _FP_SEG( ptr );
    Buff.EIP = _FP_OFF( ptr );
    ptr = MakeItSegmentedNumberOne( Buff.SS, Buff.ESP );
    Buff.SS = _FP_SEG( ptr );
    Buff.ESP = _FP_OFF( ptr );

    /*
     * execute LoadThisDLL on behalf of the program
     */
    WriteRegs( &Buff );
    DebugExecute( &Buff, DBG_C_Go, FALSE );
    if( Buff.Cmd != DBG_N_Breakpoint ) {
        rc = FALSE;
    } else {
        rc = TRUE;
    }
    WriteLinear( savecode, startLinear, codesize );
    return( rc );
}

void DoOpen( PCHAR name, int mode, int flags )
{
    BreakPointParm( OpenFile( name, mode, flags ) );
}


void DoClose( HFILE hdl )
{
    BreakPointParm( DosClose( hdl ) );
}

void DoDupFile( HFILE old, HFILE new )
{
    HFILE       new_t;
    USHORT      rc;

    new_t = new;
    rc = DosDupHandle( old, &new_t );
    if( rc != 0 ) {
        BreakPointParm( NIL_DOS_HANDLE );
    } else {
        BreakPointParm( new_t );
    }
}

static long TaskExecute( excfn rtn )
{
    long        retval;

    if( CanExecTask ) {
        Buff.CS = _FP_SEG( rtn );
        Buff.EIP = _FP_OFF( rtn );
        Buff.SS = _FP_SEG( stack );
        Buff.ESP = _FP_OFF( stack ) + sizeof( stack );
        WriteRegs( &Buff );
        /*
         * writing registers with invalid selectors will fail
         */
        if( Buff.Cmd != DBG_N_Success ) {
            return( -1 );
        }
        DebugExecute( &Buff, DBG_C_Go, FALSE );
        retval = ( Buff.EDX << 16 ) + (USHORT)Buff.EAX;
        return( retval );
    } else {
        return( -1 );
    }
}


static void saveRegs( dos_debug __far *save )
{
    save->Pid = Pid;
    save->Tid = 0;
    ReadRegs( save );
}


long TaskOpenFile( PCHAR name, int mode, int flags )
{
    dos_debug   save;
    long        rc;

    saveRegs( &save );
    WriteBuffer( (PBYTE)name, _FP_SEG( UtilBuff ), _FP_OFF( UtilBuff ), strlen( name ) + 1 );
    Buff.EDX = _FP_SEG( UtilBuff );
    Buff.EAX = _FP_OFF( UtilBuff );
    Buff.EBX = mode;
    Buff.ECX = flags;
    rc = TaskExecute( (excfn)DoOpen );
    WriteRegs( &save );
    return( rc );
}


long TaskCloseFile( HFILE hdl )
{
    dos_debug   save;
    long        rc;

    saveRegs( &save );
    Buff.EAX = hdl;
    rc = TaskExecute( (excfn)DoClose );
    WriteRegs( &save );
    return( rc );
}

HFILE TaskDupFile( HFILE old, HFILE new )
{
    dos_debug   save;
    long        rc;

    saveRegs( &save );
    Buff.EAX = old;
    Buff.EDX = new;
    rc = TaskExecute( (excfn)DoDupFile );
    WriteRegs( &save );
    return( rc );
}

bool TaskReadWord( USHORT seg, ULONG off, USHORT __far *data )
{
    dos_debug   save;
    bool        rc;

    saveRegs( &save );
    Buff.EBX = off;
    Buff.GS = seg;
    ExpectingAFault = TRUE;
    TaskExecute( (excfn)DoReadWord );
    ExpectingAFault = FALSE;
    if( Buff.Cmd != DBG_N_Breakpoint ) {
        rc = FALSE;
    } else {
        rc = TRUE;
        *data = (USHORT)Buff.EAX;
    }
    WriteRegs( &save );
    return( rc );

}

bool TaskWriteWord( USHORT seg, ULONG off, USHORT data )
{
    dos_debug   save;
    bool        rc;

    saveRegs( &save );
    Buff.EAX = data;
    Buff.EBX = off;
    Buff.GS = seg;
    ExpectingAFault = TRUE;
    TaskExecute( (excfn)DoWriteWord );
    ExpectingAFault = FALSE;
    if( Buff.Cmd != DBG_N_Breakpoint ) {
        rc = FALSE;
    } else {
        rc = TRUE;
    }
    WriteRegs( &save );
    return( rc );

}

void TaskPrint( PBYTE ptr, unsigned len )
{
    dos_debug   save;

    saveRegs( &save );
    while( len > sizeof( UtilBuff ) ) {
        WriteBuffer( ptr, _FP_SEG( UtilBuff ), _FP_OFF( UtilBuff ), sizeof( UtilBuff ) );
        Buff.EAX = _FP_OFF( UtilBuff );
        Buff.EDX = _FP_SEG( UtilBuff );
        Buff.EBX = sizeof( UtilBuff );
        TaskExecute( (excfn)DoWritePgmScrn );
        ptr += sizeof( UtilBuff );
        len -= sizeof( UtilBuff );
    }
    WriteBuffer( ptr, _FP_SEG( UtilBuff ), _FP_OFF( UtilBuff ), len );
    Buff.EAX = _FP_OFF( UtilBuff );
    Buff.EDX = _FP_SEG( UtilBuff );
    Buff.EBX = len;
    TaskExecute( (excfn)DoWritePgmScrn );
    WriteRegs( &save );
}
