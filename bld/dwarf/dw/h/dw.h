/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Public DWARF Writer library interface.
*
****************************************************************************/


#ifndef DW_H_INCLUDED
#define DW_H_INCLUDED

#include <setjmp.h>
#include "watcom.h"


/************************
 * Macros
 ************************/

#define DWENTRY

/* max storage req'd in LEB128 form to store a 32 bit int/uint */
#define MAX_LEB128      5

#define GET_SEGMENT_SIZE(c)     (((c)->segment_size == 0) ? 2 : (c)->segment_size)

#define DWSetRtns( __name, __reloc, __write, __seek, __tell, __alloc, __free ) \
    dw_funcs __name = { __reloc, __write, __seek, __tell, __alloc, __free }


/************************
 * Enumeration constants
 ************************/

/* language constants */
enum {
    DWLANG_C,
    DWLANG_CPP,
    DWLANG_FORTRAN
};

/* procedure call type */
enum {
    DW_SB_NEAR_CALL,
    DW_SB_FAR_CALL,
    DW_SB_FAR16_CALL
};

/* fundamental types */
enum {
    DW_FT_MIN = 1,
    DW_FT_ADDRESS = DW_FT_MIN,
    DW_FT_BOOLEAN,
    DW_FT_COMPLEX_FLOAT,
    DW_FT_FLOAT,
    DW_FT_SIGNED,
    DW_FT_SIGNED_CHAR,
    DW_FT_UNSIGNED,
    DW_FT_UNSIGNED_CHAR,
    DW_FT_MAX
};

/* type modifiers */
enum {
    DW_MOD_CONSTANT         = 0x10,
    DW_MOD_VOLATILE         = 0x20,
    DW_MOD_NEAR16           = 0x01,
    DW_MOD_FAR16            = 0x02,
    DW_MOD_HUGE16           = 0x03,
    DW_MOD_NEAR32           = 0x04,
    DW_MOD_FAR32            = 0x05
};

/* various flags for the generic 'flags' parameter to many functions */
enum {
    /* set to indicate die is a declaration, not a definition */

    DW_FLAG_DECLARATION         = 0x8000, // declaration, not definition

    /* accessibility flags */
    DW_FLAG_ACCESS_MASK         = 0x0003,
    DW_FLAG_PRIVATE             = 0x0001,
    DW_FLAG_PROTECTED           = 0x0002,
    DW_FLAG_PUBLIC              = 0x0003,
    /* "pointer types" used in DWPointer and DWBeginSubroutine[Type] */
    DW_PTR_TYPE_MASK            = 0x7000,
    DW_PTR_TYPE_SHIFT           = 12,
    DW_PTR_TYPE_DEFAULT         = 0x0000,
    DW_PTR_TYPE_NEAR16          = 0x1000,
    DW_PTR_TYPE_FAR16           = 0x2000,
    DW_PTR_TYPE_HUGE16          = 0x3000,
    DW_PTR_TYPE_NEAR32          = 0x4000,
    DW_PTR_TYPE_FAR32           = 0x5000,

    /* extra flags for DWPointer */
    DW_FLAG_REFERENCE           = 0x0008,

    /* extra flags for DWAddField */
    DW_FLAG_STATIC              = 0x0008,

    /* for DWAddInheritance and DWBeginSubroutine */
    DW_FLAG_VIRTUAL             = 0x0020,

    /* for DWBeginSubroutineType and DWBeginSubroutine */
    DW_FLAG_PROTOTYPED          = 0x0010,

    /* for DWBeginSubroutine */
    DW_FLAG_ARTIFICIAL          = 0x0004,
    DW_FLAG_VIRTUAL_MASK        = 0x0060,
    DW_FLAG_VIRTUAL_SHIFT       = 5,
    DW_FLAG_PURE_VIRTUAL        = 0x0040,
    DW_FLAG_MAIN                = 0x0080,
    DW_SUB_STATIC               = 0x0100,
    DW_FLAG_INLINE_MASK         = 0x0c00,
    DW_FLAG_INLINE_SHIFT        = 10,
    DW_FLAG_WAS_INLINED         = 0x0400,
    DW_FLAG_DECLARED_INLINE     = 0x0800,

    /* for DWVariable and DWConstant */
    DW_FLAG_GLOBAL              = 0x0008
};

/* operation codes for location expressions */
typedef enum {
    #define DW_LOC_OP( __n, __v )   DW_LOC_##__n,
    #include "dwlocop.h"
    #undef DW_LOC_OP
    DW_LOC_breg,
    DW_LOC_max
} dw_loc_op;

/* the debugging sections */
typedef enum dw_sectnum{
    DW_DEBUG_INFO,
    DW_DEBUG_PUBNAMES,
    DW_DEBUG_ARANGES,
    DW_DEBUG_LINE,
    DW_DEBUG_LOC,
    DW_DEBUG_ABBREV,
    DW_DEBUG_MACINFO,
    DW_DEBUG_STR,
    DW_DEBUG_REF,
    DW_DEBUG_MAX
} dw_sectnum;

/* relocation types for CLIReloc */
typedef enum {
    DW_W_LOW_PC,
    DW_W_HIGH_PC,
    DW_W_STATIC,
    DW_W_SEGMENT,
    DW_W_LABEL,
    DW_W_LABEL_SEG,
    DW_W_SECTION_POS,
    DW_W_DEFAULT_FUNCTION,
    DW_W_ARANGE_ADDR,
    DW_W_UNIT_SIZE,
    DW_W_LOC_RANGE,
    DW_W_EXT_REF,
    DW_W_MAX
} dw_reloc_type;

/* seek methods for CLISeek */
enum {
    DW_SEEK_SET,
    DW_SEEK_CUR,
    DW_SEEK_END
};

/* structure types */
typedef enum {                  // Kind of Structure
    DW_ST_NONE,
    DW_ST_CLASS,
    DW_ST_STRUCT,
    DW_ST_UNION,
} dw_struct_type;

/* for the compiler_options field */
typedef enum {
    DW_CM_BROWSER               = 0x01,
    DW_CM_DEBUGGER              = 0x02,
    DW_CM_UPPER                 = 0x04,
    DW_CM_LOWER                 = 0x08,
    DW_CM_ABBREV_GEN            = 0x10,
    DW_CM_ABBREV_PRE            = 0x20,
} dw_cm;

typedef enum {
    DW_MODEL_NONE    = 0,
    DW_MODEL_FLAT    = 1,
    DW_MODEL_SMALL   = 2,
    DW_MODEL_MEDIUM  = 3,
    DW_MODEL_COMPACT = 4,
    DW_MODEL_LARGE   = 5,
    DW_MODEL_HUGE    = 6,
} dw_model;

/* line number information */
enum {
    DW_LN_DEFAULT               = 0x00,
    DW_LN_STMT                  = 0x01,
    DW_LN_BLK                   = 0x02
};

enum {          // Kind of format for default value
    DW_DEFAULT_NONE,
    DW_DEFAULT_FUNCTION,
    DW_DEFAULT_STRING,
    DW_DEFAULT_BLOCK
};

/************************
 * Types
 ************************/

/*
 * dw_sym_handle has a client defined meaning; the DW library will pass these
 * back to the client in CLIRelocs for DW_W_STATIC
 */
typedef void            *dw_sym_handle;

/*
 * the contents of dw_targ_addr is unimportant to the DW library; it is
 * only used for sizeof( dw_targ_addr ).
 */
typedef uint_32         dw_targ_addr;
typedef uint_16         dw_targ_seg;

/*
 * dw_addr_offset is the type used for offsets from some base address.
 * Such as the start_scope parameter to typing routines, or the addr
 * parameter to line number information.
 * The code assumes this is an unsigned integer type.
 */
typedef uint_32         dw_addr_offset;

/*
 * dw_addr_delta is an integer type that can hold the largest possible
 * difference between the addr parameter for two subsequent calls to
 * DWLineNum.
 */
typedef int_32          dw_addr_delta;

/*
 * dw_linenum is a line number.  It must be an unsigned integer type.
 * dw_linenum_delta is a type that can hold the largest possible
 * difference between two adjacent line numbers passed to DWLineNum.
 */
typedef uint_32         dw_linenum;
typedef int_32          dw_linenum_delta;

/*
 * dw_column is a column number.  It must be an unsigned integer type.
 */
typedef uint_16         dw_column;
typedef int_16          dw_column_delta;

/*
 * dw_size_t is used for sizes of various things such as block constants
 * (i.e. for DWAddEnumerationConstant)
 */
typedef uint_32         dw_size_t;

/*
 * dw_uconst is an unsigned integer constant of maximum size
 * dw_sconst is a signed integer constant of maximum size
 */
typedef uint_32         dw_uconst;
typedef int_32          dw_sconst;

/* The client id; required by all DW calls except DWInit() */
typedef struct dw_client        *dw_client;

/* used for macro definitions */
typedef struct dw_macro         *dw_macro;

/* temporary value that is used to build location expressions */
typedef struct dw_loc_id        *dw_loc_id;

/* a label within a location expression */
typedef struct dw_loc_label     *dw_loc_label;

/* a temporary value used to build location lists */
typedef struct dw_list_id       *dw_list_id;

/* a 'compiled' location expression or list */
typedef struct dw_loc_handle    *dw_loc_handle;

/* an id for a debugging record */
typedef uint_32                 dw_handle;
typedef uint_16                 dw_defseg; // default size of a seg

/* output data offset */
typedef uint_32                 dw_out_offset;

typedef uint_32                 dw_sect_offs;

/* the client supplied functions */
typedef struct {
    void                (*cli_reloc)( dw_sectnum, dw_reloc_type, ... );
    void                (*cli_write)( dw_sectnum, const void *, size_t );
    void                (*cli_seek)( dw_sectnum, dw_out_offset, int );
    dw_out_offset       (*cli_tell)( dw_sectnum );
    void *              (*cli_alloc)( size_t );
    void                (*cli_free)( void * );
} dw_funcs;

typedef struct {
    uint_8              language;
    dw_cm               compiler_options;
    char  const         *producer_name;
    jmp_buf             exception_handler;
    dw_funcs            funcs;
    dw_sym_handle       abbrev_sym;
} dw_init_info;

/* compilation units */
typedef struct {
    char const      *source_filename;
    char const      *directory;
    unsigned        flags;
    unsigned        offset_size;
    unsigned        segment_size;
    dw_model        model;
    char const      *inc_list;
    size_t          inc_list_len;
    dw_sym_handle   dbg_pch;
} dw_cu_info;

/* array types */

typedef struct {
    dw_handle   index_type;
    dw_uconst   lo_data;
    dw_uconst   hi_data;
} dw_dim_info;

typedef struct {
    dw_handle   index_type;
    dw_handle   lo_data;
    dw_handle   count_data;
} dw_vardim_info;

/************************
 * Function Prototypes
 ************************/

/* initialization/finalization */
extern dw_client    DWENTRY DWInit( const dw_init_info *__info );
extern void         DWENTRY DWFini( dw_client );
extern dw_handle    DWENTRY DWBeginCompileUnit( dw_client cli, dw_cu_info *cu );
extern void         DWENTRY DWEndCompileUnit( dw_client );

/* macro definitions */
extern void         DWENTRY DWMacStartFile( dw_client, dw_linenum __line,
                                char const *__file_name );
extern void         DWENTRY DWMacEndFile( dw_client );
extern dw_macro     DWENTRY DWMacDef( dw_client, dw_linenum __line, char const *__macro_name );
extern void         DWENTRY DWMacParam( dw_client, dw_macro __macro, char const *__parm_name );
extern void         DWENTRY DWMacFini( dw_client, dw_macro __macro, char const *__definition );
extern void         DWENTRY DWMacUnDef( dw_client, dw_linenum __line, char const *__macro_name );
extern void         DWENTRY DWMacUse( dw_client, dw_linenum __line, char const *__macro_name );

/* file/line number management */
extern void         DWInitDebugLine( dw_client cli, dw_cu_info *cu );
extern void         DWFiniDebugLine( dw_client cli );
extern void         DWENTRY DWSetFile( dw_client, char const *__fname );
extern void         DWENTRY DWLineNum( dw_client, uint __info, dw_linenum __line_num,
                                dw_column __column, dw_addr_offset __addr );
extern void         DWLineAddr( dw_client  cli, dw_sym_handle sym, dw_addr_offset addr );
extern void         DWLineSeg( dw_client  cli, dw_sym_handle sym );
extern void         DWENTRY DWDeclFile( dw_client, char const *__fname );
extern void         DWENTRY DWDeclPos( dw_client, dw_linenum, dw_column );

/* reference declarations */
extern void         DWENTRY DWReference( dw_client, dw_linenum, dw_column, dw_handle );

/* Location expressions */
extern dw_loc_id    DWENTRY DWLocInit( dw_client );
extern dw_loc_label DWENTRY DWLocNewLabel( dw_client, dw_loc_id __loc );
extern void         DWENTRY DWLocSetLabel( dw_client, dw_loc_id __loc, dw_loc_label __label );
extern void         DWENTRY DWLocReg( dw_client, dw_loc_id __loc, uint __reg );
extern void         DWENTRY DWLocPiece( dw_client, dw_loc_id __loc, uint __size );
extern void         DWENTRY DWLocStatic( dw_client, dw_loc_id __loc, dw_sym_handle __sym );
extern void         DWENTRY DWLocSym( dw_client, dw_loc_id, dw_sym_handle, dw_reloc_type );
extern void         DWENTRY DWLocSegment( dw_client, dw_loc_id __loc, dw_sym_handle __sym );
extern void         DWENTRY DWLocConstU( dw_client, dw_loc_id __loc, dw_uconst __constant_value );
extern void         DWENTRY DWLocConstS( dw_client, dw_loc_id __loc, dw_sconst __constant_value );
extern void         DWENTRY DWLocOp0( dw_client, dw_loc_id __loc, dw_loc_op __op );
extern void         DWENTRY DWLocOp( dw_client, dw_loc_id __loc, dw_loc_op __op, ... );
extern dw_loc_handle DWENTRY DWLocFini( dw_client, dw_loc_id __loc );

/* Location Lists */
extern dw_list_id   DWENTRY DWListInit( dw_client );
extern void         DWENTRY DWListEntry( dw_client, dw_list_id __list, dw_sym_handle __beg,
                                dw_sym_handle __end, dw_loc_handle __location_expr );
extern void         DWENTRY DWListEntryOut( dw_client cli, dw_list_id id,
                                dw_sym_handle begin, dw_sym_handle end, dw_loc_handle loc );
extern dw_loc_handle DWENTRY DWListFini( dw_client, dw_list_id __list );
extern void         DWENTRY DWLocTrash( dw_client, dw_loc_handle __loc );

/* debug info handle refs */
extern dw_handle    DWENTRY DWHandle( dw_client cli, dw_struct_type kind );
extern void         DWENTRY DWHandleSet( dw_client cli, dw_handle set_hdl );

/* typing information */
extern dw_handle    DWENTRY DWFundamental( dw_client, char const * __name, unsigned __fund_idx, unsigned __size );
extern dw_handle    DWENTRY DWModifier( dw_client, dw_handle __base_type, uint __modifiers );
extern dw_handle    DWENTRY DWTypedef( dw_client, dw_handle __base_type, char const *__name,
                                dw_addr_offset __scope, uint __flags );
extern dw_handle    DWENTRY DWPointer( dw_client, dw_handle __base_type, uint __flags );
extern dw_handle    DWENTRY DWBasedPointer( dw_client cli, dw_handle base_type, dw_loc_handle seg, uint flags );
extern dw_handle    DWENTRY DWString( dw_client, dw_loc_handle __string_length,
                                dw_size_t __byte_size, char const *__name,
                                dw_addr_offset __scope, uint __flags );
extern dw_handle    DWENTRY DWMemberPointer( dw_client, dw_handle __containing_struct,
                                dw_loc_handle __use_location, dw_handle __base_type,
                                char const *__name, unsigned __flags );
extern dw_handle    DWENTRY DWSimpleArray( dw_client cli, dw_handle elt_type, int elt_count );
extern dw_handle    DWENTRY DWBeginArray( dw_client, dw_handle __elt_type,
                                uint_32 __stride_size, char const *__name,
                                dw_addr_offset __scope, uint __flags );
extern void         DWENTRY DWArrayDimension( dw_client, const dw_dim_info *__info);
extern void         DWENTRY DWArrayVarDim( dw_client, const dw_vardim_info * );
extern void         DWENTRY DWEndArray( dw_client );
extern dw_handle    DWENTRY DWStruct( dw_client, dw_struct_type __kind );
extern void         DWENTRY DWBeginStruct( dw_client, dw_handle __struct_hdl,
                                dw_size_t __size, char const *__name,
                                dw_addr_offset __scope, uint __flags );
extern void         DWENTRY DWAddFriend( dw_client, dw_handle __friend );
extern dw_handle    DWENTRY DWAddInheritance( dw_client, dw_handle __ancestor,
                                dw_loc_handle __loc, uint __flags );
extern dw_handle    DWENTRY DWAddField( dw_client, dw_handle __field_hdl, dw_loc_handle __loc,
                                char const *__name, uint __flags );
extern dw_handle    DWENTRY DWAddBitField( dw_client, dw_handle __field_hdl,
                                dw_loc_handle __loc, dw_size_t __byte_size,
                                uint __bit_offset, uint __bit_size,
                                char const *__name, uint __flags );
extern void         DWENTRY DWEndStruct( dw_client );

/* enumeration types */
extern dw_handle    DWENTRY DWBeginEnumeration( dw_client, dw_size_t __byte_size,
                                char const *__name, dw_addr_offset __scope, uint __flags );
extern void         DWENTRY DWAddEnumerationConstant( dw_client, dw_uconst __constant_value,
                                                         char const *__name );
extern void         DWENTRY DWEndEnumeration( dw_client );

/* subroutine types */
extern dw_handle    DWENTRY DWBeginSubroutineType( dw_client, dw_handle __return_type,
                                char const *__name, dw_addr_offset __scope, uint __flags );
extern dw_handle    DWENTRY DWAddParmToSubroutineType( dw_client, dw_handle __parm_type,
                                dw_loc_handle __loc, dw_loc_handle __seg, char const *__name );
extern dw_handle    DWENTRY DWAddEllipsisToSubroutineType( dw_client );
extern void         DWENTRY DWEndSubroutineType( dw_client );

/* namespace      */
extern dw_handle    DWENTRY DWBeginNameSpace( dw_client cli, const char *name );
extern void         DWENTRY DWEndNameSpace( dw_client cli );

/* lexical blocks */
extern dw_handle    DWENTRY DWBeginLexicalBlock( dw_client, dw_loc_handle __aseg,
                                char const *__name );
extern void         DWENTRY DWEndLexicalBlock( dw_client );

/* common blocks */
extern dw_handle    DWENTRY DWBeginCommonBlock( dw_client, dw_loc_handle __loc,
                                dw_loc_handle __aseg, char const *__name, unsigned __flags );
extern void         DWENTRY DWEndCommonBlock( dw_client );
extern dw_handle    DWENTRY DWIncludeCommonBlock( dw_client, dw_handle __common_block );

/* subroutines */
extern dw_handle    DWENTRY DWBeginInlineSubroutine( dw_client, dw_handle __subr,
                                dw_loc_handle __ret_addr_loc, dw_loc_handle __aseg );
extern dw_handle    DWENTRY DWBeginSubroutine( dw_client, uint call_type,
                                dw_handle __return_type, dw_loc_handle __return_addr_loc,
                                dw_loc_handle __frame_base_loc, dw_loc_handle __structure_loc,
                                dw_handle __member_hdl, dw_loc_handle __aseg,
                                char const * __name, dw_addr_offset __start_scope,
                                uint __flags );
extern dw_handle    DWENTRY DWBeginEntryPoint( dw_client cli, dw_handle return_type,
                                dw_loc_handle return_addr_loc, dw_loc_handle segment,
                                char const *name, dw_addr_offset start_scope, uint flags );
extern dw_handle    DWENTRY DWBeginMemFuncDecl( dw_client cli, dw_handle return_type,
                                dw_loc_handle segment, dw_loc_handle loc, char const *name,
                                uint flags );
extern dw_handle    DWENTRY DWBeginVirtMemFuncDecl( dw_client cli, dw_handle return_type,
                                dw_loc_handle vtable_loc, char const *name, uint flags );
extern void         DWENTRY DWEndSubroutine( dw_client );
extern dw_handle    DWENTRY DWFormalParameter( dw_client, dw_handle __parm_type,
                                dw_loc_handle __parm_loc, dw_loc_handle __aseg,
                                char const *__name, uint __default_value_type, ... );
extern dw_handle    DWENTRY DWEllipsis( dw_client );
extern dw_handle    DWENTRY DWLabel( dw_client, dw_loc_handle __aseg, char const *__name,
                                dw_addr_offset __start_scope );
extern dw_handle    DWENTRY DWVariable( dw_client, dw_handle __type, dw_loc_handle __loc,
                                dw_handle __member_of, dw_loc_handle __aseg,
                                char const *__name, dw_addr_offset __start_scope,
                                uint __flags );
extern dw_handle    DWENTRY DWConstant( dw_client, dw_handle __type,
                                const void *__constant_value, size_t __len,
                                dw_handle __member_of, char const * __name,
                                dw_addr_offset __start_scope, uint __flags );

/* name list      */
extern dw_handle    DWENTRY DWNameListBegin( dw_client cli, char const *name );
extern void         DWENTRY DWNameListItem( dw_client cli, dw_handle ref );
extern void         DWENTRY DWEndNameList( dw_client cli );

/* address ranges */
extern void         DWENTRY DWAddress( dw_client, uint_32 );

/* public names */
extern void         DWENTRY DWPubname( dw_client, dw_handle __hdl, char const *__name );

/* functions used for PCH */
extern dw_sect_offs DWENTRY DWGetHandleLocation( dw_client cli, dw_handle hdl );
extern dw_handle    DWENTRY DWRefPCH( dw_client cli, uint_32 ref );
extern uint_8       * DWENTRY DWLineGen( dw_linenum_delta line_incr,
                                dw_addr_delta addr_incr, uint_8 *end );

#endif
