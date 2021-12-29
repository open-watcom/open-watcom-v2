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


#include "wreglbl.h"
#include <mbstring.h>
#include "wremsg.h"
#include "ldstr.h"
#include "rcstr.grh"
#include "statwnd.h"
#include "wreribbn.h"
#include "wrestat.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define STATUS_FONTFACENAME     "Helv"
#define STATUS_FONTPOINTSIZE    8
#define MAX_STATUS_TEXT         _MAX_PATH
#define STATUS_LINE_PAD         4
#define STATUS1_WIDTH           160

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
bool WREStatusHookProc( HWND, UINT, WPARAM, LPARAM );

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
    status_block_desc   sbd[1];
    char                *font_facename;
    char                *cp;
    int                 font_pointsize;
    bool                use_default;

    memset( &lf, 0, sizeof( LOGFONT ) );
    dc = GetDC( main );
    lf.lfWeight = FW_BOLD;
    use_default = true;

    font_facename = AllocRCString( WRE_STATUSFONT );
    if( font_facename != NULL ) {
        cp = (char *)_mbschr( (unsigned char const *)font_facename, '.' );
        if( cp != NULL ) {
            *cp = '\0';
            strcpy( lf.lfFaceName, font_facename );
            cp++;
            font_pointsize = atoi( cp );
            use_default = false;
        }
        FreeRCString( font_facename );
    }

    if( use_default ) {
        strcpy( lf.lfFaceName, STATUS_FONTFACENAME );
        font_pointsize = STATUS_FONTPOINTSIZE;
    }

    lf.lfHeight = -MulDiv( font_pointsize, GetDeviceCaps( dc, LOGPIXELSY ), 72 );
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

    sbd[0].separator_width = STATUS_LINE_PAD;
    sbd[0].width = STATUS1_WIDTH;
    sbd[0].width_is_percent = false;

    StatusWndSetSeparators( WREStatusBar, 1, sbd );

    WREStatusWindow = StatusWndCreate( WREStatusBar, main, &rect, inst, NULL );

    if( WREStatusWindow == NULL ) {
        WREDisplayErrorMsg( WRE_NOCREATESTATUS );
        return( false );
    }

    /* set the text in the status window */
    WRESetStatusReadyText();

    GetWindowRect( WREStatusWindow, &rect );
    WREStatusDepth = rect.bottom - rect.top;

    return( true );
}

void WREResizeStatusWindows( RECT *rect )
{
    int         y;

    if( WREStatusWindow != NULL ) {
        y = WREGetRibbonHeight();
        if( y < ( rect->bottom - rect->top ) - WREStatusDepth )
            y = ( rect->bottom - rect->top ) - WREStatusDepth;
        MoveWindow( WREStatusWindow, 0, y, rect->right - rect->left, WREStatusDepth, TRUE );
    }
}

bool WRESetStatusReadyText( void )
{
    WRESetStatusText( NULL, "", FALSE );
    return( WRESetStatusByID( WRE_READYMSG, 0 ) );
}

bool WRESetStatusByID( msg_id id1, msg_id id2 )
{
    char        *str1;
    char        *str2;
    bool        ret;

    ret = false;
    str1 = NULL;
    str2 = NULL;

    if( id1 > 0 ) {
        str1 = AllocRCString( id1 );
    }

    if( id2 > 0 ) {
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
        return( true );
    }

    if( status1 != NULL ) {
        len = strlen( status1 );
        if( len > MAX_STATUS_TEXT )
            len = MAX_STATUS_TEXT;
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
        len = strlen( status2 );
        if( len > MAX_STATUS_TEXT )
            len = MAX_STATUS_TEXT;
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

    return( true );
}

bool WREDisplayStatusText( char *str )
{
    HDC hdc;

    if( WREStatusWindow != NULL ) {
        hdc = GetDC( WREStatusWindow );
        if( hdc != (HDC)NULL ) {
            if( str != NULL ) {
                StatusWndDrawLine( WREStatusBar, hdc, WREStatusFont, str, DT_ESC_CONTROLLED );
            } else {
                StatusWndDrawLine( WREStatusBar, hdc, WREStatusFont, WREClearStatusText, DT_ESC_CONTROLLED );
            }
            ReleaseDC( WREStatusWindow, hdc );
        }
    }

    return( true );
}

bool WREStatusHookProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    /* touch unused vars to get rid of warning */
    _wre_touch( hWnd );
    _wre_touch( wParam );
    _wre_touch( lParam );

    if( msg == WM_DESTROY ) {
        WREStatusWindow = NULL;
    }

    return( false );
}
