/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DPMI API (int 31h) in-line assembly wrappers.
*
****************************************************************************/


#ifndef __DPMI_H
#define __DPMI_H

#include "watcom.h"
#include "descript.h"

/*
 * call_struct definition for DPMI SimulateRealInt
 */
typedef struct {
    union {
        uint_32 edi;
        uint_16 di;
    };
    union {
        uint_32 esi;
        uint_16 si;
    };
    union {
        uint_32 ebp;
        uint_16 bp;
    };
    uint_32     reserved;
    union {
        uint_32 ebx;
        uint_16 bx;
        uint_8  bl;
        struct {
            uint_8  :8;
            uint_8  bh;
        };
    };
    union {
        uint_32 edx;
        uint_16 dx;
        uint_8  dl;
        struct {
            uint_8  :8;
            uint_8  dh;
        };
    };
    union {
        uint_32 ecx;
        uint_16 cx;
        uint_8  cl;
        struct {
            uint_8  :8;
            uint_8  ch;
        };
    };
    union {
        uint_32 eax;
        uint_16 ax;
        uint_8  al;
        struct {
            uint_8  :8;
            uint_8  ah;
        };
    };
    uint_16     flags;
    uint_16     es;
    uint_16     ds;
    uint_16     fs;
    uint_16     gs;
    uint_16     ip;
    uint_16     cs;
    uint_16     sp;
    uint_16     ss;
} call_struct;

/*
 * rmi_struct definition for Pharlap SimulateRealInt
 */
typedef struct {
    uint_16 inum;       /* Interrupt number */
    uint_16 ds;         /* DS register */
    uint_16 es;         /* ES register */
    uint_16 fs;         /* FS register */
    uint_16 gs;         /* GS register */
    union {             /* EAX register */
        uint_32 eax;
        uint_16 ax;
        uint_8  al;
        struct {
            uint_8  :8;
            uint_8  ah;
        };
    };
    union {             /* EDX register */
        uint_32 edx;
        uint_16 dx;
        uint_8  dl;
        struct {
            uint_8  :8;
            uint_8  dh;
        };
    };
} rmi_struct;

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

typedef enum {
    DPMI_WATCH_EXEC,
    DPMI_WATCH_WRITE,
    DPMI_WATCH_READWRITE
} dpmi_watch_type;

typedef struct {
    uint_32     linear;
    uint_32     handle;
} dpmi_mem_block;

typedef struct {
    uint_16     rm;
    uint_16     pm;
} dpmi_dos_mem_block;

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

#define TinyDPMISetRealVect                     _TinyDPMISetRealVect
#define TinyDPMIGetRealVect                     _TinyDPMIGetRealVect
#define TinyDPMISetProtectVect                  _TinyDPMISetProtectVect
#define TinyDPMIGetProtectVect                  _TinyDPMIGetProtectVect
#define TinyDPMISetProtectExcpt                 _TinyDPMISetProtectExcpt
#define TinyDPMIGetProtectExcpt                 _TinyDPMIGetProtectExcpt
#define TinyDPMIRawRMtoPMAddr                   _TinyDPMIRawRMtoPMAddr
#define TinyDPMIRawPMtoRMAddr                   _TinyDPMIRawPMtoRMAddr
#define TinyDPMISaveRMStateAddr                 _TinyDPMISaveRMStateAddr
#define TinyDPMISavePMStateAddr                 _TinyDPMISavePMStateAddr
#define TinyDPMISaveStateSize                   _TinyDPMISaveStateSize
#define TinyDPMIBase                            _TinyDPMIBase
#define TinyDPMISetBase                         _TinyDPMISetBase
#define TinyDPMISegToSel                        _TinyDPMISegToSel
#define TinyDPMICreateSel                       _TinyDPMICreateSel
#define TinyDPMICreateCSAlias                   _TinyDPMICreateCSAlias
#define TinyDPMIFreeSel                         _TinyDPMIFreeSel
#define TinyDPMISetLimit                        _TinyDPMISetLimit
#define TinyDPMISetRights                       _TinyDPMISetRights
#define TinyDPMIGetDescriptor                   _TinyDPMIGetDescriptor
#define TinyDPMISetDescriptor                   _TinyDPMISetDescriptor

#if defined( _M_I86SM ) || defined( _M_I86MM ) || !defined( _M_I86 )

#if defined( _M_I86 )
#define DPMIGetVersion                          _nDPMIGetVersion
#else
#define DPMIGetVersion                          _DPMIGetVersion
#endif
#define DPMIAllocateMemoryBlock                 _nDPMIAllocateMemoryBlock
#define DPMIGetFreeMemoryInformation            _nDPMIGetFreeMemoryInformation
#define DPMIResizeMemoryBlock                   _nDPMIResizeMemoryBlock

#define TinyDPMIAlloc(x)                        _TinyDPMIAlloc((x) >> 16, (x))
#define TinyDPMIRealloc(addr,x)                 _TinyDPMIRealloc(addr, (x) >> 16, (x))
#define TinyDPMIFree(x)                         _TinyDPMIFree((x) >> 16, (x))
#define TinyDPMIDOSAlloc                        _TinyDPMIDOSAlloc
#define TinyDPMIDOSFree                         _TinyDPMIDOSFree
#define TinyDPMISimulateRealInt                 _TinyDPMISimulateRealInt
#define TinyDPMICallRealIntFrame                _TinyDPMICallRealIntFrame
#define TinyDPMICallRealFarFrame                _TinyDPMICallRealFarFrame

#else

#define DPMIGetVersion                          _fDPMIGetVersion
#define DPMIAllocateMemoryBlock                 _fDPMIAllocateMemoryBlock
#define DPMIGetFreeMemoryInformation            _fDPMIGetFreeMemoryInformation
#define DPMIResizeMemoryBlock                   _fDPMIResizeMemoryBlock

#endif

/*
 * C run-time library flag indicating that DPMI services (host) is available
 */
extern unsigned char _DPMI;

extern void     _DPMIFreeRealModeCallBackAddress( void __far * proc );
extern void     __far *_DPMIAllocateRealModeCallBackAddress( void __far * proc, call_struct __far *cs );
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
extern int_32   _DPMISegmentToDescriptor( uint_16 );
extern int      _DPMIFreeLDTDescriptor( uint_16 );
extern uint_16  _DPMIGetNextSelectorIncrementValue( void );
extern uint_32  _DPMIGetSegmentBaseAddress( uint_16 );
extern int      _DPMISetSegmentBaseAddress( uint_16, uint_32 );
extern int      _DPMISetSegmentLimit( uint_16, uint_32 );
extern int      _DPMISetDescriptorAccessRights( uint_16, uint_16 );
extern int_16   _fDPMIAllocateMemoryBlock( dpmi_mem_block __far *, uint_32 );
extern int_16   _nDPMIAllocateMemoryBlock( dpmi_mem_block *, uint_32 );
extern int_16   _fDPMIResizeMemoryBlock( dpmi_mem_block __far *, uint_32, uint_32 );
extern int_16   _nDPMIResizeMemoryBlock( dpmi_mem_block *, uint_32, uint_32 );
extern int      _DPMIFreeMemoryBlock( uint_32 );
extern int      _DPMILockLinearRegion( uint_32, uint_32 );
extern int      _DPMIUnlockLinearRegion( uint_32, uint_32 );
extern int      _DPMIGetDescriptor( uint_16, descriptor __far * );
extern int      _DPMISetDescriptor( uint_16, descriptor __far * );
extern int_32   _DPMICreateCodeSegmentAliasDescriptor( uint_16 );
extern int      _nDPMIGetFreeMemoryInformation( dpmi_mem * );
extern int      _fDPMIGetFreeMemoryInformation( dpmi_mem __far * );
extern int      _DPMISimulateRealModeInterrupt( uint_8 interrupt, uint_8 flags,
                        uint_16 words_to_copy, call_struct __far *call_st );
extern dpmi_dos_mem_block _DPMIAllocateDOSMemoryBlock( uint_16 para );
extern int      _DPMIFreeDOSMemoryBlock( uint_16 sel );
extern void     __far *_DPMIRawPMtoRMAddr( void );
extern uint_32  _DPMIRawRMtoPMAddr( void );
extern void     __far *_DPMISaveRMStateAddr( void );
extern uint_32  _DPMISavePMStateAddr( void );
extern uint_16  _DPMISaveStateSize( void );
extern void     __far *_DPMIGetVendorSpecificAPI( char __far * );

extern int_32   _DPMISetWatch( uint_32 linear, uint_8 len, uint_8 type );
extern int      _DPMIClearWatch( uint_16 handle );
extern int      _DPMITestWatch( uint_16 handle );
extern int      _DPMIResetWatch( uint_16 handle );

void *          _TinyDPMIAlloc( uint_16 __hiw, uint_16 __low );
void *          _TinyDPMIRealloc( void *__addr, uint_16 __hiw, uint_16 __low );
void            _TinyDPMIFree( uint_16 __hiw, uint_16 __low );
uint_32         _TinyDPMIDOSAlloc( uint_16 __paras );
void            _TinyDPMIDOSFree( uint_16 __sel );
uint_32         _TinyDPMIBase( uint_16 __sel );
void __far *    _TinyDPMIGetProtectVect( uint_8 __intr );
uint_32         _TinyDPMISetProtectVect( uint_8 __intr, void ( __far __interrupt *__f )() );
void __far *    _TinyDPMIGetProtectExcpt( uint_8 __intr );
uint_32         _TinyDPMISetProtectExcpt( uint_8 __intr, void ( __far __interrupt *__f )() );
uint_32         _TinyDPMIGetRealVect( uint_8 __intr );
uint_32         _TinyDPMISetRealVect( uint_8 __intr, uint_16 __seg, uint_16 __offs );
uint_32         _TinyDPMISimulateRealInt( uint_8 __intr, uint_8 __flags, uint_16 __copy, call_struct __far *__struct );
uint_32         _TinyDPMICallRealIntFrame( uint_8 __flags, uint_16 __copy, call_struct __far *__struct );
uint_32         _TinyDPMICallRealFarFrame( uint_8 __flags, uint_16 __copy, call_struct __far *__struct );
void __far  *   _TinyDPMIRawPMtoRMAddr( void );
uint_32         _TinyDPMIRawRMtoPMAddr( void );
void __far  *   _TinyDPMISaveRMStateAddr( void );
uint_32         _TinyDPMISavePMStateAddr( void );
uint_16         _TinyDPMISaveStateSize( void );
uint_16         _TinyDPMICreateCSAlias( uint_16 __sel );
uint_32         _TinyDPMIFreeSel( uint_16 __sel );
uint_16         _TinyDPMISegToSel( uint_16 __sel );
uint_16         _TinyDPMICreateSel( uint_16 __nbsels );
uint_32         _TinyDPMISetBase( uint_16 __sel, uint_32 );
uint_32         _TinyDPMISetLimit( uint_16 __sel, uint_32 );
uint_32         _TinyDPMISetRights( uint_16 __sel, uint_16 );
uint_32         _TinyDPMIGetDescriptor( uint_16 __sel, void __far * );
uint_32         _TinyDPMISetDescriptor( uint_16 __sel, void __far * );

#include "asmbytes.h"

#define MULTIPLEX_1680  0x80 0x16
#define MULTIPLEX_1686  0x86 0x16

#define DPMI_0001       0x01 0x00
#define DPMI_0002       0x02 0x00
#define DPMI_0003       0x03 0x00
#define DPMI_0006       0x06 0x00
#define DPMI_0007       0x07 0x00
#define DPMI_0008       0x08 0x00
#define DPMI_0009       0x09 0x00
#define DPMI_000A       0x0A 0x00
#define DPMI_000B       0x0B 0x00
#define DPMI_000C       0x0C 0x00
#define DPMI_0100       0x00 0x01
#define DPMI_0101       0x01 0x01
#define DPMI_0200       0x00 0x02
#define DPMI_0201       0x01 0x02
#define DPMI_0202       0x02 0x02
#define DPMI_0203       0x03 0x02
#define DPMI_0204       0x04 0x02
#define DPMI_0205       0x05 0x02
#define DPMI_0300       0x00 0x03
#define DPMI_0301       0x01 0x03
#define DPMI_0302       0x02 0x03
#define DPMI_0303       0x03 0x03
#define DPMI_0304       0x04 0x03
#define DPMI_0305       0x05 0x03
#define DPMI_0306       0x06 0x03
#define DPMI_0400       0x00 0x04
#define DPMI_0500       0x00 0x05
#define DPMI_0501       0x01 0x05
#define DPMI_0502       0x02 0x05
#define DPMI_0503       0x03 0x05
#define DPMI_0600       0x00 0x06
#define DPMI_0601       0x01 0x06
#define DPMI_0A00       0x00 0x0A
#define DPMI_0B00       0x00 0x0B
#define DPMI_0B01       0x01 0x0B
#define DPMI_0B02       0x02 0x0B
#define DPMI_0B03       0x03 0x0B


#pragma aux _DPMIIdle = \
        _MOV_AX_W MULTIPLEX_1680 \
        _INT_2F         \
    __parm [] \
    __value \
    __modify __exact [__ax]

#pragma aux _DPMIModeDetect = \
        _MOV_AX_W MULTIPLEX_1686 \
        _INT_2F         \
    __parm [] \
    __value [__ax] \
    __modify __exact [__ax]

#if defined(__386__)
#pragma aux _DPMISetWatch = \
        _MOV_CX_BX      \
        _SHR_EBX_N 16   \
        _MOV_AX_W DPMI_0B00 \
        _INT_31         \
        _SBB_AX_AX      \
        _USE16 _MOV_AX_BX \
    __parm [__ebx] [__dl] [__dh] \
    __value [__eax] \
    __modify __exact [__eax __ebx __ecx]
#else
#pragma aux _DPMISetWatch = \
        _XCHG_BX_CX     \
        _MOV_AX_W DPMI_0B00 \
        _INT_31         \
        _SBB_CX_CX      \
    __parm [__bx __cx] [__dl] [__dh] \
    __value [__cx __bx] \
    __modify __exact [__ax __bx __cx]
#endif

#if defined(__386__)
#pragma aux _DPMIClearWatch = \
        _MOV_AX_W DPMI_0B01 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] \
    __value [__eax] \
    __modify __exact [__eax]
#else
#pragma aux _DPMIClearWatch = \
        _MOV_AX_W DPMI_0B01 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] \
    __value [__ax] \
    __modify __exact [__ax]
#endif

#if defined(__386__)
#pragma aux _DPMITestWatch = \
        _MOV_AX_W DPMI_0B02 \
        _INT_31         \
        _SBB_BX_BX      \
        _AND_AX_B 1     \
        _OR_BX_AX       \
    __parm [__bx] \
    __value [__ebx] \
    __modify __exact [__eax __ebx]
#else
#pragma aux _DPMITestWatch = \
        _MOV_AX_W DPMI_0B02 \
        _INT_31         \
        _SBB_BX_BX      \
        _AND_AX_B 1     \
        _OR_BX_AX       \
    __parm [__bx] \
    __value [__bx] \
    __modify __exact [__ax __bx]
#endif

#if defined(__386__)
#pragma aux _DPMIResetWatch = \
        _MOV_AX_W DPMI_0B03 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] \
    __value [__eax] \
    __modify __exact [__eax]
#else
#pragma aux _DPMIResetWatch = \
        _MOV_AX_W DPMI_0B03 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] \
    __value [__ax] \
    __modify __exact [__ax]
#endif

#if defined(__386__)
#pragma aux _DPMIGetVersion = \
        _PUSH_DS        \
        _MOV_DS_DX      \
        _MOV_AX_W DPMI_0400 \
        _INT_31         \
        "mov  byte ptr [esi],ah"    \
        "mov  byte ptr [esi+1],al"  \
        "mov  word ptr [esi+2],bx"  \
        "mov  byte ptr [esi+4],cl"  \
        "mov  byte ptr [esi+5],dh"  \
        "mov  byte ptr [esi+6],dl"  \
        _POP_DS         \
    __parm [__dx __esi] \
    __value \
    __modify[__ax __bx __cl __dx]
#else
#pragma aux _nDPMIGetVersion = \
        _MOV_AX_W DPMI_0400 \
        _INT_31         \
        "mov  byte ptr [si],ah"     \
        "mov  byte ptr [si+1],al"   \
        "mov  word ptr [si+2],bx"   \
        "mov  byte ptr [si+4],cl"   \
        "mov  byte ptr [si+5],dh"   \
        "mov  byte ptr [si+6],dl"   \
    __parm [__si] \
    __value \
    __modify[__ax __bx __cl __dx]

#pragma aux _fDPMIGetVersion = \
        _MOV_AX_W DPMI_0400 \
        _INT_31         \
        "mov  byte ptr es:[si],ah"      \
        "mov  byte ptr es:[si+1],al"    \
        "mov  word ptr es:[si+2],bx"    \
        "mov  byte ptr es:[si+4],cl"    \
        "mov  byte ptr es:[si+5],dh"    \
        "mov  byte ptr es:[si+6],dl"    \
    __parm [__es __si]  \
    __value \
    __modify[__ax __bx __cl __dx]
#endif

#if defined(__386__)
#pragma aux _DPMIAllocateLDTDescriptors = \
        _XOR_AX_AX      \
        _INT_31         \
        _SBB_CX_CX      \
        _USE16 _MOV_CX_AX \
    __parm [__cx] \
    __value [__ecx] \
    __modify __exact [__eax __ecx]
#else
#pragma aux _DPMIAllocateLDTDescriptors = \
        _XOR_AX_AX      \
        _INT_31         \
        _SBB_CX_CX      \
    __parm [__cx] \
    __value [__cx __ax] \
    __modify __exact [__ax __cx]
#endif

#if defined(__386__)
#pragma aux _DPMIFreeLDTDescriptor = \
        _MOV_AX_W DPMI_0001 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] \
    __value [__eax] \
    __modify __exact [__eax]
#else
#pragma aux _DPMIFreeLDTDescriptor = \
        _MOV_AX_W DPMI_0001 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] \
    __value [__ax] \
    __modify __exact [__ax]
#endif

#if defined(__386__)
#pragma aux _DPMISegmentToDescriptor = \
        _MOV_AX_W DPMI_0002 \
        _INT_31         \
        _SBB_BX_BX      \
        _USE16 _MOV_BX_AX \
    __parm [__bx] \
    __value [__ebx]\
    __modify __exact [__ax __ebx]
#else
#pragma aux _DPMISegmentToDescriptor = \
        _MOV_AX_W DPMI_0002 \
        _INT_31         \
        _SBB_BX_BX      \
    __parm [__bx] \
    __value [__bx __ax] \
    __modify __exact [__ax __bx]
#endif

#pragma aux _DPMIGetNextSelectorIncrementValue = \
        _MOV_AX_W DPMI_0003 \
        _INT_31         \
    __parm [__bx] \
    __value [__ax] \
    __modify __exact [__ax]

#if defined(__386__)
#pragma aux _DPMIGetSegmentBaseAddress = \
        _MOV_AX_W DPMI_0006 \
        _INT_31         \
        _SHL_ECX_N 16   \
        _USE16 _MOV_CX_DX \
    __parm __caller [__bx] \
    __value [__ecx] \
    __modify __exact [__ax __ecx __edx]
#else
#pragma aux _DPMIGetSegmentBaseAddress = \
        _MOV_AX_W DPMI_0006 \
        _INT_31         \
    __parm [__bx] \
    __value[__cx __dx] \
    __modify __exact [__ax __cx __dx]
#endif

#if defined(__386__)
#pragma aux _DPMISetSegmentBaseAddress = \
        _MOV_DX_CX      \
        _SHR_ECX_N 16   \
        _MOV_AX_W DPMI_0007 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] [__ecx] \
    __value [__eax] \
    __modify __exact [__eax __ecx __dx]
#else
#pragma aux _DPMISetSegmentBaseAddress = \
        _MOV_AX_W DPMI_0007 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] [__cx __dx] \
    __value [__ax] \
    __modify __exact [__ax]
#endif

#if defined(__386__)
#pragma aux _DPMISetSegmentLimit = \
        _MOV_DX_CX      \
        _SHR_ECX_N 16   \
        _MOV_AX_W DPMI_0008 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] [__ecx] \
    __value [__eax] \
    __modify __exact [__eax __ecx __dx]
#else
#pragma aux _DPMISetSegmentLimit = \
        _MOV_AX_W DPMI_0008 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] [__cx __dx] \
    __value [__ax] \
    __modify __exact [__ax]
#endif

#if defined(__386__)
#pragma aux _DPMISetDescriptorAccessRights = \
        _MOV_AX_W DPMI_0009 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] [__cx] \
    __value [__eax] \
    __modify __exact [__eax]
#else
#pragma aux _DPMISetDescriptorAccessRights = \
        _MOV_AX_W DPMI_0009 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] [__cx] \
    __value [__ax] \
    __modify __exact [__ax]
#endif

#if defined(__386__)
#pragma aux _DPMIFreeMemoryBlock =  \
        _MOV_DI_SI      \
        _SHR_ESI_N 16   \
        _MOV_AX_W DPMI_0502 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__esi] \
    __value [__eax] \
    __modify __exact [__eax __di __esi]
#else
#pragma aux _DPMIFreeMemoryBlock =  \
        _XCHG_SI_DI     \
        _MOV_AX_W DPMI_0502 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__di __si] \
    __value [__ax] \
    __modify __exact [__ax __di __si]
#endif

#if defined(__386__)
#pragma aux _DPMILockLinearRegion = \
        _MOV_DI_SI      \
        _SHR_ESI_N 16   \
        _MOV_CX_BX      \
        _SHR_EBX_N 16   \
        _MOV_AX_W DPMI_0600 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__ebx] [__esi] \
    __value [__eax] \
    __modify __exact [__eax __ebx __ecx __esi __di]

#pragma aux _DPMIUnlockLinearRegion = \
        _MOV_DI_SI      \
        _SHR_ESI_N 16   \
        _MOV_CX_BX      \
        _SHR_EBX_N 16   \
        _MOV_AX_W DPMI_0601 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__ebx] [__esi] \
    __value [__eax] \
    __modify __exact [__eax __ebx __ecx __esi __di]
#else
#pragma aux _DPMILockLinearRegion = \
        _MOV_AX_W DPMI_0600 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__cx __bx] [__si __di] \
    __value[__ax] \
    __modify __exact [__ax]

#pragma aux _DPMIUnlockLinearRegion = \
        _MOV_AX_W DPMI_0601 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__cx __bx] [__si __di] \
    __value [__ax] \
    __modify __exact [__ax]
#endif

#if defined(__386__)

#pragma aux _DPMIAllocateDOSMemoryBlock = \
        _MOV_AX_W DPMI_0100 \
        _INT_31         \
        "jnc short L1"  \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
    "L1:"               \
        _SHL_EDX_N 16   \
        _USE16 _MOV_DX_AX \
    __parm [__bx] \
    __value [__edx] \
    __modify [__eax __bx __edx]

#pragma aux _DPMIFreeDOSMemoryBlock = \
        _MOV_AX_W DPMI_0101 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__dx] \
    __value [__eax] \
    __modify __exact [__eax]

#pragma aux _DPMISimulateRealModeInterrupt = \
        _PUSH_ES        \
        _MOV_ES_DX      \
        _MOV_AX_W DPMI_0300 \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm [__bl] [__bh] [__cx] [__dx __edi] \
    __value [__eax] \
    __modify []

#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
        _MOV_AX_W DPMI_000A \
        _INT_31         \
        _SBB_BX_BX      \
        _USE16 _MOV_BX_AX \
    __parm [__bx] \
    __value [__ebx] \
    __modify __exact [__eax __ebx]

#pragma aux _DPMIGetDescriptor = \
        _PUSH_ES        \
        _MOV_ES_DX      \
        _MOV_AX_W DPMI_000B \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm [__bx] [__dx __edi] \
    __value [__eax] \
    __modify []

#pragma aux _DPMISetDescriptor = \
        _PUSH_ES        \
        _MOV_ES_DX      \
        _MOV_AX_W DPMI_000C \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm [__bx] [__dx __edi] \
    __value [__eax] \
    __modify []

#else

#pragma aux _DPMIAllocateDOSMemoryBlock = \
        _MOV_AX_W DPMI_0100 \
        _INT_31         \
        "jnc short L1"  \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
    "L1: "              \
    __parm [__bx] \
    __value [__dx __ax] \
    __modify __exact [__ax __bx __dx]

#pragma aux _DPMIFreeDOSMemoryBlock = \
        _MOV_AX_W DPMI_0101 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__dx] \
    __value [__ax] \
    __modify __exact [__ax]

#pragma aux _DPMISimulateRealModeInterrupt = \
        _MOV_AX_W DPMI_0300 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bl] [__bh] [__cx] [__es __di] \
    __value [__ax] \
    __modify []

#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
        _MOV_AX_W DPMI_000A \
        _INT_31         \
        _SBB_BX_BX      \
    __parm [__bx] \
    __value [__bx __ax] \
    __modify __exact[__ax __bx]

#pragma aux _DPMIGetDescriptor = \
        _MOV_AX_W DPMI_000B \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] [__es __di] \
    __value [__ax] \
    __modify []

#pragma aux _DPMISetDescriptor = \
        _MOV_AX_W DPMI_000C \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bx] [__es __di] \
    __value [__ax] \
    __modify []

#endif

#pragma aux _fDPMIAllocateMemoryBlock =  \
        _PUSH_ES        \
        _PUSH_AX        \
        _PUSH_DX        \
        _XCHG_BX_CX     \
        _MOV_AX_W DPMI_0501 \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_BX      \
        _POP_ES         \
        _POP_BX         \
        "mov  es:[bx],cx"       \
        "mov  es:+2H[bx],dx"    \
        "mov  es:+4H[bx],di"    \
        "mov  es:+6H[bx],si"    \
        _POP_ES         \
    __parm [__ax __dx] [__bx __cx] \
    __value [__ax] \
    __modify [__bx __cx __dx __di __si]

#pragma aux _nDPMIAllocateMemoryBlock =  \
        _PUSH_AX        \
        _XCHG_BX_CX     \
        _MOV_AX_W DPMI_0501 \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_BX      \
        _POP_BX         \
        "mov  ds:[bx],cx"       \
        "mov  ds:+2H[bx],dx"    \
        "mov  ds:+4H[bx],di"    \
        "mov  ds:+6H[bx],si"    \
    __parm [__ax] [__bx __cx] \
    __value [__ax] \
    __modify [__bx __cx __dx __di __si]

#pragma aux _fDPMIResizeMemoryBlock =  \
        _PUSH_ES        \
        _PUSH_AX        \
        _PUSH_DX        \
        _XCHG_SI_DI     \
        _XCHG_BX_CX     \
        _MOV_AX_W DPMI_0503 \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_BX      \
        _POP_ES         \
        _POP_BX         \
        "mov  es:[bx],cx"       \
        "mov  es:+2H[bx],dx"    \
        "mov  es:+4H[bx],di"    \
        "mov  es:+6H[bx],si"    \
        _POP_ES         \
    __parm [__dx __ax] [__bx __cx] [__di __si] \
    __value [__ax] \
    __modify [__di __si __bx __cx __dx]

#pragma aux _nDPMIResizeMemoryBlock =  \
        _PUSH_AX        \
        _XCHG_SI_DI     \
        _XCHG_BX_CX     \
        _MOV_AX_W DPMI_0503 \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_BX      \
        _POP_BX         \
        "mov  ds:[bx],cx"       \
        "mov  ds:+2H[bx],dx"    \
        "mov  ds:+4H[bx],di"    \
        "mov  ds:+6H[bx],si"    \
    __parm [__ax] [__bx __cx] [__di __si] \
    __value [__ax] \
    __modify [__di __si __bx __cx __dx]

#if defined(__386__)
#pragma aux _nDPMIGetFreeMemoryInformation = \
        _PUSH_ES        \
        _PUSH_DS        \
        _POP_ES         \
        _MOV_AX_W DPMI_0500 \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm [__edi] \
    __value [__eax] \
    __modify __exact [__eax]
#else
#pragma aux _fDPMIGetFreeMemoryInformation = \
        _MOV_AX_W DPMI_0500 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__es __di] \
    __value [__ax] \
    __modify __exact [__ax]

#pragma aux _nDPMIGetFreeMemoryInformation = \
        _PUSH_ES        \
        _PUSH_DS        \
        _POP_ES         \
        _MOV_AX_W DPMI_0500 \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm [__di] \
    __value [__ax] \
    __modify __exact [__ax]
#endif

#if defined(__386__)
#pragma aux _DPMIGetRealModeInterruptVector = \
        _MOV_AX_W DPMI_0200 \
        _INT_31         \
    __parm [__bl] \
    __value [__cx __edx] \
    __modify [__ax]

#pragma aux _DPMISetRealModeInterruptVector = \
        _MOV_AX_W DPMI_0201 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bl] [__cx __edx] \
    __value [__eax] \
    __modify []
#else
#pragma aux _DPMIGetRealModeInterruptVector = \
        _MOV_AX_W DPMI_0200 \
        _INT_31         \
    __parm [__bl] \
    __value [__cx __dx] \
    __modify [__ax]

#pragma aux _DPMISetRealModeInterruptVector = \
        _MOV_AX_W DPMI_0201 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bl] [__cx __dx] \
    __value [__ax] \
    __modify []
#endif

#if defined(__386__)
#pragma aux _DPMIGetPMExceptionVector = \
        _MOV_AX_W DPMI_0202 \
        _INT_31         \
    __parm [__bl] \
    __value [__cx __edx] \
    __modify [__ax]

#pragma aux _DPMISetPMExceptionVector = \
        _MOV_AX_W DPMI_0203 \
        _INT_31         \
    __parm [__bl] [__cx __edx] \
    __value \
    __modify [__ax]

#pragma aux _DPMIGetPMInterruptVector = \
        _MOV_AX_W DPMI_0204 \
        _INT_31         \
    __parm [__bl] \
    __value [__cx __edx] \
    __modify [__ax]

#pragma aux _DPMISetPMInterruptVector = \
        _MOV_AX_W DPMI_0205 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bl] [__cx __edx] \
    __value [__eax] \
    __modify []
#else
#pragma aux _DPMIGetPMExceptionVector = \
        _MOV_AX_W DPMI_0202 \
        _INT_31         \
    __parm [__bl] \
    __value [__cx __dx] \
    __modify [__ax]

#pragma aux _DPMISetPMExceptionVector = \
        _MOV_AX_W DPMI_0203 \
        _INT_31         \
    __parm [__bl] [__cx __dx] \
    __value \
    __modify [__ax]

#pragma aux _DPMIGetPMInterruptVector = \
        _MOV_AX_W DPMI_0204 \
        _INT_31         \
    __parm [__bl] \
    __value [__cx __dx] \
    __modify [__ax]

#pragma aux _DPMISetPMInterruptVector = \
        _MOV_AX_W DPMI_0205 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm [__bl] [__cx __dx] \
    __value [__ax] \
    __modify []
#endif

#if defined(__386__)
#pragma aux _DPMIAllocateRealModeCallBackAddress = \
        _PUSH_ES        \
        _PUSH_DS        \
        _MOV_DS_DX      \
        _MOV_SI_AX      \
        _MOV_ES_CX      \
        _MOV_DI_BX      \
        _MOV_AX_W DPMI_0303 \
        _INT_31         \
        _POP_DS         \
        _POP_ES         \
    __parm [__dx __eax] [__cx __ebx] \
    __value [__cx __edx] \
    __modify [__edi __esi]

#pragma aux _DPMIFreeRealModeCallBackAddress = \
        _MOV_AX_W DPMI_0304 \
        _INT_31         \
    __parm [__cx __edx] \
    __value \
    __modify []
#else
#pragma aux _DPMIAllocateRealModeCallBackAddress = \
        _PUSH_DS        \
        _MOV_DS_DX      \
        _MOV_SI_AX      \
        _MOV_AX_W DPMI_0303 \
        _INT_31         \
        _POP_DS         \
    __parm [__dx __ax] [__es __di] \
    __value [__cx __dx] \
    __modify [__si]

#pragma aux _DPMIFreeRealModeCallBackAddress = \
        _MOV_AX_W DPMI_0304 \
        _INT_31         \
    __parm [__cx __dx] \
    __value \
    __modify []
#endif


#if !defined(_M_I86)

/***************************
 * 80386 versions of pragmas
 ***************************/

#pragma aux _DPMIRawPMtoRMAddr = \
        _MOV_AX_W DPMI_0306 \
        _XOR_DI_DI      \
        _STC            \
        _INT_31         \
        _MOV_CX_SI      \
        "jnc short L1"  \
        _XOR_CX_CX      \
        _XOR_DI_DI      \
    "L1: "              \
    __parm [] \
    __value [__cx __edi] \
    __modify __exact [__eax __ecx __si __edi]

#pragma aux _DPMIRawRMtoPMAddr = \
        _MOV_AX_W DPMI_0306 \
        _STC            \
        _INT_31         \
        "jnc short L1"  \
        _XOR_BX_BX      \
        "jmp short L2"  \
    "L1:"               \
        _SHL_EBX_N 16   \
        _USE16 _MOV_BX_CX \
    "L2: "              \
    __parm [] \
    __value [__ebx] \
    __modify __exact [__eax __cx __si __edi]

#pragma aux _DPMISaveRMStateAddr = \
        _MOV_AX_W DPMI_0305 \
        _STC            \
        _INT_31         \
        _MOV_CX_SI      \
        "jnc short L1"  \
        _XOR_CX_CX      \
        _XOR_DI_DI      \
    "L1: "              \
    __parm [] \
    __value [__cx __edi] \
    __modify __exact [__ax __bx __ecx __si __edi]

#pragma aux _DPMISavePMStateAddr = \
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        "jnc short L1"  \
        _XOR_CX_CX      \
        _XOR_BX_BX      \
        "jmp short L2"  \
    "L1:"               \
        _SHL_EBX_N 16   \
        _USE16 _MOV_BX_CX \
    "L2: "              \
    __parm [] \
    __value [__ebx] \
    __modify __exact [__ax __ebx __ecx __si __edi]

#pragma aux _DPMISaveStateSize = \
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        "jnc short L1"  \
        _XOR_AX_AX      \
    "L1:"               \
    __parm [] \
    __value [__ax] \
    __modify __exact [__eax __bx __cx __si __edi]

#pragma aux _DPMIGetVendorSpecificAPI = \
        _PUSH_DS        \
        _PUSH_ES        \
        _PUSH_FS        \
        _PUSH_GS        \
        _PUSH_BP        \
        _MOV_DS_CX      \
        _MOV_AX_W DPMI_0A00 \
        _INT_31         \
        _MOV_CX_ES      \
        "jnc short L1"  \
        _XOR_CX_CX      \
        _XOR_DI_DI      \
    "L1: "              \
        _POP_BP         \
        _POP_GS         \
        _POP_FS         \
        _POP_ES         \
        _POP_DS         \
    __parm [__cx __esi] \
    __value [__cx __edi] \
    __modify [__eax __ebx __ecx __edx __esi]
#endif


#if defined(_M_I86)

/***************************
 * 8086 versions of pragmas
 ***************************/

#pragma aux _TinyDPMIGetRealVect = \
        _MOV_AX_W DPMI_0200 \
        _INT_31         \
    __parm __caller     [__bl] \
    __value             [__cx __dx] \
    __modify __exact    [__ax __bx __cx __dx]

#pragma aux _TinyDPMISetRealVect = \
        _MOV_AX_W DPMI_0201 \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_AX      \
    __parm __caller     [__bl] [__cx] [__dx] \
    __value             [__ax __dx] \
    __modify __exact    [__ax __bx __cx __dx]

#pragma aux _TinyDPMIGetProtectVect = \
        _MOV_AX_W DPMI_0204 \
        _INT_31         \
        "jnc short finish" \
        _XOR_CX_CX      \
        _XOR_DX_DX      \
    "finish:"           \
    __parm __caller     [__bl] \
    __value             [__cx __dx] \
    __modify __exact    [__ax __bx __cx __dx]

#pragma aux _TinyDPMISetProtectVect = \
        _MOV_AX_W DPMI_0205 \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_AX      \
    __parm __caller     [__bl] [__cx __dx] \
    __value             [__ax __dx] \
    __modify __exact    [__ax __bx __cx __dx]

#pragma aux _TinyDPMIRawPMtoRMAddr = \
        _MOV_AX_W DPMI_0306 \
        _INT_31         \
        _MOV_CX_SI      \
        "jnc short finish" \
        _XOR_CX_CX      \
        _XOR_DI_DI      \
    "finish:"           \
    __parm __caller     [] \
    __value             [__cx __di] \
    __modify __exact    [__ax __bx __cx __si __di]

#pragma aux _TinyDPMIRawRMtoPMAddr = \
        _MOV_AX_W DPMI_0306 \
        _INT_31         \
        "jnc short finish" \
        _XOR_BX_BX      \
        _XOR_CX_CX      \
        _XCHG_BX_CX     \
    "finish:"           \
    __parm __caller     [] \
    __value             [__bx __cx] \
    __modify __exact    [__ax __bx __cx __si __di]

#pragma aux _TinyDPMISaveRMStateAddr = \
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        "jnc short L1"  \
        _XOR_CX_CX      \
        _XOR_DI_DI      \
        "jmp short finish" \
    "L1:"               \
        _MOV_CX_SI      \
    "finish:"           \
    __parm __caller     [] \
    __value             [__cx __di] \
    __modify __exact    [__ax __bx __cx __si __di]

#pragma aux _TinyDPMISavePMStateAddr = \
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        "jnc short finish" \
        _XOR_CX_CX      \
        _XOR_BX_BX      \
    "finish:"           \
        _XCHG_BX_CX     \
    __parm __caller     [] \
    __value             [__bx __cx] \
    __modify __exact    [__ax __bx __cx __si __di]

#pragma aux _TinyDPMISaveStateSize = \
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
    "finish:"           \
    __parm __caller     [] \
    __value             [__ax] \
    __modify __exact    [__ax __bx __cx __si __di]

#pragma aux _TinyDPMICreateCSAlias = \
        _MOV_AX_W DPMI_000A \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
    "finish:"           \
    __parm __caller     [__bx] \
    __value             [__ax] \
    __modify __exact    [__ax]

#pragma aux _TinyDPMIFreeSel = \
        _MOV_AX_W DPMI_000A \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_AX      \
    __parm __caller     [__bx] \
    __value             [__ax __dx] \
    __modify __exact    [__ax]

#pragma aux _TinyDPMIBase = \
        _MOV_AX_W DPMI_0006 \
        _INT_31         \
        "jnc short finish" \
        _XOR_DX_DX      \
        _DEC_DX         \
        _MOV_CX_DX      \
    "finish:"           \
    __parm __caller     [__bx] \
    __value             [__cx __dx] \
    __modify __exact    [__ax __bx __cx __dx]

#pragma aux _TinyDPMISegToSel = \
        _MOV_AX_W DPMI_0002 \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
    "finish:"           \
    __parm              [__bx] \
    __value             [__ax] \
    __modify __exact    [__ax __bx]

#pragma aux _TinyDPMICreateSel = \
        _XOR_AX_AX      \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
    "finish:"           \
    __parm              [__cx] \
    __value             [__ax] \
    __modify __exact    [__ax]

#pragma aux _TinyDPMISetBase = \
        _MOV_AX_W DPMI_0007 \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_AX      \
    __parm              [__bx] [__cx __dx] \
    __value             [__ax __dx] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyDPMISetLimit = \
        _MOV_AX_W DPMI_0008 \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_AX      \
    __parm              [__bx] [__cx __dx] \
    __value             [__ax __dx] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyDPMISetRights = \
        _MOV_AX_W DPMI_0009 \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_AX      \
    __parm              [__bx] [__cx] \
    __value             [__ax __dx] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyDPMIGetDescriptor = \
        _PUSH_ES        \
        _MOV_ES_CX      \
        _MOV_AX_W DPMI_000B \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_AX      \
        _POP_ES         \
    __parm              [__bx] [__cx __di] \
    __value             [__ax __dx] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyDPMISetDescriptor = \
        _PUSH_ES        \
        _MOV_ES_CX      \
        _MOV_AX_W DPMI_000C \
        _INT_31         \
        _SBB_AX_AX      \
        _MOV_DX_AX      \
        _POP_ES         \
    __parm              [__bx] [__cx __di] \
    __value             [__ax __dx] \
    __modify __exact    [__ax __dx]

#else

/***************************
 * 80386 versions of pragmas
 ***************************/

#pragma aux _TinyDPMISegToSel = \
        _MOV_AX_W DPMI_0002 \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
    "finish:"           \
    __parm              [__bx] \
    __value             [__ax] \
    __modify __exact    [__ax __bx]

#pragma aux _TinyDPMICreateSel = \
        _XOR_AX_AX      \
        _INT_31         \
        "jnc short L1"  \
        _XOR_AX_AX      \
    "L1:"               \
    __parm              [__cx] \
    __value             [__ax] \
    __modify __exact    [__eax]

#pragma aux _TinyDPMISetBase = \
        _MOV_CX_DX      \
        _SHR_ECX_N 16   \
        _MOV_AX_W DPMI_0007 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm              [__bx] [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax __ecx]

#pragma aux _TinyDPMISetLimit = \
        _MOV_CX_DX      \
        _SHR_ECX_N 16   \
        _MOV_AX_W DPMI_0008 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm              [__bx] [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax __ecx]

#pragma aux _TinyDPMISetRights = \
        _MOV_AX_W DPMI_0009 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm              [__bx] [__cx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyDPMIGetDescriptor = \
        _PUSH_ES        \
        _MOV_ES_CX      \
        _MOV_AX_W DPMI_000B \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm              [__bx] [__cx __edi] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyDPMISetDescriptor = \
        _PUSH_ES        \
        _MOV_ES_CX      \
        _MOV_AX_W DPMI_000C \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm              [__bx] [__cx __edi] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyDPMICreateCSAlias = \
        _MOV_AX_W DPMI_000A \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
    "finish:"           \
    __parm __caller     [__bx] \
    __value             [__ax] \
    __modify __exact    [__eax]

#pragma aux _TinyDPMIFreeSel = \
        _MOV_AX_W DPMI_0001 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller     [__bx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyDPMIRawPMtoRMAddr = \
        _MOV_AX_W DPMI_0306 \
        _XOR_DI_DI      \
        _STC            \
        _INT_31         \
        _MOV_CX_SI      \
        "jnc short finish" \
        _XOR_CX_CX      \
        _XOR_DI_DI      \
    "finish:"           \
    __parm __caller     [] \
    __value             [__cx __edi] \
    __modify __exact    [__eax __ecx __si __edi]

#pragma aux _TinyDPMIRawRMtoPMAddr = \
        _MOV_AX_W DPMI_0306 \
        _STC            \
        _INT_31         \
        "jnc short L1"  \
        _XOR_BX_BX      \
        "jmp short finish" \
    "L1:"               \
        _SHL_EBX_N 16   \
        _USE16 _MOV_BX_CX \
    "finish:"           \
    __parm __caller     [] \
    __value             [__ebx] \
    __modify __exact    [__eax __ebx __cx __si __edi]

#pragma aux _TinyDPMISaveRMStateAddr = \
        _MOV_AX_W DPMI_0305 \
        _STC            \
        _INT_31         \
        _MOV_CX_SI      \
        "jnc short finish" \
        _XOR_CX_CX      \
        _XOR_DI_DI      \
    "finish:"           \
    __parm __caller     [] \
    __value             [__cx __edi] \
    __modify __exact    [__ax __bx __ecx __si __edi]

#pragma aux _TinyDPMISavePMStateAddr = \
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        "jnc short L1"  \
        _XOR_CX_CX      \
        _XOR_BX_BX      \
        "jmp short finish" \
    "L1:"               \
        _SHL_EBX_N 16   \
        _USE16 _MOV_BX_CX \
    "finish:"           \
    __parm __caller     [] \
    __value             [__ebx] \
    __modify __exact    [__ax __ebx __ecx __si __edi]

#pragma aux _TinyDPMISaveStateSize = \
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
    "finish:"           \
    __parm __caller     [] \
    __value             [__ax] \
    __modify __exact    [__ax __bx __cx __si __edi]

#pragma aux _TinyDPMISimulateRealInt = \
        _PUSH_ES        \
        _MOV_ES_DX      \
        _MOV_AX_W DPMI_0300 \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm __caller     [__bl] [__bh] [__cx] [__dx __edi] \
    __value             [__eax] \
    __modify __exact    [__eax __bx __cx __edi]

#pragma aux _TinyDPMICallRealFarFrame = \
        _PUSH_ES        \
        _MOV_ES_DX      \
        _MOV_AX_W DPMI_0301 \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm __caller     [__bh] [__cx] [__dx __edi] \
    __value             [__eax] \
    __modify __exact    [__eax __bx __cx __edi]

#pragma aux _TinyDPMICallRealIntFrame = \
        _PUSH_ES        \
        _MOV_ES_DX      \
        _MOV_AX_W DPMI_0302 \
        _INT_31         \
        _SBB_AX_AX      \
        _POP_ES         \
    __parm __caller     [__bh] [__cx] [__dx __edi] \
    __value             [__eax] \
    __modify __exact    [__eax __bx __cx __edi]

#pragma aux _TinyDPMIGetProtectVect = \
        _MOV_AX_W DPMI_0204 \
        _INT_31         \
        "jnc short finish" \
        _XOR_CX_CX      \
        _XOR_DX_DX      \
    "finish:"           \
    __parm __caller     [__bl] \
    __value             [__cx __edx] \
    __modify __exact    [__ax __bx __ecx __edx]

#pragma aux _TinyDPMISetProtectVect = \
        _MOV_AX_W DPMI_0205 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller     [__bl] [__cx __edx] \
    __value             [__eax] \
    __modify __exact    [__eax __bx __cx __edx]

#pragma aux _TinyDPMIGetProtectExcpt = \
        _MOV_AX_W DPMI_0202 \
        _INT_31         \
        "jnc short finish" \
        _XOR_CX_CX      \
        _XOR_DX_DX      \
    "finish:"           \
    __parm __caller     [__bl] \
    __value             [__cx __edx] \
    __modify __exact    [__ax __bx __ecx __edx]

#pragma aux _TinyDPMISetProtectExcpt = \
        _MOV_AX_W DPMI_0203 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller     [__bl] [__cx __edx] \
    __value             [__eax] \
    __modify __exact    [__eax __bx __cx __edx]

#pragma aux _TinyDPMIAlloc = \
        _MOV_AX_W DPMI_0501 \
        _INT_31         \
        _SBB_AX_AX      /* eax=-1 if alloc failed */ \
        _INC_AX         /* eax=0  if alloc failed */ \
        "je short finish" \
        _MOV_AX_BX      /* linear address returned in BX:CX */ \
        _SHL_EAX_N 16   \
        _USE16 _MOV_AX_CX \
        "mov  [eax],di" /* store handle in block */ \
        "mov  2[eax],si" /* ... */ \
    "finish:"           \
    __parm __caller     [__bx] [__cx] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx __ecx __esi __edi]

#pragma aux _TinyDPMIRealloc = \
        "mov  di,[eax]" /* get memory block handle */\
        "mov  si,2[eax]" /* ... */\
        _MOV_AX_W DPMI_0503 \
        _INT_31         \
        _SBB_AX_AX      /* eax=-1 if alloc failed */ \
        _INC_AX         /* eax=0  if alloc failed */ \
        "je short finish" \
        _MOV_AX_BX      /* linear address returned in BX:CX */ \
        _SHL_EAX_N 16   \
        _USE16 _MOV_AX_CX \
        "mov  [eax],di" /* store new handle in block */ \
        "mov  2[eax],si" /* ... */ \
    "finish:"           \
    __parm __caller     [__eax] [__bx] [__cx] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx __ecx __esi __edi]

#pragma aux _TinyDPMIFree = \
        _MOV_AX_W DPMI_0502 \
        _INT_31         \
    __parm __caller     [__si] [__di] \
    __value             \
    __modify __exact    [__eax __esi __edi]

#pragma aux _TinyDPMIBase = \
        _MOV_AX_W DPMI_0006 \
        _INT_31         \
        _MOV_AX_CX      \
        _SHL_EAX_N 16   \
        _USE16 _MOV_AX_DX \
    __parm __caller     [__bx] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx __ecx __edx]

#pragma aux _TinyDPMIDOSAlloc = \
        _MOV_AX_W DPMI_0100 \
        _INT_31         \
        _SBB_BX_BX      \
        _NOT_BX         \
        _SHL_EAX_N 16   \
        _USE16 _MOV_AX_DX \
        _AND_AX_BX      \
    __parm __caller     [__bx] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx __edx]

#pragma aux _TinyDPMIDOSFree = \
        _MOV_AX_W DPMI_0101 \
        _INT_31         \
    __parm __caller     [__dx] \
    __value             \
    __modify __exact    [__eax __edx]

#pragma aux _TinyDPMIGetRealVect = \
        _MOV_AX_W DPMI_0200 \
        _INT_31         \
        _SHL_ECX_N 16   \
        _USE16 _MOV_CX_DX \
    __parm __caller     [__bl] \
    __value             [__ecx] \
    __modify __exact    [__eax __ebx __ecx __edx]

#pragma aux _TinyDPMISetRealVect = \
        _MOV_AX_W DPMI_0201 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller     [__bl] [__cx] [__dx] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx __ecx __edx]


#endif

#endif
