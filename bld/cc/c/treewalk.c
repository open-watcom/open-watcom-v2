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
* Description:  Expression tree walker.
*
****************************************************************************/


#include "cvars.h"


unsigned        NodeTimeStamp;


int WalkExprTree( TREEPTR p,
                void (*operand)( TREEPTR ),
                void (*prefix_operator)( TREEPTR ),
                void (*infix_operator)( TREEPTR ),
                void (*postfix_operator)( TREEPTR ) )
{
    TREEPTR     parent;
    TREEPTR     temp;

    NodeTimeStamp = 0;
    parent = NULL;
    for( ;; ) {
        for( ;; ) {
            if( p->left == 0 && p->right == 0 ) break;
            ++NodeTimeStamp;
            (*prefix_operator)( p );
            if( p->left == 0 ) break;
            temp = p;
            p = p->left;
            temp->left = parent;
            temp->visit = TRUE;
            parent = temp;
        }
        if( parent == NULL ) {
            ++NodeTimeStamp;
            (*prefix_operator)( p );
        } else if( p->right == NULL ) {
            ++NodeTimeStamp;
            (*operand)( p );
            goto move_up_to_parent;
        }
        for( ;; ) {
            ++NodeTimeStamp;
            (*infix_operator)( p );     /* - perform infix operation */
            if( p->right != NULL ) {    /* - if present */
                temp = p;
                p = p->right;           /* - - get right */
                temp->right = parent;
                parent = temp;
                break;                  /* - - get out to visit right */
            }                           /* - endif */
do_postfix_call:
            ++NodeTimeStamp;
            (*postfix_operator)( p );   /* perform postfix operation */
            if( parent == NULL )        /* quit if at root node */
                return( 0 );
move_up_to_parent:
            if( parent->visit ) {
                parent->visit = FALSE;
                temp = parent;
                parent = temp->left;
                temp->left = p;
                p = temp;
            } else {
                temp = parent;
                parent = temp->right;
                temp->right = p;
                p = temp;
                // already visited left and right
                goto do_postfix_call;
            }
        }
    }
}

void NoOp( TREEPTR node )
{
    node = node;
}
