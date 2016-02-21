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
* Description:  MAD (Machine Architecture Description) types.
*
****************************************************************************/


#ifndef MADTYPES_H_INCLUDED
#define MADTYPES_H_INCLUDED

#include "madstr.h"
#include "digtypes.h"
#include "digpck.h"


#define SIGNTYPE_SIZE(x)        (-(int)(x))

struct mad_reg_set_data;
struct mad_state_data;
struct mad_disasm_data;
struct mad_trace_data;
union  mad_registers;

typedef unsigned_16             mad_string;
typedef unsigned_16             mad_type_handle;
typedef struct mad_reg_set_data mad_reg_set_data;
typedef struct mad_state_data   mad_state_data;
typedef struct mad_disasm_data  mad_disasm_data;
typedef struct mad_trace_data   mad_trace_data;
typedef struct mad_call_up_data mad_call_up_data;
typedef union  mad_registers    mad_registers;
typedef unsigned_16             mad_radix;

#define MAD_NIL_TYPE_HANDLE ((mad_type_handle)-1)

typedef unsigned_16 mad_status; enum {
    MS_OK,
    MS_FAIL,
    MS_UNSUPPORTED,
    MS_MODIFIED,
    MS_MODIFIED_SIGNIFICANTLY,
    MS_UNREGISTERED_MAD,
    MS_INVALID_MAD,
    MS_INVALID_MAD_VERSION,
    MS_INVALID_STATE,
    MS_FOPEN_FAILED,
    MS_FREAD_FAILED,
    MS_FSEEK_FAILED,
    MS_NO_MEM,
    MS_LAST,
    MS_ERR      = 0x4000
};

typedef unsigned_8 mad_address_format; enum {
    MAF_OFFSET,
    MAF_FULL
};

typedef unsigned_8 mad_type_kind; enum {
    MTK_BASIC   = 0x00,
    MTK_INTEGER = 0x01,
    MTK_ADDRESS = 0x02,
    MTK_FLOAT   = 0x04,
    MTK_CUSTOM  = 0x08,
    MTK_XMM     = 0x10,
    MTK_ALL     = MTK_INTEGER|MTK_ADDRESS|MTK_FLOAT|MTK_CUSTOM|MTK_XMM,

    MAS_MEMORY  = 0x40,
    MAS_IO      = 0x80,
    MAS_ALL     = (MAS_MEMORY|MAS_IO)
};

typedef unsigned_8 mad_numeric_representation; enum {
    MNR_UNSIGNED,
    MNR_TWOS_COMP,
    MNR_ONES_COMP,
    MNR_SIGN_MAG
};

typedef unsigned_8 mad_endianness; enum {
    ME_LITTLE,
    ME_BIG
};

#define MAD_DEFAULT_HANDLING    0

typedef struct {
    mad_type_kind       kind;
    unsigned_8          handler_code;
    dig_size_bits       bits;
} mad_type_info_basic;

typedef struct {
    mad_type_info_basic         b;
    dig_size_bits               sign_pos;
    mad_numeric_representation  nr;
    mad_endianness              endian;
} mad_type_info_integer;

typedef struct {
    mad_type_info_integer       i;
    struct {
        unsigned_8      pos;
        unsigned_8      bits;
    }                           seg;
} mad_type_info_address;

typedef struct {
    mad_type_info_integer               mantissa;
    struct {
        unsigned                bias   : 16;
        unsigned                pos    : 8;
        unsigned                base   : 7;
        unsigned                hidden : 1;
        mad_type_info_integer   data;
    }                                   exp;
} mad_type_info_float;

typedef union {
    mad_type_info_basic         b;
    mad_type_info_integer       i;
    mad_type_info_address       a;
    mad_type_info_float         f;
} mad_type_info;


typedef struct {
    const char                  *name;
    mad_type_handle             type;
    dig_size_bits               bit_start;
    unsigned_8                  bit_size;
    unsigned_8                  flags;
} mad_reg_info;

typedef unsigned_8 mad_special_reg; enum {
    MSR_IP,
    MSR_SP,
    MSR_FP
};

typedef struct {
    const void          *data;
    mad_type_handle     type;
    mad_string          name;
} mad_modify_list;

typedef unsigned_8 mad_disasm_piece; enum {
    MDP_INSTRUCTION     = 0x1,
    MDP_OPERANDS        = 0x2,
    MDP_ALL             = MDP_INSTRUCTION|MDP_OPERANDS
};

typedef unsigned_8 mad_disasm_control; enum {
    MDC_TAKEN_NOT       = 0x00,
    MDC_TAKEN_BACK      = 0x01,
    MDC_TAKEN_FORWARD   = 0x02,
    MDC_TAKEN           = 0x03,
    MDC_TAKEN_MASK      = 0x03,

    MDC_OPER            = 0x00,
    MDC_JUMP            = 0x10,
    MDC_CALL            = 0x20,
    MDC_SYSCALL         = 0x30,
    MDC_RET             = 0x40,
    MDC_SYSRET          = 0x50,
    MDC_TYPE_MASK       = 0x70,

    MDC_UNCONDITIONAL   = 0x00,
    MDC_CONDITIONAL     = 0x80,
    MDC_CONDITIONAL_MASK= 0x80
};

typedef unsigned_8 mad_memref_kind; enum {
    MMK_READ            = 0x01,
    MMK_WRITE           = 0x02,
    MMK_IMPLICIT        = 0x04,
    MMK_VOLATILE        = 0x08
};

typedef unsigned_8 mad_trace_kind; enum {
    MTRK_INTO,
    MTRK_OVER,
    MTRK_OUT,
    MTRK_NEXT
};

typedef unsigned_8 mad_trace_how; enum {
    MTRH_STOP,
    MTRH_SIMULATE,
    MTRH_STEP,
    MTRH_STEPBREAK,
    MTRH_BREAK
};


typedef unsigned_8 mad_notify_type; enum {
    MNT_ERROR,
    MNT_MODIFY_REG,
    MNT_MODIFY_IP,
    MNT_MODIFY_SP,
    MNT_MODIFY_FP,
    MNT_EXECUTE_TOUCH_SCREEN_BUFF,
    MNT_EXECUTE_LONG,
    MNT_REDRAW_DISASM,
    MNT_REDRAW_REG
};


typedef unsigned_8 mad_label_kind; enum {
    MLK_CODE,
    MLK_DATA,
    MLK_MEMORY
};

typedef struct {
    mad_string  menu;
    mad_string  on;
    mad_string  off;
} mad_toggle_strings;

#include "digunpck.h"
#endif
