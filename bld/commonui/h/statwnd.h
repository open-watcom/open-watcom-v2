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
* Description:  Status window interface.
*
****************************************************************************/


#ifndef _STATWND_H_INCLUDED
#define _STATWND_H_INCLUDED

#include "wpi.h"

/*
 *  This code will create a status window for a Windows or OS/2 application,
 *  as used in vi and wv.  Use as follows:
 *
 *  StatusWndInit -          Called once only to set up class.  Supply a
 *                           hook function and the amount of extra data.
 *
 *  StatusWndStart -         Called once per status window.  Returns a status
 *                           window handle.
 *
 *  StatusWndChangeSysColors - Changes what statwnd believes are the system
 *                             colours (does NOT change the real system
 *                             colours).
 *
 *  StatusWndCreate -        Call when you wish to actually create the status
 *                           window, passing parent, location and lpvParam.
 *
 *  StatusWndDrawLine -      Call when you wish to draw a new line.  Pass in
 *                           the display context, a font handle, a string
 *                           (which may contain STATUS_ESC_CHAR + STATUS_..
 *                           characters), and a DrawText flags setting.
 *                           If the flags are set to -1, then the string
 *                           will be parsed for escape sequences.
 *
 *  StatusWndSetSeparators - Call to set up multiple blocks.  Pass in the
 *                           number of blocks, and an array describing
 *                           each block.
 *
 *  StatusWndDestroy -       Call when you are finished with the status window.
 *
 *  StatusWndFini -          Call when you are finished with all status
 *                           windows.  Make sure all status windows have been
 *                           destroyed!
 *
 */

#define STATUS_ESC_CHAR         1
#define STATUS_FORMAT_CENTER    '|'
#define STATUS_FORMAT_RIGHT     '>'
#define STATUS_FORMAT_LEFT      '<'
#define STATUS_NEXT_BLOCK       '['

typedef BOOL (*statushook)( HWND, WPI_MSG, WPI_PARAM1, WPI_PARAM2 );

typedef struct {
    WORD        separator_width;            /* in pixels */
    WORD        width;                      /* width of block area */
    char        width_is_percent    : 1;
    char        width_is_pixels     : 1;
    char        spare               : 6;
} status_block_desc;

typedef struct statwnd statwnd;

int     StatusWndInit( WPI_INST hinstance, statushook hook, int extra, HCURSOR );
statwnd *StatusWndStart( void );
void    StatusWndChangeSysColors( COLORREF btnFace, COLORREF btnText, COLORREF btnHighlight, COLORREF btnShadow );
HWND    StatusWndCreate( statwnd *sw, HWND parent, WPI_RECT *size, WPI_INST hinstance, LPVOID lpvParam );
void    StatusWndDraw3DBox( statwnd *sw, WPI_PRES pres );
void    StatusWndDrawLine( statwnd *sw, WPI_PRES pres, WPI_FONT hfont, const char *str, UINT flags );
void    StatusWndSetSeparators( statwnd *sw, int num_items, status_block_desc *list );
int     StatusWndGetHeight( statwnd *sw );
void    StatusWndDestroy( statwnd *sw );
void    StatusWndFini( void );

#define HORZ_BORDER     4       /* width of left and right raised area  */
#define VERT_BORDER     2       /* height of top and bottom raised area */
#define BORDER_SIZE     1       /* width/height of frame line           */

/* Total height of window used for 3D border */
#define TOTAL_VERT (2 * (VERT_BORDER + BORDER_SIZE))

/* Total width of window used for 3D border */
#define TOTAL_HORZ (2 * (HORZ_BORDER + BORDER_SIZE))

/* Maximum number of sections on status bar */
#ifndef MAX_SECTIONS
    #define MAX_SECTIONS    20
#endif

#endif /* _STATWND_H_INCLUDED */
