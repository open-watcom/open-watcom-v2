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


#ifndef __MRREF_H__
#define __MRREF_H__

#include "brmerge.h"
#include "mrfile.h"

class MergeLineSection;
class MergeInfoSection;

struct RefLineCol {
            RefLineCol( void ) : line( 1 ), col( 1 ) {}

    void    setLine( uint_32 l ) { line = l; }
    void    setColumn( uint_32 c ) { col = c; }
    void    addLine( int_32 l ) { line += l; col = 0; }
    void    addColumn( int_32 c ) { col += c; }
    void    codeBase( uint_8 opcode );
    uint_8  makeCodeBase( uint_32 l, uint_32 c );

    uint_32 line;
    uint_32 col;
};

class MergeRefSection {
public:
                    MergeRefSection( MergeLineSection& line,
                                     MergeInfoSection& info,
                                     MergeFile * outFile );
                    ~MergeRefSection();


    void            mergeRefs( WCPtrOrderedVector<MergeFile>& files );

private:
    void            scanFile( MergeFile * file, uint_8 indx,
                              RefLineCol & absLnCol );
    void            skipDeadScope( MergeFile *, uint_32 &, uint_32,
                                   RefLineCol & currLnCol );

    MergeFile *         _outFile;
    MergeInfoSection&   _info;
    MergeLineSection&   _line;
};

#endif
