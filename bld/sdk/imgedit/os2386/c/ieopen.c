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


#define INCL_WINSTDFILE
#include "imgedit.h"
#include "..\h\wbitmap.h"
#include "..\h\pmicon.h"

static signed short     imgType = BITMAP_IMG;
static char             initialDir[ _MAX_PATH+_MAX_DIR ];

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
static BOOL readInImageFile( char *fullname )
{
    FILE                *fp;
    a_pm_image_file     *imgfile;
    int                 retcode;
    img_node            *node;
    a_pm_image          *image;
    char                filename[ _MAX_FNAME + _MAX_EXT ];
    char                text[ HINT_TEXT_LEN ];
    int                 file_type;
    int                 i;

    fp = fopen( fullname, "rb" );
    if (!fp) return( FALSE );

    if (imgType == BITMAP_IMG) {
        file_type = PMBITMAP_FILETYPE;
    } else if (imgType == ICON_IMG) {
        file_type = PMICON_FILETYPE;
    } else {
        file_type = PMPOINTER_FILETYPE;
    }

    imgfile = OpenPMImage( fp, file_type, &retcode );
    if (!imgfile) {
        fclose( fp );
        return( FALSE );
    }
    if (retcode != file_type) {
        if( imgType == ICON_IMG ) {
            WImgEditError( WIE_ERR_BAD_ICON_FILE, filename );
        } else {
            WImgEditError( WIE_ERR_BAD_CURSOR_FILE, filename );
        }
        ClosePMImage( imgfile );
        fclose( fp );
        return( FALSE );
    }
    node = malloc( sizeof(img_node) * imgfile->count );

    for (i=0; i < imgfile->count; ++i) {
        node[i].imgtype = imgType;
        node[i].bitcount = imgfile->resources[i].xorinfo->cBitCount;
        node[i].width = imgfile->resources[i].xorinfo->cx;
        node[i].height = imgfile->resources[i].xorinfo->cy;
        node[i].hotspot.x = imgfile->resources[i].xhotspot;
        node[i].hotspot.y = imgfile->resources[i].yhotspot;

        image = GetPMImageBits( imgfile, fp, i );
        node[i].handbitmap = PMImageToAndBitmap( image, imgfile, i );
        node[i].hxorbitmap = PMImageToWinXorBitmap(image, imgfile, i, Instance);
        node[i].num_of_images = imgfile->count;
        node[i].viewhwnd = NULL;
        if (i > 0) {
            node[i-1].nexticon = &(node[i]);
        }
        node[i].issaved = TRUE;
        node[i].next = NULL;
        strcpy( node[i].fname, strupr( fullname ) );
        FiniPMImage( image );
    }
    node[i-1].nexticon = NULL;

    ClosePMImage( imgfile );
    fclose( fp );

    GetFnameFromPath( fullname, filename );
    if (imgType == ICON_IMG) {
        sprintf( text, "Opened '%s' (%d icons)", filename, node->num_of_images );
    } else if (imgType == CURSOR_IMG) {
        sprintf( text, "Opened '%s' (%d pointers)", filename, node->num_of_images );
    }
    SetHintText( text );
    CreateNewDrawPad( node );

    free( node );
    return(TRUE);
} /* readInImageFile */

/*
 * readInBitmapFile - reads in a bitmap file
 */
static BOOL readInBitmapFile( char *fullname )
{
    HBITMAP             hrealbitmap;
    HBITMAP             hbitmap;
    HBITMAP             oldbmp1;
    HBITMAP             oldbmp2;
    BITMAPINFOHEADER2   info;
    img_node            node;
    WPI_PRES            pres;
    WPI_PRES            srcpres;
    HDC                 srcdc;
    WPI_PRES            destpres;
    HDC                 destdc;
    char                text[HINT_TEXT_LEN];
    char                filename[ _MAX_FNAME+_MAX_EXT ];

    // NOTE that ReadPMBitmapFile returns an actual hbitmap!!
    hrealbitmap = ReadPMBitmapFile( HMainWindow, fullname, &info );
    hbitmap = MakeWPIBitmap( hrealbitmap );
    GetFnameFromPath( fullname, filename );

    if ( hbitmap ) {
        if ( (info.cx > MAX_DIM) || (info.cy > MAX_DIM) ) {
            WImgEditError( WIE_ERR_BITMAP_TOO_BIG, filename );
            _wpi_deletebitmap( hbitmap );
            return( FALSE );
#if 1
        } else if (info.cBitCount > 4) {
            WImgEditError( WIE_ERR_256CLR_BITMAP, filename );
            _wpi_deletebitmap( hbitmap );
            return( FALSE );
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
        node.issaved = TRUE;
        if (node.bitcount == 1) {
            pres = _wpi_getpres( HWND_DESKTOP );
            srcpres = _wpi_createcompatiblepres( pres, Instance, &srcdc );
            destpres = _wpi_createcompatiblepres( pres, Instance, &destdc );
            _wpi_releasepres( HWND_DESKTOP, pres );

            _wpi_torgbmode( srcpres );
            _wpi_torgbmode( destpres );

            node.hxorbitmap = _wpi_createbitmap( node.width, node.height, 1, 1,
                                                                        NULL );
            _wpi_preparemono( destpres, BLACK, WHITE );

            oldbmp1 = _wpi_selectobject( srcpres, hbitmap );
            oldbmp2 = _wpi_selectobject( destpres, node.hxorbitmap );
            _wpi_bitblt( destpres, 0, 0, node.width, node.height, srcpres,
                                        0, 0, SRCCOPY );
            _wpi_selectobject( srcpres, oldbmp1 );
            _wpi_selectobject( destpres, oldbmp2 );
            _wpi_deletecompatiblepres( srcpres, srcdc );
            _wpi_deletecompatiblepres( destpres, destdc );
            _wpi_deletebitmap( hbitmap );
        } else {
            node.hxorbitmap = hbitmap;
        }

        strcpy( node.fname, fullname );
        sprintf( text, "Opened '%s'.", filename );
        SetHintText( text );

        MakeBitmap( &node, FALSE );
        CreateNewDrawPad( &node );
    } else {
        WImgEditError( WIE_ERR_BAD_BITMAP_FILE, filename );
        return( FALSE );
    }
    return( TRUE );
} /* readInBitmapFile */

/*
 * getOpenFName - let the user select a file name for an open operation
 *                fname must point to a buffer of length at least _MAX_PATH
 *                also sets the type of file (bitmap, icon, cursor).
 */
static BOOL getOpenFName( char *fname )
{
    FILEDLG             filedlg;
    char                ext[ _MAX_EXT ];
    char                drive[ _MAX_DRIVE ];
    char                path[ _MAX_PATH ];
    HWND                hdlg;
    char                fullfile[ CCHMAXPATH ];

    fname[ 0 ] = 0;
    memset( &filedlg, 0, sizeof( FILEDLG ) );
    strcpy( fullfile, initialDir );
    if ( fullfile[strlen(fullfile)-1] != '\\' ) {
        strcat( fullfile, "\\" );
    }
    if (imgType == BITMAP_IMG) {
        strcat( fullfile, "*.bmp" );
    } else if (imgType == ICON_IMG) {
        strcat( fullfile, "*.ico" );
    } else {
        strcat( fullfile, "*.ptr" );
    }

    /*
     * set the values of the filedlg structure ...
     */
    filedlg.cbSize = sizeof( FILEDLG );
    filedlg.fl = FDS_OPEN_DIALOG | FDS_CENTER;
    filedlg.pszTitle = "Open Image File";
    filedlg.pszOKButton = "Open";
    strcpy( filedlg.szFullFile, fullfile );

    hdlg = WinFileDlg( HWND_DESKTOP, HMainWindow, &filedlg );

    if ((hdlg == NULLHANDLE) || (filedlg.lReturn != DID_OK)) {
        return(FALSE);
    }

    strcpy( fname, filedlg.szFullFile );
    _splitpath( fname, drive, path, NULL, ext );
    strcpy( initialDir, drive );
    strcat( initialDir, path );
    initialDir[ strlen(initialDir)-1 ] = '\0';

    if ( !stricmp(ext, ".bmp") ) {
        imgType = BITMAP_IMG;
        return( TRUE );
    } else if ( !stricmp(ext, ".ico") ) {
        imgType = ICON_IMG;
        return( TRUE );
    } else if ( !stricmp(ext, ".ptr") ) {
        imgType = CURSOR_IMG;
        return( TRUE );
    } else {
        return( FALSE );
    }
} /* getOpenFName */

/*
 * OpenImage - Get the filename of the file to open.  Depending on the
 *              extension set the type (.ico, .bmp, .ptr) and call the
 *              appropriate function to open it.
 */
int OpenImage( void )
{
    char                fname[ _MAX_PATH ];
    char                filename[ _MAX_FNAME ];
    char                error_text[HINT_TEXT_LEN];

    if (!getOpenFName( &fname )) {
        SetHintText( "File not opened" );
        return( FALSE );
    }

    switch (imgType) {
    case BITMAP_IMG:
        if (!readInBitmapFile( fname )) {
            GetFnameFromPath( fname, filename );
            sprintf( error_text, "Error opening '%s'", filename );
            SetHintText( error_text );
            return( FALSE );
        }
        break;
    case ICON_IMG:
    case CURSOR_IMG:
        if (!readInImageFile( fname )) {
            GetFnameFromPath( fname, filename );
            sprintf( error_text, "Error opening '%s'", filename );
            SetHintText( error_text );
            return( FALSE );
        }
        break;
    default:
        return( FALSE );
        break;
    }

    SetupMenuAfterOpen();

    return( imgType );
} /* OpenImage */

/*
 * SetInitialOpenDir - sets the initial directory for the open filename
 */
void SetInitialOpenDir( char *new_dir )
{
    if (new_dir) {
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
    return(initialDir);
} /* GetInitOpenDir */

/*
 * OpenFileOnStart - opens a file on program startup
 */
void OpenFileOnStart( char *fname )
{
    int         namelen;
    char        ext[ _MAX_EXT ];
    FILE        *fp;
    char        text[ HINT_TEXT_LEN ];
    char        filename[ _MAX_FNAME+_MAX_EXT ];

    fp = fopen( fname, "r" );
    if( fp == NULL ) {
        GetFnameFromPath( fname, filename );
        sprintf( text, "'%s' could not be found!", filename );
        SetHintText( text );
        return;
    }
    fclose( fp );

    namelen = strlen( fname );
    strcpy( ext, &(fname[namelen-3]) );

    if( stricmp(ext, "bmp") == 0 ) {
        if (!readInBitmapFile( fname )) {
            return;
        }
    } else if( stricmp(ext, "ico") == 0 ) {
        if (!readInImageFile( fname )) {
            return;
        }
    } else if( stricmp(ext, "cur") == 0 ) {
        if (!readInImageFile( fname )) {
            return;
        }
    } else {
        return;
    }

    SetupMenuAfterOpen();

} /* OpenFileOnStart */

