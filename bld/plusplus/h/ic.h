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


// IC.H -- intermediate code definitions
//
// Note: ICODES.H  -- enumerates the code
//
// 91/06/10 -- J.W.Welch        -- defined

#define __x__       NULL
#define OECNT       "ICOPM_OE_COUNT"

// example:
//
// { IC( IC_EOF, NUL, "MASK1|MASK2|MASK3" ) // - end of virtual file (used internally)

//    enum                   code mask
                                          // Control Codes:
                                          // ------------------------------------------
  IC( IC_EOF,                NUL, __x__ ) // - end of virtual file (used internally)
, IC( IC_NEXT,               BIN, __x__ ) // - next block (used internally)
, IC( IC_DEF_SEG,            BIN, __x__ ) // - specify current segment (used internally)
, IC( IC_NO_OP,              NUL, __x__ ) // - no operation
, IC( IC_DTOR_DLT_BEG,       NUL, __x__ ) // - DELETE code in DTOR: start
, IC( IC_DTOR_DLT_END,       NUL, __x__ ) // - DELETE code in DTOR: end
, IC( IC_DTOR_DAR_BEG,       NUL, __x__ ) // - DELETE-ARRAY code in DTOR: start
, IC( IC_DTOR_DAR_END,       NUL, __x__ ) // - DELETE-ARRAY code in DTOR: end
, IC( IC_CTOR_COMP_BEG,      NUL, __x__ ) // - CTOR componentry: start
, IC( IC_CTOR_COMP_END,      NUL, __x__ ) // - CTOR componentry: end
, IC( IC_PCH_STOP,           NUL, __x__ ) // - mark end of CGFILE that was in progress
, IC( IC_ZAP1_REF,           NUL, __x__ ) // - previous instr is zap #1's reference instr
, IC( IC_ZAP2_REF,           NUL, __x__ ) // - previous instr is zap #2's reference instr

                                          // Label Codes:
                                          // ------------------------------------------
, IC( IC_LABGET_CS,          BIN, __x__ ) // - get a control-sequence label
, IC( IC_LABGET_GOTO,        BIN, __x__ ) // - get a number of GOTO labels
, IC( IC_LABFREE_CS,         BIN, __x__ ) // - free a control-sequence label
, IC( IC_LABEL_CS,           BIN, __x__ ) // - set label for GOTO_NEAR
, IC( IC_LABEL_GOTO,         BIN, __x__ ) // - set label for GOTO_NEAR
, IC( IC_LABDEF_CS,          BIN, __x__ ) // - define control-sequence label
, IC( IC_LABDEF_GOTO,        BIN, __x__ ) // - define goto label
, IC( IC_GOTO_NEAR,          BIN, OECNT ) // - goto near label (conditional,unconditional)
, IC( IC_SWITCH_BEG,         NUL, OECNT ) // - switch: start
, IC( IC_SWITCH_END,         NUL, OECNT ) // - switch: end
, IC( IC_SWITCH_CASE,        BIN, OECNT ) // - switch: case
, IC( IC_SWITCH_DEFAULT,     NUL, OECNT ) // - switch: default ( from code )
, IC( IC_SWITCH_OUTSIDE,     BIN, OECNT ) // - switch: default ( generated )

                                          // Primitive (leaf) Codes:
                                          // ------------------------------------------
, IC( IC_LEAF_CONST_INT,     BIN, OECNT ) // - leaf: integer constant
, IC( IC_LEAF_CONST_INT64,   CON, OECNT ) // - leaf: int-64 constant
, IC( IC_LEAF_CONST_FLT,     CON, OECNT ) // - leaf: floating-point constant
, IC( IC_LEAF_NAME_FRONT,    SYM, OECNT ) // - leaf: front-end name
, IC( IC_LEAF_CONST_STR,     STR, OECNT ) // - leaf: string constant
, IC( IC_LEAF_THIS,          NUL, OECNT ) // - leaf: "this" pointer for function
, IC( IC_LEAF_CDTOR_EXTRA,   NUL, OECNT ) // - leaf: extra int parm for ctor/dtor

                                          // Expression-control Codes:
                                          // ------------------------------------------
, IC( IC_SET_TYPE,           TYP, __x__ ) // - set type for upcoming leaves, operations
, IC( IC_EXPR_TRASH,         NUL, __x__ ) // - trash expression
, IC( IC_EXPR_DONE,          NUL, __x__ ) // - expression is now completed
, IC( IC_EXPR_TEMP,          NUL, __x__ ) // - start expression with dtorable temporaries
, IC( IC_LVALUE_TYPE,        TYP, __x__ ) // - set lvalue type
, IC( IC_OPR_TERNARY,        BIN, OECNT ) // - ternary operations
, IC( IC_OPR_BINARY,         BIN, OECNT ) // - binary operations
, IC( IC_OPR_UNARY,          BIN, OECNT ) // - unary operations
, IC( IC_OPR_INDEX,          BIN, __x__ ) // - set type for indexing
, IC( IC_BIT_MASK,           TYP, __x__ ) // - specify bit mask
, IC( IC_COPY_OBJECT,        TYP, OECNT ) // - copy to object
, IC( IC_EXPR_TS,            TYP, __x__ ) // - type-signature referenced
, IC( IC_DTOR_REF,           SYM, __x__ ) // - destructor referenced in tables
, IC( IC_EXPR_CONST,         NUL, __x__ ) // - expression is constant
, IC( IC_EXPR_VOLAT,         NUL, __x__ ) // - expression is volatile

                                          // Routine-calling Codes:
                                          // ------------------------------------------
, IC( IC_PROC_RETURN,        NUL, OECNT ) // - return
, IC( IC_CALL_PARM,          NUL, OECNT ) // - define parameter for call
, IC( IC_CALL_SETUP,         SYM, __x__ ) // - start of call (direct )
, IC( IC_CALL_EXEC,          NUL, OECNT ) // - execute call (direct)
, IC( IC_CALL_SETUP_IND,     TYP, __x__ ) // - start of call (direct )
, IC( IC_CALL_EXEC_IND,      NUL, OECNT ) // - execute call (indirect)
, IC( IC_VIRT_FUNC,          SYM, __x__ ) // - specify virtual function
, IC( IC_VFT_BEG,            SYM, __x__ ) // - start VFT definition
, IC( IC_VFT_REF,            SYM, __x__ ) // - current function references VFT

                                          // Data-generation Codes:
                                          // ------------------------------------------
, IC( IC_DATA_LABEL,         SYM, __x__ ) // - indicate label (start of conglomerate)
, IC( IC_DATA_PTR_OFFSET,    BIN, __x__ ) // - set offset for use with IC_DATA_PTR_SYM
, IC( IC_DATA_PTR_SYM,       SYM, __x__ ) // - generate a pointer relative to symbol
, IC( IC_DATA_PTR_STR,       STR, __x__ ) // - generate a pointer to a string literal
, IC( IC_DATA_INT,           BIN, __x__ ) // - generate an integer
, IC( IC_DATA_INT64,         CON, __x__ ) // - generate an int-64 constant
, IC( IC_DATA_FLT,           CON, __x__ ) // - generate an floating value
, IC( IC_DATA_SIZE,          BIN, __x__ ) // - set size for following instruction
, IC( IC_DATA_TEXT,          STR, __x__ ) // - generate string (e.g., char a[] = "asdf";)
, IC( IC_DATA_REPLICATE,     BIN, __x__ ) // - replicate binary bytes
, IC( IC_DATA_UNDEF,         BIN, __x__ ) // - generate undefined data
, IC( IC_DATA_SEG,           BIN, __x__ ) // - adjust segment id for huge data
, IC( IC_DATA_SYMBOL,        SYM, __x__ ) // - generate for symbol

                                          // Function-related Codes:
                                          // ------------------------------------------
, IC( IC_FUNCTION_OPEN,      SYM, __x__ ) // - open function scope
, IC( IC_FUNCTION_ARGS,      SCP, OECNT ) // - declare function arguments
, IC( IC_FUNCTION_EPILOGUE,  NUL, __x__ ) // - start of function epilogue
, IC( IC_FUNCTION_CLOSE,     NUL, __x__ ) // - close function scope
, IC( IC_FUNCTION_DTM,       BIN, __x__ ) // - set dtor method for function
, IC( IC_DESTRUCT,           SCP, OECNT ) // - destruct up to given symbol
, IC( IC_FUNCTION_STAB,      BIN, __x__ ) // - function requires state table
, IC( IC_COND_TRUE,          BIN, __x__ ) // - start of TRUE part of conditional block
, IC( IC_COND_FALSE,         BIN, __x__ ) // - start of FALSE part of conditional block
, IC( IC_COND_END,           BIN, __x__ ) // - end of conditional block
, IC( IC_CTOR_COMPLETE,      NUL, OECNT ) // - CTOR has completed (before a return)
, IC( IC_DTOR_REG,           NUL, OECNT ) // - generate registration for DTOR
, IC( IC_DTOR_DEREG,         NUL, OECNT ) // - generate deregistration for DTOR
, IC( IC_DTOR_USE,           SYM, __x__ ) // - DTOR will be called (directly or by R/T)
, IC( IC_DTOR_SUBOBJS,       NUL, __x__ ) // - destruct sub-objects
, IC( IC_SET_TRY_STATE,      SYM, OECNT ) // - set function state variable to try
, IC( IC_RESET_THIS,         BIN, OECNT ) // - reset "this" when not inlined
, IC( IC_DTORABLE_INIT,      NUL, OECNT ) // - initialize DTORABLE object
, IC( IC_CTOR_CODE,          NUL, OECNT ) // - start of CTOR code
, IC( IC_DESTRUCT_VAR,       SYM, OECNT ) // - destruct until symbol
, IC( IC_SET_CATCH_STATE,    NUL, OECNT ) // - set state of catch
, IC( IC_GEN_CTOR_DISP,      TYP, OECNT ) // - generate ctor-disp init code for class
, IC( IC_EXACT_VPTR_INIT,    BIN, OECNT ) // - generate exact offset vptr init
, IC( IC_VBASE_VPTR_INIT,    BIN, OECNT ) // - generate vptr init in a vbase
, IC( IC_FUNCTION_RETN,      SYM, __x__ ) // - function return symbol

                                          // Scope-related Codes:
                                          // ------------------------------------------
, IC( IC_BLOCK_OPEN,         SCP, __x__ ) // - open block scope
, IC( IC_BLOCK_CLOSE,        NUL, __x__ ) // - close block scope
, IC( IC_BLOCK_SRC,          SCP, __x__ ) // - set scope for source (for jump)
, IC( IC_BLOCK_DEAD,         SCP, __x__ ) // - start of block in dead-code
, IC( IC_BLOCK_DONE,         NUL, __x__ ) // - complete insignificant scope
, IC( IC_BLOCK_END,          SCP, __x__ ) // - end of block when dead code
, IC( IC_DTOR_AUTO,          SYM, OECNT ) // - register DTOR of auto symbol
, IC( IC_DTOR_STATIC,        SYM, OECNT ) // - register DTOR of static symbol
, IC( IC_DTOR_TEMP,          SYM, OECNT ) // - destruct temporary

                                          // Debugging Codes:
                                          // ------------------------------------------
, IC( IC_DBG_LINE,           BIN, __x__ ) // - set debugging line number
, IC( IC_DBG_SRCFILE,        SRC, __x__ ) // - set source-file for line

                                          // Data Initialization Codes
                                          // ------------------------------------------
, IC( IC_INIT_BEG,           SYM, __x__ ) // - indicate begin of initialization
, IC( IC_INIT_DONE,          NUL, __x__ ) // - indicate end of initialization
, IC( IC_INIT_TEST,          SYM, OECNT ) // - start of init-test for function static
, IC( IC_INIT_REF_BEG,       SYM, OECNT ) // - start init of dtor-temp for ref.init
, IC( IC_INIT_REF_END,       NUL, __x__ ) // - complete init of dtor-temp for ref.init
, IC( IC_INIT_SYM_BEG,       NUL, OECNT ) // - start of init of symbol
, IC( IC_INIT_SYM_END,       SYM, __x__ ) // - end of init of symbol
, IC( IC_COMPCTOR_BEG,       BIN, OECNT ) // - start component ctor with dtorable temp.s
, IC( IC_COMPCTOR_END,       BIN, __x__ ) // - end component ctor with dtorable temp.s
, IC( IC_DTOBJ_PUSH,         TYP, OECNT ) // - dtorable object: start initialization
, IC( IC_DTOBJ_POP,          NUL, __x__ ) // - dtorable object: complete initialization
, IC( IC_DTOBJ_SYM,          SYM, __x__ ) // - dtorable object: symbol for initialization
, IC( IC_DTOBJ_OFF,          BIN, __x__ ) // - dtorable object: offset for initialization
, IC( IC_DTARRAY_INDEX,      BIN, __x__ ) // - dtorable component: array element initialized

                                          // Handling of inline arguments
                                          // ------------------------------------------
, IC( IC_RARG_FETCH,         SYM, OECNT ) // - fetch reference argument
, IC( IC_RARG_PARM,          SYM, __x__ ) // - reference arg. is an argument
, IC( IC_RARG_SYM,           SYM, __x__ ) // - reference arg. is a symbol
, IC( IC_RARG_FUNC,          BIN, __x__ ) // - corresponding symbol in function scope
, IC( IC_RARG_OFFSET,        BIN, __x__ ) // - set offset from a reference argument

                                          // Exception Handling
                                          // ------------------------------------------
, IC( IC_TRY,                SYM, OECNT ) // - try block: start
, IC( IC_CTOR_END,           NUL, OECNT ) // - end of object CTOR
, IC( IC_CATCH_VAR,          SYM, OECNT ) // - set variable for next catch
, IC( IC_CATCH,              TYP, OECNT ) // - catch block
, IC( IC_EXCEPT_SPEC,        TYP, OECNT ) // - function exception specification (type)
, IC( IC_EXCEPT_FUN,         NUL, OECNT ) // - completion of above
, IC( IC_THROW_RO_BLK,       TYP, __x__ ) // - throw's R/O block
, IC( IC_AFTER_ABORTS,       NUL, OECNT ) // - reset after aborts
, IC( IC_AFTER_THROW,        NUL, OECNT ) // - reset after C++ throw
, IC( IC_TRY_DONE,           SYM, OECNT ) // - completion of try block
, IC( IC_SETJMP_DTOR,        NUL, OECNT ) // - destruction for setjmp != 0 return
, IC( IC_SET_LABEL_SV,       NUL, __x__ ) // - set state at label
, IC( IC_TRY_CATCH_DONE,     SCP, OECNT ) // - try/catch completed, w/o dtoring

                                          // Based Operations
                                          // ------------------------------------------
, IC( IC_SEGOP_SEG,          BIN, __x__ ) // - set segment #
, IC( IC_SEGNAME,            SYM, __x__ ) // - __segname( ... ) ==> segment

                                          // Operations for new and new[]
                                          // -----------------------------
, IC( IC_NEW_ALLOC,          NUL, __x__ ) // - signal new expression (to be duplicated)
, IC( IC_NEW_CTORED,         TYP, __x__ ) // - signal completion of new ctor

                                          // Operations for virtual bases
                                          // -----------------------------
, IC( IC_VB_EXACT,           BIN, __x__ ) // - virtual base: type
, IC( IC_VB_DELTA,           BIN, __x__ ) // - virtual base: virtual delta
, IC( IC_VB_OFFSET,          BIN, __x__ ) // - virtual base: virtual offset
, IC( IC_VB_INDEX,           BIN, __x__ ) // - virtual base: exact index
, IC( IC_VB_FETCH,           SYM, OECNT ) // - compute virtual base from argument
, IC( IC_RARG_VBOFFSET,      BIN, __x__ ) // - set IBP offset for IC_VB_FETCH parm.

                                          // Operations for virtual functions
                                          // ---------------------------------
, IC( IC_VF_OFFSET,          BIN, __x__ ) // - virtual fun: offset to vft
, IC( IC_VF_INDEX,           BIN, __x__ ) // - virtual fun: index in vft
, IC( IC_VF_THIS,            SYM, __x__ ) // - virtual fun: this arg.
, IC( IC_SETUP_VFUN,         SYM, __x__ ) // - virtual fun: setup for call
, IC( IC_CALL_EXEC_VFUN,     NUL, __x__ ) // - execute call (virtual function)
, IC( IC_VF_THIS_ADJ,        NUL, __x__ ) // - do "this" adjustment for virt call
, IC( IC_VF_CODED,           NUL, __x__ ) // - virtual call has been coded
, IC( IC_VFUN_PTR,           NUL, __x__ ) // - specify vfun "this"

                                          // Operations on extra CTOR/DTOR arg
                                          // ------------------------------------------
, IC( IC_CDARG_TEST_ON,      BIN, __x__ ) // - code for CDARG test, branch if on
, IC( IC_CDARG_TEST_OFF,     BIN, __x__ ) // - code for CDARG test, branch if off
, IC( IC_CDARG_LABEL,        NUL, __x__ ) // - emit label for CDARG test
, IC( IC_CDARG_VAL,          BIN, OECNT ) // - leaf: CDTOR arg. value
, IC( IC_CDARG_FETCH,        BIN, OECNT ) // - specify CDTOR arg. value

                                          // Decoration for scope-call optimizations
                                          // ----------------------------------------
, IC( IC_SCOPE_CALL_FUN,     SYM, __x__ ) // - function for scope-call optimization
, IC( IC_SCOPE_CALL_GEN,     NUL, __x__ ) // - indicate don't optimize
, IC( IC_SCOPE_CALL_TDTOR,   SYM, __x__ ) // - temporary dtor for scope-call optimization
, IC( IC_SCOPE_CALL_BDTOR,   SYM, __x__ ) // - scope dtor for scope-call optimization
, IC( IC_SCOPE_THROW,        NUL, __x__ ) // - indicate a THROW or equivalent
, IC( IC_SCOPE_CALL_CDTOR,   SYM, __x__ ) // - component dtor for scope-call optimization
, IC( IC_STMT_SCOPE_END,     NUL, __x__ ) // - signal end of statement scope

                                          // Operations for delete and delete[]
                                          // -----------------------------------
, IC( IC_DLT_DTOR_ARR,       SYM, __x__ ) // - start dtoring array to be deleted
, IC( IC_DLT_DTOR_ELM,       SYM, __x__ ) // - start dtoring element to be deleted
, IC( IC_DLT_DTOR_SIZE,      BIN, __x__ ) // - size of dtoring element in delete area
, IC( IC_DLT_DTORED,         NUL, __x__ ) // - completed dtoring of object be deleted

                                          // IC parm stack support (fixed size depth)
                                          // ------------------------------------------
, IC( IC_PARM_BIN,           BIN, __x__ ) // - push an integer on the IC code parm stack
, IC( IC_PARM_SYM,           SYM, __x__ ) // - push a SYMBOL on the IC code parm stack
, IC( IC_PARM_TYP,           TYP, __x__ ) // - push a TYPE on the IC code parm stack
, IC( IC_PARM_SCP,           SCP, __x__ ) // - push a SCOPE on the IC code parm stack
, IC( IC_PARM_XXX,           NUL, __x__ ) // - padding

#if defined(_IN_ICMASK) || !defined(NDEBUG)
                                          // Internal Debugging
                                          // ------------------------------------------
, IC( IC_TRACE_BEG,          NUL, __x__ ) // - start IC instruction trace
, IC( IC_TRACE_END,          NUL, __x__ ) // - stop IC instruction trace
#endif
                                          // virtual function thunk adjustments
                                          // ------------------------------------------
, IC( IC_VTHUNK_MDELTA,      BIN, OECNT ) // - this -= delta;
, IC( IC_VTHUNK_PDELTA,      BIN, OECNT ) // - this += delta;
, IC( IC_VTHUNK_CDISP,       NUL, OECNT ) // - this -= *((unsigned*)this-sizeof(unsigned))
, IC( IC_VTHUNK_VBASE,       BIN, OECNT ) // - this += (*this)[i]

#if defined(_IN_ICMASK) || _CPU == _AXP
                                          // ALPHA-SPECIFIC CODES
                                          // ------------------------------------------
, IC( IC_ALLOCA,             BIN, OECNT ) // - support for alloca
#endif
                                          // return optimizations
                                          // ------------------------------------------
, IC( IC_RETNOPT_BEG,        NUL, __x__ ) // - return sequence: start
, IC( IC_RETNOPT_END,        NUL, __x__ ) // - end marker
, IC( IC_RETNOPT_VAR,        SYM, __x__ ) // - sequence for var, not optimized away

                                          // RTTI
                                          // ------------------------------------------
, IC( IC_RTTI_REF,           SYM, __x__ ) // - reference RTTI info sym

                                          // PCH opcodes
                                          // ------------------------------------------
, IC( IC_PCH_PAD,            NUL, __x__ ) // - used to pad out CGFILE PCH

                                          // BROWSE-INFO OPCODES
                                          // ------------------------------------------
, IC( IC_BR_SRC_BEG,         BIN, __x__ ) // - start of browse info for a source
, IC( IC_BR_SRC_END,         NUL, __x__ ) // - end of browse info for a source
, IC( IC_BR_INC_SRC,         BIN, __x__ ) // - source name for inclusion
, IC( IC_BR_DCL_CLASS,       TYP, __x__ ) // - declare: class
, IC( IC_BR_DCL_MACRO,       BIN, __x__ ) // - declare: macro
, IC( IC_BR_DCL_TDEF,        SYM, __x__ ) // - declare: typedef
, IC( IC_BR_DCL_VAR,         SYM, __x__ ) // - declare: variable
, IC( IC_BR_DCL_FUN,         SYM, __x__ ) // - declare: function
, IC( IC_BR_REF_LOC,         BIN, __x__ ) // - reference: function
, IC( IC_BR_REF_FUN,         SYM, __x__ ) // - reference: function
, IC( IC_BR_REF_CLM,         SYM, __x__ ) // - reference: class data member
, IC( IC_BR_REF_VAR,         SYM, __x__ ) // - reference: variable
, IC( IC_BR_REF_TYPE,        TYP, __x__ ) // - reference: type
, IC( IC_BR_REF_EVAL,        SYM, __x__ ) // - reference: enumeration value
, IC( IC_BR_REF_EVAR,        SYM, __x__ ) // - reference: enumeration variable
, IC( IC_BR_REF_MACRO,       BIN, __x__ ) // - reference: macro value
, IC( IC_BR_REF_UMACRO,      BIN, __x__ ) // - reference: undefined macro
, IC( IC_BR_REFTO_FILE,      SRC, __x__ ) // - reference to file
, IC( IC_BR_REFTO_LINE,      BIN, __x__ ) // - reference to line
, IC( IC_BR_REFTO_COL,       BIN, __x__ ) // - reference to column
, IC( IC_BR_REFTO_LINECOL,   BIN, __x__ ) // - reference to line(3 bytes) col(1 byte)
, IC( IC_BR_SCOPE_OPN,       SCP, __x__ ) // - open scope
, IC( IC_BR_SCOPE_CLS,       SCP, __x__ ) // - close scope
, IC( IC_BR_NO_OP,           NUL, __x__ ) // - no operation

                                          // Add new opcodes here to avoid global re-compile
                                          // ------------------------------------------
, IC( IC_TYPEID_REF,         SYM, __x__ ) // - typeid lvalue referenced
, IC( IC_BR_PCH,             BIN, __x__ ) // - PCH file reference

                                          // Padding for patching
                                          // ------------------------------------------
, IC( IC_INIT_AUTO,          SYM, __x__ ) // - copy static init block to this auto symbol
, IC( IC_CALL_PARM_FLT,      NUL, __x__ ) // - define parameter for call (set float_used)
, IC( IC_DTOR_KIND,          BIN, OECNT ) // - kind of dtor following a ctor call

//, IC( IC_EXTRA1,             NUL, __x__ )
//, IC( IC_EXTRA2,             NUL, __x__ )
//, IC( IC_EXTRA3,             NUL, __x__ )
, IC( IC_EXTRA4,             NUL, __x__ )
, IC( IC_EXTRA5,             NUL, __x__ )
, IC( IC_EXTRA6,             NUL, __x__ )
, IC( IC_EXTRA7,             NUL, __x__ )
, IC( IC_EXTRA8,             NUL, __x__ )
, IC( IC_EXTRA9,             NUL, __x__ )

                                          // End of Codes
                                          // ------------------------------------------
, IC( IC_END,                NUL, __x__ ) // - can be used as terminator
