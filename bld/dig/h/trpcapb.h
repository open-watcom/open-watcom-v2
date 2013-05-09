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
* Description:  Support trap requests.
*
****************************************************************************/


#ifndef TRPCAPB_H

#include "trptypes.h"
#include <time.h>

#include "pushpck1.h"

#define CAPABILITIES_SUPP_NAME  "Capabilities"

enum {
    REQ_CAPABILITIES_GET_8B_BP,             /* 00 */
    REQ_CAPABILITIES_SET_8B_BP,             /* 01 */
    REQ_CAPABILITIES_GET_EXACT_BP,          /* 02 */
    REQ_CAPABILITIES_SET_EXACT_BP,          /* 03 */
};

/*======================= REQ_CAPABILITIES_GET_8B_BP ================*/
/* 
 *  Check to see if the trap support 8 byte breakpoints.
 *  If returns no error, then supported and _ret.status tells you if currently enabled or not
 */
typedef struct {
    supp_prefix         supp;
    access_req          req;
} capabilities_get_8b_bp_req;

typedef struct {
    trap_error          err;
    unsigned_32         status;     /* 0 = off, !0 = on */
} capabilities_get_8b_bp_ret;

/*======================= REQ_CAPABILITIES_SET_8B_BP ================*/

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_32         status;     /* 0 = off, !0 = on */
} capabilities_set_8b_bp_req;

typedef struct {
    trap_error          err;
    unsigned_32         status;     /* 0 = off, !0 = on */
} capabilities_set_8b_bp_ret;

/*======================= REQ_CAPABILITIES_EXACT_8B_BP ================*/
/* 
 *  Check to see if the trap support exact breakpoints.
 *  If returns no error, then supported and _ret.status tells you if currently enabled or not
 */
typedef struct {
    supp_prefix         supp;
    access_req          req;
} capabilities_get_exact_bp_req;

typedef struct {
    trap_error          err;
    unsigned_32         status;     /* 0 = off, !0 = on */
} capabilities_get_exact_bp_ret;

/*======================= REQ_CAPABILITIES_SET_EXACT_BP ================*/

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_32         status;     /* 0 = off, !0 = on */
} capabilities_set_exact_bp_req;

typedef struct {
    trap_error          err;
    unsigned_32         status;     /* 0 = off, !0 = on */
} capabilities_set_exact_bp_ret;

#include "poppck.h"

#define TRPCAPB_H

#endif
