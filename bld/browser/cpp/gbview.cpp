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


#include "browse.h"
#include "viewmgr.h"
#include "gbview.h"
#include "gbvlist.h"
#include "gtclass.h"
#include "gtfunc.h"
#include "gbvolinh.h"
#include "gbvolcll.h"


static GlobalView * GlobalView::createView( GlobalViewType vt )
//-------------------------------------------------------------
{
    GlobalView *  view;

    switch( vt ) {
    case GlobalViewSymbolList:
        view = new GlobalViewList( "List" );
        break;

    case GlobalViewInheritTree:
        view = new TreeClassWindow( "Inheritance" );
        break;

    case GlobalViewInheritOutline:
        view = new GBVInheritOutline( "Inheritance" );
        break;

    case GlobalViewCallTree:
        view = new TreeFuncWindow( "Function Calls");
        break;

    case GlobalViewCallOutline:
        view = new GBVCallOutline( "Function Calls" );
        break;
    }

    return( view );
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

GlobalView::GlobalView()
//----------------------
{
}

GlobalView::~GlobalView()
//-----------------------
{
}

