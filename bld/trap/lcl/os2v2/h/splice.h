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
* Description:  OS/2 2.x splice DLL exported functions.
*
****************************************************************************/

extern void __export DoReadWord( void );
extern void __export DoWriteWord( void );

extern void __export DoOpen( char *name, int mode, int flags );
#pragma aux DoOpen parm [eax] [edx] [ecx];

extern void __export DoClose( HFILE hdl );
#pragma aux DoClose parm [eax];

extern void __export DoDupFile( HFILE old, HFILE new );
#pragma aux DoDupFile parm [eax] [edx];

extern void __export DoWritePgmScrn( char *buff, ULONG len );
#pragma aux DoWritePgmScrn parm [eax] [edx];

extern void __export DoReadXMMRegs( struct x86_xmm *xmm_regs );
#pragma aux DoReadXMMRegs parm [eax];

extern void __export DoWriteXMMRegs( struct x86_xmm *xmm_regs );
#pragma aux DoWriteXMMRegs parm [eax];


#define XFERBUFF_SIZE      1024
#define TEMPSTACK_SIZE    16384

extern char __export TempStack[TEMPSTACK_SIZE];
extern char __export XferBuff[XFERBUFF_SIZE];
