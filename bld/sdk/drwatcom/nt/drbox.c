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


/*
 * LBPrintf - printf to a list box
 */
void LBPrintf( char *str, ... )
{
    char        tmp[256];
    va_list     al;
    HDC         dc;
    SIZE        sz;
    LRESULT     item;
    HFONT       oldfont, newfont;

    va_start( al, str );
    vsprintf( tmp, str, al );
    va_end( al );
    item = SendMessage( ListBox, LB_ADDSTRING, 0, (LONG)(LPSTR)tmp );
    SendMessage( ListBox, LB_SETTOPINDEX, index, 0 );
    dc = GetDC( ListBox );
    newfont = GetMonoFont();
    oldfont = SelectObject( dc, newfont);
    GetTextExtentPoint( dc, tmp, strlen( tmp ), &sz );
    SelectObject( dc, oldfont);
    ReleaseDC( ListBox, dc );
    if( sz.cx > ListTextWidth ) {
        ListTextWidth = sz.cx;
        SendMessage( ListBox, LB_SETHORIZONTALEXTENT, ListTextWidth, 0 );
        ListLongestItem = item;
    }

} /* LBPrintf */
