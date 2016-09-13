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


#include "dbgitem.h"
#include "madtypes.h"

typedef struct
{
    BITB    active              : 1;
    BITB    resume              : 1;
    BITB    silent              : 1;
    BITB    hit                 : 1;

    BITB    in_place            : 1;
    BITB    has_value           : 1;
    BITB    has_address         : 1;
    BITB    unmapped            : 1;

    BITB    cmds_pushed         : 1;
    BITB    expr_true           : 1;
    BITB    expr_error          : 1;
    BITB    cmd_error           : 1;

    BITB    source_open         : 1;
    BITB    activate_on_remap   : 1;
    BITB    autodestruct        : 1;
    BITB    use_countdown       : 1;

    BITB    use_cmds            : 1;
    BITB    use_condition       : 1;
} brk_status;

#define NullStatus( b ) memset( &((b)->status), 0, sizeof( (b)->status ) )

typedef struct brkp {
    mappable_addr   loc;
    item_mach       item;   /* opcode in break points */
    mad_type_handle th;     /* how to format data item */
    dig_mad         mad;    /* MAD to use when formatting */
    unsigned_8      __unused_size;
    cmd_list        *cmds;
    int             index;
    long            total_hits;
    long            countdown;
    struct brkp     *next;
    char            *condition;
    char            *error;
    char            *source_line;
    union {
        brk_status  b;
    } status;
    long            initial_countdown;
    char            *image_name;
    char            *mod_name;
    char            *sym_name;
    long            cue_diff;
    long            addr_diff;
} brkp;

#define BP_EXECUTE          MAD_NIL_TYPE_HANDLE

#define IS_BP_EXECUTE(x)    ((x) == BP_EXECUTE)
