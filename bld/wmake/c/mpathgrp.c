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


#include <stdlib.h>
#include <string.h>

#include "make.h"
#include "massert.h"
#include "memory.h"
#include "mpathgrp.h"


extern void DropPGroup( PGROUP *pg )
/**********************************/
{
    FreeSafe( pg );
}

static void truncatePart( char *part, size_t max )
{
    size_t len;

    len = strlen( part );
    if( len > max ) {
        part[ max ] = '\0';
    }
}


extern PGROUP *SplitPath( const char *path )
/******************************************/
{
    PGROUP      *pg;
    size_t      len;

    assert( path != NULL );

    pg = MallocSafe( sizeof( PGROUP ) );
    _splitpath2( path, pg->buffer, &pg->drive, &pg->dir, &pg->fname, &pg->ext );
    truncatePart( pg->drive, _MAX_DRIVE - 1 );
    truncatePart( pg->dir, _MAX_DIR - 1 );
    truncatePart( pg->fname, _MAX_FNAME - 1 );
    truncatePart( pg->ext, _MAX_EXT - 1 );
    /*
        I don't really want to make the assumption that _splitpath2 will
        fill in the buffer like so:

            drive \0 dir \0 fname \0 ext \0

        but it would be incredibly slow to assume otherwise.

    */
#if 1
    len = ( pg->ext + strlen( pg->ext ) + 1 ) - pg->buffer;
#else
    p = pg->ext;
    if( p < pg->drive ) {
        p = pg->drive;
    }
    if( p < pg->dir ) {
        p = pg->dir;
    }
    if( p < pg->fname ) {
        p = pg->fname;
    }
    len = ( p + strlen( p ) + 1 ) - pg->buffer;
#endif
    len += sizeof( PGROUP ) - _MAX_PATH2;
    MemDecreaseSize( pg, len );
    return( pg );
}
