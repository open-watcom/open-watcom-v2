/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Extended memory/EMS/XMS management interface.
*
****************************************************************************/


#ifndef _XMEM_INCLUDED
#define _XMEM_INCLUDED

/*
 * Extended memory defs
 */
#define XMEM_INTERRUPT          0x15
#define XMEM_MEMORY_SIZE        0x88
#define XMEM_MEMORY_START       0x00100000L

/* VDISK definitions */
#define VDISK_INTERRUPT         0x19
#define VDISK_NAME_OFFSET       0x12
#define VDISK_AVAIL_OFFSET      0x2c

/* GDT definitions */
typedef unsigned long   flat_address;
#define MAKE_LINEAR(x)  ((((flat_address)_FP_SEG( x )) << 4) + _FP_OFF( x ))

/* read/write data, 16 bit (byte length) segment, ring 0 */
#define GDT_RW_DATA     0x93000000L
#define GDT_ADDR        0x00ffffffL

#define GDT_SOURCE      2
#define GDT_TARGET      3

#include "pushpck1.h"
typedef struct {
    unsigned short  length;
    flat_address    address;
    unsigned short  reserved;
} descriptor;
#include "poppck.h"

typedef struct {
    flat_address    offset;
    long            amount_left;
    long            allocated;
    void            *xtd_vector;
    boolbit         inuse   : 1;
} xtd_struct;

/* extd. mem pragmas */
extern long _XtdGetSize( void );
#pragma aux _XtdGetSize = \
        "clc"  /* ibm cache bug */ \
        "mov ah,88h" \
        "int 15h"    \
        "sbb dx,dx"  \
    __parm __caller     [] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

extern void _XtdMoveMemory( descriptor __far *, unsigned short );
#pragma aux _XtdMoveMemory = \
        "mov ah,87h" \
        "int 15h"    \
    __parm __caller     [__es __si] [__cx] \
    __value             \
    __modify __exact    [__ah]

/*
 * EMS defs
 */
#define EMS_INTERRUPT           0x67
#define EMS_INTERRUPT_OFFSET    0x0a
#define EMS_MAX_PHYSICAL_PAGES  4
#define EMS_MAX_LOGICAL_PAGES   4
#define EMS_MAX_HANDLES         255
#define EMS_MAX_PAGE_SIZE       0x4000

typedef struct {
    unsigned char       handle;
    unsigned char       logical : 2;
    unsigned char       used    : 1;
} physical_descriptor;

typedef union {
    xhandle             external;
    struct {
        unsigned short  offset;
        unsigned char   logical;
        unsigned char   handle;
    } internal;
} ems_addr;

typedef struct {
    unsigned short      seg;
    unsigned short      offset;
    unsigned char       allocated;
    unsigned char       logical;
    unsigned char       max_logical;
    unsigned char       handles[EMS_MAX_HANDLES];
    physical_descriptor physical[EMS_MAX_PHYSICAL_PAGES];
    boolbit             inuse       : 1;
    boolbit             exhausted   : 1;
} ems_struct;

/* ems pragmas */
extern unsigned char _EMSStatus( void );
#pragma aux _EMSStatus = \
        "mov ah,40h"    \
        "int 67h"       \
    __parm              [] \
    __value             [__ah] \
    __modify __exact    [__ah]

extern unsigned char _EMSGetPageFrame( unsigned short __far * );
#pragma aux _EMSGetPageFrame = \
        "mov ah,41h"    \
        "int 67h"       \
        "mov [di],bx"   \
    __parm __caller     [__ds __di] \
    __value             [__ah] \
    __modify __exact    [__ah __bx]

extern unsigned char _EMSAllocateMemory( unsigned short num_pages, unsigned char __far * );
#pragma aux _EMSAllocateMemory = \
        "mov ah,43h"    \
        "int 67h"       \
        "mov [di],dl"   \
    __parm __caller     [__bx] [__ds __di] \
    __value             [__ah] \
    __modify __exact    [__ah __bx __dx]

extern unsigned char _EMSMapMemory( unsigned short, unsigned short, unsigned char );
#pragma aux _EMSMapMemory = \
        "mov ah,44h"    \
        "int 67h"       \
    __parm __caller     [__dx] [__bx] [__al] \
    __value             [__ah] \
    __modify __exact    [__ah]

extern unsigned char _EMSReleaseMemory( unsigned short );
#pragma aux _EMSReleaseMemory = \
        "mov ah,45h"    \
        "int 67h"       \
    __parm __caller     [__dx] \
    __value             [__ah] \
    __modify __exact    [__ah]

/*
 * XMS definitions
 */
#define XMS_INTERRUPT           0x2f
#define XMS_INSTALLED           0x80
#define XMS_REQD_VERSION        0x0200
#define XMS_APPLICATION_AMT     0xfff0
#define XMS_ERROR               0x80
#define XMS_REAL_HANDLE         0

#define XMS_HMA_HANDLE          XMS_REAL_HANDLE
#define XMS_HMA_SEGMENT         0xffff
#define XMS_HMA_INITIAL_OFFSET  0x0010
#define XMS_HMA_BLOCK_SIZE      0xfff0

#define XMS_MAX_BLOCK_SIZE              0x00010000L
#define XMS_MAX_BLOCK_SIZE_IN_K         64
#define XMS_BLOCK_ADJUST_SIZE_IN_K      4
#define XMS_MAX_HANDLES                 256

typedef union {
    xhandle             external;
    struct {
        unsigned short  handle;
        unsigned short  offset  : 14;
        unsigned short  fill    : 2;
    } internal;
} xms_addr;

typedef struct {
    unsigned long       offset;
    unsigned long       size;
    unsigned short      handles[XMS_MAX_HANDLES];
    unsigned short      next_handle;
    boolbit             inuse       : 1;
    boolbit             exhausted   : 1;
    boolbit             small_block : 1;
} xms_struct;

#include "pushpck1.h"
typedef struct {
    unsigned long               size;
    unsigned short              src_handle;
    union {
        unsigned long           extended;
        void                    *real;
    } src_offset;
    unsigned short              dest_handle;
    union {
        unsigned long           extended;
        void                    *real;
    } dest_offset;
} xms_move_descriptor;
#include "poppck.h"

extern unsigned char _XMSInstalled( void );
#pragma aux _XMSInstalled = \
        "mov ah,43h"        \
        "mov al,0"          \
        "int 2fh"           \
    __parm              [] \
    __value             [__al] \
    __modify __exact    [__ax]

extern void *_XMSControl( void );
#pragma aux _XMSControl =   \
        "mov ah,43h"        \
        "mov al,10h"        \
        "int 2fh"           \
    __parm              [] \
    __value             [__es __bx] \
    __modify __exact    [__ax __bx __es]

extern unsigned short _XMSVersion( void * __far * );
#pragma aux _XMSVersion =   \
        "mov ah,0"          \
        "call far ptr [si]" \
    __parm __caller     [__ds __si] \
    __value             [__ax] \
    __modify __exact    [__ax __bx __dx]

extern unsigned short _XMSRequestHMA( void * __far *, unsigned short amt );
#pragma aux _XMSRequestHMA = \
        "mov ah,1"          \
        "call far ptr [si]" \
    __parm __caller     [__ds __si] [__dx] \
    __value             [__ax] \
    __modify __exact    [__ax __bl __dx]

extern unsigned short _XMSReleaseHMA( void * __far * );
#pragma aux _XMSReleaseHMA = \
        "mov ah,2"          \
        "call far ptr [si]" \
    __parm __caller     [__ds __si] \
    __value             [__ax] \
    __modify __exact    [__ax __bl]

extern unsigned short _XMSQuerySize( void * __far * );
#pragma aux _XMSQuerySize = \
        "mov ah,8"          \
        "call far ptr [si]" \
    __parm __caller     [__ds __si] \
    __value             [__dx] \
    __modify __exact    [__ax __bl __dx]

extern unsigned short _XMSAllocate( void * __far *, unsigned short amt, unsigned short __far * );
#pragma aux _XMSAllocate =  \
        "mov ah,9"          \
        "call far ptr [si]" \
        "mov es:[di],dx"    \
    __parm __caller     [__ds __si] [__dx] [__es __di] \
    __value             [__ax] \
    __modify __exact    [__ax __bl __dx]

extern unsigned short _XMSFree( void * __far *, unsigned short );
#pragma aux _XMSFree =      \
        "mov ah,0ah"        \
        "call far ptr [si]" \
    __parm __caller     [__ds __si] [__dx] \
    __value             [__ax] \
    __modify __exact    [__ax __bl]

extern unsigned short _XMSReallocate( void * __far *, unsigned short, unsigned short );
#pragma aux _XMSReallocate = \
        "mov ah,0fh"        \
        "call far ptr [si]" \
    __parm __caller     [__ds __si] [__dx] [__bx] \
    __value             [__ax] \
    __modify __exact    [__ax __bl]


extern unsigned short _XMSMove( void * __far *, xms_move_descriptor __far * );
#pragma aux _XMSMove =      \
        "mov ah,0bh"        \
        "call far ptr es:[di]" \
    __parm __caller     [__es __di] [__ds __si] \
    __value             [__ax] \
    __modify __exact    [__ax __bl]

extern unsigned short _XMSEnableA20( void * __far * );
#pragma aux _XMSEnableA20 = \
        "mov ah,5"          \
        "call far ptr [si]" \
    __parm __caller     [__ds __si] \
    __value             [__ax] \
    __modify __exact    [__ax __bl]

extern unsigned short _XMSDisableA20( void * __far * );
#pragma aux _XMSDisableA20 = \
        "mov ah,6"          \
        "call far ptr [si]" \
    __parm __caller     [__ds __si] \
    __value             [__ax] \
    __modify __exact    [__ax __bl]

extern unsigned char _XMSOK( void * __far * );
#pragma aux _XMSOK =        \
        "mov ah,8"          \
        "call far ptr [si]" \
    __parm __caller     [__ds __si] \
    __value             [__bl] \
    __modify __exact    [__ax __bl __dx]

extern unsigned short _XMSQueryHandles( void * __far *, unsigned short, unsigned short __far * );
#pragma aux _XMSQueryHandles = \
        "mov ah,0eh"        \
        "call far ptr [si]" \
        "mov es:[di],dx"    \
    __parm __caller     [__ds __si] [__dx] [__es __di] \
    __value             [__ax] \
    __modify __exact    [__ax __bx __dx]

extern void _XMSCopyWords( void __far *, void __far *, unsigned short );
#pragma aux _XMSCopyWords = \
        "push ds"           \
        "mov  ds,dx"        \
        "rep  movsw"        \
        "pop  ds"           \
    __parm __caller     [__dx __si] [__es __di] [__cx] \
    __value             \
    __modify __exact    [__si __di __cx]

#endif
