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


// pinpclnt.cpp - implementation of class PinupClient

#include <checkbox.h>
#include "assure.h"
#include "pinup.h"
#include "pinpclnt.h"

#pragma warning 549 9; // shut up sizeof warning
defineClass(PinupClient,Object);
#pragma warning 549 4;

PinupClient::PinupClient()
    :_owner(*new Window)
    ,_create(NIL_METHOD)
    ,_alignment(AlignNone)
    ,_offset(0)
    ,_checkBox(NIL)
    ,_pinup(NIL)
    ,_attached(FALSE)
    ,_aligningPinup(FALSE)
{
}

PinupClient::PinupClient(Window& owner,CreatePinupFn create
                    ,PinupAlignment align ,int offset)
    :_owner(owner)
      ,_create(create)
    ,_alignment(align)
    ,_offset(offset)
    ,_checkBox(NIL)
    ,_pinup(NIL)
    ,_attached(TRUE)
    ,_aligningPinup(FALSE)
{
    REQUIRE(isptr(create),"PinupClient supplied NIL CreatePinupFn in constructor");
}

void PinupClient::initPinup()
{
    _pinup->setCallbacks(this
            ,(EventCB)&PinupClient::pinupPinned
            ,(DestroyCB)&PinupClient::pinupDied
            ,(EventCB)&PinupClient::pinupMoved
            );
    _attached = TRUE;
    if (_checkBox && !_checkBox->checked()) {
        _checkBox->check(TRUE);
    }
}

PinupClient::~PinupClient()
{
    if (_pinup) {
        _pinup->free();
    }
}

void PinupClient::attachControl(CheckBox& checkBox, bool clickTrigger)
{
    _checkBox = &checkBox;
    if (clickTrigger) {
        _checkBox->uponClick(this, methodOf(PinupClient,checkBoxClicked));
        show(_checkBox->checked());
    } else {
        // caller wants control of clicks
    }
}

Pinup* PinupClient::currentPinup()
{
    return _pinup;
}

Pinup& PinupClient::getPinup()  // creates one if needed
{
    if (!_pinup) {
        int x, y;
        calcAlign(&x,&y);
        _pinup = &(_owner.*_create)(x,y);
        _owner.takeFocus();
        initPinup();
    }
    return *_pinup;
}

Pinup* PinupClient::replacePinup(Pinup& newPinup)
{
    Pinup* oldPinup = _pinup;
    if (oldPinup != &newPinup) {
        _pinup = &newPinup;
        initPinup();
    }
    return oldPinup;
}

void PinupClient::forcePushPin()
{
    if (_pinup) {
        _pinup->pushPinned();
    }
}

void PinupClient::clean()
{
    if (_pinup) {
        _pinup->free();
    }
}

void PinupClient::show(bool shw)
{
    if (_pinup) {
        _pinup->showBool(shw);
    } else if(shw) {
        getPinup();
    }
}

void PinupClient::calcAlign(int* targetX, int* targetY)
{
    int x,y,w,h;
    _owner.getAbs(&x,&y,&w,&h);
    switch (_alignment) {
        case(AlignRightEdge) :
            *targetX=x+w;
            *targetY=y+_offset;
            break;
        case(AlignBottomEdge) :
            *targetX=x+_offset;
            *targetY=y+h;
            break;
        default:
            *targetX=x+20;
            *targetY=y+20;
    }
}

void PinupClient::align()
{
    if (_pinup && !_aligningPinup) {
        _aligningPinup = TRUE;
        int x,y,w,h,pinupX, pinupY, dummy;
        if (_attached) {
            calcAlign(&pinupX,&pinupY);
        } else {
            _owner.getAbs(&x,&y,&w,&h);
            _pinup->getAbs(&pinupX, &pinupY, &dummy, &dummy);
            if (abs(x+w-pinupX) < 10) {
                pinupX = x+w;
                _alignment = AlignRightEdge;
                _offset = pinupY - y;
                if (abs(_offset) < 10) {
                    _offset = 0;
                    pinupY = y;
                }
                _attached = TRUE;
            } else if (abs(y+h-pinupY) < 10) {
                pinupY = y+h;
                _alignment = AlignBottomEdge;
                _offset = pinupX - x;
                if (abs(_offset) < 10) {
                    _offset = 0;
                    pinupX = x;
                }
                _attached = TRUE;
            }
        }
        if (_attached) {
            _pinup->move(pinupX,pinupY);
            _pinup->bringToTop();
            _owner.takeFocus();
        }
        _aligningPinup = FALSE;
    }
}

bool PinupClient::attached()
{
    return _attached;
}

boolean PinupClient::checkBoxClicked(CheckBox* clickee)
{
    if (clickee->checked()) {
        getPinup().show();
    } else {
        if (_pinup) {
            _pinup->hide();
        }
    }
    return TRUE;
}

void PinupClient::pinupDied(Pinup& pinee)
{
    if (_pinup == &pinee) {
        _owner.takeFocus();
        _pinup->setCallbacks(NIL,NIL_METHOD,NIL_METHOD,NIL_METHOD);
        _pinup = NIL;
        if (_checkBox) {
            _checkBox->check(FALSE);
        }
    }
}

bool PinupClient::pinupPinned(Pinup& pinee)
{
    if (_pinup == &pinee) {
        _pinup->setCallbacks(NIL,NIL_METHOD,NIL_METHOD,NIL_METHOD);
        if (_attached) {
            _aligningPinup = TRUE;
            _attached = FALSE;
            int x,y,w,h;
            _pinup->getAbs(&x,&y,&w,&h);
            _pinup->move (x+11,y+11);
            _owner.takeFocus();
            _aligningPinup = FALSE;
        }
        _pinup = NIL;
    }
    return TRUE;
}

bool PinupClient::pinupMoved(Pinup& pinee)
{
    if (_pinup == &pinee && !_aligningPinup) {
        _attached = FALSE;  // force recheck
        align();
    }
    return TRUE;
}


