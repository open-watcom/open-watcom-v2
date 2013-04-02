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


/* % */
/* % Location expresions */
/* % */
#define LOC_BP_OFFSET           0x10
#define BP_BYTE                 0x00
#define BP_WORD                 0x01
#define BP_DWORD                0x02

#define LOC_CONSTANT            0x20
#define LOC_MEM_286             0x20
#define LOC_MEM_386             0x21
#define LOC_CONST_1             0x22
#define LOC_CONST_2             0x23
#define LOC_CONST_4             0x24

#define LOC_MULTI_REG           0x30    /* lo nibble is # regs following - 1 */

#define LOC_REG                 0x40    /* lo nibble is reg number (0-15) */


#define LOC_OPER                0x60
#define LOP_IND_2               0x00
#define LOP_IND_4               0x01
#define LOP_IND_ADDR286         0x02
#define LOP_IND_ADDR386         0x03
#define LOP_ZEB                 0x04
#define LOP_ZEW                 0x05
#define LOP_MK_FP               0x06
#define LOP_POP                 0x07
#define LOP_XCHG                0x08
#define LOP_ADD                 0x09
#define LOP_DUP                 0x0a
#define LOP_NOP                 0x0b


#define LOC_EXPR_IND            0x80
#define LOC_IND_REG             0x50  // to be moved back to wvdbg.h
#define IND_CALLOC_NEAR         0x00
#define IND_CALLOC_FAR          0x01
#define IND_RALLOC_NEAR         0x02
#define IND_RALLOC_FAR          0x03

#if _TARGET & _TARG_IAPX86
        #define LOC_MEMORY      LOC_MEM_286
#elif _TARGET & _TARG_80386
        #define LOC_MEMORY      LOC_MEM_386
#else
        #define LOC_MEMORY      LOC_MEM_386
#endif

#define FIELD_INTERNAL          0x01
#define FIELD_PUBLIC            0x02
#define FIELD_PROTECTED         0x04
#define FIELD_PRIVATE           0x08

typedef struct location {
        struct location         *next;
        union {
            union name          *be_sym;
            sym_handle          fe_sym;
            unsigned            ptr_type;
            unsigned            stk;
            unsigned_32         val;
        }                       u;
        unsigned                use;
        byte                    class;
} location;

typedef struct location_list {
        struct location_list    *link;
        dbg_loc                 loc;
} location_list;

typedef struct dbg_local {
        struct dbg_local        *link;
        dbg_loc                 loc;
        pointer                 sym;
        enum{
            DBG_SYM_VAR,
            DBG_SYM_TYPE,
            DBG_SYM_TYPEDEF,
        }kind;
} dbg_local;

typedef struct dbg_block {
        struct dbg_block        *parent;
        struct dbg_local        *locals;
        pointer                 patches;
        bck_info               *end_lbl;
        unsigned_32             start;
} dbg_block;

typedef struct dbg_rtn {
        dbg_loc                 reeturn;
        struct dbg_rtn          *parent;
        struct dbg_block        *rtn_blk;
        struct dbg_block        *blk;
        struct dbg_local        *parms;
        dbg_loc                 obj_loc;        /* for member functions */
        dbg_type                obj_type;       /* for member functions */
        unsigned_32             ret_offset;
        unsigned_32             epi_start;
        byte                    pro_size;
        byte                    obj_ptr_type;   /* for member functions */
        bck_info               *end_lbl;
} dbg_rtn;

#define DB_BUFF_SIZE    511
#define MAX_FIXUPS      2

typedef enum { FIX_FORWARD, FIX_SYMBOL, FIX_BACKHANDLE } fixup_kind;

typedef struct temp_buff {
        uint                    index;
        uint                    fix_idx;
        struct {
            pointer             p;
            byte                pos;
            fixup_kind          type;
        }                       fix[MAX_FIXUPS];
        byte                    buff[ DB_BUFF_SIZE+1 ];
} temp_buff;
