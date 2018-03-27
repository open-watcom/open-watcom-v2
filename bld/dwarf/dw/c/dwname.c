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


/*
    Handle the .debug_pubnames section
*/

#include "dwpriv.h"
#include "dwcliuti.h"
#include "dwhandle.h"
#include "dwname.h"


void DWENTRY DWPubname( dw_client cli, dw_handle hdl, const char *name )
{
    HandleReference( cli, hdl, DW_DEBUG_PUBNAMES );
    CLIWriteString( cli, DW_DEBUG_PUBNAMES, name );
}


void InitDebugPubnames( dw_client cli )
{
    /* write the set header */
    CLISectionReserveSize( cli, DW_DEBUG_PUBNAMES );
    CLIWriteU16( cli, DW_DEBUG_PUBNAMES, 2 );   /* section version */
    CLIReloc3( cli, DW_DEBUG_PUBNAMES, DW_W_SECTION_POS, DW_DEBUG_INFO );
    CLIReloc2( cli, DW_DEBUG_PUBNAMES, DW_W_UNIT_SIZE );
}


void FiniDebugPubnames( dw_client cli )
{
    /* write the terminator */
    CLISectionWriteZeros( cli, DW_DEBUG_PUBNAMES, sizeof( uint_32 ) );
    /* backpatch the section length */
    CLISectionSetSize( cli, DW_DEBUG_PUBNAMES );
}
