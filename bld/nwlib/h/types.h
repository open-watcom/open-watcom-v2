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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


typedef uint_8          ar_len;

#if 0
typedef enum {
    OKAY,
    DONE,
    ERROR,
    OUT_OF_MEMORY,
    BAD_USAGE,
    BAD_ARCHIVE,
    BAD_FILE,
    FILE_DOES_NOT_EXIST,
    FILE_NOT_FOUND_IN_ARCHIVE,
    OPEN_ERROR,
    READ_ERROR,
    WRITE_ERROR,
    CLOSE_ERROR,
    STAT_ERROR
} status;
#endif

// local header types
typedef uint_32         arch_date;
typedef uint_16         arch_uid;
typedef uint_16         arch_gid;
typedef uint_32         arch_mode;
typedef uint_32         arch_file_size;

typedef struct arch_header {
    char *              name;
    char *              ffname; // Full filename
    arch_date           date;
    arch_uid            uid;
    arch_gid            gid;
    arch_mode           mode;
    arch_file_size      size;
    char *              fnametab;
    char *              ffnametab;
    char *              nextffname;
    char *              lastffname;
} arch_header;

