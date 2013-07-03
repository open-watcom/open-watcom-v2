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

#ifdef _LONG_DOUBLE_
/* Dump a long_double value */
void DumpALD( long_double *pld )
{
    CVT_INFO    cvt;
    char        buf[256];

    cvt.ndigits  = 20;
    cvt.scale    = 0;
    cvt.flags    = G_FMT | NO_TRUNC;
    cvt.expchar  = 'e';
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

void DumpOpnd( TREEPTR opnd )
{
    SYM_ENTRY   sym;

    switch( opnd->op.opr ) {
    case OPR_ERROR:
        printf( "error" );
        break;
    case OPR_PUSHINT:
        printf( "%ld", opnd->op.u2.long_value );
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

void DumpInfix( TREEPTR node )
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
        printf( "%s L%u ", _Ops[ node->op.opr ], node->op.u2.label_index );
        break;
    case OPR_CASE:
        printf( "%s %u (L%u)", _Ops[ node->op.opr ], 
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
        printf( " %s ", CCOps[ node->op.u1.cc ] );
        break;
    case OPR_TRY:
        printf( "%s %d", _Ops[ node->op.opr ], node->op.u2.st.parent_scope );
        break;
    case OPR_UNWIND:
        printf( "%s %d", _Ops[ node->op.opr ], node->op.u2.st.u.try_index );
        break;
    case OPR_EXCEPT:
    case OPR_FINALLY:
        printf( "%s parent=%d", _Ops[ node->op.opr ], node->op.u2.st.parent_scope );
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

void DumpExpr( TREEPTR tree )
{
    WalkExprTree( tree, DumpOpnd, DumpPrefix, DumpInfix, DumpPostfix );
    printf( "\n" );
}

void DumpStmt( TREEPTR tree )
{
    printf( "line %3.3u: ", tree->op.u2.src_loc.line );
    WalkExprTree( tree->right, DumpOpnd, DumpPrefix, DumpInfix, DumpPostfix );
    printf( "\n" );
}

void DumpProgram( void )
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

static void DumpDQuad( DATA_QUAD *dq, unsigned long *psize )
{
    cg_type             data_type;
    int                 size_of_item;
    unsigned long       amount;
    SYM_ENTRY           sym;

    if( dq->flags & Q_NEAR_POINTER ) {
        data_type = TY_NEAR_POINTER;
        size_of_item = TARGET_NEAR_POINTER;
    } else if( dq->flags & Q_FAR_POINTER ) {
        data_type = TY_LONG_POINTER;
        size_of_item = TARGET_FAR_POINTER;
    } else if( dq->flags & Q_CODE_POINTER ) {
        data_type = TY_CODE_PTR;
        size_of_item = TARGET_POINTER;
#if _CPU == 8086
        if( TargetSwitches & BIG_CODE ) {
            size_of_item = TARGET_FAR_POINTER;
        }
#endif
    } else {
        data_type = TY_POINTER;
        size_of_item = TARGET_POINTER;
#if _CPU == 8086
        if( TargetSwitches & BIG_DATA ) {
            size_of_item = TARGET_FAR_POINTER;
        }
#endif
    }
    switch( dq->type ) {
    case QDT_STATIC:
        printf( "%6lu bytes (QDT_STATIC): segment %d\n",
                0, sym.u.var.segment );
        *psize = 0;
        break;
    case QDT_CHAR:
    case QDT_UCHAR:
    case QDT_BOOL:
        amount = sizeof( char );
        printf( "%6lu byte char (%s): %d\n",
                amount, dq->type == QDT_CHAR ? "QDT_CHAR" :
                dq->type == QDT_UCHAR ? "QDT_UCHAR" : "QDT_BOOL",
                dq->u.long_values[0] );
        *psize += amount;
        if( dq->flags & Q_2_INTS_IN_ONE ) {
            printf( "%6lu byte second char: %d\n",
                    dq->u.long_values[1] );
            *psize += amount;
        }
        break;
    case QDT_SHORT:
    case QDT_USHORT:
        amount = TARGET_SHORT;
        printf( "%6lu byte short (%s): %d\n",
                amount, dq->type == QDT_SHORT ? "QDT_SHORT" :
                "QDT_UINT", dq->u.long_values[0] );
        *psize += amount;
        if( dq->flags & Q_2_INTS_IN_ONE ) {
            printf( "%6lu byte second short: %d\n",
                    dq->u.long_values[1] );
            *psize += amount;
        }
        break;
    case QDT_INT:
    case QDT_UINT:
        amount = TARGET_INT;
        printf( "%6lu byte int (%s): %d\n",
                amount, dq->type == QDT_INT ? "QDT_INT" :
                "QDT_UINT", dq->u.long_values[0] );
        *psize += amount;
        if( dq->flags & Q_2_INTS_IN_ONE ) {
            printf( "%6lu byte second int: %d\n",
                    dq->u.long_values[1] );
            *psize += amount;
        }
        break;
    case QDT_LONG:
    case QDT_ULONG:
        amount = TARGET_LONG;
        printf( "%6lu byte long (%s): %d\n",
                amount, dq->type == QDT_LONG ? "QDT_LONG" :
                "QDT_ULONG", dq->u.long_values[0] );
        *psize += amount;
        if( dq->flags & Q_2_INTS_IN_ONE ) {
            printf( "%6lu byte second long: %d\n",
                    dq->u.long_values[1] );
            *psize += amount;
        }
        break;
    case QDT_LONG64:
    case QDT_ULONG64:
        amount = TARGET_LONG64;
        printf( "%6lu byte long long (%s)\n",
                amount, dq->type == QDT_LONG64 ? "QDT_LONG64" :
                "QDT_ULONG64" );
        *psize += amount;
        break;
    case QDT_FLOAT:
    case QDT_FIMAGINARY:
        amount = TARGET_FLOAT;
        printf( "%6lu byte float (%s): %f\n",
                amount, dq->type == QDT_FLOAT ? "QDT_FLOAT" :
                "QDT_IMAGINARY", dq->u.double_value );
        *psize += amount;
        break;
    case QDT_DOUBLE:
    case QDT_DIMAGINARY:
        amount = TARGET_DOUBLE;
        printf( "%6lu byte double (%s): %f\n",
                amount, dq->type == QDT_DOUBLE ? "QDT_DOUBLE" :
                "QDT_DIMAGINARY", dq->u.double_value );
        *psize += amount;
        break;
    case QDT_LONG_DOUBLE:
    case QDT_LDIMAGINARY:
        amount = TARGET_LDOUBLE;
        printf( "%6lu byte long double (%s)\n",
                amount, dq->type == QDT_DOUBLE ? "QDT_LONG_DOUBLE" :
                "QDT_LDIMAGINARY" );
        *psize += amount;
        break;
    case QDT_STRING:
        amount = size_of_item;
        printf( "%6lu byte string (QDT_STRING): \"%s\"\n", amount,
                dq->u.string_leaf->literal );
        *psize += amount;
        break;
    case QDT_POINTER:
    case QDT_ID:
        amount = size_of_item;
        printf( "%6lu byte pointer (%s): offset %x\n",
                amount, dq->type == QDT_POINTER ? "QDT_POINTER" :
                "QDT_ID", dq->u.var.offset );
        *psize += amount;
        break;
    case QDT_CONST:
        amount = dq->u.string_leaf->length;
        printf( "%6lu byte long literal (QDT_CONST): \"%s\"\n", amount,
                dq->u.string_leaf->literal );
        *psize += amount;
        break;
    case QDT_CONSTANT:
        amount = dq->u.long_values[0];
        printf( "%6lu zero bytes (QDT_CONSTANT)\n", amount );
        *psize += amount;
        break;
    default:
        assert( 0 );
    }
}

void DumpDataQuads( void )
{
    DATA_QUAD       *dq;
    void            *cookie;
    unsigned long   size;

    cookie = StartDataQuadAccess();
    if( cookie != NULL ) {
        size = 0;
        printf( "=== Data Quads ===\n" );
        for( ;; ) {
            dq = NextDataQuad();
            if( dq == NULL )
                break;
            for( ;; ) {
                DumpDQuad( dq, &size );
                if( ! (dq->flags & Q_REPEATED_DATA) )
                    break;
                dq->u.long_values[1]--;
                if( dq->u.long_values[1] == 0 )
                    break;
            }
        }
        EndDataQuadAccess( cookie );
    }
}
