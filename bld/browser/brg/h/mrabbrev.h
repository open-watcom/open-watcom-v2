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


#ifndef __MRABBREV_H__
#define __MRABBREV_H__

#include "mrfile.h"
#include "brmerge.h"

class SafeFile;

class MergeAttrib {
public:
                    MergeAttrib(){};
                    MergeAttrib( uint_32 a, uint_32 f )
                        : _attrib( a ), _form( f ) {};
                    ~MergeAttrib(){};

    MergeAttrib&    operator= ( const MergeAttrib& other ) {
                        _attrib = other._attrib;
                        _form = other._form;
                        return *this;
                    }
    bool            operator== ( const MergeAttrib& other ) const {
                        return _attrib == other._attrib
                                && _form == other._form;
                    };

    uint_32         attrib() const { return _attrib; };
    uint_32         form() const { return _form; };

    void            attrib( uint_32 a ) { _attrib = a; };
    void            form( uint_32 f ) { _form = f; };

private:
    uint_32             _attrib;
    uint_32             _form;
};

class MergeAbbrev {
public:
                    MergeAbbrev();
                    MergeAbbrev( uint_32 code );
                    ~MergeAbbrev();

    uint_32         code() const { return _code; }
    uint_32         tag() const { return _tag; }
    bool            hasChildren() const { return _children; }
    MergeAttrib&    operator[] ( int idx );
    uint            entries();

    void            readAbbrev( MergeFile * file, uint_32& off );
    void            writeAbbrev( MergeFile * outFile );

    bool            operator== ( const MergeAbbrev& other ) const {
                        return _code == other._code;
                    }
    void *          operator new( size_t ) { return _pool.alloc(); };
    void            operator delete( void * p ) { _pool.free( p ); };

    static void *   allocHNode( size_t );
    static void     freeHNode( void *, size_t );

    static void     skipAbbrev( MergeFile * file, uint_32& off );

private:
    void            setup();

    uint_32         _code;      // the abbreviation code
    uint_32         _tag;       // die tag
    bool            _children;  // DW_CHILDREN_yes | DW_CHILDREN_no

    WCValOrderedVector<MergeAttrib> *   _attribs;
    static MemoryPool                   _pool;
    static MemoryPool                   _hPool;
};

class MergeAbbrevSection {
public:
                    MergeAbbrevSection();
                    ~MergeAbbrevSection();

    MergeAbbrev *   getAbbrev( uint_32 code );

    void            scanFile( MergeFile * file, uint_8 idx );
    void            writePass( MergeFile * outFile );

private:

    WCValHashDict<uint_32,MergeAbbrev *> *    _abbrevs;
};

#endif // __MRABBREV_H__
