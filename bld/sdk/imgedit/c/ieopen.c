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
* Description:  Open file dialog.
*
****************************************************************************/


#if defined( __NT__ ) && 0
    #undef  WINVER
    #define WINVER 0x0500
    #undef  _WIN32_WINNT
    #define _WIN32_WINNT 0x0500
#endif

#include "imgedit.h"
#include <commdlg.h>
#include <shellapi.h>
#include <dos.h>
#include <cderr.h>
#include "colors.h"
#include "iebmpdat.h"
#include "wrbitmap.h"
#include "wricon.h"
#include "wrselimg.h"
#include "wresdefn.h"
#include "wclbhelp.h"
#include "pathgrp2.h"

#include "clibext.h"


#if defined( __WINDOWS__ )
    #pragma library( "commdlg.lib" )
#endif


extern bool             OpenNewFiles;

/* Local Window callback functions prototypes */
WINEXPORT UINT_PTR CALLBACK OpenOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static image_type       imgType = BITMAP_IMG;
static char             initialDir[_MAX_PATH + _MAX_DIR];

/*
 * SetupMenuAfterOpen
 */
void SetupMenuAfterOpen( void )
{
    HMENU               hmenu;

    hmenu = GetMenu( HMainWindow );

    if( !ImgedIsDDE ) {
        EnableMenuItem( hmenu, IMGED_REST, MF_ENABLED );
        EnableMenuItem( hmenu, IMGED_SAVE, MF_GRAYED );
    }
    EnableMenuItem( hmenu, IMGED_CLEAR, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_SAVE_AS, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_LINE, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_RECTO, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_RECTF, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_CIRCLEO, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_CIRCLEF, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_FREEHAND, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_FILL, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_BRUSH, MF_ENABLED );
    EnableMenuItem( hmenu, IMGED_CLIP, MF_ENABLED );

    SetFocus( HMainWindow );

} /* SetupMenuAfterOpen */

/*
 * doReadInBitmapFile
 */
static bool doReadInBitmapFile( HBITMAP hbitmap, bitmap_info *bmi, const char *fullname,
                                WRInfo *info, WResLangNode *lnode )
{
    HBITMAP             old_hbitmap1;
    HBITMAP             old_hbitmap2;
    img_node            node;
    char                filename[_MAX_FNAME + _MAX_EXT];
    BITMAPINFOHEADER    *h;
    HDC                 hdc;
    HDC                 srcdc;
    HDC                 dstdc;

    GetFnameFromPath( fullname, filename );

    if( hbitmap != NULL ) {
        h = &bmi->u.bm_info->bmiHeader;

        if( h->biWidth > MAX_DIM || h->biHeight > MAX_DIM ) {
            WImgEditError( WIE_ERR_BITMAP_TOO_BIG, filename );
            MemFree( bmi->u.bm_info );
            DeleteObject( hbitmap );
            return( false );
        }

#if 0
        /* Should not be an error... */
        else if( h->biBitCount > 8 ) {
            WImgEditError( WIE_ERR_TOO_MANY_COLOURS, filename );
            MemFree( bmi->u.bm_info );
            DeleteObject( hbitmap );
            return( false );
        }
#endif
        node.imgtype = BITMAP_IMG;
        node.width = bmi->u.bm_info->bmiHeader.biWidth;
        node.height = bmi->u.bm_info->bmiHeader.biHeight;
        node.bitcount = bmi->u.bm_info->bmiHeader.biBitCount;
        node.hotspot.x = 0;
        node.hotspot.y = 0;
        node.num_of_images = 1;
        node.nexticon = NULL;
        node.issaved = true;
        if( node.bitcount == 1 ) {
            hdc = GetDC( NULL );
            srcdc = CreateCompatibleDC( hdc );
            dstdc = CreateCompatibleDC( hdc );
            ReleaseDC( NULL, hdc );

            node.xor_hbitmap = CreateCompatibleBitmap( dstdc, node.width, node.height );
            old_hbitmap1 = SelectObject( srcdc, hbitmap );
            old_hbitmap2 = SelectObject( dstdc, node.xor_hbitmap );
            BitBlt( dstdc, 0, 0, node.width, node.height, srcdc, 0, 0, SRCCOPY );
            SelectObject( srcdc, old_hbitmap1 );
            SelectObject( dstdc, old_hbitmap2 );
            DeleteDC( srcdc );
            DeleteDC( dstdc );
            DeleteObject( hbitmap );
        } else {
            node.xor_hbitmap = hbitmap;
        }

        strcpy( node.fname, fullname );
        PrintHintTextByID( WIE_OPENEDTEXT, filename );

        node.wrinfo = info;
        node.lnode = lnode;

        MakeBitmap( &node, false );
        CreateNewDrawPad( &node );

        MemFree( bmi->u.bm_info );
        return( true );
    }
    return( false );

} /* doReadInBitmapFile */

/*
 * readInBitmapFile - read in the bitmap file, initialize the draw area and
 *                    the view window
 */
static bool readInBitmapFile( const char *fullname )
{
    bitmap_info         bmi;
    HBITMAP             hbitmap;
    HCURSOR             prevcursor;

    prevcursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
    hbitmap = ReadBitmapFile( HMainWindow, fullname, &bmi );
    SetCursor( prevcursor );

    if( hbitmap == (HBITMAP)NULL ) {
        if( bmi.u.bm_info != NULL && bmi.u.bm_info->bmiHeader.biBitCount > 32 ) { /* Was 8 */
            WImgEditError( WIE_ERR_TOO_MANY_COLORS, fullname );
        } else {
            WImgEditError( WIE_ERR_BAD_BITMAP_FILE, fullname );
        }
        if( bmi.u.bm_info != NULL ) {
            MemFree( bmi.u.bm_info );
        }
        return( false );
    }

    return( doReadInBitmapFile( hbitmap, &bmi, fullname, NULL, NULL ) );

} /* readInBitmapFile */

/*
 * ReadBitmapFromData
 */
bool ReadBitmapFromData( void *data, const char *fullname, WRInfo *info, WResLangNode *lnode )
{
    bitmap_info         bmi;
    HBITMAP             hbitmap;
    HCURSOR             prevcursor;
    bool                ok;

    prevcursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
    hbitmap = BitmapFromData( data, &bmi );
    SetCursor( prevcursor );
    if( hbitmap == (HBITMAP)NULL ) {
        WImgEditError( WIE_ERR_BAD_BITMAP_DATA, fullname );
        return( false );
    }

    ok = doReadInBitmapFile( hbitmap, &bmi, fullname, info, lnode );

    if( ok ) {
        SetupMenuAfterOpen();
    }

    return( ok );

} /* ReadBitmapFromData */

/*
 * WriteIconLoadedText
 */
void WriteIconLoadedText( char *filename, int num )
{
    char        *text;
    char        msg[_MAX_PATH];

    text = IEAllocRCString( WIE_ICONOPENEDTEXT );
    if( text != NULL ) {
        sprintf( msg, text, filename, num );
        SetHintText( msg );
        IEFreeRCString( text );
    }

} /* WriteIconLoadedText */

/*
 * readInIconFile - read the icon file and set up structures
 */
static bool readInIconFile( const char *fname )
{
    FILE                *fp;
    an_img_file         *iconfile;
    img_node            *node;
    int                 num_of_images;
    HDC                 hdc;
    int                 i;
    an_img              *icon;
    char                filename[_MAX_FNAME + _MAX_EXT];

    fp = fopen( fname, "rb" );
    if( fp == NULL ) {
        WImgEditError( WIE_ERR_FILE_NOT_OPENED, fname );
        return( false );
    }

    GetFnameFromPath( fname, filename );
#ifdef JAMIE
    {
        char msg[80];
        sprintf( msg, "Jamie: IconHeader size = %d", sizeof( an_img_file ) );
        MessageBox( HMainWindow, msg, "FYI", MB_OK );
    }
#endif
    iconfile = ImageOpen( fp );
    if( iconfile == NULL ) {
        fclose( fp );
        WImgEditError( WIE_ERR_BAD_ICON_FILE, filename );
        return( false );
    }

    num_of_images = iconfile->count;

#if 0
    /* See biBitCount test below... */
    for( i = 0; i < num_of_images; i++ ) {
        if( iconfile->resources[i].color_count != 2 &&
            iconfile->resources[i].color_count != 8 &&
            iconfile->resources[i].color_count != 16 &&
            iconfile->resources[i].color_count != 0 ) {
            WImgEditError( WIE_ERR_BAD_ICON_CLR, filename );
            ImageClose( iconfile );
            fclose( fp );
            return( false );
        }
    }
#endif

    node = MemAlloc( sizeof( img_node ) * num_of_images );

    hdc = GetDC( NULL );
    for( i = 0; i < num_of_images; i++ ) {
        icon = ImgResourceToImg( fp, iconfile, i );

        if( icon->bm->bmiHeader.biBitCount != 4 &&
            icon->bm->bmiHeader.biBitCount != 1 &&
            icon->bm->bmiHeader.biBitCount != 8 ) {
            WImgEditError( WIE_ERR_BAD_ICON_CLR, filename );
            ReleaseDC( NULL, hdc );
            ImageFini( icon );
            ImageClose( iconfile );
            fclose( fp );
            MemFree( node );
            return( false );
        }

        node[i].imgtype = ICON_IMG;
        node[i].bitcount = icon->bm->bmiHeader.biBitCount;
        node[i].width = icon->bm->bmiHeader.biWidth;
        node[i].height = icon->bm->bmiHeader.biHeight;
        node[i].hotspot.x = 0;
        node[i].hotspot.y = 0;
        node[i].and_hbitmap = ImgToAndBitmap( hdc, icon );
        node[i].xor_hbitmap = ImgToXorBitmap( hdc, icon );
        node[i].num_of_images = num_of_images;
        node[i].viewhwnd = NULL;
        node[i].wrinfo = NULL;
        node[i].lnode = NULL;
        if( i > 0 ) {
            node[i - 1].nexticon = &node[i];
        }
        node[i].issaved = true;
        node[i].next = NULL;
        strupr( strcpy( node[i].fname, fname ) );
        ImageFini( icon );
    }
    node[i - 1].nexticon = NULL;

    ReleaseDC( NULL, hdc );
    ImageClose( iconfile );
    fclose( fp );

    WriteIconLoadedText( filename, node->num_of_images );
    CreateNewDrawPad( node );

    MemFree( node );
    return( true );

} /* readInIconFile */

/*
 * ReadIconFromData - read the icon data and set up structures
 */
bool ReadIconFromData( void *data, const char *fname, WRInfo *info, WResLangNode *lnode )
{
    unsigned            pos;
    an_img_file         *iconfile;
    img_node            *node;
    int                 num_of_images;
    HDC                 hdc;
    int                 i;
    an_img              *icon;
    char                filename[_MAX_FNAME + _MAX_EXT];

    pos = 0;
    GetFnameFromPath( fname, filename );
    iconfile = ImageOpenData( (BYTE *)data, &pos );
    if( iconfile == NULL ) {
        WImgEditError( WIE_ERR_BAD_ICON_DATA, filename );
        return( false );
    }
    num_of_images = iconfile->count;

#if 0
    /* See biBitCount test below... */
    for( i = 0; i < num_of_images; i++ ) {
        if( iconfile->resources[i].color_count != 2 &&
            iconfile->resources[i].color_count != 8 &&
            iconfile->resources[i].color_count != 16 &&
            iconfile->resources[i].color_count != 0 ) {
            WImgEditError( WIE_ERR_BAD_ICON_CLR, filename );
            ImageClose( iconfile );
            return( false );
        }
    }
#endif

    node = MemAlloc( sizeof( img_node ) * num_of_images );

    hdc = GetDC( NULL );
    for( i = 0; i < num_of_images; i++ ) {
        icon = ImgResourceToImgData( (BYTE *)data, &pos, iconfile, i );
        if( icon->bm->bmiHeader.biBitCount != 4 &&
            icon->bm->bmiHeader.biBitCount != 1 &&
            icon->bm->bmiHeader.biBitCount != 8 ) {
            WImgEditError( WIE_ERR_BAD_ICON_CLR, filename );
            ReleaseDC( NULL, hdc );
            ImageFini( icon );
            ImageClose( iconfile );
            MemFree( node );
            return( false );
        }

        node[i].imgtype = ICON_IMG;
        node[i].bitcount = icon->bm->bmiHeader.biBitCount;
        node[i].width = icon->bm->bmiHeader.biWidth;
        node[i].height = icon->bm->bmiHeader.biHeight;
        node[i].hotspot.x = 0;
        node[i].hotspot.y = 0;
        node[i].and_hbitmap = ImgToAndBitmap( hdc, icon );
        node[i].xor_hbitmap = ImgToXorBitmap( hdc, icon );
        node[i].num_of_images = num_of_images;
        node[i].viewhwnd = NULL;
        if( i > 0 ) {
            node[i - 1].nexticon = &node[i];
        }
        node[i].wrinfo = NULL;
        node[i].lnode = NULL;
        if( i == 0 ) {
            node[i].wrinfo = info;
            node[i].lnode = lnode;
        }
        node[i].issaved = true;
        node[i].next = NULL;
        strupr( strcpy( node[i].fname, fname ) );
        ImageFini( icon );
    }
    node[i - 1].nexticon = NULL;

    ReleaseDC( NULL, hdc );
    ImageClose( iconfile );

    WriteIconLoadedText( filename, node->num_of_images );
    CreateNewDrawPad( node );

    MemFree( node );

    SetupMenuAfterOpen();

    return( true );

} /* ReadIconFromData */

/*
 * doReadCursor
 */
static bool doReadCursor( const char *fname, an_img_file *cursorfile, an_img *cursor,
                   WRInfo *info, WResLangNode *lnode )
{
    img_node            node;
    HDC                 hdc;
    char                filename[_MAX_FNAME + _MAX_EXT];

    if( cursorfile == NULL || cursor == NULL ) {
        return( false );
    }

    GetFnameFromPath( fname, filename );

    node.imgtype = CURSOR_IMG;
    node.width = cursor->bm->bmiHeader.biWidth;
    node.height = cursor->bm->bmiHeader.biHeight;
    if( node.height == 0 ) {
        node.height = node.width;
    }
    node.bitcount = cursor->bm->bmiHeader.biBitCount;
    node.hotspot.x = cursorfile->resources->xhotspot;
    node.hotspot.y = cursorfile->resources->yhotspot;
    node.issaved = true;
    node.num_of_images = 1;
    node.next = NULL;
    node.nexticon = NULL;
    node.wrinfo = info;
    node.lnode = lnode;

    hdc = GetDC( NULL );
    node.and_hbitmap = ImgToAndBitmap( hdc, cursor );
    node.xor_hbitmap = ImgToXorBitmap( hdc, cursor );
    ReleaseDC( NULL, hdc );

    strupr( strcpy( node.fname, fname ) );
    ImageFini( cursor );
    ImageClose( cursorfile );

    PrintHintTextByID( WIE_OPENEDTEXT, filename );
    MakeIcon( &node, false );           // also makes cursors
    CreateNewDrawPad( &node );

    return( true );

} /* doReadCursor */

/*
 * readInCursorFile - read the cursor file and set up structures
 */
static bool readInCursorFile( const char *fname )
{
    FILE                *fp;
    an_img_file         *cursorfile;
    an_img              *cursor;

    fp = fopen( fname, "rb" );
    if( fp == NULL ) {
        WImgEditError( WIE_ERR_FILE_NOT_OPENED, fname );
        return( false );
    }

    cursorfile = ImageOpen( fp );
    if( cursorfile == NULL ) {
        fclose( fp );
        WImgEditError( WIE_ERR_BAD_CURSOR_FILE, fname );
        return( false );
    }
    cursor = ImgResourceToImg( fp, cursorfile, 0 );
    fclose( fp );

    return( doReadCursor( fname, cursorfile, cursor, NULL, NULL ) );

} /* readInCursorFile */

/*
 * ReadCursorFromData - read the cursor data and set up structures
 */
bool ReadCursorFromData( void *data, const char *fname, WRInfo *info,
                         WResLangNode *lnode )
{
    unsigned            pos;
    an_img_file         *cursorfile;
    an_img              *cursor;
    bool                ok;

    pos = 0;
    cursorfile = ImageOpenData( (BYTE *)data, &pos );
    if( cursorfile == NULL ) {
        WImgEditError( WIE_ERR_BAD_CURSOR_DATA, fname );
        return( false );
    }
    cursor = ImgResourceToImgData( (BYTE *)data, &pos, cursorfile, 0 );

    ok = doReadCursor( fname, cursorfile, cursor, info, lnode );

    if( ok ) {
        SetupMenuAfterOpen();
    }

    return( ok );

} /* ReadCursorFromData */

/*
 * OpenOFNHookProc - hook used called by common dialog for 3D controls
 */
UINT_PTR CALLBACK OpenOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
#ifdef _WIN64
    /* unused parameters */ (void)hwnd; (void)wparam; (void)lparam;
#else
    /* unused parameters */ (void)wparam; (void)lparam;
#endif

    switch( msg ) {
    case WM_INITDIALOG:
        // We must call this to subclass the directory listbox even
        // if the app calls Ctl3dAutoSubclass (commdlg bug).
#ifndef _WIN64
  #if defined( __NT__ )
        // Only do it if NOT new shell.
        if( LOBYTE( LOWORD( GetVersion() ) ) < 4 ) {
  #endif
            IECtl3dSubclassDlgAll( hwnd );
  #if defined( __NT__ )
        }
  #endif
#endif
        return( TRUE );
    }
    return( FALSE );

} /* OpenOFNHookProc */

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
    imgType = GetImageFileType( pg.ext, true );
    return( imgType != UNDEF_IMG );

} /* updateOpenFileInfo */

/*
 * getOpenFName - let the user select a file name for an open operation
 *              - fname must point to a buffer of length at least _MAX_PATH
 *              - also set the type of file (bitmap, icon, cursor)
 */
static bool getOpenFName( char *fname )
{
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    bool                ok;
    long                of_size;

    fname[0] = '\0';

    of_size = sizeof( OPENFILENAME );
#ifndef _WIN64
  #if defined( __NT__ ) && (_WIN32_WINNT >= 0x0500)
    if( LOBYTE( LOWORD( GetVersion() ) ) < 5 ) {
        /* Set the appropriate structure size to make this work on Windows 95. */
        of_size = OPENFILENAME_SIZE_VERSION_400;
    }
  #endif
#endif
    memset( &of, 0, of_size );
    of.lStructSize = of_size;
    of.hwndOwner = HMainWindow;
    of.lpstrFilter = (LPSTR)IEImageFilter;
    of.lpstrDefExt = "*.*";
    of.nFilterIndex = (long)imgType;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrFileTitle = szFileTitle;
    of.nMaxFileTitle = sizeof( szFileTitle );
    of.lpstrTitle = IEOpenImageTitle;
    of.lpstrInitialDir = initialDir;
#ifndef __NT__
    /* Important! Do not use hook in Win32, you will not get the nice dialog! */
    of.lpfnHook = MakeProcInstance_OFNHOOK( OpenOFNHookProc, Instance );
    of.Flags = OFN_ENABLEHOOK;
#endif
    of.Flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ok = ( GetOpenFileName( &of ) != 0 );
#ifndef __NT__
    FreeProcInstance_OFNHOOK( of.lpfnHook );
#endif

    if( ok ) {
        ok = updateOpenFileInfo( fname );
    }
    return( ok );

} /* getOpenFName */

/*
 * readInResourceFile
 */
static bool readInResourceFile( const char *fullname )
{
    BYTE                *data;
    size_t              dsize;
    WRInfo              *info;
    WRSelectImageInfo   *sii;
    HELPFUNC            hcb;
    bool                ok;

    info = NULL;
    sii = NULL;
    data = NULL;
    ok = (fullname != NULL);

    if( ok ) {
        info = WRLoadResource( fullname, WR_DONT_KNOW );
        ok = (info != NULL);
    }

    if( ok ) {
        hcb = MakeProcInstance_HELP( IEHelpCallBack, Instance );
        sii = WRSelectImage( HMainWindow, info, hcb );
        FreeProcInstance_HELP( hcb );
        ok = (sii != NULL && sii->lnode != NULL);
    }

    if( ok ) {
        if( sii->type == RESOURCE2INT( RT_BITMAP ) ) {
            imgType = BITMAP_IMG;
            data = WRCopyResData( info, sii->lnode );
            dsize = sii->lnode->Info.Length;
            ok = (data != NULL);
            if( ok ) {
                ok = WRAddBitmapFileHeader( &data, &dsize );
            }
        } else if( sii->type == RESOURCE2INT( RT_GROUP_CURSOR ) ) {
            imgType = CURSOR_IMG;
            ok = WRCreateCursorData( info, sii->lnode, &data, &dsize );
        } else if( sii->type == RESOURCE2INT( RT_GROUP_ICON ) ) {
            imgType = ICON_IMG;
            ok = WRCreateIconData( info, sii->lnode, &data, &dsize );
        } else {
            imgType = UNDEF_IMG;
            ok = false;
        }
    }

    if( ok ) {
        if( sii->type == RESOURCE2INT( RT_BITMAP ) ) {
            ok = ReadBitmapFromData( data, fullname, info, sii->lnode );
        } else if( sii->type == RESOURCE2INT( RT_GROUP_CURSOR ) ) {
            ok = ReadCursorFromData( data, fullname, info, sii->lnode );
        } else if( sii->type == RESOURCE2INT( RT_GROUP_ICON ) ) {
            ok = ReadIconFromData( data, fullname, info, sii->lnode );
        }
    }

    if( sii != NULL ) {
        WRFreeSelectImageInfo( sii );
    }

    if( data != NULL ) {
        MemFree( data );
    }

    return( ok );

} /* readInResourceFile */

/*
 * reallyOpenImage
 */
static bool reallyOpenImage( const char *fname )
{
    char        filename[_MAX_FNAME + _MAX_EXT];
    bool        ok;

    switch( imgType ) {
    case BITMAP_IMG:
        ok = readInBitmapFile( fname );
        break;
    case ICON_IMG:
        ok = readInIconFile( fname );
        break;
    case CURSOR_IMG:
        ok = readInCursorFile( fname );
        break;
    case RESOURCE_IMG:
        ok = readInResourceFile( fname );
        break;
    default:
        GetFnameFromPath( fname, filename );
        WImgEditError( WIE_ERR_BAD_FILE_EXT, filename );
        imgType = BITMAP_IMG;
        ok = false;
    }

    return( ok );

} /* reallyOpenImage */

/*
 * OpenImage - get the filename of the file to open
 *           - depending on the extension, set the type (.ico, .bmp, .cur) and call the
 *             appropriate function to open it
 */
bool OpenImage( HANDLE hDrop )
{
    char        fname[_MAX_PATH];
    bool        ok;

    ok = false;
    if( NULL == hDrop ) {
        /*
         * Not doing a drag-drop
         */
        if( !getOpenFName( fname ) ) {
            if( CommDlgExtendedError() == FNERR_INVALIDFILENAME ) {
                WImgEditError( WIE_ERR_BAD_FILENAME, fname );
            }
            return( ok );
        }
        ok = reallyOpenImage( fname );
    } else {
        /*
         * hDrop is only ever !NULL when we're dealing with a WM_DROPFILES
         * message, and that only happens with __NT__
         */
#ifdef __NT__
        int     nFiles = DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 );
        int     i;

        ok = true;
        for( i = 0; ok && i < nFiles; i++ ) {
            DragQueryFile( hDrop, i, fname, _MAX_PATH - 1 );
            updateOpenFileInfo( fname );
            ok = reallyOpenImage( fname );
        }
#endif
    }

    if( ok ) {
        SetupMenuAfterOpen();
    }

    return( ok );

} /* OpenImage */

/*
 * getOpenPalName - let the user select a palette file name to load
 */
static bool getOpenPalName( char *fname )
{
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    bool                ok;
    long                of_size;

    fname[0] = '\0';

    of_size = sizeof( OPENFILENAME );
#ifndef _WIN64
  #if defined( __NT__ ) && (_WIN32_WINNT >= 0x0500)
    if( LOBYTE( LOWORD( GetVersion() ) ) < 5 ) {
        /* Set the appropriate structure size to make this work on Windows 95. */
        of_size = OPENFILENAME_SIZE_VERSION_400;
    }
  #endif
#endif
    memset( &of, 0, of_size );
    of.lStructSize = of_size;
    of.hwndOwner = HMainWindow;
    of.lpstrFilter = (LPSTR)IEPaletteFilter;
    of.lpstrDefExt = "*.*";
    of.nFilterIndex = 0L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrFileTitle = szFileTitle;
    of.nMaxFileTitle = sizeof( szFileTitle );
    of.lpstrTitle = IEOpenPaletteTitle;
    of.lpstrInitialDir = initialDir;
    of.Flags =  OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
#ifndef __NT__
    of.Flags |= OFN_ENABLEHOOK;
    of.lpfnHook = MakeProcInstance_OFNHOOK( OpenOFNHookProc, Instance );
#endif
    ok = ( GetOpenFileName( &of ) != 0 );
#ifndef __NT__
    FreeProcInstance_OFNHOOK( of.lpfnHook );
#endif
    return( ok );

} /* getOpenPalName */

/*
 * LoadColorPalette - load a palette
 */
bool LoadColorPalette( void )
{
    char                fname[_MAX_PATH];
    char                filename[_MAX_FNAME + _MAX_EXT];
    a_pal_file          *pal_file;
    FILE                *fp;
    WORD                file_type;

    if( !getOpenPalName( fname ) ) {
        if( CommDlgExtendedError() == FNERR_INVALIDFILENAME ) {
            WImgEditError( WIE_ERR_BAD_FILENAME, fname );
            return( false );
        }
        return( true );
    }

    pal_file = MemAlloc( sizeof( a_pal_file ) );
    fp = fopen( fname, "rb" );
    if( fp == NULL ) {
        WImgEditError( WIE_ERR_FILE_NOT_OPENED, fname );
        return( false );
    }

    GetFnameFromPath( fname, filename );

    fseek( fp, 0L, SEEK_SET );
    fread( &file_type, sizeof( WORD ), 1, fp );
    if( file_type != PALETTE_FILE ) {
        WImgEditError( WIE_ERR_BAD_PALFILE, filename );
        fclose( fp );
        return( false );
    }

    fseek( fp, 0L, SEEK_SET );
    fread( pal_file, sizeof( a_pal_file ), 1, fp );

    fclose( fp );
    SetNewPalette( pal_file );
    PrintHintTextByID( WIE_PALETTELOADEDFROM, filename );

    MemFree( pal_file );
    return( true );

} /* LoadColorPalette */

/*
 * SetInitialOpenDir - set the initial directory for the open filename
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
 * GetInitOpenDir - get the directory that we want to use as our initial one
 *                  next time we run
 */
char *GetInitOpenDir( void )
{
    return( initialDir );

} /* GetInitOpenDir */

/*
 * OpenFileOnStart - open a file on program startup
 */
void OpenFileOnStart( const char *fname )
{
    pgroup2     pg;
    FILE        *fp;
    image_type  img_type;

    fp = fopen( fname, "r" );
    if( fp == NULL ) {
        if( OpenNewFiles ) {
            if( NewImage( UNDEF_IMG, fname ) ) {
                return;
            }
        }
        WImgEditError( WIE_ERR_STARTUP_FNO, fname );
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
        if( readInIconFile( fname ) ) {
            SetupMenuAfterOpen();
        }
    } else if( img_type == CURSOR_IMG ) {
        if( readInCursorFile( fname ) ) {
            SetupMenuAfterOpen();
        }
    }

} /* OpenFileOnStart */
