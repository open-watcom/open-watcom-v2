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


#include "drpriv.h"
#include "drgettab.h"
#include <string.h>

#pragma pack( push,1);
typedef struct arange_header {
    uint_32     len;
    uint_16     version;
    uint_32     dbg_pos;
    uint_8      addr_size;
    uint_8      seg_size;
} _WCUNALIGNED arange_header;
#pragma pack( pop );
/* function prototypes */

typedef struct sec_file {
    dr_handle pos;
    dr_handle finish;
}sec_file;


static uint_32 SectInt( sec_file *file, int size ){
    uint_32 ret;

    if( size == sizeof( uint_16 ) ){
        ret = DWRVMReadWord( file->pos );
    }else if( size == sizeof( uint_32 ) ){
        ret = DWRVMReadDWord( file->pos );
    }else if( size == sizeof( uint_8 ) ){
        ret = DWRVMReadByte( file->pos );
    }else{
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    file->pos += size;
    return( ret );
}


static void SectRead( sec_file *file, void *buff, int size ){
/************************************************************/

    DWRVMRead( file->pos, buff, size  );
    file->pos += size;
}



extern void DRWalkARange( DRARNGWLK callback, void *data ){
/*******************************/
    dr_arange_data      arange;
    sec_file            file[1];
    arange_header       header;
    dr_handle           base;

    base = DWRCurrNode->sections[DR_DEBUG_INFO].base;
    file->pos = DWRCurrNode->sections[DR_DEBUG_ARANGES].base;
    file->finish = file->pos + DWRCurrNode->sections[DR_DEBUG_ARANGES].size;
    for(;;) {
        if( file->pos >= file->finish )break;
        SectRead( file, &header, sizeof( header )  );
        if( header.version != DWARF_VERSION ) DWREXCEPT( DREXCEP_BAD_DBG_VERSION );
        arange.dbg = header.dbg_pos+base;
        arange.addr_size = header.addr_size;
        arange.seg_size = header.seg_size;
        arange.is_start = TRUE;   /* start of bunch */
        for(;;){
            arange.addr = SectInt( file,  header.addr_size );
            if( header.seg_size != 0 ){
                arange.seg = SectInt(  file,  header.seg_size );
            }else{ /* flat */
                arange.seg = 0;
            }
            arange.len = SectInt(  file,  header.addr_size );
            if( arange.addr == 0 && arange.len == 0 )break;
            if( !callback( data,  &arange ) )break;
            arange.is_start = FALSE;
        }
    }
}
