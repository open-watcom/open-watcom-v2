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


#ifndef _DIRECT_H_
#define _DIRECT_H_

#ifndef _DIRECT_FIX_

#include "womp.h"
#include "pcobj.h"
#include "objrec.h"
#include "asmsym.h"

#ifndef asm_op
#include "asmops2.h"
#endif

#define MAX_LNAME       255
#define LNAME_NULL      0

#define BIT16           0
#define BIT32           1

typedef int     direct_idx;     // directive index, such as segment index,
                                // group index or lname index, etc.

#define MAGIC_FLAT_GROUP        (ModuleInfo.flat_idx)

typedef enum {
        SIM_CODE = 0,
        SIM_STACK,
        SIM_DATA,
        SIM_DATA_UN,            // .DATA?
        SIM_FARDATA,
        SIM_FARDATA_UN,         // .FARDATA?
        SIM_CONST,
        SIM_NONE,
        SIM_LAST = SIM_NONE
} sim_seg;

typedef enum {
        MOD_NONE,
        MOD_TINY,
        MOD_SMALL,
        MOD_COMPACT,
        MOD_FLAT,
        MOD_MEDIUM,
        MOD_LARGE,
        MOD_HUGE,
} mod_type;             // Memory model type

typedef enum {
        STACK_NONE,
        STACK_NEAR,
        STACK_FAR,
} dist_type;            // Stack distance
#define NUM_STACKTYPE 3

typedef enum {
        VIS_PRIVATE,
        VIS_PUBLIC,
        VIS_EXPORT,
} vis_type;             // Type of visibility for procedure

typedef enum {
        LANG_NONE,
        LANG_BASIC,
        LANG_FORTRAN,
        LANG_PASCAL,
        LANG_C,
        LANG_WATCOM_C,
        LANG_STDCALL,
        LANG_SYSCALL
} lang_type;            // Type of language specified in procedure defn

typedef enum {
        OPSYS_DOS,
        OPSYS_OS2
} os_type;              // Type of operating system
#define NUM_OS 2

enum {
    TAB_FIRST = 0,
    TAB_SEG = TAB_FIRST,        // order seg, grp, lname is important
    TAB_GRP,
    TAB_PUB,
    TAB_LIB,
    TAB_EXT,
    TAB_CONST,
    TAB_PROC,
    TAB_MACRO,
    TAB_LNAME,
    TAB_CLASS_LNAME,
    TAB_STRUCT,
    TAB_GLOBAL,
    TAB_COMM,
    TAB_LAST
};                      // tables for definitions


enum {
    QUERY_COMMENT,
    QUERY_COMMENT_DELIM,
    START_COMMENT,
    END_COMMENT
};                      // parms to Comment

/*---------------------------------------------------------------------------*/

typedef struct stacknode {
    void    *next;
    void    *elt;
} stacknode;

/*---------------------------------------------------------------------------*/
/* Structures for grpdef, segdef, externdef, pubdef, included library,       */
/* procedure and symbolic integer constants.                                 */
/*---------------------------------------------------------------------------*/
struct dir_node;

typedef struct seg_list {
    struct seg_list     *next;
    struct dir_node     *seg;
} seg_list;

typedef struct {
    direct_idx  idx;            // its group index
    seg_list    *seglist;       // list of segments in the group
    uint        numseg;         // number of segments in the group
    direct_idx  lname_idx;
} grp_info;

typedef struct {
    obj_rec     *segrec;
    direct_idx  idx;            // its group index
    uint_32     start_loc;      // starting offset of current ledata or lidata
    int_8       readonly:1;     // if the segment is readonly
    int_8       ignore:1;       // ignore this if the seg is redefined
    direct_idx  lname_idx;
    uint_32     current_loc;    // current offset in current ledata or lidata
} seg_info;

typedef struct {
    uint        idx;            // external definition index
} ext_info;

typedef struct {
    uint            idx;                // external definition index
    unsigned long   size;
    uint            distance;
} comm_info;

typedef struct {
//    char              *string;        // string assigned to the symbol
    int                 redefine:1,     // whether it is redefinable or not
                        expand_early:1; // if TRUE expand before parsing
    int                 count;          // number of tokens
    struct asm_tok      *data;          // array of asm_tok's to replace symbol
} const_info;

typedef struct regs_list {
    struct regs_list    *next;
    char                *reg;
} regs_list;

typedef struct label_list {
    struct label_list   *next;
    char                *label;         // name of parameter
    char                *replace;       // string that replaces the label
    int                 size;           // size of parameter
    int                 factor;         // for local var only
    union {
        int             is_vararg:1;    // if it is a vararg
        int             count;          // number of element in this label
    };
} label_list;

typedef struct {
    regs_list   *regslist;      // list of registers to be saved
    vis_type    visibility;     // PUBLIC, PRIVATE or EXPORT
    lang_type   langtype;       // language type
    label_list  *paralist;      // list of parameters
    label_list  *locallist;     // list of local variables
    int         parasize;       // total no. of bytes used by parameters
    int         localsize;      // total no. of bytes used by local variables
    int         mem_type;       // distance of procedure: near or far
    int         is_vararg:1;    // if it has a vararg
} proc_info;

typedef struct parm_list {
    struct parm_list    *next;
    char                *label;         // name of parameter
    char                *replace;       // string that replaces the label
    char                required;       // is parm required ( T/F )
    char                *def;           // is there a default parm?
} parm_list;

typedef struct asmlines {
    struct asmlines     *next;
    char                *line;
    char                parmcount;
} asmlines;

typedef struct {
    parm_list       *parmlist;  // list of parameters
    asmlines        *data;      // the guts of the macro - LL of strings
    char            *filename;
    unsigned int    start_line;
    char            hidden;     // if TRUE don't print error messages
} macro_info;

typedef struct {
    direct_idx      idx;                // lname index
} lname_info;

typedef struct field_list {
    struct field_list   *next;
    char                *initializer;
    char                *value;
} field_list;

typedef struct {
    unsigned short  size;       // size in bytes ( including alignment )
    unsigned short  alignment;
    field_list      *head;
    field_list      *tail;
} struct_info;

union entry {
    seg_info    *seginfo;       // info about segment definition
    grp_info    *grpinfo;       // info about group definition
    ext_info    *extinfo;       // info about external definition
    const_info  *constinfo;     // info about symbolic integer constants
    proc_info   *procinfo;
    macro_info  *macroinfo;
    lname_info  *lnameinfo;
    struct_info *structinfo;
    comm_info   *comminfo;
};

typedef struct dir_node {
    struct asm_sym      sym;
    union entry         e;
    uint_16             line;   // line number of the directive in source file
    struct dir_node     *next, *prev; // linked list of this type of symbol
} dir_node;         // List of grpdef, segdef, pubdef, externdef, included lib
                    // and symbolic integer constants.

typedef struct {
    dir_node *head;
    dir_node *tail;
} symbol_queue;     // tables array - queues of symbols of 1 type ie: segments
                    // the data are actually part of the symbol table

typedef struct proc_stack_node {
    struct proc_stack_node *next;
    dir_node *proc;
} proc_stack_node;

/*---------------------------------------------------------------------------*/

/* global structure */

typedef struct a_definition_struct {
    unsigned short  struct_depth;
    stacknode       *struct_stack;      // stack of nested structs being defined
    dir_node        *curr_struct;
} a_definition_struct;

extern a_definition_struct Definition;

typedef struct {
    dist_type   distance;       // stack distance;
    mod_type    model;          // memory model;
    lang_type   langtype;       // language;
    os_type     ostype;         // operating system;
    unsigned    use32:1;        // If 32-bit is used
    unsigned    init:1;
    unsigned    cmdline:1;
    unsigned    flat_idx;        // index of FLAT group
    char        name[_MAX_FNAME];// name of module
} module_info;                  // Information about the module

/*---------------------------------------------------------------------------*/

typedef struct {
    sim_seg     seg;                    // segment id
    char        close[MAX_LINE];        // closing line for this segment
    int_16      stack_size;             // size of stack segment
} last_seg_info;        // information about last opened simplified segment

/*---------------------------------------------------------------------------*/

enum {
    ASSUME_DS=0,
    ASSUME_ES,
    ASSUME_SS,
    ASSUME_FS,
    ASSUME_GS,
    ASSUME_CS,
    ASSUME_LAST,
    ASSUME_ERROR,
    ASSUME_NOTHING
};

typedef struct {
    asm_sym             *symbol;        /* segment or group that is to
                                           be associated with the register */
    unsigned int        error:1;        // the register is assumed to ERROR
    unsigned int        flat:1;         // the register is assumed to FLAT
} assume_info;

extern assume_info AssumeTable[ASSUME_LAST];

/*---------------------------------------------------------------------------*/

extern void             TableInit( void );
extern void             TableFini( void );

extern void             FreeTable( int );
/* Free all the directive tables */

//extern dir_dir        *DirLookup( char *, int );
/* Search for the directive node (name specified by 1st para) in the table
   ( specified by 2nd para ) */

extern void             IdxInit( void );
/* Initialize all the index variables */

extern int              ExpandMacro( int );

extern direct_idx       GetLnameIdx( char * );

extern direct_idx       LnameInsert( char * );  // Insert a lname
extern direct_idx       FindClassLnameIdx( char * ); // find index for given name
extern uint_32          GetCurrAddr( void );    // Get offset from current segment

extern uint             GetCurrSeg( void );
/* Get current segment index; 0 means none */

extern uint             GetCurrGrp( void );
/* Get current group index; 0 means none */

extern uint             GetGrpIdx( struct asm_sym * );
/* get symbol's group, from the symbol itself or from the symbol's segment */

extern uint             GetDirIdx( char *, int );
/* Get the index of either a segment, a group or an extrn defn */

extern int              GlobalDef( int );       // define an global symbol
extern int              ExtDef( int );          // define an external symbol
extern int              CommDef( int );         // define an communal symbol
extern struct asm_sym   *MakeExtern( char *name, int type, char already_defd );
extern int              PubDef( int );          // define a public symbol
extern int              GrpDef( int );          // define a group
extern int              SegDef( int );          // open or close a segment
extern int              SetCurrSeg( int );      // open or close a segment in
                                                // the second pass
extern int              ProcDef( int );         // define a procedure
extern int              LocalDef( int );        // define local variables to procedure
extern int              ProcEnd( int );         // end a procedure
extern int              Ret( int, int );        // emit return statement from procedure
extern int              WritePrologue( void );  // emit prologue statement after the
                                                // declaration of a procedure
extern int              MacroDef( int, char );  // define a macro
extern int              Startup( int );         // handle .startup & .exit
extern int              SimSeg( int );          // handle simplified segment
extern int              Include( int );         // handle an INCLUDE statement
extern int              IncludeLib( int );      // handle an INCLUDELIB statement
extern int              Model( int );           // handle .MODEL statement

extern void             ModuleInit( void );
/* Initializes the information about the module, which are contained in
   ModuleInfo */

extern int              ModuleEnd( int );       // handle END statement

extern uint_32          GetCurrSegStart(void);
/* Get offset of segment at the start of current LEDATA record */

extern dir_node *GetSeg( struct asm_sym *sym );

extern void             AssumeInit( void );     // init all assumed-register table
extern int              SetAssume( int );       // Assume a register

extern uint             GetAssume( struct asm_sym*, uint );
/* Return the assumed register of the symbol, and determine the frame and
   frame_datum of its fixup */

extern uint             GetPrefixAssume( struct asm_sym* , uint );
/* Determine the frame and frame_datum of a symbol with a register prefix */

extern int              DefineConstant( int, char, char );
/* pass in an equ statement, and store the constant defined */
extern int              StoreConstant( char *, char *, int_8 );
/* Store a constant */

extern int              FixOverride( int );
/* Get the correct frame and frame_datum for a label when there is a segment
   or group override. */

extern void             GetSymInfo( struct asm_sym * );
/* Store location information about a symbol */
extern int NameDirective( int );

extern int Comment( int, int ); /* handle COMMENT directives */

extern int AddAlias( int );
extern void FreePubQueue( void );
extern void FreeAliasQueue( void );
extern void FreeLnameQueue( void );
extern void FreeInfo( dir_node * );
extern void push( void **stack, void *elt );
extern void *pop( void **stack );
/*---------------------------------------------------------------------------*/

#undef fix
#define fix( tok, str, val, init )              tok
enum {

#else

#undef _DIRECT_FIX_
#undef fix
#define fix( tok, string, value, init_val )     { string, value, init_val }

typedef struct {
      char      *string;        // the token string
      uint      value;          // value connected to this token
      uint      init;           // explained in direct.c ( look at SegDef() )
} typeinfo;

#define INIT_ALIGN      0x1
#define INIT_COMBINE    0x2
#define INIT_USE        0x4
#define INIT_CLASS      0x8
#define INIT_MEMORY     0x10
#define INIT_STACK      0x20

extern typeinfo TypeInfo[] = {

#endif

// Strings recognized by directives

fix( TOK_READONLY,      "READONLY",     0,              0               ),
fix( TOK_BYTE,          "BYTE",         ALIGN_BYTE,     INIT_ALIGN      ),
fix( TOK_WORD,          "WORD",         ALIGN_WORD,     INIT_ALIGN      ),
fix( TOK_DWORD,         "DWORD",        ALIGN_DWORD,    INIT_ALIGN      ),
fix( TOK_PARA,          "PARA",         ALIGN_PARA,     INIT_ALIGN      ),
fix( TOK_PAGE,          "PAGE",         ALIGN_PAGE,     INIT_ALIGN      ),
fix( TOK_PRIVATE,       "PRIVATE",      COMB_INVALID,   INIT_COMBINE    ),
fix( TOK_PUBLIC,        "PUBLIC",       COMB_ADDOFF,    INIT_COMBINE    ),
fix( TOK_STACK,         "STACK",        COMB_STACK,     INIT_COMBINE    ),
fix( TOK_COMMON,        "COMMON",       COMB_COMMON,    INIT_COMBINE    ),
fix( TOK_MEMORY,        "MEMORY",       COMB_ADDOFF,    INIT_COMBINE    ),
fix( TOK_USE16,         "USE16",        FALSE,          INIT_USE        ),
fix( TOK_USE32,         "USE32",        TRUE,           INIT_USE        ),
fix( TOK_IGNORE,        "IGNORE",       0,              0               ),
fix( TOK_AT,            "AT",           0,              0               ),
fix( TOK_CLASS,         NULL,           0,              INIT_CLASS      ),
fix( TOK_TINY,          "TINY",         MOD_TINY,       INIT_MEMORY     ),
fix( TOK_SMALL,         "SMALL",        MOD_SMALL,      INIT_MEMORY     ),
fix( TOK_COMPACT,       "COMPACT",      MOD_COMPACT,    INIT_MEMORY     ),
fix( TOK_MEDIUM,        "MEDIUM",       MOD_MEDIUM,     INIT_MEMORY     ),
fix( TOK_LARGE,         "LARGE",        MOD_LARGE,      INIT_MEMORY     ),
fix( TOK_HUGE,          "HUGE",         MOD_HUGE,       INIT_MEMORY     ),
fix( TOK_FLAT,          "FLAT",         MOD_FLAT,       INIT_MEMORY     ),
fix( TOK_NEARSTACK,     "NEARSTACK",    STACK_NEAR,     INIT_STACK      ),
fix( TOK_FARSTACK,      "FARSTACK",     STACK_FAR,      INIT_STACK      ),
fix( TOK_EXT_NEAR,      "NEAR",         T_NEAR,         0               ),
fix( TOK_EXT_FAR,       "FAR",          T_FAR,          0               ),
fix( TOK_EXT_PROC,      "PROC",         T_PROC,         0               ),
fix( TOK_EXT_BYTE,      "BYTE",         T_BYTE,         0               ),
fix( TOK_EXT_SBYTE,     "SBYTE",        T_BYTE,         0               ),
fix( TOK_EXT_WORD,      "WORD",         T_WORD,         0               ),
fix( TOK_EXT_SWORD,     "SWORD",        T_WORD,         0               ),
fix( TOK_EXT_DWORD,     "DWORD",        T_DWORD,        0               ),
fix( TOK_EXT_SDWORD,    "SDWORD",       T_DWORD,        0               ),
fix( TOK_EXT_PWORD,     "PWORD",        T_FWORD,        0               ),
fix( TOK_EXT_FWORD,     "FWORD",        T_FWORD,        0               ),
fix( TOK_EXT_QWORD,     "QWORD",        T_QWORD,        0               ),
fix( TOK_EXT_TBYTE,     "TBYTE",        T_TBYTE,        0               ),
fix( TOK_EXT_ABS,       "ABS",          T_ABS2,         0               ),
fix( TOK_DS,            "DS",           ASSUME_DS,      0               ),
fix( TOK_ES,            "ES",           ASSUME_ES,      0               ),
fix( TOK_SS,            "SS",           ASSUME_SS,      0               ),
fix( TOK_FS,            "FS",           ASSUME_FS,      0               ),
fix( TOK_GS,            "GS",           ASSUME_GS,      0               ),
fix( TOK_CS,            "CS",           ASSUME_CS,      0               ),
fix( TOK_ERROR,         "ERROR",        ASSUME_ERROR,   0               ),
fix( TOK_NOTHING,       "NOTHING",      ASSUME_NOTHING, 0               ),
fix( TOK_PROC_FAR,      "FAR",          T_FAR,          0               ),
fix( TOK_PROC_NEAR,     "NEAR",         T_NEAR,         0               ),
fix( TOK_PROC_BASIC,    "BASIC",        LANG_BASIC,     0               ),
fix( TOK_PROC_FORTRAN,  "FORTRAN",      LANG_FORTRAN,   0               ),
fix( TOK_PROC_PASCAL,   "PASCAL",       LANG_PASCAL,    0               ),
fix( TOK_PROC_C,        "C",            LANG_C,         0               ),
fix( TOK_PROC_WATCOM_C, "WATCOM_C",     LANG_WATCOM_C,  0               ),
fix( TOK_PROC_STDCALL,  "STDCALL",      LANG_STDCALL,   0               ),
fix( TOK_PROC_SYSCALL,  "SYSCALL",      LANG_SYSCALL,   0               ),
fix( TOK_PROC_PRIVATE,  "PRIVATE",      VIS_PRIVATE,    0               ),
fix( TOK_PROC_PUBLIC,   "PUBLIC",       VIS_PUBLIC,     0               ),
fix( TOK_PROC_EXPORT,   "EXPORT",       VIS_EXPORT,     0               ),
fix( TOK_PROC_USES,     "USES",         0,              0               ),
fix( TOK_PROC_VARARG,   "VARARG",       0,              0               ),
fix( TOK_OS_OS2,        "OS_OS2",       OPSYS_OS2,      0               ),
fix( TOK_OS_DOS,        "OS_DOS",       OPSYS_DOS,      0               ),

};

#endif
