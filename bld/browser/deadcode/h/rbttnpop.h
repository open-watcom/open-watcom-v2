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



#ifndef rbttnpop_h
#define rbttnpop_h

#include <listbox.h>
#include "wbrdefs.h"

class RtButtonPopup : public ListBox
{
  public:
                RtButtonPopup(Window* parent
                        , char** labels, method* methods, id client=NIL);
                    // methods provides a method of type void (Object::*fn)(int index),
                    // one for each label
                    // client may be provided in popupAt() instead
                RtButtonPopup(Window* parent
                        , char** labels, method indexMethod, id client=NIL);
                    // indexMethod is a method of type void (Object::*fn)(int index),
                    // which gets called with index number of selected label
                    // client may be provided in popupAt() instead
    virtual     ~RtButtonPopup();
    void        popupAt(int x,int y, id client=NIL);
                    //NIL client doesn't change current client
    void        doFirstItem(id client=NIL);
  private:
    void        setup(char** labels);
    boolean     rmouseUp(int, int);
    boolean     mouseMv(int, int, int bStat);
    method*     _methods;
    method      _indexMethod;
    id          _client;
    int         _charH;
    static bool _mouseInMe;
};

#endif // rbttnpop_h
