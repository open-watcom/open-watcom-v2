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


#include "wobjmap.hpp"

class WObjectMapItem : public WObject
{
        public:
                WEXPORT WObjectMapItem( WObject* obj, WHANDLE hand ) : _obj(obj), _hand(hand) {}
                WEXPORT ~WObjectMapItem() {}
                virtual bool WEXPORT isEqual( const WObject* obj ) const;
                WObject* WEXPORT obj() { return _obj; }

        private:
                WObject*                _obj;
                WHANDLE                 _hand;
};


bool WEXPORT WObjectMapItem::isEqual( const WObject* obj ) const
{
        if( ((WObjectMapItem*)obj)->_obj != NULL ) {
                if( ((WObjectMapItem*)obj)->_hand != NULL ) {
                        return ((WObjectMapItem*)obj)->_obj == _obj && ((WObjectMapItem*)obj)->_hand == _hand;
                }
                return ((WObjectMapItem*)obj)->_obj == _obj;
        } else if( ((WObjectMapItem*)obj)->_hand != NULL ) {
                return ((WObjectMapItem*)obj)->_hand == _hand;
        }
        return FALSE;
}

void WEXPORT WObjectMap::setThis( WObject* obj, WHANDLE hand )
{
        add( new WObjectMapItem( obj, hand ) );
        _currThis = NULL;
}

void WEXPORT WObjectMap::clearThis( WObject* obj )
{
        WObjectMapItem  x( obj, NULL );
        delete remove( &x );
}

WObject* WEXPORT WObjectMap::findThis( WHANDLE hand )
{
        if( hand != NULL ) {
                WObjectMapItem  x( NULL, hand );
                WObjectMapItem* m = (WObjectMapItem*)find( &x );
                if( m == NULL ) {
                        return _currThis;
                }
                return m->obj();
        }
        return NULL;
}

