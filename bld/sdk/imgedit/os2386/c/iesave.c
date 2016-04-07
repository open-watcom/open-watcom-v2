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


#include "imgedit.h"
#include <malloc.h>
#include <math.h>
#include "..\h\wbitmap.h"


#define SCANLINE_SIZE   32
#define MAX_CHUNK       32768

static char     initialDir[ _MAX_PATH ];

/*
 * writeDataInPieces - writes the xor data for the bitmap in chunks
 */
static BOOL writeDataInPieces( BITMAPINFO2 *bmi, FILE *fp, img_node *node )
{
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HDC         memdc;
    int         scanline_count;
    int         one_scanline_size;
    long        chunk_size;
    int         start;
    int         num_lines;
    long        byte_count;
    BYTE        *buffer;
    HBITMAP     oldbitmap;

    pres = _wpi_getpres( HWND_DESKTOP );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    oldbitmap = _wpi_selectobject( mempres, node->hxorbitmap );

    byte_count = BITS_TO_BYTES( node->bitcount * node->width, node->height );
    start = 0;
    num_lines = SCANLINE_SIZE;
    one_scanline_size = BITS_TO_BYTES( node->width*node->bitcount, 1 );
    scanline_count = node->height;
    chunk_size = one_scanline_size * num_lines;
    while ( chunk_size > MAX_CHUNK ) {
        chunk_size >>= 1;
        num_lines = chunk_size / one_scanline_size;
    }

    buffer = calloc( chunk_size, sizeof(BYTE) );
    while (scanline_count > num_lines) {
        GpiQueryBitmapBits( mempres, start, num_lines, buffer, bmi );
        fwrite( buffer, sizeof(BYTE), chunk_size, fp );
        scanline_count -= num_lines;
        start += num_lines;
        byte_count -= chunk_size;
    }
    GpiQueryBitmapBits( mempres, start, scanline_count, buffer, bmi );
    fwrite( buffer, sizeof(BYTE), (one_scanline_size*scanline_count), fp );
    free( buffer );
    _wpi_selectobject( mempres, oldbitmap );
    _wpi_deletecompatiblepres( mempres, memdc );
    return(TRUE);
} /* writeDataInPieces */

/*
 * writeImageBits - writes the bits for the image
 */
static BOOL writeImageBits( FILE *fp, img_node *node )
{
    WPI_PRES                    pres;
    WPI_PRES                    mempres;
    HDC                         memdc;
    ULONG                       byte_count;
    img_node                    *new_image;
    BITMAPINFO2                 *bmi;
    HBITMAP                     oldbitmap;
    HBITMAP                     inverse_bitmap;
    HBITMAP                     clr_bitmap;
    BYTE                        *buffer;

    pres = _wpi_getpres( HWND_DESKTOP );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    new_image = node;
    while( new_image ) {
        bmi = GetAndBitmapInfo(new_image);
        if (!bmi) {
            return(FALSE);
        }
        /*
         * first we write the PM XOR mask (inverse mask) then the PM AND
         * mask (and mask) and then the PM colour mask (xor mask).
         */
        byte_count = BITS_TO_BYTES( new_image->width, new_image->height );
        buffer = MemAlloc( byte_count );

        inverse_bitmap = CreateInverseBitmap( new_image->handbitmap,
                                                new_image->hxorbitmap );
        oldbitmap = _wpi_selectobject( mempres, inverse_bitmap );
        GpiQueryBitmapBits( mempres, 0, new_image->height, buffer, bmi );
        fwrite( buffer, sizeof(BYTE), byte_count, fp );
        _wpi_selectobject( mempres, oldbitmap );
        _wpi_deletebitmap( inverse_bitmap );

        oldbitmap = _wpi_selectobject( mempres, new_image->handbitmap );
        GpiQueryBitmapBits( mempres, 0, new_image->height, buffer, bmi );
        fwrite( buffer, sizeof(BYTE), byte_count, fp );
        _wpi_selectobject( mempres, oldbitmap );

        free( buffer );
        FreeDIBitmapInfo( bmi );

        bmi = GetXorBitmapInfo( new_image );
        if (!bmi) {
            return(FALSE);
        }
        clr_bitmap = CreateColourBitmap( new_image->handbitmap,
                                                    new_image->hxorbitmap );
        oldbitmap = _wpi_selectobject( mempres, clr_bitmap );
        byte_count = BITS_TO_BYTES( new_image->width * new_image->bitcount,
                                                        new_image->height );
        buffer = MemAlloc( byte_count );
        GpiQueryBitmapBits( mempres, 0, node->height, buffer, bmi );
        fwrite( buffer, sizeof(BYTE), byte_count, fp );
        free( buffer );
        FreeDIBitmapInfo( bmi );
        _wpi_selectobject( mempres, oldbitmap );
        _wpi_deletebitmap( clr_bitmap );

        new_image = new_image->nexticon;
    }
    _wpi_deletecompatiblepres( mempres, memdc );
    return( TRUE );
} /* writeImageBits */

/*
 * fillFileHeader - fills the bitmap file header for the given node
 */
static BITMAPFILEHEADER2 *fillFileHeader( img_node *node )
{
    BITMAPFILEHEADER2   *fileheader;

    fileheader = MemAlloc( sizeof(BITMAPFILEHEADER2) );
    memset( fileheader, 0, sizeof(BITMAPFILEHEADER2) );

    if (node->imgtype == ICON_IMG) {
        fileheader->usType = BFT_COLORICON;
    } else {
        fileheader->usType = BFT_COLORPOINTER;
    }
    fileheader->cbSize = sizeof( BITMAPFILEHEADER2 );
    fileheader->xHotspot = node->hotspot.x;
    fileheader->yHotspot = node->hotspot.y;

    return( fileheader );
} /* fillFileHeader */

/*
 * checkForExt - if no extension is given, use the default for the given
 *               type.
 */
static void checkForExt( img_node *node )
{
    char        drive[ _MAX_PATH ];
    char        dir[ _MAX_DIR ];
    char        fname[ _MAX_FNAME ];
    char        ext[ _MAX_EXT ];
    char        *fullpath;
    img_node    *next_icon;
    char        default_ext[3][4] = {
                                "bmp",
                                "ico",
                                "cur" };

    next_icon = node;
    while (next_icon) {
        fullpath = next_icon->fname;
        _splitpath( fullpath, drive, dir, fname, ext );

        if ( strlen(ext) > 1 ) {
            return;
        }

        if ( fullpath[strlen(fullpath) - 1] != '.' ) {
            strcat( fullpath, "." );
        }
        strcat( fullpath, default_ext[ next_icon->imgtype-1 ] );
        next_icon = next_icon->nexticon;
    }
} /* checkForExt */

#if 0
/*
 * checkForPalExt - if no extension is given, use the default palette
 *              extension of .pal.
 */
static void checkForPalExt( char *filename )
{
    char        ext[ _MAX_EXT ];

    _splitpath( filename, NULL, NULL, NULL, ext );

    if ( strlen(ext) > 1 ) {
        return;
    }

    if ( filename[strlen(filename) - 1] != '.' ) {
        strcat( filename, "." );
    }
    strcat( filename, "pal" );
} /* checkForPalExt */
#endif

/*
 * getSaveFName - Get the name of the file to be saved.
 */
static BOOL getSaveFName( char *fname, int imgtype )
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
    if ( imgtype == BITMAP_IMG ) {
        strcat( fullfile, "*.bmp" );
    } else if (imgtype == ICON_IMG) {
        strcat( fullfile, "*.ico" );
    } else {
        strcat( fullfile, "*.ptr" );
    }

    /*
     * set the values of the filedlg structure ...
     */
    filedlg.cbSize = sizeof( FILEDLG );
    filedlg.fl = FDS_SAVEAS_DIALOG | FDS_CENTER;
    filedlg.pszTitle = "Save Image File";
    filedlg.pszOKButton = "Save";
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
    return(TRUE);
} /* getSaveFName */

/*
 * saveBitmapFile - gets the bitmap data and saves it in fname.
 */
static BOOL saveBitmapFile( img_node *node )
{
    BITMAPARRAYFILEHEADER2      new_file;
    BITMAPINFO2                 *bmi;
    long                        clrtable_size;
    RGB2                        *colours;
    FILE                        *fp;
    char                        text[ HINT_TEXT_LEN ];
    char                        filename[ _MAX_FNAME ];

    bmi = GetXorBitmapInfo(node);

    if (!bmi) {
        return(FALSE);
    }
    clrtable_size = sizeof(RGB2) * (1<<(node->bitcount));

    new_file.usType = BFT_BITMAPARRAY;
    new_file.cbSize = sizeof( BITMAPARRAYFILEHEADER2 );
    new_file.offNext = 0;
    new_file.cxDisplay = 0;
    new_file.cyDisplay = 0;

    new_file.bfh2.usType = BFT_BMAP;
    new_file.bfh2.cbSize = sizeof( BITMAPFILEHEADER2 );
    new_file.bfh2.xHotspot = 0;
    new_file.bfh2.yHotspot = 0;
    new_file.bfh2.offBits = new_file.cbSize + clrtable_size;
    memcpy( &(new_file.bfh2.bmp2), bmi, sizeof(BITMAPINFOHEADER2) );

    colours = (void *)&(bmi->argbColor[0]);

    fp = fopen( node->fname, "wb" );
    if (fp == NULL) {
        FreeDIBitmapInfo( bmi );
        return(FALSE);
    }

    if (fseek( fp, 0L, SEEK_SET )) {
        fclose( fp );
        FreeDIBitmapInfo( bmi );
        return(FALSE);
    }

    if ( fwrite(&new_file, sizeof(BITMAPARRAYFILEHEADER2), 1, fp) != 1 ) {
        fclose( fp );
        FreeDIBitmapInfo( bmi );
        return(FALSE);
    }

    if ( fwrite(colours, clrtable_size, 1, fp) != 1 ) {
        fclose( fp );
        FreeDIBitmapInfo( bmi );
        return(FALSE);
    }

    if (!writeDataInPieces( bmi, fp, node )) {
        fclose( fp );
        FreeDIBitmapInfo( bmi );
        MessageBox(HMainWindow, "Error writing file!", "Error",
                                                MB_OK | MB_ICONEXCLAMATION);
        SetHintText("Error saving file");
        return(FALSE);
    }

    fclose( fp );
    FreeDIBitmapInfo( bmi );

    AllowRestoreOption( node );
    SetIsSaved( node->hwnd, TRUE );
    GetFnameFromPath( node->fname, filename );
    sprintf( text, "Bitmap saved to '%s'", filename );
    SetHintText( text );
    return( TRUE );
} /* saveBitmapFile */

/*
 * saveImageFile - saves a cursor or icon file
 */
static BOOL saveImageFile( img_node *node )
{
    BITMAPARRAYFILEHEADER2      new_file;
    BITMAPFILEHEADER2           *and_part;
    BITMAPFILEHEADER2           *xor_part;
    BITMAPINFO2                 *andbmi;
    BITMAPINFO2                 *xorbmi;
    long                        clrtable_size;
    long                        and_size;
    ULONG                       nextoff;
    RGB2                        *colours;
    FILE                        *fp;
    char                        text[ HINT_TEXT_LEN ];
    char                        filename[ _MAX_FNAME ];
    img_node                    *new_image;

    fp = fopen( node->fname, "wb" );
    if (fp == NULL) {
        return(FALSE);
    }
    if (fseek( fp, 0L, SEEK_SET )) {
        fclose( fp );
        return(FALSE);
    }

    new_image = node;
    nextoff = 0;

    while( new_image ) {
        new_file.usType = BFT_BITMAPARRAY;
        new_file.cbSize = sizeof( BITMAPARRAYFILEHEADER2 );
        new_file.offNext = nextoff;
        new_file.cxDisplay = 0;
        new_file.cyDisplay = 0;

        and_part = fillFileHeader( new_image );
        memcpy( &(new_file.bfh2), and_part, sizeof(BITMAPFILEHEADER2) );
        /*
         * First the info for the AND mask
         */
        andbmi = GetAndBitmapInfo(new_image);
        if (!andbmi) {
            return(FALSE);
        }
        clrtable_size = sizeof(RGB2) * (1<<(new_image->bitcount));
        and_size = sizeof(RGB2) * 2;

        new_file.bfh2.offBits = new_file.offNext + new_file.cbSize +
                                and_size + clrtable_size +
                                sizeof(BITMAPFILEHEADER2);
        memcpy( &(new_file.bfh2.bmp2), andbmi, sizeof(BITMAPINFOHEADER2) );
        colours = (void *)&(andbmi->argbColor[0]);

        if ( fwrite(&new_file, sizeof(BITMAPARRAYFILEHEADER2), 1, fp) != 1 ) {
            fclose( fp );
            FreeDIBitmapInfo( andbmi );
            free( and_part );
            return(FALSE);
        }

        if ( fwrite(colours, and_size, 1, fp) != 1 ) {
            fclose( fp );
            FreeDIBitmapInfo( andbmi );
            free( and_part );
            return(FALSE);
        }
        free( and_part );

        /*
         * Now we write the XOR part
         */
        xor_part = fillFileHeader( new_image );
        xorbmi = GetXorBitmapInfo(new_image);
        if (!xorbmi) {
            free( xor_part );
            return(FALSE);
        }
        xor_part->offBits = new_file.bfh2.offBits +
                        BITS_TO_BYTES(new_image->width, 2*new_image->height);
        memcpy( &(xor_part->bmp2), xorbmi, sizeof(BITMAPINFOHEADER2) );
        colours = (void *)&(xorbmi->argbColor[0]);

        if ( fwrite(xor_part, sizeof(BITMAPFILEHEADER2), 1, fp) != 1 ) {
            fclose( fp );
            FreeDIBitmapInfo( xorbmi );
            free( xor_part );
            return(FALSE);
        }

        if ( fwrite(colours, clrtable_size, 1, fp) != 1 ) {
            fclose( fp );
            FreeDIBitmapInfo( xorbmi );
            free( xor_part );
            return(FALSE);
        }
        free( xor_part );
        nextoff = nextoff + sizeof(BITMAPARRAYFILEHEADER2) + and_size
                        + sizeof(BITMAPFILEHEADER2) + clrtable_size;
        new_image = new_image->nexticon;
    }

    /*
     * Now we write the bits for all the images in the file.
     */
    if (!writeImageBits( fp, node )) {
        fclose( fp );
        return( FALSE );
    }

    fclose( fp );

    AllowRestoreOption( node );
    SetIsSaved( node->hwnd, TRUE );
    GetFnameFromPath( node->fname, filename );
    if (node->imgtype == ICON_IMG) {
        sprintf( text, "Icon saved to '%s'", filename );
    } else {
        sprintf( text, "Pointer saved to '%s'", filename );
    }
    SetHintText( text );
    return( TRUE );
} /* saveImageFile */

/*
 * SaveFile - Saves the image file (bitmap, cursor or icon).
 */
BOOL SaveFile( short how )
{
    img_node    *node;
    img_node    *rootnode;
    char        new_name[ _MAX_PATH ];

    node = GetCurrentNode();
    if (!node) return(FALSE);
    rootnode = GetImageNode( node->hwnd );

    if (!rootnode) return(FALSE);

    if (strnicmp(rootnode->fname, "(Untitled)", 10) == 0) {
        how = SB_SAVE_AS;
    }

    if (how == SB_SAVE_AS) {
        if (!getSaveFName(new_name, rootnode->imgtype)) {
            return (FALSE);
        }
        node = rootnode;
        while (node) {
            strcpy( node->fname, new_name );
            node = node->nexticon;
        }
    }

    checkForExt( rootnode );

    switch( rootnode->imgtype ) {
    case BITMAP_IMG:
        if (!saveBitmapFile( rootnode )) {
            MessageBox(HMainWindow, "Error trying to save file!", "Error",
                                                MB_OK | MB_ICONEXCLAMATION);
            return(FALSE);
        }
        break;
    case ICON_IMG:
    case CURSOR_IMG:
        if (!saveImageFile( rootnode )) {
            MessageBox(HMainWindow, "Error trying to save file!", "Error",
                                                MB_OK | MB_ICONEXCLAMATION);
            return(FALSE);
        }
        break;

    default:
        return (FALSE);
    }
    return(TRUE);
} /* SaveFile */

#if 0
/*
 * getSavePalName - Get the name of the palette file to be saved.
 */
static BOOL getSavePalName( char *fname )
{
    static char         filterList[] = "Palette (*.pal)" \
                                        "\0" \
                                        "*.pal" \
                                        "\0" \
                                        "All Files (*.*)" \
                                        "\0" \
                                        "*.*" \
                                        "\0\0";
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    int                 rc;

    fname[ 0 ] = 0;
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = HMainWindow;
    of.lpstrFilter = (LPSTR) filterList;
    of.nFilterIndex = 0L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrFileTitle = szFileTitle;
    of.nMaxFileTitle = sizeof(szFileTitle);
    of.lpstrTitle = "Save Colour Palette File";
    of.lpstrInitialDir = initialDir;
    of.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
    of.lpfnHook = (LPOFNHOOKPROC)MakeProcInstance( (FARPROC)SaveHook, Instance );
    rc = GetSaveFileName( &of );
    FreeProcInstance( (FARPROC)of.lpfnHook );
    return( rc );

} /* getSavePalName */

/*
 * SaveColourPalette - saves the current colour palette.
 */
BOOL SaveColourPalette( void )
{
    a_pal_file          pal_file;
    FILE                *fp;
    char                fname[ _MAX_PATH ];
    char                filename[ _MAX_FNAME + _MAX_EXT ];
    char                text[ HINT_TEXT_LEN ];

    if ( !getSavePalName(fname) ) {
        if ( CommDlgExtendedError() != 0 ) {
            sprintf( text, "Error saving '%s'", fname );
            SetHintText( text );
            return (FALSE);
        } else {
            return (TRUE);
        }
    }
    checkForPalExt(fname);

    if ( !GetPaletteFile(&pal_file) ) {
        sprintf( text, "Current palette not 16 colours!", fname );
        SetHintText( text );
        return(TRUE);                   // Just return ... no error
    }

    sprintf( text, "Error saving '%s'", fname );
    fp = fopen( fname, "wb" );
    if (fp == NULL) {
        SetHintText( text );
        return(FALSE);
    }

    if (fseek( fp, 0L, SEEK_SET )) {
        SetHintText( text );
        return(FALSE);
    }

    if ( fwrite(&pal_file, sizeof(a_pal_file), 1, fp) != 1 ) {
        SetHintText( text );
        return(FALSE);
    }

    fclose( fp );
    GetFnameFromPath( fname, filename );
    sprintf( text, "Palette saved to '%s'", filename );
    SetHintText( text );
    return(TRUE);

} /* SaveColourPalette */
#endif

/*
 * SetInitialSaveDir - sets the initial directory for the open filename
 */
void SetInitialSaveDir( char *new_dir )
{
    if (new_dir) {
        strcpy( initialDir, new_dir );
    } else {
        strcpy( initialDir, "" );
    }
} /* SetInitialSaveDir */

/*
 * GetInitSaveDir - gets the directory which we want to use as our initial one
 *              next time we run.
 */
char *GetInitSaveDir( void )
{
    return(initialDir);
} /* GetInitSaveDir */

