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
* Description:  File I/O supplementary requests.
*
****************************************************************************/


#ifndef TRPFILE_H

#include "trptypes.h"

#include "pushpck1.h"

#define FILE_SUPP_NAME          "Files"

typedef unsigned_32     trap_fhandle; /* file handle */

enum {
    REQ_FILE_GET_CONFIG,        /* 00 */
    REQ_FILE_OPEN,              /* 01 */
    REQ_FILE_SEEK,              /* 02 */
    REQ_FILE_READ,              /* 03 */
    REQ_FILE_WRITE,             /* 04 */
    REQ_FILE_WRITE_CONSOLE,     /* 05 */
    REQ_FILE_CLOSE,             /* 06 */
    REQ_FILE_ERASE,             /* 07 */
    REQ_FILE_STRING_TO_FULLPATH,/* 08 */
    REQ_FILE_RUN_CMD            /* 09 */
};


/*======================= REQ_FILE_GET_CONFIG ================*/

typedef struct {
    supp_prefix         supp;
    access_req          req;
} file_get_config_req;

typedef struct {
    file_components     file;
} file_get_config_ret;

/*==================== REQ_FILE_OPEN ===================*/
enum {
    TF_READ     = 0x1,
    TF_WRITE    = 0x2,
    TF_CREATE   = 0x4,
    TF_EXEC     = 0x8
};

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_8          mode;
} file_open_req;

typedef struct {
    trap_error          err;
    trap_fhandle        handle;
} file_open_ret;

/*====================== REQ_FILE_SEEK ===================*/
enum {
    TF_SEEK_ORG,
    TF_SEEK_CUR,
    TF_SEEK_END
};

typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_fhandle        handle;
    unsigned_8          mode;
    unsigned_32         pos;
} _WCUNALIGNED file_seek_req;

typedef struct {
    trap_error          err;
    unsigned_32         pos;
} file_seek_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_fhandle        handle;
    unsigned_16         len;
} _WCUNALIGNED file_read_req;

typedef struct {
    trap_error          err;
    /* followed by sequence of bytes requested */
} file_read_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_fhandle        handle;
    /* followed by sequence of bytes to write */
} _WCUNALIGNED file_write_req;

typedef struct {
    trap_error          err;
    unsigned_16         len;
} file_write_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by data to write to console */
} _WCUNALIGNED file_write_console_req;

typedef file_write_ret  file_write_console_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    trap_fhandle        handle;
} _WCUNALIGNED file_close_req;

typedef struct {
    trap_error          err;
} file_close_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    /* followed by file name to be deleted */
} file_erase_req;

typedef struct {
    trap_error          err;
} file_erase_ret;

/*====================== REQ_FILE_STRING_TO_FULLPATH =================*/
enum {
    TF_TYPE_EXE,
    TF_TYPE_DBG,
    TF_TYPE_PRS,
    TF_TYPE_HLP
};

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_8          file_type;
    /* followed by file name to be searched for */
} file_string_to_fullpath_req;

typedef struct {
    trap_error          err;
    /* followed by expanded pathname */
} file_string_to_fullpath_ret;

typedef struct {
    supp_prefix         supp;
    access_req          req;
    unsigned_16         chk_size;
} _WCUNALIGNED file_run_cmd_req;

typedef struct {
    trap_error          err;
} file_run_cmd_ret;

#include "poppck.h"

#define TRPFILE_H

#endif
