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
// note: this file is never included directly from a module
//
//  ICODES.H  -- enumerates the code
//  INAMES.H  -- generates a vector of opcode names
//  ITYPVEC.H -- generates a vector of opcode types
//
//
//
// 91/06/10 -- J.W.Welch        -- defined

//
// The "//|" comments apply to the IC code immediately preceeding it.
// The ICMASK program will scan these comments for ids that
// should be used when building a set of operator masks for
// each operator.  The comment can be omitted if no masks have
// to be specified for the IC code.  They can also be repeated
// if more room is necessary to specify more ids.
//

// example:
//
// { IC( IC_EOF, NUL )             // - end of virtual file (used internally)
// //| MASK1 MASK2 MASK3
// //| MASK4

                                // Control Codes:
                                //------------------------------------------
{ IC( IC_EOF, NUL )             // - end of virtual file (used internally)
, IC( IC_NEXT, BIN )            // - next block (used internally)
, IC( IC_DEF_SEG, BIN )         // - specify current segment (used internally)
, IC( IC_NO_OP, NUL )           // - no operation
, IC( IC_DTOR_DLT_BEG, NUL )    // - DELETE code in DTOR: start
, IC( IC_DTOR_DLT_END, NUL )    // - DELETE code in DTOR: end
, IC( IC_DTOR_DAR_BEG, NUL )    // - DELETE-ARRAY code in DTOR: start
, IC( IC_DTOR_DAR_END, NUL )    // - DELETE-ARRAY code in DTOR: end
, IC( IC_CTOR_COMP_BEG, NUL )   // - CTOR componentry: start
, IC( IC_CTOR_COMP_END, NUL )   // - CTOR componentry: end
, IC( IC_PCH_STOP, NUL )        // - mark end of CGFILE that was in progress
, IC( IC_ZAP1_REF, NUL )        // - previous instr is zap #1's reference instr
, IC( IC_ZAP2_REF, NUL )        // - previous instr is zap #2's reference instr

                                // Label Codes:
                                //------------------------------------------
, IC( IC_LABGET_CS, BIN )       // - get a control-sequence label
, IC( IC_LABGET_GOTO, BIN )     // - get a number of GOTO labels
, IC( IC_LABFREE_CS, BIN )      // - free a control-sequence label
, IC( IC_LABEL_CS, BIN )        // - set label for GOTO_NEAR
, IC( IC_LABEL_GOTO, BIN )      // - set label for GOTO_NEAR
, IC( IC_LABDEF_CS, BIN )       // - define control-sequence label
, IC( IC_LABDEF_GOTO, BIN )     // - define goto label
, IC( IC_GOTO_NEAR, BIN )       // - goto near label (conditional,unconditional)
//| OE_COUNT
, IC( IC_SWITCH_BEG, NUL )      // - switch: start
//| OE_COUNT
, IC( IC_SWITCH_END, NUL )      // - switch: end
//| OE_COUNT
, IC( IC_SWITCH_CASE, BIN )     // - switch: case
//| OE_COUNT
, IC( IC_SWITCH_DEFAULT, NUL )  // - switch: default ( from code )
//| OE_COUNT
, IC( IC_SWITCH_OUTSIDE, BIN )  // - switch: default ( generated )
//| OE_COUNT

                                // Primitive (leaf) Codes:
                                //------------------------------------------
, IC( IC_LEAF_CONST_INT, BIN )  // - leaf: integer constant
//| OE_COUNT
, IC( IC_LEAF_CONST_INT64, CON )// - leaf: int-64 constant
//| OE_COUNT
, IC( IC_LEAF_CONST_FLT, CON )  // - leaf: floating-point constant
//| OE_COUNT
, IC( IC_LEAF_NAME_FRONT, SYM ) // - leaf: front-end name
//| OE_COUNT
, IC( IC_LEAF_CONST_STR, STR )  // - leaf: string constant
//| OE_COUNT
, IC( IC_LEAF_THIS, NUL )       // - leaf: "this" pointer for function
//| OE_COUNT
, IC( IC_LEAF_CDTOR_EXTRA, NUL )// - leaf: extra int parm for ctor/dtor
//| OE_COUNT

                                // Expression-control Codes:
                                //------------------------------------------
, IC( IC_SET_TYPE, TYP )        // - set type for upcoming leaves, operations
, IC( IC_EXPR_TRASH, NUL )      // - trash expression
, IC( IC_EXPR_DONE, NUL )       // - expression is now completed
, IC( IC_EXPR_TEMP, NUL )       // - start expression with dtorable temporaries
, IC( IC_LVALUE_TYPE, TYP )     // - set lvalue type
, IC( IC_OPR_TERNARY, BIN )     // - ternary operations
//| OE_COUNT
, IC( IC_OPR_BINARY, BIN )      // - binary operations
//| OE_COUNT
, IC( IC_OPR_UNARY, BIN )       // - unary operations
//| OE_COUNT
, IC( IC_OPR_INDEX, BIN )       // - set type for indexing
, IC( IC_BIT_MASK, TYP )        // - specify bit mask
, IC( IC_COPY_OBJECT, TYP )     // - copy to object
//| OE_COUNT
, IC( IC_EXPR_TS, TYP )         // - type-signature referenced
, IC( IC_DTOR_REF, SYM )        // - destructor referenced in tables
, IC( IC_EXPR_CONST, NUL )      // - expression is constant
, IC( IC_EXPR_VOLAT, NUL )      // - expression is volatile

                                // Routine-calling Codes:
                                //------------------------------------------
, IC( IC_PROC_RETURN, NUL )     // - return
//| OE_COUNT
, IC( IC_CALL_PARM, NUL )       // - define parameter for call
//| OE_COUNT
, IC( IC_CALL_SETUP, SYM )      // - start of call (direct )
, IC( IC_CALL_EXEC, NUL )       // - execute call (direct)
//| OE_COUNT
, IC( IC_CALL_SETUP_IND, TYP )  // - start of call (direct )
, IC( IC_CALL_EXEC_IND, NUL )   // - execute call (indirect)
//| OE_COUNT
, IC( IC_VIRT_FUNC, SYM )       // - specify virtual function
, IC( IC_VFT_BEG, SYM )         // - start VFT definition
, IC( IC_VFT_REF, SYM )         // - current function references VFT

                                // Data-generation Codes:
                                //------------------------------------------
, IC( IC_DATA_LABEL, SYM )      // - indicate label (start of conglomerate)
, IC( IC_DATA_PTR_OFFSET, BIN ) // - set offset for use with IC_DATA_PTR_SYM
, IC( IC_DATA_PTR_SYM, SYM )    // - generate a pointer relative to symbol
, IC( IC_DATA_PTR_STR, STR )    // - generate a pointer to a string literal
, IC( IC_DATA_INT, BIN )        // - generate an integer
, IC( IC_DATA_INT64, CON )      // - generate an int-64 constant
, IC( IC_DATA_FLT, CON )        // - generate an floating value
, IC( IC_DATA_SIZE, BIN )       // - set size for following instruction
, IC( IC_DATA_TEXT, STR )       // - generate string (e.g., char a[] = "asdf";)
, IC( IC_DATA_REPLICATE, BIN )  // - replicate binary bytes
, IC( IC_DATA_UNDEF, BIN )      // - generate undefined data
, IC( IC_DATA_SEG, BIN )        // - adjust segment id for huge data
, IC( IC_DATA_SYMBOL, SYM )     // - generate for symbol

                                // Function-related Codes:
                                //------------------------------------------
, IC( IC_FUNCTION_OPEN, SYM )   // - open function scope
, IC( IC_FUNCTION_ARGS, SCP )   // - declare function arguments
//| OE_COUNT
, IC( IC_FUNCTION_EPILOGUE, NUL)// - start of function epilogue
, IC( IC_FUNCTION_CLOSE, NUL )  // - close function scope
, IC( IC_FUNCTION_DTM, BIN )    // - set dtor method for function
, IC( IC_DESTRUCT, SCP )        // - destruct up to given symbol
//| OE_COUNT
, IC( IC_FUNCTION_STAB, BIN )   // - function requires state table
, IC( IC_COND_TRUE, BIN )       // - start of TRUE part of conditional block
, IC( IC_COND_FALSE, BIN )      // - start of FALSE part of conditional block
, IC( IC_COND_END, BIN )        // - end of conditional block
, IC( IC_CTOR_COMPLETE, NUL )   // - CTOR has completed (before a return)
//| OE_COUNT
, IC( IC_DTOR_REG, NUL )        // - generate registration for DTOR
//| OE_COUNT
, IC( IC_DTOR_DEREG, NUL )      // - generate deregistration for DTOR
//| OE_COUNT
, IC( IC_DTOR_USE, SYM )        // - DTOR will be called (directly or by R/T)
, IC( IC_DTOR_SUBOBJS, NUL )    // - destruct sub-objects
, IC( IC_SET_TRY_STATE, SYM )   // - set function state variable to try
//| OE_COUNT
, IC( IC_RESET_THIS, BIN )      // - reset "this" when not inlined
//| OE_COUNT
, IC( IC_DTORABLE_INIT, NUL )   // - initialize DTORABLE object
//| OE_COUNT
, IC( IC_CTOR_CODE, NUL )       // - start of CTOR code
//| OE_COUNT
, IC( IC_DESTRUCT_VAR, SYM )    // - destruct until symbol
//| OE_COUNT
, IC( IC_SET_CATCH_STATE, NUL ) // - set state of catch
//| OE_COUNT
, IC( IC_GEN_CTOR_DISP, TYP )   // - generate ctor-disp init code for class
//| OE_COUNT
, IC( IC_EXACT_VPTR_INIT, BIN ) // - generate exact offset vptr init
//| OE_COUNT
, IC( IC_VBASE_VPTR_INIT, BIN ) // - generate vptr init in a vbase
//| OE_COUNT
, IC( IC_FUNCTION_RETN, SYM )   // - function return symbol

                                // Scope-related Codes:
                                //------------------------------------------
, IC( IC_BLOCK_OPEN, SCP )      // - open block scope
, IC( IC_BLOCK_CLOSE, NUL )     // - close block scope
, IC( IC_BLOCK_SRC, SCP )       // - set scope for source (for jump)
, IC( IC_BLOCK_DEAD, SCP )      // - start of block in dead-code
, IC( IC_BLOCK_DONE, NUL )      // - complete insignificant scope
, IC( IC_BLOCK_END, SCP )       // - end of block when dead code
, IC( IC_DTOR_AUTO, SYM )       // - register DTOR of auto symbol
//| OE_COUNT
, IC( IC_DTOR_STATIC, SYM )     // - register DTOR of static symbol
//| OE_COUNT
, IC( IC_DTOR_TEMP, SYM )       // - destruct temporary
//| OE_COUNT

                                // Debugging Codes:
                                //------------------------------------------
, IC( IC_DBG_LINE, BIN )        // - set debugging line number
, IC( IC_DBG_SRCFILE, SRC )     // - set source-file for line

                                // Data Initialization Codes
                                //------------------------------------------
, IC( IC_INIT_BEG, SYM )        // - indicate begin of initialization
, IC( IC_INIT_DONE, NUL )       // - indicate end of initialization
, IC( IC_INIT_TEST, SYM )       // - start of init-test for function static
//| OE_COUNT
, IC( IC_INIT_REF_BEG, SYM )    // - start init of dtor-temp for ref.init
//| OE_COUNT
, IC( IC_INIT_REF_END, NUL )    // - complete init of dtor-temp for ref.init
, IC( IC_INIT_SYM_BEG, NUL )    // - start of init of symbol
//| OE_COUNT
, IC( IC_INIT_SYM_END, SYM )    // - end of init of symbol
, IC( IC_COMPCTOR_BEG, BIN )    // - start component ctor with dtorable temp.s
//| OE_COUNT
, IC( IC_COMPCTOR_END, BIN )    // - end component ctor with dtorable temp.s
, IC( IC_DTOBJ_PUSH, TYP )      // - dtorable object: start initialization
//| OE_COUNT
, IC( IC_DTOBJ_POP, NUL )       // - dtorable object: complete initialization
, IC( IC_DTOBJ_SYM, SYM )       // - dtorable object: symbol for initialization
, IC( IC_DTOBJ_OFF, BIN )       // - dtorable object: offset for initialization
, IC( IC_DTARRAY_INDEX, BIN )   // - dtorable component: array element initialized

                                // Handling of inline arguments
                                //------------------------------------------
, IC( IC_RARG_FETCH, SYM )      // - fetch reference argument
//| OE_COUNT
, IC( IC_RARG_PARM, SYM )       // - reference arg. is an argument
, IC( IC_RARG_SYM, SYM )        // - reference arg. is a symbol
, IC( IC_RARG_FUNC, BIN )       // - corresponding symbol in function scope
, IC( IC_RARG_OFFSET, BIN )     // - set offset from a reference argument

                                // Exception Handling
                                //------------------------------------------
, IC( IC_TRY, SYM )             // - try block: start
//| OE_COUNT
, IC( IC_CTOR_END, NUL )        // - end of object CTOR
//| OE_COUNT
, IC( IC_CATCH_VAR, SYM )       // - set variable for next catch
//| OE_COUNT
, IC( IC_CATCH, TYP )           // - catch block
//| OE_COUNT
, IC( IC_EXCEPT_SPEC, TYP )     // - function exception specification (type)
//| OE_COUNT
, IC( IC_EXCEPT_FUN, NUL )      // - completion of above
//| OE_COUNT
, IC( IC_THROW_RO_BLK, TYP )    // - throw's R/O block
, IC( IC_AFTER_ABORTS, NUL )    // - reset after aborts
//| OE_COUNT
, IC( IC_AFTER_THROW, NUL )     // - reset after C++ throw
//| OE_COUNT
, IC( IC_TRY_DONE, SYM )        // - completion of try block
//| OE_COUNT
, IC( IC_SETJMP_DTOR, NUL )     // - destruction for setjmp != 0 return
//| OE_COUNT
, IC( IC_SET_LABEL_SV, NUL )    // - set state at label
, IC( IC_TRY_CATCH_DONE, SCP )  // - try/catch completed, w/o dtoring
//| OE_COUNT

                                // Based Operations
                                //------------------------------------------
, IC( IC_SEGOP_SEG, BIN )       // - set segment #
, IC( IC_SEGNAME, SYM )         // - __segname( ... ) ==> segment

                                // Operations for new and new[]
                                //-----------------------------
, IC( IC_NEW_ALLOC, NUL )       // - signal new expression (to be duplicated)
, IC( IC_NEW_CTORED, TYP )      // - signal completion of new ctor

                                // Operations for virtual bases
                                //-----------------------------
, IC( IC_VB_EXACT, BIN )        // - virtual base: type
, IC( IC_VB_DELTA, BIN )        // - virtual base: virtual delta
, IC( IC_VB_OFFSET, BIN )       // - virtual base: virtual offset
, IC( IC_VB_INDEX, BIN )        // - virtual base: exact index
, IC( IC_VB_FETCH, SYM )        // - compute virtual base from argument
//| OE_COUNT
, IC( IC_RARG_VBOFFSET, BIN )   // - set IBP offset for IC_VB_FETCH parm.

                                // Operations for virtual functions
                                //---------------------------------
, IC( IC_VF_OFFSET, BIN )       // - virtual fun: offset to vft
, IC( IC_VF_INDEX, BIN )        // - virtual fun: index in vft
, IC( IC_VF_THIS, SYM )         // - virtual fun: this arg.
, IC( IC_SETUP_VFUN, SYM )      // - virtual fun: setup for call
, IC( IC_CALL_EXEC_VFUN, NUL )  // - execute call (virtual function)
, IC( IC_VF_THIS_ADJ, NUL )     // - do "this" adjustment for virt call
, IC( IC_VF_CODED, NUL )        // - virtual call has been coded
, IC( IC_VFUN_PTR, NUL )        // - specify vfun "this"

                                // Operations on extra CTOR/DTOR arg
                                //------------------------------------------
, IC( IC_CDARG_TEST_ON, BIN )   // - code for CDARG test, branch if on
, IC( IC_CDARG_TEST_OFF, BIN )  // - code for CDARG test, branch if off
, IC( IC_CDARG_LABEL, NUL )     // - emit label for CDARG test
, IC( IC_CDARG_VAL, BIN )       // - leaf: CDTOR arg. value
//| OE_COUNT
, IC( IC_CDARG_FETCH, BIN )     // - specify CDTOR arg. value
//| OE_COUNT

                                // Decoration for scope-call optimizations
                                //----------------------------------------
, IC( IC_SCOPE_CALL_FUN, SYM )  // - function for scope-call optimization
, IC( IC_SCOPE_CALL_GEN, NUL )  // - indicate don't optimize
, IC( IC_SCOPE_CALL_TDTOR, SYM )// - temporary dtor for scope-call optimization
, IC( IC_SCOPE_CALL_BDTOR, SYM )// - scope dtor for scope-call optimization
, IC( IC_SCOPE_THROW, NUL )     // - indicate a THROW or equivalent
, IC( IC_SCOPE_CALL_CDTOR, SYM )// - component dtor for scope-call optimization
, IC( IC_STMT_SCOPE_END, NUL )  // - signal end of statement scope

                                // Operations for delete and delete[]
                                //-----------------------------------
, IC( IC_DLT_DTOR_ARR, SYM )    // - start dtoring array to be deleted
, IC( IC_DLT_DTOR_ELM, SYM )    // - start dtoring element to be deleted
, IC( IC_DLT_DTOR_SIZE, BIN)    // - size of dtoring element in delete area
, IC( IC_DLT_DTORED, NUL )      // - completed dtoring of object be deleted

                                // IC parm stack support (fixed size depth)
                                //------------------------------------------
, IC( IC_PARM_BIN, BIN )        // push an integer on the IC code parm stack
, IC( IC_PARM_SYM, SYM )        // push a SYMBOL on the IC code parm stack
, IC( IC_PARM_TYP, TYP )        // push a TYPE on the IC code parm stack
, IC( IC_PARM_SCP, SCP )        // push a SCOPE on the IC code parm stack
, IC( IC_PARM_XXX, NUL )        // padding

#if defined(_IN_ICMASK) || !defined(NDEBUG)
                                // Internal Debugging
                                //------------------------------------------
, IC( IC_TRACE_BEG, NUL )       // - start IC instruction trace
, IC( IC_TRACE_END, NUL )       // - stop IC instruction trace

#endif

                                // virtual function thunk adjustments
                                //------------------------------------------
, IC( IC_VTHUNK_MDELTA, BIN )   // - this -= delta;
//| OE_COUNT
, IC( IC_VTHUNK_PDELTA, BIN )   // - this += delta;
//| OE_COUNT
, IC( IC_VTHUNK_CDISP, NUL )    // - this -= *((unsigned*)this-sizeof(unsigned))
//| OE_COUNT
, IC( IC_VTHUNK_VBASE, BIN )    // - this += (*this)[i]
//| OE_COUNT

#if defined(_IN_ICMASK) || _CPU == _AXP
                                // ALPHA-SPECIFIC CODES
                                //------------------------------------------
, IC( IC_ALLOCA, BIN )          // - support for alloca
//| OE_COUNT

#endif


                                // return optimizations
                                //------------------------------------------
, IC( IC_RETNOPT_BEG,NUL )      // - return sequence: start
, IC( IC_RETNOPT_END,NUL )      // - end marker
, IC( IC_RETNOPT_VAR,SYM )      // - sequence for var, not optimized away

                                // RTTI
                                //------------------------------------------
, IC( IC_RTTI_REF, SYM )        // - reference RTTI info sym

                                // PCH opcodes
                                //------------------------------------------
, IC( IC_PCH_PAD, NUL )         // - used to pad out CGFILE PCH

                                // BROWSE-INFO OPCODES
                                //------------------------------------------
, IC( IC_BR_SRC_BEG, BIN )      // - start of browse info for a source
, IC( IC_BR_SRC_END, NUL )      // - end of browse info for a source
, IC( IC_BR_INC_SRC, BIN )      // - source name for inclusion
, IC( IC_BR_DCL_CLASS, TYP )    // - declare: class
, IC( IC_BR_DCL_MACRO, BIN )    // - declare: macro
, IC( IC_BR_DCL_TDEF, SYM )     // - declare: typedef
, IC( IC_BR_DCL_VAR, SYM )      // - declare: variable
, IC( IC_BR_DCL_FUN, SYM )      // - declare: function
, IC( IC_BR_REF_LOC, BIN )      // - reference: function
, IC( IC_BR_REF_FUN, SYM )      // - reference: function
, IC( IC_BR_REF_CLM, SYM )      // - reference: class data member
, IC( IC_BR_REF_VAR, SYM )      // - reference: variable
, IC( IC_BR_REF_TYPE, TYP )     // - reference: type
, IC( IC_BR_REF_EVAL, SYM )     // - reference: enumeration value
, IC( IC_BR_REF_EVAR, SYM )     // - reference: enumeration variable
, IC( IC_BR_REF_MACRO, BIN )    // - reference: macro value
, IC( IC_BR_REF_UMACRO, BIN )   // - reference: undefined macro
, IC( IC_BR_REFTO_FILE, SRC )   // - reference to file
, IC( IC_BR_REFTO_LINE, BIN )   // - reference to line
, IC( IC_BR_REFTO_COL, BIN )    // - reference to column
, IC( IC_BR_REFTO_LINECOL, BIN) // - reference to line(3 bytes) col(1 byte)
, IC( IC_BR_SCOPE_OPN, SCP )    // - open scope
, IC( IC_BR_SCOPE_CLS, SCP )    // - close scope
, IC( IC_BR_NO_OP, NUL )        // - no operation

                                // Add new opcodes here to avoid global re-compile
                                //------------------------------------------

, IC( IC_TYPEID_REF, SYM )      // - typeid lvalue referenced
, IC( IC_BR_PCH, BIN )          // - PCH file reference

                                // Padding for patching
                                //------------------------------------------

, IC( IC_INIT_AUTO, SYM )       // - copy static init block to this auto symbol
, IC( IC_CALL_PARM_FLT, NUL )   // - define parameter for call (set float_used)
, IC( IC_DTOR_KIND, BIN )       // - kind of dtor following a ctor call
//| OE_COUNT

//, IC( IC_EXTRA1, NUL )
//, IC( IC_EXTRA2, NUL )
//, IC( IC_EXTRA3, NUL )
, IC( IC_EXTRA4, NUL )
, IC( IC_EXTRA5, NUL )
, IC( IC_EXTRA6, NUL )
, IC( IC_EXTRA7, NUL )
, IC( IC_EXTRA8, NUL )
, IC( IC_EXTRA9, NUL )

                                // End of Codes
                                //------------------------------------------
, IC( IC_END, NUL )             // - can be used as terminator
};
