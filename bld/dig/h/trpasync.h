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


#ifndef TRPASYNC_H
#define TRPASYNC_H

#include "trptypes.h"
#include <time.h>

#pragma pack(push,1);

#define ASYNC_SUPP_NAME "Asynch"

enum {
    REQ_ASYNC_GO,               /* 00 */
    REQ_ASYNC_INTERRUPT,        /* 01 */
    REQ_ASYNC_STOP,             /* 02 */
    REQ_ASYNC_POLL,             /* 03 */
    REQ_ASYNC_CHECK,            /* 04 */
};


//======================= REQ_ASYNC_GO

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_8          step;
    unsigned_8          source;
    unsigned_8          over;
} async_go_req;

typedef struct {
    trap_error          err;
} async_go_ret;

//======================= REQ_ASYNC_INTERRUPT

typedef struct {
    supp_prefix         supp;
    access_req          req;
} async_interrupt_req;

typedef struct {
    trap_error          err;
} async_interrupt_ret;

//======================= REQ_ASYNC_CHECK

typedef struct {
    supp_prefix         supp;
    access_req          req;
} async_check_req;

typedef struct {
    trap_error          err;
} async_check_ret;

//======================= REQ_ASYNC_STOP

typedef struct {
    supp_prefix         supp;
    access_req          req;
} async_stop_req;

typedef struct {
    trap_error          err;
} async_stop_ret;

//======================= REQ_ASYNC_POLL

typedef struct {
    supp_prefix         supp;
    access_req          req;
} async_poll_req;

typedef struct {
    addr48_ptr          stack_pointer;
    addr48_ptr          program_counter;
    unsigned_16         conditions;
    trap_error          err;
    unsigned_8          running;
} _WCUNALIGNED async_poll_ret;

#pragma pack(pop);

#endif
