/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "drwatcom.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dos.h>
#include <io.h>
#include "sopen.h"
#include "digld.h"
#include "dip.h"
#include "dipimp.h"
#include "pathgrp2.h"

#include "clibext.h"


//#define DEBUGOUT( x ) LBPrintf( ListBox, x );
#define DEBUGOUT( x )

#define QQSTR(x)    # x
#define QSTR(x)     QQSTR(x)

#if 0
FILE *PathOpen( char *name, unsigned len, const char *ext )
{
    pgroup2     pg;
    char        path[ _MAX_PATH ];
    char        *realname;
    char        *filename;

    /* unused parameters */ (void)len;

    if( ext == NULL || *ext == '\0' ) {
        realname = name;
    } else {
        realname = MemAlloc( _MAX_PATH );
        filename = MemAlloc( _MAX_FNAME );
        _splitpath2( name, pg.buffer, NULL, NULL, &pg.fname, NULL );
        _makepath( realname, NULL, NULL, pg.fname, ext );
        MemFree( realname );
        MemFree( filename );
    }
    _searchenv( realname, "PATH", path );
    if( *path == '\0' )
        return( NULL );
    return( DIGCli( Open )( path, DIG_OPEN_READ ) );
}
#endif

/*##########################################################################
  #
  # CLIENT routines for the DIG
  #
  ##########################################################################*/

void *DIGCLIENTRY( Alloc )( size_t size )
/****************************************
 * DIGCliAlloc
 */
{
    void        *ret;

    DEBUGOUT( "alloc BEGIN" );
    ret = MemAlloc( size );
    DEBUGOUT( "alloc END" );
    return( ret );
}

void *DIGCLIENTRY( Realloc )( void *ptr, size_t size )
/*****************************************************
 * DIGCliRealloc
 */
{
    void        *ret;

    DEBUGOUT( "realloc BEGIN" );
    ret = MemRealloc( ptr, size );
    DEBUGOUT( "realloc END" );
    return( ret );
}

void DIGCLIENTRY( Free )( void *ptr )
/************************************
 * DIGCliFree
 */
{
    DEBUGOUT( "free BEGIN" );
    MemFree( ptr );
    DEBUGOUT( "free END" );
}

FILE * DIGCLIENTRY( Open )( const char *path, dig_open mode )
/************************************************************
 * DIGCliOpen
 */
{
    const char  *access;

    if( mode & DIG_OPEN_APPEND ) {
        access = "ab";
    } else if( mode & (DIG_OPEN_WRITE | DIG_OPEN_CREATE) ) {
        access = "wb";
    } else {
        access = "rb";
    }
    return( fopen( path, access ) );
}

int DIGCLIENTRY( Seek )( FILE *fp, unsigned long offset, dig_seek where )
/************************************************************************
 * DIGCliSeek
 */
{
    int         mode;
    int         ret;

    DEBUGOUT( "seek BEGIN" );
    switch( where ) {
    case DIG_SEEK_ORG:
        mode = SEEK_SET;
        break;
    case DIG_SEEK_CUR:
        mode = SEEK_CUR;
        break;
    case DIG_SEEK_END:
        mode = SEEK_END;
        break;
    }
    ret = fseek( fp, offset, mode );
    DEBUGOUT( "seek END" );
    return( ret );
}

unsigned long DIGCLIENTRY( Tell )( FILE *fp )
/********************************************
 * DIGCliTell
 */
{
    unsigned long   ret;

    DEBUGOUT( "tell BEGIN" );
    ret = ftell( fp );
    DEBUGOUT( "tell END" );
    return( ret );
}

size_t DIGCLIENTRY( Read )( FILE *fp, void *buf, size_t size )
/*************************************************************
 * DIGCliRead
 */
{
    DEBUGOUT( "reading" );
    return( fread( buf, 1, size, fp ) );
}

size_t DIGCLIENTRY( Write )( FILE *fp, const void *buf, size_t size )
/********************************************************************
 * DIGCliWrite
 */
{
    return( fwrite( buf, 1, size, fp ) );
}

void DIGCLIENTRY( Close )( FILE *fp )
/************************************
 * DIGCliClose
 */
{
    fclose( fp );
}

void DIGCLIENTRY( Remove )( const char *path, dig_open mode )
/************************************************************
 * DIGCliRemove
 */
{
    /* unused parameters */ (void)mode;

    remove( path );
}

size_t DIGLoader( Find )( dig_filetype ftype, const char *base_name, size_t base_name_len,
                                const char *defext, char *filename, size_t filename_maxlen )
/*******************************************************************************************
 * DIGLoaderFind
 */
{
    char        fname[256];
    size_t      len;
    bool        found;
    char        *p;
#ifdef BLDVER
    char        buffer[_MAX_PATH];
#endif

    /* unused parameters */ (void)ftype;

    if( base_name_len == 0 )
        base_name_len = strlen( base_name );
    strncpy( fname, base_name, base_name_len );
    strcpy( fname + base_name_len, defext );
#ifdef BLDVER
    _searchenv( fname, "WD_PATH" QSTR( BLDVER ), buffer );
    if( *buffer != '\0' ) {
        found = true;
        p = buffer;
    } else {
        found = false;
        p = fname;
    }
#else
    found = true;
    p = fname;
#endif
    len = strlen( p );
    if( filename_maxlen > 0 ) {
        filename_maxlen--;
        if( filename_maxlen > len )
            filename_maxlen = len;
        if( filename_maxlen > 0 )
            strncpy( filename, p, filename_maxlen );
        filename[filename_maxlen] = '\0';
    }
    return( ( found ) ? len : 0 );
}
