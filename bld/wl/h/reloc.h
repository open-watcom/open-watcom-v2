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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/* for relocation fields */

#define BYTE_ONLY               0x0001
#define SEGMENT_ONLY            0x0002
#define SEGMENT_OFFSET          0x0003
#define OFFSET_ONLY             0x0005
#define SEGMENT_OFFSET48        0x0006
#define OFFSET48_ONLY           0x0007
#define OFFSET48_RELATIVE       0x0008

#define INTERNAL_REFERENCE      0x0000
#define IMPORTED_ORDINAL        0x0001
#define IMPORTED_NAME           0x0002
#define OSFIXUP                 0x0003
#define ADDITIVE                0x0004

#define NOV_OFFSET_CODE_RELOC   0x40000000
#define NOV_TARGET_CODE_RELOC   0x80000000

/* PE fixup types (stashed in 4 high bits of a pe_fixup_entry) */
#define PEUP 12

#define PE_FIX_ABS              (0x0<<PEUP)     /* absolute, skipped */
#define PE_FIX_HIGH             (0x1<<PEUP)     /* add high 16 of delta */
#define PE_FIX_LOW              (0x2<<PEUP)     /* add low 16 of delta */
#define PE_FIX_HIGHLOW          (0x3<<PEUP)     /* add all 32 bits of delta */
#define PE_FIX_HIGHADJ          (0x4<<PEUP)     /* see the doc */
#define PE_FIX_MIPSJMP          (0x5<<PEUP)     /* see the doc */

/* PE fixup types (stashed in 4 high bits of a pe_fixup_entry) */
#define OLD_PEUP 0

#define OLD_PE_FIX_ABS          (0x0<<OLD_PEUP) /* absolute, skipped */
#define OLD_PE_FIX_HIGH         (0x1<<OLD_PEUP) /* add high 16 of delta */
#define OLD_PE_FIX_LOW          (0x2<<OLD_PEUP) /* add low 16 of delta */
#define OLD_PE_FIX_HIGHLOW      (0x3<<OLD_PEUP) /* add all 32 bits of delta */
#define OLD_PE_FIX_HIGHADJ      (0x4<<OLD_PEUP) /* see the doc */
#define OLD_PE_FIX_MIPSJMP      (0x5<<OLD_PEUP) /* see the doc */

#define OSF_RLIDX_MASK          0x3FF
#define OSF_RLIDX_LOW(val)      (val & OSF_RLIDX_MASK)
#define OSF_RLIDX_HIGH(val)     ((val & (~OSF_RLIDX_MASK)) >> 10)
#define OSF_RLIDX_MAX           0x400

#define OSF_PAGE_SHIFT          12
#define OSF_PAGE_SIZE           (1 << OSF_PAGE_SHIFT)
#define OSF_PAGE_MASK           (OSF_PAGE_SIZE-1)
#define OSF_PAGE_COUNT( size )  (((size)+OSF_PAGE_MASK)>>OSF_PAGE_SHIFT)


#include "pushpck1.h"

typedef struct os2_reloc_item {
    unsigned_8      addr_type;          /* see below                     */
    unsigned_8      reloc_type;         /* see below                     */
    unsigned_16     reloc_offset;       /* where to put addr in curr grp */
    union addr_to_put {                 /* where,how to get addr to put  */
        struct internalreference {      /*   in this module              */
            unsigned_8      grp_num;    /*      group number             */
            unsigned_8      rsrvd;      /*      == 0                     */
            unsigned_16     off;        /*      xtrnl offset in that grp */
        } internal;
        struct importedordinal {        /*   by ordinal                  */
            unsigned_16     modref_idx; /*      in 'module_name'.dll     */
            unsigned_16     ord_num;    /*      entry point index        */
        } ordinal;
        struct importedname {           /*   by name                     */
            unsigned_16     modref_idx; /*      in 'module_name'.dll     */
            unsigned_16     impnam_off; /*      with res_name table      */
        } name;
        unsigned_32     fltpt;          // floating point patch value
    } put;
} os2_reloc_item;

typedef struct {
    dos_addr    addr;
} dos_reloc_item;

typedef struct {
    unsigned_32 reloc_offset;
} nov_reloc_item;

typedef struct {
    unsigned_32 reloc_offset;
} rex_reloc_item;

typedef struct {
    unsigned_32 offset;
    unsigned_16 segment;
} pms_reloc_item;

typedef struct {
    unsigned_16 segment;
    unsigned_32 reloc_offset;
} qnx_reloc_item;

typedef struct {
    unsigned_32 reloc_offset;
} qnx_linear_item;

typedef struct {
    unsigned_32 reloc_offset;
} zdos_reloc_item;

typedef struct {
    unsigned_8          nr_stype;
    unsigned_8          nr_flags;
    signed_16           r32_soff;
    union {
        byte            buff[12];
        struct {
            unsigned_16         r32_objmod;
            union {
                unsigned_32     intref;
                union {
                    unsigned_32 proc;
                    unsigned_32 ord;
                }               extref;
                struct {
                    unsigned_16 entry;
                    unsigned_32 addval;
                }               addfix;
            }                   r32_target;
            unsigned_16         r32_srccount;
            unsigned_16         r32_chain;
        }           fmt;
    } u;
} os2_flat_reloc_item;

/* PE fixup table structure */
typedef unsigned_16     pe_reloc_item;

typedef struct {
    pe_reloc_item       loc;
    pe_reloc_item       low_off;        // low 16 bits of target offset
} high_pe_reloc_item;

typedef struct {
    unsigned_32 virt_addr;
    unsigned_32 value;
    unsigned_16 type;
    unsigned_16 pad;
} old_pe_reloc_item;

typedef struct {
    unsigned_32 reloc_offset;
    unsigned_32 info;
    unsigned_32 addend;
} elf_reloc_item;

typedef union {
    os2_reloc_item      os2;
    dos_reloc_item      dos;
    nov_reloc_item      novell;
    qnx_reloc_item      qnx;
    qnx_linear_item     qnxl;
    rex_reloc_item      rex;
    pms_reloc_item      pms;
    os2_flat_reloc_item os2f;
    pe_reloc_item       pe;
    old_pe_reloc_item   oldpe;
    high_pe_reloc_item  hpe;
    elf_reloc_item      elf;
    zdos_reloc_item     zdos;
} reloc_item;

typedef struct reloc_info RELOC_INFO;

#include "poppck.h"

extern unsigned         FmtRelocSize;
extern RELOC_INFO       *FloatFixups;

extern void             WriteReloc( group_entry *, offset, void *, size_t );
#ifdef _QNX
extern void             WriteQNXFloatReloc( qnx_reloc_item * item );
extern void             WriteQNXLinearReloc( group_entry *, qnx_linear_item * );
#endif
extern bool             TraverseOS2RelocList( group_entry *, bool (*)(RELOC_INFO *));
extern void             FreeRelocInfo( void );
extern unsigned_32      RelocSize( RELOC_INFO * );
extern unsigned_32      DumpMaxRelocList( RELOC_INFO **, unsigned_32 );
extern bool             DumpRelocList( RELOC_INFO * );
extern void             SetRelocSize( void );
extern bool             SwapOutRelocs( void );
extern void             ResetReloc( void );
extern unsigned_32      WalkRelocList( RELOC_INFO **head, bool (*fn)( void *data, size_t size, void *ctx ), void *ctx );
