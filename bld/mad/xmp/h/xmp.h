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


#include "madimp.h"

#define NUM_REG_SET     1

struct imp_mad_state_data {
    unsigned    reg_state[NUM_REG_SET];
    unsigned    disasm_state;
};

struct mad_disasm_data {
    address             addr;
    unsigned            radix;
};

struct mad_trace_data {
    addr_off            ra;
};

struct mad_call_up_data {
    unsigned_8          dummy;
};

typedef struct mad_type_data {
    mad_string          name;
    unsigned            hex     : 1;
    union {
        const mad_type_info_basic       *b;
        const mad_type_info             *info;
    }                   u;
} mad_type_data;

extern imp_mad_state_data       *MADState;
