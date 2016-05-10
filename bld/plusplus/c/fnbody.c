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


#include "plusplus.h"

#include <stddef.h>

#include "preproc.h"
#include "memmgr.h"
#include "carve.h"
#include "ring.h"
#include "stack.h"
#include "name.h"
#include "yydriver.h"
#include "cgfront.h"
#include "cgsegid.h"
#include "codegen.h"
#include "dbg.h"
#include "template.h"
#include "class.h"
#include "fold.h"
#include "context.h"
#include "ctexcept.h"
#include "fnbody.h"
#include "initdefs.h"
#include "objmodel.h"
#include "asmstmt.h"
#ifndef NDEBUG
#include "pragdefn.h"
#endif
#include "toggle.h"

static FUNCTION_DATA *currFunction;
static SUICIDE_CALLBACK functionSuicide;

static carve_t carveCSTACK;
static carve_t carveSWCASE;
static carve_t carveFNLABEL;
static carve_t carveFNCATCH;


enum                            // EXPR_ANAL -- result of expression analysis
{   EXPR_ANAL_OK                // - expression parsed, analyzis succeeded
,   EXPR_ANAL_NONE              // - no expression existed to be parsed
,   EXPR_ANAL_ERR               // - expression parsed, analysis failed
};
typedef unsigned EXPR_ANAL;     // - typed as unsigned for efficiency

static void nextYYToken( void )
{
    ParseFlush();       // cleanup YYDRIVER
    NextToken();
}

static void mustRecog( TOKEN token )
{
    ParseFlush();       // cleanup YYDRIVER
    MustRecog( token );
}


void FunctionBodyDeadCode(      // MARK DEAD-CODE IN CURRENT FUNCTION
    void )
{
    currFunction->dead_code = true;
    currFunction->dead_diagnosed = false;
}

static void doJUMP( int op, unsigned condition, CGLABEL destination )
{
    CgFrontGotoNear( op, condition, destination );
    if( condition == O_GOTO ) {
        FunctionBodyDeadCode();
    }
}

static void dumpCSLabel( CGLABEL lbl )
{
    CgFrontLabdefCs( lbl );
    currFunction->dead_code = false;
}

static void dumpBlkLabel(       // DUMP A BLOCK LABEL
    BLK_LABEL *lab )            // - the label
{
    if( lab->defn != NULL ) {
        dumpCSLabel( lab->id );
        LabelDefine( lab->defn );
    }
}

static void dumpOutsideLabel(   // DUMP THE LABEL FOR THE OUTSIDE OF A BLOCK
    CSTACK *curr )              // - current block
{
    dumpBlkLabel( &curr->outside );
}


static LAB_REF *refBlkLabel(    // REFERENCE A BLOCK LABEL
    BLK_LABEL *lab )            // - the label
{
    if( lab->defn == NULL ) {
        lab->defn = LabelAllocLabDef();
    }
    return LabelAllocLabRef( lab->defn );
}


static void jumpBlkLabel(       // JUMP TO BLOCK LABEL
    BLK_LABEL *lab,             // - the label
    unsigned condition )        // - type of jump
{
    LabelRefFree( refBlkLabel( lab ) );
    doJUMP( IC_LABEL_CS, condition, lab->id );
}


static void jumpFwdBlkLabel(    // JUMP FORWARD TO BLOCK LABEL
    BLK_LABEL *lab )            // - the label
{
    SrcPosnEmitCurrent();
    LabelGotoFwd( refBlkLabel( lab ) );
    doJUMP( IC_LABEL_CS, O_GOTO, lab->id );
}


static void initBlkLabel(       // INITIALIZE BLOCK LABEL
    BLK_LABEL *lab )            // - the label
{
    lab->id = CgFrontLabelCs();
    lab->defn = NULL;
}

static void ensureLiveCode(     // ENSURE WE DON'T HAVE DEAD CODE (PARSE)
    void )
{
    if( currFunction->dead_code && ! currFunction->dead_diagnosed ) {
        CErr1( WARN_DEAD_CODE );
        currFunction->dead_diagnosed = true;
    }
}

static void ensureLiveExpr(     // ENSURE WE DON'T HAVE DEAD CODE (EXPR)
    PTREE expr )                // - parsed expression
{
    if( currFunction->dead_code && ! currFunction->dead_diagnosed ) {
        PTreeWarnExpr( expr, WARN_DEAD_CODE );
        currFunction->dead_diagnosed = true;
    }
}

static PTREE emitCodeExpr( PTREE expr )
{
    ensureLiveExpr( expr );
    return( IcEmitExpr( expr ) );
}

static PTREE safeParseExpr( TOKEN end_token )
{
    if( CurToken == T_EOF ) {
        return( NULL );
    }
    return( ParseExpr( end_token ) );
}

static EXPR_ANAL exprAnalyse(
    PTREE (*analyse_rtn)( PTREE ),
    PTREE *expr )
{
    EXPR_ANAL retn;

    if( *expr == NULL ) {
        retn = EXPR_ANAL_NONE;
    } else {
        *expr = (*analyse_rtn)( *expr );
        if( (*expr)->op == PT_ERROR ) {
            retn = EXPR_ANAL_ERR;
        } else {
            retn = EXPR_ANAL_OK;
        }
    }
    return( retn );
}

static void warnBoolConstVal(   // WARN: FOR A CONSTANT VALUE
    bool is_zero,               // - true ==> zero constant
    bool parsed_int_const,      // - user coded an int constant
    PTREE expr )                // - expression
{
    CSTACK *ctl;                // - top control stack

    ctl = currFunction->control;
    if( is_zero ) {
        ctl->expr_false = true;
        switch( ctl->id ) {
          case CS_IF :
            PTreeWarnExpr( expr, WARN_IF_ALWAYS_FALSE );
            break;
          case CS_FOR :
            PTreeWarnExpr( expr, WARN_FOR_FALSE );
            break;
          case CS_WHILE :
            PTreeWarnExpr( expr, WARN_WHILE_FALSE );
            break;
          case CS_DO :
            if( ! parsed_int_const ) {
                PTreeWarnExpr( expr, WARN_WHILE_FALSE );
            }
            break;
          case CS_SWITCH :
            PTreeWarnExpr( expr, WARN_SWITCH_ALWAYS_CONSTANT );
            break;
        }
    } else {
        ctl->expr_true = true;
        switch( ctl->id ) {
          case CS_WHILE :
          case CS_DO :
            if( ! parsed_int_const ) {
                PTreeWarnExpr( expr, WARN_WHILE_TRUE );
            }
            break;
          case CS_FOR :
            if( ! parsed_int_const ) {
                PTreeWarnExpr( expr, WARN_FOR_TRUE );
            }
            break;
          case CS_IF :
            PTreeWarnExpr( expr, WARN_IF_ALWAYS_TRUE );
            break;
          case CS_SWITCH :
            PTreeWarnExpr( expr, WARN_SWITCH_ALWAYS_CONSTANT );
            break;
        }
    }
}

static void warnBoolConst(      // WARN, WHEN SPECIFIC BOOLEAN CONSTANT
    PTREE expr,                 // - analysed expression
    bool parsed_int_const )     // - user coded an int constant
{
    if( NodeIsConstantInt( expr ) ) {
        warnBoolConstVal( NodeIsZeroIntConstant( expr )
                        , parsed_int_const
                        , expr );
    } else if( expr->flags & PTF_PTR_NONZERO ) {
        warnBoolConstVal( false, parsed_int_const, expr );
    }
}

static EXPR_ANAL parseBracketExpr(
    PTREE (*analyse_rtn)( PTREE ) )
{
    PTREE expr;
    EXPR_ANAL retn;
    bool parsed_int_const;

    mustRecog( T_LEFT_PAREN );
    expr = safeParseExpr( T_RIGHT_PAREN );
    parsed_int_const = NodeIsConstantInt( expr );
    retn = exprAnalyse( analyse_rtn, &expr );
    if( retn != EXPR_ANAL_NONE ) {
        warnBoolConst( expr, parsed_int_const );
        emitCodeExpr( expr );
    }
    mustRecog( T_RIGHT_PAREN );
    return( retn );
}

static void checkDeclLocation( void )
{
    switch( currFunction->control->id ) {
    case CS_IF:
        CErr1( WARN_DCL_IF );
        break;
    case CS_ELSE:
        CErr1( WARN_DCL_ELSE );
        break;
    case CS_SWITCH:
        CErr1( WARN_DCL_SWITCH );
        break;
    case CS_FOR:
        CErr1( WARN_DCL_FOR );
        break;
    case CS_WHILE:
        CErr1( WARN_DCL_WHILE );
        break;
    case CS_DO:
        CErr1( WARN_DCL_DO );
        break;
    }
}

static void emitStmtExpr( PTREE expr_tree )
{
    ensureLiveExpr( expr_tree );
    expr_tree = AnalyseStmtExpr( expr_tree );
    expr_tree = emitCodeExpr( expr_tree );
}

static void declExprStmt( bool for_stmts_decl )
{
    PTREE expr_tree;

    if( CurToken == T_EOF ) return;
    expr_tree = ParseExprDecl();
    if( expr_tree != NULL ) {
        emitStmtExpr( expr_tree );
    } else {
        if( ! for_stmts_decl ) {
            checkDeclLocation();
        }
    }
}

static CSTACK *beginControl( cs_block_type id )
{
    CSTACK *new_block;

    new_block = StackCarveAlloc( carveCSTACK, &currFunction->control );
//printf( "PUSH: %p (%u) %s\n", new_block, id, (currFunction->fn_scope) ? (ScopeFunction( currFunction->fn_scope )->name->name) : "?" );
    new_block->id = id;
    new_block->outside.defn = NULL;
    new_block->try_var = NULL;
    SrcFileGetTokenLocn( &new_block->defined );
    switch( id ) {
    case CS_IF:
        new_block->u.i.else_part.defn = NULL;
        break;
    case CS_FOR:
    case CS_DO:
    case CS_WHILE:
        new_block->u.l.continue_loop.defn = NULL;
        break;
    case CS_BLOCK:
        new_block->u.b.block = currFunction->block_count;
        break;
    }
    new_block->expr_true = false;
    new_block->expr_false = false;
    return( new_block );
}

static void labelDefFree( BLK_LABEL *lab )
{
    LabelDefFree( lab->defn );
    lab->defn = NULL;
}

static void endControl( void )
{
    CSTACK *old_block;

    old_block = StackPop( &(currFunction->control) );
    if( old_block == NULL ) return;
//printf( "POP:  %p (%u) %s\n", old_block, old_block->id, ScopeFunction( currFunction->fn_scope )->name->name );
    labelDefFree( &(old_block->outside) );
    switch( old_block->id ) {
    case CS_IF:
    case CS_ELSE:
        labelDefFree( &(old_block->u.i.else_part) );
        break;
    case CS_FOR:
    case CS_DO:
    case CS_WHILE:
        labelDefFree( &(old_block->u.l.continue_loop) );
        break;
    }
    CarveFree( carveCSTACK, old_block );
}

static void openScope( void )
{
    ScopeBeginBlock( currFunction->block_count++ );
    LabelBlockOpen( currFunction->dead_code );
}

static void closeScope( void )
{
    LabelBlockClose( currFunction->dead_code );
    ScopeEnd( SCOPE_BLOCK );
}

static void startBlock( void )
{
    beginControl( CS_BLOCK );
    openScope();
    currFunction->depth++;
}


static void endBlock( void )
{
    if( currFunction->depth > currFunction->outer_depth ) {
        closeScope();
    }
    currFunction->depth--;
}

static FNLABEL *findLabel( NAME name )
{
    FNLABEL *curr;
    FNLABEL *new_label;

    RingIterBeg( currFunction->labels, curr ) {
        if( curr->name == name ) {
            return( curr );
        }
    } RingIterEnd( curr )
    new_label = RingCarveAlloc( carveFNLABEL, &currFunction->labels );
    new_label->name = name;
    new_label->block = currFunction->block_count;
    new_label->defined = false;
    new_label->dangerous = false;
    new_label->destination.id = CgFrontLabelGoto();
    new_label->destination.defn = LabelAllocLabDef();
    if( PragToggle.unreferenced ) {
        new_label->referenced = false;
    } else {
        new_label->referenced = true;
    }
    return( new_label );
}

static void parseLabels( void )
{
    FNLABEL *label;

    label = NULL;
    for(;;) {
        if( CurToken == T_ID ) {
            LookPastName();
        } else if( CurToken != T_SAVED_ID ) {
            break;
        }
        if( LAToken != T_COLON ) break;
        label = findLabel( SavedId );
        if( label->defined ) {
            CErr2p( ERR_LABEL_ALREADY_DEFINED, label->name );
            InfMsgPtr( INF_PREVIOUS_LABEL, &label->destination.defn->locn );
        } else {
            label->defined = true;
            CgFrontLabdefGoto( label->destination.id );
            LabelDefine( label->destination.defn );
        }
        currFunction->dead_code = false;
        nextYYToken();
        nextYYToken();
    }
    if( label != NULL ) {
        if( ( CurToken == T_RIGHT_BRACE ) || ( CurToken == T_ALT_RIGHT_BRACE ) ) {
            CErr1( ERR_STMT_REQUIRED_AFTER_LABEL );
        }
    }
}


static void parseIfStmt( void )
{
    CSTACK *if_block;

    ensureLiveCode();
    if_block = beginControl( CS_IF );
    initBlkLabel( &if_block->outside );
    initBlkLabel( &if_block->u.i.else_part );
    nextYYToken();
    openScope();
    if( EXPR_ANAL_OK == parseBracketExpr( &AnalyseBoolExpr ) ) {
        if( if_block->expr_true ) {
            CgFrontCode( IC_EXPR_TRASH );
        } else if( if_block->expr_false ) {
            CgFrontCode( IC_EXPR_TRASH );
            if( ! currFunction->dead_code ) {
                jumpBlkLabel( &if_block->u.i.else_part, O_GOTO );
            }
        } else {
            if( currFunction->dead_code ) {
                CgFrontCode( IC_EXPR_TRASH );
            } else {
                jumpBlkLabel( &if_block->u.i.else_part, O_IF_FALSE );
            }
        }
    } else {
        jumpBlkLabel( &if_block->u.i.else_part, O_GOTO );
        currFunction->dead_code = false;
    }
}


static void parseElseStmt( void )
{
    CSTACK *if_block;

    nextYYToken();
    if_block = currFunction->control;
    if_block->id = CS_ELSE;
    if( ( ! currFunction->dead_code )
      &&( ! if_block->expr_false ) ) {
        jumpBlkLabel( &if_block->outside, O_GOTO );
    }
    if( ! if_block->expr_true ) {
        dumpBlkLabel( &if_block->u.i.else_part );
    }
    CgFrontLabfreeCs( 1 );
}


static CSTACK *beginLoopControl( cs_block_type id )
{
    CSTACK *loop;

    loop = beginControl( id );
    initBlkLabel( &loop->outside );
    initBlkLabel( &loop->u.l.continue_loop );
    loop->u.l.top_loop = CgFrontLabelCs();
    return( loop );
}


static CSTACK *beginLoop( cs_block_type id )
{
    CSTACK *loop;

    nextYYToken();
    loop = beginLoopControl( id );
    dumpCSLabel( loop->u.l.top_loop );
    return( loop );
}

static void parseWhileStmt( void )
{
    CSTACK *loop;

    loop = beginLoop( CS_WHILE );
    openScope();
    if( EXPR_ANAL_OK == parseBracketExpr( &AnalyseBoolExpr ) ) {
        if( loop->expr_true ) {
            CgFrontCode( IC_EXPR_TRASH );
        } else if( loop->expr_false ) {
            CgFrontCode( IC_EXPR_TRASH );
            jumpBlkLabel( &loop->outside, O_GOTO );
        } else {
            jumpBlkLabel( &loop->outside, O_IF_FALSE );
        }
    } else {
        jumpBlkLabel( &loop->outside, O_GOTO );
        currFunction->dead_code = false;
    }
}

static void parseForStmt( void )
{
    EXPR_ANAL test_type;
    EXPR_ANAL inc_type;
    PTREE test_expr;
    PTREE inc_expr;
    CSTACK *loop;
    CGLABEL around;
    bool test_was_const;

    nextYYToken();
    mustRecog( T_LEFT_PAREN );
    if( ! CompFlags.use_old_for_scope ) {
        openScope();
    }
    if( CurToken == T_SEMI_COLON ) {
        mustRecog( T_SEMI_COLON );
    } else {
        ensureLiveCode();
        declExprStmt( true );
    }
    if( CurToken == T_SEMI_COLON ) {
        test_type = EXPR_ANAL_NONE;
    } else {
        test_expr = safeParseExpr( T_SEMI_COLON );
        test_was_const = NodeIsConstantInt( test_expr );
        test_type = exprAnalyse( &AnalyseBoolExpr, &test_expr );
    }
    mustRecog( T_SEMI_COLON );
    if( CurToken != T_RIGHT_PAREN ) {
        inc_expr = safeParseExpr( T_RIGHT_PAREN );
        inc_type = exprAnalyse( &AnalyseStmtExpr, &inc_expr );
    } else {
        inc_type = EXPR_ANAL_NONE;
    }
    mustRecog( T_RIGHT_PAREN );
    loop = beginLoopControl( CS_FOR );
    switch( inc_type ) {
      case EXPR_ANAL_OK :
        around = CgFrontLabelCs();
        doJUMP( IC_LABEL_CS, O_GOTO, around );
        dumpCSLabel( loop->u.l.top_loop );
        emitCodeExpr( inc_expr );
        dumpCSLabel( around );
        CgFrontLabfreeCs( 1 );
        break;
      case EXPR_ANAL_NONE :
        dumpCSLabel( loop->u.l.top_loop );
        break;
    }
    if( test_type == EXPR_ANAL_OK ) {
        warnBoolConst( test_expr, test_was_const );
        emitCodeExpr( test_expr );
        if( loop->expr_true ) {
            CgFrontCode( IC_EXPR_TRASH );
        } else {
            jumpBlkLabel( &loop->outside, O_IF_FALSE );
        }
    }
    openScope();
}


static PTREE analyseSwitchExpr( // ANALYSE SWITCH EXPRESSION
    PTREE expr )            // - parsed expression
{
    CSTACK *switch_block;

    expr = AnalyseIntegralExpr( expr );
    if( expr->op != PT_ERROR ) {
        if( NULL != Integral64Type( expr->type ) ) {
            SetErrLoc( &expr->locn );
            CErr1( ERR_64BIT_SWITCH );
        }
        switch_block = currFunction->control;
        switch_block->u.s.type = expr->type;
        if( UnsignedIntType( expr->type ) ) {
            switch_block->u.s.is_signed = false;
        } else {
            switch_block->u.s.is_signed = true;
        }
    }
    return( expr );
}


static void parseSwitchStmt( void )
{
    CSTACK *switch_block;

    ensureLiveCode();
    nextYYToken();
    switch_block = beginControl( CS_SWITCH );
    switch_block->u.s.type = NULL;
    openScope();
    initBlkLabel( &switch_block->outside );
    switch_block->u.s.cases = NULL;
    switch_block->u.s.imm_block = NULL;
    switch_block->u.s.default_dropped = 0;
    switch_block->u.s.defn_scope = GetCurrScope();
    switch_block->u.s.is_signed = true;
    switch_block->u.s.default_locn.src_file = NULL;
    SrcFileGetTokenLocn( &switch_block->u.s.switch_locn );
    parseBracketExpr( &analyseSwitchExpr );
    CgFrontSwitchBeg();
    LabelSwitchBeg();
    FunctionBodyDeadCode();
    openScope();
    if( ( CurToken == T_LEFT_BRACE ) || ( CurToken == T_ALT_LEFT_BRACE ) ) {
        switch_block->u.s.block_after = true;
    } else {
        switch_block->u.s.block_after = false;
    }
}

static bool is_dup_case(        // DIAGNOSE A DUPLICATE CASE
    CSTACK *my_switch,          // - control-stack entry
    PTREE  case_value,          // - exprn for case
    MSG_NUM err_code )          // - code for error message
{
    SWCASE *curr;               // - current case in search
    target_ulong case_uint;     // - value to look for
    auto char buff[16];         // - buffer for duplicate value

    case_uint = case_value->u.uint_constant;
    RingIterBeg( my_switch->u.s.cases, curr ) {
        if( curr->value == case_uint ) {
            if( my_switch->u.s.is_signed ) {
                sprintf( buff, "%d", case_value->u.int_constant );
            } else {
                sprintf( buff, "%u", case_value->u.uint_constant );
            }
            CErr2p( err_code, buff );
            InfMsgPtr( INF_PREVIOUS_CASE, &curr->defined );
            return( true );
        }
    } RingIterEnd( curr )
    return( false );
}

static SWCASE *makeCaseEntry(   // MAKE CASE/DEFAULT ENTRY
    TOKEN_LOCN *srcposn,        // - position in source
    CSTACK *my_switch )         // - control information
{
    SWCASE *ce;
    PTREE case_value;
    bool casted;

    case_value = safeParseExpr( T_COLON );
    ce = NULL;
    if( case_value == NULL ) {
        srcposn->src_file = NULL;
    } else {
        case_value = PTreeForceIntegral( case_value );
        if( case_value == NULL ) {
            srcposn->src_file = NULL;
        } else {
            *srcposn = case_value->locn;
            if( ( my_switch != NULL ) && ( my_switch->u.s.type != NULL ) ) {
                if( ! is_dup_case( my_switch
                                 , case_value
                                 , ERR_DUPLICATE_CASE_VALUE ) ) {
                    case_value = CastIntConstant( case_value
                                                , my_switch->u.s.type
                                                , &casted );
                    if( ( ! casted )
                      ||( ! is_dup_case( my_switch
                                       , case_value
                                       , ERR_DUPLICATE_CONV_CASE_VALUE ) ) ) {
                        if( NULL == Integral64Type( case_value->type ) ) {
                            ce = CarveAlloc( carveSWCASE );
                            ce->next = NULL;
                            ce->value = case_value->u.uint_constant;
                            SrcFileGetTokenLocn( &ce->defined );
                        } else {
                            PTreeErrorExpr( case_value, ERR_64BIT_CASE );
                        }
                    }
                }
                LabelSwitch( my_switch->u.s.defn_scope );
            }
            PTreeFree( case_value );
        }
    }
    mustRecog( T_COLON );
    return( ce );
}

static CSTACK *findSwitch( void )
{
    CSTACK *curr;

    for( curr = currFunction->control
       ; ( curr != NULL ) && ( curr->id != CS_SWITCH )
       ; curr = curr->next );
    return( curr );
}


static void parseCaseStmt( void )
{
    CSTACK *my_switch;
    SWCASE *case_entry;
    TOKEN_LOCN srcposn;

    my_switch = findSwitch();
    if( my_switch == NULL ) {
        CErr1( ERR_MISPLACED_CASE );
        nextYYToken();
        case_entry = makeCaseEntry( &srcposn, NULL );
        if( case_entry != NULL ) {
            CarveFree( carveSWCASE, case_entry );
        }
        case_entry = NULL;
    } else {
        nextYYToken();
        case_entry = makeCaseEntry( &srcposn, my_switch );
    }
    if( case_entry == NULL ) {
        currFunction->dead_code = false;
        return;
    }
    if( my_switch->u.s.cases == NULL ) {
        my_switch->u.s.lo = case_entry->value;
        my_switch->u.s.hi = case_entry->value;
    }
    CgFrontSwitchCase( my_switch->u.s.defn_scope
                     , currFunction->dead_code
                     , &srcposn
                     , case_entry->value );
    currFunction->dead_code = false;
    RingAppend( &(my_switch->u.s.cases), case_entry );
    if( ( CurToken == T_RIGHT_BRACE ) || ( CurToken == T_ALT_RIGHT_BRACE ) ) {
        CErr1( ERR_STMT_REQUIRED_AFTER_CASE );
    }
}

static void genDefaultStmt(     // GENERATE A DEFAULT STATEMENT
    CSTACK *my_switch,          // - control block for switch
    TOKEN_LOCN *posn )          // - source position
{
    my_switch->u.s.default_dropped = true;
    CgFrontSwitchDefault( my_switch->u.s.defn_scope
                        , currFunction->dead_code
                        , posn );
    currFunction->dead_code = false;
}

static void parseDefaultStmt( void )
{
    CSTACK *my_switch;

    my_switch = findSwitch();
    if( my_switch == NULL ) {
        CErr1( ERR_MISPLACED_DEFAULT );
    } else if( my_switch->u.s.default_dropped ) {
        CErr1( ERR_ONLY_1_DEFAULT );
        InfMsgPtr( INF_PREVIOUS_DEFAULT, &my_switch->u.s.default_locn );
    } else {
        SrcFileGetTokenLocn( &my_switch->u.s.default_locn );
    }
    nextYYToken();
    mustRecog( T_COLON );
    if( ( my_switch == NULL )
      ||( my_switch->u.s.default_dropped ) ) {
        currFunction->dead_code = false;
        return;
    }
    LabelSwitch( my_switch->u.s.defn_scope );
    genDefaultStmt( my_switch, &my_switch->u.s.default_locn );
    if( ( CurToken == T_RIGHT_BRACE ) || ( CurToken == T_ALT_RIGHT_BRACE ) ) {
        CErr1( ERR_STMT_REQUIRED_AFTER_DEFAULT );
    }
}

static CSTACK *findBreakable( void )
{
    CSTACK *curr;

    Stack_forall( currFunction->control, curr ) {
        switch( curr->id ) {
        case CS_FOR:
        case CS_DO:
        case CS_WHILE:
        case CS_SWITCH:
            return( curr );
        }
    }
    return( NULL );
}

static void parseBreakStmt( void )
{
    CSTACK *enclose;

    ensureLiveCode();
    enclose = findBreakable();
    if( enclose == NULL ) {
        CErr1( ERR_MISPLACED_BREAK );
    } else if( ! currFunction->dead_code ) {
        jumpFwdBlkLabel( &enclose->outside );
    }
    nextYYToken();
    mustRecog( T_SEMI_COLON );
}

static CSTACK *findContinuable( void )
{
    CSTACK *curr;

    Stack_forall( currFunction->control, curr ) {
        switch( curr->id ) {
          case CS_FOR:
          case CS_DO:
          case CS_WHILE:
            return( curr );
        }
    }
    return( NULL );
}

static void parseContinueStmt( void )
{
    CSTACK *enclose;

    ensureLiveCode();
    enclose = findContinuable();
    if( enclose == NULL ) {
        CErr1( ERR_MISPLACED_CONTINUE );
    } else {
        jumpFwdBlkLabel( &enclose->u.l.continue_loop );
    }
    nextYYToken();
    mustRecog( T_SEMI_COLON );
}

static void parseReturnStmt( SYMBOL func )
{
    PTREE expr;
    SYMBOL return_sym;
    SYMBOL return_operand;
    bool expecting_return;
    bool optimizing_return;

    ensureLiveCode();
    nextYYToken();
    return_sym = SymFunctionReturn();
    expecting_return = true;
    optimizing_return = false;
    if( return_sym == NULL
     || currFunction->is_ctor
     || currFunction->is_dtor ) {
        expecting_return = false;
        currFunction->retn_opt = false;
    }
    return_operand = NULL;
    if( CurToken == T_SEMI_COLON ) {
        if( expecting_return ) {
            CErr1( ERR_MISSING_RETURN_VALUE );
        }
    } else {
        currFunction->ret_reqd = true;
        expr = safeParseExpr( T_SEMI_COLON );
        if( expr != NULL ) {
            if( expecting_return ) {
                if( expr->op == PT_ERROR ) {
                    PTreeFreeSubtrees( expr );
                } else {
                    if( currFunction->retn_opt ) {
                        // start bracketting
                        optimizing_return = true;
                    }
                    emitCodeExpr( AnalyseReturnExpr( func, expr ) );
                    return_operand = return_sym;
                }
            } else {
                if( return_sym == NULL ) {
                    // see C++98 6.6.3 (3)
                    expr = AnalyseStmtExpr( expr );
                    if( ( expr->type != NULL ) && VoidType( expr->type ) ) {
                        emitCodeExpr( expr );
                        expr = NULL;
                    }
                }

                if( expr != NULL ) {
                    PTreeErrorExpr( expr, ERR_NOT_EXPECTING_RETURN_VALUE );
                    PTreeFreeSubtrees( expr );
                }
            }
        }
    }
    CgFrontReturnSymbol( return_operand );
    if( optimizing_return ) {
        // end bracketting
    }
    mustRecog( T_SEMI_COLON );
    FunctionBodyDeadCode();
}

bool FnRetnOpt(                 // TEST IF RETURN OPTIMIZATION ACTIVE
    void )
{
    return currFunction->retn_opt;
}

void FnRetnOptOff(              // TURN OFF RETURN OPTIMIZATION
    void )
{
    currFunction->retn_opt = false;
}

void FnRetnOptSetSym(           // SET SYMBOL FOR RETURN OPTIMIZATION
    SYMBOL sym )                // - the symbol
{
    currFunction->retn_opt_sym = sym;
}

SYMBOL FnRetnOptSym(            // GET SYMBOL FOR RETURN OPTIMIZATION
    void )
{
    return currFunction->retn_opt_sym;
}


bool FnRetnOptimizable(         // TEST IF SYMBOL COULD BE OPTIMIZED AWAY
    SYMBOL sym )                // - candidate symbol
{
    bool retb;                  // - return: false ==> symbol never optimized

    if( currFunction->retn_opt
     && SymIsAutomatic( sym )
     && ! SymIsInMem( sym )
     && ! SymIsAnonymousMember( sym )
     && ! SymIsArgument( sym ) ) {
        SYMBOL func = ScopeFunctionInProgress();
        TYPE retn_type = FunctionDeclarationType( func->sym_type ) -> of;
        retb = TypesIdentical( retn_type, sym->sym_type );
    } else {
        retb = false;
    }
    return( retb );
}


static void parseGotoStmt( void )
{
    FNLABEL *label;
    LAB_REF *lab_ref;

    ensureLiveCode();
    nextYYToken();
    if( CurToken != T_ID ) {
        CErr1( ERR_EXPECTING_LABEL );
    } else {
        label = findLabel( NameCreateLen( Buffer, TokenLen ) );
        lab_ref = LabelAllocLabRef( label->destination.defn );
        if( label->defined ) {
            LabelGotoBwd( lab_ref );
        } else {
            LabelGotoFwd( lab_ref );
        }
        label->referenced = true;
        doJUMP( IC_LABEL_GOTO, O_GOTO, label->destination.id );
    }
    if( CurToken != T_SEMI_COLON ) {
        nextYYToken();
    }
    mustRecog( T_SEMI_COLON );
}


static SYMBOL allocTryVar(      // CREATE TRY VARIABLE, IF REQ'D
    CSTACK *try_block )         // - try block
{
    SYMBOL try_var;             // - try variable for block

    try_var = try_block->next->try_var;
    if( try_var == NULL ) {
        try_var = SymCreate( MakeExpandableType( TYP_CHAR )
                           , SC_AUTO
                           , SF_REFERENCED | SF_ADDR_TAKEN | SF_CG_ADDR_TAKEN
                           , NameDummy()
                           , try_block->u.t.defn_scope );
        try_block->next->try_var = try_var;
    }
    return try_var;
}

static void parseTryBlock(      // PARSE TRY
    void )
{
    CSTACK *try_block;          // - control stack (changed to CS_CATCH later)

    ensureLiveCode();
    ExceptionsCheck();
    ScopeMarkVisibleAutosInMem();
    try_block = beginControl( CS_TRY );
    try_block->u.t.catch_err = false;
    try_block->u.t.catches = NULL;
    try_block->u.t.catch_no = 0;
    try_block->u.t.defn_scope = GetCurrScope();
    try_block->u.t.try_locn = SrcPosnEmitCurrent();
    initBlkLabel( &try_block->outside );
    try_block->u.t.try_var = allocTryVar( try_block );
    CgFrontCodePtr( IC_TRY, try_block->u.t.try_var );
    CgSetTypeExact( GetBasicType( TYP_SINT ) );
    CgFrontSwitchBeg();
    LabelSwitchBeg();
    LabelSwitch( try_block->u.t.defn_scope );
    CgFrontSwitchDefault( try_block->u.t.defn_scope
                        , false
                        , &try_block->u.t.try_locn );
    nextYYToken();
    mustRecog( T_LEFT_BRACE );
    startBlock();
    LabelBlkTry( &try_block->u.t.try_locn, try_block->u.t.try_var );
    FunctionHasRegistration();
}


static void completeTry(        // COMPLETE TRY BLOCK (CALL AFTER LAST CATCH)
    CSTACK *try_block )         // - block to be completed
{
    SrcPosnEmit( &try_block->u.t.try_locn );
    CgFrontSwitchEnd();
    SrcPosnEmitCurrent();
    dumpOutsideLabel( try_block );
    CgFrontLabfreeCs( 1 );
    RingCarveFree( carveFNCATCH, &(try_block->u.t.catches) );
    LabelTryComplete( try_block->u.t.try_var );
}


enum                            // ATTRIBUTES FOR CATCH TYPE
{   CATT_CLS   = 0x01           // - class or ref/ptr to class
,   CATT_PTR   = 0x02           // - pointer
,   CATT_PCPTR = 0x04           // - pointer has __based, __far16
,   CATT_FAR   = 0x08           // - pointer has __far in near model
,   CATT_REF   = 0x10           // - refererence
};

static TYPE getCatchTypeAttrs(  // GET CATCH TYPE ATTRIBUTES
    TYPE spectype,              // - type specified
    TYPE *a_tester,             // - type used to test equivalence
    uint_8 *a_attrs )           // - attributes for catch type
{
    TYPE sptype;                // - type specified (canonical format)
    TYPE type;                  // - type used to test attributes
    TYPE tester;                // - type used to test attributes
    uint_8 attrs;               // - attributes
    type_flag flags;            // - flags for pointed object

    sptype = TypeCanonicalThr( spectype );
    type = TypeReferenced( sptype );
    tester = StructType( type );
    if( tester == NULL ) {
        tester = TypePointedAt( type, &flags );
        if( tester == NULL ) {
            tester = type;
            attrs = 0;
        } else {
            type = tester;
            tester = StructType( tester );
            if( tester == NULL ) {
                tester = type;
                attrs = CATT_PTR;
            } else {
                attrs = CATT_PTR + CATT_CLS;
            }
            TypePointedAt( spectype, &flags );
            if( flags & (TF1_BASED | TF1_FAR16) ) {
                attrs |= CATT_PCPTR;
            } else if( (flags & TF1_FAR) && !IsBigData() ) {
                attrs |= CATT_FAR;
            }
        }
    } else {
        if( NULL == TypeReference( spectype ) ) {
            attrs = CATT_CLS;
        } else {
            attrs = CATT_CLS | CATT_REF;
        }
    }
    *a_attrs = attrs;
    *a_tester = tester;
    return sptype;
}

static void catchMsg( MSG_NUM msg, FNCATCH *catch_entry )
{
    if( CErr1( msg ) & MS_PRINTED ) {
        InfMsgPtr( INF_PREVIOUS_CATCH, &catch_entry->defined );
    }
}

static bool makeFNCATCH(        // MAKE CATCH ENTRY
    TYPE type,                  // - type of entry
    CSTACK *try_block,          // - try block for entry
    TOKEN_LOCN* cat_locn )      // - position of catch
{
    FNCATCH *catch_entry;       // - entry for catch
    FNCATCH *test;              // - used to validate types
    uint_8 new_attrs;           // - attributes: new
    uint_8 old_attrs;           // - attributes: old
    TYPE new_test;              // - test type: new
    TYPE old_test;              // - test type: old
    bool errors;                // - indicates errors during typesig lookup
    TYPE_SIG_ACCESS access;     // - type of type-sig access
    bool retb;                  // - true ==> no errors
    #define CATT_REF_PTR_CLS (CATT_CLS | CATT_PTR | CATT_REF)

    SetErrLoc( cat_locn );
    type = getCatchTypeAttrs( type, &new_test, &new_attrs );
    if( new_attrs & CATT_PCPTR ) {
        CErr1( ERR_CANNOT_USE_PCPTR );
        try_block->u.t.catch_err = true;
        return false;
    }
    if( new_attrs & CATT_FAR ) {
        CErr1( ERR_USE_FAR );
        try_block->u.t.catch_err = true;
        return false;
    }
    if( (new_attrs & CATT_REF_PTR_CLS) == CATT_CLS ) {
        if( ! TypeDefedNonAbstract( type
                                  , NULL
                                  , ERR_CATCH_ABSTRACT
                                  , ERR_CATCH_UNDEFED ) ) {
            try_block->u.t.catch_err = true;
            return false;
        }
        if( TypeRequiresDtoring( type ) ) {
            access = TSA_COPY_CTOR | TSA_DTOR;
        } else {
            access = TSA_COPY_CTOR;
        }
    } else {
        access = 0;
    }
    retb = true;
    TypeSigFind( access, type, NULL, &errors );
    if( errors ) {
        try_block->u.t.catch_err = true;
        retb = false;
    }
    new_attrs &= ~CATT_REF;
    RingIterBeg( try_block->u.t.catches, test ) {
        if( test->type == NULL ) {
            catchMsg( ERR_CATCH_FOLLOWS_ELLIPSIS, test );
            try_block->u.t.catch_err = true;
            retb = false;
            break;
        }
        getCatchTypeAttrs( test->type, &old_test, &old_attrs );
        old_attrs &= ~CATT_REF;
        if( new_attrs == old_attrs ) {
            if( TypesIdentical( new_test, old_test ) ) {
                catchMsg( WARN_CATCH_PREVIOUS, test );
            } else if( CATT_PTR & new_attrs ) {
                if( old_test->id == TYP_VOID ) {
                    catchMsg( WARN_CATCH_PREVIOUS, test );
                }
            } else if( CATT_CLS & new_attrs ) {
                if( DERIVED_NO != TypeDerived( new_test, old_test ) ) {
                    catchMsg( WARN_CATCH_PREVIOUS, test );
                }
            }
        }
    } RingIterEnd( test )
    SetErrLoc( NULL );
    catch_entry = RingCarveAlloc( carveFNCATCH, &try_block->u.t.catches );
    catch_entry->type = type;
    catch_entry->defined = *cat_locn;
    CgFrontCodePtr( IC_CATCH_VAR, try_block->next->try_var );
    CgFrontCodePtr( IC_CATCH, type );
    return( retb );

    #undef CATT_REF_PTR_CLS
}


static SYMBOL updateTryVar(     // UPDATE TRY VARIABLE
    CSTACK *try_block,          // - try block
    TYPE type )                 // - type for catch variable
{
    TYPE try_type;              // - type for try_variable
    target_size_t size;         // - size of catch variable
    SYMBOL try_var;             // - try variable

    try_var = try_block->next->try_var;
    try_type = TypedefModifierRemove( try_var->sym_type );
    size = CgMemorySize( type );
    if( size > try_type->u.a.array_size ) {
        try_type->u.a.array_size = size;
    }
    return try_var;
}


static SYMBOL makeCatchVar(     // CREATE A CATCH VARIABLE
    SYMBOL try_var,             // - aliased symbol
    DECL_INFO *info )           // - parse information
{
    SYMBOL catch_var;           // - new catch variable
    NAME name;                  // - variable name

    if( info->id == NULL ) {
        name = NameDummy();
    } else {
        name = info->id->u.id.name;
    }
    catch_var = SymCreateCurrScope( info->type
                                  , SC_AUTO
                                  , SF_REFERENCED
                                  , name );
    catch_var->flag |= SF_ALIAS | SF_CATCH_ALIAS;
    catch_var->u.alias = try_var;
    return catch_var;
}


static void parseCatchHandler(  // PARSE CATCH STATEMENT
    void )
{
    DECL_INFO *info;            // - info. about parsed catch variable
    SYMBOL try_var;             // - try variable
    SYMBOL catch_var;           // - variable for catch
    CSTACK *try_block;          // - try block for catch
    TOKEN_LOCN posn;            // - position of catch
    TOKEN_LOCN blk;             // - position of catch block

    SrcFileGetTokenLocn( &posn );
    try_block = currFunction->control;
    if( ( try_block->id != CS_CATCH ) && ( try_block->id != CS_TRY ) ) {
        CErr1( ERR_MISPLACED_CATCH );
        try_block = NULL;
    }
    ExceptionsCheck();
    nextYYToken();
    mustRecog( T_LEFT_PAREN );
    if( CurToken == T_DOT_DOT_DOT ) {
        nextYYToken();
        info = NULL;
    } else {
        info = ParseException();
        if( info == NULL ) {
            try_block = NULL;
        }
    }
    mustRecog( T_RIGHT_PAREN );
    currFunction->dead_code = false;
    SrcPosnEmitCurrent();
    SrcFileGetTokenLocn( &blk );
    mustRecog( T_LEFT_BRACE );
    startBlock();
    SrcPosnEmitCurrent();
    if( try_block != NULL ) {
        if( info == NULL ) {
            makeFNCATCH( NULL, try_block, &posn );
        } else {
            makeFNCATCH( info->type, try_block, &posn );
            try_var = updateTryVar( try_block, info->type );
        }
        try_block->id = CS_CATCH;
        ++try_block->u.t.catch_no;
        LabelSwitch( try_block->u.t.defn_scope );
        SrcFileGetTokenLocn( &posn );
        CgFrontSwitchCase( try_block->u.t.defn_scope
                         , false
                         , &posn
                         , try_block->u.t.catch_no );
        LabelBlkCatch( &blk, try_block );
        if( info != NULL ) {
            if( ! try_block->u.t.catch_err ) {
                catch_var = makeCatchVar( try_var, info );
                if( catch_var != NULL ) {
                    LabelDeclInited( catch_var );
                    if( SymRequiresDtoring( catch_var ) ) {
                        IcEmitDtorAutoSym( catch_var );
                    }
                }
            }
            FreeDeclInfo( info );
        }
    }
}

static void parseAsmStmt( TYPE fn_type )
{
    PTREE expr;

    ParseFlush();       // cleanup YYDRIVER
    expr = AsmStmt();
    if( expr != NULL ) {
        emitStmtExpr( expr );
        if( fn_type->flag & TF1_NAKED ) {
            FunctionBodyDeadCode();
        }
    }
}


static bool endOfStmt(          // PROCESS END-OF-STATEMENT
    bool recog )                // - true ==> need to recognize token
{
    CSTACK *top_block;
    CSTACK *next;
    cs_block_type id;
    bool dead_code;

    top_block = currFunction->control;
    id = top_block->id;
    for(;;) {
        switch( id ) {
        case CS_BLOCK:
            next = top_block->next;
            switch( next->id ) {
            case CS_SWITCH :
                if( top_block->u.b.block_switch ) {
                    if( next->u.s.default_dropped || ! GetCurrScope()->u.s.dtor_naked ) {
                        if( ! next->u.s.default_dropped ) {
                            TOKEN_LOCN locn;
                            locn = SrcPosnEmitCurrent();
                            genDefaultStmt( next, &locn );
                        }
                        next->u.s.imm_block = GetCurrScope();
                    }
                }
                if( ! currFunction->dead_code ) {
                    SrcPosnEmitCurrent();
                    jumpFwdBlkLabel( &next->outside );
                }
                break;
            case CS_CATCH :
                if( ! currFunction->dead_code ) {
                    SrcPosnEmitCurrent();
                    jumpFwdBlkLabel( &next->outside );
                }
                break;
            case CS_TRY :
                if( ! currFunction->dead_code ) {
                    SrcPosnEmitCurrent();
                    jumpFwdBlkLabel( &next->outside );
                }
                break;
            }
            endBlock();
            break;
        case CS_IF:
            if( recog ) {
                nextYYToken();
                recog = false;
            }
            if( CurToken == T_ELSE ) {
                parseElseStmt();
                /* Note that the scope opened in parseIfStmt is not
                 * closed when there is an "else" part. Also note that
                 * parseElseStmt doesn't open a new scope, so the
                 * closeScope in the CS_ELSE case below will close the
                 * scope opened in parseIfStmt.
                 *
                 * See 6.4 (3): the name introduced by a declaration
                 * in a conditoin is in scope until the end of the
                 * "else" part. BTW, re-declaring that name is not
                 * allowed by the standard, but the current code
                 * doesn't catch that.
                 */ 
                return recog;
            }
            if( ! top_block->expr_true ) {
                dumpBlkLabel( &top_block->u.i.else_part );
            }
            closeScope();
            CgFrontLabfreeCs( 2 );
            break;
        case CS_ELSE:
            dead_code = currFunction->dead_code;
            if( ! top_block->expr_false ) {
                dumpOutsideLabel( top_block );
            }
            if( ! dead_code ) {
                currFunction->dead_code = false;
            }
            closeScope();
            CgFrontLabfreeCs( 1 );
            break;
        case CS_FOR:
        case CS_WHILE:
            if( top_block->u.l.continue_loop.defn != NULL ) {
                dumpBlkLabel( &top_block->u.l.continue_loop );
            }
            closeScope();
            doJUMP( IC_LABEL_CS, O_GOTO, top_block->u.l.top_loop );
            dumpOutsideLabel( top_block );
            if( id == CS_FOR && ! CompFlags.use_old_for_scope ) {
                closeScope();
            }
            CgFrontLabfreeCs( 3 );
            break;
        case CS_DO:
            if( recog ) {
                nextYYToken();
                recog = false;
            }
            mustRecog( T_WHILE );
            if( top_block->u.l.continue_loop.defn != NULL ) {
                dumpBlkLabel( &top_block->u.l.continue_loop );
            }
            if( EXPR_ANAL_OK == parseBracketExpr( &AnalyseBoolExpr ) ) {
                ensureLiveCode();
                if( top_block->expr_false ) {
                    CgFrontCode( IC_EXPR_TRASH );
                } else if( top_block->expr_true ) {
                    CgFrontCode( IC_EXPR_TRASH );
                    doJUMP( IC_LABEL_CS, O_GOTO, top_block->u.l.top_loop );
                } else {
                    jumpBlkLabel( &top_block->outside, O_IF_FALSE );
                    doJUMP( IC_LABEL_CS, O_GOTO, top_block->u.l.top_loop );
                }
            } else {
                doJUMP( IC_LABEL_CS, O_GOTO, top_block->u.l.top_loop );
                currFunction->dead_code = false;
            }
            mustRecog( T_SEMI_COLON );
            dumpOutsideLabel( top_block );
            CgFrontLabfreeCs( 3 );
            break;
        case CS_SWITCH:
            closeScope();
            if( NULL == top_block->u.s.imm_block ) {
                if( ! top_block->u.s.default_dropped ) {
                    CgFrontCodeUint( IC_SWITCH_OUTSIDE, top_block->outside.id - 1 );
                    if( top_block->outside.defn == NULL ) {
                        top_block->outside.defn = LabelAllocLabDef();
                    }
                }
                if( ( !currFunction->dead_code ) && ( !top_block->u.s.block_after ) ) {
                    jumpFwdBlkLabel( &top_block->outside );
                }
            }
            SrcPosnEmit( &top_block->u.s.switch_locn );
            CgFrontSwitchEnd();
            SrcPosnEmitCurrent();
            dumpOutsideLabel( top_block );
            CgFrontLabfreeCs( 1 );
            if( top_block->u.s.cases != NULL ) {
                RingCarveFree( carveSWCASE, &(top_block->u.s.cases) );
            } else {
                // make sure switch is valid before warning
                if( top_block->u.s.type != NULL ) {
                    SetErrLoc( &top_block->u.s.switch_locn );
                    CErr1( WARN_SWITCH_NO_CASE_LABELS );
                }
            }
            closeScope();
            break;
        case CS_TRY:
            if( recog ) {
                nextYYToken();
                recog = false;
            }
            if( CurToken == T_CATCH ) return recog;
            if( ! top_block->u.t.catch_err ) {
                CErr1( ERR_CATCH_MISSING );
            }
            completeTry( top_block );
            break;
        case CS_CATCH:
            if( recog ) {
                nextYYToken();
                recog = false;
            }
            if( CurToken == T_CATCH ) return recog;
            completeTry( top_block );
            break;
        }
        endControl();
        top_block = currFunction->control;
        if( top_block == NULL ) return recog;
        id = top_block->id;
        if( id == CS_FUNCTION || id == CS_BLOCK ) break;
    }
    return recog;
}


// Scalar return values are assigned to a magic symbol.
//
// Class return values are allocated in the caller. The magic symbol is
// in the function scope, under the magic name (as the last parameter).
//
static void insertFunctionReturn( SYMBOL func )
{
    TYPE ret;               // - return type
    TYPE base;              // - base type for return
    SYMBOL retn_sym;        // - return symbol

    ret = FunctionDeclarationType( func->sym_type );
    base = TypedefModifierRemove( ret->of );
    switch( ObjModelFunctionReturn( ret ) ) {
      case OMR_VOID :
        break;
      case OMR_SCALAR :
      case OMR_CLASS_VAL :
        retn_sym = SymAllocReturn( GetCurrScope(), base );
        CgFrontCodePtr( IC_FUNCTION_RETN, retn_sym );
        break;
      case OMR_CLASS_REF :
        if( NULL == SymFunctionReturn() ) {
            retn_sym = SymAllocReturn( GetCurrScope()->enclosing, base );
            CgFrontCodePtr( IC_FUNCTION_RETN, retn_sym );
        }
        break;
    }
}


static void exceptSpec(         // GENERATE AN EXCEPTION SPECIFICATION
    TYPE except )               // - type of spec
{
    bool err_occurred;          // - true ==> error during type-sig

    TypeSigFind( 0, except, NULL, &err_occurred );
#ifndef NDEBUG
    if( err_occurred ) {
        CFatal( "setExceptionSpecs -- unexpected error" );
    }
#endif
    CgFrontCodePtr( IC_EXCEPT_SPEC, except );
    ScopeKeep( GetCurrScope() );
}


static void setExceptionSpecs(  // SET EXCEPTION SPEC.S FOR FUNCTION
    SYMBOL func )               // - the function
{
    TYPE *excepts;              // - vector of exception specifications
    TYPE except;                // - current exception specification
    bool no_excepts_allowed;    // - true ==> no exceptions allowed

    excepts = SymFuncArgList( func )->except_spec;
    if( excepts != NULL ) {
        ExceptionsCheck();
        no_excepts_allowed = true;
        for( ; ; ) {
            except = *excepts++;
            if( NULL == except ) break;
            no_excepts_allowed = false;
            exceptSpec( except );
        }
        if( no_excepts_allowed ) {
            exceptSpec( NULL );
        }
        FunctionHasRegistration();
    }
}


static void initFunctionData(   // INITIALIZE FUNCTION DATA (BEFORE FNSTARTUP)
    SYMBOL func,                // - function
    FUNCTION_DATA *f )          // - data
{
    NAME fn_name;

    CtxFunction( func );
    f->next = currFunction;
    currFunction = f;
    f->fn_scope = NULL;
    f->depth = 0;
    f->outer_depth = 0;
    f->control = NULL;
    f->labels = NULL;
    f->block_count = 0;
    f->flag_count = 0;
    f->init_var.var = NULL;
    f->init_var.mask = 1;
    f->access_errs = NULL;
    f->retn_opt_sym = NULL;
    f->dead_code = false;
    f->dead_diagnosed = false;
    f->always_dead_code = false;
    f->is_ctor = false;
    f->is_dtor = false;
    f->is_assign = false;
    f->is_defarg = false;
    f->ret_reqd = false;
    f->has_mem_init = false;
    f->has_state_tab = false;
    f->can_throw = false;
    f->does_throw = false;
    f->ctor_test = false;
    f->floating_ss = false;
    f->retn_opt = true;
    fn_name = func->name->name;
    if( fn_name == CppConstructorName() ) {
        f->is_ctor = true;
    } else if( fn_name == CppDestructorName() ) {
        f->is_dtor = true;
    } else if( fn_name == CppOperatorName( CO_EQUAL ) ) {
        f->is_assign = true;
    }
}

static void doFnStartup( SYMBOL func
                       , FUNCTION_DATA *fdata
                       , REWRITE *mem_initial
                       , TOKEN_LOCN *srcposn
                       , enum special_func flags )
{
    PTREE mem_init;

    fdata->fn_scope = GetCurrScope();
    func->flag |= SF_INITIALIZED;
    if( flags & FUNC_NO_STACK_CHECK ) {
        /* in case the type was derived from a stack-checked function */
        func->sym_type = RemoveFunctionFlag( func->sym_type, TF1_STACK_CHECK );
    } else {
        if( PragToggle.check_stack ) {
            func->sym_type = AddFunctionFlag( func->sym_type, TF1_STACK_CHECK );
        }
    }
    fdata->init_state_tab = CgFrontFuncOpen( func, srcposn );
    CgSegIdFunction( func );
    LabelInitFunc( &fdata->label_mem );
    startBlock();
    setExceptionSpecs( func );
    insertFunctionReturn( func );
    if( fdata->is_ctor ) {
        mem_init = ClassMemInit( func, mem_initial );
        CtorPrologue( func, mem_init, &startBlock );
        FnRetnOptOff();
        startBlock();
    } else if( fdata->is_dtor ) {
        DtorPrologue( func, &fdata->end_dtor, &fdata->dtor_beg );
        FnRetnOptOff();
        startBlock();
    }
    fdata->outer_depth = fdata->depth;
}

void FunctionBodyStartup(       // COMMON START-UP FOR ALL COMPILED FUNCTIONS
    SYMBOL func,                // - function being compiled
    FUNCTION_DATA *f,           // - function data for current function
    enum special_func flags )   // - extra flags
{
    initFunctionData( func, f );
    doFnStartup( func, f, NULL, &func->locn->tl, flags );
}

static void flushToDepth1( FUNCTION_DATA *f )
{
    depth_t depth;

    depth = f->outer_depth;
    while( depth != 1 ) {
        closeScope();
        endControl();
        --depth;
    }
}


static void functionShutdown(   // COMMON SHUT-DOWN FOR ALL COMPILED FUNCTIONS
    SYMBOL func,                // - function being compiled
    FUNCTION_DATA *f )          // - function data for current function
{
    if( func != NULL ) {
        if( f->has_state_tab ) {
            CgFrontZapUint( f->init_state_tab
                          , IC_FUNCTION_STAB
                          , f->flag_count );
        }
        if( f->dead_code ) {
            CgFrontCode( IC_FUNCTION_EPILOGUE );
        }
        if( f->is_ctor ) {
            CgFrontCode( IC_CTOR_END );
            flushToDepth1( f );
            CtorEpilogue( func );
            FunctionBodyDeadCode();
        } else if( f->is_dtor ) {
            flushToDepth1( f );
            DtorEpilogue( func, f->end_dtor, &f->dtor_beg );
            FunctionBodyDeadCode();
        }
        if( f->does_throw ) {
            func->flag |= SF_LONGJUMP;
        } else if( ! f->can_throw ) {
            func->flag |= SF_NO_LONGJUMP;
        }
        if( f->ctor_test ) {
            CgFrontCtorTest();
        }
    }
    closeScope();
    CgFrontFuncClose( func );
    LabelFiniFunc( &f->label_mem );
    endControl();
    if( f->floating_ss ) {
        TargetSwitches &= ~FLOATING_SS;
    }
    // AccessErrClear();
}


void FunctionBodyShutdown(      // COMMON SHUT-DOWN FOR ALL COMPILED FUNCTIONS
    SYMBOL func,                // - function being compiled
    FUNCTION_DATA *f )          // - function data for current function
{
    DbgAssert( f == currFunction );
    functionShutdown( func, f );
    currFunction = currFunction->next;
}


static TOKEN_LOCN *posnForFunction( // GET SOURCE POSITION FOR FUNCTION
    DECL_INFO *dinfo )          // - declaration info. for function
{
    PTREE tree;                 // - tree for id
    TOKEN_LOCN *srcposn;        // - source position

    tree = dinfo->id;
    if( tree == NULL ) {
        srcposn = NULL;
    } else {
        srcposn = &tree->locn;
    }
    return srcposn;
}


static void initFunctionBody( DECL_INFO *dinfo, FUNCTION_DATA *f, TYPE fn_type )
{
    SYMBOL func;                // - function being compiled
    REWRITE *mem_init;          // - tokens for mem-initializer

    func = dinfo->sym;
    initFunctionData( func, f );
    if( dinfo->scope != NULL ) {
        /* sets scope of inline friends to scope of the class */
        /* this is controversial but currently the letter of the law */
        ScopeJumpForward( dinfo->scope );
    }
    ScopeBeginFunction( func );
    if( (TargetSwitches & FLOATING_SS) == 0 ) {
        if( fn_type->flag & TF1_INTERRUPT ) {
            if( ! CompFlags.mfi_switch_used ) {
                TargetSwitches |= FLOATING_SS;
                f->floating_ss = true;
            }
        }
    }
    InsertArgs( &(dinfo->parms) );
    mem_init = dinfo->mem_init;
    dinfo->mem_init = NULL;
    if( mem_init != NULL && f->is_ctor ) {
        f->has_mem_init = true;
    }
    beginControl( CS_FUNCTION );
    while( CurToken == T_SEMI_COLON ) {
        nextYYToken();
    }
    if( ( CurToken == T_RIGHT_BRACE ) || ( CurToken == T_ALT_RIGHT_BRACE ) ) {
        // these must execute before doFnStartup so that the prologues
        // of ctors and dtors can be affected
        if( f->is_dtor ) {
            ClassDtorNullBody( func );
        } else if( f->is_ctor && ! f->has_mem_init ) {
            ClassCtorNullBody( func );
        } else if( f->is_assign ) {
            ClassAssignNullBody( func );
        }
    }
    doFnStartup( func, f, mem_init, posnForFunction( dinfo ), FUNC_NULL );
    ScopeArgumentCheck( GetCurrScope() );
}

static void finiLabel( FNLABEL *lbl )
{
    if( ! lbl->defined ) {
        CErr2p( ERR_UNDEFINED_LABEL, lbl->name );
        InfMsgPtr( INF_PREVIOUS_LABEL_REF, &lbl->destination.defn->locn );
    } else if( ! lbl->referenced ) {
        if( CErr2p( WARN_UNREFERENCED_LABEL, lbl->name ) & MS_PRINTED ) {
            CErr2p( INF_PREVIOUS_LABEL, &lbl->destination.defn->locn );
        }
    }
    labelDefFree( &(lbl->destination) );
    CarveFree( carveFNLABEL, lbl );
}

static void freeGotoLabels( void )
{
    FNLABEL *label;

    RingIterBegSafe( currFunction->labels, label ) {
        finiLabel( label );
    } RingIterEndSafe( label )
    currFunction->labels = NULL;
}

static void finiFunctionBody( SYMBOL func )
{
    CgFrontFuncInitFlags( currFunction->init_var.var );
    freeGotoLabels();
    functionShutdown( func, currFunction );
    ScopeJumpBackward( currFunction->fn_scope );
    ScopeEnd( SCOPE_FUNCTION );
    currFunction = currFunction->next;
}

static void set_output(         // SET OUTPUT LOCATION
    SYMBOL func )               // - symbol for function
{
    CgFrontSwitchFile( func );
    SrcPosnEmitCurrent();
}

static void skipFunctionBody( unsigned depth )
{
    for(;;) {
        switch( CurToken ) {
        case T_EOF:
            return;
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            ++depth;
            break;
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            --depth;
            if( depth == 0 ) {
                return;
            }
        }
        nextYYToken();
    }
}

static void badFunction( MSG_NUM msg, SYMBOL sym )
{
    CErr2p( msg, sym );
    skipFunctionBody( 1 );
}

static void badFunctionSym( SYMBOL sym )
{
    InfSymbolDeclaration( sym );
    skipFunctionBody( 1 );
}

enum
{   RETN_NOT_REQD
,   RETN_REQUIRED
,   RETN_DFLT_INT
};

static int returnIsRequired( TYPE fn_type )
{
    TYPE return_type;
    int retn;

    if( currFunction->dead_code
     || currFunction->always_dead_code
     || currFunction->is_ctor
     || currFunction->is_dtor ) {
        retn = RETN_NOT_REQD;
    } else {
        return_type = TypedefModifierRemove( fn_type->of );
        if( return_type->id == TYP_VOID ) {
            retn = RETN_NOT_REQD;
        } else if( DefaultIntType( return_type ) == NULL ) {
            retn = RETN_REQUIRED;
        } else {
            if( currFunction->ret_reqd ) {
                retn = RETN_REQUIRED;
            } else {
                retn = RETN_DFLT_INT;
            }
        }
    }
    return retn;
}


INIT_VAR *FunctionBodyGetInit( FUNCTION_DATA *fd )
/************************************************/
{
    if( fd != NULL ) {
        return( &(fd->init_var) );
    }
    if( currFunction != NULL ) {
        return( &(currFunction->init_var) );
    }
    return( NULL );
}

static TYPE handleDefnChecks( SYMBOL func )
{
    MSG_NUM msg;
    TYPE fn_type;
    type_flag flags;

    DbgAssert( FunctionDeclarationType( func->sym_type ) != NULL );
    fn_type = TypeGetActualFlags( func->sym_type, &flags );
    fn_type->of = BindTemplateClass( fn_type->of, &func->locn->tl, false );
    if( ! TypeDefined( fn_type->of ) ) {
        SetErrLoc( &func->locn->tl );
        CErr2p( ERR_CLASS_NOT_DEFINED, TypeClassInfo( fn_type->of )->name );
    }
    if( TypeVAStartWontWork( fn_type, &msg ) ) {
        SetErrLoc( &func->locn->tl );
        CErr1( msg );
    }
    if( flags & TF1_NEAR ) {
        if( CompFlags.zm_switch_used ) {
            if( IsBigCode() && !CompFlags.zmf_switch_used ) {
                SetErrLoc( &func->locn->tl );
                CErr1( WARN_CODE_MAY_BE_SPLIT_ACROSS_SEGS );
            }
        }
    }
    return( fn_type );
}

static void handleDefnChangesToSym( SYMBOL func )
{
    SCOPE scope;
    symbol_class stg_class;

    scope = SymScope( func );
    if( ScopeType( scope, SCOPE_FILE ) ) {
        switch( func->id ) {
        case SC_EXTERN:
        case SC_NULL:
            if( SymIsInline( func ) ) {
                stg_class = SC_STATIC;
            } else {
                stg_class = SC_PUBLIC;
                CompFlags.external_defn_found = true;
            }
            func->id = stg_class;
            break;
        }
    } else if( ScopeType( scope, SCOPE_CLASS ) ) {
        if( ! SymIsInline( func ) ) {
            CompFlags.external_defn_found = true;
        }
    }
}

static bool noPendingForwardGotos( FUNCTION_DATA *fdata )
{
    CSTACK *curr_cs;
    FNLABEL *curr_lbl;

    Stack_forall( fdata->control, curr_cs ) {
        switch( curr_cs->id ) {
        case CS_BLOCK:
        case CS_FUNCTION:
            break;
        default:
            return( false );
        }
    }
    RingIterBeg( fdata->labels, curr_lbl ) {
        if( ! curr_lbl->defined ) {
            /* goto LABEL; found but LABEL: has not been seen yet */
            return( false );
        }
    } RingIterEnd( curr_lbl )
    return( true );
}

void FunctionBody( DECL_INFO *dinfo )
/***********************************/
{
    MSG_NUM msg;
    SYMBOL func;
    FUNCTION_DATA fn_data;
    TYPE fn_type;
    SCOPE enclosing_scope;
    SCOPE parsing_scope;
    bool recog_token;
    SYMBOL previous_func;
    tc_fn_control fn_control;

    func = dinfo->sym;
    if( func == NULL || ! SymIsFunction( func ) ) {
        /* something went wrong declaring the symbol! */
        skipFunctionBody( 1 );
        return;
    }
    func = TemplateFunctionTranslate( func, dinfo->friend_fn, &parsing_scope );
    dinfo->sym = func;
    CtxFunction( func );
    fn_control = TemplateFunctionControl();
    if( fn_control & TCF_GEN_FUNCTION ) {
        func->flag |= SF_MUST_GEN;
    }
    if( SymIsInitialized( func ) ) {
        if( TemplateMemberCanBeIgnored() ) {
            /* member function has already been overridden */
            skipFunctionBody( 1 );
            return;
        }
        msg = ERR_FUNCTION_REDEFINITION;
        if( GeneratedDefaultFunction( func ) ) {
            msg = ERR_CANNOT_DEFINE_DEFAULT;
        }
        /*
         these errors deal with previously def'd functions so the
         location in the symbol isn't necessarily close to this defn
         hence SetErrLoc isn't used so that we get the current token
         location
        */
        badFunction( msg, func );
        return;
    }
    if( ! dinfo->explicit_parms ) {
        SetErrLoc( &func->locn->tl );
        badFunction( ERR_BAD_FUNCTION_TYPE, func );
        return;
    }
    if( !SegmentIfBasedOK( func ) ) {
        badFunctionSym( func );
        return;
    }
    fn_type = handleDefnChecks( func );
    handleDefnChangesToSym( func );
    previous_func = CgFrontCurrentFunction();

    enclosing_scope = GetCurrScope();
    SetCurrScope( parsing_scope );
    ScopeAdjustUsing( enclosing_scope, parsing_scope );

    initFunctionBody( dinfo, &fn_data, fn_type );
    // after initFunctionBody so .DEF files can have names in their prototypes
    MainProcSetup( func );
    for(;;) {
        recog_token = false;
        set_output( func );
        parseLabels();
        switch( CurToken ) {
        case T_EOF :
            while( fn_data.depth > 0 ) {
                if( fn_data.control->id == CS_BLOCK ) {
                    endBlock();
                }
                endControl();
            }
            break;
        case T_IF:
            parseIfStmt();
            continue;
        case T_WHILE:
            parseWhileStmt();
            continue;
        case T_DO:
            beginLoop( CS_DO );
            if( ( CurToken == T_RIGHT_BRACE ) || ( CurToken == T_ALT_RIGHT_BRACE ) ) {
                CErr1( ERR_STMT_REQUIRED_AFTER_DO );
                break;
            }
            continue;
        case T_FOR:
            parseForStmt();
            continue;
        case T_SWITCH:
            parseSwitchStmt();
            continue;
        case T_CASE:
            parseCaseStmt();
            continue;
        case T_DEFAULT:
            parseDefaultStmt();
            continue;
        case T_BREAK:
            parseBreakStmt();
            if( fn_data.control->id != CS_BLOCK ) break;
            continue;
        case T_CONTINUE:
            parseContinueStmt();
            if( fn_data.control->id != CS_BLOCK ) break;
            continue;
        case T_RETURN:
            parseReturnStmt( func );
            if( fn_data.control->id != CS_BLOCK ) break;
            continue;
        case T_GOTO:
            parseGotoStmt();
            if( fn_data.control->id != CS_BLOCK ) break;
            continue;
        case T_TRY:
            parseTryBlock();
            continue;
        case T_CATCH:
            parseCatchHandler();
            continue;
        case T___ASM:
            parseAsmStmt( fn_type );
            if( fn_data.control->id != CS_BLOCK ) break;
            continue;
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            startBlock();
            if( ( fn_data.control->next->id == CS_SWITCH )
              &&( fn_data.control->next->u.s.block_after ) ) {
                fn_data.control->u.b.block_switch = true;
            }
            nextYYToken();
            continue;
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            if( fn_data.control->id != CS_BLOCK ) {
                CErr1( ERR_MISPLACED_RIGHT_BRACE );
            }
            if( fn_data.depth > fn_data.outer_depth ) {
                recog_token = true;
            }
            set_output( func );
            break;
        case T_SEMI_COLON:
            if( fn_data.control->id != CS_BLOCK ) {
                recog_token = true;
                break;
            }
            nextYYToken();
            continue;
        default:
            declExprStmt( false );
            if( fn_data.control->id != CS_BLOCK ) break;
            continue;
        }
        recog_token = endOfStmt( recog_token );
        if( recog_token ) {
            nextYYToken();
        }
        if( fn_data.dead_code ) {
            if( noPendingForwardGotos( &fn_data ) ) {
                fn_data.always_dead_code = true;
            }
        }
        if( fn_data.depth == 0 ) break;
    }
    switch( returnIsRequired( fn_type ) ) {
      case RETN_REQUIRED :
        if( ! MainProcedure( func ) ) {
            CErr1( ERR_MISSING_RETURN_VALUE );
            break;
        }
        // drops thru, see 3.6.1 (5)
      case RETN_DFLT_INT :
        if( MainProcedure( func ) ) {
            PTREE expr;
            expr = AnalyseReturnExpr( func, NodeZero() );
            IcEmitExpr( expr );
        }
        // drops thru
      case RETN_NOT_REQD :
        CgFrontReturnSymbol( NULL );
        FunctionBodyDeadCode();
        break;
    }
    finiFunctionBody( func );

    SetCurrScope( enclosing_scope );
    ScopeAdjustUsing( parsing_scope, enclosing_scope );

    CgFrontResumeFunction( previous_func );
}

static void functionSuicideHandler( void )
{
    FUNCTION_DATA *curr;

    for( curr = currFunction; curr != NULL; curr = curr->next ) {
        LabelFiniFunc( &curr->label_mem );
    }
    // clear module functions's label memory
    LabelFiniFunc( NULL );
    currFunction = NULL;
}

static void functionInit( INITFINI* defn )
{
    defn = defn;
    functionSuicide.call_back = functionSuicideHandler;
    RegisterSuicideCallback( &functionSuicide );
    carveCSTACK = CarveCreate( sizeof( CSTACK ), BLOCK_CSTACK );
    carveSWCASE = CarveCreate( sizeof( SWCASE ), BLOCK_SWCASE );
    carveFNLABEL = CarveCreate( sizeof( FNLABEL ), BLOCK_FNLABEL );
    carveFNCATCH = CarveCreate( sizeof( FNCATCH ), BLOCK_FNCATCH );
}

static void functionFini( INITFINI* defn )
{
    defn = defn;
    DbgStmt( CarveVerifyAllGone( carveCSTACK, "CSTACK" ) );
    DbgStmt( CarveVerifyAllGone( carveSWCASE, "SWCASE" ) );
    DbgStmt( CarveVerifyAllGone( carveFNLABEL, "FNLABEL" ) );
    DbgStmt( CarveVerifyAllGone( carveFNCATCH, "FNCATCH" ) );
    CarveDestroy( carveCSTACK );
    CarveDestroy( carveSWCASE );
    CarveDestroy( carveFNLABEL );
    CarveDestroy( carveFNCATCH );
}


INITDEFN( functions, functionInit, functionFini )


bool DefargBeingCompiled(       // TEST IF DEFARG-FUNCTION BEING COMPILED
    void )
{
    return currFunction->is_defarg;
}


void FunctionHasRegistration(   // RECORD THAT FUNCTION NEEDS STATE TABLE
    void )
{
    currFunction->has_state_tab = true;
}


unsigned FunctionRegistrationFlag( // GET NEXT FUNCTION REGISTRATION FLAG
    void )
{
    FunctionHasRegistration();
    return ++currFunction->flag_count;
}


bool FunctionBodyCtor(          // TEST IF COMPILING CTOR
    void )
{
    return currFunction->is_ctor;
}


ACCESS_ERR** FunctionBodyAccessErrors( // POINT AT HDR OF ACCESS ERRORS
    void )
{
    return &currFunction->access_errs;
}


// called when throw, longjmp, or function with SF_LONGJUMP encountered
//
PTREE FunctionCouldThrow(       // INDICATE FUNCTION COULD THROW / HAS LONGJUMP
    PTREE expr )                // - expression
{
    currFunction->does_throw = true;
    return PtdThrow( expr );
}


void FunctionMightThrow(        // INDICATE THAT FUNCTION MIGHT THROW
    void )
{
    currFunction->can_throw = true;
}


PTREE FunctionCalled(           // RECORD A FUNCTION CALL
    PTREE expr,                 // - expression
    SYMBOL called )             // - called function
{
    symbol_flag called_flag     // - flags for caller
        = SymThrowFlags( called );

    if( called_flag & SF_LONGJUMP ) {
        expr = FunctionCouldThrow( expr );
    } else if( (called_flag & SF_NO_LONGJUMP) == 0 ) {
        currFunction->can_throw = true;
    }
    return expr;
}


void FunctionHasCtorTest(       // INDICATE THAT FUNCTION MIGHT HAVE CTOR-TEST
    void )
{
    if( ! currFunction->ctor_test ) {
        currFunction->ctor_test = true;
        FunctionRegistrationFlag();
    }
}


type_flag FunctionThisQualifier(// RETURN THIS QUALIFIER FLAGS
    void )
{
    type_flag flags;
    SYMBOL fn_sym;

    flags = TF1_NULL;
    fn_sym = ScopeFunctionInProgress();
    if( fn_sym != NULL ) {
        flags = FunctionThisMemFlags( fn_sym );
    }
    flags &= TF1_THIS_MASK;
    DbgAssert( flags == BaseTypeClassFlags( TypeThis() ) );
    return( flags );
}

void FunctionBodyDefargStartup(
    SYMBOL func,        // - function
    FUNCTION_DATA *f )  // - function data
{
    initFunctionData(func, f);
    f->is_defarg = true;
}

void FunctionBodyDefargShutdown(
    FUNCTION_DATA *f )          // - data
{
    f = f;
    DbgAssert( f->is_defarg == true );
    currFunction = currFunction->next;
}

