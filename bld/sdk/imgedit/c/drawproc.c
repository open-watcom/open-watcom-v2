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
#include "drawproc.h"

static short            toolType = 0;
static short            cursorIndex = 0;
static HCURSOR          hCursor[NUMBER_OF_CURSORS];

/*
 * lastChanceSave - called when the user quits and the current image
 *                  is not yet saved
 *                - return FALSE if CANCEL is selected
 *                - otherwise, it return TRUE
 */
static BOOL lastChanceSave( HWND hwnd )
{
    int         retcode;
    int         how;
    HMENU       hmenu;
    img_node    *node;
    img_node    *icon;
    char        *title;
    char        *text;
    char        *msg_text;
    char        filename[_MAX_PATH];

    if( !DoImagesExist() ) {
        return( TRUE );
    }

    node = SelectImage( hwnd );
    if( node == NULL ) {
        return( TRUE );
    }

    for( icon = GetImageNode( hwnd ); icon != NULL; icon = icon->nexticon ) {
        if( icon->issaved ) {
            return( TRUE );
        }
    }

    if( strnicmp( node->fname, IEImageUntitled, strlen( IEImageUntitled ) ) != 0 ) {
        GetFnameFromPath( node->fname, filename );
        how = SB_SAVE;
    } else {
        strcpy( filename, node->fname );
        how = SB_SAVE_AS;
    }

    retcode = WPI_IDCANCEL;
    title = IEAllocRCString( WIE_CLOSETITLE );
    text = IEAllocRCString( WIE_QUERYIMAGESAVE );
    if( text != NULL ) {
        msg_text = (char *)MemAlloc( strlen( text ) + strlen( filename ) + 1 );
        if( msg_text != NULL ) {
            sprintf( msg_text, text, filename );
            retcode = _wpi_messagebox( HMainWindow, msg_text, title,
                                       MB_YESNOCANCEL | MB_ICONQUESTION );
            MemFree( msg_text );
        }
        IEFreeRCString( text );
    }
    if( title != NULL ) {
        IEFreeRCString( title );
    }

    if( retcode == WPI_IDYES ) {
        if( !SaveFile( how ) ) {
            PrintHintTextByID( WIE_FILENOTSAVED, NULL );
            return( FALSE );
        } else {
            hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );
            _wpi_enablemenuitem( hmenu, IMGED_SAVE, FALSE, FALSE );
            SetIsSaved( hwnd, TRUE );
        }
    } else if( retcode == WPI_IDCANCEL ) {
        return( FALSE );
    }
    return( TRUE );

} /* lastChanceSave */

/*
 * setTheCursor - set the cursor to be appropriate for the given window
 */
static void setTheCursor( int cursor_index, HWND hwnd )
{
#ifndef __OS2_PM__
    img_node    *node;

    if( hwnd != NULL ) {
        SET_CLASSCURSOR( hwnd, hCursor[cursorIndex] );
        return;
    }

    cursorIndex = cursor_index;
    for( node = GetHeadNode(); node != NULL; node = node->next ) {
        SET_CLASSCURSOR( node->hwnd, hCursor[cursorIndex] );
    }
#else
    hwnd = hwnd;
    cursorIndex = cursor_index;
#endif

} /* setTheCursor */

/*
 * DrawAreaWinProc - window procedure for the drawing area window
 *                 - this window is an MDI window
 */
WPI_MRESULT CALLBACK DrawAreaWinProc( HWND hwnd, WPI_MSG msg,
                                  WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    static WPI_POINT    start_pt = { -1, -1 };
    static WPI_POINT    end_pt = { -1, -1 };
    static WPI_POINT    prev_pt = { -1, -1 };
    static WPI_POINT    new_pt = { -1, -1 };
    static WPI_POINT    pt1;
    static BOOL         flbuttondown = FALSE;
    static BOOL         frbuttondown = FALSE;
    static BOOL         fdraw_shape = FALSE;
    static BOOL         firsttime;
    static BOOL         wasicon;
    int                 mousebutton;
    WPI_POINT           pointsize;
    WPI_POINT           pt2;
#ifndef __OS2_PM__
    CREATESTRUCT        *cs;
    MDICREATESTRUCT     *mdi_cs;
    MINMAXINFO          *minmax;
#endif
    img_node            *node;
    int                 i;

    switch ( msg ) {
    case WM_CREATE:
#ifndef __OS2_PM__
        cs = (CREATESTRUCT __FAR *)lparam;
        mdi_cs = (MDICREATESTRUCT __FAR *)cs->lpCreateParams;
        node = (img_node *)mdi_cs->lParam;

        for( i = 0; i < node->num_of_images; i++ ) {
            node[i].hwnd = hwnd;
        }
        AddImageNode( node );
        CreateUndoStack( node );
        setTheCursor( -1, hwnd );
        wasicon = FALSE;
#endif
        firsttime = TRUE;
        i = i;
        return( 0 );

    case WM_MDIACTIVATE:
        if( IMGED_GET_MDI_FACTIVATE( hwnd, wparam, lparam ) ) {
            if( _wpi_isiconic( hwnd ) ) {
                wasicon = TRUE;
                break;
            } else {
                wasicon = FALSE;
                FocusOnImage( hwnd );
            }
        }
        return( 0 );

#ifndef __OS2_PM__
    case WM_GETMINMAXINFO:
        node = SelectImage( hwnd );
        if( node != NULL ) {
            minmax = (MINMAXINFO *)lparam;
            minmax->ptMinTrackSize.x = node->width +
                2 * _wpi_getsystemmetrics( SM_CXFRAME );
            minmax->ptMinTrackSize.y = node->height +
                2 * _wpi_getsystemmetrics( SM_CYFRAME ) +
                _wpi_getsystemmetrics( SM_CYCAPTION ) - 1;
        }
        break;
#endif

    case UM_SHOWVIEWWINDOW:
        ShowViewWindows( hwnd );
        return 0;

    case WM_SIZE:
        if ( _imgwpi_issizerestored( wparam ) ) {
            ResizeChild( lparam, hwnd, firsttime );
            firsttime = FALSE;
            if( wasicon ) {
                FocusOnImage( hwnd );
                wasicon = FALSE;
            }
#ifndef __OS2_PM__
        } else if ( _imgwpi_issizeminimized( wparam ) ) {
            node = SelectImage( hwnd );
            HideViewWindow( hwnd );
            DeleteActiveImage();
            ClearImageText();
            SendMessage( ClientWindow, WM_MDINEXT, (WPARAM)(LPVOID)hwnd, 0L );
            wasicon = TRUE;
#endif
        }
        return( 0 );

#ifndef __OS2_PM__
    case WM_CHAR:
        pointsize = GetPointSize( hwnd );
        if( LOWORD( wparam ) == ESC_CHAR ) {
            if( !(lparam & 0x40000000) ) {
                switch( toolType ) {
                case IMGED_PASTE:
                    fdraw_shape = FALSE;
                    flbuttondown = FALSE;
                    frbuttondown = FALSE;
                    DontPaste( hwnd, &pt1, pointsize );
                    break;

                case IMGED_LINE:
                case IMGED_RECTO:
                case IMGED_RECTF:
                case IMGED_CIRCLEO:
                case IMGED_CIRCLEF:
                case IMGED_CLIP:
                    if( !flbuttondown && !frbuttondown ) {
                        break;
                    }
                    fdraw_shape = FALSE;
                    flbuttondown = FALSE;
                    frbuttondown = FALSE;
                    if( toolType == IMGED_LINE ) {
                        OutlineLine( hwnd, &start_pt, &prev_pt, &new_pt, TRUE );
                    } else if( toolType == IMGED_CLIP ) {
                        OutlineClip( hwnd, &start_pt, &prev_pt, &new_pt, TRUE );
                        SetRectExists( FALSE );
                    } else {
                        OutlineRegion( hwnd, &start_pt, &prev_pt, &new_pt, TRUE );
                    }
                    break;
                default:
                    break;
                }
            }
        }
        return( 0 );
#endif

    case WM_MOUSEMOVE:
        pointsize = GetPointSize( hwnd );
#ifdef __OS2_PM__
        WinSetPointer( HWND_DESKTOP, hCursor[cursorIndex] );
#endif
        IMGED_MAKEPOINT( wparam, lparam, new_pt );

        switch( toolType ) {
        case IMGED_SNAP:
#ifndef __OS2_PM__
            OutlineSnap();
#endif
            break;

        case IMGED_PASTE:
            pt1.x = new_pt.x / pointsize.x;
            pt1.y = new_pt.y / pointsize.y;
            pt2.x = prev_pt.x / pointsize.x;
            pt2.y = prev_pt.y / pointsize.y;
            if( pt1.x != pt2.x || pt1.y != pt2.y ) {
                DragClipBitmap( hwnd, &pt1, pointsize );
            }
            SetPosInStatus( &new_pt, &pointsize, hwnd );
            break;

        case IMGED_FREEHAND:
        case IMGED_BRUSH:
            if( flbuttondown ) {
                Paint( hwnd, &prev_pt, &new_pt, LMOUSEBUTTON );
            } else if( frbuttondown ) {
                Paint( hwnd, &prev_pt, &new_pt, RMOUSEBUTTON );
            }
            SetPosInStatus( &new_pt, &pointsize, hwnd );
            break;

        case IMGED_LINE:
        case IMGED_RECTO:
        case IMGED_RECTF:
        case IMGED_CIRCLEO:
        case IMGED_CIRCLEF:
        case IMGED_CLIP:
            if( flbuttondown || frbuttondown ) {
                if( new_pt.x / pointsize.x != end_pt.x / pointsize.x ||
                    new_pt.y / pointsize.y != end_pt.y / pointsize.y ) {
                    if( toolType == IMGED_LINE ) {
                        OutlineLine( hwnd, &start_pt, &new_pt, &prev_pt, FALSE );
                    } else if( toolType == IMGED_CLIP ) {
                        OutlineClip( hwnd, &start_pt, &new_pt, &prev_pt, FALSE );
                    } else {
                        OutlineRegion( hwnd, &start_pt, &new_pt, &prev_pt, FALSE );
                    }
                    end_pt = new_pt;
                    SetSizeInStatus( hwnd, &start_pt, &new_pt, &pointsize );
                }
            } else {
                SetPosInStatus( &new_pt, &pointsize, hwnd );
            }
            break;

        case IMGED_FILL:
            SetPosInStatus( &new_pt, &pointsize, hwnd );
            break;

        default:
            SetPosInStatus( &new_pt, &pointsize, hwnd );
            break;
        }
        prev_pt = new_pt;
        return( 0 );

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        pointsize = GetPointSize( hwnd );
        SetCapture( hwnd );
        if( msg == WM_LBUTTONDOWN ) {
            if( frbuttondown ) {
                break;
            }
#ifdef __OS2_PM__
            node = GetCurrentNode();
            if( hwnd != node->hwnd ) {
                break;
            }
#endif
            flbuttondown = TRUE;
            mousebutton = LMOUSEBUTTON;
        } else {
            if( flbuttondown ) {
                break;
            }
            node = GetCurrentNode();
            if( hwnd != node->hwnd ) {
                break;
            }
            frbuttondown = TRUE;
            mousebutton = RMOUSEBUTTON;
        }
        fdraw_shape = TRUE;
        IMGED_MAKEPOINT( wparam, lparam, start_pt );
        IMGED_MAKEPOINT( wparam, lparam, new_pt );

        switch( toolType ) {
        case IMGED_SNAP:
#ifndef __OS2_PM__
            TransferImage( hwnd );
#endif
            fdraw_shape = FALSE;
            flbuttondown = FALSE;
            frbuttondown = FALSE;
            break;

        case IMGED_FREEHAND:
        case IMGED_BRUSH:
            BeginFreeHand( hwnd );
            DrawSinglePoint( hwnd, &start_pt, mousebutton );
            break;

        case IMGED_LINE:
            IMGED_MAKEPOINT( wparam, lparam, end_pt );
            OutlineLine( hwnd, &start_pt, &new_pt, &prev_pt, TRUE );
            break;

        case IMGED_RECTO:
        case IMGED_RECTF:
        case IMGED_CIRCLEO:
        case IMGED_CIRCLEF:
            IMGED_MAKEPOINT( wparam, lparam, end_pt );
            OutlineRegion( hwnd, &start_pt, &new_pt, &prev_pt, TRUE );
            break;

        case IMGED_CLIP:
            IMGED_MAKEPOINT( wparam, lparam, end_pt );
            RedrawPrevClip( hwnd );
            OutlineClip( hwnd, &start_pt, &new_pt, &prev_pt, TRUE );
            break;

        case IMGED_FILL:
            FillArea( &start_pt, mousebutton );
            RecordImage( hwnd );
            break;

        case IMGED_HOTSPOT:
            SetIsSaved( hwnd, FALSE );
            prev_pt.x = start_pt.x / pointsize.x;
            prev_pt.y = start_pt.y / pointsize.y;
            SetNewHotSpot( &prev_pt );
            break;

        default:
            break;
        }
        prev_pt = new_pt;
        return 0;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        pointsize = GetPointSize( hwnd );
        IMGED_MAKEPOINT( wparam, lparam, end_pt );
        IMGED_MAKEPOINT( wparam, lparam, new_pt );
        if( msg == WM_LBUTTONUP ) {
            mousebutton = LMOUSEBUTTON;
        } else {
            mousebutton = RMOUSEBUTTON;
        }

        if( fdraw_shape ) {
            switch( toolType ) {
            case IMGED_PASTE:
                PasteImage( &start_pt, pointsize, hwnd );
                flbuttondown = FALSE;
                frbuttondown = FALSE;
                break;

            case IMGED_FREEHAND:
            case IMGED_BRUSH:
                if( flbuttondown || frbuttondown ) {
                    EndFreeHand( hwnd );
                    RecordImage( hwnd );
                }
                break;

            case IMGED_LINE:
                DrawLine( hwnd, &start_pt, &end_pt, mousebutton );
                RecordImage( hwnd );
                break;

            case IMGED_RECTO:
            case IMGED_RECTF:
            case IMGED_CIRCLEO:
            case IMGED_CIRCLEF:
                DisplayRegion( hwnd, &start_pt, &end_pt, mousebutton );
                RecordImage( hwnd );
                break;

            case IMGED_CLIP:
                SetClipRect( hwnd, &start_pt, &end_pt, pointsize );
                break;

            default:
                break;
            }
            fdraw_shape = FALSE;
        }
        ReleaseCapture();
        flbuttondown = FALSE;
        frbuttondown = FALSE;
        prev_pt = new_pt;
        return( 0 );

    case WM_PAINT:
        RepaintDrawArea( hwnd );
        return( 0 );

    case WM_QUERYENDSESSION:
    case WM_CLOSE:
        if( lastChanceSave( hwnd ) ) {
            CloseCurrentImage( hwnd );
            return( (WPI_MRESULT)1 );
        } else {
            return( (WPI_MRESULT)0 );
        }

#ifndef __OS2_PM__
    case WM_DESTROY:
        SetCursor( LoadCursor( NULL, IDC_ARROW ) );
        return( 0 );
#endif

    default:
        break;
    }
    return( _imgwpi_defMDIchildproc( hwnd, msg, wparam, lparam ) );

} /* DrawAreaWinProc */

/*
 * SetToolType - set the tool type
 */
int SetToolType( int toolid )
{
    HMENU       hmenu;
    int         i;
    int         prev_tool;
    img_node    *node;

    if( toolType == IMGED_CLIP ) {
        node = GetCurrentNode();
        if( node != NULL ) {
            RedrawPrevClip( node->hwnd );
            SetRectExists( FALSE );
        }
    }

    prev_tool = toolType;
    toolType = toolid;

    if( toolid == IMGED_PASTE || toolid == IMGED_SNAP ) {
        return( prev_tool );
    }

    if( HMainWindow == NULL ) {
        return( 0 );
    }
    hmenu = _wpi_getmenu( _wpi_getframe( HMainWindow ) );

    switch( toolid ) {
    case IMGED_FREEHAND:
        setTheCursor( PENCIL_CUR, NULL );
        break;
    case IMGED_LINE:
        setTheCursor( CROSS_CUR, NULL );
        break;
    case IMGED_HOTSPOT:
        setTheCursor( HOTSPOT_CUR, NULL );
        break;
    case IMGED_RECTO:
        setTheCursor( CROSS_CUR, NULL );
        break;
    case IMGED_RECTF:
        setTheCursor( CROSS_CUR, NULL );
        break;
    case IMGED_CIRCLEO:
        setTheCursor( CROSS_CUR, NULL );
        break;
    case IMGED_CIRCLEF:
        setTheCursor( CROSS_CUR, NULL );
        break;
    case IMGED_FILL:
        setTheCursor( FILL_CUR, NULL );
        break;
    case IMGED_BRUSH:
        setTheCursor( BRUSH_CUR, NULL );
        if( prev_tool != IMGED_PASTE && prev_tool !=IMGED_SNAP ) {
            WriteSetSizeText( WIE_BRUSHSIZEIS, ImgedConfigInfo.brush_size,
                              ImgedConfigInfo.brush_size );
        }
        break;
    case IMGED_CLIP:
        setTheCursor( CROSS_CUR, NULL );
        break;

    default:
        setTheCursor( PENCIL_CUR, NULL );
        toolType = IMGED_FREEHAND;
        break;
    }

    for( i = IMGED_CLIP; i <= IMGED_HOTSPOT; i++ ) {
#if 0
        if( _wpi_isitemenabled( hmenu, i ) ) {
            _wpi_checkmenuitem( hmenu, i, FALSE, FALSE );
        }
#else
        if( _wpi_isitemchecked( hmenu, i ) ) {
            _wpi_checkmenuitem( hmenu, i, FALSE, FALSE );
        }
#endif
    }
    _wpi_checkmenuitem( hmenu, toolType, TRUE, FALSE );

    SetDrawTool( toolType );
    return( prev_tool );

} /* SetToolType */

/*
 * InitializeCursors - initialize the cursors
 *                   - should be cleaned up with CleanupCursors
 */
void InitializeCursors( void )
{
    hCursor[PENCIL_CUR] = _wpi_loadcursor( Instance, PENCILCURSOR );
    hCursor[CROSS_CUR] = _wpi_loadcursor( Instance, CROSSHAIRSCUR );
    hCursor[FILL_CUR] = _wpi_loadcursor( Instance, FILLCURSOR );
    hCursor[BRUSH_CUR] = _wpi_loadcursor( Instance, PAINTBRUSHCUR );
    hCursor[HOTSPOT_CUR] = _wpi_loadcursor( Instance, HOTSPOTCUR );

} /* InitializeCursors */

/*
 * CleanupCursors - clean up the initialized cursors
 */
void CleanupCursors( void )
{
    _wpi_destroycursor( hCursor[PENCIL_CUR] );
    _wpi_destroycursor( hCursor[CROSS_CUR] );
    _wpi_destroycursor( hCursor[FILL_CUR] );
    _wpi_destroycursor( hCursor[BRUSH_CUR] );
    _wpi_destroycursor( hCursor[HOTSPOT_CUR] );

} /* CleanupCursors */
