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


#include "wmodel.hpp"
#include "wstring.hpp"

extern "C" {
    #include <stdio.h>
    #include <stdarg.h>
}

#define MAX_MESSAGE 500

WEXPORT WModel::WModel()
        : _updates( true )
{
}

WEXPORT WModel::~WModel()
{
        int icount = _views.count();
        for( int i=0; i<icount; i++ ) {
                ((WView*)_views[i])->vModelGone(); //illegal cast
        }
}

void WEXPORT WModel::updateAllViews()
{
    if( _updates ) {
        int icount = _views.count();
        for( int i=0; i<icount; i++ ) {
                ((WView*)_views[i])->updateView();  //illegal cast
        }
    }
}

void WEXPORT WModel::setUpdates( bool b )
{
    _updates = b;
    updateAllViews();
}

SayReturn WEXPORT WModel::sayf( SayStyle style, SayCode code, const char* text... )
{
    SayReturn ret = RetOk;
    if( _views.count() > 0 ) {
        char* txt = new char[MAX_MESSAGE + 1];
        va_list args;
        va_start( args, text );
        vsprintf( txt, text, args );
        ret = ((WView*)_views[0])->say( style, code, txt );  //illegal cast
        delete [] txt;
    }
    return( ret );
}
