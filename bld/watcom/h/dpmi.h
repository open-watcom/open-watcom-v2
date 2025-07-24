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


#ifndef _DPMI_H_INCLUDED
#define _DPMI_H_INCLUDED

#include "watcom.h"
#include "dosfuncx.h"
#include "descript.h"


#ifdef _M_I86
#define DPMIDATA    __far
#else
#define DPMIDATA
#endif

#define DPMI_ERROR(rc)  ((int_32)(rc) < 0)
#define DPMI_OK(rc)     ((int_32)(rc) >= 0)
#define DPMI_INFO(rc)   ((uint_16)(rc))
#define DPMI_LINFO(rc)  ((uint_32)(rc))

/*
 * DPMI registers structure definition for DPMI SimulateRealInt
 */
typedef struct {
    union dpmi_general_regs {
        struct dpmi_dwordregs {
            unsigned long   edi;
            unsigned long   esi;
            unsigned long   ebp;
            unsigned long   __reserved;
            unsigned long   ebx;
            unsigned long   edx;
            unsigned long   ecx;
            unsigned long   eax;
        } x;
        struct dpmi_wordregs {
            unsigned short  di, __filler1;
            unsigned short  si, __filler2;
            unsigned short  bp, __filler3;
            unsigned long   __reserved;
            unsigned short  bx, __filler4;
            unsigned short  dx, __filler5;
            unsigned short  cx, __filler6;
            unsigned short  ax, __filler7;
        } w;
        struct dpmi_byteregs {
            unsigned long   __filler1;
            unsigned long   __filler2;
            unsigned long   __filler3;
            unsigned long   __reserved;
            unsigned char   bl, bh; unsigned short __filler4;
            unsigned char   dl, dh; unsigned short __filler5;
            unsigned char   cl, ch; unsigned short __filler6;
            unsigned char   al, ah; unsigned short __filler7;
        } h;
    } r;
    unsigned short  flags;
    unsigned short  es;
    unsigned short  ds;
    unsigned short  fs;
    unsigned short  gs;
    unsigned short  ip;
    unsigned short  cs;
    unsigned short  sp;
    unsigned short  ss;
} dpmi_regs_struct;

/*
 * Pharlap registers structure definition for Pharlap SimulateRealInt
 */
#pragma pack( __push, 1 )
typedef struct {
    unsigned short  intno;  /* Interrupt number */
    unsigned short  ds;     /* DS register */
    unsigned short  es;     /* ES register */
    unsigned short  fs;     /* FS register */
    unsigned short  gs;     /* GS register */
    union pharlap_general_regs {
        /*
         * original Pharlap structure contains only EAX and EDX registers
         * to simplify handling in OW we add remaining registers to the end of structure
         * after EAX and EDX registers
         * it is used for inline code to save input/output value of all registers
         */
        struct pharlap_dwordregs {
            unsigned long   eax;
            unsigned long   edx;
            unsigned long   ebx;
            unsigned long   ecx;
            unsigned long   edi;
            unsigned long   esi;
            unsigned long   ebp;
        } x;
        struct pharlap_wordregs {
            unsigned short  ax, __filler1;
            unsigned short  dx, __filler2;
            unsigned short  bx, __filler3;
            unsigned short  cx, __filler4;
            unsigned short  di, __filler5;
            unsigned short  si, __filler6;
            unsigned short  bp, __filler7;
        } w;
        struct pharlap_byteregs {
            unsigned char   al, ah; unsigned short __filler1;
            unsigned char   dl, dh; unsigned short __filler2;
            unsigned char   bl, bh; unsigned short __filler3;
            unsigned char   cl, ch; unsigned short __filler4;
        } h;
    } r;
} pharlap_regs_struct;
#pragma pack( __pop )

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

typedef uint_32     dpmi_ret;

typedef void __far  *intr_addr;
typedef void __far  *proc_addr;

#define DPMISetWatch(a,b,c)                     _DPMISetWatch( (a) >> 16, (a), (b), (c) )
#define DPMIClearWatch                          _DPMIClearWatch
#define DPMITestWatch                           _DPMITestWatch
#define DPMIResetWatch                          _DPMIResetWatch
#define DPMILockLinearRegion(a,b)               _DPMILockLinearRegion( (a) >> 16, (a), (b) >> 16, (b) )
#define DPMIUnlockLinearRegion(a,b)             _DPMIUnlockLinearRegion( (a) >> 16, (a), (b) >> 16, (b) )
#define DPMIGetDescriptor                       _DPMIGetDescriptor
#define DPMISetDescriptor                       _DPMISetDescriptor
#define DPMICreateCodeSegmentAliasDescriptor    _DPMICreateCodeSegmentAliasDescriptor
#define DPMIAllocateLDTDescriptors              _DPMIAllocateLDTDescriptors
#define DPMISegmentToDescriptor                 _DPMISegmentToDescriptor
#define DPMIFreeLDTDescriptor                   _DPMIFreeLDTDescriptor
#define DPMIGetSegmentBaseAddress               _DPMIGetSegmentBaseAddress
#define DPMISetSegmentBaseAddress(a,b)          _DPMISetSegmentBaseAddress( (a), (b) >> 16, (b) )
#define DPMISetSegmentLimit(a,b)                _DPMISetSegmentLimit( (a), (b) >> 16, (b) & 0xffff )
#define DPMISetDescriptorAccessRights           _DPMISetDescriptorAccessRights
#define DPMISimulateRealModeInterrupt           _DPMISimulateRealModeInterrupt
#define DPMICallRealModeProcedureWithFarReturnFrame _DPMICallRealModeProcedureWithFarReturnFrame
#define DPMICallRealModeProcedureWithIRETFrame  _DPMICallRealModeProcedureWithIRETFrame
#define DPMIGetNextSelectorIncrementValue       _DPMIGetNextSelectorIncrementValue
#define DPMIGetRealModeInterruptVector          _DPMIGetRealModeInterruptVector
#define DPMISetRealModeInterruptVector          _DPMISetRealModeInterruptVector
#define DPMIAllocateRealModeCallBackAddress     _DPMIAllocateRealModeCallBackAddress
#define DPMIFreeRealModeCallBackAddress         _DPMIFreeRealModeCallBackAddress
#define DPMIGetPMInterruptVector                _DPMIGetPMInterruptVector
#define DPMISetPMInterruptVector                _DPMISetPMInterruptVector
#define DPMISetPMExceptionVector                _DPMISetPMExceptionVector
#define DPMIGetPMExceptionVector                _DPMIGetPMExceptionVector

#define DPMIAllocateMemoryBlock(a,b)            _DPMIAllocateMemoryBlock( (a), (b) >> 16, (b) )
#define DPMIFreeMemoryBlock(a)                  _DPMIFreeMemoryBlock( (a) >> 16, (a) )
#define DPMIResizeMemoryBlock(a,b,c)            _DPMIResizeMemoryBlock( (a), (b) >> 16, (b), (c) >> 16, (c) )
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
#define DPMIGetVersion                          _DPMIGetVersion
#define DPMIGetFreeMemoryInformation            _DPMIGetFreeMemoryInformation

#define PharlapAllocateDOSMemoryBlock           _PharlapAllocateDOSMemoryBlock
#define PharlapFreeDOSMemoryBlock               _PharlapFreeDOSMemoryBlock
#define PharlapGetPMInterruptVector             _PharlapGetPMInterruptVector
#define PharlapGetRealModeInterruptVector       _PharlapGetRealModeInterruptVector
#define PharlapSetPMInterruptVector             _PharlapSetPMInterruptVector
#define PharlapSetPMInterruptVector_passup      _PharlapSetPMInterruptVector_passup
#define PharlapSetBothInterruptVectors          _PharlapSetBothInterruptVectors
#define PharlapSetRealModeInterruptVector       _PharlapSetRealModeInterruptVector
#define PharlapSimulateRealModeInterrupt        _PharlapSimulateRealModeInterrupt
#define PharlapSimulateRealModeInterruptExt     _PharlapSimulateRealModeInterruptExt
#define PharlapGetSegmentBaseAddress            _PharlapGetSegmentBaseAddress

#define DOS4GSetPMInterruptVector_passup        _DOS4GSetPMInterruptVector_passup
#define DOS4GGetPMInterruptVector               _DOS4GGetPMInterruptVector

/*
 * C run-time library flag indicating that DPMI services (host) is available
 */
extern unsigned char _DPMI;

extern int      _DPMIFreeRealModeCallBackAddress( proc_addr proc );
extern proc_addr _DPMIAllocateRealModeCallBackAddress( proc_addr proc, dpmi_regs_struct DPMIDATA *dr );
extern intr_addr _DPMIGetRealModeInterruptVector( uint_8 iv );
extern int      _DPMISetPMInterruptVector( uint_8 iv, intr_addr intr );
extern int      _DPMISetPMExceptionVector( uint_8 iv, proc_addr proc );
extern proc_addr _DPMIGetPMExceptionVector( uint_8 iv );
extern intr_addr _DPMIGetPMInterruptVector( uint_8 iv );
extern void     _DPMISetRealModeInterruptVector( uint_8 iv, intr_addr intr );
extern int_16   _DPMIModeDetect( void );
extern void     _DPMIIdle( void );
extern void     _DPMIGetVersion( version_info DPMIDATA * );
extern int_32   _DPMIAllocateLDTDescriptors( uint_16 );
extern int_32   _DPMISegmentToDescriptor( uint_16 );
extern int      _DPMIFreeLDTDescriptor( uint_16 );
extern uint_16  _DPMIGetNextSelectorIncrementValue( void );
extern uint_32  _DPMIGetSegmentBaseAddress( uint_16 );
extern int      _DPMISetSegmentBaseAddress( uint_16, uint_16 hiw, uint_16 low );
extern int      _DPMISetSegmentLimit( uint_16, uint_16 hiw, uint_16 low );
extern int      _DPMISetDescriptorAccessRights( uint_16, uint_16 );
extern int      _DPMIAllocateMemoryBlock( dpmi_mem_block DPMIDATA *, uint_16 hiw, uint_16 low );
extern int      _DPMIResizeMemoryBlock( dpmi_mem_block DPMIDATA *, uint_16 hiw1, uint_16 low1, uint_16 hiw2, uint_16 low2 );
extern int      _DPMIFreeMemoryBlock( uint_16 hiw, uint_16 low );
extern int      _DPMILockLinearRegion( uint_16 hiw1, uint_16 low1, uint_16 hiw2, uint_16 low2 );
extern int      _DPMIUnlockLinearRegion( uint_16 hiw1, uint_16 low1, uint_16 hiw2, uint_16 low2 );
extern int      _DPMIGetDescriptor( uint_16, descriptor DPMIDATA * );
extern int      _DPMISetDescriptor( uint_16, descriptor DPMIDATA * );
extern dpmi_ret _DPMICreateCodeSegmentAliasDescriptor( uint_16 );
extern int      _DPMIGetFreeMemoryInformation( dpmi_mem DPMIDATA * );
extern int      _DPMISimulateRealModeInterrupt( uint_8 interrupt, uint_8 flags, uint_16 words_to_copy, dpmi_regs_struct DPMIDATA *dr );
extern int      _DPMICallRealModeProcedureWithFarReturnFrame( uint_8 flags, uint_16 words_to_copy, dpmi_regs_struct DPMIDATA *dr );
extern int      _DPMICallRealModeProcedureWithIRETFrame( uint_8 flags, uint_16 words_to_copy, dpmi_regs_struct DPMIDATA *dr );
extern dpmi_dos_mem_block _DPMIAllocateDOSMemoryBlock( uint_16 para );
extern int      _DPMIFreeDOSMemoryBlock( uint_16 sel );
extern proc_addr _DPMIRawPMtoRMAddr( void );
extern uint_32  _DPMIRawRMtoPMAddr( void );
extern proc_addr _DPMISaveRMStateAddr( void );
extern uint_32  _DPMISavePMStateAddr( void );
extern uint_16  _DPMISaveStateSize( void );
extern proc_addr _DPMIGetVendorSpecificAPI( char DPMIDATA * );

extern dpmi_ret _DPMISetWatch( uint_16 hiw, uint_16 low, uint_8 len, uint_8 type );
extern dpmi_ret _DPMIClearWatch( uint_16 handle );
extern dpmi_ret _DPMITestWatch( uint_16 handle );
extern dpmi_ret _DPMIResetWatch( uint_16 handle );

extern uint_16  _PharlapAllocateDOSMemoryBlock( uint_16 para );
extern uint_16  _PharlapFreeDOSMemoryBlock( uint_16 seg );
extern intr_addr _PharlapGetPMInterruptVector( uint_8 iv );
extern intr_addr _PharlapGetRealModeInterruptVector( uint_8 iv );
extern void     _PharlapSetPMInterruptVector( uint_8 iv, intr_addr intr );
extern void     _PharlapSetRealModeInterruptVector( uint_8 iv, intr_addr intr );
extern void     _PharlapSetPMInterruptVector_passup( uint_8 iv, intr_addr intr );
extern void     _PharlapSetBothInterruptVectors( uint_8 iv, intr_addr pm, intr_addr rm );
extern int      _PharlapSimulateRealModeInterrupt( pharlap_regs_struct *dp, unsigned bx, unsigned cx, unsigned di );
extern int      _PharlapSimulateRealModeInterruptExt( pharlap_regs_struct *dp );
extern uint_32  _PharlapGetSegmentBaseAddress( uint_16 );

extern void     _DOS4GSetPMInterruptVector_passup( uint_8 iv, intr_addr intr );
extern intr_addr _DOS4GGetPMInterruptVector( uint_8 iv );

#include "asmbytes.h"

#define MULTIPLEX_1680  0x80 0x16
#define MULTIPLEX_1686  0x86 0x16

#pragma aux _DPMIIdle = \
        _MOV_AX_W MULTIPLEX_1680 \
        _INT_2F         \
    __parm __caller [] \
    __value         \
    __modify __exact [__ax]

#pragma aux _DPMIModeDetect = \
        _MOV_AX_W MULTIPLEX_1686 \
        _INT_2F         \
    __parm __caller [] \
    __value         [__ax] \
    __modify __exact [__ax]


#define DPMI_0000       0x00 0x00
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
#define DPMI_0301       0x01 0x03   /* not in DOS/4GW */
#define DPMI_0302       0x02 0x03   /* not in DOS/4GW */
#define DPMI_0303       0x03 0x03   /* not in DOS/4GW */
#define DPMI_0304       0x04 0x03   /* not in DOS/4GW */
#define DPMI_0305       0x05 0x03   /* not in DOS/4GW */
#define DPMI_0306       0x06 0x03   /* not in DOS/4GW */
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


#ifdef _M_I86
#pragma aux _DPMIAllocateLDTDescriptors = \
        _MOV_AX_W DPMI_0000 \
        _INT_31         \
        _SBB_CX_CX      \
    __parm __caller [__cx] \
    __value         [__cx __ax] \
    __modify __exact [__ax __cx]
#else
#pragma aux _DPMIAllocateLDTDescriptors = \
        _MOV_AX_W DPMI_0000 \
        _INT_31         \
        _SBB_CX_CX      \
        _USE16 _MOV_CX_AX \
    __parm __caller [__cx] \
    __value         [__ecx] \
    __modify __exact [__eax __ecx]
#endif

#ifdef _M_I86
#pragma aux _DPMIFreeLDTDescriptor = \
        _MOV_AX_W DPMI_0001 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMIFreeLDTDescriptor = \
        _MOV_AX_W DPMI_0001 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMISegmentToDescriptor = \
        _MOV_AX_W DPMI_0002 \
        _INT_31         \
        _SBB_BX_BX      \
    __parm __caller [__bx] \
    __value         [__bx __ax] \
    __modify __exact [__ax __bx]
#else
#pragma aux _DPMISegmentToDescriptor = \
        _MOV_AX_W DPMI_0002 \
        _INT_31         \
        _SBB_BX_BX      \
        _USE16 _MOV_BX_AX \
    __parm __caller [__bx] \
    __value         [__ebx]\
    __modify __exact [__ax __ebx]
#endif

#pragma aux _DPMIGetNextSelectorIncrementValue = \
        _MOV_AX_W DPMI_0003 \
        _INT_31         \
    __parm __caller [__bx] \
    __value         [__ax] \
    __modify __exact [__ax]

#ifdef _M_I86
#pragma aux _DPMIGetSegmentBaseAddress = \
        _MOV_AX_W DPMI_0006 \
        _INT_31         \
    __parm __caller [__bx] \
    __value         [__cx __dx] \
    __modify __exact [__ax __cx __dx]
#else
#pragma aux _DPMIGetSegmentBaseAddress = \
        _MOV_AX_W DPMI_0006 \
        _INT_31         \
        _SHL_ECX_N 16   \
        _USE16 _MOV_CX_DX \
    __parm __caller [__bx] \
    __value         [__ecx] \
    __modify __exact [__ax __ecx __edx]
#endif

#ifdef _M_I86
#pragma aux _DPMISetSegmentBaseAddress = \
        _MOV_AX_W DPMI_0007 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__dx] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMISetSegmentBaseAddress = \
        _MOV_AX_W DPMI_0007 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__dx] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMISetSegmentLimit = \
        _MOV_AX_W DPMI_0008 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__dx] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMISetSegmentLimit = \
        _MOV_AX_W DPMI_0008 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__dx] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMISetDescriptorAccessRights = \
        _MOV_AX_W DPMI_0009 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMISetDescriptorAccessRights = \
        _MOV_AX_W DPMI_0009 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
        _MOV_AX_W DPMI_000A \
        _INT_31         \
        _SBB_BX_BX      \
    __parm __caller [__bx] \
    __value         [__bx __ax] \
    __modify __exact [__ax __bx]
#else
#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
        _MOV_AX_W DPMI_000A \
        _INT_31         \
        _SBB_BX_BX      \
        _USE16 _MOV_BX_AX \
    __parm __caller [__bx] \
    __value         [__ebx] \
    __modify __exact [__eax __ebx]
#endif

#ifdef _M_I86
#pragma aux _DPMIGetDescriptor = \
        _MOV_AX_W DPMI_000B \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__es __di] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMIGetDescriptor = \
        _SET_ES         \
        _MOV_AX_W DPMI_000B \
        _INT_31         \
        _SBB_AX_AX      \
        _RST_ES         \
    __parm __caller [__bx] [__edi] \
    __value         [__eax] \
    __modify __exact [__eax _MODIF_ES]
#endif

#ifdef _M_I86
#pragma aux _DPMISetDescriptor = \
        _MOV_AX_W DPMI_000C \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__es __di] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMISetDescriptor = \
        _SET_ES         \
        _MOV_AX_W DPMI_000C \
        _INT_31         \
        _SBB_AX_AX      \
        _RST_ES         \
    __parm __caller [__bx] [__edi] \
    __value         [__eax] \
    __modify __exact [__eax _MODIF_ES]
#endif

#ifdef _M_I86
#pragma aux _DPMIAllocateDOSMemoryBlock = \
        _MOV_AX_W DPMI_0100 \
        _INT_31         \
        _SBB_BX_BX      \
        _NOT_BX         \
        _AND_AX_BX      \
        _AND_DX_BX      \
    __parm __caller [__bx] \
    __value         [__dx __ax] \
    __modify __exact [__ax __bx __dx]
#else
#pragma aux _DPMIAllocateDOSMemoryBlock = \
        _MOV_AX_W DPMI_0100 \
        _INT_31         \
        _SBB_BX_BX      \
        _NOT_BX         \
        _SHL_EDX_N 16   \
        _USE16 _MOV_DX_AX \
        _AND_DX_BX      \
    __parm __caller [__bx] \
    __value         [__edx] \
    __modify [__eax __ebx __edx]
#endif

#ifdef _M_I86
#pragma aux _DPMIFreeDOSMemoryBlock = \
        _MOV_AX_W DPMI_0101 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__dx] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMIFreeDOSMemoryBlock = \
        _MOV_AX_W DPMI_0101 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__dx] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMIGetRealModeInterruptVector = \
        _MOV_AX_W DPMI_0200 \
        _INT_31         \
    __parm __caller [__bl] \
    __value         [__cx __dx] \
    __modify __exact [__ax __cx __dx]
#else
#pragma aux _DPMIGetRealModeInterruptVector = \
        _MOV_AX_W DPMI_0200 \
        _XOR_DX_DX      \
        _INT_31         \
    __parm __caller [__bl] \
    __value         [__cx __edx] \
    __modify __exact [__ax __cx __edx]
#endif

#ifdef _M_I86
#pragma aux _DPMISetRealModeInterruptVector = \
        _MOV_AX_W DPMI_0201 \
        _INT_31         \
    __parm __caller [__bl] [__cx __dx] \
    __value         \
    __modify __exact [__ax]
#else
#pragma aux _DPMISetRealModeInterruptVector = \
        _MOV_AX_W DPMI_0201 \
        _INT_31         \
    __parm __caller [__bl] [__cx __edx] \
    __value         \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMIGetPMExceptionVector = \
        _MOV_AX_W DPMI_0202 \
        _INT_31         \
        _SBB_AX_AX      \
        _NOT_AX         \
        _AND_CX_AX      \
        _AND_DX_AX      \
    __parm __caller [__bl] \
    __value         [__cx __dx] \
    __modify __exact [__ax __cx __dx]
#else
#pragma aux _DPMIGetPMExceptionVector = \
        _MOV_AX_W DPMI_0202 \
        _INT_31         \
        _SBB_AX_AX      \
        _NOT_AX         \
        _AND_CX_AX      \
        _AND_DX_AX      \
    __parm __caller [__bl] \
    __value         [__cx __edx] \
    __modify __exact [__eax __cx __edx]
#endif

#ifdef _M_I86
#pragma aux _DPMISetPMExceptionVector = \
        _MOV_AX_W DPMI_0203 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bl] [__cx __dx] \
    __value         [__ax]\
    __modify __exact [__ax]
#else
#pragma aux _DPMISetPMExceptionVector = \
        _MOV_AX_W DPMI_0203 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bl] [__cx __edx] \
    __value         [__eax]\
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMIGetPMInterruptVector = \
        _MOV_AX_W DPMI_0204 \
        _INT_31         \
    __parm __caller [__bl] \
    __value         [__cx __dx] \
    __modify __exact [__ax __cx __dx]
#else
#pragma aux _DPMIGetPMInterruptVector = \
        _MOV_AX_W DPMI_0204 \
        _INT_31         \
    __parm __caller [__bl] \
    __value         [__cx __edx] \
    __modify __exact [__ax __cx __edx]
#endif

#ifdef _M_I86
#pragma aux _DPMISetPMInterruptVector = \
        _MOV_AX_W DPMI_0205 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bl] [__cx __dx] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMISetPMInterruptVector = \
        _MOV_AX_W DPMI_0205 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bl] [__cx __edx] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMISimulateRealModeInterrupt = \
        _MOV_AX_W DPMI_0300 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bl] [__bh] [__cx] [__es __di] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMISimulateRealModeInterrupt = \
        _SET_ES         \
        _MOV_AX_W DPMI_0300 \
        _INT_31         \
        _SBB_AX_AX      \
        _RST_ES         \
    __parm __caller [__bl] [__bh] [__cx] [__edi] \
    __value         [__eax] \
    __modify __exact [__eax _MODIF_ES]
#endif

/*
 *  DOS/4GW DOS/4GW Pro DOS/4G
 *  no      yes         yes
 */
#ifdef _M_I86
#pragma aux _DPMICallRealModeProcedureWithFarReturnFrame = \
        _STC /* for missing service check */\
        _MOV_AX_W DPMI_0301 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller     [__bh] [__cx] [__es __di] \
    __value             [__ax] \
    __modify __exact    [__ax]
#else
#pragma aux _DPMICallRealModeProcedureWithFarReturnFrame = \
        _SET_ES         \
        _STC /* for missing service check */\
        _MOV_AX_W DPMI_0301 \
        _INT_31         \
        _SBB_AX_AX      \
        _RST_ES         \
    __parm __caller     [__bh] [__cx] [__edi] \
    __value             [__eax] \
    __modify __exact    [__eax _MODIF_ES]
#endif

/*
 *  DOS/4GW DOS/4GW Pro DOS/4G
 *  no      yes         yes
 */
#ifdef _M_I86
#pragma aux _DPMICallRealModeProcedureWithIRETFrame = \
        _STC /* for missing service check */\
        _MOV_AX_W DPMI_0302 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller     [__bh] [__cx] [__es __di] \
    __value             [__ax] \
    __modify __exact    [__ax]
#else
#pragma aux _DPMICallRealModeProcedureWithIRETFrame = \
        _SET_ES         \
        _STC /* for missing service check */\
        _MOV_AX_W DPMI_0302 \
        _INT_31         \
        _SBB_AX_AX      \
        _RST_ES         \
    __parm __caller     [__bh] [__cx] [__edi] \
    __value             [__eax] \
    __modify __exact    [__eax _MODIF_ES]
#endif

/*
 *  DOS/4GW DOS/4GW Pro DOS/4G
 *  no      yes         yes
 */
#ifdef _M_I86
#pragma aux _DPMIAllocateRealModeCallBackAddress = \
        _SAVE_DSDX      \
        _STC /* missing service check */\
        _MOV_AX_W DPMI_0303 \
        _INT_31         \
        _SBB_AX_AX      \
        _NOT_AX         \
        _AND_CX_AX      \
        _AND_DX_AX      \
        _REST_DS         \
    __parm __caller [__dx __si] [__es __di] \
    __value         [__cx __dx] \
    __modify __exact [__ax __cx __dx _MODIF_DS]
#else
#pragma aux _DPMIAllocateRealModeCallBackAddress = \
        _SET_ES         \
        _SAVE_DSDX      \
        _STC /* missing service check */\
        _MOV_AX_W DPMI_0303 \
        _INT_31         \
        _SBB_AX_AX      \
        _NOT_AX         \
        _AND_CX_AX      \
        _AND_DX_AX      \
        _REST_DS        \
        _RST_ES         \
    __parm __caller [__dx __esi] [__edi] \
    __value         [__cx __edx] \
    __modify __exact [__eax __ecx __edx _MODIF_DS _MODIF_ES]
#endif

/*
 *  DOS/4GW DOS/4GW Pro DOS/4G
 *  no      yes         yes
 */
#ifdef _M_I86
#pragma aux _DPMIFreeRealModeCallBackAddress = \
        _STC /* for missing service check */\
        _MOV_AX_W DPMI_0304 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__cx __dx] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMIFreeRealModeCallBackAddress = \
        _STC /* for missing service check */\
        _MOV_AX_W DPMI_0304 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__cx __edx] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif


/*
 *  DOS/4GW DOS/4GW Pro DOS/4G
 *  no      no          yes
 */
#ifdef _M_I86
#else
#pragma aux _DPMISaveRMStateAddr = \
        _STC /* missing service check */\
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        _SBB_AX_AX      \
        _NOT_AX         \
        _MOV_CX_SI      \
        _AND_CX_AX      \
        _AND_DI_AX      \
    __parm __caller [] \
    __value         [__cx __edi] \
    __modify __exact [__eax __bx __ecx __si __edi]
#endif

/*
 *  DOS/4GW DOS/4GW Pro DOS/4G
 *  no      no          yes
 */
#ifdef _M_I86
#else
#pragma aux _DPMISavePMStateAddr = \
        _STC /* missing service check */\
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        _SBB_AX_AX      \
        _NOT_AX         \
        _SHL_EBX_N 16   \
        _USE16 _MOV_BX_CX \
        _AND_BX_AX      \
    __parm __caller [] \
    __value         [__ebx] \
    __modify __exact [__eax __ebx __cx __si __edi]
#endif

/*
 *  DOS/4GW DOS/4GW Pro DOS/4G
 *  no      no          yes
 */
#ifdef _M_I86
#else
#pragma aux _DPMISaveStateSize = \
        _STC /* missing service check */\
        _MOV_AX_W DPMI_0305 \
        _INT_31         \
        _SBB_DI_DI      \
        _NOT_DI         \
        _AND_AX_DI      \
    __parm __caller [] \
    __value         [__ax] \
    __modify __exact [__eax __bx __cx __si __edi]
#endif

/*
 *  DOS/4GW DOS/4GW Pro DOS/4G
 *  no      no          yes
 */
#ifdef _M_I86
#else
#pragma aux _DPMIRawPMtoRMAddr = \
        _XOR_DI_DI      \
        _STC /* missing service check */\
        _MOV_AX_W DPMI_0306 \
        _INT_31         \
        _SBB_AX_AX      \
        _NOT_AX         \
        _MOV_CX_SI      \
        _AND_CX_AX      \
        _AND_DI_AX      \
    __parm __caller [] \
    __value         [__cx __edi] \
    __modify __exact [__eax __bx __ecx __si __edi]
#endif

/*
 *  DOS/4GW DOS/4GW Pro DOS/4G
 *  no      no          yes
 */
#ifdef _M_I86
#else
#pragma aux _DPMIRawRMtoPMAddr = \
        _STC /* missing service check */\
        _MOV_AX_W DPMI_0306 \
        _INT_31         \
        _SBB_AX_AX      \
        _NOT_AX         \
        _SHL_EBX_N 16   \
        _USE16 _MOV_BX_CX \
        _AND_BX_AX      \
    __parm __caller [] \
    __value         [__ebx] \
    __modify __exact [__eax __ebx __cx __si __edi]
#endif

#ifdef _M_I86
#pragma aux _DPMIGetVersion = \
        _MOV_AX_W DPMI_0400 \
        _INT_31         \
        "mov  byte ptr es:[si],ah"      \
        "mov  byte ptr es:[si+1],al"    \
        "mov  word ptr es:[si+2],bx"    \
        "mov  byte ptr es:[si+4],cl"    \
        "mov  byte ptr es:[si+5],dh"    \
        "mov  byte ptr es:[si+6],dl"    \
    __parm __caller [__es __si]  \
    __value         \
    __modify __exact [__ax __bx __cx __dx]
#else
#pragma aux _DPMIGetVersion = \
        _MOV_AX_W DPMI_0400 \
        _INT_31         \
        "mov  byte ptr [esi],ah"    \
        "mov  byte ptr [esi+1],al"  \
        "mov  word ptr [esi+2],bx"  \
        "mov  byte ptr [esi+4],cl"  \
        "mov  byte ptr [esi+5],dh"  \
        "mov  byte ptr [esi+6],dl"  \
    __parm __caller [__esi] \
    __value         \
    __modify __exact [__ax __bx __cx __dx]
#endif

#ifdef _M_I86
#pragma aux _DPMIGetFreeMemoryInformation = \
        _MOV_AX_W DPMI_0500 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__es __di] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMIGetFreeMemoryInformation = \
        _SET_ES         \
        _MOV_AX_W DPMI_0500 \
        _INT_31         \
        _SBB_AX_AX      \
        _RST_ES         \
    __parm __caller [__edi] \
    __value         [__eax] \
    __modify __exact [__eax _MODIF_ES]
#endif

#ifdef _M_I86
#pragma aux _DPMIAllocateMemoryBlock =  \
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
#else
#pragma aux _DPMIAllocateMemoryBlock =  \
        _MOV_AX_W DPMI_0501 \
        _INT_31         \
        _SBB_AX_AX      \
        "mov  [edx],cx" \
        "mov  [edx+2],bx" \
        "mov  [edx+4],di" \
        "mov  [edx+6],si" \
    __parm __caller [__edx] [__bx] [__cx] \
    __value         [__eax] \
    __modify __exact [__eax __ebx __cx __di __si]
#endif

#ifdef _M_I86
#pragma aux _DPMIFreeMemoryBlock =  \
        _MOV_AX_W DPMI_0502 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__si] [__di] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMIFreeMemoryBlock =  \
        _MOV_AX_W DPMI_0502 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__si] [__di] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMIResizeMemoryBlock =  \
        _MOV_AX_W DPMI_0503 \
        _INT_31         \
        _SBB_AX_AX      \
        _XCHG_BX_DX      \
        "mov  es:[bx],cx" \
        "mov  es:[bx+2],dx" \
        "mov  es:[bx+4],di" \
        "mov  es:[bx+6],si" \
    __parm __caller [__es __dx] [__bx] [__cx] [__si] [__di] \
    __value         [__ax] \
    __modify __exact [__ax __bx __cx __dx __di __si]
#else
#pragma aux _DPMIResizeMemoryBlock =  \
        _MOV_AX_W DPMI_0503 \
        _INT_31         \
        _SBB_AX_AX      \
        "mov  [edx],cx" \
        "mov  [edx+2],bx" \
        "mov  [edx+4],di" \
        "mov  [edx+6],si" \
    __parm __caller [__edx] [__bx] [__cx] [__si] [__di] \
    __value         [__eax] \
    __modify __exact [__eax __ebx __cx __di __si]
#endif

#ifdef _M_I86
#pragma aux _DPMILockLinearRegion = \
        _MOV_AX_W DPMI_0600 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__si] [__di] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMILockLinearRegion = \
        _MOV_AX_W DPMI_0600 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__si] [__di] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMIUnlockLinearRegion = \
        _MOV_AX_W DPMI_0601 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__si] [__di] \
    __value         [__ax] \
    __modify __exact [__ax]
#else
#pragma aux _DPMIUnlockLinearRegion = \
        _MOV_AX_W DPMI_0601 \
        _INT_31         \
        _SBB_AX_AX      \
    __parm __caller [__bx] [__cx] [__si] [__di] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#else
#pragma aux _DPMIGetVendorSpecificAPI = \
        _PUSH_DS        \
        _PUSH_ES        \
        _PUSH_FS        \
        _PUSH_GS        \
        _PUSH_BP        \
        _MOV_AX_W DPMI_0A00 \
        _INT_31         \
        _MOV_CX_ES      \
        _SBB_AX_AX      \
        _NOT_AX         \
        _AND_CX_AX      \
        _AND_DI_AX      \
        _POP_BP         \
        _POP_GS         \
        _POP_FS         \
        _POP_ES         \
        _POP_DS         \
    __parm __caller [__esi] \
    __value [__cx __edi] \
    __modify __exact [__eax __ebx __ecx __edx __esi]
#endif

#ifdef _M_I86
#pragma aux _DPMISetWatch = \
        _MOV_AX_W DPMI_0B00 \
        _INT_31         \
        _SBB_CX_CX      \
    __parm __caller [__bx] [__cx] [__dl] [__dh] \
    __value         [__cx __bx] \
    __modify __exact [__ax __bx __cx]
#else
#pragma aux _DPMISetWatch = \
        _MOV_AX_W DPMI_0B00 \
        _INT_31         \
        _SBB_AX_AX      \
        _USE16 _MOV_AX_BX \
    __parm __caller [__bx] [__cx] [__dl] [__dh] \
    __value         [__eax] \
    __modify __exact [__eax __ebx]
#endif

#ifdef _M_I86
#pragma aux _DPMIClearWatch = \
        _MOV_AX_W DPMI_0B01 \
        _INT_31         \
        _SBB_BX_BX      \
        _AND_AX_BX      \
    __parm __caller [__bx] \
    __value         [__bx __ax] \
    __modify __exact [__ax __bx]
#else
#pragma aux _DPMIClearWatch = \
        _MOV_AX_W DPMI_0B01 \
        _INT_31         \
        _SBB_BX_BX      \
        _USE16 _AND_BX_AX \
    __parm __caller [__bx] \
    __value         [__ebx] \
    __modify __exact [__eax]
#endif

#ifdef _M_I86
#pragma aux _DPMITestWatch = \
        _MOV_AX_W DPMI_0B02 \
        _INT_31         \
        _SBB_BX_BX      \
    __parm __caller [__bx] \
    __value         [__bx __ax] \
    __modify __exact [__ax __bx]
#else
#pragma aux _DPMITestWatch = \
        _MOV_AX_W DPMI_0B02 \
        _INT_31         \
        _SBB_BX_BX      \
        _USE16 _MOV_BX_AX \
    __parm __caller [__bx] \
    __value         [__ebx] \
    __modify __exact [__ax __ebx]
#endif

#ifdef _M_I86
#pragma aux _DPMIResetWatch = \
        _MOV_AX_W DPMI_0B03 \
        _INT_31         \
        _SBB_BX_BX      \
        _AND_AX_BX      \
    __parm __caller [__bx] \
    __value         [__bx __ax] \
    __modify __exact [__ax __bx]
#else
#pragma aux _DPMIResetWatch = \
        _MOV_AX_W DPMI_0B03 \
        _INT_31         \
        _SBB_BX_BX      \
        _USE16 _AND_BX_AX \
    __parm __caller [__bx] \
    __value         [__eax] \
    __modify __exact [__eax]
#endif


/*************************************
 * only 80386 version pragmas
 *************************************/

#ifndef _M_I86

/*************************************
 * Pharlap specific pragmas
 *************************************/

#define PHARLAP_2502    0x02 0x25
#define PHARLAP_2503    0x03 0x25
#define PHARLAP_2504    0x04 0x25
#define PHARLAP_2505    0x05 0x25
#define PHARLAP_2506    0x06 0x25
#define PHARLAP_2507    0x07 0x25
#define PHARLAP_2508    0x08 0x25
#define PHARLAP_2511    0x11 0x25
#define PHARLAP_25C0    0xC0 0x25
#define PHARLAP_25C1    0xC1 0x25

/*
 * if failed then return zero value
 * if OK then return uint_16 value
 */
#pragma aux _PharlapAllocateDOSMemoryBlock = \
        _MOV_AX_W PHARLAP_25C0 \
        _INT_21         \
        _SBB_BX_BX      \
        _NOT_BX         \
        _AND_AX_BX      \
    __parm __caller [__bx] \
    __value         [__ax] \
    __modify __exact [__eax __ebx]

/*
 * if OK then return zero value
 * if failed then return non-zero value
 */
#pragma aux _PharlapFreeDOSMemoryBlock = \
        _MOV_AX_W PHARLAP_25C1 \
        _INT_21         \
        _SBB_CX_CX      \
        _AND_AX_CX      \
    __parm __caller [__cx] \
    __value         [__ax] \
    __modify __exact [__eax __ecx]

#pragma aux  _PharlapGetPMInterruptVector = \
        _SAVE_ES        \
        _MOV_AX_W PHARLAP_2502 \
        _INT_21         \
        _MOV_CX_ES      \
        _REST_ES         \
    __parm __caller [__cl] \
    __value         [__cx __ebx] \
    __modify __exact [__ax __ebx __cx _MODIF_ES]

#pragma aux _PharlapGetRealModeInterruptVector = \
        _MOV_AX_W PHARLAP_2503 \
        _INT_21         \
        _MOV_CX_BX      \
        _XOR_BX_BX      \
        _USE16 _MOV_BX_CX \
        _SHR_ECX_N 16   \
    __parm __caller [__cl] \
    __value         [__cx __ebx] \
    __modify        [__ax __ebx __ecx]

#pragma aux  _PharlapSetPMInterruptVector = \
        _SAVE_DSDX      \
        _MOV_DX_AX      \
        _MOV_AX_W PHARLAP_2504 \
        _INT_21         \
        _REST_DS        \
    __parm __caller [__cl] [__dx __eax] \
    __value         \
    __modify        [__eax __edx _MODIF_DS]

#pragma aux _PharlapSetRealModeInterruptVector = \
        _SHL_EBX_N 16   \
        _USE16 _MOV_BX_AX \
        _MOV_AX_W PHARLAP_2505 \
        _INT_21         \
    __parm __caller [__cl] [__bx __eax] \
    __value         \
    __modify        [__eax __ebx]

#pragma aux  _PharlapSetPMInterruptVector_passup = \
        _SAVE_DSCX      \
        _MOV_CL_AL      \
        _MOV_AX_W PHARLAP_2506 \
        _INT_21         \
        _REST_DS         \
    __parm __caller [__al] [__cx __edx] \
    __value         \
    __modify        [__eax __ecx __edx _MODIF_DS]

#pragma aux  _PharlapSetBothInterruptVectors = \
        _SAVE_DSCX      \
        _SHL_ESI_N 16   \
        _USE16 _MOV_SI_BX \
        _MOV_BX_SI      \
        _MOV_CL_AL      \
        _MOV_AX_W PHARLAP_2507 \
        _INT_21         \
        _REST_DS         \
    __parm __caller [__al] [__cx __edx] [__si __ebx] \
    __value         \
    __modify        [__eax __ebx __ecx __edx __esi _MODIF_DS]

/*
 * if failed then return (uint_32)-1
 */
#pragma aux _PharlapGetSegmentBaseAddress = \
        _MOV_AX_W PHARLAP_2508 \
        _INT_21         \
        _SBB_AX_AX      \
        _OR_CX_AX       \
    __parm __caller [__bx] \
    __value [__ecx] \
    __modify __exact [__eax __bx __ecx]

#pragma aux _PharlapSimulateRealModeInterrupt = \
        _PUSH_BP        \
        _MOV_AX_W PHARLAP_2511 \
        _INT_21         \
        _SBB_AX_AX      \
        _POP_BP         \
    __parm __caller [__edx] [__ebx] [__ecx] [__edi] \
    __value         [__eax] \
    __modify        [__esi]

#pragma aux _PharlapSimulateRealModeInterruptExt = \
        _PUSH_BP        \
        "mov ebx,[edx+18]" \
        "mov ecx,[edx+22]" \
        "mov edi,[edx+26]" \
        "mov esi,[edx+30]" \
        "mov ebp,[edx+34]" \
        _MOV_AX_W PHARLAP_2511 \
        _INT_21         \
        "mov [edx+34],ebp" \
        "mov [edx+30],esi" \
        "mov [edx+26],edi" \
        "mov [edx+22],ecx" \
        "mov [edx+18],ebx" \
        _SBB_AX_AX      \
        _POP_BP         \
    __parm __caller [__edx] \
    __value         [__eax] \
    __modify        [__ebx __ecx __edi __esi]


/*************************************
 * Rational DOS/4G specific pragmas
 *************************************/

/*
 * only interrupt 0x08-0x2e is auto-passup
 * others need to create appropriate callback
 */
#pragma aux _DOS4GSetPMInterruptVector_passup = \
        _SAVE_DSCX      \
        _MOV_AH DOS_SET_INT \
        _INT_21         \
        _REST_DS        \
    __parm __caller [__al] [__cx __edx] \
    __value         \
    __modify __exact [__ah _MODIF_DS]

#pragma aux _DOS4GGetPMInterruptVector = \
        _SAVE_ES        \
        _MOV_AH DOS_GET_INT \
        _INT_21         \
        _MOV_DX_ES      \
        _MOV_AX_BX      \
        _REST_ES        \
    __parm __caller [__al] \
    __value         [__dx __eax] \
    __modify __exact [__eax __ebx __edx _MODIF_ES]

#endif

#endif
