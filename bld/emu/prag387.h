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


/* pragmas for inline 387 instructions */

extern double sqrt(double);
extern double fabs(double);
extern double sin(double);
extern double cos(double);
extern double tan(double);
extern double atan(double);
extern double log(double);
extern double log10(double);

#pragma aux     sqrt  = 0xd9 0xfa parm caller [8087] value [8087];
#pragma aux     fabs  = 0xd9 0xe1 parm caller [8087] value [8087];
#pragma aux     sin   = 0xd9 0xfe parm caller [8087] value [8087];
#pragma aux     cos   = 0xd9 0xff parm caller [8087] value [8087];
#pragma aux     log   = 0xd9 0xed  /* fldln2 */ \
                        0xd9 0xc9  /* fxch   */ \
                        0xd9 0xf1  /* fyl2x  */ \
                                parm caller [8087] value [8087];
#pragma aux     log10 = 0xd9 0xec  /* fldlg2 */ \
                        0xd9 0xc9  /* fxch   */ \
                        0xd9 0xf1  /* fyl2x  */ \
                                parm caller [8087] value [8087];
#pragma aux     tan   = 0xd9 0xf2  /* fptan      */ \
                        0xd9 0xd8  /* fstp st(0) */ \
                                parm caller [8087] value [8087];
#pragma aux     atan  = 0xd9 0xe8  /* fld1   */ \
                        0xd9 0xf3  /* fpatan */ \
                                parm caller [8087] value [8087];

