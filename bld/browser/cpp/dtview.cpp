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


#include "assure.h"
#include "viewmgr.h"
#include "dtview.h"
#include "dtvsym.h"
#include "dtvsrref.h"
#include "dtvsmref.h"
#include "util.h"

#ifdef DETAIL_STUFF_IMPLEMENTED
#  include "fnctxttr.h"
#  include "clstxttr.h"
#  include "strcview.h"
#endif

bool DetailView::aboutToClose()
//-----------------------------
{
    viewManager()->viewDying( this );
    return TRUE;
}

bool DetailView::focusChange( bool gettingFocus )
//-----------------------------------------------
{
    if( gettingFocus ) {
        viewManager()->eventOccured( VEGettingFocus, this );
    } else {
        viewManager()->eventOccured( VELosingFocus, this );
    }
    return FALSE;
}

static DetailView * DetailView::createView( const Symbol * sym, ViewType vt )
//---------------------------------------------------------------------------
{
    switch( vt ) {
    case ViewDetail:
        return DTViewSymbol::createView( sym );

    case ViewSourceRefs:
        return new DTVSourceRefs( sym );

    case ViewSymbolRefs:
        return new DTVSymbolRefs( sym );

#ifdef DETAIL_STUFF_IMPLEMENTED
    case ViewInherit:
        new ClassTextTree( 100, 100, (Symbol *) sym );  // FIXME -- cast away const
        return NULL;
    case ViewStructure:
        new StrucView( 100, 100, (Symbol *) sym );      // FIXME -- cast away const
        return NULL;
    case ViewCalls:
        new FuncTextTree( 100, 100, (Symbol *) sym );   // FIXME -- cast away const
        return NULL;
#endif

    default:
        NODEFAULT;
        return NULL;
    }
}

// Complain about defining trivial constructor/destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

DetailView::DetailView()
//----------------------
{
}

DetailView::~DetailView()
//-----------------------
{
}

