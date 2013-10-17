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


#ifndef __CGBACKUT_H__
#define __CGBACKUT_H__

// CGBACKUT.H -- internal (to C++ back-end) definitions
//
// 93/09/09 -- J.W.Welch    -- defined

#include "typesig.h"
#include "codegen.h"
#include "cgaux.h"
#include "conpool.h"
#include "cgio.h"
#include "vstk.h"
#include "stringl.h"

typedef union  se     SE;
typedef struct se_def SE_DEF;
typedef struct se_ref SE_REF;
typedef struct cond_label COND_LABEL;

enum {
    DSC_CONST           = 0x01,
    DSC_CODE_OK         = 0x02,
    DSC_NULL            = 0x00
};

typedef enum                    // flags controlling grouped destruction
{   DGRP_DIRECT     = 0x0001    // - TRUE ==> direct destruction
,   DGRP_TEMPS      = 0x0002    // - TRUE ==> generate DTOR of temps
,   DGRP_COMPS      = 0x0004    // - TRUE ==> generate DTOR of components
,   DGRP_CTOR       = 0x0008    // - TRUE ==> dtor of ctor
,   DGRP_COUNT      = 0x2000    // - TRUE ==> count the destructions
                                // flags used internally
,   DGRP_VTEST      = 0x0010    // - TRUE ==> virtual test done
,   DGRP_LIVE       = 0x0020    // - TRUE ==> live code
,   DGRP_DONE       = 0x0040    // - TRUE ==> CGDone for expression
,   DGRP_TRASH      = 0x0080    // - TRUE ==> CGTrash for expression
,   DGRP_CTOR_BEG   = 0x0100    // - TRUE ==> (-xst) ctor-bit enabled
,   DGRP_CTOR_END   = 0x0200    // - TRUE ==> (-xst) ctor-bit disabled
,   DGRP_XST        = 0x0400    // - TRUE ==> -xst in effect
,   DGRP_TAB_CALL   = 0x0800    // - TRUE ==> table call req'd to be genned
,   DGRP_TRY_EMIT   = 0x1000    // - TRUE ==> emit try adjustment
,   DGRP_NONE       = 0         // - place holder
} DGRP_FLAGS;

#define UNDEFINED_LABEL 0       // indicates no label definition
#define UNDEFINED_TEMP  0       // indicates no temporary defined
#define UNDEFINED_TYPE  (-1)    // indicates no type defined

typedef struct cond_info COND_INFO;
struct cond_info                // COND_INFO -- info. for conditional DTOR
{   SYMBOL sym;                 // - base symbol for test
    target_offset_t offset;     // - offset of byte to be tested/set/reset
    uint_8 mask;                // - mask for byte
    PAD_UNSIGNED
};

typedef struct                  // CGBK_INFO -- useful info for CGBK...
{   target_size_t size_code_ptr;// - size of code pointer
    target_size_t size_data_ptr;// - size of data pointer
    target_size_t size_offset;  // - size of offset
    target_size_t size_rw_base; // - base of RW_DTREG_..
    target_size_t size_fs_hand; // - size of fs handle
    PAD_UNSIGNED
} CGBK_INFO;

extern CGBK_INFO CgbkInfo;      // found in CGBKMAIN


typedef struct throw_ro THROW_RO;
struct throw_ro                 // THROW_RO -- R/O blocks for thrown types
{   THROW_RO *next;             // - next in ring( hdr. in ring_throw_ro )
    SYMBOL sym;                 // - symbol for the block
    TYPE_SIG *sig;              // - type signature for the type
    unsigned emitted : 1;       // - TRUE ==> has been emitted to codegen
    PAD_UNSIGNED
};

typedef target_offset_t STATE_VAR;

typedef struct                  // RT_DEF -- definitions for a R/T call
{   call_handle handle;         // - call handle
    cg_type type;               // - type for R/T call
} RT_DEF;

typedef struct try_impl TRY_IMPL;
struct try_impl                 // TRY_IMPL -- try's implemented in a function
{   TRY_IMPL* next;             // - next in ring
    SYMBOL try_sym;             // - try symbol
    SYMBOL jmp_sym;             // - jmpbuf variable
    target_offset_t offset_var;  // - offset of try variable from fun R/W
    target_offset_t offset_jmpbuf;//- offset of jmpbuf from fun R/W
    PAD_UNSIGNED
};

typedef struct dtreg_obj        DTREG_OBJ; // - registration variables
struct dtreg_obj
{   DTREG_OBJ* next;            // - next in ring
    SYMBOL sym;                 // - symbol for object (in use)
    SYMBOL cg_sym;              // - symbol for object (defined)
    target_offset_t offset;     // - offset of symbol
    unsigned in_use :1;         // - TRUE ==> in use
    PAD_UNSIGNED
};

typedef struct type_sig_ent     TYPE_SIG_ENT;
struct type_sig_ent             // TYPE_SIG_ENT -- entry in ring of type sig.s
{   TYPE_SIG_ENT *next;         // - next entry
    TYPE_SIG* sig;              // - type signature
};

typedef struct                  // SE_BASE -- base for state entries
{   SE* next;                   // - next in ring
    SE* prev;                   // - previous in ring
    STATE_VAR state_var;        // - state variable for entry
    uint_8 se_type;             // - type of entry
    uint_8 gen              :1; // - TRUE ==> entry is to be generated
    PAD_UNSIGNED
} SE_BASE;

typedef struct                  // SE_SYM_AUTO -- auto symbol to be DTORed
{   SE_BASE base;               // - base
    SYMBOL sym;                 // - auto symbol
    SYMBOL dtor;                // - DTOR for symbol
    target_offset_t offset;     // - offset of symbol from R/W block
    PAD_UNSIGNED
} SE_SYM_AUTO;

typedef struct                  // SE_SYM_STATIC -- static symbol to be DTORed
{   SE_BASE base;               // - base
    SYMBOL sym;                 // - static symbol
    SYMBOL dtor;                // - DTOR for symbol
} SE_SYM_STATIC;

typedef struct                  // SE_SUBOBJ -- sub-object
{   SE_BASE base;               // - base
    TYPE type;                  // - object type
    SYMBOL dtor;                // - DTOR for symbol
    target_offset_t offset;     // - offset within object (DTOR object)
    target_offset_t original;   // - offset within object (original object)
    uint_8 kind;                // - kind of object (DTC_...)
    PAD_UNSIGNED
} SE_SUBOBJ;

typedef struct                  // SE_ARRAY -- array in object table
{   SE_BASE base;               // - base
    TYPE_SIG* sig;              // - type signature for element
    target_offset_t offset;     // - offset of array
    target_size_t count;        // - number of elements
    PAD_UNSIGNED
} SE_ARRAY;

typedef struct                  // SE_CTOR_TEST -- command: test if ctor'ed
{   SE_BASE base;               // - base
    unsigned flag_no;           // - flag number
} SE_CTOR_TEST;

typedef struct                  // SE_SET_SV -- command: set_sv
{   SE_BASE base;               // - base
    SE* se;                     // - entry to be set
} SE_SET_SV;

typedef struct                  // SE_TEST_FLAG -- command: test_flag
{   SE_BASE base;               // - base
    SE* se_true;                // - entry, when TRUE
    SE* se_false;               // - entry, when FALSE
    target_offset_t index;      // - index
    PAD_UNSIGNED
} SE_TEST_FLAG;

typedef struct                  // SE_TRY -- try block start
{   SE_BASE base;               // - base
    TRY_IMPL* try_impl;         // - try implementation to be used
    TYPE_SIG_ENT* sigs;         // - type signatures for catches
    SYMBOL sym;                 // - symbol for try, catches
} SE_TRY;

typedef struct                  // SE_CATCH -- catch block start
{   SE_BASE base;               // - base
    SE* try_blk;                // - try for catch
    TYPE_SIG_ENT* sig;          // - type signature for caught variable
} SE_CATCH;

typedef struct                  // SE_FN_EXC -- function except. specification
{   SE_BASE base;               // - base
    TYPE_SIG_ENT* sigs;         // - type signatures in specification
} SE_FN_EXC;

typedef struct                  // SE_COMPONENT -- SUB-OBJECT COMPONENT
{   SE_BASE base;               // - base
    DTREG_OBJ* obj;             // - RW_DTREG_OBJECT symbol
    SYMBOL dtor;                // - DTOR for component
    target_offset_t offset;     // - offset of component
    PAD_UNSIGNED
} SE_COMPONENT;

typedef struct                  // SE_OBJECT_INIT -- static/auto object init
{   SE_BASE base;               // - base
    DTREG_OBJ* reg;             // - registration element
} SE_ARRAY_INIT;

typedef struct                  // SE_DLT_1 -- new-delete, 1 parm
{   SE_BASE base;               // - base
    SYMBOL op_del;              // - operator delete( void* )
    target_offset_t offset;     // - offset of ptr to object
    PAD_UNSIGNED
} SE_DLT_1;

typedef struct                  // SE_DLT_2 -- new-delete, 2 parms
{   SE_BASE base;               // - base
    SYMBOL op_del;              // - operator delete( void*, size_t )
    target_offset_t offset;     // - offset of ptr to object
    target_size_t size;         // - object size
    PAD_UNSIGNED
} SE_DLT_2;

typedef SE_DLT_1 SE_DLT_1_ARRAY;    // SE_DLT_1_ARRAY
typedef SE_DLT_2 SE_DLT_2_ARRAY;    // SE_DLT_2_ARRAY


union se                        // SE -- state entry: one of
{   SE_BASE base;               // - base
    SE_SYM_AUTO sym_auto;       // - auto symbol
    SE_SYM_STATIC sym_static;   // - static symbol
    SE_ARRAY array;             // - array in object table
    SE_SUBOBJ subobj;           // - sub-object
    SE_SET_SV set_sv;           // - cmd: set_sv
    SE_TEST_FLAG test_flag;     // - cmd: test_flag
    SE_TRY try_blk;             // - cmd: try
    SE_CATCH catch_blk;         // - cmd: catch
    SE_FN_EXC fn_exc;           // - cmd: fn_exc
    SE_COMPONENT component;     // - cmd: component
    SE_ARRAY_INIT array_init;   // - cmd: data array initialization
    SE_DLT_1 del_1;             // - cmd: new-delete (1 parm)
    SE_DLT_2 del_2;             // - cmd: new-delete (2 parms)
    SE_DLT_1 del_1_array;       // - cmd: new-delete-array (1 parm)
    SE_DLT_2 del_2_array;       // - cmd: new-delete-array (2 parms)
    SE_CTOR_TEST ctor_test;     // - cmd: ctor-test
};

typedef struct blk_posn BLK_POSN;

typedef struct cmd_base         CMD_BASE;
struct cmd_base
{   CMD_BASE* next;             // - next in ring
    SYMBOL sym;                 // - symbol for generated command
    unsigned emitted : 1;       // - TRUE ==> has been emitted to codegen
};

typedef struct                  // CMD_SET_SV
{   CMD_BASE base;              // - base
    STATE_VAR state_var;        // - state variable
    PAD_UNSIGNED
} CMD_SET_SV;

typedef struct                  // CMD_TEST_FLAG
{   CMD_BASE base;              // - base
    target_size_t index;        // - index
    STATE_VAR state_var_true;   // - true value
    STATE_VAR state_var_false;  // - false value
    PAD_UNSIGNED
} CMD_TEST_FLAG;

typedef struct                  // CMD_TRY
{   CMD_BASE base;              // - base
    TYPE_SIG_ENT* sigs;         // - type signatures
    target_offset_t offset_var; // - offset of try variable from fun R/W
    target_offset_t offset_jmpbuf; // - offset of jmpbuf from fun R/W
    STATE_VAR state;            // - state preceding try
    PAD_UNSIGNED
} CMD_TRY;

typedef struct                  // CMD_FN_EXC
{   CMD_BASE base;              // - base
    TYPE_SIG_ENT* sigs;         // - type signatures
} CMD_FN_EXC;

typedef struct                  // CMD_COMPONENT
{   CMD_BASE base;              // - base
    DTREG_OBJ* obj;             // - object
    SYMBOL dtor;                // - DTOR for component
    unsigned cmd_type;          // - command type
    target_offset_t offset;     // - offset of component
    PAD_UNSIGNED
} CMD_COMPONENT;

typedef struct                  // CMD_ARRAY_INIT
{   CMD_BASE base;              // - base
    DTREG_OBJ* reg;             // - registration to be used
} CMD_ARRAY_INIT;

typedef struct                  // CMD_CTOR_TEST
{   CMD_BASE base;              // - base
    unsigned flag_no;           // - flag number
} CMD_CTOR_TEST;

typedef struct                  // CMD_DEL_1
{   CMD_BASE base;              // - base
    SYMBOL op_del;              // - operator delete( void* )
    target_offset_t offset;     // - offset to ptr to object
    PAD_UNSIGNED
} CMD_DEL_1;

typedef struct                  // CMD_DEL_2
{   CMD_BASE base;              // - base
    SYMBOL op_del;              // - operator delete( void*, size_t )
    target_offset_t offset;     // - offset to ptr to object
    target_offset_t size;       // - object size
    PAD_UNSIGNED
} CMD_DEL_2;

typedef CMD_DEL_1 CMD_DEL_1_ARRAY;  // CMD_DEL_1_ARRAY
typedef CMD_DEL_2 CMD_DEL_2_ARRAY;  // CMD_DEL_2_ARRAY


typedef struct stab_defn        STAB_DEFN; // -- state table definition
struct stab_defn
{   SE* state_table;            // - state table entries
    SYMBOL ro;                  // - R/O symbol
    unsigned kind;              // - kind of state table
};


typedef struct stab_obj         STAB_OBJ; // -- state table for object
struct stab_obj
{   STAB_OBJ* next;             // - next in ring
    TYPE obj;                   // - StructType of object
    STAB_DEFN* defn;            // - definition
    STATE_VAR state_virtual;    // - object: state for last virtual base
    STATE_VAR state_direct;     // - object: state for last direct base
    PAD_UNSIGNED
};


typedef struct stab_ctl         STAB_CTL; // -- state table instance
struct stab_ctl
{   SYMBOL rw;                  // - R/W symbol
    STAB_DEFN *defn;            // - state table definition
    SE* marked_posn;            // - current state in generated code
};

typedef struct obj_init         OBJ_INIT; // - object being init'ed
struct obj_init
{   DTREG_OBJ* reg;             // - object registration
    SE* obj_se;                 // - state entry for that object
    SE* ctor_test;              // - state entry for ctor-test
    STAB_OBJ* defn;             // - components state table
    TYPE obj_type;              // - type of object
    SYMBOL obj_sym;             // - base symbol
    patch_handle patch;         // - patch handle for state
    target_size_t obj_offset;   // - offset from base symbol
    PAD_UNSIGNED
};


typedef struct call_stab        CALL_STAB; // - state-table info for call
struct call_stab
{   CALL_STAB *next;            // - next in ring
    call_handle handle;         // - handle for a call
    SE* se;                     // - current state-table position
    unsigned cd_arg;            // - integral CDTOR arg
    unsigned has_cd_arg :1;     // - "cd_arg" has been defined
    PAD_UNSIGNED
};


typedef struct fn_ctl FN_CTL;
struct fn_ctl                   // FN_CTL -- for each active file in process
{
    unsigned base_labs_cs;      // - stack base: labels (control)
    unsigned base_goto_near;    // - stack base: gotos (near)
    unsigned sym_trans_id;      // - symbol translation id at function start
    unsigned cond_flags;        // - # conditional flags
    unsigned cdtor_val;         // - CDTOR value, when has_cdtor_val is set
    unsigned cond_next;         // - next bit offset for conditional flags
    unsigned try_depth;         // - depth of nested try statements
    SYMBOL return_symbol;       // - return symbol
    SYMBOL this_sym;            // - this symbol
    SYMBOL cdtor_sym;           // - extra cdtor int parm symbol
    SYMBOL func;                // - function being generated
    SYMBOL new_ctor_ptr;        // - points at new'ed item being ctor'ed
    label_handle return_label;  // - return label
    label_handle cdarg_lab;     // - label for CDARG test
    label_handle try_label;     // - label at top of try statement
    call_handle handle;         // - handle for IBRP substitution
    back_handle prof_data;      // - profiling data handle
    SE* state_table_bound;      // - bounding entry in function state table
    SE* pre_init;               // - pos'n before symbol initialization
    SE* fun_sv;                 // - current state entry for function
    SE* ctor_components;        // - start of CTOR component SE.s
    SE* dtor_components;        // - end of dtor components
    SE* ctored_obj;             // - object ctored by current expression
    SE* marked_at_start;        // - marked position at function start
    STAB_OBJ* obj_registration; // - registration for type
    CALL_STAB* expr_calls;      // - calls for current expression
    CGFILE* cgfile;             // - CGFILE for this function

    DT_METHOD dtor_method;      // - current destruction method
    DT_METHOD func_dtor_method; // - function destruction method

    uint deregistered   :1;     // - TRUE ==> function has been de-registered
    uint has_fn_exc     :1;     // - TRUE ==> has function exception spec.
    uint is_ctor        :1;     // - TRUE ==> is a CTOR
    uint is_dtor        :1;     // - TRUE ==> is a DTOR
    uint ctor_complete  :1;     // - TRUE ==> has completed the CTOR'ing
    uint coded_return   :1;     // - TRUE ==> return in user code
    uint has_ctor_test  :1;     // - TRUE ==> function might use CTOR-TEST
    uint has_cdtor_val  :1;     // - TRUE ==> has integral CDTOR argument
    uint temp_dtoring   :1;     // - TRUE ==> temporary DTORING req'd in expr.
    uint ctor_test      :1;     // - TRUE ==> expr. causes CTOR-TEST
    uint dtor_reg_reqd  :1;     // - TRUE ==> dtor registration required
    uint debug_info     :1;     // - TRUE ==> debug info for function
    PAD_UNSIGNED
};

typedef struct                  // CTOR_FLAG_SET -- patch entry for ctor flag
{   patch_handle ph_clr;        // - handle for clr mask
    SE* se;                     // - state entry for CTOR
} CTOR_FLAG_SET;

#define UNDEF_AREL ((target_offset_t)(-1)) // undefined auto-relocation offset


// PROTOTYPES

void AutoRelFree(               // FREE ALL RELOCATIONS
    void )
;
void AutoRelRegister(           // REGISTER A RELOCATION
    SYMBOL sym,                 // - symbol
    target_offset_t* a_off )    // - addr[ offset to be relocated ]
;
void BeGenTsRef(                // GENERATE REFERENCE TO TYPE-SIGNATURE
    TYPE_SIG* ts )              // - type signature
;
TYPE_SIG_ENT* BeTypeSigEnt(     // ALLOCATE A TYPE_SIG_ENT
    TYPE type )                 // - type for signature
;
TYPE_SIG_ENT* BeTypeSigEntsCopy(// MAKE COPY OF TYPE-SIGNATURE entries
    TYPE_SIG_ENT* orig )        // - original entries
;
TYPE_SIG *BeTypeSignature(      // GET TYPE_SIG FOR A TYPE
    TYPE type )                 // - input type
;
void BeGenTypeSigEnts(          // EMIT TYPE_SIG_ENT RING
    TYPE_SIG_ENT* ring )        // - ring of entries
;
void BeGenRttiInfo(             // GENERATE RTTI INFORMATION
    void )
;
void BeGenTypeSignatures(       // GENERATE ALL TYPE SIGNATURES
    void )
;
SE* BlkPosnCurr(                // GET CURRENT BLOCK POSITION
    void )
;
SE* BlkPosnEnclosing(           // GET CURRENT POSITION OF ENCLOSING BLOCK
    void )
;
void BlkPosnPop(                // POP A BLOCK POSITION
    void )
;
void BlkPosnPush(               // PUSH A NEW BLOCK POSITION
    SCOPE scope )               // - defining scope
;
SE* BlkPosnScope(               // GET BLOCK POSITION FOR A SCOPE
    SCOPE scope )               // - scope in question
;
SE* BlkPosnTempBeg(             // GET STARTING POS'N FOR TEMP DTOR'ING
    void )
;
SE* BlkPosnTempBegSet(          // SET STARTING POS'N FOR TEMP DTOR'ING
    SE* se )                    // - starting position
;
SE* BlkPosnTempEnd(             // GET ENDING POS'N FOR TEMP DTOR'ING
    void )
;
SE* BlkPosnTempEndSet(          // SET ENDING POS'N FOR TEMP DTOR'ING
    SE* se )                    // - ending position
;
void BlkPosnTrash(              // TRASH TOP BLOCK POSITION
    void )
;
SE* BlkPosnUpdate(              // UPDATE POSITION IN CURRENT BLOCK
    SE* se )                    // - state entry for current position
;
boolean BlkPosnUseStab(         // TEST IF REALLY USING STATE TABLE IN SCOPE
    void )
;
void CallIndirectPop(           // POP AN INDIRECT CALL ENTRY
    void )
;
SYMBOL CallIndirectPush(        // PUSH SYMBOL FOR INDIRECT CALL
    TYPE type )                 // - expression type
;
void CallIndirectVirtual(       // MARK INDIRECT CALL AS VIRTUAL
    SYMBOL vfunc,               // - the virtual function
    boolean is_virtual,         // - TRUE ==> an actual virtual call
    target_offset_t adj_this,   // - adjustment for "this"
    target_offset_t adj_retn )  // - adjustment for return
;
CALL_STAB* CallStabAlloc(       // ALLOCATE CALL_STAB
    call_handle handle,         // - handle for call
    FN_CTL* fctl )              // - function hosting the call
;
boolean CallStabCdArgGet(       // GET CD-ARG FOR A CALL
    call_handle handle,         // - handle for call
    unsigned *a_cd_arg )        // - addr[ value for CD-ARG ]
;
unsigned CallStabCdArgSet(      // SET CD-ARG FOR A CALL
    call_handle handle,         // - handle for call
    unsigned cd_arg )           // - value for CD-ARG
;
void CallStabFree(              // FREE CALL_STAB
    FN_CTL* fctl,               // - function hosting the call
    CALL_STAB* cstb )           // - call information
;
SE* CallStabStateTablePosn(     // GET STATE-TABLE POSITION AT CALL POINT
    call_handle handle )        // - handle for inline call
;
boolean CallStackTopInlined(    // TEST IF TOP OF CALL STACK IS INLINED
    void )
;
call_handle CallStackPop(       // POP CALL STACK
    void )
;
void CallStackPush(             // PUSH CALL STACK
    SYMBOL func,                // - NULL, or inlined function
    call_handle handle,         // - handle for call
    cg_type cg_retn )           // - cg type of call
;
target_offset_t CallStackRetnAdj( // GET RETURN ADJUSTMENT FOR VIRTUAL CALL
    void )
;
target_offset_t CallStackRetnType( // GET RETURN TYPE FOR CALL
    void )
;
target_offset_t CallStackThisAdj( // GET "THIS" ADJUSTMENT FOR VIRTUAL CALL
    void )
;
call_handle CallStackTopHandle( // GET HANDLE FOR TOP OF CALL STACK
    void )
;
SYMBOL CallStackTopFunction(    // GET FUNCTION FOR TOP OF CALL STACK
    void )
;
cg_name CgAddrSymbol(           // PASS ADDR OF SYMBOL TO CODE GENERATOR
    SYMBOL sym )                // - symbol
;
void CgAssign(                  // EMIT AN ASSIGNMENT
    cg_name lhs,                // - lhs argument
    cg_name rhs,                // - rhs argument
    cg_type type )              // - type for assignment
;
void CgAssignPtr(               // EMIT A POINTER ASSIGNMENT
    cg_name lhs,                // - lhs argument
    cg_name rhs )               // - rhs argument
;
cg_name CgAssignStateVar(       // ASSIGN STATE-VAR VALUE
    SYMBOL blk,                 // - R/W Block
    SE* se,                     // - state entry
    target_offset_t offset )    // - offset of state variable
;
CALL_STAB* CgBackCallGened(     // SETUP FOR GENERATED CALL
    call_handle handle )        // - call handle
;
FN_CTL* CgBackFnCtlFini(        // COMPLETE FN_CTL WITH CGBKMAIN INFO
    FN_CTL* fctl )              // - current file information
;
FN_CTL* CgBackFnCtlInit(        // INITIALIZE FN_CTL WITH CGBKMAIN INFO
    FN_CTL* fctl )              // - current file information
;
unsigned CgBackInlinedDepth(    // GET CURRENT INLINED DEPTH
    void )
;
SYMBOL CgBackOpDelete(          // GET ADDRESIBLE OPERATOR DELETE FOR A TYPE
    TYPE type )                 // - the type
;
cg_name CgCallBackAutoCtor(     // SET CALL BACKS FOR A DCL'ED AUTO
    cg_name expr,               // - expression
    cg_type type,               // - type of expression
    SE* se )                    // - state entry for ctored object
;
cg_name CgCallBackCtorDone(     // SET A CALL BACK FOR A CTOR-TEST : DONE
    cg_name expr,               // - expression
    cg_type type,               // - type of expression
    SE* se )                    // - state entry for ctored object
;
cg_name CgCallBackCtorStart(    // SET A CALL BACK FOR A CTOR-TEST : START
    cg_name expr,               // - expression
    cg_type type,               // - type of expression
    SE* se )                    // - state entry to be inserted on call back
;
cg_name CgCallBackInitRefBeg(   // START CALL-BACK FOR INIT-REF
    SE* se )                    // - state entry for init-ref variable
;
cg_name CgCallBackLeft(         // MAKE A LEFT CALL-BACK
    cg_name expr,               // - expression
    void (*fun)( void* ),       // - call-back function
    void* data,                 // - data for call back
    cg_type type )              // - type of expression
;
void CgCallBackNewCtored(       // NEW OBJECT WAS CTOR'ED
    SE* se_del,                 // - state entry for delete during CTOR throw
    FN_CTL* fctl )              // - function information
;
cg_name CgCallBackRight(        // MAKE A RIGHT CALL-BACK
    cg_name expr,               // - expression
    void (*fun)( void* ),       // - call-back function
    void* data,                 // - data for call back
    cg_type type )              // - type of expression
;
cg_name CgCallBackTempCtor(     // SET CALL BACKS FOR TEMP CTORED
    cg_name expr,               // - expression
    cg_type type,               // - type of expression
    SE* se )                    // - state entry to be inserted on call back
;
void CgCdArgDefine(             // DEFINE CDOPT VALUE
    unsigned value )            // - cdopt value
;
void CgCdArgRemove(             // REMOVE CDTOR ENTRY FOR A CALL-HANDLE
    call_handle handle )        // - handle for call
;
void CgCdArgUsed(               // USE A CALL-HANDLE DIRECTLY
    call_handle handle )        // - handle for call
;
SYMBOL CgCmdArrayInit(          // GET SYMBOL FOR DTC_ARRAY_INIT COMMAND
    SE* se )                    // - state entry in state table
;
SYMBOL CgCmdComponent(          // GET SYMBOL FOR DTC_COMP... COMMAND
    SE* se )                    // - state entry in state table
;
SYMBOL CgCmdCtorTest(           // GET SYMBOL FOR CTOR-TEST COMMAND
    SE* se )                    // - state entry
;
SYMBOL CgCmdDel1(               // GET SYMBOL FOR DTC_DEL_1
    SE* se )                    // - state entry in state table
;
SYMBOL CgCmdDel1Array(          // GET SYMBOL FOR DTC_DEL_1
    SE* se )                    // - state entry in state table
;
SYMBOL CgCmdDel2(               // GET SYMBOL FOR DTC_DEL_2
    SE* se )                    // - state entry in state table
;
SYMBOL CgCmdDel2Array(          // GET SYMBOL FOR DTC_DEL_2
    SE* se )                    // - state entry in state table
;
SYMBOL CgCmdFnExc(              // GET SYMBOL FOR FN-EXCEPTION SPEC. COMMAND
    SE* se )                    // - state entry in state table
;
void CgCmdsGenerate(            // GENERATE DTOR CMD.S
    void )
;
SYMBOL CgCmdSetSv(              // GET SYMBOL FOR SET_SV CMD TO BE GEN'ED
    SE* se )                    // - state entry in state table
;
SYMBOL CgCmdTestFlag(           // GET SYMBOL FOR TEST_FLAG CMD TO BE GEN'ED
    SE* se )                    // - state entry in state table
;
SYMBOL CgCmdTry(                // GET SYMBOL FOR TRY BLOCK
    SE* se )                    // - state entry in state table
;
cg_name CgComma(                // CONSTRUCT COMMA EXPRESSION
    cg_name lhs,                // - expression on left
    cg_name rhs,                // - expression on right
    cg_type type )              // - type of right expression
;
void CgCommaBefore(             // EMIT COMMA'ED EXPRESSION BEFORE
    cg_name expr,               // - expression
    cg_type type )              // - type of above expression
;
void CgCommaOptional(           // EMIT OPTIONAL COMMA'ED EXPRESSION
    cg_name expr,               // - expression or NULL
    cg_type type )              // - type of expression
;
void CgCommaWithTopExpr(        // PUSH COMMA'D EXPRESSION WITH TOP EXPR
    cg_name expr,               // - rhs expression
    cg_type type )              // - rhs type
;
void CgControl(                 // CONTROL OPCODE
    cg_op operand,              // - operand
    cg_name expr,               // - expression
    cg_type type,               // - expression type
    label_handle label )        // - label
;
void CgCtorTestTempsRegister(   // REGISTER DTORING TEMPS FOR CTOR
    FN_CTL* fctl )              // - function control
;
SYMBOL CgDeclHiddenParm(        // DECLARE HIDDEN ARG (THIS, CDTOR)
    SCOPE scope,                // - function parameters scope
    TYPE type,                  // - symbol type
    SYMBOL symbol_model,        // - model for symbol
    unsigned specname_index )   // - special name index
;
void CgDeclParms(               // DEFINE ARGS FOR CURRENT FN IN CORRECT ORDER
    FN_CTL *fctl,               // - current function control pointer
    SCOPE scope )               // - argument scope
;
boolean CgDeclSkippableConstObj(// DETERMINE IF SKIPPABLE CONST OBJECT
    SYMBOL sym )                // - symbol
;
void CgDeclSym(                 // PROCESS SYMBOL IN BLOCK-OPEN SCOPE
    SYMBOL sym )                // - current symbol
;
cg_name CgDestructSymOffset(    // CONSTRUCT DTOR CALL FOR SYMBOL+OFFSET
    FN_CTL* fctl,               // - function control
    SYMBOL dtor,                // - destructor
    SYMBOL sym,                 // - SYMBOL to be DTOR'ed
    target_size_t offset,       // - offset from "sym"
    unsigned cdtor )            // - CDTOR to be used
;
cg_name CgDestructExpr(         // CONSTRUCT DTOR CALL FOR EXPRESSION
    SYMBOL dtor,                // - destructor
    cg_name var,                // - expression to be DTOR'ed
    unsigned cdtor )            // - CDTOR to be used
;
void CgDestructExprTemps(       // DESTRUCT TEMPS IN AN EXPRESSION
    DGRP_FLAGS pop_type,        // - type of popping
    FN_CTL* fctl )              // - current file information
;
void CgDone(                    // COMPLETE CODE-GENERATION OF EXPRESSION
    cg_name expr,               // - expression
    cg_type type )
;
void CgDtorAll(                 // DTOR ALL IN FUNCTION
    void )
;
void CgDtorSe(                  // DTOR UNTIL SE ENTRY
    SE* bound )                 // - bounding entry
;
cg_name CgDtorStatic(           // DTOR STATIC OBJECT
    SYMBOL sym )                // - object symbol
;
back_handle CgProfData(         // HANDLE FOR PROFILING DATA
    void )
;
void CgExprAttr(                // SET CONST/VOLATILE/etc. ATTRIBUTES FOR EXPR
    cg_sym_attr attr )          // - attribute
;
void CgExprPushWithAttr(        // PUSH EXPR WITH ATTRIBUTES
    cg_name expr,               // - expression
    cg_type type,               // - expression type
    cg_sym_attr attr )          // - expression attribute
;
void CgExprDtored(              // DTOR CG EXPRESSION
    cg_name expr,               // - expression
    cg_type type,               // - expression type
    DGRP_FLAGS pop_type,        // - type of popping destruction
    FN_CTL* fctl )              // - function control
;
cg_name CgExprPop(              // POP CG EXPRESSION
    void )
;
boolean CgExprPopGarbage(       // POP EXPR STACK IF TOP EXPR IS GARBAGE
    void )
;
cg_name CgExprPopType(          // POP CG EXPRESSION and TYPE
    cg_type* a_type )           // - addr[ type ]
;
void CgExprPush(                // PUSH CG EXPRESSION RESULT
    cg_name expr,               // - expression
    cg_type type )              // - expression type
;
unsigned CgExprStackSize(       // RETURN # TEMPS STACKED
    void )
;
cg_type CgExprType(             // GET EXPRESSION TYPE
    TYPE type )                 // - C++ type
;
cg_name CgFetchPtr(             // FETCH A POINTER
    cg_name operand )           // - operand to be fetched
;
cg_name CgFetchSym(             // FETCH A SYMBOL
    SYMBOL sym )                // - symbol
;
cg_name CgFetchTemp(            // FETCH A TEMPORARY
    temp_handle handle,         // - handle for temporary
    cg_type type )              // - type of temp
;
cg_name CgFetchType(            // PERFORM A FETCH
    cg_name operand,            // - operand to be fetched
    cg_type type )              // - type of fetch
;
cg_name CgFetchSymbolAddOffset( // GENERATE RVALUE( SYMBOL ) + OFFSET
    SYMBOL sym,                 // - symbol
    unsigned offset )           // - offset
;
cg_type CgFuncRetnType(         // GET CG RETURN TYPE FOR A FUNCTION
    SYMBOL func )               // - function
;
#if _CPU != _AXP
void CgFunDeregister(           // DE-REGISTER A FUNCTION
    SYMBOL rw )                 // - symbol for R/W block
;
#endif
void CgFunRegister(             // REGISTER A FUNCTION
    FN_CTL* fctl,               // - function information
    SYMBOL rw,                  // - symbol for R/W block
    SYMBOL ro )                 // - symbol for R/O block
;
cg_type CgGetCgType(            // GET CODEGEN TYPE
    TYPE type )                 // - type
;
void CgGotoReturnLabel(         // GENERATE "GOTO RETURN-LABEL"
    FN_CTL* fctl )              // - function being emitted
;
void CgLabel(                   // EMIT A LABEL
    label_handle lab )          // - the label
;
void CgLabelsFinish(            // FINISH LABELS IN A VIRTUAL STACK
    VSTK_CTL *stack,            // - the stack
    int base )                  // - base for stack
;
void CgLabelPlantReturn(        // PLANT RETURN LABEL, IF REQUIRED
    FN_CTL* fctl )              // - function being emitted
;
void CgLabelsPop(               // POP A LABELS STACK
    VSTK_CTL *stack,            // - the stack
    unsigned count )            // - number of labels to pop
;
cg_name CgMakeDup(              // MAKE A DUPLICATE
    cg_name *orig,              // - original
    cg_type cgtype )            // - and its type
;
cg_name CgMakeTwoDups(          // MAKE TWO DUPLICATES
    cg_name *orig,              // - original and destination for first dup
    cg_name *second,            // - destination for second dup
    cg_type cgtype )            // - original type
;
unsigned CgNonThunkDepth(       // COMPUTE INLINE DEPTH WITHOUT THUNKS
    FN_CTL* fctl )              // - current function
;
cg_name CgOffset(               // PASS ABSOLUTE OFFSET TO CODE GENERATOR
    unsigned offset )           // - offset value
;
cg_name CgOffsetExpr(           // MAKE OFFSET EXPRESSION
    cg_name expr,               // - lhs expression
    target_offset_t offset,     // - offset
    cg_type type )              // - resultant type
;
target_offset_t CgOffsetRw(     // COMPUTE OFFSET FROM R/W REGISTRATION
    target_offset_t offset )    // - offset within stack
;
void CgPushGarbage(             // PUSH GARBAGE (TO BE TRASHED/POPPED)
    void )
;
boolean CgRetnOptActive         // TEST IF RETURN OPTIMIZATION ACTIVE FOR FILE
    ( FN_CTL* fctl )            // - file-activation control
;
boolean CgRetnOptForFile        // TEST IF RETURN OPTIMIZATION FOR FILE
    ( CGFILE* file_ctl )        // - file control
;
boolean CgRetnOptIsOptVar       // TEST IF SYMBOL IS RETURN-OPTIMIZATION VAR
    ( FN_CTL* fctl              // - file-activation control
    , SYMBOL var )              // - var. to be tested
;
void CgRetnOpt_RETNOPT_BEG      // PROCESS IC_RETNOPT_BEG
    ( FN_CTL* fctl )            // - file-activation control
;
void CgRetnOpt_RETNOPT_VAR      // PROCESS IC_RETNOPT_VAR
    ( FN_CTL* fctl              // - file-activation control
    , SYMBOL var )              // - variable
;
cg_type CgReturnType(           // GET CG-TYPE FOR RETURN
    cg_type type )              // - code-gen type
;
cg_name CgRtCallExec(           // EXECUTE R/T CALL
    RT_DEF *def )               // - definition for call
;
void CgRtCallExecDone(          // EXECUTE R/T CALL, THEN DONE
    RT_DEF *def )               // - definition for call
;
void CgRtCallExecNoArgs(        // EXECUTE R/T CALL, WITHOUT ARGUMENTS
    RTF rt_code )               // - code for run/time call
;
void CgRtCallInit(              // SET UP A R/T CALL
    RT_DEF *def,                // - definition for call
    RTF rt_code )               // - code for run/time call
;
void CgRtParam(                 // SET UP A PARAMETER
    cg_name expr,               // - expression gen'ed
    RT_DEF *def,                // - definition for call
    cg_type type )              // - argument type
;
void CgRtParamAddrSym(          // SET UP PARAMETER: ADDR( SYMBOL )
    RT_DEF *def,                // - definition for call
    SYMBOL sym )                // - symbol
;
void CgRtParamConstOffset(      // SET UP PARAMETER: CONSTANT OFFSET
    RT_DEF *def,                // - definition for call
    unsigned value )            // - parameter value
;
cg_name CgSaveAsTemp(           // SAVE INTO A TEMPORARY
    temp_handle* a_hand,        // - addr[ temp handle ]
    cg_name expr,               // - expression to be saved
    cg_type type )              // - and its type
;
cg_name CgSideEffect(           // CONSTRUCT SIDE-EFFECT EXPRESSION
    cg_name lhs,                // - expression on left
    cg_name rhs,                // - expression on right
    cg_type type )              // - type of right expression
;
label_handle CgSwitchBeg        // GENERATE CODE FOR START OF SWITCH STMT
    ( FN_CTL* fctl )            // - function control
;
void CgSwitchCase               // GENERATE CODE FOR SWITCH CASE
    ( long case_value )         // - case value
;
void CgSwitchCaseRange          // GENERATE CODE FOR RANGE OF SWITCH CASES
    ( long range_start          // - start of range
    , long range_end )          // - end of range
;
void CgSwitchDefault            // GENERATE CODE FOR DEFAULT STMT
    ( void )
;
void CgSwitchDefaultGen         // GENERATE CODE FOR DEFAULT STMT (GEN'ED)
    ( label_handle lab )        // - label to be used
;
void CgSwitchEnd                // GENERATE CODE FOR END OF SWITCH STMT
    ( void )
;
cg_name CgSymbol(               // PASS SYMBOL TO CODE GENERATOR
    SYMBOL sym )                // - symbol
;
cg_name CgSymbolPlusOffset(     // GENERATE SYMBOL + OFFSET
    SYMBOL sym,                 // - symbol
    unsigned offset )           // - offset
;
void CgTrash(                   // COMPLETE TRASH OF EXPRESSION
    cg_name expr,               // - expression
    cg_type type )
;
SYMBOL CgVarRo(                 // MAKE R/O CODEGEN VARIABLE
    target_size_t size,         // - size of variable
    unsigned id,                // - SC_...
    NAME name )                 // - name or NULL
;
SYMBOL CgVarRw(                 // MAKE R/W CODEGEN VARIABLE
    target_size_t size,         // - size
    unsigned id )               // - SC_...
;
SYMBOL CgVarTemp(               // MAKE R/W AUTO TEMPORARY
    target_size_t size )        // - size of temp
;
SYMBOL CgVarTempTyped(          // MAKE R/W AUTO TEMPORARY, FOR cg_type
    cg_type type )              // - type of entry
;
void CondInfoEnd(               // SET UP CALL-BACK FOR IC_COND_END
    void )
;
void CondInfoDirectFlags(       // SET FOR DIRECT-FLAGS PROCESSING
    unsigned flag_bytes )       // - # bytes of flags required
;
void CondInfoFalse(             // SET UP CALL-BACK FOR IC_COND_FALSE
    void )
;
void CondInfoNewCtorBeg(        // CTOR OF NEW'ED OBJECT: START
    FN_CTL* fctl )              // - function information
;
void CondInfoNewCtorEnd(        // CTOR OF NEW'ED OBJECT: END
    FN_CTL* fctl )              // - function information
;
void CondInfoPush(              // PUSH COND_INFO STACK
    FN_CTL* fctl )              // - function control
;
void CondInfoPop(               // POP COND_INFO STACK
    void )
;
void CondInfoSetCtorTest(       // SET/RESET FLAG FOR CTOR-TEST
    FN_CTL* fctl,               // - function control
    boolean set_flag )          // - TRUE ==> set the flag; FALSE ==> clear
;
void CondInfoSetFlag(           // SET FLAG FOR CONDITIONAL DTOR BLOCK
    FN_CTL* fctl,               // - function control
    boolean set_flag )          // - TRUE ==> set the flag; FALSE ==> clear
;
void CondInfoSetup(             // SETUP UP CONDITIONAL INFORMATION
    unsigned index,             // - index of flag
    COND_INFO* cond,            // - conditional information
    FN_CTL* fctl )              // - function information
;
void CondInfoTrue(              // SET UP CALL-BACK FOR IC_COND_TRUE
    void )
;
label_handle CondLabelAdd       // ADD A CONDITIONAL LABEL
    ( COND_LABEL** a_ring       // - addr[ ring of labels ]
    , SE* se )                  // - state entry
;
boolean CondLabelEmit           // EMIT CONDITIONAL LABEL IF REQ'D
    ( COND_LABEL** a_ring       // - addr[ ring of labels ]
    , SE* se )                  // - state entry at current position
;
SE* CondLabelNext               // FIND STATE ENTRY FOR NEXT COND. LABEL
    ( COND_LABEL** a_ring       // - addr[ ring of labels ]
    , SE* se )                  // - default state entry
;
void CondLabelsEmit(            // EMIT ANY REMAINING CONDITIONAL LABELS
    COND_LABEL** a_ring )       // - addr[ ring of labels ]
;
void DgByte(                    // DATA GENERATE A BYTE
    uint_8 byte )               // - to be generated
;
void DgInitBytes(               // DATA GENERATE INIT BYTES
    target_size_t   size,       // - number of bytes
    uint_8          val )       // - byte to initialize with
;
void DgOffset(                  // GENERATE AN OFFSET VALUE
    unsigned offset )           // - offset value
;
void DgPtrSymCode(              // GENERATE POINTER FOR A CODE SYMBOL
    SYMBOL sym )                // - the symbol
;
void DgPtrSymData(              // GENERATE POINTER FOR A DATA SYMBOL
    SYMBOL sym )                // - the symbol
;
void DgPtrSymDataOffset(        // GENERATE POINTER FOR A DATA SYMBOL, OFFSET
    SYMBOL sym,                 // - the symbol
    target_size_t offset )      // - the offset
;
void DgPtrSymOff(               // GENERATE POINTER FOR A SYMBOL + OFFSET
    SYMBOL sym,                 // - the symbol
    target_size_t offset )      // - the offset
;
back_handle DgStringConst(      // STORE STRING CONSTANT WITH NULL
    STRING_CONSTANT str,        // - string to store
    uint_16         *psegid,    // - addr(string segid)
    unsigned        control )   // - control mask (DSC_*)
;
#if _CPU == _AXP
void DgAlignInternal(           // ALIGN INTERNAL CONTROL BLOCK
    void )
;
#else
#define DgAlignInternal()
#endif
void DgAlignPad(                // INSERT PADDING IN A STRUCTURE
    unsigned total )            // - number of bytes emitted so far
;
void DgAlignSymbol(             // ALIGN SYMBOL TO CORRECT BOUNDARY
    SYMBOL sym )                // - symbol to align
;
void DgSymbolDefInit(           // DATA GENERATE SYMBOL (DEFAULT DATA)
    SYMBOL sym )                // - the symbol
;
void DgUninitBytes(             // DATA GENERATE UNINIT BYTES
    target_size_t   size )      // - number of bytes
;
DT_METHOD DtmDirect(            // CONVERT DTOR METHOD TO DIRECT COUNTERPART
    DT_METHOD dtm )             // - default method
;
boolean DtmTabular(             // DETERMINE IF SCOPE TABULAR DTOR METHOD
    FN_CTL* fctl )              // - function control
;
boolean DtmTabularFunc(         // DETERMINE IF SCOPE TABULAR DESTRUCTION METHOD, FUNCTION
    FN_CTL* fctl )              // - function control
;
SE* DtorForDelBeg(              // DTORING AREA TO BE DELETED: start
    FN_CTL* fctl,               // - function information
    target_size_t elem_size,    // - size of one element in area
    unsigned dlt1,              // - entry type when one arg
    unsigned dlt2,              // - entry type when two args
    SYMBOL op_del )             // - operator delete to be used
;
void DtorForDelEnd(             // DTORING AREA TO BE DELETED: end
    FN_CTL* fctl,               // - function information
    SE* se_dlt )                // - entry
;
DTREG_OBJ* DtregActualBase(     // REGISTER AN ACTUAL BASE
    FN_CTL* fctl )              // - current function information
;
DTREG_OBJ* DtregObj(            // LOCATE RW_DTREG_OBJ SYMBOL
    FN_CTL* fctl )              // - current function information
;
unsigned FnCtlCondFlagCtor(     // GET FLAG # FOR CTOR-TEST
    FN_CTL *fctl )              // - current function information
;
unsigned FnCtlCondFlagExpr(     // START FLAGS OFFSET FOR EXPRESSION
    FN_CTL *fctl )              // - current function information
;
unsigned FnCtlCondFlagNext(     // GET NEXT FLAG OFFSET FOR EXPRESSION
    FN_CTL *fctl )              // - current function information
;
SYMBOL FnCtlNewCtorPtr(         // GET SYMBOL FOR NEW-CTORED PTR.
    FN_CTL *fctl )              // - current function information
;
void FnCtlPop(                  // POP FILE CONTROL
    void )
;
FN_CTL *FnCtlPush(              // PUSH FILE CONTROL
    call_handle handle,         // - handle for IBRP substitution
    CGFILE *cgfile )            // - fn's CGFILE
;
FN_CTL* FnCtlPrev(              // GET PREVIOUS FN_CTL ITEM
    FN_CTL* curr )              // - current file control
;
FN_CTL* FnCtlTop(               // GET TOP FN_CTL ITEM
    void )
;
void FreeDtregObjs(             // FREE OBJECT REGISTRATIONS (FUNCTION)
    void )
;
SE* FstabActualPosn(            // GET ACTUAL POSITION IN STATE TABLE
    void )
;
SE* FstabAdd(                   // ADD STATE ENTRY TO STATE TABLE
    SE* se )                    // - state entry
;
void FstabAssignStateVar(       // EMIT CODE TO ASSIGN STATE VARIABLE
    SE* se )                    // - NULL or state entry to be set
;
SE* FstabCtorTest(              // ALLOCATE CTOR-TEST COMMAND
    FN_CTL* fctl )              // - function being emitted
;
SE* FstabCurrPosn(              // GET CURRENT STATE ENTRY FOR FUNCTION
    void )
;
void FstabDeRegister(           // DE-REGISTER FUNCTION
    FN_CTL* fctl )              // - function control
;
void FstabRegister(             // REGISTER FUNCTION
    FN_CTL* fctl )              // - function control
;
#ifndef NDEBUG
void FstabDump(                 // DEBUG ONLY: DUMP FUNCTION STATE TABLE
    void )
;
#endif
cg_name FstabEmitStateVar(      // EMIT CODE TO SET STATE VARIABLE, IF REQ'D
    SE* se,                     // - NULL or state entry to be set
    FN_CTL* fctl )              // - function being emitted
;
void FstabEmitStateVarExpr(     // EMIT EXPR'N TO SET STATE VARIABLE, IF REQ'D
    SE* se,                     // - NULL or state entry to be set
    FN_CTL* fctl )              // - function being emitted
;
cg_name FstabEmitStateVarPatch( // EMIT CODE TO PATCH STATE VARIABLE, IF REQ'D
    patch_handle* a_handle,     // - addr[ handle ]
    FN_CTL* fctl )              // - function being emitted
;
#if _CPU == _AXP
SYMBOL FstabExcData(            // ALPHA: SET EXCEPTION DATA
    void )
;
SYMBOL FstabExcHandler(         // ALPHA: SET EXCEPTION HANDLER
    void )
;
SYMBOL FstabExcRw(              // ALPHA: GET R/W DATA SYMBOL
    void )
;
#endif
SE* FstabFindAuto(              // FIND AUTO VAR ENTRY IN STATE TABLE
    SYMBOL auto_var )           // - the auto variable
;
boolean FstabGenerate(          // GENERATE FUNCTION STATE TABLE
    void )
;
boolean FstabHasStateTable(     // DETERMINE IF STATE TABLE BEING GEN'ED
    void )
;
void FstabInit(                 // INITIALIZE FUNCTION STATE TABLE
    void )
;
SE* FstabMarkedPosn(            // GET MARKED POSITION
    void )
;
SE* FstabMarkedPosnSet(         // SET MARKED POSITION
    SE* se )                    // - new position
;
SE* FstabPosnGened(             // GET GENNED POSITION IF REQUIRED
    SE* src,                    // - source entry
    SE* tgt )                   // - target entry
;
SE* FstabPrecedes(              // GET PRECEDING STATE ENTRY
    SE* se )                    // - starting entry
;
SE* FstabPrevious(              // GET PREVIOUS STATE ENTRY
    SE* se )                    // - starting entry
;
void FstabPrune(                // PRUNE END OF STATE TABLE
    SE* end )                   // - actual ending position
;
void FstabRemove(               // REMOVE LAST STATE ENTRY
    void )
;
SYMBOL FstabRw(                 // GET R/W SYMBOL FOR FUNCTION STATE TABLE
    void )
;
void FstabSetDtorState(         // SET STATE VAR. FOR DTOR
    SE* se,                     // - state entry being DTOR'ed
    FN_CTL* fctl )              // - file information
;
SE* FstabSetSvSe(               // ADD SET_SV FOR SE TO STATE TABLE
    SE* tgt )                   // - state entry
;
boolean FstabSetup(             // SETUP FUNCTION STATE TABLE
    CGFILE* file_ctl,           // - current file information
    FN_CTL* fctl )              // - current file generation information
;
SE* FstabTestFlag(              // CREATE TEST-FLAG ENTRY
    unsigned flag_no,           // - flag #
    SE* se_true,                // - entry when true
    SE* se_false )              // - entry when false
;
SE* FunctionStateTablePosn(     // GET CURRENT STATE ENTRY FOR FUNCTION
    void )
;
void FunctionStateTableSetSvSe( // ADD SET_SV FOR SE TO STATE TABLE
    SE* se )                    // - state entry for SETSV command
;
void IbpAdd(                    // ADD AN IBRP ENTRY
    SYMBOL binding,             // - symbol to bind reference to
    target_offset_t offset,     // - offset into symbol
    FN_CTL* fctl )              // - current file information
;
void IbpDefineIndex(            // DEFINE CURRENT PARAMETER INDEX
    unsigned index )            // - index
;
void IbpDefineOffset(           // DEFINE OFFSET FOR BOUND REFERENCE PARAMETER
    target_offset_t offset )    // - the offset
;
void IbpDefineParms(            // START DEFINING PARAMETERS
    void )
;
void IbpDefineSym(              // DEFINE SYMBOL FOR BOUND PARAMETER
    call_handle handle,         // - handle for call
    SYMBOL sym )                // - the symbol
;
boolean IbpEmpty(               // DEBUG -- verify empty
    void )
;
cg_name IbpFetchRef(            // FETCH A REFERENCE PARAMETER
    SYMBOL orig_sym )           // - NULL or original symbol
;
cg_name IbpFetchVbRef(          // MAKE A REFERENCE FROM A BOUND PARAMETER
    SYMBOL orig_sym,            // - original symbol
    target_offset_t delta,      // - offset after vb computation
    target_offset_t vb_exact,   // - offset from original sym to base
    target_offset_t vb_offset,  // - offset to vb table
    unsigned vb_index )         // - index in vb table
;
cg_name IbpFetchVfRef(          // FETCH A VIRTUAL FUNCTION ADDRESS
    SYMBOL vfun,                // - virtual function
    cg_name this_expr,          // - expression for this
    SYMBOL vf_this,             // - original symbol (for access)
    target_offset_t vf_offset,  // - offset to vf table ptr
    unsigned vf_index,          // - index in vf table
    boolean* is_vcall,          // - addr[ TRUE ==> real virtual call ]
    target_offset_t* a_adj_this,// - addr[ this adjustment ]
    target_offset_t* a_adj_retn,// - addr[ return adjustment ]
    SYMBOL* a_exact_vfun )      // - addr[ exact vfun to be used ]
;
void IbpFlush(                  // REMOVE ALL IBRP ENTRIES FOR THIS CALL CONTEXT
    FN_CTL* fctl )              // - current file control
;
boolean IbpReference(           // LOCATE A BOUND REFERENCE
    SYMBOL sym,                 // - original symbol
    SYMBOL *trans,              // - addr[ translated symbol ]
    SYMBOL *bound,              // - addr[ bound reference ]
    target_offset_t *offset )   // - addr[ offset from bound reference ]
;
void IbpDefineVbOffset(         // DEFINE OFFSET FOR BOUND VB-REF PARAMETER
    target_offset_t vb_exact )  // - offset from original sym to base
;
void MarkFuncsToGen(            // DETERMINE FUNCTIONS TO BE GENERATED
    unsigned bounding_depth )   // - maximum inline depth
;
cg_name MstabDtorFunStatic(     // DTOR FUNCTION-SCOPE STATIC OBJECT
    SYMBOL sym )                // - object symbol
;
void MstabGenerate(             // GENERATE MODULE STATE TABLE
    void )
;
void MstabInit(                 // INITIALIZE MODULE STATE TABLE
    void )
;
void MstabModDtor(              // DTOR MODULE (FILE-SCOPE) STATIC
    SYMBOL sym )                // - symbol to be dtor'ed
;
void MstabRegister(             // EMIT CODE TO REGISTER MODULE STATE TABLE
    void )
;
OBJ_INIT* ObjInitArray(         // GET OBJ_INIT FOR INDEXING
    void )
;
TYPE ObjInitArrayBaseType(      // GET BASE TYPE FOR ARRAY
    OBJ_INIT* curr )            // - current entry
;
cg_name ObjInitAssignBase(      // ASSIGN BASE REGISTRATION
    FN_CTL* fctl,               // - current function information
    OBJ_INIT* init )            // - initialization element
;
cg_name ObjInitAssignBaseExpr(  // ASSIGN BASE REGISTRATION, FROM EXPR'N
    FN_CTL* fctl,               // - current function information
    OBJ_INIT* init,             // - initialization element
    cg_name expr )              // - expression
;
cg_name ObjInitAssignIndex(     // ASSIGN INDEX TO RT_ARRAY_INIT
    FN_CTL* fctl,               // - current function information
    OBJ_INIT* init,             // - initialization element
    unsigned index )            // - index
;
OBJ_INIT* ObjInitClass(         // GET OBJ_INIT FOR A CLASS
    void )
;
SE* ObjInitDtorAuto(            // UPDATE OBJ_INIT FOR AUTO DTOR
    SE* se,                     // - entry for symbol
    SYMBOL sym )                // - symbol needing dtor
;
OBJ_INIT* ObjInitPush(          // PUSH INITIALIZATION OBJECT (HAS COMPONENTS)
    TYPE obj_type )             // - type of object
;
OBJ_INIT* ObjInitPop(           // POP INITIALIZATION OBJECT (HAS COMPONENTS)
    void )
;
cg_name ObjInitRegActualBase    // REGISTER FOR AN ACTUAL BASE
    ( SE* se )                  // - component for actual base
;
cg_name ObjInitRegisterObj(     // CREATE AN OBJECT REGISTRATION
    FN_CTL* fctl,               // - current function information
    cg_name base_expr,          // - base expression
    boolean use_fun_cdtor )     // - TRUE ==> use CDTOR parm of function
;
OBJ_INIT* ObjInitTop(           // GET TOP INITIALIZATION OBJECT
    void )
;
SE* SeAlloc(                    // ALLOCATE AN SE ENTRY
    uint_8 se_type )            // - code for entry
;
SE* SeSetSvPosition(            // LOCATE STATE ENTRY PAST OPTIONAL SET_SV'S
    SE* se )                    // - starting position
;
STATE_VAR SeStateVar(           // GET STATE VARIABLE AT CURRENT POSITION
    SE* se )                    // - state entry
;
STATE_VAR SeStateOptimal(       // GET STATE VALUE FOR POSITION (OPTIMAL)
    SE* se )                    // - state entry
;
void StabCtlFreeStateTable(     // FREE A STATE TABLE
    STAB_CTL* sctl )            // - state-table information
;
STAB_CTL* StabCtlInit(          // INITIALIZE STAB_CTL
    STAB_CTL* stab,             // - control info: instance
    STAB_DEFN* defn )           // - control information: definition
;
SE* StabCtlPosnGened(           // GET GENERATED POSITION IF REQUIRED
    STAB_CTL* sctl,             // - control info
    SE* src,                    // - source entry
    SE* tgt )                   // - target entry
;
SE* StabCtlPrecedes(            // GET PRECEDING ENTRY OR NULL
    STAB_CTL* sctl,             // - control info
    SE* se )                    // - state entry
;
SE* StabCtlPrevious(            // GET PREVIOUS ENTRY OR NULL
    STAB_CTL* sctl,             // - control info
    SE* se )                    // - state entry
;
void StabCtlPrune(              // PRUNE END OF STATE STATE
    SE* se,                     // - ending entry
    STAB_CTL* sctl )            // - state table definition
;
void StabCtlRemove(             // REMOVE LAST STATE ENTRY
    STAB_CTL* sctl )            // - state-table information
;
SE* StabCtlSkipComp(            // SKIP PAST A COMPONENT
    STAB_CTL* sctl,             // - control info
    SE* se )                    // - state entry
;
SE* StabDefnAddSe(              // ADD STATE ENTRY TO STATE TABLE
    SE* se,                     // - entry to be added
    STAB_DEFN* defn )           // - state table definition
;
STAB_DEFN* StabDefnAllocate(    // ALLOCATE STAB_DEFN
    unsigned kind )             // - kind of table
;
void StabDefnFree(              // FREE AN STAB_DEFN
    STAB_DEFN* defn )           // - entry to be freed
;
void StabDefnFreeStateTable(    // FREE A STATE TABLE
    STAB_DEFN* defn )           // - definition for state table
;
STAB_DEFN* StabDefnInit(        // INITIALIZE STAB_DEFN
    STAB_DEFN* defn,            // - definition
    unsigned kind )             // - kind of table
;
boolean StabGenerate(           // GENERATE A STATE TABLE
    STAB_CTL* sctl )            // - state-table information
;
SE* StateTableActualPosn(       // GET (UN-OPTIMIZED) CURRENT STATE ENTRY
    STAB_CTL* sctl )            // - control info
;
SE* StateTableAdd(              // ADD TO STATE TABLE
    SE* se,                     // - state entry
    STAB_CTL* sctl )            // - state table information
;
SE* StateTableCurrPosn(         // GET STATE ENTRY FOR CURRENT POSITION
    STAB_CTL* sctl )            // - control info
;
void* SymTrans(                 // TRANSLATE SYMBOL/SCOPE
    void *src )                 // - source value
;
#ifndef NDEBUG
void SymTransEmpty(             // DEBUG: VERIFY SYMBOL TRANSLATIONS OVER
    void )
;
#else
    #define SymTransEmpty()
#endif
void SymTransFuncBeg(           // START NEW FUNCTION TRANSLATION
    FN_CTL* fctl )              // - function control
;
void SymTransFuncEnd(           // COMPLETE FUNCTION TRANSLATION
    FN_CTL* fctl )              // - function control
;
void SymTransNewBlock(          // START NEW BLOCK OF TRANSLATIONS
    void )
;
void SymTransPush(              // ADD A SYMBOL TO BE TRANSLATED
    SYMBOL src,                 // - source SYMBOL
    SYMBOL tgt )                // - target SYMBOL
;
cg_name ThrowRo(                // CREATE/ACCESS THROW R/O BLOCK
    TYPE type )                 // - type being thrown
;
void ThrowRoGen(                // GENERATE A THROW R/O BLOCK
    void )
;
TYPE TypeFromCgType(            // GET C++ TYPE FOR cg_type
    cg_type cgtype )            // - code-gen type
;

const char *CallbackName(       // GET CALLBACK FUNCTION NAME
    void *f )                   // - function pointer
;
#endif
