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
* Description:  Help file I/O functions (used by online help viewer).
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgio.h"
#include "dbgerr.h"
#include "helpio.h"
#include "posixfp.h"


static const seek_method    stream_seek_method[] = { DIO_SEEK_ORG, DIO_SEEK_CUR, DIO_SEEK_END };

HELPIO long HelpFileLen( FILE *fp )
{
    unsigned long   old;
    long            len;

    old = SeekStream( FP2POSIX( fp ), 0, DIO_SEEK_CUR );
    len = SeekStream( FP2POSIX( fp ), 0, DIO_SEEK_END );
    SeekStream( FP2POSIX( fp ), old, DIO_SEEK_ORG );
    return( len );
}

HELPIO size_t HelpRead( FILE *fp, void *buf, size_t len )
{
    return( ReadStream( FP2POSIX( fp ), buf, len ) );
}

HELPIO long HelpSeek( FILE *fp, long offset, HelpSeekType where ) {

    return( SeekStream( FP2POSIX( fp ), offset, stream_seek_method[where] ) );
}

HELPIO long HelpTell( FILE *fp )
{
    return( SeekStream( FP2POSIX( fp ), 0, DIO_SEEK_CUR ) );
}

HELPIO FILE *HelpOpen( const char *path )
{
    return( POSIX2FP( FileOpen( path, OP_READ ) ) );
}

HELPIO int HelpClose( FILE *fp )
{
    FileClose( FP2POSIX( fp ) );
    return( 0 );
}

HELPIO int HelpFileAccess( const char *path )
{
    file_handle fh;
    int         rc;

    fh = FileOpen( path, OP_READ );
    rc = -1;
    if( fh != NIL_HANDLE ) {
        rc = 0;
        FileClose( fh );
    }
    return( rc );
}

HELPIO char *HelpGetCWD( char *buf, int size )
{
    /* unused parameters */ (void)size;

    buf[0] = NULLCHAR;
    return( buf );
}

HELPIO void HelpSearchEnv( const char *name, const char *env_var, char *buf )
{
    /* unused parameters */ (void)name; (void)env_var;

    buf[0] = NULLCHAR;
}
