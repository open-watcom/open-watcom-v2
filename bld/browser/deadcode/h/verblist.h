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


#ifndef __VERBLIST_H__
#define __VERBLIST_H__

// verblist.h - declaration of class VerboseListBox
//      modifies slightly the standard ListBox behavior by trapping keyDown.
//      if a key is pressed, the callback is called.  If this returns TRUE,
//      the key has been handled.  Otherwise, the default listbox keyDown
//      is called to handle it.

#include <wlistbox.hpp>
#include <wrect.hpp>
#include "wbrdefs.h"

typedef (WObject::*bcbwi)(WWindow*,int);

class VerboseListBox : public WListBox
{
public:
                VerboseListBox( WRect &, WWindow * parent );
                ~VerboseListBox(){}
    void        setCallbacks( WObject * client
                            ,cbw uponSelectMthd
                            ,bcbwi uponKeyMthd
                            ,cbw uponDblClickMthd );
    void        onKey( WObject * client, bcbwi uponKeyMthd );
    bool        keyDown( WKeyCode, WKeyState );
  private:
    bcbwi       _keyMthd;
    WObject *   _client;
};

#endif // __VERBLIST_H__
