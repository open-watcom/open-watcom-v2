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
#define MAKE_LINEAR(x)  ((((flat_address) FP_SEG( x )) << 4) + FP_OFF( x ))

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
    bool            inuse   : 1;
} xtd_struct;

/* extd. mem pragmas */
extern long _XtdGetSize( void );
#pragma aux _XtdGetSize = \
        0xf8         /* clc  ; ibm cache bug */ \
        0xb4 0x88    /* mov ah, 87h */ \
        0xcd 0x15    /* int 15h */ \
        0x19 0xd2    /* sbb dx,dx  ; makes result negative if carry set */ \
    parm caller [] value [ax dx];

extern void _XtdMoveMemory( descriptor __far *, unsigned short );
#pragma aux _XtdMoveMemory = \
        0xb4 0x87    /* mov ah, 87h */ \
        0xcd 0x15    /* int 15h */ \
    parm caller [es si] [cx];

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
    long external;
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
    bool                inuse       : 1;
    bool                exhausted   : 1;
} ems_struct;

/* ems pragmas */
extern unsigned char _EMSStatus( void );
#pragma aux _EMSStatus = \
        0xb4 0x40   /* mov ah, 40h   ;EMS status */ \
        0xcd 0x67   /* int 67h */ \
    parm value [ah] modify exact [ax];

extern unsigned char _EMSGetPageFrame( unsigned short __far * );
#pragma aux _EMSGetPageFrame = \
        0xb4 0x41   /* mov ah, 41h   ;EMS get page frame */ \
        0xcd 0x67   /* int 67h */ \
        0x89 0x1d   /* mov [di], bx */ \
    parm caller [ds di] value [ah] modify exact [ax bx];

extern unsigned char _EMSAllocateMemory( unsigned short num_pages, unsigned char __far * );
#pragma aux _EMSAllocateMemory = \
        0xb4 0x43   /* mov ah, 43h   ;alloc EMS */ \
        0xcd 0x67   /* int 67h */ \
        0x88 0x15   /* mov [di], dl */  \
    parm caller [bx] [ds di] value [ah] modify exact [ax bx dx];

extern unsigned char _EMSMapMemory( unsigned char, unsigned char, unsigned char );
#pragma aux _EMSMapMemory = \
        0xb4 0x44   /* mov ah, 44h   ;EMS map memory */ \
        0xcd 0x67   /* int 67h */ \
    parm caller [dx] [bx] [al] value [ah] modify exact [ax bx dx];

extern unsigned char _EMSReleaseMemory( unsigned char );
#pragma aux _EMSReleaseMemory = \
        0xb4 0x45   /* mov ah, 45h   ;EMS release */ \
        0xcd 0x67   /* int 67h */ \
    parm caller [dx] value [ah] modify exact [ax dx];

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
    long    external;
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
    bool                inuse       : 1;
    bool                exhausted   : 1;
    bool                small_block : 1;
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
        0xb4 0x43       /* mov ah, 43h */ \
        0xb0 0x00       /* mov al, 0h */ \
        0xcd 0x2f       /* int 2fh */  \
    parm caller [] value [al] modify exact [ax];

extern void *_XMSControl( void );
#pragma aux _XMSControl = \
        0xb4 0x43       /* mov ah, 43h */\
        0xb0 0x10       /* mov al, 10h */\
        0xcd 0x2f       /* int 2fh */  \
    parm caller [] value [es bx] modify exact [ax bx es];

extern unsigned short _XMSVersion( void * __far * );
#pragma aux _XMSVersion = \
        0xb4 0x00       /* mov ah, 0h */ \
        0xff 0x1c       /* call far [si] */ \
    parm caller [ds si] value [ax] modify exact [ax bx dx];

extern unsigned short _XMSRequestHMA( void * __far *, unsigned short amt );
#pragma aux _XMSRequestHMA = \
        0xb4 0x01       /* mov ah, 1h */ \
        0xff 0x1c       /* call far [si] */ \
    parm caller [ds si] [dx] value [ax] modify exact [ax bx dx];

extern unsigned short _XMSReleaseHMA( void * __far * );
#pragma aux _XMSReleaseHMA = \
        0xb4 0x02       /* mov ah, 2h */\
        0xff 0x1c       /* call far [si] */ \
    parm caller [ds si] value [ax] modify exact [ax bx];

extern unsigned short _XMSQuerySize( void * __far * );
#pragma aux _XMSQuerySize = \
        0xb4 0x08       /* mov ah, 8h */\
        0xff 0x1c       /* call far [si] */ \
    parm caller [ds si] value [dx] modify exact [ax bx dx];

extern unsigned short _XMSAllocate( void * __far *, unsigned short amt, unsigned short __far * );
#pragma aux _XMSAllocate = \
        0xb4 0x09       /* mov ah, 9h */\
        0xff 0x1c       /* call far [si] */ \
        0x26 0x89 0x15  /* mov es:[di], dx  */ \
    parm caller [ds si] [dx] [es di] value [ax] modify exact [ax bx dx];

extern unsigned short _XMSFree( void * __far *, unsigned short );
#pragma aux _XMSFree = \
        0xb4 0x0a       /* mov ah, 0ah */ \
        0xff 0x1c       /* call far [si] */ \
    parm caller [ds si] [dx] value [ax] modify exact [ax bx dx];

extern unsigned short _XMSReallocate( void * __far *, unsigned short, unsigned short );
#pragma aux _XMSReallocate = \
        0xb4 0x0f       /* mov ah, 0fh */ \
        0xff 0x1c       /* call far [si] */ \
    parm caller [ds si] [dx] [bx] value [ax] modify exact [ax bx dx];


extern unsigned short _XMSMove( void * __far *, xms_move_descriptor __far * );
#pragma aux _XMSMove = \
        0xb4 0x0b       /* mov ah, 0bh */ \
        0x26 0xff 0x1d  /* call far es:[di] */ \
    parm caller [es di] [ds si] value [ax] \
    modify exact [ax bx dx si ds ];

extern unsigned short _XMSEnableA20( void * __far * );
#pragma aux _XMSEnableA20 = \
        0xb4 0x05       /* mov ah, 05h */ \
        0xff 0x1c       /* call far [si] */ \
    parm caller [ds si] value [ax] modify exact [ax bx dx];

extern unsigned short _XMSDisableA20( void * __far * );
#pragma aux _XMSDisableA20 = \
        0xb4 0x06       /* mov ah, 06h */ \
        0xff 0x1c       /* call far [si] */ \
    parm caller [ds si] value [ax] modify exact [ax bx dx];

extern unsigned char _XMSOK( void * __far * );
#pragma aux _XMSOK = \
        0xb4 0x08       /* mov ah, 08h */ \
        0xff 0x1c       /* call far [si] */ \
    parm caller [ds si] value [bl] modify exact [ax bx dx];

extern unsigned short _XMSQueryHandles( void * __far *, unsigned short, unsigned short __far * );
#pragma aux _XMSQueryHandles = \
        0xb4 0x0e       /* mov ah, 0eh */ \
        0xff 0x1c       /* call far [si] */ \
        0x26 0x89 0x15  /* mov es:[di], dx */ \
    parm caller [ds si] [dx] [es di] value [ax] modify exact [ax bx dx];

extern void _XMSCopyWords( void __far *, void __far *, unsigned short );
#pragma aux _XMSCopyWords = \
        0x1e            /* push ds */ \
        0x8e 0xda       /* mov ds,dx */ \
        0xf3 0xa5       /* rep movsw */ \
        0x1f            /* pop ds */ \
    parm caller [dx si] [es di] [cx] modify exact [si di cx];

#endif
