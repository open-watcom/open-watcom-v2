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


#ifndef CANMISC_H
#include "watcom.h"

/*
    The order of this enum is depended on in several places
*/
enum can_minimum_processor {
    CAN_PROC_8086,
    CAN_PROC_80186,
    CAN_PROC_80286,
    CAN_PROC_80386,
    CAN_PROC_80486
    /* make sure doesn't exceed 8 values */
};

/*
    The order of this enum is depended on in several places
*/
enum can_memory_model {
    CAN_MODEL_SMALL,
    CAN_MODEL_MEDIUM,
    CAN_MODEL_COMPACT,
    CAN_MODEL_LARGE,
    CAN_MODEL_HUGE,
    CAN_MODEL_FLAT,
    CAN_MODEL_TINY
    /* make sure doesn't exceed 8 values */
};

enum can_floating_point_option {
    CAN_FLOAT_EM_INLINE,
    CAN_FLOAT_EM_CALLS,
    CAN_FLOAT_87_INLINE,
    CAN_FLOAT_ALTERNATE
    /* make sure doesn't exceed 8 values */
};

enum can_src_language {
    CAN_LANG_ASM,
    CAN_LANG_C,
    CAN_LANG_FORTRAN,
    CAN_LANG_PASCAL,
    CAN_LANG_BASIC
    /* make sure this doesn't exceed 16 values */
};

struct canmisc {
    char            *compiler;      /* string containing compiler's name */

    uint_16         processor       : 3;
    uint_16         memory_model    : 3;
    uint_16         optimized       : 1;
    uint_16         lines_present   : 1;

    uint_16         floating_point  : 3;
    uint_16         types_present   : 1;/* set to 1 when type graph present */
    uint_16         src_language    : 4;

    uint_16         symbs_present   : 1;/* set to 1 when symb table present */
};

extern struct canmisc CanMisc;
extern void CanMInit( void );
extern void CanMFini( void );

#define CANMISC_H   1
#endif

