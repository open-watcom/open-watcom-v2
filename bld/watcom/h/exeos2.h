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


#ifndef _EXEOS2_H

#if defined( __WATCOMC__ )
#pragma pack(push,1);
#endif

/* OS2 EXE file header and various tables */
/* ====================================== */

typedef struct os2_exe_header {
    unsigned_16         signature;      /* signature to mark valid EXE file */
    unsigned_16         version;        /* version of linker                */
    unsigned_16         entry_off;      /* offset from header of entry table*/
    unsigned_16         entry_size;     /* size of entry table              */
    unsigned_32         chk_sum;        /* checksum of entire file          */
    unsigned_16         info;           /* various flags (see below)        */
    unsigned_16         adsegnum;       /* automatic data segment number    */
    unsigned_16         heap;           /* initial heap allocation - bytes  */
    unsigned_16         stack;          /* initial stack allocation - bytes */
    unsigned_16         IP;             /* value for IP                     */
    unsigned_16         entrynum;       /* segment number of entry segment  */
    unsigned_16         SP;             /* value for SP                     */
    unsigned_16         stacknum;       /* segment number of stack segment  */
    unsigned_16         segments;       /* total number of segments         */
    unsigned_16         modrefs;        /* # entries in module ref table    */
    unsigned_16         nonres_size;    /* size of nonresident name table   */
    unsigned_16         segment_off;    /* header offset of   segment table */
    unsigned_16         resource_off;   /* " " "             resource table */
    unsigned_16         resident_off;   /* " " "        resident name table */
    unsigned_16         module_off;     /* " " "     module reference table */
    unsigned_16         import_off;     /* " " "       imported names table */
    unsigned_32         nonres_off;     /* offset from start of file of
                                                    nonresident names table */
    unsigned_16         movable;        /* number of movable entry points   */
    unsigned_16         align;          /* segment alignment shift count    */
    unsigned_16         resource;       /* # of entries in resource table   */
    unsigned_8          target;         /* target operating system */
    unsigned_8          otherflags;     /* more flags */
    unsigned_16         gangstart;      /* these 4 added for windows */
    unsigned_16         ganglength;
    unsigned_16         swaparea;
    unsigned_16         expver;
} os2_exe_header;

#define OS2_SIGNATURE_WORD      0x454e  // 'NE'
#define RAT_SIGNATURE_WORD      0x454c  // 'LE'
#define OS2_NE_OFFSET             0x3c
#define OS2_EXE_HEADER_FOLLOWS  0x0040  /* reloc table offset 0x40 */

/******************************************************************************
 *
 *        The info field is used as follows:
 *        (W - Windows only, O - OS2 only)
 *
 *  x x x x      x x x x      x x x x      x x x x
 *  | | |            | |      | | | |      | | | |
 *  | | |            | |      | | | |      | | | +->  single autodata segment
 *  | | |            | |      | | | |      | | +--->  multiple autodata segments
 *  | | |            | |      | | | |      | +----->  if DLL, per-process
 *  | | |            | |      | | | |      |             initialization
 *  | | |            | |      | | | |      +------->  protected mode only
 *  | | |            | |      | | | +-------------->W uses LIM EMS directly
 *  | | |            | |      | | +---------------->O needs 80286
 *  | | |            | |      | |                   W each instance has own
 *  | | |            | |      | |                        EMS bank
 *  | | |            | |      | +------------------>O needs 80386
 *  | | |            | |      |                     W if DLL, global memory
 *  | | |            | |      |                          above EMS line
 *  | | |            | |      +-------------------->  needs math co-processor
 *  | | |            +-+--------------------------->  PM compatibility flags
 *  | | +------------------------------------------>  errors during link
 *  | |                                                  (not executable)
 *  | +-------------------------------------------->W if DLL, private DLL: only
 *  |                                                    one program accesses
 *  +---------------------------------------------->  1=DLL, 0=program file
 *
 *****************************************************************************/

#define OS2_SINGLE_AUTO         1
#define OS2_MULT_AUTO           2
#define OS2_INIT_INSTANCE       0x0004
#define OS2_PROT_MODE_ONLY      0x0008
#define WIN_USES_EMS_DIRECT     0x0010
#define OS2_NEEDS_80286         0x0020
#define WIN_EMS_BANK_INSTANCE   0x0020
#define OS2_NEEDS_80386         0x0040
#define WIN_EMS_GLOBAL_MEM      0x0040
#define OS2_NEEDS_MATH_CO       0x0080
#define OS2_NOT_PM_COMPATIBLE   0x0100
#define OS2_PM_COMPATIBLE       0x0200
#define OS2_PM_APP              0x0300
#define OS2_LINK_ERROR          0x2000
#define OS2_ISCONFORMING        0x4000      /* this doesn't exist anymore.*/
#define WIN_PRIVATE_DLL         0x4000
#define OS2_IS_DLL              0x8000

#define OS2_COMPATIBILITY_MASK  0x0300

/* these are used in the target field.*/

#define TARGET_OS2      1
#define TARGET_WINDOWS  2
#define TARGET_DOS4     3
#define TARGET_WIN386   4

/* these are the flags in the otherflags field */

#define OS2_LONG_FILE_NAMES     0x01
#define WIN_PROPORTIONAL_FONT   0x02    /* this can only be set when */
#define WIN_CLEAN_MEMORY        0x04    /* WIN_CLEAN_MEMORY is set. */
#define WIN_GANGLOAD_PRESENT    0x08

typedef struct segment_record {
    unsigned_16         address;        /* segment position within file     */
    unsigned_16         size;           /* segment length in bytes          */
    unsigned_16         info;           /* various flags (see below)        */
    unsigned_16         min;            /* minimum allocation (size in mem) */
} segment_record;

/******************************************************************************
 *
 *        The info field is used as follows:
 *
 *  x x x x      x x x x      x x x x      x x x x
 *    | | |      | | | |      | | | |      |     |
 *    | | |      +-| | |      | | | |      |     +-> data segment (not code)
 *    | | |        | | |      | | | |      +-------> iterated data
 *    | | |        | | |      | | | +--------------> movable segment
 *    | | |        | | |      | | +----------------> pure (sharable) segment
 *    | | |        | | |      | +------------------> preload (not loadoncall)
 *    | | |        | | |      +--------------------> read only (code or data)
 *    | | |        | | +---------------------------> seg has relocation info
 *    | | |        | +-----------------------------> seg has debugging info
 *    | | |        +-------------------------------> descriptor privilege level
 *    | | +----------------------------------------> discardable segment
 *    | +------------------------------------------> 32 bit segment
 *    +--------------------------------------------> part of huge segment
 *
 *****************************************************************************/

/* NOTE: the linker uses some of the unused bits here. If these bits become
 * used, make sure the linker developer knows about it! */

#define SEG_DATA            1
#define SEG_FLAG_1       0x02
#define SEG_FLAG_2       0x04
#define SEG_ITERATED        8
#define SEG_MOVABLE      0x10
#define SEG_PURE         0x20       /* i.e. segment is sharable. */
#define SEG_PRELOAD      0x40
#define SEG_READ_ONLY    0x80
#define SEG_RELOC       0x100
#define SEG_CONFORMING  0x200       /* was SEG_DEBUG */
#define SEG_LEVEL_1     0x400
#define SEG_LEVEL_2     0x800
#define SEG_LEVEL_3     0xC00
#define SEG_DISCARD    0x1000
#define SEG_32_BIT     0x2000
#define SEG_HUGE       0x4000
#define SEG_RESRC_HIGH 0x8000
#define SEG_SHIFT_PMODE_LVL     10
#define SEG_SHIFT_PRI_LVL       12

#define SEG_LEVEL_MASK  0x0C00

#define MOVABLE_ENTRY_PNT       0xff

typedef struct bundle_prefix {
    unsigned_8          number;         /* number of entries in bundle      */
    unsigned_8          type;           /* bundle type                      */
                                        /* - 00H: null bundle (no records)  */
                                        /* - FFH: movable segment records   */
                                        /* - nnH: fixed segment records for */
                                        /*        segment nn                */
} bundle_prefix;

typedef struct movable_record {
    unsigned_8          info;           /* flags: 1 = exported entry        */
                                        /*    2 = uses shared data segment  */
    unsigned_16         reserved;       /* reserved = 0x3FCD                */
    unsigned_8          entrynum;       /* segment # containing entry point */
    unsigned_16         entry;          /* offset of entry point            */
} movable_record;

#define ENTRY_EXPORTED  0x01
#define ENTRY_SHARED    0x02
#define IOPL_WORD_SHIFT 2

typedef struct fixed_record {
    unsigned_8          info;           /* flags: 1 = exported entry        */
                                        /*    2 = uses shared data segment  */
    unsigned_16         entry;          /* offset of entry point            */
} fixed_record;

#define OS2_DEF_SEGMENT_SHIFT  9

/* The resource table is an unsigned_16 resource shift count followed by: */
/* repeated for each type */
typedef struct resource_type_record {
    unsigned_16         type;           /* see below */
    unsigned_16         num_resources;  /* of this type */
    unsigned_32         reserved;
} resource_type_record;

/* repeated for each instance of this type */
typedef struct resource_record {
    unsigned_16         offset;         /* after resource shift */
    unsigned_16         length;         /* after resource shift */
    unsigned_16         flags;          /* resource flags */
    unsigned_16         name;
    unsigned_32         reserved;
} resource_record;

/* resource flags are any combination of SEG_MOVABLE, SEG_PURE, SEG_PRELOAD, */
/* and SEG_DISCARD */

/* If  type  is 0 then this is the end of the table, otherwise the rest of */
/* the type record is present.  type  and  name  use the same format. If the */
/* high bit is set they represent an ordinal type or name, otherwise they */
/* are an offset (without any shift) from the start of the resource table */
/* into a block of strings after the table. Strings in the block are one */
/* byte length followed by that many characters and are not null terminated. */

        /* for relocation fields */
#define REL_OFFSET_ONLY         0x0001
#define REL_SEGMENT_ONLY        0x0002
#define REL_SEGMENT_OFFSET      0x0003
#define REL_INTERNAL_REFERENCE  0x0000
#define REL_IMPORTED_ORDINAL    0x0001
#define REL_IMPORTED_NAME       0x0002
#define REL_ADDITIVE            0x0004

#if defined( __WATCOMC__ )
#pragma pack(pop);
#endif

#define _EXEOS2_H
#endif
