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
         *  Type definitions for pm
         */
#ifdef __OS2_PM__

        typedef struct {
            BYTE        rgbBlue;
            BYTE        rgbGreen;
            BYTE        rgbRed;
            BYTE        rgbReserved;
        } RGBQUAD;



#else
#endif
        /*
         * Definitions for pm
         */
#ifdef __OS2_PM__

        #define WM_QUERYENDSESSION  WM_SAVEAPPLICATION
        #define WM_MDIACTIVATE      WM_ACTIVATE
        #define FARstricmp      stricmp

        #define _imgwpi_issizemaximized( p1 ) FALSE

        #define _imgwpi_issizeminimized( p1 ) FALSE

        #define _imgwpi_issizerestored( p1 ) TRUE

        #define _wpi_dlgreturn(flag) return( MRFROMSHORT(flag) )

        #define _wpi_getcolourindex( hps, rgbclr ) \
                        GpiQueryColorIndex( hps, 0L, rgbclr );

        #define _imgwpi_mresulttoint( mresult ) LONGFROMMR( mresult )

        #define _imgwpi_defMDIchildproc( hw, msg, wp, lp ) \
                            _wpi_defwindowproc( hw, msg, wp, lp )

        #define _imgwpi_defframeproc( hw, hclw, msg, wp, lp ) \
                            _wpi_defwindowproc( hw, msg, wp, lp )
        /*
         * Definitions for win
         */
#else
        #define _wpi_dlgreturn(flag) return( flag )

        #define _wpi_getcolourindex( hdc, rgbclr )  rgbclr

        #define _imgwpi_mresulttoint( mresult ) mresult

        #define _imgwpi_issizemaximized( wp ) ( LOWORD(wp) == SIZE_MAXIMIZED )

        #define _imgwpi_issizeminimized( wp ) ( LOWORD(wp) == SIZE_MINIMIZED )

        #define _imgwpi_issizerestored( wp ) ( LOWORD(wp) == SIZE_RESTORED )

        #define _imgwpi_defMDIchildproc( hw, msg, wp, lp ) \
                        DefMDIChildProc( hw, msg, wp, lp )

        #define _imgwpi_defframeproc( hw, hclw, msg, wp, lp ) \
                        DefFrameProc( hw, hclw, msg, wp, lp )
#endif
