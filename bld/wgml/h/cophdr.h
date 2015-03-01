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
* Description:  Declares an enum and a function which are used to parse
*               .COP file headers:
*                   cop_file_type
*                   parse_header()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
****************************************************************************/

#ifndef COPHDR_H_INCLUDED
#define COPHDR_H_INCLUDED

#include <stdio.h>

/* Enum definition. */

/* This enum is used for the return value of function parse_header(). */

typedef enum {
    dir_v4_1_se,        // The file is a same-endian version 4.1 directory file.
    se_v4_1_not_dir,    // The file is a same-endian version 4.1 device, driver, or font file.
    not_se_v4_1,        // The file is not same-endian and/or not version 4.1.
    not_bin_dev,        // The file is not a binary device file at all.
    file_error          // An error occurred while reading the file.
} cop_file_type;

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern cop_file_type    parse_header( FILE * in_file );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPHDR_H_INCLUDED */
