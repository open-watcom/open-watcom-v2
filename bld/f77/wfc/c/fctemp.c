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
* Description:  CG routines for assigning a cg_name to a temporary
*
****************************************************************************/


#include "ftnstd.h"
#include "cg.h"
#include "wf77defs.h"
#include "tmpdefs.h"
#include "global.h"
#include "fcdatad.h"
#include "model.h"
#include "cgprotos.h"
#include "fmemmgr.h"
#include "chain.h"
#include "fctemp.h"


//=========================================================================

static  tmp_handle      TmpList;


void    InitTmps( void ) {
//==================

// Initialize temporary management.

    TmpList = NULL;
}


void    FiniTmps( void ) {
//==================

// Finalize temporary management.

    FreeChain( &TmpList );
}


tmp_handle      AllocTmp( cg_type typ ) {
//=======================================

// Allocate a temporary.

    tmp_handle  tmp;

    for( tmp = TmpList; tmp != NULL; tmp = tmp->link ) {
        if( tmp->avail &&
            ( BETypeLength( typ ) <= BETypeLength( tmp->typ ) ) ) {
            tmp->avail = false;
            return( tmp );
        }
    }
    tmp = FMemAlloc( sizeof( tmp_tracker ) );
    tmp->tmp = CGTemp( typ );
    tmp->typ = typ;
    tmp->avail = false;
    tmp->link = TmpList;
    TmpList = tmp;
    return( tmp );
}


void    FreeTmps( void ) {
//==================

// Free all temporaries after compiling a statement.

    tmp_handle  tmp;

    for( tmp = TmpList; tmp != NULL; tmp = tmp->link ) {
        tmp->avail = true;
    }
}


cg_name TmpPtr( tmp_handle tmp, cg_type typ ) {
//=============================================

// Return the pointer to a temporary.

    return( CGTempName( tmp->tmp, typ ) );
}


tmp_handle      MkTmp( cg_name val, cg_type typ ) {
//=================================================

// Allocate a temporary and store into it.

    tmp_handle  tmp;

    tmp = AllocTmp( typ );
    CGTrash( CGAssign( TmpPtr( tmp, typ ), val, typ ) );
    return( tmp );
}


cg_name TmpVal( tmp_handle tmp, cg_type typ ) {
//=============================================

// Return the value of a temporary.

    return( CGUnary( O_POINTS, TmpPtr( tmp, typ ), typ ) );
}


void    CloneCGName( cg_name original, cg_name *clone_1, cg_name *clone_2 ) {
//===========================================================================

// Duplicate a cg-name.

    cg_name     *clones;

    clones = CGDuplicate( original );
    *clone_1 = clones[0];
    *clone_2 = clones[1];
}
