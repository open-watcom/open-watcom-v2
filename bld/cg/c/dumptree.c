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
* Description:  Dump expression tree.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "addrname.h"
#include "model.h"
#include "cgdefs.h"
#include "procdef.h"
#include "tree.h"
#include "dumpio.h"

extern  void            DumpOperand(name*);
extern  void            Dumpan(an);
extern  void            DumpClass(type_class_def);

extern  type_class_def  TypeClass(type_def*);

static  void            DumpSubTree( tn node, int indent );

static char * Ops[] = {
    #define STR(x) #x
    #define PICK(e,i,d1,d2,ot,pnum,attr)  STR(<O_##e>),
    #include "cgops.h"
    #undef PICK
    #undef STR
};

static char * Null = { "" };
static char * LvEq = { "=(lv)" };
static char * Eq = { "=" };
static char * PostEq = { "=(post)" };
static char * Question = { "?" };
static char * Colon = { ":" };

static  void    DumpIndent( int i ) {
/***********************************/

    while( --i >= 0 ) {
        DumpChar( ' ' );
    }
}


static  void    DumpStrType(tn node, const char *s1, const char *s2, int indent) {
/********************************************************************************/

    DumpIndent( indent );
    DumpString( s1 );
    DumpString( s2 );
    DumpChar( ' ' );
    DumpClass( TypeClass( node->tipe ) );
    DumpNL();
}


static  void    DumpOpType( tn node, int indent ) {
/*************************************************/

    DumpStrType( node, Ops[ node->op ], Null, indent );
}


static  void    DumpCall( tn what, int indent ) {
/***********************************************/

    tn  scan;

    DumpIndent( indent );
    DumpLiteral( "<O_CALL>" );
    DumpNL();
    DumpSubTree( what->u.left->u.left, indent+2 );
    for( scan = what->rite; scan != NULL; scan = scan->rite ) {
        DumpIndent( indent );
        DumpLiteral( "<O_PARM>" );
        DumpNL();
        DumpSubTree( scan->u.left, indent+2 );
    }
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


extern  void    DumpTree( tn node ) {
/***********************************/

    DumpSubTree( node, 0 );
    DumpNL();
}
