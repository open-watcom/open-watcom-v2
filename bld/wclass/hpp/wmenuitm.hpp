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


#ifndef wmenuitem_class
#define wmenuitem_class

#include "_windows.hpp"
#include "wmenuobj.hpp"
#include "wstring.hpp"

WCLASS WMenuItem;
typedef void (WObject::*cbm)( WMenuItem* m );
typedef void (WObject::*cbh)( WMenuItem* m, const char *h );

WCLASS WPopupMenu;
WCLASS WWindow;
WCLASS WMenuItem : public WMenuObject {
    public:
        WEXPORT WMenuItem( const char *text, WObject *obj, cbm pick,
                           cbh hint=NULL, const char *htext=NULL );
        WEXPORT ~WMenuItem();

        void WEXPORT setParent( WPopupMenu* parent ) { _parent = parent; }
        WPopupMenu * WEXPORT parent() { return( _parent ); }
        virtual void WEXPORT picked();
        virtual void WEXPORT hilighted( bool );
        void setTagPtr( void *tagPtr ) { _tagPtr = tagPtr; }
        void *tagPtr() { return( _tagPtr ); }
        virtual void attachMenu( WWindow *, int );
        virtual void detachMenu();

    private:
        WPopupMenu      *_parent;
        WObject         *_client;
        cbm             _pick;
        cbh             _hint;
        WString         _hintText;
        void            *_tagPtr;
};

#endif
