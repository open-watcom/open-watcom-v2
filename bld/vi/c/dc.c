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


#include "vi.h"
#include "winaux.h"
#include "win.h"
#ifdef __WIN__
    #include "font.h"
    #include "color.h"
#endif
#include <assert.h>

static void initDCLine( dc dc )
{
    dc->display = TRUE;
    dc->valid = FALSE;
    dc->text = NULL;
    dc->textlen = 0;
    dc->ss = SSNewBlock();
}

static void deinitDCLine( dc dc )
{
    if( dc->text ) {
        MemFree( dc->text );
    }
    SSKillBlock( dc->ss );
}

void DCCreate( void )
{
    int     nlines, i;
    dc      dc;

    assert( CurrentInfo );
    nlines = WindowAuxInfo( CurrentInfo->CurrentWindow,
                            WIND_INFO_TEXT_LINES );
    CurrentInfo->dc = NULL;
    if( nlines != 0 ) {
        CurrentInfo->dc = dc = MemAlloc( nlines * sizeof( dc_line ) );
    }
    for( i = 0; i < nlines; i++ ) {
        initDCLine( dc );
        dc++;
    }
    CurrentInfo->dc_size = nlines;
}

void DCResize( info *info )
{
    int     nlines, extra;
    dc      dc;

    if( info == NULL ) {
        // Windows is sending WM_SIZE before we've set CurrentInfo -
        // hang on & cache will be initialized in DCCreate in a moment
        return;
    }
    nlines = WindowAuxInfo( info->CurrentWindow, WIND_INFO_TEXT_LINES );
    extra = nlines - info->dc_size;
    dc = info->dc;
    dc += info->dc_size - 1;
    while( extra < 0 ) {
        deinitDCLine( dc );
        dc--;
        extra++;
    }
    if( nlines == 0 ) {
        // no room to display anything - trash the cache
        if( info->dc ) {
            MemFree( info->dc );
        }
        info->dc = NULL;
    } else {
        info->dc = dc = MemReAlloc( info->dc, nlines * sizeof( dc_line ) );
        dc += info->dc_size;
        while( extra > 0 ) {
            initDCLine( dc );
            dc++;
            extra--;
        }
    }
    info->dc_size = nlines;
}

void DCScroll( int nlines )
{
    dc      dc, dc_i, dc_temp;
    int     bit, rest;
    int     i, a;

    assert( CurrentInfo );
    if( CurrentInfo->dc_size == 0 ) {
        return;
    }
    dc = CurrentInfo->dc;

    // 'wrap' pointers so don't need to free/allocate ss blocks, etc.
    dc_temp = MemAlloc( CurrentInfo->dc_size * sizeof( dc_line ) );
    bit = abs( nlines ) * sizeof( dc_line );
    rest = (CurrentInfo->dc_size - abs( nlines )) * sizeof( dc_line );
    if( nlines > 0 ) {
        memmove( dc_temp, dc + CurrentInfo->dc_size - nlines, bit );
        memmove( dc + nlines, dc, rest );
        memmove( dc, dc_temp, bit );
        dc_i = dc;
    } else {
        memmove( dc_temp, dc, bit );
        memmove( dc, dc - nlines, rest );
        memmove( dc + CurrentInfo->dc_size + nlines, dc_temp, bit );
        dc_i = dc + CurrentInfo->dc_size + nlines;
    }
    MemFree( dc_temp );

    a = abs( nlines );
    for( i = 0; i < a; i++ ) {
        dc_i->valid = FALSE;
        dc_i++;
    }
}

void DCDestroy( void )
{
    int     i;
    dc      dc;
    int     nlines;

    assert( CurrentInfo );
    dc = CurrentInfo->dc;
    if( !dc ) {
        return;
    }
    nlines = CurrentInfo->dc_size;
    for( i = 0; i < nlines; i++ ) {
        deinitDCLine( dc );
        dc++;
    }
    MemFree( CurrentInfo->dc );
    CurrentInfo->dc = 0;
    CurrentInfo->dc_size = 0;
}

vi_rc DCUpdate( void )
{
    vi_rc           rc;
    int             i, nlines;
    fcb             *fcb;
    line            *line;
    dc              dc;
    bool            firstLine, firstTilde;
    linenum         line_no;
    int             displayOffset;
    char            *displayText;
#ifdef __WIN__
    HDC             hdc_wnd;
#ifdef BITBLT_BUFFER_DISPLAY
    HDC             hdc_mem;
    HBITMAP         hbitmap;
    type_style      *ws;
#endif
#else
    bool            hasMouse;
    unsigned int    hdc_wnd = 0;
#endif

    if( EditFlags.Quiet || CurrentInfo == NULL ) {
        return( ERR_NO_ERR );
    }

#ifdef __WIN__
    MyHideCaret( CurrentWindow );
    hdc_wnd = GetDC( CurrentWindow );
#ifdef BITBLT_BUFFER_DISPLAY
    hdc_mem = CreateCompatibleDC( hdc_wnd );
    ws = &(SEType[SE_WHITESPACE]);
    hbitmap = CreateCompatibleBitmap( hdc_wnd,
                    WindowAuxInfo( CurrentWindow, WIND_INFO_WIDTH ),
                    FontHeight( ws->font ) );
    SelectObject( hdc_mem, hbitmap );
    SelectObject( hdc_mem, ColorBrush( ws->background ) );
#endif
#else
    hasMouse = DisplayMouse( FALSE );
#endif

    rc = CGimmeLinePtr( LeftTopPos.line, &fcb, &line );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    nlines = CurrentInfo->dc_size;
    dc = CurrentInfo->dc;
    firstLine = TRUE;
    firstTilde = TRUE;
    for( i = 0, line_no = LeftTopPos.line; i < nlines; i++, line_no++ ) {
        if( dc->display ) {
            if( line ) {
                if( firstLine ) {
                    if( dc->valid ) {
                         // major speedup
                         SSInitLanguageFlagsGivenValues( &(dc->flags) );
                    } else {
                        SSInitLanguageFlags( line_no );
                    }
                    firstLine = FALSE;
                }

                displayText = line->data;
                if( line->inf.ld.nolinedata ) {
                    if( WorkLine->len >= 0 ) {
                        displayText = WorkLine->data;
                    } else {
                        displayText = "*** ERR NULL DATA ***";
                    }
                }
                displayOffset = VirtualLineLen( displayText );
                if( displayOffset > LeftTopPos.column ) {
                    displayOffset = LeftTopPos.column;
                }
            } else {
                if( EditFlags.DrawTildes ) {
                    displayText = "~";
                } else {
                    displayText = "";
                    if( firstTilde ) {
                        displayText = FileEndString;
                        firstTilde = FALSE;
                    }
                }
                displayOffset = 0;
            }
#ifdef BITBLT_BUFFER_DISPLAY
            DisplayLineInWindowWithSyntaxStyle( CurrentWindow, i + 1,
                                line, line_no, displayText, displayOffset,
                                hdc_wnd,
                                hdc_mem );
#else
            DisplayLineInWindowWithSyntaxStyle( CurrentWindow, i + 1,
                                line, line_no, displayText, displayOffset,
                                hdc_wnd );
#endif
            dc->display = FALSE;
        } else {
            // just in case displaying 2+ blocks in one update
            firstLine = TRUE;
        }
        if( line ) {
            rc = CGimmeNextLinePtr( &fcb, &line );
        }
        if( rc != ERR_NO_ERR && rc != ERR_NO_MORE_LINES ) {
            return( rc );
        }
        dc++;
    }
#ifdef __WIN__
#ifdef BITBLT_BUFFER_DISPLAY
    DeleteDC( hdc_mem );
    DeleteObject( hbitmap );
#endif
    ReleaseDC( CurrentWindow, hdc_wnd );
    MyShowCaret( CurrentWindow );
#else
    DisplayMouse( hasMouse );
#endif
    return( ERR_NO_ERR );
}

void DCUpdateAll( void )
{
    info        *curr, *info;
    bool        saved_cinfo;

    if( EditFlags.Quiet ) {
        return;
    }

    saved_cinfo = FALSE;
    info = CurrentInfo;
    for( curr = info; curr != NULL; curr = curr->next ) {
        if( curr->CurrentFile->needs_display ) {
            if( !saved_cinfo ) {
                SaveCurrentInfo();
                saved_cinfo = TRUE;
            }
            RestoreInfo( curr );
            DCUpdate();
            curr->CurrentFile->needs_display = FALSE;
        }
    }
    if( saved_cinfo ) {
        RestoreInfo( info );
        SetWindowCursor();
    }
}

void shaveRange( int *start, int *end )
{
    int     tmp;

    if( *start > *end ) {
        tmp = *start;
        *start = *end;
        *end = tmp;
    }
    if( *start < 0 ) {
        *start = 0;
    }
    if( *end < 0 ) {
        *end = 0;
    }
    if( *end >= CurrentInfo->dc_size ) {
        *end = CurrentInfo->dc_size - 1;
    }
    if( *start >= CurrentInfo->dc_size ) {
        *start = CurrentInfo->dc_size - 1;
    }
}

void DCDisplaySomeLines( int start, int end )
{
    int         i;
    dc          dc;
    linenum     line_no;
    line        *line;
    fcb         *cfcb;
    vi_rc       rc;

    if( EditFlags.DisplayHold || CurrentFile == NULL || CurrentInfo == NULL ) {
        return;
    }

    assert( CurrentInfo );

    if( CurrentInfo->dc_size == 0 ) {
        return;
    }
    CurrentFile->needs_display = TRUE;
    shaveRange( &start, &end );

    cfcb = CurrentFcb;
    line_no = LeftTopPos.line + start;
    rc = CGimmeLinePtr( line_no, &cfcb, &line );
    if( rc != ERR_NO_ERR && rc != ERR_NO_SUCH_LINE ) {
        return;
    }
    CTurnOffFileDisplayBits();
    cfcb->on_display = TRUE;

    dc = CurrentInfo->dc;
    dc += start;
    for( i = start; i <= end; i++ ) {
        dc->display = TRUE;
        dc++;
    }
}

void DCDisplayAllLines( void )
{
    if( CurrentInfo == NULL ) return;
    DCDisplaySomeLines( 0, CurrentInfo->dc_size - 1 );
}

void DCInvalidateSomeLines( int start, int end )
{
    int         i;
    dc          dc;

    if( EditFlags.DisplayHold || CurrentFile == NULL ) {
        return;
    }
    assert( CurrentInfo );

    shaveRange( &start, &end );
    if( CurrentInfo->dc_size == 0 ) {
        return;
    }

    dc = CurrentInfo->dc;
    dc += start;
    for( i = start; i <= end; i++ ) {
        dc->valid = FALSE;
        dc++;
    }
}

void DCInvalidateAllLines( void )
{
    if( EditFlags.DisplayHold || CurrentFile == NULL ) {
        return;
    }
    assert( CurrentInfo );
    DCInvalidateSomeLines( 0, CurrentInfo->dc_size - 1 );
}

dc DCFindLine( int c_line_no, window_id id )
{
    info    *info;
    dc      dc;

    for( info = InfoHead; info != NULL; info = info->next ) {
        if( info->CurrentWindow == id ) {
            break;
        }
    }
    assert( info );
    assert( c_line_no >= 0 && c_line_no < info->dc_size );

    dc = info->dc;
    return( dc + c_line_no );
}

void DCValidateLine( dc dc_line, int start_col, char *text )
{
    int     nlen;

    // assumes ss has already been filled correctly
    dc_line->start_col = start_col;
    nlen = strlen( text ) + 1;
    if( dc_line->text ) {
        if( dc_line->textlen < nlen ) {
            // realloc might needlessly memcpy stuff around
            MemFree( dc_line->text );
            dc_line->text = MemAlloc( nlen );
        }
    } else {
        dc_line->text = MemAlloc( max( MIN_CACHE_LINE_LENGTH, nlen ) );
    }
    memcpy( dc_line->text, text, nlen );
    dc_line->textlen = nlen;
    dc_line->valid = TRUE;
}
