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


/*--------------------------------------------------------------------------
   FUNCTIONS
        NOTE- When adding macros to this file, be sure to append them to the
              END of the file.  This file will eventually be converted to
              a library.  The method of conversion will be from the start
              of the file to the end.
 ---------------------------------------------------------------------------*/

/************************************************************************/
/* The following macros represent Windows APIs that have corresponding  */
/* PM APIs.  Hence you may leave the API name as the Windows API and    */
/* WPI will translate it to the equivalent PM API.                      */
/************************************************************************/

    #define GetBkColor( pres ) GpiQueryBackColor( pres )

    #define lstrlen( buf ) strlen( buf )

    #define LOWORD( word ) LOUSHORT( (ULONG) word )

    #define HIWORD( word ) HIUSHORT( (ULONG) word )

    #define LPtoDP( pres, line, num ) \
        // nothing

    #define DPtoLP( pres, line, num ) \
        // nothing

    #define CopyRect( dst, src ) memcpy( (dst), (src), sizeof(WPI_RECT) )

    #define RectVisible( pres, rect ) GpiRectVisible( pres, rect )

    #define PtVisible( pres, pt ) GpiPtVisible( pres, pt )

    #define IsDlgButtonChecked( hwnd, id ) WinQueryButtonCheckstate( hwnd, id )

    #define CheckDlgButton( hwnd, id, action ) WinCheckButton( \
                                 hwnd, (USHORT) id, (USHORT) action )

    #define LoadString( inst, id, value, len ) WinLoadString( \
                                inst.hab, inst.mod_handle, (ULONG) id, (LONG) len, (PSZ) value )

    #define GetClassName( hwnd, buf, size ) WinQueryClassName( \
                                hwnd, (LONG) size, (PCH) buf )

    #define SaveDC( pres ) GpiSavePS( pres )

    #define RestoreDC( pres, pres_id ) GpiRestorePS( pres, pres_id )

    #define GetMessage( wpi_msg, hdl, first, last ) WinGetMsg( hdl.hab, wpi_msg, NULLHANDLE, first, last )

    #define PostAppMessage( hmq, msg, parm1, parm2 ) WinPostMsg( hmq, msg, (WPI_PARAM1) parm1, (WPI_PARAM2) parm2 )

    #define MessageBox( parent, text, title, style ) WinMessageBox( HWND_DESKTOP, parent, text, title, 0, style )

    #define SendMessage( hwnd, msgid, parm1, parm2 ) WinSendMsg( hwnd, \
                (WPI_MSG) msgid, (WPI_PARAM1) parm1, (WPI_PARAM2) parm2 )

    #define SendDlgItemMessage( hwnd, item, msgid, parm1, parm2 ) \
        WinSendDlgItemMsg( hwnd, (ULONG)item, (ULONG)msgid, \
                                        (WPI_PARAM1)parm1, (WPI_PARAM2)parm2 )

    #define SetDlgItemText( hwnd, item, text ) \
        WinSetDlgItemText( (HWND) hwnd, (ULONG) item, (PSZ) text )

    #define GetDlgItemText( hwnd, item, text, size ) \
        WinQueryDlgItemText( (HWND)hwnd, (ULONG)item, (LONG)size, (PSZ)text )

    #define InvalidateRect( hwnd, rect, erase ) WinInvalidateRect( hwnd, rect, erase )

    #define GetWindowTextLength( hwnd ) WinQueryWindowTextLength( hwnd )

    #define SetBkMode( pres, mode ) GpiSetBackMix( pres, mode )

    #define GetBkMode( pres, mode ) GpiQueryBackMix( pres, mode )

    #define GetFocus() PM1632WinQueryFocus( HWND_DESKTOP )

    #define SetFocus( hwnd ) WinSetFocus( HWND_DESKTOP, hwnd )

    #define GetObject( obj, len, into ) len

    #define GlobalHandle( ptr ) ptr

    #define GlobalLock( hmem ) hmem

    #define GlobalUnlock( hmem ) \
        // nothing

    #define GlobalUnlockCheck( hmem ) TRUE

    #define GlobalAlloc( flags, size ) _wpi_malloc( (size_t) size )

    #define GlobalFree( hmem ) _wpi_free( (void *) hmem )

    #define GlobalReAlloc( hmem, size, flags ) (HANDLE) _wpi_realloc( hmem, size )

    #define LocalHandle( ptr ) ptr

    #define LocalLock( hmem ) hmem

    #define LocalUnlock( hmem ) \
        // nothing

    #define LocalUnlockCheck( hmem ) TRUE

    #define LocalAlloc( flags, size ) _wpi_malloc( (size_t) size )

    #define LocalFree( hmem ) _wpi_free( (void *) hmem )

    #define LocalReAlloc(hmem, size, flags) _wpi_realloc((void *)hmem, (size_t)size)

    #define Catch( buf ) setjmp( buf )

    #define Throw( buf, error ) longjmp( buf, error )

    #define GetMenu(hframe) WinWindowFromID(hframe, FID_MENU)

extern BOOL _wpi_showwindow( HWND hwnd, int state );

    #define ShowWindow(hwnd, state) _wpi_showwindow( hwnd, state )

    #define SetWindowText( hwnd, text ) WinSetWindowText(hwnd, text)

    #define DefWindowProc(hwnd, msg, mp1, mp2) \
                                        WinDefWindowProc(hwnd, msg, mp1, mp2)

    #define DestroyWindow( hwnd ) WinDestroyWindow( hwnd )

    #define SetWindowPos(hwnd, z, x, y, cx, cy, flags) \
                        WinSetWindowPos(hwnd, z, x, y, cx, cy, flags)

extern void GetWindowRect( HWND hwnd, WPI_RECT *rect );

    #define UpdateWindow( hwnd ) WinUpdateWindow(hwnd)

    #define SetCapture( hwnd ) WinSetCapture(HWND_DESKTOP, hwnd)

    #define ReleaseCapture() WinSetCapture(HWND_DESKTOP, NULLHANDLE)

    #define SelectPalette(hps, hpal) GpiSelectPalette(hps, hpal)

    #define SetStretchBltMode( hdc, mode ) \
                // do nothing

    #define MapWindowsPoints( hfrom, hto, ppts, num ) \
                                WinMapWindowsPoints( hfrom, hto, ppts, num )

/************************************************************************/
/* The following macros macro the Windows APIs to PM APIs using the     */
/* WPI naming convention.                                               */
/************************************************************************/

    /* I'm leaving this as a macro mainly because win1632 has the point */
    /* parameter passed by value and to change this to a function would */
    /* cause a whole bunch of 'pt not assigned a value' warnings.       */
    #define WPI_MAKEPOINT( mp1, mp2, pt ) \
    { \
        POINTS  newpt; \
        newpt = (*((POINTS FAR*)&(mp1))); \
        (pt).x = newpt.x; \
        (pt).y = newpt.y; \
    }

    #define _wpi_intersectrect( inst, prdest, prsrc1, prsrc2 ) \
                      WinIntersectRect( inst.hab, prdest, prsrc1, prsrc2 )

    #define _wpi_isrectempty( inst, prect ) \
                      WinIsRectEmpty( inst.hab, prect )

    #define _wpi_lptodp( pres, line, num ) \
        // nothing

    #define _wpi_dptolp( pres, line, num ) \
        // nothing

extern void _wpi_releasepres( HWND hwnd, WPI_PRES pres );

    #define _wpi_screentoclient( hclient, ppt ) \
        WinMapWindowPoints(HWND_DESKTOP, hclient, ppt, 1)

    #define _wpi_clienttoscreen( hclient, ppt ) \
        WinMapWindowPoints(hclient, HWND_DESKTOP, ppt, 1)

    #define _wpi_mapwindowpoints( hfrom, hto, ppts, num ) \
                                WinMapWindowPoints( hfrom, hto, ppts, num )

    #define _wpi_clipcursor( rect ) \
        // nothing

    #define _wpi_unregisterclass( name, inst ) \
        // nothing

    #define _wpi_freeprocinstance( proc ) \
        // nothing

    #define _wpi_setinst( class, inst ) \
        // nothing

    #define _wpi_setclassproc( class, proc ) (class)->lpfnWndProc = (PFNWP)proc

    #define _wpi_setclassinst( class, inst ) (class)->hInstance = inst

    #define _wpi_setclassname( class, name ) (class)->lpszClassName = name

    #define _wpi_setclassstyle( class, wpi_style ) (class)->style = wpi_style

    #define _wpi_setclassbackground( class, background ) \
                                            (class)->hbrBackground = background

    #define _wpi_setclassextra( class, extra ) (class)->cbWndExtra = extra

    #define _wpi_setstretchbltmode( mem, mode ) \
        // nothing

extern void _wpi_setqmsgvalues( WPI_QMSG *qmsg, HWND hwnd, WPI_MSG message,
                        WPI_PARAM1 parm1, WPI_PARAM2 parm2, ULONG wpi_time,
                        WPI_POINT wpi_pt );
extern void _wpi_getqmsgvalues( WPI_QMSG qmsg, HWND *hwnd, WPI_MSG *message,
                        WPI_PARAM1 *parm1, WPI_PARAM2 *parm2, ULONG *wpi_time,
                        WPI_POINT *wpi_pt );

    #define _wpi_qmsgmessage( pwpi_qmsg ) (pwpi_qmsg)->msg

    #define _wpi_qmsgparam1( pwpi_qmsg ) (pwpi_qmsg)->mp1

    #define _wpi_qmsgparam2( pwpi_qmsg ) (pwpi_qmsg)->mp2

    #define _wpi_drawhwnd( drw ) (drw)->hwnd

    #define _wpi_drawpres( drw ) (drw)->hps

extern void _wpi_suspendthread( UINT thread_id, WPI_QMSG *msg );

    #define _wpi_resumethread( thread_id ) DosResumeThread( thread_id )

    #define _wpi_settextalign( pres, horiz_flags, vert_flags ) \
        GpiSetTextAlignment( pres, horiz_flags, vert_flags )

    #define _wpi_setbrushorigin( pres, pt ) GpiSetPatternRefPoint( pres, pt )

extern void _wpi_setlogbrushsolid( LOGBRUSH *brush );
extern void _wpi_setlogbrushnull( LOGBRUSH *brush );
extern void _wpi_setlogbrushhollow( LOGBRUSH *brush );

    #define _wpi_setlogbrushstyle( plogbrush, style ) \
                                        (plogbrush)->usMixMode = style

    #define _wpi_setlogbrushsymbol( plogbrush, symbol ) \
                                        (plogbrush)->usSymbol = symbol

    #define _wpi_setlogbrushcolour( plogbrush, colour ) \
                                        (plogbrush)->lColor = colour

    #define _wpi_logbrushcolour( plogbrush ) (plogbrush)->lColor

extern HPEN _wpi_createpen( USHORT type, short width, WPI_COLOUR colour );
extern void _wpi_setpoint( WPI_POINT *pt, int x, int y );

    #define _wpi_moveto( pres, point ) GpiSetCurrentPosition( pres, point )

    #define _wpi_movetoex( pres, point, extra ) \
                                        GpiSetCurrentPosition( pres, point )

    #define _wpi_lineto( pres, point ) GpiLine( pres, point )

extern WPI_COLOUR _wpi_setpixel( WPI_PRES hps, int x, int y, WPI_COLOUR clr );
extern void _wpi_drawfocusrect( WPI_PRES pres, WPI_RECT *rect );
extern BOOL _wpi_rectangle( WPI_PRES pres, int left, int top, int right,
                                                                int bottom );
extern BOOL _wpi_polygon( WPI_PRES pres, WPI_POINT *pts, int num_pts );
extern int _wpi_devicecapableinch( WPI_PRES pres, int what );

    #define _wpi_getcurrenttime( inst ) WinGetCurrentTime( (inst).hab )

    #define _wpi_getcurrpos( pres, pt ) GpiQueryCurrentPosition( pres, pt )

extern HBRUSH _wpi_createpatternbrush( WPI_HANDLE bitmap );
extern HBRUSH _wpi_createsolidbrush( WPI_COLOUR colour );
extern HBRUSH _wpi_createnullbrush( void );
extern HPEN _wpi_createnullpen( void );

    #define _wpi_deletebrush( brush ) _wpi_free( (WPI_OBJECT *)brush )

    #define _wpi_deletenullbrush( brush ) _wpi_free( (WPI_OBJECT *)brush )

    #define _wpi_deletenullpen( pen ) _wpi_free( (WPI_OBJECT *)pen )

    #define _wpi_deletepen( pen ) _wpi_free( (WPI_OBJECT *)pen )

    #define _wpi_gpipop( pres, num ) GpiPop( pres, num )

    #define _wpi_preserveattrs( pres ) GpiSetAttrMode( pres, AM_PRESERVE )

extern void _wpi_getpaintrect( PAINTSTRUCT *ps, WPI_RECT *rect );

    #define _wpi_muldiv( a, b, c ) ((int)(((long)(a) * (long)(b)) / (long)(c)))

extern void _wpi_setmaxtracksize( WPI_MINMAXINFO *info, int width, int height );
extern void _wpi_setmintracksize( WPI_MINMAXINFO *info, int width, int height );

    #define _wpi_setdoubleclicktime( rate ) \
                            WinSetSysValue( HWND_DESKTOP, SV_DBLCLKTIME, rate )

    #define _wpi_iswindowenabled( hwnd ) WinIsWindowEnabled( hwnd )

extern void _wpi_hitrect( int xc, int yc, int ytop, int ybottom, int xleft,
                                            int xright, int mxp, int myp );
extern void _wpi_arc( WPI_PRES pres, int xx1, int yy1, int xx2, int yy2,
                                         int xx3, int yy3, int xx4, int yy4 );
extern void _wpi_pie( WPI_PRES pres, int xx1, int yy1, int xx2, int yy2,
                                         int xx3, int yy3, int xx4, int yy4 );
extern BOOL _wpi_ellipse( WPI_PRES pres, int left, int top, int right,
                                                                int bottom );
    #define _wpi_setmapmode( pres, what ) what

extern WPI_TASK _wpi_getcurrenttask( void );

    #define _wpi_getrvalue( color ) ((BYTE) (color >> 16))

    #define _wpi_getgvalue( color ) ((BYTE) ((WORD) color >> 8))

    #define _wpi_getbvalue( color ) ((BYTE) color)

    #define _wpi_getrgb( red, green, blue ) \
            (LONG)(((LONG) (red) << 16) + ((LONG) (green) << 8) + (LONG) (blue))

    #define _wpi_getmessage( inst, wpi_msg, hdl, first, last ) \
                    WinGetMsg( (inst).hab, wpi_msg, NULLHANDLE, first, last )

    #define _wpi_translatemessage( msg ) \
        // nothing

    #define _wpi_postquitmessage( parm ) \
      // nothing

extern void _wpi_setmodhandle( char *name, WPI_INST *inst );

    #define _wpi_issameinst( inst1, inst2 ) ( (inst1).hab == (inst2).hab && \
                                    (inst1).mod_handle == (inst2).mod_handle )

extern void _wpi_setwpiinst( HINSTANCE hinst, HMODULE module, WPI_INST *inst );

    #define _wpi_dispatchmessage( inst, msg ) WinDispatchMsg( inst.hab, msg )

    #define _wpi_postmessage( hwnd, msg, parm1, parm2 ) \
        WinPostMsg( hwnd, (WPI_MSG)msg, (WPI_PARAM1)parm1, (WPI_PARAM2)parm2 )

    #define _wpi_postappmessage( hmq, msg, parm1, parm2 ) \
        WinPostMsg( hmq, (WPI_MSG)msg, (WPI_PARAM1)parm1, (WPI_PARAM2)parm2 )

    #define _wpi_peekmessage( inst, qmsg, hwnd, min, max, remove ) \
                        WinPeekMsg( (inst).hab, qmsg, hwnd, min, max, remove )

    #define _wpi_sendmessage( hwnd, msgid, parm1, parm2 ) \
        WinSendMsg( hwnd, (WPI_MSG)msgid, (WPI_PARAM1)parm1, (WPI_PARAM2)parm2 )

    #define _wpi_senddlgitemmessage( hwnd, item, msgid, parm1, parm2 ) \
        WinSendDlgItemMsg( hwnd, (ULONG)item, (ULONG)msgid, (WPI_PARAM1)parm1, \
                                                            (WPI_PARAM2) parm2 )

    #define _wpi_getdlgitemcbtext( hwnd, item, selection, len, text ) \
        _wpi_senddlgitemmessage( hwnd, item, CB_GETLBTEXT, \
                        MPFROM2SHORT( selection, len ), (DWORD)(LPSTR) text )

    #define _wpi_getdlgitemlbtext( hwnd, item, selection, len, text ) \
        _wpi_senddlgitemmessage( hwnd, item, LB_GETTEXT, \
                        MPFROM2SHORT( selection, len ), (DWORD)(LPSTR) text )

    #define _wpi_setdlgitemtext( hwnd, item, text ) \
                                WinSetDlgItemText( hwnd, (ULONG) item, text )

    #define _wpi_getdlgitemtext( hwnd, item, text, size ) \
                        WinQueryDlgItemText( hwnd, (ULONG) item, size, text )

extern short _wpi_getdlgitemshort( HWND hwnd, int item, BOOL *trans, BOOL issigned );
extern int _wpi_getdlgitemint( HWND hwnd, int item, BOOL *retcode, BOOL issigned );

    #define _wpi_setdlgitemshort( hwnd, item, value, signed ) \
        WinSetDlgItemShort( hwnd, (ULONG) item, (USHORT) value, (BOOL) signed )

    #define _wpi_setdlgitemint( hwnd, item, value, signed ) \
        WinSetDlgItemShort( hwnd, (ULONG) item, (USHORT) value, (BOOL) signed )

    #define _wpi_getdlgitem( parent, id ) WinWindowFromID( parent, id )

    #define _wpi_ret_wminitdlg( bool_val ) ( (WPI_DLGRESULT) !bool_val )

extern void _wpi_preparemono( WPI_PRES hps, WPI_COLOUR colour,
                                                    WPI_COLOUR back_colour );

    #define _wpi_torgbmode( pres ) GpiCreateLogColorTable( pres, 0L, \
                                                    LCOLF_RGB, 0L, 0L, NULL )

extern void _wpi_bitblt( WPI_PRES dest, int x_dest, int y_dest, int cx,
                int cy, WPI_PRES source, int x_src, int y_src, LONG format );
extern void _wpi_patblt( WPI_PRES dest, int x_pos, int y_pos, int cx,
                                                        int cy, LONG format );
extern WPI_PRES _wpi_createos2normpres( WPI_INST inst, HWND hwnd );
extern void _wpi_deleteos2normpres( WPI_PRES pres );
extern WPI_PRES _wpi_createcompatiblepres( WPI_PRES pres, WPI_INST inst, HDC *hdc );
extern void _wpi_deletepres( WPI_PRES pres, HDC hdc );
extern void _wpi_deletecompatiblepres( WPI_PRES pres, HDC hdc );
extern WPI_HANDLE _wpi_selectbitmap( WPI_PRES pres, WPI_HANDLE bitmap );
extern void _wpi_getoldbitmap( WPI_PRES pres, WPI_HANDLE oldobj );
extern void _wpi_deletebitmap( WPI_HANDLE bmp );

    #define _wpi_translateaccelerator( inst, hwnd, accel, msg ) \
                            WinTranslateAccel( (inst).hab, hwnd, accel, msg )

    #define _wpi_loadaccelerators( inst, id ) \
            WinLoadAccelTable( (inst).hab, (inst).mod_handle, (ULONG)id )

    #define _wpi_getscreenpres() WinGetScreenPS( HWND_DESKTOP )

    #define _wpi_releasescreenpres( hps ) // nothing

extern WPI_HANDLE _wpi_loadbitmap( WPI_INST inst, int id );
extern void _wpi_getbitmapdim( WPI_HANDLE hbmp, int *pwidth, int *pheight );
extern WPI_PRES _wpi_getpres( HWND hwnd );
extern WPI_HANDLE _wpi_createcompatiblebitmap( WPI_PRES pres, int width, int height );

    #define _wpi_preparesyscolour() WinSetSysColors( HWND_DESKTOP, LCOL_RESET, \
                                    LCOLF_INDRGB, 0L, (ULONG) 0, (PLONG) NULL )

    #define _wpi_getsyscolour( format ) \
                                    WinQuerySysColor( HWND_DESKTOP, format, 0L )

extern void _wpi_getwindowrect( HWND hwnd, WPI_RECT *rect );

    #define _wpi_getcursorpos( ppt ) WinQueryPointerPos(HWND_DESKTOP, ppt)

    #define _wpi_getnearestcolor( pres, colour ) \
                                        GpiQueryNearestColor( pres, 0L, colour )

    #define _wpi_offsetrect( inst, rect, x, y ) \
                                        WinOffsetRect( (inst).hab, rect, x, y )

extern void _wpi_fillrect( WPI_PRES pres, WPI_RECT *rect, WPI_COLOUR colour,
                                                            HBRUSH brush );

    #define _wpi_beginpaint( hwnd, pres, paint ) \
                                            WinBeginPaint( hwnd, pres, paint )

    #define _wpi_endpaint( hwnd, pres, paint ) WinEndPaint( pres )

    #define _wpi_registerclass( cl ) WinRegisterClass( (cl)->hInstance.hab, \
        (cl)->lpszClassName, (cl)->lpfnWndProc, (cl)->style, (cl)->cbWndExtra )

extern BOOL _wpi_getclassinfo( WPI_INST inst, PSZ name, WPI_WNDCLASS *info );

    #define _wpi_getclassproc( class ) (class)->lpfnWndProc

    #define _wpi_getcreateparms( lpcs ) lpcs->pCtlData

extern HWND _wpi_createwindow( LPSTR class, LPSTR name, ULONG frame_style,
                        ULONG create_flags, ULONG client_style, int x, int y,
                        int width, int height, HWND parent, HMENU menu,
                        WPI_INST inst, void *param, HWND *frame_hwnd );
extern HWND _wpi_createobjwindow( LPSTR class, LPSTR name, ULONG style, int x,
                        int y, int width, int height, HWND parent, HMENU menu,
                        WPI_INST inst, void *pparam, HWND *pframe );

    #define _wpi_getwindowtext( hwnd, text, size ) \
                                        WinQueryWindowText( hwnd, size, text )

    #define _wpi_getwindowword( hwnd, id ) WinQueryWindowUShort( hwnd, id )

    #define _wpi_setwindowword( hwnd, id, data  ) \
                                        WinSetWindowUShort( hwnd, id, data )

    #define _wpi_getwindowlong( hwnd, id ) WinQueryWindowULong( hwnd, id )

    #define _wpi_setwindowlong( hwnd, id, data ) \
                                            WinSetWindowULong( hwnd, id, data )

    #define _wpi_makeprocinstance( proc, inst ) ( (WPI_PROC) proc )

    #define _wpi_makeenumprocinstance( proc, inst ) ( (WPI_ENUMPROC) proc )

    #define _wpi_makelineddaprocinstance( proc, inst ) ((WPI_LINEDDAPROC)proc)

    #define _wpi_defdlgproc( hwnd, msg, mp1, mp2 ) \
                                            WinDefDlgProc(hwnd, msg, mp1, mp2)

    #define _wpi_callwindowproc( proc, hwnd, msg, parm1, parm2 ) \
        ((proc)( hwnd, msg, (WPI_PARAM1) parm1, (WPI_PARAM2) parm2 ))

extern void _wpi_enddialog( HWND hwnd, ULONG result );
extern int _wpi_dialogbox( HWND parent, WPI_PROC proc, WPI_INST inst,
                                                    int res_id, void *data );
extern void _wpi_stretchblt( WPI_PRES dest, int x_dest, int y_dest,
        int cx_dest, int cy_dest, WPI_PRES src, int x_src, int y_src, int cx_src,
        int cy_src, LONG rop );
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
extern void GetClientRect( HWND hwnd, WPI_RECT *prect );

    #define _wpi_getclientrect( hwnd, prect ) GetClientRect( hwnd, prect )

    #define _wpi_cvth_y( y, h )  ((h) - (y) - 1)

    #define _wpi_cvth_pt( pt, h ) (pt)->y = ((h) - (pt)->y - 1)

extern void _wpi_cvth_rect( WPI_RECT *rect, LONG height );
extern LONG _wpi_cvth_wanchor( LONG y, LONG window_height, LONG parent_height );

    #define _wpi_cvts_y( y ) \
        _wpi_cvth_y( y, (int)WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN ) )

extern LONG _wpi_cvtc_y( HWND hwnd, LONG y );
extern void _wpi_cvtc_rect( HWND hwnd, WPI_RECT *rect );

    #define _wpi_cvts_pt( pt ) \
        _wpi_cvth_pt( pt, (int)WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN ) )

    #define _wpi_cvts_rect( rect, h ) \
        _wpi_cvth_rect( rect, (int)WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN) )

extern LONG _wpi_cvts_wanchor( LONG y, LONG window_height );

    #define _wpi_convertheight( h, top, bottom ) bottom + ( top - h - 1 )

    #define _wpi_borderrect( pres, rect, brush, fore, back ) \
        WinDrawBorder( pres, rect, 1, 1, fore, back, DB_STANDARD )

    #define _wpi_getdlgctrlid( hwnd ) WinQueryWindowUShort( hwnd, QWS_ID )

    #define _wpi_getwindowid( hwnd ) WinQueryWindowUShort( hwnd, QWS_ID )

    #define _wpi_isrightid( hwnd, id ) (id==WinQueryWindowUShort(hwnd, QWS_ID))

    #define _wpi_messagebox( parent, text, title, style ) \
                    WinMessageBox( HWND_DESKTOP, parent, text, title, 0, style )

extern BOOL _wpi_ptinrect( WPI_RECT *prect, WPI_POINT pt );

    #define _wpi_createrectrgn( pres, rects ) GpiCreateRegion( pres, 1, rects )

    #define _wpi_deletecliprgn( pres, rgn ) GpiDestroyRegion( pres, rgn )

extern int _wpi_selectcliprgn( WPI_PRES pres, HRGN rgn );

    #define _wpi_getboss( hwnd ) PM1632WinQueryWindow( hwnd, QW_OWNER )

    #define _wpi_ischild( hwnd1, hwnd2 ) WinIsChild( hwnd1, hwnd2 )

    #define _wpi_getowner( hwnd ) PM1632WinQueryWindow( hwnd, QW_OWNER )

    #define _wpi_setowner( win, who ) WinSetOwner( win, who )

    #define _wpi_getvk( parm1, parm2 ) SHORT2FROMMP( parm2 )

    #define _wpi_getmousex( parm1, parm2 ) ( ( (SHORT) parm1 ) )

    #define _wpi_getmousey( parm1, parm2 ) ( ( (LONG) parm1 >> 16 ) )

    #define _wpi_getwmsizex( parm1, parm2 ) ( ( (SHORT) parm2 ) )

    #define _wpi_getwmsizey( parm1, parm2 ) ( ( (LONG) parm2 >> 16 ) )

    #define _wpi_ismsgsetfocus( msg, parm2 ) \
                                ( msg == WM_SETFOCUS && SHORT1FROMMP( parm2 ) )

    #define _wpi_ismsgkillfocus( msg, parm2 ) \
                                ( msg == WM_SETFOCUS && !SHORT1FROMMP( parm2 ) )

    #define _wpi_ismsgkeydown( msg, parm1 ) \
         ( msg == WM_CHAR && !(SHORT1FROMMP( parm1 ) == KC_KEYUP ) )

    #define _wpi_ismsgkeyup( msg, parm1 ) \
         ( msg == WM_CHAR && (SHORT1FROMMP( parm1 ) == KC_KEYUP ) )

    #define _wpi_isdown( parm1, parm2 ) ( !( (int) parm2 & KC_KEYUP ) )

extern HFILE _wpi_fileopen( LPSTR filename, int format );
extern HFILE _wpi_filecreate( LPSTR filename, int format );
extern PM1632_FILESIZETYPE _wpi_fileclose( HFILE hfile );
extern PM1632_FILESIZETYPE _wpi_filewrite( HFILE hfile, void *buf,
                                                    PM1632_FILESIZETYPE size );
extern PM1632_FILESIZETYPE _wpi_fileread( HFILE hfile, void *buf,
                                                    PM1632_FILESIZETYPE size );

    #define _wpi_isdialogmessage( win_hld, wpi_msg ) ( (wpi_msg)->hwnd == win_hld )
    #define _wpi_ismessage( wpi_msg, id ) ( (wpi_msg).msg == id )

    #define _wpi_isntdblclk( parm1, parm2 ) SHORT2FROMMP( parm1 ) != LN_ENTER

extern HBRUSH _wpi_createbrush( LOGBRUSH *log_brush );

    #define _wpi_getstockobject( object ) 0

    #define _wpi_dlgbaseheight( inst, size ) \
        { \
            HDC                 dc; \
            DEVOPENSTRUC        dop = { NULL, "DISPLAY", NULL, NULL, \
                                        NULL, NULL, NULL, NULL, NULL }; \
            dc = DevOpenDC( inst.hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA) &dop, \
                                                        NULLHANDLE ); \
            DevQueryCaps( dc, CAPS_CHAR_HEIGHT, 1L, (LONG *) size ); \
            DevCloseDC( dc ); \
         }

    #define _wpi_dlgbasewidth( inst, size ) \
        { \
            HDC                 dc; \
            DEVOPENSTRUC        dop = { NULL, "DISPLAY", NULL, NULL, \
                                        NULL, NULL, NULL, NULL, NULL }; \
            dc = DevOpenDC( inst.hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA) &dop, \
                                                        NULLHANDLE ); \
            DevQueryCaps( dc, CAPS_CHAR_WIDTH, 1L, (LONG *) size ); \
            DevCloseDC( dc ); \
         }

extern WPI_COLOUR _wpi_settextcolor( WPI_PRES, WPI_COLOUR );
extern WPI_COLOUR _wpi_setbackcolour( WPI_PRES, WPI_COLOUR );

    #define _wpi_getbackcolour( pres ) GpiQueryBackColor( pres )

extern BOOL _wpi_textout( WPI_PRES pres, int left, int top, LPSTR text, ULONG
                                                                        len );
    #define _wpi_getdeffm( fm ) memset( &fm, 0, sizeof( FONTMETRICS ) )

    #define _wpi_getfontattrs2( fm, attr ) \
        (attr)->usRecordLength = sizeof( FATTRS ); \
        (attr)->fsSelection = (fm).fsSelection; \
        (attr)->lMatch = (fm).lMatch; \
        if( (fm).szFacename != NULL ) { \
            strcpy( (attr)->szFacename, (fm).szFacename ); \
        } else { \
            (attr)->szFacename[0] = '\0'; \
        } \
        (attr)->idRegistry = (fm).idRegistry; \
        (attr)->usCodePage = (fm).usCodePage; \
        (attr)->fsType = (fm).fsType; \
        (attr)->lMaxBaselineExt = (fm).lMaxBaselineExt; \
        (attr)->lAveCharWidth = (fm).lAveCharWidth; \
        (attr)->fsFontUse = FATTR_FONTUSE_NOMIX;


    /***** _WPI_F_* Font Functions *****/
    /* These functions are a set of replacement WPI font functions
       which should work. All the other functions are crap; this
       is an attempt to do it right. DJP */

    #define _wpi_f_setfontitalic( font, style ) \
                        (font)->attr.fsSelection = \
                        ( (font)->attr.fsSelection & ~FATTR_SEL_ITALIC ) + \
                        ( (style) ? FATTR_SEL_ITALIC : 0 )

    #define _wpi_f_setfontunderline( font, style ) \
                        (font)->attr.fsSelection = \
                        ( (font)->attr.fsSelection & ~FATTR_SEL_UNDERSCORE ) + \
                        ( (style) ? FATTR_SEL_UNDERSCORE : 0 )

    #define _wpi_f_setfontstrikeout( font, style ) \
                        (font)->attr.fsSelection = \
                        ( (font)->attr.fsSelection & ~FATTR_SEL_STRIKEOUT ) + \
                        ( (style) ? FATTR_SEL_STRIKEOUT : 0 )

    #define _wpi_f_setfontbold( font, style ) \
                        (font)->attr.fsSelection = \
                        ( (font)->attr.fsSelection & ~FATTR_SEL_BOLD ) + \
                        ( (style) ? FATTR_SEL_BOLD : 0 )

    #define _wpi_f_getfontitalic( font ) \
                ( ( (font)->attr.fsSelection & FATTR_SEL_ITALIC ) ? 1 : 0 )

    #define _wpi_f_getfontunderline( font ) \
                ( ( (font)->attr.fsSelection & FATTR_SEL_UNDERSCORE ) ? 1 : 0 )

    #define _wpi_f_getfontstrikeout( font ) \
                ( ( (font)->attr.fsSelection & FATTR_SEL_STRIKEOUT ) ? 1 : 0 )

    #define _wpi_f_getfontbold( font ) \
                ( ( (font)->attr.fsSelection & FATTR_SEL_BOLD ) ? 1 : 0 )

    //#define _wpi_f_setfontsizeable( font, is_sizeable )
    // shouldn't be done. Changes font selection algorithm

    #define _wpi_f_setfontsize( font, point_size, pix_size ) \
        (font)->pt_size = point_size

    extern LONG _wpi_f_getfontsize( WPI_F_FONT *font );

    //#define _wpi_f_setfontwidth( font, width )
        // Although OS/2 does make it possible to set this,
        // it doesn't make a lot of sense. You can't do it in Windows,
        // and it would be wierd in OS/2.

    extern void _wpi_f_default( WPI_F_FONT *font );

    extern void _wpi_f_setfontescapement( WPI_F_FONT *font, LONG escapement );

    extern void _wpi_f_setfontorientation( WPI_F_FONT *font, LONG orientation );

    #define _wpi_f_setfontcharset( font, set ) \
        (font)->attr.usCodePage = set

    #define _wpi_f_getfontcharset( font ) \
        ( (font)->attr.usCodePage )

    extern void _wpi_f_setfontfacename( WPI_F_FONT *font, PSZ name );

    #define _wpi_f_getfontfacename( font ) \
        ( (font)->attr.szFacename )

    #define _wpi_f_setfontprecision( font, out, clip ) \
        // nothing

    #define _wpi_f_setfontquality( font, quality ) \
        // nothing

    #define _wpi_f_setfontpitch( font, pitch ) \
        // nothing

    #define _wpi_f_deletefont( font ) \
        _wpi_free( (WPI_F_FONT *) font )

    extern HFONT _wpi_f_selectfont( WPI_PRES pres, HFONT font );

    extern void _wpi_f_getoldfont( WPI_PRES pres, HFONT oldfont );

    extern void _wpi_f_getsystemfont( WPI_PRES pres, WPI_F_FONT *font );

    extern HFONT _wpi_f_createfont( WPI_F_FONT *font );

    /**** end of _WPI_F_* Font Functions *****/


    #define _wpi_getfontfacename( font ) (font).szFacename

    #define _wpi_setfontbold( font, bold ) \
        { \
            BOOL                tmp_bool; \
            tmp_bool = bold; \
            if( tmp_bool ) { \
                (font)->usWeightClass = 7; \
                (font)->fsSelection = ( (font)->fsSelection | FATTR_SEL_BOLD ); \
            } else { \
                (font)->usWeightClass = 6; \
                (font)->fsSelection = ( (font)->fsSelection & ~FATTR_SEL_BOLD ); \
            } \
        }

    #define _wpi_getfontbold( font ) ( (font)->usWeightClass == 7 )

    #define _wpi_setfontitalic( font, italic ) \
        { \
            BOOL                tmp_bool; \
            tmp_bool = italic; \
            if( tmp_bool ) { \
                (font)->fsSelection = (font)->fsSelection | FATTR_SEL_ITALIC; \
            } else { \
                (font)->fsSelection = (font)->fsSelection & ~FATTR_SEL_ITALIC; \
            } \
        }

    #define _wpi_getfontitalic( font ) ( (font)->fsSelection & FATTR_SEL_ITALIC )

    #define _wpi_setfontunderline( font, underline ) \
        { \
            BOOL                tmp_bool; \
            tmp_bool = underline; \
            if( tmp_bool ) { \
                (font)->fsSelection = (font)->fsSelection | FATTR_SEL_UNDERSCORE; \
            } else { \
                (font)->fsSelection = ((font)->fsSelection & ~FATTR_SEL_UNDERSCORE); \
            } \
        }

    #define _wpi_getfontunderline( font ) ( (font)->fsSelection & FATTR_SEL_UNDERSCORE )

    #define _wpi_setfontstrikeout( font, strikeout ) \
        { \
            BOOL                tmp_bool; \
            tmp_bool = strikeout; \
            if( tmp_bool ) { \
                (font)->fsSelection = (font)->fsSelection | FATTR_SEL_STRIKEOUT; \
            } else { \
                (font)->fsSelection = ((font)->fsSelection & ~FATTR_SEL_STRIKEOUT); \
            } \
        }

    #define _wpi_getfontstrikeout( font ) ( (font)->fsSelection & FATTR_SEL_STRIKEOUT )

    #define _wpi_isfontsizeable( font ) ( (font)->fsDefn & FM_DEFN_OUTLINE )

    #define _wpi_isfonthldsizeable( font_hld ) ( (font_hld)->fsFontUse & FATTR_FONTUSE_OUTLINE )

    #define _wpi_setfontsizeable( font, is_sizeable ) \
        { \
            BOOL                wpi_tmp; \
            wpi_tmp = is_sizeable; \
            if( wpi_tmp ) { \
                (font)->fsDefn |= FM_DEFN_OUTLINE; \
            } else { \
                (font)->fsDefn &= ~FM_DEFN_OUTLINE; \
            } \
        }

    #define _wpi_setfontheight( font, height ) \
        (font)->lMaxBaselineExt = height;

    #define _wpi_setfontwidth( font, width ) \
        (font)->lAveCharWidth = width;

    #define _wpi_setfontpointsize( font, size, pixel_size, match_no ) \
        if( ( (font)->fsDefn & FM_DEFN_OUTLINE ) \
                        || ( (font)->lMaxBaselineExt == 0 ) \
                        || ( (font)->lAveCharWidth == 0 ) ){ \
            (font)->lMaxBaselineExt = size; \
            (font)->lAveCharWidth = size; \
        } \
        (font)->sNominalPointSize = size * 10; \
        if( (font)->sMaximumPointSize < size * 10 ) { \
            (font)->sMaximumPointSize = size * 10; \
        } \
        if( (match_no) >= 0 ) { \
            (font)->lMatch = (match_no); \
        }

    #define _wpi_setfontescapement( font, escapement ) \
        // nothing

    #define _wpi_setfontorientation( font, orientation ) \
        // nothing

    #define _wpi_setfontprecision( font, out, clip ) \
        // nothing

    #define _wpi_setfontquality( font, quality ) \
        // nothing

    #define _wpi_setfontcharset( font, set ) \
        // nothing

    #define _wpi_getfontcharset( font ) 0

    #define _wpi_setfontfacename( font, name ) \
        { \
            char        *wpi_tmp_str = NULL; \
            wpi_tmp_str = name; \
            (font)->szFacename[0] = '\0'; \
            (font)->szFamilyname[0] = '\0'; \
            if( wpi_tmp_str == NULL || strlen( wpi_tmp_str ) < 1 ) { \
                wpi_tmp_str = NULL; \
            } else if( strcmp( name, "MS Sans Serif" ) == 0 ) { \
                strcpy( (font)->szFacename, "Helvetica" ); \
                strcpy( (font)->szFamilyname, "Helvetica" ); \
            } else if( strcmp( name, "MS Serif" ) == 0 ) { \
                strcpy( (font)->szFacename, "Times New Roman" ); \
                strcpy( (font)->szFamilyname, "Times New Roman" ); \
            } else if( strcmp( name, "Arial" ) == 0 ) { \
                strcpy( (font)->szFacename, "Helvetica" ); \
                strcpy( (font)->szFamilyname, "Helvetica" ); \
            } else if( strcmp( name, "Times" ) == 0 ) { \
                strcpy( (font)->szFacename, "Times New Roman" ); \
                strcpy( (font)->szFamilyname, "Times New Roman" ); \
            } else { \
                strcpy( (font)->szFacename, name ); \
                strcpy( (font)->szFamilyname, name ); \
            } \
        }

    #define _wpi_setfontpitch( font, pitch ) \
        // nothing

    #define _wpi_fontmatch( font ) ( (font)->lMatch )

    #define _wpi_getfontpitch( font ) 0

    #define _wpi_getfontheight( font ) ( (font)->lMaxBaselineExt )

extern void _wpi_gettextface( WPI_PRES pres, int size, LPSTR buf );

    #define _wpi_gettextmetrics( pres, tm ) \
                        GpiQueryFontMetrics( pres, sizeof( FONTMETRICS ), tm )

    #define _wpi_metricheight( metric ) (metric).lMaxBaselineExt

    #define _wpi_metricexleading( metric ) (metric).lExternalLeading

    #define _wpi_metricascent( metric ) (metric).lMaxAscender

    #define _wpi_metricdescent( metric ) (metric).lMaxDescender

    #define _wpi_metricavecharwidth( metric ) (metric).lAveCharWidth

    #define _wpi_metricoverhang( metric )       0

extern int _wpi_getmetricpointsize( WPI_PRES pres, WPI_TEXTMETRIC *textmetric,
                                            int *pix_size, int *match_num );

    #define _wpi_metricmaxcharwidth( metric ) max( (metric).lAveCharWidth, \
                                                        (metric).lMaxCharInc )

    #define _wpi_metricileading( metric ) (metric).lInternalLeading

extern void _wpi_enumfonts( WPI_PRES pres, char *facename,
                                        WPI_ENUMFONTPROC proc, char *data );
extern void _wpi_enumchildwindows( HWND hwnd, WPI_ENUMPROC proc, LONG data );

    #define _wpi_getnextwindow( hwnd ) WinGetNextWindow( hwnd )

    #define _wpi_getid( parm1 ) SHORT1FROMMP( parm1 )

    #define _wpi_isbuttoncode( parm1, parm2, code ) \
                                            ( SHORT2FROMMP( parm1 ) == code )

    #define _wpi_switchbuttoncode( parm1, parm2 ) SHORT2FROMMP( parm1 )

    #define _wpi_isbuttonchecked( hwnd, id ) WinQueryButtonCheckstate(hwnd, id)

    #define _wpi_checkdlgbutton( hwnd, id, action ) \
                            WinCheckButton( hwnd, (USHORT) id, (USHORT) action )

extern void _wpi_checkradiobutton( HWND hwnd, int start_id, int end_id,
                                                                int check_id );
extern int _wpi_intersectcliprect( WPI_PRES pres, WPI_RECTDIM left,
                    WPI_RECTDIM top, WPI_RECTDIM right, WPI_RECTDIM bottom );
extern BOOL _wpi_setviewportorg( WPI_PRES pres, int x, int y );

extern HMODULE _wpi_loadlibrary( WPI_INST inst, LPSTR name );

extern void _wpi_freelibrary( WPI_INST inst, HMODULE module );

extern WPI_PROC _wpi_loadprocedure( WPI_INST inst, HMODULE mod, LPSTR proc );

    #define _wpi_loadstring( inst, id, value, len ) \
        WinLoadString( (inst).hab, (inst).mod_handle, (ULONG)id, (LONG)len, \
                                                                (PSZ)value )

    #define _wpi_getclassname( hwnd, buf, size ) \
                            WinQueryClassName( hwnd, (LONG)size, (PCH)buf )

    #define _wpi_getwindowtextlength( hwnd ) WinQueryWindowTextLength( hwnd )

extern LONG _wpi_getbitmapbits( WPI_HANDLE hbitmap, int size, BYTE *bits );
extern LONG _wpi_setbitmapbits( WPI_HANDLE hbitmap, int size, BYTE *bits );

    #define _wpi_getobject( obj, len, info ) len

    #define _wpi_setanchorblock( hwnd, inst ) \
                                    (inst).hab = WinQueryAnchorBlock( hwnd )

    #define _wpi_invalidaterect( hwnd, rect, erase ) \
                                        WinInvalidateRect( hwnd, rect, erase )

    #define _wpi_validaterect( hwnd, rect, children ) \
                                        WinValidateRect( hwnd, rect, children )

extern void _wpi_getbmphdrvalues( WPI_BITMAPINFOHEADER bmih, ULONG *size,
        int *cx, int *cy, short *planes, short *bc, int *comp,
        int *size_image, int *xpels, int *ypels, int *used, int *important );
extern void _wpi_setbmphdrvalues( WPI_BITMAPINFOHEADER *bmih, ULONG size,
        int cx, int cy, short planes, short bc, int comp, int size_image,
        int xpels, int ypels, int used, int important );

    #define _wpi_getbmpheader( bmp ) *bmp

    #define _wpi_getbmpcolors( bmp ) (bmp)->argbColor

extern void _wpi_bmptorgb( WPI_BITMAPINFOHEADER *bmp_info );
extern void _wpi_setimagemode( WPI_PRES pres, USHORT mode );
extern void _wpi_setimagebackmode( WPI_PRES pres, USHORT mode );
extern USHORT _wpi_imagemode( WPI_PRES pres );
extern USHORT _wpi_imagebackmode( WPI_PRES pres );

    #define _wpi_setbackmode( pres, mode ) GpiSetBackMix( pres, mode )

    #define _wpi_getbackmode( pres ) GpiQueryBackMix( pres )

    #define _wpi_getfocus() PM1632WinQueryFocus( HWND_DESKTOP )

    #define _wpi_setfocus( hwnd ) WinSetFocus( HWND_DESKTOP, hwnd )

    #define _wpi_setactivewindow( hwnd ) WinSetActiveWindow( HWND_DESKTOP, hwnd)

    #define _wpi_isitemchecked(hwnd, id) WinIsMenuItemChecked( (HWND) hwnd, id)

    #define _wpi_isitemenabled(hwnd, id) WinIsMenuItemEnabled( (HWND) hwnd, id)

    #define _wpi_getcurrentsysmenu(hwnd) WinWindowFromID(hwnd, FID_SYSMENU)

extern HMENU _wpi_getsysmenu( HWND hwnd );

    #define _wpi_ismenucommand( parm1, parm2 ) \
                                        ( SHORT1FROMMP( parm2 ) == CMDSRC_MENU )

extern void _wpi_deletesysmenupos( HMENU hmenu, SHORT pos );

    #define _wpi_getframe(hclient) PM1632WinQueryWindow(hclient, QW_PARENT)

    #define _wpi_getclient( hframe ) WinWindowFromID( hframe, FID_CLIENT )

    #define _wpi_savepres( pres ) GpiSavePS( pres )

    #define _wpi_restorepres( pres, pres_id ) GpiRestorePS( pres, pres_id )

    #define _wpi_wmgetfont( hwnd, hfont ) \
        WinQueryPresParam( hwnd, PP_FONTHANDLE, 0, NULL, sizeof( WPI_FONT ), \
                                                        hfont, QPF_NOINHERIT )

    #define _wpi_createfont( fontmetrics, wfont ) \
        _wpi_malloc2( wfont, 1 ); \
        _wpi_getfontattrs( &fontmetrics, wfont ); \
        (wfont)->fsType = 0; \

    #define _wpi_createrealfont( fontmetrics, wfont ) \
        { \
            WPI_PRES            pres; \
            _wpi_malloc2( wfont, 1 ); \
            _wpi_getfontattrs( &fontmetrics, wfont ); \
            (wfont)->fsFontUse = FATTR_FONTUSE_OUTLINE; \
            (wfont)->fsType = 0; \
            pres = WinGetScreenPS( HWND_DESKTOP ); \
            if( GpiCreateLogFont( (WPI_PRES) pres, (PSTR8) NULL, (LONG) 2L, \
                        (PFATTRS) wfont ) != FONT_MATCH ) { \
                _wpi_getfontattrs2( fontmetrics, wfont ); \
                (wfont)->fsType = 0; \
                if( GpiCreateLogFont( (WPI_PRES) pres, (PSTR8) NULL, (LONG) 2L, \
                            (PFATTRS) wfont ) != FONT_MATCH ) { \
                    _wpi_getfontattrs2( fontmetrics, wfont ); \
                    (wfont)->lMatch = 0; \
                    (wfont)->fsType = 0; \
                    if( GpiCreateLogFont( (WPI_PRES) pres, (PSTR8) NULL, (LONG) 2L, \
                                (PFATTRS) wfont ) != FONT_MATCH ) { \
                        WPI_FONT                tmp_font; \
                        tmp_font = _wpi_getsystemfont(); \
                        memcpy( wfont, tmp_font, sizeof( FATTRS ) ); \
                    } \
                } \
            } \
            WinReleasePS( pres ); \
        }

extern WPI_FONT _wpi_selectfont( WPI_PRES hps, WPI_FONT wfont );

    #define _wpi_deletefont( wfont ) \
        _wpi_free( wfont );

    #define _wpi_getoldfont( hps, oldfont ) \
        GpiCreateLogFont( hps, NULL, 3L, oldfont ); \
        GpiSetCharSet( hps, 3L ); \
        _wpi_free( oldfont );

extern HBRUSH _wpi_selectbrush( WPI_PRES pres, HBRUSH brush );
extern void _wpi_getoldbrush( WPI_PRES pres, HBRUSH oldbrush );
extern HPEN _wpi_selectpen( WPI_PRES pres, HPEN obj );
extern void _wpi_getoldpen( WPI_PRES pres, HPEN oldobj );

    #define _wpi_rectvisible( pres, rect ) GpiRectVisible( pres, rect )

    #define _wpi_ptvisible( pres, pt ) GpiPtVisible( pres, pt )

extern void _wpi_gettextextent( WPI_PRES pres, LPSTR string, int len_string,
                                                    int *width, int *height );
extern int _wpi_setrop2( WPI_PRES pres, int mode );

    #define _wpi_defwindowproc(hwnd, msg, mp1, mp2) \
                                        WinDefWindowProc(hwnd, msg, mp1, mp2)

    #define _wpi_getsystemmetrics( id ) WinQuerySysValue(HWND_DESKTOP, id)

    #define _wpi_getheightrect( rc ) abs((rc).yTop - (rc).yBottom)

    #define _wpi_getwidthrect( rc ) abs((rc).xRight - (rc).xLeft)

    #define _wpi_dismissdialog( hwnd, ret ) WinDismissDlg( hwnd, ret )

    #define _wpi_destroywindow( hwnd ) WinDestroyWindow( hwnd )

    #define _wpi_setwindowpos(hwnd, z, x, y, cx, cy, flags) \
                        WinSetWindowPos(hwnd, z, x, y, cx, cy, flags)

extern BOOL _wpi_iswindow( WPI_INST inst, HWND hwnd );

    #define _wpi_iswindowshowing( hwnd ) WinIsWindowShowing( hwnd )

    #define _wpi_updatewindow( hwnd ) WinUpdateWindow( hwnd )

    #define _wpi_bringwindowtotop( hwnd ) \
                WinSetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER)

    #define _wpi_setcapture(hwnd) WinSetCapture(HWND_DESKTOP, hwnd)

    #define _wpi_releasecapture() WinSetCapture(HWND_DESKTOP, NULLHANDLE)

    #define _wpi_getcapture() PM1632WinQueryCapture( HWND_DESKTOP )

    #define _wpi_selectpalette(hps, hpal) GpiSelectPalette(hps, hpal)

extern int _wpi_getdibits( WPI_PRES pres, WPI_HANDLE bitmap, UINT start,
            UINT slcount, BYTE *buffer, WPI_BITMAPINFO *info, UINT notused );
extern WPI_HANDLE _wpi_createdibitmap( WPI_PRES pres, WPI_BITMAP *info,
                    ULONG opt, BYTE *data, WPI_BITMAPINFO *table, int opt2 );
extern HBITMAP _wpi_createbitmap( int width, int height, int planes,
                                                    int bitcount, BYTE *bits );

    #define _wpi_fatalappexit( inst, num, string ) WinTerminate( (inst).hab )

extern void _wpi_recttowpirect( RECT *src_rc, WPI_RECT *dest_rc );
extern void _wpi_wpirecttorect( WPI_RECT *src_rc, RECT *dest_rc );

    #define _wpi_inflaterect(inst, r, cx, cy) \
                                        WinInflateRect((inst).hab, r, cx, cy)

    #define _wpi_unrealizeobject( hobj ) \
        // do nothing right now

    #define _wpi_getlocalhdl( mem ) mem

    #define _wpi_getglobalhdl( mem ) mem

    #define _wpi_getmenu( hframe ) WinWindowFromID( hframe, FID_MENU )

    #define _wpi_showwindow(hwnd, state) _wpi_showwindow( hwnd, state )

    #define _wpi_setwindowtext( hwnd, text ) WinSetWindowText( hwnd, text )

    #define _wpi_registerclipboardformat( fmt ) \
                                WinAddAtom( WinQuerySystemAtomTable(), fmt )

    #define _wpi_isclipboardformatavailable( inst, fmt, info ) \
                                WinQueryClipbrdFmtInfo( (inst).hab, fmt, info )

    #define _wpi_openclipboard( inst, hwnd ) WinOpenClipbrd( (inst).hab )

    #define _wpi_closeclipboard( inst ) WinCloseClipbrd( (inst).hab )

    #define _wpi_getclipboardowner( inst ) WinQueryClipbrdOwner( (inst).hab )

    #define _wpi_loadmenu( inst, id ) (HWND) \
                    WinLoadMenu( HWND_DESKTOP, (inst).mod_handle, (ULONG) id )

    #define _wpi_setmenuitemtext( hwnd, item_id, text ) \
                                WinSetMenuItemText( (HWND) hwnd, item_id, text )

    #define _wpi_getsyscursor( id ) WinQuerySysPointer( HWND_DESKTOP, id, FALSE)

    #define _wpi_loadcursor( winst, id ) \
                        WinLoadPointer( HWND_DESKTOP, (winst).mod_handle, id )

    #define _wpi_destroycursor( hptr ) WinDestroyPointer( hptr )

extern HCURSOR _wpi_setcursor( HCURSOR newcursor );
extern void _wpi_setclipboarddata( WPI_INST inst, UINT format, WPI_HANDLE data,
                                                                BOOL is_bitmap);
extern WPI_HANDLE _wpi_getclipboarddata( WPI_INST inst, UINT format );

    #define _wpi_emptyclipboard( inst ) WinEmptyClipbrd( (inst).hab )

    #define _wpi_createdialog( inst, id, hwnd, proc ) \
            WinLoadDlg( HWND_DESKTOP, hwnd, (WPI_PROC)proc, (inst).mod_handle, \
                                                    (ULONG)id, (PVOID)NULL )

    #define _wpi_enablewindow( hwnd, bool ) WinEnableWindow( hwnd, bool )

    #define _wpi_ismenuitemvalid( hwnd, id ) WinIsMenuItemValid( hwnd, id )

extern DWORD _wpi_getmessagepos( WPI_INST inst );

    #define _wpi_windowfrompoint( pt ) \
                            PM1632WinWindowFromPoint( HWND_DESKTOP, pt, TRUE )

    #define _wpi_childwindowfrompoint( hwnd, pt ) \
                                    PM1632WinWindowFromPoint( hwnd, pt, FALSE )

extern BOOL _wpi_movewindow( HWND hwnd, int x, int y, int width, int height,
                                                            BOOL repaint );
extern HINI _wpi_openinifile( WPI_INST inst, char *name );

    #define _wpi_closeinifile( hini ) PrfCloseProfile( hini )

    #define _wpi_getprivateprofileint( hini, app, key, default, name ) \
            PrfQueryProfileInt( hini, app, key, default )

extern int _wpi_getprivateprofilestring( HINI hini, LPSTR app,
                LPSTR key, LPSTR def, LPSTR buf, int size, LPSTR dummy );

    #define _wpi_writeprivateprofilestring( hini, app, key, data, name ) \
            PrfWriteProfileString( hini, app, key, data )

extern void _wpi_getbitmapparms( WPI_HANDLE hbitmap, int *width, int *height,
                        int *planes, int *notused1, int *bitcount );
extern LONG _wpi_getbitmapstruct( WPI_HANDLE bitmap, WPI_BITMAP *info );
#define _wpi_bitmapwidth( pinfo ) (pinfo)->cx
#define _wpi_bitmapheight( pinfo ) (pinfo)->cy
#define _wpi_bitmapbitcount( pinfo ) (pinfo)->cBitCount
#define _wpi_bitmapplanes( pinfo ) (pinfo)->cPlanes
#define _wpi_bitmapbitspixel( pinfo ) 0             // not used in PM
extern WPI_COLOUR _wpi_getpixel( WPI_PRES hps, int x, int y );
extern BOOL _wpi_extfloodfill( WPI_PRES hps, int x, int y, WPI_COLOUR clr,
                                                                UINT mode );
extern void _wpi_getinidirectory( WPI_INST inst, LPSTR dir_info, int size );

    #define _wpi_getasynckeystate( code ) \
                                    WinGetPhysKeyState( HWND_DESKTOP, code )

    #define _wpi_escape( dc, code, incount, indata, outdata, outcount ) \
         DevEscape( dc, code, incount, indata, outcount, outdata )

extern BOOL _wpi_isiconic( HWND hwnd );

#define GET_WM_MOUSEMOVE_POSX( parm1, parm2 ) _wpi_getmousex( parm1, parm2 )
#define GET_WM_MOUSEMOVE_POSY( parm1, parm2 ) _wpi_getmousey( parm1, parm2 )

    #define GET_WM_INITMENU_MENU( parm1, parm2 ) (HMENU) parm2

    #define GET_WM_VSCROLL_CMD( parm1, parm2 ) SHORT2FROMMP( parm2 )

    #define GET_WM_VSCROLL_POS( parm1, parm2 ) SHORT1FROMMP( parm2 )

    #define WPI_MAKEP2( x1, x2 ) MPFROM2SHORT( x1, x2 )

    #define WPI_MAKEP1( x1, x2 ) MPFROM2SHORT( x1, x2 )

    #define _wpi_copyrect( dst, src ) memcpy( (dst), (src), sizeof(WPI_RECT) )

    #define _wpi_setredraw( hwnd, fdraw ) WinEnableWindowUpdate( hwnd, fdraw )

    #define _wpi_getfirstchild( hwnd ) PM1632WinQueryWindow( hwnd, QW_TOP )

    #define IsWindowVisible(hwnd) WinIsWindowVisible(hwnd)

void _wpi_getrestoredrect( HWND hwnd, WPI_RECT *prect );

    #define _wpi_setrestoredrect( hwnd, prect ) \
        WinSetWindowPos( (hwnd), NULL, (prect)->xLeft, (prect)->yBottom, \
                         (prect)->xRight - (prect)->xLeft, \
                         (prect)->yTop   - (prect)->yBottom, \
                         SWP_SHOW ) \

    #define _wpi_createpopupmenu() WinCreateMenu( HWND_DESKTOP, NULL )

    #define _wpi_getmenuitemcount( hmenu ) \
                                    WinSendMsg( hmenu, MM_QUERYITEMCOUNT, 0, 0 )

    #define _wpi_maximizewindow( hwnd ) \
                        WinSetWindowPos( (hwnd), NULL, 0,0,0,0, SWP_MAXIMIZE )

    #define _wpi_minimizewindow( hwnd ) \
                        WinSetWindowPos( (hwnd), NULL, 0,0,0,0, SWP_MINIMIZE )

extern BOOL _wpi_iszoomed( HWND hwnd );

    #define _wpi_iswindowvisible( hwnd ) WinIsWindowVisible( hwnd )

    #define _wpi_getupdaterect( hwnd, prect ) WinQueryUpdateRect( hwnd, prect )

    #define _wpi_subclasswindow( hwnd, new ) \
        ( (WPI_PROC) WinSubclassWindow( hwnd, new ) )

extern BOOL _wpi_insertmenu( HMENU hmenu, unsigned pos, unsigned menu_flags,
                             unsigned attr_flags, unsigned id,
                             HMENU popup, char *text, BOOL by_position );
extern BOOL _wpi_appendmenu( HMENU hmenu, unsigned menu_flags,
                             unsigned attr_flags, unsigned id,
                             HMENU popup, char *text );
extern BOOL _wpi_getmenustate( HMENU hmenu, unsigned id, WPI_MENUSTATE *state,
                               BOOL by_position );
extern void _wpi_getmenuflagsfromstate( WPI_MENUSTATE *state,
                                        unsigned *menu_flags,
                                        unsigned *attr_flags );

    #define _wpi_ismenucheckedfromstate( pstate ) \
                                        ( (pstate)->afAttribute & MIA_CHECKED )

    #define _wpi_ismenuenabledfromstate( pstate ) \
                                    ( !((pstate)->afAttribute & MIA_DISABLED) )

    #define _wpi_ismenuseparatorfromstate( pstate ) \
                                        ( (pstate)->afStyle & MIS_SEPARATOR )

    #define _wpi_ismenupopupfromstate( pstate ) \
                                            ( (pstate)->afStyle & MIS_SUBMENU )

extern BOOL _wpi_modifymenu( HMENU hmenu, unsigned id, unsigned menu_flags,
                             unsigned attr_flags, unsigned new_id,
                             HMENU new_popup, char *new_text, BOOL by_position );

    #define _wpi_createmenu() WinCreateMenu( HWND_DESKTOP, NULL )

extern HMENU _wpi_getsubmenu( HMENU hmenu, unsigned pos );
extern HMENU _wpi_getsystemmenu( HWND hwnd );
extern BOOL _wpi_setmenu( HWND hwnd, HMENU hmenu );
extern BOOL _wpi_deletemenu( HMENU hmenu, unsigned id, BOOL by_position );
extern BOOL _wpi_checkmenuitem( HMENU hmenu, unsigned id,
                                BOOL fchecked, BOOL by_position );
extern BOOL _wpi_enablemenuitem( HMENU hmenu, unsigned id,
                                 BOOL fenabled, BOOL by_position );
extern BOOL _wpi_setmenutext( HMENU hmenu, unsigned id,
                              char *text, BOOL by_position );
extern BOOL _wpi_getmenutext( HMENU hmenu, unsigned id, char *text, int ctext,
                              BOOL by_position );
extern UINT _wpi_getmenuitemid( HMENU hmenu, unsigned pos );

    #define _wpi_destroymenu( hmenu ) WinDestroyWindow( hmenu )

extern int _wpi_getdevicecaps( WPI_PRES pres, int what );

    #define _wpi_restorewindow( hwnd ) \
        WinSetWindowPos( hwnd, HWND_TOP, 0,0,0,0, SWP_RESTORE )

extern void _wpi_getfontattrs( FONTMETRICS *fm, WPI_FONT attr );

extern BOOL _wpi_setscrollpos( HWND parent, int scroll, int pos, BOOL redraw );
extern int _wpi_getscrollpos( HWND hwnd, int scroll );
extern BOOL _wpi_setscrollrange( HWND hwnd, int scroll, int min, int max, BOOL redraw );
extern BOOL _wpi_getscrollrange( HWND hwnd, int scroll, int *min, int *max );

extern BOOL _wpi_drawtext( WPI_PRES hps, char *text, int len,
                           WPI_RECT *rect, unsigned format );

extern WPI_FONT _wpi_getsystemfont( void );

extern void _wpi_drawmenubar( HWND hwnd );

    #define _wpi_scrollwindow( hwnd, dx, dy, scroll, clip ) \
        WinScrollWindow( hwnd, dx, -(dy), scroll, clip, NULLHANDLE, \
                         NULLHANDLE, SW_INVALIDATERGN )

extern HWND _wpi_getparent( HWND hwnd );

    #define _wpi_setparent( win, parent ) \
        WinSetParent( win, parent, TRUE )

    #define _wpi_getkeystate( vkey ) \
                                WinGetKeyState( HWND_DESKTOP, (int)(vkey) )

    #pragma aux __wpi_selectobject parm [];
extern WPI_HANDLE __wpi_selectobject( WPI_PRES pres, WPI_HANDLE v_obj,
                                                        void *v_old_obj );
    #define _wpi_selectobject( pres, obj ) \
        __wpi_selectobject( pres, obj, alloca( sizeof(WPI_OBJECT) ) )

extern void _wpi_deleteobject( WPI_HANDLE object );
extern char *_wpi_menutext2pm( char *text );
extern void _wpi_menutext2win( char *text );

    #define _wpi_freemenutext( ptext ) _wpi_free( ptext )

    #define _wpi_is_close_menuselect( p1, p2 ) ( SHORT1FROMMP( p1 ) == 0xffff )

    #define GET_WM_MENUSELECT_ITEM( p1, p2 ) ( SHORT1FROMMP( p1 ) )

extern BOOL _wpi_trackpopupmenu( HMENU hmenu, ULONG flags, LONG x, LONG y,
                                 HWND parent );
extern void _wpi_setrgbquadvalues( WPI_RGBQUAD *rgb, BYTE red, BYTE green,
                                                    BYTE blue, BYTE option );

extern LONG _wpi_getclipbox( WPI_PRES pres, WPI_PRECT rcl );

extern BOOL _wpi_equalrect( WPI_PRECT prect1, WPI_PRECT prect2 );
extern WPI_COLOUR _wpi_palettergb( WPI_PRES, short, short, short );
extern int _wpi_dlg_command( HWND dlg_hld, WPI_MSG *msg, WPI_PARAM1 *parm1,
                                                        WPI_PARAM2 *parm2 );
extern void _wpi_linedda( int x1, int y1, int x2, int y2,
                                    WPI_LINEDDAPROC line_proc, WPI_PARAM2 lp );

extern BOOL _wpi_is_dbcs( void );

extern void _wpi_fix_dbcs( HWND dlg );
