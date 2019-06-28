/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


/* Structure now declared in wresrtns.c */
/* This file should be replace by a call to the WResSetRtns macro in projects */
/* that define their own low level routines. */
/* by default it uses ISO C file I/O functions */
/* this example uses POSIX file I/O functions */

#if 0

#include <stdlib.h>
#include "wio.h"
#include "posixfp.h"
#include "layer0.h"


static FILE *wres_open( const char *name, int omode )
{
    switch( omode ) {
    default:
    case WRES_OPEN_RO:
        return( POSIX2FP( open( name, O_BINARY | O_RDONLY ) ) );
    case WRES_OPEN_RW:
        return( POSIX2FP( open( name, O_BINARY | O_RDWR | O_CREAT, PMODE_RW ) ) );
    case WRES_OPEN_NEW:
        return( POSIX2FP( open( name, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, PMODE_RW ) ) );
    }
}

static int wres_close( FILE *fp )
{
    return( close( FP2POSIX( fp ) ) );
}

static WResFileSSize wres_read( FILE *fp, void *buf, WResFileSize size )
{
    return( posix_read( FP2POSIX( fp ), buf, size ) );
}

static WResFileSSize wres_write( FILE *fp, const void *buf, WResFileSize size )
{
    return( posix_write( FP2POSIX( fp ), buf, size ) );
}

static int wres_seek( FILE *fp, long pos, int where )
{
    return( lseek( FP2POSIX( fp ), pos, where ) == -1L );
}

static long wres_tell( FILE *fp )
{
    return( tell( FP2POSIX( fp ) ) );
}

static bool wres_ioerr( FILE *fp, size_t rc )
{
    /* unused parameters */ (void)fp;

    return( rc == (size_t)-1 );
}

WResSetRtns( wres_open, wres_close, wres_read, wres_write, wres_seek, wres_tell, wres_ioerr, wres_alloc, wres_free );

#endif
