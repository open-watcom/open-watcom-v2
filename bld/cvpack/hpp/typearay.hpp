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


#include <wclist.h>
#include <wcvector.h>
#include "typercrd.hpp"

class GlobalTypeContainer {

    public :

        GlobalTypeContainer() { }

        virtual ~GlobalTypeContainer() { }

        virtual WCPtrSList<LFTypeRecord>& Find( LFTypeRecord* ) = 0;

        virtual void Insert( LFTypeRecord* ) = 0;

};

class GlobalListContainer : public GlobalTypeContainer {

    public :

        GlobalListContainer()  { }
        virtual ~GlobalListContainer()  { }

        virtual WCPtrSList<LFTypeRecord>& Find( LFTypeRecord* ) {
            return _typeList;
        }

        virtual void Insert( LFTypeRecord* t ) {
            _typeList.append( t );
        }

    private :

        WCPtrSList<LFTypeRecord>    _typeList;
};

class GlobalHashContainer : public GlobalTypeContainer {

    enum {
        DEF_BUCKET_SIZE = 47,
        TABLE_LOAD      = 23,
    };

    public :

        GlobalHashContainer()
                : _buckets( new WCPtrSList<LFTypeRecord> [DEF_BUCKET_SIZE] ),
                  _bucketSize(DEF_BUCKET_SIZE),
                  _entries(0) { }

        virtual ~GlobalHashContainer() {
            if (_buckets) {
                delete [] _buckets;
                _buckets=NULL;
            }
        }

        virtual WCPtrSList<LFTypeRecord>& Find( LFTypeRecord* t ) {
            return _buckets[HFn(t->Variant())%_bucketSize];
        }

        virtual void Insert( LFTypeRecord* );

    private :

        static unsigned_32 HFn( VariantString& );

        void Rehash();

        WCPtrSList<LFTypeRecord>*       _buckets;
        uint                            _bucketSize;
        uint                            _entries;
};

class TypeArray {

    public :

        TypeArray() : _typeArray(DEF_ARRAY_SIZE,DEF_ARRAY_SIZE) { }

        virtual ~TypeArray() { }

        void Insert( LFTypeRecord* t ) {
            _typeArray.insert(t);
            DerivedInsert(t);
        }

        //
        // make const reference so that change can not be made through
        // [] operator.
        //
        LFTypeRecord* const & operator [] ( const type_index i ) {
            return _typeArray[ToTableIndex(i)];
        }

        LFTypeRecord* const & operator [] ( const type_index i ) const {
            return _typeArray[ToTableIndex(i)];
        }

        uint Entries() const {
            return _typeArray.entries();
        }

    protected :

       virtual void DerivedInsert( LFTypeRecord* ) = 0;

       void ArrayClear() {
           _typeArray.clear();
       }

    private :

        WCPtrOrderedVector<LFTypeRecord>    _typeArray;
};

class GlobalTypeArray : public TypeArray {

    public :

        GlobalTypeArray();

        virtual ~GlobalTypeArray() {
            for ( uint i = 0; i < LF_OEM+1; i++ ) {
                delete _mapTypeArray[i];
            }
        }

        WCPtrSList<LFTypeRecord>& Find( LFTypeRecord* ) ;

    protected :

        virtual void DerivedInsert( LFTypeRecord* );

    private :

        GlobalTypeContainer*                _mapTypeArray[LF_OEM+1];
};


class LocalTypeArray : public TypeArray {

    friend class SstGlobalTypes;

    public :

        LocalTypeArray() { }
        virtual ~LocalTypeArray() { }

        void Clear() {
            TypeArray::ArrayClear();
            _hashRecords.clear();
            _otherRecords.clear();
        }

    protected :

        virtual void DerivedInsert( LFTypeRecord* );

    private :

        WCPtrSList<LFTypeRecord>        _hashRecords;
        WCPtrSList<LFTypeRecord>        _otherRecords;
};
