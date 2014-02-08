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


#include "wstrmap.hpp"

class WStringMapItem : public WObject
{
        public:
                WEXPORT WStringMapItem( WObject* obj, WString *hand ) : _obj(obj), _hand(hand) {}
                WEXPORT ~WStringMapItem() {}
                virtual bool WEXPORT isEqual( const WObject* obj ) const;
                WObject* WEXPORT obj() { return _obj; }

        private:
                WObject*                _obj;
                WString*                _hand;
};


bool WEXPORT WStringMapItem::isEqual( const WObject* obj ) const
{
        if( ((WStringMapItem*)obj)->_obj != NULL ) {
                if( ((WStringMapItem*)obj)->_hand != NULL ) {
                        return ((WStringMapItem*)obj)->_obj == _obj && ((WStringMapItem*)obj)->_hand->isEqual( _hand );
                }
                return ((WStringMapItem*)obj)->_obj == _obj;
        } else if( ((WStringMapItem*)obj)->_hand != NULL ) {
                return ((WStringMapItem*)obj)->_hand->isEqual( _hand );
        }
        return FALSE;
}

void WEXPORT WStringMap::setThis( WObject* obj, WString *hand )
{
        add( new WStringMapItem( obj, hand ) );
        _currThis = NULL;
}

void WEXPORT WStringMap::clearThis( WObject* obj )
{
        WStringMapItem  x( obj, NULL );
        delete remove( &x );
}

WObject* WEXPORT WStringMap::findThis( WString *hand )
{
        if( hand != NULL ) {
                WStringMapItem  x( NULL, hand );
                WStringMapItem* m = (WStringMapItem*)find( &x );
                if( m == NULL ) {
                        return _currThis;
                }
                return m->obj();
        }
        return NULL;
}
