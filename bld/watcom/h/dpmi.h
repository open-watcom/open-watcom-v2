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


#ifndef __DPMI_H
#define __DPMI_H
#ifdef __OSI__
 extern  void   __Int31();
 #define _INT_31        "call __Int31"
#else
 #define _INT_31        "int 0x31"
#endif

#include "watcom.h"

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
#define DPMIModeDetect                          _DPMIModeDetect
#define DPMIRawRMtoPMAddr                       _DPMIRawRMtoPMAddr
#define DPMIRawPMtoRMAddr                       _DPMIRawPMtoRMAddr
#define DPMISaveRMStateAddr                     _DPMISaveRMStateAddr
#define DPMISavePMStateAddr                     _DPMISavePMStateAddr
#define DPMISaveStateSize                       _DPMISaveStateSize
#define DPMIGetVenderSpecificAPI                _DPMIGetVenderSpecificAPI

#if defined( M_I86SM ) || defined( M_I86MM ) || defined( __386__ )
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
extern void _DPMISetPMInterruptVector( char iv, void __far * ptr );
extern void _DPMISetPMExceptionVector( char iv, void __far * ptr );
extern void __far *_DPMIGetPMExceptionVector( char iv );
extern void __far *_DPMIGetPMInterruptVector( char iv );
extern void _DPMISetRealModeInterruptVector( char iv, void __far * ptr );
extern short _DPMIModeDetect( void );
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
void __far  *_DPMIRawPMtoRMAddr( void );
unsigned long    _DPMIRawRMtoPMAddr( void );
void __far  *_DPMISaveRMStateAddr( void );
unsigned long     _DPMISavePMStateAddr( void );
unsigned short      _DPMISaveStateSize( void );
void __far *_DPMIGetVenderSpecificAPI( char __far * );

typedef enum {
    DPMI_WATCH_EXEC,
    DPMI_WATCH_WRITE,
    DPMI_WATCH_READWRITE
} dpmi_watch_type;

typedef long dpmi_watch_handle;

extern dpmi_watch_handle _DPMISetWatch( unsigned long linear, char len, dpmi_watch_type type );
extern void _DPMIClearWatch( short handle );
extern short _DPMITestWatch( short handle );
extern void _DPMIResetWatch( short handle );

#pragma aux _DPMIModeDetect = \
        "mov ax,1686h"  \
        "int 2fh"  \
        value[ax];

#pragma aux _DPMISetWatch = \
        "mov    ax,0b00h" \
        "xchg   bx,cx"  \
        _INT_31         \
        "sbb    cx,cx" \
        parm[bx cx] [ dl ] [ dh ] value [ cx bx ];

#pragma aux _DPMIResetWatch = \
        "mov ax,0b03h" \
        _INT_31 \
        parm[ bx ];

#pragma aux _DPMIClearWatch = \
        "mov ax,0b01h" \
        _INT_31 \
        parm[ bx ];

#pragma aux _DPMITestWatch = \
        "mov ax,0b02h" \
        _INT_31 \
        "and ax,1" \
        parm[ bx ] value[ ax ];

#if defined(__386__)
#pragma aux _DPMIGetVersion = \
                                 " push ds " \
                                 " mov ds,dx " \
      /*0xb8 0x00 0x04*/         " mov ax,0400h "  \
      /*0xcd 0x31*/              _INT_31 \
      /*0x88 0x24*/              " mov  byte ptr [esi],ah "  \
      /*0x88 0x44 0x01*/         " mov  byte ptr [esi+1],al "  \
      /*0x89 0x5C 0x02*/         " mov  word ptr [esi+2],bx "  \
      /*0x88 0x4C 0x04*/         " mov  byte ptr [esi+4],cl "  \
                                 " mov  byte ptr [esi+5],dh "  \
                                 " mov  byte ptr [esi+6],dl "  \
                                 " pop ds " \
        parm[dx esi] modify[ax bx cl dx];
#else
#pragma aux _nDPMIGetVersion = \
      /*0xb8 0x00 0x04*/         " mov ax,0400h "  \
      /*0xcd 0x31*/              _INT_31 \
      /*0x88 0x24*/              " mov  byte ptr [si],ah "  \
      /*0x88 0x44 0x01*/         " mov  byte ptr [si+1],al "  \
      /*0x89 0x5C 0x02*/         " mov  word ptr [si+2],bx "  \
      /*0x88 0x4C 0x04*/         " mov  byte ptr [si+4],cl "  \
                                 " mov  byte ptr [si+5],dh "  \
                                 " mov  byte ptr [si+6],dl "  \
        parm[si] modify[ax bx cl dx];

#pragma aux _fDPMIGetVersion = \
      /*0xb8 0x00 0x04*/         " mov ax,0400h "  \
      /*0xcd 0x31*/              _INT_31 \
      /*0x26 0x88 0x24*/         " mov  byte ptr es:[si],ah "  \
      /*0x26 0x88 0x44 0x01*/    " mov  byte ptr es:[si+1],al "  \
      /*0x26 0x89 0x5C 0x02*/    " mov  word ptr es:[si+2],bx "  \
      /*0x26 0x88 0x4C 0x04*/    " mov  byte ptr es:[si+4],cl "  \
                                 " mov  byte ptr es:[si+5],dh "  \
                                 " mov  byte ptr es:[si+6],dl "  \
        parm[es si] modify[ax bx cl dx];
#endif

#if defined(__386__)
#pragma aux _DPMIAllocateLDTDescriptors = \
      /*0xb8 0x00 0x00*/        " mov ax,0 "  \
      /*0xcd 0x31*/             _INT_31 \
                                " mov dx,ax " \
                                " sbb ax,ax "  \
                                " shl eax,16 " \
                                " mov ax,dx " \
        parm [cx] value[eax] modify[dx];
#else
#pragma aux _DPMIAllocateLDTDescriptors = \
      /*0xb8 0x00 0x00*/        " mov ax,0 "  \
      /*0xcd 0x31*/             _INT_31 \
      /*0x1b 0xd2*/             " sbb dx,dx "  \
        parm [cx] value[ax dx];
#endif

#pragma aux _DPMIFreeLDTDescriptor = \
      /*0xb8 0x01 0x00*/        " mov ax,1 "  \
      /*0xcd 0x31*/             _INT_31 \
        parm [bx];

#pragma aux _DPMISegmentToDescriptor = \
      /*0xb8 0x02 0x00*/        " mov ax,2 "  \
      /*0xcd 0x31*/             _INT_31 \
        parm [bx];

#pragma aux _DPMIGetNextSelectorIncrementValue = \
      /*0xb8 0x03 0x00*/        " mov ax,3 "  \
      /*0xcd 0x31*/             _INT_31 \
        parm [bx];

#if defined(__386__)

#pragma aux _DPMIGetSegmentBaseAddress = \
        "mov ah,0"      \
        "mov al,6"      \
        _INT_31 \
        "mov eax,ecx"   \
        "shl eax,16"    \
        "mov ax,dx"     \
        parm caller     [bx] \
        value           [eax] \
        modify exact    [eax ebx ecx edx];
#else

#pragma aux _DPMIGetSegmentBaseAddress = \
      /*0xb8 0x06 0x00*/        " mov ax,6 "  \
      /*0xcd 0x31*/             _INT_31 \
        parm [bx] value[cx dx];
#endif

#pragma aux _DPMISetSegmentBaseAddress = \
      /*0xb8 0x07 0x00*/        " mov ax,7 "  \
      /*0xcd 0x31*/             _INT_31 \
        parm [bx] [cx dx];

#if defined(__386__)
#pragma aux _DPMISetSegmentLimit = \
                                " mov ax,dx " \
                                " shr edx,16 " \
                                " mov cx,dx " \
                                " mov dx,ax " \
      /*0xb8 0x08 0x00*/        " mov ax,8 " \
      /*0xcd 0x31*/             _INT_31 \
      /*0x1B 0xC0*/             " sbb ax,ax " \
        parm [bx] [edx] value[ax];
#else
#pragma aux _DPMISetSegmentLimit = \
      /*0xb8 0x08 0x00*/        " mov ax,8 "  \
      /*0xcd 0x31*/             _INT_31 \
      /*0x1B 0xC0*/             " sbb ax,ax "  \
        parm [bx] [cx dx] value[ax];
#endif

#pragma aux _DPMIFreeMemoryBlock =  \
      /*0x87 0xF7*/        " xchg si,di "  \
      /*0xB8 0x02 0x05*/   " mov ax,0502h  "  \
      /*0xCD 0x31*/        _INT_31 \
      /*0x1B 0xC0*/        " sbb ax,ax "  \
        parm [di si] value[ax];

#pragma aux _DPMISetDescriptorAccessRights = \
      /*0xb8 0x09 0x00*/   " mov ax,9 "  \
      /*0xcd 0x31*/        _INT_31 \
        parm [bx] [cx];

#if defined(__386__)
#pragma aux _DPMILockLinearRegion = \
        "mov di,dx"     \
        "shr edx,16"    \
        "mov si,dx"     \
        "mov bx,ax"     \
        "shr eax,16"    \
        "mov cx,ax"     \
        "mov ax,0600h"  \
        _INT_31 \
        "sbb ax,ax"     \
        parm [eax] [edx] value[ax] modify [bx cx si di];

#pragma aux _DPMIUnlockLinearRegion = \
        "mov di,dx"     \
        "shr edx,16"    \
        "mov si,dx"     \
        "mov bx,ax"     \
        "shr eax,16"    \
        "mov cx,ax"     \
        "mov ax,0600h"  \
        _INT_31 \
        "sbb ax,ax"     \
        parm [eax] [edx] value[ax] modify [bx cx si di];
#else
#pragma aux _DPMILockLinearRegion = \
      /*0xb8 0x00 0x06*/   " mov ax,0600h "  \
      /*0xcd 0x31*/        _INT_31 \
      /*0x1B 0xC0*/        " sbb ax,ax "  \
        parm [cx bx] [si di] value[ax];

#pragma aux _DPMIUnlockLinearRegion = \
      /*0xb8 0x01 0x06*/   " mov ax,0601h "  \
      /*0xcd 0x31*/        _INT_31 \
      /*0x1B 0xC0*/        " sbb ax,ax "  \
        parm [cx bx] [si di] value[ax];
#endif

#if defined(__386__)

#pragma aux _DPMIAllocateDOSMemoryBlock = \
      /*0xB8 0x00 0x01*/   " mov  ax,0100h  "  \
      /*0xCD 0x31*/        _INT_31 \
      /*0x73 0x02*/        " jnc  short ok "  \
      /*0x2B 0xD2*/        " sub  dx,dx "  \
                       " ok: xchg ax,dx "  \
                           " shl  eax,16 "  \
                           " mov  ax,dx "  \
        parm[bx] value [eax] modify [dx];

#pragma aux _DPMIFreeDOSMemoryBlock = \
      /*0xB8 0x01 0x01*/   " mov ax,0101h  "  \
      /*0xCD 0x31*/        _INT_31 \
      /*0x1B 0xC0*/        " sbb eax,eax "  \
        parm[dx] value [eax];

#pragma aux _DPMISimulateRealModeInterrupt = \
                        "push es"               \
                        "mov es,dx"             \
      /*0xB8 0x00 0x03*/   " mov ax,0300h  "  \
      /*0xCD 0x31*/        _INT_31 \
                        "pop es"                \
      /*0x1B 0xC0*/        " sbb eax,eax "  \
        parm[bl] [bh] [cx] [dx edi] value [eax];

#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
      /*0xb8 0x0a 0x00*/   " mov ax,000ah "  \
      /*0xCD 0x31*/        _INT_31 \
                            "rcl eax,1" \
                            "ror eax,1" \
        parm[bx] value [eax] ;

#pragma aux _DPMIGetDescriptor = \
        "push   es"             \
        "mov    es,dx"          \
        "mov    ax,000bh"       \
        _INT_31 \
        "pop    es"             \
        "sbb    eax,eax"        \
        parm[bx] [dx edi] value [eax] ;

#pragma aux _DPMISetDescriptor = \
        "push   es"             \
        "mov    es,dx"          \
        "mov    ax,000ch"       \
        _INT_31 \
        "pop    es"             \
        "sbb    eax,eax"        \
        parm[bx] [dx edi] value [eax] ;
#else

#pragma aux _DPMIAllocateDOSMemoryBlock = \
      /*0xB8 0x00 0x01*/   " mov ax,0100h  "  \
      /*0xCD 0x31*/        _INT_31 \
        0x73 0x02         /* jnc short ok */ \
      /*0x2B 0xD2*/        " sub dx,dx "  \
        parm[bx] value [dx ax];

#pragma aux _DPMIFreeDOSMemoryBlock = \
      /*0xB8 0x01 0x01*/   " mov ax,0101h  "  \
      /*0xCD 0x31*/        _INT_31 \
      /*0x1B 0xC0*/        " sbb ax,ax "  \
        parm[dx] value [ax];

#pragma aux _DPMISimulateRealModeInterrupt = \
      /*0xB8 0x00 0x03*/   " mov ax,0300h  "  \
      /*0xCD 0x31*/        _INT_31 \
      /*0x1B 0xC0*/        " sbb ax,ax "  \
        parm[bl] [bh] [cx] [es di] value [ax];

#pragma aux _DPMICreateCodeSegmentAliasDescriptor = \
      /*0xb8 0x0a 0x00*/   " mov ax,000ah "  \
      /*0xcd 0x31*/        _INT_31 \
      /*0x1b 0xd2*/        " sbb dx,dx "  \
        parm[bx] value[dx ax];

#pragma aux _DPMIGetDescriptor = \
        "mov    ax,000bh"       \
        _INT_31 \
        "sbb    ax,ax"          \
        parm[bx] [es di] value [ax] ;

#pragma aux _DPMISetDescriptor = \
        "mov    ax,000ch"       \
        _INT_31 \
        "sbb    ax,ax"          \
        parm[bx] [es di] value [ax] ;

#endif

#pragma aux _fDPMIAllocateMemoryBlock =  \
      /*0x06*/                   " push    es "  \
      /*0x50*/                   " push    ax "  \
      /*0x52*/                   " push    dx "  \
      /*0x87 0xd9*/              " xchg    bx,cx "  \
      /*0xb8 0x01 0x05*/         " mov ax,0501H "  \
      /*0xcd 0x31*/              _INT_31 \
      /*0x1b 0xc0*/              " sbb ax,ax "  \
      /*0x8b 0xd3*/              " mov dx,bx "  \
      /*0x07*/                   " pop es "  \
      /*0x5b*/                   " pop bx "  \
      /*0x26 0x89 0x0f*/         " mov es:[bx],cx "  \
      /*0x26 0x89 0x57 0x02*/    " mov es:+2H[bx],dx "  \
      /*0x26 0x89 0x7f 0x04*/    " mov es:+4H[bx],di "  \
      /*0x26 0x89 0x77 0x06*/    " mov es:+6H[bx],si "  \
      /*0x07*/                   " pop es "  \
        parm [ax dx] [bx cx]  \
        value[ax] modify[bx cx dx di si];

#pragma aux _nDPMIAllocateMemoryBlock =  \
      /*0x50*/                   " push    ax "  \
      /*0x87 0xd9*/              " xchg    bx,cx "  \
      /*0xb8 0x01 0x05*/         " mov ax,0501H "  \
      /*0xcd 0x31*/              _INT_31 \
      /*0x1b 0xc0*/              " sbb ax,ax "  \
      /*0x8b 0xd3*/              " mov dx,bx "  \
      /*0x5b*/                   " pop bx "  \
      /*0x89 0x0F*/              " mov ds:[bx],cx "  \
      /*0x89 0x57 0x02*/         " mov ds:+2H[bx],dx  "  \
      /*0x89 0x7F 0x04*/         " mov ds:+4H[bx],di "  \
      /*0x89 0x77 0x06*/         " mov ds:+6H[bx],si  "  \
        parm [ax] [bx cx]  \
        value[ax] modify[bx cx dx di si];

#pragma aux _fDPMIResizeMemoryBlock =  \
      /*0x06*/                   " push es "  \
      /*0x50*/                   " push ax "  \
      /*0x52*/                   " push dx "  \
      /*0x87 0xF7*/              " xchg si,di "  \
      /*0x87 0xd9*/              " xchg bx,cx "  \
      /*0xB8 0x03 0x05*/         " mov ax,0503h  "  \
      /*0xCD 0x31*/              _INT_31 \
      /*0x1B 0xC0*/              " sbb ax,ax "  \
      /*0x8b 0xd3*/              " mov dx,bx "  \
      /*0x07*/                   " pop es "  \
      /*0x5b*/                   " pop bx "  \
      /*0x26 0x89 0x0f*/         " mov es:[bx],cx "  \
      /*0x26 0x89 0x57 0x02*/    " mov es:+2H[bx],dx "  \
      /*0x26 0x89 0x7f 0x04*/    " mov es:+4H[bx],di "  \
      /*0x26 0x89 0x77 0x06*/    " mov es:+6H[bx],si "  \
      /*0x07*/                   " pop es "  \
        parm [dx ax] [bx cx] [di si] \
        value[ax] modify[di si bx cx dx];

#pragma aux _nDPMIResizeMemoryBlock =  \
      /*0x50*/             " push ax "  \
      /*0x87 0xF7*/        " xchg si,di "  \
      /*0x87 0xd9*/        " xchg bx,cx "  \
      /*0xB8 0x03 0x05*/   " mov ax,0503h  "  \
      /*0xCD 0x31*/        _INT_31 \
      /*0x1B 0xC0*/        " sbb ax,ax "  \
      /*0x8b 0xd3*/        " mov dx,bx "  \
      /*0x5b*/             " pop bx "  \
      /*0x89 0x0F*/        " mov ds:[bx],cx "  \
      /*0x89 0x57 0x02*/   " mov ds:+2H[bx],dx  "  \
      /*0x89 0x7F 0x04*/   " mov ds:+4H[bx],di "  \
      /*0x89 0x77 0x06*/   " mov ds:+6H[bx],si  "  \
        parm [ax] [bx cx] [di si] \
        value[ax] modify[di si bx cx dx];

#pragma aux _fDPMIGetFreeMemoryInformation = \
      /*0xb8 0x00 0x05*/   " mov ax,0500h "  \
      /*0xcd 0x31*/        _INT_31 \
      /*0x1B 0xC0*/        " sbb ax,ax "  \
        parm[es di] value[ax];

#pragma aux _nDPMIGetFreeMemoryInformation = \
      /*0x06*/             " push es "  \
      /*0x1E*/             " push ds "  \
      /*0x07*/             " pop es "  \
      /*0xb8 0x00 0x05*/   " mov ax,0500h "  \
      /*0xcd 0x31*/        _INT_31 \
      /*0x07*/             " pop es "  \
      /*0x1B 0xC0*/        " sbb ax,ax "  \
        parm[di] value[ax];

#if defined(__386__)
#define __DX    edx
#else
#define __DX    dx
#endif

#pragma aux _DPMIGetRealModeInterruptVector = \
        "mov    ax,0200h" \
        _INT_31 \
        parm [bl] value[cx __DX] modify[ax];

#pragma aux _DPMISetRealModeInterruptVector = \
        "mov    ax,0201h" \
        _INT_31 \
        parm [bl] [cx __DX] modify[ax];

#pragma aux _DPMIGetPMExceptionVector = \
        "mov    ax,0202h" \
        _INT_31 \
        parm [bl] value[cx __DX] modify[ax];

#pragma aux _DPMISetPMExceptionVector = \
        "mov    ax,0203h" \
        _INT_31 \
        parm [bl] [cx __DX] modify[ax];

#pragma aux _DPMIGetPMInterruptVector = \
        "mov    ax,0204h" \
        _INT_31 \
        parm [bl] value[cx __DX] modify[ax];

#pragma aux _DPMISetPMInterruptVector = \
        "mov    ax,0205h" \
        _INT_31 \
        parm [bl] [cx __DX] modify[ax];

#if defined(__386__)
#pragma aux _DPMIAllocateRealModeCallBackAddress = \
        "push   es" \
        "push   ds" \
        "mov    ds,dx" \
        "mov    esi,eax" \
        "mov    es,cx" \
        "mov    edi,ebx" \
        "mov    ax,0303h" \
        _INT_31 \
        "pop    ds" \
        "pop    es" \
        parm[dx eax] [cx ebx] value[cx edx] modify[edi esi];

#pragma aux _DPMIFreeRealModeCallBackAddress = \
        "mov    ax,0304h" \
        _INT_31 \
        parm[cx edx];
#else
#pragma aux _DPMIAllocateRealModeCallBackAddress = \
        "push   ds" \
        "mov    ds,dx" \
        "mov    si,ax" \
        "mov    ax,0303h" \
        _INT_31 \
        "pop    ds" \
        parm[dx ax] [es di] value[cx dx] modify[si];

#pragma aux _DPMIFreeRealModeCallBackAddress = \
        "mov    ax,0304h" \
        _INT_31 \
        parm[cx dx];
#endif

#if defined(__386__)
#pragma aux             _DPMIRawPMtoRMAddr = \
        "mov ah,3"      \
        "mov al,6"      \
        "xor edi,edi"   \
        "stc"           \
        _INT_31 \
        "mov cx,si"     \
        "jnc L1"        \
        "xor cx,cx"     \
        "xor edi,edi"   \
        "L1:"           \
        value           [cx edi] \
        modify exact    [eax cx si edi];

#pragma aux             _DPMIRawRMtoPMAddr = \
        "mov ah,3"      \
        "mov al,6"      \
        "stc"           \
        _INT_31 \
        "jnc L1"        \
        "xor ebx,ebx"   \
        "jmp short L2"  \
        "L1:"           \
        "shl ebx,16"    \
        "mov bx,cx"     \
        "L2:"           \
        value           [ebx] \
        modify exact    [eax cx si edi];

#pragma aux             _DPMISaveRMStateAddr = \
        "mov ah,3"      \
        "mov al,5"      \
        "stc"           \
        _INT_31 \
        "mov cx,si"     \
        "jnc L1"        \
        "xor cx,cx"     \
        "xor edi,edi"   \
        "L1:"           \
        value           [cx edi] \
        modify exact    [ax bx cx si edi];

#pragma aux             _DPMISavePMStateAddr = \
        "mov ah,3"      \
        "mov al,5"      \
        _INT_31 \
        "jnc L1"        \
        "xor cx,cx"     \
        "xor ebx,ebx"   \
        "jmp short L2"  \
        "L1:"           \
        "shl ebx,16"    \
        "mov bx,cx"     \
        "L2:"           \
        value           [ebx] \
        modify exact    [ax bx cx si edi];

#pragma aux             _DPMISaveStateSize = \
        "mov ah,3"      \
        "mov al,5"      \
        _INT_31 \
        "jnc L1"        \
        "xor eax,eax"   \
        "L1:"           \
        value           [ax] \
        modify exact    [eax bx cx si edi];

#pragma aux             _DPMIGetVenderSpecificAPI = \
        "push ds"               \
        "push es"               \
        "push fs"               \
        "push gs"               \
        "push ebp"              \
        "mov  ds,cx"            \
        "mov ah,0x0a"           \
        "mov al,0"              \
        _INT_31                 \
        "mov cx,es"             \
        "jnc L1"                \
        "xor ecx,ecx"           \
        "xor edi,edi"           \
        "L1:"                   \
        "pop ebp"               \
        "pop gs"                \
        "pop fs"                \
        "pop es"                \
        "pop ds"                \
        parm [cx esi] value [cx edi] modify [eax ebx ecx edx esi]
#endif

#endif
