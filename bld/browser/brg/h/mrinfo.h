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
* Description:  DWARF information merger class declaration.
*
****************************************************************************/


#ifndef MRINFO_H_INCLUDED
#define MRINFO_H_INCLUDED

#include "brmerge.h"
#include "mroffset.h"
#include "mrdie.h"

class DIETree;
class MergeFile;
class MergeAbbrev;
class MergeAbbrevSection;
class MergeLineSection;
class MergeRelocate;
class MergeDIE;
class MergeOffset;
class MergeNameKey;
class MergeCompunitHdr;
class MergeInfoPP;

class MergeInfoSection {
public:
                    MergeInfoSection( int numFiles,
                                      MergeAbbrevSection& abbrevs,
                                      MergeLineSection& line );
                    ~MergeInfoSection();

    uint_8                  getAddrSize();
    MergeAbbrevSection &    getAbbrev() { return _abbrevs; }
    MergeLineSection &      getLine() { return _line; }

    MergeRelocate & getReloc() {
                        return *_relocator;
                    }

    void            scanFile( MergeFile * file, uint_8 idx );
    void            writePass( MergeFile * outFile,
                               WCPtrOrderedVector<MergeFile> & inFiles );

    #if INSTRUMENTS
    void            print();
    #endif

private:
    void            relocPass( MergeInfoPP & pp );
    void            readDIE( MergeFile * file, MergeOffset startOff,
                           MergeDIE * prt, MergeOffset& moff, uint_32 abbCode );
    void            readCompUnitHdr( MergeFile * file, MergeOffset& moff );
    int             getFileLenDelta( MergeFile * file, MergeOffset& moff,
                                     uint_32 form );

    MergeAbbrevSection&                     _abbrevs;
    MergeLineSection&                       _line;

    MergeCompunitHdr *                      _compunitHdr;

    MergeRelocate *                         _relocator;
    DIETree *                               _diesByName;
    WCPtrOrderedVector<MergeDIE> *          _nameLess;
};


#endif // MRINFO_H_INCLUDED
