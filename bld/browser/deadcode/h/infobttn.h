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


#ifndef infobttn_h
#define infobttn_h

// infobttn.h - declaration of class InfoButton
// displays name of Symbol supplied in constructor, creates a
// ViewSym from the Symbol if it is pressed

#include <wstring.hpp>
#include <wpshbttn.hpp>
#include "wbrdefs.h"

class Symbol;

typedef void (WObject::*InfoRqstCB)(WObject *info);

class InfoButton : public WPushButton
{
public:
                InfoButton();
                    // ctors modify w,h to indicate final size based on text
                InfoButton( char * text, WWindow * parent, int x, int y,
                            int & w, int & h );
                InfoButton( Symbol * info, WWindow * parent, int x,
                            int y, int & w, int & h );
    void        setInfoRqstCB( InfoRqstCB cb, WObject * client );
    virtual     ~InfoButton(); // frees _info

    // event handlers
    void        clicked( WWindow * );

  private:
    void        setup( const char * text, int & w, int & h);
    Symbol *    _info;
    InfoRqstCB  _showInfo;
    WObject *   _myclient;
};


#endif
