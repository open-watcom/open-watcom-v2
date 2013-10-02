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


typedef struct {
    segment_id  segment;
    unsigned    offset;
} dbg_patch_handle;

typedef struct name_entry {
    dbg_patch_handle    patch;
    dbg_type            refno;
    dbg_type            scope;
    uint                len;
    char                name[1];
} name_entry;

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
    unsigned long       vfptr_off;
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
    unsigned long       size;
    dbg_type            me;
    unsigned            is_struct :1;
    unsigned            is_nested :1;
    unsigned            is_cnested:1;
    unsigned            is_class  :1;
    unsigned long       vtbl_off;
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
    unsigned long   size;
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

