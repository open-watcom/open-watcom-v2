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
//  context.h   --Class to write the |CONTEXT internal file in a .HLP file.
//                For the Watcom .HLP compiler.
//
#ifndef CONTEXT_H
#define CONTEXT_H

#include <watcom.h>
#include "myfile.h"
#include "hlpdir.h"
#include "btrees.h"

struct FutureHash;  // forward declaration

// Class to write the |CONTEXT b-tree to a .HLP file.
class HFContext : public Dumpable
{
    Btree   *_data;
    FutureHash  *_head;
    FutureHash  *_tail;
    static char const   _conMagic[];

    // Assignment of HFContext's is not permitted.
    HFContext( HFContext const & ){};
    HFContext & operator=( HFContext const & ) { return *this; };

public:
    HFContext( HFSDirectory *d_file );
    ~HFContext();

    // Add a topic to the tree.
    void    addOffset( uint_32 hval, uint_32 off );

    // Record a referenced to a topic (which may not be defined)
    void    recordContext( char const str[] );

    // Look up a topic in the tree.
    uint_32 getOffset( uint_32 hval );

    static const uint_32 _badValue;

    // New definitions of the Dumpable virtual functions.
    uint_32 size() { return _data->size(); };
    int     dump( OutFile * dest );
};

#endif
