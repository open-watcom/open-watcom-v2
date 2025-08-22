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
* Description:  16-bit DPMI function wrappers for Win386.
*
****************************************************************************/


#include "descript.h"
#include "asmbytes.h"


#define ALIAS_OFFS(x)   ((LPWORD)(x))[0]
#define ALIAS_SEL(x)    ((LPWORD)(x))[1]

#define WDPMI_ERROR(rc) ((long)(rc) < 0)
#define WDPMI_OK(rc)    ((long)(rc) >= 0)
#define WDPMI_INFO(rc)  ((WORD)rc)

typedef DWORD       wdpmi_ret;

typedef struct wdpmi_mem_block {
    DWORD       linear;
    DWORD       handle;
} wdpmi_mem_block;

#define DPMI_0000   0x00 0x00
#define DPMI_0001   0x01 0x00
#define DPMI_0003   0x03 0x00
#define DPMI_0006   0x06 0x00
#define DPMI_0007   0x07 0x00
#define DPMI_0008   0x08 0x00
#define DPMI_0009   0x09 0x00
#define DPMI_000B   0x0b 0x00
#define DPMI_0400   0x00 0x04
#define DPMI_0501   0x01 0x05
#define DPMI_0502   0x02 0x05

#define WDPMI_FreeMemoryBlock(a)                __WDPMI_FreeMemoryBlock( (a) >> 16, (a) )
#define WDPMI_SetSegmentBaseAddress(a,b)        __WDPMI_SetSegmentBaseAddress( (a), (b) >> 16, (b) )
#define WDPMI_AllocateLDTDescriptors            __WDPMI_AllocateLDTDescriptors
#define WDPMI_FreeLDTDescriptor                 __WDPMI_FreeLDTDescriptor
#define WDPMI_SetDescriptorAccessRights         __WDPMI_SetDescriptorAccessRights
#define WDPMI_SetSegmentLimit(a,b)              __WDPMI_SetSegmentLimit( (a), (b) >> 16, (b) & 0xffff )
#define WDPMI_AllocateMemoryBlock(a,b)          __WDPMI_AllocateMemoryBlock( (a), (b) >> 16, (b) )
#define WDPMI_GetNextSelectorIncrementValue     __WDPMI_GetNextSelectorIncrementValue
#define WDPMI_GetDescriptor                     __WDPMI_GetDescriptor
#define WDPMI_GetSegmentBaseAddress             __WDPMI_GetSegmentBaseAddress

extern int      __WDPMI_FreeMemoryBlock( WORD hiw, WORD low );
extern int      __WDPMI_SetSegmentBaseAddress( WORD, WORD hiw, WORD low );
extern long     __WDPMI_AllocateLDTDescriptors( WORD );
extern int      __WDPMI_FreeLDTDescriptor( WORD );
extern int      __WDPMI_SetDescriptorAccessRights( WORD, WORD );
extern int      __WDPMI_SetSegmentLimit( WORD, WORD hiw, WORD low );
extern int      __WDPMI_AllocateMemoryBlock( wdpmi_mem_block __far *, WORD hiw, WORD low );
extern WORD     __WDPMI_GetNextSelectorIncrementValue( void );
extern int      __WDPMI_GetDescriptor( WORD, descriptor __far * );
extern DWORD    __WDPMI_GetSegmentBaseAddress( WORD );

#pragma aux __WDPMI_AllocateLDTDescriptors = \
        _MOV_AX_W DPMI_0000 \
        _INT_31         \
        _SBB_CX_CX      \
    __parm __caller [__cx] \
    __value         [__cx __ax] \
    __modify __exact [__ax __cx]

/*
 * If any segment register contains invalid selector then issue happen when
 * segment register is accessed.
 *
 * it looks like DPMI service for free LDT descriptor access all segment registers.
 * On Windows 3.0 "386 enhanced mode" if ES, FS, or GS contain an invalid selector
 * or the selector being freed, it will crash dump to DOS within the DPMI call
 * to free an LDT descriptor.
 *
 * Little info how segment registers are used in Windows 32-bit Extender.
 *
 * Use of segment registers in 16-bit and 32-bit code is different.
 * In 16-bit code segment registers ES, FS and GS are always volatile and DS
 * register is volatile for big data memory models (not used in extender code).
 * In 32-bit code only FS and GS are volatile and DS and ES registers
 * are fixed to flat memory selector.
 * If code return from 16-bit code then ES and DS registers are restored in
 * thunk code to flat memory selector.
 *
 * A way to work around this problem is to reset (set to null selector) volatile
 * segment registers before calling DPMI service for free LDT descriptor.
 * - in 32-bit code reset FS and GS registers
 * - in 16-bit code reset ES, FS and GS registers
 *
 */
#pragma aux __WDPMI_FreeLDTDescriptor = \
        /* reset segment registers */ \
        _XOR_AX_AX      \
        _MOV_ES_AX      \
        _MOV_FS_AX      \
        _MOV_GS_AX      \
        /* call DPMI service */ \
        _MOV_AX_W DPMI_0001 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] \
    __value         [__ax] \
    __modify __exact [__ax __es __fs __gs]

#pragma aux __WDPMI_GetNextSelectorIncrementValue = \
        _MOV_AX_W DPMI_0003 \
        _INT_31         \
    __parm __caller [__bx] \
    __value         [__ax] \
    __modify __exact [__ax]

/*
 * if failed then return (uint_32)-1
 */
#pragma aux __WDPMI_GetSegmentBaseAddress = \
        _MOV_AX_W DPMI_0006 \
        _INT_31         \
        _SBB_AX_AX      \
        _OR_CX_AX       \
        _OR_DX_AX       \
    __parm __caller [__bx] \
    __value         [__cx __dx] \
    __modify __exact [__ax __cx __dx]

#pragma aux __WDPMI_SetSegmentBaseAddress = \
        _MOV_AX_W DPMI_0007 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__dx] \
    __value         [__ax] \
    __modify __exact [__ax]

#pragma aux __WDPMI_SetSegmentLimit = \
        _MOV_AX_W DPMI_0008 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__dx] \
    __value         [__ax] \
    __modify __exact [__ax]

#pragma aux __WDPMI_SetDescriptorAccessRights = \
        _MOV_AX_W DPMI_0009 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] \
    __value         [__ax] \
    __modify __exact [__ax]

#pragma aux __WDPMI_GetDescriptor = \
        _MOV_AX_W DPMI_000B \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__es __di] \
    __value         [__ax] \
    __modify __exact [__ax]

#pragma aux __WDPMI_AllocateMemoryBlock =  \
        _MOV_AX_W DPMI_0501 \
        _INT_31         \
        _SBB_AX_AX      \
        _XCHG_BX_DX      \
        "mov  es:[bx],cx" \
        "mov  es:[bx+2],dx" \
        "mov  es:[bx+4],di" \
        "mov  es:[bx+6],si" \
    __parm __caller [__es __dx] [__bx] [__cx] \
    __value         [__ax] \
    __modify __exact [__ax __bx __cx __dx __di __si]

#pragma aux __WDPMI_FreeMemoryBlock =  \
        _MOV_AX_W DPMI_0502 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__si] [__di] \
    __value         [__ax] \
    __modify __exact [__ax]


extern bool     _WDPMI_GetAliases( DWORD offs32, LPDWORD palias, WORD count);
extern bool     _WDPMI_GetAlias( DWORD offs32, LPDWORD palias );
#pragma aux _WDPMI_GetAlias __parm [__dx __ax] [__es __si]
extern void     _WDPMI_FreeAlias( DWORD alias );
#pragma aux _WDPMI_FreeAlias __parm [__dx __ax]
extern bool     _WDPMI_GetHugeAlias( DWORD offs32, LPDWORD palias, DWORD size );
extern void     _WDPMI_FreeHugeAlias( DWORD alias, DWORD size );
extern bool     _WDPMI_Get32( wdpmi_mem_block _DLLFAR *addr_data, DWORD len );
extern void     _WDPMI_Free32( DWORD handle );
extern bool     InitFlatAddrSpace( DWORD baseaddr, DWORD len );
extern void     FreeDPMIMemBlocks( void );
extern bool     InitSelectorCache( void );
extern void     FiniSelectorCache( void );
extern void     FiniSelList( void );
