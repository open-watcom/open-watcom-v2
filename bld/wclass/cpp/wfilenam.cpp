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

static void removeSepFromEnd( char *dir, char pathsep )
{
    size_t len;

#ifndef __UNIX__
    if( dir[0] != '\0' && dir[1] == ':' )
        dir += 2;
#endif
    if( dir[0] != '\0' && dir[1] != '\0' ) {
        len = strlen( dir ) - 1;
        if( dir[len] == pathsep ) {
            dir[len] = '\0';
        }
    }
}

static void addSepToEnd( char *dir, char pathsep )
{
    size_t len;

#ifndef __UNIX__
    if( dir[0] != '\0' && dir[1] == ':' )
        dir += 2;
#endif
    if( dir[0] != '\0' && ( dir[0] != '\\' || dir[1] != '\0' ) ) {
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
    char    tmp[PATH_MAX + 1];

    if( f != NULL ) {
        _splitpath2( f, s.buffer, &s.drive, &s.dir, &s.fname, &s.ext );
    } else {
        getcwd( tmp, sizeof( tmp ) );
        addSepToEnd( tmp, pathsep );
        _splitpath2( tmp, s.buffer, &s.drive, &s.dir, &s.fname, &s.ext );
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
    const char  *s;
    char        from;

    WString::readSelf( p );
    if( PATHSEP_CHAR == '/' ) {
        from = '\\';
    } else {
        from = '/';
    }
    s = *this;
    for( x = s; (x = strchr( x, from )) != NULL; ++x ) {
        setChar( x - s, PATHSEP_CHAR );
    }
}

void WEXPORT WFileName::writeSelf( WObjectFile& p )
{
    const char  *x;
    const char  *s;
    char        from;

    if( PATHSEP_CHAR == '/' ) {
        from = '\\';
    } else {
        from = '/';
    }
    s = *this;
    for( x = s; (x = strchr( x, from )) != NULL; ++x ) {
        setChar( x - s, PATHSEP_CHAR );
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
    pgroup2 x;
    pgroup2 s;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, &x.ext );
    _splitpath2( name, s.buffer, NULL, NULL, &s.fname, &s.ext );
    if( strchr( x.fname, '*' ) ) {
        x.fname = s.fname;
    }
    if( strchr( x.ext, '*' ) ) {
        x.ext = s.ext;
    }
    makepath( tmp, x.drive, x.dir, x.fname, x.ext );
    *this = tmp;
}

void WEXPORT WFileName::relativeTo( const char* f )
{
    pgroup2 x;
    pgroup2 s;
    char    tmp[PATH_MAX + 1];
    int     i;

    _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, &x.ext );
    if( x.dir[0] == PATHSEP_CHAR ) {
        splitref( s, f, PATHSEP_CHAR );
        if( s.dir[0] == PATHSEP_CHAR && strieq( s.drive, x.drive ) ) {
            x.drive[0] = '\0';
            int b = 0;
            for( i = 1; x.dir[i] != '\0' && s.dir[i] != '\0'; i++ ) {
                if( tolower( (unsigned char)x.dir[i] ) != tolower( (unsigned char)s.dir[i] ) )
                    break;
                if( x.dir[i] == PATHSEP_CHAR ) {
                    b = i;
                }
            }
            if( b == 0 ) {
                s.dir = x.dir;
            } else {
                int n = 0;
                for( ; s.dir[i] != '\0'; i++ ) {
                    if( s.dir[i] == PATHSEP_CHAR ) {
                        n++;
                    }
                }
                s.dir[0] = '\0';
                if( n > 0 ) {
                    for( int j = 0; j < n; j++ ) {
                        strcpy( &s.dir[3 * j], PARENTSEP_STR );
                    }
                }
                strcpy( &s.dir[3 * n], &x.dir[b + 1] );
            }
            makepath( tmp, x.drive, s.dir, x.fname, x.ext );
            *this = tmp;
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
    pgroup2 x;
    pgroup2 s;
    char    tmp[PATH_MAX + 1];

    for( i = 0; i < icount; i++ ) {
        if( strncmp( &(*this)[i], "$(", 2 ) == 0 ) {
            return;
        }
    }
//
    relativeTo( f );
    splitref( s, f, PATHSEP_CHAR );
    _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, &x.ext );
    if( x.drive[0] == '\0' ) {
        x.drive = s.drive;
    }
    if( x.dir[0] == PATHSEP_CHAR ) {
        s.dir = x.dir;
    } else if( x.dir[0] == '.' ) {
        for( i = 0; strnicmp( &x.dir[i], PARENTSEP_STR, 3 ) == 0; i += 3 )
            ;
        removeSepFromEnd( s.dir, PATHSEP_CHAR );
        k = strlen( s.dir );
        for( j = 0; k > 0 && j < i; j += 3 ) {
            for( ; k > 0; k-- ) {
                if( s.dir[k] == PATHSEP_CHAR ) {
                    break;
                }
            }
            if( k == 0 && s.dir[0] == PATHSEP_CHAR ) {
                s.dir[1] = '\0';
            } else {
                s.dir[k] = '\0';
            }
        }
        addSepToEnd( s.dir, PATHSEP_CHAR );
        strcat( s.dir, &x.dir[i] );
    } else {
        strcat( s.dir, x.dir );
    }
    makepath( tmp, x.drive, s.dir, x.fname, x.ext );
    *this = tmp;
}

bool WEXPORT WFileName::setCWD() const
{
    pgroup2 x;
#ifndef __UNIX__
    int     olddrive;
    bool    ok;
#endif

    _splitpath2( *this, x.buffer, &x.drive, &x.dir, NULL, NULL );
#ifndef __UNIX__
    if( setdrive( x.drive, &olddrive ) == 0 )
        return( false );
#endif
    if( x.dir[0] == '\0' ) {
        return( true );
    }
    removeSepFromEnd( x.dir, PATHSEP_CHAR );
#ifdef __UNIX__
    return( chdir( x.dir ) == 0 );
#else
    ok = ( chdir( x.dir ) == 0 );
    if( !ok ) {
        _chdrive( olddrive );
    }
    return( ok );
#endif
}

void WEXPORT WFileName::getCWD( bool slash )
{
    char    tmp[PATH_MAX + 1];

    getcwd( tmp, sizeof( tmp ) - 1 );
    if( slash ) {
        addSepToEnd( tmp, PATHSEP_CHAR );
    }
    *this = tmp;
}

bool WEXPORT WFileName::makeDir() const
{
    pgroup2 x;
#ifndef __UNIX__
    int     olddrive;
    bool    ok;
#endif

    _splitpath2( *this, x.buffer, &x.drive, &x.dir, NULL, NULL );
#ifndef __UNIX__
    if( setdrive( x.drive, &olddrive ) == 0 )
        return( false );
#endif
    if( x.dir[0] == '\0' ) {
        return( true );
    }
    removeSepFromEnd( x.dir, PATHSEP_CHAR );
#ifdef __UNIX__
    return( mkdir( x.dir, PMODE_RX_USR_W ) == 0 );
#else
    ok = ( mkdir( x.dir ) == 0 );
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
    if( attribs != NULL ) {
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
    pgroup2 x;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, NULL, &x.dir, &x.fname, &x.ext );
    makepath( tmp, drive, x.dir, x.fname, x.ext );
    *this = tmp;
}

const char* WEXPORT WFileName::drive()
{
    pgroup2 x;

    _splitpath2( *this, x.buffer, &x.drive, NULL, NULL, NULL );
    _drive.puts( x.drive );
    return( _drive );
}

void WEXPORT WFileName::setDir( const char* dir )
{
    pgroup2 x;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, &x.drive, NULL, &x.fname, &x.ext );
    makepath( tmp, x.drive, dir, x.fname, x.ext );
    *this = tmp;
}

const char* WEXPORT WFileName::dir( bool slash )
{
    pgroup2 x;

    _splitpath2( *this, x.buffer, NULL, &x.dir, NULL, NULL );
    if( !slash ) {
        removeSepFromEnd( x.dir, PATHSEP_CHAR );
    }
    _dir.puts( x.dir );
    return( _dir );
}

void WEXPORT WFileName::setFName( const char* fName )
{
    pgroup2 x;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, &x.drive, &x.dir, NULL, &x.ext );
    makepath( tmp, x.drive, x.dir, fName, x.ext );
    *this = tmp;
}

const char* WEXPORT WFileName::fName()
{
    pgroup2 x;

    _splitpath2( *this, x.buffer, NULL, NULL, &x.fname, NULL );
    _fname.puts( x.fname );
    return( _fname );
}

void WEXPORT WFileName::setExt( const char* ext )
{
    pgroup2 x;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, NULL );
    makepath( tmp, x.drive, x.dir, x.fname, ext );
    *this = tmp;
}

const char* WEXPORT WFileName::ext()
{
    pgroup2 x;

    _splitpath2( *this, x.buffer, NULL, NULL, NULL, &x.ext );
    _ext.puts( x.ext );
    return( _ext );
}

void WEXPORT WFileName::setExtIfNone( const char* ext )
{
    pgroup2 x;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, &x.ext );
    if( x.ext[0] == '\0' ) {
        makepath( tmp, x.drive, x.dir, x.fname, ext );
        *this = tmp;
    }
}

static const char legalChars[] = { "_^$~!#%&-{}()@`'." };
static bool isSpecialChar( char ch )
{
    char const        *ptr;

    for( ptr = legalChars; *ptr != '\0'; ptr++ ) {
        if( *ptr == ch ) {
            return( true );
        }
    }
    return( false );
}

static const char illegalChars[] = { "\\/:*?\"<>|" };
static bool isIllegalChar( char ch )
{
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
    pgroup2 x;

    if( !isMask() ) {
        size_t len = size();
        if( len > 0 && ( (*this)[(size_t)0] != ch || (*this)[len - 1] != ch ) ) {
            _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, &x.ext );
            if( isLongDirName( x.dir, PATHSEP_STR ) || isLongName( x.fname ) ) {
                return( true );
            }
        }
    }
    return( false );
}

void WEXPORT WFileName::removeQuotes( char ch )
{
    size_t len = size();

    if( len-- > 1 ) {
        if( (*this)[(size_t)0] == ch && (*this)[len] == ch ) {
            deleteChar( len );
            deleteChar( 0 );
        }
    }
}

void WEXPORT WFileName::addQuotes( char ch )
{
    char        tmp[PATH_MAX + 3];
    char        *d;
    const char  *s;
    size_t      len;

    len = size();
    d = tmp;
    s = *this;
    *d++ = ch;
    while( len-- > 0 ) {
        *d++ = *s++;
    }
    *d++ = ch;
    *d = '\0';
    *this = tmp;
}

bool WEXPORT WFileName::legal() const
{
    pgroup2 x;

    if( !isMask() ) {
        _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, &x.ext );
        if( x.fname[0] != '\0' ) {
            bool isLong = needQuotes();
            size_t len = strlen( x.fname );
            for( size_t i = 0; i < len; i++ ) {
                char ch = x.fname[i];
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
    pgroup2 x;
    pgroup2 m;

    if( mask == NULL )
        mask = "";
    _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, &x.ext );
    _splitpath2( mask, m.buffer, &m.drive, &m.dir, &m.fname, &m.ext );
    bool ok = true;
    if( ok && (ctrlFlags & matchDrive) ) {
        WString d( x.drive );
        ok = ok & d.match( m.drive );
    }
    if( ok && (ctrlFlags & matchDir) ) {
        WString d( x.dir );
        ok = ok & d.match( m.dir );
    }
    if( ok && (ctrlFlags & matchFName) ) {
        WString d( x.fname );
        ok = ok & d.match( m.fname );
    }
    if( ok && (ctrlFlags & matchExt) ) {
        WString d( x.ext );
        ok = ok & d.match( m.ext );
    }
    return( ok );
}

void WEXPORT WFileName::fullName( WFileName& f ) const
{
    char    tmp[PATH_MAX + 1];

    tmp[0] = '\0';
    _fullpath( tmp, *this, sizeof( tmp ) );
    f = tmp;
}

void WEXPORT WFileName::noExt( WFileName& f ) const
{
    pgroup2 x;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, NULL );
    makepath( tmp, x.drive, x.dir, x.fname, NULL );
    f = tmp;
}

void WEXPORT WFileName::noPath( WFileName& f ) const
{
    pgroup2 x;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, NULL, NULL, &x.fname, &x.ext );
    makepath( tmp, NULL, NULL, x.fname, x.ext );
    f = tmp;
}

void WEXPORT WFileName::noPathNoExt( WFileName& f ) const
{
    pgroup2 x;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, NULL, NULL, &x.fname, NULL );
    makepath( tmp, NULL, NULL, x.fname, NULL );
    f = tmp;
}

void WEXPORT WFileName::path( WFileName& f, bool slash ) const
{
    pgroup2 x;
    char    tmp[PATH_MAX + 1];

    _splitpath2( *this, x.buffer, &x.drive, &x.dir, NULL, NULL );
    makepath( tmp, x.drive, x.dir, NULL, NULL );
    if( !slash ) {
        removeSepFromEnd( tmp, PATHSEP_CHAR );
    }
    f = tmp;
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
    pgroup2 x;
    pgroup2 s;
    char    tmp[PATH_MAX + 1];

    if( path == NULL ) {
        return( false );
    }
    _splitpath2( *this, x.buffer, &x.drive, &x.dir, &x.fname, &x.ext );
    if( x.dir[0] == PATHSEP_CHAR || x.dir[0] == '.' ) {
        return( false );
    }
    _splitpath2( path, s.buffer, &s.drive, &s.dir, NULL, NULL );
    if( s.dir[0] == '\0' && s.drive[0] == '\0' ) {
        return( false );
    }
    if( x.drive[0] != '\0' && x.drive[0] != s.drive[0] ) {
        return( false );
    }
    strcat( s.dir, x.dir );
    makepath( tmp, s.drive, s.dir, x.fname, x.ext );
    *this = tmp;
    return( true );
}

#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
#pragma disable_message( 657 )
#endif

WEXPORT WFileName::~WFileName()
{
}
