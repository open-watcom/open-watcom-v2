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
* Description:  Define FORTRAN 77 symbol table types.
*
****************************************************************************/


#include "symdefs.h"
#include "symflgs.h"
#include "symtypes.h"
#include "symacc.h"
#include "ifdefs.h"
#include "cg.h"

#define SEG_NULL        0       // NULL segment id
#if _CPU == 8086 || _CPU == 386
  #define SEG_LDATA     1       // local data segment for initialized data
  #define SEG_UDATA     2       // local data segment for uninitialized data
  #define SEG_CDATA     3       // constant and literal data segment
                                // (also BACK segment)
  #define SEG_FREE      4       // first free segment id
#else
  #define SEG_TDATA     1       // Text (code) segment
  #define SEG_LDATA     2       // local data segment for initialized data
  #define SEG_UDATA     3       // local data segment for uninitialized data
  #define SEG_CDATA     4       // constant and literal data segment
                                // (also BACK segment)
  #define SEG_FREE      5       // first free segment id
#endif


#define ALIGN_BYTE      1       // align segment on byte boundary
#define ALIGN_WORD      2       // align segment on word boundary
#define ALIGN_DWORD     4       // align segment on double word boundary
#define ALIGN_QWORD     8       // align segment on quad word boundary
#define ALIGN_SEGMENT   16      // align segment on segment boundary


typedef back_handle     obj_addr;              // back handle

typedef unsigned_32     db_handle;

#include "structin.h"
#include "namelist.h"

// symbol table information for variables:
// =======================================

typedef union vi {
    struct com_eq       *ec_ext;        // common/equivalence extension
    segment_id          seg_id;         //   variables not in common/equivalence
    void                *alt_scb;       // SCB for character arguments
    unsigned short      cg_typ;         // cg-type for local character
} vi;

typedef struct var {
    union {
        struct act_dim_list *dim_ext;   // pointer to dimension extension
        void                *bck_hdl;
    } u;
    union vi            vi;             // variable information
} var;

// symbol table information for common blocks:
// ===========================================

typedef struct common_block {
    sym_id      first;                  // first symbol in common list
    segment_id  seg_id;                 // segment id of common block
} common_block;

// symbol table information for parameter constants:
// =================================================

typedef struct p_constant {
    sym_id       value;                 // point to value (constant or literal)
} p_constant;

// symbol table information for subprograms:
// =========================================

typedef struct subprog {
    union {
        label_id        entry;          // entry label
        signed          imp_segid;      // segment id for external subprograms
    } u;
    void                *alt_scb;       // SCB for character*(*) functions
} subprog;

// symbol table information for intrinsic functions:
// =================================================

typedef struct i_function {
    IFF                 index;          // intrinsic function index
    union {
        int             num_args;       // number of arguments
        signed          imp_segid;      // segment id for intrinsic function
    } u;
} i_function;

// symbol table information for statement functions:
// =================================================

typedef struct sf_header {
    int                 ref_count;      // reference count
    struct sf_parm      *parm_list;     // pointer to argument list
    sym_id              link;           // chain of statement functions
} sf_header;

typedef struct st_function {
    struct sf_header    *header;        // pointer to header
    union {
        label_id        location;       // entry label
        obj_ptr         sequence;       // F-Code sequence
    } u;
} st_function;

// symbol table information for remote blocks:
// ===========================================

typedef struct remote_block {
    int                 ref_count;      // reference count
    label_id            entry;          // entry label
} remote_block;

// symbol table information for magic symbols:
// ===========================================
//
// MSB                                   LSB
// +----+----+----+----+----+----+----+----+
// | 1  |                   | 2  |    3    |
// +----+----+----+----+----+----+----+----+
//
// 1:   Magic bit, always on for magic symbols
// 2:   Localized bit, on if and only if symbol is not passed to a function at
//      any time
// 3:   Class field denoting Class of magic symbol, see definitions below.

#define MAGIC_BIT       0x80            // Must always be on if symbol is magic

#define MAGIC_CLASSMASK 0x83            // Class mask to determine type
#define MAGIC_LABEL     0x80            //      Magic symbol is a Label
#define MAGIC_TEMP      0x81            //      Magic symbol is a temporary
#define MAGIC_STATIC    0x82            //      Magic symbol is a static
#define MAGIC_SHADOW    0x83            //      Magic symbol is a shadow

#define MAGIC_LOCALIZED 0x04            //      Magic symbol is non volatile

// Manipulation macros
#define _MgcIsMagic( sym ) \
        ( sym->u.ns.u2.magic_flags & MAGIC_BIT )

#define _MgcClass( sym ) \
        ( sym->u.ns.u2.magic_flags & MAGIC_CLASSMASK )

#define _MgcSetClass( sym, class ) \
        sym->u.ns.u2.magic_flags = ( MAGIC_BIT | class )

#define _MgcSetLocalTemp( sym ) \
        sym->u.ns.u2.magic_flags |= ( MAGIC_BIT | MAGIC_LOCALIZED )

#define _MgcIsLocalTemp( sym ) \
        ( _MgcIsMagic( sym ) && ( sym->u.ns.u2.magic_flags & MAGIC_LOCALIZED ) )


typedef union tmp_info {
    int                 tmp_index;      // index into tmporary area
    sym_id              stat_off;       // sym_id of static temporary
} tmp_info;

typedef struct m_sym {
    sym_id              sym;            // shadowed symbol
    union {
        intstar4        *value;         // value of implied-DO variables
        unsigned short  cg_typ;         // cg-type for character temporaries
    } u;                                //   and equivalence sets allocated
} m_sym;

// symbol table structure for constants:
// =====================================

typedef struct constant {
    sym_id              link;           // pointer to next constant in chain
    void                *address;       // back handle
    TYPE                typ;            // type of constant
    byte                size;           // size of constant
    ftn_type            value;          // value of constant
} constant;

// symbol table structure for literals:
// ====================================

typedef struct literal {
    sym_id              link;           // pointer to next literal in chain
    void                *address;       // back handle
    uint                length;         // length of literal
    unsigned_8          flags;          // constant appeared in DATA statement
    byte                value;          // value of literal
} literal;

#define LT_DATA_STMT            0x01    // literal used in DATA statement
#define LT_EXEC_STMT            0x02    // literal used in executable statement
#define LT_SCB_TMP_REFERENCE    0x04    // temporary reference to SCB
#define LT_SCB_REQUIRED         0x08    // SCB required

// symbol table structure for statement numbers:
// =============================================

typedef struct stmtno {
    sym_id              link;           // link to next statement # entry
    uint                ref_count;      // reference count
    unsigned_16         block;          // block # statement # appeared in
    unsigned_16         flags;          // statement # flags
    label_id            address;        // label of statement #
    int                 line;           // source line statement # appeared in
    unsigned_16         number;         // statement #
} stmtno;

// union of all symbols:
// =====================

// Notes:
// ======

// 1. A negative value in "name_len" identifies the symbol as a magic symbol.
// 2. The "name_len" field is used to identify the class of magic symbol.
// 3. If _CPU == 8086, the "size" field is a multiple of 16 and the
//    "typ" field is the size modulo 16 for common blocks.
// 4. If _OPT_CG == _ON, the "typ" field contains the segment id of the common
//    block.

typedef struct named_symbol {
    sym_id              link;           // link to next symbol in chain
    unsigned_16         flags;          // symbol flags
    union {
      struct {
        TYPE            typ;            // symbol type
        byte            xflags;         // extra symbol flags
      } s;
      unsigned_16       xsize;          // extra size information for common
    } u1;                               // blocks
    union {
        signed char     name_len;       // length of symbol name
        byte            magic_flags;    // information about the magic symbol
    } u2;
    union {
      obj_addr          address;        // address of symbol
      db_handle         dbh;            // browse handle
    } u3;
    union {
        uint            size;           // size of data type
        struct fstruct  *record;        // pointer to structure definition
        sym_id          sym_record;     // sym_id type pointer to definition
    } xt;
    union {
        struct var          va;         // information for variables
        struct subprog      sp;         // information for subprograms
        struct common_block cb;         // information for common blocks
        struct st_function  sf;         // information for statement functions
        struct remote_block rb;         // information for remote blocks
        struct i_function   fi;         // information for intrinsic functions
        struct p_constant   pc;         // information for parameter constants
        struct m_sym        ms;         // information for magic symbols
    } si;
    char        name[STD_SYMLEN];       // symbol name
} named_symbol;

typedef struct symbol {
    union {
        struct named_symbol ns;         // named symbol
        struct literal      lt;         // literal
        struct constant     cn;         // constant
        struct stmtno       st;         // statement number
        struct fstruct      sd;         // structure definition
        struct field        fd;         // field definition
        struct name_list    nl;         // name list definition
    } u;
} symbol;
