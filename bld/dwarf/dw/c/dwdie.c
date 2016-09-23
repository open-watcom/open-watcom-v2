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
#include "dwinfo.h"
#include "dwabbrev.h"
#include "dwmem.h"
#include "dwdecl.h"
#include "dwrefer.h"
#include "dwutils.h"
#include "dwdie.h"


void InitDIE( dw_client cli )
{
    cli->die.tree = NULL;
}


void FiniDIE( dw_client cli )
{
    cli = cli;
    _Assert( cli->die.tree == NULL );
}


void StartChildren( dw_client cli )
{
    die_tree        *new;

    /* make a new entry in the tree */
    new = CLIAlloc( cli, sizeof( *new ) );
    new->parent = cli->die.tree;
    cli->die.tree = new;

    /* prepare for sibling relocation */
    new->sibling = 0;
}


static void doTheSiblingThing( dw_client cli )
{
    debug_ref       sibling;
    char            buf[sizeof( debug_ref )];

    sibling = cli->die.tree->sibling;
    if( sibling == 0 )
        return;  /* no previous sibling */

    /* relocate previous sibling */
    WriteRef( buf, InfoSkip( cli, 0 ) );
    InfoPatch( cli, sibling, buf, sizeof( buf ) );
}


void EndChildren( dw_client cli )
{
    doTheSiblingThing( cli );

    /* move up a level in the tree */
    cli->die.tree = FreeLink( cli, cli->die.tree );

    Info8( cli, 0 );
}


void StartDIE( dw_client cli, abbrev_code abbrev )
{
    int         haskids;
    unsigned    abbrevnum;

    doTheSiblingThing( cli );

    if( abbrev & AB_START_REF ) {
        StartRef( cli );
    }

    haskids = (abbrev & AB_SIBLING) != 0;       // ab_sibling in "always on"
    abbrev &= ~AB_ALWAYS;                       // class of attributes

    /* emit the abbreviation for this DIE */
    abbrevnum = MarkAbbrevAsUsed( cli, &abbrev );
    InfoULEB128( cli, abbrevnum );

    /* AT_sibling reference */
    if( haskids ) {
        cli->die.tree->sibling = InfoSkip( cli, sizeof( debug_ref ) );
    } else {
        cli->die.tree->sibling = 0;
    }

    /* AT_decl_* */
    if( abbrev & AB_DECL ) {
        EmitDecl( cli );
    }
}


void EndDIE( dw_client cli )
{
    cli = cli;
}
