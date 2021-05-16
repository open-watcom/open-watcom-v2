/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <time.h>
#include "passby.h"
#include "cgaux.h"
#include "auxflags.h"


#if _CPU == 386
#define ARG_NEAR            ARG_SIZE_4
#define ARG_FAR             ARG_SIZE_8
#else
#define ARG_NEAR            ARG_SIZE_2
#define ARG_FAR             ARG_SIZE_4
#endif

typedef struct aux_info {
    call_class          cclass;
    byte_seq            *code;
    hw_reg_set          *parms;
    hw_reg_set          returns;
    hw_reg_set          streturn;
    hw_reg_set          save;
    char                *objname;
    unsigned            use;
    aux_flags           flags;
    struct aux_info     *link;
    pass_by             *arg_info;
    size_t              sym_len;
    char                sym_name[1];
} aux_info;

typedef struct default_lib {
    struct default_lib  *link;
    char                lib[2]; // 1 for priority and 1 for NULLCHAR
} default_lib;

typedef struct arr_info {
    struct arr_info     *link;
    uint                len;
    char                arr[1];
} arr_info;

typedef struct dep_info {
    struct dep_info     *link;
    time_t              time_stamp;
    char                fn[1];
} dep_info;

extern  default_lib     *DefaultLibs;
extern  dep_info        *DependencyInfo;
