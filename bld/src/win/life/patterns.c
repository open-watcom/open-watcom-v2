#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <malloc.h>
#include <sys/stat.h>
#include "life.h"

static HBITMAP          MenuBitMap;
static HMENU            PatternMenu;
static HDC              MenuDC;
static BITMAP           MenuBitInfo;
static HBITMAP          *MenuPatterns;
static char             **Patterns;
static char Pixie[] = { 1, 1, 1 };
static char *Cursor = Pixie;



extern BOOL ReadAPatternFile( char *name, int i )
/************************************************

    Read the pattern found in file "name", into pattern position "i"
*/
{
    FILE        *io;
    int         ch;
    char        *array;
    char        *pattern;
    int         xdim,ydim;
    struct stat status;

    Patterns = realloc( Patterns, (i+1)*sizeof( Patterns[0] ) );
    if( Patterns == NULL ) return( NoMemory() );
    MenuPatterns = realloc( MenuPatterns, (i+1)* sizeof( MenuPatterns[0] ) );
    if( MenuPatterns == NULL ) return( NoMemory() );
    stat( name, &status );
    if( status.st_size == 0 ) return( FALSE );
    pattern = malloc( status.st_size );
    if( pattern == NULL ) return( NoMemory() );
    Patterns[i] = pattern;
    io = fopen( name, "r" );
    if( io == NULL ) return( FALSE );
    array = pattern+2;
    xdim = 0;
    ydim = 0;
    for( ;; ) {
        ch = fgetc( io );
        if( ch == EOF ) break;
        if( ch == '\n' ) {
            ++ydim;
            pattern[0] = xdim;
            xdim = 0;
        } else {
            ++xdim;
            if( ch == ' ' || ch == '_' ) {
                *array = FALSE;
            } else {
                *array = TRUE;
            }
            ++array;
        }
    }
    fclose( io );
    pattern[1] = ydim;
    if( pattern[0] == 0 || ydim == 0 ) {
        free( Patterns[i] );
        return( FALSE );
    }
    return( TRUE );
}


extern BOOL ReadPatterns( void )
/*******************************

    Read in all the pattern files from disk.
    (*.LIF in the same directory LIFE.EXE)
*/
{
    char        filename[_MAX_PATH];
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    DIR         *dir_handle;
    struct dirent *dir_entry;
    int         i;

    GetModuleFileName( ThisInst, filename, _MAX_PATH );
    _splitpath( filename, drive, dir, NULL, NULL );
    _makepath( filename, drive, dir, "*", ".LIF" );
    dir_handle = opendir( filename );
    Patterns = malloc( sizeof( Patterns[0] ) );
    Patterns[0] = &Pixie;
    MenuPatterns = malloc( 1 * sizeof( MenuPatterns[0] ) );
    if( MenuPatterns == NULL ) return( NoMemory() );
    i = 1;
    if( dir_handle != NULL ) {
        while( dir_entry = readdir( dir_handle ) ) {
            _makepath( filename, drive, dir, dir_entry->d_name, NULL );
            if( !ReadAPatternFile( filename, i ) ) {
                Error( "Error reading file" );
                return( FALSE );
            }
            ++i;
        }
    }
    NumberPatterns = i;
    return( TRUE );
}


extern void FreePatterns( void )
/*******************************

    Free up the patterns array, and corresponding bit maps
*/
{
    int         i;

    if( MenuBitMap != (HBITMAP)0 )
        DeleteObject( MenuBitMap );
    for( i = 1; i < NumberPatterns; ++i ) {
        free( Patterns[i] );
    }
    free( Patterns );
}


extern void ReflectAboutXequalY( char *pattern )
/**********************************************

    Reflect an array "pattern" about the line x=y.
*/
{
    unsigned    x_dim,y_dim;
    unsigned    x,y;
    char        *new;

    x_dim = *pattern++;
    y_dim = *pattern++;
    new = alloca( x_dim * y_dim + 2 );
    *new++ = y_dim;
    *new++ = x_dim;
    for( x = 0; x < x_dim; x++ ) {
        for( y = 0; y < y_dim; y++ ) {
            new[ x*y_dim + y ] = pattern[ y*x_dim + x ];
        }
    }
    new -= 2;
    pattern -= 2;
    for( x = 0; x < x_dim*y_dim + 2; ++x ) {
        pattern[ x ] = new[ x ];
    }
}


extern void ReflectAboutYAxis( char *pattern )
/********************************************

    Reflect an array about the Y axis.
*/
{
    unsigned    x_dim,y_dim;
    unsigned    x,y,xlate_x;
    char        temp;

    x_dim = *pattern++;
    y_dim = *pattern++;
    for( x = 0; x < x_dim / 2; ++x ) {
        xlate_x = x_dim - x - 1;
        for( y = 0; y < y_dim; ++y ) {
            temp = pattern[ y * x_dim + x ];
            pattern[ y * x_dim + x ] = pattern[ y * x_dim + xlate_x ];
            pattern[ y * x_dim + xlate_x ] = temp;
        }
    }
}


extern void TransformPatterns( void (*rtn)(char *) )
/***************************************************

    Transform each pattern array.
    "Rtn" is ReflectAboutYAxis or ReflectAboutXEqualY.
*/
{
    int         i;

    for( i = 0; i < NumberPatterns; ++i ) {
        rtn( Patterns[i] );
    }
}


static void LoadPatternFile( void )
/**********************************

    Load the pattern file named in "Buffer" into our pattern menu.
*/
{
    if( ReadAPatternFile( Buffer, NumberPatterns ) ) {
        ++NumberPatterns;
        CreatePatternMenu();
    }
}


extern void WritePatternFile( void )
/***********************************

    Write the selected region to a pattern file (Prompt for name)
*/
{
    pixels      start_x, end_x;
    pixels      start_y, end_y;
    pixels      x, y;
    FILE        *io;
    int         rc;

    if( !GetFileName( "Save A Pattern File", TRUE,
        "*.LIF", Buffer, BUFSIZ ) ) return;
    if( !access( Buffer, F_OK ) ) {

        char            buf[ _MAX_PATH + 100 ];

        sprintf( buf, "Overwrite file %s?", Buffer );
        rc = MessageBox( (HWND)0, buf, "Save A Pattern File",
                         MB_YESNO | MB_ICONEXCLAMATION );
        if( rc != IDYES ) return;
    }
    GetSelectedCoords( &start_x, &end_x, &start_y, &end_y );
    io = fopen( Buffer, "w" );
    if( io != NULL ) {
        for( y = start_y; y < end_y; ++y ) {
            for( x = start_x; x < end_x; ++x ) {
                if( CellPointer( x, y )->alive ) {
                    fputc( 'X', io );
                } else {
                    fputc( '_', io );
                }
            }
            fputc( '\n', io );
        }
        fclose( io );
    }
    LoadPatternFile();
}


extern void LoadNewPattern( void )
/*********************************

    Load a new pattern file into the menu (Prompt for name)
*/
{
    if( GetFileName( "Read A Pattern File", FALSE,
                "*.LIF", Buffer, BUFSIZ ) ) {
        LoadPatternFile();
    }
}


static HBITMAP CreateAMenuBitMap( char *pattern, pixels *total_height )
/**********************************************************************

    Create a bit map suitable for use in the Pattern menu based on "pattern".
    Add the height of the pattern to *total_height
*/
{
    HDC         dc;
    HDC         memory_dc;
    HBITMAP     bit_map;
    pixels      x,y;
    char        dim_x, dim_y;
    pixels      grid_x, grid_y;
    pixels      size_x, size_y;

    dim_x = *pattern++;
    dim_y = *pattern++;
    size_x = MenuBitInfo.bmWidth * dim_x;
    size_y = MenuBitInfo.bmHeight * dim_y;
    if( size_x > MAX_MENU_SIZE_X ) size_x = MAX_MENU_SIZE_X;
    if( size_y > MAX_MENU_SIZE_Y ) size_y = MAX_MENU_SIZE_Y;
    grid_x = size_x / dim_x;
    grid_y = size_y / dim_y;
    if( grid_x == 0 ) grid_x = 1;
    if( grid_y == 0 ) grid_y = 1;
    if( size_x < MIN_MENU_SIZE_X ) size_x = MIN_MENU_SIZE_X;
    if( size_y < MIN_MENU_SIZE_Y ) size_y = MIN_MENU_SIZE_Y;

    dc = GetDC( WinHandle );
    memory_dc = CreateCompatibleDC( dc );
    bit_map = CreateCompatibleBitmap( dc, size_x, size_y );
    SelectObject( memory_dc, bit_map );
    PatBlt( memory_dc, 0, 0, size_x, size_y, WHITENESS );
    for( y = 0; y < dim_y; ++y ) {
        for( x = 0; x < dim_x; ++x ) {
            if( *pattern++ ) {
                StretchBlt( memory_dc, x*grid_x, y*grid_y,
                            grid_x, grid_y, MenuDC,
                            0, 0,
                            MenuBitInfo.bmWidth, MenuBitInfo.bmHeight, SRCCOPY );
            }
        }
    }
    DeleteDC( memory_dc );
    ReleaseDC( WinHandle, dc );
    *total_height += size_y;
    return( bit_map );
}



extern void CreatePatternMenu( void )
/************************************

    Create the "&Pattern" menu, based upon MenuPatterns[]
*/
{
    HDC         dc;
    int         i;
    HMENU       menu;
    pixels      menu_height;

    if( MenuBitMap == (HBITMAP)0 ) {
        MenuBitMap = LoadBitmap( ThisInst, "MenuBitMap" );
        dc = GetDC( WinHandle );
        MenuDC = CreateCompatibleDC( dc );
        ReleaseDC( WinHandle, dc );
        SelectObject( MenuDC, MenuBitMap );
        GetObject( MenuBitMap, sizeof( BITMAP ), (LPSTR)&MenuBitInfo );
    }
    menu = CreateMenu();
    if( menu == (HMENU)0 ) {
        Error( "No room to create a new menu\n" );
        return;
    }
    PatternMenu = menu;
    i = 0;
    menu_height = 0;
    for( ;; ) {
        MenuPatterns[i] = CreateAMenuBitMap( Patterns[i], &menu_height );
        AppendMenu( PatternMenu, MF_BITMAP+MF_CHECKED,
                    MENU_PATTERN+i, (LPSTR)(LONG)MenuPatterns[i] );
        if( (i+1) == NumberPatterns ) break;
        if( menu_height > ScreenHeight / 3 ) {
            AppendMenu( PatternMenu, MF_MENUBARBREAK, 0, (LPSTR)(LONG)NULL );
            menu_height = 0;
        } else {
            AppendMenu( PatternMenu, MF_SEPARATOR, 0, (LPSTR)(LONG)NULL );
        }
        ++i;
    }
    AppendMenu( PatternMenu, MF_MENUBARBREAK, 0, (LPSTR)(LONG)NULL );
    AppendMenu( PatternMenu, MF_STRING, MENU_ROTATE_P90, "&Rotate +90" );
    AppendMenu( PatternMenu, MF_STRING, MENU_ROTATE_M90, "&Rotate -90" );
    AppendMenu( PatternMenu, MF_STRING, MENU_ROTATE_180, "&Rotate 180" );
    AppendMenu( PatternMenu, MF_STRING, MENU_REFLECT_X,  "&Flip Top/Bottom" );
    AppendMenu( PatternMenu, MF_STRING, MENU_REFLECT_Y,  "&Flip Left/Right" );
    ModifyMenu( GetMenu( WinHandle ), 2, MF_BYPOSITION | MF_POPUP,
                (UINT)PatternMenu, "&Pattern" );
}

extern void SetCurrPattern( int i )
/**********************************

    Set the currently selected pattern to "i"
*/
{
    Cursor = Patterns[ i ];
}

extern BOOL IsCurrPattern( int i )
/*********************************

    Is "i" the currently selected pattern?
*/
{
    return( Cursor == Patterns[ i ] );
}

extern void DrawPattern( pixels pixel_x, pixels pixel_y, BOOL erase )
/********************************************************************

    Draw the currently selected pattern at screen location (pixel_x,pixel_y).
    If "erase" is true, we wipe out a rectangle the size of the pattern.
*/
{
    pixels      screen_x, screen_y;
    pixels      x, y;
    pixels      wrap_x, wrap_y;
    HDC         dc;
    cell_ptr    cell;
    char        dim_x, dim_y;
    char        *pattern;
    BOOL        change;


    screen_x = pixel_x / BitInfo.bmWidth;
    screen_y = pixel_y / BitInfo.bmHeight;
    pattern = Cursor;
    dim_x = *pattern++;
    dim_y = *pattern++;
    change = FALSE;
    dc = GetDC( WinHandle );
    for( y = 0; y < dim_y; ++y ) {
        for( x = 0; x < dim_x; ++x ) {
            wrap_x = screen_x + x;
            wrap_y = screen_y + y;
            WrapAround( &wrap_x, &wrap_y );
            cell = CellPointer( wrap_x, wrap_y );
            if( erase ) {
                change |= TurnOffCell( dc, cell, wrap_x, wrap_y );
            } else if( *pattern ) {
                change |= TurnOnCell( dc, cell, wrap_x, wrap_y );
            }
            ++pattern;
        }
    }
    ReleaseDC( WinHandle, dc );
    if( change ) SetCaption();
}
