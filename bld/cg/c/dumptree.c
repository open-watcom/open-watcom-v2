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


#include "standard.h"
#include "sysmacro.h"
#include "coderep.h"
#include "addrname.h"
#include "model.h"
#include "cgdefs.h"
#include "procdef.h"
#include "tree.h"
#include "dump.h"

extern  void            DumpNL();
extern  void            DumpOperand(name*);
extern  void            DumpInt(int);
extern  void            Dumpan(an);
extern  void            DumpClass(type_class_def);
extern  type_class_def  TypeClass(type_def*);
extern  void            DumpPtr( pointer );

static char * Ops[] = {
        "<O_NOP>",
        "<O_PLUS>",
        "<O_INTERNAL_01>",
        "<O_MINUS>",
        "<O_INTERNAL_02>",
        "<O_TIMES>",
        "<O_INTERNAL_03>",
        "<O_DIV>",
        "<O_MOD>",
        "<O_AND>",
        "<O_OR>",
        "<O_XOR>",
        "<O_RSHIFT>",
        "<O_LSHIFT>",
        "<O_POW>",
        "<O_P5DIV>",
        "<O_ATAN2>",
        "<O_FMOD>",
        "<O_UMINUS>",
        "<O_COMPLEMENT>",
        "<O_LOG>",
        "<O_COS>",
        "<O_SIN>",
        "<O_TAN>",
        "<O_SQRT>",
        "<O_FABS>",
        "<O_ACOS>",
        "<O_ASIN>",
        "<O_ATAN>",
        "<O_COSH>",
        "<O_EXP>",
        "<O_LOG10>",
        "<O_SINH>",
        "<O_TANH>",
        "<O_PTR_TO_NATIVE>",
        "<O_PTR_TO_FORIEGN>",
        "<O_SLACK_19>",
        "<O_CONVERT>",
        "<O_INTERNAL_05>",
        "<O_INTERNAL_06>",
        "<O_ROUND>",
        "<O_GETS>",
        "<O_INTERNAL_07>",
        "<O_INTERNAL_08>",
        "<O_INTERNAL_09>",
        "<O_INTERNAL_10>",
        "<O_INTERNAL_11>",
        "<O_INTERNAL_14>",
        "<O_INTERNAL_15>",
        "<O_EQ>",
        "<O_NE>",
        "<O_GT>",
        "<O_LE>",
        "<O_LT>",
        "<O_GE>",
        "<O_INTERNAL_12>",
        "<O_INTERNAL_17>",
        "<O_INTERNAL_18>",
        "<O_INTERNAL_19>",
        "<O_INTERNAL_20>",
        "<O_INTERNAL_21>",
        "<O_INTERNAL_22>",
        "<O_INTERNAL_23>",
        "<O_INTERNAL_24>",
        "<O_INTERNAL_25>",
        "<O_INTERNAL_26>",
        "<O_INTERNAL_27>",
        "<O_INTERNAL_28>",
        "<O_INTERNAL_29>",
        "<O_INTERNAL_30>",
        "<O_INTERNAL_31>",
        "<O_INTERNAL_32>",
        "<O_INTERNAL_33>",
        "<O_INTERNAL_34>",
        "<O_SLACK_29>",
        "<O_SLACK_30>",
        "<O_SLACK_31>",
        "<O_SLACK_32>",
        "<O_SLACK_33>",
        "<O_SLACK_34>",
        "<O_SLACK_35>",
        "<O_SLACK_36>",
        "<O_SLACK_37>",
        "<O_SLACK_38>",
        "<O_SLACK_39>",
        "<O_INTERNAL_13>",
        "<O_FLOW_AND>",
        "<O_FLOW_OR>",
        "<O_FLOW_OUT>",
        "<O_FLOW_NOT>",
        "<O_POINTS>",
        "<O_GOTO>",
        "<O_BIG_GOTO>",
        "<O_IF_TRUE>",
        "<O_IF_FALSE>",
        "<O_INVOKE_LABEL>",
        "<O_LABEL>",
        "<O_BIG_LABEL>",
        "<O_LABEL_RETURN>",
        "<O_PROC>",
        "<O_PARM_DEF>",
        "<O_AUTO_DEF>",
        "<O_COMMA>",
        "<O_PASS_PROC_PARM>",
        "<O_DEFN_PROC_PARM>",
        "<O_CALL_PROC_PARM>",
        "<O_PRE_GETS>",
        "<O_POST_GETS>",
        "<O_SIDE_EFFECT>",
        ""
};

static char * Null = { "" };
static char * LvEq = { "=(lv)" };
static char * Eq = { "=" };
static char * PostEq = { "=(post)" };
static char * Question = { "?" };
static char * Colon = { ":" };

extern  void    DumpTree( tn node ) {
/***********************************/

    DumpSubTree( node, 0 );
    DumpNL();
}


static  void    DumpSubTree( tn node, int indent ) {
/**************************************************/

    switch( node->class ) {
    case TN_LEAF:
        DumpIndent( indent );
        Dumpan( node->u.addr );
        break;
    case TN_CONS:
        DumpIndent( indent );
        DumpOperand( node->u.name );
        DumpNL();
        break;
    case TN_BINARY:
    case TN_COMPARE:
    case TN_COMMA:
    case TN_SIDE_EFFECT:
        DumpSubTree( node->u.left, indent+2 );
        DumpOpType( node, indent );
        DumpSubTree( node->rite, indent+2 );
        break;
    case TN_LV_ASSIGN:
        DumpSubTree( node->u.left, indent+2 );
        DumpStrType( node, LvEq, Null, indent );
        DumpSubTree( node->rite, indent+2 );
        break;
    case TN_ASSIGN:
        DumpSubTree( node->u.left, indent+2 );
        DumpStrType( node, Eq, Null, indent );
        DumpSubTree( node->rite, indent+2 );
        break;
    case TN_UNARY:
        DumpOpType( node, indent );
        DumpSubTree( node->u.left, indent+2 );
        break;
    case TN_PRE_GETS:
        DumpSubTree( node->u.left, indent+2 );
        DumpStrType( node, Ops[ node->op ], Eq, indent );
        DumpSubTree( node->rite, indent+2 );
        break;
    case TN_LV_PRE_GETS:
        DumpSubTree( node->u.left, indent+2 );
        DumpStrType( node, Ops[ node->op ], LvEq, indent );
        DumpSubTree( node->rite, indent+2 );
        break;
    case TN_POST_GETS:
        DumpSubTree( node->u.left, indent+2 );
        DumpStrType( node, Ops[ node->op ], PostEq, indent );
        DumpSubTree( node->rite, indent+2 );
        break;
    case TN_FLOW:
        if( node->rite != NULL ) {
            DumpSubTree( node->u.left, indent+2 );
            DumpOpType( node, indent );
            DumpSubTree( node->rite, indent+2 );
        } else {
            DumpOpType( node, indent );
            DumpSubTree( node->u.left, indent+2 );
        }
        break;
    case TN_CALL:
        DumpCall( node, indent );
        break;
    case TN_FLOW_OUT:
        DumpStrType( node, Ops[ O_FLOW_OUT ], Null, indent );
        DumpSubTree( node->u.left, indent+2 );
        break;
    case TN_QUESTION:
        DumpSubTree( node->u.left, indent+2 );
        DumpStrType( node, Question, Null, indent );
        DumpSubTree( node->rite->u.left, indent+2 );
        DumpStrType( node, Colon, Null, indent );
        DumpSubTree( node->rite->rite, indent+2 );
        break;
    case TN_BIT_LVALUE:
    case TN_BIT_RVALUE:
        DumpIndent( indent );
        if( node->class == TN_BIT_LVALUE ) {
            DumpLiteral( "O_BIT_LVALUE " );
        } else {
            DumpLiteral( "O_BIT_RVALUE " );
        }
        DumpInt( ((btn)node)->start );
        DumpLiteral( "for " );
        DumpInt( ((btn)node)->len );
        DumpNL();
        DumpSubTree( node->u.left, indent+2 );
        break;
    case TN_CALLBACK:
        DumpSubTree( node->rite, indent+2 );
        DumpIndent( indent );
        DumpLiteral( "CALLBACK" );
        DumpNL();
        DumpSubTree( node->u.left, indent+2 );
        break;
    case TN_PATCH:
        DumpIndent( indent );
        DumpLiteral( "O_PATCH_NODE (" );
        DumpPtr( node->u.handle );
        DumpLiteral( " )" );
        DumpNL();
        break;
    case TN_HANDLE:
        DumpIndent( indent );
        DumpLiteral( "HANDLE( " );
        DumpPtr( node->u.handle );
        DumpLiteral( " )" );
        DumpNL();
        break;
    }
}


static  void    DumpOpType( tn node, int indent ) {
/*************************************************/

    DumpStrType( node, Ops[ node->op ], Null, indent );
}

static  void    DumpStrType(tn node, char *s1, char *s2, int indent) {
/********************************************************************/

    DumpIndent( indent );
    DumpString( s1 );
    DumpString( s2 );
    DumpLiteral( " " );
    DumpClass( TypeClass( node->tipe ) );
    DumpNL();
}


static  void    DumpIndent( int i ) {
/***********************************/

    while( --i >= 0 ) {
        DumpLiteral( " " );
    }
}


static  void    DumpCall( tn what, int indent ) {
/***********************************************/

    tn  scan;

    DumpIndent( indent );
    DumpLiteral( "<O_CALL>" );
    DumpNL();
    DumpSubTree( what->u.left->u.left, indent+2 );
    scan = what->rite;
    while( scan != NULL ) {
        DumpIndent( indent );
        DumpLiteral( "<O_PARM>" );
        DumpNL();
        DumpSubTree( scan->u.left, indent+2 );
        scan = scan->rite;
    }
}
