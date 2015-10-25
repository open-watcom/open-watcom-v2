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


#define MAX_DRAW_WIDTH  512
#define MAX_DRAW_HEIGHT 256
#define POINTSIZE_MIN   3
#define MAKELOGPTX( a ) ((int)(a - (a % pointSize.x)))
#define MAKELOGPTY( a ) ((int)(a - (a % pointSize.y)))

// Macros for 16-bit vs. NT
#if defined( __OS2_PM__ )
    #define SET_HWND_PARAM2( lp, hwnd )     lp = (WPI_PARAM2)hwnd
    #define GET_HWND_PARAM2( lp )           (HWND)lp
#elif defined( __NT__ )
    #define SET_HWND_PARAM2( lp, hwnd )     lp = (LPARAM)hwnd
    #define GET_HWND_PARAM2( lp )           (HWND)lp
#else
    #define SET_HWND_PARAM2( lp, hwnd )     lp = MAKELPARAM( hwnd, 0 )
    #define GET_HWND_PARAM2( lp )           (HWND)LOWORD( lp )
#endif

WINEXPORT BOOL CALLBACK GridEnumProc( HWND hwnd, LONG lparam );
