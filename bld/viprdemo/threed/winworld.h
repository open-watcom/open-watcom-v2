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


#ifndef CGR_WINWORLD_H
#define CGR_WINWORLD_H
/*
 Description:
 ============
    This file defines the constants for the WINDOWS world window
    library defined in window.c

*/

enum {
    TEXT_H_LEFT = 1,    // to keep this in sink with text_align in cgrchart.h
    TEXT_H_CENTER,
    TEXT_H_RIGHT
};

enum {
     TEXT_V_TOP = 1,
     TEXT_V_CENTER,
     TEXT_V_BOTTOM
};

enum {
    FILL_BORDER,                // border only: interior not touched (pen)
    FILL_INTERIOR,              // interior only: border not touched (brush)
    FILL_BORDER_CLEAR,          // border WITH interior erased to bkgd (pen)
    FILL_BORDER_FILL            // border and interior (pen & brush)
};

typedef struct cgr_window {
    float       xl;             // left x coord
    float       xr;             // right x coord
    float       yb;             // bottom y coord
    float       yt;             // top y coord
} cgr_window;

#endif
