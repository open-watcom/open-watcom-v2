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
* Description:  Convert C statements to expression trees.
*
****************************************************************************/


#include "cvars.h"
#include "i64.h"
#include "pragdefn.h"
#include "cfeinfo.h"

typedef struct block_entry {
    struct block_entry  *prev_block;
    struct block_entry  *prev_loop;
    TOKEN       block_type;
    int         top_label;
    int         break_label;
    int         continue_label;
    tryindex_t  try_index;      /* TRY: current index */
    tryindex_t  parent_index;   /* TRY: parent index */
    TREEPTR     inc_var;        /* variable used in FOR statement */
    SYM_HANDLE  sym_list;       /* symbols defined in this block */
    bool        gen_endblock;   /* set if OPR_ENDBLOCK needed */
} BLOCKDEFN, *BLOCKPTR;

// values for return_type
enum return_with {
    RETURN_WITH_NONE    = 0,
    RETURN_WITH_NO_EXPR = 1,
    RETURN_WITH_EXPR    = 2,
};

struct return_info {
    enum return_with    with;
    bool                with_expr;
};

extern int          NodeCount;
int                 LabelIndex;
SYM_LISTS           *SymListHeads;

static TREEPTR      LastStmt;
static BLOCKPTR     BlockStack;
static BLOCKPTR     LoopStack;
static SWITCHPTR    SwitchStack;

extern  int     LoopDecl( SYM_HANDLE *sym_head );

void StmtInit( void )
{
    FirstStmt = NULL;
    LastStmt = NULL;
    SymListHeads = NULL;
}


static void ChkStringLeaf( TREEPTR leaf )
{
    if( leaf->op.opr == OPR_PUSHSTRING ) {
        leaf->op.u2.string_handle->ref_count++;
    }
}


static void ChkUseful( void )
{
    if( CompFlags.useful_side_effect ) {
        CWarn1( WARN_USEFUL_SIDE_EFFECT, ERR_USEFUL_SIDE_EFFECT );
    } else {
        CWarn1( WARN_MEANINGLESS, ERR_MEANINGLESS );
    }
}


static void ChkStmtExpr( void )
{
    TREEPTR     tree;

    tree = Expr();
    if( CompFlags.meaningless_stmt == 1 ) {
        ChkUseful();
    }
    AddStmt( tree );
    if( CompFlags.pending_dead_code ) {
        DeadCode = 1;
    }
}


SYM_HANDLE GetBlockSymList( void )
{
    return( BlockStack->sym_list );
}


void InitStmt( void )
{
    BlockStack  = NULL;
    LoopStack   = NULL;
    SwitchStack = NULL;
}


void SwitchPurge( void )
{
    SWITCHPTR   sw;
    CASEPTR     c_entry, c_tmp;

    while( (sw = SwitchStack) != NULL ) {
        SwitchStack = sw->prev_switch;
        c_entry = sw->case_list;
        while( c_entry != NULL ) {
            c_tmp = c_entry->next_case;
            CMemFree( c_entry );
            c_entry = c_tmp;
        }
        CMemFree( sw );
    }
}


void AddStmt( TREEPTR stmt )
{
    WalkExprTree( stmt, ChkStringLeaf, NoOp, NoOp, DoConstFold );
    stmt = ExprNode( 0, OPR_STMT, stmt );
    stmt->op.u2.src_loc = SrcLoc;
    stmt->op.u1.unroll_count = UnrollCount;
    if( FirstStmt == NULL )  FirstStmt = stmt;
    if( LastStmt != NULL ) {
        LastStmt->left = stmt;
    }
    LastStmt = stmt;
}


static void AddSymList( SYM_HANDLE sym_handle )
{
    SYM_LISTS   *symlist;

    symlist = (SYM_LISTS *)CMemAlloc( sizeof( SYM_LISTS ) );
    symlist->next = SymListHeads;
    symlist->sym_head = sym_handle;
    SymListHeads = symlist;
}


void GenFunctionNode( SYM_HANDLE sym_handle )
{
    TREEPTR     tree;
    SYMPTR      sym;

    tree = LeafNode( OPR_FUNCTION );
    sym = SymGetPtr( sym_handle );
    tree->op.u2.func.sym_handle = sym_handle;
    tree->op.u2.func.flags = FUNC_NONE;
    if( (Toggles & TOGGLE_INLINE) || (sym->attrib & FLAG_INLINE) ){
        if( !sym->naked ){
            if( strcmp( sym->name, "main" ) != 0 ) {
                tree->op.u2.func.flags |= FUNC_OK_TO_INLINE;
            }
        }
    }
    tree->op.flags = OpFlags( sym->attrib );
    tree->expr_type = sym->sym_type->object;    // function return type
    AddStmt( tree );
    // Evil, evil globals! But we need this for later lookups in cgen.c
    sym->u.func.start_of_func = LastStmt;
    CurFuncNode = tree;
    NodeCount = 0;
}


int NextLabel( void )
{
    return( ++LabelIndex );
}


static void DropLabel( LABEL_INDEX label )
{
    TREEPTR     tree;

    CompFlags.label_dropped = 1;
    DeadCode = 0;
    tree = LeafNode( OPR_LABEL );
    tree->op.u2.label_index = label;
    AddStmt( tree );
}


static void DropBreakLabel( void )
{
    if( BlockStack->break_label != 0 ) {        /* 05-apr-92 */
        DropLabel( BlockStack->break_label );
    }
}


static void DropContinueLabel( void )
{
    if( BlockStack->continue_label != 0 ) {
        DropLabel( BlockStack->continue_label );
    }
}


static TREEPTR BracketExpr( void )
{
    TREEPTR     tree;

    MustRecog( T_LEFT_PAREN );
    tree = Expr();
    FoldExprTree( tree );
    MustRecog( T_RIGHT_PAREN );
    return( tree );
}


static void JumpCond( TREEPTR expr,
                     LABEL_INDEX label,
                     opr_code jump_opcode,
                     opr_code jump_opposite )
{
    TREEPTR     tree;

    tree = BoolExpr( expr );
    if( tree->op.opr == OPR_NOT ) {
        tree->op.opr = jump_opposite;
    } else {
        tree = ExprNode( 0, jump_opcode, tree );
    }
    tree->op.u2.label_index = label;
    AddStmt( tree );
}


static void Jump( LABEL_INDEX label )
{
    TREEPTR     tree;

    if( ! DeadCode ) {
        tree = LeafNode( OPR_JUMP );
        tree->op.u2.label_index = label;
        AddStmt( tree );
        DeadCode = 1;
    }
}


static int JumpFalse( TREEPTR expr, LABEL_INDEX label )
{
    int         jump_generated;

    jump_generated = 0;
    if( expr->op.opr == OPR_PUSHINT ) {
        if( ! expr->op.u2.long_value ) {
            Jump( label );
            jump_generated = 1;
        }
        FreeExprNode( expr );
    } else {
        JumpCond( expr, label, OPR_JUMPFALSE, OPR_JUMPTRUE );
        jump_generated = 1;
    }
    return( jump_generated );
}


static void JumpTrue( TREEPTR expr, LABEL_INDEX label )
{
    if( expr->op.opr == OPR_PUSHINT ) {
        if( expr->op.u2.long_value ) {
            Jump( label );
        }
        FreeExprNode( expr );
    } else {
        JumpCond( expr, label, OPR_JUMPTRUE, OPR_JUMPFALSE );
    }
}


void LookAhead( void )
{
    SavedId = CStrSave( Buffer );       /* save current id */
    SavedHash = HashValue;              /* and its hash value */
    SavedTokenLoc = TokenLoc;           /* save linenum and fno 09-jul-95 */
    NextToken();                        /* get next token */
    LAToken = CurToken;                 /* save it in look ahead */
    CurToken = T_SAVED_ID;              /* go back to saved id */
}


static int GrabLabels( void )
{
    LABELPTR    label;

    label = NULL;
    for( ;; ) {
        if( CurToken == T_SAVED_ID ) {          /* 09-jul-95 */
            SrcLoc = SavedTokenLoc;
        } else {
            SrcLoc = TokenLoc;
            if( CurToken != T_ID ) break;
            LookAhead();
        }
        if( LAToken != T_COLON ) break; /* quit if look ahead not : */
        label = LkLabel( SavedId );
        if( label->defined != 0 ) {
            CErr2p( ERR_LABEL_ALREADY_DEFINED, label->name );
        } else {
            DropLabel( label->ref_list );
            label->defined = 1;         /* indicate label defined now */
        }
        CMemFree( SavedId );            /* free the saved id */
        SavedId = NULL;
        NextToken();                    /* get next token */
    }
    if( label != NULL ) {                       /* label was found */
        if( CurToken == T_RIGHT_BRACE ) {
            CErr1( ERR_STMT_REQUIRED_AFTER_LABEL );
        }
        return( 1 );                    // indicate label found
    }
    return( 0 );
}


static void UnWindTry( tryindex_t try_scope )
{
#ifdef __SEH__
    TREEPTR     tree;

    tree = LeafNode( OPR_UNWIND );
    tree->op.u2.st.u.try_index = try_scope;
    AddStmt( tree );
#else
    try_scope = try_scope;
#endif
}


static void ReturnStmt( SYM_HANDLE func_result, struct return_info *info )
{
    TREEPTR             tree;
    BLOCKPTR            block;
    enum return_with    with;

    NextToken();
    if( CurToken != T_SEMI_COLON ) {
        TYPEPTR     func_type;

        func_type = CurFunc->sym_type->object;
        SKIP_TYPEDEFS( func_type );
        tree = RValue( Expr() );
        ChkRetType( tree );
        tree = FixupAss( tree, func_type );
        tree = ExprNode( 0, OPR_RETURN, tree );
        tree->expr_type = func_type;
        tree->op.u2.sym_handle = func_result;
        AddStmt( tree );
        with = RETURN_WITH_EXPR;
        info->with_expr = TRUE;
    } else {
        with = RETURN_WITH_NO_EXPR;
    }
    if( info->with == RETURN_WITH_NONE ) {
        info->with = with;
    }
    if( info->with != with ) {
        CErr1( ERR_INCONSISTENT_USE_OF_RETURN );
    }
    for( block = BlockStack; block != NULL; block = block->prev_block ) {
        if( (block->block_type == T__TRY) || (block->block_type == T___TRY) ) {
            UnWindTry( TRYSCOPE_NONE );
            break;
        }
    }
}


static void SetFuncReturnNode( TREEPTR tree )
{
    TYPEPTR     typ;

    typ = CurFunc->sym_type->object;
    tree->expr_type = typ;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_STRUCT || typ->decl_type == TYPE_UNION ) {
        tree->right = LeafNode( OPR_NOP );      // place holder
        tree->right->expr_type = NULL;
    }
}


static void ChkRetValue( void )
{
    TYPEPTR     typ;

    typ = CurFunc->sym_type;
    typ = typ->object;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type != TYPE_VOID ) {
        CWarn2p( WARN_MISSING_RETURN_VALUE, ERR_MISSING_RETURN_VALUE, CurFunc->name );
    }
}

static SYM_HANDLE GetLocalVarDecls( void )
{
    SYM_HANDLE  symlist;
    SYM_HANDLE  symhandle;
    SYM_ENTRY   sym;

    DeclList( &symlist );
    if( symlist != 0 ) {
        symhandle = CurFunc->u.func.locals;
        // symhandle will be non-zero if MakeNewSym was called while
        // parsing the declaration list.
        if( symhandle != 0 ) {          // if some temporaries were created
            for( ;; ) {                 // - find end of list
                SymGet( &sym, symhandle );
                if( sym.handle == 0 ) break;
                symhandle = sym.handle;
            }
            sym.handle = symlist;
            SymReplace( &sym, symhandle );
        } else {
            CurFunc->u.func.locals = symlist;
        }
    }
    return( symlist );
}


static void DeadMsg( void )
/*************************/
{
    CWarn1( WARN_DEAD_CODE, ERR_DEAD_CODE );
    DeadCode = 2;   /* so we don't get more messages */
}


static void StartNewBlock( void )
{
    BLOCKPTR    block;

    block = (BLOCKPTR)CMemAlloc( sizeof( BLOCKDEFN ) );
    block->block_type = CurToken;
    block->top_label = 0;
    block->continue_label = 0;
    block->break_label = 0;
    block->prev_block = BlockStack;
    block->prev_loop = LoopStack;
    block->gen_endblock = FALSE;
    BlockStack = block;
}


static void LeftBrace( void )
{
    TREEPTR     tree;

/*
    <storage> <type> function( <parms> )
    {   <- this is SymLevel == 1
    (weird code is for SymLevel > 1 )
*/
    // DeclList might generate some quads to do some initializers
    // if that happens, we want them output after the OPR_NEWBLOCK node
    StartNewBlock();
    NextToken();
    ++SymLevel;
    tree = LeafNode( OPR_NEWBLOCK );
    AddStmt( tree );
    BlockStack->sym_list = 0;
    BlockStack->gen_endblock = TRUE;
    DeclList( &BlockStack->sym_list );
    tree->op.u2.sym_handle = BlockStack->sym_list;
}


static void JumpBreak( BLOCKPTR block )
{
    if( !DeadCode ) {                           /* 05-apr-92 */
        if( block->break_label == 0 ) {
            block->break_label = NextLabel();
        }
        Jump( block->break_label );
    }
}


static void BreakStmt( void )
{
    BLOCKPTR    block;
    tryindex_t  try_scope;

    NextToken();
    try_scope = TRYSCOPE_UNDEF;
    block = BlockStack;
    if( block != NULL ) {
        while( block != LoopStack ) {
            if( block->block_type == T_SWITCH )
                break;
            if( (block->block_type == T__TRY) || (block->block_type == T___TRY) ) {
                try_scope = block->parent_index;
            }
            block = block->prev_block;
        }
    }
    if( block != NULL ) {
        if( try_scope != TRYSCOPE_UNDEF ) {
            UnWindTry( try_scope );
        }
        JumpBreak( block );
    } else {
        CErr1( ERR_MISPLACED_BREAK );
    }
    MustRecog( T_SEMI_COLON );
}


#ifdef __SEH__
static void LeaveStmt( void )
{
    BLOCKPTR    block;

    NextToken();
    block = BlockStack;
    while( block != NULL ) {
        if( (block->block_type == T__TRY) || (block->block_type == T___TRY) )
            break;
        block = block->prev_block;
    }
    if( block != NULL ) {
        JumpBreak( block );
    } else {
        CErr1( ERR_MISPLACED_LEAVE );
    }
    MustRecog( T_SEMI_COLON );
}
#endif


static void ContinueStmt( void )
{
    BLOCKPTR    block;
    tryindex_t  try_scope;

    NextToken();
    if( LoopStack != NULL ) {
        if( ! DeadCode ) {                              /* 05-apr-92 */
            try_scope = TRYSCOPE_UNDEF;
            block = BlockStack;
            while( block != LoopStack ) {
                if( ( block->block_type == T__TRY ) || ( block->block_type == T___TRY ) ) {
                    try_scope = block->parent_index;
                }
                block = block->prev_block;
            }
            if( try_scope != TRYSCOPE_UNDEF ) {
                UnWindTry( try_scope );
            }
            if( LoopStack->continue_label == 0 ) {
                LoopStack->continue_label = NextLabel();
            }
            Jump( LoopStack->continue_label );
        }
    } else {
        CErr1( ERR_MISPLACED_CONTINUE );
    }
    MustRecog( T_SEMI_COLON );
}


static void DefaultStmt( void )
{
    NextToken();
    MustRecog( T_COLON );
    if( SwitchStack ) {
        if( SwitchStack->default_label == 0 ) {
            SwitchStack->default_label = NextLabel();
            DropLabel( SwitchStack->default_label );
        } else {
            CErr1( ERR_ONLY_1_DEFAULT );
        }
        if( CurToken == T_RIGHT_BRACE ) {
            CErr1( ERR_STMT_REQUIRED_AFTER_DEFAULT );
        }
    } else {
        CErr1( ERR_MISPLACED_DEFAULT );
    }
}


static void ElseStmt( void )
{
    LABEL_INDEX if_label;

    SrcLoc = TokenLoc;
    NextToken();
    BlockStack->block_type = T_ELSE;
    if_label = BlockStack->break_label;
    if( DeadCode ) {                            /* 05-apr-92 */
        BlockStack->break_label = 0;
    } else {
        BlockStack->break_label = NextLabel();
        Jump( BlockStack->break_label );
    }
    DropLabel( if_label );
}


static void GotoStmt( void )
{
    LABELPTR    label;

    NextToken();
    if( CurToken != T_ID ) {
        CErr1( ERR_EXPECTING_LABEL );
    } else {
        label = LkLabel( Buffer );
        label->referenced = 1;                          /* 05-apr-91 */
        Jump( label->ref_list );
    }
    NextToken();
    MustRecog( T_SEMI_COLON );
}


static void PushBlock( void )
{
    StartNewBlock();
    ++SymLevel;
    BlockStack->sym_list = 0;
}


static void NewLoop( void )
{
    StartNewBlock();
    LoopStack = BlockStack;
    LoopStack->top_label = NextLabel();
    DropLabel( LoopStack->top_label );
    ++LoopDepth;
}


static void ForStmt( void )
{
    bool    parsed_semi_colon = FALSE;

    NextToken();
    MustRecog( T_LEFT_PAREN );
    if( CompFlags.c99_extensions ) {
        PushBlock();    // 'for' opens new scope
    }
    if( CurToken != T_SEMI_COLON ) {
        if( CompFlags.c99_extensions ) {
            TREEPTR     tree;

            tree = LeafNode( OPR_NEWBLOCK );
            AddStmt( tree );
            BlockStack->gen_endblock = TRUE;
            if( !LoopDecl( &BlockStack->sym_list ) ) {
                ChkStmtExpr();      // no declarator, try init_expr
            } else {
                parsed_semi_colon = TRUE;   // LoopDecl ate it up
            }
            tree->op.u2.sym_handle = BlockStack->sym_list;
        } else {
            ChkStmtExpr();          // init_expr
        }
    }
    if( !parsed_semi_colon ) {
        MustRecog( T_SEMI_COLON );
    }
    NewLoop();
    BlockStack->block_type = T_FOR;
    if( CurToken != T_SEMI_COLON ) {
        BlockStack->break_label = NextLabel();
        if( !JumpFalse( Expr(), BlockStack->break_label ) ) {
            BlockStack->break_label = 0;        /* 09-sep-92 */
        }
    }
    MustRecog( T_SEMI_COLON );
    BlockStack->inc_var = NULL;
    if( CurToken != T_RIGHT_PAREN ) {
        BlockStack->inc_var = Expr();                   // save this
        if( CompFlags.meaningless_stmt == 1 ) {
            ChkUseful();
        }
    }
    MustRecog( T_RIGHT_PAREN );
}


static void EndForStmt( void )
{
    DropContinueLabel();
    if( BlockStack->inc_var ) {
        AddStmt( BlockStack->inc_var );
    }
    Jump( BlockStack->top_label );
}


static void StmtExpr( void )
{
    ChkStmtExpr();
    if( ExpectingToken( T_SEMI_COLON ) ) {
        NextToken();
    }
}


static void AddCaseLabel( unsigned long value )
{
    CASEPTR         ce, prev_ce, new_ce;
    unsigned long   old_value, converted_value;
    TREEPTR         tree;
    char            buffer[12];

    prev_ce = NULL;
#if 0
    leaf.u.ulong_konst = value;
    leaf.data_type = SwitchStack->case_type;
    SetLeafType( &leaf, 1 );
//      converted_value = value & SwitchStack->case_mask;
    converted_value = leaf.u.ulong_konst;
#else
    converted_value = value;
#endif
    old_value = converted_value + 1;  /* make old_value different */
    for( ce = SwitchStack->case_list; ce; ce = ce->next_case ) {
        old_value = ce->value;
        if( old_value >= converted_value ) break;
        prev_ce = ce;
    }
    if( converted_value == old_value ) {   /* duplicate case value found */
        sprintf( buffer, SwitchStack->case_format, value );
        CErr2p( ERR_DUPLICATE_CASE_VALUE, buffer );
    } else {
        new_ce = (CASEPTR)CMemAlloc( sizeof( CASEDEFN ) );
        new_ce->value = converted_value;
        if( prev_ce == NULL ) {
            new_ce->next_case = SwitchStack->case_list;
            SwitchStack->case_list = new_ce;
        } else {
            prev_ce->next_case = new_ce;
            new_ce->next_case = ce;
        }
        /* Check if the previous statement was a 'case'. If so, reuse the label, as generating
         * too many labels seriously slows down code generation.
         */
        if( prev_ce && LastStmt->op.opr == OPR_STMT && LastStmt->right->op.opr == OPR_CASE ) {
            new_ce->label = SwitchStack->last_case_label;
            new_ce->gen_label = FALSE;
        } else {
            new_ce->label = NextLabel();
            new_ce->gen_label = TRUE;
        }
        SwitchStack->number_of_cases++;
        if( converted_value < SwitchStack->low_value ) {
            SwitchStack->low_value = converted_value;
        }
        if( converted_value > SwitchStack->high_value ) {
            SwitchStack->high_value = converted_value;
        }
        SwitchStack->last_case_label = new_ce->label;
        tree = LeafNode( OPR_CASE );
        tree->op.u2.case_info = new_ce;
        AddStmt( tree );
    }
}


static void CaseStmt( void )
{
    const_val   val;

    NextToken();
    if( SwitchStack ) {
        if( ConstExprAndType( &val ) ) {
            if( ( val.type == TYPE_ULONG64 ) && !U64IsU32( val.value ) ) {
                CErr1( ERR_CONSTANT_TOO_BIG );
            } else if( ( val.type == TYPE_LONG64 ) && !I64IsI32( val.value ) ) {
                CErr1( ERR_CONSTANT_TOO_BIG );
            }
            AddCaseLabel( U32FetchTrunc( val.value) );
        }
        MustRecog( T_COLON );
        if( CurToken == T_RIGHT_BRACE ) {
            CErr1( ERR_STMT_REQUIRED_AFTER_CASE );
        }
    } else {
        CErr1( ERR_MISPLACED_CASE );
        ConstExprAndType( &val );        /* grab constant expression */
        MustRecog( T_COLON );
    }
}


#ifdef __SEH__
static void MarkTryVolatile( SYM_HANDLE sym_handle )
{
    SYM_ENTRY   sym;

    for( ;; ) {
        if( sym_handle == 0 ) break;
        SymGet( &sym, sym_handle );
        sym.flags |= SYM_TRY_VOLATILE; //force into memory
        SymReplace( &sym, sym_handle );
        sym_handle = sym.handle;
    }
}


static void TryStmt( void )
{
    TREEPTR     tree;

    MarkTryVolatile( CurFunc->u.func.parms );
    MarkTryVolatile( CurFunc->u.func.locals );
    CurFuncNode->op.u2.func.flags |=  FUNC_USES_SEH;
    CurFuncNode->op.u2.func.flags &= ~FUNC_OK_TO_INLINE;
    CurToken = T__TRY;
    StartNewBlock();
    NextToken();
    BlockStack->parent_index = TryScope;
    ++TryCount;
    BlockStack->try_index = TryCount;
    TryScope = TryCount;
    tree = LeafNode( OPR_TRY );
    tree->op.u2.st.u.try_index = 0;
    tree->op.u2.st.parent_scope = TryCount;
    AddStmt( tree );
}


static SYM_HANDLE DummyTrySymbol( void )
{
    SYM_ENTRY   sym;
    SYM_HANDLE  sym_handle;

    sym_handle = MakeNewSym( &sym, 'T', GetType( TYPE_VOID ), SC_STATIC );
    SymReplace( &sym, sym_handle );
    return( sym_handle );
}


static int EndTry( void )
{
    tryindex_t  parent_scope;
    TREEPTR     expr;
    TREEPTR     func;
    TREEPTR     tree;
    TYPEPTR     typ;
    int         expr_type;

    DropBreakLabel();           /* _leave jumps to this label */
    parent_scope = BlockStack->parent_index;
    tree = LeafNode( OPR_TRY );
    tree->op.u2.st.u.try_index = BlockStack->try_index;
    tree->op.u2.st.parent_scope = parent_scope;
    AddStmt( tree );
    if( (CurToken == T__EXCEPT) || (CurToken == T___EXCEPT) ) {
        NextToken();
        BlockStack->block_type = T__EXCEPT;
        BlockStack->break_label = NextLabel();
        Jump( BlockStack->break_label );
        DeadCode = 0;
        tree = LeafNode( OPR_EXCEPT );
        tree->op.u2.st.u.try_sym_handle = DummyTrySymbol();
        tree->op.u2.st.parent_scope = parent_scope;
        AddStmt( tree );
        CompFlags.exception_filter_expr = 1;
        expr = RValue( BracketExpr() );
        CompFlags.exception_filter_expr = 0;
        CompFlags.exception_handler = 1;
        typ = TypeOf( expr );
        expr_type = DataTypeOf( typ );
        if( expr_type != TYPE_VOID ) {
            if( expr_type > TYPE_ULONG ) {
                CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            }
        }
        func = VarLeaf( SymGetPtr( SymExcept ), SymExcept );
        func->op.opr = OPR_FUNCNAME;
        expr = ExprNode( NULL, OPR_PARM, expr );
        expr->expr_type = typ;
        expr->op.u2.result_type = typ;
        tree = ExprNode( func, OPR_CALL, expr );
        tree->expr_type = GetType( TYPE_VOID );
        AddStmt( tree );
        return( 1 );
    } else if( (CurToken == T__FINALLY) || (CurToken == T___FINALLY) ) {
        CompFlags.in_finally_block = 1;
        NextToken();
        BlockStack->block_type = T__FINALLY;
        DeadCode = 0;
        tree = LeafNode( OPR_FINALLY );
        tree->op.u2.st.u.try_sym_handle = DummyTrySymbol();
        tree->op.u2.st.parent_scope = parent_scope;
        AddStmt( tree );
        return( 1 );
    }
    return( 0 );
}
#endif


static void PopBlock( void )
{
    BLOCKPTR    block;
    TREEPTR     tree;

    if( BlockStack->gen_endblock ) {
        tree = LeafNode( OPR_ENDBLOCK );
        tree->op.u2.sym_handle = BlockStack->sym_list;
        AddStmt( tree );
    }
    if( BlockStack->sym_list != 0 ) {
        AddSymList( BlockStack->sym_list );
    }
    block = BlockStack;
    LoopStack = block->prev_loop;
    BlockStack = block->prev_block;
    CMemFree( block );
}


static void SwitchStmt( void )
{
    SWITCHPTR   sw;
    TREEPTR     tree;
    TYPEPTR     typ;
    int         switch_type;

    StartNewBlock();
    NextToken();
    sw = (SWITCHPTR)CMemAlloc( sizeof( SWITCHDEFN ) );
    sw->prev_switch = SwitchStack;
    sw->low_value = ~0l;
    sw->high_value = 0;
    sw->case_format = "%ld";        /* assume signed cases */
    SwitchStack = sw;
    switch_type = TYPE_INT;         /* assume int */
    tree = RValue( BracketExpr() );
    typ = TypeOf( tree );
    if( typ->decl_type == TYPE_ENUM ) typ = typ->object;
    if( typ->decl_type == TYPE_UFIELD ) {
        if( typ->u.f.field_width == (TARGET_INT * 8) ) {
            sw->case_format = "%lu";
            switch_type = TYPE_UINT;
        }
    }
    switch( typ->decl_type ) {
    case TYPE_USHORT:
    case TYPE_UINT:
        sw->case_format = "%lu";
        switch_type = TYPE_UINT;
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_INT:
    case TYPE_FIELD:
    case TYPE_UFIELD:
        break;
    case TYPE_ULONG:
        sw->case_format = "%lu";
        switch_type = TYPE_ULONG;
        break;
    case TYPE_LONG:
        switch_type = TYPE_LONG;
        break;
    default:
        CErr1( ERR_INVALID_TYPE_FOR_SWITCH );
    }
    tree = ExprNode( 0, OPR_SWITCH, tree );
    tree->op.u2.switch_info = sw;
    AddStmt( tree );
}


static void EndSwitch( void )
{
    SWITCHPTR   sw;
//    CASEPTR   ce;

    sw = SwitchStack;
    SwitchStack = sw->prev_switch;
    if( sw->default_label == 0 ) {
        if( BlockStack->break_label == 0 ) {
            sw->default_label = NextLabel();
            DropLabel( sw->default_label );
        } else {
            sw->default_label = BlockStack->break_label;
        }
    }
//      if( sw->case_list == NULL ) {
//          CWarn1( WARN_EMPTY_SWITCH, ERR_EMPTY_SWITCH );
//      }
#if 0
    for( ; ce = sw->case_list; ) {
        sw->case_list = ce->next_case;
        CMemFree( ce );
    }
    CMemFree( sw );
#endif
}


static void EndOfStmt( void )
{
    do {
        switch( BlockStack->block_type ) {
        case T_LEFT_BRACE:
            EndBlock();
            break;
        case T_IF:
            if( CurToken == T_ELSE ) {
                ElseStmt();
                return;
            }
            DropBreakLabel();
            break;
        case T_ELSE:
            DropBreakLabel();
            break;
        case T_WHILE:
            DropContinueLabel();
            Jump( BlockStack->top_label );
            --LoopDepth;
            DropBreakLabel();
            break;
        case T_FOR:
            EndForStmt();
            --LoopDepth;
            DropBreakLabel();
            if( CompFlags.c99_extensions ) {
                EndBlock();     /* Terminate the scope introduced by 'for' */
                PopBlock();
            }
            break;
        case T_DO:
            DropContinueLabel();
            MustRecog( T_WHILE );
            SrcLoc = TokenLoc;
            JumpTrue( BracketExpr(), BlockStack->top_label );
            MustRecog( T_SEMI_COLON );
            SrcLoc = TokenLoc;
            --LoopDepth;
            DropBreakLabel();
            break;
        case T_SWITCH:
            EndSwitch();
            DropBreakLabel();
            break;
#ifdef __SEH__
        case T__TRY:
        case T___TRY:
            if( EndTry() )
                return;
            break;
        case T__EXCEPT:
        case T___EXCEPT:
            DropBreakLabel();
            TryScope = BlockStack->parent_index;
            CompFlags.exception_handler = 0;
            break;
        case T__FINALLY:
        case T___FINALLY:
            AddStmt( LeafNode( OPR_END_FINALLY ) );
            CompFlags.in_finally_block = 0;
            TryScope = BlockStack->parent_index;
            break;
#endif
        }
        PopBlock();
        if( BlockStack == NULL ) break;
    } while( BlockStack->block_type != T_LEFT_BRACE );
}


static bool IsDeclarator( TOKEN token )
{
    SYM_HANDLE      sym_handle;
    SYM_ENTRY       sym;

    /* If token class is storage class or qualifier, it's a declaration */
    if( TokenClass[ token ] == TC_STG_CLASS
    ||  TokenClass[ token ] == TC_QUALIFIER
    ||  TokenClass[ token ] == TC_DECLSPEC ) {
        return( TRUE );
    }

    /* If token is one of the following, it's a declaration */
    switch( token ) {
    case T_VOID:
    case T_CHAR:
    case T_SHORT:
    case T_INT:
    case T_LONG:
    case T_FLOAT:
    case T_DOUBLE:
    case T_SIGNED:
    case T_UNSIGNED:
    case T__COMPLEX:
    case T__IMAGINARY:
    case T__BOOL:
    case T___INT64:
    case T_STRUCT:
    case T_UNION:
    case T_ENUM:
        return( TRUE );
    default:
        break;
    }

    /* If token is an ID, it might be a typedef */
    if( (CurToken == T_ID) || (CurToken == T_SAVED_ID) ) {
        if( CurToken == T_ID ) {
            sym_handle = SymLookTypedef( HashValue, Buffer, &sym );
        } else {    /* T_SAVED_ID */
            sym_handle = SymLookTypedef( SavedHash, SavedId, &sym );
        }
        if( sym_handle != 0 && sym.stg_class == SC_TYPEDEF ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


static void FixupC99MainReturn( SYM_HANDLE func_result, struct return_info *info )
{
    TREEPTR             tree;
    TYPEPTR             main_type;

    /* In C99 mode, return statement need not be explicit for main()... */
    main_type = CurFunc->sym_type->object;
    SKIP_TYPEDEFS( main_type );
    /* ... as long as return type is compatible with int */
    if( main_type->decl_type == TYPE_INT ) {
        tree = IntLeaf( 0 );    /* zero is the default return value */
        tree = ExprNode( 0, OPR_RETURN, tree );
        tree->expr_type = main_type;
        tree->op.u2.sym_handle = func_result;
        AddStmt( tree );
        info->with_expr = TRUE;
    }
}


void Statement( void )
{
    LABEL_INDEX         end_of_func_label;
    SYM_HANDLE          func_result;
    TREEPTR             tree;
    bool                return_at_outer_level;
    bool                skip_to_next_token;
    bool                declaration_allowed;
    struct return_info  return_info;
    SYM_ENTRY           sym;

#ifndef NDEBUG
    if( DebugFlag >= 1 ) {
        printf( "***** line %u, func=%s\n", TokenLoc.line, CurFunc->name );
        PrintStats();
    }
#endif
    ++FuncCount;
    return_info.with = RETURN_WITH_NONE; /* indicate no return statements */
    return_info.with_expr = FALSE;
    CompFlags.label_dropped = 0;
    CompFlags.addr_of_auto_taken = 0;           /* 23-oct-91 */
    end_of_func_label = 0;
    return_at_outer_level = FALSE;              /* 28-feb-92 */
    declaration_allowed   = FALSE;
    DeadCode = 0;
    LoopDepth = 0;
    LabelIndex = 0;
    BlockStack = NULL;
    LoopStack = NULL;
    SwitchStack = NULL;
#ifdef __SEH__
    TryCount = TRYSCOPE_NONE;
    TryScope = TRYSCOPE_NONE;
#endif
    StartNewBlock();
    NextToken();                        // skip over {
    ++SymLevel;
    tree = LeafNode( OPR_LABELCOUNT );
    AddStmt( tree );
    if( GrabLabels() == 0 ) {                           /* 29-nov-94 */
        GetLocalVarDecls();
    }
    func_result = MakeNewSym( &sym, 'R', CurFunc->sym_type->object, SC_AUTO );
    sym.flags |= SYM_FUNC_RETURN_VAR;   /* 25-oct-91 */
    SymReplace( &sym, func_result );
    for( ;; ) {
        CompFlags.pending_dead_code = 0;
        if( GrabLabels() == 0 && declaration_allowed && IsDeclarator( CurToken ) ) {
            GetLocalVarDecls();
        }
        if( CompFlags.c99_extensions ) {
            declaration_allowed = TRUE;
        }
        skip_to_next_token = FALSE;
        switch( CurToken ) {
        case T_IF:
            StartNewBlock();
            NextToken();
            BlockStack->break_label = NextLabel();
            JumpFalse( BracketExpr(), BlockStack->break_label );
            /* 23-dec-88, only issue msg if ';' is on same line as 'if' */
            if( CurToken == T_SEMI_COLON  &&  SrcLoc.line == TokenLoc.line && SrcLoc.fno == TokenLoc.fno ) {
                SetErrLoc( &TokenLoc );
                NextToken();    /* look ahead for else keyword */
                if( CurToken != T_ELSE ) {              /* 02-apr-91 */
                    ChkUseful();                        /* 08-dec-88 */
                }
                InitErrLoc();
                break;
            }
            declaration_allowed = FALSE;
            continue;
        case T_WHILE:
            NewLoop();
            NextToken();
            BlockStack->break_label = NextLabel();
            if( !JumpFalse( BracketExpr(), BlockStack->break_label ) ) {
                BlockStack->break_label = 0;            /* 09-sep-92 */
            }
            if( CurToken == T_SEMI_COLON ) {            /* 08-dec-88 */
                if( ! CompFlags.useful_side_effect ) {
                    CWarn1( WARN_MEANINGLESS, ERR_MEANINGLESS );
                }
            }
            declaration_allowed = FALSE;
            continue;
        case T_DO:
            NewLoop();
            NextToken();
            if( CurToken == T_RIGHT_BRACE ) {
                CErr1( ERR_STMT_REQUIRED_AFTER_DO );
                break;
            }
            declaration_allowed = FALSE;
            continue;
        case T_FOR:
            ForStmt();
            declaration_allowed = FALSE;
            continue;
        case T_SWITCH:
            SwitchStmt();
            DeadCode = 1;
            declaration_allowed = FALSE;
            continue;
        case T_CASE:
            DeadCode = 0;
            CaseStmt();
            declaration_allowed = FALSE;
            continue;
        case T_DEFAULT:
            DefaultStmt();
            declaration_allowed = FALSE;
            continue;
        case T_BREAK:
            BreakStmt();
            DeadCode = 1;
            if( BlockStack->block_type != T_LEFT_BRACE ) break;
            continue;
        case T_CONTINUE:
            ContinueStmt();
            DeadCode = 1;
            if( BlockStack->block_type != T_LEFT_BRACE ) break;
            continue;
#ifdef __SEH__
        case T__LEAVE:
        case T___LEAVE:
            LeaveStmt();
            DeadCode = 1;
            if( BlockStack->block_type != T_LEFT_BRACE ) break;
            continue;
#endif
        case T_RETURN:
            ReturnStmt( func_result, &return_info );
            if( BlockStack->prev_block == NULL ) {      /* 28-feb-92 */
                return_at_outer_level = TRUE;
            }
            MustRecog( T_SEMI_COLON );
            if( SymLevel != 1  ||  CurToken != T_RIGHT_BRACE  ||
                    BlockStack->block_type != T_LEFT_BRACE ) {
                if( end_of_func_label == 0 ) {
                    end_of_func_label = NextLabel();
                }
                Jump( end_of_func_label );
            }
            if( BlockStack->block_type != T_LEFT_BRACE ) break;
            continue;
        case T_GOTO:
            GotoStmt();
            if( BlockStack->block_type != T_LEFT_BRACE ) break;
            continue;
        case T_SEMI_COLON:
            NextToken();
            if( BlockStack->block_type != T_LEFT_BRACE ) {
                if( CurToken == T_ELSE ) {
                    declaration_allowed = FALSE;
                }
                break;
            }
            continue;
        case T_LEFT_BRACE:
            LeftBrace();
            continue;
        case T_RIGHT_BRACE:
            if( BlockStack->block_type != T_LEFT_BRACE ) {
                CErr1( ERR_MISPLACED_RIGHT_BRACE );
            }
            if( BlockStack->prev_block == NULL ) {
                skip_to_next_token = TRUE;
            } else {
                NextToken();
            }
            break;
        case T_EXTERN:
        case T_STATIC:
        case T_AUTO:
        case T_REGISTER:
        case T_VOID:
        case T_CHAR:
        case T_SHORT:
        case T_INT:
        case T_LONG:
        case T_FLOAT:
        case T_DOUBLE:
        case T_SIGNED:
        case T_UNSIGNED:
            if( CompFlags.c99_extensions )
                CErr1( ERR_UNEXPECTED_DECLARATION );
            else
                CErr1( ERR_MISSING_RIGHT_BRACE );
            break;
        case T_EOF:
            CErr1( ERR_MISSING_RIGHT_BRACE );
            break;
#ifdef __SEH__
        case T__TRY:
        case T___TRY:
            TryStmt();
            continue;
#endif
        case T___ASM:
            AsmStmt();
            continue;
        default:
            if( DeadCode == 1 ) {
                DeadMsg();
            }
            StmtExpr();
            if( BlockStack->block_type != T_LEFT_BRACE ) break;
            continue;
        }
        EndOfStmt();
        if( BlockStack == NULL ) break;
        if( skip_to_next_token ) {
            NextToken();
        }
    }
    /* C99 has special semantics for return value of main() */
    if( CompFlags.c99_extensions && !strcmp( CurFunc->name, "main" ) ) {
        if( !return_at_outer_level ) {
            FixupC99MainReturn( func_result, &return_info );
            return_at_outer_level = TRUE;
        }
    }
    if( !return_info.with_expr ) {   /* no return values present */
        if( !DeadCode && !CurFunc->naked ) {
            ChkRetValue();
        }
    } else if( ! return_at_outer_level ) {              /* 28-feb-92 */
        if( ! DeadCode && !CurFunc->naked ) {
            CWarn2p( WARN_MISSING_RETURN_VALUE, ERR_MISSING_RETURN_VALUE, CurFunc->name );
        }
    }
    if( end_of_func_label != 0 ) {
        DropLabel( end_of_func_label );
    }
    DeadCode = 0;
    tree->op.u2.label_count = LabelIndex;
    tree = LeafNode( OPR_FUNCEND );
    if( !return_info.with_expr ) {
        tree->expr_type = NULL;
        tree->op.u2.sym_handle = 0;
    } else {
        tree->op.u2.sym_handle = func_result;
        SetFuncReturnNode( tree );
    }
    AddStmt( tree );
    AddSymList( CurFunc->u.func.locals );
    SrcLoc = TokenLoc;
    FreeLabels();
    if( skip_to_next_token ) {
        NextToken();
    }
    if( CompFlags.generate_prototypes ) {
        if( DefFile == NULL ) {
            OpenDefFile();
        }
        if( DefFile != NULL && CurFunc->stg_class == SC_NULL ) {
            /* function exported */
            DumpFuncDefn();
        }
    }

    if( Toggles & TOGGLE_INLINE ) {
        if( Inline_Threshold < NodeCount ) {
            CurFuncNode->op.u2.func.flags &= ~FUNC_OK_TO_INLINE;
        }
    }
    if( VarParm( CurFunc ) ) {
        CurFuncNode->op.u2.func.flags &= ~FUNC_OK_TO_INLINE;
    }
    NodeCount = 0;
    if( CompFlags.addr_of_auto_taken ) {                /* 23-oct-91 */
        CurFunc->flags &= ~ SYM_OK_TO_RECURSE;
    }
}
