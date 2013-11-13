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


#include "targsys.h"

/* aligned */
typedef enum {
    // base class 0x00 - 0x0F
#define pick_class(x) x,
#include "occlasss.h"
#undef pick_class
    // base class attributes
    ATTR_FAR   = 0x10,
    ATTR_SHORT = 0x20,
    ATTR_POP   = 0x40,
    ATTR_FLOAT = 0x80
} oc_class;

#define ATTR_IRET       ATTR_SHORT      /*  dual purpose bit */
#define GET_BASE        0x0f            /*  get base from class */
#define BASE_CLASS(x)   ((x)&GET_BASE)

#define INFO_LINE               0x00
#define INFO_LDONE              0x10
#define INFO_DEAD_JMP           0x20
#define INFO_DBG_RTN_BEG        0x30
#define INFO_DBG_BLK_BEG        0x40
#define INFO_DBG_PRO_END        0x50
#define INFO_DBG_EPI_BEG        0x60
#define INFO_DBG_BLK_END        0x70
#define INFO_DBG_RTN_END        0x80
#define INFO_SELECT             0x90
#define INFO_FUNC_START         0xa0
#define INFO_FUNC_END           0xb0

#define INFO_MASK               0xf0

#define OC_LINENUM              (OC_INFO + INFO_LINE)
#define OC_LDONE                (OC_INFO + INFO_LDONE)
#define OC_DEAD_JMP             (OC_INFO + INFO_DEAD_JMP)

typedef byte                    oc_length;
typedef byte                    obj_length;
typedef byte                    cond_no;

#define NULL_COND               16
#define MAX_OBJ_LEN             128

typedef struct oc_header {
        obj_length              objlen;
        oc_class                class;
        oc_length               reclen;
} oc_header;

typedef struct oc_entry {
        oc_header               hdr;
        byte                    data[ 1 ];
} oc_entry;

typedef struct oc_handle {
        oc_header               hdr;
        struct ins_entry        *ref;       // must be at this position, code rely on this (see also _LblRef macro in optmac.h)
        label_handle            handle;    // must be at this position, code rely on this (see also _Label macro in optmac.h)
#if _TARGET & _TARG_RISC
        cg_linenum              line;
#endif
} oc_handle;

typedef struct oc_debug {
        oc_header               hdr;
        pointer                 ptr;
} oc_debug;

typedef struct oc_select {
        oc_header               hdr;
        bool                    starts;
} oc_select;

typedef struct oc_jcond {
        oc_header               hdr;
        struct ins_entry        *ref;       // must be at this position, code rely on this (see also _LblRef macro in optmac.h)
        label_handle            handle;    // must be at this position, code rely on this (see also _Label macro in optmac.h)
        cond_no                 cond;
#if _TARGET & _TARG_RISC
        int                     index;
        int                     index2;     // MIPS can do reg/reg cond branches
#endif
} oc_jcond;

typedef struct oc_ret {
        oc_header               hdr;
        struct ins_entry        *ref;       // must be at this position, code rely on this (see also _LblRef macro in optmac.h)
        uint                    pops;
} oc_ret;

typedef struct oc_idata {
        oc_header               hdr;
        byte                    pat;
} oc_idata;

typedef struct oc_linenum {
        oc_header               hdr;
        unsigned_32             line;
        bool                    label_line;
} oc_linenum;

typedef struct oc_riscins {
        oc_header               hdr;
        unsigned_32             opcode;
        pointer                 sym;            // symbol to which there is reloc (if any)
        unsigned_32             reloc;          // type of reloc (owl_reloc_type)
} oc_riscins;

typedef struct oc_func_start {
        oc_header               hdr;
        pointer                 lbl;
        cg_linenum              line;
} oc_func_start;

#define _HasReloc( ins )        ( (ins)->sym != NULL )

typedef struct ins_link {
        struct ins_entry        *prev;
        struct ins_entry        *next;
} ins_link;

typedef union any_oc {
        struct oc_linenum       oc_linenum;
        struct oc_idata         oc_idata;
        struct oc_ret           oc_ret;
        struct oc_jcond         oc_jcond;
        struct oc_select        oc_select;
        struct oc_debug         oc_debug;
        struct oc_handle        oc_handle;
        struct oc_header        oc_header;
        struct oc_entry         oc_entry;
#if _TARGET & _TARG_RISC
        struct oc_riscins       oc_rins;
        struct oc_func_start    oc_func;
#endif
} any_oc;

typedef struct ins_entry {
        struct ins_link         ins;
        union any_oc            oc;
} ins_entry;

typedef enum {
    OC_DEST_SHORT,
    OC_DEST_NEAR,
    OC_DEST_CHEAP,
    OC_DEST_FAR
} oc_dest_attr;

#if _TARGET & _TARG_INTEL
#define MAX_INS         16      /*  max instruction size */
#define MAX_SHORT_FWD   127
#define MAX_SHORT_BWD   (128-2)
#else
#define MAX_INS         4
#define MAX_SHORT_FWD   ( 1 << 23 )
#define MAX_SHORT_BWD   ( ( 1 << 23 ) - 4 )
#endif
