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
* Description:  DWARF format definitions, including Watcom specifics.
*
****************************************************************************/


#ifndef _DWARF_H_INCLUDED_
#define _DWARF_H_INCLUDED_

#include "pushpck1.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
   All informations are derived from DWARF Debugging Information Format ( by
   Unix International Programming Languages SIG, Version 2, Draft 6, dated
   April 12, 1993 )

*/

#define DWARF_IMPL_VERSION  2   /* uint_16 */

/*
    IMPORTANT:
        TAG_*, AT_*, and FORM_* MUST be #define's since they are used in
        the compiling of dwabbrev.dat!  Further, they must expand to an
        integer constant.
*/

/* Tag encodings: Figure 14 & 15 */

typedef enum {
#define DWTAGI( __n, __v  )   DW_TAG_##__n = __v,
#include "dwtagi.h"
#undef  DWTAGI
}dw_tagnum;

/* Attribute form encodings: Figure 19.  */

typedef enum {
#define DWFORMI( __n, __v  )   DW_FORM_##__n = __v,
#include "dwformi.h"
#undef  DWFORMI
}dw_formnum;

/* Attribute encodings: Figure 17 & 18  */

typedef enum {
#define DWATI( __n, __v  )   DW_AT_##__n = __v,
#include "dwati.h"
#undef  DWATI
}dw_atnum;

/* Location operation encodings: section 7.7, figure 23 */

#define DW_OP_lit( __x )        ( (__x) + 0x30 )        // valid for 0..31 only
#define DW_OP_reg( __x )        ( (__x) + 0x50 )        // valid for 0..31 only
#define DW_OP_breg( __x )       ( (__x) + 0x70 )        // valid for 0..31 only
#include "dwloccl.h"
typedef enum {
    DW_OP_regx          = 0x90,
    DW_OP_addr          = 0x03,
    DW_OP_const1u       = 0x08,
    DW_OP_const1s,
    DW_OP_const2u,
    DW_OP_const2s,
    DW_OP_const4u,
    DW_OP_const4s,
    DW_OP_const8u,
    DW_OP_const8s,
    DW_OP_constu,
    DW_OP_consts,
    DW_OP_lit0          = 0x30,
    DW_OP_lit31         = 0x4f,
    DW_OP_reg0          = 0x50,
    DW_OP_reg31         = 0x6f,
    DW_OP_breg0         = 0x70,
    DW_OP_breg31        = 0x8f,
#define DW_LOC_OP( __n, __v )   DW_OP_##__n = __v,
#include "dwlocop.h"
#undef DW_LOC_OP
    DW_OP_lo_user       = 0xe0,
    DW_OP_hi_user       = 0xff
}dw_op;

/* Base Type encodings: Figure 24. */
typedef enum {
    DW_ATE_address          =0x01,
    DW_ATE_boolean          =0x02,
    DW_ATE_complex_float    =0x03,
    DW_ATE_float            =0x04,
    DW_ATE_signed           =0x05,
    DW_ATE_signed_char      =0x06,
    DW_ATE_unsigned         =0x07,
    DW_ATE_unsigned_char    =0x08,
    DW_ATE_lo_user          =0x80,
    DW_ATE_hi_user          =0xff,
}dw_ate;
/* Accessibility codes: Figure 25 */
typedef enum {
    DW_ACCESS_public      = 1,
    DW_ACCESS_protected   = 2,
    DW_ACCESS_private     = 3,
}dw_access;
/* Virtuality codes: Figure 27 */
typedef enum {
    DW_VIRTUALITY_none           =  0,
    DW_VIRTUALITY_virtual        =  1,
    DW_VIRTUALITY_pure_virtual   =  2,
}dw_virtuality;
/* Language encodings: Figure 28.  */
typedef enum {
    DW_LANG_C89             = 0x0001,
    DW_LANG_C               = 0x0002,
    DW_LANG_Ada83           = 0x0003,
    DW_LANG_C_plus_plus     = 0x0004,
    DW_LANG_Cobol74         = 0x0005,
    DW_LANG_Cobol85         = 0x0006,
    DW_LANG_Fortran77       = 0x0007,
    DW_LANG_Fortran90       = 0x0008,
    DW_LANG_Pascal83        = 0x0009,
    DW_LANG_Modula2         = 0x000a,
    DW_LANG_lo_user         = 0x8000,
    DW_LANG_hi_user         = 0xffff,
}dw_langnum;
/* Identifier case encodings: Figure 29 */
typedef enum {
    DW_ID_case_sensitive   =0,
    DW_ID_up_case          =1,
    DW_ID_down_case        =2,
    DW_ID_case_insensitive =3,
}dw_id;
/* Calling convention encodings: Figure 30 */
typedef enum {
    DW_CC_normal           =0x01,
    DW_CC_program          =0x02,
    DW_CC_nocall           =0x03,
    DW_CC_lo_user          =0x40,
    DW_CC_hi_user          =0xff,
}dw_cc;
/* Inline codes: Figure 31 */
typedef enum {
    DW_INL_not_inlined             =0,
    DW_INL_inlined                 =1,
    DW_INL_declared_not_inlined    =2,
    DW_INL_declared_inlined        =3,
}dw_inl;
/* Ordering encodings: Figure 32.  */
typedef enum {
  DW_ORD_row_major =  0,
  DW_ORD_col_major =  1,
}dw_ord;

/* Standard opcode encodings: figure 34 */

typedef enum {
    DW_LNS_copy = 1,
    DW_LNS_advance_pc,
    DW_LNS_advance_line,
    DW_LNS_set_file,
    DW_LNS_set_column,
    DW_LNS_negate_stmt,
    DW_LNS_set_basic_block,
    DW_LNS_const_add_pc,
    DW_LNS_fixed_advance_pc
}dw_lns;

/* extended opcode encodings: figure 35 */

typedef enum {
    DW_LNE_end_sequence = 1,
    DW_LNE_set_address,
    DW_LNE_define_file,
#if 1
    /*
    //  Carl Young - 2004-07-05
    //  Despite recognizing the need for this extended opcode, I disagree with its use. Dwarf 3
    //  may yet add more extended instructions which will screw us over using enumeration value 4!
    */
    DW_LNE_set_segment
#endif
}dw_lne;

/* Macinfo type encodings: figure 36 */

#define DW_MACINFO_define       1
#define DW_MACINFO_undef        2
#define DW_MACINFO_start_file   3
#define DW_MACINFO_end_file     4
#define DW_MACINFO_vendor_ext   255

/* WATCOM_references extension */

#define REF_BEGIN_SCOPE         0x01
#define REF_END_SCOPE           0x02
#define REF_SET_FILE            0x03
#define REF_SET_LINE            0x04
#define REF_SET_COLUMN          0x05
#define REF_ADD_LINE            0x06
#define REF_ADD_COLUMN          0x07
#define REF_COPY                0x08
#define REF_CODE_BASE           0x10
#define REF_COLUMN_RANGE        80

/* AT_WATCOM_memory_model  */
typedef enum {
    DW_MEM_MODEL_none    = 0,
    DW_MEM_MODEL_flat    = 1,
    DW_MEM_MODEL_small   = 2,
    DW_MEM_MODEL_medium  = 3,
    DW_MEM_MODEL_compact = 4,
    DW_MEM_MODEL_large   = 5,
    DW_MEM_MODEL_huge    = 6,
}dw_mem_model;

/* AT_address_class values */
typedef enum {
    DW_ADDR_none,
    DW_ADDR_near16,
    DW_ADDR_far16,
    DW_ADDR_huge16,
    DW_ADDR_near32,
    DW_ADDR_far32
}dw_addr;

/* the child determination byte */
typedef enum {
    DW_CHILDREN_no,
    DW_CHILDREN_yes
}dw_children;

/* handy constants section */

#define COMPILE_UNIT_HDR_SIZE   11    // 4 + 2 + 4 + 1
#define DWLINE_OPCODE_BASE      10

// these next three constants do not have to be defined this way, but it
// makes for consistancy between projects (and thus we can steal code).

#define DW_MIN_INSTR_LENGTH     1
#define DWLINE_BASE             (-1)
#define DWLINE_RANGE            4

/* *****NOTE*******NOTE******** BIG KLUDGE FOLLOWS *****NOTE*****NOTE******
 *
 * in the dwarf reading library, we assume that the abbrev codes are unique -
 * i.e. that a particular abbreviation code corresponds to only one kind
 * of abbreviation.  This used to be true, since the dwarf writing library was
 * the only thing we had generating dwarf.  Now that the linker is doing it,
 * we have a problem of conflicting abbrev codes.  So... until the reading
 * library assumption is removed, we have to keep the abbrev code spaces
 * separate!  This next constant is used in the dwarf reading library for this.
***************************************************************************/

#define LAST_LINKER_ABBREV 4

/* handy structures section */

typedef struct {
    unsigned_32 total_length;
    unsigned_16 version;
    unsigned_32 prologue_length;
    unsigned_8  minimum_instruction_length;
    unsigned_8  default_is_stmt;
    signed_8    line_base;
    unsigned_8  line_range;
    unsigned_8  opcode_base;
    unsigned_8  standard_opcode_lengths[DWLINE_OPCODE_BASE - 1];
} _WCUNALIGNED stmt_prologue;


typedef struct {
    unsigned_32 offset;
    unsigned_16 segment;
    unsigned_32 length;
} _WCUNALIGNED segmented_arange_tuple;

typedef struct {
    unsigned_32 offset;
    unsigned_32 length;
} flat_arange_tuple;

typedef union {
    segmented_arange_tuple      s;
    flat_arange_tuple           f;
} arange_tuple;

typedef struct {
    unsigned_32 length;
    unsigned_16 version;
    unsigned_32 debug_offset;
    unsigned_8  offset_size;
    unsigned_8  segment_size;
//   unsigned_8 padding[ 8 ];   // to make it a multiple of a tuple size.
} _WCUNALIGNED arange_prologue;

typedef struct {
    unsigned_32 length;
    unsigned_16 version;
    unsigned_32 abbrev_offset;
    unsigned_8  addr_size;
} _WCUNALIGNED compuhdr_prologue;

typedef struct {
    unsigned_32 length;
    unsigned_16 version;
    unsigned_32 debug_offset;
    unsigned_32 debug_size;
} _WCUNALIGNED pubnames_prologue;

#ifdef __cplusplus
};
#endif
#include "poppck.h"
#endif
