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


#ifndef _MBRFILE_H
#define _MBRFILE_H

// System includes --------------------------------------------------------

#include <dr.h>
#include <string.h>
#include <wcvector.h>

// Project includes -------------------------------------------------------

#include "elffile.h"
#include "abbrev.h"
#include "linepro.h"
#include "fileinfo.h"

typedef bool (* CBAbbrev)( uint_32 code, const uint_8 *, unsigned long, void * );

class MBRFile : public ElfFile {
public:
                        MBRFile( char * filename );
                        ~MBRFile();

    enum MBRErr { Invalid };

    void                scanAbbreviations( CBAbbrev cb, void * data );
    void                getSectionRawData( dr_section, uint_8 * &pData, unsigned long * = NULL );
    uint_32             readULEB128( dr_section sect );
    bool                operator== ( const MBRFile& other ) {
                            return this == &other;
                        };

    #if 0 // ITB - think it's obsolete
    void                getSectionFilePtr( dr_section section, MFile * file,
                                           unsigned long * len );
    #endif

    // .debug_line processing

    void                getLineInfo( LinePrologue & pro,
                                     unsigned long & length );

private:
    friend class DwarfFileMerger;

    uint_8 *            readFileInfo( uint_8 * data, FileInfo * & filename );

    uint_8 *            _sectionData[ DR_DEBUG_NUM_SECTS ];
};

typedef WCPtrOrderedVector<MBRFile> MBRFileVector;

#endif

