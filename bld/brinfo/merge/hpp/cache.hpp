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


/*
    CACHE.HPP
    ---------
    Support for saving/loading browser information after multiple .BRM files
    have been merged.

    Browser save files are composed of various "components", each
    of which stores information from one module in the DLL.  Components
    are identified by ANSI strings which had better be unique, dammit.
    The last component in any save file is a directory, so components
    can be loaded in a different order from that in which they were saved.
    Size information is saved for each component, but other than that
    the format of individual components is determined entirely by the
    module which uses it.
*/

#ifndef _CACHE_HPP
#define _CACHE_HPP

/*  Forward declarations.
*/

struct  CacheHeader;
struct  DirEntry;
template<class T> class LList;


/*  CacheOutFile:
      Class for saving browser information to a file.
*/

class CacheOutFile {
    public:
        CacheOutFile();
        ~CacheOutFile();

        WBool Open( char const *filename );
        void Close();

        void StartComponent( char const *name );
        void EndComponent();

        void AddData( void *data, int length );
        void AddDword( uint_32 dword, int compress=1 );
        void AddByte( uint_8 byte );

    private:
        LList<DirEntry> *_directory;
        DirEntry        *_curEntry;

        CacheHeader     *_header;

        FILE            *_file;
        uint_32         _filePos;

        int             _numComponents;
};


/*  CacheInFile:
      Class for loading browser information from a file.
*/

class CacheInFile {
    public:
        CacheInFile();
        ~CacheInFile();

        WBool Open( char const *filename );
        void Close();

        WBool OpenComponent( char const *name );
        void CloseComponent();

        int ReadData( void *data, int length );
        int ReadDword( void *dword, int compress=1 );
        int ReadByte( uint_8 &byte );
    private:
        LList<DirEntry>         *_directory;
        DirEntry                *_curEntry;
        uint_32                 _curEnd;

        FILE                    *_file;
        uint_32                 _filePos;
        uint_32                 _fileLen;

        int                     _numComponents;
};

#endif  // _CACHE_HPP
