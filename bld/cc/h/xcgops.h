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


// C Opcodes (intermediate code)
#ifndef _XCGOPS_INCLUDED
#define _XCGOPS_INCLUDED
#include "cgaux.h"

enum ops {
        OPR_ADD,        // +
        OPR_SUB,        // -
        OPR_MUL,        // *
        OPR_DIV,        // /
        OPR_NEG,        // negate
        OPR_CMP,        // compare
        OPR_MOD,        // %
        OPR_COM,        // ~
        OPR_NOT,        // !
        OPR_OR,         // |
        OPR_AND,        // &
        OPR_XOR,        // ^
        OPR_RSHIFT,     // >>
        OPR_LSHIFT,     // <<
        OPR_EQUALS,     // lvalue = rvalue
        OPR_OR_EQUAL,   // |=

        OPR_AND_EQUAL,  // &=   0x10
        OPR_XOR_EQUAL,  // ^=
        OPR_RSHIFT_EQUAL,// >>=
        OPR_LSHIFT_EQUAL,// <<=
        OPR_PLUS_EQUAL, // +=
        OPR_MINUS_EQUAL,// -=
        OPR_TIMES_EQUAL,// *=
        OPR_DIV_EQUAL,  // /=
        OPR_MOD_EQUAL,  // %=
        OPR_QUESTION,   // ?
        OPR_COLON,      // :
        OPR_OR_OR,      // ||
        OPR_AND_AND,    // &&
        OPR_POINTS,     // *ptr
        OPR_PUSHBACKHDL,// created by XCGBackName
        OPR_CALLBACK,   // callback

        OPR_POSTINC,    // lvalue++     0x20
        OPR_POSTDEC,    // lvalue--
        OPR_CONVERT,    // do conversion
        OPR_PUSHSYM,    // push sym_handle
        OPR_PUSHADDR,   // push address of sym_handle
        OPR_PUSHINT,    // push integer constant
        OPR_PUSHFLOAT,  // push float constant
        OPR_PUSHSTRING, // push address of string literal
        OPR_CONVERT_PTR,// convert pointer
        OPR_NOP,        // no operation
        OPR_DOT,        // sym.field
        OPR_ARROW,      // sym->field
        OPR_INDEX,      // array[index]
        OPR_ADDROF,     // & expr
        OPR_FARPTR,     // segment :> offset
        OPR_FUNCNAME,   // function name

        OPR_CALL,       // function call        0x30
        OPR_CALL_INDIRECT,// indirect function call
        OPR_PARM,       // function parm
        OPR_COMMA,      // expr , expr
        OPR_RETURN,     // return( expr )
        OPR_LABEL,      // label
        OPR_CASE,       // case label
        OPR_JUMPTRUE,   // jump if true
        OPR_JUMPFALSE,  // jump if false
        OPR_JUMP,       // jump
        OPR_SWITCH,     // switch
        OPR_FUNCTION,   // start of function
        OPR_FUNCEND,    // end of function
        OPR_STMT,       // node for linking statements together
        OPR_NEWBLOCK,   // start of new block with local variables
        OPR_ENDBLOCK,   // end of block

        OPR_TRY,        // start of try block   0x40
        OPR_EXCEPT,     // start of except block
        OPR_EXCEPT_CODE,// __exception_code
        OPR_EXCEPT_INFO,// __exception_info
        OPR_UNWIND,     // unwind from try block
        OPR_FINALLY,    // finally block
        OPR_END_FINALLY,// end of finally block
        OPR_ERROR,      // error node
        OPR_CAST,       // cast type
        OPR_LABELCOUNT, // number of labels used in function
        OPR_MATHFUNC,   // intrinsic math function eg. sin, cos,...
        OPR_MATHFUNC2,  // intrinsic math function with 2 parms, eg. atan2
        OPR_VASTART,    // va_start (for ALPHA)
        OPR_INDEX2,     // part of a multi-dimensional array
        OPR_ALLOCA,     // alloca (for ALPHA)
        OPR_PATCHNODE,  // patch node

        OPR_INLINE_CALL,// call is to be made inline
        OPR_TEMPADDR,   // address of temp
        OPR_PUSHTEMP,   // push value of temp
        OPR_PUSH_PARM,  // push parm onto stack (Alpha)
        OPR_POST_OR,    // C++ "bool++" operation
        OPR_SIDE_EFFECT,// similar to OPR_COMMA
        OPR_INDEX_ADDR, // want the address of an index expression
        OPR_DBG_BEGBLOCK,// start of new block with local variables
        OPR_DBG_ENDBLOCK,// end of block
        OPR_ABNORMAL_TERMINATION, // SEH _abnormal_termination()
};

enum  condition_codes {
        CC_EQ,
        CC_NE,
        CC_LT,
        CC_LE,
        CC_GT,
        CC_GE,
        CC_ALWAYS,
        CC_B,
        CC_BE,
        CC_A,
        CC_AE
};

typedef enum{
        OPFLAG_NONE        = 0x00,         // nothing
        OPFLAG_CONST       = 0x01,         // this node contains a const
        OPFLAG_VOLATILE    = 0x02,         // this node contains a volatile
        OPFLAG_RVALUE      = 0x04,         // push rvalue of .  -> and *
        OPFLAG_LVALUE_CAST = 0x08,      // lvalue has been casted
        // Intel machines can have different kinds of pointers
        OPFLAG_NEARPTR     = 0x10,         // address is a near pointer
        OPFLAG_FARPTR      = 0x20,         // address is a far pointer
        OPFLAG_HUGEPTR     = 0x40,         // address is a huge pointer
        OPFLAG_FAR16PTR    = 0x60,         // address is a far16 pointer
        // RISC machines only have one kind of pointer,
        // but can have pointers to unaligned data
        OPFLAG_UNALIGNED   = 0x80,         // pointer points to unaligned data
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

typedef unsigned short  LABEL_INDEX;

typedef struct expr_node *TREEPTR;

struct patch_entry {
        TREEPTR owner;
        long    value;
};

typedef struct  case_entry {
        struct  case_entry *next_case;
        long    value;
        int     label;
} CASEDEFN, *CASEPTR;

typedef struct  switch_entry {
        struct  switch_entry *prev_switch;
        unsigned int     default_label;
        int              number_of_cases;
        struct  case_entry *case_list;
        unsigned long    low_value;
        unsigned long    high_value;
        char            *case_format;           /* "%ld" or "%lu" */
} SWITCHDEFN, *SWITCHPTR;

typedef struct  string_literal {
        struct string_literal *next_string;
        back_handle     cg_back_handle; /* back handle for string */
        unsigned short  length;         /* length of literal string   */
        unsigned short  ref_count;      /* reference count */
        char            flags;          /* 0 or FLAG_FAR */
        char            literal[1];     /* actual literal string */
} STRING_LITERAL;

#include "xfloat.h"

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
struct expr_node;

typedef struct  opnode {
    unsigned short      unused;
    enum ops            opr;            // see enum ops above
    union {
        op_flags                flags;
        unsigned char           const_type;     // OPR_PUSHINT, OPR_PUSHFLOAT
        enum  condition_codes   cc;             // OPR_CMP: EQ,NE,LT,LE,GT,GE
        unsigned char           mathfunc;       // OPR_MATHFUNC
        unsigned char           unroll_count;   // OPR_STMT
    };
    union {
        cg_sym_handle   sym_handle;     // OPR_PUSHSYM, OPR_PUSHADDR, ...
                                        // OPR_CALL_INDIRECT
        unsigned        source_fno;     // OPR_STMT
        long            long_value;     // OPR_PUSHINT
        unsigned long   ulong_value;    // OPR_PUSHINT
        FLOATVAL        *float_value;   // OPR_PUSHFLOAT
        STRING_LITERAL  *string_handle; // OPR_PUSHSTRING
        TYPEPTR         result_type;    // for operators(+-*/%|&^) etc
        TYPEPTR         compare_type;   // OPR_CMP
        int             label_count;    // OPR_LABELCOUNT
        LABEL_INDEX     label_index;
        void            *label_list;    // OPR_AND_AND, OPR_OR_OR
        SWITCHPTR       switch_info;    // OPR_SWITCH
        struct expr_node *func_info;    // OPR_FUNCTION - list of parms
                                        // OPR_FUNCEND, OPR_RETURN
        struct  try_info {
            union {
                SYM_HANDLE      try_sym_handle; // OPR_EXCEPT, OPR_FINALLY
                short           try_index;      // OPR_TRY
            };
            short       parent_scope;
        };
        struct  ptr_conv_info {         // OPR_CONVERT_PTR
            char        oldptr_class;
            char        newptr_class;
        };
    };
} OPNODE;
#if 0
enum    cgflags {
    CG_DO_NOTHING       = 0x01, // ignore this node
    CG_LVALUE           = 0x02, // this is an lvalue
    CG_UNSIGNED         = 0x04, // perform unsigned operation (vs signed)
    CG_SMALL_STRUCT     = 0x04, // dtype=TYPE_STRUCT, size = 1,2,or 4
    CG_VALUE_USED       = 0x08, // value of tree is used in bigger expr
    CG_TEMP             = 0x10, // value has been stored into a temp
    CG_DONT_USE_EAX_EDX = 0x20, // don't use eax or edx in this node
    CG_LEFT_OP_EQUALS   = 0x40, // indicates left opnd of op= operator
    CG_PREFERRED_REG    = 0x80, // indicates that tree->reg is preferred
                                // e.g. right node of shift is ECX
};

#define CNV_INTEGER (CNV_INT | CNV_UINT | CNV_LONG | CNV_ULONG | CNV_LONGLONG)
#define CNV_FLOATING    (CNV_FLOAT | CNV_DOUBLE | CNV_LONG_DOUBLE)
#endif
typedef struct expr_node {
    struct expr_node    *left;
    struct expr_node    *right;
    union {
        TYPEPTR         expr_type;      // used during pass 1
        TREEPTR         thread;         // used during pass 2 full codegen
        int     srclinenum;             // OPR_STMT, and OPR_NOP for callnode
    };
    OPNODE          op;
} EXPRNODE;

extern  TREEPTR LeafNode(int);
extern  TREEPTR ExprNode(TREEPTR,int,TREEPTR);
extern  TREEPTR ErrorNode(TREEPTR);
extern  void    FreeExprNode(TREEPTR);
extern  void    FreeExprTree(TREEPTR);
extern  void    InitExprTree(void);
extern  void    NoOp(TREEPTR);
extern  int     WalkExprTree( TREEPTR,
                        void (*operand)(TREEPTR),
                        void (*prefix_operator)(TREEPTR),
                        void (*infix_operator)(TREEPTR),
                        void (*postfix_operator)(TREEPTR) );
extern  void    CastFloatValue(TREEPTR,int);
extern  void    CastConstValue(TREEPTR,int);
extern void     InitExpressCode(int,int);
#endif
