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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "zipint.h"

#include "pushpck1.h"
typedef struct {
    char            signature[4];
    unsigned_16     disk_number;            // not supported by libzip
    unsigned_16     disk_having_cd;         // not supported by libzip
    unsigned_16     num_entries_on_disk;    // not supported by libzip
    unsigned_16     total_num_entries;
    unsigned_32     cd_size;
    unsigned_32     cd_offset;
    unsigned_16     comment_length;
//    char            comment[comment_length]
} wzip_cdir;

typedef struct {
    char            signature[4];
    unsigned_16     version_made_by;
    unsigned_16     version_needed;
    unsigned_16     flags;
    unsigned_16     method;
    unsigned_16     mod_time;
    unsigned_16     mod_date;
    unsigned_32     crc32;
    unsigned_32     compressed_size;
    unsigned_32     uncompressed_size;
    unsigned_16     file_name_length;
    unsigned_16     extra_field_length;
    unsigned_16     file_comment_length;
    unsigned_16     disk;                   // not supported by libzip
    unsigned_16     int_attrib;
    unsigned_32     ext_attrib;
    unsigned_32     offset;
//    char            filename[file_name_length]
//    char            extrafield[extra_field_length]
//    char            comment[file_comment_length]
} wzip_dirent;
#include "poppck.h"

#define SIZE_CENTRAL_MAGIC  (sizeof( CENTRAL_MAGIC ) - 1)
#define SIZE_EOCD_MAGIC     (sizeof( EOCD_MAGIC ) - 1)
