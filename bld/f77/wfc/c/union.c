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


//
// UNION        : UNION/MAP statement processor
//

#include "ftnstd.h"
#include "global.h"
#include "errcod.h"
#include "segsw.h"
#include "recog.h"
#include "insert.h"
#include "utility.h"
#include "csutls.h"
#include "rststruc.h"
#include "proctbl.h"
#include "union.h"


extern  char            *StmtKeywords[];


void    EndOfStatement( void ) {
//========================

    ReqNOpn();
    AdvanceITPtr();
    ReqEOS();
}


void    CpUnion( void ) {
//=================

// Process UNION statement.
//      UNION

    CSExtn();
    SgmtSw |= SG_DEFINING_UNION;
    EndOfStatement();
    STUnion();
}


void    CpEndUnion( void ) {
//====================

// Process ENDUNION statement.
//      ENDUNION

    CSExtn();
    if( (SgmtSw & SG_DEFINING_STRUCTURE) == 0 ) {
        StmtPtrErr( SP_UNMATCHED, StmtKeywords[ PR_UNION ] );
    }
    SgmtSw &= ~SG_DEFINING_UNION;
    EndOfStatement();
}


void    CpMap( void ) {
//===============

// Process MAP statement.
//      MAP

    CSExtn();
    SgmtSw |= SG_DEFINING_MAP;
    EndOfStatement();
    STMap();
}


void    CpEndMap( void ) {
//====================

// Process ENDMAP statement.
//      ENDMAP

    CSExtn();
    if( (SgmtSw & SG_DEFINING_MAP) == 0 ) {
        StmtPtrErr( SP_UNMATCHED, StmtKeywords[ PR_MAP ] );
    }
    SgmtSw &= ~SG_DEFINING_MAP;
    EndOfStatement();
}

