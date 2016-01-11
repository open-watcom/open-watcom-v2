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


#include "bool.h"
#include "imgedit.h"
#include "iconinfo.h"
#include "ieprofil.h"
#include "aboutdlg.h"
#include "iedde.h"
#include "banner.h"

#include "wwinhelp.h"

extern BOOL FusionCalled;

static BOOL ImgEdEnableMenuInput = FALSE;

/*
 * IEEnableMenuInput
 */
void IEEnableMenuInput( BOOL enable )
{
    ImgEdEnableMenuInput = enable;

} /* IEEnableMenuInput */

/*
 * enableMainItems - enable menu items
 */
static void enableMainItems( HMENU hmenu )
{
    img_node    *node;

    if( hmenu == NULL ) {
        return;
    }

    node = GetCurrentNode();
    if( !DoImagesExist() ) {
        _wpi_enablemenuitem( hmenu, IMGED_SAVE, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SAVE_AS, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_CLOSE, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_CLOSEALL, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_ARRANGE, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_TILE, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_CASCADE, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_ROTATECC, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_ROTATECL, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_FLIPHORZ, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_FLIPVERT, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_UP, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_DOWN, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_RIGHT, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_LEFT, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SIZE, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_MAXIMIZE, FALSE, FALSE );
    } else if( node != NULL ) {
        CheckForClipboard( hmenu );
        CheckForUndo( node );
        _wpi_enablemenuitem( hmenu, IMGED_SAVE, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SAVE_AS, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_CLOSE, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_CLOSEALL, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_ARRANGE, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_TILE, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_CASCADE, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_ROTATECC, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_ROTATECL, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_FLIPHORZ, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_FLIPVERT, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_UP, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_DOWN, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_RIGHT, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_LEFT, TRUE, FALSE );
        if( node->imgtype != BITMAP_IMG ) {
            _wpi_enablemenuitem( hmenu, IMGED_SIZE, FALSE, FALSE );
        } else {
            _wpi_enablemenuitem( hmenu, IMGED_SIZE, TRUE, FALSE );
        }
        _wpi_enablemenuitem( hmenu, IMGED_MAXIMIZE, TRUE, FALSE );
    } else {
        _wpi_enablemenuitem( hmenu, IMGED_SAVE, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SAVE_AS, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_CLOSE, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_SIZE, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_CLOSEALL, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_ARRANGE, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_TILE, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_CASCADE, TRUE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_MAXIMIZE, FALSE, FALSE );
    }

    if( FusionCalled ) {
        _wpi_enablemenuitem( hmenu, IMGED_NEW, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_OPEN, FALSE, FALSE );
    }

} /* enableMainItems */

#ifndef __OS2_PM__

/*
 * createClientWindow - create the client window required by MDI
 */
void createClientWindow( HWND hwnd )
{
    CLIENTCREATESTRUCT  ccs;
    RECT                clientrect;
    short               height;

    GetClientRect( hwnd, &clientrect );
    height = clientrect.bottom - FUNCTIONBAR_WIDTH - StatusWidth;

    ccs.hWindowMenu = GetSubMenu( GetMenu( hwnd ), 5 );
    ccs.idFirstChild = IDM_FIRSTCHILD;
    ClientWindow = CreateWindow( "mdiclient", NULL,
                                 WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_HSCROLL |
                                 WS_VSCROLL | MDIS_ALLCHILDSTYLES,
                                 0, FUNCTIONBAR_WIDTH + 1, clientrect.right, height,
                                 hwnd, 0, Instance, (LPSTR)&ccs );

} /* createClientWindow */

/*
 * setClientSize - set the new size of the client area
 */
static void setClientSize( HWND hwnd )
{
    RECT        rcclient;
    short       height;
    int         func_height;

    func_height = GetFunctionBarHeight();
    GetClientRect( hwnd, &rcclient );
    height = rcclient.bottom - func_height - StatusWidth;

    MoveWindow( ClientWindow, 0, func_height, rcclient.right, height, TRUE );

} /* setClientSize */

#else

/*
 * resizeClientArea - resize the current area of the client
 */
static void resizeClientArea( WPI_PARAM2 mp2 )
{
    SHORT       width;
    SHORT       height;

    if( ClientWindow == NULL ) {
        return;
    }

    width = SHORT1FROMMP( mp2 );
    height = SHORT2FROMMP( mp2 );

    WinSetWindowPos( ClientWindow, HWND_TOP, 0, STATUS_WIDTH, width,
                     height - STATUS_WIDTH - FUNCTIONBAR_WIDTH, SWP_SIZE | SWP_MOVE );

} /* resizeClientArea */

#endif

/*
 * checkBrushItem - check the brush size item in the main menu
 */
static void checkBrushItem( HMENU hmenu, int newitem )
{
    int         i;
    int         brush_size = 2;

    for(i = IMGED_2x2; i <= IMGED_5x5; i++ ) {
        _wpi_checkmenuitem( hmenu, i, FALSE, FALSE );
        if( i == newitem ) {
            SetBrushSize( brush_size );
        }
        brush_size++;
    }
    _wpi_checkmenuitem( hmenu, newitem, TRUE, FALSE );

} /* checkBrushItem */

/*
 * IEIsMenuIDValid
 */
static BOOL IEIsMenuIDValid( HMENU menu, int id )
{
    UINT st;

    if( !ImgEdEnableMenuInput ) {
        return( FALSE );
    }

    if( menu == (HMENU)NULL ) {
        return( TRUE );
    }

    // put any menu identifiers that you would like forced here
    switch( id ) {
    case IMGED_CLOSEALL:
        if( ImgedIsDDE ) {
            return( TRUE );
        }
        break;
    }

    st = GetMenuState( menu, id, MF_BYCOMMAND );

    if( st == -1 || (st & MF_GRAYED) == MF_GRAYED ) {
        return( FALSE );
    }

    return( TRUE );

} /* IEIsMenuIDValid */

/*
 * ImgEdFrameProc - handle messages for the image editor application
 */
WPI_MRESULT CALLBACK ImgEdFrameProc( HWND hwnd, WPI_MSG msg,
                                 WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    static BOOL         window_destroyed = FALSE;
    static HMENU        hmenu;
    int                 cmdid;
    img_node            *node;
    WPI_RECT            rcmain;
#ifndef __OS2_PM__
    about_info          ai;
#endif
    WPI_RECTDIM         left, top;

    if( !window_destroyed ) {
        enableMainItems( hmenu );
    }

    switch( msg ) {
    case UM_EXIT:
        _wpi_sendmessage( hwnd, WM_COMMAND, IMGED_CLOSEALL, 0L );
        /* fall through */

    case UM_EXIT_NO_SAVE:
        if( _wpi_getfirstchild( _wpi_getclient( ClientWindow ) ) != NULL ) {
            break;
        }
#ifndef __OS2_PM__
        _wpi_destroywindow( _wpi_getframe( hwnd ) );
#else
        _wpi_sendmessage( hwnd, WM_CLOSE, 0, 0 );
#endif

        break;

    case UM_SAVE_ALL:
        SaveAllImages();
        break;

    case WM_CREATE:
        hmenu = _wpi_getmenu( _wpi_getframe( hwnd ) );
#ifndef __OS2_PM__
        createClientWindow( hwnd );
#endif
        if( !InitStatusLine( hwnd ) ) {
            return( -1 );
        }

        InitFunctionBar( hwnd );
        InitIconInfo();
        InitializeCursors();

        /*
         * Set values from profile information ...
         */
        if( ImgedConfigInfo.brush_size <= 5 && ImgedConfigInfo.brush_size >= 2 ) {
            checkBrushItem( hmenu, IMGED_2x2 - 2 + ImgedConfigInfo.brush_size );
        }
        if( ImgedConfigInfo.grid_on ) {
            CheckGridItem( hmenu );
        }
        if( ImgedConfigInfo.square_grid ) {
            CheckSquareGrid( hmenu );
        }
        if( ImgedConfigInfo.show_state & SET_SHOW_VIEW ) {
            CheckViewItem( hmenu );
        }

        _wpi_enablemenuitem( hmenu, IMGED_CRESET, FALSE, FALSE );
        _wpi_enablemenuitem( hmenu, IMGED_RCOLOR, FALSE, FALSE );
#ifndef __OS2_PM__
        // not necessary for PM
        InitMenus( hmenu );
#endif
        SetHintText( IEAppTitle );
        return( 0 );
#ifdef __NT__
    case WM_DROPFILES:
        OpenImage( (HANDLE)wparam );
        break;
#endif
    case WM_MOVE:
        _wpi_getwindowrect( hwnd, &rcmain );
        if( !ImgedConfigInfo.ismaximized ) {
            ImgedConfigInfo.last_xpos = ImgedConfigInfo.x_pos;
            ImgedConfigInfo.last_ypos = ImgedConfigInfo.y_pos;
            _wpi_getrectvalues( rcmain, &left, &top, NULL, NULL );
            ImgedConfigInfo.x_pos = (short)left;
            ImgedConfigInfo.y_pos = (short)top;
        }
        return( 0 );

    case WM_SIZE:
        ResizeFunctionBar( lparam );
        ResizeStatusBar( lparam );
#ifndef __OS2_PM__
        if( ClientWindow != NULL ) {
            setClientSize( hwnd );
        }
#else
        resizeClientArea( lparam );
#endif

        if( !_imgwpi_issizeminimized( wparam ) && !_imgwpi_issizemaximized( wparam ) ) {
            _wpi_getwindowrect( hwnd, &rcmain );
            ImgedConfigInfo.width = (short)_wpi_getwidthrect( rcmain );
            ImgedConfigInfo.height = (short)_wpi_getheightrect( rcmain );
            ImgedConfigInfo.ismaximized = FALSE;
        } else {
            ImgedConfigInfo.x_pos = ImgedConfigInfo.last_xpos;
            ImgedConfigInfo.y_pos = ImgedConfigInfo.last_ypos;
            ImgedConfigInfo.ismaximized = _imgwpi_issizemaximized( wparam );
        }
        return( FALSE );

    case WM_MENUSELECT:
#ifndef __OS2_PM__
        if( GET_WM_MENUSELECT_FLAGS( wparam, lparam ) & MF_SEPARATOR ) {
            break;
        }
        if( GET_WM_MENUSELECT_FLAGS( wparam, lparam ) & MF_SYSMENU ) {
            PrintHintTextByID( WIE_SYSMENUOPERATIONS, NULL );
            break;
        }
#endif
        ShowHintText( LOWORD( wparam ) );
        break;

    case WM_COMMAND:
        cmdid = LOWORD( wparam );
        if( !IEIsMenuIDValid( hmenu, cmdid ) ) {
            break;
        }
        switch( cmdid ) {
        case IMGED_NEW:
            if( !ImgedIsDDE ) {
                if( !NewImage( UNDEF_IMG, NULL ) ) {
                    PrintHintTextByID( WIE_NEIMAGENOTCREATED, NULL );
                }
            }
            break;

        case IMGED_CLOSE:
            node = GetCurrentNode();
            if( node != NULL ) {
                _wpi_sendmessage( node->hwnd, WM_CLOSE, 0, 0L );
            }
            break;

        case IMGED_CLOSEALL:
            CloseAllImages();
            break;

        case IMGED_HELP:
            IEHelpRoutine();
            break;

        case IMGED_HELP_SEARCH:
            IEHelpSearchRoutine();
            break;

        case IMGED_HELP_ON_HELP:
            IEHelpOnHelpRoutine();
            break;

        case IMGED_ABOUT:
#ifndef __OS2_PM__
            ai.owner = hwnd;
            ai.inst = Instance;
            ai.name = IEAllocRCString( WIE_ABOUTTEXT );
            ai.version = IEAllocRCString( WIE_ABOUTVERSION );
            ai.first_cr_year = "1994";
            ai.title = IEAllocRCString( WIE_ABOUTTITLE );
            DoAbout( &ai );
            if( ai.name != NULL ) {
                IEFreeRCString( ai.name );
            }
            if( ai.version != NULL ) {
                IEFreeRCString( ai.version );
            }
            if( ai.title != NULL ) {
                IEFreeRCString( ai.title );
            }
#endif
            break;

#ifndef __OS2_PM__
        case IMGED_DDE_UPDATE_PRJ:
            IEUpdateDDEEditSession();
            break;
#endif

        case IMGED_SAVE_AS:
            SaveFile( SB_SAVE_AS );
            break;

        case IMGED_SAVE:
            SaveFile( SB_SAVE );
            break;

        case IMGED_OPEN:
            if( !ImgedIsDDE ) {
                OpenImage( NULL );
            }
            break;

        case IMGED_CLEAR:
            ClearImage();
            break;

        case IMGED_NEWIMG:
            AddNewIcon();
            break;

        case IMGED_SELIMG:
            SelectIconImg();
            break;

        case IMGED_DELIMG:
            DeleteIconImg();
            break;

        case IMGED_UNDO:
            UndoOp();
            break;

        case IMGED_REDO:
            RedoOp();
            break;

        case IMGED_REST:
            RestoreImage();
            break;

        case IMGED_SNAP:
#ifndef __OS2_PM__
            SnapPicture();
#endif
            break;

        case IMGED_RIGHT:
        case IMGED_LEFT:
        case IMGED_UP:
        case IMGED_DOWN:
            ShiftImage( cmdid );
            break;

        case IMGED_FLIPHORZ:
        case IMGED_FLIPVERT:
            FlipImage( cmdid );
            break;

        case IMGED_ROTATECC:
        case IMGED_ROTATECL:
            RotateImage( cmdid );
            break;

        case IMGED_PASTE:
            PlaceAndPaste();
            break;

        case IMGED_COPY:
            IECopyImage();
            break;

        case IMGED_CUT:
            CutImage();
            break;

        case IMGED_COLOR:
            CheckPaletteItem( hmenu );
            break;

        case IMGED_VIEW:
            CheckViewItem( hmenu );
            break;

        case IMGED_TOOLBAR:
            CheckToolbarItem( hmenu );
            break;

        case IMGED_SQUARE:
            CheckSquareGrid( hmenu );
            break;

        case IMGED_SIZE:
            ChangeImageSize();
            break;

        case IMGED_GRID:
            CheckGridItem( hmenu );
            break;

        case IMGED_MAXIMIZE:
            MaximizeCurrentChild();
            break;

        case IMGED_SETTINGS:
            SelectOptions();
            break;

        case IMGED_2x2:
        case IMGED_3x3:
        case IMGED_4x4:
        case IMGED_5x5:
            checkBrushItem( hmenu, cmdid );
            break;

        case IMGED_CEDIT:
#ifndef __OS2_PM__
            EditColors();
#endif
            break;

        case IMGED_CRESET:
#ifndef __OS2_PM__
            RestoreColors();
#endif
            break;

        case IMGED_CSCREEN:
            ChooseBkColor();
            break;

        case IMGED_SCOLOR:
#ifndef __OS2_PM__
            SaveColorPalette();
#endif
            break;

        case IMGED_LCOLOR:
#ifndef __OS2_PM__
            if( LoadColorPalette() ) {
                _wpi_enablemenuitem( hmenu, IMGED_RCOLOR, TRUE, FALSE );
            }
#endif
            break;

        case IMGED_RCOLOR:
            RestoreColorPalette();
            break;

        case IMGED_FREEHAND:
        case IMGED_LINE:
        case IMGED_RECTO:
        case IMGED_RECTF:
        case IMGED_CIRCLEO:
        case IMGED_CIRCLEF:
        case IMGED_FILL:
        case IMGED_BRUSH:
        case IMGED_CLIP:
        case IMGED_HOTSPOT:
            SetToolType( cmdid );
            PushToolButton( cmdid );
            break;

        case IMGED_ARRANGE:
#ifndef __OS2_PM__
            SendMessage( ClientWindow, WM_MDIICONARRANGE, 0, 0L );
#endif
            break;

        case IMGED_TILE:
#ifndef __OS2_PM__
            SendMessage( ClientWindow, WM_MDITILE, MDITILE_VERTICAL, 0L );
#endif
            break;

        case IMGED_CASCADE:
#ifndef __OS2_PM__
            SendMessage( ClientWindow, WM_MDICASCADE, MDITILE_SKIPDISABLED, 0L );
#endif
            break;

        case IMGED_EXIT:
            _wpi_sendmessage( hwnd, WM_COMMAND, IMGED_CLOSEALL, 0L );

            if( _wpi_getfirstchild( _wpi_getclient( ClientWindow ) ) != NULL ) {
                break;
            }
#ifndef __OS2_PM__
            _wpi_destroywindow( _wpi_getframe( hwnd ) );
#else
            _wpi_sendmessage( hwnd, WM_CLOSE, 0, 0 );
#endif
            break;

        default:
#if 1
            return( _imgwpi_defframeproc( hwnd, ClientWindow, msg, wparam, lparam ) );
#else
            return( 0 );
#endif
        }
        return( 0 );

#ifndef __OS2_PM__
    case WM_COMPACTING:
        RelieveUndos();
        return 0;
#endif

    case WM_QUERYENDSESSION:
        if( _wpi_isiconic( _wpi_getframe( hwnd ) ) ) {
            if( ImgedConfigInfo.ismaximized ) {
                _wpi_maximizewindow( _wpi_getframe( hwnd ) );
            } else {
                _wpi_showwindow( _wpi_getframe( hwnd ), SW_SHOWNORMAL );
            }
        }
        _wpi_sendmessage( hwnd, WM_COMMAND, IMGED_CLOSEALL, 0L );

        if( _wpi_getfirstchild( _wpi_getclient( ClientWindow ) ) != NULL ) {
            return( 0 );
        }
        return( (WPI_MRESULT)1 );

    case WM_CLOSE:
        // wParam is non-zero if the DDE connection died
        if( !wparam && !ImgEdEnableMenuInput ) {
            // this prevents the user from closing the editor during
            // DDE initialization
            return( 0 );
        }
        _wpi_sendmessage( hwnd, WM_COMMAND, IMGED_CLOSEALL, 0L );
#ifdef __OS2_PM__
        return( _wpi_defwindowproc( hwnd, msg, wparam, lparam ) );
#else

        if( _wpi_getfirstchild( _wpi_getclient( ClientWindow ) ) != NULL ) {
            return( 0 );
        }
        window_destroyed = TRUE;
        _wpi_destroywindow( _wpi_getframe( hwnd ) );
        return( 0 );
#endif

    case WM_DESTROY:
#ifndef __OS2_PM__
        WWinHelp( HMainWindow, "resimg.hlp", HELP_QUIT, 0 );
#endif
        FiniStatusLine();
        CleanupClipboard();
        CleanupCursors();
        CloseToolBar();
        CloseFunctionBar();
        _wpi_deletefont( SmallFont );
        _wpi_postquitmessage( 0 );
        return( 0 );
    default:
        break;
    }
    return( _imgwpi_defframeproc( hwnd, ClientWindow, msg, wparam, lparam ) );

} /* ImgEdFrameProc */

#ifdef __OS2_PM__

/*
 * This is necessary since the window does not paint itself
 */
WPI_MRESULT CALLBACK ClientProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 mp1, WPI_PARAM2 mp2 )
{
    RECTL       rect;
    WPI_PRES    pres;

    switch( msg ) {
    case WM_PAINT:
        pres = WinBeginPaint( hwnd, 0L, (PRECTL)&rect );
        WinFillRect( pres, (PRECTL)&rect, CLR_BACKGROUND );
        WinEndPaint( pres );
        break;
    default:
        return( _wpi_defwindowproc( hwnd, msg, mp1, mp2 ) );
    }
    return( 0 );

} /* ClientProc */

#endif

/*
 * IEHelpRoutine
 */
void IEHelpRoutine( void )
{
#ifndef __OS2_PM__
    if( !WHtmlHelp( HMainWindow, "resimg.chm", HELP_CONTENTS, 0 ) ) {
        WWinHelp( HMainWindow, "resimg.hlp", HELP_CONTENTS, 0 );
    }
#endif

} /* IEHelpRoutine */

/*
 * IEHelpSearchRoutine
 */
void IEHelpSearchRoutine( void )
{
#ifndef __OS2_PM__
    if( !WHtmlHelp( HMainWindow, "resimg.chm", HELP_PARTIALKEY, (HELP_DATA)"" ) ) {
        WWinHelp( HMainWindow, "resimg.hlp", HELP_PARTIALKEY, (HELP_DATA)"" );
    }
#endif

} /* IEHelpSearchRoutine */

/*
 * IEHelpOnHelpRoutine
 */
void IEHelpOnHelpRoutine( void )
{
#ifndef __OS2_PM__
    WWinHelp( HMainWindow, "winhelp.hlp", HELP_HELPONHELP, 0 );
#endif

} /* IEHelpOnHelpRoutine */

/*
 * IEHelpCallback
 */
void CALLBACK IEHelpCallBack( void )
{
    IEHelpRoutine();

} /* IEHelpCallback */
