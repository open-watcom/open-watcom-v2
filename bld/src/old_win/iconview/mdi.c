#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "view.h"
#include "icon.h"
#include "mdi.h"
#include "util.h"
#include "switch.h"
#include "filedll.h"

extern char IconClass[];

/*
 * DrawMdiIcon - draws the icon in MDI_icon on the screen by blitting
 * the XOR_mask onto the area given by rect (after anding the background
 * with the AND_mask).
 */

static void DrawMdiIcon( HDC hdc, an_MDI_icon *MDI_icon, RECT *r )
{
    BITMAP              bm;
    HDC                 memDC;
    POINT               src_org, src_size;
    POINT               dst_org, dst_size;

    dst_size.y = r->bottom - r->top + 1;
    dst_size.x = r->right - r->left + 1;
    DPtoLP( hdc, &dst_size, 1 );

    dst_org.x = r->left;
    dst_org.y = r->top;
    DPtoLP( hdc, &dst_org, 1 );

    GetObject( MDI_icon->AND_bitmap, sizeof( BITMAP ), (LPSTR) &bm );

    memDC = CreateCompatibleDC( hdc );
    SelectObject( memDC, MDI_icon->AND_bitmap );

    src_size.x = bm.bmWidth;
    src_size.y = bm.bmHeight;
    DPtoLP( hdc, &src_size, 1 );

    src_org.x = 0;
    src_org.y = 0;
    DPtoLP( hdc, &src_org, 1 );

    StretchBlt( hdc, dst_org.x, dst_org.y, dst_size.x, dst_size.y,
        memDC, src_org.x, src_org.y, src_size.x, src_size.y, SRCAND );

    SelectObject( memDC, MDI_icon->XOR_bitmap );

    StretchBlt( hdc, dst_org.x, dst_org.y, dst_size.x, dst_size.y,
        memDC, src_org.x, src_org.y, src_size.x, src_size.y, SRCCOPY );

    DeleteDC( memDC );
} /* DrawMdiIcon */

/*
 * MdiReadIcon - reads in the icon contained in the file fp, and
 * creates an MDI child window for the new icon. The bitmaps are
 * associated with the window by using the GetWindowLong procs.
 */

HWND MdiReadIcon( FILE *fp, char *title, char *file_name, long flags )
{
    HWND                hwnd;
    MDICREATESTRUCT     mdi;
    an_icon_file        *icon_file;
    an_icon             *icon;
    an_MDI_icon         *MDI_icon;
    HDC                 hdc;

    mdi.szClass = IconClass;
    mdi.szTitle = title;
    mdi.hOwner = Instance;
    mdi.x = (signed short) CW_USEDEFAULT;
    mdi.y = (signed short) CW_USEDEFAULT;
    mdi.cx = (signed short) CW_USEDEFAULT;
    mdi.cy = (signed short) CW_USEDEFAULT;
    mdi.style = flags;
    mdi.lParam = NULL;

    hwnd = SendMessage( ClientWindow, WM_MDICREATE, 0, (LONG)(LPSTR)&mdi );
    MDI_icon = (an_MDI_icon *)GetWindowLong( hwnd, 0 );
    hdc = GetDC( hwnd );

    icon_file = IconOpen( fp );
    icon = IconResourceToIcon( fp, icon_file, 0 );
    MDI_icon->XOR_bitmap = IconToXorBitmap( hdc, icon );
    MDI_icon->AND_bitmap = IconToAndBitmap( hdc, icon );
    IconFini( icon );
    IconClose( icon_file );
    MDI_icon->file_name = strdup( file_name );
    MDI_icon->current_icon = 0;

    ReleaseDC( hwnd, hdc );
    return( hwnd );
} /* MdiReadIcon */

/*
 * MdiOpenIcon - prompts the user for a file name using the standard
 * file open dialog boxes and then calls MdiReadIcon to do teh actual
 * work of creating a window for the screen and transforming the icon
 * file into usable bitmaps.
 */

HWND MdiOpenIcon()
{
    FILEOPEN            of;
    char                filename[ _MAX_PATH ];
    char                title[ _MAX_PATH ];
    char                ext[16];
    FILE                *fp;
    HWND                hwnd;
    HANDLE              dll;
#if defined(__WINDOWS_386__)
    HINDIR              h;
#else
    BOOL                (PASCAL FAR *gsn)( LPFILEOPEN );
#endif
    FARPROC             farp;
    DWORD               a1,a2;
    BOOL                rc;

    dll = LoadLibrary( "filedll.dll" );
    if( dll < 32 ) {
        MessageBox( NULL, "Could not find filedll.dll!", "Icon Viewer", MB_OK );
        return( FALSE );
    }
    farp = (FARPROC) GetProcAddress( dll,"GetFileName" );
#if defined(__WINDOWS_386__)
    h = GetIndirectFunctionHandle( farp, INDIR_PTR, INDIR_ENDLIST );
#else
    gsn = (void FAR *) farp;
#endif

    filename[ 0 ] = '\0';
    of.hwnd = FrameWindow;
    a1 = AllocAlias16( "*.ico" );
    of.ext = (LPSTR) a1;
    a2 = AllocAlias16( filename );
    of.name = (LPSTR) a2;
    of.namelen = sizeof( filename );
    of.type = FILE_OPEN;
    of.title = NULL;
#if defined(__WINDOWS_386__)
    rc = InvokeIndirectFunction( h, &of );
#else
    rc = gsn( &of );
#endif
    if( rc ) {
        _splitpath( filename, NULL, NULL, title, ext );
        strcat( title, ext );
        fp = fopen( filename, "rb" );
        hwnd = MdiReadIcon( fp, title, filename, WS_ICONIC );
        ShowWindow( hwnd, SW_SHOWMINIMIZED );
        UpdateWindow( hwnd );
        fclose( fp );
    }
    FreeAlias16( a1 );
    FreeAlias16( a2 );
    FreeLibrary( dll );
    return( hwnd );

} /* MdiIconOpen */

/*
 * ReloadIcon - destroys the current bitmap for an MDI icon and loads
 * in the version which occurs at the offset given by MDI_icon->current_icon.
 * No error checking is performed because we know that the icon version was
 * selected via a dialog with a combo box and hence must be valid.
 */

static void ReloadIcon( HWND hwnd, an_MDI_icon *MDI_icon )
{
    FILE            *fp;
    an_icon_file    *icon_file;
    an_icon         *icon;
    HDC             hdc;

    fp = fopen( MDI_icon->file_name, "rb" );
    if( !fp ) return;
    DeleteObject( MDI_icon->XOR_bitmap );
    DeleteObject( MDI_icon->AND_bitmap );
    icon_file = IconOpen( fp );
    icon = IconResourceToIcon( fp, icon_file, MDI_icon->current_icon );
    hdc = GetDC( hwnd );
    MDI_icon->XOR_bitmap = IconToXorBitmap( hdc, icon );
    MDI_icon->AND_bitmap = IconToAndBitmap( hdc, icon );
    ReleaseDC( hwnd, hdc );
    IconFini( icon );
    IconClose( icon_file );
    fclose( fp );
} /* ReloadIcon */

/*
 * This is the window proc for an individual MDI child window containing
 * a displayable icon.
 */

LONG _EXPORT FAR PASCAL MdiIconProc( HWND hwnd, unsigned msg,
                                    WORD wparam, LONG lparam )
{
    an_MDI_icon     *icon;
    HDC             hdc;
    PAINTSTRUCT     ps;
    RECT            rect;

    icon = (an_MDI_icon *) GetWindowLong( hwnd, 0 );
    switch( msg ) {
    case WM_CREATE:
        icon = UtilMalloc( sizeof( an_MDI_icon ) );
        icon->XOR_bitmap = 0;   /* use as flag so we don't paint */
        SetWindowLong( hwnd, 0, (LONG) (LPVOID) icon );
        break;

    case WM_COMMAND:
        if( wparam == IDM_SWITCHICON ) {
            if( SwitchIcon( icon ) ) {
                ReloadIcon( hwnd, icon );
                InvalidateRect( hwnd, NULL, TRUE );
                UpdateWindow( hwnd );
            }
        }
        break;

    case WM_PAINT:
        if( icon->XOR_bitmap ) {
            hdc = BeginPaint( hwnd, &ps );
            /* draw the bitmap in the client area of the MDI window */
            GetClientRect( hwnd, &rect );
            DrawMdiIcon( hdc, icon, &rect );
            EndPaint( hwnd, &ps );
        }
        break;

    case WM_DESTROY:
        /* destroy the BITMAP and the ICON */
        DeleteObject( icon->XOR_bitmap );
        DeleteObject( icon->AND_bitmap );
        UtilFree( icon->file_name );
        UtilFree( icon );
        break;
    default:
        return( DefMDIChildProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );
} /* MdiIconProc */
