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
 *  This code will create multiple status windows for a windows app,
 *  like used in vi and wvideo.  Use as follows :
 *
 *  StatusWndInit -          called once only to set up class.  Supply a
 *                           hook function and the amount of extra data.
 *
 *  StatusWndStart -         called once per status window. Returns a status
 *                           window handle.
 *
 *  StatusWndCreate -        call when you wish to actually create the status
 *                           window, passing a status window handle, parent,
 *                           location, and lpvParam.
 *
 *  StatusWndDrawLine -      call when you wish to draw a new line.  Pass in
 *                           the display context, a font handle, a string
 *                           (which may contain STATUS_ESC_CHAR + STATUS_..
 *                           characters), and a DrawText flags setting.
 *                           If the flags are set to -1, then the string
 *                           will be parsed for escape sequences.
 *
 *  StatusWndSetSeparators - call to set up multiple blocks.  Pass in the
 *                           number of blocks, and an array describing
 *                           each block.
 *
 *  StatusWndDestroy -       call when you are finished with a status window
 *
 *  StatusWndFini -          call when you are finished with all status
 *                           windows. Make sure all status windows have been
 *                           destroyed!
 *
 */

#define STATUS_ESC_CHAR         1
#define STATUS_FORMAT_CENTER    '|'
#define STATUS_FORMAT_RIGHT     '>'
#define STATUS_FORMAT_LEFT      '<'
#define STATUS_NEXT_BLOCK       '['

#ifndef MAX_SECTIONS
#define MAX_SECTIONS    20
#endif

typedef BOOL (*statushook)( HWND, unsigned, UINT, LONG );

typedef struct {
    WORD        separator_width;        // in pixels
    WORD        width;                  // width of block area
    char        width_is_percent:1;
    char        width_is_pixels:1;
    char        spare:6;
} status_block_desc;

typedef struct statwnd {
    HWND                win;
    int                 numSections;
    status_block_desc   sectionDesc[MAX_SECTIONS];
    LPSTR               sectionData[MAX_SECTIONS+1];
    UINT                sectionDataFlags[MAX_SECTIONS+1];
    HFONT               sectionDataFont;
    RECT                statusRect;
} statwnd;

extern  int      StatusWndInit( HINSTANCE hinstance, statushook hook, int extra );
extern  statwnd *StatusWndStart( void );
extern  int      StatusWndCreate( statwnd *, HWND parent, RECT *size,
                                  HINSTANCE hinstance, LPVOID lpvParam );
extern  void     StatusWndDraw3DBox( statwnd *, HDC hdc );
extern  void     StatusWndDrawLine( statwnd *, HDC hdc, HFONT hfont,
                                    char *str, UINT flags );
extern  void     StatusWndSetSeparators( statwnd *, int num_items,
                                         status_block_desc *list );
extern  void     StatusWndDestroy( statwnd * );
extern  void     StatusWndFini( void );

#define HORZ_BORDER     4       /* width of left and right raised area  */
#define VERT_BORDER     2       /* height of top and bottom raised area */
#define BORDER_SIZE     1       /* width/height of frame line           */

/* total height of window used for 3d border */
#define TOTAL_VERT ( 2*(VERT_BORDER + BORDER_SIZE ) )

/* total width of window used for 3d border */
#define TOTAL_HORZ ( 2*(HORZ_BORDER + BORDER_SIZE ) )
