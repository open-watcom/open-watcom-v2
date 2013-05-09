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
* Description:  Asynchronous debugging supplementary trap requests.
*
****************************************************************************/


#ifndef TRPASYNC_H
#define TRPASYNC_H

#include "trptypes.h"
#include <time.h>

#include "pushpck1.h"

#define ASYNC_SUPP_NAME "Asynch"

enum {
    REQ_ASYNC_GO,               /* 00 */
    REQ_ASYNC_STEP,             /* 01 */
    REQ_ASYNC_POLL,             /* 02 */
    REQ_ASYNC_STOP,             /* 03 */
};


/*=================== REQ_ASYNC_GO ===================*/

typedef struct {
    supp_prefix         supp;
    access_req          req;
} async_go_req;

typedef struct {
    addr48_ptr          stack_pointer;
    addr48_ptr          program_counter;
    unsigned_16         conditions;
} _WCUNALIGNED async_go_ret;

/*=================== REQ_ASYNC_STEP ===================*/

typedef async_go_req     async_step_req;
typedef async_go_ret     async_step_ret;

/*=================== REQ_ASYNC_POLL ===================*/

typedef async_go_req     async_poll_req;
typedef async_go_ret     async_poll_ret;

/*=================== REQ_ASYNC_STOP ===================*/

typedef async_go_req     async_stop_req;
typedef async_go_ret     async_stop_ret;

#include "poppck.h"

#endif
