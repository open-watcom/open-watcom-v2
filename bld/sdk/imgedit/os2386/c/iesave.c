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


#include "imgedit.h"
#include <math.h>
#include "..\h\wbitmap.h"
#include "pathgrp2.h"


#define SCANLINE_SIZE   32
#define MAX_CHUNK       32768

static char     initialDir[_MAX_PATH];

/*
 * writeDataInPieces - writes the xor data for the bitmap in chunks
 */
static bool writeDataInPieces( BITMAPINFO2 *bmi, FILE *fp, img_node *node )
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
    WPI_HBITMAP old_hbitmap;

    pres = _wpi_getpres( HWND_DESKTOP );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    old_hbitmap = _wpi_selectbitmap( mempres, node->xor_hbitmap );

    byte_count = BITS_TO_BYTES( node->bitcount * node->width, node->height );
    start = 0;
    num_lines = SCANLINE_SIZE;
    one_scanline_size = BITS_TO_BYTES( node->width*node->bitcount, 1 );
    scanline_count = node->height;
    chunk_size = one_scanline_size * num_lines;
    while( chunk_size > MAX_CHUNK ) {
        chunk_size >>= 1;
        num_lines = chunk_size / one_scanline_size;
    }

    buffer = calloc( chunk_size, sizeof( BYTE ) );
    while( scanline_count > num_lines ) {
        GpiQueryBitmapBits( mempres, start, num_lines, buffer, bmi );
        fwrite( buffer, sizeof( BYTE ), chunk_size, fp );
        scanline_count -= num_lines;
        start += num_lines;
        byte_count -= chunk_size;
    }
    GpiQueryBitmapBits( mempres, start, scanline_count, buffer, bmi );
    fwrite( buffer, sizeof( BYTE ), one_scanline_size * scanline_count, fp );
    free( buffer );
    _wpi_selectbitmap( mempres, old_hbitmap );
    _wpi_deletecompatiblepres( mempres, memdc );
    return( true );
} /* writeDataInPieces */

/*
 * writeImageBits - writes the bits for the image
 */
static bool writeImageBits( FILE *fp, img_node *node )
{
    WPI_PRES                    pres;
    WPI_PRES                    mempres;
    HDC                         memdc;
    ULONG                       byte_count;
    img_node                    *new_image;
    BITMAPINFO2                 *bmi;
    WPI_HBITMAP                 old_hbitmap;
    WPI_HBITMAP                 inverse_hbitmap;
    WPI_HBITMAP                 clr_hbitmap;
    BYTE                        *buffer;
    bool                        ok;

    ok = true;
    pres = _wpi_getpres( HWND_DESKTOP );
    mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    for( new_image = node; new_image != NULL; new_image = new_image->nexticon ) {
        bmi = GetAndBitmapInfo( new_image );
        if( bmi == NULL ) {
            ok = false;
            break;
        }
        /*
         * first we write the PM XOR mask (inverse mask) then the PM AND
         * mask (and mask) and then the PM colour mask (xor mask).
         */
        byte_count = BITS_TO_BYTES( new_image->width, new_image->height );
        buffer = MemAlloc( byte_count );

        inverse_hbitmap = CreateInverseBitmap( new_image->and_hbitmap, new_image->xor_hbitmap );
        old_hbitmap = _wpi_selectbitmap( mempres, inverse_hbitmap );
        GpiQueryBitmapBits( mempres, 0, new_image->height, buffer, bmi );
        fwrite( buffer, sizeof( BYTE ), byte_count, fp );
        _wpi_selectbitmap( mempres, old_hbitmap );
        _wpi_deletebitmap( inverse_hbitmap );

        old_hbitmap = _wpi_selectbitmap( mempres, new_image->and_hbitmap );
        GpiQueryBitmapBits( mempres, 0, new_image->height, buffer, bmi );
        fwrite( buffer, sizeof( BYTE ), byte_count, fp );
        _wpi_selectbitmap( mempres, old_hbitmap );

        MemFree( buffer );
        FreeDIBitmapInfo( bmi );

        bmi = GetXorBitmapInfo( new_image );
        if( bmi == NULL ) {
            ok = false;
            break;
        }
        clr_hbitmap = CreateColourBitmap( new_image->and_hbitmap, new_image->xor_hbitmap );
        old_hbitmap = _wpi_selectbitmap( mempres, clr_hbitmap );
        byte_count = BITS_TO_BYTES( new_image->width * new_image->bitcount, new_image->height );
        buffer = MemAlloc( byte_count );
        GpiQueryBitmapBits( mempres, 0, node->height, buffer, bmi );
        fwrite( buffer, sizeof( BYTE ), byte_count, fp );
        MemFree( buffer );
        FreeDIBitmapInfo( bmi );
        _wpi_selectbitmap( mempres, old_hbitmap );
        _wpi_deletebitmap( clr_hbitmap );
    }
    _wpi_deletecompatiblepres( mempres, memdc );
    return( ok );
} /* writeImageBits */

/*
 * fillFileHeader - fills the bitmap file header for the given node
 */
static BITMAPFILEHEADER2 *fillFileHeader( img_node *node )
{
    BITMAPFILEHEADER2   *fileheader;

    fileheader = MemAlloc( sizeof( BITMAPFILEHEADER2 ) );
    memset( fileheader, 0, sizeof( BITMAPFILEHEADER2 ) );

    if( node->imgtype == ICON_IMG ) {
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
 * initSaveFileInfo
 */
static bool initSaveFileInfo( char *fullfile, image_type img_type )
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

} /* initSaveFileInfo */

/*
 * updateSaveFileInfo
 */
static bool updateSaveFileInfo( const char *fname )
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
    return( true );

} /* updateSaveFileInfo */

/*
 * getSaveFName - Get the name of the file to be saved.
 */
static bool getSaveFName( char *fname, image_type img_type )
{
    FILEDLG             filedlg;
    bool                ok;
    char                fullfile[CCHMAXPATH];

    fname[0] = '\0';

    initSaveFileInfo( fullfile, img_type );

    /*
     * set the values of the filedlg structure ...
     */
    memset( &filedlg, 0, sizeof( FILEDLG ) );
    filedlg.cbSize = sizeof( FILEDLG );
    filedlg.fl = FDS_SAVEAS_DIALOG | FDS_CENTER;
    filedlg.pszTitle = "Save Image File";
    filedlg.pszOKButton = "Save";
    strcpy( filedlg.szFullFile, fullfile );

    ok = ( WinFileDlg( HWND_DESKTOP, HMainWindow, &filedlg ) != NULLHANDLE && filedlg.lReturn == DID_OK );

    if( ok ) {
        strcpy( fname, filedlg.szFullFile );
        ok = updateSaveFileInfo( fname );
    }
    return( ok );

} /* getSaveFName */

/*
 * saveBitmapFile - gets the bitmap data and saves it in fname.
 */
static bool saveBitmapFile( img_node *node )
{
    BITMAPARRAYFILEHEADER2      new_file;
    BITMAPINFO2                 *bmi;
    long                        clrtable_size;
    RGB2                        *colours;
    FILE                        *fp;
    char                        text[HINT_TEXT_LEN];
    char                        filename[_MAX_FNAME];
    bool                        ok;

    ok = false;
    bmi = GetXorBitmapInfo(node);
    if( bmi != NULL ) {
        clrtable_size = sizeof( RGB2 ) * ( 1 << ( node->bitcount ) );

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
        memcpy( &(new_file.bfh2.bmp2), bmi, sizeof( BITMAPINFOHEADER2 ) );

        colours = (void *)&(bmi->argbColor[0]);

        fp = fopen( node->fname, "wb" );
        if( fp != NULL ) {
            if( fseek( fp, 0L, SEEK_SET ) == 0 ) {
                if( fwrite( &new_file, sizeof( BITMAPARRAYFILEHEADER2 ), 1, fp ) == 1 ) {
                    if( fwrite( colours, clrtable_size, 1, fp ) == 1 ) {
                        if( writeDataInPieces( bmi, fp, node ) ) {
                            ok = true;
                        } else {
                            MessageBox( HMainWindow, "Error writing file!", "Error", MB_OK | MB_ICONEXCLAMATION );
                            SetHintText( "Error saving file" );
                        }
                    }
                }
            }
            fclose( fp );
        }
        FreeDIBitmapInfo( bmi );
        if( ok ) {
            AllowRestoreOption( node );
            SetIsSaved( node->hwnd, true );
            GetFnameFromPath( node->fname, filename );
            sprintf( text, "Bitmap saved to '%s'", filename );
            SetHintText( text );
        }
    }
    return( ok );
} /* saveBitmapFile */

/*
 * saveImageFile - saves a cursor or icon file
 */
static bool saveImageFile( img_node *node )
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
    char                        text[HINT_TEXT_LEN];
    char                        filename[_MAX_FNAME];
    img_node                    *new_image;
    bool                        ok;

    ok = false;
    fp = fopen( node->fname, "wb" );
    if( fp != NULL ) {
        if( fseek( fp, 0L, SEEK_SET ) == 0 ) {
            ok = true;
            nextoff = 0;
            for( new_image = node; new_image != NULL; new_image = new_image->nexticon ) {
                new_file.usType = BFT_BITMAPARRAY;
                new_file.cbSize = sizeof( BITMAPARRAYFILEHEADER2 );
                new_file.offNext = nextoff;
                new_file.cxDisplay = 0;
                new_file.cyDisplay = 0;

                and_part = fillFileHeader( new_image );
                memcpy( &(new_file.bfh2), and_part, sizeof( BITMAPFILEHEADER2 ) );
                /*
                 * First the info for the AND mask
                 */
                andbmi = GetAndBitmapInfo( new_image );
                if( andbmi == NULL ) {
                    ok = false;
                } else {
                    clrtable_size = sizeof( RGB2 ) * ( 1 << new_image->bitcount );
                    and_size = sizeof( RGB2 ) * 2;

                    new_file.bfh2.offBits = new_file.offNext + new_file.cbSize +
                                            and_size + clrtable_size +
                                            sizeof( BITMAPFILEHEADER2 );
                    memcpy( &(new_file.bfh2.bmp2), andbmi, sizeof( BITMAPINFOHEADER2 ) );
                    colours = (void *)&(andbmi->argbColor[0]);

                    if( fwrite( &new_file, sizeof( BITMAPARRAYFILEHEADER2 ), 1, fp ) != 1 ) {
                        ok = false;
                    } else if( fwrite( colours, and_size, 1, fp ) != 1 ) {
                        ok = false;
                    }
                    FreeDIBitmapInfo( andbmi );
                }
                MemFree( and_part );

                if( ok ) {
                    /*
                     * Now we write the XOR part
                     */
                    xor_part = fillFileHeader( new_image );
                    xorbmi = GetXorBitmapInfo( new_image );
                    if( xorbmi == NULL ) {
                        ok = false;
                    } else {
                        xor_part->offBits = new_file.bfh2.offBits + BITS_TO_BYTES( new_image->width, 2 * new_image->height );
                        memcpy( &(xor_part->bmp2), xorbmi, sizeof( BITMAPINFOHEADER2 ) );
                        colours = (void *)&(xorbmi->argbColor[0]);
                        if( fwrite( xor_part, sizeof( BITMAPFILEHEADER2 ), 1, fp ) != 1 ) {
                            ok = false;
                        } else if( fwrite( colours, clrtable_size, 1, fp ) != 1 ) {
                            ok = false;
                        }
                        FreeDIBitmapInfo( xorbmi );
                    }
                    MemFree( xor_part );
                    nextoff = nextoff + sizeof( BITMAPARRAYFILEHEADER2 ) + and_size + sizeof( BITMAPFILEHEADER2 ) + clrtable_size;
                }
                if( !rc ) {
                    break;
                }
            }
        }
        fclose( fp );
    }
    if( ok ) {
        /*
         * Now we write the bits for all the images in the file.
         */
        if( !writeImageBits( fp, node ) ) {
            return( false );
        }
        AllowRestoreOption( node );
        SetIsSaved( node->hwnd, true );
        GetFnameFromPath( node->fname, filename );
        if( node->imgtype == ICON_IMG ) {
            sprintf( text, "Icon saved to '%s'", filename );
        } else {
            sprintf( text, "Pointer saved to '%s'", filename );
        }
        SetHintText( text );
    }
    return( ok );
} /* saveImageFile */

/*
 * SaveFile - Saves the image file (bitmap, cursor or icon).
 */
bool SaveFile( short how )
{
    img_node    *node;
    img_node    *rootnode;
    char        new_name[_MAX_PATH];
    bool        ok;

    node = GetCurrentNode();
    ok = ( node != NULL )
    if( ok ) {
        rootnode = GetImageNode( node->hwnd );
        ok = ( rootnode != NULL )
    }
    if( ok ) {
        if( strnicmp( rootnode->fname, "(Untitled)", 10 ) == 0 ) {
            how = SB_SAVE_AS;
        }
        if( how == SB_SAVE_AS ) {
            ok = getSaveFName( new_name, rootnode->imgtype );
            if( ok ) {
                for( node = rootnode; node != NULL; node = node->nexticon ) {
                    strcpy( node->fname, new_name );
                }
            }
        }
    }
    if( ok ) {
        CheckForExt( rootnode );

        switch( rootnode->imgtype ) {
        case BITMAP_IMG:
            ok = saveBitmapFile( rootnode );
            break;
        case ICON_IMG:
        case CURSOR_IMG:
            ok = saveImageFile( rootnode );
            break;
        }
        if( !ok ) {
            MessageBox( HMainWindow, "Error trying to save file!", "Error", MB_OK | MB_ICONEXCLAMATION );
        }
    }
    return( ok );

} /* SaveFile */

#if 0
/*
 * getSavePalName - Get the name of the palette file to be saved.
 */
static bool getSavePalName( char *fname )
{
    static char         filterList[] = "Palette (*.pal)\0*.pal\0" \
                                        "All Files (*.*)\0*.*\0" \
                                        "\0";
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    bool                ok;

    fname[0] = '\0';

    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = HMainWindow;
    of.lpstrFilter = (LPSTR)filterList;
    of.nFilterIndex = 0L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrFileTitle = szFileTitle;
    of.nMaxFileTitle = sizeof( szFileTitle );
    of.lpstrTitle = "Save Colour Palette File";
    of.lpstrInitialDir = initialDir;
    of.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
    of.lpfnHook = MakeProcInstance_OFNHOOK( SaveOFNHookProc, Instance );
    ok = ( GetSaveFileName( &of ) != 0 );
    FreeProcInstance_OFNHOOK( of.lpfnHook );
    return( ok );

} /* getSavePalName */

/*
 * SaveColourPalette - saves the current colour palette.
 */
bool SaveColourPalette( void )
{
    a_pal_file          pal_file;
    FILE                *fp;
    char                fname[_MAX_PATH];
    char                filename[_MAX_FNAME + _MAX_EXT];
    char                text[HINT_TEXT_LEN];
    bool                ok;

    if( !getSavePalName( fname ) ) {
        if( CommDlgExtendedError() != 0 ) {
            sprintf( text, "Error saving '%s'", fname );
            SetHintText( text );
            return( false );
        }
        return( true );
    }
    CheckForPalExt( fname );
    if( !GetPaletteFile( &pal_file ) ) {
        sprintf( text, "Current palette not 16 colours!", fname );
        SetHintText( text );
        return( true );                   // Just return ... no error
    }

    ok = false;
    sprintf( text, "Error saving '%s'", fname );
    fp = fopen( fname, "wb" );
    if( fp != NULL ) {
        if( fseek( fp, 0L, SEEK_SET ) == 0 ) {
            if( fwrite( &pal_file, sizeof( a_pal_file ), 1, fp ) == 1 ) {
                ok = true;
            }
        }
        fclose( fp );
    }
    if( ok ) {
        GetFnameFromPath( fname, filename );
        sprintf( text, "Palette saved to '%s'", filename );
    }
    SetHintText( text );
    return( ok );

} /* SaveColourPalette */
#endif

/*
 * SetInitialSaveDir - sets the initial directory for the open filename
 */
void SetInitialSaveDir( char *new_dir )
{
    if( new_dir != NULL ) {
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
    return( initialDir );
} /* GetInitSaveDir */

