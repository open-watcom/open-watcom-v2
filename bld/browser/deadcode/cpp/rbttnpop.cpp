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



#include <string.h>
#include <poppwndw.h>
#include <notifier.h>
#include "rbttnpop.h"

bool RtButtonPopup::_mouseInMe=FALSE;

RtButtonPopup::RtButtonPopup(Window* prnt, char** labels
                , method indexMethod, id client)
        :ListBox(Frame(0.0F,0.0F,1.0F,1.0F)
                        ,new PopupWindow(Frame(100,100,5,5), prnt, StyleBorder|StyleHidden)
                        ,StyleDefault)
        ,_client(client)
        ,_indexMethod(indexMethod)
        ,_methods(NIL)
{
        setup(labels);
}

RtButtonPopup::RtButtonPopup(Window* prnt, char** labels, method* methods, id client)
        :ListBox(Frame(0.0F,0.0F,1.0F,1.0F)
                        ,new PopupWindow(Frame(100,100,5,5), prnt, StyleBorder|StyleHidden)
                        ,StyleDefault)
        ,_client(client)
        ,_indexMethod(NIL)
        ,_methods(methods)
{
        setup(labels);
}

RtButtonPopup::~RtButtonPopup()
{
        Window* frame = getParent();
        // do my cleanup, if any
        frame->removeChild(this);  // avoid double delete
        frame->free();
}

void RtButtonPopup::setup(char** labels)
{
        Window* frame = getParent();
        appendList(labels);
        int cw;
        fontSize(&cw,&_charH);
        int w,h;
        w=h=0;
        int i=0;
        while(labels[i]) {
                h+=_charH;
                int len = strlen(labels[i]) * cw;
                if (len > w) w=len;
                i++;
        }
        frame->size(w+10,h+2);
}

void RtButtonPopup::popupAt(int x, int y, id client)
{
        if(client) {
                _client = client;
        }
        Window* frame = getParent();
        frame->move(x-5,y-10);
        frame->show(); // redundant ?
        frame->bringToTop();
//      frame->getParent()->takeFocus();

        if (count() > 0) {
                selectItem(0);
        }
        notifier->mouseTracking(TRUE);
        notifier->captureMouseFor(this);
        _mouseInMe=TRUE;
}

boolean RtButtonPopup::rmouseUp(int /*x*/, int /*y*/)
{
        getParent()->hide();
        notifier->releaseCapture();
        notifier->mouseTracking(FALSE);
        if (_mouseInMe) {
                int item = selectedItem();
                if (_client) {
                        if (_methods && _methods[item]) {
                                _client->perform(_methods[item],item);
                        } else if ( _indexMethod) {
                                _client->perform(_indexMethod,item);
                        }
                }
                _mouseInMe = FALSE;
        }
        return TRUE;
}

boolean RtButtonPopup::mouseMv(int /*x*/, int y, int /*bStat*/)
{
        if (notifier->whereIsMouse() == this) {
                _mouseInMe = TRUE;
                int item = y/_charH;
                if (item != selectedItem()) {
                        selectItem( y/_charH );
                }
        } else {
                _mouseInMe = FALSE;
                selectItem(-1);
        }
        return TRUE;
}

void RtButtonPopup::doFirstItem(id client)
{
        if (client) {
                _client = client;
        }
        if (_client) {
                if (_methods && _methods[0]) {
                        _client->perform(_methods[0],0);
                } else if ( _indexMethod) {
                        _client->perform(_indexMethod,0);
                }
        }
}
