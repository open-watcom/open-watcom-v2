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


// dump routines
#include "cvars.h"
#include "cgdefs.h"


static char *_Ops[] = {
    "+",
    "-",
    "*",
    "/",
    "-",
    "cmp",
    "%",
    "~",
    "!",
    "|",
    "&",
    "^",
    ">>",
    "<<",
    "=",
    "|=",
    "&=",
    "^=",
    ">>=",
    "<<=",
    "+=",
    "-=",
    "*=",
    "/=",
    "%=",
    "?",
    ":",
    "||",
    "&&",
    "*",
    "spare",
    "spare",
    "++",
    "--",
    "convert",
    "pushsym",
    "pushaddr",
    "pushint",
    "pushfloat",
    "pushstring",
    "convert_ptr",
    "nop",
    ".",
    "->",
    "[]",
    "&",
    ":>",
    "funcname",
    "call",
    "*call",
    ",",
    ",",
    "return",
    "label",
    "case",
    "jumptrue",
    "jumpfalse",
    "jump",
    "switch",
    "function",
    "funcend",
    "stmt",
    "{",
    "}",
    "_try",
    "_except",
    "_exception_code",
    "_exception_info",
    "unwind",
    "_finally",
    "_end_finally",
    "error",
    "cast",
    "label_count",
    "mathfunc",
    "mathfunc2",
    "vastart",
    "index2[]",
    "alloca",
    "patchnode",
    "inline_call",
    "temp_addr",
    "push_temp",
    "push_parm",
    "post ||",
    "side_effect",
    "&index",
};

char    *CCOps[] = { "==", "!=", "<", "<=", ">", ">=" };

void PageOutQuads() {}
void PageOutLeafs() {}
void GenQuad() {}
void WriteOutSegment() {}
void SetExpressCGInterface() {}

extern  TREEPTR FirstStmt;

static void DumpAString( STR_HANDLE str_handle )
{
    unsigned    len;
    char        *p;

    len = str_handle->length;
    printf( "\"" );
    p = &str_handle->literal[0];
    while( len != 0 ) {
        if( *p == '\0' ) break;
        if( *p == '\n' ) {
            printf( "\\n" );
        } else {
            printf( "%c", *p );
        }
        ++p;
        --len;
    }
    printf( "\"" );
}

void DumpOpnd( TREEPTR opnd )
{
    SYM_ENTRY   sym;

    switch( opnd->op.opr ) {
    case OPR_ERROR:
        printf( "error" );
        break;
    case OPR_PUSHINT:
        printf( "%ld", opnd->op.long_value );
        break;
    case OPR_PUSHFLOAT:
        printf( "%s", opnd->op.float_value->string );
        break;
    case OPR_PUSHSTRING:
        DumpAString( opnd->op.string_handle );
        break;
    case OPR_PUSHSYM:
    case OPR_PUSHADDR:
    case OPR_FUNCNAME:
        SymGet( &sym, opnd->op.sym_handle );
        printf( "%s", sym.name );
        break;
    case OPR_EXCEPT_CODE:
    case OPR_EXCEPT_INFO:
        printf( "%s ", _Ops[ opnd->op.opr ] );
        break;
    case OPR_NOP:
        break;
    default:
        printf( "opnd.opr=%d", opnd->op.opr );
        break;
    }
}

void DumpPrefix( TREEPTR node )
{
    TREEPTR     op1;

    switch( node->op.opr ) {
    case OPR_PARM:
        op1 = node->left;
        if( op1 != NULL ) {
            node->left = node->right;
            node->right = op1;
        }
        break;
    case OPR_MATHFUNC:
    case OPR_MATHFUNC2:
        switch( node->op.mathfunc ) {
        case O_LOG:     printf( "__LOG(" );     break;
        case O_COS:     printf( "__COS(" );     break;
        case O_SIN:     printf( "__SIN(" );     break;
        case O_TAN:     printf( "__TAN(" );     break;
        case O_SQRT:    printf( "__SQRT(" );    break;
        case O_FABS:    printf( "__FABS(" );    break;
        case O_POW:     printf( "__POW(" );     break;
        case O_ATAN2:   printf( "__ATAN2(" );   break;
        case O_FMOD:    printf( "__FMOD(" );    break;
        case O_ACOS:    printf( "__ACOS(" );    break;
        case O_ASIN:    printf( "__ASIN(" );    break;
        case O_ATAN:    printf( "__ATAN(" );    break;
        case O_COSH:    printf( "__COSH(" );    break;
        case O_EXP:     printf( "__EXP(" );     break;
        case O_LOG10:   printf( "__LOG10(" );   break;
        case O_SINH:    printf( "__SINH(" );    break;
        case O_TANH:    printf( "__TANH(" );    break;
        }
        break;
    case OPR_VASTART:
        printf( "va_start(" );
        break;
    case OPR_POINTS:
        printf( "*" );
        break;
    case OPR_COMMA:
        printf( "(" );
        break;
    }
}

void DumpInfix( TREEPTR node )
{
    SYM_ENTRY   sym;

    switch( node->op.opr ) {
    case OPR_FUNCTION:
        SymGet( &sym, node->op.func.sym_handle );
        printf( "function %s", sym.name );
        break;
    case OPR_LABELCOUNT:
        printf( "label count=%u", node->op.label_count );
        break;
    case OPR_FUNCEND:
        printf( "funcend\n" );
        break;
    case OPR_LABEL:
    case OPR_CASE:
    case OPR_JUMP:
    case OPR_JUMPTRUE:
    case OPR_JUMPFALSE:
        printf( "%s L%u ", _Ops[ node->op.opr ], node->op.label_index );
        break;
    case OPR_INDEX:
        printf( "[" );
        break;
    case OPR_CALL:
        printf( "(" );
        break;
    case OPR_COM:
    case OPR_NOT:
    case OPR_ADDROF:
        printf( "%s", _Ops[ node->op.opr ] );
        break;
    case OPR_MATHFUNC:
    case OPR_MATHFUNC2:
    case OPR_VASTART:
    case OPR_PARM:
        if( node->left != NULL ) {
            printf( ", " );
        }
        break;
    case OPR_CMP:
        printf( " %s ", CCOps[ node->op.cc ] );
        break;
    case OPR_TRY:
        printf( "%s %d", _Ops[ node->op.opr ], node->op.parent_scope );
        break;
    case OPR_UNWIND:
        printf( "%s %d", _Ops[ node->op.opr ], node->op.try_index );
        break;
    case OPR_EXCEPT:
    case OPR_FINALLY:
        printf( "%s parent=%d", _Ops[ node->op.opr ], node->op.parent_scope );
        break;
    default:
        printf( " " );
        // fall through
    case OPR_RETURN:
        printf( "%s ", _Ops[ node->op.opr ] );
        break;
    case OPR_POINTS:
        // already printed in prefix routine
        break;
    }
}

void DumpPostfix( TREEPTR node )
{
    TREEPTR     op1;

    switch( node->op.opr ) {
    case OPR_INDEX:
        printf( "]" );
        break;
    case OPR_CALL:
    case OPR_MATHFUNC:
    case OPR_MATHFUNC2:
    case OPR_COMMA:
    case OPR_VASTART:
        printf( ")" );
        break;
    case OPR_PARM:
        op1 = node->left;
        if( op1 != NULL ) {
            node->left = node->right;
            node->right = op1;
        }
        break;
    }
}

void DumpStmt( TREEPTR tree )
{
    printf( "line %3.3u: ", tree->srclinenum );
    WalkExprTree( tree->right, DumpOpnd, DumpPrefix, DumpInfix, DumpPostfix );
    printf( "\n" );
}

void DumpProgram()
{
    TREEPTR     tree;

    tree = FirstStmt;
    while( tree != NULL ) {
        if( tree->op.opr != OPR_STMT ) {
            printf( "expecting OPR_STMT node\n" );
            exit( 1 );
        }
        DumpStmt( tree );
        tree = tree->left;
    }
}
