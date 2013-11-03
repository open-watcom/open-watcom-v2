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


#ifndef __TREE__H__
#define __TREE__H__

#include "useinfo.h"

/* aligned */
typedef enum {
        TF_VOLATILE     = 0x01,
        TF_USED         = 0x02,
        TF_DEMOTED      = 0x04,
        TF_REVERSE      = 0x08,
        TF_CONSTANT     = 0x10,
        TF_UNALIGNED    = 0x20,
        TF_HAS_LEFT     = 0x40,
} tn_flags;


typedef enum {
        TN_LEAF,
        TN_UNARY,
        TN_BINARY,
        TN_COMPARE,
        TN_ASSIGN,
        TN_LV_ASSIGN,
        TN_FLOW,
        TN_PRE_GETS,
        TN_LV_PRE_GETS,
        TN_POST_GETS,
        TN_PARM,
        TN_CALL,
        TN_COMMA,
        TN_FLOW_OUT,
        TN_QUESTION,
        TN_COLON,
        TN_BIT_LVALUE,
        TN_BIT_RVALUE,
        TN_CONS,
        TN_WARP,
        TN_CALLBACK,
        TN_HANDLE,
        TN_PATCH,
        TN_NUMBER_OF_CLASSES,
        TN_SIDE_EFFECT
} tn_class;

typedef struct tree_node {
#ifndef NDEBUG
        use_info                useinfo;
#endif
        struct type_def         *tipe;
        union {
            struct tree_node    *left;
            union name          *name;
            struct address_name *addr;
            void                *handle;
            cg_callback         callback;
        } u;
        uint                    kids;
        tn_class                class;
        tn_flags                flags;
        cg_op                   op;
        struct tree_node        *rite;
        struct type_def         *optipe;
        name                    *base;
        uint                    alignment;
} tree_node;

typedef struct bit_tree_node {
#ifndef NDEBUG
        use_info                useinfo;
#endif
        struct type_def         *tipe;
        union {
          struct tree_node      *left;
          union name            *name;
          struct address_name   *addr;
        } u;
        uint                    kids;
        tn_class                class;
        tn_flags                flags;
        struct type_def         *optipe;
        byte                    start;
        byte                    len;
        bool                    is_signed;
} bit_tree_node;

typedef struct bit_tree_node    *btn;
typedef struct tree_node        *tn;
typedef union  either_tree_node *atn;

typedef union either_tree_node {
        tree_node       t;
        bit_tree_node   b;
} either_tree_node;

typedef union tn_btn {
        tn      t;
        btn     b;
} tn_btn;
#endif // __TREE__H__
