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


#ifndef _DWARF_FILE_MERGER_H
#define _DWARF_FILE_MERGER_H

// System includes --------------------------------------------------------

#include <wobject.hpp>
#include <wsample.h>

// Project includes -------------------------------------------------------

#include "abbrev.h"
#include "mbrfile.h"
#include "filetabl.h"
#include "deftable.h"
#include "refupdat.h"
#include "waitlist.h"
#include "symstrm.h"

class WString;

class DwarfFileMerger : public WObject {
public :
                        DwarfFileMerger( ElfFile * outFile );
                        ~DwarfFileMerger();
    void                doMerge();

protected :

    /*
     | Miscellaneous
     */
    void                writeMergedFile();
    void                writeDwarfSections( ElfFile * file, int sect );
    void                writeDebugMacro( ElfFile * file );
    void                writeDebugLine( ElfFile * file );
    void                writeDebugRef( ElfFile * file );
    void                writeDebugAbbrev( ElfFile * file );
    void                writeDebugInfo( ElfFile * file );
    uint_32             getDebugLineLength();
    uint_32             getDebugRefLength();
    void                fixCompUnitSymbol();
    uint_32             getDebugInfoLength();

    /*
     | .debug_abbrev methods -----------------------------------------------
     */
    void                writeAbbreviations();
    static void         deleteAbbrev( Abbreviation * & ab );
    static bool         cbAbbrevs( uint_32 code, const uint_8 * pAb,
                                   unsigned long length, void * data );


    /*
     | .debug_line methods -------------------------------------------------
     */
    void                writePrologue();
    void                mergeLineSection( int mbrIndex, MFile & outfile );
    void                addDirectories( int mbrIndex, DebugObjectVector & dirs );
    void                addFiles( int mbrIndex, DebugObjectVector & files );
    void                processStatementProgram( MBRFile & infile,
                                                 MFile & outfile,
                                                 unsigned long length,
                                                 int mbrIndex,
                                                 int fileBase );
    void                processExtendedOpcode( MBRFile & infile,
                                               MFile & outfile,
                                               int mbrIndex,
                                               unsigned long & currLength );
    void                processStandardOpcode( MBRFile & infile,
                                               MFile & outfile,
                                               uint_8 opcode,
                                               int mbrIndex,
                                               unsigned long & currLength,
                                               int fileBase );
    void                writeFileInfo( MFile & file, FileInfo * info );
    void                writeLineSetup( MFile & file, int mbrIndex );

    /*
     | .debug_info methods -------------------------------------------------
     */
    void                processInfoPass1( int mbrIndex );
    void                processInfoPass2( int mbrIndex );
    void                processSymbolsPass1( uint_8 * p, int mbrIndex, unsigned long );
    void                skipSymbol( uint_8 * & p, Abbreviation * pAbbrev );
    void                processSymbolPass1( uint_8 * & p, int mbrIndex, Abbreviation * pAbbrev, uint_32 offset );

    void                processSymbolsPass2( uint_8 * , int mbrIndex, unsigned long );
    bool                getSymbol( uint_8 * & p, int mbrIndex, SymbolInfo * sym, uint_32 length );
    void                processDefinedSymbol( uint_8 * & p, SymbolInfo * sym, SymbolInfo * defSym );
    bool                processDeclaredSymbol( uint_8 * & p, SymbolInfo * sym );
    void                updateSymbolStream( int mbrIndex,
                                            uint_32 oldOff,
                                            uint_32 newOff );
    void                processBasicSymbol( uint_8 * & p, SymbolInfo * sym );

    uint_8 *            readDataForm( uint_8 * p,
                                      uint_16 form,
                                      uint_32 * data );
    MergeReference *    processReference( uint_8 * & p,
                                          uint_16 form,
                                          uint_32 pos,
                                          SymbolInfo * sym,
                                          int mbrIndex );
    void                processChildSymbol( uint_8 * & p,
                                            SymbolInfo * sym,
                                            bool & isLastSib,
                                            bool & isChild );
    void                processChildren( uint_8 * & p, SymbolInfo * sym );
    void                updateFileIndex( uint_8 * & p, uint_16 form, int mbrIndex );
    void                writeDataForm( uint_8 * & p,
                                       uint_16 form,
                                       uint_32 data );
    void                skipSymbol( uint_8 * & p, uint_8 * abbrev );
    void                skipChildren( uint_8 * & p, SymbolInfo * prt );

    // .debug_ref ---------------------------------------------------------

    void                processRefSection( int mbrIndex, MFile & outfile );

    // Debugging ----------------------------------------------------------

    #if DEBUG_DUMP
    void                dumpData();
    #endif

static void writeHook( void * cli, ElfFile * file, int sect );

private:

    ElfFile *                   _outputFile;
    LinePrologue *              _prologue;
    SymbolStream                _symbolStream;

    MBRFileVector               _mbrFileTable;
    AbbrevTable                 _abbrevList;

    WaitingList                 _waitingList;
    ReferenceUpdateTable        _refUpdateTable;
    DefinedSymbolTable          _definedSymbols;
    FileTable                   _globalFileTable;
    FileTable                   _globalDirTable;

    SymbolInfo *                _compUnitSymbol;

    static char *               _lineSectionName;
    static char *               _abbrevSectionName;
    static char *               _prologueFileName;
    static char *               _infoSectionName;
    static char *               _refSectionName;
};

#endif

