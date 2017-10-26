/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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


#include "dwpriv.h"
#include "dwcliuti.h"
#include "dwarange.h"

void DWENTRY DWAddress( dw_client cli, uint_32 len )
{
    CLIReloc2( cli, DW_DEBUG_ARANGES, DW_W_ARANGE_ADDR );
    if( cli->offset_size == sizeof( uint_32 ) ) {
        CLIWriteU32( cli, DW_DEBUG_ARANGES, len );
    } else {
        CLIWriteU16( cli, DW_DEBUG_ARANGES, len );
    }
}


void InitDebugAranges( dw_client cli )
{
    uint_8      buf[2];

    CLISectionReserveSize( cli, DW_DEBUG_ARANGES );
    CLIWriteU16( cli, DW_DEBUG_ARANGES, 2 );    /* section version */
    CLIReloc3( cli, DW_DEBUG_ARANGES, DW_W_SECTION_POS, DW_DEBUG_INFO );
    buf[0] = cli->offset_size;
    buf[1] = cli->segment_size;
    CLIWrite( cli, DW_DEBUG_ARANGES, buf, 2 );
}


void FiniDebugAranges( dw_client cli )
{
    CLISectionWriteZeros( cli, DW_DEBUG_ARANGES, cli->segment_size + 2 * cli->offset_size );
    /* backpatch the section length */
    CLISectionSetSize( cli, DW_DEBUG_ARANGES );
}
