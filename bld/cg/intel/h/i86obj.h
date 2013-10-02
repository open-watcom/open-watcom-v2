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


/* aligned */
#include "pcobj.h"
#include "targsys.h"
#include "escape.h"

#define BUFFSIZE        256     /*  size of object code buffer */
#define TOLERANCE       64
#define TRADEOFF        14      /*  minimum size of iterated for a new record */
#define SYS_REC_SIZE    512     /*  size of physical output record */
                                /*  must match NAMEPCO spec */
#define MAXNAME         40      /*  INTEL's limit for a name (+len byte) */
#define IMPORT_BASE     1       /*  start of import indexs */
#define INDEX_SIZE      2       /*  maximum index size, in bytes */

#define SEG_ALGN_BYTE    SEGATTR_A( BYTE )   /*    1-byte aligned, relocatable */
#define SEG_ALGN_WORD    SEGATTR_A( WORD )   /*    2-byte aligned, relocatable */
#define SEG_ALGN_PARA    SEGATTR_A( PARA )   /*   16-byte aligned, relocatable */
#define SEG_ALGN_PAGE    SEGATTR_A( PAGE )   /*  256-byte aligned, relocatable */
#define SEG_ALGN_DWORD   SEGATTR_A( DWORD )  /*    4-byte aligned, relocatable */
#define SEG_ALGN_4K      SEGATTR_A( PAGE4K ) /*   4K-byte aligned, relocatable */
#define SEG_COMB_PRIVATE SEGATTR_C( INVALID )    /*  non-combinable */
#define SEG_COMB_NORMAL  SEGATTR_C( ADDOFF )     /*  normal */
#define SEG_COMB_COMMON  SEGATTR_C( COMMON )     /*  common block */
#define SEG_BIG          SEGATTR_BIG  /*  64K segment */
#define SEG_USE_32       SEGATTR_P    /*  in MS format == USE32 */

#define SEGDEF_ATTR     0       /*  offset of attr in segdef */
#define SEGDEF_SIZE     1       /*  offset of seg size in segdef */

//TODO: these should be dependent \whatcom\h\pcobj.h
#define LOCAT_ABS       0xc0    /*  locat-byte, absolute field */
#define LOCAT_REL       0x80
#define S_LOCAT_LOC     2       /*  shift count, LOC field in locat-byte */

#define FIXDAT  0x04            /*  background for fixdat byte */
#define FIXDAT_FRAME_IMPLIED    0x50    /*  frame implied by target */
#define FIXDAT_FRAME_GROUP      0x10    /*  frame is a group */
#define FIXDAT_FRAME_SEG        0       /*  frame is segment */


#define DEBUG_MAJOR_VERSION     1
#define DEBUG_MINOR_VERSION     3
/* various COMENT records to use */
#define VERSION_COMMENT     ((CMT_LANGUAGE_TRANS << 8)|0x00 )
#define SOURCE_COMMENT      CMT_NP( COMPILER_OPTIONS )
#define LINKER_COMMENT      CMT_NP( LINKER_DIRECTIVE )
#define DISASM_COMMENT      CMT_NP( DISASM_DIRECTIVE )
#define PHAR_LAP_COMMENT    CMT_NP( EASY_OMF )
#define LIBNAME_COMMENT     CMT_NP( DEFAULT_LIBRARY )
#define MODEL_COMMENT       CMT_NP( WAT_PROC_MODEL )
#define DEBUG_COMMENT       CMT_NP( MS_OMF )
#define EXPORT_COMMENT      CMT_NP( DLL_ENTRY )
#define DEPENDENCY_COMMENT  CMT_NP( DEPENDENCY )
#define WEAK_EXTRN_COMMENT  CMT_NP( WKEXT )
#define LAZY_EXTRN_COMMENT  CMT_NP( LZEXT )

typedef unsigned_16     omf_idx;

typedef enum {
/*  order is important -- see documentation */
/*  on fixups */
        BASE_SEG,
        BASE_GRP,
        BASE_IMP,
        BASE_ABS
} base_type;

#include "pushpck1.h"

typedef struct fixup {
        byte                    locatof;
        byte                    fset;
        byte                    fixdat;
} fixup;

#include "poppck.h"

/* Data Structures used internally in object file generation */

typedef struct hassle_note {
        struct hassle_note      *link;
        sym_handle              sym;
        long_offset             where;
        objhandle               hdl;
} hassle_note;

enum {
    /* prefix_comdat_state values */
    PCS_OFF,
    PCS_NEED,
    PCS_ACTIVE
};

typedef struct index_rec {
        struct object           *obj;
        long_offset             location;
        long_offset             max_written;
        long_offset             max_size;
        omf_idx                 base;   /*  base for relocs */
        omf_idx                 sidx;   /*  index for segment */
        omf_idx                 nidx;   /*  name index for segment */
        omf_idx                 cidx;   /*  class name index */
        unsigned                big                     : 1;
        unsigned                need_base_set           : 1;
        unsigned                rom                     : 1;
        unsigned                data_in_code            : 1;
        unsigned                data_ptr_in_code        : 1;
        unsigned                private                 : 1;
        unsigned                exec                    : 1;
        unsigned                start_data_in_code      : 1;
        unsigned                prefix_comdat_state     : 2;
        segment_id              seg;    /*  front-end i.d. */
        base_type               btype;
        byte                    attr;
        byte                    data_prefix_size;
        unsigned_32             comdat_size;
        unsigned_32             total_comdat_size;
        sym_handle              comdat_symbol;
        label_handle            comdat_label;
        omf_idx                 comdat_nidx;
        void                    *virt_func_refs;
        omf_idx                 comdat_prefix_import;
} index_rec;

typedef enum {
    F_OFFSET        = 0,
    F_BIG_OFFSET    = 1,
    F_LDR_OFFSET    = 2,
    F_BASE          = 3,
    F_PTR           = 4,
    F_MASK          = 0x0f,
    F_TLS           = 0x10, /* special trigger value */
    F_FAR16         = 0x20, /* special trigger value */
    F_ALT_DLLIMP    = 0x40, /* special trigger value */
} fix_class;

#define F_CLASS(c)  (c & F_MASK)

typedef enum {
        AP_HAVE_VALUE           = 0x01,
        AP_HAVE_OFFSET          = 0x02
} abspatch_flags;

typedef struct abspatch {
        struct abspatch         *link;
        object                  *obj;
        patch                   pat;
        long_offset             value;
        abspatch_flags          flags;
} abspatch;

extern void             OutAbsPatch(abspatch *,patch_attr);
extern void             DoLblRef( label_handle lbl, segment_id seg, offset val, escape_class kind );
