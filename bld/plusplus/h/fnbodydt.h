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


#ifndef __FNBODYDT_H__
#define __FNBODYDT_H__

// FNBODYDT -- data used in FNBODY (exposed to other modules)
//
// 92/11/19 -- J.W.Welch        -- extracted from FNBODY

typedef struct access_err ACCESS_ERR;

typedef struct function_data FUNCTION_DATA;
struct function_data {
    FUNCTION_DATA       *next;
    SCOPE               fn_scope;       // SCOPE_FUNCTION for function
    depth_t             depth;          // amount of block nesting
    depth_t             outer_depth;    // amount of special outer block nesting
    blk_count_t         block_count;    // # of opened blocks
    unsigned            flag_count;     // # of flags used in registration
    CSTACK              *control;       // control structure stack
    FNLABEL             *labels;        // function's labels
    LAB_MEM             label_mem;      // memory for LABEL.C
    CGLABEL             end_dtor;       // label for end of dtor code
    CGFILE_INS          dtor_beg;       // IC_NO_OP before dtor code
    CGFILE_INS          init_state_tab; // IC_NO_OP zapped to IC_FUNCTION_STAB
    INIT_VAR            init_var;       // symbol info for static once only
    ACCESS_ERR*         access_errs;    // access errors
    SYMBOL              retn_opt_sym;   // SYMBOL for return optimization

    unsigned            is_ctor : 1;    // function is a constructor
    unsigned            is_dtor : 1;    // function is a destructor
    unsigned            is_assign : 1;  // function is a operator =
    unsigned            is_defarg : 1;  // function is def.arg compilation
    unsigned            dead_code : 1;  // set if unconditional jump output
    unsigned            dead_diagnosed:1;//dead code has been diagnosed
    unsigned            always_dead_code:1;// set if jump output with no pending lbls
    unsigned            ret_reqd : 1;   // a return with a value was performed
    unsigned            has_mem_init :1;// ctor had a mem-initializer
    unsigned            has_state_tab:1;// function requires state-table
    unsigned            can_throw    :1;// could throw or has longjump
    unsigned            does_throw   :1;// has throw or has longjump
    unsigned            ctor_test    :1;// might need CTOR_TEST opcode
    unsigned            retn_opt     :1;// can do return optimization
    unsigned            floating_ss  :1;// interrupt function

    char                filler[8];      // allow for growth during patches
};

#endif
