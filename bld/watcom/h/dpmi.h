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
* Description:  DPMI API int 31h wrappers.
*
****************************************************************************/


#ifndef __DPMI_H
#define __DPMI_H

#include "watcom.h"


#define _INT            0xcd

#if defined( __OSI__ ) && defined( __CALL31__ )
 extern  void   __Int31();
 #define _INT_31        "call __Int31"
#else
 #define _INT_31        _INT 0x31
#endif

#define DOS_SEG( a ) ((uint_16)(a & 0Xffff))
#define DOS_SEL( a ) ((uint_16)(a >> 16L))

/*
 * access right definitions
 */
#define ACCESS_DATA16      0x0093     /* 0000 0000 1001 0011 */
#define ACCESS_DATA16BIG   0x8093     /* 1000 0000 1001 0011 */
#define ACCESS_CODE32SMALL 0x409B     /* 0100 0000 1001 1011 */
#define ACCESS_CODE32BIG   0xC09B     /* 1100 0000 1001 1011 */
#define ACCESS_DATA32SMALL 0x4093     /* 0100 0000 1001 0011 */
#define ACCESS_DATA32BIG   0xC093     /* 1100 0000 1001 0011 */
#define ACCESS_CODE 1
#define ACCESS_DATA 2

typedef struct {
    uint_32     edi;
    uint_32     esi;
    uint_32     ebp;
    uint_32     reserved;
    uint_32     ebx;
    uint_32     edx;
    uint_32     ecx;
    uint_32     eax;
    uint_16     flags;
    uint_16     es;
    uint_16     ds;
    uint_16     fs;
    uint_16     gs;
    uint_16     ip;
    uint_16     cs;
    uint_16     sp;
    uint_16     ss;
} rm_call_struct;

typedef struct {
    uint_32     largest_free;
    uint_32     max_unlocked_page_alloc;
    uint_32     max_locked_page_alloc;
    uint_32     linear_addr_space_in_pages;
    uint_32     total_unlocked_pages;
    uint_32     free_pages;
    uint_32     physical_pages;
    uint_32     free_linear_addr_space_in_pages;
    uint_32     size_of_page_file_in_pages;
    uint_32     fill[4];
} dpmi_mem;

typedef struct {
    uint_8      major_version;
    uint_8      minor_version;
    int_16      flags;
    uint_8      processor_type;
    uint_8      master_pic_base_interrupt;
    uint_8      slave_pic_base_interrupt;
} version_info;
#define VERSION_80386   0x0001

/* Definitions for manipulating protected mode descriptors ... used
 * with DPMIGetDescriptor, DPMISetDescriptor, DPMISetDescriptorAccessRights,
 * etc.
 */
typedef enum {
    DSC_ACCESSED        = 0x01,
    DSC_RDWR            = 0x02,
    DSC_EXPAND_DOWN     = 0x04,
    DSC_EXECUTE         = 0x08,
    DSC_MUST_BE_1       = 0x10,
    DSC_PRESENT         = 0x80,
    DSC_USER_AVAIL      = 0x1000,
    DSC_MUST_BE_0       = 0x2000,
    DSC_USE32           = 0x4000,
    DSC_PAGE_GRANULAR   = 0x8000
} dsc_flags;

typedef struct {
    uint_8  accessed : 1;
    uint_8  rdwr     : 1;
    uint_8  exp_down : 1;
    uint_8  execute  : 1;
    uint_8  mustbe_1 : 1;
    uint_8  dpl      : 2;
    uint_8  present  : 1;
} dsc_type;

typedef struct {
    uint_8           : 4;
    uint_8  useravail: 1;
    uint_8  mustbe_0 : 1;
    uint_8  use32    : 1;
    uint_8  page_gran: 1;
} dsc_xtype;

typedef struct {
    uint_16         lim_0_15;
    uint_16         base_0_15;
    uint_8          base_16_23;
    dsc_type        type;
    union {
        struct {
            uint_8  lim_16_19: 4;
            uint_8           : 4;
        };
        dsc_xtype   xtype;
    };
    uint_8          base_24_31;
} descriptor;

typedef enum {
    DPMI_WATCH_EXEC,
    DPMI_WATCH_WRITE,
    DPMI_WATCH_READWRITE
} dpmi_watch_type;

typedef struct {
    uint_32     linear;
    uint_32     handle;
} dpmi_mem_block;

#define DPMISetWatch                            _DPMISetWatch
#define DPMIClearWatch                          _DPMIClearWatch
#define DPMITestWatch                           _DPMITestWatch
#define DPMIResetWatch                          _DPMIResetWatch
#define DPMIFreeMemoryBlock                     _DPMIFreeMemoryBlock
#define DPMILockLinearRegion                    _DPMILockLinearRegion
#define DPMIUnlockLinearRegion                  _DPMIUnlockLinearRegion
#define DPMIGetDescriptor                       _DPMIGetDescriptor
#define DPMISetDescriptor                       _DPMISetDescriptor
#define DPMICreateCodeSegmentAliasDescriptor    _DPMICreateCodeSegmentAliasDescriptor
#define DPMIAllocateLDTDescriptors              _DPMIAllocateLDTDescriptors
#define DPMISegmentToDescriptor                 _DPMISegmentToDescriptor
#define DPMIFreeLDTDescriptor                   _DPMIFreeLDTDescriptor
#define DPMIGetSegmentBaseAddress               _DPMIGetSegmentBaseAddress
#define DPMISetSegmentBaseAddress               _DPMISetSegmentBaseAddress
#define DPMISetSegmentLimit                     _DPMISetSegmentLimit
#define DPMISetDescriptorAccessRights           _DPMISetDescriptorAccessRights
#define DPMISimulateRealModeInterrupt           _DPMISimulateRealModeInterrupt
#define DPMIGetNextSelectorIncrementValue       _DPMIGetNextSelectorIncrementValue
#define DPMIGetRealModeInterruptVector          _DPMIGetRealModeInterruptVector
#define DPMISetRealModeInterruptVector          _DPMISetRealModeInterruptVector
#define DPMIAllocateRealModeCallBackAddress     _DPMIAllocateRealModeCallBackAddress
#define DPMIFreeRealModeCallBackAddress         _DPMIFreeRealModeCallBackAddress
#define DPMIGetPMInterruptVector                _DPMIGetPMInterruptVector
#define DPMISetPMInterruptVector                _DPMISetPMInterruptVector
#define DPMISetPMExceptionVector                _DPMISetPMExceptionVector
#define DPMIGetPMExceptionVector                _DPMIGetPMExceptionVector

#define DPMIAllocateDOSMemoryBlock              _DPMIAllocateDOSMemoryBlock
#define DPMIFreeDOSMemoryBlock                  _DPMIFreeDOSMemoryBlock
#define DPMIIdle                                _DPMIIdle
#define DPMIModeDetect                          _DPMIModeDetect
#define DPMIRawRMtoPMAddr                       _DPMIRawRMtoPMAddr
#define DPMIRawPMtoRMAddr                       _DPMIRawPMtoRMAddr
#define DPMISaveRMStateAddr                     _DPMISaveRMStateAddr
#define DPMISavePMStateAddr                     _DPMISavePMStateAddr
#define DPMISaveStateSize                       _DPMISaveStateSize
#define DPMIGetVendorSpecificAPI                _DPMIGetVendorSpecificAPI

#if defined( _M_I86SM ) || defined( _M_I86MM ) || defined( __386__ )
#if defined(__386__)
#define DPMIGetVersion                          _DPMIGetVersion
#else
#define DPMIGetVersion                          _nDPMIGetVersion
#endif
#define DPMIAllocateMemoryBlock                 _nDPMIAllocateMemoryBlock
#define DPMIGetFreeMemoryInformation            _nDPMIGetFreeMemoryInformation
#define DPMIResizeMemoryBlock                   _nDPMIResizeMemoryBlock
#else
#define DPMIGetVersion                          _fDPMIGetVersion
#define DPMIAllocateMemoryBlock                 _fDPMIAllocateMemoryBlock
#define DPMIGetFreeMemoryInformation            _fDPMIGetFreeMemoryInformation
#define DPMIResizeMemoryBlock                   _fDPMIResizeMemoryBlock
#endif

extern void     _DPMIFreeRealModeCallBackAddress( void __far * proc );
extern void     __far *_DPMIAllocateRealModeCallBackAddress( void __far * proc, rm_call_struct __far *cs );
extern void     __far *_DPMIGetRealModeInterruptVector( uint_8 iv );
extern int      _DPMISetPMInterruptVector( uint_8 iv, void __far * ptr );
extern void     _DPMISetPMExceptionVector( uint_8 iv, void __far * ptr );
extern void     __far *_DPMIGetPMExceptionVector( uint_8 iv );
extern void     __far *_DPMIGetPMInterruptVector( uint_8 iv );
extern int      _DPMISetRealModeInterruptVector( uint_8 iv, void __far * ptr );
extern int_16   _DPMIModeDetect( void );
extern void     _DPMIIdle( void );
extern void     _DPMIGetVersion( version_info __far * );
extern void     _fDPMIGetVersion( version_info __far * );
extern void     _nDPMIGetVersion( version_info * );
extern int_32   _DPMIAllocateLDTDescriptors( uint_16 );
extern uint_16  _DPMISegmentToDescriptor( uint_16 );
extern void     _DPMIFreeLDTDescriptor( uint_16 );
extern uint_16  _DPMIGetNextSelectorIncrementValue( void );
extern uint_32  _DPMIGetSegmentBaseAddress( uint_16 );
extern void     _DPMISetSegmentBaseAddress( uint_16, uint_32 );
extern int_16   _DPMISetSegmentLimit( uint_16, uint_32 );
extern void     _DPMISetDescriptorAccessRights( uint_16, uint_16 );
extern int_16   _fDPMIAllocateMemoryBlock( dpmi_mem_block __far *, uint_32 );
extern int_16   _nDPMIAllocateMemoryBlock( dpmi_mem_block *, uint_32 );
extern int_16   _fDPMIResizeMemoryBlock( dpmi_mem_block __far *, uint_32, uint_32 );
extern int_16   _nDPMIResizeMemoryBlock( dpmi_mem_block *, uint_32, uint_32 );
extern int_16   _DPMIFreeMemoryBlock( uint_32 );
extern int_16   _DPMILockLinearRegion( uint_32, uint_32 );
extern int_16   _DPMIUnlockLinearRegion( uint_32, uint_32 );
extern int      _DPMIGetDescriptor( uint_16, descriptor __far * );
extern int      _DPMISetDescriptor( uint_16, descriptor __far * );
extern int_32   _DPMICreateCodeSegmentAliasDescriptor( uint_16 );
extern int_16   _nDPMIGetFreeMemoryInformation( dpmi_mem * );
extern int_16   _fDPMIGetFreeMemoryInformation( dpmi_mem __far * );
extern int      _DPMISimulateRealModeInterrupt( uint_8 interrupt, uint_8 flags,
                        uint_16 words_to_copy, rm_call_struct __far *call_st );
extern int_32   _DPMIAllocateDOSMemoryBlock( uint_16 para );
extern void     _DPMIFreeDOSMemoryBlock( uint_16 sel );
extern void     __far *_DPMIRawPMtoRMAddr( void );
extern uint_32  _DPMIRawRMtoPMAddr( void );
extern void     __far *_DPMISaveRMStateAddr( void );
extern uint_32  _DPMISavePMStateAddr( void );
extern uint_16  _DPMISaveStateSize( void );
extern void     __far *_DPMIGetVendorSpecificAPI( char __far * );

extern int_32   _DPMISetWatch( uint_32 linear, uint_8 len, uint_8 type );
extern void     _DPMIClearWatch( uint_16 handle );
extern int      _DPMITestWatch( uint_16 handle );
extern void     _DPMIResetWatch( uint_16 handle );

#pragma aux _DPMIModeDetect = \
        "mov    ax,1686h"   \
        "int 2fh"           \
    __parm [] \
    __value [__ax] \
    __modify []

#pragma aux _DPMIIdle = \
        "mov    ax,1680h"   \
        "int 2fh"           \
    __parm [] \
    __value \
    __modify [__ax]

#if defined(__386__)
#pragma aux _DPMISetWatch = \
        "mov    ax,0b00h"   \
        "mov    cx,bx"      \
        "shr    ebx,16"     \
        _INT_31             \
        "sbb    eax,eax"    \
        "mov    ax,bx"      \
    __parm [__ebx] [__dl] [__dh] \
    __value [__eax] \
    __modify __exact [__eax __ebx __ecx]
#else
#pragma aux _DPMISetWatch = \
        "mov    ax,0b00h"   \
        "xchg   bx,cx"      \
        _INT_31             \
        "sbb    cx,cx"      \
    __parm [__bx __cx] [__dl] [__dh] \
    __value [__cx __bx] \
    __modify __exact [__ax __bx __cx]
#endif

#pragma aux _DPMIClearWatch = \
        "mov    ax,0b01h"   \
        _INT_31             \
    __parm [__bx] \
    __value \
    __modify __exact [__ax]

#if defined(__386__)
#pragma aux _DPMITestWatch = \
        "mov  ax,0b02h"     \
        _INT_31             \
        "sbb  ebx,ebx"      \
        "and  eax,1"        \
        "or   ebx,eax"      \
    __parm [__bx] \
    __value [__ebx] \
    __modify __exact [__eax __ebx]
#else
#pragma aux _DPMITestWatch = \
        "mov  ax,0b02h"     \
        _INT_31             \
        "sbb  bx,bx"        \
        "and  ax,1"         \
        "or   bx,ax"        \
    __parm [__bx] \
    __value [__bx] \
    __modify __exact [__ax __bx]
#endif

#pragma aux _DPMIResetWatch = \
        "mov    ax,0b03h"   \
        _INT_31             \
    __parm [__bx] \
    __value \
    __modify __exact [__ax]

#if defined(__386__)
#pragma aux _DPMIGetVersion = \
        "push   ds"         \
        "mov    ds,edx"     \
        "mov    ax,400h"    \
        _INT_31             \
        "mov    byte ptr [esi],ah"  \
        "mov    byte ptr [esi+1],al"  \
        "mov    word ptr [esi+2],bx"  \
        "mov    byte ptr [esi+4],cl"  \
        "mov    byte ptr [esi+5],dh"  \
        "mov    byte ptr [esi+6],dl"  \
        "pop    ds"         \
    __parm [__dx __esi] \
    __value \
    __modify[__ax __bx __cl __dx]
#else
#pragma aux _nDPMIGetVersion = \
        "mov    ax,400h"    \
        _INT_31             \
        "mov    byte ptr [si],ah"  \
        "mov    byte ptr [si+1],al"  \
        "mov    word ptr [si+2],bx"  \
        "mov    byte ptr [si+4],cl"  \
        "mov    byte ptr [si+5],dh"  \
        "mov    byte ptr [si+6],dl"  \
    __parm [__si] \
    __value \
    __modify[__ax __bx __cl __dx]

#pragma aux _fDPMIGetVersion = \
        "mov    ax,400h"    \
        _INT_31             \
        "mov    byte ptr es:[si],ah"  \
        "mov    byte ptr es:[si+1],al"  \
        "mov    word ptr es:[si+2],bx"  \
        "mov    byte ptr es:[si+4],cl"  \
        "mov    byte ptr es:[si+5],dh"  \
        "mov    byte ptr es:[si+6],dl"  \
    __parm [__es __si] \
    __value \
    __modify[__ax __bx __cl __dx]
#endif

#if defined(__386__)
#pragma aux _DPMIAllocateLDTDescriptors = \
        "xor  eax,eax"  \
        _INT_31         \
        "sbb  ecx,ecx"  \
        "mov  cx,ax"    \
    __parm [__cx] \
    __value [__ecx] \
    __modify __exact [__eax __ecx]
#else
#pragma aux _DPMIAllocateLDTDescriptors = \
        "xor  ax,ax"    \
        _INT_31         \
        "sbb  cx,cx"    \
    __parm [__cx] \
    __value [__cx __ax] \
    __modify __exact [__ax __cx]
#endif

#pragma aux _DPMIFreeLDTDescriptor = \
        "mov    ax,1"       \
        _INT_31             \
    __parm [__bx] \
    __value \
    __modify[__ax]

#pragma aux _DPMISegmentToDescriptor = \
        "mov    ax,2"       \
        _INT_31             \
    __parm [__bx] \
    __value \
    __modify[__ax]

#pragma aux _DPMIGetNextSelectorIncrementValue = \
        "mov    ax,3"       \
        _INT_31             \
    __parm [__bx] \
    __value \
    __modify[__ax]

#if defined(__386__)

#pragma aux _DPMIGetSegmentBaseAddress = \
        "xor    eax,eax"    \
        "mov    al,6"       \
        _INT_31             \
        "mov    eax,ecx"    \
        "shl    eax,16"     \
        "mov    ax,dx"      \
    __parm __caller [__bx] \
    __value [__eax] \
    __modify __exact [__eax __ebx __ecx __edx]
#else

#pragma aux _DPMIGetSegmentBaseAddress = \
        "mov    ax,6"       \
        _INT_31             \
    __parm [__bx] \
    __value[__cx __dx] \
    __modify []
#endif

#pragma aux _DPMISetSegmentBaseAddress = \
        "mov    ax,7"       \
        _INT_31             \
    __parm [__bx] [__cx __dx] \
    __value \
    __modify []

#if defined(__386__)
#pragma aux _DPMISetSegmentLimit = \
        "mov    ax,dx"      \
        "shr    edx,16"     \
        "mov    cx,dx"      \
        "mov    dx,ax"      \
        "mov    ax,8"       \
        _INT_31             \
        "xor    eax,eax"    \
    __parm [__bx] [__edx] \
    __value [__ax] \
    __modify []
#else
#pragma aux _DPMISetSegmentLimit = \
        "mov    ax,8"       \
        _INT_31             \
        "sbb    ax,ax"      \
    __parm [__bx] [__cx __dx] \
    __value [__ax] \
    __modify []
#endif

#pragma aux _DPMIFreeMemoryBlock =  \
        "xchg   si,di"      \
        "mov    ax,502h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    __parm [__di __si] \
    __value [__ax] \
    __modify []

#pragma aux _DPMISetDescriptorAccessRights = \
        "mov    ax,9"       \
        _INT_31             \
    __parm [__bx] [__cx] \
    __value \
    __modify []

#if defined(__386__)
#pragma aux _DPMILockLinearRegion = \
        "mov    di,dx"      \
        "shr    edx,16"     \
        "mov    si,dx"      \
        "mov    bx,ax"      \
        "shr    eax,16"     \
        "mov    cx,ax"      \
        "mov    ax,600h"    \
        _INT_31             \
        "sbb    eax,eax"    \
    __parm [__eax] [__edx] \
    __value [__ax] \
    __modify [__bx __cx __si __di]

#pragma aux _DPMIUnlockLinearRegion = \
        "mov    di,dx"      \
        "shr    edx,16"     \
        "mov    si,dx"      \
        "mov    bx,ax"      \
        "shr    eax,16"     \
        "mov    cx,ax"      \
        "mov    ax,601h"    \
        _INT_31             \
        "xor    eax,eax"    \
    __parm [__eax] [__edx] \
    __value[__ax] \
    __modify [__bx __cx __si __di]
#else
#pragma aux _DPMILockLinearRegion = \
        "mov    ax,600h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    __parm [__cx __bx] [__si __di] \
    __value[__ax] \
    __modify []

#pragma aux _DPMIUnlockLinearRegion = \
        "mov    ax,601h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    __parm [__cx __bx] [__si __di] \
    __value [__ax] \
    __modify []
#endif

#if defined(__386__)

#pragma aux _DPMIAllocateDOSMemoryBlock = \
        "mov    ax,100h"    \
        _INT_31             \
        "jnc short L1"      \
        "sbb    dx,dx"      \
    "L1: xchg   ax,dx"      \
        "shl    eax,16"     \
        "mov    ax,dx"      \
    __parm [__bx] \
    __value [__eax] \
    __modify [__dx]

#pragma aux _DPMIFreeDOSMemoryBlock = \
        "mov    ax,101h"    \
        _INT_31             \
        "sbb    eax,eax"    \
    __parm [__dx] \
    __value [__eax] \
    __modify []

#pragma aux _DPMISimulateRealModeInterrupt = \
        "push   es"         \
        "mov    es,edx"     \
        "mov    ax,300h"    \
        _INT_31             \
        "pop    es"         \
        "sbb    eax,eax"    \
    __parm [__bl] [__bh] [__cx] [__dx __edi] \
    __value [__eax] \
    __modify []

#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
        "mov    ax,0ah"     \
        _INT_31             \
        "rcl    eax,1"      \
        "ror    eax,1"      \
    __parm [__bx] \
    __value [__eax] \
    __modify []

#pragma aux _DPMIGetDescriptor = \
        "push   es"         \
        "mov    es,edx"     \
        "mov    ax,0bh"     \
        _INT_31             \
        "pop    es"         \
        "sbb    eax,eax"    \
    __parm [__bx] [__dx __edi] \
    __value [__eax] \
    __modify []

#pragma aux _DPMISetDescriptor = \
        "push   es"         \
        "mov    es,edx"     \
        "mov    ax,0ch"     \
        _INT_31             \
        "pop    es"         \
        "sbb    eax,eax"    \
    __parm [__bx] [__dx __edi] \
    __value [__eax] \
    __modify []
#else

#pragma aux _DPMIAllocateDOSMemoryBlock = \
        "mov ax,100h"       \
        _INT_31             \
        "jnc short L1"      \
        "sbb dx,dx"         \
    "L1: "                  \
    __parm [__bx] \
    __value [__dx __ax] \
    __modify []

#pragma aux _DPMIFreeDOSMemoryBlock = \
        "mov ax,101h"       \
        _INT_31             \
        "sbb ax,ax"         \
    __parm [__dx] \
    __value [__ax] \
    __modify []

#pragma aux _DPMISimulateRealModeInterrupt = \
        "mov ax,300h"       \
        _INT_31             \
        "sbb ax,ax"         \
    __parm [__bl] [__bh] [__cx] [__es __di] \
    __value [__ax] \
    __modify []

#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
        "mov ax,0ah"        \
        _INT_31             \
        "sbb dx,dx"         \
    __parm [__bx] \
    __value [__dx __ax] \
    __modify []

#pragma aux _DPMIGetDescriptor = \
        "mov    ax,0bh"     \
        _INT_31             \
        "sbb    ax,ax"      \
    __parm [__bx] [__es __di] \
    __value [__ax] \
    __modify []

#pragma aux _DPMISetDescriptor = \
        "mov    ax,0ch"     \
        _INT_31             \
        "sbb    ax,ax"      \
    __parm [__bx] [__es __di] \
    __value [__ax] \
    __modify []

#endif

#pragma aux _fDPMIAllocateMemoryBlock =  \
        "push   es"         \
        "push   ax"         \
        "push   dx"         \
        "xchg   bx,cx"      \
        "mov    ax,501H"    \
        _INT_31             \
        "sbb    ax,ax"      \
        "mov    dx,bx"      \
        "pop    es"         \
        "pop    bx"         \
        "mov    es:[bx],cx" \
        "mov    es:+2H[bx],dx"  \
        "mov    es:+4H[bx],di"  \
        "mov    es:+6H[bx],si"  \
        "pop    es"         \
    __parm [__ax __dx] [__bx __cx] \
    __value [__ax] \
    __modify [__bx __cx __dx __di __si]

#pragma aux _nDPMIAllocateMemoryBlock =  \
        "push   ax"         \
        "xchg   bx,cx"      \
        "mov    ax,501H"    \
        _INT_31             \
        "sbb    ax,ax"      \
        "mov    dx,bx"      \
        "pop    bx"         \
        "mov    ds:[bx],cx" \
        "mov    ds:+2H[bx],dx"  \
        "mov    ds:+4H[bx],di"  \
        "mov    ds:+6H[bx],si"  \
    __parm [__ax] [__bx __cx] \
    __value [__ax] \
    __modify [__bx __cx __dx __di __si]

#pragma aux _fDPMIResizeMemoryBlock =  \
        "push   es"         \
        "push   ax"         \
        "push   dx"         \
        "xchg   si,di"      \
        "xchg   bx,cx"      \
        "mov    ax,503h"    \
        _INT_31             \
        "sbb    ax,ax"      \
        "mov    dx,bx"      \
        "pop    es"         \
        "pop    bx"         \
        "mov    es:[bx],cx" \
        "mov    es:+2H[bx],dx"  \
        "mov    es:+4H[bx],di"  \
        "mov    es:+6H[bx],si"  \
        "pop    es"         \
    __parm [__dx __ax] [__bx __cx] [__di __si] \
    __value [__ax] \
    __modify [__di __si __bx __cx __dx]

#pragma aux _nDPMIResizeMemoryBlock =  \
        "push   ax"         \
        "xchg   si,di"      \
        "xchg   bx,cx"      \
        "mov    ax,503h"    \
        _INT_31             \
        "sbb    ax,ax"      \
        "mov    dx,bx"      \
        "pop    bx"         \
        "mov    ds:[bx],cx" \
        "mov    ds:+2H[bx],dx"  \
        "mov    ds:+4H[bx],di"  \
        "mov    ds:+6H[bx],si"  \
    __parm [__ax] [__bx __cx] [__di __si] \
    __value [__ax] \
    __modify [__di __si __bx __cx __dx]

#pragma aux _fDPMIGetFreeMemoryInformation = \
        "mov    ax,500h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    __parm [__es __di] \
    __value [__ax] \
    __modify []

#pragma aux _nDPMIGetFreeMemoryInformation = \
        "push   es"         \
        "push   ds"         \
        "pop    es"         \
        "mov    ax,500h"    \
        _INT_31             \
        "pop    es"         \
        "sbb    ax,ax"      \
    __parm [__di] \
    __value [__ax] \
    __modify []

#if defined(__386__)
#pragma aux _DPMIGetRealModeInterruptVector = \
        "mov    ax,200h"    \
        _INT_31             \
    __parm [__bl] \
    __value [__cx __edx] \
    __modify [__ax]

#pragma aux _DPMISetRealModeInterruptVector = \
        "mov    ax,201h"    \
        _INT_31             \
        "sbb    eax,eax"    \
    __parm [__bl] [__cx __edx] \
    __value [__eax] \
    __modify []
#else
#pragma aux _DPMIGetRealModeInterruptVector = \
        "mov    ax,200h"    \
        _INT_31             \
    __parm [__bl] \
    __value [__cx __dx] \
    __modify [__ax]

#pragma aux _DPMISetRealModeInterruptVector = \
        "mov    ax,201h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    __parm [__bl] [__cx __dx] \
    __value [__ax] \
    __modify []
#endif

#if defined(__386__)
#pragma aux _DPMIGetPMExceptionVector = \
        "mov    ax,202h"    \
        _INT_31             \
    __parm [__bl] \
    __value [__cx __edx] \
    __modify [__ax]

#pragma aux _DPMISetPMExceptionVector = \
        "mov    ax,203h"    \
        _INT_31             \
    __parm [__bl] [__cx __edx] \
    __value \
    __modify [__ax]

#pragma aux _DPMIGetPMInterruptVector = \
        "mov    ax,204h"    \
        _INT_31             \
    __parm [__bl] \
    __value [__cx __edx] \
    __modify [__ax]

#pragma aux _DPMISetPMInterruptVector = \
        "mov    ax,205h"    \
        _INT_31             \
        "sbb    eax,eax"    \
    __parm [__bl] [__cx __edx] \
    __value [__eax] \
    __modify []
#else
#pragma aux _DPMIGetPMExceptionVector = \
        "mov    ax,202h"    \
        _INT_31             \
    __parm [__bl] \
    __value [__cx __dx] \
    __modify [__ax]

#pragma aux _DPMISetPMExceptionVector = \
        "mov    ax,203h"    \
        _INT_31             \
    __parm [__bl] [__cx __dx] \
    __value \
    __modify [__ax]

#pragma aux _DPMIGetPMInterruptVector = \
        "mov    ax,204h"    \
        _INT_31             \
    __parm [__bl] \
    __value [__cx __dx] \
    __modify [__ax]

#pragma aux _DPMISetPMInterruptVector = \
        "mov    ax,205h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    __parm [__bl] [__cx __dx] \
    __value [__ax] \
    __modify []
#endif

#if defined(__386__)
#pragma aux _DPMIAllocateRealModeCallBackAddress = \
        "push   es"         \
        "push   ds"         \
        "mov    ds,edx"     \
        "mov    esi,eax"    \
        "mov    es,ecx"     \
        "mov    edi,ebx"    \
        "mov    ax,303h"    \
        _INT_31             \
        "pop    ds"         \
        "pop    es"         \
    __parm [__dx __eax] [__cx __ebx] \
    __value [__cx __edx] \
    __modify [__edi __esi]

#pragma aux _DPMIFreeRealModeCallBackAddress = \
        "mov    ax,304h"    \
        _INT_31             \
    __parm [__cx __edx] \
    __value \
    __modify []
#else
#pragma aux _DPMIAllocateRealModeCallBackAddress = \
        "push   ds"         \
        "mov    ds,dx"      \
        "mov    si,ax"      \
        "mov    ax,303h"    \
        _INT_31             \
        "pop    ds"         \
    __parm [__dx __ax] [__es __di] \
    __value [__cx __dx] \
    __modify [__si]

#pragma aux _DPMIFreeRealModeCallBackAddress = \
        "mov    ax,304h"    \
        _INT_31             \
    __parm [__cx __dx] \
    __value \
    __modify []
#endif

#if defined(__386__)
#pragma aux             _DPMIRawPMtoRMAddr = \
        "mov    ax,306h"    \
        "xor    edi,edi"    \
        "stc"               \
        _INT_31             \
        "mov    cx,si"      \
        "jnc short L1"      \
        "xor    cx,cx"      \
        "xor    edi,edi"    \
    "L1: "                  \
    __parm [] \
    __value [__cx __edi] \
    __modify __exact [__eax __cx __si __edi]

#pragma aux             _DPMIRawRMtoPMAddr = \
        "mov    ax,306h"    \
        "stc"               \
        _INT_31             \
        "jnc short L1"      \
        "xor    ebx,ebx"    \
        "jmp short L2"      \
    "L1: shl    ebx,16"     \
        "mov    bx,cx"      \
    "L2: "                  \
    __parm [] \
    __value [__ebx] \
    __modify __exact [__eax __cx __si __edi]

#pragma aux             _DPMISaveRMStateAddr = \
        "mov    ax,305h"    \
        "stc"               \
        _INT_31             \
        "mov    cx,si"      \
        "jnc short L1"      \
        "xor    cx,cx"      \
        "xor    edi,edi"    \
    "L1: "                  \
    __parm [] \
    __value [__cx __edi] \
    __modify __exact [__ax __bx __cx __si __edi]

#pragma aux             _DPMISavePMStateAddr = \
        "mov    ax,305h"    \
        _INT_31             \
        "jnc short L1"      \
        "xor    cx,cx"      \
        "xor    ebx,ebx"    \
        "jmp short L2"      \
    "L1: shl    ebx,16"     \
        "mov    bx,cx"      \
    "L2: "                  \
    __parm [] \
    __value [__ebx] \
    __modify __exact [__ax __bx __cx __si __edi]

#pragma aux             _DPMISaveStateSize = \
        "mov    ax,305h"    \
        _INT_31             \
        "jnc short L1"      \
        "xor    eax,eax"    \
    "L1:"                   \
    __parm [] \
    __value [__ax] \
    __modify __exact [__eax __bx __cx __si __edi]

#pragma aux             _DPMIGetVendorSpecificAPI = \
        "push   ds"         \
        "push   es"         \
        "push   fs"         \
        "push   gs"         \
        "push   ebp"        \
        "mov    ds,ecx"     \
        "xor    eax,eax"    \
        "mov    ah,0ah"     \
        _INT_31             \
        "mov    ecx,es"     \
        "jnc short L1"      \
        "xor    ecx,ecx"    \
        "xor    edi,edi"    \
    "L1: pop    ebp"        \
        "pop    gs"         \
        "pop    fs"         \
        "pop    es"         \
        "pop    ds"         \
    __parm [__cx __esi] \
    __value [__cx __edi] \
    __modify [__eax __ebx __ecx __edx __esi]
#endif

#endif
