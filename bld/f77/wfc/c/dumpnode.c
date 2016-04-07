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


#include "ftnstd.h"
#include "global.h"
#include "opr.h"
#include "dumpnode.h"

#include <stdio.h>


static  char    *Opn[] = {
    "OPN_PHI",
    "OPN_NAM",
    "OPN_LIT",
    "OPN_HOL",
    "OPN_LGL",
    "OPN_INT",
    "OPN_REA",
    "OPN_DBL"
    "OPN_EXT"
    "OPN_OCT"
    "OPN_HEX"
    "OPN_FMT"
};


static  char    *Opr( byte opr ) {
//================================

    switch( opr ) {
    case OPR_PHI :
        return( "OPR_PHI " );
    case OPR_TRM :
        return( "OPR_TRM " );
    case OPR_FBR :
        return( "OPR_FBR " );
    case OPR_LBR :
        return( "OPR_LBR " );
    case OPR_COM :
        return( "OPR_COM " );
    case OPR_COL :
        return( "OPR_COL " );
    case OPR_EQU :
        return( "OPR_EQU " );
    case OPR_EQV :
        return( "OPR_EQV " );
    case OPR_NEQV :
        return( "OPR_NEQV" );
    case OPR_OR :
        return( "OPR_OR  " );
    case OPR_AND :
        return( "OPR_AND " );
    case OPR_NOT :
        return( "OPR_NOT " );
    case OPR_EQ :
        return( "OPR_EQ  " );
    case OPR_NE :
        return( "OPR_NE  " );
    case OPR_LT :
        return( "OPR_LT  " );
    case OPR_GE :
        return( "OPR_GE  " );
    case OPR_LE :
        return( "OPR_LE  " );
    case OPR_GT :
        return( "OPR_GT  " );
    case OPR_PLS :
        return( "OPR_PLS " );
    case OPR_MIN :
        return( "OPR_MIN " );
    case OPR_MUL :
        return( "OPR_MUL " );
    case OPR_DIV :
        return( "OPR_DIV " );
    case OPR_EXP :
        return( "OPR_EXP " );
    case OPR_CAT :
        return( "OPR_CAT " );
    case OPR_RBR :
        return( "OPR_RBR " );
    case OPR_DPT :
        return( "OPR_DPT " );
    case OPR_AST :
        return( "OPR_AST " );
    case OPR_FLD :
        return( "OPR_FLD " );
    default :
        return( "UNKNOWN " );
    }
}


static  void    LLine( void ) {
//=======================

    printf( "+-----------+------------+\n" );
}


static  void    Node( void ) {
//======================

    printf( "|  %s  |  %s  |\n", Opr( CITNode->opr ), Opn[ CITNode->opn.ds ] );
}


void    DumpLex( void ) {
//=================

    LLine();
    while( CITNode != NULL ) {
        Node();
        LLine();
        CITNode = CITNode->link;
    }
}



