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


#ifndef wtptlist_class
#define wtptlist_class

#include <stdlib.h>
#include "wobject.hpp"

typedef int (*TComp)( const void *, const void *);

template<class T> class TemplateList : public WObject
{
        public:
                WEXPORT TemplateList();
                WEXPORT TemplateList( TemplateList& );
                WEXPORT ~TemplateList();
                int WEXPORT count() { return _free; }
                T operator[]( int index ) { return _set[ index ]; }
                virtual T WEXPORT add( T obj );
                int WEXPORT indexOfSame( T obj );
                int WEXPORT indexOfElem( T* elem );
                virtual T WEXPORT removeAt( int index );
                T replaceAt( int index, T obj );
                T insertAt( int index, T obj );
                T WEXPORT removeSame( T obj );
                void WEXPORT reset();
                void WEXPORT sort( TComp );
                T* WEXPORT search( void * key, TComp compFn );
        protected:
                T* _set;
                int     _count;
                int     _free;
        private:
                void growBlock();
};


template<class T> TemplateList<T>::TemplateList()
                : _set( NULL )
                , _count( 0 )
                , _free( 0 )
{
}

template<class T> TemplateList<T>::TemplateList( TemplateList& x )
                : _set( NULL )
                , _count( x._count )
                , _free( x._free )
{
        if( x._set ) {
                _set = new T[ _count ];
                for( int i=0; i<_count; i++ ) {
                        _set[i] = x._set[i];
                }
        }
}

template<class T> TemplateList<T>::~TemplateList()
{
        delete _set;
}

#if 0
// what to do here?
TemplateList* WEXPORT TemplateList::createSelf( WObjectFile& )
{
        return new TemplateList();
}

void WEXPORT TemplateList::readSelf( WObjectFile& p )
{
        int count;
        p.readObject( &count );
        for( int i=0; i<count; i++ ) {
                add( p.readObject() );
        }
}

void WEXPORT TemplateList::writeSelf( WObjectFile& p )
{
        p.writeObject( count() );
        for( int i=0; i<count(); i++ ) {
                p.writeObject( _set[i] );
        }
}
#endif

template<class T> void TemplateList<T>::reset()
{
        delete _set;
        _set = NULL;
        _count = 0;
        _free = 0;
}

template<class T> void TemplateList<T>::growBlock()
{
        if( _set == NULL ) {
                static int _countInit = 10;
                _set = new T[ _countInit ];
                if( _set ) {
                        _count = _countInit;
                        _free = 0;
                }
        }
        if( _set ) {
                if( _free >= _count ) {
                        static int _countIncr = 5;
                        T* nset = new T[ _count + _countIncr ];
                        if( nset ) {
                                for( int i=0; i<_count; i++ ) {
                                        nset[i] = _set[i];
                                }
                                delete _set;
                                _set = nset;
                                _count += _countIncr;
                        }
                }
        }
}

template<class T> T WEXPORT TemplateList<T>::add( T obj )
{
        growBlock();
        if( _set ) {
                _set[ _free ] = obj;
                _free += 1;
                return obj;
        }
        return NULL;
}

template<class T> T WEXPORT TemplateList<T>::insertAt( int i, T obj )
/*
 * NOTE: this doesn't insert anything after the end of the string.
*/
{
        growBlock();
        if( _set != NULL && i <= _free ) {
                if( i < _free ) {
                        memmove( _set + i + 1, _set + i, (_free - i)*sizeof(T));
                }
                _set[ i ] = obj;
                _free += 1;
                return obj;
        }
        return NULL;
}

template<class T> int WEXPORT TemplateList<T>::indexOfElem( T* elem )
{
    return (int) ((unsigned long) (elem - _set)) / sizeof(T);
}

template<class T> int WEXPORT TemplateList<T>::indexOfSame( T obj )
{
        for( int i=0; i<_free; i++ ) {
                if( _set[ i ] ) {
                        if( obj == _set[ i ] ) {
                                return i;
                        }
                }
        }
        return -1;
}

template<class T> T WEXPORT TemplateList<T>::replaceAt( int i, T obj )
{
        if( i >= 0 && i < _free ) {
                T o = _set[ i ];
                _set[ i ] = obj;
                return o;
        }
        return NULL;
}

template<class T> T WEXPORT TemplateList<T>::removeAt( int i )
{
        if( i >= 0 && i < _free ) {
                T obj = _set[ i ];
                _free -= 1;
                if( _free > 0 ) {
                        for( ; i< _count-1; i++ ) {
                                _set[ i ] = _set[ i+1 ];
                        }
                } else {
                        delete _set;
                        _set = NULL;
                }
                return obj;
        }
        return NULL;
}

template<class T> T WEXPORT TemplateList<T>::removeSame( T obj )
{
        return removeAt( indexOfSame( obj ) );
}

template<class T> T* WEXPORT TemplateList<T>::search( void * key,
                                                      TComp compFn )
{
    return (T*) bsearch( key, _set, _free, sizeof(T), compFn );
}

template<class T> void WEXPORT TemplateList<T>::sort( TComp compFn )
{
    qsort( _set, _free, sizeof(T), compFn );
}

#endif //wtptlist_class

