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


#include "brmerge.h"
#include "elffile.h"
#include "fileinfo.h"
#include "mrtmchk.h"
#include "wchash.h"
#include "wcvector.h"

struct MergeComponent {
                        MergeComponent() : _time( 0 ), _enabled(FALSE) {}
                        MergeComponent( const MergeComponent & o ) {
                            *this = o;
                        }
                        MergeComponent( FileInfo & finf, bool en ) {
                            _time = finf.st_mtime();
                            _enabled = en;
                        }
    MergeComponent &    operator = ( const MergeComponent & o ) {
                            _time = o._time;
                            _enabled = o._enabled;
                            return *this;
                        }

    time_t          _time;
    bool            _enabled;
};

static uint HashString( const String & s ) {
    return WCHashBase::bitHash( (const char *) ((String &)s), s.length() );
}

bool CheckUpToDate( FileInfo & inf, WCValSList<String> & en,
                    WCValSList<String> & dis )
//----------------------------------------------------------
{
    WCValSListIter<String>                      enabIt( en );
    WCValSListIter<String>                      disbIt( dis );
    WCValHashDict<String,MergeComponent>        dict( HashString );
    CompFileList                                components;
    ElfFile                                     database( inf.name(), TRUE );
    int                                         i;
    String                                      key;
    int                                         found;
    MergeComponent                              compFile;

    if( !inf.exists() ) {
        return FALSE;
    }

    while( ++ enabIt ) {
        FileInfo finf( enabIt.current() );
        dict[ enabIt.current() ] = MergeComponent( finf, TRUE );
    }

    while( ++ disbIt ) {
        FileInfo finf( disbIt.current() );
        dict[ disbIt.current() ] = MergeComponent( finf, TRUE );
    }

    database.initSections();
    components = database.getComponentFiles();

    if( ( en.entries() + dis.entries() ) != components->entries() ) {
        // not same # of files
            #if INSTRUMENTS
                Log.printf( "\ndifferent number of .mbr's than req'ed\n\n" );
            #endif
        dict.clear();
        return FALSE;
    }

    for( i = 0; i < components->entries(); i += 1 ) {
        ComponentFile * file = (*components)[ i ];

        found = dict.findKeyAndValue( String(file->name), key, compFile );
        if( !found ) {
            // database has a file that is not wanted
            #if INSTRUMENTS
                Log.printf( "\n%s: .mbr file in database, not requested\n\n",
                            file->name );
            #endif
            dict.clear();
            return FALSE;
        }

        if( (file->enabled && !compFile._enabled) ||
            (!file->enabled && compFile._enabled) ) {
            // file enabling is different
            #if INSTRUMENTS
                Log.printf( "\n%s: .mbr file in database %s, requested %s\n\n",
                            file->name, file->enabled ? "enabled" : "disabled",
                            compFile._enabled ? "enabled" : "disabled" );
            #endif
            dict.clear();
            return FALSE;
        }

        if( file->time != compFile._time ) {
            // different time stamps on files
            #if INSTRUMENTS
                Log.printf( "\n%s: .mbr in database different time: (database, disk)\n", file->name );
                Log.printf( "%s", ctime( &file->time ) );
                Log.printf( "%s\n\n", ctime( &compFile._time ) );
            #endif
            dict.clear();
            return FALSE;
        }
    }

    dict.clear();
    return TRUE;
}
