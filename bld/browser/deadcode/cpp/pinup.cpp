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


// pinup.cpp - implementation of class Pinup

#include <bitmap.h>
#include <event.h>
#include <rect.h>
#include <notifier.h>
#include "pinup.h"

BitMap* Pinup::_bmUp=NIL;       // bitmap for up state
BitMap* Pinup::_bmDown=NIL;     // bitmap for down state
BitMap* Pinup::_bmPinned=NIL;    // bitmap for disabled state
int Pinup::_pinX = 23;// bitmap location rel to client 0
int Pinup::_pinY = 18;// bitmap location rel to client 0
int Pinup::_pinW = 20;// bitmap width
int Pinup::_pinH = 18;// bitmap height

#pragma warning 549 9; // shut up sizeof warning
defineClass(Pinup,PopupWindow);
#pragma warning 549 4;

Pinup::Pinup()
{
}

Pinup::Pinup(int x, int y, int w, int h, Window& prnt
            , bool shwPin )
    :_client(NIL)
    ,_pushPinCallback(NIL_METHOD)
    ,_movedCallback(NIL_METHOD)
    ,_destroyCallback(NIL_METHOD)
    ,_pinState(BttnUp)
    ,_showPin(shwPin)
{
    (*this)(Frame(x,y,w,h),&prnt,StyleCaption
                                |StyleSizable
                                |StyleCloseBox
                                |StyleMinimize
//                              |StyleMaximize
                                );

    if(!_bmUp && _showPin) {
        _bmUp = new BitMap("pinup");
        _bmDown = new BitMap("pindown");
        _bmPinned = new BitMap("pinned");
        int borderW    = GetSystemMetrics (SM_CXFRAME); //MS_WINDOWS
        int closeBoxW  = GetSystemMetrics (SM_CXSIZE); //MS_WINDOWS
        int closeBoxH  = GetSystemMetrics (SM_CYSIZE); //MS_WINDOWS
        _pinX = borderW + closeBoxW + 1;
        _pinY = borderW;
        _bmUp->sizeOfImage(&_pinW,&_pinH);
        if (_pinH > closeBoxH) _pinH = closeBoxH;
    }
    notifier->enableWinEvents(TRUE);
    if( _client ) {
        paintPin( BttnUp );
    } else {
        paintPin( BttnDisabled );
    }
}

Pinup::~Pinup()
{
    hide();
    disconnect();
}

void Pinup::disconnect()
{
    if (_client && _destroyCallback)  {
            (_client->*_destroyCallback)(*this);
    }
}

void Pinup::getInfo(WinInfo &info)
{
    PopupWindow::getInfo(info);
    info.style = WS_OVERLAPPEDWINDOW
                    |WS_VISIBLE
                    |WS_CLIPCHILDREN
                    |WS_MINIMIZEBOX
//                  |WS_MAXIMIZEBOX
                    |WS_THICKFRAME
                    |WS_SYSMENU;
}


bool Pinup::inPin(int qx, int qy)
{
    int x,y,w,h;
    getAbs(&x,&y,&w,&h);
    x += _pinX;
    y += _pinY;
    return ((qx >= x  && qx < x+_pinW) && (qy >= y  && qy < y+_pinH) ) ;
}

boolean Pinup::paintPin(BttnState newState)
{
    if (newState!=BttnDefault){
        _pinState = newState;
    }

    if (_showPin && !isHidden() && !isIconic()){
        int x,y,w,h;

        BitMap* map = NIL;

        switch (_pinState) {
            case BttnDown:
                map= _bmDown;
                break;
            case BttnUp:
                map= _bmUp;
                break;
            case BttnDisabled:
                map= _bmPinned;
                break;
        }

        if (map) {
            getAbs(&x,&y,&w,&h);

//          HDC globalDC = GetDC(NIL); // MS_WINDOWS - to draw on non-client area
            HDC titleDC = GetWindowDC(hWnd); // MS_WINDOWS - to draw on non-client area
            HDC srcDC = map->getHndl();
            BitBlt(titleDC,_pinX,_pinY,_pinW,_pinH,srcDC,0,0,SRCCOPY);// MS_WINDOWS
            ReleaseDC(hWnd,titleDC); //MS_WINDOWS
            map->ungetHndl();
        }
    }
    return TRUE;
}

long Pinup::winDispatch(Event& event)
{
    event.returnValue=FALSE;
    static mouseCaptured = FALSE;
    int x,y;
    switch (event.msg) {
        case WM_NCACTIVATE:
        case WM_NCPAINT:
            if (_showPin) {
                DefWindowProc(hWnd,event.msg,event.wParam,event.lParam);
                paintPin();
                event.returnValue=TRUE;
            }
            break;
        case WM_NCLBUTTONDOWN:
        case WM_NCRBUTTONDOWN:
            if (_showPin && _pinState != BttnDisabled) {
                x=LOWORD(event.lParam);
                y=HIWORD(event.lParam);
                if (inPin(x,y)) {
                    notifier->captureMouseFor(this);
                    mouseCaptured = TRUE;
                    _pinState = BttnDown;
                    DefWindowProc(hWnd,event.msg,event.wParam,event.lParam);
                    paintPin(BttnDown);
                    event.returnValue=TRUE;
                }
            }
            break;
        case WM_MOUSEMOVE:
            if (mouseCaptured) {
                x=LOWORD(event.lParam);
                y=HIWORD(event.lParam);
                mapToScreen(&x,&y);
                if (inPin(x,y)) {
                    if (_pinState!=BttnDown) {
                        paintPin(BttnDown);
                    }
                } else {
                    if (_pinState!=BttnUp) {
                        paintPin(BttnUp);
                    }
                }
            }
            break;
//      case WM_NCLBUTTONUP:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
            if (mouseCaptured) {
                notifier->releaseCapture();
                mouseCaptured=FALSE;
                x=LOWORD(event.lParam);
                y=HIWORD(event.lParam);
                _pinState = BttnUp;
                paintPin(BttnUp);
                mapToScreen(&x,&y);
                if (inPin(x,y)) {
                    pushPinned();
                    event.returnValue=TRUE;
                }
            }
            break;
        case WM_SIZE:
            // filter out events associated with min/max buttons
            MinMaxFlag minmax = MinMaxNone;
            if (isIconic()) {
                pushPinned();
                minmax = MinMaxIconic;
            } else if (IsZoomed(hwnd())) {  //MS_WINDOWS
                minmax = MinMaxZoomed;
            }
            event.returnValue=resized(LOWORD(event.lParam)
                                , HIWORD(event.lParam)
                                ,minmax);
            break;
    }
    return event.returnValue;
}

boolean    Pinup::key(int key, char ch)
{
    if (key == VK_ESCAPE) { //META_ESCAPE
        hide();
        free();
        return TRUE;
    } else {
        return PopupWindow::key(key,ch);
    }
}

void Pinup::setCallbacks(id client
        , EventCB pushPinCB
        , DestroyCB destroyCB
        , EventCB movedCB)
{
    _client = client;
    _pushPinCallback = pushPinCB;
    _movedCallback = movedCB;
    _destroyCallback = destroyCB;
    if( _client ) {
        paintPin( BttnUp );
    } else {
        paintPin( BttnDown );
    }
}

bool Pinup::pushPinned()
{
    if (_client && _pushPinCallback) {
        (_client->*_pushPinCallback)(*this);
    }
    paintPin(BttnDisabled);
    return TRUE;
}

bool Pinup::isIconic()
{
    return IsIconic(hwnd()); // MS_WINDOWS
}


boolean    Pinup::close()
{
    hide();
    free();
    return TRUE;
}

boolean Pinup::resized(int w, int h)
{
    int x, y, absW, absH;
    getAbs(&x, &y, &absW, &absH);
    winFrame.set(x, y, absW, absH);
    return PopupWindow::resized(w,h);
}

boolean Pinup::moved(int x,int y)
{
    if (_client && _movedCallback)
        (_client->*_movedCallback)(*this);
    return PopupWindow::moved(x,y);
}

