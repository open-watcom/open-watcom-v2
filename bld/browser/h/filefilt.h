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


#ifndef __FILEFILT_H__
#define __FILEFILT_H__

#include <string.hpp>
#include <dr.h>

#include "view.h"
#include "wbrdefs.h"

class CheckedFile;

template <class Type> class WCPtrOrderedVector;
template <class Type> class WCPtrSortedVector;

enum FFPatternType {
    FFIncludeAll,
    FFExcludeAll,
    FFInclude,
    FFExclude,
};

class FFiltPattern
{
public:
                        FFiltPattern() : _type( (FFPatternType) 0 ) {}
                        FFiltPattern( FFPatternType tp, const char * pat )
                                : _type( tp ), _pattern( pat ) {}
                        FFiltPattern( const FFiltPattern & o )
                            : _type( o._type ), _pattern( o._pattern ) {}

        FFiltPattern &  operator=( const FFiltPattern & o );
        int             operator==( const FFiltPattern & o ) const;
        bool            match( String & str );

        FFPatternType   _type;
        String          _pattern;
};

class FFiltEntry
{
public:
                        FFiltEntry() : _enabled( FALSE ) {}
                        FFiltEntry( const char * nm, bool en )
                            : _enabled( en ), _name( nm ) {}
                        FFiltEntry( const FFiltEntry & o )
                            : _enabled( o._enabled ), _name( o._name ) {}

        FFiltEntry &    operator=( const FFiltEntry & o );
        int             operator==( const FFiltEntry & o ) const;
        int             operator<( const FFiltEntry & o ) const;

        bool            _enabled;
        String          _name;
};

class FileFilter  : public ViewEventReceiver
{
public:
                        FileFilter();
                        FileFilter( const FileFilter & o );
    virtual             ~FileFilter();

    virtual void        event( ViewEvent ve, View * view );
    virtual ViewEvent   wantEvents() { return VEBrowseFileChange; }

            FileFilter& operator=( const FileFilter & o );

            void        loadFiles();

            bool        enabled( const char * file );
            bool        matches( dr_handle drhdl );

            void        includeAll();
            void        excludeAll();
            void        include( const char * pattern );
            void        exclude( const char * pattern );

            uint        numEntries();
            FFiltEntry* entry( uint idx );

            uint            numPatterns();
            FFiltPattern*   pattern( uint idx );

            void        read( CheckedFile & );
            void        write( CheckedFile & );

protected:
            uint        matchesAll();

private:
            void        applyPattern( FFiltPattern * pat );
    static  bool        fileHook( char * name, void * me );

            uint                                _matchesAll;
            WCPtrSortedVector<FFiltEntry> *     _entries;
            WCPtrOrderedVector<FFiltPattern> *  _patterns;
};

#endif // __FILEFILT_H__
