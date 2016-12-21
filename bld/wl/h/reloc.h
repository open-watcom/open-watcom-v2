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
        unsigned_32     fltpt;          // floating point fixup value
    } put;
} os2_reloc_item;

        /* for relocation fields */
#define BYTE_ONLY                       0x0001
#define SEGMENT_ONLY                    0x0002
#define SEGMENT_OFFSET                  0x0003
#define OFFSET_ONLY                     0x0005
#define SEGMENT_OFFSET48                0x0006
#define OFFSET48_ONLY                   0x0007
#define OFFSET48_RELATIVE               0x0008

#define INTERNAL_REFERENCE              0x0000
#define IMPORTED_ORDINAL                0x0001
#define IMPORTED_NAME                   0x0002
#define OSFIXUP                         0x0003
#define ADDITIVE                        0x0004

#define WIN_FFIX_DS_OVERRIDE            1       // FIARQQ
#define WIN_FFIX_SS_OVERRIDE            2       // FISRQQ
#define WIN_FFIX_CS_OVERRIDE            3       // FICRQQ
#define WIN_FFIX_ES_OVERRIDE            4       // FIERQQ
#define WIN_FFIX_DR_SYMBOL              5       // FIDRQQ
#define WIN_FFIX_WR_SYMBOL              6       // FIWRQQ

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

typedef union {
    byte        buff[ 12 ];
    struct {
        unsigned_8          nr_stype;
        unsigned_8          nr_flags;
        signed_16           r32_soff;
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
} os2_flat_reloc_item;

#define NOV_OFFSET_CODE_RELOC   0x40000000
#define NOV_TARGET_CODE_RELOC   0x80000000

/* PE fixup table structure */
typedef unsigned_16     pe_reloc_item;
typedef struct {
    pe_reloc_item       loc;
    pe_reloc_item       low_off;        // low 16 bits of target offset
} high_pe_reloc_item;

#define PEUP 12

typedef struct {
    unsigned_32 virt_addr;
    unsigned_32 value;
    unsigned_16 type;
    unsigned_16 pad;
} old_pe_reloc_item;

#define OLD_PEUP 0

/* PE fixup types (stashed in 4 high bits of a pe_fixup_entry) */
#define PE_FIX_ABS      (0x0<<PEUP)     /* absolute, skipped */
#define PE_FIX_HIGH     (0x1<<PEUP)     /* add high 16 of delta */
#define PE_FIX_LOW      (0x2<<PEUP)     /* add low 16 of delta */
#define PE_FIX_HIGHLOW  (0x3<<PEUP)     /* add all 32 bits of delta */
#define PE_FIX_HIGHADJ  (0x4<<PEUP)     /* see the doc */
#define PE_FIX_MIPSJMP  (0x5<<PEUP)     /* see the doc */

/* PE fixup types (stashed in 4 high bits of a pe_fixup_entry) */
#define OLD_PE_FIX_ABS          (0x0<<OLD_PEUP) /* absolute, skipped */
#define OLD_PE_FIX_HIGH         (0x1<<OLD_PEUP) /* add high 16 of delta */
#define OLD_PE_FIX_LOW          (0x2<<OLD_PEUP) /* add low 16 of delta */
#define OLD_PE_FIX_HIGHLOW      (0x3<<OLD_PEUP) /* add all 32 bits of delta */
#define OLD_PE_FIX_HIGHADJ      (0x4<<OLD_PEUP) /* see the doc */
#define OLD_PE_FIX_MIPSJMP      (0x5<<OLD_PEUP) /* see the doc */

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

typedef struct base_reloc {
    unsigned            rel_size;       /* actual size of reloc item */
    unsigned            fix_size;       /* size of field being fixed up */
    offset              fix_off;        /* start addr of field being fixed */
    unsigned            isfloat : 1;
    unsigned            isqnxlinear : 1;
    reloc_item          item;
} base_reloc;

#define OSF_RLIDX_MASK          0x3FF
#define OSF_RLIDX_LOW(val)      (val & OSF_RLIDX_MASK)
#define OSF_RLIDX_HIGH(val)     ((val & (~OSF_RLIDX_MASK)) >> 10)
#define OSF_RLIDX_MAX           0x400

#define OSF_PAGE_SHIFT 12
#define OSF_PAGE_SIZE   (1 << OSF_PAGE_SHIFT)
#define OSF_PAGE_MASK   (OSF_PAGE_SIZE-1)
#define OSF_FIXUP_TO_ALIAS  0x10
#define OSF_SOURCE_MASK     0x0f
#define OSF_TARGET_MASK     0x03
#define OSF_ADDITIVE        0x04
#define OSF_ADDITIVE32      0x20
#define OSF_OBJMOD_16BITS   0x40
#define OSF_TARGOFF_32BITS  0x10
#define OSF_IMPORD_8BITS    0x80
#define OSF_32BIT_SELF_REL  8
#define OSF_PAGE_COUNT( size )  (((size)+OSF_PAGE_MASK)>>OSF_PAGE_SHIFT)

typedef struct reloc_info RELOC_INFO;

extern void             WriteReloc( group_entry *, offset, void *, unsigned );
extern void             FloatReloc( reloc_item * item );
extern void             QNXLinearReloc( group_entry *, reloc_item * );
extern bool             TraverseOS2RelocList( group_entry *,
                                              bool (*)(RELOC_INFO *));
extern void             FreeRelocInfo( void );
extern unsigned_32      RelocSize( RELOC_INFO * );
extern unsigned_32      DumpMaxRelocList( RELOC_INFO **, unsigned_32 );
extern bool             DumpRelocList( RELOC_INFO * );
extern void             SetRelocSize( void );
extern bool             SwapOutRelocs( void );
extern void             ResetReloc( void );
extern unsigned_32      WalkRelocList( RELOC_INFO **head, bool (*fn)( void *data, unsigned_32 size, void *ctx ), void *ctx );

extern unsigned         FmtRelocSize;
extern RELOC_INFO *     FloatFixups;

#include "poppck.h"
