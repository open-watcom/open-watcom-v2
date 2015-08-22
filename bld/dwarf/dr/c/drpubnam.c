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
* Description:  DWARF .debug_pubnames section processing.
*
****************************************************************************/


#include "drpriv.h"
#include "drgettab.h"
#include "drutils.h"
#include <string.h>

#include "pushpck1.h"
typedef struct pubname_header {
    uint_32     len;
    uint_16     version;
    uint_32     dbg_pos;
    uint_32     dbg_length;
} _WCUNALIGNED pubname_header;
#include "poppck.h"

extern void DRWalkPubName( DRPUBWLK callback, void *data )
/********************************************************/
{
    dr_pubname_data     pubname;
    pubname_header      header;
    dr_handle           pos;
    dr_handle           finish;
    dr_handle           unit_end;
    uint_32             dbg_handle;
    dr_handle           dbg_base;
    char                str_buff[256];
    char                *str;
    int                 str_len;
    int                 curr_len;

    pos = DWRCurrNode->sections[DR_DEBUG_PUBNAMES].base;
    dbg_base = DWRCurrNode->sections[DR_DEBUG_INFO].base;
    finish = pos + DWRCurrNode->sections[DR_DEBUG_PUBNAMES].size;
    str = str_buff;
    str_len = sizeof( str_buff );
    while( pos < finish ) {
        DWRVMRead( pos, &header, sizeof( header ) );
        if( DWRCurrNode->byte_swap ) {
            SWAP_32( header.len );
            SWAP_16( header.version );
            SWAP_32( header.dbg_pos );
            SWAP_32( header.dbg_length );
        }
        if( header.version != DWARF_VERSION )
            DWREXCEPT( DREXCEP_BAD_DBG_VERSION );
        unit_end = pos + header.len + sizeof( uint_32 );
        pos += sizeof( header );
        pubname.dbg_cu = dbg_base + header.dbg_pos;
        pubname.is_start = TRUE;
        for( ;; ) {
            dbg_handle = DWRVMReadDWord( pos );
            if( dbg_handle == 0 )
                break;
            pos += sizeof( uint_32 );
            pubname.dbg_handle = pubname.dbg_cu + dbg_handle;
            curr_len = DWRVMGetStrBuff( pos, str, str_len );
            pubname.len = curr_len - 1;
            if( curr_len > str_len ) {
                if( str != str_buff ) {
                    DWRFREE( str );
                }
                str = DWRALLOC( curr_len );
                str_len = curr_len;
                curr_len = DWRVMGetStrBuff( pos, str, curr_len );
            }
            pos += curr_len;
            pubname.name = str;
            if( !callback( data,  &pubname ) ) {
                break;
            }
            pubname.is_start = FALSE;
        }
        pos = unit_end;
    }
    if( str != str_buff ) {
        DWRFREE( str );
    }
}
