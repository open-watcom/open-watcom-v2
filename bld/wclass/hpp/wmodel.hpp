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


#ifndef wmodel_class
#define wmodel_class

#include "wobject.hpp"
#include "wvlist.hpp"
#include "wview.hpp"

WCLASS WModel
{
        public:
                WEXPORT WModel();
                virtual WEXPORT ~WModel();
                WView* WEXPORT registerView( WView* view )
                        { return (WView*)_views.add( (WObject*)view ); } //illegal cast
                WView* WEXPORT deRegisterView( WView* view )
                        { return (WView*)_views.removeSame( (WObject*)view ); } //illegal cast
                void WEXPORT updateAllViews();
                void WEXPORT setUpdates( bool b=TRUE );
                SayReturn WEXPORT sayf( SayStyle style, SayCode code, const char* text... );
        private:
                WVList          _views;
                bool            _updates;
};

#endif
