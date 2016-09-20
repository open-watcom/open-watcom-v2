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
* Description:  Miscellaneous librarian utility functions.
*
****************************************************************************/


#include "wlib.h"
#include "wio.h"

#include "clibext.h"


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
    file_offset     size;
    file_offset     bytes_read;

    size = arch->size;
    if( size == 0 ) {
        *contents = NULL;
        return;
    }

    if( size % 2 == 1 ) {
        size++;
    }
    *contents = MemAlloc( size );
    bytes_read = LibRead( io, *contents, size );
    if( bytes_read != size ) {
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
static void CopyBytes( char *buffer, libfile source, libfile dest, file_offset len )
{
    file_offset bytes;

    bytes = LibRead( source, buffer, len );
    if( bytes == len ) {
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

static char     drive[_MAX_DRIVE];
static char     dir[_MAX_DIR];
static char     fname[_MAX_FNAME];
static char     fext[_MAX_EXT];

bool SameFile( const char *a, const char *b )
{
    char fulla[_MAX_PATH];

    _fullpath( fulla, a, sizeof( fulla ) );
    _fullpath( path, b, sizeof( path ) );
    return( FNCMP( fulla, path ) == 0 );
}

bool SameName( const char *a, const char *b )
{
    _splitpath( a, NULL, NULL, path, fext );
    _splitpath( b, NULL, NULL, fname, fext );
    return( FNCMP( path, fname ) == 0 );
}

char *MakeFName( const char *a )
{
    _splitpath( a, NULL, NULL, fname, fext );
    return( fname );
}

bool IsExt( const char *a, const char *b )
{
    _splitpath( a, NULL, NULL, NULL, fext );
    return( FNCMP( fext, b ) == 0 );
}

void DefaultExtension( char *name, const char *def_ext )
{
    _splitpath( name, drive, dir, fname, fext );
    if( fext[0] == '\0' ) {
        _makepath( name, drive, dir, fname, def_ext );
    }
}

char *MakeObjOutputName( const char *src, const char *new )
{
    if( new != NULL ) {
        _splitpath( new, NULL, NULL, fname, fext );
        if( *fname == 0 )
            _splitpath( src, NULL, NULL, fname, NULL );
        _makepath( path, NULL, Options.output_directory, fname, fext );
    } else {
        _splitpath( src, NULL, NULL, fname, fext );
        _makepath( path, NULL, Options.output_directory, fname, EXT_OBJ );
    }
    return( path );
}

char *MakeListName( void )
{
    _splitpath( Options.input_name, NULL, NULL, fname, fext );
    _makepath( path, NULL, NULL, fname, EXT_LST );
    return( path );
}

char *MakeBakName( void )
{
    _splitpath( Options.input_name, drive, dir, fname, fext );
    _makepath( path, drive, dir, fname, EXT_BAK );
    return( path );
}

char *MakeTmpName( char *buffer )
{
    char name[9];
    long initial;
    long count;

    _splitpath( Options.input_name, drive, dir, fname, fext );

    /*
     * For whatever it's worth, we'll only check 9999 files before
     * quitting ;-)
     */
    initial = time( NULL ) % 1000L;
    for( count = ( initial + 1L ) % 1000L; count != initial; count = ( count + 1L ) % 1000L ) {
        sprintf( name, "_wlib%03ld", count );
        _makepath( buffer, drive, dir, name, "$$$" );

        if( access( buffer, 0 ) != 0 ) {
            break;
        }
    }

    if( count == initial ) {
        FatalError( ERR_CANT_WRITE, "temporary file", strerror( errno ) );
    }

    return( buffer );
}

char *TrimPath( char *name )
{
    _splitpath( name, NULL, NULL, fname, fext );
    _makepath( name, NULL, NULL, fname, fext );
    return( name );
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
