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
* Description:  PharLap executable formats.
*
****************************************************************************/


#ifndef _EXEPHAR_H

#include "pushpck1.h"

typedef struct simple_header {
    unsigned_16     signature;
    unsigned_16     mod_size;       // file size mod 512
    unsigned_16     file_size;      // file size in blocks
    unsigned_16     num_relocs;     // always zero.
    unsigned_16     hdr_size;       // size of header in paragraphs
    unsigned_16     min_data;       // minimum data in 4K pages.
    unsigned_16     max_data;       // maximum data in 4K pages.
    unsigned_32     ESP;            // initial ESP value.
    unsigned_16     checksum;
    unsigned_32     EIP;            // initial EIP value.
    unsigned_16     reloc_offset;   // always 0x1E.
    unsigned_16     overlay_num;    // always 0.
    unsigned_16     always_one;     // always 1.
} simple_header;

typedef struct extended_header {
    unsigned_16     signature;
    unsigned_16     format_level;
    unsigned_16     header_size;    // in bytes
    unsigned_32     file_size;      // in bytes;
    unsigned_16     checksum;
    unsigned_32     rtp_offset;     // byte offset of run-time param block.
    unsigned_32     rtp_size;       // byte size of run-time param block.
    unsigned_32     reloc_offset;   // byte offset of relocation table.
    unsigned_32     reloc_size;     // byte size of relocation table.
    unsigned_32     sit_offset;     // byte offset of segment info table.
    unsigned_32     sit_size;       // byte size of segment info table.
    unsigned_16     sit_entry_size; // byte size of a seg info table entry
    unsigned_32     load_offset;    // byte offset of load image in file.
    unsigned_32     load_size;      // byte size of load image.
    unsigned_32     sym_offset;     // byte offset of symbol table in file
    unsigned_32     sym_size;       // byte size of symbol table.
    unsigned_32     gdt_offset;     // byte offset of GDT in load image
    unsigned_32     gdt_size;       // byte size of GDT in load image.
    unsigned_32     ldt_offset;     // byte offset of LDT in load image.
    unsigned_32     ldt_size;       // byte size of LDT in load image.
    unsigned_32     idt_offset;     // byte offset of IDT in load image.
    unsigned_32     idt_size;       // byte size of idt in load image.
    unsigned_32     tss_offset;     // byte offset of tss in load image.
    unsigned_32     tss_size;       // byte size of tss in load image.
    unsigned_32     min_extra;      // minimum # of extra bytes to allocate.
    unsigned_32     max_extra;      // maximum # of extra bytes to allocate.
    unsigned_32     offset;         // offset of program image.
    unsigned_32     ESP;
    unsigned_16     SS;
    unsigned_32     EIP;
    unsigned_16     CS;
    unsigned_16     LDT;
    unsigned_16     TSS;
    unsigned_16     flags;          // load image flags.
    unsigned_32     mem_req;        // byte load image memory requirements
    unsigned_32     reserved1;      // must be zero
    unsigned_32     stack_size;     // initial stack size
} extended_header;

// this value is for the flags field
#define PHAR_PACK   0x0001

typedef struct rtpblock {        // run-time parameter block.
    unsigned_16     signature;
    unsigned_16     minreal;
    unsigned_16     maxreal;
    unsigned_16     minibuf;
    unsigned_16     maxibuf;
    unsigned_16     nistack;
    unsigned_16     istksize;
    unsigned_32     realbreak;
    unsigned_16     callbufs;
    unsigned_16     extender_flags;     /* for undocumented "runtime flags" */
    unsigned_16     unpriv;
} rtpblock;

typedef struct {
    unsigned_16 selector;
    unsigned_16 flags;
    unsigned_32 base;
    unsigned_32 extra;
} seg_info_table;

#define SIMPLE_SIGNATURE    0x504D      // 'MP'
#define EXTENDED_SIGNATURE  0x3350      // 'P3'
#define RTP_SIGNATURE       0x5844      // 'DX'
#define REX_SIGNATURE       0x514D      // 'MQ'

#define PHAR_FORMAT_FLAT        1
#define PHAR_FORMAT_SEGMENTED   2

#include "poppck.h"

#define _EXEPHAR_H
#endif

