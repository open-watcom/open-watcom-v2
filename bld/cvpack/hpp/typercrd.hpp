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


#ifndef TYPERECORD_H_INCLUDED
#define TYPERECORD_H_INCLUDED

#include <wcstack.h>
#include <assert.h>
#include "packtype.hpp"
#include "makeexe.hpp"

typedef LFLeafStruct* (*PTF)(const char*, const uint);

class SstGlobalTypes;
class Recorder;
class MyNodeStack;

class LFTypeRecord {

    public :

        LFTypeRecord( const type_index, const char* );
        LFTypeRecord( LFTypeRecord* );

        //
        // Make destructor empty to avoid destructor to destruct object.
        // In this particular program, LFTypeRecord objects are destruct
        // by explicitly calling the ManualDestruct() function so that copying
        // is not needed when inserting a record from the local type table to
        // the global type table.  This transfer can be done via pointers
        // only, which saves some processing time.  This is bad style, only
        // implemented here to improve performance.  A copy constructor can
        // be added to have a better style :)
        //
        virtual ~LFTypeRecord() { }

        virtual bool IsEquivalent( LFTypeRecord& ) {
            throw InternalError("LFLocalTypeRecord::IsEquivalent gets call.");
        }

        void ManualDestruct() {
            delete _typeStr;
        }

        void FixTypeIndex() {
            _typeStr -> FixTypeIndex();
        }

        // For the sake of putting in WCList.
        bool operator == ( const LFTypeRecord& ) const;
        /* {
            throw InternalError("LFTypeRecord::== gets call.");
        } */

        uint Length() const {
            return _length + WORD; // including length field.
        }

        const LFLeafStruct* TypeString() const {
            return _typeStr;
        }

        void Put( ExeMaker& eMaker ) {
            eMaker.DumpToExe(_length);
            _typeStr -> Put(eMaker);
        }

        leaf_index Index() {
            return _typeStr -> Index();
        }

        VariantString& Variant() {
            return _typeStr -> Variant();
        }

        type_index TypeIndex() const {
            return _typeIndex;
        }

    private :

        static uint ConvertIndex( const leaf_index );

        static PTF            _typeConstructorTable[];

        const unsigned_16     _length;
        LFLeafStruct*         _typeStr;
        const type_index      _typeIndex;
};

typedef LFTypeRecord LFLocalTypeRecord;

/*class LFLocalTypeRecord : public LFTypeRecord {

    public :

        LFLocalTypeRecord(const type_index, const char* buffer);
        virtual ~LFLocalTypeRecord() { }

        type_index TypeIndex() const {
            return _typeIndex;
        }

    private :

        const type_index      _typeIndex;
}; */

class LFGlobalTypeRecord : public LFTypeRecord {

    friend class MyNodeStack;

    typedef struct {
        bool    isInLoop:1;
        bool    isStopPoint:1;
        bool    unused:6;
    } node_flags;

    public :

        LFGlobalTypeRecord(const type_index, const char* buffer);
        LFGlobalTypeRecord( LFLocalTypeRecord* );
        virtual ~LFGlobalTypeRecord() { }

        bool IsEquivalent( LFLocalTypeRecord& target );

    private :

        bool HasVisited() const {
            return ( _targetAdr ? TRUE : FALSE );
        }

        void MarkLoop(const bool isStopPoint) {
            _flags.isInLoop = TRUE;
            _flags.isStopPoint = isStopPoint;
        }

        void MarkNode( LFLocalTypeRecord* targetAdr ) {
            _targetAdr = targetAdr;
        }

        bool IsInLoop() {
            return _flags.isInLoop;
        }

        bool IsStopPoint() {
            return _flags.isStopPoint;
        }

        void UnMarkNode() {
            _targetAdr = NULL;
            _flags.isInLoop = FALSE;
            _flags.isStopPoint = FALSE;
        }

        LFLocalTypeRecord*      _targetAdr;
        node_flags              _flags;

        static Recorder         _recorder;
        static MyNodeStack      _nodeStack;
};

class Recorder {

    public :

        Recorder() { }
        ~Recorder() { }

        void Record( const type_index ti ) {
            _recordLst.push( ti );
        }

        void Clear() {
            _recordLst.clear();
        }

        void RewindAndUndo();

    private :

        WCStack< type_index, WCValSList<type_index> >   _recordLst;
};

class MyNodeStack {

    public :

        MyNodeStack() { }
        ~MyNodeStack() { }

        void Push( LFGlobalTypeRecord* a ) {
            _nodeLst.insert(a);
            //assert( _nodeLst.insert( a ) );
        }

        void MarkLoop( LFGlobalTypeRecord* );

        LFGlobalTypeRecord* Pop() {
            if ( _nodeLst.entries() > 0 ) {
                return _nodeLst.get();
            }
            return NULL;
        }

    private :

       WCPtrSList< LFGlobalTypeRecord >     _nodeLst;
};
#endif
