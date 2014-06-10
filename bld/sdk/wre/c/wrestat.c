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


#include "precomp.h"
#include <string.h>
#include <mbstring.h>
#include <stdlib.h>
#include "wreglbl.h"
#include "wremsg.h"
#include "ldstr.h"
#include "rcstr.gh"
#include "statwnd.h"
#include "wreribbn.h"
#include "wrestat.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define STATUS_FONTNAME   "Helv"
#define STATUS_POINTSIZE  8
#define MAX_STATUS_TEXT   _MAX_PATH
#define STATUS_LINE_PAD   4
#define STATUS1_WIDTH     160

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
BOOL WREStatusHookProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool WREDisplayStatusText( char * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static void      *WREStatusBar = NULL;
static HWND      WREStatusWindow = NULL;
static HFONT     WREStatusFont = NULL;
static char      WREStatusText[2 * MAX_STATUS_TEXT + 2] = { 0 };
static char      WREClearStatusText[7] = {
    ' ', STATUS_ESC_CHAR, STATUS_NEXT_BLOCK,
    ' ', STATUS_ESC_CHAR, STATUS_NEXT_BLOCK,
    0
};
static int       WREStatusDepth = 0;

int WREGetStatusDepth( void )
{
    return( WREStatusDepth );
}

void WREDestroyStatusLine( void )
{
    if( WREStatusWindow != NULL ) {
        DestroyWindow( WREStatusWindow );
        StatusWndDestroy( WREStatusBar );
        StatusWndFini();
    }

    if( WREStatusFont != NULL ) {
        DeleteObject( WREStatusFont );
    }
}

bool WRECreateStatusLine( HWND main, HINSTANCE inst )
{
    RECT                rect;
    LOGFONT             lf;
    TEXTMETRIC          tm;
    HFONT               old_font;
    HDC                 dc;
    status_block_desc   sbd;
    char                *status_font;
    char                *cp;
    int                 point_size;
    bool                use_default;

    memset( &lf, 0, sizeof( LOGFONT ) );
    dc = GetDC( main );
    lf.lfWeight = FW_BOLD;
    use_default = TRUE;

    status_font = AllocRCString( WRE_STATUSFONT );
    if( status_font != NULL ) {
        cp = (char *)_mbschr( (unsigned char const *)status_font, '.' );
        if( cp != NULL ) {
            *cp = '\0';
            strcpy( lf.lfFaceName, status_font );
            cp++;
            point_size = atoi( cp );
            use_default = FALSE;
        }
        FreeRCString( status_font );
    }

    if( use_default ) {
        strcpy( lf.lfFaceName, STATUS_FONTNAME );
        point_size = STATUS_POINTSIZE;
    }

    lf.lfHeight = -MulDiv( point_size, GetDeviceCaps( dc, LOGPIXELSY ), 72 );
    WREStatusFont = CreateFontIndirect( &lf );
    old_font = SelectObject( dc, WREStatusFont );
    GetTextMetrics( dc, &tm );
    SelectObject( dc, old_font );
    ReleaseDC( main, dc );

    GetClientRect( main, &rect );

    WREStatusDepth = tm.tmHeight + STATUS_LINE_PAD + VERT_BORDER * 2;
    rect.top = rect.bottom - WREStatusDepth;

    StatusWndInit( inst, WREStatusHookProc, 0, (HCURSOR)NULL );
    WREStatusBar = StatusWndStart();

    sbd.separator_width = STATUS_LINE_PAD;
    sbd.width = STATUS1_WIDTH;
    sbd.width_is_percent = FALSE;
    sbd.width_is_pixels = TRUE;

    StatusWndSetSeparators( WREStatusBar, 1, &sbd );

    WREStatusWindow = StatusWndCreate( WREStatusBar, main, &rect, inst, NULL );

    if( WREStatusWindow == NULL ) {
        WREDisplayErrorMsg( WRE_NOCREATESTATUS );
        return( FALSE );
    }

    /* set the text in the status window */
    WRESetStatusReadyText();

    GetWindowRect( WREStatusWindow, &rect );
    WREStatusDepth = rect.bottom - rect.top;

    return( TRUE );
}

void WREResizeStatusWindows( RECT *rect )
{
    int         y;

    if( WREStatusWindow != NULL ) {
        y = WREGetRibbonHeight();
        y = max( y, (rect->bottom - rect->top) - WREStatusDepth );
        MoveWindow( WREStatusWindow, 0, y, rect->right - rect->left, WREStatusDepth, TRUE );
    }
}

bool WRESetStatusReadyText( void )
{
    WRESetStatusText( NULL, "", FALSE );
    return( WRESetStatusByID( WRE_READYMSG, -1 ) );
}

bool WRESetStatusByID( DWORD id1, DWORD id2 )
{
    char        *str1;
    char        *str2;
    bool        ret;

    ret = FALSE;
    str1 = NULL;
    str2 = NULL;

    if( id1 != -1 ) {
        str1 = AllocRCString( id1 );
    }

    if( id2 != -1 ) {
        str2 = AllocRCString( id2 );
    }

    ret = WRESetStatusText( str1, str2, TRUE );

    if( str1 != NULL ) {
        FreeRCString( str1 );
    }

    if( str2 != NULL ) {
        FreeRCString( str2 );
    }

    return( ret );
}

bool WRESetStatusText( const char *status1, const char *status2, int redisplay )
{
    int len;
    int pos;

    /* touch unused vars to get rid of warning */
    _wre_touch( redisplay );

    if( WREStatusWindow == NULL ) {
        return( TRUE );
    }

    if( status1 != NULL ) {
        len = min( strlen( status1 ), MAX_STATUS_TEXT );
        if( len != 0 ) {
            memcpy( WREStatusText, status1, len );
            pos = len;
        } else {
            WREStatusText[0] = ' ';
            pos = 1;
        }
    } else {
        pos = 0;
    }

    if( status2 != NULL ) {
        WREStatusText[pos++] = STATUS_ESC_CHAR;
        WREStatusText[pos++] = STATUS_NEXT_BLOCK;
        len = min( strlen( status2 ), MAX_STATUS_TEXT );
        if( len != 0 ) {
            memcpy ( WREStatusText + pos, status2, len );
            WREStatusText[pos + len] = '\0';
        } else {
            WREStatusText[pos++] = ' ';
            WREStatusText[pos]   = '\0';
        }
    } else {
        WREStatusText[pos++] = '\0';
    }

    if( status1 != NULL || status2 != NULL ) {
        return( WREDisplayStatusText( WREStatusText ) );
    }

    return( TRUE );
}

bool WREDisplayStatusText( char *str )
{
    HDC hdc;

    if( WREStatusWindow != NULL ) {
        hdc = GetDC( WREStatusWindow );
        if( hdc != (HDC)NULL ) {
            if( str != NULL ) {
                StatusWndDrawLine( WREStatusBar, hdc, WREStatusFont, str, -1 );
            } else {
                StatusWndDrawLine( WREStatusBar, hdc, WREStatusFont,
                                   WREClearStatusText, -1 );
            }
            ReleaseDC( WREStatusWindow, hdc );
        }
    }

    return( TRUE );
}

BOOL WREStatusHookProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    /* touch unused vars to get rid of warning */
    _wre_touch( hWnd );
    _wre_touch( wParam );
    _wre_touch( lParam );

    if( msg == WM_DESTROY ) {
        WREStatusWindow = NULL;
    }

    return( FALSE );
}
