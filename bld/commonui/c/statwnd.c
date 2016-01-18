/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Status window for Windows and OS/2.
*
****************************************************************************/


#include "commonui.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "statwnd.h"
#include "mem.h"
#include "loadcc.h"
#include "wprocmap.h"

/*
 * The window must always be the first member of this structure so that
 * GetHintHwnd() in hint.c will work properly.
 */
typedef struct statwnd {
    HWND                win;
    int                 numSections;
    status_block_desc   sectionDesc[MAX_SECTIONS];
    char                *sectionData[MAX_SECTIONS + 1];
    UINT                sectionDataFlags[MAX_SECTIONS + 1];
    WPI_FONT            sectionDataFont;
    WPI_RECT            statusRect;
    int                 wndHeight;
} statwnd;

static char                     *className = "StatusWnd";
#ifdef __NT__
static HFONT                    systemDataFont;
#endif
static HPEN                     penLight;
static HPEN                     penShade;
static HBRUSH                   brushButtonFace;
static COLORREF                 colorButtonFace;
static COLORREF                 colorTextFace;
static statushook               statusWndHookFunc;
static int                      classWinExtra;
static bool                     hasGDIObjects = true;
static bool                     classRegistered;
static WPI_INST                 classHandle;
static statwnd                  *currentStatWnd;

/*
 * getRect - get a rectangle
 */
static void getRect( statwnd *sw, WPI_RECT *r, int i )
{
    WPI_RECTDIM pos;
    WPI_RECTDIM left;
    WPI_RECTDIM right;
    WPI_RECTDIM top;
    WPI_RECTDIM bottom;
    WPI_RECTDIM r_left;
    WPI_RECTDIM r_right;
    WPI_RECTDIM r_top;
    WPI_RECTDIM r_bottom;

    *r = sw->statusRect;
    _wpi_getrectvalues( sw->statusRect, &left, &top, &right, &bottom );
    _wpi_getrectvalues( *r, &r_left, &r_top, &r_right, &r_bottom );
    if( i > 0 ) {
        if( sw->sectionDesc[i - 1].width_is_percent ) {
            if( r_right > r_left ) {
                pos = (WORD)( ( ( r_right - r_left ) * (DWORD)sw->sectionDesc[i].width ) / 100L );
            } else {
                pos = (WORD)( ( ( r_left - r_right ) * (DWORD)sw->sectionDesc[i].width ) / 100L );
            }
        } else {
            pos = sw->sectionDesc[i - 1].width;
        }
        r_left = pos + sw->sectionDesc[i - 1].separator_width;
    }
    if( i == sw->numSections ) {
        pos = right;
    } else if( sw->sectionDesc[i].width_is_percent ) {
        if( r_right > r_left ) {
            pos = (WORD)( ( ( r_right - r_left ) * (DWORD)sw->sectionDesc[i].width ) / 100L );
        } else {
            pos = (WORD)( ( ( r_left - r_right ) * (DWORD)sw->sectionDesc[i].width ) / 100L );
        }
    } else {
        pos = sw->sectionDesc[i].width;
    }
    _wpi_setrectvalues( r, r_left, r_top, pos, r_bottom );

} /* getRect */

static WPI_FONT oldFont;
static HBRUSH   oldBrush;
static COLORREF oldBkColor;
#ifndef __OS2_PM__
static COLORREF oldTextColor;
#endif

/*
 * initPRES - initialize our presentation space for drawing text
 */
static bool initPRES( statwnd *sw, WPI_PRES pres )
{
    if( sw->sectionDataFont == NULL ) {
        return( false );
    }
#ifdef __NT__
    oldFont = _wpi_selectfont( pres, systemDataFont );
#else
    oldFont = _wpi_selectfont( pres, sw->sectionDataFont );
#endif
    oldBrush = _wpi_selectbrush( pres, brushButtonFace );
    oldBkColor = _wpi_getbackcolour( pres );
    _wpi_setbackcolour( pres, colorButtonFace );
#ifdef __OS2_PM__
    GpiSetBackMix( pres, BM_OVERPAINT );
#else
    oldTextColor = GetTextColor( pres );
    SetTextColor( pres, colorTextFace );
#endif
    return( true );

} /* initPRES */

/*
 * finiPRES - finished with our presentation space
 */
static void finiPRES( WPI_PRES pres )
{
    _wpi_getoldbrush( pres, oldBrush );
    _wpi_getoldfont( pres, oldFont );
    _wpi_setbackcolour( pres, oldBkColor );
#ifdef __OS2_PM__
    GpiSetBackMix( pres, BM_LEAVEALONE );
#else
    SetTextColor( pres, oldTextColor );
#endif

} /* finiPRES */

/*
 * makeInsideRect - make a rectangle the inside of a rectangle
 */
static void makeInsideRect( WPI_RECT *r )
{
    WPI_RECTDIM r_left;
    WPI_RECTDIM r_right;
    WPI_RECTDIM r_top;
    WPI_RECTDIM r_bottom;

    _wpi_getrectvalues( *r, &r_left, &r_top, &r_right, &r_bottom );
    r_left += BORDER_SIZE;
    r_top += BORDER_SIZE;
    r_right -= BORDER_SIZE;
    r_bottom -= BORDER_SIZE;

    _wpi_setrectvalues( r, r_left, r_top, r_right, r_bottom );

} /* makeInsideRect */

/*
 * outlineRect - draw the outline of a rectangle
 */
static void outlineRect( statwnd *sw, WPI_PRES pres, WPI_RECT *r )
{
    WPI_POINT   pt;
    WPI_RECTDIM left;
    WPI_RECTDIM right;
    WPI_RECTDIM top;
    WPI_RECTDIM bottom;
    HPEN        oldpen;

    sw = sw;
    _wpi_getrectvalues( *r, &left, &top, &right, &bottom );

    _wpi_setpoint( &pt, left, bottom - 1 );
    _wpi_cvth_pt( &pt, sw->wndHeight );
    _wpi_movetoex( pres, &pt, NULL );

    oldpen = _wpi_selectobject( pres, penLight );
    pt.x = right - 1;
    _wpi_lineto( pres, &pt );
    pt.y = _wpi_cvth_y( top, sw->wndHeight );
    _wpi_lineto( pres, &pt );

    _wpi_selectobject( pres, penShade );

    pt.x = left;
    _wpi_lineto( pres, &pt );
    pt.y = _wpi_cvth_y( bottom - 1, sw->wndHeight );
    _wpi_lineto( pres, &pt );

    _wpi_selectobject( pres, oldpen );

} /* outlineRect */

/*
 * StatusWndCallback - handle messages for the status window
 */
WINEXPORT WPI_MRESULT CALLBACK StatusWndCallback( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    PAINTSTRUCT ps;
    WPI_RECT    r;
    int         i;
    WPI_PRES    pres;
    statwnd     *sw;

    sw = (statwnd *)_wpi_getwindowlongptr( hwnd, classWinExtra );

    if( statusWndHookFunc != NULL ) {
        if( statusWndHookFunc( hwnd, msg, wparam, lparam ) ) {
            return( 0 );
        }
    }

    if( (msg == WM_SIZE || msg == WM_PAINT) && sw == NULL ) {
        return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
    }

    switch( msg ) {
    case WM_CREATE:
        _wpi_setwindowlongptr( hwnd, classWinExtra, (LONG_PTR)currentStatWnd );
        return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
    case WM_SIZE:
        GetClientRect( hwnd, &sw->statusRect );
        sw->wndHeight = _wpi_getheightrect( sw->statusRect );
        _wpi_inflaterect( classHandle, &sw->statusRect, -HORZ_BORDER, -VERT_BORDER );
        return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
    case WM_PAINT:
        pres = _wpi_beginpaint( hwnd, NULLHANDLE, &ps );
#ifdef __OS2_PM__
        WinFillRect( pres, &ps, CLR_PALEGRAY );
#endif
#ifdef __NT__
        /*
         * We have to do this little trick because currently this window does not receive
         * the WM_SYSCOLORCHANGE message when it should.
         */
        if( colorButtonFace != GetSysColor( COLOR_BTNFACE ) ) {
            RECT rs;
            if( hasGDIObjects ) {
                _wpi_deleteobject( penLight );
                _wpi_deleteobject( penShade );
                _wpi_deleteobject( brushButtonFace );
            }
            colorButtonFace = GetSysColor( COLOR_BTNFACE );
            _wpi_setbackcolour( pres, colorButtonFace );
            brushButtonFace = CreateSolidBrush( colorButtonFace );
            penLight = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNHIGHLIGHT ) );
            penShade = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) );
            hasGDIObjects = true;
            GetClientRect( hwnd, &rs );
            FillRect( pres, &rs, brushButtonFace );
        }
#endif
        StatusWndDraw3DBox( sw, pres );
        if( initPRES( sw, pres ) ) {
            for( i = 0; i <= sw->numSections; i++ ) {
                if( sw->sectionData[i] != NULL ) {
                    getRect( sw, &r, i );
                    makeInsideRect( &r );
                    _wpi_drawtext( pres, sw->sectionData[i], -1, &r, sw->sectionDataFlags[i] );
                }
            }
            finiPRES( pres );
        }
        _wpi_endpaint( hwnd, pres, &ps );
        break;
#ifdef __NT__
    case WM_SYSCOLORCHANGE:
        if( hasGDIObjects ) {
            DeleteObject( penLight );
            DeleteObject( penShade );
            DeleteObject( brushButtonFace );
            hasGDIObjects = false;
        }
        colorButtonFace = GetSysColor( COLOR_BTNFACE );
        colorTextFace = GetSysColor( COLOR_BTNTEXT );
        brushButtonFace = CreateSolidBrush( colorButtonFace );
        penLight = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNHIGHLIGHT ) );
        penShade = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) );
        hasGDIObjects = true;
        break;
#endif
    case WM_ERASEBKGND:
#ifdef __NT__
        if( colorButtonFace != GetSysColor( COLOR_BTNFACE ) ) {
            /*
             * If WM_SYSCOLORCHANGE message is not received by this window, we
             * have to fake it.
             */
            SendMessage( hwnd, WM_SYSCOLORCHANGE, 0, 0L );
        }
#endif
        GetClientRect( hwnd, &r );
        _wpi_unrealizeobject( brushButtonFace );
        _wpi_fillrect( (WPI_PRES)wparam, &r, colorButtonFace, brushButtonFace );
        break;
    default:
        return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );

} /* StatusWndCallback */

/*
 * StatusWndInit - initialize for using the status window
 */
bool StatusWndInit( WPI_INST hinstance, statushook hook, int extra, HCURSOR hDefaultCursor )
{
    bool        rc;
#ifdef __OS2_PM__
    /* OS/2 PM version of the initialization */

    colorButtonFace = CLR_PALEGRAY;

    if( !hasGDIObjects ) {
        brushButtonFace = _wpi_createsolidbrush( colorButtonFace );
        penLight = _wpi_createpen( PS_SOLID, 1, CLR_WHITE );
        penShade = _wpi_createpen( PS_SOLID, 1, CLR_DARKGRAY );
        hasGDIObjects = true;
    }

    statusWndHookFunc = hook;

    rc = true;
    if( !classRegistered ) {
        memcpy( &classHandle, &hinstance, sizeof( WPI_INST ) );
        rc = WinRegisterClass( hinstance.hab, className, (PFNWP)StatusWndCallback,
                               CS_SIZEREDRAW | CS_CLIPSIBLINGS,
                               extra + sizeof( statwnd * ) );
        classWinExtra = extra;
        classRegistered = true;
    }
#else
    /* Win16 and Win32 version of the initialization */
    WNDCLASS    wc;

  #ifdef __NT__
    if( LoadCommCtrl() ) {
        rc = true;
    } else {
  #endif
        if( !hasGDIObjects ) {
            colorButtonFace = GetSysColor( COLOR_BTNFACE );
            colorTextFace = GetSysColor( COLOR_BTNTEXT );
            brushButtonFace = CreateSolidBrush( colorButtonFace );
            penLight = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNHIGHLIGHT ) );
            penShade = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) );
            hasGDIObjects = true;
        }

        statusWndHookFunc = hook;

        rc = true;
        if( !GetClassInfo( hinstance, className, &wc ) ) {
            classHandle = hinstance;
            classWinExtra = extra;
            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = GetWndProc( StatusWndCallback );
            wc.cbClsExtra = 0;
            wc.cbWndExtra = extra + sizeof( statwnd * );
            wc.hInstance = hinstance;
            wc.hIcon = LoadIcon( (HINSTANCE)NULL, IDI_APPLICATION );
            wc.hCursor = hDefaultCursor;
            if( wc.hCursor == NULL ) {
                wc.hCursor = LoadCursor( (HINSTANCE)NULL, IDC_ARROW );
            }
            wc.hbrBackground = (HBRUSH)0;
            wc.lpszMenuName = NULL;
            wc.lpszClassName = className;
            rc = ( RegisterClass( &wc ) != 0 );
            classRegistered = true;
        }
  #ifdef __NT__
    }
  #endif
#endif
    return( rc );

} /* StatusWndInit */

/*
 * StatusWndStart - start a status window
 */
statwnd *StatusWndStart( void )
{
    statwnd *sw;

    sw = (statwnd *)MemAlloc( sizeof( statwnd ) );
    if( sw != NULL ) {
        memset( sw, 0, sizeof( statwnd ) );
    }
    return( sw );

} /* StatusWndStart */

/*
 * StatusWndChangeSysColors - fiddle with what StatusWnd believes
 *                            are the system colors
 */
void StatusWndChangeSysColors( COLORREF btnFace, COLORREF btnText,
                               COLORREF btnHighlight, COLORREF btnShadow )
{
#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        if( hasGDIObjects ) {
            _wpi_deleteobject( penLight );
            _wpi_deleteobject( penShade );
            _wpi_deleteobject( brushButtonFace );
        }

        colorButtonFace = btnFace;
        colorTextFace = btnText;
        brushButtonFace = _wpi_createsolidbrush( btnFace );
        penLight = _wpi_createpen( PS_SOLID, 1, btnHighlight );
        penShade = _wpi_createpen( PS_SOLID, 1, btnShadow );
        hasGDIObjects = true;
#ifdef __NT__
    }
#endif
}

#ifdef __NT__

/*
 * updateParts - update the parts of a native status bar
 */
static void updateParts( statwnd *sw )
{
    int     i;
    RECT    rc;
    int     width;
    int     *parts;

    parts = (int *)MemAlloc( sizeof( int ) * ( sw->numSections + 1 ) );
    GetClientRect( sw->win, &rc );
    width = rc.right - rc.left;
    for( i = 0; i < sw->numSections; i++ ) {
        if( sw->sectionDesc[i].width_is_percent ) {
            parts[i] = sw->sectionDesc[i].width * width / 100;
        } else {
            parts[i] = sw->sectionDesc[i].width;
        }
        if( i > 0 && parts[i] != -1 ) {
            parts[i] += sw->sectionDesc[i - 1].separator_width;
        }
    }
    if( sw->numSections == 0 || parts[sw->numSections - 1] != -1 ) {
        parts[sw->numSections] = -1;
        SendMessage( sw->win, SB_SETPARTS, sw->numSections + 1, (LPARAM)parts );
    } else {
        SendMessage( sw->win, SB_SETPARTS, sw->numSections, (LPARAM)parts );
    }
    MemFree( parts );

} /* updateParts */

#endif

/*
 * StatusWndCreate - create the status window
 */
HWND StatusWndCreate( statwnd *sw, HWND parent, WPI_RECT *size, WPI_INST hinstance,
                      LPVOID lpvParam )
{
#ifndef __OS2_PM__
    /* Win16 and Win32 version of creation */
    currentStatWnd = sw;
#ifdef __NT__
    if( IsCommCtrlLoaded() ) {
        sw->win = CreateWindow( STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE |
                                WS_CLIPSIBLINGS | SBARS_SIZEGRIP,
                                0, 0, 0, 0, parent, NULL, hinstance, NULL );
        GetWindowRect( sw->win, size );
        if( sw->numSections > 0 ) {
            updateParts( sw );
        }
    } else if( LOBYTE( LOWORD( GetVersion() ) ) >= 4 ) {
        sw->win = CreateWindow( className, NULL, WS_CHILD, size->left, size->top,
                                size->right - size->left, size->bottom - size->top, parent,
                                (HMENU)NULL, hinstance, lpvParam );
    } else {
        sw->win = CreateWindow( className, NULL, WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
                                size->left, size->top, size->right - size->left,
                                size->bottom - size->top, parent, (HMENU)NULL, hinstance,
                                lpvParam );
    }
#else
    sw->win = CreateWindow( className, NULL, WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
                            size->left, size->top, size->right - size->left,
                            size->bottom - size->top, parent, (HMENU)NULL, hinstance,
                            lpvParam );
#endif
    if( sw->win != NULL ) {
#ifdef __NT__
       if( LOBYTE( LOWORD( GetVersion() ) ) >= 4 ) {
           /* New shell active, Win95 or later */
           systemDataFont = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
       } else {
           systemDataFont = (HFONT)GetStockObject( SYSTEM_FONT );
       }
#endif
        ShowWindow( sw->win, SW_SHOWNORMAL );
        UpdateWindow( sw->win );
    }
#else
    /* OS/2 PM version of creation */
    ULONG       flags;

    hinstance = hinstance;
    lpvParam = lpvParam;
    flags = FCF_BORDER;
    currentStatWnd = sw;

    sw->win = WinCreateStdWindow( parent, WS_VISIBLE, &flags, className, "", 0L,
                                  (HMODULE)0, 10, NULL );
    if( sw->win != NULLHANDLE ) {
        WinSetWindowPos( sw->win, HWND_TOP, size->xLeft, size->yBottom,
                         size->xRight - size->xLeft, size->yTop - size->yBottom,
                         SWP_SIZE | SWP_MOVE | SWP_SHOW );
    }
#endif

    sw->wndHeight = _wpi_getheightrect( *size );
    return( sw->win );

} /* StatusWndCreate */

/*
 * StatusWndDraw3DBox - called by StatusWndDrawLine or externally
 *                      if StatusWndDrawLine is not used.
 */
void StatusWndDraw3DBox( statwnd *sw, WPI_PRES pres )
{
    HPEN        old_pen;
    int         i;
    WPI_RECT    r;

    old_pen = _wpi_selectobject( pres, penLight );
    for( i = 0; i <= sw->numSections; i++ ) {
        getRect( sw, &r, i );
        outlineRect( sw, pres, &r );
        makeInsideRect( &r );
        _wpi_fillrect( pres, &r, colorButtonFace, brushButtonFace );
    }
    _wpi_selectobject( pres, old_pen );

} /* StatusWndDraw3DBox */

/*
 * outputText - output a text string
 */
void outputText( statwnd *sw, WPI_PRES pres, char *buff, WPI_RECT *r, UINT flags,
                 int curr_block )
{
    WPI_RECT    ir;
    WPI_RECT    draw_rect;
    size_t      len;
    int         ext;
    int         width;
    WPI_RECTDIM ir_left;
    WPI_RECTDIM ir_right;
    WPI_RECTDIM ir_top;
    WPI_RECTDIM ir_bottom;
    WPI_RECTDIM r_left;
    WPI_RECTDIM r_right;
    WPI_RECTDIM r_top;
    WPI_RECTDIM r_bottom;
    UINT        pmflags;
#ifndef __NT__
    int         height;
#else
    SIZE        sz;
#endif

    if( sw->sectionData[curr_block] != NULL ) {
        if( !strcmp( buff, sw->sectionData[curr_block] ) &&
            flags == sw->sectionDataFlags[curr_block] ) {
            return;
        }
    }

    len = strlen( buff );
    if( len == 0 ) {
        return;
    }
    MemFree( sw->sectionData[curr_block] );
    sw->sectionData[curr_block] = MemAlloc( len + 1 );
    memcpy( sw->sectionData[curr_block], buff, len + 1 );
    sw->sectionDataFlags[curr_block] = flags | DT_TEXTATTRS;

#ifndef __NT__
    _wpi_gettextextent( pres, buff, (int)len, &ext, &height );
#else
    GetTextExtentPoint( pres, buff, (int)len, &sz );
    ext = sz.cx;
#endif
    ir = *r;
    _wpi_getrectvalues( ir, &ir_left, &ir_top, &ir_right, &ir_bottom );
#ifdef __OS2_PM__
    ir_left++;
    ir_top++;
    ir_bottom++;
    _wpi_setrectvalues( &draw_rect, ir_left, ir_top, ir_right, ir_bottom );
#else
    draw_rect = *r;
#endif
    if( flags & DT_CENTER ) {
        width = ( ir_right - ir_left - ext ) / 2;
        if( width > 0 ) {
            ir_right = ir_left + width;
            _wpi_setrectvalues( &ir, ir_left, ir_top, ir_right, ir_bottom );
            _wpi_fillrect( pres, &ir, colorButtonFace, brushButtonFace );

            _wpi_getrectvalues( *r, &r_left, &r_top, &r_right, &r_bottom );
            ir_right = r_right;
            ir_left = r_right - width;
            _wpi_setrectvalues( &ir, ir_left, ir_top, ir_right, ir_bottom );
            _wpi_fillrect( pres, &ir, colorButtonFace, brushButtonFace );
        }
    } else if( flags & DT_RIGHT ) {
        ir_right -= ext;
        if( ir_right >= ir_left ) {
            _wpi_setrectvalues( &ir, ir_left, ir_top, ir_right, ir_bottom );
            _wpi_fillrect( pres, &ir, colorButtonFace, brushButtonFace );
        }
    } else {
        ir_left += ext;
        if( ir_left < ir_right ) {
            _wpi_setrectvalues( &ir, ir_left, ir_top, ir_right, ir_bottom );
            _wpi_fillrect( pres, &ir, colorButtonFace, brushButtonFace );
        }
    }
    pmflags = flags | DT_TEXTATTRS;
    _wpi_drawtext( pres, buff, -1, &draw_rect, pmflags );

} /* outputText */

/*
 * StatusWndDrawLine - draws a line in the status bar
 */
void StatusWndDrawLine( statwnd *sw, WPI_PRES pres, WPI_FONT hfont, const char *str, UINT flags )
{
    WPI_RECT    rect;
    char        buff[256];
    char        *bptr;
    int         curr_block;

    curr_block = 0;
#ifdef __NT__
    hfont = hfont;
    sw->sectionDataFont = systemDataFont;
#else
    sw->sectionDataFont = hfont;
#endif
#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        if( !initPRES( sw, pres ) ) {
            return;
        }
        getRect( sw, &rect, curr_block );
        makeInsideRect( &rect );
        bptr = (char *)str;
        if( flags == (UINT)-1  ) {
            flags = DT_VCENTER | DT_LEFT;
            bptr = buff;
            while( *str != '\0' ) {
                if( *str == STATUS_ESC_CHAR ) {
                    str++;
                    switch( *str ) {
                    case STATUS_NEXT_BLOCK:
                        *bptr = 0;
                        outputText( sw, pres, buff, &rect, flags, curr_block );
                        curr_block++;
                        getRect( sw, &rect, curr_block );
                        makeInsideRect( &rect );
                        flags = DT_VCENTER | DT_LEFT;
                        bptr = buff;
                        break;
                    case STATUS_FORMAT_CENTER:
                        flags &= ~(DT_RIGHT | DT_LEFT);
                        flags |= DT_CENTER;
                        break;
                    case STATUS_FORMAT_RIGHT:
                        flags &= ~(DT_CENTER | DT_LEFT);
                        flags |= DT_RIGHT;
                        break;
                    case STATUS_FORMAT_LEFT:
                        flags &= ~(DT_CENTER | DT_RIGHT);
                        flags |= DT_LEFT;
                        break;
                    }
                } else {
                    *bptr++ = *str;
                }
                str++;
            }
            *bptr = 0;
            bptr = buff;
        }
        outputText( sw, pres, bptr, &rect, flags, curr_block );
        finiPRES( pres );
#ifdef __NT__
    } else {
        bptr = (char *)str;
        if( flags == (UINT)-1 ) {
            bptr = buff;
            while( *str != '\0' ) {
                if( *str == STATUS_ESC_CHAR ) {
                    str++;
                    if( *str == STATUS_NEXT_BLOCK ) {
                        *bptr = 0;
                        if( strlen( buff ) > 0 ) {
                            SendMessage( sw->win, SB_SETTEXT, curr_block, (LPARAM)buff );
                        }
                        curr_block++;
                        bptr = buff;
                    }
                } else {
                    *bptr++ = *str;
                }
                str++;
            }
            *bptr = 0;
            bptr = buff;
        }
        if( strlen( bptr ) > 0 ) {
            SendMessage( sw->win, SB_SETTEXT, curr_block, (LPARAM)bptr );
        }
    }
#endif

} /* StatusWndDrawLine */

/*
 * StatusWndSetSeparators - set the separator list
 */
void StatusWndSetSeparators( statwnd *sw, int num_items, status_block_desc *list )
{
    int     i;

    if( num_items > MAX_SECTIONS ) {
        num_items = MAX_SECTIONS;
    }
    for( i = 0; i < num_items; i++ ) {
        sw->sectionDesc[i] = list[i];
    }
    sw->numSections = num_items;

#ifdef __NT__
    if( IsCommCtrlLoaded() && sw->win != NULL ) {
        updateParts( sw );
    }
#endif

} /* StatusWndSetSeparators */

/*
 * StatusWndGetHeight - get the height of the status window for Win32 native status bars
 */
int StatusWndGetHeight( statwnd *sw )
{
#ifdef __NT__
    if( IsCommCtrlLoaded() ) {
        return( sw->wndHeight );
    }
#endif
    return( 0 );
}

/*
 * StatusWndDestroy - cleans up everything allocated for the status window
 */
void StatusWndDestroy( statwnd *sw )
{
    int i;

    if( sw != NULL ) {
        if( sw->win != NULLHANDLE ) {
            _wpi_destroywindow( sw->win );
        }
        for( i = 0; i <= sw->numSections; i++ ) {
            MemFree( sw->sectionData[i] );
            sw->sectionData[i] = NULL;
        }
        MemFree( sw );
    }

} /* StatusWndDestroy */

/*
 * StatusWndFini - cleans up everything allocated for the status window
 */
void StatusWndFini( void )
{
    if( hasGDIObjects ) {
        _wpi_deleteobject( penLight );
        _wpi_deleteobject( penShade );
        _wpi_deleteobject( brushButtonFace );
        hasGDIObjects = false;
    }
    if( classRegistered ) {
        _wpi_unregisterclass( className, classHandle );
        classRegistered = false;
    }

} /* StatusWndFini */

