/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Remote file info supplementary trap requests.
*
****************************************************************************/


#ifndef TRPFINFO_H
#define TRPFINFO_H

#include <time.h>
#include "trptypes.h"

#define FILE_INFO_SUPP_NAME     FileInfo
#define TRAP_FILE_INFO(s)       TRAP_SYM( FILE_INFO_SUPP_NAME, s )

//#define REQ_FILE_INFO_DEF(sym,func)
#define REQ_FILE_INFO_DEFS() \
    REQ_FILE_INFO_DEF( GET_DATE,   get_date ) \
    REQ_FILE_INFO_DEF( SET_DATE,   set_date )

enum {
    #define REQ_FILE_INFO_DEF(sym,func)   REQ_FILE_INFO_ ## sym,
    REQ_FILE_INFO_DEFS()
    #undef REQ_FILE_INFO_DEF
};

#include "pushpck1.h"

/*======================= REQ_FILE_GET_DATE ================*/

typedef struct {
    supp_prefix         supp;
    trap_req            req;
    /* followed by file name */
} file_info_get_date_req;

typedef struct {
    trap_error          err;
    long                date;
} file_info_get_date_ret;

/*======================= REQ_FILE_SET_DATE ================*/

typedef struct {
    supp_prefix         supp;
    trap_req            req;
    long                date;
    /* followed by file name */
} file_info_set_date_req;

typedef struct {
    trap_error          err;
} file_info_set_date_ret;

#include "poppck.h"

#endif
