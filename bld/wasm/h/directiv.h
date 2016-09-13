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
* Description:  directives declarations
*
****************************************************************************/


#ifndef _DIRECTIV_H_
#define _DIRECTIV_H_

#include "objrec.h"

#define MAX_LNAME       255
#define LNAME_NULL      0

typedef uint_16 direct_idx; // directive index, such as segment index,
                            //group index or lname index, etc. follow womp (OMF)

/* Paul Edwards
   Note that there is code that is dependent on the ordering
   of these model types. */
typedef enum {
    MOD_NONE    = 0,
    MOD_TINY    = 1,
    MOD_SMALL   = 2,
    MOD_COMPACT = 3,
    MOD_MEDIUM  = 4,
    MOD_LARGE   = 5,
    MOD_HUGE    = 6,
    MOD_FLAT    = 7,
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
    OPSYS_DOS,
    OPSYS_OS2
} os_type;              // Type of operating system
#define NUM_OS 2

enum {
    TAB_FIRST = 0,
    TAB_SEG = TAB_FIRST,  // order seg, grp, lname is important
    TAB_GRP,
    TAB_LIB,
    TAB_EXT,
    TAB_CONST,
    TAB_PROC,
    TAB_MACRO,
    TAB_CLASS_LNAME,
    TAB_STRUCT,
    TAB_LAST,
    TAB_COMM             // TAB_COMM is not included in tables, it is assigned to TAB_EXT
};                       // tables for definitions

enum {
    QUERY_COMMENT,
    QUERY_COMMENT_DELIM,
    START_COMMENT,
    END_COMMENT
};                      // parms to Comment

typedef enum {
    IRP_CHAR,
    IRP_WORD,
    IRP_REPEAT
} irp_type;

typedef enum {
    SEGTYPE_UNDEF = -1,
    SEGTYPE_ISDATA,
    SEGTYPE_ISCODE
} seg_type;

/*---------------------------------------------------------------------------*/

typedef struct stacknode {
    void    *next;
    void    *elt;
} stacknode;

/*---------------------------------------------------------------------------*/
/* Structures for grpdef, segdef, externdef, pubdef, included library,       */
/* procedure and symbolic integer constants.                                 */
/*---------------------------------------------------------------------------*/

typedef struct seg_list {
    struct seg_list     *next;
    struct dir_node     *seg;
} seg_list;

typedef struct {
    direct_idx          idx;            // group lname/order index
    seg_list            *seglist;       // list of segments in the group
    direct_idx          numseg;         // number of segments in the group
} grp_info;

typedef struct {
    direct_idx          idx;            // segment lname/order index
    struct asm_sym      *group;         // its group
    uint_32             start_loc;      // starting offset of current ledata or lidata
    uint_8              align       :4; // align field (enum segdef_align_values)
    uint_8              combine     :4; // combine field (values in pcobj.h)
    BITB                readonly    :1; // if the segment is readonly
    BITB                ignore      :1; // ignore this if the seg is redefined
    BITB                use_32      :1; // 32-bit segment
    seg_type            iscode;         // segment is belonging to "CODE" or 'DATA' class
    uint_32             current_loc;    // current offset in current ledata or lidata
    uint_32             length;         // segment length
    uint_16             abs_frame;      // frame for absolute segment
    struct asm_sym      *class_name;    // segment class name (lname)
} seg_info;

typedef struct {
    direct_idx          idx;            // external definition index
    BITB                use32       :1;
    BITB                comm        :1;
    BITB                global      :1;
} ext_info;

typedef struct {
    direct_idx          idx;            // external definition index
    BITB                use32       :1;
    BITB                comm        :1;
    unsigned long       size;
    uint                distance;
} comm_info;

typedef struct {
    BITB                predef      :1; // whether it is predefined symbol
    BITB                redefine    :1; // whether it is redefinable or not
    BITB                expand_early:1; // if true expand before parsing
    token_idx           count;          // number of tokens
    asm_tok             *data;          // array of asm_tok's to replace symbol
} const_info;

typedef struct regs_list {
    struct regs_list    *next;
    char                *reg;
} regs_list;

typedef struct label_list {
    struct label_list   *next;
    char                *label;         // name of parameter
    char                *replace;       // string that replaces the label
    struct asm_sym      *sym;           // structure definition or local label symbol
    int                 size;           // size of parameter
    int                 factor;         // for local var only
    bool                is_register;    // for arguments only
    union {
        BITB            is_vararg   :1; // if it is a vararg
        token_idx       count;          // number of element in this label
    } u;
} label_list;

typedef struct {
    regs_list           *regslist;      // list of registers to be saved
    label_list          *paralist;      // list of parameters
    label_list          *locallist;     // list of local variables
    label_list          *labellist;     // list of local labels
    unsigned long       parasize;       // total no. of bytes used by parameters
    unsigned long       localsize;      // total no. of bytes used by local variables
    memtype             mem_type;       // distance of procedure: near or far
    BITB                is_vararg   :1; // if it has a vararg
    BITB                pe_type     :1; // prolog/epilog code type 0:8086/186 1:286 and above
    BITB                export      :1; // EXPORT procedure
} proc_info;

typedef struct parm_list {
    struct parm_list    *next;
    char                *label;         // name of parameter
    char                *replace;       // string that replaces the label
    bool                required;       // is parm required ( T/F )
    char                *def;           // is there a default parm?
} parm_list;

typedef struct asmlines {
    struct asmlines     *next;
    char                *line;
    char                parmcount;
} asmlines;

typedef struct  fname_list {
        struct  fname_list *next;
        time_t  mtime;
        char    *name;
        char    *fullname;
} FNAME;

typedef struct {
    parm_list           *parmlist;  // list of parameters
    asmlines            *data;      // the guts of the macro - LL of strings
    const FNAME         *srcfile;
    bool                hidden;     // if true don't print error messages
} macro_info;

typedef struct {
    direct_idx          idx;                // lname index
} lname_info;

typedef struct field_list {
    struct field_list   *next;
    char                *initializer;
    char                *value;
    struct asm_sym      *sym;
} field_list;

typedef struct {
    unsigned            size;       // size in bytes ( including alignment )
    unsigned short      alignment;
    field_list          *head;
    field_list          *tail;
} struct_info;

union entry {
    seg_info            *seginfo;       // info about segment definition
    grp_info            *grpinfo;       // info about group definition
    ext_info            *extinfo;       // info about external definition
    const_info          *constinfo;     // info about symbolic integer constants
    proc_info           *procinfo;
    macro_info          *macroinfo;
    lname_info          *lnameinfo;
    struct_info         *structinfo;
    comm_info           *comminfo;
};

typedef struct dir_node {
    struct asm_sym      sym;
    union entry         e;
    unsigned long       line_num;     // line number of the directive in source file
    struct dir_node     *next, *prev; // linked list of this type of symbol
} dir_node;         // List of grpdef, segdef, pubdef, externdef, included lib
                    // and symbolic integer constants.

typedef struct {
    dir_node            *head;
    dir_node            *tail;
} symbol_queue;     // tables array - queues of symbols of 1 type ie: segments
                    // the data are actually part of the symbol table

/*---------------------------------------------------------------------------*/

/* global structure */

typedef struct a_definition_struct {
    unsigned short      struct_depth;
    stacknode           *struct_stack;      // stack of nested structs being defined
    dir_node            *curr_struct;
} a_definition_struct;

extern a_definition_struct      Definition;

enum assume_reg {
    ASSUME_DS = 0,
    ASSUME_ES,
    ASSUME_SS,
    ASSUME_FS,
    ASSUME_GS,
    ASSUME_CS,
    ASSUME_ERROR,
    ASSUME_NOTHING
};

#define ASSUME_FIRST    ASSUME_DS
#define ASSUME_LAST     ASSUME_ERROR

typedef struct {
    dist_type           distance;        // stack distance;
    mod_type            model;           // memory model;
#if defined( _STANDALONE_ )
    lang_type           langtype;        // language;
#endif
    os_type             ostype;          // operating system;
    bool                use32;           // If 32-bit segment is used
    bool                cmdline;
    bool                defUse32;        // default segment size 32-bit
    bool                mseg;            // mixed segments (16/32-bit)
    struct asm_sym      *flat_grp;       // FLAT group symbol
    char                *name;           // name of module
    const FNAME         *srcfile;
} module_info;                           // Information about the module

extern module_info      ModuleInfo;

#define IS_PROC_FAR()   ( ModuleInfo.model == MOD_MEDIUM || ModuleInfo.model == MOD_LARGE || ModuleInfo.model == MOD_HUGE )

extern seg_list         *CurrSeg;       // points to stack of opened segments

/*---------------------------------------------------------------------------*/

extern dir_node         *dir_insert( const char *, int );
extern void             dir_to_sym( dir_node * );
extern void             dir_change( dir_node *, int );
extern void             dir_init( dir_node  *, int );

extern uint_32          GetCurrAddr( void );    // Get offset from current segment

extern dir_node         *GetCurrSeg( void );
/* Get current segment; NULL means none */

extern bool             ExtDef( token_idx, bool );    // define an global or external symbol
extern bool             CommDef( token_idx );         // define an communal symbol
extern bool             PubDef( token_idx );          // define a public symbol
extern bool             GrpDef( token_idx );          // define a group
extern bool             SegDef( token_idx );          // open or close a segment
extern bool             SetCurrSeg( token_idx );      // open or close a segment in
                                                // the second pass
extern bool             ProcDef( token_idx, bool );   // define a procedure
extern bool             ProcEnd( token_idx );         // end a procedure
extern bool             LocalDef( token_idx );        // define local variables to procedure
extern bool             ArgDef( token_idx );          // define arguments in procedure
extern bool             UsesDef( token_idx );         // define used registers in procedure
extern bool             EnumDef( token_idx );         // handles enumerated values
extern bool             Ret( token_idx, token_idx, bool ); // emit return statement from procedure
extern bool             WritePrologue( const char * ); // emit prologue statement after the
                                                // declaration of a procedure
extern bool             GetQueueMacroHidden( void );
extern bool             MacroDef( token_idx, bool );  // define a macro
extern bool             MacroEnd( bool );       // end a macro
extern bool             Startup( token_idx );         // handle .startup & .exit
extern bool             SimSeg( token_idx );          // handle simplified segment
extern bool             Include( token_idx );         // handle an INCLUDE statement
extern bool             IncludeLib( token_idx );      // handle an INCLUDELIB statement
extern bool             Model( token_idx );           // handle .MODEL statement

extern bool             CheckForLang( token_idx, int *lang );

/* Init/fini the information about the module, which are contained in ModuleInfo */
extern void             ModuleInit( void );
extern void             ModuleFini( void );

extern bool             ModuleEnd( token_idx );       // handle END statement

extern bool             Locals( token_idx );          // handle [NO]LOCALS statement

extern bool             ChangeCurrentLocation( bool relative, int_32 value, bool select_data );
extern bool             OrgDirective( token_idx i );
extern bool             AlignDirective( asm_token directive, token_idx i );
extern bool             ForDirective( token_idx, irp_type );

extern void             DefFlatGroup( void );
extern bool             SymIs32( struct asm_sym *sym );

extern bool             directive( token_idx, asm_token );

extern void             ProcStackInit( void );
extern void             ProcStackFini( void );

extern uint_32          GetCurrSegStart(void);
/* Get offset of segment at the start of current LEDATA record */

#define GetSeg( x )     (dir_node *)x->segment

#define SEGISCODE( x )  ( x->seg->e.seginfo->iscode == SEGTYPE_ISCODE )

extern struct asm_sym   *GetGrp( struct asm_sym * );

extern void             AssumeInit( void );     // init all assumed-register table
extern bool             SetAssume( token_idx );       // Assume a register

extern enum assume_reg  GetAssume( struct asm_sym*, enum assume_reg );
/* Return the assumed register of the symbol, and determine the frame and
   frame_datum of its fixup */

extern enum assume_reg  GetPrefixAssume( struct asm_sym*, enum assume_reg );
/* Determine the frame and frame_datum of a symbol with a register prefix */

extern bool             FixOverride( token_idx );
/* Get the correct frame and frame_datum for a label when there is a segment
   or group override. */

extern void             GetSymInfo( struct asm_sym * );
/* Store location information about a symbol */
extern bool             NameDirective( token_idx );

extern bool             Comment( int, token_idx, const char * ); /* handle COMMENT directives */

extern bool             AddAlias( token_idx );
extern void             FreeInfo( dir_node * );
extern void             push( void *stack, void *elt );
extern void             *pop( void *stack );
extern uint_32          GetCurrSegAlign( void );
extern void             wipe_space( char *token );
extern bool             SetUse32Def( bool );

/*---------------------------------------------------------------------------
 *   included from write.c
 *---------------------------------------------------------------------------*/

extern dir_node         *CurrProc;      // current procedure
extern unsigned long    LineNumber;
extern bool             PhaseError;

extern void             FlushCurrSeg( void );
extern const FNAME      *AddFlist( char const *filename );
extern void             OutSelect( bool );
extern void             WriteObjModule( void );
extern void             AddLinnumDataRef( void );

/*---------------------------------------------------------------------------
 *   included from asmline.c
 *---------------------------------------------------------------------------*/

extern const FNAME      *get_curr_srcfile( void );

#endif
