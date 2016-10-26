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
* Description:  ORL hash table definition.
*
****************************************************************************/


#ifndef ORL_HASH_DEF_INCLUDED
#define ORL_HASH_DEF_INCLUDED

/* hash table definitions */
typedef enum {
    ORL_HASH_STRING,
    ORL_HASH_NUMBER
} orl_hash_table_type;

typedef uint_32     orl_hash_value;
typedef uint_32     orl_hash_table_size;
typedef void        *orl_hash_data;

typedef int         (*orl_hash_comparison_func)( orl_hash_data, orl_hash_data );

struct orl_hash_data_struct {
    orl_hash_data               data;
    struct orl_hash_data_struct *next;
};

typedef struct orl_hash_data_struct orl_hash_data_struct;

struct orl_hash_struct {
    orl_hash_data               key;
    orl_hash_data_struct        *data_struct;
    struct orl_hash_struct      *next;
};

typedef struct orl_hash_struct  orl_hash_struct;

struct orl_hash_table_struct {
    orl_hash_table_type         type;
    orl_hash_table_size         size;
    orl_funcs                   *funcs;
    orl_hash_comparison_func    compare;
    orl_hash_struct             **table;
};

typedef struct orl_hash_table_struct orl_hash_table_struct;
typedef orl_hash_table_struct   *orl_hash_table;

#endif
