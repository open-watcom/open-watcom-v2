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
* Description:  Remote environment supplementary trap requests.
*
****************************************************************************/


#ifndef TRPENV_H

#include "trptypes.h"
#include <time.h>

#define ENV_SUPP_NAME   Environment
#define TRAP_ENV(s)     TRAP_SYM( ENV_SUPP_NAME, s )

//#define REQ_ENV_DEF(sym,func)
#define REQ_ENV_DEFS() \
    REQ_ENV_DEF( GET_VAR,   get_var ) \
    REQ_ENV_DEF( SET_VAR,   set_var )

enum {
    #define REQ_ENV_DEF(sym,func)   REQ_ENV_ ## sym,
    REQ_ENV_DEFS()
    #undef REQ_ENV_DEF
};

#include "pushpck1.h"

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
