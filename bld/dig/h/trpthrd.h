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
* Description:  Thread debugging supplementary requests.
*
****************************************************************************/


#ifndef TRPTHRD_H

#include "trptypes.h"

#pragma pack( push, 1 )

#define THREAD_SUPP_NAME                "Threads"

typedef unsigned_32     trap_thandle; /* thread handle */
enum {
    THREAD_THAWED,
    THREAD_FROZEN
};


enum {
    REQ_THREAD_GET_NEXT,        /* 00 */
    REQ_THREAD_SET,             /* 01 */
    REQ_THREAD_FREEZE,          /* 02 */
    REQ_THREAD_THAW,            /* 03 */
    REQ_THREAD_GET_EXTRA        /* 04 */
};


typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_thandle        thread;
} _WCUNALIGNED thread_get_next_req;

typedef struct {
    trap_thandle        thread;
    unsigned_8          state;
} thread_get_next_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_thandle        thread;
} _WCUNALIGNED thread_set_req;

typedef struct {
    trap_error          err;
    trap_thandle        old_thread;
} thread_set_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_thandle        thread;
} _WCUNALIGNED thread_freeze_req;

typedef struct {
    trap_error          err;
} thread_freeze_ret;

typedef thread_freeze_req               thread_thaw_req;
typedef thread_freeze_ret               thread_thaw_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_thandle        thread;
} _WCUNALIGNED thread_get_extra_req;

/* return for REQ_THREAD_GET_EXTRA is a string with the thread extra info */

#pragma pack( pop )

#define TRPTHRD_H

#endif
