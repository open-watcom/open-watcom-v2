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


#include "wpch.hpp"
#include "avltree.hpp"

static Pool AvlTreeBase::AvlNode::avlPool( AvlLinkPool, sizeof(AvlNode), 128 );
static Pool AvlTreeBase::_stackPool( AvlStackPool, sizeof(AvlStack), 2 );


AvlTreeBase::AvlTreeBase()
/************************/
{
    _current = _head = NULL;
    _stack = NULL;
    _numEntries = 0;
}


AvlTreeBase::~AvlTreeBase()
/*************************/
{
    Clear();
    if( _stack != NULL ){
        _stackPool.Release( _stack );
    }
}


void AvlTreeBase::Clear()
/***********************/
{
    AvlNode     *toDelete = NULL;
    AvlNode     *current;
    int         sp;

    if( _head == NULL ){
        return;
    }
    current = _head;
    if( _stack == NULL ){
        _stack = (AvlStack *) _stackPool.Get();
    }
    sp = 0;
    for(;;) {
        if( current->left != NULL ) {
            _stack->array[sp] = current;
            ++sp;
            WAssert( sp < AVL_STACK_SIZE );
            current = current->left;
            continue;
        }
    left_subtree_is_done:
        if( current->right != NULL ) {
            _stack->array[sp] = current;
            ++sp;
            WAssert( sp < AVL_STACK_SIZE );
            current = current->right;
            continue;
        }
    right_subtree_is_done:
        delete current;
        if( sp == 0 ) break;
        --sp;
        if( current == (_stack->array[sp])->left ) {
            current = _stack->array[sp];
            goto left_subtree_is_done;
        } else {
            current = _stack->array[sp];
            goto right_subtree_is_done;
        }
    }

    _head = NULL;
    _current = NULL;
    _numEntries = 0;
    _stackPool.Release( _stack );
    _stack = NULL;

    return;
}


void *AvlTreeBase::Remove( uint_32 key )
/**************************************/
{
    void        *result;
    AvlNode     *search, *prev;
    AvlNode     *child;
    AvlNode     *temp;

    result = NULL;
    prev = NULL;
    search = _head;
    for( ;; ){
        if( search == NULL ){
            break;
        } else if( search->key < key ){
            prev = search;
            search = search->right;
        } else if( search->key > key ){
            prev = search;
            search = search->left;
        } else {
            result = search->data;
            if( search->left == NULL ){
                child = search->right;
            } else if( search->right == NULL ){
                child = search->left;
            } else {
                temp = NULL;
                child = search->right;
                while( child->left != NULL ){
                    temp = child;
                    child = child->left;
                }
                child->left = search->left;
                if( temp != NULL ){
                    temp->left= child->right;
                    child->right = search->right;
                }
            }
            if( prev == NULL ){
                _head = child;
            } else if( prev->left == search ){
                prev->left = child;
            } else {
                prev->right = child;
            }
            delete search;
            break;
        }
    }
    return result;
}


void *AvlTreeBase::Find( uint_32 key )
/********************************/
{
    AvlNode *search;

    search = _head;
    for( ;; ){
        if( search == NULL ){
            return NULL;
        } else if( key == search->key ){
            break;
        } else if( key < search->key ) {
            search = search->left;
        } else {
            search = search->right;
        }
    }
    return search->data;
}


void *AvlTreeBase::FindLeft( uint_32 key )
/************************************/
{
    AvlNode     *search;
    void        *result;

    search = _head;
    result = NULL;
    for( ;; ){
        if( search == NULL ){
            return result;
        } else if( key == search->key ){
            break;
        } else if( key < search->key ){
            search = search->left;
        } else {
            result = search->data;
            search = search->right;
        }
    }
    return search->data;
}


void *AvlTreeBase::FindRight( uint_32 key )
/************************************/
{
    AvlNode     *search;
    void        *result;

    search = _head;
    result = NULL;
    for( ;; ){
        if( search == NULL ){
            return result;
        } else if( key == search->key ){
            break;
        } else if( key < search->key ){
            result = search->data;
            search = search->left;
        } else {
            search = search->right;
        }
    }
    return search->data;
}


WBool AvlTreeBase::Insert( uint_32 key, void *data, WBool replace )
/*****************************************************************/
{
    AvlNode *search;            /* P */
    AvlNode *adjust;            /* P */
    AvlNode *balance;           /* P */
    AvlNode *father;            /* T */
    AvlNode *currBalance;       /* S */
    AvlNode *underBalance;      /* R */
    AvlNode *next;              /* Q */
    AvlNode *newNode;           /* Q */

    if( _head == NULL ){
        newNode = new AvlNode;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->key = key;
        newNode->data = data;
        newNode->equalSubtrees = 1;
        _head = newNode;
        goto done;
    }

    /* algorithm is from Knuth Vol. 3 (2nd edition) p.445 */
    father = NULL;
    balance = _head;
    search = _head;
    for(;;) {
        if( key == search->key ) {
            if( replace ){
                search->data = data;
                return TRUE;
            } else {
                return FALSE;
            }
        } else if( key < search->key ) {
            next = search->left;
            if( next == NULL ) {
                newNode = new AvlNode;
                search->left = newNode;
                break;
            }
        } else {
            next = search->right;
            if( next == NULL ) {
                newNode = new AvlNode;
                search->right = newNode;
                break;
            }
        }
        if( ! next->equalSubtrees ) {
            father = search;
            balance = next;
        }
        search = next;
    }
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->key = key;
    newNode->data = data;
    newNode->equalSubtrees = 1;
    if( key < balance->key ) {
        underBalance = balance->left;
    } else {
        underBalance = balance->right;
    }
    adjust = underBalance;
    while( adjust != newNode ) {
        adjust->equalSubtrees = 0;
        if( key < adjust->key ) {
            adjust->leftHeavy = 1;
            adjust = adjust->left;
        } else {
            adjust->leftHeavy = 0;
            adjust = adjust->right;
        }
    }
    if( key < balance->key ) {
        if( balance->equalSubtrees ) {
            balance->equalSubtrees = 0;
            balance->leftHeavy = 1;
            goto done;
        }
        if( balance->leftHeavy ) {
            if( underBalance->leftHeavy ) {
                /* single rotation */
                currBalance = underBalance;
                balance->left = underBalance->right;
                underBalance->right = balance;
                balance->equalSubtrees = 1;
                underBalance->equalSubtrees = 1;
            } else {
                /* double rotation */
                currBalance = underBalance->right;
                underBalance->right = currBalance->left;
                currBalance->left = underBalance;
                balance->left = currBalance->right;
                currBalance->right = balance;
                if( currBalance->equalSubtrees ) {
                    balance->equalSubtrees = 1;
                    underBalance->equalSubtrees = 1;
                } else if( currBalance->leftHeavy ) {
                    balance->equalSubtrees = 0;
                    balance->leftHeavy = 0;
                    underBalance->equalSubtrees = 1;
                } else {
                    balance->equalSubtrees = 1;
                    underBalance->equalSubtrees = 0;
                    underBalance->leftHeavy = 1;
                }
                currBalance->equalSubtrees = 1;
            }
        } else {
            balance->equalSubtrees = 1;
            goto done;
        }
    } else {
        if( balance->equalSubtrees ) {
            balance->equalSubtrees = 0;
            balance->leftHeavy = 0;
            goto done;
        }
        if( ! balance->leftHeavy ) {
            if( ! underBalance->leftHeavy ) {
                /* single rotation */
                currBalance = underBalance;
                balance->right = underBalance->left;
                underBalance->left = balance;
                balance->equalSubtrees = 1;
                underBalance->equalSubtrees = 1;
            } else {
                /* double rotation */
                currBalance = underBalance->left;
                underBalance->left = currBalance->right;
                currBalance->right = underBalance;
                balance->right = currBalance->left;
                currBalance->left = balance;
                if( currBalance->equalSubtrees ) {
                    balance->equalSubtrees = 1;
                    underBalance->equalSubtrees = 1;
                } else if( ! currBalance->leftHeavy ) {
                    balance->equalSubtrees = 0;
                    balance->leftHeavy = 1;
                    underBalance->equalSubtrees = 1;
                } else {
                    balance->equalSubtrees = 1;
                    underBalance->equalSubtrees = 0;
                    underBalance->leftHeavy = 0;
                }
                currBalance->equalSubtrees = 1;
            }
        } else {
            balance->equalSubtrees = 1;
            goto done;
        }
    }
    if( father == NULL ){
        _head = currBalance;
    } else if( balance == father->right ) {
        father->right = currBalance;
    } else {
        father->left = currBalance;
    }
done:
    _numEntries++;
    return TRUE;
}

void *AvlTreeBase::First()
/************************/
{
    if( _head == NULL ){
        return NULL;
    }

    if( _stack == NULL ){
        _stack = (AvlStack *) _stackPool.Get();
    }

    _sp = 0;
    _current = _head;
    while( _current->left != NULL ){
        _stack->array[_sp] = _current;
        ++_sp;
        WAssert( _sp < AVL_STACK_SIZE );
        _current = _current->left;
    }
    return _current->data;
}


void *AvlTreeBase::Next()
/***********************/
{
    if( _current == NULL ){
        return NULL;
    }
    if( _current->right == NULL ) {
        if( _sp == 0 ) {
            _stackPool.Release( _stack );
            _stack = NULL;
            return NULL;
        }
        --_sp;
        _current = _stack->array[_sp];
        return _current->data;
    }
    _current = _current->right;
    while( _current->left != NULL ) {
        _stack->array[_sp] = _current;
        ++_sp;
        WAssert( _sp < AVL_STACK_SIZE );
        _current = _current->left;
    }
    return _current->data;
}
