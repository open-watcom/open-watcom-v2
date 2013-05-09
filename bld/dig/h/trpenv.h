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
* Description:  Remote environment supplementary trap requests.
*
****************************************************************************/


#ifndef TRPENV_H

#include "trptypes.h"
#include <time.h>

#include "pushpck1.h"

#define ENV_SUPP_NAME   "Environment"

enum {
    REQ_ENV_GET_VAR,            /* 00 */
    REQ_ENV_SET_VAR,            /* 01 */
};


/*======================= REQ_FILE_GET_ENV ================*/

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_32         res_len;
    /* followed by name of environment variable to get */
} env_get_var_req;

typedef struct {
    trap_error          err;
    /* followed by value of environment variable */
} env_get_var_ret;

/*======================= REQ_FILE_SET_ENV ================*/

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by name of environment variable to set */
    /* followed by value of environment variable to set */
} env_set_var_req;

typedef struct {
    trap_error          err;
} env_set_var_ret;

#include "poppck.h"

#define TRPENV_H

#endif
