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
* Description:  Dump internal C compiler data structures.
*
****************************************************************************/


#include "cvars.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "dumpapi.h"

static char *_Ops[] = {
    #define pick1(enum,dump,cgenum) dump,
    #include "copcodes.h"
    #undef pick1
};

char    *CCOps[] = {
    #define pick1(enum,dump,cgenum) dump,
    #include "copcond.h"
    #undef pick1
};

static void DumpAString( STR_HANDLE str_handle )
{
    target_size len;
    char        *p;

    printf( "\"" );
    for( p = str_handle->literal, len = str_handle->length;
      len > 0 && *p != '\0'; --len, ++p ) {
        if( *p == '\n' ) {
            printf( "\\n" );
        } else {
            printf( "%c", *p );
        }
    }
    printf( "\"" );
}

#ifdef _LONG_DOUBLE_
/* Dump a long_double value */
void DumpALD( long_double *pld )
{
    CVT_INFO    cvt;
    char        buf[256];

    cvt.ndigits = 20;
    cvt.scale = 0;
    cvt.flags = G_FMT | NO_TRUNC;
    cvt.expchar = 'e';
    cvt.expwidth = 8;
    __LDcvt( pld, &cvt, buf );
    printf( "%s", buf );
}

/* Dump a long double followed by newline */
void DumpALDNL( long_double *pld )
{
    DumpALD( pld );
    printf( "\n" );
}
#endif

static void DumpOpnd( TREEPTR opnd )
{
    SYM_ENTRY   sym;

    switch( opnd->op.opr ) {
    case OPR_ERROR:
        printf( "error" );
        break;
    case OPR_PUSHINT:
        printf( "%d", opnd->op.u2.long_value );
        break;
    case OPR_PUSHFLOAT:
#ifdef _LONG_DOUBLE_
        DumpALD( &opnd->op.u2.float_value->ld );
#else
        if( opnd->op.u2.float_value->len != 0 ) {
            printf( "%s", opnd->op.u2.float_value->string );
        } else {
            printf( "%g", opnd->op.u2.float_value->ld.u.value );
        }
#endif
        break;
    case OPR_PUSHSTRING:
        DumpAString( opnd->op.u2.string_handle );
        break;
    case OPR_PUSHSYM:
    case OPR_PUSHADDR:
    case OPR_FUNCNAME:
        SymGet( &sym, opnd->op.u2.sym_handle );
        printf( "%s", sym.name );
        break;
    case OPR_EXCEPT_CODE:
    case OPR_EXCEPT_INFO:
        printf( "%s ", _Ops[opnd->op.opr] );
        break;
    case OPR_NOP:
        break;
    default:
        printf( "opnd.opr=%d", opnd->op.opr );
        break;
    }
}

static void DumpPrefix( TREEPTR node )
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
        switch( node->op.u1.mathfunc ) {
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

static void DumpInfix( TREEPTR node )
{
    SYM_ENTRY   sym;

    switch( node->op.opr ) {
    case OPR_FUNCTION:
        SymGet( &sym, node->op.u2.func.sym_handle );
        printf( "function %s", sym.name );
        break;
    case OPR_LABELCOUNT:
        printf( "label count=%u", node->op.u2.label_count );
        break;
    case OPR_FUNCEND:
        printf( "funcend\n" );
        break;
    case OPR_LABEL:
    case OPR_JUMP:
    case OPR_JUMPTRUE:
    case OPR_JUMPFALSE:
        printf( "%s L%u ", _Ops[node->op.opr], node->op.u2.label_index );
        break;
    case OPR_CASE:
        printf( "%s %u (L%u)", _Ops[node->op.opr],
                node->op.u2.case_info->value, node->op.u2.case_info->label );
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
        printf( "%s", _Ops[node->op.opr] );
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
        printf( " %s ", CCOps[node->op.u1.cc] );
        break;
    case OPR_TRY:
        printf( "%s %d", _Ops[node->op.opr], node->op.u2.st.parent_scope );
        break;
    case OPR_UNWIND:
        printf( "%s %d", _Ops[node->op.opr], node->op.u2.st.u.try_index );
        break;
    case OPR_EXCEPT:
    case OPR_FINALLY:
        printf( "%s parent=%d", _Ops[node->op.opr], node->op.u2.st.parent_scope );
        break;
    default:
        printf( " " );
        // fall through
    case OPR_RETURN:
        printf( "%s ", _Ops[node->op.opr] );
        break;
    case OPR_POINTS:
        // already printed in prefix routine
        break;
    }
}

static void DumpPostfix( TREEPTR node )
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

void DumpExpr( TREEPTR tree )
{
    WalkExprTree( tree, DumpOpnd, DumpPrefix, DumpInfix, DumpPostfix );
    printf( "\n" );
}

static void DumpStmt( TREEPTR tree )
{
    printf( "line %3.3u: ", tree->op.u2.src_loc.line );
    WalkExprTree( tree->right, DumpOpnd, DumpPrefix, DumpInfix, DumpPostfix );
    printf( "\n" );
}

void DumpProgram( void )
{
    TREEPTR     tree;

    for( tree = FirstStmt; tree != NULL; tree = tree->left ) {
        if( tree->op.opr != OPR_STMT ) {
            printf( "expecting OPR_STMT node\n" );
            exit( 1 );
        }
        DumpStmt( tree );
    }
}

static void DumpDQuad(DATA_QUAD *dq, target_size size)
{
    target_size         size_of_item;
    target_size         amount;
    SYM_ENTRY           sym;

    if( dq->flags & Q_NEAR_POINTER ) {
        size_of_item = TARGET_NEAR_POINTER;
    } else if( dq->flags & Q_FAR_POINTER ) {
        size_of_item = TARGET_FAR_POINTER;
    } else if( dq->flags & Q_CODE_POINTER ) {
        size_of_item = TARGET_POINTER;
#if _CPU == 8086
        if( TargetSwitches & BIG_CODE ) {
            size_of_item = TARGET_FAR_POINTER;
        }
#endif
    } else {
        size_of_item = TARGET_POINTER;
#if _CPU == 8086
        if( TargetSwitches & BIG_DATA ) {
            size_of_item = TARGET_FAR_POINTER;
        }
#endif
    }
    switch( dq->type ) {
    case QDT_STATIC:
        SymGet( &sym, dq->u.var.sym_handle );
        printf( "0 bytes (QDT_STATIC): segment %d\n", sym.u.var.segid );
        break;
    case QDT_CHAR:
    case QDT_UCHAR:
    case QDT_BOOL:
        printf( "%6u byte char (%s): %d\n",
                size, dq->type == QDT_CHAR ? "QDT_CHAR" :
                dq->type == QDT_UCHAR ? "QDT_UCHAR" : "QDT_BOOL",
                dq->u.long_values[0] );
        break;
    case QDT_SHORT:
    case QDT_USHORT:
        printf( "%6u byte short (%s): %d\n",
                size, dq->type == QDT_SHORT ? "QDT_SHORT" :
                "QDT_UINT", dq->u.long_values[0] );
        break;
    case QDT_INT:
    case QDT_UINT:
        printf( "%6u byte int (%s): %d\n",
                size, dq->type == QDT_INT ? "QDT_INT" :
                "QDT_UINT", dq->u.long_values[0] );
        break;
    case QDT_LONG:
    case QDT_ULONG:
        printf( "%6u byte long (%s): %d\n",
                size, dq->type == QDT_LONG ? "QDT_LONG" :
                "QDT_ULONG", dq->u.long_values[0] );
        break;
    case QDT_LONG64:
    case QDT_ULONG64:
        printf( "%6u byte long long (%s)\n",
                size, dq->type == QDT_LONG64 ? "QDT_LONG64" :
                "QDT_ULONG64" );
        break;
    case QDT_FLOAT:
    case QDT_FIMAGINARY:
        printf( "%6u byte float (%s): %f\n",
                size, dq->type == QDT_FLOAT ? "QDT_FLOAT" :
                "QDT_IMAGINARY", dq->u.double_value );
        break;
    case QDT_DOUBLE:
    case QDT_DIMAGINARY:
        printf( "%6u byte double (%s): %f\n",
                size, dq->type == QDT_DOUBLE ? "QDT_DOUBLE" :
                "QDT_DIMAGINARY", dq->u.double_value );
        break;
    case QDT_LONG_DOUBLE:
    case QDT_LDIMAGINARY:
        printf( "%6u byte long double (%s)\n",
                size, dq->type == QDT_LONG_DOUBLE ? "QDT_LONG_DOUBLE" :
                "QDT_LDIMAGINARY" );
        break;
    case QDT_STRING:
        printf( "%6u byte string (QDT_STRING): \"%s\"\n",
                size_of_item, dq->u.string.handle->literal );
        break;
    case QDT_POINTER:
    case QDT_ID:
        printf( "%6u byte pointer (%s): offset %x\n",
                size_of_item, dq->type == QDT_POINTER ? "QDT_POINTER" :
                "QDT_ID", dq->u.var.offset );
        break;
    case QDT_CONST:
        printf( "%6u byte long literal (QDT_CONST): \"%.*s\"\n", size,
                size, dq->u.string.handle->literal + dq->u.string.offset );
        break;
    case QDT_CONSTANT:
        printf( "%6u zero bytes (QDT_CONSTANT)\n", size);
        break;
    default:
        assert( 0 );
    }
}

void DumpDataQuads( void )
{
    DATA_QUAD       *dq;
    void            *cookie;
    target_size     size;

    cookie = StartDataQuadAccess();
    if( cookie != NULL ) {
        size = 0;
        printf( "=== Data Quads ===\n" );
        while ((dq = NextDataQuad(&size)))
            DumpDQuad(dq, size);
        EndDataQuadAccess( cookie );
    }
}

