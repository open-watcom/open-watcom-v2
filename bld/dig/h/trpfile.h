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
* Description:  File I/O supplementary requests.
*
****************************************************************************/


#ifndef TRPFILE_H
#define TRPFILE_H

#include "trptypes.h"

#define FILE_SUPP_NAME      Files
#define TRAP_FILE(s)        TRAP_SYM( FILE_SUPP_NAME, s )

//#define REQ_FILE_DEF(sym,func)
#define REQ_FILE_DEFS() \
    REQ_FILE_DEF( GET_CONFIG,         get_config ) \
    REQ_FILE_DEF( OPEN,               open ) \
    REQ_FILE_DEF( SEEK,               seek ) \
    REQ_FILE_DEF( READ,               read ) \
    REQ_FILE_DEF( WRITE,              write ) \
    REQ_FILE_DEF( WRITE_CONSOLE,      write_console ) \
    REQ_FILE_DEF( CLOSE,              close ) \
    REQ_FILE_DEF( ERASE,              erase ) \
    REQ_FILE_DEF( STRING_TO_FULLPATH, file_to_fullpath ) \
    REQ_FILE_DEF( RUN_CMD,            run_cmd )

enum {
    #define REQ_FILE_DEF(sym,func)   REQ_FILE_ ## sym,
    REQ_FILE_DEFS()
    #undef REQ_FILE_DEF
};

#include "pushpck1.h"

typedef unsigned_64     trap_fhandle; /* file handle */

/*======================= REQ_FILE_GET_CONFIG ================*/

typedef struct {
    supp_prefix         supp;
    trap_req            req;
} file_get_config_req;

typedef struct {
    file_components     file;
} file_get_config_ret;

/*==================== REQ_FILE_OPEN ===================*/
typedef struct {
    supp_prefix         supp;
    trap_req            req;
    unsigned_8          mode;
} file_open_req;

typedef struct {
    trap_error          err;
    trap_fhandle        handle;
} file_open_ret;

/*====================== REQ_FILE_SEEK ===================*/
typedef struct {
    supp_prefix         supp;
    trap_req            req;
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
    trap_req            req;
    trap_fhandle        handle;
    unsigned_16         len;
} _WCUNALIGNED file_read_req;

typedef struct {
    trap_error          err;
    /* followed by sequence of bytes requested */
} file_read_ret;

typedef struct {
    supp_prefix         supp;
    trap_req            req;
    trap_fhandle        handle;
    /* followed by sequence of bytes to write */
} _WCUNALIGNED file_write_req;

typedef struct {
    trap_error          err;
    unsigned_16         len;
} file_write_ret;

typedef struct {
    supp_prefix         supp;
    trap_req            req;
    /* followed by data to write to console */
} _WCUNALIGNED file_write_console_req;

typedef struct {
    trap_error          err;
    unsigned_16         len;
} file_write_console_ret;

typedef struct {
    supp_prefix         supp;
    trap_req            req;
    trap_fhandle        handle;
} _WCUNALIGNED file_close_req;

typedef struct {
    trap_error          err;
} file_close_ret;

typedef struct {
    supp_prefix         supp;
    trap_req            req;
    /* followed by file name to be deleted */
} file_erase_req;

typedef struct {
    trap_error          err;
} file_erase_ret;

/*====================== REQ_FILE_STRING_TO_FULLPATH =================*/
typedef struct {
    supp_prefix         supp;
    trap_req            req;
    unsigned_8          file_type;
    /* followed by file name to be searched for */
} file_string_to_fullpath_req;

typedef struct {
    trap_error          err;
    /* followed by expanded pathname */
} file_string_to_fullpath_ret;

typedef struct {
    supp_prefix         supp;
    trap_req            req;
    unsigned_16         chk_size;
} _WCUNALIGNED file_run_cmd_req;

typedef struct {
    trap_error          err;
} file_run_cmd_ret;

#include "poppck.h"

#endif
