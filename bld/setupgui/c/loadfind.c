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
* Description:  Custom version of FindResources() for text mode installer.
*
****************************************************************************/


#include <string.h>
#include "watcom.h"
#include "wresall.h"
#include "wresset2.h"
#include "wresrtns.h"

#include "pushpck1.h"
typedef struct dbgheader {
    uint_16     signature;
    uint_8      exe_major_ver;
    uint_8      exe_minor_ver;
    uint_8      obj_major_ver;
    uint_8      obj_minor_ver;
    uint_16     lang_size;
    uint_16     seg_size;
    uint_32     debug_size;
} dbgheader;

typedef struct {
    char        signature[4];
    uint_16     disk_number;        
    uint_16     disk_having_cd;     
    uint_16     num_entries_on_disk;
    uint_16     total_num_entries;
    uint_32     cd_size;
    uint_32     cd_offset;
    uint_16     comment_length;
} zip_eocd;

typedef struct {
    char        signature[4];
    uint_16     version_made_by;
    uint_16     version_needed;
    uint_16     flags;
    uint_16     method;
    uint_16     mod_time;
    uint_16     mod_date;
    uint_32     crc32;
    uint_32     compressed_size;
    uint_32     uncompressed_size;
    uint_16     file_name_length;
    uint_16     extra_field_length;
    uint_16     file_comment_length;
    uint_16     disk;                   // not supported by libzip
    uint_16     int_attrib;
    uint_32     ext_attrib;
    uint_32     offset;
} zip_cdfh;
#include "poppck.h"

#define VALID_SIGNATURE 0x8386
#define FOX_SIGNATURE1  0x8300
#define FOX_SIGNATURE2  0x8301
#define WAT_RES_SIG     0x8302

long                    FileShift = 0;

/* look for the resource information in a debugger record at the end of file */
bool FindResources( PHANDLE_INFO hInstance )
{
    long        currpos;
    long        offset;
    dbgheader   header;
    zip_eocd    eocd;
    zip_cdfh    cdfh;
    bool        notfound;

    notfound = true;
    FileShift = 0;
    offset = sizeof( dbgheader );

    /* Look for a PKZIP header and skip archive if present */
    if( WRESSEEK( hInstance->handle, -(long)sizeof( eocd ), SEEK_END ) != -1 ) {
        if( WRESREAD( hInstance->handle, &eocd, sizeof( eocd ) ) == sizeof( eocd ) ) {
            if( memcmp( &eocd.signature, "PK\005\006", 4 ) == 0 ) {
                if( WRESSEEK( hInstance->handle, eocd.cd_offset, SEEK_SET ) != -1 ) {
                    if( WRESREAD( hInstance->handle, &cdfh, sizeof( cdfh ) ) == sizeof( cdfh ) ) {
                        if( memcmp( &cdfh.signature, "PK\001\002", 4 ) == 0 ) {
                            offset += eocd.cd_offset + eocd.cd_size - cdfh.offset + sizeof( eocd );
                        }
                    }
                }
            }
        }
    }
    currpos = WRESSEEK( hInstance->handle, -offset, SEEK_END );
    for( ;; ) {
        WRESREAD( hInstance->handle, &header, sizeof( dbgheader ) );
        if( header.signature == WAT_RES_SIG ) {
            notfound = false;
            FileShift = currpos - header.debug_size + sizeof( dbgheader );
            break;
        } else if( header.signature == VALID_SIGNATURE ||
                   header.signature == FOX_SIGNATURE1 ||
                   header.signature == FOX_SIGNATURE2 ) {
            currpos -= header.debug_size;
            WRESSEEK( hInstance->handle, currpos, SEEK_SET );
        } else {        /* did not find the resource information */
            break;
        }
    }
    return( notfound );
}
