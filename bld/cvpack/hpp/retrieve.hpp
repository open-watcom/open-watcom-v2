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


#ifndef _RETRIEVE_H_INCLUDED

#define _RETRIEVE_H_INCLUDED

#include <iostream.h>
#include <fstream.h>

#ifndef _CV4W_H_INCLUDED
#define _CV4W_H_INCLUDED
#include "cv4w.h"
#endif

#include "cverror.hpp"

//
// Code to indicate subsections that are not associated with a module.
//
static const MODULE_INDEPENDENT = 0xffff;

#define NB05 "NB05"
#define NB09 "NB09"

//static const char* NB05 = "NB05";
//static const char* NB09 = "NB09";

typedef struct {
    unsigned_32 offset;
    unsigned_32 length;
} dir_info;

struct cv_dir_entry {
    unsigned_16     subsection;
    unsigned_16     iMod;
    unsigned_32     lfo;
    unsigned_32     cb;

    cv_dir_entry() { }
    ~cv_dir_entry() { }
    // for the sake of putting in linked lists.
    bool operator == ( const cv_dir_entry& ) const;
};

typedef unsigned_32 module;

class Directory {

    public :

        Directory( ifstream&, streampos );
        ~Directory();

        bool GetDirInfo( dir_info&    di,
                         const sst    subsection,
                         const module mod = MODULE_INDEPENDENT );
        bool IsAtSubsection( const sst subsection );
    private :

        static bool IsModule( const unsigned_16 );
        static bool IsModuleBasis( const unsigned_16, const unsigned_16 );
        static int CompMod( const module, const module );
        static int CompSub( const unsigned_16, const unsigned_16 );
        static int Compare( const void*, const void* );
        static unsigned_8   _orderTable[];

/*        bool LinearFind( dir_info&    di,
                         const sst    subsection,
                         const module mod ) const;  */
        //
        // Directory header.
        //
        cv_subsection_directory    _cvDirHeader;
        cv_dir_entry*              _cvDirEntry;
        uint                       _currentSub;

};

class Retriever {

    enum {
        DEF_BUF_SIZE = 0x10000, // 64K.
        DEF_MISS_THRESHOLD = 2,
    };

    public :

        Retriever(ifstream&);

        ~Retriever() {
            delete [] _heapBuffer;
        }

        bool ReadSubsection( char*&       buffer,
                             unsigned_32& length,
                             const sst    subsection,
                             const module mod = MODULE_INDEPENDENT );
        bool IsAtSubsection( const sst subsection );
        unsigned_32 TellExeLength() const {
            return _lfaBase;
        }

        bool ReadExeCode(char* buffer) {
            if ( SeekRead(buffer,0L,_lfaBase) != _lfaBase ) {
                return FALSE;
            }
            return TRUE;
        }

    private :

        ifstream        _inputFile;
        streampos       _lfaBase;
        Directory       _aDirectory;
        unsigned_32     _pageStartOffset;
        char*           _heapBuffer;
        unsigned_8      _missRate;

        static char     _inputBuffer[DEF_BUF_SIZE];
        // Check for the validity of the CV signature.
        void CheckSig();

        // Function called by the constructor to locate the base address of
        // the debugging infos.
        streampos GetBasePos();

        // Function called by the constructor to locate the address of the
        // subsection directory.
        streampos GetDirPos();

        // Seek to a particular position and read some data.
        unsigned_32 SeekRead(void*, streampos, size_t);

        void ReadPage( const unsigned_32 );
        char* Read( const dir_info& );
        unsigned_32 LocalPageOff( const unsigned_32 ) const;
        bool IsCrossPage( const dir_info& ) const;
        bool IsInCurrentPage( const unsigned_32 ) const;
};
#endif
