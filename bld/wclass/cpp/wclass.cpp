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
#include "wstring.hpp"
#include "wobjfile.hpp"

class WClassList
{
    public:
        WClassList() {}
        ~WClassList();

        WObject* find( WObject *obj );
        WObject* add( WObject* obj );
    private:
        void growBlock();

        static WObject** _set;
        static int     _count;
        static int     _free;
};

WObject ** WClassList::_set = (WObject **)NULL;
int WClassList::_count = 0;
int WClassList::_free = 0;

WClassList::~WClassList()
{
    if( _set != NULL ) {
        for( int i=_free; i>0; i-- ) {
            if( _set[i - 1] != NULL ) {
                delete _set[i - 1];
            }
        }
        _free = 0;
        delete[] _set;
        _set = NULL;
    }
}

WObject* WClassList::find( WObject* obj )
{
    for( int i=0; i<_free; i++ ) {
        if( _set[i] != NULL ) {
            if( _set[i]->isEqual( obj ) ) {
                return( _set[i] );
            }
        }
    }
    return( NULL );
}

WObject* WClassList::add( WObject* obj )
{
    growBlock();
    if( _set != NULL ) {
        _set[_free] = obj;
        _free += 1;
        return( obj );
    }
    return( NULL );
}

void WClassList::growBlock()
{
    if( _set == NULL ) {
        static int _countInit = 10;
        _set = new WObject*[_countInit];
        if( _set != NULL ) {
            _count = _countInit;
            _free = 0;
        }
    }
    if( _set != NULL ) {
        if( _free >= _count ) {
            static int _countIncr = 5;
            WObject** nset = new WObject*[_count + _countIncr];
            if( nset != NULL ) {
                for( int i=0; i<_count; i++ ) {
                    nset[i] = _set[i];
                }
                delete[] _set;
                _set = nset;
                _count += _countIncr;
            }
        }
    }
}

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
