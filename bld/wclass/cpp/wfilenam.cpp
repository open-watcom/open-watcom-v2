/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "wfilenam.hpp"
#include "wobjfile.hpp"

extern "C" {
    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
#ifdef __UNIX__
    #include <sys/types.h>
#else
    #include <direct.h>
#endif
    #include <time.h>
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
    #include "wio.h"
    #include "pathgrp2.h"
};

#include "clibext.h"


#define PATHSEP_CHAR    (_pathsep[2])
#define PATHSEP_STR     (_pathsep + 2)
#define PARENTSEP_STR   (_pathsep)

static pgroup2  _x;
static char     _result_buf[PATH_MAX + 1];

static void removeSepFromEnd( char *dir, char pathsep )
{
    size_t len;

    if( dir[0] != '\0' ) {
        len = strlen( dir ) - 1;
        if( dir[len] == pathsep ) {
            dir[len] = '\0';
        }
    }
}

static void addSepToEnd( char *dir, char pathsep )
{
    size_t len;

    if( dir[0] != '\0' ) {
        len = strlen( dir );
        if( dir[len - 1] != pathsep ) {
            dir[len++] = pathsep;
            dir[len] = '\0';
        }
    }
}

#ifndef __UNIX__
static bool setdrive( const char* drive, int* olddrive )
{
    if( drive[0] != '\0' ) {
        *olddrive = _getdrive();
        int drv = toupper( (unsigned char)drive[0] ) - 'A' + 1;    // 1='A'; 2='B'; ...
        if( *olddrive != drv ) {
            if( _chdrive( drv ) ) {
                return( false );
            }
        }
    }
    return( true );
}
#endif

static void makepath( char* path, const char* drive, const char* dir, const char* fname, const char* ext )
{
    path[2] = '\0';     //avoid bug in _makepath
    _makepath( path, drive, dir, fname, ext );
}

static void splitref( pgroup2& s, const char* f, char pathsep )
{
    if( f != NULL ) {
        _splitpath2( f, s.buffer, &s.drive, &s.dir, &s.fname, &s.ext );
    } else {
        getcwd( _result_buf, sizeof( _result_buf ) );
        addSepToEnd( _result_buf, pathsep );
        _splitpath2( _result_buf, s.buffer, &s.drive, &s.dir, &s.fname, &s.ext );
    }
}

Define( WFileName )

WEXPORT WFileName::WFileName( const char* name, char pathsep )
    : WString( name )
{
    _pathsep[0] = '.';
    _pathsep[1] = '.';
    _pathsep[2] = pathsep;
    _pathsep[3] = '\0';
}

#ifndef NOPERSIST
WFileName* WEXPORT WFileName::createSelf( WObjectFile& )
{
    return( new WFileName() );
}

void WEXPORT WFileName::readSelf( WObjectFile& p )
{
    const char  *x;
    char        from;

    WString::readSelf( p );
    if( PATHSEP_CHAR == '/' ) {
        from = '\\';
    } else {
        from = '/';
    }
    for( x = *this; (x = strchr( x, from )) != NULL; ++x ) {
        setChar( x - *this, PATHSEP_CHAR );
    }
}

void WEXPORT WFileName::writeSelf( WObjectFile& p )
{
    const char  *x;
    char        from;

    if( PATHSEP_CHAR == '/' ) {
        from = '\\';
    } else {
        from = '/';
    }
    for( x = *this; (x = strchr( x, from )) != NULL; ++x ) {
        setChar( x - *this, PATHSEP_CHAR );
    }
    WString::writeSelf( p );
}
#endif

WFileName& WFileName::operator=( const WFileName& f )
{
    WString::operator=( f );
    return( *this );
}

void WEXPORT WFileName::merge( const char* name )
{
    pgroup2     s;

    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, &_x.ext );
    _splitpath2( name, s.buffer, NULL, NULL, &s.fname, &s.ext );
    if( strchr( _x.fname, '*' ) ) {
        _x.fname = s.fname;
    }
    if( strchr( _x.ext, '*' ) ) {
        _x.ext = s.ext;
    }
    makepath( _result_buf, _x.drive, _x.dir, _x.fname, _x.ext );
    *this = _result_buf;
}

void WEXPORT WFileName::relativeTo( const char* f )
{
    int     i;

    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, &_x.ext );
    if( _x.dir[0] == PATHSEP_CHAR ) {
        pgroup2         s;
        splitref( s, f, PATHSEP_CHAR );
        if( s.dir[0] == PATHSEP_CHAR && strieq( s.drive, _x.drive ) ) {
            _x.drive[0] = '\0';
            int b = 0;
            for( i = 1; _x.dir[i] != '\0' && s.dir[i] != '\0'; i++ ) {
                if( tolower( (unsigned char)_x.dir[i] ) != tolower( (unsigned char)s.dir[i] ) )
                    break;
                if( _x.dir[i] == PATHSEP_CHAR ) {
                    b = i;
                }
            }
            if( b == 0 ) {
                s.dir = _x.dir;
            } else {
                int n = 0;
                for( ; s.dir[i] != '\0'; i++ ) {
                    if( s.dir[i] == PATHSEP_CHAR ) {
                        n++;
                    }
                }
                s.dir[0] = '\0';
                if( n > 0 ) {
                    for( int j=0; j<n; j++ ) {
                        strcpy( &s.dir[3 * j], PARENTSEP_STR );
                    }
                }
                strcpy( &s.dir[3 * n], &_x.dir[b + 1] );
            }
            makepath( _result_buf, _x.drive, s.dir, _x.fname, _x.ext );
            *this = _result_buf;
        }
    }
}

void WEXPORT WFileName::absoluteTo( const char* f )
{
//
    size_t  icount = size();
    size_t  i;
    size_t  j;
    size_t  k;
    pgroup2 s;

    for( i = 0; i < icount; i++ ) {
        if( strncmp( &(*this)[i], "$(", 2 ) == 0 ) {
            return;
        }
    }
//
    relativeTo( f );
    splitref( s, f, PATHSEP_CHAR );
    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, &_x.ext );
    if( _x.drive[0] == '\0' ) {
        _x.drive = s.drive;
    }
    if( _x.dir[0] == PATHSEP_CHAR ) {
        s.dir = _x.dir;
    } else if( _x.dir[0] == '.' ) {
        for( i = 0; strnicmp( &_x.dir[i], PARENTSEP_STR, 3 ) == 0; i += 3 )
            ;
        removeSepFromEnd( s.dir, PATHSEP_CHAR );
        for( j = 0; j < i; j += 3 ) {
            for( k = strlen( s.dir ); k > 0; k-- ) {
                if( s.dir[k] == PATHSEP_CHAR ) {
                    break;
                }
            }
            s.dir[k] = '\0';
        }
        addSepToEnd( s.dir, PATHSEP_CHAR );
        strcat( s.dir, &_x.dir[i] );
    } else {
        strcat( s.dir, _x.dir );
    }
    makepath( _result_buf, _x.drive, s.dir, _x.fname, _x.ext );
    *this = _result_buf;
}

bool WEXPORT WFileName::setCWD() const
{
#ifndef __UNIX__
    int olddrive;
    bool ok;
#endif

    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, NULL, NULL );
#ifndef __UNIX__
    if( setdrive( _x.drive, &olddrive ) == 0 )
        return( false );
#endif
    if( _x.dir[0] == '\0' ) {
        return( true );
    }
    removeSepFromEnd( _x.dir, PATHSEP_CHAR );
#ifdef __UNIX__
    return( chdir( _x.dir ) == 0 );
#else
    ok = ( chdir( _x.dir ) == 0 );
    if( !ok ) {
        _chdrive( olddrive );
    }
    return( ok );
#endif
}

void WEXPORT WFileName::getCWD( bool slash )
{
    getcwd( _result_buf, sizeof( _result_buf ) - 1 );
    if( slash ) {
        addSepToEnd( _result_buf, PATHSEP_CHAR );
    }
    *this = _result_buf;
}

bool WEXPORT WFileName::makeDir() const
{
#ifndef __UNIX__
    int olddrive;
    bool ok;
#endif

    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, NULL, NULL );
#ifndef __UNIX__
    if( setdrive( _x.drive, &olddrive ) == 0 )
        return( false );
#endif
    if( _x.dir[0] == '\0' ) {
        return( true );
    }
    removeSepFromEnd( _x.dir, PATHSEP_CHAR );
#ifdef __UNIX__
    return( mkdir( _x.dir, PMODE_RX_USR_W ) == 0 );
#else
    ok = ( mkdir( _x.dir ) == 0 );
    _chdrive( olddrive );
    return( ok );
#endif
}

bool WEXPORT WFileName::dirExists() const
{
    WFileName t;
    t.getCWD();
    if( setCWD() ) {
        t.setCWD();
        return( true );
    }
    return( false );
}

bool WEXPORT WFileName::attribs( unsigned* attribs ) const
{
#ifdef __UNIX__
    /* XXX needs to be fixed: just to get it going */
    struct stat st;
    if (attribs != NULL ) {
        *attribs = 0;
    }
    return( stat( *this, &st ) == 0 );
#else
    struct _finddata_t fileinfo;
    intptr_t handle;
    int rc;
    bool found;

    found = false;
    handle = _findfirst( *this, &fileinfo );
    if( handle != -1 ) {
        for( rc = 0; rc != -1; rc = _findnext( handle, &fileinfo ) ) {
            if( (fileinfo.attrib & (_A_HIDDEN | _A_SYSTEM | _A_SUBDIR | _A_VOLID)) == 0 ) {
                if( attribs != NULL ) {
                    *attribs = fileinfo.attrib;
                }
                found = true;
                break;
            }
        }
        _findclose( handle );
    }
    return( found );
#endif
}

void WEXPORT WFileName::touch( time_t tm ) const
{
    if( tm == 0 ) {
        struct tm t;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_mday = 1;
        t.tm_mon = 0;
        t.tm_year = 80;
        t.tm_wday = 0;
        t.tm_yday = 0;
        t.tm_isdst = 0;
        tm = mktime( &t );
    }
    struct utimbuf ut;
    ut.actime = 0;
    ut.modtime = tm;
    utime( *this, &ut );
}

void WEXPORT WFileName::setDrive( const char* drive )
{
    char    tmp_buf[PATH_MAX + 1];

    _splitpath2( *this, _x.buffer, NULL, &_x.dir, &_x.fname, &_x.ext );
    makepath( tmp_buf, drive, _x.dir, _x.fname, _x.ext );
    *this = tmp_buf;
}

const char* WEXPORT WFileName::drive() const
{
    _splitpath2( *this, _result_buf, &_x.drive, NULL, NULL, NULL );
    return( _x.drive );
}

void WEXPORT WFileName::setDir( const char* dir )
{
    char    tmp_buf[PATH_MAX + 1];

    _splitpath2( *this, _x.buffer, &_x.drive, NULL, &_x.fname, &_x.ext );
    makepath( tmp_buf, _x.drive, dir, _x.fname, _x.ext );
    *this = tmp_buf;
}

const char* WEXPORT WFileName::dir( bool slash ) const
{
    _splitpath2( *this, _result_buf, NULL, &_x.dir, NULL, NULL );
    if( !slash ) {
        removeSepFromEnd( _x.dir, PATHSEP_CHAR );
    }
    return( _x.dir );
}

void WEXPORT WFileName::setFName( const char* fName )
{
    char    tmp_buf[PATH_MAX + 1];

    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, NULL, &_x.ext );
    makepath( tmp_buf, _x.drive, _x.dir, fName, _x.ext );
    *this = tmp_buf;
}

const char* WEXPORT WFileName::fName() const
{
    _splitpath2( *this, _result_buf, NULL, NULL, &_x.fname, NULL );
    return( _x.fname );
}

void WEXPORT WFileName::setExt( const char* ext )
{
    char    tmp_buf[PATH_MAX + 1];

    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, NULL );
    makepath( tmp_buf, _x.drive, _x.dir, _x.fname, ext );
    *this = tmp_buf;
}

const char* WEXPORT WFileName::ext() const
{
    _splitpath2( *this, _result_buf, NULL, NULL, NULL, &_x.ext );
    return( _x.ext );
}

void WEXPORT WFileName::setExtIfNone( const char* ext )
{
    char    tmp_buf[PATH_MAX + 1];

    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, &_x.ext );
    if( _x.ext[0] == '\0' ) {
        makepath( tmp_buf, _x.drive, _x.dir, _x.fname, ext );
        *this = tmp_buf;
    }
}

static const char legalChars[] = { "_^$~!#%&-{}()@`'." };
static bool isSpecialChar( char ch ) {
    char const        *ptr;

    for( ptr = legalChars; *ptr != '\0'; ptr++ ) {
        if( *ptr == ch ) {
            return( true );
        }
    }
    return( false );
}

static const char illegalChars[] = { "\\/:*?\"<>|" };
static bool isIllegalChar( char ch ) {
    char const        *ptr;

    for( ptr = illegalChars; *ptr != '\0'; ptr++ ) {
        if( *ptr == ch ) {
            return( true );
        }
    }
    return( false );
}

static bool isLongName( char* fname )
{

    if( fname[0] != '\0' ) {
        size_t len = strlen( fname );
        for( size_t i = 0; i < len; i++ ) {
            char ch = fname[i];
            if( !isalnum( ch ) && !isSpecialChar( ch ) ) {
               return( true );
            }
        }
    }
    return( false );
}

static bool isLongDirName( char* dirNames, const char *pathseps )
{
    char* cpDirNames;
    char* aDirName;
    bool rc = false;

    cpDirNames = strdup( dirNames );
    for( aDirName = strtok( cpDirNames, pathseps ); aDirName != NULL; aDirName = strtok( NULL, pathseps ) ) {
        if( isLongName( aDirName ) ) {
            rc = true;
            break;
        }
    }
    free( cpDirNames );
    return( rc );
}

bool WEXPORT WFileName::needQuotes( char ch ) const
{
    if( !isMask() ) {
        size_t len = size();
        if( len > 0 && ( (*this)[(size_t)0] != ch || (*this)[len - 1] != ch ) ) {
            _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, &_x.ext );
            if( isLongDirName( _x.dir, PATHSEP_STR ) || isLongName( _x.fname ) ) {
                return( true );
            }
        }
    }
    return( false );
}

void WEXPORT WFileName::removeQuotes( char ch )
{
    size_t len = size();

    if( len > 1 ) {
        len--;
        if( (*this)[(size_t)0] == ch && (*this)[len] == ch ) {
            deleteChar( len );
            deleteChar( 0 );
        }
    }
}

void WEXPORT WFileName::addQuotes( char ch )
{
    size_t len = size();
    _result_buf[0] = ch;
    for( size_t i=0; i<len; i++ ) {
        _result_buf[i + 1] = (*this)[i];
    }
    _result_buf[len + 1] = ch;
    _result_buf[len + 2] = '\0';
    *this = _result_buf;
}

bool WEXPORT WFileName::legal() const
{
    if( !isMask() ) {
        _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, &_x.ext );
        if( _x.fname[0] != '\0' ) {
            bool isLong = needQuotes();
            size_t len = strlen( _x.fname );
            for( size_t i = 0; i < len; i++ ) {
                char ch = _x.fname[i];
                if( isLong ) {
                    if( isIllegalChar( ch ) ) {
                        return( false );
                    }
                } else {
                    if( !isalnum( ch ) && !isSpecialChar( ch ) ) {
                        return( false );
                    }
                }
            }
            return( true );
        }
    }
    return( false );
}

bool WEXPORT WFileName::match( const char* mask, char ctrlFlags ) const
{
    if( mask == NULL )
        mask = "";
    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, &_x.ext );
    pgroup2     m;
    _splitpath2( mask, m.buffer, &m.drive, &m.dir, &m.fname, &m.ext );
    bool ok = true;
    if( ok && (ctrlFlags & matchDrive) ) {
        WString d( _x.drive );
        ok = ok & d.match( m.drive );
    }
    if( ok && (ctrlFlags & matchDir) ) {
        WString d( _x.dir );
        ok = ok & d.match( m.dir );
    }
    if( ok && (ctrlFlags & matchFName) ) {
        WString d( _x.fname );
        ok = ok & d.match( m.fname );
    }
    if( ok && (ctrlFlags & matchExt) ) {
        WString d( _x.ext );
        ok = ok & d.match( m.ext );
    }
    return( ok );
}

void WEXPORT WFileName::fullName( WFileName& f ) const
{
    _result_buf[0] = '\0';
    _fullpath( _result_buf, *this, sizeof( _result_buf ) );
    f = _result_buf;
}

void WEXPORT WFileName::noExt( WFileName& f ) const
{
    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, NULL );
    makepath( _result_buf, _x.drive, _x.dir, _x.fname, NULL );
    f = _result_buf;
}

void WEXPORT WFileName::noPath( WFileName& f ) const
{
    _splitpath2( *this, _x.buffer, NULL, NULL, &_x.fname, &_x.ext );
    makepath( _result_buf, NULL, NULL, _x.fname, _x.ext );
    f = _result_buf;
}

void WEXPORT WFileName::noPathNoExt( WFileName& f ) const
{
    _splitpath2( *this, _x.buffer, NULL, NULL, &_x.fname, NULL );
    makepath( _result_buf, NULL, NULL, _x.fname, NULL );
    f = _result_buf;
}

void WEXPORT WFileName::path( WFileName& f, bool slash ) const
{
    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, NULL, NULL );
    makepath( _result_buf, _x.drive, _x.dir, NULL, NULL );
    if( !slash ) {
        removeSepFromEnd( _result_buf, PATHSEP_CHAR );
    }
    f = _result_buf;
}

bool WEXPORT WFileName::removeFile() const
{
    return( remove( *this ) == 0 );
}

bool WEXPORT WFileName::renameFile( const char* newname ) const
{
    return( rename( *this, newname ) == 0 );
}


char WEXPORT WFileName::getPathSep()
{
    return( PATHSEP_CHAR );
}

char WEXPORT WFileName::setPathSep( char pathsep )
{
    char    old_pathsep;

    old_pathsep = PATHSEP_CHAR;
    PATHSEP_CHAR = pathsep;
    return( old_pathsep );
}

void WEXPORT WFileName::normalize()
{
    if( PATHSEP_CHAR == '/' ) {

    } else {

    }
}

bool WEXPORT WFileName::addPath( const char *path )
{
    pgroup2     s;

    if( path == NULL ) {
        return( false );
    }
    _splitpath2( *this, _x.buffer, &_x.drive, &_x.dir, &_x.fname, &_x.ext );
    if( _x.dir[0] == PATHSEP_CHAR || _x.dir[0] == '.' ) {
        return( false );
    }
    _splitpath2( path, s.buffer, &s.drive, &s.dir, NULL, NULL );
    if( s.dir[0] == '\0' && s.drive[0] == '\0' ) {
        return( false );
    }
    if( _x.drive[0] != '\0' && _x.drive[0] != s.drive[0] ) {
        return( false );
    }
    strcat( s.dir, _x.dir );
    makepath( _result_buf, s.drive, s.dir, _x.fname, _x.ext );
    *this = _result_buf;
    return( true );
}

#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
#pragma disable_message( 657 )
#endif

WEXPORT WFileName::~WFileName()
{
}
