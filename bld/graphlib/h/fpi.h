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


/* Since the calling conventions for -fpc and -fpi(87) are different,
   the following routines have been defined (for WATFOR-77). These routines
   are called instead of the regular ones if __FPI__ is defined */

short _WCI86FAR _CGRAPH          _lineto_w_87( double, double );
short _WCI86FAR _CGRAPH          _rectangle_w_87( short, double, double, double, double );
short _WCI86FAR _CGRAPH          _arc_w_87( double, double, double, double, double,
                                       double, double, double );
short _WCI86FAR _CGRAPH          _ellipse_w_87( short, double, double, double, double );
short _WCI86FAR _CGRAPH          _pie_w_87( short, double, double, double, double,
                                       double, double, double, double );
short _WCI86FAR _CGRAPH          _floodfill_w_87( double, double, short );
short _WCI86FAR _CGRAPH          _setpixel_w_87( double, double );
short _WCI86FAR _CGRAPH          _getpixel_w_87( double, double );
struct xycoord _WCI86FAR _CGRAPH  _getviewcoord_w_87( double, double );
struct _wxycoord _WCI86FAR _CGRAPH _moveto_w_87( double, double );
short _WCI86FAR _CGRAPH          _setwindow_87( short, double, double, double, double );
void _WCI86FAR _CGRAPH           _setcharsize_w_87( double, double );
void _WCI86FAR _CGRAPH           _setcharspacing_w_87( double );
short _WCI86FAR _CGRAPH          _grtext_w_87( double, double, char _WCI86FAR * );
void _WCI86FAR _CGRAPH           _getimage_w_87( double, double, double, double, char _WCI86HUGE * );
void _WCI86FAR _CGRAPH           _putimage_w_87( double, double, char _WCI86HUGE *, short );
long _WCI86FAR _CGRAPH           _imagesize_w_87( double, double, double, double );

// Only use eax if not using stack calling conventions
#ifndef __STACK
#if defined( __386__ )
    #pragma aux fpi parm [eax];
#else
    #pragma aux fpi parm [ax];
#endif
#endif

#pragma aux (fpi)  _arc_w_87;
#pragma aux (fpi)  _ellipse_w_87;
#pragma aux (fpi)  _floodfill_w_87;
#pragma aux (fpi)  _getimage_w_87;
#pragma aux (fpi)  _getpixel_w_87;
#pragma aux (fpi)  _getviewcoord_w_87;
#pragma aux (fpi)  _grtext_w_87;
#pragma aux (fpi)  _imagesize_w_87;
#pragma aux (fpi)  _lineto_w_87;
#pragma aux (fpi)  _moveto_w_87;
#pragma aux (fpi)  _pie_w_87;
#pragma aux (fpi)  _putimage_w_87;
#pragma aux (fpi)  _rectangle_w_87;
#pragma aux (fpi)  _setcharsize_w_87;
#pragma aux (fpi)  _setcharspacing_w_87;
#pragma aux (fpi)  _setpixel_w_87;
#pragma aux (fpi)  _setwindow_87;
