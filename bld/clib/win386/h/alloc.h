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


#define INCLUDE_MMSYSTEM_H
#include "cover.h"
#include <stdio.h>
#include <stddef.h>
#include <dos.h>

extern WORD __pascal DPMIGetAlias( DWORD, DWORD * );
extern WORD __pascal DPMIGetHugeAlias( DWORD, DWORD *, DWORD );
extern void __pascal DPMIFreeAlias( DWORD );
extern void __pascal DPMIFreeHugeAlias( DWORD, DWORD );
extern WORD __pascal DPMIResizeDS( DWORD );

extern void ClearFS_GS( void );
#pragma aux ClearFS_GS = \
0x33 0xC0 /* xor        eax,eax */      \
0x8E 0xE0 /* mov        fs,ax */        \
0x8E 0xE8 /* mov        gs,ax */        \
        modify [eax];
