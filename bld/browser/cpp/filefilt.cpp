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
#include <stdlib.h>
#include <wcvector.h>

#include "assure.h"
#include "busyntc.h"
#include "chfile.h"
#include "dbmgr.h"
#include "death.h"
#include "filefilt.h"
#include "viewmgr.h"
#include "wbrwin.h"

#define FNAMECHARCMP( a, b ) ( tolower( (int) a ) - tolower( (int) b ) )

//------------------ FFiltPattern -------------------------------

FFiltPattern & FFiltPattern::operator=( const FFiltPattern & o )
//--------------------------------------------------------------
{
    _type = o._type;
    _pattern = o._pattern;
    return *this;
}

int FFiltPattern::operator==( const FFiltPattern & o ) const
//----------------------------------------------------------
{
    return _pattern == o._pattern;
}

struct FullName {
    char drive[ _MAX_DRIVE ];
    char dir[ _MAX_DIR ];
    char fname[ _MAX_FNAME ];
    char ext[ _MAX_EXT ];
};

bool FFiltPattern::match( String & s )
//------------------------------------
{
    FullName pat;
    FullName file;
    int      i;

    if( (_type == FFIncludeAll) || (_type == FFExcludeAll) ) {
        return TRUE;
    }

    _splitpath( s, file.drive, file.dir, file.fname, file.ext );
    _splitpath( _pattern, pat.drive, pat.dir, pat.fname, pat.ext );

    if( stricmp( file.drive, pat.drive ) ) {
        return FALSE;
    }

    if( stricmp( file.dir, pat.dir ) ) {
        return FALSE;
    }

    for( i = 0; i < _MAX_FNAME; i += 1 ) {
        if( pat.fname[ i ] == '*' ) {
            break;
        }
        if( pat.fname[ i ] != '?' &&
            FNAMECHARCMP( pat.fname[ i ], file.fname[ i ] ) ) {

            return FALSE;
        }
        if( pat.fname[ i ] == '\0' ) {
            break;
        }
    }

    for( i = 0; i < _MAX_EXT; i += 1 ) {
        if( pat.ext[ i ] == '*' ) {
            break;
        }
        if( pat.ext[ i ] != '?' &&
            FNAMECHARCMP( pat.ext[ i ], file.ext[ i ] ) ) {
            return FALSE;
        }
        if( pat.ext[ i ] == '\0' ) {
            break;
        }
    }

    return TRUE;
}

//------------------ FFiltEntry -------------------------------

FFiltEntry & FFiltEntry::operator=( const FFiltEntry & o )
//--------------------------------------------------------
{
    _enabled = o._enabled;
    _name = o._name;
    return *this;
}

int FFiltEntry::operator==( const FFiltEntry & o ) const
//------------------------------------------------------
{
    return !stricmp( _name, o._name);
}

int FFiltEntry::operator<( const FFiltEntry & o ) const
//-----------------------------------------------------
{
    return (_name < o._name);
}

//------------------ FileFilter -------------------------------

enum FFMatch {
    FFMNotSet,
    FFMMatchAll,
    FFMDontMatchAll,
};

FileFilter::FileFilter()
        : _matchesAll( FFMNotSet )
//--------------------------------
{
    _entries = new WCPtrSortedVector<FFiltEntry>;
    _patterns = new WCPtrOrderedVector<FFiltPattern>;
    includeAll();
    loadFiles();

    WBRWinBase::viewManager()->registerForEvents( this );
}

FileFilter::FileFilter( const FileFilter & o )
            : _matchesAll( FFMNotSet )
//--------------------------------------------
{
    _entries = new WCPtrSortedVector<FFiltEntry>;
    _patterns = new WCPtrOrderedVector<FFiltPattern>;

    *this = o;

    WBRWinBase::viewManager()->registerForEvents( this );
}

FileFilter::~FileFilter()
//-----------------------
{
    _entries->clearAndDestroy();
    delete _entries;
    _patterns->clearAndDestroy();
    delete _patterns;

    WBRWinBase::viewManager()->unregister( this );
}

void FileFilter::event( ViewEvent ve, View * )
//--------------------------------------------
{
    switch( ve ) {
    case VEBrowseFileChange:
        loadFiles();
        break;
    default:
        NODEFAULT;
    }
}

FileFilter & FileFilter::operator=( const FileFilter & o )
//--------------------------------------------------------
{
    int i;

    _entries->clearAndDestroy();
    _patterns->clearAndDestroy();

    for( i = 0; i < o._entries->entries(); i += 1 ) {
        _entries->insert( new FFiltEntry( *(*o._entries)[ i ] ) );
    }

    for( i = 0; i < o._patterns->entries(); i += 1 ) {
        _patterns->append( new FFiltPattern( *(*o._patterns)[ i ] ) );
    }

    return *this;
}

static bool FileFilter::fileHook( char * name, void * me )
//--------------------------------------------------------
{
    FileFilter * filter = (FileFilter *) me;
    FFiltEntry * entry = new FFiltEntry( name, TRUE );

    filter->_entries->insert( entry );

    return TRUE;    // continue
}

void FileFilter::loadFiles()
//--------------------------
{
    int i;

    if( WBRWinBase::dbManager()->module() == NULL ) {
        return;
    }

    BusyNotice busy( "Loading..." );
    _entries->clearAndDestroy();
    DRGetFileNameList( fileHook, this );

    for( i = 0; i < _patterns->entries(); i += 1 ) {
        applyPattern( (*_patterns)[ i ] );
    }
}

void FileFilter::applyPattern( FFiltPattern * pat )
//-------------------------------------------------
{
    int i;
    bool enabled = (pat->_type == FFIncludeAll) || (pat->_type == FFInclude);

    for( i = 0; i < _entries->entries(); i += 1 ) {
        if( pat->match( (*_entries)[ i ]->_name ) ) {
            (*_entries)[ i ]->_enabled = enabled;
        }
    }
}

void FileFilter::read( CheckedFile & file )
//-----------------------------------------
{
    uint_16 numPatterns;
    int     i;
    uint_8  type;

    _patterns->clearAndDestroy();

    file.read( &numPatterns, sizeof( uint_16 ) );

    for( i = 0; i < numPatterns; i += 1 ) {
        FFiltPattern * pat = new FFiltPattern;

        file.read( &type, sizeof( uint_8 ) );
        pat->_type = (FFPatternType) type;
        file.readNString( pat->_pattern );

        _patterns->append( pat );
        applyPattern( pat );
    }
}

void FileFilter::write( CheckedFile & file )
//------------------------------------------
{
    uint_16 numPatterns;
    int     i;
    uint_8  type;

    numPatterns = (uint_16) _patterns->entries();
    file.write( &numPatterns, sizeof( uint_16 ) );

    for( i = 0; i < numPatterns; i += 1 ) {
        FFiltPattern * pat = (*_patterns)[ i ];

        type = (uint_8)pat->_type;

        file.write( &type, sizeof( type ) );
        file.writeNString( pat->_pattern );
    }
}

uint FileFilter::numEntries()
//---------------------------
{
    return _entries->entries();
}

FFiltEntry * FileFilter::entry( uint idx )
//----------------------------------------
{
    ASSERTION( idx < _entries->entries() );

    return (*_entries)[ idx ];
}

uint FileFilter::numPatterns()
//----------------------------
{
    return _patterns->entries();
}

FFiltPattern * FileFilter::pattern( uint idx )
//--------------------------------------------
{
    ASSERTION( idx < _patterns->entries() );

    return (*_patterns)[ idx ];
}


bool FileFilter::enabled( const char * file )
//-------------------------------------------
// the file name must be case-exact comparison
// with one of the elements of the list or death
// ensues
{
    FFiltEntry   search( file, FALSE );
    FFiltEntry * entry;

    if( file == NULL ) {
        return TRUE;
    }

    entry = _entries->find( &search );

    #if DEBUG
        if( entry == NULL ) {
            WString msg;
            msg.printf( "No file found for \"%s\" -- files are:\n", file ? file : "NULL" );
            for( int i = 0; i < _entries->entries(); i += 1 ) {
                msg.concatf( " %s", (const char *) (*_entries)[ i ]->_name );
            }
            errMessage( msg );
        }
    #endif

    if( entry == NULL ) {
        throw( DEATH_BY_BAD_FILE );
    }

    return entry->_enabled;
}

bool FileFilter::matches( dr_handle hdl )
//---------------------------------------
{
    if( _matchesAll == FFMNotSet ) {
        _matchesAll = matchesAll();
    }

    if( _matchesAll == FFMMatchAll ) {
        return TRUE;
    }

    return enabled( DRGetFileName( hdl ) );
}

uint FileFilter::matchesAll()
//---------------------------
{
    int i;

    for( i = _entries->entries(); i > 0; i -= 1 ) {
        FFiltEntry * entry = (*_entries)[ i ];

        if( !entry->_enabled ) {
            return FFMDontMatchAll;
        }
    }

    return FFMMatchAll;
}

void FileFilter::includeAll()
//---------------------------
{
    FFiltPattern * inclAll;

    _patterns->clearAndDestroy();

    inclAll = new FFiltPattern( FFIncludeAll, NULL );
    applyPattern( inclAll );

    _patterns->append( inclAll );
}

void FileFilter::excludeAll()
//---------------------------
{
    FFiltPattern * exclAll;

    _patterns->clearAndDestroy();

    exclAll = new FFiltPattern( FFExcludeAll, NULL );
    applyPattern( exclAll );

    _patterns->append( exclAll );
}

void FileFilter::include( const char * pattern )
//----------------------------------------------
{
    FFiltPattern * incl;

    incl = new FFiltPattern( FFInclude, pattern );
    applyPattern( incl );

    _patterns->removeAll( incl );
    _patterns->append( incl );
}

void FileFilter::exclude( const char * pattern )
//----------------------------------------------
{
    FFiltPattern * excl;

    excl = new FFiltPattern( FFExclude, pattern );
    applyPattern( excl );

    _patterns->removeAll( excl );
    _patterns->append( excl );
}

