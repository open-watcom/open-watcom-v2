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


#ifndef __CGRONT__H__
#define __CGRONT__H__

// CGFRONT.H -- interface with code-generation for front-end

#include "cgdata.h"
#include "cgiobuff.h"
#include "cgio.h"
#include "segment.h"
#include "ptree.h"
#include "module.h"
#include "intsupp.h"
#include "pragdefn.h"

#define BIT_VECT( b1, b2, b3, b4, b5, b6, b7, b8 ) \
    b8 + b7*2 + b6*4 + b5*8 + b4*16 + b3*32 + b2*64 +b1*128

typedef enum                    // DTORING_KIND -- kind of DTORing to be applied
{   DTORING_NONE                // - none
,   DTORING_SCOPE               // - auto variable or component in DTOR
,   DTORING_TEMP                // - temporary variable
,   DTORING_COMPONENT           // - component
} DTORING_KIND;

enum                            // DEFINE TYPE OF POINTER CONVERSION
{   CNVPTR_DERIVED_ONLY    = 0  // - can only convert derived
,   CNVPTR_VIRT_TO_DERIVED = 1  // - can convert virtual to derived
,   CNVPTR_CAST            = 2  // - a cast conversion (from the C++ source)
,   CNVPTR_CONST_VOLATILE  = 4  // - const/volatile checks may be needed later
,   CNVPTR_NO_TRUNC        = 8  // - don't do truncation check
};

typedef enum                    // DEFINE RESULTS OF CONVERSION
{   CNV_OK                      // - conversion successful or not required
,   CNV_OK_TRUNC                // - as above, but truncation occurred
,   CNV_OK_CV                   // - ok, but need to check const/volatile
,   CNV_OK_TRUNC_CV             // - truncated, need to check const/vol.
,   CNV_OK_TRUNC_CAST_CV        // - cast truncated, need to check const/vol.
,   CNV_OK_TRUNC_CAST           // - ptr truncation on cast occurred
,   CNV_WORKED = CNV_OK_TRUNC_CAST // ********* above ==> worked
,   CNV_TRUNC_THIS              // - truncating "this" pointer
,   CNV_ERR                     // - conversion attempted, error occurred
,   CNV_VIRT_DER                // - attempt to convert virt. base to derived
,   CNV_IMPOSSIBLE              // - conversion impossible
,   CNV_AMBIGUOUS               // - conversion ambiguous
,   CNV_PRIVATE                 // - conversion through a private base
,   CNV_PROTECTED               // - conversion through a protected base
} CNV_RETN;

typedef enum                    // DEFINE TYPE OF SIZE CONVERSION NEEDED
{   CNV_PTR_SAME                // - no conversion required
,   CNV_PTR_EXPAND              // - pointer must be enlarged
,   CNV_PTR_TRUNCATE            // - pointer must be truncated
} CNV_PTR_RESULT;

typedef enum                    // DEFINE TYPES OF CONVERSIONS
{   CNV_INIT        = 0x0001    // - initialization (direct)
,   CNV_INIT_COPY   = 0x8001    // - initialization (copy)
,   CNV_FUNC_ARG    = 0x0002    // - function argument
,   CNV_FUNC_THIS   = 0x0042    // - 'this' ptr for function
,  CNV_FUNC_CD_THIS = 0x0082   // - 'this' ptr for CTOR, DTOR
,   CNV_FUNC_RET    = 0x0004    // - function return
,   CNV_FUNC_DARG   = 0x0044    // - function return (of default argument)
,   CNV_CAST        = 0x0008    // - explicit cast
,   CNV_EXPR        = 0x0010    // - expression
,   CNV_ASSIGN      = 0x0020    // - assignment

                                // DERIVED TYPES OF CONVERSIONS

,   CNV_TYPE = ( CNV_INIT | CNV_FUNC_ARG | CNV_FUNC_RET | CNV_EXPR
               | CNV_CAST | CNV_ASSIGN )                        // 0x003F
} CNV_REQD;

#define _CNV_TYPE( val ) ((val) & CNV_TYPE )

typedef enum                    // CODES FOR PC POINTER REPRESENTATIONS
{   PC_PTR_REGULAR              // - regular pointer
,   PC_PTR_BASED_STRING         // - based( __segment(name) )
        = TF1_BASED_STRING >> 8
,   PC_PTR_BASED_SELF           // - based( __self )
        = TF1_BASED_SELF >> 8
,   PC_PTR_BASED_VOID           // - based( void )
        = TF1_BASED_VOID >> 8
,   PC_PTR_BASED_FETCH          // - based( ptr )
        = TF1_BASED_FETCH >> 8
,   PC_PTR_BASED_ADD            // - based( segment # )
        = TF1_BASED_ADD >> 8
,   PC_PTR_FAR16                // - far16
,   PC_PTR_NOT                  // - not a pointer
} PC_PTR;

typedef enum                    // CODES FOR ADDR(FUNCTION)
{   ADDR_FN_NONE                // - not addr of function
,   ADDR_FN_ONE                 // - addr of function (not overloaded)
,   ADDR_FN_MANY                // - addr of function (overloaded)
,   ADDR_FN_ONE_USED            // - use of function (not overloaded)
,   ADDR_FN_MANY_USED           // - use of function (overloaded)
} addr_func_t;

enum                            // control bits for EffectCtor
{   EFFECT_EXACT        = 0x01  // - exact version of class
,   EFFECT_VALUE_THIS   = 0x02  // - force value of EffectCtor expr to be 'this'
,   EFFECT_DECOR_COMP   = 0x10  // - add component decoration
,   EFFECT_DECOR_SCOPE  = 0x20  // - add scope decoration
,   EFFECT_DECOR_TEMP   = 0x40  // - add temporary decoration
,   EFFECT_VIRTUAL      = 0x80  // - is a virtual base
,   EFFECT_NULL         = 0x00

,   EFFECT_CTOR_DECOR   =       // - add decoration
        EFFECT_DECOR_COMP  |
        EFFECT_DECOR_SCOPE |
        EFFECT_DECOR_TEMP
};

typedef enum                    // TEMP_TYPE:
{   TEMP_TYPE_NONE              // - not defined
,   TEMP_TYPE_EXPR              // - auto, expression duration
,   TEMP_TYPE_BLOCK             // - auto, block duration
,   TEMP_TYPE_STATIC            // - static
} TEMP_TYPE;

typedef struct                  // USED FOR DIAGNOSIS OF CONVERSIONS
{   int msg_impossible;         // - error message for CNV_IMPOSSIBLE
    int msg_ambiguous;          // - error message for CNV_AMBIGUOUS
    int msg_virt_der;           // - error message for CNV_VIRT_DIR
    int msg_private;            // - error message for CNV_PRIVATE
    int msg_protected;          // - error message for CNV_PROTECTED
} CNV_DIAG;

typedef struct                  // USED FOR DIAGNOSIS OF CALL
{   int msg_ambiguous;          // - when ambiguous
    int msg_no_match_one;       // - no match, only one function declared
    int msg_no_match_many;      // - no match, several functions declared
} CALL_DIAG;

typedef struct                  // USED FOR DIAGNOSIS OF SYMBOL ACCESS
{   int msg_no_this;            // - "this" required, not supplied
    int msg_extra_this;         // - "this" supplied, may not be used
    int msg_enclosing;          // - "this" accesses item in enclosing class
} SYMBOL_DIAG;

typedef enum                    // CTD -- common-type derivations
{   CTD_NO                      // - neither derived from other
,   CTD_LEFT                    // - left derived, non-virtually
,   CTD_LEFT_VIRTUAL            // - left derived, virtually
,   CTD_LEFT_AMBIGUOUS          // - left derived, ambiguously
,   CTD_LEFT_PRIVATE            // - left derived, across private base
,   CTD_LEFT_PROTECTED          // - left derived, across protected base
,   CTD_RIGHT                   // - right derived, non-virtually
,   CTD_RIGHT_VIRTUAL           // - right derived, virtually
,   CTD_RIGHT_AMBIGUOUS         // - right derived, ambiguously
,   CTD_RIGHT_PRIVATE           // - right derived, across private base
,   CTD_RIGHT_PROTECTED         // - right derived, across protected base
} CTD;

typedef enum                    // CALL_OPT -- types of call optimizations
{   CALL_OPT_NONE               // - none detected
,   CALL_OPT_ERR                // - error (used for convenience)
,   CALL_OPT_CTOR               // - ctor to be optimized
,   CALL_OPT_FUN_CALL           // - function-call to be optimized
,   CALL_OPT_BIN_COPY           // - binary-copy optimization
} CALL_OPT;


// MACROS

#define ___NodeIsOp(node,op) ( (node)->cgop == (op) )
#define conversionWorked( val ) ( (val) <= CNV_WORKED )


// PROTOTYPES : exposed to C++ compiler

PTREE AddCastNode               // ADD A CAST NODE
    ( PTREE expr                // - expression
    , TYPE type )               // - type
;
TYPE AnalyseAssignment(         // ANALYSE assignment operation
    TYPE left,                  // - assignement type
    PTREE right )               // - right node
;
PTREE AnalyseBoolExpr(          // ANALYZE A BOOLEAN EXPRESSION
    PTREE expr )
;
PTREE AnalyseCall(              // ANALYSIS FOR CALL
    PTREE expr,                 // - call expression
    CALL_DIAG *diagostic )      // - diagnostics used for function problems
;
CALL_OPT AnalyseCallOpts(       // ANALYSE CALL OPTIMIZATIONS
    TYPE type,                  // - class type of target operand
    PTREE src,                  // - source operand
    PTREE* a_dtor,              // - addr[ CO_DTOR ]
    PTREE* a_right )            // - addr[ base source operand ]
;
PTREE AnalyseCtorExpr(          // ANALYSE CTOR EXPRESSION
    PTREE expr )                // - expression
;
PTREE AnalyseDelete(            // ANALYSE DELETE OPERATOR
    PTREE expr,                 // - delete expression
    boolean in_dtor )           // - TRUE ==> call from inside DTOR
;
PTREE AnalyseDtorCall(          // ANALYSIS FOR SPECIAL DTOR CALLS
    TYPE class_type,            // - class to be destructed
    PTREE this_node,            // - expression for address of class
    target_offset_t extra )     // - constant for extra second parm
;
PTREE AnalyseInitExpr(          // ANALYZE AN INITIALIZATION EXPRESSION
    PTREE expr,                 // - expresion
    boolean is_static )         // - is target a static
;
PTREE AnalyseIntegralExpr(      // ANALYZE AN EXPRESSION, MAKE IT AN INT. VALUE
    PTREE expr )
;
void AnalyseIntTrunc(           // ANALYSE INTEGRAL TRUNCATION
    PTREE expr,                 // - expression for error
    TYPE type )                 // - target type
;
boolean AnalyseLvalue(          // ANALYSE AN LVALUE
    PTREE *a_expr )             // - addr( expression to be analysed )
;
boolean AnalyseLvalueAddrOf(    // ANALYSE LVALUE FOR "&"
    PTREE *a_expr )             // - addr[ expression to be analysed ]
;
PTREE AnalyseLvArrow(           // ANALYSE LVALUE "->"
    PTREE expr )                // - addr[ expression to be analysed ]
;
PTREE AnalyseLvDot(             // ANALYSE LVALUE "."
    PTREE expr )                // - addr[ expression to be analysed ]
;
PTREE AnalyseNew(               // ANALYSE A "NEW" OPERATOR
    PTREE expr,                 // - "new" expression
    TYPE type )                 // - type of expression
;
PTREE AnalyseNode(              // ANALYSE PTREE NODE FOR SEMANTICS
    PTREE expr )                // - current node
;
PTREE AnalyseOffsetOf(          // ANALYSE OFFSETOF
    PTREE expr )                // - expression to be analysed
;
PTREE AnalyseOperator(          // ANALYSE AN OPERATOR
    PTREE expr )                // - expression
;
unsigned AnalysePtrCV(          // CHECK PTR CONVERSION FOR CONST/VOLATILE
    PTREE expr,                 // - expression for error
    TYPE proto,                 // - type of target
    TYPE argument,              // - type of source
    unsigned conversion )       // - type of conversion
;
PTREE AnalyseReturnClassVal     // RETURN CLASS VALUE
    ( PTREE retn_expr )         // - expression for return
;
PTREE AnalyseReturnExpr(        // ANALYSE A RETURN EXPRESSION
    SYMBOL func,                // - function to which the expression applies
    PTREE expr )                // - expression
;
PTREE AnalyseReturnSimpleVal    // RETURN A SIMPLE VALUE
    ( PTREE expr )              // - expression for return
;
PTREE AnalyseStmtExpr(          // ANALYZE A STATEMENT EXPRESSION
    PTREE expr )
;
PTREE AnalyseRawExpr(           // ANALYZE A SUB-TREE WITH NO EXTRA SEMANTICS
    PTREE expr )
;
boolean AnalyseSymbolAccess(    // ANALYSE ACCESS TO SYMBOL
    PTREE expr,                 // - expression for errors
    PTREE symbol,               // - symbol being accessed
    PTREE this_expr,            // - expression for "this"
    SYMBOL_DIAG *diag )         // - diagnosis to be used
;
boolean AnalyseThisDataItem(    // ANALYSE "THIS" DATA ITEM IN PARSE TREE
    PTREE *a_expr )             // - addr[ expression ]
;
CNV_RETN AnalyseTypeCtor(       // ANALYSE CONSTRUCTOR FOR A TYPE
    SCOPE scope,                // - start scope for component ctors (NULLable)
    TYPE type,                  // - type for CTOR (may be an abstract class)
    unsigned conversion,        // - type of conversion reqd
    SYMBOL *ctor,               // - ctor to be filled in
    PTREE *initial )            // - addr( initialization arguments )
;
PTREE AnalyseValueExpr(         // ANALYZE A EXPRESSION, MAKE IT A VALUE
    PTREE expr )
;
target_size_t ArrayTypeNumberItems( // GET ACTUAL NUMBER OF ITEMS FOR AN ARRAY
    TYPE artype )               // - an array type
;
PTREE CallArgumentExactCtor(    // GET EXACT CTOR ARG., IF REQUIRED
    TYPE type,                  // - type for class
    boolean exact )             // - TRUE ==> exact CTORing of classes
;
PTREE CallArgsArrange(          // ARRANGE CALL ARGUMENTS
    TYPE ftype,                 // - function type
    PTREE callnode,             // - node for call
    PTREE userargs,             // - user arguments
    PTREE thisnode,             // - this node
    PTREE cdtor,                // - cdtor node (ignored when thisnode==NULL)
    PTREE retnnode )            // - return node (for struct return)
;
CALL_DIAG* CallDiagFromCnvDiag  // MAKE CALL_DIAG FROM CNV_DIAG
    ( CALL_DIAG* call_diag      // - call diagnosis
    , CNV_DIAG* cnv_diag )      // - conversion diagnosis
;
boolean CastCommonClass         // CAST (IMPLICITLY) TO A COMMON CLASS
    ( PTREE* a_expr             // - addr[ expression ]
    , CNV_DIAG* diagnosis )     // - diagnosis
;
PTREE CastImplicit              // IMPLICIT CAST
    ( PTREE expr                // - expression
    , TYPE type                 // - target type
    , CNV_REQD reqd             // - required kind of conversion
    , CNV_DIAG* diagnosis )     // - diagnosis
;
PTREE CastImplicitCommonPtrExpr // CONVERT TO COMMON PTR EXPRESSION
    ( PTREE expr                // - the expression
    , CNV_DIAG *diagnosis       // - diagnosis
    , boolean check_cv )        // - TRUE ==> check CV QUALIFICATION
;
PTREE CastImplicitRight         // IMPLICIT CAST OF RIGHT OPERAND
    ( PTREE expr                // - expression
    , TYPE type                 // - target type
    , CNV_REQD reqd             // - required kind of conversion
    , CNV_DIAG* diagnosis )     // - diagnosis
;
void CDtorScheduleArgRemap(     // SCHEDULE CTOR/DTOR ARG.S REMAP, IF REQ'D
    SYMBOL cdtor )              // - CTOR OR DTOR
;
void CgFrontCode(               // EMIT TO CODE SEGMENT
    CGINTEROP opcode )          // - intermediate code
;
void CgFrontCodeInt(            // EMIT (code,int) TO CODE SEGMENT
    CGINTEROP opcode,           // - opcode
    int value )                 // - value
;
void CgFrontCodePtr(            // EMIT (code,ptr) TO CODE SEGMENT
    CGINTEROP opcode,           // - opcode
    void *value )               // - value
;
void CgFrontCodeUint(           // EMIT (code,unsigned) TO CODE SEGMENT
    CGINTEROP opcode,           // - opcode
    unsigned value )            // - value
;
void CgFrontCtorTest(           // INDICATE FUNCTION MIGHT HAVE CTOR-TEST
    void )
;
SYMBOL CgFrontCurrentFunction(  // GET CURRENT FUNCTION BEING GENERATED
    void )
;
void CgFrontData(               // EMIT TO DATA FILE
    CGINTEROP opcode )          // - intermediate code
;
void CgFrontDataInt(            // EMIT (code,int) TO DATA SEGMENT
    CGINTEROP opcode,           // - opcode
    int value )                 // - value
;
void CgFrontDataUint(           // EMIT (code,unsigned) TO DATA SEGMENT
    CGINTEROP opcode,           // - opcode
    unsigned value )            // - value
;
void CgFrontDataPtr(            // EMIT (code,ptr) TO DATA SEGMENT
    CGINTEROP opcode,           // - opcode
    void *value )               // - value
;
void CgFrontDbgLine(            // SET LINE FOR DEBUGGING
    TOKEN_LOCN* posn )          // - source file position
;
void CgFrontFileScope(          // SET FILE SCOPE
    SCOPE file_scope )          // - file scope
;
void CgFrontRetnOptEnd(         // COMPLETE BRACKETTING FOR RETURN OPT.
    void )
;
boolean CgFrontRetnOptVar(      // START BRACKETTING FOR VAR. (RETURN OPT>)
    SYMBOL var )                // - the symbol
;
void CgFrontFini(               // RELEASE ANY FRONT-END CODE GENERATION MEMORY
    void )
;
void CgFrontFiniRef(            // EMIT TO FINI-REF SEGMENT
    void )
;
void CgFrontFuncArgs(           // DECLARE FUNCTION ARGUMENTS
    SCOPE scope )               // - scope containing arguments
;
void CgFrontFuncClose(          // CLOSE A FUNCTION (AND ITS FILE)
    SYMBOL func )               // - symbol for function
;
void CgFrontFuncInitFlags(      // GENERATE INIT FLAGS FOR FUNCTION
    SYMBOL var )                // - NULL OR FLAGS VARIABLE
;
CGFILE_INS CgFrontLastIns(      // RETURN LOCATION OF LAST WRITTEN CODE IC
    void )
;
CGFILE_INS CgFrontFuncOpen(     // OPEN A FUNCTION (AND ITS FILE)
    SYMBOL func,                // - symbol for function
    TOKEN_LOCN *posn )          // - source-file position
;
void CgFrontGotoNear(           // EMIT GOTO IN NEAR SPACE (CS,GOTO)
    CGINTEROP opcode,           // - opcode to determine type of label ref.
    unsigned condition,         // - condition for goto
    CGLABEL label )             // - label number
;
void CgFrontInitRef(            // EMIT TO INIT-REF SEGMENT
    void )
;
void CgFrontLabdefCs(           // DEFINE A CS LABEL
    CGLABEL label )             // - label number
;
void CgFrontLabdefGoto(         // DEFINE A GOTO LABEL
    CGLABEL label )             // - label number
;
unsigned CgFrontLabelCs(        // GET NEXT AVAILABLE LABEL # (CONTROL SEQ)
    void )
;
unsigned CgFrontLabelGoto(      // GET NEXT AVAILABLE LABEL # (GOTO)
    void )
;
void CgFrontLabfreeCs(          // FREE CS LABELS
    unsigned count )            // - number of labels to be freed
;
void CgFrontModInitClose(       // CLOSE AFTER MODULE-INIT. PROCESSING
    SCOPE save_scope,           // - from ModuleInitResume
    void *save )                // - result from CgFrontDefArgOpen
;
void CgFrontModInitFini(        // PROCESS MODULE-INIT. AFTER FRONT-END
    void )
;
void CgFrontModInitInit(        // INITIALIZE MODULE-INITIALIZATION
    void )
;
void *CgFrontModInitOpen(       // OPEN FOR MODULE-INITIALIZATION CODE GEN
    SCOPE *save_scope )         // - more info to save
;
void CgFrontModInitUsed(        // MARK MODULE-INIT. FUNCTION USED
    void )
;
void CgFrontResultBoolean(      // SET RESULTANT TYPE TO BE BOOLEAN
    void )
;
void CgFrontResumeFunction(     // RESUME A FUNCTION
    SYMBOL func )               // - function being resumed
;
void CgFrontReturnSymbol(       // SET RETURN SYMBOL
    SYMBOL return_operand )     // - the symbol
;
void CgFrontScopeCall(          // GENERATE IC_SCOPE_CALL, IF REQ'D
    SYMBOL fun,                 // - function called
    SYMBOL dtor,                // - dtor, when function is ctor
    DTORING_KIND kind )         // - kind of dtoring
;
void CgFrontStatInit(           // START OF STATIC-INIT TEST IN FUNCTION
    void )
;
void CgFrontSwitchBeg(          // START A SWITCH STATEMENT
    void )
;
void CgFrontSwitchCase(         // CASE STATEMENT IN SWITCH STATEMENT
    SCOPE scope_sw,             // - scope for switch jump
    boolean deadcode,           // - dead-code state
    TOKEN_LOCN *posn,           // - source-file position
    uint_32 value )             // - switch value
;
void CgFrontSwitchDefault(      // DEFAULT STATEMENT IN SWITCH STATEMENT
    SCOPE scope_sw,             // - scope for switch jump
    boolean deadcode,           // - dead-code state
    TOKEN_LOCN *posn )          // - source-file position
;
void CgFrontSwitchEnd(          // END A SWITCH STATEMENT
    void )
;
void *CgFrontSwitchFile(        // SWITCH VIRTUAL FUNCTION FILES
    void *symbol )              // - symbol for file
;
void CgFrontSymbol(             // EMIT A SYMBOL
    SYMBOL sym )                // - the symbol
;
void CgFrontZapPtr(             // ZAP A WRITTEN RECORD (PTR OPERAND)
    CGFILE_INS location,        // - location to be zapped
    CGINTEROP opcode,           // - opcode
    void *operand )             // - operand
;
void CgFrontZapUint(            // ZAP A WRITTEN RECORD (UNSIGNED OPERAND)
    CGFILE_INS location,        // - location to be zapped
    CGINTEROP opcode,           // - opcode
    unsigned operand )          // - operand
;
void CgSetType(                 // SET NORMAL TYPE FOR GENERATION
    TYPE type )                 // - the type
;
void CgSetTypeExact(            // SET EXACT TYPE FOR GENERATION
    TYPE type )                 // - the type
;
unsigned CgTypeTruncation(      // GET CNV_... FOR TRUNCATION
    TYPE tgt,                   // - target type
    TYPE src )                  // - source type
;
boolean CharIsPromoted          // CHECK IF char PROMOTED TO int
    ( TYPE src_type             // - source type
    , TYPE tgt_type )           // - target type
;
PTREE CheckCharPromotion        // CHECK FOR CHARACTER-TO-INT promotion
    ( PTREE expr )              // - expression
;
boolean ClassAccessDefaultCopy( // CHECK ACCESS TO DEFAULT COPY CTOR
    TYPE type )                 // - type for class
;
PTREE ClassAssign(              // ASSIGNMENT TO CLASS OBJECT
    PTREE expr )                // - expression ( "=" at top )
;
unsigned ClassConvert(          // CONVERT FROM/TO CLASS OBJECT
    PTREE *a_expr )             // - addr[ expression ( "=" at top ) ]
;
PTREE ClassCopyTemp(            // COPY A TEMPORARY
    TYPE cl_type,               // - class type
    PTREE expr,                 // - source expression
    PTREE temp_node )           // - target temporary
;
PTREE ClassDefaultCopy(         // COPY TO CLASS OBJECT
    PTREE tgt,                  // - target expression
    PTREE src )                 // - source expression
;
PTREE ClassDefaultCopyDiag(     // COPY TO CLASS OBJECT, WITH DIAGNOSIS
    PTREE tgt,                  // - target expression
    PTREE src,                  // - source expression
    CNV_DIAG* diag )            // - conversion diagnosis
;
PTREE ClassDefaultCopyTemp(     // COPY TEMP TO CLASS OBJECT, WITH DIAGNOSIS
    PTREE tgt,                  // - target expression
    PTREE src,                  // - source expression
    CNV_DIAG *cnvdiag )         // - conversion diagnosis
;
boolean ClassAccessDtor(        // CHECK ACCESS TO DTOR
    TYPE type )                 // - type for class
;
CNV_RETN ClassDefaultCtorDefine(// DEFINE DEFAULT CTOR FOR A CLASS
    TYPE cl_type,               // - type for class
    SYMBOL *pctor )             // - ptr( default CTOR )
;
CNV_RETN ClassDefaultCtorFind(  // FIND DEFAULT CTOR FOR A CLASS
    TYPE cl_type,               // - type for class
    SYMBOL *pctor,              // - ptr( default CTOR )
    TOKEN_LOCN *src_locn )      // - source location for errors
;
SYMBOL ClassDefaultOpEq(        // GET DEFAULT OP= FOR A CLASS
    TYPE cltype,                // - class type
    TYPE derived )              // - derived access type
;
SYMBOL ClassFunMakeAddressable( // MAKE SURE THE FUNCTION CAN BE ADDRESSED
    SYMBOL ctordtor )           // - default ctordtor symbol
;
PTREE ClassInitialize(          // INITIALIZE A CLASS OBJECT
    PTREE tgt,                  // - target object
    PTREE src )                 // - source expression
;
TYPE ClassTypeForType(          // GET CLASS TYPE FOR TYPE OR REFERENCE TO IT
    TYPE type )                 // - input type
;
unsigned ConversionDiagnose(    // DIAGNOSE RETURN FROM A CONVERSION
    unsigned retn,              // - return value: CNV_...
    PTREE expr,                 // - current expression
    CNV_DIAG *diagnosis )       // - diagnosis information
;
void ConversionDiagnoseInf(     // DIAGNOSE SOURCE/TARGET CONVERSION
    void )
;
void ConversionDiagnoseInfTgt(  // DIAGNOSE TARGET CONVERSION
    void )
;
void ConversionDiagLR           // DIAGNOSE LEFT, RIGHT OPERANDS
    ( TYPE left                 // - left type
    , TYPE right )              // - right type
;
void ConversionInfDisable(      // DISABLE DISPLAY OF CONVERSION INFORMATION
    void )
;
void ConversionTypesSet(        // RE-SET CONVERSION TYPES
    TYPE src,                   // - new source
    TYPE tgt )                  // - new target
;
PTREE ConvertBoolean(           // CONVERSION TO BOOLEAN EXPRESSION
    PTREE bexpr )               // - the boolean expression
;
PTREE ConvertCommonReference(   // CONVERT TO COMMON REFERENCE
    PTREE expr,                 // - binary expression
    CNV_DIAG *diagnosis )       // - used to diagnose errors
;
boolean ConvertCommonType(      // CONVERT TO COMMON TYPE (:, ==, !=)
    PTREE *expr,                // - addr[ expression ]
    CNV_DIAG *diag_class,       // - diagnosis: class
    CNV_DIAG *diag_mem_ptr )    // - diagnosis: member ptr.
;
unsigned ConvertExpr(           // GENERALIZED CONVERSION
    PTREE *expr_addr,           // - addr( expression to be converted )
    TYPE proto,                 // - prototype
    unsigned conversion )       // - type of conversion
;
unsigned ConvertExprDiagnosed(  // CONVERT AND DIAGNOSE EXPRESSION
    PTREE *expr_addr,           // - addr( expression to be converted )
    TYPE proto,                 // - prototype
    unsigned conversion,        // - type of conversion
    PTREE expr_diag,            // - expression for diagnosis
    CNV_DIAG *diagnosis )       // - diagnosis information
;
unsigned ConvertExprDiagResult( // CONVERT AND DIAGNOSE RESULT
    PTREE *expr_addr,           // - addr( expression to be converted )
    TYPE proto,                 // - prototype
    unsigned conversion,        // - type of conversion
    CNV_DIAG *diagnosis )       // - diagnosis information
;
PTREE ConvertMembPtrConst(      // CONVERT TO TEMP. A MEMBER-PTR CONST
    PTREE *a_expr )             // - addr [expression]
;
unsigned ConvertOvFunNode(      // CONVERT FUN (FUN IS OVERLOADED), NO FREE
    TYPE tgt,                   // - target type
    PTREE func )                // - overloaded function
;
PTREE CopyDirect                // DIRECT COPY
    ( PTREE src                 // - source expression
    , PTREE tgt                 // - target expression
    , TYPE type                 // - type copied
    , CNV_DIAG* diag )          // - diagnosis
;
PTREE CopyInit                  // INITIALIZATION COPY
    ( PTREE src                 // - source expression
    , PTREE tgt                 // - target expression
    , TYPE type                 // - type copied
    , CNV_DIAG* diag )          // - diagnosis
;
PTREE CopyOptimize              // COPY OPTIMIZATION
    ( PTREE right               // - base source operand
    , PTREE src                 // - source operand
    , PTREE left                // - target operand
    , PTREE dtor                // - NULL or CO_DTOR expression
    , CALL_OPT opt )            // - type of optimization
;
PTREE CopyClassRetnVal          // COPY TO RETURN SYMBOL A CLASS VALUE
    ( PTREE src                 // - source expression
    , PTREE tgt                 // - target expression
    , TYPE type                 // - type copied
    , CNV_DIAG* diag )          // - diagnosis
;
CNV_RETN CtorFind(              // FIND CONSTRUCTOR FOR ARGUMENT LIST
    SCOPE access,               // - accessing derived class
    TYPE cl_type,               // - type for class
    arg_list *alist,            // - arguments list
    PTREE *ptlist,              // - parse tree nodes for arguments
    TOKEN_LOCN *locn,           // - location for access errors
    SYMBOL *ctor )              // - addr( constructor symbol )
;
unsigned CtorFindFuncConv(      // FIND CONSTRUCTOR FOR USE WITH FUNC. ARG
    TYPE cl_type,               // - type for class
    arg_list *alist,            // - arguments list
    PTREE *ptlist,              // - parse tree nodes for arguments
    SYMBOL *ctor )              // - addr( constructor symbol )
;
FNOV_LIST *CtorFindList(   // FIND LIST OF ALL CONSTRUCTORS
    TYPE src                    // - src type
  , TYPE tgt                    // - tgt type
);
void CtorPrologue(              // GENERATE PROLOGUE FOR CTOR
    SYMBOL ctor,                // - constructor SYMBOL
    PTREE mem_init,             // - optional mem-initializer
    void (*push_scope)          // - pushes SCOPE into control structure
        ( void ) )
;
void CtorEpilogue(              // GENERATE EPILOGUE FOR CTOR
    SYMBOL ctor )               // - constructor SYMBOL
;
boolean DefargBeingCompiled(    // TEST IF DEFARG-FUNCTION BEING COMPILED
    void )
;
void DgStaticDataMember(        // DATA GENERATE: STATIC DATA MEMBER OF CLASS
    SYMBOL sym )                // - symbol for member
;
void DgSymbol(                  // DATA GENERATE: SYMBOL
    SYMBOL sym )                // - symbol for member
;
void DgSymbolLabel(             // SET LABEL FOR SYMBOL
    SYMBOL sym )                // - symbol for label
;
void DgSegmentIncrement(        // INCREMENT SEGID FOR DATA
     void )
;
void DgPadBytes(                // STORE SIZE UNITIALIZED BYTES
    target_size_t size )        // - number of bytes
;
void DgZeroBytes(               // STORE SIZE ZERO BYTES
    target_size_t size )        // - number of bytes
;
void DgStoreString(             // STORE CHARACTER STRING
    PTREE expr )                // - expression for string
;
boolean DgStoreBitfield(        // STORE BITFIELD VALUE
    TYPE type,                  // - type of bitfield
    target_ulong value )        // - value to store
;
void DgStoreConstScalar(        // STORE CONST SCALAR IN SYMBOL
    PTREE expr,                 // - value to store
    TYPE type,                  // - type of scalar
    SYMBOL sym )                // - symbol to store it in
;
boolean DgStoreScalar(          // STORE SCALAR VALUE
    PTREE expr,                 // - expression to store (caller must free)
    target_size_t offset,       // - associated offset
    TYPE type )                 // - type to store
;
void DgInitBegin(               // BEGIN POTENTIALLY OPTIONAL INITIALIZATION
    SYMBOL sym )                // - symbol being initialized
;
void DgInitDone(                // END POTENTIALLY OPTIONAL INITIALIZATION
    void )
;
void DtorPrologue(              // GENERATE PROLOGUE FOR DTOR
    SYMBOL dtor,                // - DTOR
    CGLABEL *end,               // - returns label of "real" end of dtor
    CGFILE_INS *rtbeg )         // - position for run-time call
;
void DtorEpilogue(              // GENERATE EPILOGUE FOR DTOR
    SYMBOL dtor,                // - DTOR
    CGLABEL end,                // - "real" end of dtor
    CGFILE_INS *rtbeg )         // - position for run-time call
;
boolean GeneratedDefaultFunction(// IS SYMBOL A DEFAULT CTOR,COPY, OR DTOR?
    SYMBOL sym )                // - symbol to check
;
PTREE EffectCtor(               // EFFECT A CONSTRUCTION
    PTREE initial,              // - initialization list
    SYMBOL ctor,                // - constructor
    TYPE base_type,             // - type being constructed
    PTREE this_node,            // - NULL, or node for "this"
    TOKEN_LOCN *locn,           // - initialization location
    unsigned control )          // - control mask (EFFECT_* defined above)
;
void EmitAnalysedStmt(          // EMIT ANALYSED STMT (IF IT EXISTS)
    PTREE stmt )                // - the statement
;
void EmitVfunThunk(             // EMIT THUNK FOR VIRTUAL FUNCTION
    SCOPE host_scope,           // - scope for thunk insertion
    THUNK_ACTION *thunk )       // - thunk description
;
TYPE EnumTypeForType(           // GET ENUM TYPE FOR TYPE OR REFERENCE TO IT
    TYPE type )                 // - input type
;
boolean ExprIsLvalue            // TEST IF EXPRESSION IS LVALUE
    ( PTREE expr )              // - expression
;
void GenerateDefaultCtor(       // EMIT A DEFAULT CTOR
    SYMBOL sym )                // - the ctor
;
void GenerateDefaultCopy(       // EMIT A DEFAULT COPY CTOR
    SYMBOL sym )                // - the copy ctor
;
void GenerateDefaultDtor(       // EMIT A DEFAULT DTOR
    SYMBOL sym )                // - the dtor
;
void GenerateDefaultAssign(     // EMIT A DEFAULT ASSIGN
    SYMBOL sym )                // - the assignment operator
;
void IcEmitDtorAutoSym(         // EMIT EXPRESSION TO MARK AUTO AS DTOR'ED
    SYMBOL sym )                // - symbol to be dtor'ed
;
void IcEmitDtorStaticSym(       // EMIT EXPRESSION TO MARK STATIC AS DTOR'ED
    SYMBOL sym )                // - symbol to be dtor'ed
;
PTREE IcEmitExpr(               // EMIT EXPRESSION
    PTREE expr )                // - the expression
;
void InitRefFixup(              // TRAVERSAL FOR REFERENCE INITIALIZATION
    PTREE expr )                // - initialization expression
;
boolean IsBigCode(              // TEST IF DEFAULT MEMORY MODEL IS BIG CODE
    void )
;
boolean IsBigData(              // TEST IF DEFAULT MEMORY MODEL IS BIG DATA
    void )
;
boolean IsHugeData(             // TEST IF DEFAULT MEMORY MODEL IS HUGE DATA
    void )
;
TYPE LvalueErr(                 // NOT-LVALUE ERROR (NODE)
    PTREE expr_chk,             // - expression, to be checked
    PTREE expr )                // - expression
;
TYPE LvalueErrLeft(             // NOT-LVALUE ERROR (LEFT NODE)
    PTREE expr_chk,             // - expression, to be checked
    PTREE expr )                // - expression
;
arg_list *MakeMemberArgList(    // ALLOCATE ARG.LIST FOR MEMBER FUNC.S
    SYMBOL sym,                 // - member function
    int num_args )              // - number of arguments
;
arg_list *MakeMockArgList(      // ALLOCATE MOCK ARG.LIST FOR OVERLOADING
    TYPE type,                  // - function type
    int num_args )              // - number of args in list
;
PTREE MakeNodeSymbol(           // MAKE PT_SYMBOL NODE FROM SYMBOL
    SYMBOL sym )                // - symbol
;
boolean MainProcedure(          // DETECT IF SYMBOL IS "MAIN" procedure
    SYMBOL sym )                // - symbol
;
void MainProcSetup(             // SETUP FOR "MAIN"
    SYMBOL sym )                // - symbol
;
CNV_RETN MembPtrAssign(         // ASSIGNMENT TO MEMBER POINTER
    PTREE *a_expr )             // - addr[ assignment expression ]
;
PTREE MembPtrCommonType(        // IMPLICIT CONVERSION TO COMMON TYPE
    PTREE expr )                // - binary expression
;
PTREE MembPtrCompare(           // COMPARISON OF MEMBER POINTERS
    PTREE expr )                // - comparison expression
;
PTREE MembPtrZero(              // MAKE A NULL MEMBER POINTER CONSTANT
    TYPE type )                 // - type for constant
;
CNV_RETN MembPtrConvert(        // CONVERT A MEMBER POINTER
    PTREE *a_expr,              // - addr[ conversion expression, not class ]
    TYPE tgt_type,              // - target type (member-pointer)
    unsigned conversion )       // - type of conversion
;
PTREE MembPtrDereference(       // DO '.*' AND '->*' operations
    PTREE expr )                // - expression to be processed
;
TYPE MembPtrDerefFnPtr(         // GET TYPE OF DE-REFERENCING FUNC. POINTER
    void )
;
PTREE MembPtrExtend             // FAKE AN ADDRESS-OF NODE FOR BARE FUNCTION
    ( PTREE expr )              // - expression
;
boolean MembPtrExtension        // TEST IF EXTENDED FUNCTION MEMBER PTR
    ( PTREE expr )              // - expression
;
PTREE MembPtrFuncArg(           // EXPRESSION FOR FUNCTION ARGUMENT
    PTREE arg )                 // - converted expression
;
CNV_RETN MembPtrReint(          // REINTERPRET A MEMBER POINTER
    PTREE *a_expr,              // - addr[ conversion expression, not class ]
    TYPE tgt_type )             // - target type (member-pointer)
;
boolean MembPtrZeroConst(       // DETERMINE IF ZERO MEMBER-PTR CONSTANT
    PTREE expr )                // - CO_MEMPTR_CONST expression
;
PTREE NodeAddSideEffect(        // ADD A SIDE-EFFECT EXPRESSION
    PTREE side_effect,          // - side-effect expression
    PTREE expr )                // - original expression
;
PTREE NodeActualNonOverloaded(  // POSITION OVER DEFAULT-ARG SYMBOLS
    PTREE node )                // - PT_SYMBOL for function
;
addr_func_t NodeAddrOfFun(      // GET PTREE FOR &FUN (FUN IS OVERLOADED)
    PTREE oper,                 // - expression
    PTREE *addr_func )          // - addr[ &function ]
;
PTREE NodeArg(                  // MAKE A SINGLE ARGUMENT NODE
    PTREE argval )              // - value for argument
;
PTREE NodeArgument(             // MAKE AN ARGUMENT NODE
    PTREE left,                 // - left subtree
    PTREE right )               // - right subtree
;
PTREE NodeArgumentExactCtor(    // ADD EXACT CTOR ARG., IF REQUIRED
    PTREE args,                 // - other arguments
    TYPE type,                  // - type for class
    boolean exact )             // - TRUE ==> exact CTORing of classes
;
PTREE NodeArguments(            // MAKE A LIST OF ARGUMENTS
    PTREE arg,                  // - first arg
    ... )                       // - NULL terminated, in reverse order
;
PTREE NodeAssign(               // CREATE ASSIGNMENT NODE
    PTREE tgt,                  // - target
    PTREE src )                 // - source
;
PTREE NodeAssignRef(            // CREATE ASSIGNMENT NODE FOR REFERENCE
    PTREE tgt,                  // - target
    PTREE src )                 // - source
;
PTREE NodeAssignTemporary(      // ASSIGN NODE TO A TEMPORARY
    TYPE type,                  // - type of temporary
    PTREE expr )                // - the expression to be assigned to temp
;
PTREE NodeAssignTemporaryNode(  // ASSIGN NODE TO A TEMPORARY NODE
    TYPE type,                  // - type of temporary
    PTREE expr,                 // - the expression to be assigned to temp
    PTREE temp_node )           // - node for temporary symbol
;
PTREE NodeBasedStr(             // BUILD EXPRESSION FOR TF1_BASED_STRING TYPE
    TYPE expr_type )            // - TF1_BASED_STRING type
;
PTREE NodeBinary(               // MAKE A BINARY NODE
    CGOP op,                    // - operator
    PTREE left,                 // - left operand
    PTREE right )               // - right operand
;
boolean NodeBitField(           // TEST IF NODE IS A BIT FIELD
    PTREE node )                // - the node
;
PTREE NodeBitQuestAssign(       // ASSIGN (expr?bit-fld:bit-fld) = expr
    PTREE expr )                // - the expression
;
void NodeBuildArgList(          // BUILD ARGUMENT LIST FROM CALLER ARG.S
    arg_list *alist,            // - argument structure
    PTREE *ptlist,              // - list of parse tree nodes
    PTREE arg,                  // - arguments
    unsigned count )            // - number of arguments
;
boolean NodeCallsCtor(          // DETERMINE IF NODE CALLS CTOR
    PTREE node )                // - a call node
;
PTREE NodeCDtorArg(             // BUILD CONSTANT NODE FOR CDTOR EXTRA ARG
    target_offset_t code )      // - the code
;
PTREE NodeCDtorExtra(           // MAKE A CTOR/DTOR EXTRA PARM NODE
    void )
;
CNV_RETN NodeCheckCnvPtrVoid(   // CHECK CONVERSION TO 'VOID*'
    PTREE *a_expr,              // - addr( resultant tree )
    TYPE src,                   // - source type
    TYPE tgt )                  // - target type
;
CNV_RETN NodeCheckPtrCastTrunc( // CHECK FOR CAST POINTER TRUNCATION WARNING
    TYPE tgt,                   // - target type
    TYPE src )                  // - source type
;
CNV_RETN NodeCheckPtrTrunc(     // CHECK FOR POINTER TRUNCATION WARNING
    TYPE tgt,                   // - target type
    TYPE src )                  // - source type
;
PTREE NodeComma(                // MAKE A COMMA PTREE NODE
    PTREE left,                 // - left operand
    PTREE right )               // - right operand
;
PTREE NodeCommaIfSideEffect(    // MAKE A COMMA PTREE NODE (IF LHS HAS side-effects)
    PTREE left,                 // - left operand
    PTREE right )               // - right operand
;
PTREE NodeSetBooleanType(       // SET NODE TO TYPE OF A REL-OP EXPR
    PTREE expr )                // - expression
;
PTREE NodeSetType               // SET NODE TYPE, FLAGS
    ( PTREE expr                // - the node
    , TYPE type                 // - new type for node
    , PTF_FLAG flags )          // - new flags
;
PTREE NodeCompareToZero(        // MAKE A COMPARE-TO-ZERO NODE, IF REQ'D
    PTREE expr )
;
PTREE NodeConvertToBool(        // MAKE A CONVERT-TO-BOOL NODE, IF REQ'D
    PTREE expr )
;
int NodeConstantValue(          // GET CONSTANT VALUE FOR A NODE
    PTREE node )                // - a constant node
;
PTREE NodeConvert(              // MAKE A CONVERSION NODE
    TYPE type,                  // - type for conversion
    PTREE expr )                // - expression to be converted
;
boolean NodeConvertArgument(    // CONVERT AN ARGUMENT VALUE
    PTREE *a_expr,              // - addr( argument value )
    TYPE proto )                // - prototype type
;
PTREE NodeConvertFlags(         // MAKE A CONVERSION NODE WITH FLAGS, IF REQ'D
    TYPE type,                  // - type for conversion
    PTREE expr,                 // - expression to be converted
    PTF_FLAG flags )            // - flags to be added
;
PTREE NodeConvertCallArgList(   // CONVERT CALL ARGUMENT LIST, AS REQ'D
    PTREE call_expr,            // - call expression (for errors only)
    unsigned acount,            // - # args, caller
    TYPE type,                  // - function type
    PTREE *args )               // - addr( caller argument nodes )
;
PTREE NodeConvertClassExact(    // CONVERT TO "CLASS_EXACT"
    PTREE node )                // - node
;
CNV_RETN NodeConvertPtr(        // CONVERT A POINTER
    unsigned conversion,        // - type of conversion
    PTREE *expr,                // - expression to be converted
    TYPE src,                   // - source type (converted from)
    TYPE tgt )                  // - target type (converted to)
;
void NodeConvertToBasePtr(      // CONVERT TO A BASE PTR, USING SEARCH_RESULT
    PTREE *expr,                // - addr( ptr to be converted )
    TYPE base,                  // - base type
    SEARCH_RESULT *result,      // - search result
    boolean positive )          // - TRUE ==> use positive value
;
PTREE NodeConvertVirtualPtr(    // EXECUTE A VIRTUAL BASE CAST
    PTREE expr,                 // - expr to cast
    TYPE final_type,            // - final type after cast
    target_offset_t vb_offset,  // - offset of vbptr
    vindex vb_index )           // - index in vbtable
;
PTREE NodeCopyClassObject(      // COPY OBJECT W/O CTOR
    PTREE tgt,                  // - target object (LVALUE)
    PTREE src )                 // - source object (RVALUE)
;
boolean NodeDerefPtr(           // DEREFERENCE A POINTER
    PTREE *a_ptr )              // - addr[ ptr operand ]
;
PTREE NodeDone(                 // MAKE A NODE-DONE
    PTREE expr )                // - expression
;
PTREE NodeDottedFunction        // BUILD A DOT NODE FOR A FUNCTION
    ( PTREE left                // - left operand
    , PTREE right )             // - right operand
;
PTREE NodeDtorExpr(             // MARK FOR DTOR'ING AFTER EXPRESSION
    PTREE expr,                 // - expression computing symbol
    SYMBOL sym )                // - SYMBOL being computed
;
PTREE NodeDupExpr(              // DUPLICATE EXPRESSION
    PTREE *expr )               // - addr( expression )
;
PTREE NodeFetch(                // FETCH A VALUE
    PTREE curr )                // - node to be transformed
;
PTREE NodeFetchClassExact(      // FETCH AS "CLASS_EXACT"
    PTREE node )                // - node to be fetched
;
PTREE NodeFetchReference(       // FETCH A REFERENCE, IF REQ'D
    PTREE expr )                // - expression
;
PTREE NodeForceLvalue           // FORCE EXPRESSION TO BE LVALUE
    ( PTREE expr )              // - expression
;
void NodeFreeDupedExpr(         // FREE AN EXPRESSION WITH DUPLICATES
    PTREE expr )                // - expression to be freed
;
void NodeFreeSearchResult(      // FREE SEARCH_RESULT FROM A NODE
    PTREE node )                // - the node
;
PTREE NodeFromConstSym(         // BUILD CONSTANT NODE FROM CONSTANT SYMBOL
    SYMBOL con )                // - constant symbol
;
PTREE NodeFuncForCall(          // GET FUNCTION NODE FOR CALL
    PTREE call_node )           // - a call node
;
PTREE NodeGetCallExpr(          // POINT AT CALL EXPRESSION
    PTREE expr )                // - result from AnalyseCall
;
PTREE NodeGetConstantNode(      // RETURN CONSTANT NODE
    PTREE node )                // - node
;
boolean NodeGetIbpSymbol(       // GET BOUND-REFERENCE SYMBOL, IF POSSIBLE
    PTREE node,                 // - node
    SYMBOL* a_ibp,              // - bound parameter to use
    target_offset_t* a_offset ) // - addr[ offset to basing symbol ]
;
PTREE NodeIc(                   // ADD A PTREE-IC NODE
    CGINTEROP opcode )          // - opcode
;
PTREE NodeIcUnsigned(           // ADD A PTREE-IC NODE, UNSIGNED OPERAND
    CGINTEROP opcode,           // - opcode
    unsigned operand )          // - operand
;
PTREE NodeIntDummy              // BUILD A DUMMY INTEGRAL NODE
    ( void )
;
PTREE NodeIntegralConstant      // BUILD AN INTEGRAL NODE FOR A VALUE
    ( int val                   // - value
    , TYPE type )               // - node type (integral,enum,ptr)
;
#ifdef NDEBUG
#define NodeIsBinaryOp(node,op) ___NodeIsOp(node,op)
#else
boolean NodeIsBinaryOp(         // TEST IF BINARY OPERATION OF GIVEN TYPE
    PTREE node,                 // - node
    CGOP operation )            // - operation
;
#endif
boolean NodeIsConstantInt(      // TEST IF A CONSTANT INT NODE
    PTREE node )                // - node
;
boolean NodeIsConstant(         // TEST IF A CONSTANT NODE
    PTREE node )                // - node
;
#if 0 // not used
boolean NodeIsCppFuncCall(      // DETERMINE IF NODE IS CALL TO C++ FUNCTION
    PTREE node )                // - node
;
#endif
boolean NodeIsIntConstant       // TEST IF INTEGRAL CONSTANT AND GET VALUE
    ( PTREE node                // - potential constant node
    , INT_CONSTANT* pval )      // - addr[ value ]
;
#ifdef NDEBUG
#define NodeIsUnaryOp(node,op) ___NodeIsOp(node,op)
#else
boolean NodeIsUnaryOp(          // TEST IF UNARY OPERATION OF GIVEN TYPE
    PTREE node,                 // - node
    CGOP operation )            // - operation
;
#endif
boolean NodeIsZeroConstant(     // TEST IF A ZERO CONSTANT
    PTREE node )                // - node
;
boolean NodeIsZeroIntConstant(  // TEST IF A ZERO INTEGER CONSTANT
    PTREE node )                // - node
;
PTREE NodeLvExtract             // EXTRACT LVALUE, IF POSSIBLE
    ( PTREE expr )              // - expression
;
PTREE NodeLvForRefClass         // MAKE LVALUE FOR REF CLASS
    ( PTREE expr )              // - LVALUE ref-class expression
;
PTREE NodeUnComma(              // EXTRACT OUT UNCOMMA'D EXPR (rest is stashed)
    PTREE expr,                 // - (possibly comma'd) expr
    PTREE *pextra )             // - extra subtrees stored here
;
PTREE NodeMakeCall(             // FABRICATE A FUNCTION CALL EXPRESSION
    SYMBOL proc,                // - procedure
    TYPE type,                  // - return type
    PTREE args )                // - arguments
;
PTREE NodeMakeCallee(           // MAKE A CALLEE NODE
    SYMBOL func )               // - function
;
PTREE NodeMakeVirtCall(         // FABRICATE A VIRTUAL FUNCTION CALL EXPR'N
    SYMBOL proc,                // - procedure
    SEARCH_RESULT *result,      // - search result for lookup
    TYPE type,                  // - return type
    PTREE args )                // - arguments
;
PTREE NodeMarkRvalue(           // SET TYPE, FLAGS FOR LVALUE
    PTREE node )                // - node
;
PTREE NodeModInitTemporary(     // CREATE TEMP NODE (STATIC IF IN MODULE-INIT)
    TYPE type )                 // - type of temporary
;
boolean NodeNonConstRefToTemp(  // CHECK IF TEMP. PASSED AS NON-CONST REF
    TYPE arg_type,              // - possible non-const reference
    PTREE node )                // - possible temporary
;
PTREE NodeOffset(               // BUILD CONSTANT NODE FOR AN OFFSET
    target_offset_t offset )    // - the offset
;
PTREE NodeTestExpr(             // GENERATE A TERNARY TEST EXPRESSION
    PTREE b_expr,               // - boolean expression
    PTREE t_expr,               // - TRUE expression
    PTREE f_expr )              // - FALSE expression
;
PTREE NodePromoteDups(          // PROMOTE/REMOVE DUPLICATE NODES
    PTREE dup,                  // - tree to be processed
    TOKEN_LOCN* locn )          // - expression location
;
PTREE NodePruneLeft(            // PRUNE LEFT OPERAND
    PTREE expr )                // - expression
;
PTREE NodePruneLeftTop(         // PRUNE LEFT, THEN TOP
    PTREE expr )                // - expression to be pruned
;
PTREE NodePruneRight(            // PRUNE RIGHT OPERAND
    PTREE expr )                // - expression
;
PTREE NodePruneTop(             // PRUNE TOP OPERAND
    PTREE expr )                // - expression
;
boolean NodePtrNonZero(         // TEST IF A PTR NODE IS ALWAYS NON-ZERO
    PTREE node )                // - node to be tested
;
boolean NodeReferencesTemporary( // CHECK IF NODE PRODUCES OR IS TEMPORARY
    PTREE node )                // - possible temporary
;
PTREE NodeRemoveCasts(          // REMOVE CASTING FROM NODE
    PTREE node )                // - starting node
;
PTREE NodeRemoveCastsCommas(    // REMOVE COMMAS, DTORING, CASTING FROM NODE
    PTREE node )                // - starting node
;
PTREE NodeReplace(              // REPLACE EXPRESSION WITH ANOTHER
    PTREE old,                  // - expression to be replaced
    PTREE replace )             // - replacement expression
;
PTREE NodeReplaceTop(           // REPLACE TOP EXPRESSION WITH ANOTHER
    PTREE old,                  // - expression to be replaced
    PTREE replace )             // - replacement expression
;
PTREE* NodeReturnSrc(           // GET ADDR OF SOURCE OPERAND RETURNED
    PTREE* src,                 // - addr[ operand ]
    PTREE* dtor )               // - addr[ CO_DTOR operand ]
;
PTREE NodeReturnSymbol(         // MAKE NODE FOR RETURN SYMBOL
    void )
;
PTREE NodeReverseArgs(          // REVERSE CALL ARGUMENTS
    unsigned *arg_count,        // - # args
    PTREE arg )                 // - start of arg. list (to be reversed)
;
PTREE NodeRvalue(               // GET RVALUE, IF LVALUE
    PTREE curr )                // - node to be transformed
;
PTREE NodeRvalueExact(          // SET RVALUE (EXACT)
    PTREE node )                // - current node
;
PTREE NodeRvalueExactLeft(      // SET RVALUE (EXACT) ON LEFT
    PTREE node )                // - current node
;
PTREE NodeRvalueExactRight(     // SET RVALUE (EXACT) ON RIGHT
    PTREE node )                // - current node
;
PTREE NodeRvalueLeft(           // SET RVALUE ON LEFT
    PTREE node )                // - current node
;
PTREE NodeRvalueRight(          // SET RVALUE ON RIGHT
    PTREE node )                // - current node
;
PTREE NodeRvForRefClass         // MAKE RVALUE FOR REF CLASS
    ( PTREE expr )              // - LVALUE ref-class expression
;
PTREE NodeSegname(              // BUILD EXPRESSION FOR __segname
    char* segname )             // - name of segment
;
PTREE NodeSetMemoryExact(       // SET PTF_MEMORY_EXACT, IF REQ'D
    PTREE expr )                // - expression
;
TYPE NodeSetReference(          // MAKE AN LVALUE IF REFERENCE TYPE
    TYPE type,                  // - type
    PTREE node )                // - node
;
PTREE NodeStripPoints(          // STRIP * APPLIED TO & OPERATION
    PTREE expr )                // - * expression
;
PTREE NodeSymbol(               // CONVERT NODE TO BE SYMBOL
    PTREE expr,                 // - node
    SYMBOL sym,                 // - symbol
    SEARCH_RESULT *result )     // - result of lookup
;
PTREE NodeSymbolCallee(         // MAKE SYMBOL FOR A CALLEE
    PTREE expr,                 // - node
    SYMBOL sym,                 // - symbol
    SEARCH_RESULT *result )     // - search result
;
PTREE NodeSymbolNoRef(          // FILL IN NODE FOR A SYMBOL, NO REF. SETTING
    PTREE expr,                 // - node
    SYMBOL sym,                 // - symbol
    SEARCH_RESULT *result )     // - search result
;
PTREE NodeTemporary(            // CREATE TEMPORARY AND NODE FOR IT
    TYPE type )                 // - type of temporary
;
PTREE NodeThis(                 // MAKE A RVALUE "THIS" NODE
    void )
;
PTREE NodeThisCopyLocation(     // MAKE A RVALUE "THIS" NODE WITH LOCATION
    PTREE use_locn )            // - node to grab locn from
;
TYPE NodeType(                  // GET TYPE FOR A NODE
    PTREE node )                // - the node
;
PTREE NodeUnary(                // MAKE A UNARY NODE
    CGOP op,                    // - operator
    PTREE expr )                // - operand
;
PTREE NodeUnaryCopy(            // MAKE A UNARY NODE, COPY ATTRIBUTES
    CGOP op,                    // - operator
    PTREE expr )                // - operand
;
void NodeUnduplicate(           // UNDUPLICATE A NODE
    PTREE node )                // - node to be undupicated
;
void NodeWarnPtrTrunc(          // WARN FOR POINTER/REFERENCE TRUNCATION
    PTREE node )                // - node for warning
;
void NodeWarnPtrTruncCast(      // WARN FOR CAST POINTER/REFERENCE TRUNCATION
    PTREE node )                // - node for warning
;
PTREE NodeZero                  // BUILD A ZERO NODE
    ( void )
;
PTREE OverloadOperator(         // HANDLE OPERATOR OVERLOADING, IF REQ'D
    PTREE op )                  // - node containing operator
;
boolean PointerToFuncEquivalent( // TEST IF EQUIVALENT TO PTR(FUNCTION)
    TYPE type )
;
unsigned PtrConvertCommon(      // CONVERT TO COMMON PTR
    PTREE expr )                // - expression
;
boolean ReverseParms(           // ASK IF PRAGMA REQUIRES REVERSED PARMS
    AUX_INFO * pragma )         // - pragma
;
boolean PragmaName(             // RETURN TRUE IF NAME OF PRAGMA IS SIMPLE
    AUX_INFO *pragma,           // - pragma
    char **id )                 // - returned name
;
void ScopeGenAccessReset(       // RESET ACCESS SCOPE FOR GENERATION
    void )
;
void ScopeGenAccessSet(         // SET ACCESS SCOPE FOR GENERATION
    TYPE cltype )               // - type for inlining
;
unsigned SizeTargetSizeT(       // GET SIZE OF TARGET'S size_t
    void )
;
void SrcPosnEmit(               // EMIT SOURCE POSITION
    TOKEN_LOCN *posn )          // - position to emit
;
TOKEN_LOCN SrcPosnEmitCurrent(  // EMIT LINE NO. FOR CURRENT POSITION
    void )
;
unsigned StaticInitFuncBeg(     // START INITIALIZATION OF STATIC IN FUNCTION
    void )
;
void StaticInitFuncEnd(         // END INITIALIZATION OF STATIC IN FUNCTION
    unsigned init_label )       // - label #
;
SYMBOL StaticInitSymbol(        // CREATE STATIC INITIALIZATION SYMBOL
    SYMBOL sym )                // - symbol type
;
SYMBOL TemporaryAlloc(          // ALLOCATE DEFAULT TEMPORARY
    TYPE type )                 // - type of temporary
;
SYMBOL TemporaryAllocNoStorage( // ALLOCATE DEFAULT TEMPORARY WITHOUT STORAGE
    TYPE type )                 // - type of temporary
;
TEMP_TYPE TemporaryClass(       // SET DEFAULT CLASS FOR TEMPORARIES
    TEMP_TYPE new_type )        // - new type
;
CLASSINFO *TypeClassInfo(       // GET CLASSINFO PTR FOR A TYPE
    TYPE type )                 // - input type
;
TYPE TypeBinArithResult(        // TYPE OF BINARY ARITHMETIC RESULT
    TYPE op1,                   // - type[1]
    TYPE op2 )                  // - type[2]
;
CTD TypeCommonDerivation(       // GET COMMON TYPE DERIVATION FOR TWO TYPES
    TYPE type1,                 // - type [1] (left)
    TYPE type2 )                // - type [2] (right)
;
TYPE TypeAutoDefault(           // ADD NEAR QUALIFIER FOR AUTO SYMBOL
    TYPE type,                  // - a type
    PTREE expr )                // - possible PT_SYMBOL of SC_AUTO
;
TYPE TypeConvertFromPcPtr(      // TRANSFORM TYPE AFTER CONVERSION FROM PC PTR
    TYPE type )                 // - pointer type
;
boolean TypeDefedNonAbstract    // REQUIRE DEFINED, NON-ABSTRACT TYPE
    ( TYPE type                 // - the type
    , PTREE expr                // - NULL or expression for error
    , MSG_NUM msg_abstract      // - message when abstract
    , MSG_NUM msg_undefed )     // - message when undefined
;
boolean TypeExactDtorable(      // TEST IF EXACT TYPE REALLY NEEDS DTOR'ING
    TYPE type )                 // - declared type
;
TYPE TypeForLvalue              // GET TYPE FOR LVALUE
    ( PTREE expr )              // - lvalue expression
;
TYPE TypeFunctionCalled(        // GET FUNCTION DECLARATION TYPE CALLED
    TYPE type )                 // - function type called
;
boolean TypeHasDtorableObjects( // TEST IF TYPE HAS DTORABLE SUB-OBJECTS
    TYPE type )                 // - a class type
;
boolean TypeIsBasedPtr(         // SEE IF A PTR TO BASED ITEM
    TYPE type )                 // - the type
;
#if 0 // not used
boolean TypeIsCppFunc(          // TEST IF C++ FUNCTION TYPE
    TYPE type )                 // - type to be tested
;
#endif
TYPE TypeMergeForMember(        // CREATE MERGED TYPE FOR A MEMBER
    TYPE owner,                 // - type on left (.), type pointed at (->)
    TYPE member )               // - type for member
;
PC_PTR TypePcPtr(               // CLASSIFY PTR FOR PC
    type_flag flag )            // - modifier flags for pointed item
;
boolean TypeReallyDefCtorable(  // TEST IF TYPE REALLY NEEDS DEFAULT CTOR'ING
    TYPE type )                 // - declared type
;
boolean TypeReallyDtorable(     // TEST IF TYPE REALLY NEEDS DTOR'ING
    TYPE type )                 // - declared type
;
TYPE TypeRebuildPcPtr(          // REBUILD PC-PTR TYPE
    TYPE type,                  // - type of element pointed at (unmodified)
    type_flag old_flags,        // - old flags
    type_flag new_flags )       // - new flags
;
TYPE TypeReferenced(            // GET TYPE OR TYPE REFERENCED
    TYPE type )                 // - original type
;
boolean TypeRequiresRWMemory(   // TEST IF TYPE MUST BE IN NON-CONST STORAGE
    TYPE type )                 // - type
;
boolean TypeRequiresCtoring(    // TEST IF TYPE MUST BE CTOR'ED
    TYPE type )                 // - type
;
boolean TypeRequiresCtorParm(   // TEST IF EXTRA CTOR PARM REQUIRED
    TYPE type )                 // - the type
;
boolean TypeRequiresDtoring(    // TEST IF TYPE MUST BE DTOR'ED
    TYPE type )                 // - type
;
TYPE TypeSegAddr(               // GET INTERNAL TYPE OF BASE :> ADDRESS
    void )
;
TYPE TypeSegId(                 // GET TYPE OF SEGMENT ID
    void )
;
TYPE TypeSegOp(                 // GET TYPE FOR :> OPERATION
    TYPE type )                 // - type of RHS of :>
;
TYPE TypeTargetSizeT(           // GET TYPE OF TARGET'S size_t
    void )
;
TYPE TypeThis(                  // GET TYPE OF THIS FOR MEMBER BEING DEFINED
    void )
;
TYPE TypeThisExists(            // GET BASE TYPE OF THIS FOR MEMBER BEING DEFINED
    void )
;
TYPE TypeThisForCall(           // GET "THIS" TYPE FOR A CALL
    PTREE this_node,            // - this node
    SYMBOL sym )                // - function being called
;
TYPE TypeThisSymbol(            // GET TYPE OF THIS FOR SYMBOL MEMBER
    SYMBOL sym,                 // - symbol
    boolean reference )         // - use reference?
;
boolean TypeTruncByMemModel(    // TEST TYPE TRUNCATED TO DEF. MEMORY MODEL
    TYPE type )                 // - the type
;
TYPE TypeUnArithResult(         // TYPE OF UNARY ARITHMETIC RESULT
    TYPE op1 )                  // - type
;
PTREE UdcCall                   // CALL UDC FUNCTION
    ( PTREE src                 // - source expression
    , TYPE udcf_type            // - type for udcf
    , CALL_DIAG* diagnosis )    // - call diagnosis
;
TYPE UdcFindType                // FIND TARGET TYPE FOR UDCF
    ( TYPE src                  // - source class or reference to it
    , TYPE tgt )                // - target type
;
TYPE UserDefTypeForType(        // GET ENUM or CLASS TYPE FOR TYPE OR REFERENCE TO IT
    TYPE type )                 // - input type
;
unsigned UserDefCnvToType(      // DO A USER-DEFINED CONVERSION TO A TYPE
    PTREE *expr,                // - addr( expression to be converted )
    TYPE src,                   // - source type (a class)
    TYPE tgt )                  // - target type
;

#endif
