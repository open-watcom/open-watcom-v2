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


typedef enum {
     S_NONE,
     S_INIT,
     S_START,
     S_STOP,
     S_FINI
} cg_state;

typedef enum
{    LEAF
,    OP
,    CALLBACK
} nclass;

typedef pointer sym;

typedef struct name_entry {
    dbg_type            refno;
    dbg_type            scope;
    char                *name;
} name_entry;


typedef struct field_entry{
    union field_any  *next;
    enum {
        FIELD_OFFSET,    /* member */
        FIELD_LOC,       /* member */
        FIELD_STFIELD,    /* static */
        FIELD_INHERIT,   /* bclass */
        FIELD_METHOD,    /* method */
        FIELD_NESTED,    /* nested */
        FIELD_VFUNC      /* vfunc info */
    }                   field_type;
} field_entry;

typedef void CB_FUN( pointer );     // call-back prototype

typedef struct call_back {          // call-back entry (pointed to by l field)
    CB_FUN* function;               // - function
    pointer data;                   // - data passed to it
} call_back;

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


typedef union field_any{
    field_entry   entry;
    field_member  member;
    field_stfield stfield;
    field_bclass  bclass;
    field_method  method;
    field_nested  nested;
}field_any;

typedef struct struct_list {
    field_entry         *list;
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
    char                name[1];
} struct_list;
typedef struct const_entry {
    struct const_entry  *next;
    signed_32           val;
    char                *name;
} const_entry;

typedef struct dim_entry{
    struct dim_entry *next;
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

typedef union{
    dim_entry entry;
    dim_var   var;
    dim_con   con;
}dim_any;

typedef struct array_list {
    dim_entry      *list;
    uint            num;
    unsigned        size;
    dbg_type        base;
    unsigned        is_col_major :1;
} array_list;
typedef struct enum_list {
    uint                num;
    const_entry         *list;
    cg_type             tipe;
} enum_list;

typedef struct parm_entry {
    struct parm_entry   *next;
    dbg_type            tipe;
} parm_entry;
typedef struct proc_list {
    uint                num;
    struct parm_entry   *list;
    cg_type             call;
    dbg_type            ret;
} proc_list;

typedef struct a {
    struct a    *n;
    sym_handle  s;
    int         o;
} a;
typedef struct s {
    struct s    *n;
    fe_attr     a;
    sym_handle  s;
} s;
typedef struct n {
    struct n    *n;
    nclass      c;
    cg_op       o;
    struct n    *l;
    struct n    *r;
    int         i;
    int         id;
    cg_type     t;
    byte        st;
    byte        ln;
    sym_handle  h;
    pointer     src;        // source, when supplied by CGFEName
    int         burnt;      // node has been past to a cg routine
} n;
typedef struct l {
    struct l    *n;
    int         i;
    int         ddef;
    int         dref;
    sym_handle  cref;
    sym_handle  cdef;
    int         idef;
} l;
typedef struct b {
    struct b    *n;
    sym s;
    l   *lp;
    unsigned_32 loc;
    int i;
} b;
typedef struct t {
    struct t    *n;
    int i;
} t;
typedef struct ip {
    struct ip   *n;
    cg_type     t;
} ip;
typedef struct ic {
    struct ic   *n;
    struct ip   *p;
    int         c;
    int         d;
    cg_type     t;
    sym_handle  h;
} ic;
typedef struct rh {
    struct rh   *n;
    signed_32   l;
    signed_32   h;
    l   *lb;
} rh;
typedef struct sh {
    l   *o;
    rh  *r;
    int i;
} sh;

#define MAX_SEG         500
#define MIN_SEG         0
#define UBUFF_LEN       128

enum {
    O_CALL = MAX_OP,
    O_PARM,
    O_RETURN,
    O_LV_GETS,
    O_LV_PRE_GETS,
    OP_CHOOSE,
    OP_JOIN,
    OP_BIT_FIELD,
    OP_WARP,
    STUB_MAX_OP
};
