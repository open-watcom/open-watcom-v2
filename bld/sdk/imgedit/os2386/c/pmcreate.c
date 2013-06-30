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
#include "ieclrpal.h"
#include "ieprofil.h"

static int      newChildNumber = 999;
static int      viewWindowNumber = 1999;

/*
 * PM_CreateColourPal - pm version to create the colour palette
 */
void PM_CreateColourPal( void )
{
    ULONG       flags;
    HWND        client;

    flags = FCF_TITLEBAR        |
            FCF_BORDER          |
            FCF_SYSMENU         |
            FCF_NOMOVEWITHOWNER;

    HColourPalette = WinCreateStdWindow(
        HWND_DESKTOP,
        0,
        &flags,
        PaletteClass,
        "Color Palette",
        0,
        (HMODULE)0,
        IMGED_CLR_PAL,
        &client);

    if(!HColourPalette) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }
    /*
     * This will allow the colour palette to float outside the main window.
     */
    WinSetOwner( HColourPalette, _wpi_getframe(HMainWindow) );
    WinSetWindowPos( HColourPalette, HWND_TOP, ImgedConfigInfo.pal_xpos,
                                ImgedConfigInfo.pal_ypos, CP_WIDTH,
                                CP_HEIGHT, SWP_SIZE | SWP_MOVE );
    HColourPalette = client;

} /* PM_CreateColourPal */

/*
 * PM_CreateCurrentDisp - creates the display window for the current colours
 *                        in the colour palette ... for PM
 */
HWND PM_CreateCurrentDisp( HWND hparent )
{
    HWND        frame_wnd;
    ULONG       flags;

    flags = 0L;

    frame_wnd = WinCreateStdWindow(
        hparent,
        WS_VISIBLE,
        &flags,
        CURRENT_CLASS,
        "",
        0L,
        (HMODULE)0,
        0,
        NULL);

    if(!frame_wnd) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }
    WinSetWindowPos( frame_wnd, HWND_TOP, CUR_WND_X, CUR_WND_Y, CUR_WND_WIDTH,
                        CUR_WND_HEIGHT, SWP_SIZE | SWP_MOVE | SWP_SHOW );

    return(frame_wnd);
} /* PM_CreateCurrentDisp */

/*
 * PM_CreateColourCtrls - creates the controls for the colour palette for
 *                        PM
 */
void PM_CreateColourCtrls( HWND hpar, HWND *colours, HWND *screenclrs,
                                        HWND *screentxt, HWND *inversetxt )
{
    ULONG       flags;
    HWND        frame_wnd;
    HWND        hwnd1;
    HWND        hwnd2;
    HWND        hwnd3;
    HWND        client;
    LONG        grayindex = CLR_PALEGRAY;
    LONG        blackindex = CLR_BLACK;

    hwnd1 = WinCreateWindow(
        hpar,
        WC_STATIC,
        "",
        WS_VISIBLE | SS_TEXT | DT_VCENTER | DT_RIGHT,
        TEXT1X,                                 /* Initial X position */
        TEXT1Y-1,                               /* Initial Y position */
        TEXT_WIDTH,                             /* Initial X size */
        TEXTHEIGHT+1,                           /* Initial Y size */
        hpar,
        HWND_TOP,
        0,
        NULL,
        NULL);

    if(!(hwnd1)) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }
    WinSetPresParam( hwnd1, PP_FONTNAMESIZE, 7L, (PVOID)"8.Helv" );
    WinSetPresParam( hwnd1, PP_FOREGROUNDCOLORINDEX, (ULONG)sizeof(LONG),
                                (PVOID)&blackindex );
    WinSetPresParam( hwnd1, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(LONG),
                                (PVOID)&grayindex );
    SetWindowText( hwnd1, "Fill:" );

    hwnd2 = WinCreateWindow(
        hpar,
        WC_STATIC,
        "",
        WS_VISIBLE | SS_TEXT | DT_VCENTER | DT_RIGHT,
        TEXT1X,                                 /* Initial X position */
        TEXT2Y-2,                               /* Initial Y position */
        TEXT_WIDTH,                             /* Initial X size */
        TEXTHEIGHT+1,                           /* Initial Y size */
        hpar,
        HWND_TOP,
        1,
        NULL,
        NULL);

    if(!(hwnd2)) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }
    WinSetPresParam( hwnd2, PP_FONTNAMESIZE, 7L, (PVOID)"8.Helv" );
    WinSetPresParam( hwnd2, PP_FOREGROUNDCOLORINDEX, (ULONG)sizeof(LONG),
                                (PVOID)&blackindex );
    WinSetPresParam( hwnd2, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(LONG),
                                (PVOID)&grayindex );
    SetWindowText( hwnd2, "Draw:" );

    hwnd3 = WinCreateWindow(
        hpar,
        WC_STATIC,
        "",
        WS_VISIBLE | SS_TEXT | DT_VCENTER | DT_RIGHT,
        LINE1X,                                 /* Initial X position */
        LINE1Y,                                 /* Initial Y position */
        TEXT_WIDTH+5,                           /* Initial X size */
        TEXTHEIGHT+1,                           /* Initial Y size */
        hpar,
        HWND_TOP,
        5,
        NULL,
        NULL);

    if(!hwnd3) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }
    WinSetPresParam( hwnd3, PP_FONTNAMESIZE, 7L, (PVOID)"8.Helv" );
    WinSetPresParam( hwnd3, PP_FOREGROUNDCOLORINDEX, (ULONG)sizeof(LONG),
                                (PVOID)&blackindex );
    WinSetPresParam( hwnd3, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(LONG),
                                (PVOID)&grayindex );
    SetWindowText( hwnd3, " " );
    *inversetxt = hwnd3;

    hwnd3 = WinCreateWindow(
        hpar,
        WC_STATIC,
        "",
        WS_VISIBLE | SS_TEXT | DT_VCENTER | DT_RIGHT,
        LINE1X,                                 /* Initial X position */
        LINE2Y,                                 /* Initial Y position */
        TEXT_WIDTH+5,                           /* Initial X size */
        TEXTHEIGHT+1,                           /* Initial Y size */
        hpar,
        HWND_TOP,
        4,
        NULL,
        NULL);

    if(!hwnd3) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }
    WinSetPresParam( hwnd3, PP_FONTNAMESIZE, 7L, (PVOID)"8.Helv" );
    WinSetPresParam( hwnd3, PP_FOREGROUNDCOLORINDEX, (ULONG)sizeof(LONG),
                                (PVOID)&blackindex );
    WinSetPresParam( hwnd3, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(LONG),
                                (PVOID)&grayindex );
    SetWindowText( hwnd3, " " );
    *screentxt = hwnd3;

    flags = 0L;

    frame_wnd = WinCreateStdWindow(
        hpar,
        WS_VISIBLE,
        &flags,
        AVAIL_CLASS,
        "",
        WS_VISIBLE,
        (HMODULE)0,
        2,
        &client);

    if(!client) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }
    *colours = client;
    WinSetPresParam( *colours, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(RGB),
                                (PVOID)&grayindex );
    WinSetWindowPos( frame_wnd, HWND_TOP, COL_WND_X, COL_WND_Y, COL_WND_WIDTH,
                        COL_WND_HEIGHT, SWP_SIZE | SWP_MOVE | SWP_SHOW );

    frame_wnd = WinCreateStdWindow(
        hpar,
//      WS_VISIBLE,
        0L,
        &flags,
        SCREEN_CLASS,
        "",
//      WS_VISIBLE,
        0L,
        (HMODULE)0,
        3,
        &client);

    if(!frame_wnd) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }
    *screenclrs = client;
    WinSetPresParam( frame_wnd, PP_BACKGROUNDCOLORINDEX, (ULONG)sizeof(RGB),
                                (PVOID)&grayindex );
    WinSetWindowPos( frame_wnd, HWND_TOP, SCRN_WND_X, SCRN_WND_Y, SCRN_WND_WIDTH,
                        SCRN_WND_HEIGHT, SWP_SIZE | SWP_MOVE | SWP_SHOW );
} /* PM_CreateColourCtrls */

/*
 * PMNewDrawPad - creates a new draw pad which is a child of the client
 *                    window.  These are the mdi children (PM version).
 */
HWND PMNewDrawPad( img_node *node )
{
    short               y_adjustment;
    short               x_adjustment;
    short               pad_x;
    short               pad_y;
    int                 i;
    img_node            *temp;
    WPI_POINT           origin;
    char                filename[ 20 ];
    HWND                frame_wnd;
    ULONG               flags;
    char                *classname;
    HWND                drawarea;
    WPI_RECT            rcclient;

    node->viewhwnd = CreateViewWin( node->width, node->height );

    ++newChildNumber;
    temp = node->nexticon;

    for(i=1; i < node->num_of_images; ++i) {
        temp->viewhwnd = node->viewhwnd;
        temp = temp->nexticon;
    }
    GetFnameFromPath( node->fname, filename );

    if (node->imgtype == BITMAP_IMG) {
        classname = DrawAreaClassB;
    } else if (node->imgtype == ICON_IMG) {
        classname = DrawAreaClassI;
    } else {
        classname = DrawAreaClassC;
    }
    flags = FCF_TITLEBAR        |
            FCF_SIZEBORDER      |
            FCF_SYSMENU;

    x_adjustment = (short)(2*WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER));
    y_adjustment = (short)(2*WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER) +
                        WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR));

    CalculateDims( node->width, node->height, &pad_x, &pad_y );
    GetClientRect( ClientWindow, &rcclient );
    origin.x = 0L;
    origin.y = _wpi_cvth_wanchor( 0L, pad_y + y_adjustment,
                                            _wpi_getheightrect( rcclient ) );
    FindOrigin( &origin );
//    origin.y = rcclient.yTop - pad_y - STATUS_WIDTH;
//    new_h = _wpi_getheightrect( rcclient );
//    new_y = _wpi_cvth_wanchor( origin.y, y_adjustment + pad_y, new_h );

    frame_wnd = WinCreateStdWindow(
        ClientWindow,
        0L,
        &flags,
        classname,
        filename,
        WS_VISIBLE,
        (HMODULE)0,
        0,
        &drawarea);

    if(!frame_wnd || !drawarea) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }

    for (i=0; i < node->num_of_images; ++i) {
        node[i].hwnd = drawarea;
    }
    AddImageNode( node );
    CreateUndoStack( node );

    WinSetWindowPos(frame_wnd, HWND_TOP, origin.x, origin.y,
                    x_adjustment + pad_x,
                    y_adjustment + pad_y,
                    SWP_MOVE | SWP_SHOW | SWP_SIZE);
    return( drawarea );

} /* PMNewDrawPad */

/*
 * PMCreateViewWin - creates a view window ... PM version
 */
HWND PMCreateViewWin( HWND hviewwnd, BOOL foneview, int *showstate,
                                                int width, int height )
{
    HWND        client;
    HWND        frame;
    ULONG       flags;
    RECTL       location;
    int         x,y;
    int         h_adj;
    int         v_adj;

    ++viewWindowNumber;
    if ((hviewwnd) && (foneview)) {
        WinQueryWindowRect( hviewwnd, &location );
        x = location.xLeft;
        y = location.yBottom;
    } else {
        x = ImgedConfigInfo.view_xpos;
        y = ImgedConfigInfo.view_ypos;
    }
    flags = FCF_TITLEBAR        |
            FCF_BORDER          |
            FCF_SYSMENU         |
            FCF_NOMOVEWITHOWNER;

    frame = WinCreateStdWindow(
        HWND_DESKTOP,
        0L,
        &flags,
        NULL,
        "",
        0L,
        (HMODULE)0,
        viewWindowNumber,
        NULL);

    client = WinCreateWindow(
        frame,
        ViewWinClass,
        "",
        WS_VISIBLE,
        0,                                      /* Initial X position */
        0,                                      /* Initial Y position */
        0,                                      /* Initial X size */
        0,                                      /* Initial Y size */
        frame,
        HWND_TOP,
        FID_CLIENT,
        NULL,
        NULL);

    if(!frame || !client) {
        DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
        DosExit(EXIT_PROCESS, RETURN_ERROR);
    }

    h_adj = 2*WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER) + 2 * BORDER_WIDTH;
    v_adj = 2*WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER) +
            WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) + 2*BORDER_WIDTH;

    WinSetOwner( frame, _wpi_getframe(HMainWindow) );
    WinSetWindowPos(frame, HWND_TOP, x, y, h_adj + width, v_adj + height,
                        SWP_MOVE | SWP_SIZE | SWP_HIDE );

    if ( ImgedConfigInfo.show_state & SET_SHOW_VIEW ) {
        *showstate = TRUE;
        WinShowWindow( frame, TRUE );
    } else {
        *showstate = FALSE;
        WinShowWindow( frame, FALSE );
    }
    return( client );
} /* PMCreateViewWin */

