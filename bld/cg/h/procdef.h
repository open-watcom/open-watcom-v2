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


#include "targproc.h"

#define ROUTINE_LOADS_DS                  0x8000

typedef enum {
        GENERATED_PROLOG                = 0x01,
        GENERATED_EPILOG                = 0x02,
#include "targprol.h"
        _GENERATED__LAST                = 0x8000        // force to 2 bytes
} prolog_state_def;

typedef struct var_set {
        type_length     size;
        type_length     base;   /*  base offset from frame pointer */
} var_set;

typedef struct parm_state {
        hw_reg_set              used;
        hw_reg_set              *table;
        hw_reg_set              *curr_entry;
        type_length              offset;
#if _TARGET & _TARG_PPC
        uint_32                 gr;
        uint_32                 fr;
#endif
} parm_state;


typedef struct call_state {
        parm_state              parm;
        hw_reg_set              used;
        hw_reg_set              modify;
        hw_reg_set              unalterable;
        hw_reg_set              return_reg;
        call_attributes         attr;
        call_registers          regs;
} call_state;

typedef struct label_id {
        struct label_id         *next;
        label_handle            label;
        block_num               block_id;
} label_id;

typedef struct proc_def {
        target_proc_def         targ;
        struct var_set          parms;
        struct var_set          locals;
        struct call_state       state;
        struct block            *head_block;
        struct block            *tail_block;
        struct proc_def         *next_proc;
        union  name             *names[N_INDEXED+1];
        struct block            *curr_block;
        pointer                 frame_index;
        pointer                 label;
        int                     ins_id;
        bool                    block_by_block;
        bool                    untrimmed;
        bool                    contains_call;
        byte                    lex_level;
        prolog_state_def        prolog_state;
        union  name             *lasttemp;
        union  name             *dummy_index;
        instruction             *parms_list;
} proc_def;
