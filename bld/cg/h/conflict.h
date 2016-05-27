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
* Description:  Definitions for conflict manager.
*
****************************************************************************/


/* aligned */
#define MAX_SAVE                0xFFFFFFFF
#define DEL_CONFLICT_SAVE       10

typedef unsigned_32             save_def;

typedef enum {
    CST_CONF_IS_NEXT            = 0x0001,
    CST_SAVINGS_JUST_CALCULATED = 0x0002,
    CST_CONFLICT_ON_HOLD        = 0x0004,
    CST_SAVINGS_CALCULATED      = 0x0008,

    CST_CANNOT_SPLIT            = 0x0010,
    CST_CHANGES_OTHERS          = 0x0020,
    CST_NEEDS_INDEX             = 0x0040,
    CST_NEEDS_INDEX_SPLIT       = 0x0080,

    CST_INDEX_SPLIT             = 0x0100,
    CST_NEEDS_SEGMENT           = 0x0200,
    CST_NEEDS_SEGMENT_SPLIT     = 0x0400,
    CST_SEGMENT_SPLIT           = 0x0800,

    CST_WAS_SEGMENT             = 0x1000,
    CST_OK_ACROSS_CALLS         = 0x2000,
    CST_CONF_VISITED            = 0x4000,
    CST_NEVER_TOO_GREEDY        = 0x8000
} conflict_state;

#define CST_PERMANENT_FLAGS (CST_NEEDS_INDEX+CST_INDEX_SPLIT+CST_CONFLICT_ON_HOLD\
                            +CST_NEVER_TOO_GREEDY+CST_OK_ACROSS_CALLS\
                            +CST_NEEDS_SEGMENT+CST_SEGMENT_SPLIT)

#define CST_VALID_SEGMENT   (CST_NEEDS_SEGMENT+CST_NEEDS_SEGMENT_SPLIT+\
                            CST_SEGMENT_SPLIT+CST_WAS_SEGMENT)

typedef struct reg_tree {
        struct reg_tree         *lo;
        struct reg_tree         *hi;
        union name              *temp;
        union name              *alt;
        hw_reg_set              *regs;
        hw_reg_set              chosen;
        type_length             size;
        type_length             offset;
        byte                    idx; /* aka reg_set_index */
        bool                    has_name;
} reg_tree;

typedef struct instruction_range {
        struct instruction      *first;
        struct instruction      *last;
} instruction_range;

typedef struct conflict_id {
        local_bit_set           within_block;
        global_bit_set          out_of_block;
} conflict_id;

typedef struct possible_for_alias {
        struct possible_for_alias  *next;
        name                       *temp;
        byte                       possible;        /*  aka reg_set_index */
} possible_for_alias;

typedef struct conflict_node {  /*  target independent */
        struct conflict_node    *next_conflict;
        struct conflict_node    *next_for_name;
        union  name             *name;
        struct block            *start_block;
        struct reg_tree         *tree;
        save_def                savings;
        int                     num_constrained;     /* constrained conflicts */
        int                     available;           /*  regs available */
        struct name_set         with;
        struct instruction_range ins_range;
        conflict_id             id;
        conflict_state          state;
        byte                    possible;            /*  aka reg_set_index */
        possible_for_alias      *possible_for_alias_list;
} conflict_node;

#define _SetTrue( node, bit )   node->state |= (bit)
#define _SetFalse( node, bit )  node->state &= ~(bit)

#define _Is( node, bit )        ((node->state & (bit)) != 0)
#define _Isnt( node, bit )      ((node->state & (bit)) == 0)
