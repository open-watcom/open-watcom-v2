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


#ifndef __DIALOG_H__

// ignore "temporary object used to initialize non-constant reference" warning
#pragma warning 665 9

#include <wstd.h>
#include <wcskip.h>

#define YYPARSER DialogParser
#include "yydriver.h"
#undef  YYPARSER

#include "rect.h"
#include "control.h"

template <class Type> class WCValOrderedVector;
template <class Type> class WCPtrSkipList;

class Dialog {

friend class Binding;

public:
                                        Dialog( const char * name, Rect rect );
                                        ~Dialog();

            int                         addRect( Rect & r );
            void                        setCaption( const char * text );
            void                        setFont( int ptsize, const char * text );
            void                        setStyle( long style );
            void                        addControl( const char * text, const char * id, const char * cls, long style, int rect );

private:
            const char *                _name;
            Rect                        _rect;
            const char *                _caption;

            int                         _fontSize;
            const char *                _fontName;

            uint_32                     _style;

            WCValOrderedVector<Rect> *  _rectangles;
            WCPtrSkipList<Control> *    _controls;
};


#define __DIALOG_H__
#endif
