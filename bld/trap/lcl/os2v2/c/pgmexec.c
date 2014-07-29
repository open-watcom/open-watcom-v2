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
* Description:  Special access routines spliced into the debuggee.
*
****************************************************************************/

/****************************************************************************

 A bit of explanation is needed here. This is a gross hack. But a necessary
 hack - allowing us to circumvent certain limitations of the OS/2 debugging
 interface as well as add nifty features to the debugger.

 Example: It is not possible to read the TIB (pointed to by debuggee's FS:0)
 through DosDebug. Yet the ability to read this data is extremely helpful
 when debugging exception handling, figuring out stack limits and the like.
 So what do we do? Splice a chunk of code into the debuggee's address space.
 When we need to perform special order access, we save debuggee's registers,
 point ESP to special stack, point EIP to our routine and resume execution.
 The routine will hit a breakpoint when it's done, at which point the
 debugger will restore all registers. Not pretty but works.

 This mechanism also gives us ability to do things like redirect debuggee's
 file handles, print on its screen and the like.

 Implementation: To perform these feats of magic, we force load a helper DLL
 in debuggee's context. We have a simple routine that does that defined here.
 We copy it into the debuggee and run it there. We also load the DLL in this
 trap file. Due to the way DLLs are implemented on OS/2, we know that the
 addresses of routines in the DLL mapped in the debuggee will be identical
 to addresses that we see in the debugger context (because the trap file
 and the debuggee always run on the same machine).

****************************************************************************/


#include <string.h>
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#include <os2.h>
#include <string.h>
#include "cpuglob.h"
#include "dosdebug.h"
#include "trpimp.h"
#include "os2trap.h"
#include "os2v2acc.h"
#include "madregs.h"
#include "splice.h"

#define LOAD_HELPER_DLL_SIZE      8

extern uDB_t            Buff;

bool CausePgmToLoadHelperDLL( ULONG startLinear )
{
    HMODULE     hmodHelper;
    char        savecode[LOAD_HELPER_DLL_SIZE];
    USHORT      codesize;
    USHORT      len;
    loadstack_t *loadstack;
    USHORT      dll_name_len;
    USHORT      size;
    char        szHelperDLL[BUFF_SIZE];
    bool        rc;

    /*
     * save a chunk of the program's code, and put in LoadThisDLL instead
     */
    if( DosQueryModuleHandle( "wdsplice", &hmodHelper ) != 0 )
        return( FALSE );

    if( DosQueryModuleName( hmodHelper, BUFF_SIZE, szHelperDLL ) != 0 )
        return( FALSE );

    codesize = (char *)EndLoadHelperDLL - (char *)LoadHelperDLL;
    if( codesize > LOAD_HELPER_DLL_SIZE )
        return( FALSE );
    ReadLinear( savecode, startLinear, codesize );
    if( Buff.Cmd != DBG_N_Success )
        return( FALSE );
    WriteLinear( (char*)LoadHelperDLL, startLinear, codesize );

    /*
     * set up the stack for the routine LoadHelperDLL; first set up
     * the stack contents in temporary buffer, then copy it onto
     * debuggee's real stack
     */
    dll_name_len = (strlen(szHelperDLL) + 3) & ~3; // DWORD align
    size = sizeof(loadstack_t) + dll_name_len;
    loadstack = (loadstack_t*)TempStack;
    Buff.ESP -= size;
    strcpy( (char *)loadstack->load_name, szHelperDLL );
    // Offsets must be relative to where loadstack will end up!
    loadstack->mod_name  = (PSZ)(MakeItFlatNumberOne( Buff.SS, Buff.ESP ) + 20);
    loadstack->phmod     = (HMODULE*)(MakeItFlatNumberOne( Buff.SS, Buff.ESP ) + 16);
    loadstack->fail_name = loadstack->mod_name;  // Reuse buffer
    loadstack->fail_len  = dll_name_len;
    len = WriteBuffer( (char *)loadstack, Buff.SS, Buff.ESP, size );
    if( len != size )
        return( FALSE );

    /*
     * set up flat CS:EIP, SS:ESP for execution; note that this works for
     * 16-bit apps as well
     */
    Buff.EIP = startLinear;
    Buff.CS  = FlatCS;
    Buff.ESP = MakeItFlatNumberOne( Buff.SS, Buff.ESP );
    Buff.SS  = FlatDS;
    Buff.DS  = FlatDS;
    Buff.ES  = FlatDS;

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
    /*
     * put back original memory contents
     */
    WriteLinear( savecode, startLinear, codesize );
    return( rc );
}


long TaskExecute( excfn rtn )
{
    long        retval;

    /* Note that we need to save and restore the ExceptNum and
     * ExpectingAFault globals. Yucky globals!
     */
    if( CanExecTask ) {
        ULONG   oldExcNum = ExceptNum;

        ExpectingAFault = TRUE;
        Buff.CS  = FlatCS;
        Buff.EIP = (ULONG)rtn;
        Buff.SS  = FlatDS;
        Buff.DS  = FlatDS;
        Buff.ES  = FlatDS;
        Buff.ESP = (ULONG)(TempStack + TEMPSTACK_SIZE);
        WriteRegs( &Buff );
        /*
         * writing registers with invalid selectors will fail
         */
        if( Buff.Cmd != DBG_N_Success ) {
            retval = -1;
        } else {
            DebugExecute( &Buff, DBG_C_Go, FALSE );
            retval = Buff.EAX;
        }
        ExpectingAFault = FALSE;
        ExceptNum = oldExcNum;
        return( retval );
    } else {
        return( -1 );
    }
}


static void saveRegs( uDB_t *save )
{
    save->Pid = Pid;
    save->Tid = 0;
    ReadRegs( save );
}


long TaskOpenFile( char *name, int mode, int flags )
{
    uDB_t       save;
    long        rc;

    saveRegs( &save );
    WriteLinear( name, (ULONG)&XferBuff, strlen(name) + 1 );
    Buff.EAX = (ULONG)&XferBuff;
    Buff.EDX = mode;
    Buff.ECX = flags;
    rc = TaskExecute( (excfn)DoOpen );
    WriteRegs( &save );
    return( rc );
}


long TaskCloseFile( HFILE hdl )
{
    uDB_t       save;
    long        rc;

    saveRegs( &save );
    Buff.EAX = hdl;
    rc = TaskExecute( (excfn)DoClose );
    WriteRegs( &save );
    return( rc );
}


HFILE TaskDupFile( HFILE old, HFILE new )
{
    uDB_t       save;
    long        rc;

    saveRegs( &save );
    Buff.EAX = old;
    Buff.EDX = new;
    rc = TaskExecute( (excfn)DoDupFile );
    WriteRegs( &save );
    return( rc );
}


bool TaskReadWord( USHORT seg, ULONG off, USHORT *data )
{
    uDB_t       save;
    bool        rc;

    saveRegs( &save );
    Buff.EBX = off;
    Buff.GS  = seg;
    TaskExecute( (excfn)DoReadWord );
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
    uDB_t       save;
    bool        rc;

    saveRegs( &save );
    Buff.EAX = data;
    Buff.EBX = off;
    Buff.GS  = seg;
    TaskExecute( (excfn)DoWriteWord );
    if( Buff.Cmd != DBG_N_Breakpoint ) {
        rc = FALSE;
    } else {
        rc = TRUE;
    }
    WriteRegs( &save );
    return( rc );
}


void TaskPrint( byte *ptr, unsigned len )
{
    uDB_t       save;

    saveRegs( &save );
    while( len > sizeof( XferBuff ) ) {
        WriteLinear( ptr, (ULONG)&XferBuff, sizeof( XferBuff ) );
        Buff.EAX = (ULONG)&XferBuff;
        Buff.EDX = sizeof( XferBuff );
        TaskExecute( (excfn)DoWritePgmScrn );
        ptr += sizeof( XferBuff );
        len -= sizeof( XferBuff );
    }
    WriteLinear( ptr, (ULONG)&XferBuff, len );
    Buff.EAX = (ULONG)&XferBuff;
    Buff.EDX = len;
    TaskExecute( (excfn)DoWritePgmScrn );
    WriteRegs( &save );
}


void TaskReadXMMRegs( struct x86_xmm *xmm_regs )
{
    uDB_t       save;

    saveRegs( &save );
    Buff.EAX = (ULONG)&XferBuff;
    TaskExecute( (excfn)DoReadXMMRegs );
    ReadLinear( (void*)xmm_regs, (ULONG)&XferBuff, sizeof( *xmm_regs ) );
    WriteRegs( &save );
}


void TaskWriteXMMRegs( struct x86_xmm *xmm_regs )
{
    uDB_t       save;

    saveRegs( &save );
    WriteLinear( (void*)xmm_regs, (ULONG)&XferBuff, sizeof( *xmm_regs ) );
    Buff.EAX = (ULONG)&XferBuff;
    TaskExecute( (excfn)DoWriteXMMRegs );
    WriteRegs( &save );
}
