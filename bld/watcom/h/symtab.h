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


typedef struct avl_node {
    struct avl_node     *left;
    struct avl_node     *right;
    void                *key;
    unsigned char       equal_subtrees : 1;
    unsigned char       left_heavy : 1;
} avl_node;

typedef struct symbol_table {
    int                 (*cmp)( void *key1, void *key2 );
    unsigned            height;
    avl_node            head;
} *symbol_table;

symbol_table SymInit( int (*cmp)( void *key1, void *key2 ) );
void SymAdd( symbol_table symtab, void *key );
void *SymFind( symbol_table symtab, void *key );
/*
    SymFini destroys the symbol_table and all the avl_nodes in the tree.
    If you want to free the key's then you must do it by SymWalking before
    SymFini.
*/
void SymFini( symbol_table symtab );
/*
    For SymWalk: If process returns -1 then abort the walking.
                 If process returns  0 then continue walking.
    SymWalk returns -1 if walking aborted, 0 otherwise.
    SymWalk does an in-order walk of the symbol table.
*/
int SymWalk( symbol_table symtab, void *parm,
    int (*process)( void *key, void *parm ) );

