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
* Description:  Internal disassembler types.
*
****************************************************************************/


typedef enum {
    DHR_DONE,
    DHR_CONTINUE,
    DHR_INVALID
} dis_handler_return;

typedef struct {
    unsigned            name;
    dis_opcode          opcode;
    dis_opcode          mask;
    dis_handler_return  (*handler)( dis_handle *, void *, dis_dec_ins * );
} dis_ins_descript;

struct dis_range {
    unsigned_8  mask;
    unsigned_8  shift;
    unsigned_16 index;
};

typedef signed_16       dis_selector;

struct dis_cpu_data {
    const dis_range     *range;
    const int           *range_pos;
    void                (*preproc_hook)( dis_handle *, void *, dis_dec_ins * );
    dis_handler_return  (*decode_check)( int, dis_dec_ins * );
    size_t              (*ins_hook)( dis_handle *, void *, dis_dec_ins *, dis_format_flags, char *ins );
    size_t              (*flag_hook)( dis_handle *, void *, dis_dec_ins *, dis_format_flags, char *ins );
    size_t              (*op_hook)( dis_handle *, void *, dis_dec_ins *, dis_format_flags, unsigned op, char *op_buff, unsigned buff_len );
    size_t              (*post_op_hook)( dis_handle *, void *, dis_dec_ins *, dis_format_flags, unsigned op, char *op_buff, unsigned buff_len );
    const unsigned char *max_insnamep;
    unsigned char       inssize_inc;
};

#if DISCPU & DISCPU_axp
#include "disaxp.h"
#endif
#if DISCPU & DISCPU_ppc
#include "disppc.h"
#endif
#if DISCPU & DISCPU_x86
#include "disx86.h"
#endif
#if DISCPU & DISCPU_x64
#include "disx64.h"
#endif
#if DISCPU & DISCPU_jvm
#include "disjvm.h"
#endif
#if DISCPU & DISCPU_sparc
#include "dissparc.h"
#endif
#if DISCPU & DISCPU_mips
#include "dismips.h"
#endif

extern const dis_ins_descript   DisInstructionTable[];
extern const unsigned short     DisRefTypeTable[];
