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


#ifdef __OS2_PM__

#define INCL_PM
#define INCL_WINFRAMEMGR
#define INCL_NLS
#define INCL_GPILCIDS
#define INCL_GPIPRIMITIVES
#include <os2.h>

#else

#include <windows.h>

#endif
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "wpi.h"
#include "wstatus.h"
#include "mem.h"

#define STATUS_DIM      WPI_RECTDIM
#ifndef MAX_SECTIONS
#define MAX_SECTIONS    20
#endif

static char                     *className = "StatusWnd";
static int                      numSections;
static status_block_desc        sectionDesc[MAX_SECTIONS];
static char                     *sectionData[MAX_SECTIONS+1];
static UINT                     sectionDataFlags[MAX_SECTIONS+1];
static WPI_FONT                 sectionDataFont;
static HPEN                     penLight;
static HPEN                     penShade;
static HBRUSH                   brushButtonFace;
static COLORREF                 colorButtonFace;
static statushook               statusWndHookFunc;
static WPI_RECT                 statusRect;
static BOOL                     hasGDIObjects;
static BOOL                     classRegistered;
static WPI_INST                 classHandle;
static int                      wndHeight;

#if defined( UNIX )
#define CB    LONG
#elif defined(__WINDOWS_386__)
#define CB      LONG FAR PASCAL
#elif defined(__WINDOWS__)
#define CB      LONG __export FAR PASCAL
#elif defined(__NT__)
#define CB      LONG __export __stdcall
#elif defined(__OS2_PM__)
#define CB      MRESULT EXPENTRY
#elif defined(__QNX__) /* Willows */
#define CB      LONG
#else
#error CB return type not configured
#endif

/*
 * getRect - get a rectangle
 */
static void getRect( WPI_RECT *r, int i )
{
    WORD        pos;
    WORD        width;
    STATUS_DIM  left;
    STATUS_DIM  right;
    STATUS_DIM  top;
    STATUS_DIM  bottom;
    STATUS_DIM  r_left;
    STATUS_DIM  r_right;
    STATUS_DIM  r_top;
    STATUS_DIM  r_bottom;

    *r = statusRect;
    width = _wpi_getwidthrect( statusRect );
    _wpi_getrectvalues( statusRect, &left, &top, &right, &bottom );
    _wpi_getrectvalues( *r, &r_left, &r_top, &r_right, &r_bottom );

    if( i > 0 ) {
        if( sectionDesc[i-1].width_is_percent ) {
            pos = (WORD) (((DWORD) width * (DWORD) sectionDesc[i].width)/100L);
        } else {
            pos = sectionDesc[i-1].width;
        }
        r_left = pos+sectionDesc[i-1].separator_width;
    }
    if( i == numSections ) {
        pos = right;
    } else if( sectionDesc[i].width_is_percent ) {
        pos = (WORD) (((DWORD)width * (DWORD)sectionDesc[i].width)/100L);
    } else {
        pos = sectionDesc[i].width;
    }
    _wpi_setrectvalues( r, r_left, r_top, pos, r_bottom );
} /* getRect */

static WPI_FONT oldFont;
static HBRUSH   oldBrush;
static COLORREF oldBkColor;

/*
 * initPRES - initialize our presentation space for drawing text (hdc for
 *              windows and hps for pm)
 */
static char initPRES( WPI_PRES pres )
{
    if( sectionDataFont == NULL ) {
        return( FALSE );
    }
    oldFont = _wpi_selectfont( pres, sectionDataFont );
    oldBrush = _wpi_selectbrush( pres, brushButtonFace );
    oldBkColor = _wpi_getbackcolour( pres );
    _wpi_setbackcolour( pres, colorButtonFace );
#ifdef __OS2_PM__
    GpiSetBackMix( pres, BM_OVERPAINT );
#endif
    return( TRUE );
} /* initPRES */

/*
 * finiPRES - finished with our PRES
 */
static void finiPRES( WPI_PRES pres )
{
    _wpi_getoldbrush( pres, oldBrush );
    _wpi_getoldfont( pres, oldFont );
    _wpi_setbackcolour( pres, oldBkColor );
#ifdef __OS2_PM__
    GpiSetBackMix( pres, BM_LEAVEALONE );
#endif

} /* finiPRES */

/*
 * StatusWndCallback - handle messages for
 */
CB StatusWndCallback( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam  )
{
    PAINTSTRUCT ps;
    WPI_RECT    r;
    int         i;
    WPI_PRES    pres;

    if( statusWndHookFunc != NULL ) {
        if( statusWndHookFunc( hwnd, msg, wparam, lparam ) ) {
            return( 0 );
        }
    }
    switch( msg ) {
    case WM_SIZE:
        GetClientRect( hwnd, &statusRect );
        wndHeight = _wpi_getheightrect( statusRect );
        _wpi_inflaterect(classHandle, &statusRect, -HORZ_BORDER, -VERT_BORDER);
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    case WM_PAINT:
        pres = _wpi_beginpaint( hwnd, NULL, &ps );
#ifdef __OS2_PM__
        WinFillRect( pres, &ps, CLR_PALEGRAY );
#endif
        StatusWndDraw3DBox( pres );
        if( initPRES( pres ) ) {
            for( i=0;i<=numSections;i++ ) {
                if( sectionData[i] != NULL ) {
                    getRect( &r, i );
                    makeInsideRect( &r );
                    _wpi_drawtext( pres, sectionData[i], -1, &r,
                                                    sectionDataFlags[i] );
                }
            }
            finiPRES( pres );
        }
        _wpi_endpaint( hwnd, pres, &ps );
        break;
    case WM_ERASEBKGND:
        GetClientRect( hwnd, &r );
        _wpi_unrealizeobject( brushButtonFace );
        _wpi_fillrect( (WPI_PRES)wparam, &r, colorButtonFace, brushButtonFace );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );
} /* StatusWndCallback */

/*
 * StatusWndInit - initialize for using the status window
 */
int StatusWndInit( WPI_INST hinstance, statushook hook, int extra )
{
#ifndef __OS2_PM__
        /*
         ******************
         * Windows Version of the initialization
         ******************
         */
    WNDCLASS    wc;
    int         rc;

    colorButtonFace = GetSysColor( COLOR_BTNFACE );
    if( !hasGDIObjects ) {
        brushButtonFace = CreateSolidBrush( colorButtonFace );
        penLight = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNHIGHLIGHT ) );
        penShade = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) );
        hasGDIObjects = TRUE;
    }

    statusWndHookFunc = hook;

    rc = TRUE;
    if( !classRegistered ) {
        classHandle = hinstance;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = (LPVOID) StatusWndCallback;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = extra;
        wc.hInstance = hinstance;
        wc.hIcon = LoadIcon( (HINSTANCE)NULL, IDI_APPLICATION );
        wc.hCursor = LoadCursor( (HINSTANCE)NULL, IDC_ARROW );
        wc.hbrBackground = (HBRUSH) 0;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = className;
        rc = RegisterClass( &wc );
        classRegistered = TRUE;
    }
    return( rc );
#else
        /*
         ******************
         * PM Version of the initialization
         ******************
         */
    int         rc;

    colorButtonFace = CLR_PALEGRAY;

    if( !hasGDIObjects ) {
        brushButtonFace = _wpi_createsolidbrush( colorButtonFace );
        penLight = _wpi_createpen( PS_SOLID, 1, CLR_WHITE );
        penShade = _wpi_createpen( PS_SOLID, 1, CLR_DARKGRAY );
        hasGDIObjects = TRUE;
    }

    statusWndHookFunc = hook;

    rc = TRUE;
    if( !classRegistered ) {
        memcpy( &classHandle, &hinstance, sizeof(WPI_INST) );
        rc = WinRegisterClass( hinstance.hab, className,
                                (PFNWP)StatusWndCallback,
                                CS_SIZEREDRAW | CS_CLIPSIBLINGS, extra );
        classRegistered = TRUE;
    }
    return( rc );
#endif
} /* StatusWndInit */

/*
 * StatusWndCreate - create the status window
 */
HWND StatusWndCreate( HWND parent, WPI_RECT *size, WPI_INST hinstance,
                                                        LPVOID lpvParam )
{
    HWND        stat;

#ifndef __OS2_PM__
        /*
         ****************
         * Windows version of create
         ****************
         */
    stat = CreateWindow( className, NULL,
                         WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
                         size->left, size->top,
                         size->right - size->left, size->bottom - size->top,
                         parent, (HMENU)NULL, hinstance, lpvParam );
    if( stat != NULL ) {
        ShowWindow( stat, SW_SHOWNORMAL );
        UpdateWindow( stat );
    }
#else
        /*
         ****************
         * PM version of create
         ****************
         */
    ULONG       flags;

    hinstance = hinstance;
    lpvParam = lpvParam;
    flags = FCF_BORDER;

    stat = WinCreateStdWindow( parent, WS_VISIBLE, &flags, className, "", 0L,
                                                    (HMODULE)0, 10, NULL);
    if(stat != NULLHANDLE) {
        WinSetWindowPos( stat, HWND_TOP, size->xLeft, size->yBottom,
                        size->xRight - size->xLeft, size->yTop - size->yBottom,
                        SWP_SIZE | SWP_MOVE | SWP_SHOW );
    }
#endif
    wndHeight = _wpi_getheightrect( *size );
    return( stat );
} /* StatusWndCreate */

/*
 * makeInsideRect - make a rectangle the inside of a rectangle
 */
static void makeInsideRect( WPI_RECT *r )
{
    STATUS_DIM  r_left;
    STATUS_DIM  r_right;
    STATUS_DIM  r_top;
    STATUS_DIM  r_bottom;

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
static void outlineRect( WPI_PRES pres, WPI_RECT *r )
{
    WPI_POINT   pt;
    STATUS_DIM  left;
    STATUS_DIM  right;
    STATUS_DIM  top;
    STATUS_DIM  bottom;
    HPEN        oldpen;

    _wpi_getrectvalues( *r, &left, &top, &right, &bottom );

    _wpi_setpoint( &pt, left, bottom-1 );
    _wpi_cvth_pt( &pt, wndHeight );
    _wpi_movetoex( pres, &pt, NULL );

    oldpen = _wpi_selectobject( pres, penLight );
    pt.x = right - 1;
    _wpi_lineto( pres, &pt );
    pt.y = _wpi_cvth_y( top, wndHeight );
    _wpi_lineto( pres, &pt );

    _wpi_selectobject( pres, penShade );

    pt.x = left;
    _wpi_lineto( pres, &pt );
    pt.y = _wpi_cvth_y( bottom-1, wndHeight );
    _wpi_lineto( pres, &pt );

    _wpi_selectobject( pres, oldpen );
} /* outlineRect */

/*
 * StatusWndDraw3DBox - called by StatusWndDrawLine or externally
 *                      in StatusWndDrawLine is not used.
 */
void StatusWndDraw3DBox( WPI_PRES pres )
{
    int         i;
    WPI_RECT    r;

    for( i=0;i<=numSections;i++ ) {
        getRect( &r, i );
        outlineRect( pres, &r );
        makeInsideRect( &r );
        _wpi_fillrect( pres, &r, colorButtonFace, brushButtonFace );
    }

} /* StatusWndDraw3DBox */

/*
 * outputText - output a text string
 */
void outputText( WPI_PRES pres, char *buff, WPI_RECT *r, UINT flags,
                                                            int curr_block )
{
    WPI_RECT    ir;
    WPI_RECT    draw_rect;
    int         len;
    int         ext;
    int         width;
    STATUS_DIM  ir_left;
    STATUS_DIM  ir_right;
    STATUS_DIM  ir_top;
    STATUS_DIM  ir_bottom;
    STATUS_DIM  r_left;
    STATUS_DIM  r_right;
    STATUS_DIM  r_top;
    STATUS_DIM  r_bottom;
    UINT        pmflags;

    if( sectionData[ curr_block ] != NULL ) {
        if( !strcmp( buff, sectionData[ curr_block ] ) ) {
            return;
        }
    }

    len = strlen( buff );
    if( len == 0 ) {
        return;
    }
    MemFree( sectionData[ curr_block ] );
    sectionData[ curr_block ] = MemAlloc( len+1 );
    memcpy( sectionData[ curr_block ], buff, len+1 );
    sectionDataFlags[ curr_block ] = flags | DT_TEXTATTRS;

#ifndef __NT__
    {
        int             height;

        _wpi_gettextextent( pres, buff, len, &ext, &height );
    }
#else
    {
        SIZE    sz;

        GetTextExtentPoint( pres, buff, len, &sz );
        ext = sz.cx;
    }
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
        width = (ir_right - ir_left - ext)/2;
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
void StatusWndDrawLine( WPI_PRES pres, WPI_FONT hfont, char *str, UINT flags )
{
    WPI_RECT    rect;
    char        buff[256];
    char        *bptr;
    int         curr_block;

    curr_block = 0;
    sectionDataFont = hfont;
    if( !initPRES( pres ) ) {
        return;
    }
    getRect( &rect, curr_block );
    makeInsideRect( &rect );
    bptr = str;
    if( flags == (UINT) -1  ) {
        flags = DT_VCENTER | DT_LEFT;
        bptr = buff;
        while( *str ) {
            if( *str == STATUS_ESC_CHAR ) {
                str++;
                switch( *str ) {
                case STATUS_NEXT_BLOCK:
                    *bptr = 0;
                    outputText( pres, buff, &rect, flags, curr_block );
                    curr_block++;
                    getRect( &rect, curr_block );
                    makeInsideRect( &rect );
                    flags = DT_VCENTER | DT_LEFT;
                    bptr = buff;
                    break;
                case STATUS_FORMAT_CENTER:
                    flags &= ~(DT_RIGHT|DT_LEFT);
                    flags |= DT_CENTER;
                    break;
                case STATUS_FORMAT_RIGHT:
                    flags &= ~(DT_CENTER|DT_LEFT);
                    flags |= DT_RIGHT;
                    break;
                case STATUS_FORMAT_LEFT:
                    flags &= ~(DT_CENTER|DT_RIGHT);
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
    outputText( pres, bptr, &rect, flags, curr_block );
    finiPRES( pres );
} /* StatusWndDrawLine */

/*
 * StatusWndSetSeparators - set the separator list
 */
void StatusWndSetSeparators( int num_items, status_block_desc *list )
{
    int i;

    if( num_items > MAX_SECTIONS ) {
        num_items = MAX_SECTIONS;
    }
    for( i=0;i<num_items;i++ ) {
        sectionDesc[i] = list[i];
    }
    numSections = num_items;

} /* StatusWndSetSeparators */

/*
 * StatusWndFini - cleans up everything allocated for the status window
 */
void StatusWndFini( void )
{
    int i;

    if( hasGDIObjects ) {
        _wpi_deleteobject( penLight );
        _wpi_deleteobject( penShade );
        _wpi_deleteobject( brushButtonFace );
        hasGDIObjects = FALSE;
    }
    for( i=0;i<=numSections;i++ ) {
        MemFree( sectionData[i] );
        sectionData[i] = NULL;
    }
    numSections = 0;
    if( classRegistered ) {
        _wpi_unregisterclass( className, classHandle );
        classRegistered = FALSE;
    }
} /* StatusWndFini */

