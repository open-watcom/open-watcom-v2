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


#ifndef pinup_h
#define pinup_h

#include <poppwndw.h>
#include "callback.h"
#include "wbrdefs.h"

enum BttnState{BttnUp,BttnDown,BttnDisabled,BttnDefault};
enum MinMaxFlag{MinMaxNone,MinMaxZoomed,MinMaxIconic};

class Event;

class Pinup : public PopupWindow
{
    DeclareClass(Pinup);
  public:
                    Pinup();
                    Pinup(int x, int y, int w, int h, Window& parent
                        , bool showPin=TRUE );
    virtual         ~Pinup();
    void            getInfo(WinInfo&);

    void            setCallbacks(id client
                        , EventCB pushPinCB
                        , DestroyCB destroyCB
                        , EventCB movedCB=NIL_METHOD);
    virtual bool    pushPinned();
    void            showPin(bool show){_showPin = show;}
    bool            isIconic();

    // event handlers
    boolean         close();
    boolean         moved(int x, int y);
    bool            attached() const {return isptr(_client);}
    boolean         key(int key, char ch); // close on ESCAPE
    boolean         resized(int w, int h); // saves size in winFrame
        // pinups often care if resize event is zoom or icon;
    virtual boolean resized(int w, int h, MinMaxFlag){return resized(w,h);}

  private:
    boolean         paintPin(BttnState pinState = BttnDefault);
    bool            inPin(int x, int y); // screen coord
    void            disconnect();
    long            winDispatch(Event&);
    EventCB         _pushPinCallback;
    EventCB         _movedCallback;
    DestroyCB       _destroyCallback;
    id              _client;
    BttnState       _pinState;
    bool            _showPin;
    static BitMap*  _bmUp;    // bitmap for up state
    static BitMap*  _bmDown;// bitmap for down state
    static BitMap*  _bmPinned;// bitmap for disabled state
    static int      _pinX;// bitmap location rel to client 0
    static int      _pinY;// bitmap location rel to client 0
    static int      _pinW;// bitmap width
    static int      _pinH;// bitmap height
};

extern Class* PinupCls;

#endif // pinup_h
