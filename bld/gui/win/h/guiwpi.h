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


#ifndef __OS2_PM__
    #define _wpi_createwindow_ex( exstyle, class, name, frame_style, create_flags, \
                    client_style, x, y, width, height, parent, menu, inst, \
                    pparam, pframe_hwnd ) \
        CreateWindowEx( (exstyle), (class), (name), (frame_style) | (create_flags) | \
                (client_style), x, y, width, height, parent, menu, inst, \
                pparam )

    #define     _wpi_setmaxposition( info, pos_x, pos_y ) \
        (info)->ptMaxPosition.x = pos_x; (info)->ptMaxPosition.y = pos_y
    #define     _wpi_sethbrbackground( hwnd, brush ) SET_HBRBACKGROUND( hwnd, brush )
    #define     _wpi_loadicon( hinst, icon ) LoadIcon( hinst, icon )
    #define     _wpi_getsysicon( hinst, icon ) LoadIcon( hinst, icon )
    #define     _wpi_destroyicon( icon ) DestroyIcon( icon )
    #define     _wpi_help( hwnd, file, key, top ) WinHelp( wnd, file, key, topic )
    #define _wpi_createanywindow( class, name, style, x, y, width, height, \
                        parent_hwnd, menu, inst, param, hwnd, id, frame ) \
        *hwnd = CreateWindow( class, name, style, x, y, width, height, \
                parent_hwnd, menu, inst, param );
    #define _wpi_makeintresource( res ) MAKEINTRESOURCE( res )
#else
    #define _wpi_createwindow_ex( exstyle, class, name, frame_style, create_flags, \
                    client_style, x, y, width, height, parent, menu, inst, \
                    pparam, pframe_hwnd ) \
            _wpi_createwindow( (class), (name), (frame_style), (create_flags), \
                    (client_style), (x), (y), (width), (height), (parent), (menu), (inst), \
                    (pparam), (pframe_hwnd) )
    #define     EM_GETSEL               EM_QUERYSEL
    #define     IDI_APPLICATION         SPTR_APPICON
    #define     _wpi_setmaxposition( info, pos_x, pos_y )
        // nothing
    #define     _wpi_sethbrbackground( hwnd, brush )
        // nothing
    #define     _wpi_loadicon( hinst, icon ) \
        (WPI_HICON) ( _wpi_loadcursor( (hinst), (ULONG)(icon) ) )
    #define     _wpi_getsysicon( hinst, icon ) \
        (WPI_HICON) WinQuerySysPointer( HWND_DESKTOP, icon, TRUE)
    #define     _wpi_destroyicon( icon ) WinDestroyPointer( (HPOINTER)icon )
    #define     _wpi_help( hwnd, file, key, top ) \
        WinCreateHelpInstance( wnd, file, key, topic ) // ???
    #define _wpi_createanywindow( class, name, style, x, y, width, height, \
                    parent_hwnd, menu, inst, param, hwnd, id, frame_hwnd ) \
            *frame_hwnd = WinCreateWindow( parent_hwnd, class, name, style, \
                                           x, y, width, height, parent_hwnd, \
                                           HWND_TOP, id, param, NULL );
    #define _wpi_makeintresource( res ) ( res )
#endif

#ifndef __OS2_PM__
    #define GUI_RECTDIM WPI_RECTDIM
    #define _wpi_cvth_y_plus1( y, h ) (y)
    #define _wpi_cvth_y_size( y, h, size ) (y)
    #define _wpi_cvth_y_size_plus1( y, h, size ) (y)
    #define _wpi_cvtc_y_plus1( hwnd, y ) (y)
    #define _wpi_cvtc_y_size( hwnd, y, size ) (y)
    #define _wpi_cvtc_y_size_plus1( hwnd, y, size ) (y)
    #define _wpi_cvtc_rect_plus1( hwnd, prect ) // do nothing
    #define _wpi_cvth_rect_plus1( prect, h ) // do nothing
    #define MB_MOVEABLE         0
    #define _wpi_rationalize_rect( prect ) \
        (prect)->right -= (prect)->left; \
        (prect)->left = 0; \
        (prect)->bottom -= (prect)->top; \
        (prect)->top = 0;
#else
    #define GUI_RECTDIM WPI_RECTDIM
    #define _wpi_cvth_y_plus1( y, h ) ((h) - (y))
    #define _wpi_cvth_y_size( y, h, size ) ((h) - (y) - 1 - (size))
    #define _wpi_cvth_y_size_plus1( y, h, size ) ((h) - (y) - (size))
    #define _wpi_cvtc_y_plus1( hwnd, y ) ( _wpi_cvtc_y( (hwnd), (y) ) + 1 )
    #define _wpi_cvtc_y_size( hwnd, y, size ) \
        ( _wpi_cvtc_y( (hwnd), (y) ) - (size) )
    #define _wpi_cvtc_y_size_plus1( hwnd, y, size ) \
        ( _wpi_cvtc_y( (hwnd), (y) ) - (size) + 1 )
    #define _wpi_cvtc_rect_plus1( hwnd, prect ) \
        _wpi_cvtc_rect( hwnd, (prect) ); \
        (prect)->yBottom++; \
        (prect)->yTop++;
    #define _wpi_cvth_rect_plus1( rect, h ) \
            (rect)->yBottom = _wpi_cvth_y_plus1( (rect)->yBottom, h ); \
            (rect)->yTop = _wpi_cvth_y_plus1( (rect)->yTop, h );
    #define LBN_DBLCLK          LN_ENTER
    #define LBN_KILLFOCUS       LN_KILLFOCUS
    #define WM_NCACTIVATE       TBM_SETHILITE
    #define _wpi_rationalize_rect( prect ) \
        (prect)->xRight -= (prect)->xLeft; \
        (prect)->xLeft = 0; \
        (prect)->yTop -= (prect)->yBottom; \
        (prect)->yBottom = 0;
#endif

