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
* Description:  Internal CFE types.
*
****************************************************************************/


typedef char    *SYM_NAMEPTR;

#include "cg.h"
#include "cgaux.h"

/* only for long_double */
#include "xfloat.h"

#define L       I64LO32
#define H       I64HI32

typedef back_handle     BACK_HANDLE;
typedef label_handle    LABEL_HANDLE;
typedef char            *NAME_HANDLE;
typedef cg_sym_handle   CGSYM_HANDLE;
typedef pointer         CGPOINTER;
typedef dbg_type        dbug_type;
typedef signed_64       int64;
typedef unsigned_64     uint64;

typedef void            *SYM_HANDLE;
typedef SYM_HANDLE      sym_handle;

typedef unsigned char   id_hash_idx;
typedef unsigned short  mac_hash_idx;
typedef unsigned short  str_hash_idx;
typedef unsigned char   parm_hash_idx;

typedef signed int      id_level_stype;
typedef unsigned char   id_level_type;
typedef int             field_level_stype;
typedef int             expr_level_type;

typedef unsigned char   bitfield_width;

typedef unsigned char   unroll_type;

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

    LANG_CDECL      = 0x0040,       /* Y0040         Y0040 */
    LANG_PASCAL     = 0x0080,       /* Y0080         Y0080 */
    LANG_FORTRAN    = 0x00C0,       /* Y00C0         Y00C0 */
    LANG_SYSCALL    = 0x0100,       /* Y0100         Y0100 */     /* 04-jul-91 */
    LANG_STDCALL    = 0x0140,       /* Y0140         Y0140 */     /* 08-jan-92 */
    LANG_OPTLINK    = 0x0180,       /* Y0180         Y0180 */
    LANG_FASTCALL   = 0x01C0,       /* Y01C0         Y01C0 */
    LANG_WATCALL    = 0x0200,       /* Y0200         Y0200 */

    FLAG_SAVEREGS   = 0x0400,       /* Y0400         Y0400 */
    FLAG_LOADDS     = 0x0800,       /* Y0800         Y0800 */
    FLAG_EXPORT     = 0x1000,       /* Y1000         Y1000 */
    FLAG_BASED      = 0x2000,       /* Y2000         Y2000 _based ptr or var */
    FLAG_SEGMENT    = 0x4000,       /* Y4000         Y4000 __segment type */
    FLAG_FAR16      = 0x8000,       /* Y8000         Y8000 __far16 modifier */
    FLAG_UNALIGNED  =0x10000,       /*                     _Packed structures */
    FLAG_INLINE     =0x20000,       /* Y20000              _inline keyword */
    FLAG_WAS_ARRAY  =0x20000,       /* Y20000              for "char *argv[]" */
} type_modifiers;

#define MASK_CV_QUALIFIERS  (FLAG_CONST|FLAG_VOLATILE)
#define MASK_QUALIFIERS     (MASK_CV_QUALIFIERS|FLAG_UNALIGNED|FLAG_RESTRICT)
#define MASK_MEM_MODEL      (FLAG_NEAR|FLAG_FAR|FLAG_HUGE|FLAG_FAR16)
#define MASK_LANGUAGES      (LANG_CDECL|LANG_PASCAL|LANG_FORTRAN|LANG_SYSCALL|LANG_STDCALL|LANG_OPTLINK|LANG_FASTCALL|LANG_WATCALL)

#define MASK_ALL_MEM_MODELS (MASK_MEM_MODEL|FLAG_BASED)
#define MASK_PTR            (MASK_QUALIFIERS|MASK_ALL_MEM_MODELS)
#define MASK_FUNC           (MASK_LANGUAGES|FLAG_INLINE|FLAG_LOADDS|FLAG_EXPORT|FLAG_SAVEREGS)

typedef enum sym_flags {
    SYM_FUNCTION            = 0x01,     /* ON => function, OFF => variable */
    SYM_REFERENCED          = 0x02,     /* symbol was referenced in program */
    SYM_TYPE_GIVEN          = 0x04,     /* a ret type was specified for this func */
    SYM_EMITTED             = 0x04,     /* symbol has been emitted to cgen */
    SYM_DEFINED             = 0x08,     /* symbol has been defined */
    SYM_INITIALIZED         = 0x10,     /* variable has been initialized */
    SYM_OLD_STYLE_FUNC      = 0x10,     /* function defined with old-style defn */
    SYM_TEMP                = 0x20,     /* this is a temp created by MakeNewSym */
    SYM_CHECK_STACK         = 0x40,     /* function requires stack checking code */
    SYM_ASSIGNED            = 0x40,     /* variable has been assigned to */
    SYM_ADDR_TAKEN          = 0x80,     /* variable/func has had its address taken */
    SYM_IGNORE_UNREFERENCE  = 0x0100,   /* don't print msg if unreferenced */
    SYM_OK_TO_RECURSE       = 0x0200,   /* OK to do tail recursion */
    SYM_FUNC_RETURN_VAR     = 0x0400,   /* this is function return variable */
    SYM_FUNC_NEEDS_THUNK    = 0x0800,   /* need a thunk prolog? */
    SYM_BASED_STACK_VARS    = 0x1000,   /* _based(_segname("_STACK")) vars */
    SYM_USED_IN_PRAGMA      = 0x2000,   /* symbol referenced in a pragma */
    SYM_INTRINSIC           = 0x4000,   /* #pragma intrinsic(name) */
    SYM_TRY_VOLATILE        = 0x8000,   /* volatile because try is used */
} sym_flags;

typedef enum stg_classes {  // used in sym.stg_class
    SC_NULL         = 0,    // storage class not specified
    SC_FORWARD,             // forward declaration
    SC_EXTERN,              // extern
    SC_STATIC,              // static
    SC_TYPEDEF,             // typedef
    SC_AUTO,                // auto
    SC_REGISTER             // register
} stg_classes;

typedef enum declspec_class {      //note declspec field 2-bits in SYM_ENTRY
    DECLSPEC_NONE       =    0,
    DECLSPEC_DLLIMPORT,
    DECLSPEC_DLLEXPORT,
    DECLSPEC_THREAD,
} declspec_class;

typedef enum string_flags {     // string literal flags
    STRLIT_WIDE         = 0x80, // must not conflict with FLAG_MEM_MODEL
} string_flags;

typedef struct  string_literal {
    struct string_literal *next_string;
    BACK_HANDLE         back_handle;    /* back handle for string */
    target_size         length;         /* length of literal string */
    unsigned short      ref_count;      /* reference count */
    char                flags;          /* 0 or FLAG_FAR */
    char                *literal;       /* actual literal string */
} STRING_LITERAL, *STR_HANDLE;

#define SYM_INVALID     ((SYM_HANDLE)(pointer_int)-1)    // invalid symbol; never a real sym

typedef struct parm_list {
    struct parm_list        *next_parm;
    struct type_definition  *parm_type;
} parm_list;

typedef struct array_info {
    target_size     dimension;
    int             refno;
    bool            unspecified_dim;    // or flexible array member?
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
    TYPE_UNDEFINED = -1,
    #define pick1(enum,cgtype,asmtype,name,size) TYPE_##enum,
    #include "cdatatyp.h"
    #undef pick1
    TYPE_LAST_ENTRY,        /* make sure this is always last */
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
//          TYPE_POINTER -> dummy TYPE_TYPEDEF const -> TYPE_STRUCT
//
    TF2_DUMMY_TYPEDEF     = 0x04,   // gone now dummy typedef to record modifiers
    TF2_TYPE_PLAIN_CHAR   = 0x10,   // indicates plain char
    TF2_TYPE_SEGMENT      = 0x20,   // indicates __segment type
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
            segment_id      segment;    /* TYPE_POINTER */
            SYM_HANDLE      based_sym;  /* var with seg of based ptr*/
            BASED_KIND      based_kind; /* kind of base variable    */
            type_modifiers  decl_flags; /* only symbols, fn and ptr have attribs */
        } p;
        struct tag_entry    *tag;       /* STRUCT, UNION, ENUM, also used by pre-compiled header */
        SYM_HANDLE          typedefn;   /* TYPE_TYPEDEF */
        struct {                        /* TYPE_FUNCTION */
            struct type_definition **parms; /* also used by pre-compiled header */
            type_modifiers  decl_flags; /* only symbols, fn and ptr have attribs */
        } fn;
        struct {                        /* TYPE_FIELD or TYPE_UFIELD */
            unsigned char field_width;  /* # of bits */
            unsigned char field_start;  /* # of bits to << by */
            DATA_TYPE     field_type;   /* TYPE_xxxx of field */
        } f;
        array_info          *array;     /* TYPE_ARRAY, also used by pre-compiled header */
    } u;
} TYPEDEFN, *TYPEPTR;

extern  void WalkTypeList( void (*func)(TYPEPTR) );
extern  void WalkFuncTypeList( void (*func)(TYPEPTR,int) );

typedef struct textsegment {    /* used for #pragma alloc_text(seg,func1,func2,...) */
    struct textsegment *next;       /* also used by pre-compiled header */
    segment_id         segment;
    int                index;
    char               segname[1];
} textsegment;

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
    int                 len;
    char                filename[1];
} INCFILE;

typedef struct source_loc {
    unsigned        fno;
    unsigned        line;
} source_loc;

typedef struct xref_entry {
    struct xref_entry       *next_xref;
    source_loc              src_loc;
} XREF_ENTRY, *XREFPTR;

extern  XREFPTR NewXref( XREFPTR );

typedef struct id_hash_entry {         /* SYMBOL TABLE structure */
    struct id_hash_entry   *next_sym;  /* also used by pre-compiled header */
    TYPEPTR         sym_type;           /* also used by pre-compiled header */
    SYM_HANDLE      handle;
#if defined( _M_IX86 ) || defined( _M_X64 )
    unsigned char   level;
#else
    int             level;
#endif
    char            name[1];
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
            int         offset;
            segment_id  segment;        /* segment identifier */
        } var;
        struct {
            SYM_HANDLE      parms;
            SYM_HANDLE      locals;
            TREEPTR         start_of_func;  /* starting tree node */
        } func;
    } u;
    textsegment         *seginfo;           /* also used by pre-compiled header */
    dw_handle           dwarf_handle;       /* used for browsing info; could be
                                             * perhaps stored in 'info' union. */
    type_modifiers      mods;               /* LANG_CDECL, _PASCAL, _FORTRAN */
    sym_flags           flags;
#if defined( _M_IX86 ) || defined( _M_X64 )
    unsigned char       level;
#else
    int                 level;
#endif
    struct {
        unsigned char stg_class  : 3;
        unsigned char declspec   : 2;
        unsigned char naked      : 1;
        unsigned char is_parm    : 1;
        unsigned char rent       : 1;
    } attribs;
} SYM_ENTRY, *SYMPTR;

typedef struct parm_entry {
    struct  parm_entry      *next_parm;
    SYM_ENTRY               sym;
} PARM_ENTRY, *PARMPTR;


typedef struct field_entry {
    struct field_entry  *next_field;    /* also used by pre-compiled header */
    TYPEPTR             field_type;     /* also used by pre-compiled header */
    XREFPTR             xref;
    unsigned            offset;
    type_modifiers      attrib;         /* LANG_CDECL, _PASCAL, _FORTRAN */
    int                 level;
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
    unsigned            size;           /* size of STRUCT, UNION or ENUM */
    int                 refno;          /* also used by pre-compiled header */
#if defined( _M_IX86 ) || defined( _M_X64 )
    id_hash_idx         hash;           /* hash value for tag */
    unsigned char       level;
    unsigned char       alignment;      /* alignment required */
#else
    id_hash_idx         hash;
    int                 level;
    unsigned            alignment;
#endif
    union   {
        ENUMPTR         last_enum;         /* for ENUM */
        FIELDPTR        last_field;        /* for STRUCT or UNION */
    } u1;
    char                name[1];
} TAGDEFN, *TAGPTR;

extern  void WalkTagList( void (*func)(TAGPTR) );

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
        int         long_values[2];
        unsigned    ulong_values[2];
        int64       long64;
        double      double_value;
        long_double long_double_value;
        STR_HANDLE  string_leaf;
        struct {
            int         offset;
            SYM_HANDLE  sym_handle;
        } var;
    } u;
    enum quad_type  type;
    enum quad_flags flags;
} DATA_QUAD;

typedef struct {
    TYPEPTR              typ;       // type seen
    int                  seg;       // seg from a typedef
    stg_classes          stg;       // storage class
    type_modifiers       mod;       // const, vol flags
    type_modifiers       decl_mod;  // declspec call info
    declspec_class       decl;      // dllimport...
    bool                 naked;     // declspec naked
} decl_info;

typedef enum {
    DECL_STATE_NONE    = 0x00,
    DECL_STATE_NOTYPE  = 0x01,
    DECL_STATE_ISPARM  = 0x02,
    DECL_STATE_NOSTWRN = 0x04,
    DECL_STATE_FORLOOP = 0x08,
} decl_state;

#include <stddef.h>
#include "cgaux.h"
#include "cops.h"

typedef struct label_entry {
    struct symtab_entry     *thread;
    struct label_entry      *next_label;
    LABEL_INDEX             ref_list;
    unsigned                defined     : 1;
    unsigned                referenced  : 1;
    char                    name[1];
} LABELDEFN, *LABELPTR;

typedef struct segment_list {
    struct segment_list     *next_segment;
    segment_id              segment;
    unsigned                size_left;
} segment_list;

struct debug_fwd_types {
    struct  debug_fwd_types *next;
    TYPEPTR                 typ;
    dbg_name                debug_name;
    unsigned                scope;
};

typedef struct seg_info {
    SEGADDR_T index;        /* segment #, EMS page #, disk seek # */
    unsigned allocated : 1; /* 1 => has been allocated */
} seg_info;

typedef enum {
    PPCTL_NORMAL          = 0x00, // expand macros, treat <eol> as white space
    PPCTL_EOL             = 0x01, // return <end-of-line> as a token
    PPCTL_NO_EXPAND       = 0x02, // don't expand macros
    PPCTL_ASM             = 0x04, // pre-processor is in _asm statement
} ppctl_t;

#define PPCTL_ENABLE_ASM()      CompFlags.pre_processing |= PPCTL_ASM
#define PPCTL_DISABLE_ASM()     CompFlags.pre_processing &= ~PPCTL_ASM
#define PPCTL_ENABLE_EOL()      CompFlags.pre_processing |= PPCTL_EOL
#define PPCTL_DISABLE_EOL()     CompFlags.pre_processing &= ~PPCTL_EOL
#define PPCTL_ENABLE_MACROS()   CompFlags.pre_processing &= ~PPCTL_NO_EXPAND
#define PPCTL_DISABLE_MACROS()  CompFlags.pre_processing |= PPCTL_NO_EXPAND

struct comp_flags {
    unsigned label_dropped          : 1;
    unsigned has_main               : 1;
    unsigned float_used             : 1;
    unsigned signed_char            : 1;
    unsigned stats_printed          : 1;
    unsigned far_strings            : 1;
    unsigned check_syntax           : 1;
    unsigned meaningless_stmt       : 1;

    unsigned pre_processing         : 3;    /* pre-processor control bits */
    unsigned scanning_cpp_comment   : 1;
    unsigned thread_data_present    : 1;    /* __declspec(thread) */
    unsigned in_finally_block       : 1;    /* in _finally { ... } */
    unsigned unix_ext               : 1;    /* like sizeof( void ) == 1 */
    unsigned slack_byte_warning     : 1;

    unsigned ef_switch_used         : 1;
    unsigned in_pragma              : 1;
    unsigned br_switch_used         : 1;    /* -br: use DLL C runtime */
    unsigned extensions_enabled     : 1;
    unsigned inline_functions       : 1;
    unsigned auto_agg_inits         : 1;
    unsigned use_full_codegen_od    : 1;
    unsigned has_wchar_entry        : 1;

    unsigned bc_switch_used         : 1;    /* build charater mode */
    unsigned bg_switch_used         : 1;    /* build gui      mode */
    unsigned emit_all_default_libs  : 1;
    unsigned emit_targimp_symbols   : 1;    /* emit per target auto symbols */
    unsigned low_on_memory_printed  : 1;
    unsigned extra_stats_wanted     : 1;
    unsigned external_defn_found    : 1;
    unsigned scanning_comment       : 1;

    unsigned initializing_data      : 1;
    unsigned dump_prototypes        : 1;    /* keep typedefs in prototypes*/
    unsigned non_zero_data          : 1;
    unsigned quiet_mode             : 1;
    unsigned useful_side_effect     : 1;
    unsigned keep_comments          : 1;    /* wcpp - output comments */
    unsigned cpp_line_wanted        : 1;    /* wcpp - emit #line    */
    unsigned cpp_ignore_line        : 1;    /* wcpp - ignore #line */

    unsigned generate_prototypes    : 1;    /* generate prototypes  */
    unsigned no_conmsg              : 1;    /* don't write wng &err to console */
    unsigned bss_segment_used       : 1;
    unsigned zu_switch_used         : 1;
    unsigned extended_defines       : 1;
    unsigned errfile_written        : 1;
    unsigned main_has_parms         : 1;    /* on if "main" has parm(s) */
    unsigned register_conventions   : 1;    /* on for -3r, off for -3s */

    unsigned pgm_used_8087          : 1;    /* on => 8087 ins. generated */
    unsigned emit_library_names     : 1;    /* on => put LIB name in obj */
    unsigned strings_in_code_segment: 1;    /* on => put strings in CODE */
    unsigned ok_to_use_precompiled_hdr: 1;  /* on => ok to use PCH */
    unsigned strict_ANSI            : 1;    /* on => strict ANSI C (-zA)*/
    unsigned expand_macros          : 1;    /* on => expand macros in WCPP*/
    unsigned exception_filter_expr  : 1;    /* on => parsing _except(expr)*/
    unsigned exception_handler      : 1;    /* on => inside _except block*/

    unsigned comments_wanted        : 1;    /* on => comments wanted     */
    unsigned wide_char_string       : 1;    /* on => T_STRING is L"xxx"  */
    unsigned banner_printed         : 1;    /* on => banner printed      */
    unsigned undefine_all_macros    : 1;    /* on => -u all macros       */
    unsigned emit_browser_info      : 1;    /* -db emit broswer info */
    unsigned rescan_buffer_done     : 1;    /* ## re-scan buffer used up */
    unsigned cpp_output             : 1;    /* WCC doing CPP output      */
    unsigned cpp_output_to_file     : 1;    /* WCC doing CPP output to?.i*/

/*  /d1+
    generate info on BP-chains if possible
    generate sym info on the following items:
            - autos with address taken
            - autos of type struct or array
            - externs and statics
            - parms with /3s
*/
    unsigned debug_info_some        : 1;    /* d1 + some typing info     */
    unsigned register_conv_set      : 1;    /* has call conv been set    */
    unsigned emit_names             : 1;    /* /en switch used           */
    unsigned cpp_output_requested   : 1;    /* CPP output requested      */
    unsigned warnings_cause_bad_exit: 1;    /* warnings=>non-zero exit   */
    unsigned save_restore_segregs   : 1;    /* save/restore segregs      */
    unsigned has_winmain            : 1;    /* WinMain defined           */
    unsigned make_enums_an_int      : 1;    /* force all enums to be int */

    unsigned original_enum_setting  : 1;    /* reset value if pragma used*/
    unsigned zc_switch_used         : 1;    /* -zc switch specified   */
    unsigned use_unicode            : 1;    /* use unicode for L"abc" */
    unsigned op_switch_used         : 1;    /* -op force floats to mem */
    unsigned no_debug_type_names    : 1;    /* -d2~ switch specified  */
    unsigned asciiout_used          : 1;    /* (asciiout specified  */
    unsigned addr_of_auto_taken     : 1;    /*=>can't opt tail recursion*/
    unsigned sg_switch_used         : 1;    /* /sg switch used */

    unsigned bm_switch_used         : 1;    /* /bm switch used */
    unsigned bd_switch_used         : 1;    /* /bd switch used */
    unsigned bw_switch_used         : 1;    /* /bw switch used */
    unsigned zm_switch_used         : 1;    /* /zm switch used */
    unsigned has_libmain            : 1;    /* LibMain defined */
    unsigned ep_switch_used         : 1;    /* emit prolog hooks */
    unsigned ee_switch_used         : 1;    /* emit epilog hooks */
    unsigned dump_types_with_names  : 1;    /* -d3 information */

    unsigned ec_switch_used         : 1;    /* emit coverage hooks */
    unsigned jis_to_unicode         : 1;    /* convert JIS to UNICODE */
    unsigned using_overlays         : 1;    /* user doing overlays */
    unsigned unique_functions       : 1;    /* func addrs are unique */
    unsigned st_switch_used         : 1;    /* touch stack through esp */
    unsigned make_precompiled_header: 1;    /* make precompiled header */
    unsigned emit_dependencies      : 1;    /* include file dependencies*/
    unsigned multiple_code_segments : 1;    /* more than 1 code seg */

    unsigned returns_promoted       : 1;    /* return char/short as int */
    unsigned pending_dead_code      : 1;    /* aborts func in an expr */
    unsigned use_precompiled_header : 1;    /* use precompiled header */
    unsigned doing_macro_expansion  : 1;    /* doing macro expansion */
    unsigned no_pch_warnings        : 1;    /* disable PCH warnings */
    unsigned align_structs_on_qwords: 1;    /* for Alpha */
    unsigned no_check_inits         : 1;    /* ease init  type checking */
    unsigned no_check_qualifiers    : 1;    /* ease qualifier mismatch */

    unsigned use_stdcall_at_number  : 1;    /* add @nn thing */
    unsigned rent                   : 1;    /* make re-entrant r/w split thind  */
    unsigned unaligned_segs         : 1;    /* don't align segments */
    unsigned trigraph_alert         : 1;    /* trigraph char alert */
    unsigned generate_auto_depend   : 1;    /* Generate make auto depend file */
    unsigned c99_extensions         : 1;    /* C99 extensions enabled */
    unsigned use_long_double        : 1;    /* Make CC send long double types to code gen */
    unsigned track_includes         : 1;    /* report opens of include files */

    unsigned ignore_fnf             : 1;    /* ignore file not found errors */
    unsigned disable_ialias         : 1;    /* supress inclusion of _ialias.h */
    unsigned cpp_ignore_env         : 1;    /* ignore *INCLUDE env var(s) */
    unsigned ignore_default_dirs    : 1;    /* ignore default directories for file search .,../h,../c, etc. */
    unsigned pragma_library         : 1;    /* pragma library simulate -zlf option */
    unsigned oldmacros_enabled      : 1;    /* enable old predefined macros (non-ISO compatible) */
};

struct global_comp_flags {  // things that live across compiles
    unsigned cc_reuse               : 1;    /* in a reusable version batch, dll*/
    unsigned cc_first_use           : 1;    /* first time thru           */
    unsigned ignore_environment     : 1;
    unsigned ignore_current_dir     : 1;
    unsigned ide_cmd_line_has_files : 1;
    unsigned ide_console_output     : 1;
    unsigned progress_messages      : 1;
//    unsigned dll_active             : 1;
};

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
    TS_LINUX
};

/* values for ESCChar routine */
#define RTN_SAVE_NEXT_CHAR      0
#define RTN_NEXT_BUF_CHAR       1

typedef struct call_list {
    struct call_list    *next;
    TREEPTR             callnode;
    source_loc          src_loc;
} call_list;
