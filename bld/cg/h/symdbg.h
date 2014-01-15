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
#define LOC_BP_OFFSET       0x10
#define BP_BYTE             0x00
#define BP_WORD             0x01
#define BP_DWORD            0x02

#define LOC_CONSTANT        0x20
#define LOC_MEM_16          0x20
#define LOC_MEM_32          0x21
#define LOC_CONST_1         0x22
#define LOC_CONST_2         0x23
#define LOC_CONST_4         0x24

#define LOC_MULTI_REG       0x30    /* lo nibble is # regs following - 1 */

#define LOC_REG             0x40    /* lo nibble is reg number (0-15) */


#define LOC_OPER            0x60
#define LOP_IND_2           0x00
#define LOP_IND_4           0x01
#define LOP_IND_ADDR_16     0x02
#define LOP_IND_ADDR_32     0x03
#define LOP_ZEB             0x04
#define LOP_ZEW             0x05
#define LOP_MK_FP           0x06
#define LOP_POP             0x07
#define LOP_XCHG            0x08
#define LOP_ADD             0x09
#define LOP_DUP             0x0a
#define LOP_NOP             0x0b


#define LOC_EXPR_IND        0x80
#define LOC_IND_REG         0x50  // to be moved back to wvdbg.h
#define IND_CALLOC_NEAR     0x00
#define IND_CALLOC_FAR      0x01
#define IND_RALLOC_NEAR     0x02
#define IND_RALLOC_FAR      0x03

#if _TARGET & _TARG_IAPX86
    #define LOC_MEMORY      LOC_MEM_16
#else
    #define LOC_MEMORY      LOC_MEM_32
#endif

#define FIELD_INTERNAL      0x01
#define FIELD_PUBLIC        0x02
#define FIELD_PROTECTED     0x04
#define FIELD_PRIVATE       0x08

#define NO_OFFSET   ((offset)-1)

typedef struct field_entry{
    union field_any  *next;
    enum {
        FIELD_OFFSET,    /* member */
        FIELD_LOC,       /* member */
        FIELD_STFIELD,   /* stfield*/
        FIELD_INHERIT,   /* bclass */
        FIELD_METHOD,    /* method */
        FIELD_NESTED,    /* nested */
        FIELD_VFUNC      /* vfunc info */
    }                   field_type;
} field_entry;

typedef struct{
    field_entry         entry;
    union {
        unsigned_32     off;
        dbg_loc         loc;
    }                   u;
    uint                attr;
    uint                len;
    dbg_type            base;
    byte                b_strt;
    byte                b_len;
    char                name[1]; /* variable */
} field_member;

typedef struct{
    field_entry         entry;
    dbg_loc             loc;
    uint                attr;
    dbg_type            base;
    char                name[1]; /* variable */
} field_stfield;

typedef struct {
    field_entry         entry;
    union {
        unsigned_32     off;
        dbg_loc         adjustor;
    }                   u;
    uint                attr;
    dbg_type            base;
    char                kind;
} field_bclass;

typedef struct{
    field_entry         entry;
    union {
        unsigned_32     off;
        dbg_loc         loc;
    }                   u;
    uint                attr;
    uint                kind;
    uint                len;
    dbg_type            base;
    char                name[1]; /* variable */
}field_method;

typedef struct{
    field_entry         entry;
    dbg_type            base;
    char                name[1]; /* variable */
}field_nested;

typedef struct{
    field_entry         entry;
    dbg_type            base;
    char                name[1]; /* variable */
}field_friend;

typedef struct{
    field_entry         entry;
    unsigned            vfptr_off;
    cg_type             vft_cgtype;
    int                 vft_size;
    dbg_type            base;
}field_vfunc;

typedef union field_any{
    field_entry   entry;
    field_member  member;
    field_stfield stfield;
    field_bclass  bclass;
    field_method  method;
    field_nested  nested;
    field_vfunc   vfunc;
}field_any;

typedef struct struct_list {
    field_any          *list;
    field_any           **list_tail;
    uint                num;
    unsigned            size;
    dbg_type            me;
    unsigned            is_struct :1;
    unsigned            is_nested :1;
    unsigned            is_cnested:1;
    unsigned            is_class  :1;
    unsigned            vtbl_off;
    dbg_type            vtbl_type;
    cg_type             ptr_type;
    int                 vtbl_esize;
    field_vfunc        *vf;
    char                name[1];
} struct_list;

typedef struct const_entry {
    struct const_entry  *next;
    signed_64           val;
    uint                len;
    char                name[1];
} const_entry;

typedef struct enum_list {
    const_entry *list;
    uint        num;
    cg_type     tipe;
    unsigned    is_nested :1;
    unsigned    is_c      :1;
} enum_list;

typedef struct dim_entry{
    union dim_any *next;
    enum {
        DIM_VAR,
        DIM_CON,
    }kind;
}dim_entry;

typedef struct{
    dim_entry   entry;
    back_handle dims;
    int         off;
    cg_type     lo_bound_tipe;
    cg_type     num_elts_tipe;
}dim_var;

typedef struct{
    dim_entry entry;
    signed_32 lo;
    signed_32 hi;
    dbg_type  idx;
}dim_con;

typedef union dim_any {
    dim_entry entry;
    dim_var   var;
    dim_con   con;
}dim_any;

typedef struct array_list {
    dim_any         *list;
    uint            num;
    unsigned        size;
    dbg_type        base;
    unsigned        is_col_major :1;
    unsigned        is_variable  :1;
} array_list;

typedef struct parm_entry {
    struct parm_entry   *next;
    dbg_type            tipe;
} parm_entry;

typedef struct proc_list {
    parm_entry  *list;
    uint        num;
    dbg_type    ret;
    cg_type     call;
    dbg_type    cls;
    dbg_type    this;
} proc_list;


typedef struct location {
    struct location         *next;
    union {
        union name          *be_sym;
        cg_sym_handle       fe_sym;
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
    enum {
        DBG_SYM_VAR,
        DBG_SYM_TYPE,
        DBG_SYM_TYPEDEF,
    } kind;
} dbg_local;

typedef struct {
    segment_id  segment;
    unsigned    offset;
} dbg_patch;

typedef struct block_patch {
    struct block_patch      *link;
    dbg_patch               patch;
} block_patch;

typedef struct name_entry {
    dbg_patch               patch;
    dbg_type                refno;
    dbg_type                scope;
    uint                    len;
    char                    name[1];
} name_entry;

typedef struct dbg_block {
    struct dbg_block        *parent;
    dbg_local               *locals;
    block_patch             *patches;
    back_handle             end_lbl;
    unsigned_32             start;
} dbg_block;

typedef struct dbg_rtn {
    dbg_loc                 reeturn;
    struct dbg_rtn          *parent;
    dbg_block               *rtn_blk;
    dbg_block               *blk;
    dbg_local               *parms;
    dbg_loc                 obj_loc;        /* for member functions */
    dbg_type                obj_type;       /* for member functions */
    unsigned_32             ret_offset;
    unsigned_32             epi_start;
    byte                    pro_size;
    byte                    obj_ptr_type;   /* for member functions */
    back_handle             end_lbl;
} dbg_rtn;

#define DB_BUFF_SIZE        511
#define MAX_FIXUPS          2

typedef enum {
    FIX_FORWARD,
    FIX_SYMBOL,
    FIX_BACKHANDLE
} fixup_kind;

typedef struct temp_buff {
    uint                    index;
    uint                    fix_idx;
    struct {
        pointer             p;
        byte                pos;
        fixup_kind          type;
    }                       fix[MAX_FIXUPS];
    byte                    buff[DB_BUFF_SIZE + 1];
} temp_buff;
