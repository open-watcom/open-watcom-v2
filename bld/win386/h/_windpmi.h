/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DPMI function wrappers for Win386.
*
****************************************************************************/


#include "dpmi.h"



extern WORD     _DPMIGetAliases( DWORD offset, DWORD __far *desc, WORD cnt);
extern WORD     _DPMIGetAlias( DWORD offset, DWORD __far *desc);
#pragma aux _DPMIGetAlias parm[dx ax] [es si] value[ax];
extern void     _DPMIFreeAlias( WORD sel );
extern WORD     _DPMIGetHugeAlias( DWORD offset, DWORD __far *res, DWORD size );
extern void     _DPMIFreeHugeAlias( DWORD desc, DWORD size );
extern WORD     _DPMIGet32( dpmi_mem_block _FAR *addr_data, DWORD len );
extern void     _DPMIFree32( DWORD handle );
extern WORD     InitFlatAddrSpace( DWORD baseaddr, DWORD len );
extern void     FreeDPMIMemBlocks( void );
extern void     GetDataSelectorInfo( void );
extern int      InitSelectorCache( void );
extern void     FiniSelectorCache( void );
extern void     FiniSelList( void );
