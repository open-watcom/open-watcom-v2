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


#include "wfilenam.hpp"
#include "wobjfile.hpp"

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
#ifndef __UNIX__
    #include <direct.h>
    #include <dos.h>
#endif
    #include <time.h>
#ifdef __UNIX__
    #include <sys/types.h>
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
    #include "wio.h"
};

#include "clibext.h"


#define PATHSEP_CHAR    (_pathsep[2])
#define PATHSEP_STR     (_pathsep + 2)
#define PARENTSEP_STR   (_pathsep)

typedef struct fullName {
    char        path[_MAX_PATH + 1];
    char        drive[_MAX_DRIVE + 1];
    char        dir[_MAX_DIR + 1];
    char        fname[_MAX_FNAME + 1];
    char        ext[_MAX_EXT + 1];
} FullName;

#ifndef __UNIX__
static bool setdrive( const char* drive, unsigned* olddrive )
{
    if( strlen( drive ) > 0 ) {
        _dos_getdrive( olddrive );
        unsigned drv = toupper( drive[0]) - 'A' + 1;    //1='A'; 2='B'; ...
        if( *olddrive != drv ) {
            unsigned total; _dos_setdrive( drv, &total );
            unsigned newdrive; _dos_getdrive( &newdrive );
            if( drv != newdrive ) {
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

static void splitpath( const char* path, char* drive, char* dir, char* fname, char* ext, const char* pathsep )
{
    _splitpath( path, drive, dir, fname, ext );
    if( dir != NULL ) {
        size_t dirlen = strlen( dir );
        if( dirlen > 1 && dir[dirlen - 1] == pathsep[0] ) {
            dir[dirlen - 1] = '\0';
        }
    }
}

static void splitref( FullName& s, const char* f, const char *pathsep )
{
    if( f != NULL ) {
        _splitpath( f, s.drive, s.dir, s.fname, s.ext );
    } else {
        char cwd[_MAX_PATH + 1];
        getcwd( cwd, sizeof( cwd ) );
        size_t icount = strlen( cwd );
        if( cwd[icount - 1] != pathsep[0] ) {
            cwd[icount] = pathsep[0];
            cwd[icount + 1] = '\0';
        }
        _splitpath( cwd, s.drive, s.dir, s.fname, s.ext );
    }
}

static FullName _x;

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
    const char *x;
    char from;

    WString::readSelf( p );
    if( PATHSEP_CHAR == '/' ) {
        from = '\\';
    } else {
        from = '/';
    }
    for( x = *this; (x = strchr( x, from )) != NULL; ++x ) {
        *(char *)x = PATHSEP_CHAR;
    }
}

void WEXPORT WFileName::writeSelf( WObjectFile& p )
{
    const char *x;
    char    from;

    if( PATHSEP_CHAR == '/' ) {
        from = '\\';
    } else {
        from = '/';
    }
    for( x = *this; (x = strchr( x, from )) != NULL; ++x ) {
        *(char *)x = PATHSEP_CHAR;
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
    _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    FullName    s;
    _splitpath( name, s.drive, s.dir, s.fname, s.ext );
    if( strchr( _x.fname, '*' ) ) {
        strcpy( _x.fname, s.fname );
    }
    if( strchr( _x.ext, '*' ) ) {
        strcpy( _x.ext, s.ext );
    }
    makepath( _x.path, _x.drive, _x.dir, _x.fname, _x.ext );
    *this = _x.path;
}

void WEXPORT WFileName::relativeTo( const char* f )
{
    int     i;

    _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    if( _x.dir[0] == PATHSEP_CHAR ) {
        FullName        s;
        splitref( s, f, PATHSEP_STR );
        if( s.dir[0] == PATHSEP_CHAR && strieq( s.drive, _x.drive ) ) {
            _x.drive[0] = '\0';
            int b = 0;
            for( i=1; _x.dir[i] != '\0' && s.dir[i] != '\0'; i++ ) {
                if( tolower( (unsigned char)_x.dir[i] ) != tolower( (unsigned char)s.dir[i] ) ) break;
                if( s.dir[i] == PATHSEP_CHAR ) b = i;
            }
            if( b == 0 ) {
                strcpy( s.dir, _x.dir );
            } else {
                int n = 0;
                for( ; s.dir[i] != '\0'; i++ ) {
                    if( s.dir[i] == PATHSEP_CHAR )  n++;
                }
                s.dir[0] = '\0';
                if( n > 0 ) {
                    for( int j=0; j<n; j++ ) {
                        strcpy( &s.dir[3 * j], PARENTSEP_STR );
                    }
                }
                strcpy( &s.dir[3 * n], &_x.dir[b + 1] );
            }
            makepath( _x.path, _x.drive, s.dir, _x.fname, _x.ext );
            *this = _x.path;
        }
    }
}

void WEXPORT WFileName::absoluteTo( const char* f )
{
//
    size_t icount = size();
    size_t i, j;
    size_t k;

    for( i = 0; i < icount; i++ ) {
        if( strncmp( &(*this)[i], "$(", 2 ) == 0 ) {
            return;
        }
    }
//
    relativeTo( f );
    FullName    s;
    splitref( s, f, PATHSEP_STR );
    _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    if( _x.drive[0] == '\0' ) {
        strcpy( _x.drive, s.drive );
    }
    if( _x.dir[0] == PATHSEP_CHAR ) {
        strcpy( s.dir, _x.dir );
    } else if( _x.dir[0] == '.' ) {
        for( i = 0; strnicmp( &_x.dir[i], PARENTSEP_STR, 3 ) == 0; i += 3 );
        size_t slen = strlen( s.dir );
        if( slen > 0 && s.dir[slen - 1] == PATHSEP_CHAR ) {
            s.dir[slen - 1] = '\0';
        }
        for( j = 0; j < i; j += 3 ) {
            for( k = strlen( s.dir ); k > 0; k-- ) {
                if( s.dir[k] == PATHSEP_CHAR ) break;
            }
            s.dir[k] = '\0';
        }
        strcat( s.dir, PATHSEP_STR );
        strcat( s.dir, &_x.dir[i] );
    } else {
        strcat( s.dir, _x.dir );
    }
    makepath( _x.path, _x.drive, s.dir, _x.fname, _x.ext );
    *this = _x.path;
}

#ifdef __UNIX__
bool WEXPORT WFileName::setCWD() const
{
    splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext, PATHSEP_STR );
    if( strlen( _x.dir ) > 0 ) {
        return( chdir( _x.dir ) == 0 );
    }
    return( true );
}
#else
bool WEXPORT WFileName::setCWD() const
{
    splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext, PATHSEP_STR );
    unsigned olddrive;
    if( setdrive( _x.drive, &olddrive ) ) {
        if( strlen( _x.dir ) > 0 ) {
            int ret = chdir( _x.dir );
            if( ret == 0 ) {
                return( true );
            }
            unsigned total;
            _dos_setdrive( olddrive, &total );
            return( false );
        }
        return( true );
    }
    return( false );
}
#endif

void WEXPORT WFileName::getCWD( bool slash )
{
    getcwd( _x.path, sizeof( _x.path ) );
    if( slash ) {
        strcat( _x.path, PATHSEP_STR );
    }
    *this = _x.path;
}

#ifdef __UNIX__
bool WEXPORT WFileName::makeDir() const
{
    splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext, PATHSEP_STR );
    if( strlen( _x.dir ) > 0 ) {
        return( mkdir( _x.dir, 0755 ) == 0 );
    }
    return( true );
}
#else
bool WEXPORT WFileName::makeDir() const
{
    splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext, PATHSEP_STR );
    if( strlen( _x.dir ) > 0 ) {
        unsigned olddrive;
        if( setdrive( _x.drive, &olddrive ) ) {
            int ret = mkdir( _x.dir );
            unsigned total;
            _dos_setdrive( olddrive, &total );
            return( ret == 0 );
        }
        return( false );
    }
    return( true );
}
#endif

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

#ifdef __UNIX__
bool WEXPORT WFileName::attribs( char* attribs ) const
{
    /* XXX needs to be fixed: just to get it going */
    struct stat st;
    if (attribs != NULL ) {
        *attribs = 0;
    }
    return( stat( *this, &st ) == 0 );
}
#else
bool WEXPORT WFileName::attribs( char* attribs ) const
{
    struct find_t fileinfo;
    #define FIND_STYLE _A_NORMAL
    int rc = _dos_findfirst( *this, FIND_STYLE, &fileinfo );
    if( rc == 0 ) {
        if( attribs != NULL ) {
            *attribs = fileinfo.attrib;
        }
    }
    #undef FIND_STYLE
    #ifndef __WINDOWS__
    _dos_findclose( &fileinfo );
    #endif
    return( rc == 0 );
}
#endif

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
    _splitpath( *this, NULL, _x.dir, _x.fname, _x.ext );
    makepath( _x.path, drive, _x.dir, _x.fname, _x.ext );
    *this = _x.path;
}

const char* WEXPORT WFileName::drive() const
{
    _splitpath( *this, _x.drive, NULL, NULL, NULL );
    return( _x.drive );
}

void WEXPORT WFileName::setDir( const char* dir )
{
    _splitpath( *this, _x.drive, NULL, _x.fname, _x.ext );
    makepath( _x.path, _x.drive, dir, _x.fname, _x.ext );
    *this = _x.path;
}

const char* WEXPORT WFileName::dir( bool slash ) const
{
    if( slash ) {
        _splitpath( *this, NULL, _x.dir, NULL, NULL );
    } else {
        splitpath( *this, NULL, _x.dir, NULL, NULL, PATHSEP_STR );
    }
    return( _x.dir );
}

void WEXPORT WFileName::setFName( const char* fName )
{
    _splitpath( *this, _x.drive, _x.dir, NULL, _x.ext );
    makepath( _x.path, _x.drive, _x.dir, fName, _x.ext );
    *this = _x.path;
}

const char* WEXPORT WFileName::fName() const
{
    _splitpath( *this, NULL, NULL, _x.fname, NULL );
    return( _x.fname );
}

void WEXPORT WFileName::setExt( const char* ext )
{
    _splitpath( *this, _x.drive, _x.dir, _x.fname, NULL );
    makepath( _x.path, _x.drive, _x.dir, _x.fname, ext );
    *this = _x.path;
}

const char* WEXPORT WFileName::ext() const
{
    _splitpath( *this, NULL, NULL, NULL, _x.ext );
    return( _x.ext );
}

static const char legalChars[] = { "_^$~!#%&-{}()@`'." };
static bool isSpecialChar( char ch ) {
    char const        *ptr;

    ptr = legalChars;
    while( *ptr != '\0' ) {
        if( *ptr == ch )
            return( true );
        ptr++;
    }
    return( false );
}

static const char illegalChars[] = { "\\/:*?\"<>|" };
static bool isIllegalChar( char ch ) {
    char const        *ptr;

    ptr = illegalChars;
    while( *ptr != '\0' ) {
        if( *ptr == ch )
            return( true );
        ptr++;
    }
    return( false );
}

static bool isLongName( char* fname )
{
    size_t len = strlen( fname );
    if( len > 0 ) {
        for( size_t i = 0; i < len; i++ ) {
            char ch = fname[i];
            if( !isalnum( ch ) && !isSpecialChar( ch ) ) {
               return( true );
            }
        }
        return( false );
    }
    return( false );
}

static bool isLongDirName( char* dirNames, const char *pathsep )
{
    char* cpDirNames;
    char* aDirName;
    bool rc = false;

    cpDirNames = strdup( dirNames );
    aDirName = strtok( cpDirNames, pathsep );
    while( aDirName != NULL ) {
        if( isLongName( aDirName ) ) {
            rc = true;
            break;
        }
        aDirName = strtok( NULL, pathsep );
    }
    return( rc );
}

bool WEXPORT WFileName::needQuotes( char ch ) const
{
    if( !isMask() ) {
        size_t len = size();
        if( len > 0 && ( (*this)[(size_t)0] != ch || (*this)[len - 1] != ch ) ) {
            _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
            if( isLongDirName( _x.dir, PATHSEP_STR ) || isLongName( _x.fname ) ) {
                return( true );
            }
        }
    }
    return( false );
}

void WEXPORT WFileName::removeQuotes( char ch )
{
    size_t len = size() - 1;
    if( (*this)[(size_t)0] == ch && (*this)[len] == ch ) {
        deleteChar( len );
        deleteChar( 0 );
    }
}

void WEXPORT WFileName::addQuotes( char ch )
{
    size_t len = size();
    char* quotedName = new char[len + 3];
    quotedName[0] = ch;
    for( size_t i=0; i<len; i++ ) {
        quotedName[i + 1] = (*this)[i];
    }
    quotedName[len + 1] = ch;
    quotedName[len + 2] = '\0';
    (*this) = quotedName;
    delete [] quotedName;
}

bool WEXPORT WFileName::legal() const
{
    if( !isMask() ) {
        _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
        bool isLong = needQuotes();
        size_t len = strlen( _x.fname );
        if( len > 0 ) {
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
    if( mask == NULL ) mask = "";
    _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    FullName    m;
    _splitpath( mask, m.drive, m.dir, m.fname, m.ext );
    bool ok = true;
    if( ok && (ctrlFlags&matchDrive) ) {
        WString d( _x.drive );
        ok = ok & d.match( m.drive );
    }
    if( ok && (ctrlFlags&matchDir) ) {
        WString d( _x.dir );
        ok = ok & d.match( m.dir );
    }
    if( ok && (ctrlFlags&matchFName) ) {
        WString d( _x.fname );
        ok = ok & d.match( m.fname );
    }
    if( ok && (ctrlFlags&matchExt) ) {
        WString d( _x.ext );
        ok = ok & d.match( m.ext );
    }
    return( ok );
}

void WEXPORT WFileName::fullName( WFileName& f ) const
{
    char buff[_MAX_PATH + 1];
    buff[0] = '\0';
    _fullpath( buff, *this, _MAX_PATH );
    f = buff;
}

void WEXPORT WFileName::noExt( WFileName& f ) const
{
    _splitpath( *this, _x.drive, _x.dir, _x.fname, NULL );
    makepath( _x.path, _x.drive, _x.dir, _x.fname, NULL );
    f = _x.path;
}

void WEXPORT WFileName::noPath( WFileName& f ) const
{
    _splitpath( *this, NULL, NULL, _x.fname, _x.ext );
    makepath( _x.path, NULL, NULL, _x.fname, _x.ext );
    f = _x.path;
}

void WEXPORT WFileName::path( WFileName& f, bool slash ) const
{
    _splitpath( *this, _x.drive, _x.dir, NULL, NULL );
    makepath( _x.path, _x.drive, _x.dir, NULL, NULL );
    if( !slash ) {
        size_t len = strlen( _x.path );
        if( len > 0 && _x.path[len - 1] == PATHSEP_CHAR ) {
            _x.path[len - 1] = '\0';
        }
    }
    f = _x.path;
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
    FullName    s;

    if( path == NULL ) {
        return( false );
    }
    _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    if( _x.dir[0] == PATHSEP_CHAR || _x.dir[0] == '.' ) {
        return( false );
    }
    _splitpath( path, s.drive, s.dir, NULL, NULL );
    if( s.dir[0] == '\0' && s.drive[0] == '\0' ) {
        return( false );
    }
    if( _x.drive[0] != '\0' && _x.drive[0] != s.drive[0] ) {
        return( false );
    }
    strcat( s.dir, _x.dir );
    makepath( _x.path, s.drive, s.dir, _x.fname, _x.ext );
    *this = _x.path;
    return( true );
}

#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 657 9
#endif

WEXPORT WFileName::~WFileName()
{
}
