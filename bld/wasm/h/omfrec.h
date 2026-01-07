/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OMF record definitions.
*
****************************************************************************/


#ifndef OMFREC_H
#define OMFREC_H

#include "pcobj.h"


typedef struct {
    uint_16 frame;          /* frame number of physical reference       */
    uint_32 offset;         /* offset into reference                    */
} physref;

typedef struct {
    uint_8  frame       :3; /* F_ types from pcobj.h                    */
    uint_8  target      :3; /* T_ types from pcobj.h (only T0-T3)       */
    uint_8  is_secondary:1; /* can write target in a secondary manner   */

    uint_16 frame_datum;    /* datum for different frame methods        */
    uint_16 target_datum;   /* datum for different target methods       */
    int_32  target_offset;  /* offset of target for target method       */
} logref;

typedef union {
    logref  log;
    physref phys;
} logphys;


typedef struct coment_info {
    uint_8  attr;           /* attribute field from coment record       */
    uint_8  class;          /* class field from coment record           */
} coment_info;
/*
    A COMENT record is created by filling in the above fields, and attaching
    any appropriate data with the Obj...() functions below.
*/


typedef struct modend_info {
    uint_8  main_module :1; /* module is a main module                  */
    uint_8  start_addrs :1; /* module has start address                 */
    uint_8  is_logical  :1; /* is logical or physical reference         */
    logphys ref;            /* a logical or physical reference          */
} modend_info;
/*
    A MODEND is described completely by the above information; no data
    should be attached to a MODEND.
*/


typedef struct lnames_info {
    uint_16 first_idx;      /* index of first name in this record       */
    uint_16 num_names;      /* number of names in this record           */
} lnames_info;
/*
    LNAMES, EXTDEFs, and COMDEFs all use this structure.  The actual
    LNAMES/etc are in the data attached to the record.
*/


typedef struct grpdef_info {
    uint_16 idx;            /* index of this grpdef record              */
} grpdef_info;
/*
    The data that defines the GRPDEF should be attached to this record.
*/


enum segdef_align_values {
    SEGDEF_ALIGN_ABS        = 0,/* absolute segment - no alignment          */
    SEGDEF_ALIGN_BYTE       = 1,/* relocatable seg  - byte aligned          */
    SEGDEF_ALIGN_WORD       = 2,/*                  - word aligned          */
    SEGDEF_ALIGN_PARA       = 3,/*                  - para aligned          */
    SEGDEF_ALIGN_PAGE       = 4,/*                  - page aligned          */
    SEGDEF_ALIGN_DWORD      = 5,/*                  - dword aligned         */
    SEGDEF_ALIGN_4KPAGE     = 6 /*                  - 4k page aligned       */
    /* if more than 16 types then adjust bitfield width in segdef_info */
};

typedef struct segdef_info {
    uint_16 idx;            /* index for this segment                   */
    uint_8  align       :4; /* align field (enum segdef_align_values)   */
    uint_8  combine     :4; /* combine field (values in pcobj.h)        */
    uint_8  use_32      :1; /* use_32 for this segment                  */
    uint_8  access_valid:1; /* does next field have valid value         */
    uint_8  access_attr :2; /* easy omf access attributes (see pcobj.h) */
    physref abs;            /* (conditional) absolute physical reference*/
    uint_32 seg_length;     /* length of this segment                   */
    uint_16 seg_name_idx;   /* name index of this segment               */
    uint_16 class_name_idx; /* class name index of this segment         */
    uint_16 ovl_name_idx;   /* overlay name index of this segment       */
} segdef_info;
/*
    All data necessary for a SEGDEF is defined in the above structure.  No
    data should be attached to the record.
*/


typedef struct ledata_info {
    uint_16     idx;        /* index of segment the data belongs to     */
    uint_32     offset;     /* offset into segment of start of data     */
} ledata_info;
/*
    LEDATAs and LIDATAs both use this structure.  The data that comprises the
    record should be attached.
*/


typedef struct base_info {
    uint_16 grp_idx;        /* index of the group base                  */
    uint_16 seg_idx;        /* index of the segment                     */
    uint_16 frame;          /* valid if grp_idx == 0 && seg_idx == 0    */
} base_info;                /* appears at beginning of appropriate recs */
/*
    This appears at the beginning of LINNUMs and PUBDEFs.  (see the
    appropriate structures.
*/


typedef struct comdat_info {
    base_info   base;
    uint_8      flags;
    uint_8      attributes;
    uint_8      align;
    uint_32     offset;
    uint_16     type_idx;
    uint_16     public_name_idx;
} comdat_info;


/*
    The ordering of this enumerated type is depended on in several places.
*/
enum {                      /* method of fixing up location:            */
    FIX_LO_BYTE,            /* relocate lo byte of offset               */
    FIX_OFFSET,             /* relocate offset (2 bytes)                */
    FIX_BASE,               /* relocate base (2 bytes)                  */
    FIX_POINTER,            /* relocate pointer (base:offset 4 bytes)   */
    FIX_HI_BYTE,            /* relocate hi byte of offset               */
    FIX_OFFSET386,          /* relocate offset (4 bytes)                */
    FIX_POINTER386,         /* relocate pointer (base:offset 6 bytes)   */
    /* don't define more than 8 values without changing bit field */
};

typedef struct fixuprec {
    struct fixuprec *next;          /* useful for placing in linked list        */
    uint_8          loc_method      :3;
    uint_8          self_relative   :1; /* self or seg relative                 */
    uint_8          loader_resolved :1; /* loader resolved relocation           */
    uint_32         loc_offset;     /* see note below                           */
    logref          lr;             /* logical reference data                   */
} fixuprec;

/*
    fixuprec.loc_offset has two purposes:  when the fixuprec is built by FixGetFix,
    this field will contain the value of the "data_rec_offset" 10-bit offset
    into the previous LE/LIDATA.  When the fixuprec is written, this field must
    contain a similar value (10-bit number which is offset from beginning of
    most recent LE/LIDATA).  However, it is a full 32-bit wide field to allow
    other routines room to play.  i.e., in the WATCOM parser uses the 32-bit
    field as a full offset into the debugging information segments ($$TYPES or
    $$SYMBOLS); this is handled by WAT2CAN0.C.
*/

typedef struct fixinfo {
    /* caller should not touch these fields */
    struct {
        uint_8  method;
        uint_16 datum;
    } trd[ 8 ];
} fixinfo;

enum fixgen_types {
    FIX_GEN_INTEL,
    FIX_GEN_MS386
};
#define FIX_GEN_MAX     11          /* max number of bytes FixGenFix requires */

/*
 * forward declaration
 */
typedef struct obj_rec      *obj_rec_handle;

typedef struct fixup_info {
    obj_rec_handle  data_rec;   /* ptr to the data record this belongs to   */
    fixuprec        *fixup;     /* linked list of processed fixups          */
} fixup_info;
/*
    No data should be attached to these records; all information is in
    the linked list of fixup records.
*/

typedef struct linnum_data {
    uint_16 number;         /* line number in source file               */
    uint_32 offset;         /* offset into segment                      */
} linnum_data;

typedef struct linnum_info {
    base_info       base;       /* base information                         */
    uint_16         num_lines;  /* number of elements in following array    */
    linnum_data     *lines;     /* array of size num_lines                  */
} linnum_info;

typedef struct linsym_info {
    uint_8          flags;      /* for LINSYM records                       */
    uint_16         public_name_idx; /* for LINSYM records                  */
    uint_16         num_lines;  /* number of elements in following array    */
    linnum_data     *lines;     /* array of size num_lines                  */
} linsym_info;
/*
    No data should be attached to these records.  All necessary information
    is in the lines array.
*/
typedef uint_16     name_handle;

typedef struct pubdef_data {
    name_handle name;           /* name of this public                      */
    uint_32     offset;         /* public offset                            */
    union {                     /* see PUBDEF.h for more information        */
        uint_16     idx;        /* Intel OMF type index                     */
    } type;
} pubdef_data;

typedef struct pubdef_info {
    base_info   base;           /* base information                         */
    uint_16     num_pubs;       /* number of publics in following array     */
    pubdef_data *pubs;          /* array of size num_pubs                   */
    uint_8      free_pubs : 1;  /* can we AsmFree the pubs array?           */
    uint_8      processed : 1;  /* for use by dbg_generator (init'd to 0)   */
} pubdef_info;
/*
 * (This format for PUBDEFs is probably only useful for OMF output.)
 * No data should be attached to this record.
 * Everything is described by the pubs array.
 */

typedef union objrec_info {
    coment_info  coment;
    modend_info  modend;
    lnames_info  lnames;
    lnames_info  llnames;
    lnames_info  extdef;
    lnames_info  comdef;
    lnames_info  cextdef;
    grpdef_info  grpdef;
    segdef_info  segdef;
    ledata_info  ledata;
    ledata_info  lidata;
    base_info    base;
    fixup_info   fixupp;
    linnum_info  linnum;
    linsym_info  linsym;
    pubdef_info  pubdef;
    comdat_info  comdat;
} objrec_info;

typedef struct obj_rec {
    struct obj_rec  *next;
    uint_16         length;     /* the length field for this record  (PRIVATE)  */
    uint_16         curoff;     /* offset of next read within record (PRIVATE)  */
    uint_8          *data;      /* data for this record              (PRIVATE)  */
    uint_8          command;    /* the command field for this record            */
    uint_8          is_32   : 1;/* is this a Microsoft 32bit record             */
    uint_8          free_data:1;/* should we AsmFree( data )??       (PRIVATE)  */
    objrec_info     u;          /* data depending on record type                */
} obj_rec;

typedef struct lifix {
    struct lifix    *next;
    uint_16         lower_bound;
    int_16          delta;
} lifix;

typedef struct lifix_list {
    lifix   *head;
} lifix_list;

#endif

