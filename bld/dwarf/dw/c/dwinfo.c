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
* Description:  Manage the .debug_info section.
*
****************************************************************************/


#include "dwpriv.h"
#include "dwcliuti.h"
#include "dwabbrev.h"
#include "dwmem.h"
#include "dwcnf.h"
#include "dwloc.h"
#include "dwinfo.h"


void InfoReloc( dw_client cli, uint reloc_type )
{
    CLIReloc2( cli, DW_DEBUG_INFO, reloc_type );
}


void Info8( dw_client cli, uint_8 value )
{
    CLIWriteU8( cli, DW_DEBUG_INFO, value );
}


void Info16( dw_client cli, uint_16 value )
{
    CLIWriteU16( cli, DW_DEBUG_INFO, value );
}


void Info32( dw_client cli, uint_32 value )
{
    CLIWriteU32( cli, DW_DEBUG_INFO, value );
}


void InfoLEB128( dw_client cli, dw_sconst value )
{
    CLIWriteLEB128( cli, DW_DEBUG_INFO, value );
}


void InfoULEB128( dw_client cli, dw_uconst value )
{
    CLIWriteULEB128( cli, DW_DEBUG_INFO, value );
}


void InfoBytes( dw_client cli, const void *buf, size_t size )
{
    CLIWrite( cli, DW_DEBUG_INFO, buf, size );
}


void InfoString( dw_client cli, const char *str )
{
    CLIWriteString( cli, DW_DEBUG_INFO, str );
}

void InfoEmitLocExprNull( dw_client cli, size_t size )
{
    EmitLocExprNull( cli, DW_DEBUG_INFO, size );
}

uint_32 InfoEmitLocExpr( dw_client cli, size_t size, dw_loc_handle loc )
{
    return( EmitLocExpr( cli, DW_DEBUG_INFO, size, loc ) );
}

void InfoHandleWriteOffset( dw_client cli, dw_handle hdl )
{
    HandleWriteOffset( cli, hdl, DW_DEBUG_INFO );
}

void InfoHandleReference( dw_client cli, dw_handle hdl )
{
    if( cli->compiler_options & DW_CM_BROWSER ) {
        DWReference( cli, cli->decl.line, cli->decl.column, hdl );
    }
    InfoHandleWriteOffset( cli, hdl );
}


void InitDebugInfo( dw_client cli )
{
    /* leave room for the length field */
    CLISectionReserveSize( cli, DW_DEBUG_INFO );
    Info16( cli, 2 );   /* section version */
    /* abbrev start */
    if( cli->compiler_options & DW_CM_ABBREV_PRE ) {
        CLIReloc4( cli, DW_DEBUG_INFO, DW_W_EXT_REF, cli->abbrev_sym, 0 );
    } else {
        CLISectionSeekAbs( cli, DW_DEBUG_ABBREV, 0 );
        CLIReloc3( cli, DW_DEBUG_INFO, DW_W_SECTION_POS, DW_DEBUG_ABBREV );
        CLISectionSeekEnd( cli, DW_DEBUG_ABBREV );
    }
    Info8( cli, cli->offset_size );
}


void FiniDebugInfo( dw_client cli )
{
    /* backpatch the section length */
    CLISectionSetSize( cli, DW_DEBUG_INFO );
}
