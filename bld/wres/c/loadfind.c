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
* Description:  Locate resources in an executable file.
*
****************************************************************************/


#include <string.h>
#include "wresall.h"
#include "wresset2.h"
#include "wresrtns.h"

/* Include patch signature header shared with BPATCH */
#include "patchsig.h"

#define VALID_SIGNATURE 0x8386
#define FOX_SIGNATURE1  0x8300
#define FOX_SIGNATURE2  0x8301
#define WAT_RES_SIG     0x8302

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
#include "poppck.h"

WResFileOffset    FileShift = 0;

unsigned char FindResources( PHANDLE_INFO hInstance )
/* look for the resource information in a debugger record at the end of file */
{
    WResFileOffset  currpos;
    WResFileOffset  offset;
    dbgheader       header;
    bool            notfound;
    char            buffer[sizeof( PATCH_LEVEL )];

    notfound = true;
    FileShift = 0;
    offset = sizeof( dbgheader );
    if( WRESSEEK( hInstance->handle, -(WResFileOffset)sizeof( PATCH_LEVEL ), SEEK_END ) != -1 ) {
        if( WRESREAD( hInstance->handle, buffer, sizeof( PATCH_LEVEL ) ) == sizeof( PATCH_LEVEL ) ) {
            if( memcmp( buffer, PATCH_LEVEL, PATCH_LEVEL_HEAD_SIZE ) == 0 ) {
                offset += sizeof( PATCH_LEVEL );
            }
        }
    }
    WRESSEEK( hInstance->handle, -offset, SEEK_END );
    currpos = WRESTELL( hInstance->handle );
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
