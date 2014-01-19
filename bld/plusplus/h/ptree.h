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
* Description:  Parse tree structures, definitions, and prototypes.
*
****************************************************************************/


#ifndef _PTREE_H

#include "i64.h"

#include "stringl.h"
#include "ppops.h"
#include "srcfile.h"
#include "ptreedec.h"

#define PTreePCHRead()      PTreeMapIndex( (PTREE)(pointer_int)PCHReadCVIndex() )
#define PTreePCHWrite(x)    PCHWriteCVIndex( (cv_index)(pointer_int)PTreeGetIndex(x) )

/*
    flags are organized into three classes:

    (1) PTS_*           -- read/only flag based on PTREE->op
    (2) PTO_*           -- read/only flag based on PTREE->cgop
                           (if ( PTS_FLAG(PTREE->op) & PTS_HAS_CGOP ) != 0 )
    (3) PTF_*           -- read/write flag in PTREE->flag
*/

typedef enum
{   PTS_UNARY           = 0x0001        // node may have a valid u.subtree[0]
,   PTS_BINARY          = 0x0002        // node may have a valid u.subtree[1]
,   PTS_HAS_CGOP        = 0x0004        // node is unary/binary with a cgop

,   PTS_OPERATOR        = PTS_UNARY     // node may have valid subtrees
                        | PTS_BINARY
,   PTS_NULL            = 0x0000
} PTS_FLAG;

typedef enum                            // operator attributes
{   PTO_UNARY           = 0x0001        // - unary operator
,   PTO_BINARY          = 0x0002        // - binary operator
,   PTO_BOOLEAN         = 0x0004        // - operator returns a boolean
,   PTO_OVLOAD          = 0x0008        // - operator can be overloaded
,   PTO_CNV             = 0x01f0        // - conversion num (need PTO_CNV_SHIFT)
,   PTO_RVALUE          = 0x0200        // - makes an rvalue from an lvalue
,   PTO_ASSIGN_SAME     = 0x0400        // - <lhs> @= <rhs>; rhs will be truncated
,   PTO_NULL            = 0x0000
                                        // derived attributes
,   PTO_CAN_OVERLOAD    = PTO_CNV       // - 0 != mask ==> can overload
                        | PTO_OVLOAD
} PTO_FLAG;

#define PTO_CNV_SHIFT   4

#define PtfFlags                          /* Flags for Ptree Nodes                  */ \
  PtfFlag( SIDE_EFF        , 0x00000001 ) /* - side effect in sub-tree              */ \
, PtfFlag( LVALUE          , 0x00000002 ) /* - sub-tree is lvalue                   */ \
, PtfFlag( BOOLEAN         , 0x00000004 ) /* - node is a boolean result             */ \
, PtfFlag( MEMORY_EXACT    , 0x00000008 ) /* - memory for class is exact            */ \
, PtfFlag( MEANINGFUL      , 0x00000010 ) /* - sub-tree is meaningful (1)           */ \
, PtfFlag( TEMP_CONV       , 0x00000020 ) /* - temp. created by conversion          */ \
, PtfFlag( THROW_EXPR      , 0x00000040 ) /* - expression is throw expression       */ \
, PtfFlag( CALLED_ONLY     , 0x00000080 ) /* - expression can only be called        */ \
, PtfFlag( NEEDS_THIS      , 0x00000080 ) /* - PT_SYMBOL without "this"             */ \
, PtfFlag( COLON_QUALED    , 0x00000100 ) /* - PT_SYMBOL was :: qualified           */ \
, PtfFlag( PTR_NONZERO     , 0x00000200 ) /* - pointer is known to be non-zero      */ \
, PtfFlag( CLASS_RVREF     , 0x00000400 ) /* - use ref type for class rvalue        */ \
, PtfFlag( COND_END        , 0x00000800 ) /* - conditional end on flow expr         */ \
, PtfFlag( ARG_THIS        , 0x00001000 ) /* - CO_LIST: this argument               */ \
, PtfFlag( ARG_RETURN      , 0x00002000 ) /* - CO_LIST: return argument             */ \
, PtfFlag( DUP_VISITED     , 0x00004000 ) /* - PT_DUP_EXPR: dup node processed      */ \
, PtfFlag( STATIC_INIT     , 0x00008000 ) /* - PT_BINARY: static init.              */ \
, PtfFlag( BLOCK_INIT      , 0x00010000 ) /* - PT_BINARY: block init.               */ \
, PtfFlag( DTORABLE_INIT   , 0x00020000 ) /* - initialization of DTORABLE object    */ \
, PtfFlag( ARG_THIS_VFUN   , 0x00040000 ) /* - CO_LIST: this argument for vfun      */ \
, PtfFlag( LV_CHECKED      , 0x00080000 ) /* - set to indicate no LV checking       */ \
, PtfFlag( RETN_OPT        , 0x00100000 ) /* - CO_DONE: expr is return optimized    */ \
, PtfFlag( WAS_INDEX       , 0x00200000 ) /* - CO_DOT: was CO_INDEX                 */ \
, PtfFlag( TYPES_DIAGED    , 0x00400000 ) /* - PT_ERROR: types have been diag'ed    */ \
, PtfFlag( DEFARG_COPY     , 0x00800000 ) /* - ptree is copy of a defarg ptree      */ \
, PtfFlag( KEEP_MPTR_SIMPLE, 0x01000000 ) /* - if RHS ends up a member-ptr const, leave it alone! */ \
, PtfFlag( ALREADY_ANALYSED, 0x02000000 ) /* - ptree already analysed               */ \
, PtfFlag( TYPENAME,         0x04000000 ) /* - typename                             */ \
, PtfFlag( TRAVERSE_LEFT   , 0x80000000 ) /* - traversing left tree                 */ \

typedef enum                            // operand definitions
{
    #define PtfFlag(a,b) PTF_ ## a = b
    PtfFlags
    #undef  PtfFlag
,   PTF_NULL            = 0x00000000    // - no flags set

,   PTF_ARGS = PTF_ARG_THIS             // - argument flags
             | PTF_ARG_RETURN
             | PTF_ARG_THIS_VFUN

,   PTF_FETCH = PTF_SIDE_EFF
              | PTF_LVALUE
              | PTF_MEMORY_EXACT
              | PTF_MEANINGFUL
              | PTF_PTR_NONZERO
              | PTF_CLASS_RVREF

,   PTF_NEVER_PROPPED = PTF_CALLED_ONLY
                      | PTF_COLON_QUALED
                      | PTF_COND_END
                      | PTF_ARG_THIS
                      | PTF_ARG_RETURN
                      | PTF_DTORABLE_INIT
} PTF_FLAG;

/*
    notes:
        (1) a sub-tree is meaningful if the operator on top has a side-effect
            or if it can appear as a top-most operator in a statement expr
            (e.g., cast to (void), *<ptr-expr> is allowed so putc macro is OK)
*/

#define PTO_OPERATOR    ( PTO_BINARY | PTO_UNARY )

#define PTO_BINARY_OVLD ( PTO_BINARY | PTO_OVLOAD )
#define PTO_UNARY_OVLD  ( PTO_UNARY | PTO_OVLOAD )
#define PTO_BINARY_CPP  ( PTO_BINARY )
#define PTO_UNARY_CPP   ( PTO_UNARY )
#define PTO_BINARY_NO_OVLD ( PTO_BINARY )
#define PTO_UNARY_NO_OVLD  ( PTO_UNARY  )

#define PTO_BIN_ARITH   ( PTO_BINARY_OVLD  )
#define PTO_UN_ARITH    ( PTO_UNARY_OVLD )
#define PTO_BIN_ASSIGN  ( PTO_BINARY_OVLD )
#define PTO_UN_ASSIGN   ( PTO_UNARY_OVLD )
#define PTO_COMPARES    ( PTO_BINARY_OVLD | PTO_BOOLEAN )
#define PTO_FLOW_UNARY  ( PTO_UNARY_OVLD  | PTO_BOOLEAN )
#define PTO_FLOW_BINARY ( PTO_BINARY_OVLD | PTO_BOOLEAN )
#define PTO_ADDR_OPER   ( PTO_UNARY_OVLD  )
#define PTO_INDEX       ( PTO_BINARY_OVLD )
#define PTO_POINTS_TO   ( PTO_BINARY_OVLD )
#define PTO_FUNCALL     ( PTO_BINARY_OVLD )
#define PTO_DOT         ( PTO_BINARY_NO_OVLD )
#define PTO_FETCH       ( PTO_UNARY_CPP | PTO_RVALUE )
#define PTO_VFETCH      ( PTO_UNARY_CPP      )

typedef enum
    #define PTOP( code, bits ) code
    #include "ptreeop.h"
    #undef PTOP
ptree_op_t;

#define PT_ERROR PT_NULL

typedef struct parse_tree_node {
    TYPE                type;
    PTF_FLAG            flags;
    ptree_op_t          op;
    CGOP                cgop;
    CGOP                id_cgop;
    uint_8              filler;
    SYMBOL_NAME         sym_name;
    PTD                 *decor;                 // decoration for node
    TOKEN_LOCN          locn;
    union {
        STRING_CONSTANT         string;         // PT_STRING_CONSTANT
#if defined( WATCOM_BIG_ENDIAN )
// overlay on second 32-bitter
      struct {
        target_long             int_padding;
        target_long             int_constant;   // PT_INT_CONSTANT
      };
      struct {
        target_ulong            uint_padding;
        target_ulong            uint_constant;  // PT_INT_CONSTANT
      };
#else
// overlay on first 32-bitter
        target_long             int_constant;   // PT_INT_CONSTANT
        target_ulong            uint_constant;  // PT_INT_CONSTANT
#endif
        float_handle            floating_constant; // PT_FLOATING_CONSTANT
        signed_64               int64_constant; // PT_INT_CONSTANT
        struct {                                // PT_ID
            NAME                name;
            SCOPE               scope;
        } id;
        struct {                                // PT_TYPE
            PTREE               next;
            SCOPE               scope;
        } type;
        struct {                                // PT_SYMBOL (normal)
            SYMBOL              symbol;         // - symbol entry
            SEARCH_RESULT *     result;         // - result type from look-up
        } symcg;
        struct {                                // PT_DUP_EXPR (normal)
            PTREE               subtree[1];     // - subtree or NULL
            PTREE               node;           // - duplication partner
        } dup;
        CGINTER                 ic;             // PT_IC
        PTREE                   subtree[2];     // all others
    } u;
} PTREE_NODE;

extern PTREE PTreeAlloc( void );
extern PTREE PTreeAssign( PTREE to, PTREE from );
extern PTREE PTreeAssignReloc( PTREE to, PTREE from, RELOC_LIST *reloc_list );
extern PTREE PTreeFree( PTREE );
extern void PTreeFreeSubtrees( PTREE );
extern PTREE PTreeBoolConstant( int );
extern PTREE PTreeIntConstant( int, type_id );
extern PTREE PTreeInt64Constant( signed_64, type_id );
//extern PTREE PTreeUIntConstant( unsigned, type_id );
extern PTREE PTreeFloatingConstantStr( const char *, type_id );
extern PTREE PTreeFloatingConstant( float_handle, type_id );
extern PTREE PTreeId( NAME );
extern PTREE PTreeIdSym( SYMBOL );
extern PTREE PTreeDottedSyms( SYMBOL base, SYMBOL qual );
extern PTREE PTreeSymbol( SYMBOL, char * );
extern PTREE PTreeThis( void );
extern PTREE PTreeCDtorExtra( void );
extern PTREE PTreeLiteral( STRING_CONSTANT );
extern PTREE PTreeLiteralWide( STRING_CONSTANT );
extern PTREE PTreeDupExpr( PTREE );
extern PTREE PTreeType( TYPE );
extern PTREE PTreeForceIntegral( PTREE );
extern PTREE PTreeNonZeroConstantExpr( PTREE );
extern PTREE PTreeTListAppend( PTREE, PTREE );
extern PTREE PTreeReplaceLeft( PTREE, PTREE );
extern PTREE PTreeReplaceRight( PTREE, PTREE );
extern PTREE ThrowsAnything( void );
extern PTREE ConvertToType( PTREE );

typedef enum {                  // parse tree traversal states
    PTREE_LEFT,                 // - descend down left subtree (if possible)
    PTREE_RIGHT,                // - descend down right subtree (if possible)
    PTREE_PROCESS,              // - process current tree node
    PTREE_UNWIND,               // - move back up to parent of current tree node
} PTREE_STATE;

PTREE PTreeBinary(              // CREATE BINARY PTREE NODE
    CGOP cgop,                  // - operator for code generation
    PTREE sub_1,                // - subtree to left
    PTREE sub_2 )               // - subtree to right
;
PTREE PTreeCopySrcLocation(     // COPY LOCATION OF SOURCE
    PTREE tgt,                  // - target node
    PTREE src )                 // - source node
;
PTF_FLAG PTreeEffFlags(         // GET MEANINGFUL FLAGS FOR A PTREE OPERAND
    PTREE node )                // - node
;
PTREE PTreeSetErrLoc(           // SET THE ERROR LOCATION FOR A TREE
    PTREE tree )                // - tree with error
;
msg_status_t PTreeErrorExpr(    // ISSUE ERROR MESSAGE FOR PTREE NODE
    PTREE expr,                 // - node for error
    MSG_NUM err_code )          // - error code
;
void PTreeErrorExprName(        // ISSUE ERROR MESSAGE FOR PTREE NODE, NAME
    PTREE expr,                 // - node for error
    MSG_NUM err_code,           // - error code
    NAME name )                 // - name
;
void PTreeErrorExprSym(         // ISSUE ERROR MESSAGE FOR PTREE NODE, SYMBOL
    PTREE expr,                 // - node for error
    MSG_NUM err_code,           // - error code
    SYMBOL sym )                // - symbol
;
void PTreeErrorExprSymInf(      // ISSUE ERROR MESSAGE FOR PTREE NODE, SYMBOL
    PTREE expr,                 // - node for error
    MSG_NUM err_code,           // - error code
    SYMBOL sym )                // - symbol
;
void PTreeErrorExprType(        // ISSUE ERROR MESSAGE FOR PTREE NODE, TYPE
    PTREE expr,                 // - node for error
    MSG_NUM err_code,           // - error code
    TYPE type )                 // - type
;
void PTreeErrorExprNameType(    // ISSUE ERROR MESSAGE FOR PTREE NODE, NAME, TYPE
    PTREE expr,                 // - node for error
    MSG_NUM err_code,           // - error code
    NAME name,                  // - name
    TYPE type )                 // - type
;
PTREE PTreeErrorNode(           // SET NODE TO BE AN ERROR
    PTREE curr )                // - node to be transformed
;
PTREE PTreeIc(                  // CREATE PT_IC NODE
    CGINTEROP opcode,           // - IC opcode
    CGVALUE value )             // - IC value
;
PTO_FLAG PTreeOpFlags(          // GET FLAGS FOR A PTREE NODE
    PTREE curr )                // - current node
;
PTREE PTreeOp(                  // FETCH (OVER COMMAS) PTREE PTR.
    PTREE *addr_expr )          // - addr( PTREE node )
;
PTREE PTreeOpLeft(              // GET LEFT NODE, SKIPPING "," OPERATOR
    PTREE expr )                // - expression
;
PTREE PTreeOpRight(             // GET RIGHT NODE, SKIPPING "," OPERATOR
    PTREE expr )                // - expression
;
PTREE PTreePromoteLocn(         // ENSURE TOP OF TREE HAS SOURCE LOCATION
    PTREE tree )                // - the tree
;
bool PTreePropogateError(       // CHECK AND PROPOGATE ERRORS FROM SUB-TREES
    PTREE curr )                // - current node
;
PTREE *PTreeRef(                // FIND REFERENCE TO OPERAND
    PTREE *tgt )                // - target location
;
PTREE *PTreeRefLeft(            // FIND REFERENCE TO LEFT OPERAND
    PTREE expr )                // - expression
;
PTREE *PTreeRefRight(           // FIND REFERENCE TO RIGHT OPERAND
    PTREE expr )                // - expression
;
PTREE PTreeSetLocn(             // SET LOCATION IN A PTREE NODE
    PTREE tree,                 // - node
    TOKEN_LOCN *locn )          // - location
;
PTREE PTreeTraverseInitRef(     // TRAVERSE A PTREE FOR REFERENCE INIT.
    PTREE tree,                 // - tree to be traversed
    PTREE (*visit_rtn)          // - visiting routine
        ( PTREE curr ) )        // - - addr( current node )
;
PTREE PTreeTraversePostfix(     // TRAVERSE A PTREE IN (LEFT,RIGHT,SELF) ORDER
    PTREE tree,                 // - tree to be traversed
    PTREE (*visit_rtn)          // - visiting routine
        ( PTREE curr ) )        // - - current node
;
PTREE PTreeTraversePrefix(      // TRAVERSE A PTREE IN (SELF,LEFT,RIGHT) ORDER
    PTREE tree,                 // - tree to be traversed
    PTREE (*visit_rtn)          // - visiting routine
        ( PTREE curr ) )        // - - addr( current node )
;
PTREE PTreeCopyPrefix(          // COPY A PTREE IN (SELF,LEFT,RIGHT) ORDER
    PTREE tree,                 // - tree to be copied
    PTREE (*copy_rtn)           // - copy routine
        ( PTREE curr,           // - - addr( current node )
          void *param ),        // - - parameter
    void *param  )              // param to pass to copy_rtn
;
PTREE PTreeUnary(               // CREATE UNARY PTREE NODE
    CGOP cgop,                  // - operator for code generation
    PTREE sub_1 )               // - subtree to left
;
msg_status_t PTreeWarnExpr(     // ISSUE WARNING MESSAGE FOR PTREE NODE
    PTREE expr,                 // - node for error
    MSG_NUM err_code )          // - error code
;

extern PTREE SimpleDestructorId( TYPE );
extern PTREE MakeDestructorId( PTREE );
extern PTREE MakeDestructorIdFromType( DECL_SPEC * );
extern PTREE MakeOperatorId( CGOP );
extern PTREE MakeUserConversionId( DECL_SPEC *, DECL_INFO * );
extern PTREE MakeFunctionLikeCast( DECL_SPEC *, PTREE );
extern PTREE MakeNormalCast( PTREE, PTREE );
extern PTREE MakeMemInitItem( DECL_SPEC *, PTREE, PTREE, TOKEN_LOCN * );
extern PTREE MakeIdFromType( DECL_SPEC * );
extern PTREE MakeDeleteExpr( PTREE, CGOP, PTREE );
extern PTREE MakeGlobalId( PTREE );
extern PTREE MakeGlobalOperatorId( PTREE, CGOP );
extern PTREE MakeScopedId( PTREE );
extern PTREE CheckScopedId( PTREE );
extern PTREE MakeScopedDestructorId( PTREE, PTREE );
extern PTREE MakeScopedUserConversionId( PTREE, PTREE );
extern PTREE MakeScopedOperatorId( PTREE, CGOP );
extern bool IsLinkerConstant( PTREE, PTREE *, target_size_t * );
extern bool IsStringConstant( PTREE, bool * );
extern SYMBOL FunctionSymbol( PTREE );
extern PTREE PTreeIntrinsicOperator( PTREE, CGOP );
extern PTREE PTreeExtractLocn( PTREE, TOKEN_LOCN * );
extern void PTreeDeleteSizeExpr( PTREE );
extern PTREE PTreeStringLiteralConcat( PTREE, PTREE );
extern PTREE CutAwayQualification( PTREE );
extern PTREE MakeBuiltinIsFloat( PTREE );
extern PTREE PTreeOffsetof( PTREE, PTREE );
extern PTREE PTreeCheckFloatRepresentation( PTREE tree );
extern PTREE PTreeMSSizeofKludge( PTREE );

extern PTREE PTreeGetIndex( PTREE );
extern PTREE PTreeMapIndex( PTREE );

unsigned PTreeGetFPRaw( PTREE, char *, unsigned );

#define _PTREE_H
#endif
