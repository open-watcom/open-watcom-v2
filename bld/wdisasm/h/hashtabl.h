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


#include "watcom.h"
#include "standard.h"

typedef uint_32         hash_value;
typedef uint_32         hash_table_size;
typedef uint_32         hash_data;

typedef enum {
    HASH_STRING,
    HASH_NUMBER
} hash_table_type;

typedef
    int         (*hash_table_comparison_func)( hash_value, hash_value );

struct hash_struct {
    hash_value                  key;
    hash_data                   data;
    struct hash_struct *        next;
};

typedef struct hash_struct hash_struct;

struct hash_table_struct {
    hash_table_size             size;
    hash_table_type             type;
    hash_table_comparison_func  compare;
    hash_struct **              table;
};

typedef struct hash_table_struct hash_table_struct;
typedef hash_table_struct * hash_table;

extern bool HashTableInsert( hash_table hash_tbl, hash_value key, hash_data data );
extern hash_data *HashTableQuery( hash_table hash_tbl, hash_value key );
extern hash_table HashTableCreate( hash_table_size size, hash_table_type type, hash_table_comparison_func func );
extern void HashTableFree( hash_table hash_tbl );
