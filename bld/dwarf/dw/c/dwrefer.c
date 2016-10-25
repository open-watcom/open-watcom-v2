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
* Description:  Process referecnes.
*
****************************************************************************/


#include "dwpriv.h"
#include "dwrefer.h"
#include "dwhandle.h"
#include "dwutils.h"
#include "dwmem.h"

/*
    We use these delayed_ref structures to avoid emitting sequences such
    as:
        REF_BEGIN_SCOPE
        REF_END_SCOPE
    (Basically to avoid emitting null reference information.)
*/

struct delayed_ref {
    struct delayed_ref          *next;
    debug_ref                   offset;
    uint                        scope;
};


static void emitDelayed( dw_client cli )
{
    struct delayed_ref          *cur;
    uint_8                      buf[1 + MAX_LEB128];
    uint_8                      *end;

    /* delayed_refs are stacked up; we want to emit them in FIFO order */
    for( cur = ReverseChain( cli->references.delayed ); cur != NULL; cur = CarveFreeLink( cli->references.delay_carver, cur ) ) {
        buf[0] = REF_BEGIN_SCOPE;
        WriteRef( buf + 1, cur->offset );
        CLIWrite( cli, DW_DEBUG_REF, buf, 1 + sizeof( debug_ref ) );
    }
    cli->references.delayed = 0;
    if( cli->references.delayed_file ) {
        buf[0] = REF_SET_FILE;
        end = ULEB128( buf + 1, cli->references.delayed_file );
        CLIWrite( cli, DW_DEBUG_REF, buf, end - buf );
        cli->references.delayed_file = 0;
    }
}


void StartRef( dw_client cli )
{
    struct delayed_ref          *new;

    /*
        We just stack up the StartRef until we find that we have to actually
        emit it.
    */
    new = CarveAlloc( cli, cli->references.delay_carver );
    new->next = cli->references.delayed;
    cli->references.delayed = new;
    new->offset = CLITell( cli, DW_DEBUG_INFO ) - cli->section_base[DW_DEBUG_INFO];
    new->scope = cli->references.scope;
    ++cli->references.scope;
}


void EndRef( dw_client cli )
{
    struct delayed_ref *        this;
    char                        buf[1];

    /*
        We have to check if we actually emitted a REF_START_REF
        for this scope
    */
    --cli->references.scope;
    this = cli->references.delayed;
    if( this != NULL && this->scope == cli->references.scope ) {
        cli->references.delayed = CarveFreeLink( cli->references.delay_carver, this );
    } else {
        buf[0] = REF_END_SCOPE;
        CLIWrite( cli, DW_DEBUG_REF, buf, sizeof( buf ) );
    }
}



void DWENTRY DWReference( dw_client cli, dw_linenum line, dw_column column, dw_handle dependant )
{
    dw_linenum_delta            line_delta;
    dw_column_delta             column_delta;
    uint_8                      buf[1 + MAX_LEB128];
    uint_8                      *end;

    /*
        We actually have a reference for this scope, so emit all the
        delayed REF_START_REFS.
    */
    emitDelayed( cli );

    line_delta = line - cli->references.line;
    cli->references.line = line;
    if( line_delta < 0 || line_delta >= ( 255 - REF_CODE_BASE ) / REF_COLUMN_RANGE ) {
        buf[0] = REF_ADD_LINE;
        end = LEB128( buf + 1, line_delta );
        CLIWrite( cli, DW_DEBUG_REF, buf, end - buf );
        cli->references.column = 0;
        line_delta = 0;
    } else if( line_delta != 0 ) {
        cli->references.column = 0;
    }
    column_delta = column - cli->references.column;
    cli->references.column = column;
    if( column_delta < 0 || column_delta >= REF_COLUMN_RANGE ) {
        buf[0] = REF_ADD_COLUMN;
        end = LEB128( buf + 1, column_delta );
        CLIWrite( cli, DW_DEBUG_REF, buf, end - buf );
        column_delta = 0;
    }
    _Assert( line_delta >= 0
        && line_delta * REF_COLUMN_RANGE <= 255 - REF_CODE_BASE-REF_COLUMN_RANGE
        && column_delta >= 0
        && column_delta < REF_COLUMN_RANGE );
    buf[0] = REF_CODE_BASE + line_delta * REF_COLUMN_RANGE + column_delta;
    CLIWrite( cli, DW_DEBUG_REF, buf, 1 );
    HandleReference( cli, dependant, DW_DEBUG_REF );
}


void SetReferenceFile( dw_client cli, uint file )
{
    cli->references.delayed_file = file;
}


void InitReferences( dw_client cli )
{
    cli->references.line = 1;
    cli->references.column = 1;
    cli->references.delayed = NULL;
    cli->references.delay_carver = CarveCreate( cli, sizeof( struct delayed_ref ), 16 );
    cli->references.scope = 0;
    cli->references.delayed_file = 0;

    CLISeek( cli, DW_DEBUG_REF, sizeof( uint_32 ), DW_SEEK_CUR );
}


void FiniReferences( dw_client cli )
{
    char                        buf[sizeof( uint_32 )];
    long                        size;

    size = CLITell( cli, DW_DEBUG_REF ) - sizeof( uint_32 ) - cli->section_base[DW_DEBUG_REF];
    WriteU32( buf, size );
    CLISeek( cli, DW_DEBUG_REF, cli->section_base[DW_DEBUG_REF], DW_SEEK_SET );
    CLIWrite( cli, DW_DEBUG_REF, buf, sizeof( buf ) );
    CLISeek( cli, DW_DEBUG_REF, 0, DW_SEEK_END );

    CarveDestroy( cli, cli->references.delay_carver );
}
