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


#ifndef PM1632_H
#define PM1632_H
#ifdef __FLAT__
    #define PM1632WinQueryWindow( hwnd, query ) WinQueryWindow( hwnd, query )
    #define PM1632WinQueryCapture( parm ) WinQueryCapture( parm )
    #define PM1632WinQueryFocus( parm ) WinQueryFocus( parm )
    #define PM1632SWP_FLAG( swp ) ( (swp).fl)
    #define PM1632_WINDOW_MSG   ULONG
    #define PM1632_BITMAPINFO2          BITMAPINFO2
    #define PM1632_BITMAPINFOHEADER2    BITMAPINFOHEADER2
    #define PM1632_RGB2                 RGB2
    #define PM1632WinWindowFromPoint( hwnd, pt, bool ) WinWindowFromPoint( hwnd, pt, bool )
    #define TPM_LEFTALIGN               0
    #define TPM_LEFTBUTTON              PU_MOUSEBUTTON1
    #define TPM_RIGHTBUTTON             PU_MOUSEBUTTON2
    #define PM1632_APIRET               APIRET
    #define PM1632_FILESIZETYPE         ULONG
#else
    #define PM1632WinQueryWindow( hwnd, query ) WinQueryWindow( hwnd, query, 0 )
    #define PM1632WinQueryCapture( parm ) WinQueryCapture( parm, 0 )
    #define PM1632WinQueryFocus( parm ) WinQueryFocus( parm, 0 )
    #define PM1632SWP_FLAG( swp ) ( (swp).fs)
    #define PM1632_WINDOW_MSG   USHORT
    #define PM1632_BITMAPINFO2          BITMAPINFO
    #define PM1632_BITMAPINFOHEADER2    BITMAPINFOHEADER
    #define PM1632_RGB2                 RGB
    #define PM1632WinWindowFromPoint( hwnd, pt, bool ) WinWindowFromPoint( hwnd, pt, bool, FALSE )
    #define PU_KEYBOARD                 0
    #define PU_HCONSTRAIN               0
    #define PU_VCONSTRAIN               0
    #define TPM_LEFTALIGN               0
    #define TPM_LEFTBUTTON              0
    #define TPM_RIGHTBUTTON             0
    #define PM1632_APIRET               USHORT
    #define PM1632_FILESIZETYPE         USHORT
#endif

#define CALLBACK EXPENTRY
#endif // PM1632_H
