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


#include <windows.h>
#include <string.h>
#include <mbstring.h>
#include <stdio.h>
#include "wglbl.h"
#include "wmem.h"
#include "wmsg.h"
#include "wmain.h"
#include "wstat.h"
#include "wmsgfile.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define STATUS_FONTNAME   "Helv"
#define STATUS_POINTSIZE  8
#define STATUS_LINE_PAD   4
#define STATUS1_WIDTH     160

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL WStatusWndProc ( HWND, unsigned, UINT, LONG );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static Bool   WDisplayStatusText ( wstatbar * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static int       WStatusDepth  = 0;
static HFONT     WStatusFont   = NULL;

Bool WInitStatusLines( HINSTANCE inst )
{
    LOGFONT             lf;
    TEXTMETRIC          tm;
    HFONT               old_font;
    HDC                 dc;
    char                *status_font;
    char                *cp;
    int                 point_size;
    Bool                use_default;

    memset ( &lf, 0, sizeof(LOGFONT) );
    dc = GetDC ( (HWND)NULL );
    lf.lfWeight = FW_BOLD;
    use_default = TRUE;

    status_font = WAllocRCString( W_STATUSFONT );
    if( status_font ) {
        cp = _mbschr( status_font, '.' );
        if( cp ) {
            *cp = '\0';
            strcpy( lf.lfFaceName, status_font );
            cp++;
            point_size = atoi( cp );
            use_default = FALSE;
        }
        WFreeRCString( status_font );
    }

    if( use_default ) {
        strcpy( lf.lfFaceName, STATUS_FONTNAME );
        point_size = STATUS_POINTSIZE;
    }

    lf.lfHeight = -MulDiv( point_size, GetDeviceCaps ( dc, LOGPIXELSY ), 72 );
    WStatusFont = CreateFontIndirect ( &lf );
    old_font = SelectObject ( dc, WStatusFont );
    GetTextMetrics ( dc, &tm );
    SelectObject ( dc, old_font );
    ReleaseDC ( (HWND)NULL, dc );

    WStatusDepth = tm.tmHeight + STATUS_LINE_PAD + VERT_BORDER * 2;

    StatusWndInit( inst, WStatusWndProc, 0 );

    return( TRUE );
}

void WFiniStatusLines( void )
{
    if( WStatusFont != (HFONT) NULL ) {
        DeleteObject( WStatusFont );
    }

    StatusWndFini();
}

int WGetStatusDepth ( void )
{
    return ( WStatusDepth );
}

void WResizeStatusWindows ( wstatbar *wsb, RECT *rect )
{
    if ( wsb->stat->win ) {
        MoveWindow ( wsb->stat->win, 0,
                     max ( 0, (rect->bottom - rect->top) - WStatusDepth ),
                     (rect->right - rect->left), WStatusDepth, TRUE );
    }
}

void WDestroyStatusLine ( wstatbar *wsb )
{
    if ( wsb ) {
        StatusWndDestroy ( wsb->stat );
        WMemFree ( wsb );
    }
}

wstatbar *WCreateStatusLine ( HWND parent, HINSTANCE inst )
{
    wstatbar          *wsb;
    RECT               rect;
    status_block_desc  sbd;

    wsb = (wstatbar *) WMemAlloc ( sizeof(wstatbar) );
    if ( wsb ) {
        wsb->stat = StatusWndStart ();
        if ( !wsb->stat ) {
            return ( NULL );
        }
    } else {
        return ( NULL );
    }

    GetClientRect ( parent, &rect );

    rect.top = rect.bottom - WStatusDepth;

    sbd.separator_width   = STATUS_LINE_PAD;
    sbd.width             = STATUS1_WIDTH;
    sbd.width_is_percent  = FALSE;
    sbd.width_is_pixels   = TRUE;

    StatusWndSetSeparators ( wsb->stat, 1, &sbd );

    if( !StatusWndCreate( wsb->stat, parent, &rect, inst, NULL ) ) {
        WDisplayErrorMsg( W_STATUSNOTCREATED );
        WDestroyStatusLine( wsb );
        return ( NULL );
    }

    WSetStatusReadyText( wsb );

    return( wsb );
}

Bool WSetStatusReadyText( wstatbar *wsb )
{
    WSetStatusText( wsb, NULL, "" );
    return( WSetStatusByID( wsb, W_READYMSG, -1 ) );
}

Bool WSetStatusByID( wstatbar *wsb, DWORD id1, DWORD id2 )
{
    char        *str1;
    char        *str2;
    Bool        ret;

    ret = FALSE;
    str1 = NULL;
    str2 = NULL;

    if( id1 != -1 ) {
        str1 = WAllocRCString( id1 );
    }

    if( id2 != -1 ) {
        str2 = WAllocRCString( id2 );
    }

    ret = WSetStatusText( wsb, str1, str2 );

    if( str1 ) {
        WFreeRCString( str1 );
    }

    if( str2 ) {
        WFreeRCString( str2 );
    }

    return( ret );
}

Bool WSetStatusText ( wstatbar *wsb, const char *s1, const char *s2 )
{
    int             len;
    int             pos;
    WAccelEditInfo *einfo;

    if ( !wsb ) {
        einfo = WGetCurrentEditInfo ();
        if ( einfo ) {
            wsb = einfo->wsb;
            if ( !wsb ) {
                return ( FALSE );
            }
        } else {
            return ( FALSE );
        }
    }

    if ( wsb->stat->win == (HWND) NULL ) {
        return ( TRUE );
    }

    if ( s1 ) {
        len = min ( strlen (s1), MAX_STATUS_TEXT );
        if ( len ) {
            memcpy ( wsb->text, s1, len );
            pos = len;
        } else {
            wsb->text[0] = ' ';
            pos = 1;
        }
    } else {
        pos = 0;
    }

    if ( s2 ) {
        wsb->text[pos++] = STATUS_ESC_CHAR;
        wsb->text[pos++] = STATUS_NEXT_BLOCK;
        len = min ( strlen (s2), MAX_STATUS_TEXT );
        if ( len ) {
            memcpy ( wsb->text+pos, s2, len );
            wsb->text[pos+len] = '\0';
        } else {
            wsb->text[pos++] = ' ';
            wsb->text[pos]   = '\0';
        }
    } else {
        wsb->text[pos++] = '\0';
    }

    if ( s1 || s2 ) {
        return ( WDisplayStatusText ( wsb ) );
    }

    return ( TRUE );
}

BOOL WStatusWndProc ( HWND hWnd, unsigned msg, UINT wParam, LONG lParam )
{
    /* touch unused vars to get rid of warning */
    _wtouch(hWnd);
    _wtouch(wParam);
    _wtouch(lParam);
    _wtouch(msg);

    return( FALSE );
}

Bool WDisplayStatusText( wstatbar *wsb )
{
    HDC    hdc;

    hdc = GetDC ( wsb->stat->win );
    if ( hdc != (HDC)NULL ) {
        StatusWndDrawLine ( wsb->stat, hdc, WStatusFont, wsb->text, -1 );
        ReleaseDC ( wsb->stat->win, hdc );
    }

    return ( TRUE );
}

