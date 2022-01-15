/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Miscellaneous librarian utility functions.
*
****************************************************************************/


#include "wlib.h"
#include "wio.h"
#include "pathgrp2.h"

#include "clibext.h"


#ifdef __UNIX__
#define FNCMP strcmp
#else
#define FNCMP stricmp
#endif

int SymbolNameCmp( const char *s1, const char *s2)
{
    if( Options.respect_case ) {
        return( strcmp( s1, s2 ) );
    } else {
        return( stricmp( s1, s2 ) );
    }
}


void GetFileContents( const char *name, libfile io, arch_header *arch, char **contents )
{
    size_t  size;

    size = arch->size;
    if( size == 0 ) {
        *contents = NULL;
        return;
    }
    Round2var( size );
    *contents = MemAlloc( size );
    if( LibRead( io, *contents, size ) != size ) {
        BadLibrary( name );
    }
}

void NewArchHeader( arch_header *arch, char *name )
{
    struct stat         buf;

    if( stat( name, &buf ) == -1 ) {
        FatalError( ERR_CANT_FIND, name );
    }
    arch->name = name;
    arch->ffname = NULL;
    arch->date = buf.st_mtime;
    arch->uid = buf.st_uid;
    arch->gid = buf.st_gid;
    arch->mode = buf.st_mode;
    arch->size = buf.st_size;
}

static void CopyBytes( char *buffer, libfile source, libfile dest, size_t len )
{
    if( LibRead( source, buffer, len ) == len ) {
        LibWrite( dest, buffer, len );
    } else {
        LibReadError( source );
    }
}

void Copy( libfile source, libfile dest, file_offset size )
{
    char        buffer[4096];


    while( size > sizeof( buffer ) ) {
        CopyBytes( buffer, source, dest, sizeof( buffer ) );
        size -= sizeof( buffer );
    }
    if( size != 0 ) {
        CopyBytes( buffer, source, dest, size );
    }
}

static char     path[_MAX_PATH];

static pgroup2  pg1;
static pgroup2  pg2;

bool IsSameFile( const char *a, const char *b )
{
    _fullpath( pg1.buffer, a, sizeof( pg1.buffer ) );
    _fullpath( pg2.buffer, b, sizeof( pg2.buffer ) );
    return( FNCMP( pg1.buffer, pg2.buffer ) == 0 );
}

bool IsSameFNameCase( const char *a, const char *b )
{
    _splitpath2( a, pg1.buffer, NULL, NULL, &pg1.fname, NULL );
    _splitpath2( b, pg2.buffer, NULL, NULL, &pg2.fname, NULL );
    if( Options.respect_case ) {
        return( strcmp( pg1.fname, pg2.fname ) == 0 );
    } else {
        return( FNCMP( pg1.fname, pg2.fname ) == 0 );
    }
}

char *MakeFName( const char *a )
{
    _splitpath2( a, pg1.buffer, NULL, NULL, &pg1.fname, NULL );
    strcpy( path, pg1.fname );
    return( path );
}

bool IsExt( const char *a, const char *b )
{
    _splitpath2( a, pg1.buffer, NULL, NULL, NULL, &pg1.ext );
    return( pg1.ext[0] == '.' && FNCMP( pg1.ext + 1, b ) == 0 );
}

void DefaultExtension( char *name, const char *def_ext )
{
    _splitpath2( name, pg1.buffer, &pg1.drive, &pg1.dir, &pg1.fname, &pg1.ext );
    if( pg1.ext[0] == '\0' ) {
        _makepath( name, pg1.drive, pg1.dir, pg1.fname, def_ext );
    }
}

char *MakeObjOutputName( const char *src, const char *new )
{
    if( new != NULL ) {
        _splitpath2( new, pg1.buffer, NULL, NULL, &pg1.fname, &pg1.ext );
        if( pg1.fname[0] == '\0' ) {
            _splitpath2( src, pg2.buffer, NULL, NULL, &pg2.fname, NULL );
            pg1.fname = pg2.fname;
        }
    } else {
        _splitpath2( src, pg1.buffer, NULL, NULL, &pg1.fname, NULL );
        pg1.ext = EXT_OBJ;
    }
    _makepath( path, NULL, Options.output_directory, pg1.fname, pg1.ext );
    return( path );
}

char *MakeListName( void )
{
    _splitpath2( Options.input_name, pg1.buffer, NULL, NULL, &pg1.fname, NULL );
    _makepath( path, NULL, NULL, pg1.fname, EXT_LST );
    return( path );
}

char *MakeBakName( void )
{
    _splitpath2( Options.input_name, pg1.buffer, &pg1.drive, &pg1.dir, &pg1.fname, NULL );
    _makepath( path, pg1.drive, pg1.dir, pg1.fname, EXT_BAK );
    return( path );
}

char *TrimPath( const char *name )
{
    _splitpath2( name, pg1.buffer, NULL, NULL, &pg1.fname, &pg1.ext );
    _makepath( path, NULL, NULL, pg1.fname, pg1.ext );
    return( path );
}

void TrimPathInPlace( char *name )
{
    _splitpath2( name, pg1.buffer, NULL, NULL, &pg1.fname, &pg1.ext );
    _makepath( name, NULL, NULL, pg1.fname, pg1.ext );
}

char *MakeTmpName( char *buffer )
{
    char name[9];
    long initial;
    long count;

    _splitpath2( Options.input_name, pg1.buffer, &pg1.drive, &pg1.dir, NULL, NULL );

    /*
     * For whatever it's worth, we'll only check 9999 files before
     * quitting ;-)
     */
    initial = time( NULL ) % 1000L;
    for( count = ( initial + 1L ) % 1000L; count != initial; count = ( count + 1L ) % 1000L ) {
        sprintf( name, "_wlib%03ld", count );
        _makepath( buffer, pg1.drive, pg1.dir, name, "$$$" );

        if( access( buffer, 0 ) != 0 ) {
            break;
        }
    }

    if( count == initial ) {
        FatalError( ERR_CANT_WRITE, "temporary file", strerror( errno ) );
    }

    return( buffer );
}

char    *FormSym( const char *name )
{
    static      char    buff[128];

    if( Options.mangled ) {
        strcpy( buff, name );
    } else {
        __demangle_l( name, 0, buff, sizeof( buff ) );
    }
    return( buff );
}

char *LibFormat( void )
{
    switch( Options.libtype ) {
    case WL_LTYPE_AR:
        return( "AR" );
    case WL_LTYPE_MLIB:
        return( "MLIB" );
    case WL_LTYPE_OMF:
        return( "LIB" );
    default:
        return( "unknown format" );
    }
}
