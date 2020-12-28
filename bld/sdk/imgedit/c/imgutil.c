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


#include "imgedit.h"
#include "pathgrp2.h"

#include "clibext.h"


const char *GetImageFileExt( image_type img_type, bool res )
{
    const char  *ext;

    if( img_type == BITMAP_IMG ) {
        ext = "bmp";
    } else if( img_type == ICON_IMG ) {
        ext = "ico";
    } else if( img_type == CURSOR_IMG ) {
#ifdef __OS2__
        ext = "ptr";
#else
        ext = "cur";
#endif
    } else if( res && img_type == RESOURCE_IMG ) {
        ext = "res";
    } else if( res && img_type == EXE_IMG ) {
        ext = "exe";
    } else if( res && img_type == DLL_IMG ) {
        ext = "dll";
    } else {
        ext = NULL;
    }
    return( ext );
}

image_type GetImageFileType( const char *ext, bool res )
{
    image_type  img_type;

    if( CMPFEXT( ext, "bmp" ) ) {
        img_type = BITMAP_IMG;
    } else if( CMPFEXT( ext, "ico" ) ) {
        img_type = ICON_IMG;
#ifdef __OS2__
    } else if( CMPFEXT( ext, "ptr" ) ) {
#else
    } else if( CMPFEXT( ext, "cur" ) ) {
#endif
        img_type = CURSOR_IMG;
    } else if( res && CMPFEXT( ext, "res" ) ) {
        img_type = RESOURCE_IMG;
    } else if( res && CMPFEXT( ext, "exe" ) ) {
        img_type = RESOURCE_IMG;
    } else if( res && CMPFEXT( ext, "dll" ) ) {
        img_type = RESOURCE_IMG;
    } else {
        img_type = UNDEF_IMG;
    }
    return( img_type );
}

/*
 * CheckForExt - if no extension is given, use the default for the given type
 */
void CheckForExt( img_node *node )
{
    pgroup2     pg;
    img_node    *next_icon;

    for( next_icon = node; next_icon != NULL; next_icon = next_icon->nexticon ) {
        _splitpath2( next_icon->fname, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        if( pg.ext[0] != '\0' ) {
            return;
        }
        _makepath( next_icon->fname, pg.drive, pg.dir, pg.fname, GetImageFileExt( next_icon->imgtype, false ) );
    }

} /* CheckForExt */

/*
 * CheckForPalExt - if no extension is given, use the default palette
 *                  extension of .pal.
 */
void CheckForPalExt( char *filename )
{
    pgroup2     pg;

    _splitpath2( filename, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] == '\0' ) {
        _makepath( filename, pg.drive, pg.dir, pg.fname, "pal" );
    }

} /* CheckForPalExt */
