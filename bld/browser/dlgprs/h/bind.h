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


#ifndef __BIND_H__

#ifdef __WATCOMC__
// ignore "temporary object used to initialize non-constant reference" warning
#pragma warning 665 9
#endif

#include "wstd.h"
#include <vector>

#define YYPARSER BindingParser
#include "yydriver.h"
#undef  YYPARSER

#include "rect.h"
#include "control.h"

enum RectCoords {
    Absolute,
    Relative
};

class Dialog;
class CheckedBufferedFile;

class Binding {
public:
                                        Binding( const char * name );
                                        ~Binding();

            size_t                      addAbsRelRect( const Rect & r );
            void                        addControl( const char * text, const char * id, int absrelrect );

            void                        bind( Dialog * dlg, const char * header, const char * cpp );
            void                        writeTimeStamp( CheckedBufferedFile & );
private:
            void                        bindHeader( const char * header );
            void                        bindSource( Dialog * dlg, const char * cpp, const char * header );

            const char                  *_name;
            std::vector<Rect>           *_rectangles;
            std::vector<Control>        *_controls;
};

#define __BIND_H__
#endif

