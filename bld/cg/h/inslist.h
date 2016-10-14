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
* Description:  Instruction lists data structures.
*
****************************************************************************/


#include <limits.h>
#include "pattern.h"


#define G(x)    (x)->u.gen_table->generate

/* aligned */
typedef int                     instruction_id;
#define MAX_INS_ID              INT_MAX

typedef unsigned_32             source_line_number;

typedef enum {
        INS_NEEDS_WORK,
        OPERANDS_NEED_WORK,
        INS_READY
} instruction_state;

typedef enum {
        CALL_READS_NO_MEMORY    = 0x01,
        CALL_WRITES_NO_MEMORY   = 0x02,
        CALL_IGNORES_RETURN     = 0x04,
        #include "targcall.h"
} call_flags;

typedef enum {
        INS_VISITED             = 0x01,
        INS_MARKED              = 0x02,
        INS_DEMOTED             = 0x04,
        INS_PARAMETER           = 0x08,
        INS_PROMOTED            = 0x10,
        INS_CC_USED             = 0x20,
        INS_RISCIFIED           = 0x40,
        INS_CODE_POINTER        = 0x80,
        INS_INDEX_ADJUST        = INS_CODE_POINTER,     /* used by scheduler */
        INS_SPLIT               = INS_PROMOTED,         /* used by FixMem16Moves() */
} instruction_flags;

typedef enum {
        NOP_DBGINFO             = 0x01,
        NOP_DBGINFO_START       = 0x02,
        NOP_SOURCE_QUEUE        = 0x04,
        NOP_ZAP_INFO            = 0x08,
} nop_flags;

#define FIRST_INS_ID    ((instruction_id)0)
#define LAST_INS_ID     ((instruction_id)0xFFFF)

typedef struct name_set {
        hw_reg_set              regs;
        global_bit_set          out_of_block;
        local_bit_set           within_block;
} name_set;

#define _NameSetInit( x ) \
    { \
        HW_CAsgn( x.regs, HW_EMPTY ); \
        _LBitInit( x.within_block, EMPTY ); \
        _GBitInit( x.out_of_block, EMPTY ); \
    }

typedef struct ins_header {
        struct instruction      *prev;
        struct instruction      *next;
        struct name_set         live;
        source_line_number      line_num;
        opcode_defs             opcode;
        instruction_state       state;
} ins_header;

typedef struct instruction {
        struct ins_header       head;
        opcode_entry            *table;
        union {
            opcode_entry        *gen_table;     /*  do not merge this one! */
        } u;
        union {
            struct instruction  *parm_list;
            struct instruction  *cse_link;
        } u2;
        struct register_name    *zap;
        union name              *result;        /*  result location */
        instruction_id          id;
        type_class_def          type_class;
        type_class_def          base_type_class;
        unsigned_16             sequence;
        union {
                byte            byte;
                bool            bool_flag;
                call_flags      call_flags;
                nop_flags       nop_flags;
                byte            zap_value;      /* for conversions on AXP */
        }                       flags;
        union {
            byte                index_needs;    /*  a.k.a. reg_set_index */
            byte                stk_max;
        } t;
        byte                    stk_entry;
        byte                    num_operands;
        byte                    stk_exit;
        union {
            byte                stk_extra;
            byte                stk_depth;
        }                       s;
        instruction_flags       ins_flags;
        union name               *operands[1]; /*  operands */
} instruction;

#define CALL_OP_USED            0
#define CALL_OP_USED2           1
#define CALL_OP_POPS            1
#define CALL_OP_ADDR            2

#define NO_JUMP         2
#define INS_PER_BLOCK   75
#define MAX_OPS_PER_INS 4
#define INS_SIZE        ( offsetof( instruction, operands ) + MAX_OPS_PER_INS * sizeof( name * ) )

#define _TrueIndex( i )              ( (i)->flags.byte & 0x0f )
#define _FalseIndex( i )             ( ( (i)->flags.byte & 0xf0 ) >> 4 )
#define _SetBlockIndex( i, t, f )    (i)->flags.byte = (t) | ( (f) << 4 )

#define _IsConvert( ins )            ( (ins)->head.opcode == OP_CONVERT || \
                                       (ins)->head.opcode == OP_ROUND )

#define _OpClass( ins )              ( _IsConvert( ins ) \
                                       ? (ins)->base_type_class \
                                       : (ins)->type_class )

#define _INS_NOT_BLOCK( ins )        if ( (ins)->head.opcode == OP_BLOCK ) Zoiks( ZOIKS_142 )

typedef enum {
    MB_FALSE    = false,
    MB_TRUE     = true,
    MB_MAYBE    = 2
} bool_maybe;
