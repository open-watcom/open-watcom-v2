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


// System includes --------------------------------------------------------

#include <wcvector.h>
#if DEBUG
#include <stdio.h>
#endif

template< class U > class IndexedObject {
    friend class Table<U>;

    IndexedObject( int i, U o ) : index(i), obj(o) {}

    int index;
    U obj;
};

//
// Table indexed by an integer value.  The table could be full of
// lots of empty entries so it would make sense to hash it instead...
//
template<class T> class Table {
public:
    typedef void (*DeleteCallback)( T & obj );

                        Table();
                        ~Table();

    virtual bool        placeAt( int index, T obj );
    int                 count();
    T                   operator[]( int index );
    void                iterateOver( DeleteCallback fn );

protected:

    typedef IndexedObject<T>    Object;

    int                         _maxIndex;
    WCValOrderedVector<void *>  _list;

};

template<class T> Table<T>::Table()
    : _maxIndex( 0 )
/*********************************/
{
}

template<class T> Table<T>::~Table()
/**********************************/
{
    int i;

    #if DEBUG_LIST
    printf( "template<class T> Table<T> _list has %d items\n", _list.entries() );
    #endif

    for( i = 0; i < _list.entries(); i += 1 ) {
        delete ( Object * ) _list[ i ];
    }
}

template<class T> bool Table<T>::placeAt( int index, T o )
/********************************************************/
{
    int i;
    Object * obj;

    for( i = 0; i < _list.entries(); i += 1 ) {
        if( ((Object *)_list[ i ])->index == index ) {
            return FALSE;                       //<-- Note early return
        }
    }

    obj = new Object( index, o );
    _list.insert( obj );
    return TRUE;
}

template<class T> int Table<T>::count()
/*************************************/
{
    int max = -1;
    int i;

    for( i = 0; i < _list.entries(); i += 1 ) {
        if( ((Object *)_list[ i ])->index > max ) {
            max = (( Object * )_list[ i ])->index;
        }
    }

    return max;
}

template<class T> T Table<T>::operator[]( int index )
/***************************************************/
{
    int i;

    for( i = 0; i < _list.entries(); i += 1 ) {
        if( ((Object* )_list[ i ])->index == index ) {
            return (( Object * )_list[ i ])->obj;
        }
    }
    return NULL;
}


template<class T> void Table<T>::iterateOver( DeleteCallback fn )
/***************************************************************/
{
    int i;

    for( i = 0; i < _list.entries(); i += 1 ) {
        fn((( Object * )_list[ i ])->obj );
    }
}
