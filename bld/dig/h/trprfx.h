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
* Description:  Remote file access supplementary trap requests.
*
****************************************************************************/


#ifndef TRPRFX_H

#include "trpfile.h"

#pragma pack( push, 1 )

#define RFX_SUPP_NAME           "RFX"

enum {
    REQ_RFX_RENAME,             /* 00 */
    REQ_RFX_MKDIR,              /* 01 */
    REQ_RFX_RMDIR,              /* 02 */
    REQ_RFX_SETDRIVE,           /* 03 */
    REQ_RFX_GETDRIVE,           /* 04 */
    REQ_RFX_SETCWD,             /* 05 */
    REQ_RFX_GETCWD,             /* 06 */
    REQ_RFX_SETDATETIME,        /* 07 */
    REQ_RFX_GETDATETIME,        /* 08 */
    REQ_RFX_GETFREESPACE,       /* 09 */
    REQ_RFX_SETFILEATTR,        /* 10 */
    REQ_RFX_GETFILEATTR,        /* 11 */
    REQ_RFX_NAMETOCANNONICAL,   /* 12 */
    REQ_RFX_FINDFIRST,          /* 13 */
    REQ_RFX_FINDNEXT,           /* 14 */
    REQ_RFX_FINDCLOSE           /* 15 */
};

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by old_name, new_name strings */
} rfx_rename_req;

typedef struct {
    trap_error          err;
} rfx_rename_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by directory name string */
} rfx_mkdir_req;

typedef struct {
    trap_error          err;
} rfx_mkdir_ret;

typedef rfx_mkdir_req           rfx_rmdir_req;
typedef rfx_mkdir_ret           rfx_rmdir_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_8          drive;
} rfx_setdrive_req;

typedef struct {
    trap_error          err;
} rfx_setdrive_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
} rfx_getdrive_req;

typedef struct {
    unsigned_8          drive;
} rfx_getdrive_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by new cwd string */
} rfx_setcwd_req;

typedef struct {
    trap_error          err;
} rfx_setcwd_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_8          drive;
} rfx_getcwd_req;

typedef struct {
    trap_error          err;
    /* followed by CWD string for the drive */
} rfx_getcwd_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_fhandle        handle;
    unsigned_32         time;
} _WCUNALIGNED rfx_setdatetime_req;

/* no rfx_setdatatime_ret */

typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_fhandle        handle;
} _WCUNALIGNED rfx_getdatetime_req;

typedef struct {
    unsigned_32         time;
} rfx_getdatetime_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_8          drive;
} rfx_getfreespace_req;

typedef struct {
    unsigned_32         size;
} rfx_getfreespace_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_32         attribute;
    /* followed by file name string */
} _WCUNALIGNED rfx_setfileattr_req;

typedef struct {
    trap_error          err;
} rfx_setfileattr_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by file name string */
} rfx_getfileattr_req;

typedef struct {
    unsigned_32         attribute;
} rfx_getfileattr_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by file name string */
} rfx_nametocannonical_req;

typedef struct {
    trap_error          err;
    /* followed by cannonical file name string */
} rfx_nametocannonical_ret;

/*============================ RFX_FIND_FIRST ===============*/
typedef struct {
    struct {
        unsigned_8      spare1[13];
        unsigned_16     dir_entry_num;
        unsigned_16     cluster;
        unsigned_8      spare2[4];
    }           dos;
    unsigned_8  attr;
    unsigned_16 time;
    unsigned_16 date;
    unsigned_32 size;
    char        name[14];
} _WCUNALIGNED trap_dta;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_8          attrib;
    /* followed by directory name string */
} rfx_findfirst_req;

typedef struct {
    trap_error          err;
    /* followed by a trap_dta */
} rfx_findfirst_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by a trap_dta */
} rfx_findnext_req;

typedef struct {
    trap_error          err;
    /* followed by a trap_dta */
} rfx_findnext_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
} rfx_findclose_req;

typedef struct {
    trap_error          err;
    /* followed by a trap_dta */
} rfx_findclose_ret;

#pragma pack( pop )

#define TRPRFX_H

#endif
