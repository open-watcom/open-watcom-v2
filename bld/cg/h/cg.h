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
* Description:  Common types and constants used by code generator interface.
*
****************************************************************************/


#ifndef _CG_H_INCLUDED
#define _CG_H_INCLUDED

typedef enum {
    CG_SWITCH_SCAN      = 0x0001,
    CG_SWITCH_TABLE     = 0x0002,
    CG_SWITCH_BSEARCH   = 0x0004,
} cg_switch_type;

#define CG_SWITCH_ALL   (CG_SWITCH_SCAN|CG_SWITCH_TABLE|CG_SWITCH_BSEARCH)

typedef enum {
/*  return values for FEAttr() */
    FE_PROC             = 0x00000001,   /* is the symbol a func/procedure */
    FE_STATIC           = 0x00000002,   /* as in the C keyword 'static' */
    FE_GLOBAL           = 0x00000004,   /* ie. import/export */
    FE_IMPORT           = 0x00000008,   /* is it imported? */
    FE_CONSTANT         = 0x00000010,   /* is symbol constant? */
    FE_MEMORY           = 0x00000020,   /* forces symbol to memory */
    FE_VISIBLE          = 0x00000040,   /* is visible outside procedure? */
    FE_NOALIAS          = 0x00000080,   /* name has no aliases (pointers to)*/
    FE_UNIQUE           = 0x00000100,   /* func address has to be unique*/
    FE_COMMON           = 0x00000200,   /* COMDAT/COMDEF symbol */
    FE_ADDR_TAKEN       = 0x00000400,   /* symbol has address taken */
    FE_VOLATILE         = 0x00000800,   /* symbol is volatile */
    FE_INTERNAL         = 0x00001000,   /* symbol is internal */
    FE_ONESEG           = 0x00002000,   /* symbol is less than a segment in size - ie not huge */
    FE_DLLEXPORT        = 0x00004000,   /* symbol is exported */
    FE_DLLIMPORT        = 0x00008000,   /* symbol is imported from DLL */
    FE_VARARGS          = 0x00010000,   /* symbol is a varargs function */
    FE_UNALIGNED        = 0x00020000,   /* no longer used - use CGAlign instead */
    FE_COMPILER         = 0x00040000,   /* compiler generated i.e thunks */
    FE_THREAD_DATA      = 0x00080000,   /* part of a thread-local storage block */
    FE_NAKED            = 0x00100000,   /* naked function - ie no prolog/epilogue */
} fe_attr;

typedef enum {
/*  request values for FEExtName() */
    EXTN_BASENAME,      /* return symbol base name */
    EXTN_PATTERN,       /* return symbol name pattern */
    EXTN_PRMSIZE,       /* return symbol parameters size */
    EXTN_CALLBACKNAME,  /* return callback function name */
} extn_request;

#define UNDEFSEG        (segment_id)-1

typedef char                *char_ptr;
typedef const char          *cchar_ptr;

typedef struct tree_node    *cg_name;           /* retval for CGName(),CGUnary()*/
typedef struct tree_node    *call_handle;       /* retval for CGInitCall(), etc. */
typedef struct symbol       *cg_sym_handle;
typedef struct code_lbl     *label_handle;      /* 2nd parm to CGName for CG_LBL */
typedef struct select_node  *sel_handle;        /* return value for CGSelInit() */
typedef struct bck_info     *back_handle;       /* return value for BENewBack() */
typedef struct aux_info     *aux_handle;        /* first parm to FEAuxInfo (sometimes) */
typedef struct temp_name    *temp_handle;       /* first parm to FEAuxInfo (sometimes) */
typedef struct cfloat       *float_handle;      /* for the BF... routines */
typedef void                *callback_handle;   /* passed to rtn from callback node */
typedef void                *patch_handle;      /* used to stuff patchable ints into tree */

typedef void                (*cg_callback)( callback_handle );

typedef short               segment_id;         /* must be signed */

typedef unsigned_32         cg_linenum;

typedef unsigned            objhandle;

/* symbolic debugging type handles */
typedef unsigned_32         dbg_type;
typedef struct struct_list  *dbg_struct;
typedef struct array_list   *dbg_array;
typedef struct enum_list    *dbg_enum;
typedef struct proc_list    *dbg_proc;
typedef struct name_entry   *dbg_name;
typedef struct location     *dbg_loc;

typedef enum {
    T_DBG_COMPLEX   = 0x47,
    T_DBG_DCOMPLEX  = 0x4f
} dbg_ftn_type;                         /* please forgive me! */


/* operator constants for DB* routine location expressions */
typedef enum {
    DB_OP_POINTS,
    DB_OP_ZEX,
    DB_OP_XCHG,
    DB_OP_MK_FP,
    DB_OP_ADD,
    DB_OP_DUP,
    DB_OP_POP
} dbg_loc_op;

/* field attribute values for debug info */
typedef enum {
    FIELD_ATTR_NONE         = 0x00,
    FIELD_ATTR_INTERNAL     = 0x01,
    FIELD_ATTR_PUBLIC       = 0x02,
    FIELD_ATTR_PROTECTED    = 0x04,
    FIELD_ATTR_PRIVATE      = 0x08
} dbg_field_attr;

typedef enum {
    INHERIT_NONE,
    INHERIT_DBASE,
    INHERIT_VBASE,
    INHERIT_IVBASE
} dbg_inh_attr;

typedef enum {
    METHOD_VANILLA,
    METHOD_STATIC,
    METHOD_FRIEND,
    METHOD_VIRTUAL
} dbg_method_attr;

typedef enum {
    CG_SYM_CONSTANT,
    CG_SYM_VOLATILE,
    CG_SYM_UNALIGNED,
} cg_sym_attr;

typedef enum {
    EXEC            = 0x0001,       /* vs non-executable */
    GLOBAL          = 0x0002,       /* vs local */
    INIT            = 0x0004,       /* vs uninitialized */
    ROM             = 0x0008,       /* read only */
    BACK            = 0x0010,       /* back end may place data here */
    COMMON          = 0x0020,       /* common block data */
    PRIVATE         = 0x0040,       /* private segment */
    GIVEN_NAME      = 0x0080,       /* use the segment name as given */
    COMDAT          = 0x0100,       /* COMDAT segment (ALPHA) */
    THREAD_LOCAL    = 0x0200,       /* Thread Local Storage */
    NOGROUP         = 0x0400        /* Not part of a group (DGROUP) */
} seg_attr;


typedef enum {
    MSG_INFO_FILE,              /* informational message about file (string) */
    MSG_CODE_SIZE,              /* code size message (int) */
    MSG_DATA_SIZE,              /* data size message (int) */
    MSG_ERROR,                  /* cg error message (string) */
    MSG_FATAL,                  /* fatal error  (string) */
    MSG_INFO_PROC,              /* info message about current proc (string) */
    MSG_BAD_PARM_REGISTER,      /* bad "aux" parm      (parm num) */
    MSG_BAD_RETURN_REGISTER,    /* bad "aux" value     (sym) */
    MSG_REGALLOC_DIED,          /* register alloc ran out of mem (sym) */
    MSG_SCOREBOARD_DIED,        /* scoreboard ran out of mem   (sym) */
    MSG_PEEPHOLE_FLUSHED,       /* peep hole optimizer flushed (none) */
    MSG_BACK_END_ERROR,         /* back end error (int) */
    MSG_BAD_SAVE,               /* bad "aux" modify (sym) */
    MSG_WANT_MORE_DATA,         /* back end wants more data space (int) */
    MSG_BLIP,                   /* blip */
    MSG_BAD_LINKAGE,            /* cannot resolve linkage conventions (sym) */
    MSG_SCHEDULER_DIED,         /* ins scheduler ran out of mem (sym) */
    MSG_NO_SEG_REGS,            /* accessing far memory with no seg regs */
    MSG_BAD_PEG_REG,            /* bad register pegged to a segment */
    MSG_SYMBOL_TOO_LONG,        /* symbol too long, truncated (sym) */
    MSG_INFO                    /* general informational message (string) */
} msg_class;

#define DBG_NIL_TYPE    ((dbg_type)0)
#define DBG_FWD_TYPE    ((dbg_type)-1)

typedef union   cg_init_info {
    struct {
        unsigned revision   : 10;
        unsigned target     : 5;
        unsigned is_large   : 1;
    } version;
    int     success;
} cg_init_info;

enum {
    II_TARG_8086,
    II_TARG_80386,
    II_TARG_STUB,
    II_TARG_CHECK,
    II_TARG_370,
    II_TARG_AXP,
    II_TARG_PPC,
    II_TARG_MIPS
};

#define II_REVISION     9

/*
    Front end should do

    cg_init_info    info;

    info = BEInit(...);
    if( info.success == 0 ) {
        // The code generator didn't initialize! terminate (dnpg,dncthd)
    } else {
        if( info.version.revision != II_REVISION ) {
           // crap out --- wrong version of code generator
       }
       // look at info.version.is_large and info.version.target
    }

*/

#endif
