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
* Description:  Data types and functions for directive processing.
*
****************************************************************************/


typedef enum {
    DIROP_INTEGER,
    DIROP_FLOATING,
    DIROP_SYMBOL,
    DIROP_NUMLABEL_REF,     // [0-9][bf]
    DIROP_LINE,
    DIROP_STRING,
    DIROP_REP_INT,
    DIROP_REP_FLT,
    DIROP_ERROR,
} dirop_type;

typedef enum {
    DOF_INT     = 0x001,
    DOF_FLT     = 0x002,
    DOF_SYM     = 0x004,
    DOF_NUMREF  = 0x008,
    DOF_LINE    = 0x010,
    DOF_STR     = 0x020,
    DOF_REP_INT = 0x040,
    DOF_REP_FLT = 0x080,
    DOF_ERROR   = 0x100,
    DOF_NONE    = 0x200,
} dirop_flags;              // what kind of operands can a directive accept

typedef union {
    int_32      integer;
    double      floating;
} number_t;

// Unnamed symbol (numeric labels) uses label_ref field instead of sym
typedef struct {
    union {
        sym_handle      sym;
        int_32          label_ref;
    }                   target;
    asm_reloc_type      type;
    int_32              offset;         // sym +/- offset
} dirop_sym;

typedef struct {
    number_t    num;
    int_32      rep;
} dirop_repeat;

typedef union {
    number_t            number;
    dirop_sym           symbol;
    char                *string;
    dirop_repeat        repeat;
} dirop_data;

typedef struct dir_operand {
    struct dir_operand  *next;
    dirop_type          type;
    dirop_data          content;
} dir_operand;

#define NUMBER_INTEGER( x )     ((x)->content.number.integer)
#define NUMBER_FLOAT( x )       ((x)->content.number.floating)

#define SYMBOL_HANDLE( x )      ((x)->content.symbol.target.sym)
#define SYMBOL_LABEL_REF( x )   ((x)->content.symbol.target.label_ref)
#define SYMBOL_RELOC_TYPE( x )  ((x)->content.symbol.type)
#define SYMBOL_OFFSET( x )      ((x)->content.symbol.offset)

#define STRING_CONTENT( x )     ((x)->content.string)

#define REPEAT_NUMBER( x )      ((x)->content.repeat.num)
#define REPEAT_INTEGER( x )     ((x)->content.repeat.num.integer)
#define REPEAT_FLOAT( x )       ((x)->content.repeat.num.floating)
#define REPEAT_COUNT( x )       ((x)->content.repeat.rep)

typedef uint_8 dir_opcount;

typedef struct directive_t {
    sym_handle  dir_sym;
    dir_opcount num_operands;
    dir_operand *operand_list;
    dir_operand *operand_tail;
} directive_t;

typedef enum {
    DT_NOPARM,
    DT_NOP_NOP,
#ifdef AS_ALPHA
    DT_NOP_FNOP,
#endif
#ifdef _STANDALONE_
    DT_SEC_TEXT = AS_SECTION_TEXT,
    DT_SEC_DATA = AS_SECTION_DATA,
    DT_SEC_BSS = AS_SECTION_BSS,
    DT_SEC_PDATA = AS_SECTION_PDATA,
    DT_SEC_DEBUG_P = AS_SECTION_DEBUG_P,
    DT_SEC_DEBUG_S = AS_SECTION_DEBUG_S,
    DT_SEC_DEBUG_T = AS_SECTION_DEBUG_T,
    DT_SEC_RDATA = AS_SECTION_RDATA,
    DT_SEC_XDATA = AS_SECTION_XDATA,
    DT_SEC_YDATA = AS_SECTION_YDATA,
#ifdef AS_PPC
    DT_SEC_RELDATA = AS_SECTION_RELDATA,
    DT_SEC_TOCD = AS_SECTION_TOCD,
#endif
    DT_USERSEC_NEW,
#endif
    DT_STR_NULL,
    DT_STR_NONULL,
    DT_LNK_GLOBAL,
    DT_LNK_LOCAL,
    DT_VAL_FIRST,
        DT_VAL_INT8 = DT_VAL_FIRST,
        DT_VAL_DOUBLE,
        DT_VAL_FLOAT,
        DT_VAL_INT16,
        DT_VAL_INT32,
        DT_VAL_INT64,
    DT_VAL_LAST = DT_VAL_INT64,
} dir_table_enum;

typedef bool (*dir_func)( directive_t *, dir_table_enum );

typedef struct {
    const char          *name;
    dir_func            func;
    dir_table_enum      parm;
    dirop_flags         flags;
} dir_table;

typedef enum {
    NONE        = 0x0000,
    AT          = 0x0001,
    MACRO       = 0x0002,
    REORDER     = 0x0004,
    VOLATILE    = 0x0008,
    MOVE        = 0x0010,
} dir_set_flags;

extern dir_set_flags AsDirSetOptions;

#define _DirSet( x )    ( AsDirSetOptions |= (x) )
#define _DirUnSet( x )  ( AsDirSetOptions &= ~(x) )
#define _DirIsSet( x )  ( ( AsDirSetOptions & (x) ) != NONE )

extern dir_operand *AsDirOpLine( const char *string );
extern dir_operand *AsDirOpString( const char *string );
extern dir_operand *AsDirOpNumber( expr_tree *expr );
extern dir_operand *AsDirOpIdentifier( asm_reloc_type rtype, sym_handle symbol, expr_tree *offset, int sign );
extern dir_operand *AsDirOpNumLabelRef( asm_reloc_type rtype, int_32 label_ref, expr_tree *offset, int sign );
extern dir_operand *AsDirOpRepeat( expr_tree *expr, expr_tree *repeat );
extern void AsDirOpDestroy( dir_operand *dirop );

extern void AsDirInit( void );
extern void AsDirSetNextScanState( sym_handle sym );
extern bool AsDirGetNextScanState( void );
extern directive_t *AsDirCreate( sym_handle sym );
extern void AsDirAddOperand( directive_t *directive, dir_operand *dirop );
extern void AsDirDestroy( directive_t *directive );
extern bool AsDirParse( directive_t *directive );
extern void AsDirFini( void );

#define DirOpLine                       AsDirOpLine
#define DirOpString                     AsDirOpString
#define DirOpNumber                     AsDirOpNumber
#define DirOpIdentifier                 AsDirOpIdentifier
#define DirOpNumLabelRef                AsDirOpNumLabelRef
#define DirOpRepeat                     AsDirOpRepeat
#define DirOpDestroy                    AsDirOpDestroy

#define DirInit                         AsDirInit
#define DirSetNextScanState             AsDirSetNextScanState
#define DirGetNextScanState             AsDirGetNextScanState
#define DirCreate                       AsDirCreate
#define DirAddOperand                   AsDirAddOperand
#define DirDestroy                      AsDirDestroy
#define DirParse                        AsDirParse
#define DirFini                         AsDirFini
