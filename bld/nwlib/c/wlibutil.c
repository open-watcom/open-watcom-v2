/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
#include <errno.h>
#include "wio.h"
#include "pathgrp2.h"
#include "roundmac.h"

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

    if( arch->size == 0 ) {
        *contents = NULL;
        return;
    }
    size = __ROUND_UP_SIZE_EVEN( arch->size );
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
    arch->libtype = WL_LTYPE_NONE;
    arch->fnametab = NULL;
    arch->ffnametab = NULL;
}

static void CopyBytes( char *buffer, libfile src, libfile dst, size_t len )
{
    if( LibRead( src, buffer, len ) == len ) {
        LibWrite( dst, buffer, len );
    } else {
        LibReadError( src );
    }
}

void Copy( libfile src, libfile dst, file_offset size )
{
    char        buffer[4096];


    while( size > sizeof( buffer ) ) {
        CopyBytes( buffer, src, dst, sizeof( buffer ) );
        size -= sizeof( buffer );
    }
    if( size != 0 ) {
        CopyBytes( buffer, src, dst, size );
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

bool IsSameModuleCase( const char *a, const char *b, int cmp_mode )
/*
 * for COFF/ELF objects the real file name is hold in library
 *   the comparision is transparent for file name with extension
 *
 * for OMF objects comparision is not transparen because in object
 *   file is available source file name or module name (THEADR record)
 *   the comparision is possible only for source file base name or
 *   module name
 *   for OMF files is the object file name is derived from source
 *   file name that only comparision of file base name is possible
 *   we use following logic:
 *   - remove first level of extension and compare base name
 *   - remove second level of extension and compare base name again
 */
{
    _splitpath2( a, pg1.buffer, NULL, NULL, &pg1.fname, &pg1.ext );
    _splitpath2( b, pg2.buffer, NULL, NULL, &pg2.fname, &pg2.ext );
    if( cmp_mode == 1 ) {
        /*
         * OMF format
         * compare only base name and try to remove second level of extension
         */
        if( FNCMP( pg1.fname, pg2.fname ) ) {
            /*
             * remove second level of extension and compare base name again
             */
            strcpy( path, pg1.fname );
            _splitpath2( path, pg1.buffer, NULL, NULL, &pg1.fname, NULL );
            strcpy( path, pg2.fname );
            _splitpath2( pg2.fname, pg2.buffer, NULL, NULL, &pg2.fname, NULL );
            if( FNCMP( pg1.fname, pg2.fname ) ) {
                return( false );
            }
        }
    } else {
        /*
         * AR formats
         * compare base name and extension
         */
        if( FNCMP( pg1.fname, pg2.fname ) ) {
            return( false );
        }
        if( FNCMP( pg1.ext, pg2.ext ) ) {
            return( false );
        }
    }
    return( true );
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
    char name[20];
    int  initial;
    int  count;

    _splitpath2( Options.input_name, pg1.buffer, &pg1.drive, &pg1.dir, NULL, NULL );

    /*
     * For whatever it's worth, we'll only check 999 files before
     * quitting ;-)
     */
    initial = time( NULL ) % 1000;
    for( count = ( initial + 1 ) % 1000; count != initial; count = ( count + 1 ) % 1000 ) {
        sprintf( name, "_wlib%03d", count );
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
