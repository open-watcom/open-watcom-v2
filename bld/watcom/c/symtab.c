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


#include <stddef.h>
#include <stdlib.h>
#include "walloca.h"
#include "symtab.h"

symbol_table SymInit( int (*cmp)( void *key1, void *key2 ) )
/**********************************************************/
{
    symbol_table new;

    new = malloc( sizeof( * new ) );
    new->head.left = NULL;
    new->head.right = NULL;
    new->head.equal_subtrees = 0;
    new->head.left_heavy = 0;
    new->head.key = NULL;
    new->height = 0;
    new->cmp = cmp;

    return( new );
}

void SymFini( symbol_table symtab )
/*********************************/
{
    avl_node *p;
    avl_node *p_right;
    avl_node **stack;
    avl_node **sp;

    p = symtab->head.right;
    if( p != NULL ) {
        stack = alloca( symtab->height * sizeof( avl_node * ) );
        sp = stack;
        for(;;) {
            /* process tree rooted at 'p' */
            if( p->left ) {
                *sp = p;
                ++sp;
                p = p->left;
                continue;
            }
        left_subtree_is_done:
            p_right = p->right; /* save right pointer past free call */
            free( p );
            if( p_right ) {
                p = p_right;
                continue;
            }
            if( sp == stack ) break;
            --sp;
            p = *sp;
            goto left_subtree_is_done;
        }
    }
    free( symtab );
}

void *SymFind( symbol_table symtab, void *key )
/*********************************************/
{
    avl_node *curr_search;
    avl_node *next;
    int comp;

    curr_search = symtab->head.right;
    for(;;) {
        if( curr_search == NULL ) {
            return( NULL );
        }
        comp = symtab->cmp( key, curr_search->key );
        if( comp == 0 ) {
            return( curr_search->key );
        } else if( comp < 0 ) {
            next = curr_search->left;
        } else {
            next = curr_search->right;
        }
        curr_search = next;
    }
}

void SymAdd( symbol_table symtab, void *key )
/*******************************************/
{
    avl_node *curr_search;      /* P */
    avl_node *curr_adjust;      /* P */
    avl_node *curr_balance;     /* P */
    avl_node *father;           /* T */
    avl_node *balance;          /* S */
    avl_node *under_balance;    /* R */
    avl_node *next;             /* Q */
    avl_node *new;              /* Q */
    int comp;

    /* algorithm is from Knuth Vol. 3 (2nd edition) p.445 */
    father = &(symtab->head);
    balance = symtab->head.right;
    curr_search = symtab->head.right;
    if( curr_search == NULL ) {
        new = malloc( sizeof( avl_node ) );
        new->left = NULL;
        new->right = NULL;
        new->key = key;
        new->equal_subtrees = 1;
        symtab->head.right = new;
        return;
    }
    for(;;) {
        comp = symtab->cmp( key, curr_search->key );
        if( comp == 0 ) {
            return;
        } else if( comp > 0 ) {
            next = curr_search->right;
            if( next == NULL ) {
                new = malloc( sizeof( avl_node ) );
                curr_search->right = new;
                break;
            }
        } else {
            next = curr_search->left;
            if( next == NULL ) {
                new = malloc( sizeof( avl_node ) );
                curr_search->left = new;
                break;
            }
        }
        if( ! next->equal_subtrees ) {
            father = curr_search;
            balance = next;
        }
        curr_search = next;
    }
    new->left = NULL;
    new->right = NULL;
    new->key = key;
    new->equal_subtrees = 1;
    if( symtab->cmp( key, balance->key ) < 0 ) {
        under_balance = balance->left;
    } else {
        under_balance = balance->right;
    }
    curr_adjust = under_balance;
    while( curr_adjust != new ) {
        curr_adjust->equal_subtrees = 0;
        if( symtab->cmp( key, curr_adjust->key ) < 0 ) {
            curr_adjust->left_heavy = 1;
            curr_adjust = curr_adjust->left;
        } else {
            curr_adjust->left_heavy = 0;
            curr_adjust = curr_adjust->right;
        }
    }
    if( symtab->cmp( key, balance->key ) < 0 ) {
        if( balance->equal_subtrees ) {
            balance->equal_subtrees = 0;
            balance->left_heavy = 1;
            symtab->height++;
            return;
        }
        if( balance->left_heavy ) {
            if( under_balance->left_heavy ) {
                /* single rotation */
                curr_balance = under_balance;
                balance->left = under_balance->right;
                under_balance->right = balance;
                balance->equal_subtrees = 1;
                under_balance->equal_subtrees = 1;
            } else {
                /* double rotation */
                curr_balance = under_balance->right;
                under_balance->right = curr_balance->left;
                curr_balance->left = under_balance;
                balance->left = curr_balance->right;
                curr_balance->right = balance;
                if( curr_balance->equal_subtrees ) {
                    balance->equal_subtrees = 1;
                    under_balance->equal_subtrees = 1;
                } else if( curr_balance->left_heavy ) {
                    balance->equal_subtrees = 0;
                    balance->left_heavy = 0;
                    under_balance->equal_subtrees = 1;
                } else {
                    balance->equal_subtrees = 1;
                    under_balance->equal_subtrees = 0;
                    under_balance->left_heavy = 1;
                }
                curr_balance->equal_subtrees = 1;
            }
        } else {
            balance->equal_subtrees = 1;
            return;
        }
    } else {
        if( balance->equal_subtrees ) {
            balance->equal_subtrees = 0;
            balance->left_heavy = 0;
            symtab->height++;
            return;
        }
        if( ! balance->left_heavy ) {
            if( ! under_balance->left_heavy ) {
                /* single rotation */
                curr_balance = under_balance;
                balance->right = under_balance->left;
                under_balance->left = balance;
                balance->equal_subtrees = 1;
                under_balance->equal_subtrees = 1;
            } else {
                /* double rotation */
                curr_balance = under_balance->left;
                under_balance->left = curr_balance->right;
                curr_balance->right = under_balance;
                balance->right = curr_balance->left;
                curr_balance->left = balance;
                if( curr_balance->equal_subtrees ) {
                    balance->equal_subtrees = 1;
                    under_balance->equal_subtrees = 1;
                } else if( ! curr_balance->left_heavy ) {
                    balance->equal_subtrees = 0;
                    balance->left_heavy = 1;
                    under_balance->equal_subtrees = 1;
                } else {
                    balance->equal_subtrees = 1;
                    under_balance->equal_subtrees = 0;
                    under_balance->left_heavy = 0;
                }
                curr_balance->equal_subtrees = 1;
            }
        } else {
            balance->equal_subtrees = 1;
            return;
        }
    }
    if( balance == father->right ) {
        father->right = curr_balance;
    } else {
        father->left = curr_balance;
    }
    return;
}

int SymWalk( symbol_table symtab, void *parm,
    int (*process)( void *key1, void *parm ) )
/********************************************/
{
    avl_node *p;
    avl_node **stack;
    avl_node **sp;
    int      ret_code;

    p = symtab->head.right;
    if( p == NULL ) {   /* 16-Oct-90 DJG */
        return( 0 );
    }
    stack = alloca( symtab->height * sizeof( avl_node * ) );
    sp = stack;
    for(;;) {
        /* print tree rooted at 'p' */
        if( p->left ) {
            *sp = p;
            ++sp;
            p = p->left;
            continue;
        }
    left_subtree_is_done:
        ret_code = process( p->key, parm );
        if( ret_code != 0 ) {
            return( ret_code );
        }
        if( p->right ) {
            p = p->right;
            continue;
        }
        if( sp == stack ) break;
        --sp;
        p = *sp;
        goto left_subtree_is_done;
    }
    return( 0 );
}
