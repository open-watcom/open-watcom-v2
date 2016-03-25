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


typedef enum CursorOps {
    COP_INIT,
    COP_FINI,
    COP_ARROW,
    COP_DROPFT,
    COP_DROPCLR,
    COP_NODROP,
    COP_STATMOVE,
    COP_DROPSS
} CursorOps;

enum {
    DIVIDE_BETWEEN,
    DIVIDE_MIDDLE
};

enum drawUpDown {
    DRAW_UP,
    DRAW_DOWN
};

//extern int  TextWidth( HWND, char *, int, int );
//extern int  StringWidth( HWND, char *, int );
extern void WriteText( window_id, int, int, type_style *, const char *, int );
extern void WriteString( window_id, int, int, type_style *, const char *);
extern HDC  TextGetDC( window_id, type_style *);
extern void TextReleaseDC( window_id, HDC );
extern int  MyTextExtent( window_id, type_style *, char *, unsigned );
extern int  MyStringExtent( window_id, type_style *, char * );
extern void BlankRect( window_id, vi_color, int, int, int, int );
extern void BlankRectIndirect( window_id, vi_color, RECT * );
extern void ClientToRowCol( window_id, int, int, int *, int *, int );
extern void ToggleHourglass( bool );
int         GetNumWindowTypes( void );
HWND        GetOwnedWindow( POINT );
void        CursorOp( CursorOps );
void        MoveWindowTopRight( HWND );
void        UpdateIntSetting( HWND, int, int, long );
void        UpdateStrSetting( HWND, int, int, char * );
void        DoStrSet( char *, int );
void        UpdateBoolSetting( HWND, int, int, bool );
void        SetEditInt( HWND, UINT, int );
void        DrawRectangleUpDown( HWND, int );
