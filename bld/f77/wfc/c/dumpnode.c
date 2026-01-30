/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "ftnstd.h"
#include "global.h"
#include "opr.h"
#include "dumpnode.h"


static const char   * const Opn[] = {
    #define pick(dsopn_id,opn_proc,opn_id) opn_id,
    #include "tokdsopn.h"
    #undef pick
};

static const char   * const Opr[] = {
    #define pick(id,opr_index,proc_index) #id,
    #include "oprdefn.h"
    #undef pick
};


static  void    LLine( void )
//=======================
{
    printf( "+-----------+------------+\n" );
}


static  void    Node( void )
//======================
{
    printf( "|  %s  |  %s  |\n", Opr[CITNode->opr], Opn[CITNode->opn.ds] );
}


void    DumpLex( void )
//=================
{
    LLine();
    while( CITNode != NULL ) {
        Node();
        LLine();
        CITNode = CITNode->link;
    }
}
