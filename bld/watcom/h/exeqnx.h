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


#ifndef _EXEQNX_H

#pragma pack(push,1);

// structures used in processing QNX load files.

#define QNX_VERSION 400

#define QNX_MAX_REC_SIZE 0x7E00     // was 0xFFFF
#define QNX_MAX_FIXUPS   0x7E00      // maximum fixup size that can be put
                                    // into one record.
                                    // was 65532

#define QNX_READ_WRITE  0
#define QNX_READ_ONLY   1
#define QNX_EXEC_READ   2
#define QNX_EXEC_ONLY   3

enum {
    LMF_HEADER_REC = 0,
    LMF_COMMENT_REC,
    LMF_LOAD_REC,
    LMF_FIXUP_REC,
    LMF_8087_FIXUP_REC,
    LMF_IMAGE_END_REC,
    LMF_RESOURCE_REC,
    LMF_RW_END_REC,
    LMF_LINEAR_FIXUP_REC
    /* 9  -> 15  reserved for future expansion */
    /* 16 -> 255 for user defined records */
};

typedef struct lmf_record {
    unsigned_8      rec_type;
    unsigned_8      reserved;       // must be 0
    unsigned_16     data_nbytes;    // size of the data record after this.
    unsigned_16     spare;          // must be 0
} lmf_record;

typedef struct lmf_data {
    unsigned_16     segment;
    unsigned_32     offset;
} lmf_data;

#define _TCF_LONG_LIVED                 0x0001
#define _TCF_32BIT                      0x0002
#define _TCF_PRIV_MASK                  0x000c
#define _TCF_FLAT                       0x0010

#define SEG16_CODE_FIXUP                0x0004
#define LINEAR32_CODE_FIXUP             0x80000000
#define LINEAR32_SELF_RELATIVE_FIXUP    0x40000000

typedef struct lmf_header {
    unsigned_16     version;
    unsigned_16     cflags;
    unsigned_16     cpu;            // 86,186,286,386,486
    unsigned_16     fpu;            // 0, 87,287,387
    unsigned_16     code_index;     // segment of code start;
    unsigned_16     stack_index;    // segment to put the stack
    unsigned_16     heap_index;     // segment to start DS at.
    unsigned_16     argv_index;     // segment to put argv & environment.
    unsigned_16     spare2[4];      // must be zero;
    unsigned_32     code_offset;    // starting offset of code.
    unsigned_32     stack_nbytes;   // stack size
    unsigned_32     heap_nbytes;    // initial size of heap (optional).
    unsigned_32     image_base;     // starting address of image
    unsigned_32     spare3[2];
//  unsigned_32     seg_nbytes[1];  // variable length array of seg. sizes.
} lmf_header;

typedef struct lmf_eof {
    unsigned_8  spare[6];
} lmf_eof;

/* values for the res_type field in the lmf_resource structure */
enum {
    RES_USAGE = 0
};

typedef struct lmf_resource {
    unsigned_16 res_type;
    unsigned_16 spare[3];
} lmf_resource;

typedef struct lmf_rw_end {
    unsigned_16     verify;
    unsigned_32     signature;
} lmf_rw_end;

#define VERIFY_OFFSET 36

#pragma pack(pop);

#define _EXEQNX_H
#endif
