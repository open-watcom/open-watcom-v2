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


#include "bool.h"
#include "distypex.h"

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
    unsigned            (*ins_hook)( dis_handle *, void *, dis_dec_ins *, dis_format_flags, char *ins );
    unsigned            (*flag_hook)( dis_handle *, void *, dis_dec_ins *, dis_format_flags, char *ins );
    unsigned            (*op_hook)( dis_handle *, void *, dis_dec_ins *, dis_format_flags, unsigned op, char *op_buff );
    const unsigned char *max_insnamep;
    unsigned char       inssize_inc;
};

unsigned        DisGetString( unsigned, char *, int );
char            *DisAddReg( dis_register, char *, dis_format_flags );
char            *DisOpFormat( void *, dis_dec_ins *, dis_format_flags, unsigned, char * );
