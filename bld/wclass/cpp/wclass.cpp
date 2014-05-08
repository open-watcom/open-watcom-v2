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


#include "wobject.hpp"
#include "wvlist1.hpp"
#include "wstring.hpp"
#include "wobjfile.hpp"

class WClassList : public WVList1
{
    public:
        WClassList() {}
        ~WClassList() { deleteContents() ; }
};

class WClassMapItem: public WObject
{
    public:
        WEXPORT WClassMapItem( const char* name, ctor ctor, int csize ) : _name(name), _ctor(ctor), _csize(csize) {}
        WEXPORT ~WClassMapItem() {}
        virtual bool WEXPORT isEqual( const WObject* obj ) const;
        const char*     _name;
        ctor            _ctor;
        int             _csize;
};

bool WEXPORT WClassMapItem::isEqual( const WObject* obj ) const
{
    return( streq( _name, ((WClassMapItem*)obj)->_name ) );
}

WClassList _classMap;

bool WEXPORT WClass::addClass( const char* name, ctor ctor, int csize )
{
    _classMap.add( new WClassMapItem( name, ctor, csize ) );
    return( true );
}

WObject* WClass::createObject( const char* name, WObjectFile& p )
{
    WClassMapItem t( name, NULL, 0 );
    WClassMapItem* m = (WClassMapItem*)_classMap.find( &t );
    return( (*m->_ctor)( p ) );
}
