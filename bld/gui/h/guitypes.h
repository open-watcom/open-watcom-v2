/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "guixtype.h"

#define GUIEVENT( wnd, event, param )   ((*wnd->gui_call_back)(wnd, event, param))

#define GUIMulDiv(a,b,c,d) ((a)(((long)b * (long)c) / (long)d))

#define COPYRECT( old, new ) { \
    (new).x = (old).x; (new).y = (old).y; \
    (new).width = (old).width; (new).height = (old).height; }

#define COPYRECTX( old, new ) { \
    (new).s_x = (old).s_x; (new).s_y = (old).s_y; \
    (new).s_width = (old).s_width; (new).s_height = (old).s_height; }

typedef struct guix_coord {
    guix_ord            x;
    guix_ord            y;
} guix_coord;

typedef struct guix_point {
    guix_ord            x;
    guix_ord            y;
} guix_point;
