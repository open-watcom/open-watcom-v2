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
* Description:  Special access routines spliced into the debuggee. See
*               pgmexec.c for description.
*
****************************************************************************/


#include <stddef.h>
#include <string.h>
#define INCL_BASE
#include <os2.h>
#include "madregs.h"
#include "splice.h"
#include "cpuglob.h"

/* We need separate stack for executing spliced code. We really wouldn't
 * want to mess up debuggee's stack!
 */
char __export TempStack[TEMPSTACK_SIZE];

char __export XferBuff[XFERBUFF_SIZE];

#define OPEN_CREATE  1
#define OPEN_PRIVATE 2

static long OpenFile( char *name, ULONG mode, int flags )
{
    HFILE       hdl;
    ULONG       action;
    ULONG       openflags;
    ULONG       openmode;
    APIRET      rc;

    if( flags & OPEN_CREATE ) {
        openflags = 0x12;
        openmode = 0x2042;
    } else {
        openflags = 0x01;
        openmode = mode | 0x2040;
    }
    if( flags & OPEN_PRIVATE ) {
        openmode |= 0x80;
    }
    rc = DosOpen( name,          /* name */
                 &hdl,           /* handle to be filled in */
                 &action,        /* action taken */
                 0,              /* initial allocation */
                 0,              /* normal file */
                 openflags,      /* open the file */
                 openmode,       /* deny-none, inheritance */
                 0 );             /* reserved */
    if( rc != 0 )
        return 0xFFFF0000 | rc;
    return( hdl );
}


void doReadWord( void );
#pragma aux doReadWord =           \
    "mov  ax, word ptr gs:[ebx]"   \
    "int  3";

void __export DoReadWord( void )
{
    doReadWord();
}

void doWriteWord( void );
#pragma aux doWriteWord =          \
    "mov  word ptr gs:[ebx], ax"   \
    "int  3";

void __export DoWriteWord( void )
{
    doWriteWord();
}

void __export DoOpen( char *name, int mode, int flags )
{
    BreakPointParm( OpenFile( name, mode, flags ) );
}

void __export DoClose( HFILE hdl )
{
    BreakPointParm( DosClose( hdl ) );
}

void __export DoDupFile( HFILE old, HFILE new )
{
    HFILE       new_t;
    USHORT      rc;

    new_t = new;
    rc = DosDupHandle( old, &new_t );
    if( rc != 0 ) {
        BreakPointParm( (HFILE) - 1 );
    } else {
        BreakPointParm( new_t );
    }
}

void __export DoWritePgmScrn( char *buff, ULONG len )
{
    ULONG   written;

    DosWrite( 2, buff, len, &written );
    BreakPointParm( 0 );
}


void fxsave( unsigned char *addr );
#pragma aux fxsave parm [eax] =  \
    ".686" \
    ".XMM" \
    "fxsave [eax]";

void __export DoReadXMMRegs( struct x86_xmm *xmm_regs )
{
    unsigned char   fxsave_buff[ 512 + 16 ];
    unsigned char   *aligned_buf;

    /* The FXSAVE buffer must be 16-byte aligned! */
    aligned_buf = (unsigned char*)(((unsigned)fxsave_buff + 15) & ~15);
    fxsave( aligned_buf );
    memcpy( xmm_regs->xmm, aligned_buf + 160, 8 * 16 );
    xmm_regs->mxcsr = *(unsigned_32*)(aligned_buf + 20);
    BreakPointParm( 0 );
}

void fxrstor( unsigned char *addr );
#pragma aux fxrstor parm [eax] =  \
    ".XMM" \
    ".686" \
    "fxrstor [eax]";

void __export DoWriteXMMRegs( struct x86_xmm *xmm_regs )
{
    unsigned char   fxrstor_buff[ 512 + 16 ];
    unsigned char   *aligned_buf;

    /* The FXRSTOR buffer must be 16-byte aligned! */
    aligned_buf = (unsigned char*)(((unsigned)fxrstor_buff + 15) & ~15);
    memcpy( aligned_buf + 160, xmm_regs->xmm, 8 * 16 );
    *(unsigned_32*)(aligned_buf + 20) = xmm_regs->mxcsr;
    fxrstor( aligned_buf );
    BreakPointParm( 0 );
}
