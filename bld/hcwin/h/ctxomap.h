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


//
//  ctxomap.h   --Class to implement the |CTXOMAP internal file of a
//        .HLP file.  For the Watcom .HLP compiler.
//

#ifndef CTXOMAP_H
#define CTXOMAP_H

#include "myfile.h"
#include "hlpdir.h"
#include "context.h"
#include <watcom.h> // for uint_32, etc.


//
//  HFCtxomap   --Class to write the |CTXOMAP.
//        Depends on the HFContext class.
//

class HFCtxomap : public Dumpable
{
    // Structure to store map records internally.
    struct CmapRec : public Dumpable
    {
    uint_32     _mapnum;
    uint_32     _offset;
    CmapRec     *_nextRec;

    CmapRec( uint_32 num, uint_32 h_val );
    uint_32     size() { return 8L; };
    int     dump( OutFile * dest );
    };

    uint_16     _numRecords;
    CmapRec     *_firstRec, *_lastRec;
    uint_16     _size;
    HFContext       *_offsetFile;
    int         _resolved;

    // Assignment of HFCtxomap's is not permitted.
    HFCtxomap( HFCtxomap const & ){};
    HFCtxomap & operator=( HFCtxomap const & ) { return *this; };

public:
    HFCtxomap( HFSDirectory *d_file, HFContext *offsets );
    ~HFCtxomap();

    // Specify a map record by the hash value of the context string.
    void addMapRec( uint_32 num, uint_32 h_val );

    uint_32     size();
    int         dump( OutFile * dest );
};

#endif
