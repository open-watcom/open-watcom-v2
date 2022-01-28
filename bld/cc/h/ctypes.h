/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal CFE types.
*
****************************************************************************/


#include "cgstd.h"
#include <stddef.h>
#include "cg.h"
#include "cgaux.h"

/* only for long_double */
#include "xfloat.h"

#define ID_HASH_SIZE            241
typedef unsigned char   id_hash_idx;

#define MACRO_HASH_SIZE         4093
typedef unsigned short  mac_hash_idx;

#define STRING_HASH_SIZE        1024
typedef unsigned short  str_hash_idx;

#define MAX_PARM_LIST_HASH_SIZE 15
typedef unsigned char   parm_hash_idx;

typedef back_handle     BACK_HANDLE;
typedef label_handle    LABEL_HANDLE;
typedef char            *NAME_HANDLE;
typedef cg_sym_handle   CGSYM_HANDLE;
typedef pointer         CGPOINTER;
typedef dbg_type        dbug_type;
typedef signed_64       int64;
typedef unsigned_64     uint64;

typedef void            *SYM_HANDLE;
typedef const char      *SYM_NAMEPTR;

typedef signed int      id_level_stype;
typedef unsigned char   id_level_type;
typedef int             field_level_stype;
typedef int             expr_level_type;

typedef unsigned char   bitfield_width;
typedef unsigned char   unroll_type;
typedef unsigned char   align_type;

/* CONST, VOLATILE can appear in typ->u.p.decl_flags and leaf->leaf_flags.
*  NEAR, FAR, HUGE can appear in typ->u.p.decl_flags, leaf->leaf_flags,
*                               and sym->attrib.
*  CDECL,PASCAL,FORTRAN,SYSCALL,STDCALL,OPTLINK,FASTCALL,WATCOM
*                       can appear in typ->u.p.decl_flags and sym->attrib.
*
* freed by CFOLD
*/
typedef enum    type_modifiers {    /* type   leaf   sym   */
    FLAG_NONE       = 0x0000,
    FLAG_CONST      = 0x0001,       /* Y0001  Y0001        */
    FLAG_VOLATILE   = 0x0002,       /* Y0002  Y0002        */
    FLAG_RESTRICT   = 0x0004,       /* see how this flies  */

    FLAG_NEAR       = 0x0008,       /* Y0008  Y0008  Y0008 */
    FLAG_FAR        = 0x0010,       /* Y0010  Y0010  Y0010 */
    FLAG_HUGE       = 0x0020,       /* Y0020  Y0020  Y0020 */
    FLAG_INTERRUPT  = (FLAG_NEAR+FLAG_FAR), /* interrupt function */
    /* FLAG_NEAR + FLAG_FAR both on ==> interrupt far */

    LANG_CDECL      = 0x00040,      /* Y0040         Y0040 */
    LANG_PASCAL     = 0x00080,      /* Y0080         Y0080 */
    LANG_FORTRAN    = 0x000C0,      /* Y00C0         Y00C0 */
    LANG_SYSCALL    = 0x00100,      /* Y0100         Y0100 */
    LANG_STDCALL    = 0x00140,      /* Y0140         Y0140 */
    LANG_OPTLINK    = 0x00180,      /* Y0180         Y0180 */
    LANG_FASTCALL   = 0x001C0,      /* Y01C0         Y01C0 */
    LANG_WATCALL    = 0x00200,      /* Y0200         Y0200 */

    FLAG_SAVEREGS   = 0x00400,      /* Y0400         Y0400 */
    FLAG_LOADDS     = 0x00800,      /* Y0800         Y0800 */
    FLAG_EXPORT     = 0x01000,      /* Y1000         Y1000 */
    FLAG_BASED      = 0x02000,      /* Y2000         Y2000 _based ptr or var */
    FLAG_SEGMENT    = 0x04000,      /* Y4000         Y4000 __segment type */
    FLAG_FAR16      = 0x08000,      /* Y8000         Y8000 __far16 modifier */
    FLAG_UNALIGNED  = 0x10000,      /*                     _Packed structures */
    FLAG_INLINE     = 0x20000,      /* Y20000              _inline keyword */
    FLAG_WAS_ARRAY  = 0x20000,      /* Y20000              for "char *argv[]" */
    FLAG_ABORTS     = 0x40000,      /* Y40000              __declspec(aborts) */
    FLAG_NORETURN   = 0x80000,      /* Y80000              __declspec(noreturn) */
    FLAG_FARSS      = 0x100000,     /* Y100000             use far ss for auto variables (-zu) */
} type_modifiers;

#define MASK_CV_QUALIFIERS  (FLAG_CONST|FLAG_VOLATILE)
#define MASK_QUALIFIERS     (MASK_CV_QUALIFIERS|FLAG_UNALIGNED|FLAG_RESTRICT)
#define MASK_MEM_MODEL      (FLAG_NEAR|FLAG_FAR|FLAG_HUGE|FLAG_FAR16)
#define MASK_LANGUAGES      (LANG_CDECL|LANG_PASCAL|LANG_FORTRAN|LANG_SYSCALL|LANG_STDCALL|LANG_OPTLINK|LANG_FASTCALL|LANG_WATCALL)

#define MASK_ALL_MEM_MODELS (MASK_MEM_MODEL|FLAG_BASED)
#define MASK_PTR            (MASK_QUALIFIERS|MASK_ALL_MEM_MODELS)
#define MASK_FUNC           (MASK_LANGUAGES|FLAG_INLINE|FLAG_LOADDS|FLAG_EXPORT|FLAG_SAVEREGS|FLAG_ABORTS|FLAG_NORETURN|FLAG_FARSS)

typedef enum sym_flags {
    SYM_NONE                = 0x00000000,   /* no symbol */
    SYM_FUNCTION            = 0x00000001,   /* ON => function, OFF => variable */
    SYM_REFERENCED          = 0x00000002,   /* symbol was referenced in program */
    SYM_EMITTED             = 0x00000004,   /* symbol has been emitted to cgen */
    SYM_DEFINED             = 0x00000008,   /* symbol has been defined */
    SYM_INITIALIZED         = 0x00000010,   /* variable has been initialized */
    SYM_TEMP                = 0x00000020,   /* this is a temp created by MakeNewSym */
    SYM_ASSIGNED            = 0x00000040,   /* variable has been assigned to */
    SYM_ADDR_TAKEN          = 0x00000080,   /* variable/func has had its address taken */
    SYM_IGNORE_UNREFERENCE  = 0x00000100,   /* don't print msg if unreferenced */
    SYM_OK_TO_RECURSE       = 0x00000200,   /* OK to do tail recursion */
    SYM_FUNC_RETURN_VAR     = 0x00000400,   /* this is function return variable */
    SYM_FUNC_NEEDS_THUNK    = 0x00000800,   /* need a thunk prolog? */
    SYM_BASED_STACK_VARS    = 0x00001000,   /* _based(_segname("_STACK")) vars */
    SYM_USED_IN_PRAGMA      = 0x00002000,   /* symbol referenced in a pragma */
    SYM_INTRINSIC           = 0x00004000,   /* #pragma intrinsic(name) */
    SYM_TRY_VOLATILE        = 0x00008000,   /* volatile because try is used */
    SYM_TYPE_GIVEN          = 0x00010000,   /* a ret type was specified for this func */
    SYM_OLD_STYLE_FUNC      = 0x00020000,   /* function defined with old-style defn */
    SYM_CHECK_STACK         = 0x00040000,   /* function requires stack checking code */
} sym_flags;

typedef enum stg_classes {  // used in sym.stg_class
    SC_NONE         = 0,    // storage class not specified
    SC_FORWARD,             // forward declaration
    SC_EXTERN,              // extern
    SC_STATIC,              // static
    SC_TYPEDEF,             // typedef
    SC_AUTO,                // auto
    SC_REGISTER             // register
} stg_classes;

typedef enum declspec_class {      //note declspec field 2-bits in SYM_ENTRY
    DECLSPEC_NONE       = 0,
    DECLSPEC_DLLIMPORT,
    DECLSPEC_DLLEXPORT,
    DECLSPEC_THREAD,
} declspec_class;

typedef enum string_flags { // string literal flags
    STRLIT_NONE    = 0,
    STRLIT_FAR     = 0x01,
    STRLIT_CONST   = 0x02,
    STRLIT_WIDE    = 0x04,
} string_flags;

typedef struct  string_literal {
    struct string_literal *next_string;
    BACK_HANDLE         back_handle;    /* back handle for string */
    target_size         length;         /* length of literal string */
    unsigned short      ref_count;      /* reference count */
    string_flags        flags;          /* 0 or FLAG_FAR */
    char                *literal;       /* actual literal string */
} STRING_LITERAL, *STR_HANDLE;

#define SYM_INVALID     ((SYM_HANDLE)(pointer_uint)-1)  // invalid symbol; never a real sym
#define SYM_NULL        ((SYM_HANDLE)(pointer_uint)0)

typedef struct array_info {
    target_size         dimension;
    int                 refno;
    bool                unspecified_dim;    // or flexible array member?
} array_info;

typedef enum BASED_KIND {
    BASED_NONE,
    BASED_VOID,          //__based( void )       segment:>offset base op
    BASED_SELFSEG,       //__based( (__segment) __self ) use seg of self
    BASED_VAR,           //__based( <var> )             add to var pointer
    BASED_SEGVAR,        //__based( <var> )             add to var seg
    BASED_VARSEG,        //__based( (__segment) <var> ) ) use seg of var
    BASED_SEGNAME,       //__based( __segname( "name" )   use seg of segname
} BASED_KIND;

/* matches CompTable[] in ccheck.c */
/* matches CTypenames[] table in cdump.c */
/* matches CGDataType[] table in cgen.c */
/* matches AddResult[],SubResult[],IntResult[],ShiftResult[],BinResult[],
           CnvTable[] tables in cmath.c */
/* matches AsmDataType[] table in cpragx86.c */
/* matches CTypeSizes[] table in ctype.c */

typedef enum DATA_TYPE {
    TYP_UNDEFINED = -1,
    #define pick1(enum,cgtype,asmtype,name,size)    TYP_##enum,
    #include "cdatatyp.h"
    #undef pick1
    TYP_LAST_ENTRY,         /* make sure this is always last */
} DATA_TYPE;

// values for type->type_flags
enum type_state {
    TF2_DWARF_DEF =  0x01,            // dwarf type has been defined
    TF2_DWARF_FWD =  0x02,            // dwarf forward reference
    TF2_DWARF     =  (TF2_DWARF_DEF|TF2_DWARF_FWD),  // dwarf is active
//  If the following flag is on, then it indicates a dummy typedef
//  It is used to record modifiers such as const, volatile
//  e.g.
//          struct foo { int a,b };
//          const struct foo *p;
//          type of p will be:
//          TYP_POINTER -> dummy TYP_TYPEDEF const -> TYP_STRUCT
//
    TF2_DUMMY_TYPEDEF   = 0x04,     // gone now dummy typedef to record modifiers
    TF2_TYP_PLAIN_CHAR  = 0x10,     // indicates plain char
    TF2_TYP_SEGMENT     = 0x20,     // indicates __segment type
};

typedef struct type_definition {
    DATA_TYPE               decl_type;
    enum type_state         type_flags;
    struct type_definition  *object;    /* also used by pre-compiled headers */
    struct type_definition  *next_type;
    union {
        dw_handle           dwarf_type; /* used for browsing info */
        dbug_type           debug_type; /* pass 2: debug type */
        int                 type_index; /* for pre-compiled hdrs */
    } u1;
    union {
        struct {
            segment_id      segid;          /* TYP_POINTER */
            SYM_HANDLE      based_sym;      /* var with seg of based ptr*/
            BASED_KIND      based_kind;     /* kind of base variable    */
            type_modifiers  decl_flags;     /* only symbols, fn and ptr have attribs */
        } p;
        struct tag_entry    *tag;           /* STRUCT, UNION, ENUM, also used by pre-compiled header */
        SYM_HANDLE          typedefn;       /* TYP_TYPEDEF */
        struct {                            /* TYP_FUNCTION */
            struct type_definition **parms; /* also used by pre-compiled header */
            type_modifiers  decl_flags;     /* only symbols, fn and ptr have attribs */
        } fn;
        struct {                            /* TYP_FIELD or TYP_UFIELD */
            bitfield_width  field_width;    /* # of bits */
            bitfield_width  field_start;    /* # of bits to << by */
            DATA_TYPE       field_type;     /* TYP_xxxx of field */
        } f;
        array_info          *array;         /* TYP_ARRAY, also used by pre-compiled header */
    } u;
} TYPEDEFN, *TYPEPTR;

typedef struct parm_list {
    struct parm_list    *next_parm;
    TYPEPTR             parm_type;
} parm_list;

typedef struct textsegment {        /* used for #pragma alloc_text(seg,func1,func2,...) */
    struct textsegment *next;       /* also used by pre-compiled header */
    size_t             class;
    segment_id         segid;
    int                index;
    char               segname[1];
} textsegment;

typedef enum src_file_type {
    FT_SRC,
    FT_HEADER,
    FT_HEADER_FORCED,
    FT_HEADER_PRE,
    FT_LIBRARY,
} src_file_type;

typedef struct fname_list {
    struct fname_list   *next;      /* also used by pre-compiled header */
    time_t              mtime;      /* from stat.st_mtime */
    unsigned            index;
    unsigned            index_db;
    bool                rwflag;
    bool                once;
    char                *fullpath;
    char                name[1];
} fname_list, *FNAMEPTR;

#define DBFILE_INVALID  ((unsigned)-1)

typedef struct rdir_list {
    struct rdir_list    *next;      /* also used by pre-compiled header */
    char                name[1];
} rdir_list, *RDIRPTR;

typedef struct ialias_list {
    struct ialias_list  *next;      /* also used by pre-compiled header */
    char                *real_name; /* also used by pre-compiled header */
    bool                is_lib;
    char                alias_name[1];
} ialias_list, *IALIASPTR;

typedef struct incfile {
    struct incfile      *nextfile;
    char                filename[1];
} INCFILE;

typedef struct source_loc {
    unsigned            fno;
    unsigned            line;
    unsigned            column;
} source_loc;

typedef struct xref_entry {
    struct xref_entry   *next_xref;
    source_loc          src_loc;
} XREF_ENTRY, *XREFPTR;

typedef struct id_hash_entry {         /* SYMBOL TABLE structure */
    struct id_hash_entry   *next_sym;  /* also used by pre-compiled header */
    TYPEPTR             sym_type;      /* also used by pre-compiled header */
    SYM_HANDLE          handle;
    id_level_type       level;
    char                name[1];
} id_hash_entry, *SYM_HASHPTR;

typedef struct expr_node    *TREEPTR;

typedef struct symtab_entry {           /* SYMBOL TABLE structure */
    char                *name;
    TYPEPTR             sym_type;       /* also used by pre-compiled header */
    SYM_HANDLE          handle;
    source_loc          src_loc;
    union {
        BACK_HANDLE     backinfo;
        id_hash_idx     hash;
        temp_handle     return_var;     /* for the .R symbol */
    } info;
    union {
        struct {
            target_ssize offset;
            segment_id  segid;          /* segment identifier */
        } var;
        struct {
            SYM_HANDLE  parms;
            SYM_HANDLE  locals;
            TREEPTR     start_of_func;  /* starting tree node */
        } func;
    } u;
    textsegment         *seginfo;       /* also used by pre-compiled header */
    dw_handle           dwarf_handle;   /* used for browsing info; could be perhaps stored in 'info' union. */
    type_modifiers      mods;           /* LANG_CDECL, _PASCAL, _FORTRAN */
    sym_flags           flags;
    id_level_type       level;
    struct {
        unsigned char   stg_class : 3;
        unsigned char   declspec  : 2;
        unsigned char   naked     : 1;
        unsigned char   is_parm   : 1;
        unsigned char   rent      : 1;
    } attribs;
} SYM_ENTRY, *SYMPTR;

typedef struct parm_entry {
    struct  parm_entry  *next_parm;
    SYM_ENTRY           sym;
} PARM_ENTRY, *PARMPTR;


typedef struct field_entry {
    struct field_entry  *next_field;    /* also used by pre-compiled header */
    TYPEPTR             field_type;     /* also used by pre-compiled header */
    XREFPTR             xref;
    target_size         offset;
    type_modifiers      attrib;         /* LANG_CDECL, _PASCAL, _FORTRAN */
    field_level_stype   level;
    id_hash_idx         hash;
    struct field_entry  *next_field_same_hash; /* also used by PCH for length */
    char                name[1];
} FIELD_ENTRY, *FIELDPTR;

typedef struct enum_entry {
    struct enum_entry   *next_enum;     /* used in hash table, also used by PCH for length */
    struct enum_entry   *thread;        /* list belonging to same enum */
    XREFPTR             xref;
    struct tag_entry    *parent;        /* also used by pre-compiled header */
    id_hash_idx         hash;
    uint64              value;
    source_loc          src_loc;
    char                name[1];
} ENUMDEFN, *ENUMPTR;

typedef struct tag_entry {
    struct tag_entry    *next_tag;      /* also used by pre-compiled header */
    TYPEPTR             sym_type;       /* also used by pre-compiled header */
    XREFPTR             xref;
    union {
        ENUMPTR         enum_list;      /* for ENUM */
        FIELDPTR        field_list;     /* for STRUCT or UNION */
    } u;
    target_size         size;           /* size of STRUCT, UNION or ENUM */
    int                 refno;          /* also used by pre-compiled header */
    id_hash_idx         hash;           /* hash value for tag */
    id_level_type       level;
    align_type          alignment;      /* alignment required */
    union   {
        ENUMPTR         last_enum;      /* for ENUM */
        FIELDPTR        last_field;     /* for STRUCT or UNION */
    } u1;
    char                name[1];
} TAGDEFN, *TAGPTR;

/* flags for QUAD.flags field */

enum quad_flags {           /* code data */
    Q_2_INTS_IN_ONE = 0x02, /*       Y02   two integral values */
    Q_DATA          = 0x04, /*  Y04  Y04   DATA_QUAD */
    Q_NEAR_POINTER  = 0x08, /*       Y08   near T_ID */
    Q_FAR_POINTER   = 0x10, /*       Y10   far T_ID */
    Q_CODE_POINTER  = 0x20, /*       Y20   function name */
    Q_REPEATED_DATA = 0x80, /*       Y80   repeated data item */
    Q_NULL          = 0x00
};

enum quad_type {
    #define pick1(enum,cgtype,asmtype,name,size) QDT_##enum,
    #include "cdatatyp.h"
    #undef pick1
    QDT_STATIC,
    QDT_CONSTANT,
    QDT_STRING,
    QDT_CONST,
    QDT_ID,
};

typedef struct {
    union   {
        int             long_values[2];
        unsigned        ulong_values[2];
        int64           long64;
        double          double_value;
        long_double     long_double_value;
        STR_HANDLE      string_leaf;
        struct {
            target_ssize offset;
            SYM_HANDLE  sym_handle;
        } var;
    } u;
    enum quad_type      type;
    enum quad_flags     flags;
} DATA_QUAD;
/* macros to accessing data in data quad structure */
#define u_size          u.ulong_values[0]
#define u_rpt_count     u.ulong_values[1]
#define u_long_value1   u.long_values[0]
#define u_long_value2   u.long_values[1]

typedef struct {
    TYPEPTR             typ;        // type seen
    segment_id          segid;      // seg from a typedef
    stg_classes         stg;        // storage class
    type_modifiers      mod;        // const, vol flags
    type_modifiers      decl_mod;   // declspec call info
    declspec_class      decl;       // dllimport...
    bool                naked;      // declspec naked
} decl_info;

typedef enum {
    DECL_STATE_NONE     = 0x00,
    DECL_STATE_NOTYPE   = 0x01,
    DECL_STATE_ISPARM   = 0x02,
    DECL_STATE_NOSTWRN  = 0x04,
    DECL_STATE_FORLOOP  = 0x08,
} decl_state;

#include "cops.h"

typedef struct label_entry {
    struct symtab_entry *thread;
    struct label_entry  *next_label;
    LABEL_INDEX         ref_list;
    boolbit             defined     : 1;
    boolbit             referenced  : 1;
    char                name[1];
} LABELDEFN, *LABELPTR;

typedef struct segment_list {
    struct segment_list *next_segment;
    segment_id          segid;
    unsigned            size_left;
} segment_list;

struct debug_fwd_types {
    struct  debug_fwd_types *next;
    TYPEPTR             typ;
    dbg_name            debug_name;
    unsigned            scope;
};

typedef enum ppctl_t {
    PPCTL_NO_EXPAND         = 0x01, // don't expand macros
    PPCTL_EOL               = 0x02, // return <end-of-line> as a token
    PPCTL_NO_LEX_ERRORS     = 0x04, // don't diagnose lexical problems
    PPCTL_ASM               = 0x08, // pre-processor is in _asm statement
    PPCTL_NORMAL            = 0x00  // expand macros, treat <eol> as white space, diagnose lexical problems
} ppctl_t;

#define PPCTL_ENABLE_ASM()          PPControl |= PPCTL_ASM
#define PPCTL_DISABLE_ASM()         PPControl &= ~PPCTL_ASM
#define PPCTL_ENABLE_EOL()          PPControl |= PPCTL_EOL
#define PPCTL_DISABLE_EOL()         PPControl &= ~PPCTL_EOL
#define PPCTL_ENABLE_MACROS()       PPControl &= ~PPCTL_NO_EXPAND
#define PPCTL_DISABLE_MACROS()      PPControl |= PPCTL_NO_EXPAND
#define PPCTL_ENABLE_LEX_ERRORS()   PPControl &= ~PPCTL_NO_LEX_ERRORS
#define PPCTL_DISABLE_LEX_ERRORS()  PPControl |= PPCTL_NO_LEX_ERRORS

#define IS_PPCTL_NORMAL()           ((PPControl & (PPCTL_EOL | PPCTL_NO_EXPAND)) == 0)

typedef struct comp_flags {
    boolbit label_dropped                       : 1;
    boolbit has_main                            : 1;
    boolbit float_used                          : 1;
    boolbit signed_char                         : 1;
    boolbit stats_printed                       : 1;
    boolbit far_strings                         : 1;
    boolbit check_syntax                        : 1;
    boolbit meaningless_stmt                    : 1;

    boolbit scanning_cpp_comment                : 1;
    boolbit thread_data_present                 : 1;    /* __declspec(thread) */
    boolbit in_finally_block                    : 1;    /* in _finally { ... } */
    boolbit unix_ext                            : 1;    /* like sizeof( void ) == 1 */
    boolbit slack_byte_warning                  : 1;

    boolbit ef_switch_used                      : 1;
    boolbit in_pragma                           : 1;
    boolbit br_switch_used                      : 1;    /* -br: use DLL C runtime */
    boolbit extensions_enabled                  : 1;
    boolbit inline_functions                    : 1;
    boolbit auto_agg_inits                      : 1;
    boolbit use_full_codegen_od                 : 1;
    boolbit has_wchar_entry                     : 1;

    boolbit bc_switch_used                      : 1;    /* build charater mode */
    boolbit bg_switch_used                      : 1;    /* build gui      mode */
    boolbit emit_all_default_libs               : 1;
    boolbit emit_targimp_symbols                : 1;    /* emit per target auto symbols */
    boolbit low_on_memory_printed               : 1;
    boolbit extra_stats_wanted                  : 1;
    boolbit external_defn_found                 : 1;
    boolbit scanning_comment                    : 1;

    boolbit initializing_data                   : 1;
    boolbit dump_prototypes                     : 1;    /* keep typedefs in prototypes*/
    boolbit non_zero_data                       : 1;
    boolbit quiet_mode                          : 1;
    boolbit useful_side_effect                  : 1;
    boolbit cpp_keep_comments                   : 1;    /* wcpp - output comments */
    boolbit cpp_line_wanted                     : 1;    /* wcpp - emit #line    */
    boolbit cpp_ignore_line                     : 1;    /* wcpp - ignore #line */

    boolbit generate_prototypes                 : 1;    /* generate prototypes  */
    boolbit eq_switch_used                      : 1;    /* don't write wng &err to console */
    boolbit bss_segment_used                    : 1;
    boolbit zu_switch_used                      : 1;
    boolbit extended_defines                    : 1;
    boolbit errfile_written                     : 1;
    boolbit main_has_parms                      : 1;    /* on if "main" has parm(s) */
    boolbit register_conventions                : 1;    /* on for -3r, off for -3s */

    boolbit pgm_used_8087                       : 1;    /* on => 8087 ins. generated */
    boolbit emit_library_names                  : 1;    /* on => put LIB name in obj */
    boolbit strings_in_code_segment             : 1;    /* on => put strings in CODE */
    boolbit ok_to_use_precompiled_hdr           : 1;    /* on => ok to use PCH */
    boolbit strict_ANSI                         : 1;    /* on => strict ANSI C (-zA)*/
    boolbit expand_macros                       : 1;    /* on => expand macros in WCPP*/
    boolbit exception_filter_expr               : 1;    /* on => parsing _except(expr)*/
    boolbit exception_handler                   : 1;    /* on => inside _except block*/

    boolbit wide_char_string                    : 1;    /* on => T_STRING is L"xxx"  */
    boolbit banner_printed                      : 1;    /* on => banner printed      */
    boolbit undefine_all_macros                 : 1;    /* on => -u all macros       */
    boolbit emit_browser_info                   : 1;    /* -db emit broswer info     */
    boolbit rescan_buffer_done                  : 1;    /* ## re-scan buffer used up */
    boolbit cpp_output                          : 1;    /* compiler doing CPP output */
    boolbit cpp_output_to_file                  : 1;    /* compiler doing CPP output to file (default extension .i) */

/*  /d1+
    generate info on BP-chains if possible
    generate sym info on the following items:
            - autos with address taken
            - autos of type struct or array
            - externs and statics
            - parms with /3s
*/
    boolbit debug_info_some                     : 1;    /* d1 + some typing info     */
    boolbit register_conv_set                   : 1;    /* has call conv been set    */
    boolbit emit_names                          : 1;    /* /en switch used           */
    boolbit cpp_mode                            : 1;    /* compiler CPP mode         */
    boolbit warnings_cause_bad_exit             : 1;    /* warnings=>non-zero exit   */
    boolbit save_restore_segregs                : 1;    /* save/restore segregs      */
    boolbit has_winmain                         : 1;    /* WinMain defined           */
    boolbit make_enums_an_int                   : 1;    /* force all enums to be int */

    boolbit original_enum_setting               : 1;    /* reset value if pragma used*/
    boolbit zc_switch_used                      : 1;    /* -zc switch specified   */
    boolbit use_unicode                         : 1;    /* use unicode for L"abc" */
    boolbit op_switch_used                      : 1;    /* -op force floats to mem */
    boolbit no_debug_type_names                 : 1;    /* -d2~ switch specified  */
    boolbit asciiout_used                       : 1;    /* (asciiout specified  */
    boolbit addr_of_auto_taken                  : 1;    /*=>can't opt tail recursion*/
    boolbit sg_switch_used                      : 1;    /* /sg switch used */

    boolbit bm_switch_used                      : 1;    /* /bm switch used */
    boolbit bd_switch_used                      : 1;    /* /bd switch used */
    boolbit bw_switch_used                      : 1;    /* /bw switch used */
    boolbit zm_switch_used                      : 1;    /* /zm switch used */
    boolbit has_libmain                         : 1;    /* LibMain defined */
    boolbit ep_switch_used                      : 1;    /* emit prolog hooks */
    boolbit ee_switch_used                      : 1;    /* emit epilog hooks */
    boolbit dump_types_with_names               : 1;    /* -d3 information */

    boolbit ec_switch_used                      : 1;    /* emit coverage hooks */
    boolbit jis_to_unicode                      : 1;    /* convert JIS to UNICODE */
    boolbit using_overlays                      : 1;    /* user doing overlays */
    boolbit unique_functions                    : 1;    /* func addrs are unique */
    boolbit st_switch_used                      : 1;    /* touch stack through esp */
    boolbit make_precompiled_header             : 1;    /* make precompiled header */
    boolbit emit_dependencies                   : 1;    /* include file dependencies*/
    boolbit multiple_code_segments              : 1;    /* more than 1 code seg */

    boolbit returns_promoted                    : 1;    /* return char/short as int */
    boolbit pending_dead_code                   : 1;    /* aborts func in an expr */
    boolbit use_precompiled_header              : 1;    /* use precompiled header */
    boolbit doing_macro_expansion               : 1;    /* doing macro expansion */
    boolbit no_pch_warnings                     : 1;    /* disable PCH warnings */
    boolbit align_structs_on_qwords             : 1;    /* for Alpha */
    boolbit no_check_inits                      : 1;    /* ease init  type checking */
    boolbit no_check_qualifiers                 : 1;    /* ease qualifier mismatch */

    boolbit use_stdcall_at_number               : 1;    /* add @nn thing */
    boolbit rent                                : 1;    /* make re-entrant r/w split thind  */
    boolbit unaligned_segs                      : 1;    /* don't align segments */
    boolbit trigraph_alert                      : 1;    /* trigraph char alert */
    boolbit generate_auto_depend                : 1;    /* Generate make auto depend file */
    boolbit c99_extensions                      : 1;    /* C99 extensions enabled */
    boolbit use_long_double                     : 1;    /* Make CC send long double types to code gen */
    boolbit track_includes                      : 1;    /* report opens of include files */

    boolbit cpp_ignore_env                      : 1;    /* ignore *INCLUDE env var(s) */
    boolbit ignore_default_dirs                 : 1;    /* ignore default directories for file search .,../h,../c, etc. */
    boolbit pragma_library                      : 1;    /* pragma library simulate -zlf option */
    boolbit non_iso_compliant_names_enabled     : 1;    /* enable all compiler non-ISO compliant names (macros, symbols, etc.) */
    boolbit check_truncated_fnames              : 1;    /* do not check for truncated versions of file names */
} comp_flags;

typedef struct global_comp_flags {  // things that live across compiles
    boolbit cc_reuse                            : 1;    /* in a reusable version batch, dll*/
    boolbit cc_first_use                        : 1;    /* first time thru           */
    boolbit ignore_environment                  : 1;
    boolbit ignore_current_dir                  : 1;
    boolbit ide_cmd_line_has_files              : 1;
    boolbit ide_console_output                  : 1;
    boolbit progress_messages                   : 1;
//    boolbit dll_active                          : 1;
} global_comp_flags;

/* Target System types */
enum {
    TS_OTHER,
    TS_DOS,
    TS_NT,
    TS_WINDOWS,
    TS_CHEAP_WINDOWS,
    TS_NETWARE,
    TS_OS2,
    TS_QNX,
    TS_NETWARE5,
    TS_LINUX,
    TS_RDOS
};

typedef struct call_list {
    struct call_list    *next;
    TREEPTR             callnode;
    source_loc          src_loc;
} call_list;

extern void     WalkTypeList( void (*func)(TYPEPTR) );
extern void     WalkFuncTypeList( void (*func)(TYPEPTR,int) );
extern XREFPTR  NewXref( XREFPTR );
extern void     WalkTagList( void (*func)(TAGPTR) );
