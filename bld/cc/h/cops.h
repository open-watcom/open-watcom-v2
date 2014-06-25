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
* Description:  Parse tree operators and data structures.
*
****************************************************************************/


#define IsConstLeaf(opnd)   (opnd->op.opr==OPR_PUSHINT||opnd->op.opr==OPR_PUSHFLOAT)

typedef enum ops {
    #define pick1(enum,dump,cgenum) enum,
    #include "copcodes.h"
    #undef pick1
} opr_code;

typedef enum condition_codes {
    #define pick1(enum,dump,cgenum) enum,
    #include "copcond.h"
    #undef pick1
} cond_code;

typedef enum{
    OPFLAG_NONE        = 0x00,          // nothing
    OPFLAG_CONST       = 0x01,          // this node contains a const
    OPFLAG_VOLATILE    = 0x02,          // this node contains a volatile
    OPFLAG_RVALUE      = 0x04,          // push rvalue of .  -> and *
    OPFLAG_LVALUE_CAST = 0x08,          // lvalue has been casted
    // Intel machines can have different kinds of pointers
    OPFLAG_NEARPTR     = 0x10,          // address is a near pointer
    OPFLAG_FARPTR      = 0x20,          // address is a far pointer
    OPFLAG_HUGEPTR     = 0x40,          // address is a huge pointer
    OPFLAG_FAR16PTR    = 0x60,          // address is a far16 pointer
    // RISC machines only have one kind of pointer,
    // but can have pointers to unaligned data
    OPFLAG_UNALIGNED   = 0x80,          // pointer points to unaligned data
    OPFLAG_MEM_MODEL =  OPFLAG_NEARPTR | OPFLAG_FARPTR
              | OPFLAG_HUGEPTR | OPFLAG_FAR16PTR,
}op_flags;
#define Far16Pointer(flags)   ((flags & OPFLAG_FAR16PTR) == OPFLAG_FAR16PTR)

typedef enum    pointer_class{
    PTR_NEAR = 0,
    PTR_BASED,
    PTR_FAR,
    PTR_FAR16,
    PTR_HUGE,
    PTR_INTERRUPT = PTR_HUGE,
    PTR_FUNC,
    PTR_FUNC_BASED,
    PTR_FUNC_FAR,
    PTR_FUNC_FAR16,
    PTR_FUNC_INTERRUPT,
    PTR_NOT,
}pointer_class;

#define FAR16_PTRCLASS(cls)     ((cls == PTR_FAR16) || (cls == PTR_FUNC_FAR16))

#define MAX_INLINE_DEPTH  3             // how deep to inline
typedef enum{
    FUNC_NONE         = 0x00,
    FUNC_OK_TO_INLINE = 0x01,           // can inline this node
    FUNC_INUSE        = 0x02,           // inuse as inline or gen
    FUNC_USES_SEH     = 0x04,           // uses structure exceptions
    FUNC_USED         = 0x08,           // function should really be emitted
    FUNC_MARKED       = 0x10,           // function marked for emit investigation
}func_flags;

typedef unsigned short  LABEL_INDEX;

struct patch_entry {
    TREEPTR owner;
    int     value;
};

typedef struct case_entry {
    struct case_entry   *next_case;
    int                 value;
    LABEL_INDEX         label;
    bool                gen_label;
} CASEDEFN, *CASEPTR;

typedef struct  switch_entry {
    struct switch_entry *prev_switch;
    LABEL_INDEX         default_label;
    int                 number_of_cases;
    CASEPTR             case_list;
    unsigned            low_value;
    unsigned            high_value;
    LABEL_INDEX         last_case_label;
    char                *case_format;   /* "%ld" or "%lu" */
} SWITCHDEFN, *SWITCHPTR;

typedef struct  string_literal {
    struct string_literal *next_string;
    BACK_HANDLE         back_handle;    /* back handle for string */
    unsigned short      length;         /* length of literal string */
    unsigned short      ref_count;      /* reference count */
    char                flags;          /* 0 or FLAG_FAR */
    char                *literal;       /* actual literal string */
} STRING_LITERAL;

// if we end up converting the string to binary to store in long_double
// then at the same time, we should set string[0] to '\0' to indicate
// that the string has been invalidated.
//
typedef struct floatval {
    struct floatval     *next;
    long_double         ld;
    unsigned short      len;
    unsigned char       type;
    char                string[2];
} FLOATVAL;

typedef short           tryindex_t;
#define TRYSCOPE_UNDEF  ((tryindex_t)-2)
#define TRYSCOPE_NONE   ((tryindex_t)-1)

struct expr_node;

typedef struct  opnode {
    opr_code            opr;            // see opr_code above
    op_flags            flags;
    union {
        DATA_TYPE       const_type;     // OPR_PUSHINT, OPR_PUSHFLOAT
        cond_code       cc;             // OPR_CMP: EQ,NE,LT,LE,GT,GE
        unsigned char   mathfunc;       // OPR_MATHFUNC
        unsigned char   unroll_count;   // OPR_STMT
    } u1;
    union {
        SYM_HANDLE      sym_handle;     // OPR_PUSHSYM, OPR_PUSHADDR, ...
                                        // OPR_CALL_INDIRECT
        source_loc      src_loc;        // OPR_STMT
        int             long_value;     // OPR_PUSHINT
        unsigned int    ulong_value;    // OPR_PUSHINT
        int64           long64_value;   // OPR_PUSHINT
        uint64          ulong64_value;  // OPR_PUSHINT
        FLOATVAL        *float_value;   // OPR_PUSHFLOAT
        STRING_LITERAL  *string_handle; // OPR_PUSHSTRING
        TYPEPTR         result_type;    // for operators(+-*/%|&^) etc
        TYPEPTR         compare_type;   // OPR_CMP
        LABEL_INDEX     label_count;    // OPR_LABELCOUNT
        LABEL_INDEX     label_index;
        void            *label_list;    // OPR_AND_AND, OPR_OR_OR
        SWITCHPTR       switch_info;    // OPR_SWITCH
        CASEPTR         case_info;      // OPR_CASE
        struct func_info {              // OPR_FUNCEND, OPR_RETURN
            SYM_HANDLE      sym_handle; // OPR_FUNCTION
            func_flags      flags;
        } func;
        struct { /* try_info */
            union {
                SYM_HANDLE      try_sym_handle; // OPR_EXCEPT, OPR_FINALLY
                tryindex_t      try_index;      // OPR_TRY
            } u;
            tryindex_t  parent_scope;
        } st;
        struct { /* ptr_conv_info */    // OPR_CONVERT_PTR
            char        oldptr_class;
            char        newptr_class;
        } sp;
    } u2;
} OPNODE;

typedef struct expr_node {
    struct expr_node    *left;
    struct expr_node    *right;
    union {
        TYPEPTR         expr_type;      // used during pass 1
        TREEPTR         thread;         // used during pass 2 full codegen
    } u;
    OPNODE          op;
    bool            visit;
    bool            checked;            // opnd values have been checked
} EXPRNODE;

typedef struct  sym_lists {
    struct sym_lists    *next;
    SYM_HANDLE          sym_head;
} SYM_LISTS;

extern TREEPTR  LeafNode(opr_code);
extern TREEPTR  ExprNode(TREEPTR,opr_code,TREEPTR);
extern TREEPTR  ErrorNode(TREEPTR);
extern void     FreeExprNode(TREEPTR);
extern void     FreeExprTree(TREEPTR);
extern void     InitExprTree(void);
extern void     NoOp(TREEPTR);
extern int      WalkExprTree( TREEPTR,
                    void (*operand)(TREEPTR),
                    void (*prefix_operator)(TREEPTR),
                    void (*infix_operator)(TREEPTR),
                    void (*postfix_operator)(TREEPTR) );
extern void     CastFloatValue(TREEPTR,DATA_TYPE);
extern void     CastConstValue(TREEPTR,DATA_TYPE);
extern void     MakeBinaryFloat(TREEPTR);
