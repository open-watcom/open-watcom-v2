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


#include <stdio.h>
#include <time.h>
#include <wcvector.h>
#include <wclist.h>
#include <wclistit.h>

#include "blip.h"
#include "chfile.h"
#include "death.h"
#include "dwmerger.h"
#include "elffile.h"
#include "fileinfo.h"
#include "mrabbrev.h"
#include "mrfile.h"
#include "mrinfo.h"
#include "mrline.h"
#include "mrtmchk.h"
#include "mrref.h"
#include "util.h"

#if INSTRUMENTS
extern DebuggingLog Log( "WBRG" );
#endif

DwarfFileMerger::DwarfFileMerger( const char * file,
                                  WCValSList<String> & enabled,
                                  WCValSList<String> & disabled )
                : _totalSize( 0 )
                , _upToDate( false )
//---------------------------------------------------------------
{
    MergeFile *             mFile;
    WCValSListIter<String>  enabIt( enabled );
    WCValSListIter<String>  disbIt( disabled );
    FileInfo                finf( file );
    unsigned long *         drSizes;
    int                     i;
    BlipCount               blip( "Verifying Input..." );

    _outputFile = new MergeFile( file );
    _inputFiles = new WCPtrOrderedVector<MergeFile>;

    if( finf.exists() ) {
        _upToDate = CheckUpToDate( finf, enabled, disabled );
    }

    if( !_upToDate ) {
        /* add disabled components */
        while( ++ disbIt ) {
            _outputFile->addComponentFile( disbIt.current(), false );
        }

        /* add enabled components and create mergeFiles for them */
        while( ++ enabIt ) {
            _outputFile->addComponentFile( enabIt.current(), true );

            mFile = new MergeFile( enabIt.current() );

            try {
                mFile->initSections();

                drSizes = mFile->getDRSizes();
                for( i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
                    _totalSize += drSizes[ i ];
                }
            } catch( CauseOfDeath cause ) {
                if( cause == DEATH_BY_BAD_SIGNATURE ) {
                    errMessage( "\n\n%s: invalid Browser module",
                                (const char *) enabIt.current() );
                } else {
                    #ifdef STANDALONE_MERGER
                        fputs( "\n", stderr );
                    #endif

                    IdentifyAssassin( cause );
                }

                throw;
            } catch( FileExcept oops ) {
                errMessage( "\n\n%s: %s", oops._fileName, oops._message );
                throw;
            }

            _inputFiles->append( mFile );
        }
    }

    blip.end( "done" );
}

DwarfFileMerger::~DwarfFileMerger()
//---------------------------------
{
    _inputFiles->clearAndDestroy();

    delete _inputFiles;
    delete _outputFile;
}

void DwarfFileMerger::doMerge( bool quiet )
//-----------------------------------------
{
    uint_8              i;
    unsigned long *     drSizes;
    unsigned long       currSize = 0;
    int                 sectCtr;
    char                message[ 100 ];

    if( _upToDate ) {   // do this before constructing variables
        return;
    }

    MergeAbbrevSection  abbrevSect;
    MergeLineSection    lineSect;
    MergeInfoSection    infoSect( _inputFiles->entries(),
                                    abbrevSect, lineSect );
    MergeRefSection     refSect( lineSect, infoSect, _outputFile );
    BlipCount *         blip;

    try {
    #if INSTRUMENTS
        Log.printf( "Merging %d files:\n", _inputFiles->entries() );
        for( i = 0; i < _inputFiles->entries(); i += 1 ) {
            Log.printf( "%s\n", (*_inputFiles)[ i ]->getFileName() );
        }
        Log.printf( "\n%s", Log.timeStamp() );
        Log.startTiming();
    #endif

    if( !quiet ) {
        blip = new BlipCount( "reading:" );
        blip->end( "" );
    }

    for( i = 0; i < _inputFiles->entries(); i += 1 ) {
        MergeFile * file = (*_inputFiles)[ i ];
        drSizes = file->getDRSizes();

        if( !quiet ) {
            sprintf( message, " %s: ", file->getFileName() );
            blip->start( message );
        }

        abbrevSect.scanFile( file, i );
        lineSect.scanFile( file, i );
        infoSect.scanFile( file, i );

        for( sectCtr = 0; sectCtr < DR_DEBUG_NUM_SECTS; sectCtr += 1 ) {
            currSize += drSizes[ sectCtr ];
        }

        if( !quiet ) {
            sprintf( message, "%3.1f%% done", 100.0 * ( (double)currSize / (double)_totalSize ) );
            blip->end( message );
        }
    }

    if( !quiet ) {
        blip->start( "Writing..." );
    }

    _outputFile->startWrite();

    _outputFile->startWriteSect( DR_DEBUG_ABBREV );
    abbrevSect.writePass( _outputFile );
    _outputFile->endWriteSect();

    _outputFile->startWriteSect( DR_DEBUG_LINE );
    lineSect.writePass( _outputFile );
    _outputFile->endWriteSect();

    _outputFile->startWriteSect( DR_DEBUG_INFO );
    infoSect.writePass( _outputFile, *_inputFiles );
    _outputFile->endWriteSect();

    refSect.mergeRefs( *_inputFiles );

    _outputFile->endWrite();

    if( !quiet ) {
        blip->end( "done" );
    }

    for( i = 0; i < _inputFiles->entries(); i += 1 ) {
        (*_inputFiles)[ i ]->endRead();
    }

    #if INSTRUMENTS
        Log.printf( "\nMerging end - %.2f seconds.\n", Log.endTiming() );
    #endif

    } catch( FileExcept oops ) {
        const char * const actions[] = { "open", "clos", "read", "writ", "seek", "tell", "stat'", };

        errMessage( "\n\n%s: %s\n", oops._fileName, oops._message );

        #if INSTRUMENTS
        Log.printf( "Error %sing file %s -- %d: %s\n", actions[ oops._action ],
                        oops._fileName, oops._error, oops._message );
        #endif
    } catch( CauseOfDeath cause ) {
        #ifdef STANDALONE_MERGER
            fputs( "\n", stderr );
        #endif

        IdentifyAssassin( cause );
        throw;
    } catch( MergeExcept oops ) {
        #if INSTRUMENTS
        Log.printf( "Merger Error - %d, %s\n", oops._death, oops._message );
        #endif

        errMessage( "\n\nMerging Halted: %s", oops._message );
    }
}
