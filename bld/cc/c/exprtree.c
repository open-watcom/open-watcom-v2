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
* Description:  Expression tree utility routines.
*
****************************************************************************/


#include "cvars.h"


TREEPTR     ExprNodeList;
unsigned    NodeCount;

void InitExprTree( void )
{
    ExprNodeList = NULL;
}

static void AllocMoreExprNodes( void )
{
    TREEPTR     node;
    int         i;

    node = (TREEPTR)CMemAlloc( 500 * sizeof( EXPRNODE ) );
    ExprNodeList = node;
    for( i = 0; i < (500 - 1); i++ ) {
        node->left = node + 1;
        ++node;
    }
    node->left = NULL;
}

TREEPTR ExprNode( TREEPTR left, opr_code opcode, TREEPTR right )
{
    TREEPTR     node;

    if( ExprNodeList == NULL ) {
        AllocMoreExprNodes();
    }
    node = ExprNodeList;
    ExprNodeList = node->left;
    node->left = left;
    node->right = right;
    node->op.opr = opcode;
    node->op.flags = OPFLAG_NONE;
    node->visit = FALSE;
    node->checked = FALSE;
    ++NodeCount;
    return( node );
}

TREEPTR LeafNode( opr_code opr )
{
    return( ExprNode( NULL, opr, NULL ) );
}

void FreeExprNode( TREEPTR node )
{
    SWITCHPTR   sw;
    CASEPTR     cse;

    if( node != NULL ) {
        if( node->op.opr == OPR_PUSHFLOAT ) {
            CMemFree( node->op.u2.float_value );
        } else if( node->op.opr == OPR_SWITCH ) {
            sw = node->op.u2.switch_info;
            for( ; (cse = sw->case_list) != NULL; ) {
                sw->case_list = cse->next_case;
                CMemFree( cse );
            }
            CMemFree( sw );
        } else if ( node->op.opr == OPR_CALL ) {
            ChkCallNode( node );
        }
        --NodeCount;
        node->left = ExprNodeList;
        ExprNodeList = node;
    }
}

void FreeExprTree( TREEPTR root )
{
    TREEPTR     subtree;
    TREEPTR     node;

    while( root != NULL ) {
        subtree = root->right;
        root->right = NULL;
        while( (node = subtree) != NULL ) {
            subtree = node->left;
            node->left = root;
            root = node;
        }
        node = root;
        root = root->left;
        FreeExprNode( node );
    }
}
