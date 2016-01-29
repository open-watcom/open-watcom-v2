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
* Description:  DPMI function wrappers for Win386.
*
****************************************************************************/


#include "dpmi.h"


#pragma aux DPMIGetAliases parm[dx ax] [es si] [cx] value[ax];
extern WORD     DPMIGetAliases( DWORD offset, DWORD __far *res, WORD cnt);
extern void     DPMIFreeAlias( WORD sel );
extern WORD     DPMIGetHugeAlias( DWORD offset, DWORD __far *res, DWORD size );
extern void     DPMIFreeHugeAlias( DWORD desc, DWORD size );
extern WORD     PASCAL FAR __DPMIGetAlias( DWORD offset, DWORD __far *res );
extern void     PASCAL FAR __DPMIFreeAlias( DWORD desc );
extern WORD     PASCAL FAR __DPMIGetHugeAlias( DWORD offset, DWORD __far *res, DWORD size );
extern void     PASCAL FAR __DPMIFreeHugeAlias( DWORD desc, DWORD size );
extern void     setLimitAndAddr( WORD sel, DWORD addr, DWORD len, WORD type );
extern WORD     DPMIGet32( dpmi_mem_block _FAR *addr_data, DWORD len );
extern WORD     InitFlatAddrSpace( DWORD baseaddr, DWORD len );
extern void     DPMIFree32( DWORD handle );
extern DWORD    PASCAL FAR __DPMIAlloc( DWORD size );
extern WORD     PASCAL FAR __DPMIFree( DWORD addr );
extern void     FreeDPMIMemBlocks( void );
extern void     GetDataSelectorInfo( void );
extern int      InitSelectorCache( void );
extern void     FiniSelectorCache( void );
extern void     FiniSelList( void );
