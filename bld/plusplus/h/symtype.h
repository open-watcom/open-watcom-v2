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


#ifndef _SYMTYPE_H

typedef struct cdopt_cache CDOPT_CACHE;
typedef struct friend_list FRIEND;
typedef struct name_space NAME_SPACE;
typedef struct using_ns USING_NS;
typedef struct sym_region SYM_REGION;
typedef struct scope *SCOPE;
typedef struct symbol_name *SYMBOL_NAME;
typedef struct decl_spec DECL_SPEC;
typedef struct base_class BASE_CLASS;
typedef struct search_result SEARCH_RESULT;
typedef struct class_table CLASS_TABLE;
typedef struct class_vbtable CLASS_VBTABLE;
typedef struct class_vftable CLASS_VFTABLE;
typedef struct thunk_cast THUNK_CAST;
typedef struct thunk_action THUNK_ACTION;
typedef struct member_ptr_cast MEMBER_PTR_CAST;
typedef struct gen_leap GEN_LEAP;
typedef struct reloc_list RELOC_LIST;

typedef struct parse_tree_node *PTREE;          // defined in PTREE.H
typedef struct rewrite_package REWRITE;         // defined in REWRITE.H
typedef struct template_info TEMPLATE_INFO;     // defined in TEMPLATE.H
typedef struct fn_template_defn FN_TEMPLATE_DEFN;//defined in TEMPLATE.H
typedef struct func_list FNOV_LIST;             // defined in FNOVLOAD.H
typedef struct pool_con POOL_CON;               // defined in CONPOOL.H

#include "linkage.h"
#include "toknlocn.h"
#include "hashtab.h"

/* types used for collecting decl-specifiers */

/* only one of these may be specified except typedef+mutable is allowed */
typedef enum {
    STG_TYPEDEF         = 0x01,
    STG_REGISTER        = 0x02,
    STG_AUTO            = 0x04,
    STG_EXTERN          = 0x08,
    STG_STATIC          = 0x10,
    STG_MUTABLE         = 0x20,
    STG_NULL            = 0x00
} stg_class_t;

/* any one of these may be specified */
typedef enum {
    STY_CONST           = 0x01,
    STY_VOLATILE        = 0x02,
    STY_FRIEND          = 0x04,
    STY_INLINE          = 0x08,
    STY_VIRTUAL         = 0x10,
    STY_EXPLICIT        = 0x20,
    STY_UNALIGNED       = 0x40,
    STY_DSPEC_MOD       = ( STY_CONST
                          | STY_VOLATILE
                          | STY_UNALIGNED
                          ),
    STY_FN_MODIFIER     = ( STY_FRIEND
                          | STY_INLINE
                          | STY_VIRTUAL
                          | STY_EXPLICIT
                          ),
    STY_NULL            = 0x00
} specifier_t;

/* only certain combinations may be specified */
typedef enum {
    STM_CHAR            = 0x0001,
    STM_INT             = 0x0002,
    STM_LONG            = 0x0004,
    STM_FLOAT           = 0x0008,
    STM_DOUBLE          = 0x0010,
    STM_SIGNED          = 0x0020,
    STM_UNSIGNED        = 0x0040,
    STM_SHORT           = 0x0080,
    STM_VOID            = 0x0100,
    STM_SEGMENT         = 0x0200,
    STM_BOOL            = 0x0400,
    STM_INT64           = 0x0800,
    STM_NULL            = 0x0000
} scalar_t;

typedef enum {
    STS_DLLIMPORT       = 0x0001,
    STS_DLLEXPORT       = 0x0002,
    STS_NAKED           = 0x0004,
    STS_THREAD          = 0x0008,
    STS_MODIFIER        = 0x0010,
    STS_NULL            = 0x0000
} ms_declspec_t;

// check PTypeCombine if any new fields are added
struct decl_spec {
    DECL_SPEC           *prev;
    SYMBOL              typedef_defined;    // typedef needs to be def'd
    SYMBOL              prev_sym;           // 'p' in char *p, q;
    SCOPE               scope;              // 'S::' part of the type
    TYPE                partial;            // typedef part of decl-spec
    TYPE                ms_declspec_fnmod;  // __declspec( <fn-modifier> )
    LINKAGE             linkage;            // non-NULL if extern "?"
    PTREE               id;                 // id from typedef references
    char                *name;              // name of typedef ('C' in class C)
    stg_class_t         stg_class;          // storage class part of decl-spec
    scalar_t            scalar;             // scalar tokens part of decl-spec
    specifier_t         specifier;          // type qualifiers part of decl-spec
    ms_declspec_t       ms_declspec;        // __declspec( <id> ) modifiers

    int                 : 0;
    unsigned            type_defined : 1;   // a type was defined
    unsigned            type_declared : 1;  // a type was declared
    unsigned            ctor_name : 1;      // decl-spec can be a ctor-name
    unsigned            is_default : 1;     // no decl-spec has been specified
    unsigned            diagnosed : 1;      // use of decl-spec diagnosed
    unsigned            decl_checked : 1;   // type checked (& built) already
    unsigned            type_elaborated : 1;// "class C" or "enum E"
    unsigned            nameless_allowed :1;// nameless declaration is allowed

    unsigned            generic : 1;        // <class T, ... templare type arg
    unsigned            class_instantiation:1;// C<X,x> instantiation
    unsigned            no_more_linkage : 1;// no more extern "?" allowed
    unsigned            arg_declspec : 1;   // decl-specs for an argument
    unsigned            class_idiom : 1;    // "class C;" idiom used

    int : 0;
};

typedef struct decl_info DECL_INFO;
struct decl_info {
    DECL_INFO           *next;
    DECL_INFO           *parms;         // function parms (NULLable)
    PTREE               id;             // declarator id (NULLable)
    SCOPE               scope;          // 'C' part of 'C::id' (NULLable)
    SCOPE               friend_scope;   // SCOPE friend should be in
    SYMBOL              sym;            // declared symbol (NULLable)
    SYMBOL              generic_sym;    // symbol for template <class T>
    SYMBOL              proto_sym;      // SC_DEFAULT sym for this parm
    TYPE                list;           // list of declarator types
    TYPE                type;           // final declared type
    PTREE               defarg_expr;    // initial/default value (NULLable)
    REWRITE             *body;          // storage for function body
    REWRITE             *mem_init;      // storage for mem-initializer
    REWRITE             *defarg_rewrite;// storage for default argument
    char                *name;          // name of symbol (NULLable)
    TOKEN_LOCN          init_locn;      // location of '(' for inits
    unsigned            sym_used : 1;   // don't free 'sym'
    unsigned            friend_fn : 1;  // symbol is a friend function
    unsigned            fn_defn : 1;    // function is being defined
    unsigned            template_member:1;// declaring a template member
    unsigned            has_dspec : 1;  // has decl-specifiers (set by DeclFunction)
    unsigned            has_defarg : 1; // has default argument
    unsigned            explicit_parms : 1;// explicit parms in declarator
};

// types dealing with representing types

typedef enum {
    TYP_MIN             = 0x00,
    TYP_ERROR           = 0x00,
    TYP_BOOL            = 0x01,
    TYP_CHAR            = 0x02,
    TYP_SCHAR           = 0x03,
    TYP_UCHAR           = 0x04,
    TYP_WCHAR           = 0x05,
    TYP_SSHORT          = 0x06,
    TYP_USHORT          = 0x07,
    TYP_SINT            = 0x08,
    TYP_UINT            = 0x09,
    TYP_SLONG           = 0x0a,
    TYP_ULONG           = 0x0b,
    TYP_SLONG64         = 0x0c,
    TYP_ULONG64         = 0x0d,
    TYP_FLOAT           = 0x0e,
    TYP_DOUBLE          = 0x0f,
    TYP_LONG_DOUBLE     = 0x10,
    TYP_ENUM            = 0x11,
    TYP_POINTER         = 0x12,
    TYP_TYPEDEF         = 0x13,
    TYP_CLASS           = 0x14,
    TYP_BITFIELD        = 0x15,
    TYP_FUNCTION        = 0x16,
    TYP_ARRAY           = 0x17,
    TYP_DOT_DOT_DOT     = 0x18,
    TYP_VOID            = 0x19,
    TYP_MODIFIER        = 0x1a,
    TYP_MEMBER_POINTER  = 0x1b,
    TYP_GENERIC         = 0x1c,
    TYP_FREE            = 0x1d,
    TYP_MAX,

    TYP_FIRST_VALID     = TYP_BOOL,
} type_id;

typedef enum {
    TF1_FIRST           = 0x00000001,               // TYP_ERROR
    TF1_SPECIAL_FMT     = 0x00000002,
    TF1_DEFAULT         = 0x00000001,               // TYP_SINT
    TF1_CLEAN           = 0x00000002,
    TF1_SEGMENT         = 0x00000001,               // TYP_USHORT
    TF1_ZERO_SIZE       = 0x00000001,               // TYP_ARRAY
    TF1_LOADDS          = 0x00000001,               // TYP_FUNCTION
    TF1_SAVEREGS        = 0x00000002,               // *************************
    TF1_PLUSPLUS        = 0x00000004,               // * Remember to add any   *
    TF1_INLINE          = 0x00000008,               // * new flags that should *
    TF1_VIRTUAL         = 0x00000010,               // * be ignored during a   *
    TF1_INTERRUPT       = 0x00000020,               // * fn type comparision   *
    TF1_PURE            = 0x00000040,               // * to TF1_FN_IGNORE      *
    TF1_STACK_CHECK     = 0x00000080,               // *************************
    TF1_INTRINSIC       = 0x00000100,
    TF1_EXPLICIT        = 0x00000200,
    TF1_NAKED           = 0x00000400,
    TF1_PLUSPLUS_SET    = 0x80000000,   /* note 1 */// don't change TF1_PLUSPLUS
    TF1_CONST           = 0x00000001,               // TYP_MODIFIER
    TF1_VOLATILE        = 0x00000002,
    TF1_MUTABLE         = 0x00000004,
    TF1_NEAR            = 0x00000010,
    TF1_FAR             = 0x00000020,
    TF1_FAR16           = 0x00000040,
    TF1_HUGE            = 0x00000080,
    TF1_BASED           = 0x00000700,               // mask to extract type of base
    TF1_BASED_STRING    = 0x00000100,               // _based( "string" )
    TF1_BASED_SELF      = 0x00000200,               // _based((_segment)_self)
    TF1_BASED_VOID      = 0x00000300,               // _based(void)
    TF1_BASED_FETCH     = 0x00000400,               // _based(s) (_segment s;)
    TF1_BASED_ADD       = 0x00000500,               // _based(p) (char *p;)
    TF1_DLLEXPORT       = 0x00000800,
    TF1_DLLIMPORT       = 0x00001000,
    TF1_UNALIGNED       = 0x00002000,
    TF1_THREAD          = 0x00004000,
    TF1_COMMON          = 0x80000000,   /* note 2 */// same addr in all modules
    TF1_IN_MEM          = 0x40000000,   /* note 2 */// sym was used in inline asm
    TF1_TYP_FUNCTION    = 0x80000000,   /* note 1 */// modifier is a fn modifier
    TF1_OUTERMOST       = 0x40000000,   /* note 1 */// modifier percolates out
    TF1_HUG_FUNCTION    = 0x20000000,   /* note 1 */// modifier hugs functions

    /* notes:
        (1) flag is only used in pre-FinishDeclarator types (thrown away)
        (2) flag is only used to adjust post-FinishDeclarator types (can
            occupy note 1 bit positions)
    */
    TF1_REFERENCE       = 0x00000001,               // TYP_POINTER
    TF1_FAR_BUT_NEAR    = 0x00000002,               // object is near but typed as far
    TF1_UNNAMED         = 0x00000001,               // TYP_ENUM, TYP_TYPEDEF
    TF1_UNION           = 0x00000001,               // TYP_CLASS
    TF1_STRUCT          = 0x00000002,
    TF1_UNBOUND         = 0x00000004,               // unbound class template
    TF1_INSTANTIATION   = 0x00000008,               // class is an instance of a class template
    TF1_SPECIFIC        = 0x00000010,               // class is a specific instantiation
    TF1_VISITED         = 0x80000000,               // used temporarily in traversals
    TF1_STDOP           = 0x00000001,               // TYP_VOID for Std Ops
    TF1_STDOP_ARITH     = 0x00000002,               // TYP_VOID for arith Std Ops
    TF1_USED            = 0x00000001,               // TYP_GENERIC

    /* used (during parsing) to quietly change __huge to __far for the 386 */
#if _CPU == 8086
    TF1_SET_HUGE        = TF1_HUGE,
#else
    TF1_SET_HUGE        = TF1_FAR,
#endif
    /* used (during parsing) to quietly change __far16 to __far for the 8086 */
#if _CPU == 386
    TF1_SET_FAR16       = TF1_FAR16,
#else
    TF1_SET_FAR16       = TF1_FAR,
#endif

    /* multi-bit constants */
    TF1_MOD_IGNORE      = ( TF1_COMMON          // ignored during type comparison
                          | TF1_IN_MEM
                          ),
    TF1_FN_IGNORE       = ( TF1_INLINE          // ignored during type comparison
                          | TF1_VIRTUAL
                          | TF1_STACK_CHECK
                          | TF1_LOADDS
                          | TF1_INTRINSIC
                          | TF1_NAKED
                          | TF1_PURE
                          | TF1_EXPLICIT
                          ),
    TF1_FN_THUNK_KEEP   = ( TF1_PLUSPLUS        // fn-mods to keep when making
                          | TF1_LOADDS          // a thunk function for it
                          | TF1_SAVEREGS
                          | TF1_STACK_CHECK
                          ),
    TF1_MPTR_REMOVE     = ( TF1_MUTABLE         // flags to remove when creating
                          | TF1_DLLEXPORT
                          ),                    // the base type of a member ptr
    TF1_FN_MEMBER       = ( TF1_SAVEREGS        // allowable fn-modifiers for
                          | TF1_LOADDS          // member functions
                          ),
    TF1_FN_PERMANENT    = ( TF1_PLUSPLUS        // when combining two function
                          ),                    // types, these flags in the
                                                // original cannot change
    TF1_FN_CANT_ADD_LATER=( TF1_VIRTUAL         // flags that cannot be added
                          | TF1_PURE            // in subsequent declarations
                          | TF1_EXPLICIT
                          ),
    TF1_CV_MASK         = ( TF1_CONST
                          | TF1_VOLATILE
                          ),
    TF1_STAY_MEMORY     = ( TF1_VOLATILE        // object should not be in reg
                          | TF1_IN_MEM
                          ),
    TF1_MEM_MODEL       = ( TF1_NEAR
                          | TF1_FAR
                          | TF1_FAR16
                          | TF1_HUGE
                          | TF1_BASED
                          ),
    TF1_THIS_MASK       = ( TF1_CV_MASK         // relevant for "this" qualifier
                          | TF1_UNALIGNED
                          | TF1_MEM_MODEL
                          ),
    TF1_DISPLAY         = ( TF1_CV_MASK         // visible during type formatting
                          | TF1_MUTABLE
                          | TF1_UNALIGNED
                          | TF1_DLLIMPORT
                          | TF1_THREAD
                          | TF1_DLLEXPORT
                          | TF1_MEM_MODEL
                          ),
    TF1_OK_FOR_VAR      = ( TF1_DLLEXPORT ),    // mods that like to modify fns
                                                // but are OK for vars
    TF1_OK_FOR_AUTO     = ( TF1_CONST           // mods that can be used for
                          | TF1_VOLATILE        // auto vars
                          | TF1_IN_MEM ),
    TF1_MOD_MOVE        = ( TF1_DLLEXPORT       // mods that can move past fn
                          | TF1_MEM_MODEL       // and array declarators
                          ),
    TF1_MOD_MEMBER      = ( TF1_DLLEXPORT       // allowable modifiers for
                          | TF1_THREAD          // all members
                          | TF1_DLLIMPORT
                          ),
#if _CPU == 8086
    TF1_DEFAULT_FAR     = ( TF1_DLLEXPORT       // mods that cause default far
                          | TF1_DLLIMPORT       // to be used for the mem modifier
                          ),
#else
    TF1_DEFAULT_FAR     = TF1_FAR,
#endif
    TF1_MOD_OMIT_LATER  = ( TF1_DLLEXPORT       // modifiers that can be spec'd
                          | TF1_THREAD          // once but don't have to be
                          | TF1_DLLIMPORT       // repeated in redeclarations
                          ),
    TF1_MOD_ADD_LATER   = ( TF1_DLLEXPORT       // modifiers that can be added
                          ),                    // in subsequent declarations
                                                // (uses of this in decl.c must
                                                //  be fixed if TF1_MEM_MODEL
                                                //  is added to this mask)

    TF1_NULL            = 0x00000000
} type_flag;

/*
    These bits control access to the two fields in the 'dbg_info'
    structure, namely, 'dwh' and 'type'.
*/
typedef enum {                                  // for dbg field
    TF2_DWARF_DEF       = 0x01,                 // dwarf is defined
    TF2_DWARF_FWD       = 0x02,                 // dwarf is forward ref
    TF2_SYMDBG          = 0x04,                 // Watcom/CV4 info stored
    TF2_HASH            = 0x08,                 // used for type hash value
    TF2_DBG_IN_PCH      = 0x10,                 // type is in PCH (affects dbg info)
    TF2_PCH_DBG_EXTERN  = 0x20,                 // dwarf handle is in pch_handle

    TF2_DWARF           = ( TF2_DWARF_DEF       // contains dwarf handle
                          | TF2_DWARF_FWD ),
    TF2_NON_SYMDBG      = ( TF2_DWARF           // symdbg is not active
                          | TF2_HASH ),         // if these are on
    TF2_NULL            = 0x00
} type_dbgflag;

typedef PCH_struct {            /* used for keeping track of function arguments */
    unsigned    num_args;
    type_flag   qualifier;
    int         : 0;
    TYPE        *except_spec;   /* (NULL==...) array of types (NULL terminated) */
    TYPE        type_list[1];
} arg_list;

#define AUTO_ARG_MAX 16         // # arg.s in temporary structs

typedef struct                  // TEMP_ARG_LIST
{   arg_list base;              // - base
    TYPE array[ AUTO_ARG_MAX-1];// - default # (1 in arg_list)
} TEMP_ARG_LIST;

typedef PTREE TEMP_PT_LIST[ AUTO_ARG_MAX ]; // TEMP_PT_LIST

//
// list of comparisons to exclude in TypeCompareExclude
//
typedef enum {
    TC1_NOT_ENUM_CHAR   = 0x0001,       // don't exclude enum and char
    TC1_PTR_FUN         = 0x0002,       // ignore function vs. ptr to function
    TC1_FUN_LINKAGE     = 0x0004,       // ignore function linkage
    TC1_NOT_MEM_MODEL   = 0x0008,       // ignore default memory model
    TC1_NULL            = 0x0000
} type_exclude;

PCH_struct friend_list {
    FRIEND              *next;          // - next in ring
    SYMBOL              sym;            // - friendly symbol
};

struct reloc_list {
    RELOC_LIST *next;
    SYMBOL orig;
    SYMBOL dest;
};

typedef enum {
    IN_PRIVATE          = 0x0001,       // base access specifier
    IN_PROTECTED        = 0x0002,
    IN_PUBLIC           = 0x0004,       // used to catch duplicates
    IN_VIRTUAL          = 0x0008,       // direct virtual base
    IN_INDIRECT_VIRTUAL = 0x0010,       // indirect virtual base
    IN_USING_VF_INDICES = 0x0020,       // we are reusing this base's vfn indices
    IN_DIRECT           = 0x0040,       // base is a direct base
    IN_LATTICE          = 0x0080,       // lattice structure has been formed
    IN_CTOR_DISP        = 0x0100,       // base class has ctor-disp allocated
    IN_FREE             = 0x8000,       // used for precompiled headers
    IN_ACCESS_SPECIFIED = ( IN_PROTECTED
                          | IN_PUBLIC
                          | IN_PRIVATE ),
    IN_IS_VIRTUAL       = ( IN_VIRTUAL
                          | IN_INDIRECT_VIRTUAL ),
    IN_NULL             = 0x0000
} inherit_flag;

typedef enum                            // PC_CALLIMPL -- PC call implementations
{   CALL_IMPL_CPP                       // - normal C++, not ellipsis
,   CALL_IMPL_C                         // - normal C, not ellipsis
,   CALL_IMPL_REV_CPP                   // - reversed, C++
,   CALL_IMPL_REV_C                     // - reversed, C
,   CALL_IMPL_ELL_CPP                   // - ellipsis, C++
,   CALL_IMPL_ELL_C                     // - ellipsis, extern "C"
} PC_CALLIMPL;

typedef uint_32     dbg_handle;

typedef struct {
    dbg_handle      handle;             // debug info handle
    dbg_handle      pch_handle;         // debug info handle from PCH
} dbg_info;

PCH_struct base_class {
    BASE_CLASS          *next;
    TYPE                type;           // base class type
    target_offset_t     delta;          // offset within this class
    uint_16             vb_index;       // index into virtual base table
    inherit_flag        flag;           // inheritance flags
};

#define _IsIndirectVirtualBase( b ) \
        (((b)->flag & IN_INDIRECT_VIRTUAL) != 0 )
#define _IsDirectVirtualBase( b ) \
        (((b)->flag & IN_VIRTUAL) != 0 )
#define _IsDirectNonVirtualBase( b ) \
        (((b)->flag & IN_IS_VIRTUAL) == 0 )
#define _IsVirtualBase( b )     \
        (((b)->flag & IN_IS_VIRTUAL) != 0 )
#define _IsDirectBase( b )      \
        (((b)->flag & IN_DIRECT) != 0 )
#define _UsingVFIndices( b )      \
        (((b)->flag & IN_USING_VF_INDICES) != 0 )

typedef PCH_struct {
    BASE_CLASS      *bases;         // base classes
    FRIEND          *friends;       // ring of friends
    char            *name;          // name of class
    CDOPT_CACHE     *cdopt_cache;   // CDOPT info cache
    TYPE            class_mod;      // type representing class <mods> X mods
    CGREFNO         refno;          // code-generator ref #
    dbg_handle      dbg_no_vbases;  // for Watcom -d2 info
    target_offset_t size;           // size of class (including vbases)
    target_offset_t vsize;          // size of class (excluding vbases)
    target_offset_t vb_offset;      // offset of vbptr field
    target_offset_t vf_offset;      // offset of vfptr field
    uint_16         last_vfn;       // last virtual function index def'd
    uint_16         last_vbase;     // last virtual base index def'd
    uint_16         index;          // class unique ordering index
    uint_8          max_align;      // maximum alignment of fields

    int             : 0;

    unsigned        defined : 1;    // class is defined fully
    unsigned        opened : 1;     // class defn has been started
    unsigned        unnamed : 1;    // cannot have ctors, dtors, etc.
    unsigned        corrupted : 1;  // errors occurred during declaration!
    unsigned        abstract : 1;   // contains pure virtual functions
    unsigned        abstract_OK : 1;// abstract flag is set properly
    unsigned        anonymous : 1;  // class is an anonymous union/struct
    unsigned        has_def_opeq : 1; // has an explicit default operator=

    unsigned        has_ctor : 1;   // has an explicit constructor
    unsigned        has_dtor : 1;   // has an explicit dtor
    unsigned        has_pure : 1;   // has an explicit pure fn
    unsigned        has_vfptr : 1;  // has a vfptr field
    unsigned        has_vbptr : 1;  // has a vbptr field
    unsigned        has_data : 1;   // contains non-static data members
    unsigned        has_vfn : 1;    // contains virtual functions
    unsigned        has_vcdtor : 1; // contains an explicit ctor/dtor in
                                    // the presence of virtual functions

    unsigned        ctor_defined :1;// default ctor defined
    unsigned        copy_defined :1;// default copy ctor defined
    unsigned        dtor_defined :1;// default dtor defined
    unsigned        assign_defined:1;// default assignment defined
    unsigned        ctor_gen : 1;   // default ctor generated
    unsigned        copy_gen : 1;   // default copy ctor generated
    unsigned        dtor_gen : 1;   // default dtor generated
    unsigned        assign_gen : 1; // default assign generated

    unsigned        ctor_user_code : 1;   // ctor has user code
    unsigned        copy_user_code : 1;   // copy has user code
    unsigned        dtor_user_code : 1;   // dtor has user code
    unsigned        assign_user_code : 1; // assign has user code
    unsigned        ctor_user_code_checked : 1; // ctor_user_code was checked
    unsigned        copy_user_code_checked : 1; // copy_user_code was checked
    unsigned        dtor_user_code_checked : 1; // dtor_user_code was checked
    unsigned        assign_user_code_checked: 1;// assign_user_code was checked

    unsigned        needs_ctor : 1; // must be constructed
    unsigned        needs_dtor : 1; // must be destructed
    unsigned        needs_vdtor : 1;// must have a virtual destructor
    unsigned        needs_assign : 1;// must be assigned with op=
    unsigned        const_copy : 1; // copy ctor takes const C &
    unsigned        const_assign :1;// assignment takes const C &
    unsigned        const_ref : 1;  // contains a const or reference member
    unsigned        zero_array : 1; // contains a zero sized array as last member

    unsigned        free : 1;       // used for precompiled headers
    unsigned        lattice : 1;    // more than one direct ref to a vbase
    unsigned        passed_ref : 1; // class value is passed as a reference
    unsigned        has_def_ctor :1;// has an explicit default constructor
    unsigned        vftable_done :1;// vftable has been collected already
    unsigned        vbtable_done :1;// vftable has been collected already
    unsigned        has_udc : 1;    // has a user-defined conversion declared
    unsigned        common : 1;     // used when searching for common bases

    unsigned        has_comp_info:1;// has compiler generated info inside
    unsigned        has_mutable : 1;// has a mutable data member
    unsigned        empty : 1;      // class has zero size
    unsigned        has_fn : 1;     // has any member function

    int : 0;
} CLASSINFO;

// kludge alert:
//      ScopeRttiLeaps uses 'of' in a TYP_CLASS as a cache temporarily

PCH_struct type {
    TYPE                next;           // used for keeping track of dup. types
    TYPE                of;
    union {
        struct {                        // TYP_ERROR
            unsigned    fmt;
        } e;
        struct {                        // TYP_NULL
            void        *init1;
            void        *init2;
        } i;
        struct {                        // TYP_TYPEDEF, TYP_ENUM
            SYMBOL      sym;
            SCOPE       scope;
        } t;
        struct {                        // TYP_CLASS
            SCOPE       scope;
            CLASSINFO   *info;
        } c;
        struct {                        // TYP_BITFIELD
            unsigned    field_start;
            unsigned    field_width;
        } b;
        struct {                        // TYP_ARRAY
            CGREFNO     refno;
            target_size_t array_size;
        } a;
        struct {                        // TYP_MODIFIER
            void        *base;
            void        *pragma;
        } m;
        struct {                        // TYP_FUNCTION
            arg_list    *args;
            void        *pragma;
        } f;
        struct {                        // TYP_MEMBER_POINTER
            TYPE        host;           // may not be TYP_CLASS! (can be NULL)
        } mp;
        struct {                        // TYP_GENERIC
            unsigned    index;          // keeps template args distinct
        } g;
    } u;
    dbg_info            dbg;            // FOR D2 AND DWARF
    type_flag           flag;
    type_id             id;
    type_dbgflag        dbgflag;
};

#define SCOPE_DEFS                                                                    \
 SCOPE_DEF(SCOPE_FILE,          MIN_HASHTAB_SIZE+2 ) /* file scope                  */\
,SCOPE_DEF(SCOPE_CLASS,         MIN_HASHTAB_SIZE+2 ) /* class scope                 */\
,SCOPE_DEF(SCOPE_FUNCTION,      MIN_HASHTAB_SIZE )   /* function arguments          */\
,SCOPE_DEF(SCOPE_BLOCK,         MIN_HASHTAB_SIZE )   /* {} block scopes             */\
,SCOPE_DEF(SCOPE_TEMPLATE_DECL, MIN_HASHTAB_SIZE )   /* template declaration scope  */\
,SCOPE_DEF(SCOPE_TEMPLATE_INST, MIN_HASHTAB_SIZE )   /* template instantiation scope*/\
,SCOPE_DEF(SCOPE_TEMPLATE_PARM, MIN_HASHTAB_SIZE )   /* template parameters scope   */\
,SCOPE_DEF(SCOPE_FREE,          MIN_HASHTAB_SIZE )   /* unused scope (pcheader)     */

typedef enum {
    #define SCOPE_DEF(a,b) a
    SCOPE_DEFS
    #undef SCOPE_DEF
,   SCOPE_MAX
} scope_type_t;

#define SC_DEFS                                                           \
 SC_DEF(SC_NULL                )/* not defined                          */\
,SC_DEF(SC_EXTERN              )/* external reference                   */\
,SC_DEF(SC_STATIC              )/* static definition                    */\
,SC_DEF(SC_AUTO                )/* automatic storage                    */\
,SC_DEF(SC_REGISTER            )/* register storage                     */\
,SC_DEF(SC_TYPEDEF             )/* typedef symbol                       */\
,SC_DEF(SC_ACCESS              )/* member access symbol (11.3)          */\
,SC_DEF(SC_DEFAULT             )/* represents a default argument value  */\
,SC_DEF(SC_ENUM                )/* enumerated constant                  */\
,SC_DEF(SC_MEMBER              )/* class member                         */\
,SC_DEF(SC_PUBLIC              )/* defined and symbol exported          */\
,SC_DEF(SC_NAMESPACE           )/* symbol is a namespace id             */\
,SC_DEF(SC_CLASS_TEMPLATE      )/* symbol is a class template           */\
,SC_DEF(SC_FUNCTION_TEMPLATE   )/* symbol is a function template        */\
                                /* **** used only in Code Generation:   */\
,SC_DEF(SC_VIRTUAL_FUNCTION    )/* indirect symbol for a virt. fn call  */\
                                /* **** only in template instantiation  */\
,SC_DEF(SC_ADDRESS_ALIAS       )/* symbol use in template instantiations*/\
                                /* **** used only when writing pchdrs   */\
,SC_DEF(SC_FREE                )/* used for precompiled headers         */\
,SC_DEF(SC_MAX                 )


typedef enum {
    #define SC_DEF(a) a
    SC_DEFS
    #undef SC_DEF
} symbol_class;

typedef enum                            // flags for symbol.flag
                                        // general flags
{   SF_REFERENCED       = 0x00000001    // - symbol has been referenced
,   SF_INITIALIZED      = 0x00000002    // - storage for symbol is initialized
,   SF_CONSTANT_INT     = 0x00000004    // - constant value in symbol
,   SF_ADDR_TAKEN       = 0x00000008    // - address of symbol has been taken
,   SF_NO_REF_WARN      = 0x00000010    // - don't warn if unreferenced
,   SF_FAR16_CALLER     = 0x00000020    // - function called a far16 function
,   SF_ADDR_THUNK       = 0x00000040    // - addressability thunk (u.thunk_calls is set)
,   SF_DBG_ADDR_TAKEN   = 0x00000080    // - addr of sym taken in debug info

                                        // if neither set, it is undecided
                                        //  what function does
,   SF_LONGJUMP         = 0x00000100    // - function: has longjump, throw
,   SF_NO_LONGJUMP      = 0x00000200    // - function: no longjump, throw

,   SF_ERROR            = 0x00000400    // - symbol is erroneous
,   SF_CONST_NO_CODE    = 0x00000800    // - symbol is const & doesn't need code
                                        //   to initialize it (const double d=1;)
,   SF_TEMPLATE_FN      = 0x00001000    // - generated from a fn template
                                        //   - u.alias is fn template symbol
                                        //     (if it is known)
,   SF_CONSTANT_INT64   = 0x00002000    // - constant int-64 value in symbol
,   SF_ALIAS            = 0x00004000    // - sym is a true alias of another sym
,   SF_CG_ADDR_TAKEN    = 0x00008000    // - address is known outside generated code
,   SF_ENUM_UINT        = 0x00010000    // - constant unsigned value in symbol

                                        // specific flags:

                                        // SCOPE_CLASS
,   SF_PROTECTED        = 0x80000000    // - symbol is protected
,   SF_PRIVATE          = 0x40000000    // - symbol is private
,   SF_IN_VFT           = 0x20000000    // - function in gen'ed VFT

                                        // SCOPE_FILE
,   SF_PLUSPLUS         = 0x80000000    // - symbol is extern "C++"
,   SF_IS_THROW         = 0x40000000    // - R/T call is a C++ throw

                                        // - variable
,   SF_ANONYMOUS        = 0x08000000    // - - member of an anonymous union
,   SF_RETN_OPT         = 0x04000000    // - - optimized away by return opt.
,   SF_CATCH_ALIAS      = 0x02000000    // - - catch variable alias

                                        // - function
,   SF_DONT_INLINE      = 0x08000000    // - - don't inline this function!
,   SF_MUST_GEN         = 0x04000000    // - - function must be generated

                                        // - function, during back-end
,   SF_CG_INLINEABLE    = 0x02000000    // - - function could be inlined

    /* multi-bit constants */
,   SF_ACCESS           = ( SF_PRIVATE  // - - access for member
                          | SF_PROTECTED )
,   SF_FN_TEMPLATE_COPY = ( SF_PLUSPLUS // - - flags that need to be copied
                          )             //     when a function is instantiated
,   SF_FN_LONGJUMP      = ( SF_LONGJUMP // - - decidable if function can
                          | SF_NO_LONGJUMP ) //throw, longjump, etc

,   SF_NULL             = 0x00000000
} symbol_flag;

typedef enum                            // flags for symbol.flags2
                                        // hdl control flags
{   SF2_TOKEN_LOCN       = 0x01         // - has token location
,   SF2_CG_HANDLE        = 0x02         // - has code-gen handle
,   SF2_DW_HANDLE_FWD    = 0x04         // - has forward dwarf handle
,   SF2_DW_HANDLE_DEF    = 0x08         // - has defined dwarf handle
,   SF2_SYMDBG           = 0x10         // - has defined symdbg handle

    /* multi-bit constants */
,   SF2_DW_HANDLE        = ( SF2_DW_HANDLE_FWD
                           | SF2_DW_HANDLE_DEF )
,   SF2_HDL_MASK         = ( SF2_DW_HANDLE  // - - 'handles' union value
                           | SF2_CG_HANDLE )

,   SF2_NULL             = 0x00
} symbol_flag2;

PCH_struct symbol {                     // SYMBOL in symbol table
    SYMBOL              next;           // - next in ring
    SYMBOL              thread;         // - SCOPE ordering OR defarg base chain
    TYPE                sym_type;       // - TYPE for symbol
    SYMBOL_NAME         name;           // - SYMBOL_NAME for symbol
//  union {
//      DW_TOKEN_LOCN   *location;      // - location during parsing
//      void            *cg_handle;     // - handle during code-generation
//  } hdl;
    SYM_TOKEN_LOCN      *locn;          // - location, (dwarf or cg) handle
    union {
        target_ulong    uval;           // - SC_ENUM -- unsigned value
        target_long     sval;           // - SC_ENUM -- signed value
        POOL_CON*       pval;           // - SC_ENUM, const int: - pool value
        target_offset_t offset;         // - SC_MEMBER -- data offset
        TEMPLATE_INFO   *tinfo;         // - SC_CLASS_TEMPLATE -- info for it
        FN_TEMPLATE_DEFN *defn;         // - SC_FUNCTION_TEMPLATE -- defn for it
        PTREE           defarg_info;    // - SC_DEFAULT -- defarg info
                                        //   use op=PT_TYPE,
                                        //     next is defarg expr
                                        //     scope for temporaries (or NULL)
        TYPE            udc_type;       // - SC_ACCESS -- extra info for UDCs
        SYMBOL          alias;          // - SF_ALIAS -- aliased symbol
        SYMBOL          virt_fun;       // - SC_VIRTUAL_FUNCTION -- virt. func.
        unsigned        scalar_order;   // - SC_FUNCTION -- used in dummy scalars
        SYMBOL          thunk_calls;    // - SC_FUNCTION -- when thunk, orig. function
        NAME_SPACE      *ns;            // - SC_NAMESPACE -- info for it
        SYMBOL          sym;
        TYPE            type;
        int             sym_offset;     // - SC_AUTO,SC_REGISTER -- fast cgen
    } u;
    symbol_flag         flag;           // - flags
    uint_8              flag2;          // - flags2
    symbol_class        id;             // - storage class
    fe_seg_id           segid;          // - segment id
};

// adding a symbol to the SYMBOL_NAME has to be consistently
// a push or append for namespace multi-sym returns
#define _AddSymToRing( h, r )           RingAppend( (h), (r) );

PCH_struct symbol_name {
    SYMBOL_NAME         next;
    SYMBOL              name_type;
    SYMBOL              name_syms;
    char                *name;
    SCOPE               containing;
};

// region includes 'first' up to and including 'last' (both fields are non-NULL)
PCH_struct sym_region {                 // list of symbols from same SYMBOL_NAME
    SYM_REGION          *next;
    SYMBOL              from;           // - first symbol
    SYMBOL              to;             // - last symbol
};

PCH_struct name_space {
    SYMBOL              sym;            // - sym of namespace
    SCOPE               scope;          // - scope of namespace
    SYMBOL              last_sym;       // - scope's last sym (see TEMPLATE.C)
    NAME_SPACE          *all;           // - link together all namespaces
    union {
        unsigned flags;
        struct {
          unsigned      global_fs : 1;  // - global filescope
          unsigned      free : 1;       // - used for PCH
          unsigned      unnamed : 1;    // - unnamed namespace
        };
    };
};

PCH_struct using_ns {
    USING_NS            *next;
    SCOPE               trigger;        // - if we find nothing in this scope,
                                        //   search in the alternate scope
    SCOPE               using_scope;    // - alternate namespace scope to search
};

/*
    - we shouldn't need 'using_list' to be in 'SCOPE' since the ISO C++ WP
      is arranged so that 'using_list''s are only part of namespaces
      (common scope will always be a namespace).  We have 'using_list'
      in scope in case we have to support the "bug" in MS/MetaWare where
      using takes effect in the current scope (which may be a block scope).
      If both MetaWare and MS (and Borland) eventually get it right, we
      can move 'using_list' into NAME_SPACE.

    - struct scope can be reduced in size if 'id' moves into the flags
        as "unsigned id : 8;" (this removes 1 DWORD from the size
        with no loss of alignment or efficiency; remember to fix
        PCH read/write code in SCOPE.C to not explicitly save/restore
        'id' since it will be covered by 'flags')

        note: for debugging the current layout is best (id is an enum)
                so the above change should only be done for the production
                (not development) version
*/

// all fields should be treated as read-only and private to SCOPE.C
PCH_struct scope {
    SCOPE               enclosing;      // - lexically enclosing scope
    HASHTAB             names;          // - names in this scope
    SYMBOL              ordered;        // - list of variables in order of decl
    USING_NS            *using_list;    // - list of "using namespace X"'s
    union {                             // - owner of scope
        NAME_SPACE      *ns;            // -- name space for SCOPE_FILE
        SYMBOL          sym;            // -- function owning SCOPE_FUNCTION
        TYPE            type;           // -- class owning SCOPE_CLASS
        unsigned        index;          // -- index for SCOPE_BLOCK
        TEMPLATE_INFO   *tinfo;         // -- SCOPE_TEMPLATE_PARM (classes)
        FN_TEMPLATE_DEFN *defn;         // -- SCOPE_TEMPLATE_PARM (functions)
    } owner;
    union {
        unsigned        flags;
        struct {
          unsigned      keep : 1;       // - indicates scope contains info
          unsigned      dtor_reqd : 1;  // - SCOPE_BLK -- need to dtor
          unsigned      dtor_naked : 1; // - SCOPE_BLK -- has naked dtor syms
          unsigned      try_catch : 1;  // - SCOPE_BLK -- try/catch block
          unsigned      arg_check : 1;  // - check decls against arg scope
          unsigned      cg_stab   : 1;  // - generate for scope
          unsigned      in_unnamed : 1; // - enclosed in an unnamed namespace
          unsigned      colour : 1;     // - using in common enclosing algorithm
          unsigned      fn_template : 1;// - SCOPE_TEMPLATE_PARM -- function
          unsigned      dirty : 1;      // - a symbol has been added
        };
    };
    scope_type_t        id;             // - type of scope
};

/*
  Accessing a member or virtual member function involves two phases

  (1) accessing the final class
        if( exact type of class is known )then
            class_addr = addr + 'exact_delta'
        else
            if( 'non_virtual' is FALSE )then
                vbptr_addr = addr + 'vb_offset'
                vb_delta = (*vb_ptr_addr)['vb_index']
                addr = vbptr_addr + vb_delta
            endif
            class_addr = addr + 'delta'
        endif
  (2) accessing the member within the final class
        - accessing a data member
          member_addr = class_addr + 'offset'
        - calling a member function
          "this" = class_addr
          if( virtual call is required )then
              vfptr_addr = class_addr + 'vf_offset'
              member_addr = (*vf_ptr_addr)['sym->u.offset']
          else
              member_addr = direct call address
          endif

    general notes:

    - if( 'regions' != NULL )then
        ignore 'sym_name', it is included in the sym region list
        which gives the full list of symbols to consider
      endif
*/
struct search_result {                  // * means private to SCOPE.C
    SCOPE               scope;          // - scope containing sym_name/sym
    SYMBOL_NAME         sym_name;       // - SYMBOL_NAME found
    SYMBOL              sym;            // - set if user-defd conversion found
    SCOPE               start;          // * scope that initiated the search
    SCOPE               access_decl;    // - first access-decl encountered
    SYM_REGION          *region;        // - list of symbols (if req'd)
    MSG_NUM             error_msg;      // * error message
    MSG_NUM             info_msg;       // * info message
    SYMBOL              info1;          // * parm for info message #1
    SYMBOL              info2;          // * parm for info message #2
    target_offset_t     vb_offset;      // - offset of vftable pointer
    target_offset_t     vb_index;       // - index of virtual base
    target_offset_t     delta;          // - last base class offset
    target_offset_t     exact_delta;    // - last base class direct offset
    target_offset_t     offset;         // - member offset
    target_offset_t     vf_offset;      // - offset of vftable pointer
    TOKEN_LOCN          errlocn;        // * location for errors
    inherit_flag        perm;           // * access permission
    unsigned            simple : 1;     // - name is in a FILE or BLOCK scope
    unsigned            non_virtual : 1;// - use delta offset to find scope
                                        // - only valid for lexical lookup
    unsigned            use_this : 1;   // - may use "this" to access
    unsigned            no_this : 1;    // - cannot use "this" to access
    unsigned            ambiguous : 1;  // * name was ambiguously found
    unsigned            mixed_static :1;// * ovload; if non-static, it's ambig!
    unsigned            cant_be_auto :1;// * nested function refs an auto
    unsigned            protected_OK :1;// * protected SYMBOL access is OK
    unsigned            ignore_access:1;// * don't report access errors
    unsigned            lookup_error :1;// * general lookup error detected
    int                 : 0;
};

typedef enum {
    DERIVED_NO                  = 0,    // class not derived from base
    DERIVED_YES                 = 1,    // class derived from base
    DERIVED_YES_BUT_VIRTUAL     = 2,    // class derived through virtual base
    DERIVED_YES_BUT_AMBIGUOUS   = 3,    // class ambiguously derived from base
    DERIVED_YES_BUT_PRIVATE     = 4,    // class derived through private base
    DERIVED_YES_BUT_PROTECTED   = 5,    // class derived through protected base
} derived_status;

struct class_table {
    CLASS_TABLE         *next;          /* must be RingFreed after use */
    target_offset_t     vb_offset;      /* offset of vbptr */
    target_offset_t     vb_index;       /* index into vbtable */
    target_offset_t     delta;          /* delta table ptr goes in */
    target_offset_t     exact_delta;    /* exact delta table ptr goes in */
    unsigned            count;          /* number of things def'd */
    unsigned            ctor_disp : 1;  /* apply ctor-disp adjustment */
};

struct class_vbtable {
    CLASS_TABLE         h;              /* header */
    TYPE                data[1];        /* NULL terminated */
};

/* sequence:
        (1) this -= delta
        (2) if ctor_disp then
                this -= *(this - sizeof(unsigned))
        (3)     if input_virtual then
                    this += vb_offset
                    this += (*this)[vb_index]
                endif
                this += delta
            endif
        (4) call override
        (5) if output_virtual then
                this += vb_offset
                this += (*this)[vb_index]
            endif
            this += delta
*/
struct thunk_cast {
    target_offset_t     vb_offset;
    target_offset_t     vb_index;
    target_offset_t     delta;
};

struct thunk_action {
    SYMBOL              sym;
    SYMBOL              thunk;                  // symbol for thunk function
    target_offset_t     delta;                  // step 1
    THUNK_CAST          in;                     // step 3
    SYMBOL              override;               // step 4
    THUNK_CAST          out;                    // step 5
    unsigned            ctor_disp : 1;          // control for step 2
    unsigned            input_virtual : 1;      // control for step 3
    unsigned            output_virtual : 1;     // control for step 5
    unsigned            non_empty : 1;          // thunk is necessary
    unsigned            last_entry : 1;         // last entry in vftable
    unsigned            possible_ambiguity : 1; // vftable entry may be ambiguous
    int                 : 0;
};

struct class_vftable {
    CLASS_TABLE         h;              /* header */
    unsigned            amt_left;       /* # of vfns left to go */
    unsigned            ambiguities : 1;/* has potentially ambiguous entries */
    unsigned            corrupted : 1;  /* definitely has bad entries */
    int                 : 0;
    THUNK_ACTION        data[1];        /* terminated if last_entry is TRUE */
};

/*
    - ANALYSE code must determine that 'derived' is derived from 'base' in an
      accessible manner (i.e., there should be no ambiguity/permission errors)
    - set fields labelled 'I' and call ScopeMemberPtrCastAction
    - steps for a safe (B -> D) member pointer cast are:

        GENERATE:
            new_delta = mp.delta;
            new_index = mp.index;
        if( 'delta_reqd' )then          -- delta adjustment
            if( 'test_reqd' )then
                GENERATE:
                    if( new_index == 0 ) {
                        new_delta += 'delta'
                    }
            else
                GENERATE:
                    new_delta += 'delta'
            endif
        endif
        if( 'mapping_reqd' )then                -- index mapping
            if( 'mapping' == NULL )then
                if( 'single_mapping' ) then
                    if( 'vb_index' == 0 )then
                        GENERATE:
                            if( new_index > 'single_test' ) {
                                new_index = 'vb_index'
                            }
                    else
                        GENERATE:
                            if( new_index == 'single_test' ) {
                                new_index = 'vb_index'
                            }
                    endif
                else
                    GENERATE:
                        new_index = 'vb_index'
                endif
            else
                GENERATE:
                    new_index = 'mapping'[ new_index ]
            endif
        endif

    - steps for an unsafe (D->B) member pointer cast are the same except that
      the "index mapping" executes before the "delta adjustment" (which uses
      a "-=" instead of a "+=")

    - future: we could generate exceptions when we know the conversion will
      never work properly
*/
struct member_ptr_cast {                /* I - input, O - output, * - private */
    SCOPE               base;           /* I: base class cope */
    SCOPE               derived;        /* I: derived from 'base' */
    target_offset_t     delta;          /* O: amount to adjust delta by */
    target_offset_t     single_test;    /* O: single idx val that needs mapping */
    target_offset_t     vb_index;       /* O: new value for 'index' */
    SYMBOL              mapping;        /* O: unsigned array to map indices */
    unsigned            safe : 1;       /* I: casting from 'base' to 'derived' */
    unsigned            init_conv : 1;  /* I: convert from found base to final base */
    unsigned            delta_reqd : 1; /* O: TRUE if delta adjustment is req'd */
    unsigned            mapping_reqd : 1;/*O: TRUE is index mapping req'd */
    unsigned            test_reqd : 1;  /* O: TRUE if index == 0 test is req'd */
    unsigned            single_mapping:1;/*O: only one index value needs mapping */
};

/*
    RTTI base class access leaps:

        if( control & RL_VIRTUAL ) {
            this += RTTI.vbptr;
            this += (*this)[ vb_index ];
        }
        this += offset;
*/
struct gen_leap {
    GEN_LEAP            *next;
    TYPE                type;           /* base class type */
    target_offset_t     vb_index;       /* index into virtual base table */
    target_offset_t     offset;         /* offset to add */
    unsigned            control;        /* RL_* control mask */
};

typedef enum {
    BGT_EXACT           = 0,            // exact match
    BGT_TRIVIAL         = 1,            // trivial conversion req'd
    BGT_DERIVED         = 2,            // derived class conversion req'd
    BGT_MAX
} bgt_control;

// defined in LINKAGE.C

extern void LinkageReset( void );
extern LINKAGE LinkageAdd( char *, void * );
extern void LinkagePush( char * );
extern void LinkagePushC( void );
extern void LinkagePushCpp( void );
extern void LinkageBlock( void );
extern void LinkagePop( void );
extern LINKAGE LinkageMergePop( void );
extern int  LinkageDeclaration( void );
extern void LinkageSet( SYMBOL, char * );
extern boolean LinkageIsCpp( SYMBOL );
extern boolean LinkageIsC( SYMBOL );
extern boolean LinkageIsCurr( SYMBOL );
extern boolean LinkageSpecified( void );

// defined in SCOPE.C

extern SCOPE CurrScope;
extern SCOPE FileScope;
extern SCOPE InternalScope;
extern SYMBOL ChipBugSym;
extern SYMBOL PCHDebugSym;
extern SYMBOL DFAbbrevSym;

extern boolean ScopeAccessType( scope_type_t );
extern boolean ScopeType( SCOPE, scope_type_t );
extern boolean ScopeEquivalent( SCOPE, scope_type_t );
#define ScopeId( s )    ((s)->id)
#define ScopeEnclosedInUnnamedNameSpace( s )    ((s)->in_unnamed)

typedef enum {
    FVS_VIRTUAL_ABOVE   = 0x01, // sym[0] will be set
    FVS_USE_INDEX       = 0x02, // sym[0] will be set
    FVS_RETURN_THUNK    = 0x04, // sym[0] will be set
    FVS_NAME_SAME_TABLE = 0x08, // sym[1] will be set
    FVS_NULL            = 0x00
} find_virtual_status;
extern find_virtual_status ScopeFindVirtual( SCOPE, SYMBOL [2], char * );

extern void ScopeAddUsing( SCOPE, SCOPE );
extern SCOPE ScopeIsGlobalNameSpace( SCOPE );
extern SCOPE ScopeIsUnnamedNameSpace( SCOPE );
extern void ScopeBeginFunction( SYMBOL );
extern void ScopeBeginBlock( unsigned );
extern SCOPE ScopeBegin( scope_type_t );
extern SCOPE ScopeEnd( scope_type_t );
extern void ScopeEndFileScope( void );
extern SCOPE ScopeCreate( scope_type_t );
extern void ScopeOpen( SCOPE );
extern void ScopeRestoreUsing( SCOPE );
extern void ScopeEstablish( SCOPE );
extern SCOPE ScopeOpenNameSpace( char *, SYMBOL );
extern SCOPE ScopeSetEnclosing( SCOPE, SCOPE );
extern SCOPE ScopeEstablishEnclosing( SCOPE, SCOPE );
extern void ScopeSetClassOwner( SCOPE, TYPE );
extern SCOPE ScopeSetContaining( SYMBOL_NAME, SCOPE );
extern SCOPE ScopeClose( void );
extern void ScopeCreatePCHDebugSym( void );
extern target_offset_t ScopeVBPtrOffset( SCOPE );
extern boolean ScopeHasVirtualBases( SCOPE );
extern boolean ScopeHasVirtualFunctions( SCOPE );
extern void ScopeJumpForward( SCOPE );
extern void ScopeJumpBackward( SCOPE );
extern BASE_CLASS *ScopeFindVFPtrInVBase( TYPE );
extern TYPE ScopeFindBoundBase( TYPE, TYPE );
extern boolean ScopeHasPureFunctions( SCOPE );
extern void ScopeNotePureFunctions( TYPE );
extern SYMBOL ScopePureVirtualThunk( THUNK_ACTION * );
extern SYMBOL ScopeAlreadyExists( SCOPE, char * );
extern SYMBOL_NAME ScopeYYLexical( SCOPE, char * );
extern SYMBOL_NAME ScopeYYMember( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindSymbol( SYMBOL );
extern SEARCH_RESULT *ScopeFindLexicalNameSpace( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindLexicalColonColon( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindMemberColonColon( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindBaseMember( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindLexicalClassType( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindLexicalEnumType( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindNakedFriend( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindNaked( SCOPE, char * );
extern SEARCH_RESULT *ScopeContainsNaked( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindScopedNaked( SCOPE, SCOPE, char * );
extern SEARCH_RESULT *ScopeFindMember( SCOPE, char * );
extern SEARCH_RESULT *ScopeFindScopedMember( SCOPE, SCOPE, char * );
extern SEARCH_RESULT *ScopeFindScopedMemberConversion( SCOPE, SCOPE, TYPE, type_flag );
extern SEARCH_RESULT *ScopeFindNakedConversion( SCOPE, TYPE, type_flag );
extern SEARCH_RESULT *ScopeFindScopedNakedConversion( SCOPE, SCOPE, TYPE, type_flag );
extern FNOV_LIST *ScopeConversionList( SCOPE, type_flag, TYPE );
extern SEARCH_RESULT *ScopeContainsMember( SCOPE, char * );
extern boolean ScopeImmediateCheck( SEARCH_RESULT * );
extern boolean ScopeAmbiguousSymbol( SEARCH_RESULT *, SYMBOL );
extern boolean ScopeCheckSymbol( SEARCH_RESULT *, SYMBOL );
extern void ScopeDontCheckSymbol( SEARCH_RESULT * );
extern void ScopeFreeResult( SEARCH_RESULT * );
extern SEARCH_RESULT *ScopeDupResult( SEARCH_RESULT * );
extern boolean ScopeEnclosed( SCOPE, SCOPE );
extern void ScopeKeep( SCOPE );
extern void ScopeArgumentCheck( SCOPE );
extern void ScopeQualifyPush( SCOPE, SCOPE );
extern SCOPE ScopeQualifyPop( void );
extern SYMBOL ScopeOrderedStart( SCOPE );
extern SYMBOL ScopeOrderedNext( SYMBOL, SYMBOL );
extern SYMBOL ScopeOrderedFirst( SCOPE );
extern SYMBOL ScopeOrderedLast( SCOPE );
extern SCOPE ScopeEnclosingId( SCOPE, scope_type_t );
extern SCOPE ScopeNearestNonClass( SCOPE );
extern SCOPE ScopeNearestFile( SCOPE );
extern SCOPE ScopeFunctionScopeInProgress();
extern SYMBOL ScopeFunctionScope( SCOPE );
extern void ScopeMemberModuleFunction( SCOPE, SCOPE );
extern void ScopeRestoreModuleFunction( SCOPE );
extern boolean ScopeLocalClass( SCOPE );
extern SCOPE ScopeHostClass( SCOPE );
extern BASE_CLASS *ScopeFindVBase( SCOPE, TYPE );
extern void ScopeMemberPtrCastAction( MEMBER_PTR_CAST * );
extern void ScopeEmitIndexMappings( void );
extern void ScopeClear( SCOPE );
extern boolean ScopeDebugable( SCOPE );
extern void ScopeSetParmClass( SCOPE, TEMPLATE_INFO * );
extern void ScopeSetParmFn( SCOPE, FN_TEMPLATE_DEFN * );
extern void ScopeSetParmCopy( SCOPE, SCOPE );

typedef enum {
    SCV_CTOR        = 0x01,     // collect will be used for ctor code
    SCV_NO_DIAGNOSE = 0x02,     // diagnostic msgs are not allowed
    SCV_NULL        = 0x00
} scv_control;

extern CLASS_VBTABLE *ScopeCollectVBTable( SCOPE, scv_control );
extern CLASS_VFTABLE *ScopeCollectVFTable( SCOPE, scv_control );

/* front-end #pragma support */
extern SYMBOL ScopeASMUseSymbol( char *, boolean * );
extern void ScopeASMUsesAuto( void );
extern SYMBOL ScopeASMLookup( char * );
extern SYMBOL ScopeIntrinsic( boolean );
extern void ScopeAuxName( char *, void * );

extern SYMBOL ScopeInsert( SCOPE, SYMBOL, char * );
extern boolean ScopeCarefulInsert( SCOPE, SYMBOL *, char * );
extern SYMBOL ScopePromoteSymbol( SCOPE, SYMBOL, char * );
extern void ScopeInsertErrorSym( SCOPE, PTREE );
extern void ScopeRawAddFriend( CLASSINFO *, SYMBOL );
extern void ScopeAddFriend( SCOPE, SYMBOL );
extern SYMBOL AllocSymbol( void );
extern SYMBOL AllocTypedSymbol( TYPE );
extern SYMBOL_NAME AllocSymbolName( char *, SCOPE );
extern void FreeSymbol( SYMBOL );
extern void FreeSymbolName( SYMBOL_NAME );
extern void ScopeBurn( SCOPE );
extern void ScopeMarkVisibleAutosInMem( void );
extern void ScopeWalkNames( SCOPE, void (*)( SYMBOL_NAME ) );
extern void ScopeWalkSymbols( SCOPE, void (*)( SYMBOL ) );
extern void ScopeWalkOrderedSymbols( SCOPE, void (*)( SYMBOL ) );
extern void ScopeWalkDataMembers( SCOPE, void (*)(SYMBOL,void*), void* );
extern void ScopeWalkAllNameSpaces( void (*)(NAME_SPACE*,void*), void* );
extern unsigned ScopeRttiClasses( TYPE );
extern unsigned ScopeRttiLeaps( TYPE, GEN_LEAP ** );
extern void ScopeRttiFreeLeaps( GEN_LEAP * );
extern derived_status ScopeDerivedCount( SCOPE, SCOPE, unsigned * );
extern derived_status ScopeDerived( SCOPE, SCOPE );
extern SCOPE ScopeCommonBase( SCOPE, SCOPE );
extern target_offset_t ScopeBaseDelta( SCOPE , SCOPE );
extern SEARCH_RESULT *ScopeBaseResult( SCOPE , SCOPE );
extern SEARCH_RESULT *ScopeResultFromBase( TYPE, BASE_CLASS * );
extern BASE_CLASS *ScopeInherits( SCOPE );
extern FRIEND *ScopeFriends( SCOPE );
extern boolean ScopeDirectBase( SCOPE, TYPE );
extern boolean ScopeIndirectVBase( SCOPE, TYPE );
extern char *ScopeUnnamedNamespaceName( TOKEN_LOCN * );
extern char *ScopeNameSpaceName( SCOPE );
extern char *ScopeNameSpaceFormatName( SCOPE );
extern TYPE ScopeClass( SCOPE );
extern SYMBOL ScopeFunction( SCOPE );
extern unsigned ScopeIndex( SCOPE );
extern boolean VariableName( SYMBOL_NAME );
extern SYMBOL ScopeFunctionInProgress( void );
extern SCOPE ScopeForTemps( void );
extern SYMBOL ScopeFuncParm( unsigned );
extern void ScopeResultErrLocn( SEARCH_RESULT*, TOKEN_LOCN* );
extern boolean ScopeSameVFuns( SYMBOL, SYMBOL );

SYMBOL ScopeFindExactVfun(      // FIND EXACT VIRTUAL FUNCTION IN DERIVED CLASS
    SYMBOL vfun,                // - virtual fun in a base class
    SCOPE scope,                // - scope for derived class
    target_offset_t* a_adj_this,// - adjustment for this
    target_offset_t* a_adj_retn)// - adjustment for return
;

void ScopeWalkAncestry(         // VISIT ONCE ALL CLASSES IN ANCESTRY
    SCOPE scope,                // - class scope
    void (*rtn)(                // - walker routine
        SCOPE,                  // -- base class scope
        void * ),               // -- user data
    void *data )                // - user supplied data
;

void ScopeWalkDirectBases(          // WALK DIRECT BASES
    SCOPE scope,                    // - scope
    void (*rtn)(BASE_CLASS*,void*), // - routine
    void *data )                   // - data to be passed
;

void ScopeWalkVirtualBases(         // WALK VIRTUAL BASES
    SCOPE scope,                    // - scope
    void (*rtn)(BASE_CLASS*,void*), // - routine
    void *data )                    // - data to be passed
;

// defined in TYPE.C

extern TYPE TypeError;

extern type_flag BaseTypeClassFlags( TYPE );
extern TYPE TypedefRemove( TYPE );
extern TYPE TypedefModifierRemove( TYPE );
TYPE TypedefModifierRemoveOnly( TYPE );
extern arg_list *TypeArgList( TYPE type );
extern boolean TypeCompareExclude( TYPE, TYPE, type_exclude );
extern boolean TypesSameExclude( TYPE, TYPE, type_exclude );
extern boolean TypesIdentical( TYPE, TYPE );
// extern boolean TypesSame( TYPE, TYPE );
extern SYMBOL MakeTypeidSym( TYPE );
extern SYMBOL MakeVATableSym( SCOPE );
extern SYMBOL MakeVBTableSym( SCOPE, unsigned, target_offset_t );
extern SYMBOL MakeVFTableSym( SCOPE, unsigned, target_offset_t );
extern SYMBOL MakeVMTableSym( SCOPE, SCOPE, boolean * );

extern target_offset_t PackAlignment( target_offset_t, target_size_t );

extern boolean TypePartiallyDefined( TYPE );
extern boolean TypeDefined( TYPE );
extern boolean TypeAbstract( TYPE );
extern TYPE TypeNeedsCtor( TYPE );
extern boolean TypeIsCharString( TYPE );
extern boolean TypeIsClassInstantiation( TYPE );
extern boolean TypeIsConst( TYPE );
extern boolean TypeIsAnonymousEnum( TYPE );
extern boolean TypeHasEllipsisArg( TYPE );
extern boolean TypeHasNumArgs( TYPE, unsigned );
extern boolean TypeHasReverseArgs( TYPE );
extern boolean TypeVAStartWontWork( TYPE, int * );
extern TYPE DefaultIntType( TYPE );
extern TYPE CleanIntType( TYPE );
extern TYPE SegmentShortType( TYPE );
extern TYPE VoidType( TYPE );
extern TYPE ArrayType( TYPE );
extern TYPE ArithType( TYPE );
extern TYPE EnumType( TYPE );
extern TYPE BoolType( TYPE );
extern TYPE GenericType( TYPE );
extern TYPE BasedType( TYPE );
extern TYPE BasedPtrType( TYPE );
extern TYPE IntegralType( TYPE );
extern TYPE Integral64Type( TYPE );
extern TYPE FloatingType( TYPE );
extern TYPE MemberPtrType( TYPE );
extern TYPE MemberPtrClass( TYPE );
extern TYPE PointerType( TYPE );
extern TYPE PointerTypeEquivalent( TYPE );
extern TYPE AlignmentType( TYPE );
extern TYPE ArrayBaseType( TYPE type );
extern TYPE ArrayModifiedBaseType( TYPE type );
extern TYPE FunctionDeclarationType( TYPE );
extern type_flag FunctionThisFlags( SYMBOL );
extern type_flag FunctionThisMemFlags( SYMBOL );
extern TYPE StructType( TYPE );
extern TYPE StructOpened( TYPE );
extern TYPE PolymorphicType( TYPE );
extern TYPE ElaboratableType( TYPE );
extern TYPE AbstractClassType( TYPE );
extern boolean SignedIntType( TYPE );
extern boolean UnsignedIntType( TYPE );
extern TYPE TypeMergeForMember( TYPE, TYPE );
extern TYPE TypePointedAt( TYPE, type_flag* );
extern TYPE TypePointedAtModified( TYPE );
extern TYPE TypePointedAtReplace( TYPE, TYPE );
extern boolean TypeHasVirtualBases( TYPE );
extern boolean TypeCtorDispRequired( TYPE, TYPE );
extern boolean TypeHasSpecialFields( TYPE );
extern boolean TypeParmSize( TYPE, target_size_t * );
extern boolean TypeBasesEqual( type_flag, void *, void * );

extern SCOPE TypeScope( TYPE );
extern char *SimpleTypeName( TYPE );
extern char *AnonymousEnumExtraName( TYPE );

extern void TypedefUsingDecl( DECL_SPEC *, SYMBOL, TOKEN_LOCN * );
extern TYPE MakeType( type_id );
extern TYPE MakeClassType( void );
extern TYPE MakeTypeOf( TYPE, TYPE );
extern TYPE MakeNamespaceType( void );
extern TYPE GetBasicType( type_id );
extern void PTypeSignedChar( void );
extern void PTypeForceLinkagePush( DECL_SPEC * );
extern DECL_SPEC *PTypeLinkage( void );
extern DECL_SPEC *PTypeStgClass( stg_class_t );
extern DECL_SPEC *PTypeMSDeclSpec( DECL_SPEC *, PTREE );
extern DECL_SPEC *PTypeMSDeclSpecModifier( DECL_SPEC *, TYPE );
extern DECL_SPEC *PTypeScalar( scalar_t );
extern DECL_SPEC *PTypeSpecifier( specifier_t );
extern DECL_SPEC *PTypeActualType( TYPE );
extern DECL_SPEC *PTypeActualTypeName( TYPE, PTREE );
extern DECL_SPEC *PTypeClassInstantiation( TYPE, PTREE );
extern void PTypeClassInstantiationUndo( DECL_SPEC * );
extern DECL_SPEC *PTypeCombine( DECL_SPEC *, DECL_SPEC * );
extern DECL_SPEC *PTypeDone( DECL_SPEC *, boolean );
extern DECL_SPEC *PTypeDefault( void );
extern DECL_SPEC *PTypeTypeid( PTREE );
extern DECL_SPEC *PTypeExpr( PTREE );
extern void PTypeRelease( DECL_SPEC * );
extern void PTypePop( DECL_SPEC * );
extern TYPE PTypeListOfTypes( type_id );
extern DECL_SPEC *CheckNoDeclarator( DECL_SPEC * );
extern DECL_SPEC *CheckTypeSpecifier( DECL_SPEC * );
extern void CheckUniqueType( TYPE );
extern TYPE CheckDupType( TYPE );
extern PTREE ProcessBitfieldId( PTREE );
extern TYPE CheckBitfieldType( DECL_SPEC *, target_long );
extern void PTypeCheckInit( void );
extern TYPE MakeArrayType( unsigned long );
extern TYPE MakeExpandableType( type_id );
extern TYPE MakeModifiedType( TYPE, type_flag );
extern TYPE MakeCommonCodeData( TYPE );
extern TYPE MakeForceInMemory( TYPE );
extern TYPE MakeArrayOf( unsigned long, TYPE );
extern TYPE MakeInternalType( target_size_t );
extern TYPE MakeConstModifier( void );
extern TYPE MakeVolatileModifierModifier( void );
extern TYPE MakeConstTypeOf( TYPE type );
extern TYPE MakeVolatileTypeOf( TYPE type );
extern TYPE MakeCompilerReadWriteData( TYPE );
extern TYPE MakeCompilerConstData( TYPE );
extern TYPE MakeCompilerConstCommonData( TYPE );
extern TYPE MakeFlagModifier( type_flag flag );
extern TYPE MakePointerType( type_flag, specifier_t );
extern TYPE MakeSeg16Pointer( specifier_t );
extern TYPE MakeMemberPointer( PTREE, specifier_t );
extern TYPE MakeMemberPointerTo( TYPE class_type, TYPE base_type );
extern TYPE MakeFarPointerToNear( TYPE );
extern TYPE MakePointerTo( TYPE );
extern TYPE MakeReferenceTo( TYPE );
extern TYPE MakeConstReferenceTo( TYPE );
extern TYPE GetReferenceTo( TYPE );
extern TYPE MakeTypedefOf( TYPE, SCOPE, SYMBOL );
extern TYPE TypeClassModCombine( TYPE, TYPE );
extern TYPE MakeClassModDeclSpec( DECL_SPEC * );
extern boolean IdenticalClassModifiers( TYPE, TYPE );
extern TYPE AbsorbBaseClassModifiers( TYPE, type_flag *, type_flag *, void ** );
extern TYPE ProcessClassModifiers( TYPE, type_flag *, type_flag *, void ** );
extern void SetFnClassMods( TYPE, type_flag, void * );
extern void CheckDeclarationDSpec( DECL_SPEC *, SCOPE );
extern void CheckFunctionDSpec( DECL_SPEC * );
extern DECL_SPEC *CheckArgDSpec( DECL_SPEC * );
extern derived_status TypeDerived( TYPE, TYPE );
extern TYPE TypeCommonBase( TYPE, TYPE );
extern TYPE PointerTypeForReference( TYPE );
extern TYPE PointerTypeForArray( TYPE );
extern TYPE DupArrayTypeForArray( TYPE, target_size_t );
extern TYPE DupArrayTypeForArrayFlag( TYPE, target_size_t, type_flag );
extern TYPE TypeOverrideModifiers( TYPE, TYPE );
extern TYPE MakeNonInlineFunction( TYPE );
extern TYPE MakeInlineFunction( TYPE );
extern TYPE MakeVirtualFunction( TYPE );
extern TYPE MakePureFunction( TYPE );
extern TYPE MakePureVirtualFunction( TYPE );
extern TYPE MakePlusPlusFunction( TYPE );
extern TYPE ChangeFunctionPragma( TYPE, void * );
extern TYPE AddFunctionFlag( TYPE, type_flag );
extern TYPE RemoveFunctionFlag( TYPE, type_flag );
extern TYPE RemoveFunctionPragma( TYPE );
extern TYPE MakeThunkFunction( TYPE );
extern TYPE MakeThunkPragmaFunction( TYPE );
extern TYPE MakeCombinedFunctionType( TYPE, TYPE, type_flag );
extern void CheckDimension( PTREE dim_expr );
extern DECL_INFO *MakeDeclarator( DECL_SPEC *, PTREE );
extern DECL_INFO *MakeAbstractDeclarator( TYPE );
extern DECL_INFO *DeclSpecDeclarator( DECL_SPEC * );
extern DECL_INFO *AddDeclarator( DECL_INFO *, TYPE );
extern DECL_INFO *AddExplicitParms( DECL_INFO *, DECL_INFO * );
extern DECL_INFO *FinishDeclarator( DECL_SPEC *, DECL_INFO * );
extern DECL_INFO *AddMSCVQualifierKludge( type_flag, DECL_INFO * );
extern DECL_INFO *AddArrayDeclarator( DECL_INFO *, PTREE );
extern DECL_INFO *AddArgument( DECL_INFO *, DECL_INFO * );
extern DECL_INFO *AddEllipseArg( DECL_INFO * );
extern void FreeDeclInfo( DECL_INFO * );
extern void FreeArgs( DECL_INFO * );
extern boolean ProcessTemplateArgs( DECL_INFO * );
extern DECL_INFO *InsertDeclInfo( SCOPE, DECL_INFO * );
extern void ProcessDefArgs( DECL_INFO * );
extern SYMBOL InsertSymbol( SCOPE, SYMBOL, char *name );
extern void InsertArgs( DECL_INFO ** );
extern PTREE TypeDeclarator( DECL_INFO * );
extern PTREE MakeConstructorId( DECL_SPEC * );
extern arg_list *InitArgList( arg_list * );
extern arg_list *AllocArgListPerm( int );
extern arg_list *AllocArgListTemp( int );
extern TYPE MakeFnType( DECL_INFO **, specifier_t, PTREE );
extern TYPE MakeSimpleFunction( TYPE, ... );
extern TYPE MakeSimpleFlagFunction( type_flag, TYPE, ... );
extern TYPE MakeModifiableFunction( TYPE, ... );
extern TYPE TypeUserConversion( DECL_SPEC *, DECL_INFO * );
extern PTREE DoDeclSpec( DECL_SPEC * );
extern TYPE MakeBasedModifier( type_flag, boolean, PTREE );
extern TYPE MakeBitfieldType( DECL_SPEC *, TYPE, unsigned, unsigned );
extern PTREE MakeScalarDestructor( DECL_SPEC *, PTREE, DECL_SPEC * );
extern uint_32 TypeHash( TYPE type );
extern boolean ArrowMemberOK( SYMBOL );
extern DECL_INFO *MakeNewDynamicArray( PTREE );
extern DECL_INFO *MakeNewPointer( specifier_t, DECL_INFO *, PTREE );
extern DECL_INFO *MakeNewDeclarator( DECL_SPEC *, DECL_INFO *, DECL_INFO * );
extern DECL_INFO *MakeNewTypeId( PTREE );
extern PTREE MakeNewExpr( PTREE, PTREE, DECL_INFO *, PTREE );
extern void CheckNewModifier( type_flag );
extern TYPE MakeVFTableFieldType( boolean );
extern TYPE MakeVBTableFieldType( boolean );
extern void VerifyPureFunction( DECL_INFO *, PTREE );
extern void VerifyMemberFunction( DECL_SPEC *, DECL_INFO * );
extern void TypedefReset( SYMBOL, TYPE );
extern void *TypeHasPragma( TYPE );
extern TYPE MakePragmaModifier( void * );
extern TYPE AddNonFunctionPragma( TYPE, TYPE );
extern void ForceNoDefaultArgs( DECL_INFO *, int );

extern TYPE MakePragma( char * );
extern TYPE MakeIndexPragma( unsigned );
extern boolean CurrFunctionHasEllipsis( void );
extern void TypeTraverse( type_id, void (*)( TYPE, void *), void * );
extern boolean FunctionUsesAllTypes( SYMBOL, SCOPE, void (*)( SYMBOL ) );
extern void ClearAllGenericBindings( void * );
extern boolean BindFunction( SYMBOL, SYMBOL );
extern type_flag ExplicitModifierFlags( TYPE );

extern TYPE BindGenericTypes( arg_list *, SYMBOL, TOKEN_LOCN *, bgt_control * );

arg_list* ArgListTempAlloc(     // ALLOCATE TEMPORARY ARG LIST
    TEMP_ARG_LIST* default_args,// - default args
    unsigned count )            // - # arguments
;
#if 0
void ArgListTempFree(           // FREE TEMPORARY ARG LIST
    arg_list* used,             // - used list
    unsigned count )            // - # arguments
;
#else
#define ArgListTempFree( used, count )  \
    if( (count) > AUTO_ARG_MAX ) {      \
        ArgListTempFreeMem( used );     \
    }
#endif
void ArgListTempFreeMem(        // FREE TEMPORARY ARG LIST
    arg_list* used )            // - used list
;
SYMBOL CopyCtorFind(            // FIND (OR CREATE) A COPY CTOR FOR A TYPE
    TYPE type,                  // - class type
    TOKEN_LOCN *locn )          // - error location
;
type_flag DefaultMemoryFlag(    // GET DEFAULT MEMORY FLAG FOR A TYPE
    TYPE type )                 // - unmodified type
;
SYMBOL DtorFind(                // FIND DTOR FOR A DIRECT CALL
    TYPE type )                 // - a class type
;
SYMBOL DtorFindLocn(            // FIND DTOR FOR A DIRECT CALL, WITH ERR LOC'N
    TYPE type,                  // - a class type
    TOKEN_LOCN *loc )           // - location for errors
;
SYMBOL RoDtorFind(              // FIND DTOR FOR USE WITH R/O BLOCKS
    SYMBOL sym )                // - symbol to be DTOR'd
;
SYMBOL RoDtorFindType(          // FIND DTOR FOR USE WITH R/O BLOCKS
    TYPE type )                 // - type for DTOR
;
SEARCH_RESULT *DtorFindResult(  // FIND DTOR FOR A POSSIBLY VIRTUAL CALL
    TYPE type )                 // - a class type
;
TYPE MakeBasedModifierOf(       // MAKE BASED MODIFIER FOR A TYPE
    TYPE type,                  // - basic type
    type_flag flags,            // - modifier flags
    void *baser )               // - baser
;
PC_CALLIMPL PcCallImpl(         // OBTAIN PC CALL IMPLEMENTATION
    TYPE ftype )                // - function type
;
#if 0
PTREE* PtListAlloc(             // ALLOCATE PTREE LIST (TEMPORARILY)
    PTREE* default_list,        // - default list
    unsigned count )            // - # arguments
;
#else
#define PtListAlloc( default_list, count )                      \
    ( ( (count) <= AUTO_ARG_MAX )                               \
      ? default_list                                            \
      : (PTREE*)CMemAlloc( count * sizeof( PTREE ) )            \
    )
#endif
#if 0
void PtListFree(                // FREE PTREE LIST
    PTREE* actual_list,         // - list used
    unsigned count )            // - # arguments
;
#else
#define PtListFree( actual_list, count )                        \
    if( (count) > AUTO_ARG_MAX ) {                              \
        CMemFree( actual_list );                                \
    }
#endif
TYPE TypeGetActualFlags(        // GET ACTUAL MODIFIER FLAGS
    TYPE type,                  // - type
    type_flag *flags )          // - ptr( flags )
;
TYPE TypeModExtract(            // EXTRACT MODIFIER INFORMATION
    TYPE type,                  // - input type
    type_flag *flags,           // - addr[ modifier flags]
    void **a_baser,             // - __based element
    type_exclude mask )         // - exclusions
;
TYPE TypeModFlags(              // GET MODIFIER FLAGS, UNMODIFIED TYPE
    TYPE type,                  // - type
    type_flag *pflag )          // - ptr( flags )
;
TYPE TypeModFlagsEC(            // GET MODIFIER FLAGS, UNMODIFIED TYPE
                                // don't skip char and enum
    TYPE type,                  // - type
    type_flag *pflag )          // - ptr( flags )
;
TYPE TypeModFlagsBaseEC(        // GET MODIFIER FLAGS & BASE, UNMODIFIED TYPE
                                // don't skip char and enum
    TYPE type,                  // - type
    type_flag *pflag,           // - ptr( flag )
    void **base )               // - ptr( base )
;
TYPE CgStripType(               // STRIP ONE LEVEL OF TYPE INFORMATION
    TYPE type )                 // - type
;
unsigned CgTypeOutput(          // COMPUTE TYPE FOR CODE GENERATOR
    TYPE type )                 // - C++ type
;
boolean IsCgTypeAggregate(      // CAN TYPE CAN BE INITIALIZED AS AGGREGATE?
    TYPE type,                  // - C++ type
    boolean string )            // - array of string not aggregate
;
target_size_t CgMemorySize(     // COMPUTE SIZE OF A TYPE IN MEMORY
    TYPE type )                 // - type
;
unsigned CgTypePtrSym(          // COMPUTE OUTPUT TYPE OF POINTER TO SYMBOL
    SYMBOL sym )                // - symbol
;
target_size_t CgTypeSize(       // COMPUTE SIZE OF A TYPE
    TYPE type )                 // - type
;
unsigned CgTypeSym(             // COMPUTE OUTPUT TYPE FOR SYMBOL
    SYMBOL sym )                // - the symbol
;
TYPE TypePointerDiff(           // GET TYPE FOR DIFFERENCE OF POINTERS
    TYPE type )                 // - node for a type
;
TYPE TypeReference(             // GET REFERENCE TYPE
    TYPE type )                 // - the type
;

// type cache support
typedef enum typc_index {
    #include "typcache.h"
} typc_index;

extern TYPE TypeCache[];
#define TypeGetCache( tci )     ( TypeCache[ (tci) ] )
#define TypeSetCache( tci, ty ) ( TypeCache[ (tci) ] = ( ty ) )

#define TypeSegmentShort()      TypeGetCache( TYPC_SEGMENT_SHORT )
#define TypePtrToVoid()         TypeGetCache( TYPC_VOID_PTR )
#define TypeVoidFunOfVoid()     TypeGetCache( TYPC_VOID_FUN_OF_VOID )
#define TypeVoidFunOfPtrVoid()  TypeGetCache( TYPC_VOID_FUN_OF_PTR_VOID )
#define TypePtrVoidFunOfVoid()  TypeGetCache( TYPC_PTR_VOID_FUN_OF_VOID )
#define TypePtrToVoidStdop()    TypeGetCache( TYPC_PTR_STDOP_VOID )
#define TypePtrToConstVoidStdop() TypeGetCache( TYPC_PTR_CONST_STDOP_VOID )
#define TypePtrToVolatileVoidStdop() TypeGetCache( TYPC_PTR_VOLATILE_STDOP_VOID )
#define TypePtrToConstVolatileVoidStdop() TypeGetCache( TYPC_PTR_CONST_VOLATILE_STDOP_VOID )
#define TypePtrToVoidStdopArith() TypeGetCache( TYPC_PTR_STDOP_ARITH_VOID )
#define TypePtrToConstVoidStdopArith() TypeGetCache( TYPC_PTR_CONST_STDOP_ARITH_VOID )
#define MakeCDtorExtraArgType() TypeGetCache( TYPC_CDTOR_ARG )
#define TypePtrVoidFunOfCDtorArg() TypeGetCache( TYPC_PTR_VOID_FUN_OF_CDTOR_ARG )
#define TypeVoidHandlerFunOfVoid() TypeGetCache( TYPC_VOID_HANDLER_FUN_OF_VOID )

// pre-compiled header support
TYPE TypeGetIndex( TYPE );
TYPE TypeMapIndex( TYPE );
CLASSINFO *ClassInfoGetIndex( CLASSINFO * );
CLASSINFO *ClassInfoMapIndex( CLASSINFO * );
SYMBOL_NAME SymbolNameGetIndex( SYMBOL_NAME );
SYMBOL_NAME SymbolNameMapIndex( SYMBOL_NAME );
SYMBOL SymbolGetIndex( SYMBOL );
SYMBOL SymbolMapIndex( SYMBOL );
SCOPE ScopeGetIndex( SCOPE );
SCOPE ScopeMapIndex( SCOPE );

#define _SYMTYPE_H
#endif
