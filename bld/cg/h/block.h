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


#include "dominati.h"

#define _IsBlkAttr( b, a )      ( ((b)->class & (a)) != 0 )
#define _MarkBlkAttrNot( b, a ) ( (b)->class &= ~(a) )
#define _MarkBlkAttr( b, a )    ( (b)->class |=  (a) )
#define _SetBlkAttr( b, a )     ( (b)->class  =  (a) )

#define _IsBlkVisited( b )      _IsBlkAttr( b, BLK_BLOCK_VISITED )
#define _MarkBlkUnVisited( b )  _MarkBlkAttrNot( b, BLK_BLOCK_VISITED )
#define _MarkBlkVisited( b )    _MarkBlkAttr( b, BLK_BLOCK_VISITED )

#define _IsBlkMarked( b )       _IsBlkAttr( b, BLK_BLOCK_MARKED )
#define _MarkBlkUnMarked( b )   _MarkBlkAttrNot( b, BLK_BLOCK_MARKED )
#define _MarkBlkMarked( b )     _MarkBlkAttr( b, BLK_BLOCK_MARKED )

/* aligned */
#define MAX_INTERVAL_DEPTH      255U

typedef unsigned_32             interval_depth;

typedef unsigned_32             block_flags;

typedef enum {
        BLOCK_LABEL_DIES        = 0x01,
        DEST_LABEL_DIES         = 0x02,
        SOURCE_IS_PREHEADER     = 0x04,
        DEST_IS_BLOCK           = 0x08,
        DOWN_ONE_CALL           = 0x10,
        ONE_ITER_EXIT           = 0x20,
        DEST_IS_HEADER          = 0x40
} block_flags_consts;

typedef enum {
        BLK_RETURN                  = 0x00000001,
        BLK_JUMP                    = 0x00000002,
        BLK_CONDITIONAL             = 0x00000004,
        BLK_SELECT                  = 0x00000008,

        BLK_ITERATIONS_KNOWN        = 0x00000010,
        BLK_BIG_LABEL               = 0x00000020,
        BLK_CALL_LABEL              = 0x00000040,
        BLK_LABEL_RETURN            = 0x00000080,

        BLK_LOOP_HEADER             = 0x00000100,
        BLK_IN_LOOP                 = 0x00000200,
        BLK_LOOP_EXIT               = 0x00000400,
        BLK_BLOCK_VISITED           = 0x00000800,

        BLK_RETURNED_TO             = 0x00001000,
        BLK_UNKNOWN_DESTINATION     = 0x00002000,
        BLK_BLOCK_MARKED            = 0x00004000,
        BLK_MULTIPLE_EXITS          = 0x00008000,

        BLK_DONT_UNROLL             = 0x00010000,
        BLK_IGNORE                  = 0x00020000,
        BLK_FLOODED                 = 0x00040000,

        BLK_BIG_JUMP                = 0x00000000    /* no longer supported */
} block_class;


typedef struct block            *block_pointer;

typedef struct block_edge {
        struct destination {
            union {
                block_pointer   blk;
                label_handle    lbl;
            } u;
        }                       destination;    /* target */
        struct block            *source;        /* source of edge */
        struct block_edge       *next_source;   /* next source to same target */
        interval_depth          join_level;     /* interval levels joined */
        block_flags             flags;
} block_edge;

typedef struct interval_def {
        struct interval_def     *parent;
        struct block            *first_block;
        struct block            *last_block;
        struct interval_def     *sub_int;
        struct interval_def     *next_sub_int;
        struct interval_def     *link;
        interval_depth          level;
} interval_def;

typedef struct data_flow_def {
        global_bit_set  in;
        global_bit_set  out;
        global_bit_set  def;
        global_bit_set  use;
        global_bit_set  call_exempt;
        global_bit_set  need_load;
        global_bit_set  need_store;
} data_flow_def;

typedef struct block_ins {
        struct ins_header       hd;
        struct block            *blk;
} block_ins;

#define _BLOCK( ins ) ( ( (block_ins *)ins)->blk )

typedef struct block {
        struct block_ins        ins;
        struct block            *next_block;    /* used for DFS */
        struct block            *prev_block;
        union {
            struct interval_def *interval;
            struct block        *partition;
            struct block        *loop;
        } u;
        struct block            *loop_head;
        struct data_flow_def    *dataflow;
        struct block_edge       *input_edges;
        pointer                 cc;             /* AKA cc_control */
        dominator_info          dom;            /* least node in dominator set */
        type_length             stack_depth;    /* set by FlowSave stuff */
        union {
            struct block        *alter_ego;     /* used in loop unrolling */
            struct block        *next;          /* used for CALL_LABEL kludge */
        } v;
        label_handle            label;          /* front end identification */
        local_bit_set           available_bit;
        interval_depth          depth;          /* loop nesting depth */
        block_num               id;             /* internal identification */
        block_num               gen_id;
        block_num               inputs;         /* number of input edges */
        block_num               targets;        /* number of target blocks */
        block_class             class;
        signed_32               iterations;
        unsigned_32             unroll_count;
        struct block_edge       edge[ 1 ];
} block;
