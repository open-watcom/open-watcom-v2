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


#include "cvars.h"

// values for return_type
enum return_with {
    RETURN_WITH_NONE      =0,
    RETURN_WITH_NO_EXPR   =1,
    RETURN_WITH_EXPR =2,
};
struct  return_info {
    enum  return_with  with;
    bool               with_expr;
};

extern int             NodeCount;
TREEPTR         FirstStmt;
TREEPTR         LastStmt;
TREEPTR         CurFuncNode;
int             LabelIndex;
SYM_LISTS       *SymListHeads;

void StmtInit( void ){
    FirstStmt = NULL;
    LastStmt = NULL;
    SymListHeads = NULL;
}

void ChkStringLeaf( TREEPTR leaf )
{
    if( leaf->op.opr == OPR_PUSHSTRING ) {
        leaf->op.string_handle->ref_count++;
    }
}


void AddStmt( TREEPTR stmt )
{
    WalkExprTree( stmt, ChkStringLeaf, NoOp, NoOp, DoConstFold );
    stmt = ExprNode( 0, OPR_STMT, stmt );
    stmt->op.source_fno = SrcFno;
    stmt->srclinenum = SrcLineNum;
    stmt->op.unroll_count = UnrollCount;
    if( FirstStmt == NULL )  FirstStmt = stmt;
    if( LastStmt != NULL ) {
        LastStmt->left = stmt;
    }
    LastStmt = stmt;
}

void AddSymList( SYM_HANDLE sym_handle )
{
    SYM_LISTS   *symlist;

    symlist = (SYM_LISTS *)CMemAlloc( sizeof(SYM_LISTS) );
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
    tree->op.func.sym_handle = sym_handle;
    tree->op.func.flags = FUNC_NONE;
    if( Toggles & TOGGLE_INLINE | sym->attrib & FLAG_INLINE ){
        if( !sym->naked ){
            if( strcmp( sym->name, "main" ) != 0 ) {
                tree->op.func.flags |= FUNC_OK_TO_INLINE;
            }
        }
    }
    tree->op.flags = OpFlags( sym->attrib );
    tree->expr_type = sym->sym_type->object;    // function return type
    AddStmt( tree );
    CurFuncNode = tree;
    NodeCount = 0;
}

int NextLabel()
{
    return( ++LabelIndex );
}

static void DropLabel( LABEL_INDEX label )
{
    TREEPTR     tree;

    CompFlags.label_dropped = 1;
    DeadCode = 0;
    tree = LeafNode( OPR_LABEL );
    tree->op.label_index = label;
    AddStmt( tree );
}

static TREEPTR BracketExpr()
{
    TREEPTR     tree;

    MustRecog( T_LEFT_PAREN );
    tree = Expr();
    FoldExprTree( tree );
    MustRecog( T_RIGHT_PAREN );
    return( tree );
}


static Jump( LABEL_INDEX label )
{
    TREEPTR     tree;

    if( ! DeadCode ) {
        tree = LeafNode( OPR_JUMP );
        tree->op.label_index = label;
        AddStmt( tree );
        DeadCode = 1;
    }
}


static int JumpFalse( TREEPTR expr, LABEL_INDEX label )
{
    int         jump_generated;

    jump_generated = 0;
    if( expr->op.opr == OPR_PUSHINT ) {
        if( ! expr->op.long_value ) {
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
        if( expr->op.long_value ) {
            Jump( label );
        }
        FreeExprNode( expr );
    } else {
        JumpCond( expr, label, OPR_JUMPTRUE, OPR_JUMPFALSE );
    }
}


static void JumpCond( TREEPTR expr,
                     LABEL_INDEX label,
                     int jump_opcode,
                     int jump_opposite )
{
    TREEPTR     tree;

    tree = BoolExpr( expr );
    if( tree->op.opr == OPR_NOT ) {
        tree->op.opr = jump_opposite;
    } else {
        tree = ExprNode( 0, jump_opcode, tree );
    }
    tree->op.label_index = label;
    AddStmt( tree );
}


void LookAhead(void)
{
    SavedId = CStrSave( Buffer );        /* save current id */
    SavedHash = HashValue;              /* and its hash value */
    SavedTokenLine = TokenLine;         /* save linenum and fno 09-jul-95 */
    SavedTokenFno  = TokenFno;
    NextToken();                        /* get next token */
    LAToken = CurToken;                 /* save it in look ahead */
    CurToken = T_SAVED_ID;              /* go back to saved id */
}


static int GrabLabels()
{
    LABELPTR    label;

    label = NULL;
    for(;;) {
        if( CurToken == T_SAVED_ID ) {          /* 09-jul-95 */
            SrcLineNum = SavedTokenLine;
            SrcFno     = SavedTokenFno;
        } else {
            SrcLineNum = TokenLine;
            SrcFno     = TokenFno;
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


static void ReturnStmt( SYM_HANDLE func_result, struct return_info *info )
{
    TREEPTR     tree;
    BLOCKPTR    block;
    enum  return_with with;

    NextToken();
    if( CurToken != T_SEMI_COLON ) {
        tree = RValue( Expr() );
        ChkRetType( tree );
        tree = ExprNode( 0, OPR_RETURN, tree );
        tree->expr_type = CurFunc->sym_type->object;
        tree->op.sym_handle = func_result;
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
    block = BlockStack;                                 /* 16-apr-94 */
    while( block != NULL ) {
        if( block->block_type == T__TRY ) break;
        block = block->prev_block;
    }
    if( block != NULL ) {
        UnWindTry( -1 );
    }
}

static void SetFuncReturnNode( TREEPTR tree )
{
    TYPEPTR     typ;

    typ = CurFunc->sym_type->object;
    tree->expr_type = typ;
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    if( typ->decl_type == TYPE_STRUCT || typ->decl_type == TYPE_UNION ) {
        tree->right = LeafNode( OPR_NOP );      // place holder
        tree->right->expr_type = NULL;
    }
}

static void ChkRetValue()
{
    TYPEPTR     typ;

    typ = CurFunc->sym_type;
    if( CurFunc->flags & SYM_TYPE_GIVEN ) {
        typ = typ->object;
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        if( typ->decl_type != TYPE_VOID ) {
            CWarn( WARN_MISSING_RETURN_VALUE,
                    ERR_MISSING_RETURN_VALUE, CurFunc->name );
        }
    } else {  //default is type_int make void to catch bad use
        CurFunc->sym_type = FuncNode( GetType( TYPE_VOID ),
                                    FLAG_NONE, typ->u.parms );
    }
}

void GetLocalVarDecls()
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
            for(;;) {                   // - find end of list
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
}

static void DeadMsg( void ){
/***************************/
    CWarn1( WARN_DEAD_CODE, ERR_DEAD_CODE );
    DeadCode = 2;   /* so we don't get more messages */
}

void Statement(void)
{
    LABEL_INDEX        end_of_func_label;
    SYM_HANDLE         func_result;
    TREEPTR            tree;
    char               return_at_outer_level;
    char               skip_to_next_token;
    struct return_info return_info;
    SYM_ENTRY sym;

#ifndef NDEBUG
    if( DebugFlag >= 1 ) {
        printf( "***** line %d, func=%s\n", TokenLine, CurFunc->name );
        PrintStats();
    }
#endif
    ++FuncCount;
    return_info.with = RETURN_WITH_NONE; /* indicate no return statements */
    return_info.with_expr = FALSE;
    CompFlags.label_dropped = 0;
    CompFlags.addr_of_auto_taken = 0;           /* 23-oct-91 */
    end_of_func_label = 0;
    return_at_outer_level = 0;                  /* 28-feb-92 */
    DeadCode = 0;
    LoopDepth = 0;
    LabelIndex = 0;
    BlockStack = NULL;
    LoopStack = NULL;
    SwitchStack = NULL;
#ifdef __SEH__
    TryCount = -1;
    TryScope = -1;
#endif
    startNewBlock();
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
        GrabLabels();
        skip_to_next_token = 0;
        switch( CurToken ) {
        case T_IF:
            startNewBlock();
            NextToken();
            BlockStack->break_label = NextLabel();
            JumpFalse( BracketExpr(), BlockStack->break_label );
            /* 23-dec-88, only issue msg if ';' is on same line as 'if' */
            if( CurToken == T_SEMI_COLON  &&  SrcLineNum == TokenLine ) {
                SetErrLoc( ErrFName, TokenLine );       /* 04-nov-91 */
                NextToken();    /* look ahead for else keyword */
                if( CurToken != T_ELSE ) {                      /* 02-apr-91 */
                    ChkUseful();            /* 08-dec-88 */
                }
                SetErrLoc( NULL, 0 );           /* 04-nov-91 */
                break;
            }
            continue;
        case T_WHILE:
            NewLoop();
            NextToken();
            BlockStack->break_label = NextLabel();
            if( ! JumpFalse( BracketExpr(), BlockStack->break_label ) ) {
                BlockStack->break_label = 0;    /* 09-sep-92 */
            }
            if( CurToken == T_SEMI_COLON ) {    /* 08-dec-88 */
                if( ! CompFlags.useful_side_effect ) {
                    CWarn1( WARN_MEANINGLESS, ERR_MEANINGLESS );
                }
            }
            continue;
        case T_DO:
            NewLoop();
            NextToken();
            if( CurToken == T_RIGHT_BRACE ) {
                CErr1( ERR_STMT_REQUIRED_AFTER_DO );
                break;
            }
            continue;
        case T_FOR:
            ForStmt();
            continue;
        case T_SWITCH:
            SwitchStmt();
            DeadCode = 1;
            continue;
        case T_CASE:
            DeadCode = 0;
            CaseStmt();
            continue;
        case T_DEFAULT:
            DefaultStmt();
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
            LeaveStmt();
            DeadCode = 1;
            if( BlockStack->block_type != T_LEFT_BRACE ) break;
            continue;
#endif
        case T_RETURN:
            ReturnStmt( func_result, &return_info );
            if( BlockStack->prev_block == NULL ) {      /* 28-feb-92 */
                return_at_outer_level = 1;
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
            if( BlockStack->block_type != T_LEFT_BRACE ) break;
            continue;
        case T_LEFT_BRACE:
            LeftBrace();
            continue;
        case T_RIGHT_BRACE:
            if( BlockStack->block_type != T_LEFT_BRACE ) {
                CErr1( ERR_MISPLACED_RIGHT_BRACE );
            }
            if( BlockStack->prev_block == NULL ) {
                skip_to_next_token = 1;
            } else {
                NextToken();
            }
            break;
        case T_STATIC:
        case T_EXTERN:
        case T_VOID:
        case T_INT:
        case T_LONG:
        case T_SHORT:
        case T_FLOAT:
        case T_DOUBLE:
        case T_EOF:
            CErr1( ERR_MISSING_RIGHT_BRACE );
            break;
#ifdef __SEH__
        case T__TRY:
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
        if( skip_to_next_token )  NextToken();
    }
    if( !return_info.with_expr ) {   /* no return values present */
        if( !CurFunc->naked ){
            ChkRetValue();
        }
    } else if( ! return_at_outer_level ) {              /* 28-feb-92 */
        if( ! DeadCode && !CurFunc->naked ) {

            CWarn( WARN_MISSING_RETURN_VALUE,
                    ERR_MISSING_RETURN_VALUE, CurFunc->name );
        }
    }
    if( end_of_func_label != 0 ) DropLabel( end_of_func_label );
    DeadCode = 0;
    tree->op.label_count = LabelIndex;
    tree = LeafNode( OPR_FUNCEND );
    if( !return_info.with_expr ) {
        tree->expr_type = NULL;
        tree->op.sym_handle = 0;
    } else {
        tree->op.sym_handle = func_result;
        SetFuncReturnNode( tree );
    }
    AddStmt( tree );
    AddSymList( CurFunc->u.func.locals );
    SrcLineNum = TokenLine;
    SrcFno = TokenFno;
    FreeLabels();
    if( skip_to_next_token )  NextToken();
    if( CompFlags.generate_prototypes ) {
        if( DefFile == NULL )  OpenDefFile();
        if( DefFile != NULL  &&
            CurFunc->stg_class == SC_NULL ) {  /* if function exported */
            DumpFuncDefn();
        }
    }

    if( Toggles & TOGGLE_INLINE ){
        if( Inline_Threshold < NodeCount ){
            CurFuncNode->op.func.flags &= ~FUNC_OK_TO_INLINE;
        }
    }
    if( VarParm( CurFunc ) ) {
        CurFuncNode->op.func.flags &= ~FUNC_OK_TO_INLINE;
    }
    NodeCount = 0;
    if( CompFlags.addr_of_auto_taken ) {                /* 23-oct-91 */
        CurFunc->flags &= ~ SYM_OK_TO_RECURSE;
    }
}


static void EndOfStmt()
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
            break;
        case T_DO:
            DropContinueLabel();
            MustRecog( T_WHILE );
            SrcLineNum = TokenLine;             /* 20-dec-88 */
            SrcFno = TokenFno;
            JumpTrue( BracketExpr(), BlockStack->top_label );
            MustRecog( T_SEMI_COLON );
            SrcLineNum = TokenLine;
            SrcFno = TokenFno;
            --LoopDepth;
            DropBreakLabel();
            break;
        case T_SWITCH:
            EndSwitch();
            DropBreakLabel();
            break;
#ifdef __SEH__
        case T__TRY:
            if( EndTry() )      return;
            break;
        case T__EXCEPT:
            DropBreakLabel();
            TryScope = BlockStack->parent_index;
            CompFlags.exception_handler = 0;
            break;
        case T__FINALLY:                                /* 23-mar-94 */
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


static void LeftBrace()
{
    TREEPTR     tree;
    TREEPTR     first_stmt;
    TREEPTR     last_stmt;
    TREEPTR     new_first;
    TREEPTR     new_last;

/*
    <storage> <type> function( <parms> )
    {   <- this is SymLevel == 1
    (weird code is for SymLevel > 1 )
*/
    // DeclList might generate some quads to do some initializers
    // if that happens, we want them output after the OPR_NEWBLOCK node
    startNewBlock();
    NextToken();
    ++SymLevel;
    first_stmt = FirstStmt;
    last_stmt = LastStmt;
    FirstStmt = NULL;
    LastStmt = NULL;
    BlockStack->sym_list = 0;
    DeclList( &BlockStack->sym_list );
    new_first = FirstStmt;
    new_last = LastStmt;
    FirstStmt = first_stmt;
    LastStmt = last_stmt;
    if( BlockStack->sym_list != 0 ) {
        tree = LeafNode( OPR_NEWBLOCK );
        tree->op.sym_handle = BlockStack->sym_list;
        AddStmt( tree );
    }
    if( new_first != NULL ) {
        LastStmt->left = new_first;
        LastStmt = new_last;
    }
}

static void JumpBreak( BLOCKPTR block )
{
    if( ! DeadCode ) {                          /* 05-apr-92 */
        if( block->break_label == 0 ) {
            block->break_label = NextLabel();
        }
        Jump( block->break_label );
    }
}

static void BreakStmt()
{
    BLOCKPTR    block;
    int         try_scope;

    NextToken();
    try_scope = -2;
    block = BlockStack;
    if( block != NULL ) {
        while( block != LoopStack ) {
            if( block->block_type == T_SWITCH ) break;
            if( block->block_type == T__TRY ) {
                try_scope = block->parent_index;
            }
            block = block->prev_block;
        }
    }
    if( block != NULL ) {
        if( try_scope != -2 ) {
            UnWindTry( try_scope );
        }
        JumpBreak( block );
    } else {
        CErr1( ERR_MISPLACED_BREAK );
    }
    MustRecog( T_SEMI_COLON );
}

#ifdef __SEH__
static void LeaveStmt()
{
    BLOCKPTR    block;

    NextToken();
    block = BlockStack;
    while( block != NULL ) {
        if( block->block_type == T__TRY ) break;
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

static void ContinueStmt()
{
    BLOCKPTR    block;
    int         try_scope;

    NextToken();
    if( LoopStack != NULL ) {
        if( ! DeadCode ) {                              /* 05-apr-92 */
            try_scope = -2;
            block = BlockStack;
            while( block != LoopStack ) {
                if( block->block_type == T__TRY ) {
                    try_scope = block->parent_index;
                }
                block = block->prev_block;
            }
            if( try_scope != -2 ) {
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

static void DropBreakLabel()
{
    if( BlockStack->break_label != 0 ) {        /* 05-apr-92 */
        DropLabel( BlockStack->break_label );
    }
}

static void DropContinueLabel()
{
    if( BlockStack->continue_label != 0 ) {
        DropLabel( BlockStack->continue_label );
    }
}


static void DefaultStmt()
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


static void ElseStmt()
{
    LABEL_INDEX if_label;

    SrcLineNum = TokenLine;             /* 18-jan-89 */
    SrcFno     = TokenFno;
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


static void GotoStmt()
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


static void ForStmt()
{
    NextToken();
    MustRecog( T_LEFT_PAREN );
    if( CurToken != T_SEMI_COLON )  ChkStmtExpr();      // init_expr
    MustRecog( T_SEMI_COLON );
    NewLoop();
    BlockStack->block_type = T_FOR;
    if( CurToken != T_SEMI_COLON ) {
        BlockStack->break_label = NextLabel();
        if( ! JumpFalse( Expr(), BlockStack->break_label ) ) {
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

static void EndForStmt()
{
    DropContinueLabel();
    if( BlockStack->inc_var ) {
        AddStmt( BlockStack->inc_var );
    }
    Jump( BlockStack->top_label );
}


static void StmtExpr()
{
    ChkStmtExpr();
    switch( CurToken ) {
    case T_IF:
    case T_WHILE:
    case T_DO:
    case T_FOR:
    case T_SWITCH:
    case T_CASE:
    case T_DEFAULT:
    case T_BREAK:
    case T_CONTINUE:
    case T_RETURN:
    case T_GOTO:
    case T_LEFT_BRACE:
    case T_RIGHT_BRACE:
        Expecting( Tokens[ T_SEMI_COLON ] );
        break;
    default:
        Expecting( Tokens[ T_SEMI_COLON ] );
    case T_SEMI_COLON:
        NextToken();
        break;
    }
}


void ChkStmtExpr()
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


static void ChkUseful()
{
    if( CompFlags.useful_side_effect ) {
        CWarn1( WARN_USEFUL_SIDE_EFFECT, ERR_USEFUL_SIDE_EFFECT );
    } else {
        CWarn1( WARN_MEANINGLESS, ERR_MEANINGLESS );
    }
}


static void AddCaseLabel( unsigned long value )
{
    CASEPTR     ce, prev_ce, new_ce;
    unsigned long old_value, converted_value;
    TREEPTR     tree;
    auto char   buffer[12];

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
        new_ce = (CASEPTR) CMemAlloc( sizeof( CASEDEFN ) );
        new_ce->value = converted_value;
        new_ce->label = NextLabel();
        if( prev_ce == NULL ) {
            new_ce->next_case = SwitchStack->case_list;
            SwitchStack->case_list = new_ce;
        } else {
            prev_ce->next_case = new_ce;
            new_ce->next_case = ce;
        }
        SwitchStack->number_of_cases++;
        if( converted_value < SwitchStack->low_value ) {
            SwitchStack->low_value = converted_value;
        }
        if( converted_value > SwitchStack->high_value ) {
            SwitchStack->high_value = converted_value;
        }
        tree = LeafNode( OPR_CASE );
        tree->op.label_index = new_ce->label;
        AddStmt( tree );
    }
}


static void CaseStmt()
{
    const_val val;
    NextToken();
    if( SwitchStack ) {
        if( ConstExprAndType( &val ) ){
            AddCaseLabel( val.val32 );
        }
        MustRecog( T_COLON );
        if( CurToken == T_RIGHT_BRACE ) {
            CErr1( ERR_STMT_REQUIRED_AFTER_CASE );
        }
//      FlushScoreBoard();
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

    for(;;) {
        if( sym_handle == 0 ) break;
        SymGet( &sym, sym_handle );
        sym.flags |= SYM_TRY_VOLATILE; //force into memory
        SymReplace( &sym, sym_handle );
        sym_handle = sym.handle;
    }
}

static void TryStmt()
{
    TREEPTR     tree;

    MarkTryVolatile( CurFunc->u.func.parms );
    MarkTryVolatile( CurFunc->u.func.locals );
    CurFuncNode->op.func.flags |=  FUNC_USES_SEH;
    CurFuncNode->op.func.flags &= ~FUNC_OK_TO_INLINE;
    CurToken = T__TRY;
    startNewBlock();
    NextToken();
    BlockStack->parent_index = TryScope;
    ++TryCount;
    BlockStack->try_index = TryCount;
    TryScope = TryCount;
    tree = LeafNode( OPR_TRY );
    tree->op.try_index = 0;
    tree->op.parent_scope = TryCount;
    AddStmt( tree );
}

static SYM_HANDLE DummyTrySymbol()
{
    SYM_ENTRY   sym;
    SYM_HANDLE  sym_handle;

    sym_handle = MakeNewSym( &sym, 'T', GetType( TYPE_VOID ), SC_STATIC );
    SymReplace( &sym, sym_handle );
    return( sym_handle );
}

static int EndTry()
{
    int         parent_scope;
    TREEPTR     expr;
    TREEPTR     func;
    TREEPTR     tree;
    TYPEPTR     typ;
    int         expr_type;

    DropBreakLabel();           /* _leave jumps to this label */
    parent_scope = BlockStack->parent_index;
    tree = LeafNode( OPR_TRY );
    tree->op.try_index = BlockStack->try_index;
    tree->op.parent_scope = parent_scope;
    AddStmt( tree );
    if( CurToken == T__EXCEPT ) {
        NextToken();
        BlockStack->block_type = T__EXCEPT;
        BlockStack->break_label = NextLabel();
        Jump( BlockStack->break_label );
        DeadCode = 0;
        tree = LeafNode( OPR_EXCEPT );
        tree->op.try_sym_handle = DummyTrySymbol();
        tree->op.parent_scope = parent_scope;
        AddStmt( tree );
        CompFlags.exception_filter_expr = 1;
        expr = RValue( BracketExpr() );
        CompFlags.exception_filter_expr = 0;
        CompFlags.exception_handler = 1;
        typ = TypeOf( expr );
        expr_type = DataTypeOf( typ->decl_type );
        if( expr_type != TYPE_VOID ) {
            if( expr_type > TYPE_ULONG ) {
                CErr1( ERR_EXPR_MUST_BE_INTEGRAL );
            }
        }
        func = VarLeaf( SymGetPtr( SymExcept ), SymExcept );
        func->op.opr = OPR_FUNCNAME;
        expr = ExprNode( NULL, OPR_PARM, expr );
        expr->expr_type = typ;
        expr->op.result_type = typ;
        tree = ExprNode( func, OPR_CALL, expr );
        tree->expr_type = GetType( TYPE_VOID );
        AddStmt( tree );
        return( 1 );
    } else if( CurToken == T__FINALLY ) {
        CompFlags.in_finally_block = 1;
        NextToken();
        BlockStack->block_type = T__FINALLY;
        DeadCode = 0;
        tree = LeafNode( OPR_FINALLY );
        tree->op.try_sym_handle = DummyTrySymbol();
        tree->op.parent_scope = parent_scope;
        AddStmt( tree );
        return( 1 );
    }
    return( 0 );
}
#endif

static void UnWindTry( int try_scope )
{
#ifdef __SEH__
    TREEPTR     tree;

    tree = LeafNode( OPR_UNWIND );
    tree->op.try_index = try_scope;
    AddStmt( tree );
#else
    try_scope = try_scope;
#endif
}

static void NewLoop()
{
    startNewBlock();
    LoopStack = BlockStack;
    LoopStack->top_label = NextLabel();
    DropLabel( LoopStack->top_label );
    ++LoopDepth;
}


static void startNewBlock()
{
    BLOCKPTR    block;

    block = (BLOCKPTR) CMemAlloc( sizeof( BLOCKDEFN ) );
    block->block_type = CurToken;
    block->top_label = 0;
    block->continue_label = 0;
    block->break_label = 0;
    block->prev_block = BlockStack;
    block->prev_loop = LoopStack;
    BlockStack = block;
}


static void PopBlock()
{
    BLOCKPTR    block;
    TREEPTR     tree;

    if( BlockStack->sym_list != 0 ) {
        tree = LeafNode( OPR_ENDBLOCK );
        tree->op.sym_handle = BlockStack->sym_list;
        AddStmt( tree );
        AddSymList( BlockStack->sym_list );
    }
    block = BlockStack;
    LoopStack = block->prev_loop;
    BlockStack = block->prev_block;
    CMemFree( block );
}


static void SwitchStmt()
{
    SWITCHPTR   sw;
    TREEPTR     tree;
    TYPEPTR     typ;
    int         switch_type;

    startNewBlock();
    NextToken();
    sw = (SWITCHPTR) CMemAlloc( sizeof( SWITCHDEFN ) );
    sw->prev_switch = SwitchStack;
    sw->low_value = ~0l;
    sw->high_value = 0;
    sw->case_format = "%ld";        /* assume signed cases */
    SwitchStack = sw;
    switch_type = TYPE_INT;     /* assume int */
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
    tree->op.switch_info = sw;
    AddStmt( tree );
}


static void EndSwitch()
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
//    FlushScoreBoard();
}

#if 0

void PurgeBlockStack()
{
    BLOCKPTR block;

    block = BlockStack;
    while( block != NULL ) {
        BlockStack = block->prev_block;
        CMemFree( block );
        block = BlockStack;
    }
    LoopStack = NULL;
}
#endif
