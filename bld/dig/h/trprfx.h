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
* Description:  Remote file access supplementary trap requests.
*
****************************************************************************/


#ifndef TRPRFX_H
#define TRPRFX_H

#include "trpfile.h"


#define RFX_SUPP_NAME       Rfx
#define TRAP_RFX(s)         TRAP_SYM( RFX_SUPP_NAME, s )

//#define REQ_RFX_DEF(sym,func)
#define REQ_RFX_DEFS() \
    REQ_RFX_DEF( RENAME,          rename ) \
    REQ_RFX_DEF( MKDIR,           mkdir ) \
    REQ_RFX_DEF( RMDIR,           rmdir ) \
    REQ_RFX_DEF( SETDRIVE,        setdrive ) \
    REQ_RFX_DEF( GETDRIVE,        getdrive ) \
    REQ_RFX_DEF( SETCWD,          setcwd ) \
    REQ_RFX_DEF( GETCWD,          getcwd ) \
    REQ_RFX_DEF( SETDATETIME,     setdatetime ) \
    REQ_RFX_DEF( GETDATETIME,     getdatetime ) \
    REQ_RFX_DEF( GETFREESPACE,    getfreespace ) \
    REQ_RFX_DEF( SETFILEATTR,     setfileattr ) \
    REQ_RFX_DEF( GETFILEATTR,     getfileattr ) \
    REQ_RFX_DEF( NAMETOCANONICAL, nametocanonical ) \
    REQ_RFX_DEF( FINDFIRST,       findfirst ) \
    REQ_RFX_DEF( FINDNEXT,        findnext ) \
    REQ_RFX_DEF( FINDCLOSE,       findclose )

enum {
    #define REQ_RFX_DEF(sym,func)   REQ_RFX_ ## sym,
    REQ_RFX_DEFS()
    #undef REQ_RFX_DEF
};

#define RFX_NAME_MAX            259

#include "pushpck1.h"

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
} rfx_nametocanonical_req;

typedef struct {
    trap_error          err;
    /* followed by canonical file name string */
} rfx_nametocanonical_ret;

/*============================ RFX_FIND_FIRST ===============*/
typedef struct __rfx_dta {  /* total size 21 bytes */
    union {
        unsigned_8      reserved1[13];
        struct {
            long long       handle;
            unsigned_16     attrib;
        }               s;
    }               u1;
    union {
        struct {
            unsigned_16     dir_entry_num;
            unsigned_16     cluster;
        }               dos;
        struct {
            unsigned_16     time;
            unsigned_16     date;
        }               stamp;
        unsigned_32     id;
    }               u;
//    unsigned_8      reserved2[4];
} __rfx_dta;

#define DTARFX_HANDLE_OF(x)     (((__rfx_dta *)(x)->reserved)->u1.s.handle)
#define DTARFX_ATTRIB_OF(x)     (((__rfx_dta *)(x)->reserved)->u1.s.attrib)
#define DTARFX_DIR_NUM_OF(x)    (((__rfx_dta *)(x)->reserved)->u.dos.dir_entry_num)
#define DTARFX_CLUSTER_OF(x)    (((__rfx_dta *)(x)->reserved)->u.dos.cluster)
#define DTARFX_TIME_OF(x)       (((__rfx_dta *)(x)->reserved)->u.stamp.time)
#define DTARFX_DATE_OF(x)       (((__rfx_dta *)(x)->reserved)->u.stamp.date)
#define DTARFX_ID_OF(x)         (((__rfx_dta *)(x)->reserved)->u.id)

#define DTARFX_INVALID_HANDLE   ((long long)~0)
#define DTARFX_INVALID_ID       ((unsigned_32)~0)

typedef struct {
    unsigned_8          reserved[21];
    unsigned_8          attr;
    unsigned_16         time;
    unsigned_16         date;
    unsigned_32         size;
    char                name[RFX_NAME_MAX + 1];
} _WCUNALIGNED rfx_find;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_8          attrib;
    /* followed by directory name string */
} rfx_findfirst_req;

typedef struct {
    trap_error          err;
    /* followed by a rfx_find */
} rfx_findfirst_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by a rfx_find */
} rfx_findnext_req;

typedef struct {
    trap_error          err;
    /* followed by a rfx_find */
} rfx_findnext_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by a rfx_find */
} rfx_findclose_req;

typedef struct {
    trap_error          err;
} rfx_findclose_ret;

#include "poppck.h"

#endif
