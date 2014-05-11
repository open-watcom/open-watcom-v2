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
* Description:  Open file dialog.
*
****************************************************************************/


#include "imgedit.h"
#include <commdlg.h>
#include <shellapi.h>
#include <malloc.h>
#include <dos.h>
#include <cderr.h>
#include "colors.h"
#include "iebmpdat.h"
#include "wrbitmap.h"
#include "wricon.h"
#include "wrselimg.h"
#include "iemem.h"

#ifdef __NT__
    #pragma library( "shell32.lib" )
#else
    #pragma library( "commdlg.lib" )
#endif

static signed short     imgType = BITMAP_IMG;
static char             initialDir[_MAX_PATH + _MAX_DIR];

extern BOOL OpenNewFiles;

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
static BOOL doReadInBitmapFile( HBITMAP hbitmap, bitmap_info *bmi, char *fullname,
                                WRInfo *info, WResLangNode *lnode )
{
    HBITMAP             oldbmp1;
    HBITMAP             oldbmp2;
    img_node            node;
    char                filename[_MAX_FNAME + _MAX_EXT];
    BITMAPINFOHEADER    *h;
    HDC                 hdc;
    HDC                 srcdc;
    HDC                 destdc;

    GetFnameFromPath( fullname, filename );

    if( hbitmap != NULL ) {
        h = &bmi->u.bm_info->bmiHeader;

        if( h->biWidth > MAX_DIM || h->biHeight > MAX_DIM ) {
            WImgEditError( WIE_ERR_BITMAP_TOO_BIG, filename );
            MemFree( bmi->u.bm_info );
            DeleteObject( hbitmap );
            return( FALSE );
        }

#if 0
        /* Should not be an error... */
        else if( h->biBitCount > 8 ) {
            WImgEditError( WIE_ERR_TOO_MANY_COLOURS, filename );
            MemFree( bmi->u.bm_info );
            DeleteObject( hbitmap );
            return( FALSE );
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
        node.issaved = TRUE;
        if( node.bitcount == 1 ) {
            hdc = GetDC( NULL );
            srcdc = CreateCompatibleDC( hdc );
            destdc = CreateCompatibleDC( hdc );
            ReleaseDC( NULL, hdc );

            node.hxorbitmap = CreateCompatibleBitmap( destdc, node.width, node.height );
            oldbmp1 = SelectObject( srcdc, hbitmap );
            oldbmp2 = SelectObject( destdc, node.hxorbitmap );
            BitBlt( destdc, 0, 0, node.width, node.height, srcdc, 0, 0, SRCCOPY );
            SelectObject( srcdc, oldbmp1 );
            SelectObject( destdc, oldbmp2 );
            DeleteDC( srcdc );
            DeleteDC( destdc );
            DeleteObject( hbitmap );
        } else {
            node.hxorbitmap = hbitmap;
        }

        strcpy( node.fname, fullname );
        PrintHintTextByID( WIE_OPENEDTEXT, filename );

        node.wrinfo = info;
        node.lnode = lnode;

        MakeBitmap( &node, FALSE );
        CreateNewDrawPad( &node );

        MemFree( bmi->u.bm_info );
        return( TRUE );
    }
    return( FALSE );

} /* doReadInBitmapFile */

/*
 * readInBitmapFile - read in the bitmap file, initialize the draw area and
 *                    the view window
 */
static BOOL readInBitmapFile( char *fullname )
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
        return( FALSE );
    }

    return( doReadInBitmapFile( hbitmap, &bmi, fullname, NULL, NULL ) );

} /* readInBitmapFile */

/*
 * ReadBitmapFromData
 */
BOOL ReadBitmapFromData( void *data, char *fullname, WRInfo *info, WResLangNode *lnode )
{
    bitmap_info         bmi;
    HBITMAP             hbitmap;
    HCURSOR             prevcursor;
    BOOL                ret;

    prevcursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
    hbitmap = BitmapFromData( data, &bmi );
    SetCursor( prevcursor );
    if( hbitmap == (HBITMAP)NULL ) {
        WImgEditError( WIE_ERR_BAD_BITMAP_DATA, fullname );
        return( FALSE );
    }

    ret = doReadInBitmapFile( hbitmap, &bmi, fullname, info, lnode );

    if( ret ) {
        SetupMenuAfterOpen();
    }

    return( ret );

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
static BOOL readInIconFile( char *fname )
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
        return( FALSE );
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
        return( FALSE );
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
            return( FALSE );
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
            return( FALSE );
        }

        node[i].imgtype = ICON_IMG;
        node[i].bitcount = icon->bm->bmiHeader.biBitCount;
        node[i].width = icon->bm->bmiHeader.biWidth;
        node[i].height = icon->bm->bmiHeader.biHeight;
        node[i].hotspot.x = 0;
        node[i].hotspot.y = 0;
        node[i].handbitmap = ImgToAndBitmap( hdc, icon );
        node[i].hxorbitmap = ImgToXorBitmap( hdc, icon );
        node[i].num_of_images = num_of_images;
        node[i].viewhwnd = NULL;
        node[i].wrinfo = NULL;
        node[i].lnode = NULL;
        if( i > 0 ) {
            node[i - 1].nexticon = &node[i];
        }
        node[i].issaved = TRUE;
        node[i].next = NULL;
        strcpy( node[i].fname, strupr( fname ) );
        ImageFini( icon );
    }
    node[i - 1].nexticon = NULL;

    ReleaseDC( NULL, hdc );
    ImageClose( iconfile );
    fclose( fp );

    WriteIconLoadedText( filename, node->num_of_images );
    CreateNewDrawPad( node );

    MemFree( node );
    return( TRUE );

} /* readInIconFile */

/*
 * ReadIconFromData - read the icon data and set up structures
 */
BOOL ReadIconFromData( void *data, char *fname, WRInfo *info, WResLangNode *lnode )
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
        return( FALSE );
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
            return( FALSE );
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
            return( FALSE );
        }

        node[i].imgtype = ICON_IMG;
        node[i].bitcount = icon->bm->bmiHeader.biBitCount;
        node[i].width = icon->bm->bmiHeader.biWidth;
        node[i].height = icon->bm->bmiHeader.biHeight;
        node[i].hotspot.x = 0;
        node[i].hotspot.y = 0;
        node[i].handbitmap = ImgToAndBitmap( hdc, icon );
        node[i].hxorbitmap = ImgToXorBitmap( hdc, icon );
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
        node[i].issaved = TRUE;
        node[i].next = NULL;
        strcpy( node[i].fname, strupr( fname ) );
        ImageFini( icon );
    }
    node[i - 1].nexticon = NULL;

    ReleaseDC( NULL, hdc );
    ImageClose( iconfile );

    WriteIconLoadedText( filename, node->num_of_images );
    CreateNewDrawPad( node );

    MemFree( node );

    SetupMenuAfterOpen();

    return( TRUE );

} /* ReadIconFromData */

/*
 * doReadCursor
 */
BOOL doReadCursor( char *fname, an_img_file *cursorfile, an_img *cursor,
                   WRInfo *info, WResLangNode *lnode )
{
    img_node            node;
    HDC                 hdc;
    char                filename[_MAX_FNAME + _MAX_EXT];

    if( cursorfile == NULL || cursor == NULL ) {
        return( FALSE );
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
    node.issaved = TRUE;
    node.num_of_images = 1;
    node.next = NULL;
    node.nexticon = NULL;
    node.wrinfo = info;
    node.lnode = lnode;

    hdc = GetDC( NULL );
    node.handbitmap = ImgToAndBitmap( hdc, cursor );
    node.hxorbitmap = ImgToXorBitmap( hdc, cursor );
    ReleaseDC( NULL, hdc );

    strcpy( node.fname, strupr( fname ) );
    ImageFini( cursor );
    ImageClose( cursorfile );

    PrintHintTextByID( WIE_OPENEDTEXT, filename );
    MakeIcon( &node, FALSE );           // also makes cursors
    CreateNewDrawPad( &node );

    return( TRUE );

} /* doReadCursor */

/*
 * readInCursorFile - read the cursor file and set up structures
 */
static BOOL readInCursorFile( char *fname )
{
    FILE                *fp;
    an_img_file         *cursorfile;
    an_img              *cursor;

    fp = fopen( fname, "rb" );
    if( fp == NULL ) {
        WImgEditError( WIE_ERR_FILE_NOT_OPENED, fname );
        return( FALSE );
    }

    cursorfile = ImageOpen( fp );
    if( cursorfile == NULL ) {
        fclose( fp );
        WImgEditError( WIE_ERR_BAD_CURSOR_FILE, fname );
        return( FALSE );
    }
    cursor = ImgResourceToImg( fp, cursorfile, 0 );
    fclose( fp );

    return( doReadCursor( fname, cursorfile, cursor, NULL, NULL ) );

} /* readInCursorFile */

/*
 * ReadCursorFromData - read the cursor data and set up structures
 */
BOOL ReadCursorFromData( void *data, char *fname, WRInfo *info,
                         WResLangNode *lnode )
{
    unsigned            pos;
    an_img_file         *cursorfile;
    an_img              *cursor;
    BOOL                ret;

    pos = 0;
    cursorfile = ImageOpenData( (BYTE *)data, &pos );
    if( cursorfile == NULL ) {
        WImgEditError( WIE_ERR_BAD_CURSOR_DATA, fname );
        return( FALSE );
    }
    cursor = ImgResourceToImgData( (BYTE *)data, &pos, cursorfile, 0 );

    ret = doReadCursor( fname, cursorfile, cursor, info, lnode );

    if( ret ) {
        SetupMenuAfterOpen();
    }

    return( ret );

} /* ReadCursorFromData */

/*
 * OpenHook - hook used called by common dialog for 3D controls
 */
BOOL CALLBACK OpenHook( HWND hwnd, int msg, UINT wparam, LONG lparam )
{
    wparam = wparam;
    lparam = lparam;
    hwnd = hwnd;
    switch( msg ) {
    case WM_INITDIALOG:
        // We must call this to subclass the directory listbox even
        // if the app calls Ctl3dAutoSubclass (commdlg bug).
#if defined( __NT__ )
        // Only do it if NOT new shell.
        if( LOBYTE( LOWORD( GetVersion() ) ) < 4 ) {
#endif
            IECtl3dSubclassDlg( hwnd, CTL3D_ALL );
#if defined( __NT__ )
        }
#endif
        return( TRUE );
    }
    return( FALSE );

} /* OpenHook */

/*
 * getImageTypeFromFilename
 */
static int getImageTypeFromFilename( char *fname )
{
    char                ext[_MAX_EXT];
    char                drive[_MAX_DRIVE];
    char                path[_MAX_PATH];

    _splitpath( fname, drive, path, NULL, ext );
    strcpy( initialDir, drive );
    strcat( initialDir, path );
    initialDir[strlen( initialDir ) - 1] = '\0';

    if( !stricmp( ext, ".bmp" ) ) {
        return( BITMAP_IMG );
    } else if( !stricmp( ext, ".ico" ) ) {
        return( ICON_IMG );
    } else if( !stricmp( ext, ".cur" ) ) {
        return( CURSOR_IMG );
    } else if( !stricmp( ext, ".res" ) || !stricmp( ext, ".exe" ) ||
               !stricmp( ext, ".dll" ) ) {
        return( RESOURCE_IMG );
    } else {
        return( UNDEF_IMG );
    }

} /* getImageTypeFromFilename */

/*
 * getOpenFName - let the user select a file name for an open operation
 *              - fname must point to a buffer of length at least _MAX_PATH
 *              - also set the type of file (bitmap, icon, cursor)
 */
static BOOL getOpenFName( char *fname )
{
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    int                 rc;
    long                of_size;
#if defined( __NT__ ) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    OSVERSIONINFO       os_info;
#endif

    of_size = sizeof( OPENFILENAME );
#if defined( __NT__ ) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    os_info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os_info );
    if( os_info.dwMajorVersion < 5 ) {
        /* Set the appropriate structure size to make this work on Windows 95. */
        of_size = OPENFILENAME_SIZE_VERSION_400;
    }
#endif    
    fname[0] = '\0';
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
#if !defined( __NT__ )
    /* Important! Do not use hook in Win32, you will not get the nice dialog! */
    of.lpfnHook = (LPOFNHOOKPROC)MakeProcInstance( (FARPROC)OpenHook, Instance );
    of.Flags = OFN_ENABLEHOOK;
#endif
    of.Flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    rc = GetOpenFileName( &of );
#ifndef __NT__
    FreeProcInstance( (FARPROC)of.lpfnHook );
#endif

    if( rc ) {
        imgType = getImageTypeFromFilename( fname );
        return( TRUE );
    } else {
        return( FALSE );
    }

} /* getOpenFName */

/*
 * readInResourceFile
 */
static BOOL readInResourceFile( char *fullname )
{
    BYTE                *data;
    uint_32             dsize;
    WRInfo              *info;
    WRSelectImageInfo   *sii;
    WPI_PROC            cb;
    BOOL                ok;

    info = NULL;
    sii = NULL;
    data = NULL;
    ok = (fullname != NULL);

    if( ok ) {
        info = WRLoadResource( fullname, WR_DONT_KNOW );
        ok = (info != NULL);
    }

    if( ok ) {
        cb = _wpi_makeprocinstance( (WPI_PROC)IEHelpCallBack, Instance );
        sii = WRSelectImage( HMainWindow, info, cb );
        _wpi_freeprocinstance( cb );
        ok = (sii != NULL && sii->lnode != NULL);
    }

    if( ok ) {
        if( sii->type == (uint_16)RT_BITMAP ) {
            imgType = BITMAP_IMG;
            data = WRCopyResData( info, sii->lnode );
            dsize = sii->lnode->Info.Length;
            ok = (data != NULL);
            if( ok ) {
                ok = WRAddBitmapFileHeader( &data, &dsize );
            }
        } else if( sii->type == (uint_16)RT_GROUP_CURSOR ) {
            imgType = CURSOR_IMG;
            ok = WRCreateCursorData( info, sii->lnode, &data, &dsize );
        } else if( sii->type == (uint_16)RT_GROUP_ICON ) {
            imgType = ICON_IMG;
            ok = WRCreateIconData( info, sii->lnode, &data, &dsize );
        } else {
            imgType = UNDEF_IMG;
            ok = FALSE;
        }
    }

    if( ok ) {
        if( sii->type == (uint_16)RT_BITMAP ) {
            ok = ReadBitmapFromData( data, fullname, info, sii->lnode );
        } else if( sii->type == (uint_16)RT_GROUP_CURSOR ) {
            ok = ReadCursorFromData( data, fullname, info, sii->lnode );
        } else if( sii->type == (uint_16)RT_GROUP_ICON ) {
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
static int reallyOpenImage( char *fname )
{
    char                filename[_MAX_FNAME + _MAX_EXT];
    
    switch( imgType ) {
    case BITMAP_IMG:
        if( !readInBitmapFile( fname ) ) {
            return( FALSE );
        }
        break;
    case ICON_IMG:
        if( !readInIconFile( fname ) ) {
            return( FALSE );
        }
        break;
    case CURSOR_IMG:
        if( !readInCursorFile( fname ) ) {
            return( FALSE );
        }
        break;
    case RESOURCE_IMG:
        if( !readInResourceFile( fname ) ) {
            return( FALSE );
        }
        break;
    default:
        GetFnameFromPath( fname, filename );
        WImgEditError( WIE_ERR_BAD_FILE_EXT, filename );
        imgType = BITMAP_IMG;
        return( FALSE );
    }

    return( imgType );

} /* reallyOpenImage */

/*
 * OpenImage - get the filename of the file to open
 *           - depending on the extension, set the type (.ico, .bmp, .cur) and call the
 *             appropriate function to open it
 */
int OpenImage( HANDLE hDrop )
{
    char                fname[_MAX_PATH];
    int                 rv = FALSE;

    if( NULL == hDrop ) {
        /*
         * Not doing a drag-drop
         */
        if( !getOpenFName( &fname ) ) {
            if( CommDlgExtendedError() == FNERR_INVALIDFILENAME ) {
                WImgEditError( WIE_ERR_BAD_FILENAME, fname );
                return( FALSE );
            }
            return( FALSE );
        }
        rv = reallyOpenImage( fname );
    } else {
        /*
         * hDrop is only ever !NULL when we're dealing with a WM_DROPFILES
         * message, and that only happens with __NT__
         */
#ifdef __NT__
        int     nFiles = DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 );
        int     i;
        
        for( i = 0, rv = TRUE; rv && i < nFiles; i++ ) {
            DragQueryFile( hDrop, i, fname, _MAX_PATH - 1 );
            imgType = getImageTypeFromFilename( fname );
            rv = reallyOpenImage( fname );
        }
#endif
    }
    
    if( rv ) {
        SetupMenuAfterOpen();
    }

    return( rv );

} /* OpenImage */

/*
 * getOpenPalName - let the user select a palette file name to load
 */
static BOOL getOpenPalName( char *fname )
{
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    int                 rc;
    long                of_size;
#if defined( __NT__ ) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    OSVERSIONINFO       os_info;
#endif

    of_size = sizeof( OPENFILENAME );
#if defined( __NT__ ) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    os_info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os_info );
    if( os_info.dwMajorVersion < 5 ) {
        /* Set the appropriate structure size to make this work on Windows 95. */
        of_size = OPENFILENAME_SIZE_VERSION_400;
    }
#endif    

    fname[0] = 0;
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
    of.Flags =  OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
                OFN_HIDEREADONLY;
#if !defined( __NT__ )
    of.Flags |= OFN_ENABLEHOOK;
    of.lpfnHook = (LPOFNHOOKPROC)MakeProcInstance( (FARPROC)OpenHook, Instance );
#endif
    rc = GetOpenFileName( &of );
#ifndef __NT__
    FreeProcInstance( (FARPROC)of.lpfnHook );
#endif
    return( rc );

} /* getOpenPalName */

/*
 * LoadColorPalette - load a palette
 */
BOOL LoadColorPalette( void )
{
    char                fname[_MAX_PATH];
    char                filename[_MAX_FNAME + _MAX_EXT];
    a_pal_file          *pal_file;
    FILE                *fp;
    WORD                file_type;

    if( !getOpenPalName( &fname ) ) {
        if( CommDlgExtendedError() == FNERR_INVALIDFILENAME ) {
            WImgEditError( WIE_ERR_BAD_FILENAME, fname );
            return( FALSE );
        }
        return( TRUE );
    }

    pal_file = MemAlloc( sizeof( a_pal_file ) );
    fp = fopen( fname, "rb" );
    if( fp == NULL ) {
        WImgEditError( WIE_ERR_FILE_NOT_OPENED, fname );
        return( FALSE );
    }

    GetFnameFromPath( fname, filename );

    fseek( fp, 0L, SEEK_SET );
    fread( &file_type, sizeof( WORD ), 1, fp );
    if( file_type != PALETTE_FILE ) {
        WImgEditError( WIE_ERR_BAD_PALFILE, filename );
        fclose( fp );
        return( FALSE );
    }

    fseek( fp, 0L, SEEK_SET );
    fread( pal_file, sizeof( a_pal_file ), 1, fp );

    fclose( fp );
    SetNewPalette( pal_file );
    PrintHintTextByID( WIE_PALETTELOADEDFROM, filename );

    MemFree( pal_file );
    return( TRUE );

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
void OpenFileOnStart( char *fname )
{
    int         namelen;
    char        ext[_MAX_EXT];
    FILE        *fp;

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

    namelen = strlen( fname );
    strcpy( ext, &fname[namelen - 3] );

    if( stricmp( ext, "bmp" ) == 0 ) {
        if( !readInBitmapFile( fname ) ) {
            return;
        }
    } else if( stricmp( ext, "ico" ) == 0 ) {
        if( !readInIconFile( fname ) ) {
            return;
        }
    } else if( stricmp( ext, "cur" ) == 0 ) {
        if( !readInCursorFile( fname ) ) {
            return;
        }
    } else {
        return;
    }

    SetupMenuAfterOpen();

} /* OpenFileOnStart */
