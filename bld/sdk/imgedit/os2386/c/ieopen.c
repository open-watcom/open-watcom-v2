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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#define INCL_WINSTDFILE
#include "imgedit.h"
#include "..\h\wbitmap.h"
#include "..\h\pmicon.h"
#include "pathgrp2.h"


static image_type       imgType = BITMAP_IMG;
static char             initialDir[_MAX_PATH + _MAX_DIR];

void SetupMenuAfterOpen( void )
{
    HMENU               hmenu;

    hmenu = GetMenu( _wpi_getframe(HMainWindow) );
    _wpi_enablemenuitem( hmenu, IMGED_CLEAR, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_REST, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_SAVE, MF_GRAYED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_SAVE_AS, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_LINE, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_RECTO, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_RECTF, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_CIRCLEO, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_CIRCLEF, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_FREEHAND, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_FILL, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_BRUSH, MF_ENABLED, FALSE );
    _wpi_enablemenuitem( hmenu, IMGED_CLIP, MF_ENABLED, FALSE );

} /* SetupMenuAfterOpen */

/*
 * readInImageFile - reads in an icon or cursor file (bitmaps too??!!)
 */
static bool readInImageFile( const char *fullname )
{
    FILE                *fp;
    a_pm_image_file     *imgfile;
    int                 retcode;
    img_node            *node;
    a_pm_image          *image;
    char                filename[_MAX_FNAME + _MAX_EXT];
    char                text[HINT_TEXT_LEN];
    int                 file_type;
    int                 i;

    fp = fopen( fullname, "rb" );
    if( fp == NULL )
        return( false );

    if( imgType == BITMAP_IMG ) {
        file_type = PMBITMAP_FILETYPE;
    } else if( imgType == ICON_IMG ) {
        file_type = PMICON_FILETYPE;
    } else {
        file_type = PMPOINTER_FILETYPE;
    }

    imgfile = OpenPMImage( fp, file_type, &retcode );
    if( imgfile == NULL ) {
        fclose( fp );
        return( false );
    }
    if( retcode != file_type ) {
        if( imgType == ICON_IMG ) {
            WImgEditError( WIE_ERR_BAD_ICON_FILE, filename );
        } else {
            WImgEditError( WIE_ERR_BAD_CURSOR_FILE, filename );
        }
        ClosePMImage( imgfile );
        fclose( fp );
        return( false );
    }
    node = malloc( sizeof(img_node) * imgfile->count );

    for( i = 0; i < imgfile->count; ++i ) {
        node[i].imgtype = imgType;
        node[i].bitcount = imgfile->resources[i].xorinfo->cBitCount;
        node[i].width = imgfile->resources[i].xorinfo->cx;
        node[i].height = imgfile->resources[i].xorinfo->cy;
        node[i].hotspot.x = imgfile->resources[i].xhotspot;
        node[i].hotspot.y = imgfile->resources[i].yhotspot;

        image = GetPMImageBits( imgfile, fp, i );
        node[i].and_hbitmap = PMImageToAndBitmap( image, imgfile, i );
        node[i].xor_hbitmap = PMImageToWinXorBitmap(image, imgfile, i, Instance);
        node[i].num_of_images = imgfile->count;
        node[i].viewhwnd = NULL;
        if( i > 0 ) {
            node[i-1].nexticon = &(node[i]);
        }
        node[i].issaved = true;
        node[i].next = NULL;
        strupr( strcpy( node[i].fname, fullname ) );
        FiniPMImage( image );
    }
    node[i-1].nexticon = NULL;

    ClosePMImage( imgfile );
    fclose( fp );

    GetFnameFromPath( fullname, filename );
    if( imgType == ICON_IMG ) {
        sprintf( text, "Opened '%s' (%d icons)", filename, node->num_of_images );
    } else if( imgType == CURSOR_IMG ) {
        sprintf( text, "Opened '%s' (%d pointers)", filename, node->num_of_images );
    }
    SetHintText( text );
    CreateNewDrawPad( node );

    free( node );
    return( true );

} /* readInImageFile */

/*
 * readInBitmapFile - reads in a bitmap file
 */
static bool readInBitmapFile( const char *fullname )
{
    HBITMAP             real_hbitmap;
    WPI_HBITMAP         hbitmap;
    WPI_HBITMAP         old_hbitmap1;
    WPI_HBITMAP         old_hbitmap2;
    BITMAPINFOHEADER2   info;
    img_node            node;
    WPI_PRES            pres;
    WPI_PRES            srcpres;
    HDC                 srcdc;
    WPI_PRES            dstpres;
    HDC                 dstdc;
    char                text[HINT_TEXT_LEN];
    char                filename[_MAX_FNAME + _MAX_EXT];

    // NOTE that ReadPMBitmapFile returns an actual hbitmap!!
    real_hbitmap = ReadPMBitmapFile( HMainWindow, fullname, &info );
    hbitmap = _wpi_makewpibitmap( real_hbitmap );
    GetFnameFromPath( fullname, filename );

    if( hbitmap ) {
        if( ( info.cx > MAX_DIM ) || ( info.cy > MAX_DIM ) ) {
            WImgEditError( WIE_ERR_BITMAP_TOO_BIG, filename );
            _wpi_deletebitmap( hbitmap );
            return( false );
#if 1
        } else if( info.cBitCount > 4 ) {
            WImgEditError( WIE_ERR_256CLR_BITMAP, filename );
            _wpi_deletebitmap( hbitmap );
            return( false );
#endif
        }
        node.imgtype = BITMAP_IMG;
        node.width = info.cx;
        node.height = info.cy;
        node.bitcount = info.cBitCount;
        node.hotspot.x = 0;
        node.hotspot.y = 0;
        node.num_of_images = 1;
        node.nexticon = NULL;
        node.issaved = true;
        if( node.bitcount == 1 ) {
            pres = _wpi_getpres( HWND_DESKTOP );
            srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
            dstpres = _wpi_createcompatiblepres( pres, Instance, &dstdc );
            _wpi_releasepres( HWND_DESKTOP, pres );

            _wpi_torgbmode( srcpres );
            _wpi_torgbmode( dstpres );

            node.xor_hbitmap = _wpi_createbitmap( node.width, node.height, 1, 1, NULL );
            _wpi_preparemono( dstpres, BLACK, WHITE );

            old_hbitmap1 = _wpi_selectbitmap( srcpres, hbitmap );
            old_hbitmap2 = _wpi_selectbitmap( dstpres, node.xor_hbitmap );
            _wpi_bitblt( dstpres, 0, 0, node.width, node.height, srcpres,
                                        0, 0, SRCCOPY );
            _wpi_selectbitmap( srcpres, old_hbitmap1 );
            _wpi_selectbitmap( dstpres, old_hbitmap2 );
            _wpi_deletecompatiblepres( srcpres, srcdc );
            _wpi_deletecompatiblepres( dstpres, dstdc );
            _wpi_deletebitmap( hbitmap );
        } else {
            node.xor_hbitmap = hbitmap;
        }

        strcpy( node.fname, fullname );
        sprintf( text, "Opened '%s'.", filename );
        SetHintText( text );

        MakeBitmap( &node, false );
        CreateNewDrawPad( &node );
    } else {
        WImgEditError( WIE_ERR_BAD_BITMAP_FILE, filename );
        return( false );
    }
    return( true );

} /* readInBitmapFile */

/*
 * initOpenFileInfo
 */
static bool initOpenFileInfo( const char *fullfile, image_type img_type )
{
    char        *fname;
    size_t      len;

    fname = "*";
    if( initialDir[0] != '\0' ) {
        len = strlen( initialDir ) - 1;
        if( initialDir[len] != ':' && initialDir[len] != '\\' ) {
            fname = "\\*";
        }
    }
    _makepath( fullfile, NULL, initialDir, fname, GetImageFileExt( img_type, false ) );
    return( true );

} /* initOpenFileInfo */

/*
 * updateOpenFileInfo
 */
static bool updateOpenFileInfo( const char *fname )
{
    pgroup2             pg;
    size_t              len;

    _splitpath2( fname, pg.buffer, &pg.drive, &pg.dir, NULL, &pg.ext );
    if( pg.dir[0] != '\0' ) {
        len = strlen( pg.dir ) - 1;
        if( len > 0 && pg.dir[len] == '\\' ) {
            pg.dir[len] = '\0';
        }
    }
    _makepath( initialDir, pg.drive, pg.dir, NULL, NULL );
    imgType = GetImageFileType( pg.ext, false );
    return( imgType != UNDEF_IMG );

} /* updateOpenFileInfo */

/*
 * getOpenFName - let the user select a file name for an open operation
 *                fname must point to a buffer of length at least _MAX_PATH
 *                also sets the type of file (bitmap, icon, cursor).
 */
static bool getOpenFName( char *fname )
{
    FILEDLG             filedlg;
    bool                ok;
    char                fullfile[CCHMAXPATH];

    fname[0] = '\0';

    initOpenFileInfo( fullfile, imgType );

    /*
     * set the values of the filedlg structure ...
     */
    memset( &filedlg, 0, sizeof( FILEDLG ) );
    filedlg.cbSize = sizeof( FILEDLG );
    filedlg.fl = FDS_OPEN_DIALOG | FDS_CENTER;
    filedlg.pszTitle = "Open Image File";
    filedlg.pszOKButton = "Open";
    strcpy( filedlg.szFullFile, fullfile );

    ok = ( WinFileDlg( HWND_DESKTOP, HMainWindow, &filedlg ) != NULLHANDLE && filedlg.lReturn == DID_OK );

    if( ok ) {
        strcpy( fname, filedlg.szFullFile );
        ok = updateOpenFileInfo( fname );
    }
    return( ok );

} /* getOpenFName */

/*
 * OpenImage - Get the filename of the file to open.  Depending on the
 *              extension set the type (.ico, .bmp, .ptr) and call the
 *              appropriate function to open it.
 */
bool OpenImage( void )
{
    char                fname[_MAX_PATH];
    char                filename[_MAX_FNAME];
    char                error_text[HINT_TEXT_LEN];

    if( !getOpenFName( &fname ) ) {
        SetHintText( "File not opened" );
        return( false );
    }

    switch( imgType ) {
    case BITMAP_IMG:
        if( readInBitmapFile( fname ) ) {
            SetupMenuAfterOpen();
            return( true );
        }
        GetFnameFromPath( fname, filename );
        sprintf( error_text, "Error opening '%s'", filename );
        SetHintText( error_text );
        break;
    case ICON_IMG:
    case CURSOR_IMG:
        if( readInImageFile( fname ) ) {
            SetupMenuAfterOpen();
            return( true );
        }
        GetFnameFromPath( fname, filename );
        sprintf( error_text, "Error opening '%s'", filename );
        SetHintText( error_text );
        break;
    }
    return( false );

} /* OpenImage */

/*
 * SetInitialOpenDir - sets the initial directory for the open filename
 */
void SetInitialOpenDir( char *new_dir )
{
    if( new_dir != NULL ) {
        strcpy( initialDir, new_dir );
    } else {
        strcpy( initialDir, "" );
    }
} /* SetInitialOpenDir */

/*
 * GetInitOpenDir - gets the directory which we want to use as our initial one
 *              next time we run.
 */
char *GetInitOpenDir( void )
{
    return( initialDir );

} /* GetInitOpenDir */

/*
 * OpenFileOnStart - opens a file on program startup
 */
void OpenFileOnStart( const char *fname )
{
    pgroup2     pg;
    FILE        *fp;
    char        text[HINT_TEXT_LEN];
    char        filename[_MAX_FNAME + _MAX_EXT];
    image_type  img_type;

    fp = fopen( fname, "r" );
    if( fp == NULL ) {
        GetFnameFromPath( fname, filename );
        sprintf( text, "'%s' could not be found!", filename );
        SetHintText( text );
        return;
    }
    fclose( fp );

    _splitpath2( fname, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    img_type = GetImageFileType( pg.ext, false );
    if( img_type == BITMAP_IMG ) {
        if( readInBitmapFile( fname ) ) {
            SetupMenuAfterOpen();
        }
    } else if( img_type == ICON_IMG ) {
        if( readInImageFile( fname ) ) {
            SetupMenuAfterOpen();
        }
    } else if( img_type == CURSOR_IMG ) {
        if( readInImageFile( fname ) ) {
            SetupMenuAfterOpen();
        }
    }

} /* OpenFileOnStart */
