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
#include "iemem.h"

static void             *statusBar;
static HWND             statusBarWnd;
static char             leftBlock[3];
static char             nextBlock[5];
static char             hotspotPosition[7];
static char             imgSizePosition[9];
static char             hintTextPosition[11];

static char             *PosText = NULL;
static char             *SizeText = NULL;
static char             *SetPosText = NULL;
static char             *PositionText = NULL;
static char             *SetSizeText = NULL;
static char             *PositionSizeText = NULL;
static char             *SetHotSpotText = NULL;
static char             *HotSpotText = NULL;
static char             *SetBitmapText = NULL;
static char             *SetCursorText = NULL;
static char             *SetIconText = NULL;
static char             *ImageText = NULL;

int StatusWidth = INIT_STATUS_WIDTH;

/*
 * InitStatusLine - initializes the status line ...
 */
BOOL InitStatusLine( HWND parent )
{
    WPI_RECT            client;
    WPI_RECT            rcsize;
    status_block_desc   sbd[4];
    WPI_PRES            pres;
    char                *text;
    char                next_block[3];
    int                 len;

    CreateStatusFont();

    sprintf( leftBlock, "%c%c", STATUS_ESC_CHAR, STATUS_FORMAT_LEFT );
    sprintf( next_block, "%c%c", STATUS_ESC_CHAR, STATUS_NEXT_BLOCK );
    strcpy( nextBlock, next_block );
    strcat( nextBlock, leftBlock );

    strcpy( hotspotPosition, next_block );
    strcat( hotspotPosition, next_block );
    strcat( hotspotPosition, leftBlock );

    PosText = IEAllocRCString( WIE_STATUSPOSTEXT );
    SizeText = IEAllocRCString( WIE_STATUSSIZETEXT );
    SetPosText = IEAllocRCString( WIE_STATUSPOSINFO );

    if( SetPosText != NULL ) {
        PositionText = (char *)MemAlloc( strlen( leftBlock ) +
                                         strlen( SetPosText ) + 20 + 1 );
        text = IEAllocRCString( WIE_STATUSSIZEINFO );
        if( text != NULL ) {
            SetSizeText = (char *)MemAlloc( strlen( SetPosText ) + strlen( text ) + 1 );
            if( SetSizeText != NULL ) {
                strcpy( SetSizeText, SetPosText );
                strcat( SetSizeText, text );
                PositionSizeText = (char *)MemAlloc( strlen( leftBlock ) +
                    strlen( SetSizeText ) + strlen( nextBlock ) + 40 + 1 );
            }
            IEFreeRCString( text );
        }
    }

    SetHotSpotText = IEAllocRCString( WIE_STATUSHOTSPOTINFO );
    if( SetHotSpotText != NULL ) {
        HotSpotText = (char *)MemAlloc( strlen( hotspotPosition ) +
                                        strlen( SetHotSpotText ) + 20 + 1 );
    }

    SetBitmapText = IEAllocRCString( WIE_BITMAPIMAGETEXT );
    SetCursorText = IEAllocRCString( WIE_CURSORIMAGETEXT );
    SetIconText = IEAllocRCString( WIE_ICONIMAGETEXT );
    if( SetBitmapText != NULL && SetCursorText != NULL && SetIconText != NULL ) {
        int len1;
        len = strlen( SetBitmapText );
        len1 = strlen( SetCursorText );
        if( len1 > len )
            len = len1;
        len1 = strlen( SetIconText );
        if( len1 > len )
            len = len1;
        len += strlen( imgSizePosition ) + 30 + 1;
        ImageText = (char *)MemAlloc( len );
    }

    StatusWndInit( Instance, (statushook)NULL, sizeof( LPVOID ), NULL );
    statusBar = StatusWndStart();

    GetClientRect( parent, &client );

#ifdef __OS2_PM__
    rcsize.xLeft = -1;
    rcsize.xRight = client.xRight + 1;
    rcsize.yBottom = -1;
    rcsize.yTop = INIT_STATUS_WIDTH - 1;
#else
    rcsize.left = -1;
    rcsize.right = client.right + 1;
    rcsize.bottom = client.bottom + 1;
    rcsize.top = client.bottom - INIT_STATUS_WIDTH + 1;
#endif

    sbd[0].separator_width = 5;
    sbd[0].width = 82;
    sbd[0].width_is_percent = 0;
    sbd[0].width_is_pixels = 1;

    sbd[1].separator_width = 5;
    sbd[1].width = 168;
    sbd[1].width_is_percent = 0;
    sbd[1].width_is_pixels = 1;

    sbd[2].separator_width = 5;
    sbd[2].width = 255;
    sbd[2].width_is_percent = 0;
    sbd[2].width_is_pixels = 1;

    sbd[3].separator_width = 5;
    sbd[3].width = 400;
    sbd[3].width_is_percent = 0;
    sbd[3].width_is_pixels = 1;

    StatusWndSetSeparators( statusBar, 4, &sbd );

    strcpy( imgSizePosition, next_block );
    strcat( imgSizePosition, next_block );
    strcat( imgSizePosition, next_block );
    strcat( imgSizePosition, leftBlock );

    strcpy( hintTextPosition, next_block );
    strcat( hintTextPosition, next_block );
    strcat( hintTextPosition, next_block );
    strcat( hintTextPosition, next_block );
    strcat( hintTextPosition, leftBlock );

    statusBarWnd = StatusWndCreate( statusBar, parent, &rcsize, Instance, (LPVOID)NULL );

    text = NULL;
    if( PosText != NULL && SizeText != NULL ) {
        text = (char *)MemAlloc( strlen( PosText ) + strlen( SizeText ) +
                                 strlen( leftBlock ) + strlen( nextBlock ) + 1 );
    }

    if( text != NULL ) {
        sprintf( text, "%s%s%s%s", leftBlock, PosText, nextBlock, SizeText );
        pres = _wpi_getpres( statusBarWnd );
        StatusWndDrawLine( statusBar, pres, SmallFont, text, -1 );
        _wpi_releasepres( statusBarWnd, pres );
        MemFree( text );
    }

    GetWindowRect( statusBarWnd, &rcsize );
    StatusWidth = rcsize.bottom - rcsize.top;

    return( TRUE );

} /* InitStatusLine */

/*
 * SetPosInStatus - set the position in the status window
 */
void SetPosInStatus( WPI_POINT *pt, WPI_POINT *pointsize, HWND hwnd )
{
    WPI_PRES    pres;
    WPI_RECT    rcclient;
    int         x;
    int         y;

    GetClientRect( hwnd, &rcclient );

    if( _wpi_ptinrect( &rcclient, *pt ) && PositionText != NULL && SetPosText != NULL ) {
        if( pointsize->x == 0 ) {
            pointsize->x = 1;
        }
        if( pointsize->y == 0 ) {
            pointsize->y = 1;
        }
        x = pt->x / pointsize->x;
        y = pt->y / pointsize->y;

        sprintf( PositionText, SetPosText, leftBlock, x, y );
        pres = _wpi_getpres( statusBarWnd );
        StatusWndDrawLine( statusBar, pres, SmallFont, PositionText, -1 );
        _wpi_releasepres( statusBarWnd, pres );
    }

} /* SetPosInStatus */

/*
 * SetSizeInStatus - set the size in the status line
 */
void SetSizeInStatus( HWND hwnd, WPI_POINT *startpt, WPI_POINT *endpt, WPI_POINT *pointsize )
{
    WPI_POINT   pt1;
    WPI_POINT   pt2;
    WPI_POINT   pos;
    int         width;
    int         height;
    WPI_PRES    pres;

    if( SetSizeText == NULL || PositionSizeText == NULL ) {
        return;
    }

    CheckBounds( hwnd, startpt );
    CheckBounds( hwnd, endpt );

    pt1.x = startpt->x;
    if( pt1.x > endpt->x )
        pt1.x = endpt->x;
    pt1.x /= pointsize->x;
    pt1.y = startpt->y;
    if( pt1.y > endpt->y )
        pt1.y = endpt->y;
    pt1.y /= pointsize->y;
    pt2.x = startpt->x;
    if( pt2.x < endpt->x )
        pt2.x = endpt->x;
    pt2.x /= pointsize->x;
    pt2.y = startpt->y;
    if( pt2.y < endpt->y )
        pt2.y = endpt->y;
    pt2.y /= pointsize->y;

    width = pt2.x - pt1.x + 1;
    height = pt2.y - pt1.y + 1;

    pos.x = endpt->x / pointsize->x;
    pos.y = endpt->y / pointsize->y;
    sprintf( PositionSizeText, SetSizeText, leftBlock, pos.x, pos.y, nextBlock, width, height );
    pres = _wpi_getpres( statusBarWnd );
    StatusWndDrawLine( statusBar, pres, SmallFont, PositionSizeText, -1 );
    _wpi_releasepres( statusBarWnd, pres );

} /* SetSizeInStatus */

/*
 * ResizeStatusBar - resize the status bar
 */
void ResizeStatusBar( WPI_PARAM2 lparam )
{
    short       width;
    short       height;
    short       y;

    if( statusBarWnd == NULL ) {
        return;
    }

#ifdef __OS2_PM__
    width = SHORT1FROMMP( lparam ) + 2;
    height = height;
    y = -1;
#else
    width = (int_16)LOWORD( lparam ) + 2;
    height = (int_16)HIWORD( lparam );
    y = height - StatusWidth + 1;
    if( y < FUNCTIONBAR_WIDTH )
        y = FUNCTIONBAR_WIDTH;
#endif
    _wpi_setwindowpos( statusBarWnd, HWND_TOP, -1, y, width, StatusWidth, SWP_SHOWWINDOW | SWP_MOVE | SWP_SIZE );

} /* ResizeStatusBar */

/*
 * FiniStatusLine - close up the status line
 */
void FiniStatusLine( void )
{
    DestroyWindow( statusBarWnd );
    StatusWndDestroy( statusBar );
    StatusWndFini();
    if( PosText != NULL ) {
        IEFreeRCString( PosText );
    }
    if( SizeText != NULL ) {
        IEFreeRCString( SizeText );
    }
    if( SetPosText != NULL ) {
        IEFreeRCString( SetPosText );
    }
    if( SetHotSpotText != NULL ) {
        IEFreeRCString( SetHotSpotText );
    }
    if( SetBitmapText != NULL ) {
        IEFreeRCString( SetBitmapText );
    }
    if( SetCursorText != NULL ) {
        IEFreeRCString( SetCursorText );
    }
    if( SetIconText != NULL ) {
        IEFreeRCString( SetIconText );
    }
    if( PositionText != NULL ) {
        MemFree( PositionText );
    }
    if( HotSpotText != NULL ) {
        MemFree( HotSpotText );
    }
    if( SetSizeText != NULL ) {
        MemFree( SetSizeText );
    }
    if( PositionSizeText != NULL ) {
        MemFree( PositionSizeText );
    }
    if( ImageText != NULL ) {
        MemFree( ImageText );
    }

} /* FiniStatusLine */

/*
 * SetHotSpot - set the text in the hot spot window
 */
void SetHotSpot( img_node *node )
{
    WPI_PRES    pres;

    if( HotSpotText == NULL || SetHotSpotText == NULL ) {
        return;
    }

#ifdef __OS2_PM__
    if( node->imgtype == CURSOR_IMG || node->imgtype == ICON_IMG ) {
#else
    if( node->imgtype == CURSOR_IMG ) {
#endif
        sprintf( HotSpotText, SetHotSpotText, hotspotPosition,
                 node->hotspot.x, node->hotspot.y );
    } else {
        sprintf( HotSpotText, "%s ", hotspotPosition );
    }

    pres = _wpi_getpres( statusBarWnd );
    StatusWndDrawLine( statusBar, pres, SmallFont, HotSpotText, -1 );
    _wpi_releasepres( statusBarWnd, pres );

} /* SetHotSpot */

/*
 * DisplayImageText - display the text in the status window giving the
 *                    image type size and color scheme
 */
void DisplayImageText( img_node *node )
{
    WPI_PRES    pres;

    switch( node->imgtype ) {
    case BITMAP_IMG:
        sprintf( ImageText, SetBitmapText, imgSizePosition,
                 node->width, node->height, 1 << node->bitcount );
        break;

    case ICON_IMG:
        sprintf( ImageText, SetIconText, imgSizePosition,
                 node->width, node->height, 1 << node->bitcount );
        break;

    case CURSOR_IMG:
        sprintf( ImageText, SetCursorText, imgSizePosition,
                 node->width, node->height, 1 << node->bitcount );
        break;

    default:
        return;
    }

    pres = _wpi_getpres( statusBarWnd );
    StatusWndDrawLine( statusBar, pres, SmallFont, ImageText, -1 );
    _wpi_releasepres( statusBarWnd, pres );

} /* DisplayImageText */

/*
 * ClearImageText - clear the text in the image size section
 */
void ClearImageText( void )
{
    WPI_PRES    pres;
    char        text[20];

    strcpy( text, imgSizePosition );
    strcat( text, "    " );

    pres = _wpi_getpres( statusBarWnd );
    StatusWndDrawLine( statusBar, pres, SmallFont, text, -1 );
    _wpi_releasepres( statusBarWnd, pres );

} /* ClearImageText */

/*
 * IEPrintAmtText
 */
void IEPrintAmtText( DWORD message, int amt )
{
    char        *text;
    char        *msg;

    text = IEAllocRCString( message );
    if( text != NULL ) {
        msg = (char *)MemAlloc( strlen( text ) + 10 + 1 );
        if( msg != NULL ) {
            sprintf( msg, text, amt );
            SetHintText( msg );
            MemFree( msg );
        }
        IEFreeRCString( text );
    }

} /* IEPrintAmtText */

/*
 * WriteSetSizeText
 */
void WriteSetSizeText( DWORD msg, int x, int y )
{
    char        *text;
    char        *msg_text;

    text = IEAllocRCString( msg );
    if( text != NULL ) {
        msg_text = (char *)MemAlloc( strlen( text ) + 20 + 1 );
        if( msg_text != NULL ) {
            sprintf( msg_text, text, x, y );
            SetHintText( msg_text );
            MemFree( msg_text );
        }
        IEFreeRCString( text );
    }

} /* WriteSetSizeText */

/*
 * PrintHintTextByID
 */
void PrintHintTextByID( DWORD id, char *fname )
{
    char        *msg;

    msg = IEAllocRCString( id );
    if( msg != NULL ) {
        PrintHintText( msg, fname );
        IEFreeRCString( msg );
    }

} /* PrintHintTextByID */

/*
 * PrintHintText
 */
void PrintHintText( char *msg, char *fname )
{
    char        *text;

    if( msg != NULL ) {
        if( fname != NULL ) {
            text = (char *)MemAlloc( strlen(msg) + strlen( fname ) + 1 );
            if( text != NULL ) {
                sprintf( text, msg, fname );
                SetHintText( text );
                MemFree( text );
            }
        } else {
            SetHintText( msg );
        }
    }

} /* PrintHintText */

/*
 * SetHintText - set the hint text
 */
void SetHintText( char *msg )
{
    char        *text;
    int         len;
    WPI_PRES    pres;

    len = strlen( hintTextPosition ) + 1;
    if( msg != NULL ) {
        len += strlen( msg );
    } else {
        len++;
    }

    text = (char *)MemAlloc( len );
    if( text != NULL ) {
        strcpy( text, hintTextPosition );
        if( msg != NULL ) {
            strcat( text, msg );
        } else {
            strcat( text, " " );
        }
        pres = _wpi_getpres( statusBarWnd );
        StatusWndDrawLine( statusBar, pres, SmallFont, text, -1 );
        _wpi_releasepres( statusBarWnd, pres );
        MemFree( text );
    }

} /* SetHintText */
