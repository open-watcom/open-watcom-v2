/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#if 0
#include "pathgrp2.h"

#include "clibext.h"
#endif


//#define DEBUGOUT( x ) LBPrintf( ListBox, x );
#define DEBUGOUT( x )

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
    return( DIGCli( Open )( path, DIG_READ ) );
}
#endif

/*##########################################################################
  #
  # CLIENT routines for the DIG
  #
  ##########################################################################*/

/*
 * DIGCliAlloc
 */
void *DIGCLIENTRY( Alloc )( size_t size )
{
    void        *ret;

    DEBUGOUT( "alloc BEGIN" );
    ret = MemAlloc( size );
    DEBUGOUT( "alloc END" );
    return( ret );
}

/*
 * DIGCliRealloc
 */
void *DIGCLIENTRY( Realloc )( void *ptr, size_t size )
{
    void        *ret;

    DEBUGOUT( "realloc BEGIN" );
    ret = MemRealloc( ptr, size );
    DEBUGOUT( "realloc END" );
    return( ret );
}

/*
 * DIGCliFree
 */
void DIGCLIENTRY( Free )( void *ptr )
{
    DEBUGOUT( "free BEGIN" );
    MemFree( ptr );
    DEBUGOUT( "free END" );
}

/*
 * DIGCliOpen
 */
FILE * DIGCLIENTRY( Open )( const char *path, dig_open mode )
{
    const char  *access;

    if( mode & DIG_APPEND ) {
        access = "ab";
    } else if( mode & (DIG_WRITE | DIG_CREATE) ) {
        access = "wb";
    } else {
        access = "rb";
    }
    return( fopen( path, access ) );
}

/*
 * DIGCliSeek
 */
int DIGCLIENTRY( Seek )( FILE *fp, unsigned long offset, dig_seek dipmode )
{
    int         mode;
    int         ret;

    DEBUGOUT( "seek BEGIN" );
    switch( dipmode ) {
    case DIG_ORG:
        mode = SEEK_SET;
        break;
    case DIG_CUR:
        mode = SEEK_CUR;
        break;
    case DIG_END:
        mode = SEEK_END;
        break;
    }
    ret = fseek( fp, offset, mode );
    DEBUGOUT( "seek END" );
    return( ret );
}

/*
 * DIGCliTell
 */
unsigned long DIGCLIENTRY( Tell )( FILE *fp )
{
    unsigned long   ret;

    DEBUGOUT( "tell BEGIN" );
    ret = ftell( fp );
    DEBUGOUT( "tell END" );
    return( ret );
}

/*
 * DIGCliRead
 */
size_t DIGCLIENTRY( Read )( FILE *fp, void *buf, size_t size )
{
    DEBUGOUT( "reading" );
    return( fread( buf, 1, size, fp ) );
}

/*
 * DIGCliWrite
 */
size_t DIGCLIENTRY( Write )( FILE *fp, const void *buf, size_t size )
{
    return( fwrite( buf, 1, size, fp ) );
}

/*
 * DIGCliClose
 */
void DIGCLIENTRY( Close )( FILE *fp )
{
    fclose( fp );
}

/*
 * DIGCliRemove
 */
void DIGCLIENTRY( Remove )( const char *path, dig_open mode )
{
    mode = mode;
    remove( path );
}


/*
 * DIGCliMachineData
 */
unsigned DIGCLIENTRY( MachineData )( address addr, dig_info_type info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
{
    addr = addr;
    info_type = info_type;
    in_size = in_size;
    in = in;
    out_size = out_size;
    out = out;
    return( 0 );
}
