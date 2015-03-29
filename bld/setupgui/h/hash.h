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
* Description:  Function prototypes for hash table manipulation.
*
****************************************************************************/

#include "vhandle.h"

typedef void        *hash_handle;
typedef const char  *hash_key;

/* standard compare function return, 0 if equal, < 0 if s1 < s2, > 0 is s1 > s2
 */
typedef int     (*hash_key_cmp)( hash_key s1, hash_key s2 );

typedef struct hash_element_struct {
    struct hash_element_struct  *next;
    hash_key                    key;
    vhandle                     data;
} hash_element;

typedef struct hash_table_struct {
    size_t              size;
    hash_key_cmp        cmp_func;
    hash_element        *table[1];
} hash_table;

extern hash_table       *HashInit( size_t size, hash_key_cmp func );
extern bool             HashInsert( hash_table *ht, hash_key k, vhandle data );
extern vhandle          HashFind( hash_table *ht, hash_key k );
extern void             HashFini( hash_table *ht );
