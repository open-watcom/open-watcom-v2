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
* Description:  Definitions for the 3D graphics library.
*
****************************************************************************/


typedef enum three_d_attr {
    THREE_D_HORZ_ANGLE,
    THREE_D_VERT_ANGLE,
    THREE_D_ZOOM,
    THREE_D_PERSPECTIVE,
    THREE_D_BRIGHTNESS,
    THREE_D_CONTRAST,
    THREE_D_LIGHT_HORZ,
    THREE_D_LIGHT_VERT,
    THREE_D_LIGHT_DEPTH
} three_d_attr;

typedef int three_d_handle;

#if defined(__OS2__)
    #define ENTRYPOINT  __export _System
    #define WPI_PRES    HPS
#else
    #define RECTL       RECT
    #define WPI_PRES    HDC
    #if defined(__WINDOWS_386__)
        #define ENTRYPOINT
    #else
        #define ENTRYPOINT      __export FAR PASCAL
    #endif
#endif

void ENTRYPOINT three_d_init();
void ENTRYPOINT three_d_fini();
three_d_handle ENTRYPOINT three_d_begin( WPI_PRES dc, RECTL * draw_area );
void ENTRYPOINT three_d_box( three_d_handle handle,
                  float x1, float x2, float y1, float y2, float z1, float z2,
                  unsigned red, unsigned green, unsigned blue );
void ENTRYPOINT three_d_draw( three_d_handle handle );
void ENTRYPOINT three_d_end( three_d_handle handle );

// Attribute setting & changing

int ENTRYPOINT three_d_get( three_d_handle handle, three_d_attr attr );
void ENTRYPOINT three_d_set( three_d_handle handle, three_d_attr attr, int new_val );
