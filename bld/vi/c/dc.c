/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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

#include "clibext.h"


static void initDCLine( dc_line *dcline )
{
    dcline->display = true;
    dcline->valid = false;
    dcline->text = NULL;
    dcline->textlen = 0;
    dcline->ss = SSNewBlock();
}

static void deinitDCLine( dc_line *dcline )
{
    if( dcline->text ) {
        MemFree( dcline->text );
    }
    SSKillBlock( dcline->ss );
}

void DCCreate( void )
{
    int     nlines, i;
    dc_line *dcline;

    assert( CurrentInfo );
    nlines = WindowAuxInfo( CurrentInfo->current_window_id, WIND_INFO_TEXT_LINES );
    CurrentInfo->dclines = NULL;
    if( nlines > 0 ) {
        dcline = MemAlloc( nlines * sizeof( dc_line ) );
        CurrentInfo->dclines = dcline;
        for( i = 0; i < nlines; i++ ) {
            initDCLine( dcline );
            dcline++;
        }
    }
    CurrentInfo->dc_size = nlines;
}

void DCResize( info *info )
{
    int     nlines, extra;
    dc_line *dcline;

    if( info == NULL ) {
        // Windows is sending WM_SIZE before we've set CurrentInfo -
        // hang on & cache will be initialized in DCCreate in a moment
        return;
    }
    nlines = WindowAuxInfo( info->current_window_id, WIND_INFO_TEXT_LINES );
    dcline = info->dclines;
    dcline += info->dc_size - 1;
    for( extra = nlines - info->dc_size; extra < 0; ++extra ) {
        deinitDCLine( dcline );
        dcline--;
    }
    if( nlines == 0 ) {
        // no room to display anything - trash the cache
        if( info->dclines != NULL ) {
            MemFree( info->dclines );
        }
        info->dclines = NULL;
    } else {
        info->dclines = dcline = MemReAlloc( info->dclines, nlines * sizeof( dc_line ) );
        dcline += info->dc_size;
        for( ; extra > 0; --extra ) {
            initDCLine( dcline );
            dcline++;
        }
    }
    info->dc_size = nlines;
}

void DCScroll( int nlines )
{
    dc_line     *dcline;
    dc_line     *dcline_i;
    dc_line     *dcline_temp;
    int         bit;
    int         rest;
    int         i;
    int         a;

    assert( CurrentInfo );
    if( CurrentInfo->dc_size == 0 ) {
        return;
    }
    dcline = CurrentInfo->dclines;

    // 'wrap' pointers so don't need to free/allocate ss blocks, etc.
    dcline_temp = MemAlloc( CurrentInfo->dc_size * sizeof( dc_line ) );
    bit = abs( nlines ) * sizeof( dc_line );
    rest = (CurrentInfo->dc_size - abs( nlines )) * sizeof( dc_line );
    if( nlines > 0 ) {
        memmove( dcline_temp, dcline + CurrentInfo->dc_size - nlines, bit );
        memmove( dcline + nlines, dcline, rest );
        memmove( dcline, dcline_temp, bit );
        dcline_i = dcline;
    } else {
        memmove( dcline_temp, dcline, bit );
        memmove( dcline, dcline - nlines, rest );
        memmove( dcline + CurrentInfo->dc_size + nlines, dcline_temp, bit );
        dcline_i = dcline + CurrentInfo->dc_size + nlines;
    }
    MemFree( dcline_temp );

    a = abs( nlines );
    for( i = 0; i < a; i++ ) {
        dcline_i->valid = false;
        dcline_i++;
    }
}

void DCDestroy( void )
{
    int         i;
    dc_line     *dcline;
    int         nlines;

    assert( CurrentInfo );
    dcline = CurrentInfo->dclines;
    if( dcline == NULL ) {
        return;
    }
    nlines = CurrentInfo->dc_size;
    for( i = 0; i < nlines; i++ ) {
        deinitDCLine( dcline );
        dcline++;
    }
    MemFree( CurrentInfo->dclines );
    CurrentInfo->dclines = NULL;
    CurrentInfo->dc_size = 0;
}

vi_rc DCUpdate( void )
{
    vi_rc           rc;
    int             i, nlines;
    fcb             *fcb;
    line            *line;
    dc_line         *dcline;
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
    MyHideCaret( current_window_id );
    hdc_wnd = GetDC( current_window_id );
#ifdef BITBLT_BUFFER_DISPLAY
    hdc_mem = CreateCompatibleDC( hdc_wnd );
    ws = &(SEType[SE_WHITESPACE]);
    hbitmap = CreateCompatibleBitmap( hdc_wnd,
                    WindowAuxInfo( current_window_id, WIND_INFO_WIDTH ),
                    FontHeight( ws->font ) );
    SelectObject( hdc_mem, hbitmap );
    SelectObject( hdc_mem, ColorBrush( ws->background ) );
#endif
#else
    hasMouse = DisplayMouse( false );
#endif

    rc = CGimmeLinePtr( LeftTopPos.line, &fcb, &line );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }

    nlines = CurrentInfo->dc_size;
    dcline = CurrentInfo->dclines;
    firstLine = true;
    firstTilde = true;
    for( i = 0, line_no = LeftTopPos.line; i < nlines; i++, line_no++ ) {
        if( dcline->display ) {
            if( line ) {
                if( firstLine ) {
                    if( dcline->valid ) {
                         // major speedup
                         SSInitLanguageFlagsGivenValues( &(dcline->flags) );
                    } else {
                        SSInitLanguageFlags( line_no );
                    }
                    firstLine = false;
                }

                displayText = line->data;
                if( line->u.ld.nolinedata ) {
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
                        displayText = EditVars.FileEndString;
                        firstTilde = false;
                    }
                }
                displayOffset = 0;
            }
#ifdef BITBLT_BUFFER_DISPLAY
            DisplayLineInWindowWithSyntaxStyle( current_window_id, i + 1,
                                line, line_no, displayText, displayOffset,
                                hdc_wnd,
                                hdc_mem );
#else
            DisplayLineInWindowWithSyntaxStyle( current_window_id, i + 1,
                                line, line_no, displayText, displayOffset,
                                hdc_wnd );
#endif
            dcline->display = false;
        } else {
            // just in case displaying 2+ blocks in one update
            firstLine = true;
        }
        if( line ) {
            rc = CGimmeNextLinePtr( &fcb, &line );
        }
        if( rc != ERR_NO_ERR && rc != ERR_NO_MORE_LINES ) {
            return( rc );
        }
        dcline++;
    }
#ifdef __WIN__
#ifdef BITBLT_BUFFER_DISPLAY
    DeleteDC( hdc_mem );
    DeleteObject( hbitmap );
#endif
    ReleaseDC( current_window_id, hdc_wnd );
    MyShowCaret( current_window_id );
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

    saved_cinfo = false;
    info = CurrentInfo;
    for( curr = info; curr != NULL; curr = curr->next ) {
        if( curr->CurrentFile->needs_display ) {
            if( !saved_cinfo ) {
                SaveCurrentInfo();
                saved_cinfo = true;
            }
            RestoreInfo( curr );
            DCUpdate();
            curr->CurrentFile->needs_display = false;
        }
    }
    if( saved_cinfo ) {
        RestoreInfo( info );
        SetWindowCursor();
    }
}

static void shaveRange( int *start, int *end )
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
    dc_line     *dcline;
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
    CurrentFile->needs_display = true;
    shaveRange( &start, &end );

    cfcb = CurrentFcb;
    line_no = LeftTopPos.line + start;
    rc = CGimmeLinePtr( line_no, &cfcb, &line );
    if( rc != ERR_NO_ERR && rc != ERR_NO_SUCH_LINE ) {
        return;
    }
    CTurnOffFileDisplayBits();
    cfcb->on_display = true;

    dcline = CurrentInfo->dclines;
    dcline += start;
    for( i = start; i <= end; i++ ) {
        dcline->display = true;
        dcline++;
    }
}

void DCDisplayAllLines( void )
{
    if( CurrentInfo == NULL )
        return;
    DCDisplaySomeLines( 0, CurrentInfo->dc_size - 1 );
}

void DCInvalidateSomeLines( int start, int end )
{
    int         i;
    dc_line     *dcline;

    if( EditFlags.DisplayHold || CurrentFile == NULL ) {
        return;
    }
    assert( CurrentInfo );

    shaveRange( &start, &end );
    if( CurrentInfo->dc_size == 0 ) {
        return;
    }

    dcline = CurrentInfo->dclines;
    dcline += start;
    for( i = start; i <= end; i++ ) {
        dcline->valid = false;
        dcline++;
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

dc_line *DCFindLine( int c_line_no, window_id id )
{
    info        *info;
    dc_line     *dcline;

    for( info = InfoHead; info != NULL; info = info->next ) {
        if( info->current_window_id == id ) {
            break;
        }
    }
    assert( info );
    assert( c_line_no >= 0 && c_line_no < info->dc_size );

    dcline = info->dclines;
    return( dcline + c_line_no );
}

void DCValidateLine( dc_line *dcline, int start_col, char *text )
{
    int     nlen;

    // assumes ss has already been filled correctly
    dcline->start_col = start_col;
    nlen = strlen( text ) + 1;
    if( dcline->text ) {
        if( dcline->textlen < nlen ) {
            // realloc might needlessly memcpy stuff around
            MemFree( dcline->text );
            dcline->text = MemAlloc( nlen );
        }
    } else {
        size_t  len;

        len = nlen;
        if( len < MIN_CACHE_LINE_LENGTH )
            len = MIN_CACHE_LINE_LENGTH;
        dcline->text = MemAlloc( len );
    }
    memcpy( dcline->text, text, nlen );
    dcline->textlen = nlen;
    dcline->valid = true;
}
