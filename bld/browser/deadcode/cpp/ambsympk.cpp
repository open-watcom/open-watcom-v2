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


#include "symbol.h"
#include "assure.h"
#include "ambsympk.h"

#define startW 200
#define startH 180

AmbigSymPick::AmbigSymPick( WVList & infos ,int x, int y )
    : ObjectPickDlg( x, y, startW, startH, infos,
                (nameMethod) &Symbol::scopedName,
                *this , (pickCallback) &AmbigSymPick::choose,
                (pickCallback) &AmbigSymPick::popObject,
                "Ambiguous name matches:" )
{
    long cnt = _objList->count();
    if( cnt ){
        _objList->select( cnt-1 );
    }
}

void AmbigSymPick::choose( WObject * )
//------------------------------------
// Called when the user made a selection from the list box.
{
    // NYI: possible pinned window communication
}

void AmbigSymPick::dblClick( WObject * selected )
//-----------------------------------------------
// Called when the user double clicked on item in the list box.
{
    ASSERT(selected != NULL,"AmbigSymPick::select got NULL object",-1);
    popDetail( (const Symbol *) selected );
}
