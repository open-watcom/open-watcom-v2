/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2025 The Open Watcom Contributors. All Rights Reserved.
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


#define ALIAS_OFFS(x)   ((WORD __far *)(x))[0]
#define ALIAS_SEL(x)    ((WORD __far *)(x))[1]

/*
 * Segment registers ES, FS and GS in 16-bit and GS in 32-bit Windows code are volatile.
 * It means code generator don't cleanup these registers and content is last
 * assigned value.
 *
 * Strange Windows 3.0 "386 enhanced mode" probably has DPMI bug:
 *      If ES, FS, or GS contain the selector being freed, it will crash dump
 *      to DOS within the DPMI call to free an LDT descriptor.
 *
 * A way to work around this problem is to clear any of these registers before
 * calling DPMI if the register contains a selector for the descriptor being freed.
 */
extern int  WDPMI_FreeLDTDescriptor( WORD );
#pragma aux WDPMI_FreeLDTDescriptor = \
        /* fix CPL bits */ \
        _MOV_CX_BX      \
        _OR_CL 3        \
        _XOR_DX_DX      \
        /* reset ES register if necessary */ \
        _MOV_AX_ES      \
        _OR_AL 3        \
        _CMP_AX_CX      \
        _JNZ 2          \
        _MOV_ES_DX      \
        /* reset FS register if necessary */ \
        _MOV_AX_FS      \
        _OR_AL 3        \
        _CMP_AX_CX      \
        _JNZ 2          \
        _MOV_FS_DX      \
        /* reset GS register if necessary */ \
        _MOV_AX_GS      \
        _OR_AL 3        \
        _CMP_AX_CX      \
        _JNZ 2          \
        _MOV_GS_DX      \
        /* call DPMI service */ \
        _MOV_AX_W DPMI_0001 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] \
    __value         [__ax] \
    __modify __exact [__ax __cx __dx __es __fs __gs]


extern bool     _DPMI_GetAliases( DWORD offs32, LPDWORD palias, WORD count);
extern bool     _DPMI_GetAlias( DWORD offs32, LPDWORD palias );
#pragma aux _DPMI_GetAlias __parm [__dx __ax] [__es __si]
extern void     _DPMI_FreeAlias( DWORD alias );
#pragma aux _DPMI_FreeAlias __parm [__dx __ax]
extern bool     _DPMI_GetHugeAlias( DWORD offs32, LPDWORD palias, DWORD size );
extern void     _DPMI_FreeHugeAlias( DWORD alias, DWORD size );
extern bool     _DPMI_Get32( dpmi_mem_block _DLLFAR *addr_data, DWORD len );
extern void     _DPMI_Free32( DWORD handle );
extern bool     InitFlatAddrSpace( DWORD baseaddr, DWORD len );
extern void     FreeDPMIMemBlocks( void );
extern void     GetDataSelectorInfo( void );
extern bool     InitSelectorCache( void );
extern void     FiniSelectorCache( void );
extern void     FiniSelList( void );
