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


#ifndef pinpclnt_h
#define pinpclnt_h

#include <object.h>
#include "wbrdefs.h"
class Pinup;
class CheckBox;

typedef Pinup& (Object::*CreatePinupFn)(int x, int y);
enum PinupAlignment {AlignRightEdge,AlignBottomEdge,AlignNone};

class PinupClient : public Object
{
    DeclareClass(PinupClient);
  public:
                PinupClient();
                PinupClient(Window& owner,CreatePinupFn
                    ,PinupAlignment,int offset=0);
    virtual     ~PinupClient();
    void        show(bool);
    void        clean(); // kills existin Pinup
    void        attachControl(CheckBox& checkBox, bool clickTrigger=TRUE);
                    //if clickTrigger is false, user must handle uponClick
                    // callbacks and call PinupClient::show explicitly;
                    // closing Pinup will still turn off checkBox
    void        align(); //align Pinup with _owner xywh, _offset & _alignment
    bool        attached(); //joined at the hip to my _owner?
    Pinup*      currentPinup();  // may return NIL
    Pinup&      getPinup();  // creates one if needed
    Pinup*      replacePinup(Pinup& newPinup);
    void        forcePushPin(); // tells Pinup to pushPin, if it exists

//  event handlers
    boolean     checkBoxClicked(CheckBox* clickee);
    void        pinupDied(Pinup& pinee);
    bool        pinupPinned(Pinup& pinee);
    bool        pinupMoved(Pinup& pinee);

  private:
    void        calcAlign(int* x, int* y); // asks _owner for coords
    void        initPinup();  // set callbacks, reset focus to _owner
    Window&     _owner;
    CreatePinupFn _create;
    Pinup*      _pinup;
    PinupAlignment _alignment;
    bool        _attached;
    int         _offset;
    CheckBox*   _checkBox;
    bool        _aligningPinup;
};

extern Class* PinupClientCls;

#endif // pinpclnt_h
