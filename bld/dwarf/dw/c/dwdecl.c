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


#include "dwpriv.h"
#include "dwline.h"
#include "dwinfo.h"
#include "dwdecl.h"
#include "dwrefer.h"

void DWENTRY DWDeclFile(
    dw_client                   cli,
    const char *                name )
{
    cli->decl.file = GetFileNumber( cli, name );
    cli->decl.changed = 1;
    SetReferenceFile( cli, cli->decl.file );
}


void DWENTRY DWDeclPos(
    dw_client                   cli,
    dw_linenum                  line,
    dw_column                   column )
{
    cli->decl.line = line;
    cli->decl.column = column;
    cli->decl.changed = 1;
}


abbrev_code CheckDecl(
    dw_client                   cli,
    abbrev_code                 abbrev )
{
    if( cli->decl.changed && abbrev & AB_DECL ) return( AB_DECL );
    cli->decl.changed = 0;
    return( 0 );
}


void EmitDecl(
    dw_client                   cli )
{
    if( cli->decl.changed ) {
        /* AT_decl_file */
        InfoULEB128( cli, cli->decl.file );
        /* AT_decl_line */
        InfoULEB128( cli, cli->decl.line );
        /* AT_decl_column */
        InfoULEB128( cli, cli->decl.column );
        cli->decl.changed = 0;
    }
}


void InitDecl(
    dw_client                   cli )
{
    cli->decl.file = 1;
    cli->decl.line = 0;
    cli->decl.column = 0;
    cli->decl.changed = 0;
}


void FiniDecl(
    dw_client                   cli )
{
    cli = cli;
}
