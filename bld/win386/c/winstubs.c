/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Covers for several base Windows API functions (16-bit code).
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <windows.h>
#include "bool.h"
#include "winext.h"
#include "_windpmi.h"
#include "winstubs.h"
#include "windata.h"


extern void PutByte( char, WORD, DWORD );
#pragma aux PutByte = \
        "shl  edx,16"       \
        "mov  dx,ax"        \
        "mov  es:[edx],bl"  \
    __parm      [__bl] [__es] [__dx __ax] \
    __value     \
    __modify    [__dx]

extern char GetByte( WORD, DWORD );
#pragma aux GetByte = \
        "shl  edx,16"       \
        "mov  dx,ax"        \
        "mov  al,es:[edx]"  \
    __parm      [__es] [__dx __ax] \
    __value     [__al] \
    __modify    [__dx]

/*
 * GetAlias - get a 16 bit alias to 32 bit memory
 */
DWORD GetAlias( LPDWORD ptr )
{
    DWORD       alias;
    DWORD       orig;

    orig = *ptr;
    if( orig >= 0xFFFF0000 ) {
        *ptr = orig & 0xFFFFL;
//    } else if( orig >= DataSelectorSize ) {
    } else if( orig ) {
        _WDPMI_GetAlias( orig, &alias );
        *ptr = alias;
    }
    return( orig );

} /* GetAlias */

/*
 * ReleaseAlias - give back a orig data to 32 bit memory
 */
void ReleaseAlias( LPDWORD ptr, DWORD orig )
{
    DWORD   alias;

    alias = *ptr;
    if( alias != orig ) {
        *ptr = orig;
        _WDPMI_FreeAlias( alias );
    }

} /* ReleaseAlias */


/*
 * __GetMessage - cover for GetMessage
 */
BOOL FAR PASCAL __GetMessage( LPMSG msg, HWND a, WORD b, WORD c )
{
    MSG         tmpmsg;
    BOOL        rc;

    rc = GetMessage( &tmpmsg, a, b, c );
    *msg = tmpmsg;
    return( rc );

} /* __GetMessage */

/*
 * __PeekMessage - cover for PeekMessage
 */
BOOL FAR PASCAL __PeekMessage( LPMSG msg, HWND a, WORD b, WORD c, WORD d )
{
    MSG         tmpmsg;
    BOOL        rc;

    rc = PeekMessage( &tmpmsg, a, b, c, d );
    *msg = tmpmsg;
    return( rc );

} /* __PeekMessage */

#if 0
/*
 * __RegisterClass - cover for register class
 */
BOOL  FAR PASCAL __RegisterClass( LPWNDCLASS wc )
{
    BOOL        rc;
    DWORD       odata1;
    DWORD       odata2;

    odata1 = GETALIAS( &wc->lpszMenuName );
    odata2 = GETALIAS( &wc->lpszClassName );
    rc = RegisterClass( wc );
    RELEASEALIAS( &wc->lpszClassName, odata2 );
    RELEASEALIAS( &wc->lpszMenuName, odata1 );

    return( rc );

} /* __RegisterClass */
#endif

/*
 * __ModifyMenu - cover function for ModifyMenu
 */
BOOL FAR PASCAL __ModifyMenu( HMENU a, WORD b, WORD fl, WORD d, LPSTR z )
{
    DWORD       alias;
    BOOL        rc;

    if( !( ( (fl & MF_OWNERDRAW) != 0 ) || ( (fl & MF_BITMAP) != 0 ) ) && z != NULL ) {
        _WDPMI_GetAlias( (DWORD)z, &alias );
        rc = ModifyMenu( a, b, fl, d, (LPSTR)alias );
        _WDPMI_FreeAlias( alias );
    } else {
        rc = ModifyMenu( a, b, fl, d, z );
    }
    return( rc );

} /* __ModifyMenu */

/*
 * __InsertMenu - cover function for InsertMenu
 */
BOOL  FAR PASCAL __InsertMenu( HMENU a, WORD b, WORD fl, WORD d, LPSTR z )
{
    DWORD       alias;
    BOOL        rc;

    if( !( ( (fl & MF_OWNERDRAW) != 0 ) || ( (fl & MF_BITMAP) != 0 ) ) && z != NULL ) {
        _WDPMI_GetAlias( (DWORD)z, &alias );
        rc = InsertMenu( a, b, fl, d, (LPSTR)alias );
        _WDPMI_FreeAlias( alias );
    } else {
        rc = InsertMenu( a, b, fl, d, z );
    }
    return( rc );

} /* __InsertMenu */

/*
 * __AppendMenu - cover function for AppendMenu
 */
BOOL  FAR PASCAL __AppendMenu( HMENU a, WORD fl, WORD c, LPSTR z )
{
    DWORD       alias;
    BOOL        rc;

    if( !( ( (fl & MF_OWNERDRAW) != 0 ) || ( (fl & MF_BITMAP) != 0 ) ) && z != NULL ) {
        _WDPMI_GetAlias( (DWORD)z, &alias );
        rc = AppendMenu( a, fl, c, (LPSTR)alias );
        _WDPMI_FreeAlias( alias );
    } else {
        rc = AppendMenu( a, fl, c, z );
    }
    return( rc );

} /* __AppendMenu */

/*
 * __Escape - cover function for escape
 */
int FAR PASCAL __Escape( HDC a, int b, int c, LPSTR d, LPSTR e )
{
    int         rc;
    DWORD       odata;

    /*
     * no alias for d yet, since sometimes it is a function pointer
     * (bloody microsoft weenies)
     */
    if( b == NEXTBAND ) {
        RECT    r;

        r = *(LPRECT)e;
        rc = Escape( a, b, c, NULL, &r );
        *(LPRECT)e = r;
    } else if( b == SETABORTPROC ) {
        rc = Escape( a, b, c, d, e );
    } else {
        odata = GETALIAS( &d );
        rc = Escape( a, b, c, d, e );
        RELEASEALIAS( &d, odata );
    }
    return( rc );

} /* __Escape */

/*
 * __GetInstanceData - cover for get instance data.
 *
 *      Another Microsoft nightmare from hell. To make this work
 *      in 32 bit land, a special kludge is needed.  we obtain the
 *      data selector of the other instance, and then copy the
 *      data at the specified 32-bit offset into the 32-bit offset
 *      of ourselves.
 */
int FAR PASCAL __GetInstanceData( HANDLE a, DWORD offset, int len )
{
    int         got;
    WORD        sel;
    char        ch;

    got = GetInstanceData( a, (PBYTE)&MyDataSelector, sizeof( WORD ) );
    if( got == 2 ) {
        sel = MyDataSelector;
        MyDataSelector = DataSelector;
        for( got = len; got > 0; got-- ) {
            ch = GetByte( sel, offset );
            PutByte( ch, DataSelector, offset );
            offset++;
        }
    } else {
        return( 0 );
    }
    return( len );

} /* __GetInstanceData */

/*
 * __AnsiPrev - cover function for AnsiPrev
 *              we need to hack and slash to get the correct pointer
 *              back to the 32-bit previous character
 */
LPSTR FAR PASCAL __AnsiPrev( LPSTR a, LPSTR b )
{
    LPSTR       b2;
    LPSTR       res;
    DWORD       alias;

    _WDPMI_GetAlias( (DWORD)a, &alias );
    b2 = (LPSTR)( alias + ( (DWORD)b - (DWORD)a ) );
    res = AnsiPrev( (LPSTR)alias, b2 );
    res = a + ( (DWORD)res - (DWORD)alias );
    _WDPMI_FreeAlias( alias );
    return( res );

} /* __AnsiPrev */

/*
 * __AnsiNext - cover function for AnsiNext
 *              we need to hack and slash to get the correct pointer
 *              back to the 32-bit next character
 */
LPSTR FAR PASCAL __AnsiNext( LPSTR a )
{
    LPSTR       res;
    DWORD       alias;

    _WDPMI_GetAlias( (DWORD)a, &alias );
    res = AnsiNext( (LPSTR)alias );
    res = (LPSTR)( (DWORD)a + ( (DWORD)res - (DWORD)alias ) );
    _WDPMI_FreeAlias( alias );
    return( res );

} /* __AnsiNext */

/*
 * __StartDoc - cover function for StartDoc
 */
int FAR PASCAL __StartDoc( HDC hdc, LPDOCINFO di )
{
    int         rc;
    DWORD       odata1;
    DWORD       odata2;

    odata1 = GETALIAS( &di->lpszDocName );
    odata2 = GETALIAS( &di->lpszOutput );
    rc = StartDoc( hdc, di );
    RELEASEALIAS( &di->lpszOutput, odata2 );
    RELEASEALIAS( &di->lpszDocName, odata1 );
    return( rc );

} /* __StartDoc */

/*
 * __WinHelp - cover function for WinHelp
 */
BOOL FAR PASCAL __WinHelp( HWND hwnd, LPCSTR hfile, UINT cmd, DWORD data )
{
    DWORD       odata;
    BOOL        rc;

    switch( cmd ) {
    case HELP_KEY:
    case HELP_PARTIALKEY:
    case HELP_MULTIKEY:
    case HELP_COMMAND:
    case HELP_SETWINPOS:
        odata = GETALIAS( &data );
        rc = WinHelp( hwnd, hfile, cmd, data );
        RELEASEALIAS( &data, odata );
        break;
    default:
        rc = WinHelp( hwnd, hfile, cmd, data );
        break;
    }
    return( rc );

} /* __WinHelp */
