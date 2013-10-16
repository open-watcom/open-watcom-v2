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


#include "cgaux.h"

// The following describes argument information:

typedef enum {
    PASS_BY_REFERENCE       = 0x0001,
    PASS_BY_VALUE           = 0x0002,
    PASS_BY_DESCRIPTOR      = 0x0004,
    PASS_BY_NODESCRIPTOR    = 0x0008,
    PASS_BY_DATA            = 0x0010,

    ARG_SIZE_1              = 0x0800,
    ARG_SIZE_2              = 0x1000,
    ARG_SIZE_4              = 0x2000,
    ARG_SIZE_8              = 0x4000,
    ARG_SIZE_16             = 0x8000
} pass_info;

#if _CPU == 386
#define ARG_NEAR            ARG_SIZE_4
#define ARG_FAR             ARG_SIZE_8
#else
#define ARG_NEAR            ARG_SIZE_2
#define ARG_FAR             ARG_SIZE_4
#endif

typedef struct pass_by {
    struct pass_by      *link;
    pass_info           info;
} pass_by;

typedef struct aux_info {
    struct aux_info     *link;
    call_class          cclass;
    hw_reg_set          save;
    hw_reg_set          returns;
    hw_reg_set          *parms;
    byte_seq            *code;
    hw_reg_set          streturn;
    char                *objname;
    pass_by             *arg_info;
    byte                sym_len;
    char                sym_name[1];
} aux_info;

typedef struct aux_entry {
    struct aux_info     *info;
    struct aux_entry    *next;
    pass_by             *arg_info;
    byte                sym_len;
    char                sym_name[1];
};

typedef struct default_lib {
    struct default_lib  *link;
    char                lib[2]; // 1 for priority and 1 for NULLCHAR
} default_lib;

typedef struct arr_info {
    struct arr_info     *link;
    char                arr[1];
} arr_info;

#include <time.h>

typedef struct dep_info {
    struct dep_info     *link;
    time_t              time_stamp;
    char                fn[1];
} dep_info;

