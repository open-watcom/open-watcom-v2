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
* Description:  Public DWARF Writer library interface.
*
****************************************************************************/


#ifndef DW_H_INCLUDED
#define DW_H_INCLUDED

#include <watcom.h>

#include "dwcnf.h"

/*
    Types
*/


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

/*
    Constants
*/


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
enum {
#define DW_LOC_OP( __n, __v )   DW_LOC_##__n,
#include "dwlocop.h"
#undef DW_LOC_OP
    DW_LOC_breg,
    DW_LOC_max
};


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
} dw_relocs;


/* seek methods for CLISeek */
enum {
    DW_SEEK_SET,
    DW_SEEK_CUR,
    DW_SEEK_END
};


/* the client supplied functions */
typedef struct {
    void                (*cli_reloc)( dw_sectnum, dw_relocs, ... );
    void                (*cli_write)( dw_sectnum, const void *, size_t );
    void                (*cli_seek)( dw_sectnum, long, uint );
    long                (*cli_tell)( dw_sectnum );
    void *              (*cli_alloc)( size_t );
    void                (*cli_free)( void * );
} dw_funcs;

#define DWSetRtns( __name, __reloc, __write, __seek, __tell, __alloc, __free ) \
    struct dw_funcs __name = {  \
        __reloc,                \
        __write,                \
        __seek,                 \
        __tell,                 \
        __alloc,                \
        __free                  \
    }

/* for the compiler_options field */
typedef enum {
    DW_CM_BROWSER               = 0x01,
    DW_CM_DEBUGGER              = 0x02,
    DW_CM_UPPER                 = 0x04,
    DW_CM_LOWER                 = 0x08,
    DW_CM_ABBREV_GEN            = 0x10,
    DW_CM_ABBREV_PRE            = 0x20,
} dw_cm;

typedef struct {
    uint_8              language;
    dw_cm               compiler_options;
    char  const         *producer_name;
    jmp_buf             exception_handler;
    dw_funcs            funcs;
    dw_sym_handle       abbrev_sym;
} dw_init_info;

typedef enum {
    DW_MODEL_NONE    = 0,
    DW_MODEL_FLAT    = 1,
    DW_MODEL_SMALL   = 2,
    DW_MODEL_MEDIUM  = 3,
    DW_MODEL_COMPACT = 4,
    DW_MODEL_LARGE   = 5,
    DW_MODEL_HUGE    = 6,
} dw_model;

/*
    Function Prototypes
*/
#define DWENTRY

/* initialization/finalization */
dw_client       DWENTRY DWInit( const dw_init_info *__info );
void            DWENTRY DWFini( dw_client );

/* compilation units */
typedef struct {
    char const     *source_filename;
    char const     *directory;
    unsigned        flags;
    unsigned        offset_size;
    unsigned        segment_size;
    dw_model        model;
    char const     *inc_list;
    unsigned        inc_list_len;
    dw_sym_handle   dbg_pch;
} dw_cu_info;

dw_handle       DWENTRY DWBeginCompileUnit( dw_client cli, dw_cu_info *cu );
void            DWENTRY DWEndCompileUnit( dw_client );

/* macro definitions */
void            DWENTRY DWMacStartFile( dw_client, dw_linenum __line,
                            char const *__file_name );
void            DWENTRY DWMacEndFile( dw_client );
dw_macro        DWENTRY DWMacDef( dw_client, dw_linenum __line,
                            char const *__macro_name );
void            DWENTRY DWMacParam( dw_client, dw_macro __macro,
                            char const *__parm_name );
void            DWENTRY DWMacFini( dw_client, dw_macro __macro,
                            char const *__definition );
void            DWENTRY DWMacUnDef( dw_client, dw_linenum __line,
                            char const *__macro_name );
void            DWENTRY DWMacUse( dw_client, dw_linenum __line,
                            char const *__macro_name );

/* file/line number management */

extern void DWInitDebugLine( dw_client cli, dw_cu_info *cu );

extern void DWFiniDebugLine( dw_client cli );

void            DWENTRY DWSetFile( dw_client, char const *__fname );
/* line number information */
enum {
    DW_LN_DEFAULT               = 0x00,
    DW_LN_STMT                  = 0x01,
    DW_LN_BLK                   = 0x02
};
void            DWENTRY DWLineNum( dw_client, uint __info,
                            dw_linenum __line_num, dw_column __column,
                            dw_addr_offset __addr );
void DWLineAddr(  dw_client  cli, dw_sym_handle sym, dw_addr_offset addr );
void            DWLineSeg(  dw_client  cli, dw_sym_handle sym );
void            DWENTRY DWDeclFile( dw_client, char const *__fname );
void            DWENTRY DWDeclPos( dw_client, dw_linenum, dw_column );

unsigned        DWENTRY DWLineGen( dw_linenum_delta, dw_addr_delta, uint_8 * );

/* reference declarations */
void            DWENTRY DWReference( dw_client, dw_linenum,
                            dw_column, dw_handle );

/* Location expressions */
dw_loc_id       DWENTRY DWLocInit( dw_client );
dw_loc_label    DWENTRY DWLocNewLabel( dw_client, dw_loc_id __loc );
void            DWENTRY DWLocSetLabel( dw_client, dw_loc_id __loc,
                            dw_loc_label __label );
void            DWENTRY DWLocReg( dw_client, dw_loc_id __loc,
                            uint __reg );
void            DWENTRY DWLocPiece( dw_client, dw_loc_id __loc,
                            uint __size );
void            DWENTRY DWLocStatic( dw_client, dw_loc_id __loc,
                            dw_sym_handle __sym );
void            DWENTRY DWLocSym( dw_client, dw_loc_id,
                            dw_sym_handle, dw_relocs );
void            DWENTRY DWLocSegment( dw_client, dw_loc_id __loc,
                            dw_sym_handle __sym );
void            DWENTRY DWLocConstU( dw_client, dw_loc_id __loc,
                            dw_uconst __constant_value );
void            DWENTRY DWLocConstS( dw_client, dw_loc_id __loc,
                            dw_sconst __constant_value );
void            DWENTRY DWLocOp0( dw_client, dw_loc_id __loc,
                            uint __op );
void            DWENTRY DWLocOp( dw_client, dw_loc_id __loc,
                            uint __op, ... );
dw_loc_handle   DWENTRY DWLocFini( dw_client, dw_loc_id __loc );

/* Location Lists */
dw_list_id      DWENTRY DWListInit( dw_client );
void            DWENTRY DWListEntry( dw_client, dw_list_id __list,
                            dw_sym_handle __beg, dw_sym_handle __end,
                            dw_loc_handle __location_expr );
void            DWENTRY DWListEntryOut(
                      dw_client                   cli,
                      dw_list_id                  id,
                      dw_sym_handle               begin,
                      dw_sym_handle               end,
                      dw_loc_handle               loc );
dw_loc_handle   DWENTRY DWListFini( dw_client, dw_list_id __list );
void            DWENTRY DWLocTrash( dw_client, dw_loc_handle __loc );
/* debug info handle refs */
dw_handle DWENTRY DWHandle( dw_client cli, uint kind );
void DWENTRY DWHandleSet( dw_client cli, dw_handle set_hdl );

/* typing information */
dw_handle       DWENTRY DWFundamental( dw_client, char const * __name,
                            unsigned __fund_idx, unsigned __size );
dw_handle       DWENTRY DWModifier( dw_client, dw_handle __base_type,
                            uint __modifiers );
dw_handle       DWENTRY DWTypedef( dw_client, dw_handle __base_type,
                            char const *__name, dw_addr_offset __scope,
                            uint __flags );
dw_handle       DWENTRY DWPointer( dw_client, dw_handle __base_type,
                            uint __flags );
dw_handle       DWENTRY DWBasedPointer(  dw_client       cli,
                                         dw_handle       base_type,
                                         dw_loc_handle   seg,
                                         uint            flags );
dw_handle       DWENTRY DWString( dw_client, dw_loc_handle __string_length,
                            dw_size_t __byte_size, char const *__name,
                            dw_addr_offset __scope, uint __flags );
dw_handle       DWENTRY DWMemberPointer( dw_client,
                            dw_handle __containing_struct,
                            dw_loc_handle __use_location, dw_handle __base_type,
                            char const *__name, unsigned __flags );

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

dw_handle DWENTRY DWSimpleArray( dw_client cli, dw_handle elt_type, int elt_count );
dw_handle       DWENTRY DWBeginArray( dw_client, dw_handle __elt_type,
                            uint __stride_size, char const *__name,
                            dw_addr_offset __scope, uint __flags );
void            DWENTRY DWArrayDimension( dw_client, const dw_dim_info *__info);
void            DWENTRY DWArrayVarDim( dw_client, const dw_vardim_info * );
void            DWENTRY DWEndArray( dw_client );

/* structure types */
typedef enum {                  // Kind of Structure
    DW_ST_NONE,
    DW_ST_CLASS,
    DW_ST_STRUCT,
    DW_ST_UNION,
} dw_st;

dw_handle       DWENTRY DWStruct( dw_client, uint __kind );
void            DWENTRY DWBeginStruct( dw_client, dw_handle __struct_hdl,
                            dw_size_t __size, char const *__name,
                            dw_addr_offset __scope, uint __flags );
void            DWENTRY DWAddFriend( dw_client, dw_handle __friend );
dw_handle       DWENTRY DWAddInheritance( dw_client, dw_handle __ancestor,
                            dw_loc_handle __loc, uint __flags );
dw_handle       DWENTRY DWAddField( dw_client, dw_handle __field_hdl,
                            dw_loc_handle __loc, char const *__name,
                            uint __flags );
dw_handle       DWENTRY DWAddBitField( dw_client, dw_handle __field_hdl,
                            dw_loc_handle __loc, dw_size_t __byte_size,
                            uint __bit_offset, uint __bit_size,
                            char const *__name, uint __flags );
void            DWENTRY DWEndStruct( dw_client );


/* enumeration types */
dw_handle       DWENTRY DWBeginEnumeration( dw_client, dw_size_t __byte_size,
                            char const *__name, dw_addr_offset __scope,
                            uint __flags );
void            DWENTRY DWAddConstant( dw_client, dw_uconst __constant_value,
                            char const *__name );
void            DWENTRY DWEndEnumeration( dw_client );


/* subroutine types */
dw_handle       DWENTRY DWBeginSubroutineType( dw_client,
                            dw_handle __return_type, char const *__name,
                            dw_addr_offset __scope, uint __flags );
dw_handle       DWENTRY DWAddParmToSubroutineType( dw_client,
                            dw_handle __parm_type, dw_loc_handle __loc,
                            dw_loc_handle __seg, char const *__name );
dw_handle       DWENTRY DWAddEllipsisToSubroutineType( dw_client );
void            DWENTRY DWEndSubroutineType( dw_client );


/* namespace      */
dw_handle DWENTRY DWBeginNameSpace( dw_client cli, const char *name );
void DWENTRY DWEndNameSpace( dw_client cli );
/* lexical blocks */
dw_handle       DWENTRY DWBeginLexicalBlock( dw_client,
                            dw_loc_handle __aseg, char const *__name );
void            DWENTRY DWEndLexicalBlock( dw_client );


/* common blocks */
dw_handle       DWENTRY DWBeginCommonBlock( dw_client, dw_loc_handle __loc,
                            dw_loc_handle __aseg, char const *__name,
                            unsigned __flags );
void            DWENTRY DWEndCommonBlock( dw_client );
dw_handle       DWENTRY DWIncludeCommonBlock( dw_client,
                            dw_handle __common_block );

/* subroutines */
dw_handle       DWENTRY DWBeginInlineSubroutine( dw_client,
                            dw_handle __subr, dw_loc_handle __ret_addr_loc,
                            dw_loc_handle __aseg );
dw_handle       DWENTRY DWBeginSubroutine( dw_client, uint call_type,
                            dw_handle __return_type,
                            dw_loc_handle __return_addr_loc,
                            dw_loc_handle __frame_base_loc,
                            dw_loc_handle __structure_loc,
                            dw_handle __member_hdl, dw_loc_handle __aseg,
                            char const * __name, dw_addr_offset __start_scope,
                            uint __flags );
dw_handle DWENTRY DWBeginEntryPoint(
    dw_client                   cli,
    dw_handle                   return_type,
    dw_loc_handle               return_addr_loc,
    dw_loc_handle               segment,
    char const *                name,
    dw_addr_offset              start_scope,
    uint                        flags );

dw_handle DWENTRY DWBeginMemFuncDecl(
    dw_client                   cli,
    dw_handle                   return_type,
    dw_loc_handle               segment,
    dw_loc_handle               loc,
    char const *                name,
    uint                        flags );
dw_handle DWENTRY DWBeginVirtMemFuncDecl(
    dw_client                   cli,
    dw_handle                   return_type,
    dw_loc_handle               vtable_loc,
    char const *                name,
    uint                        flags );

void            DWENTRY DWEndSubroutine( dw_client );
enum {          // Kind of format for default value
    DW_DEFAULT_NONE,
    DW_DEFAULT_FUNCTION,
    DW_DEFAULT_STRING,
    DW_DEFAULT_BLOCK
};
dw_handle       DWENTRY DWFormalParameter( dw_client, dw_handle __parm_type,
                            dw_loc_handle __parm_loc, dw_loc_handle __aseg,
                            char const *__name, uint __default_value_type,
                            ... );
dw_handle       DWENTRY DWEllipsis( dw_client );
dw_handle       DWENTRY DWLabel( dw_client, dw_loc_handle __aseg,
                            char const *__name, dw_addr_offset __start_scope );
dw_handle       DWENTRY DWVariable( dw_client, dw_handle __type,
                            dw_loc_handle __loc, dw_handle __member_of,
                            dw_loc_handle __aseg, char const *__name,
                            dw_addr_offset __start_scope, uint __flags );
dw_handle       DWENTRY DWConstant( dw_client, dw_handle __type,
                            const void *__constant_value, size_t __len,
                            dw_handle __member_of, char const * __name,
                            dw_addr_offset __start_scope, uint __flags );
/* name list      */
dw_handle       DWENTRY DWNameListBegin( dw_client cli, char const *name );
void            DWENTRY DWNameListItem( dw_client cli, dw_handle ref );
void            DWENTRY DWEndNameList( dw_client cli );
/* address ranges */
void            DWENTRY DWAddress( dw_client, uint_32 );

/* public names */
void            DWENTRY DWPubname( dw_client, dw_handle __hdl,
                            char const *__name );
/* util used for PCH */
uint_32         DWENTRY DWDebugRefOffset( dw_client cli, dw_handle hdl );
dw_handle       DWENTRY DWRefPCH( dw_client cli, uint_32 ref );

#endif
