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

#if defined(__386__)
#define __DX    edx
#else
#define __DX    dx
#endif

#define DOS_SEG( a ) ((unsigned short) (a & 0Xffff))
#define DOS_SEL( a ) ((unsigned short) (a >> 16L))

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
unsigned long edi;
unsigned long esi;
unsigned long ebp;
unsigned long reserved;
unsigned long ebx;
unsigned long edx;
unsigned long ecx;
unsigned long eax;
unsigned short flags;
unsigned short es;
unsigned short ds;
unsigned short fs;
unsigned short gs;
unsigned short ip;
unsigned short cs;
unsigned short sp;
unsigned short ss;
} rm_call_struct;

typedef struct {
unsigned long largest_free;
unsigned long max_unlocked_page_alloc;
unsigned long max_locked_page_alloc;
unsigned long linear_addr_space_in_pages;
unsigned long total_unlocked_pages;
unsigned long free_pages;
unsigned long physical_pages;
unsigned long free_linear_addr_space_in_pages;
unsigned long size_of_page_file_in_pages;
unsigned long fill[4];
} dpmi_mem;

typedef struct {
char major_version;
char minor_version;
short flags;
char processor_type;
char master_pic_base_interrupt;
char slave_pic_base_interrupt;
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
    unsigned_8  accessed : 1;
    unsigned_8  rdwr     : 1;
    unsigned_8  exp_down : 1;
    unsigned_8  execute  : 1;
    unsigned_8  mustbe_1 : 1;
    unsigned_8  dpl      : 2;
    unsigned_8  present  : 1;
} dsc_type;

typedef struct {
    unsigned_8           : 4;
    unsigned_8  useravail: 1;
    unsigned_8  mustbe_0 : 1;
    unsigned_8  use32    : 1;
    unsigned_8  page_gran: 1;
} dsc_xtype;

typedef struct {
    unsigned_16         lim_0_15;
    unsigned_16         base_0_15;
    unsigned_8          base_16_23;
    dsc_type            type;
    union {
        struct {
            unsigned_8  lim_16_19: 4;
            unsigned_8           : 4;
        };
        dsc_xtype xtype;
    };
    unsigned_8          base_24_31;
} descriptor;

typedef enum {
    DPMI_WATCH_EXEC,
    DPMI_WATCH_WRITE,
    DPMI_WATCH_READWRITE
} dpmi_watch_type;

typedef long dpmi_watch_handle;

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

extern void _DPMIFreeRealModeCallBackAddress( void __far * proc );
extern void __far *_DPMIAllocateRealModeCallBackAddress( void __far * proc, rm_call_struct __far *cs );
extern void __far *_DPMIGetRealModeInterruptVector( char iv );
extern int  _DPMISetPMInterruptVector( char iv, void __far * ptr );
extern void _DPMISetPMExceptionVector( char iv, void __far * ptr );
extern void __far *_DPMIGetPMExceptionVector( char iv );
extern void __far *_DPMIGetPMInterruptVector( char iv );
extern int  _DPMISetRealModeInterruptVector( char iv, void __far * ptr );
extern short _DPMIModeDetect( void );
extern void _DPMIIdle( void );
extern void _DPMIGetVersion( version_info __far * );
extern void _fDPMIGetVersion( version_info __far * );
extern void _nDPMIGetVersion( version_info * );
extern long _DPMIAllocateLDTDescriptors( short );
extern short _DPMISegmentToDescriptor( short );
extern void _DPMIFreeLDTDescriptor( short );
extern short _DPMIGetNextSelectorIncrementValue( void );
extern long _DPMIGetSegmentBaseAddress( short );
extern void _DPMISetSegmentBaseAddress( short, long );
extern unsigned short _DPMISetSegmentLimit( short, long );
extern void _DPMISetDescriptorAccessRights( short, short );
extern short _fDPMIAllocateMemoryBlock( long __far *, long );
extern short _nDPMIAllocateMemoryBlock( long *, long );
extern short _fDPMIResizeMemoryBlock( long __far *, long, long );
extern short _nDPMIResizeMemoryBlock( long *, long, long );
extern short _DPMIFreeMemoryBlock( long );
extern short _DPMILockLinearRegion( long, long );
extern short _DPMIUnlockLinearRegion( long, long );
extern int   _DPMIGetDescriptor( unsigned short, descriptor __far * );
extern int   _DPMISetDescriptor( unsigned short, descriptor __far * );
extern long _DPMICreateCodeSegmentAliasDescriptor( short );
extern short _nDPMIGetFreeMemoryInformation( dpmi_mem * );
extern short _fDPMIGetFreeMemoryInformation( dpmi_mem __far * );
extern int _DPMISimulateRealModeInterrupt( char interrupt, char flags,
                        unsigned short words_to_copy, rm_call_struct __far *call_st );
extern long _DPMIAllocateDOSMemoryBlock( short para );
extern void _DPMIFreeDOSMemoryBlock( short sel );
extern void __far  *_DPMIRawPMtoRMAddr( void );
extern unsigned long    _DPMIRawRMtoPMAddr( void );
extern void __far  *_DPMISaveRMStateAddr( void );
extern unsigned long     _DPMISavePMStateAddr( void );
extern unsigned short      _DPMISaveStateSize( void );
extern void __far *_DPMIGetVendorSpecificAPI( char __far * );

extern dpmi_watch_handle _DPMISetWatch( unsigned long linear, char len, dpmi_watch_type type );
extern void _DPMIClearWatch( short handle );
extern short _DPMITestWatch( short handle );
extern void _DPMIResetWatch( short handle );

#pragma aux _DPMIModeDetect = \
        "mov    ax,1686h"   \
        "int 2fh"           \
        value[ax];

#pragma aux _DPMIIdle = \
        "mov    ax,1680h"   \
        "int 2fh";

#if defined(__386__)
#pragma aux _DPMISetWatch = \
        "mov    ax,0b00h"   \
        "mov    cx,bx"      \
        "shr    ebx,16"     \
        _INT_31             \
        "sbb    eax,eax"    \
        "mov    ax,bx"      \
    parm [ebx] [dl] [dh] modify [ecx] value [eax];
#else
#pragma aux _DPMISetWatch = \
        "mov    ax,0b00h"   \
        "xchg   bx,cx"      \
        _INT_31             \
        "sbb    cx,cx"      \
    parm [bx cx] [dl] [dh] value [cx bx];
#endif

#pragma aux _DPMIResetWatch = \
        "mov    ax,0b03h"   \
        _INT_31             \
    parm [bx];

#pragma aux _DPMIClearWatch = \
        "mov    ax,0b01h"   \
        _INT_31             \
    parm [bx];

#pragma aux _DPMITestWatch = \
        "mov    ax,0b02h"   \
        _INT_31             \
        "and    ax,1"       \
    parm [bx] value [ax];

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
    parm [dx esi] modify[ax bx cl dx];
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
    parm [si] modify[ax bx cl dx];

#pragma aux _fDPMIGetVersion = \
        "mov    ax,400h"    \
        _INT_31             \
        "mov    byte ptr es:[si],ah"  \
        "mov    byte ptr es:[si+1],al"  \
        "mov    word ptr es:[si+2],bx"  \
        "mov    byte ptr es:[si+4],cl"  \
        "mov    byte ptr es:[si+5],dh"  \
        "mov    byte ptr es:[si+6],dl"  \
    parm [es si] modify[ax bx cl dx];
#endif

#if defined(__386__)
#pragma aux _DPMIAllocateLDTDescriptors = \
        "xor    eax,eax"    \
        _INT_31             \
        "mov    dx,ax"      \
        "sbb    ax,ax"      \
        "shl    eax,16"     \
        "mov    ax,dx"      \
    parm [cx] value[eax] modify[dx];
#else
#pragma aux _DPMIAllocateLDTDescriptors = \
        "xor    ax,ax"      \
        _INT_31             \
        "sbb    dx,dx"      \
    parm [cx] value[ax dx];
#endif

#pragma aux _DPMIFreeLDTDescriptor = \
        "mov    ax,1"       \
        _INT_31             \
    parm [bx];

#pragma aux _DPMISegmentToDescriptor = \
        "mov    ax,2"       \
        _INT_31             \
    parm [bx];

#pragma aux _DPMIGetNextSelectorIncrementValue = \
        "mov    ax,3"       \
        _INT_31             \
    parm [bx];

#if defined(__386__)

#pragma aux _DPMIGetSegmentBaseAddress = \
        "xor    eax,eax"    \
        "mov    al,6"       \
        _INT_31             \
        "mov    eax,ecx"    \
        "shl    eax,16"     \
        "mov    ax,dx"      \
    parm caller [bx] value [eax] modify exact [eax ebx ecx edx];
#else

#pragma aux _DPMIGetSegmentBaseAddress = \
        "mov    ax,6"       \
        _INT_31             \
    parm [bx] value[cx dx];
#endif

#pragma aux _DPMISetSegmentBaseAddress = \
        "mov    ax,7"       \
        _INT_31             \
    parm [bx] [cx dx];

#if defined(__386__)
#pragma aux _DPMISetSegmentLimit = \
        "mov    ax,dx"      \
        "shr    edx,16"     \
        "mov    cx,dx"      \
        "mov    dx,ax"      \
        "mov    ax,8"       \
        _INT_31             \
        "xor    eax,eax"    \
    parm [bx] [edx] value[ax];
#else
#pragma aux _DPMISetSegmentLimit = \
        "mov    ax,8"       \
        _INT_31             \
        "sbb    ax,ax"      \
    parm [bx] [cx dx] value[ax];
#endif

#pragma aux _DPMIFreeMemoryBlock =  \
        "xchg   si,di"      \
        "mov    ax,502h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    parm [di si] value[ax];

#pragma aux _DPMISetDescriptorAccessRights = \
        "mov    ax,9"       \
        _INT_31             \
    parm [bx] [cx];

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
    parm [eax] [edx] value[ax] modify [bx cx si di];

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
    parm [eax] [edx] value[ax] modify [bx cx si di];
#else
#pragma aux _DPMILockLinearRegion = \
        "mov    ax,600h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    parm [cx bx] [si di] value[ax];

#pragma aux _DPMIUnlockLinearRegion = \
        "mov    ax,601h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    parm [cx bx] [si di] value[ax];
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
    parm [bx] value [eax] modify [dx];

#pragma aux _DPMIFreeDOSMemoryBlock = \
        "mov    ax,101h"    \
        _INT_31             \
        "sbb    eax,eax"    \
    parm [dx] value [eax];

#pragma aux _DPMISimulateRealModeInterrupt = \
        "push   es"         \
        "mov    es,edx"     \
        "mov    ax,300h"    \
        _INT_31             \
        "pop    es"         \
        "sbb    eax,eax"    \
    parm [bl] [bh] [cx] [dx edi] value [eax];

#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
        "mov    ax,0ah"     \
        _INT_31             \
        "rcl    eax,1"      \
        "ror    eax,1"      \
    parm [bx] value [eax] ;

#pragma aux _DPMIGetDescriptor = \
        "push   es"         \
        "mov    es,edx"     \
        "mov    ax,0bh"     \
        _INT_31             \
        "pop    es"         \
        "sbb    eax,eax"    \
    parm [bx] [dx edi] value [eax] ;

#pragma aux _DPMISetDescriptor = \
        "push   es"         \
        "mov    es,edx"     \
        "mov    ax,0ch"     \
        _INT_31             \
        "pop    es"         \
        "sbb    eax,eax"    \
    parm [bx] [dx edi] value [eax] ;
#else

#pragma aux _DPMIAllocateDOSMemoryBlock = \
        "mov ax,100h"       \
        _INT_31             \
        "jnc short L1"      \
        "sbb dx,dx"         \
    "L1: "                  \
    parm [bx] value [dx ax];

#pragma aux _DPMIFreeDOSMemoryBlock = \
        "mov ax,101h"       \
        _INT_31             \
        "sbb ax,ax"         \
    parm [dx] value [ax];

#pragma aux _DPMISimulateRealModeInterrupt = \
        "mov ax,300h"       \
        _INT_31             \
        "sbb ax,ax"         \
    parm [bl] [bh] [cx] [es di] value [ax];

#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
        "mov ax,0ah"        \
        _INT_31             \
        "sbb dx,dx"         \
    parm [bx] value[dx ax];

#pragma aux _DPMIGetDescriptor = \
        "mov    ax,0bh"     \
        _INT_31             \
        "sbb    ax,ax"      \
    parm [bx] [es di] value [ax] ;

#pragma aux _DPMISetDescriptor = \
        "mov    ax,0ch"     \
        _INT_31             \
        "sbb    ax,ax"      \
    parm [bx] [es di] value [ax] ;

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
    parm [ax dx] [bx cx] value[ax] modify[bx cx dx di si];

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
    parm [ax] [bx cx] value[ax] modify[bx cx dx di si];

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
    parm [dx ax] [bx cx] [di si] value[ax] modify[di si bx cx dx];

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
    parm [ax] [bx cx] [di si] value[ax] modify[di si bx cx dx];

#pragma aux _fDPMIGetFreeMemoryInformation = \
        "mov    ax,500h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    parm [es di] value[ax];

#pragma aux _nDPMIGetFreeMemoryInformation = \
        "push   es"         \
        "push   ds"         \
        "pop    es"         \
        "mov    ax,500h"    \
        _INT_31             \
        "pop    es"         \
        "sbb    ax,ax"      \
    parm [di] value[ax];

#pragma aux _DPMIGetRealModeInterruptVector = \
        "mov    ax,200h"    \
        _INT_31             \
    parm [bl] value[cx __DX] modify[ax];

#if defined(__386__)
#pragma aux _DPMISetRealModeInterruptVector = \
        "mov    ax,201h"    \
        _INT_31             \
        "sbb    eax,eax"    \
    parm [bl] [cx edx] value [eax];
#else
#pragma aux _DPMISetRealModeInterruptVector = \
        "mov    ax,201h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    parm [bl] [cx dx] value [ax];
#endif

#pragma aux _DPMIGetPMExceptionVector = \
        "mov    ax,202h"    \
        _INT_31             \
    parm [bl] value[cx __DX] modify[ax];

#pragma aux _DPMISetPMExceptionVector = \
        "mov    ax,203h"    \
        _INT_31             \
    parm [bl] [cx __DX] modify[ax];

#pragma aux _DPMIGetPMInterruptVector = \
        "mov    ax,204h"    \
        _INT_31             \
    parm [bl] value[cx __DX] modify[ax];

#if defined(__386__)
#pragma aux _DPMISetPMInterruptVector = \
        "mov    ax,205h"    \
        _INT_31             \
        "sbb    eax,eax"    \
    parm [bl] [cx edx] value [eax];
#else
#pragma aux _DPMISetPMInterruptVector = \
        "mov    ax,205h"    \
        _INT_31             \
        "sbb    ax,ax"      \
    parm [bl] [cx dx] value [ax];
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
    parm [dx eax] [cx ebx] value [cx edx] modify [edi esi];

#pragma aux _DPMIFreeRealModeCallBackAddress = \
        "mov    ax,304h"    \
        _INT_31             \
    parm [cx edx];
#else
#pragma aux _DPMIAllocateRealModeCallBackAddress = \
        "push   ds"         \
        "mov    ds,dx"      \
        "mov    si,ax"      \
        "mov    ax,303h"    \
        _INT_31             \
        "pop    ds"         \
    parm [dx ax] [es di] value [cx dx] modify [si];

#pragma aux _DPMIFreeRealModeCallBackAddress = \
        "mov    ax,304h"    \
        _INT_31             \
    parm [cx dx];
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
    value [cx edi] modify exact [eax cx si edi];

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
    value [ebx] modify exact [eax cx si edi];

#pragma aux             _DPMISaveRMStateAddr = \
        "mov    ax,305h"    \
        "stc"               \
        _INT_31             \
        "mov    cx,si"      \
        "jnc short L1"      \
        "xor    cx,cx"      \
        "xor    edi,edi"    \
    "L1: "                  \
    value [cx edi] modify exact [ax bx cx si edi];

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
    value [ebx] modify exact [ax bx cx si edi];

#pragma aux             _DPMISaveStateSize = \
        "mov    ax,305h"    \
        _INT_31             \
        "jnc short L1"      \
        "xor    eax,eax"    \
    "L1:"                   \
    value [ax] modify exact [eax bx cx si edi];

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
    parm [cx esi] value [cx edi] modify [eax ebx ecx edx esi]
#endif

#endif
