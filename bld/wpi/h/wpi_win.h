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


/*************************************************************************
 * FUNCTIONS
 *      NOTE- When adding macros to this file, be sure to append them to the
 *            END of the file.  This file will eventually be converted to
 *            a library.  The method of conversion will be from the start
 *            of the file to the end.
 *************************************************************************/

    #define GlobalUnlockCheck( hmem ) GlobalUnlock( hmem )

    #define LocalUnlockCheck( hmem ) LocalUnlock( hmem )


    #define WPI_MAKEPOINT(wp, lp, pt) MAKE_POINT( pt, lp );

    #define _wpi_isrectempty( inst, prect ) \
                      IsRectEmpty( prect )

    #define _wpi_intersectrect( inst, prdest, prsrc1, prsrc2 ) \
                                IntersectRect( prdest, prsrc1, prsrc2 )

extern void _wpi_setpoint( WPI_POINT *pt, int x, int y );

#ifdef __NT__
    #define _wpi_moveto( pres, point ) MoveToEx( pres, (point)->x, (point)->y, NULL )
#else
    #define _wpi_moveto( pres, point ) MoveTo( pres, (point)->x, (point)->y )
#endif

    #define _wpi_movetoex( pres, point, extra ) \
        MoveToEx( pres, (point)->x, (point)->y, extra )

    #define _wpi_lineto( pres, point ) LineTo( pres, (point)->x, (point)->y )

    #define _wpi_setpixel( pres, x, y, colour ) SetPixel( pres, x, y, colour )

    #define _wpi_devicecapableinch( pres, what ) GetDeviceCaps( pres, what )

    #define _wpi_getcurrenttime( inst ) GetCurrentTime()

extern void _wpi_getcurrpos( WPI_PRES pres, WPI_POINT *pt );

    #define _wpi_createsolidbrush( colour ) CreateSolidBrush( colour )

    #define _wpi_createnullbrush( ) GetStockObject( NULL_BRUSH )

    #define _wpi_deletenullbrush( brush ) \
        // nothing

    #define _wpi_createnullpen( ) GetStockObject( NULL_PEN )

    #define _wpi_deletenullpen( pen ) \
        // nothing

    #define _wpi_polygon( pres, pts, num_pts ) Polygon( pres, pts, num_pts )

    #define _wpi_gpipop( pres, num ) \
        // nothing

    #define _wpi_preserveattrs( pres ) \
        // nothing

extern void _wpi_getpaintrect( PAINTSTRUCT *ps, WPI_RECT *rect );

    #define _wpi_muldiv( a, b, c ) MulDiv( a, b, c )

    #define _wpi_setmaxtracksize(info,width,height) \
        info->ptMaxSize.x = width; info->ptMaxSize.y = height

    #define _wpi_setmintracksize(info,width,height) \
        info->ptMinTrackSize.x = width; info->ptMinTrackSize.y = height

    #define _wpi_setdoubleclicktime( rate ) SetDoubleClickTime( rate )

    #define _wpi_iswindowenabled( hwnd ) IsWindowEnabled( hwnd )

    #define _wpi_ellipse( pres, x1, y1, x2, y2 ) Ellipse( pres, x1, y1, x2, y2 )

    #define _wpi_lptodp( pres, line, num ) LPtoDP( pres, line, num )

    #define _wpi_dptolp( pres, line, num ) DPtoLP( pres, line, num )

    #define _wpi_setmapmode( pres, what ) SetMapMode( pres, what )

    #define _wpi_getcurrenttask( ) GetCurrentTask()

    #define _wpi_getrvalue( color ) GetRValue( color )

    #define _wpi_getgvalue( color ) GetGValue( color )

    #define _wpi_getbvalue( color ) GetBValue( color )

    #define _wpi_getrgb( red, green, blue ) RGB( red, green, blue )

    #define _wpi_getmessage( inst, pmsg, hwnd, first, last ) \
        GetMessage( pmsg, hwnd, first, last )

    #define _wpi_translatemessage( msg ) TranslateMessage( msg )

    #define _wpi_postquitmessage( parm ) PostQuitMessage( parm )

    #define _wpi_setmodhandle( name, inst ) *inst = *inst;

    #define _wpi_issameinst( inst1, inst2 ) ( inst1 == inst2 )

    #define _wpi_setwpiinst( inst, mod_handle, ret ) *(ret) = (HANDLE)(inst);

    #define _wpi_dispatchmessage( inst, msg ) DispatchMessage( msg )

    #define _wpi_postmessage( hwnd, msg, parm1, parm2 ) \
                                        PostMessage( hwnd, msg, parm1, parm2 )

    #define _wpi_postappmessage( hmq, msg, parm1, parm2 ) \
                                        PostAppMessage( hmq, msg, parm1, parm2 )

    #define _wpi_peekmessage( inst, qmsg, hwnd, min, max, remove ) \
                                PeekMessage( qmsg, hwnd, min, max, remove )

    #define _wpi_sendmessage( hwnd, msgid, parm1, parm2 ) \
                                SendMessage( hwnd, msgid, parm1, parm2 )

    #define _wpi_senddlgitemmessage( hwnd, item, msgid, parm1, parm2 ) \
                        SendDlgItemMessage( hwnd, item, msgid, parm1, parm2 )

    #define _wpi_getdlgitemcbtext( hwnd, item, selection, len, text ) \
        _wpi_senddlgitemmessage( hwnd, item, CB_GETLBTEXT, \
                        selection, (DWORD)(LPSTR) text )

    #define _wpi_getdlgitemlbtext( hwnd, item, selection, len, text ) \
        _wpi_senddlgitemmessage( hwnd, item, LB_GETTEXT, \
                        selection, (DWORD)(LPSTR) text )

    #define _wpi_setdlgitemtext( hwnd, item, text ) \
                                            SetDlgItemText( hwnd, item, text )

    #define _wpi_getdlgitemtext( hwnd, item, text, size ) \
                                        GetDlgItemText( hwnd, item, text, size )

    #define _wpi_getdlgitemshort( hwnd, item, ptrans, issigned ) \
                            (short)GetDlgItemInt( hwnd, item, ptrans, issigned )

    #define _wpi_getdlgitemint( hwnd, item, ftrns, signed ) \
        GetDlgItemInt( hwnd, item, ftrns, signed )

    #define _wpi_setdlgitemshort( hwnd, item, value, signed  ) \
        SetDlgItemInt( hwnd, item, value, signed );

    #define _wpi_setdlgitemint( hwnd, item, value, signed  ) \
        SetDlgItemInt( hwnd, item, value, signed );

    #define _wpi_ret_wminitdlg( bool_val ) ( (WPI_DLGRESULT) bool_val )

extern void _wpi_preparemono( WPI_PRES hdc, WPI_COLOUR colour,
                                                    WPI_COLOUR back_colour );

    #define _wpi_torgbmode( pres ) \
                // do nothing

    #define _wpi_bitblt( dest, x_pos, y_pos, width, height, source, new_x, new_y, format ) \
        BitBlt( dest, x_pos, y_pos, width, height, source, new_x, new_y, format );

    #define _wpi_patblt( dest, x, y, cx, cy, format ) \
        PatBlt( dest, x, y, cx, cy, format );

    #define _wpi_createos2normpres( pres, inst, hwnd ) pres = 0

    #define _wpi_deleteos2normpres( pres ) pres = 0

extern WPI_PRES _wpi_createcompatiblepres( WPI_PRES pres, WPI_INST inst, HDC *hdc );

    #define _wpi_deletepres( pres, hdc ) DeleteDC( pres )

    #define _wpi_deletecompatiblepres( pres, hdc ) DeleteDC( pres )

    #define _wpi_selectbitmap( pres, new_bmp ) SelectObject( pres, new_bmp )

    #define _wpi_getoldbitmap( pres, bmp ) SelectObject( pres, bmp )

    #define _wpi_deletebitmap( bmp ) DeleteObject( bmp )

    #define _wpi_translateaccelerator( inst, hwnd, accel, msg ) \
                                    TranslateAccelerator( hwnd, accel, msg )

    #define _wpi_loadaccelerators( inst, id ) LoadAccelerators( inst, id )

    #define _wpi_getscreenpres() GetDC( GetDesktopWindow() )

    #define _wpi_releasescreenpres( hps ) ReleaseDC( GetDesktopWindow(), hps )

    #define _wpi_loadbitmap( inst, id ) LoadBitmap( inst, id )

extern void _wpi_getbitmapdim( HBITMAP bmp, int *pwidth, int *pheight );

    #define _wpi_getpres( hdl ) GetDC( hdl )

    #define _wpi_releasepres( hdl, pres ) ReleaseDC( hdl, pres )

    #define _wpi_createcompatiblebitmap( pres, width, height ) \
        CreateCompatibleBitmap( pres, width, height )

    #define _wpi_preparesyscolour( format ) \
        // nothing

    #define _wpi_getsyscolour( format ) GetSysColor( format )

    #define _wpi_getwindowrect( hwnd, rect ) GetWindowRect( hwnd, rect )

    #define _wpi_getdlgitem( parent, id ) GetDlgItem( parent, id )

    #define _wpi_screentoclient( hdl, pt ) ScreenToClient( hdl, pt )

    #define _wpi_clienttoscreen( hdl, pt ) ClientToScreen( hdl, pt )

    #define _wpi_mapwindowpoints( hfrom, hto, ppts, num ) \
                                MapWindowPoints( hfrom, hto, ppts, num )

    #define _wpi_getcursorpos( pt ) GetCursorPos( pt )

    #define _wpi_clipcursor( rect ) ClipCursor( rect )

    #define _wpi_getnearestcolor( pres, colour ) GetNearestColor( pres, colour )

    #define _wpi_offsetrect( inst, rect, x, y ) OffsetRect( rect, x, y )

    #define _wpi_fillrect( pres, rect, colour, brush ) \
        FillRect( pres, rect, brush );

    #define _wpi_beginpaint( hwnd, pres, paint ) BeginPaint( hwnd, paint )

    #define _wpi_endpaint( hwnd, pres, paint ) EndPaint( hwnd, paint )

    #define _wpi_createwindow( class, name, frame_style, create_flags, \
                    client_style, x, y, width, height, parent, menu, inst, \
                    pparam, pframe_hwnd ) \
        CreateWindow( (class), (name), (frame_style) | (create_flags) | \
                (client_style), x, y, width, height, parent, menu, inst, \
                pparam )

    #define _wpi_createobjwindow( class, name, style, x, y, width, height, \
                        parent, menu, inst, pparam, pframe ) \
        CreateWindow( class, name, style, x, y, width, height, parent, menu, \
                                                                inst, pparam )

    #define _wpi_getwindowtext( hwnd, text, size ) \
                                            GetWindowText( hwnd, text, size )

    #define _wpi_getwindowword( hwnd, id ) GetWindowWord( hwnd, id )

    #define _wpi_setwindowword( hwnd, id, data ) SetWindowWord( hwnd, id, data )

    #define _wpi_getwindowlong( hwnd, id ) GetWindowLong( hwnd, id )

    #define _wpi_setwindowlong( hwnd, id, data ) SetWindowLong( hwnd, id, data )

#ifdef _WIN64
    #define _wpi_getwindowlongptr( hwnd, id ) GetWindowLongPtr( hwnd, id )

    #define _wpi_setwindowlongptr( hwnd, id, data ) SetWindowLongPtr( hwnd, id, data )
#else
    #define _wpi_getwindowlongptr( hwnd, id ) GetWindowLong( hwnd, id )

    #define _wpi_setwindowlongptr( hwnd, id, data ) SetWindowLong( hwnd, id, (LONG)data )
#endif

    #define _wpi_registerclass( class ) RegisterClass( class )

    #define _wpi_unregisterclass( name, inst ) UnregisterClass( name, inst )

    #define _wpi_getclassinfo( inst, name, info ) \
                                            GetClassInfo( NULL, name, info )

    #define _wpi_makeprocinstance( proc, inst ) MakeProcInstance( proc, inst )

    #define _wpi_makeenumprocinstance( proc, inst ) \
                                    (WPI_ENUMPROC)MakeProcInstance( (FARPROC)proc, inst )

    #define _wpi_makelineddaprocinstance( proc, inst ) \
                                    (WPI_LINEDDAPROC)MakeProcInstance( (FARPROC)proc, inst )

    #define _wpi_defdlgproc( hwnd, msg, mp1, mp2 ) FALSE

    #define _wpi_freeprocinstance( proc ) FreeProcInstance( (FARPROC)proc )

    #define _wpi_getclassproc( class ) (class)->lpfnWndProc

    #define _wpi_getcreateparms( lpcs ) (lpcs)->lpCreateParams

    #define _wpi_callwindowproc( proc, hwnd, msg, parm1, parm2 ) \
        CallWindowProc( proc, hwnd, msg, parm1, parm2 )

    #define _wpi_setclassproc( class, proc ) \
        (class)->lpfnWndProc = (WNDPROC) (proc)

    #define _wpi_setclassinst( class, inst ) (class)->hInstance = (inst)

    #define _wpi_setclassname( class, name ) (class)->lpszClassName = (name)

    #define _wpi_setclassstyle( class, wpi_style ) (class)->style = (wpi_style)

    #define _wpi_setclassbackground( class, background ) \
        (class)->hbrBackground = (background)

    #define _wpi_setclassextra( class, extra ) \
        (class)->cbWndExtra = (extra)

    #define _wpi_enddialog( hwnd, result ) EndDialog( hwnd, result )

    #define _wpi_dialogbox( parent, proc, inst, res_id, data ) \
        DialogBoxParam( inst, res_id, parent, proc, (DWORD)(LPARAM)(data) )

    #define _wpi_setstretchbltmode( mem, mode ) SetStretchBltMode( mem, mode )

    #define _wpi_stretchblt( dest, x_dest, y_dest, cx_dest, cy_dest, src, \
                                x_src, y_src, cx_src, cy_src, rop )  \
        StretchBlt( dest, x_dest, y_dest, cx_dest, cy_dest, \
                                src, x_src, y_src, cx_src, cy_src, rop )

extern void _wpi_setrectvalues( WPI_RECT *rect, WPI_RECTDIM in_left,
            WPI_RECTDIM in_top, WPI_RECTDIM in_right, WPI_RECTDIM in_bottom );
extern void _wpi_getrectvalues( WPI_RECT rect, WPI_RECTDIM *left,
                WPI_RECTDIM *top, WPI_RECTDIM *right, WPI_RECTDIM *bottom );
extern void _wpi_setwrectvalues( WPI_RECT *rect, WPI_RECTDIM in_left,
            WPI_RECTDIM in_top, WPI_RECTDIM in_right, WPI_RECTDIM in_bottom );
extern void _wpi_getwrectvalues( WPI_RECT rect, WPI_RECTDIM *left,
                WPI_RECTDIM *top, WPI_RECTDIM *right, WPI_RECTDIM *bottom);
extern void _wpi_setintrectvalues( WPI_RECT *rect, int in_left, int in_top,
                                                int in_right, int in_bottom );
extern void _wpi_getintrectvalues( WPI_RECT rect, int *left, int *top,
                                                    int *right, int *bottom );
extern void _wpi_setintwrectvalues( WPI_RECT *rect, int in_left, int in_top,
                                                int in_right, int in_bottom );
extern void _wpi_getintwrectvalues( WPI_RECT rect, int *left, int *top,
                                                    int *right, int *bottom);

    #define _wpi_convertheight( height, window_top, window_bottom ) height

    #define _wpi_cvth_y( y, h ) (y)

    #define _wpi_cvth_pt( pt, h )

    #define _wpi_cvth_rect( rect, h )

    #define _wpi_cvth_wanchor( y, wh, ph ) (y)

    #define _wpi_cvts_y( y ) (y)

    #define _wpi_cvtc_y( hwnd, y ) (y)

    #define _wpi_cvtc_rect( hwnd, rect ) //nothing

    #define _wpi_cvts_pt( pt )

    #define _wpi_cvts_rect( rect, h )

    #define _wpi_cvts_wanchor( y, wh ) (y)

    #define _wpi_borderrect( pres, rect, brush, fore, back ) \
        FrameRect( pres, rect, brush )

    #define _wpi_getdlgctrlid( hwnd ) GetDlgCtrlID( hwnd )

    #define _wpi_getwindowid( hwnd ) GetDlgCtrlID( hwnd )

    #define _wpi_isrightid( hwnd, id ) ( id == GetDlgCtrlID(hwnd) )

    #define _wpi_messagebox( parent, text, title, style ) \
                                    MessageBox( parent, text, title, style )

    #define _wpi_createrectrgn( pres, rects ) \
        CreateRectRgnIndirect( rects )

    #define _wpi_deletecliprgn( pres, rgn ) DeleteObject( rgn )

    #define _wpi_selectcliprgn( pres, rgn ) SelectClipRgn( pres, rgn )

    #define _wpi_getboss( hwnd ) GetWindowWord( hwnd, GWW_HWNDPARENT )

    #define _wpi_getparent( hwnd ) GetParent( hwnd )

    #define _wpi_setparent( win, parent ) SetParent( win, parent )

    #define _wpi_ischild( hwnd1, hwnd2 ) IsChild( hwnd1, hwnd2 )

    #define _wpi_getowner( hwnd ) GetWindow( hwnd, GW_OWNER )

    #define _wpi_setowner( win, parent ) SetParent( win, parent )

    #define _wpi_getvk( parm1, parm2 ) (parm1)

    #define _wpi_getmousex( parm1, parm2 ) LOWORD( parm2 )

    #define _wpi_getmousey( parm1, parm2 ) HIWORD( parm2 )

    #define _wpi_getwmsizex( parm1, parm2 ) LOWORD( parm2 )

    #define _wpi_getwmsizey( parm1, parm2 ) HIWORD( parm2 )

    #define _wpi_ismsgsetfocus( msg, parm2 ) ( (msg) == WM_SETFOCUS )

    #define _wpi_ismsgkillfocus( msg, parm2 ) ( (msg) == WM_KILLFOCUS )

    #define _wpi_ismsgkeydown( msg, parm1 ) ( (msg) == WM_KEYDOWN )

    #define _wpi_ismsgkeyup( msg, parm1 ) ( (msg) == WM_KEYUP )

    #define _wpi_isdown( parm1, parm2 ) ( (parm1) & MK_LBUTTON )

    #define _wpi_fileopen( filename, format ) _lopen( filename, format )

    #define _wpi_filecreate( filename, format ) _lcreat( filename, 0 )

    #define _wpi_fileclose( file_hdl ) _lclose( file_hdl )

    #define _wpi_filewrite( hfile, buf, size ) _lwrite( hfile, buf, size )

    #define _wpi_fileread( hfile, buf, size ) _lread( hfile, buf, size )

    #define _wpi_isdialogmessage( wpi_hwnd, wpi_msg ) \
                                        IsDialogMessage( wpi_hwnd, wpi_msg )

    #define _wpi_ismessage( msg, id ) ( (msg).message == (id) )

    #define _wpi_isntdblclk( parm1, parm2 ) \
                 (GET_WM_COMMAND_CMD( parm1, parm2 ) != LBN_DBLCLK)

    #define _wpi_createbrush( log_brush ) CreateBrushIndirect( log_brush )

    #define _wpi_deletebrush( brush )  DeleteObject( brush )

#ifdef __NT__
    #define _wpi_setbrushorigin( pres, pt ) SetBrushOrgEx( pres, (pt)->x, (pt)->y, NULL )
#else
    #define _wpi_setbrushorigin( pres, pt ) SetBrushOrg( pres, (pt)->x, (pt)->y )
#endif

    #define _wpi_setlogbrushsolid( plogbrush ) (plogbrush)->lbStyle = BS_SOLID

    #define _wpi_setlogbrushnull( plogbrush ) (plogbrush)->lbStyle = BS_NULL

    #define _wpi_setlogbrushhollow( plbrush ) (plbrush)->lbStyle = BS_HOLLOW

    #define _wpi_setlogbrushstyle( plogbrush, style ) \
                                            (plogbrush)->lbStyle = style

    #define _wpi_setlogbrushsymbol( plogbrush, symbol ) \
                                            (plogbrush)->lbHatch = symbol

    #define _wpi_setlogbrushcolour( plogbrush, colour ) \
                                            (plogbrush)->lbColor = colour

    #define _wpi_logbrushcolour( brush ) (plogbrush)->lbColor

    #define _wpi_drawfocusrect( pres, rect ) DrawFocusRect( pres, rect )

    #define _wpi_rectangle( pres, left, top, right, bottom ) \
        Rectangle( pres, left, top, right, bottom )

    #define _wpi_getstockobject( object ) GetStockObject( object )

    #define _wpi_dlgbaseheight( inst, size ) \
        *(size) = HIWORD( GetDialogBaseUnits() );

    #define _wpi_dlgbasewidth( inst, size ) \
        *(size) = LOWORD( GetDialogBaseUnits() );

extern void _wpi_setqmsgvalues( WPI_QMSG *qmsg, HWND hwnd, WPI_MSG wpi_msg,
                        WPI_PARAM1 wparam, WPI_PARAM2 lparam, ULONG wpi_time,
                        WPI_POINT pt );
extern void _wpi_getqmsgvalues( WPI_QMSG qmsg, HWND *hwnd, WPI_MSG *wpi_msg,
                        WPI_PARAM1 *wparam, WPI_PARAM2 *lparam, ULONG *wpi_time,
                        WPI_POINT *pt );

    #define _wpi_qmsgmessage( pwpi_qmsg ) ((pwpi_qmsg)->message)

    #define _wpi_qmsgparam1( pwpi_qmsg ) ((pwpi_qmsg)->wParam)

    #define _wpi_qmsgparam2( pwpi_qmsg ) ((pwpi_qmsg)->lParam)

    #define _wpi_drawhwnd( drw ) ((drw)->hwndItem)

    #define _wpi_drawpres( drw ) ((drw)->hDC)

extern void _wpi_suspendthread( UINT thread_id, WPI_QMSG *msg );

    #define _wpi_resumethread( thread_id ) \
                            PostAppMessage( thread_id, WM_QUIT, NULL, NULL )

    #define _wpi_settextcolor( pres, colour ) SetTextColor( pres, colour )

    #define _wpi_settextalign( pres, horiz_flags, vert_flags ) \
                            SetTextAlign( pres, (horiz_flags) | (vert_flags) )

    #define _wpi_textout( pres, left, top, text, len ) \
                                        TextOut( pres, left, top, text, len )

    #define _wpi_exttextout( hdc, x, y, opt, pr, str, len, spacing ) \
                           ExtTextOut( hdc, x, y, opt, pr, str, len, spacing )

    /***** _WPI_F_* Font Functions *****/
    /* These functions are a set of replacement WPI font functions
       which should work. All the other functions are crap; this
       is an attempt to do it right. DJP */

    #define _wpi_f_setfontitalic( font, style ) \
                        (font)->lfItalic = (style) ? -1 : 0

    #define _wpi_f_setfontunderline( font, style ) \
                        (font)->lfUnderline = (style) ? -1 : 0

    #define _wpi_f_setfontstrikeout( font, style ) \
                        (font)->lfStrikeOut = (style) ? -1 : 0

    #define _wpi_f_setfontbold( font, style ) \
                        (font)->lfWeight = (style) ? 700 : 400

    #define _wpi_f_getfontitalic( font ) \
                        ( (font)->lfItalic )

    #define _wpi_f_getfontunderline( font ) \
                        ( (font)->lfUnderline )


    #define _wpi_f_getfontstrikeout( font ) \
                        ( (font)->lfStrikeOut )

    #define _wpi_f_getfontbold( font ) \
                        ( ( (font)->lfWeight == 700 ) ? 1 : 0 )

    #define _wpi_f_setfontsize( font, point_size, pix_size ) \
        (font)->lfHeight = pix_size

    #define _wpi_f_getfontsize( font ) \
                        ( (font)->lfHeight )

    #define _wpi_f_default( font ) \
                        ( memset( font, 0, sizeof( *font ) ) )

    #define _wpi_f_setfontescapement( font, escapement ) \
                        (font)->lfEscapement = (escapement)

    #define _wpi_f_setfontorientation( font, orientation ) \
                        (font)->lfOrientation = (orientation)

    #define _wpi_f_setfontcharset( font, set ) \
                        (font)->lfCharSet = (set)

    #define _wpi_f_getfontcharset( font ) \
                        (font)->lfCharSet

    #define _wpi_f_setfontfacename( font, name ) \
                strcpy( (font)->lfFaceName, ( (name) != NULL ) ? (name) : "" )

    #define _wpi_f_getfontfacename( font ) \
                ( (font)->lfFaceName )

    #define _wpi_f_setfontprecision( font, out, clip ) \
        (font)->lfOutPrecision = (out); \
        (font)->lfClipPrecision = (clip);

    #define _wpi_f_setfontquality( font, quality ) \
        (font)->lfQuality = (quality);

    #define _wpi_f_setfontpitch( font, pitch ) \
        (font)->lfPitchAndFamily = (pitch);

    #define _wpi_f_deletefont( hfont ) \
                         DeleteObject( hfont )

    #define _wpi_f_selectfont( hdc, font ) SelectObject( hdc, font )

    #define _wpi_f_getoldfont( hdc, oldfont ) SelectObject( hdc, oldfont )

#ifdef __NT__
    #define _wpi_f_getsystemfont( hdc, font ) \
        GetObject( \
            GetStockObject( LOBYTE(LOWORD(GetVersion())) >= 4 ? \
            DEFAULT_GUI_FONT : SYSTEM_FONT ) \
            , sizeof( *(font) ), (font) )
#else
    #define _wpi_f_getsystemfont( hdc, font ) \
        GetObject( GetStockObject( SYSTEM_FONT ), sizeof( *(font) ), (font) )
#endif

    #define _wpi_f_createfont( font ) \
                CreateFontIndirect( font )

    /**** end of _WPI_F_* Font Functions *****/
    #define _wpi_getdeffm( fm ) (fm) = (fm)

    #define _wpi_getfontfacename( font ) ((font).lfFaceName)

    #define _wpi_setfontbold( font, bold ) \
        { \
            BOOL                tmp_bool; \
            tmp_bool = (bold); \
            if( tmp_bool ) { \
                (font)->lfWeight = 700; \
            } else { \
                (font)->lfWeight = 400; \
            } \
        }

    #define _wpi_getfontbold( font ) ( (font)->lfWeight == 700 )

    #define _wpi_setfontitalic( font, italic ) \
        (font)->lfItalic = (italic) ? -1 : 0;

    #define _wpi_getfontitalic( font ) ( (font)->lfItalic )

    #define _wpi_setfontunderline( font, underline ) \
        (font)->lfUnderline = (underline) ? -1 : 0;

    #define _wpi_getfontunderline( font ) ( (font)->lfUnderline )

    #define _wpi_setfontstrikeout( font, strikeout ) \
        (font)->lfStrikeOut = (strikeout) ? -1 : 0;

    #define _wpi_getfontstrikeout( font ) ( (font)->lfStrikeOut )

    #define _wpi_isfontsizeable( font ) FALSE

    #define _wpi_setfontsizeable( font, is_sizeable ) \
        // nothing

    #define _wpi_setfontheight( font, height ) \
        (font)->lfHeight = (height);

    #define _wpi_setfontwidth( font, width ) \
        (font)->lfWidth = (width);

    #define _wpi_setfontpointsize( font, size, pixel_size, match_no ) \
        _wpi_setfontwidth( font, 0 ); \
        _wpi_setfontheight( font, -(pixel_size) );

    #define _wpi_setfontescapement( font, escapement ) \
        (font)->lfEscapement = (escapement);

    #define _wpi_setfontorientation( font, orientation ) \
        (font)->lfOrientation = (orientation);

    #define _wpi_setfontprecision( font, out, clip ) \
        (font)->lfOutPrecision = (out); \
        (font)->lfClipPrecision = (clip);

    #define _wpi_setfontquality( font, quality ) \
        (font)->lfQuality = (quality);

    #define _wpi_setfontcharset( font, set ) \
        (font)->lfCharSet = (set);

    #define _wpi_getfontcharset( font ) (font)->lfCharSet

    #define _wpi_setfontfacename( font, name ) \
        { \
            char        *tmp_str; \
            tmp_str = (name); \
            if( tmp_str != NULL ) { \
                strcpy( (font)->lfFaceName, (name) ); \
            } else { \
                (font)->lfFaceName[0] = '\0'; \
            } \
        }

    #define _wpi_setfontpitch( font, pitch ) \
        (font)->lfPitchAndFamily = (pitch);

    #define _wpi_fontmatch( font ) 0

    #define _wpi_getfontpitch( font ) ( (font)->lfPitchAndFamily )

    #define _wpi_getfontheight( font ) ( (font)->lfHeight )

    #define _wpi_gettextface( pres, size, buf ) GetTextFace( pres, size, buf )

    #define _wpi_gettextmetrics( pres, tm ) GetTextMetrics( pres, tm )

    #define _wpi_metricheight( metric ) ( (metric).tmHeight )

    #define _wpi_metricexleading( metric ) ( (metric).tmExternalLeading )

    #define _wpi_metricascent( metric ) (metric).tmAscent

    #define _wpi_metricdescent( metric ) (metric).tmDescent

    #define _wpi_metricavecharwidth( metric ) ( (metric).tmAveCharWidth )

    #define _wpi_metricoverhang( metric ) ( (metric).tmOverhang )

    #define _wpi_metricileading( metric ) ( (metric).tmInternalLeading )

extern int _wpi_getmetricpointsize( WPI_PRES pres, WPI_TEXTMETRIC *tm,
                                            int *pix_size, int *match_num );

    #define _wpi_metricmaxcharwidth( metric ) ( (metric).tmMaxCharWidth )

    #define _wpi_enumfonts( pres, facename, proc, data ) \
        EnumFonts( pres, (LPSTR)facename, proc, (LPSTR)(data) )

    #define _wpi_enumchildwindows( hwnd, proc, lp ) \
                EnumChildWindows( hwnd, proc, lp )

    #define _wpi_getnextwindow( hwnd ) GetNextWindow( hwnd, GW_HWNDNEXT )

    #define _wpi_setbackcolour( pres, colour ) SetBkColor( pres, colour )

    #define _wpi_getbackcolour( pres ) GetBkColor( pres )

    #define _wpi_switchbuttoncode( parm1, parm2 ) \
                                         GET_WM_COMMAND_CMD( parm1, parm2 )

    #define _wpi_getid( parm1 ) LOWORD( parm1 )

    #define _wpi_isbuttoncode( parm1, parm2, code ) \
                         ( GET_WM_COMMAND_CMD( parm1, parm2 ) == (code))

    #define _wpi_isbuttonchecked( hwnd, id ) IsDlgButtonChecked( hwnd, id )

    #define _wpi_checkdlgbutton( hwnd, id, action ) \
                                            CheckDlgButton( hwnd, id, action )

    #define _wpi_checkradiobutton( hwnd, start_id, end_id, check_id ) \
         CheckRadioButton( hwnd, start_id, end_id, check_id )

    #define _wpi_intersectcliprect( pres, left, top, right, bottom ) \
                            IntersectClipRect( pres, left, top, right, bottom )

    #define _wpi_setviewportorg( pres, x, y ) SetViewportOrg( pres, x, y )

    #define _wpi_loadlibrary( inst, libname ) LoadLibrary( libname )

    #define _wpi_freelibrary( inst, lib_hdl ) FreeLibrary( lib_hdl )

    #define _wpi_loadprocedure( inst, lib, proc ) GetProcAddress( lib, proc )

    #define _wpi_loadstring( inst, id, value, len ) LoadString( inst, id, value, len )

    #define _wpi_getclassname( hwnd, buf, size ) GetClassName( hwnd, buf, size )

    #define _wpi_getwindowtextlength( hwnd ) GetWindowTextLength( hwnd )

    #define _wpi_getbitmapbits(bmp, size, bits) GetBitmapBits( bmp, size, bits )

    #define _wpi_setbitmapbits(bmp, size, bits) SetBitmapBits( bmp, size, bits )

    #define _wpi_getobject( obj, len, into ) GetObject( obj, len, into )

    #define _wpi_setanchorblock( pres, inst ) ((inst) = (inst));

    #define _wpi_invalidaterect( hwnd, rect, erase ) InvalidateRect( hwnd, rect, erase )

    #define _wpi_validaterect( hwnd, rect, children ) ValidateRect( hwnd, rect )

extern void _wpi_getbmphdrvalues( WPI_BITMAPINFOHEADER bmih, ULONG *size,
            int *cx, int *cy, short *planes, short *bc, int *comp,
            int *size_image, int *xpels, int *ypels, int *used, int *important);
extern void _wpi_setbmphdrvalues( WPI_BITMAPINFOHEADER *bmih, ULONG size,
            int cx, int cy, short planes, short bc, int comp,
            int size_image, int xpels, int ypels, int used, int important );

    #define _wpi_getbmpheader( bmp ) ( (bmp)->bmiHeader )

    #define _wpi_getbmpcolors( bmp ) ( (bmp)->bmiColors )

    #define _wpi_createpen( type, width, colour ) \
        CreatePen( (int)(type), (int)(width), (WPI_COLOUR)(colour) )

    #define _wpi_deletepen( pen ) DeleteObject( pen )

    #define _wpi_bmptorgb( bmp_info ) \
        // nothing

    #define _wpi_setimagemode( pres, mode ) \
        // nothing

    #define _wpi_setimagebackmode( pres, mode ) \
        // nothing

    #define _wpi_imagemode \
        // do nothing

    #define _wpi_imagebackmode( pres, mode ) \
        // nothing

    #define _wpi_setbackmode( pres, mode ) SetBkMode( pres, mode )

    #define _wpi_getbackmode( pres ) GetBkMode( pres )

    #define _wpi_getfocus() GetFocus()

    #define _wpi_setfocus( hwnd ) SetFocus( hwnd )

    #define _wpi_setactivewindow( hwnd ) SetActiveWindow( hwnd )

    #define _wpi_isitemchecked(hmenu, id) \
                        (GetMenuState(hmenu, id, MF_BYCOMMAND) & MF_CHECKED)

    #define _wpi_isitemenabled(hmenu, id) \
            (!(GetMenuState(hmenu, id, MF_BYCOMMAND) & (MF_GRAYED|MF_DISABLED)))

    #define _wpi_getcurrentsysmenu(hwnd) GetSystemMenu(hwnd, FALSE)

    #define _wpi_ismenucommand( parm1, parm2 ) \
                                ( GET_WM_COMMAND_HWND( parm1, parm2 ) == 0 )

    #define _wpi_deletesysmenupos( hmenu, pos ) \
                                        DeleteMenu(hmenu, pos, MF_BYPOSITION)

    #define _wpi_getframe(hwnd) hwnd

    #define _wpi_getclient( hwnd ) hwnd

    #define _wpi_savepres( pres ) SaveDC( pres )

    #define _wpi_restorepres( pres, pres_id ) RestoreDC( pres, pres_id )

#ifdef __NT__
    #define _wpi_getsystemfont() \
        GetStockObject( LOBYTE(LOWORD(GetVersion())) >= 4 ? \
        DEFAULT_GUI_FONT : SYSTEM_FONT )
#else
    #define _wpi_getsystemfont() GetStockObject( SYSTEM_FONT )
#endif

    #define _wpi_wmgetfont( hwnd, font_hld ) \
        font_hld = _wpi_sendmessage( hwnd, WM_GETFONT, NULL, NULL );

    #define _wpi_createfont( logfont, hfont ) \
        hfont = CreateFontIndirect( &logfont );

    #define _wpi_createrealfont( logfont, hfont ) \
        hfont = CreateFontIndirect( &logfont );

    #define _wpi_deletefont( hfont ) DeleteObject( hfont );

    #define _wpi_selectfont( hdc, font ) SelectObject( hdc, font );

    #define _wpi_getoldfont( hdc, oldfont ) SelectObject( hdc, oldfont );

    #define _wpi_selectbrush( pres, brush ) SelectObject( pres, brush )

    #define _wpi_getoldbrush( pres, oldbrush ) SelectObject( pres, oldbrush )

    #define _wpi_selectobject( pres, obj ) SelectObject( pres, obj )

    #define _wpi_deleteobject( obj ) DeleteObject( obj )

    #define _wpi_selectpen( pres, pen ) SelectObject( pres, pen )

    #define _wpi_getoldpen( pres, oldpen ) SelectObject( pres, oldpen )

    #define _wpi_ptinrect( prect, pt ) PtInRect( prect, pt )

    #define _wpi_rectvisible( pres, rect ) RectVisible( pres, rect )

    #define _wpi_ptvisible( pres, pt ) PtVisible( pres, pt )

extern void _wpi_gettextextent( WPI_PRES pres, LPCSTR string, int len_string,
                                                    int *width, int *height );

    #define _wpi_arc( pres, x1, y1, x2, y2, x3, y3, x4, y4 ) \
        Arc( pres, x1, y1, x2, y2, x3, y3, x4, y4 )

    #define _wpi_pie( pres, x1, y1, x2, y2, x3, y3, x4, y4 ) \
        Pie( pres, x1, y1, x2, y2, x3, y3, x4, y4 )

    #define _wpi_setrop2( pres, mode ) SetROP2( pres, mode )

    #define _wpi_defwindowproc(hwnd, msg, mp1, mp2) \
                                            DefWindowProc(hwnd, msg, mp1, mp2)

    #define _wpi_getclientrect( hwnd, prect ) GetClientRect( hwnd, prect )

    #define _wpi_getsystemmetrics( id ) GetSystemMetrics( id )

    #define _wpi_getheightrect( rc ) abs( (rc).bottom - (rc).top )

    #define _wpi_getwidthrect( rc ) abs( (rc).right - (rc).left )

    #define _wpi_dismissdialog( hwnd, ret ) // nothing

    #define _wpi_destroywindow( hwnd ) DestroyWindow( hwnd )

    #define _wpi_setwindowpos(hwnd, z, x, y, cx, cy, flags) \
                                    SetWindowPos(hwnd, z, x, y, cx, cy, flags)

    #define _wpi_iswindow( inst, hwnd ) ( IsWindow( hwnd ) != 0 ) // NT returns 0,1 - Win95 returns 0,0xff348900 Ouch!

    #define _wpi_iswindowshowing( hwnd ) TRUE

    #define _wpi_updatewindow( hwnd ) UpdateWindow( hwnd )

    #define _wpi_bringwindowtotop( hwnd ) BringWindowToTop(hwnd)

    #define _wpi_setcapture(hwnd) SetCapture(hwnd)

    #define _wpi_releasecapture() ReleaseCapture()

    #define _wpi_getcapture() GetCapture()

    #define _wpi_selectpalette(hdc, hpal) SelectPalette(hdc, hpal, TRUE)

    #define _wpi_createpatternbrush( bitmap ) CreatePatternBrush( bitmap )

    #define _wpi_getdibits( pres, bitmap, start, scans, buffer, info, clruse ) \
        GetDIBits( pres, bitmap, start, scans, buffer, info, clruse )

    #define _wpi_createdibitmap( pres, info, opt, data, table, opt2 ) \
        CreateDIBitmap( pres, info, opt, data, table, opt2 )

    // PM takes bitcount in place of bitspixel
    #define _wpi_createbitmap( width, height, planes, bitspixel, bits ) \
        CreateBitmap( width, height, planes, bitspixel, bits )

    #define _wpi_fatalappexit( inst, num, string ) FatalAppExit( num, string )

    #define _wpi_wpirecttorect(src_rc, dest_rc)  ( *(dest_rc) = *(src_rc) )

    #define _wpi_recttowpirect(src_rc, dest_rc) ( *(dest_rc) = *(src_rc) )

    #define _wpi_inflaterect( inst, r, cx, cy ) InflateRect( r, cx, cy )

    #define _wpi_drawtext(hps, text, len, rect, format) \
                DrawText(hps, text, len, rect, format)

    #define _wpi_unrealizeobject( hobj ) UnrealizeObject( hobj )

#ifdef __NT__
    #define _wpi_getlocalhdl( mem ) LocalHandle( mem )
    #define _wpi_getglobalhdl( mem ) GlobalHandle( mem )
#else
    #define _wpi_getlocalhdl( mem ) LocalHandle( (void NEAR*)LOWORD( mem ) )
    #define _wpi_getglobalhdl( mem ) (HGLOBAL)LOWORD(GlobalHandle( (UINT)HIWORD( mem ) ))
#endif

    #define _wpi_getmenu( hframe ) GetMenu( hframe )

    #define _wpi_showwindow(hwnd, state) ShowWindow(hwnd, state)

    #define _wpi_setwindowtext( hwnd, text ) SetWindowText( hwnd, text )

    #define _wpi_registerclipboardformat( fmt ) RegisterClipboardFormat( fmt )

    #define _wpi_isclipboardformatavailable( inst, fmt, info ) \
                                            IsClipboardFormatAvailable( fmt )

    #define _wpi_openclipboard( inst, hwnd ) OpenClipboard( hwnd )

    #define _wpi_closeclipboard( inst ) CloseClipboard()

    #define _wpi_getclipboardowner( inst ) GetClipboardOwner()

    #define _wpi_loadmenu( inst, id ) LoadMenu( inst, id )

    #define _wpi_setmenuitemtext( mnu, item_id, text ) \
            ModifyMenu( mnu, item_id, MF_STRING | MF_BYCOMMAND, item_id, text )

    #define _wpi_getsyscursor( id ) LoadCursor( NULL, id )

    #define _wpi_loadcursor( inst, id ) LoadCursor( inst, id )

    #define _wpi_destroycursor( hcur ) DestroyCursor( hcur )

    #define _wpi_setcursor( newcursor ) SetCursor( newcursor )

    #define _wpi_setclipboarddata( inst, format, data, is_bitmap ) \
        SetClipboardData( format, data )

    #define _wpi_getclipboarddata( inst, format ) GetClipboardData( format )

    #define _wpi_emptyclipboard( inst ) EmptyClipboard()

    #define _wpi_createdialog( inst, id, hwnd, proc ) \
                                        CreateDialog( inst, id, hwnd, proc )

    #define _wpi_enablewindow( hwnd, bool ) EnableWindow( hwnd, bool )

    #define _wpi_ismenuitemvalid( hwnd, id ) \
                                ( GetMenuState( hwnd, id, MF_BYCOMMAND ) == -1 )

    #define _wpi_drawmenubar( hwnd ) DrawMenuBar( hwnd )

    #define _wpi_getmessagepos( inst ) GetMessagePos()

    #define _wpi_windowfrompoint( pt ) WindowFromPoint( pt )

    #define _wpi_childwindowfrompoint( hwnd, pt ) \
                                                ChildWindowFromPoint( hwnd, pt )

    #define _wpi_movewindow( hwnd, x, y, width, height, repaint ) \
        MoveWindow( hwnd, x, y, width, height, repaint )

    #define _wpi_getscrollrange( hwnd, scroll, min, max ) \
        GetScrollRange( hwnd, scroll, min, max );

    #define _wpi_setscrollrange( hwnd, scroll, min, max, bool ) \
        SetScrollRange( hwnd, scroll, min, max, bool );

    #define _wpi_getscrollpos( hwnd, scroll ) GetScrollPos( hwnd, scroll )

    #define _wpi_setscrollpos( hwnd, scroll, pos, bool ) \
                                        SetScrollPos( hwnd, scroll, pos, bool )

    #define _wpi_openinifile( inst, name ) 0

    #define _wpi_closeinifile( hini ) \
                // do nothing

    #define _wpi_getprivateprofileint( hini, app, key, default, name ) \
                        GetPrivateProfileInt( app, key, default, name );

    #define _wpi_getprivateprofilestring( hini, app, key, default, returned, size, name ) \
        GetPrivateProfileString( app, key, default, returned, size, name );

    #define _wpi_writeprivateprofilestring( hini, app, key, data, name ) \
        WritePrivateProfileString( app, key, data, name );

extern void _wpi_getbitmapparms( HBITMAP bitmap, int *width, int *height,
                            int *planes, int *widthbytes, int *bitspixel );
    #define _wpi_getbitmapstruct( hbitmap, pinfo ) \
                GetObject( hbitmap, sizeof(WPI_BITMAP), (pinfo) )
    // The following retrieve the fields from the BITMAP structure
    #define _wpi_bitmapwidth( pinfo ) (pinfo)->bmWidth
    #define _wpi_bitmapheight( pinfo ) (pinfo)->bmHeight
    #define _wpi_bitmapbitcount( pinfo ) (pinfo)->bmWidthBytes
    #define _wpi_bitmapplanes( pinfo ) (pinfo)->bmPlanes
    #define _wpi_bitmapbitspixel( pinfo ) (pinfo)->bmBitsPixel

    #define _wpi_getpixel( hdc, x, y ) GetPixel( hdc, x, y )

    #define _wpi_extfloodfill( hdc, ptx, pty, clr, mode ) \
                        ExtFloodFill( hdc, ptx, pty, clr, mode )

    #define _wpi_getinidirectory( inst, dir_info, size ) \
                                        GetWindowsDirectory( dir_info, size )

    #define _wpi_getasynckeystate( code ) GetAsyncKeyState( code )

    #define _wpi_escape( dc, code, incount, indata, outdata, outcount ) \
        Escape( dc, code, incount, indata, outdata )

    #define _wpi_isiconic( hwnd ) IsIconic( hwnd )

#define GET_WM_MOUSEMOVE_POSX( parm1, parm2 ) _wpi_getmousex( parm1, parm2 )
#define GET_WM_MOUSEMOVE_POSY( parm1, parm2 ) _wpi_getmousey( parm1, parm2 )

    #define GET_WM_INITMENU_MENU( parm1, parm2 ) (HMENU) parm1

    #define GET_WM_VSCROLL_CMD( wp, lp ) GET_WM_VSCROLL_CODE( wp, lp )

    #define SHORT1FROMMP( parm1 ) LOWORD( parm1 )

    #define MPFROM2SHORT( s1, s2 ) MAKELONG( s1, s2 )

    #define WPI_MAKEP2( x1, x2 ) MAKELONG( x1, x2 )

    #define WPI_MAKEP1( x1, x2 ) (WPARAM)x1

    #define _wpi_copyrect( dest, src ) CopyRect( dest, src )

    #define _wpi_setredraw( hwnd, redraw ) \
        _wpi_sendmessage( hwnd, WM_SETREDRAW, \
                          (WPI_PARAM1) (redraw), (WPI_PARAM2) NULL )

    #define _wpi_getfirstchild( hwnd ) GetWindow( hwnd, GW_CHILD )

extern void _wpi_getrestoredrect( HWND hwnd, WPI_RECT *prect );
extern void _wpi_setrestoredrect( HWND hwnd, WPI_RECT *prect );

    #define _wpi_createpopupmenu() CreatePopupMenu()

    #define _wpi_getmenuitemcount( hmenu ) GetMenuItemCount( hmenu )

    #define _wpi_maximizewindow( hwnd ) ShowWindow( (hwnd), SW_SHOWMAXIMIZED )

    #define _wpi_minimizewindow( hwnd ) ShowWindow( (hwnd), SW_SHOWMINIMIZED )

    #define _wpi_iszoomed( hwnd ) IsZoomed( hwnd )

    #define _wpi_iswindowvisible( hwnd ) IsWindowVisible( hwnd )

    #define _wpi_getupdaterect( hwnd, prect ) GetUpdateRect(hwnd, prect, FALSE)

extern WPI_PROC _wpi_subclasswindow( HWND hwnd, WPI_PROC newproc );

extern BOOL _wpi_insertmenu( HMENU hmenu, unsigned pos, unsigned menu_flags,
                             unsigned attr_flags, unsigned new_id,
                             HMENU popup, const char *text, BOOL by_position );

extern BOOL _wpi_appendmenu( HMENU hmenu, unsigned menu_flags,
                             unsigned attr_flags, unsigned new_id,
                             HMENU popup, const char *text );

extern BOOL _wpi_getmenustate( HMENU hmenu, unsigned id, WPI_MENUSTATE *state,
                               BOOL by_position );

extern void _wpi_getmenuflagsfromstate( WPI_MENUSTATE *state,
                                        unsigned *menu_flags,
                                        unsigned *attr_flags );

    #define _wpi_ismenucheckedfromstate( pstate ) ( (*(pstate) & MF_CHECKED) != 0 )

    #define _wpi_ismenuenabledfromstate( pstate ) ( (*(pstate) & MF_DISABLED) == 0 )

    #define _wpi_ismenuseparatorfromstate( pstate ) ( (*(pstate) & MF_SEPARATOR) != 0 )

    #define _wpi_ismenupopupfromstate( pstate ) ( (*(pstate) & MF_POPUP) != 0 )

extern BOOL _wpi_modifymenu( HMENU hmenu, unsigned pos, unsigned menu_flags,
                             unsigned attr_flags, unsigned new_id,
                             HMENU new_popup, const char *new_text,
                             BOOL by_position );

    #define _wpi_createmenu() CreateMenu()

    #define _wpi_getsubmenu( hwnd, pos ) GetSubMenu( hwnd, pos )

    #define _wpi_getsystemmenu(hwnd) GetSystemMenu(hwnd, FALSE)

extern BOOL _wpi_setmenu( HWND hwnd, HMENU hmenu );

    #define _wpi_deletemenu(hmenu, id, by_position) \
        DeleteMenu(hmenu, id, (by_position) ? MF_BYPOSITION : MF_BYCOMMAND )

    #define _wpi_checkmenuitem(hmenu, id, fchecked, by_position )  \
        CheckMenuItem( hmenu, id, ((fchecked) ? MF_CHECKED : MF_UNCHECKED) | \
                       ((by_position) ? MF_BYPOSITION : MF_BYCOMMAND) )

    #define _wpi_enablemenuitem(hmenu, id, fenabled, by_position )  \
        EnableMenuItem( hmenu, id, \
                       ((fenabled) ? MF_ENABLED : MF_GRAYED ) | \
                       ((by_position) ? MF_BYPOSITION : MF_BYCOMMAND) )

extern BOOL _wpi_setmenutext( HMENU hmenu, unsigned id, const char *text,
                              BOOL by_position );

extern BOOL _wpi_getmenutext( HMENU hmenu, unsigned id, char *text, int ctext,
                              BOOL by_position );

    #define _wpi_getmenuitemid(hmenu, pos) GetMenuItemID( hmenu, pos )

    #define _wpi_destroymenu( hmenu ) DestroyMenu( hmenu )

    #define _wpi_getdevicecaps( pres, what ) GetDeviceCaps( pres, what )

    #define _wpi_restorewindow( hwnd ) ShowWindow( hwnd, SW_RESTORE )

    #define _wpi_scrollwindow( hwnd, dx, dy, scroll, clip ) \
        ScrollWindow( hwnd, dx, dy, scroll, clip )

    #define _wpi_getkeystate( vkey ) GetKeyState( (int)(vkey) )

    #define _wpi_menutext2win( ptext ) // do nothing

    #define _wpi_menutext2pm( ptext ) (char *)ptext

    #define _wpi_freemenutext( ptext ) // do nothing

    #define _wpi_is_close_menuselect( p1, p2 ) \
        ( (GET_WM_MENUSELECT_FLAGS( p1, p2 )) == (WORD)-1 )

    #define _wpi_trackpopupmenu( hmenu, flags, x, y, parent ) \
        TrackPopupMenu( hmenu, flags, x, y, 0, parent, 0 )

extern void _wpi_setrgbquadvalues( WPI_RGBQUAD *rgb, BYTE red, BYTE green,
                                                    BYTE blue, BYTE option );

    #define _wpi_getclipbox( pres, rcl ) GetClipBox( pres, rcl )

    #define _wpi_equalrect( prect1, prect2 ) EqualRect( prect1, prect2 )

    #define _wpi_palettergb( pres, red, green, blue ) PALETTERGB( red, green, blue )

extern int _wpi_dlg_command( HWND dlg_hld, WPI_MSG *msg, WPI_PARAM1 *parm1,
                                                            WPI_PARAM2 *parm2 );
    #define _wpi_linedda( x1, y1, x2, y2, proc, lp ) \
                                            LineDDA( x1, y1, x2, y2, proc, lp )

    #define _wpi_is_dbcs() ( FALSE )
    #define _wpi_fix_dbcs( dlg )
