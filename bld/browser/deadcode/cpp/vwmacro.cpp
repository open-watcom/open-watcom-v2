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


#include <wpshbttn.hpp>
#include <wlistbox.hpp>
#include <wcheckbx.hpp>
#include <wtext.hpp>
#include "wbrdefs.h"
#include "assure.h"
#include "symbol.h"
#include "vwcreate.h"
#include "macrosym.h"
#include "vwmacro.h"


ViewMacro::ViewMacro( int x, int y, WBRWindow * prnt, const Symbol *info,
                      bool derived )
    :ViewSymbol( x, y, prnt, info, TRUE/*derived call*/)
{
    if( !derived ) {
        refresh();
        setup();
    } // else derived class will call these
}

void ViewMacro::setup()
/*********************/
{
    WVList & kids = children();
    ulong startIndex = kids.count();

    ViewSymbol::setup();

    WText * label = new WText( this, WRect(viewLeft,0,100,viewHeight),
                               "Definition:" );
//  label->setFont(stdFont);
    label->show();

    label = new WText( this, WRect(viewLeft,0,-4,30),
                             ((MacroSym *)_model)->value() );
//  label->setFont(stdFont);
    label->show();
    arrangeKids( startIndex );
}

ViewSymbol * ViewMacro::create( int x, int y, WBRWindow *prnt,
                                              const Symbol *info )
/****************************************************************/
{
    return new ViewMacro( x, y, prnt, info );
}
