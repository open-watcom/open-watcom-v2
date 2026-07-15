/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
#include "machtype.h"
#include "wdbginfo.h"
/* Include patch signature header shared with BPATCH */
#include "patchsig.h"


#define SEEK_POSBACK(p)     (-(long)(p))

long                WResFileShift = 0;

bool FindResourcesX( PHANDLE_INFO hinfo, bool res_file )
/*************************************************************
 * set position of resource info in the file (WResFileShift)
 * it is 0 if it is external resource file (GUI project)
 * or look for the resource information in a debugger record at the end of file
 */
{
    long                currpos;
    long                offset;
    bool                error;
    char                buffer[sizeof( PATCH_LEVEL )];
    uint_16             sign;
    uint_32             size;

    error = false;
    WResFileShift = 0;
    if( !res_file ) {
        /* Look for a PATCH header and skip block if present */
        offset = sizeof( master_dbg_header );
        if( !WRESSEEK( hinfo->fp, SEEK_POSBACK( sizeof( PATCH_LEVEL ) ), SEEK_END ) ) {
            if( WRESREAD( hinfo->fp, buffer, sizeof( PATCH_LEVEL ) ) == sizeof( PATCH_LEVEL ) ) {
                if( memcmp( buffer, PATCH_LEVEL, PATCH_LEVEL_HEAD_SIZE ) == 0 ) {
                    offset += sizeof( PATCH_LEVEL );
                }
            }
        }
        error = WRESSEEK( hinfo->fp, SEEK_POSBACK( offset ), SEEK_END );
        if( error )
            return( true );
        currpos = WRESTELL( hinfo->fp );
        for( ;; ) {
            sign = ResReadUint16( &error, hinfo->fp );
            if( error )
                break;
            error = WRESSEEK( hinfo->fp, offsetof( master_dbg_header, debug_size ) - sizeof( uint_16 ), SEEK_CUR );
            if( error )
                break;
            size = ResReadUint32( &error, hinfo->fp );
            if( error )
                break;
            if( sign == WAT_RES_SIG ) {
                WResFileShift = currpos - size + sizeof( master_dbg_header );
                break;
            } else if( sign == WAT_DBG_SIGNATURE
              || sign == FOX_SIGNATURE1
              || sign == FOX_SIGNATURE2 ) {
                currpos -= size;
                error = WRESSEEK( hinfo->fp, currpos, SEEK_SET );
                if( error ) {
                    break;
                }
            } else {        /* did not find the resource information */
                error = true;
                break;
            }
        }
    }
    return( error );
}

bool FindResources( PHANDLE_INFO hinfo )
{
    return( FindResourcesX( hinfo, false ) );
}
