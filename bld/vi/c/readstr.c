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


#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include "vi.h"
#include "keys.h"
#include "win.h"
#include "source.h"
#include "mouse.h"
#ifdef __WIN__
#include "winvi.h"
#include "utils.h"
#endif

#define HANDLED     0

typedef struct input_win_info {
    window_id       id;
    type_style      style;
    int             width;
    int             line;       /* we don't support multi-line input yet */
} input_win_info;

typedef struct input_buffer {
    char            *prompt;
    char            *buffer;
#ifdef __WIN__
    char            *cache;
#endif
    char            *last_str;
    int             buffer_length;
    history_data    *history;
    int             curr_hist;
    input_win_info  window;
    type_style      style;
    int             curr_pos;
    int             left_column;
    int             line;
    unsigned        overstrike:1;
} input_buffer;

/*
 * This is half of a kluge connected with the window proc for the
 * command window under the __WIN__ environs. Puke...
 */
bool ReadingAString = FALSE;

/*
 * General utility funtions:
 *      insertChar( input_buffer *, char ) - insert the given character at the
 *          current position paying attention to overstrike mode. This
 *          function will update curr_pos.
 *      deleteChar( input_buffer *, backwards ) - deletes the character at the
 *          current position if possible and updates curr_pos.
 *      displayLine( input_buffer * ) - displays the line in the window and
 *          sets the cursor to curr_pos.
 *      saveStr( input_buffer *, str ) - saves the string in the input buffer
 *          to the location given. Enough memory to hold string is assumed.
 */

static bool insertChar( input_buffer *input, int ch )
{
    char            *ptr;
    int             len;

    if( input->curr_pos >= input->buffer_length - 1 ) {
        return( FALSE );
    }

    /* if we are in overstrike more or at the end of the line... */
    if( input->overstrike ) {
        len = strlen( input->buffer ) + 1;
        input->buffer[ input->curr_pos++ ] = ch;
        if( input->curr_pos == len ) {
            input->buffer[ input->curr_pos ] = 0;
        }
    } else {
        if( strlen( input->buffer ) >= input->buffer_length - 1 ) {
            return( FALSE );
        }
        ptr = &input->buffer[ input->curr_pos++ ];
        memmove( ptr + 1, ptr, strlen( ptr ) + 1 );
        *ptr = ch;
    }
    if( input->curr_pos >= input->left_column + input->window.width - 1 ) {
        input->left_column += 1;
    }
    return( TRUE );

} /* insertChar */

static bool deleteChar( input_buffer *input, bool backwards )
{
    char            *ptr;

    if( backwards ) {
        if( input->curr_pos == 0 ) {
            return( FALSE );
        }
        input->curr_pos -= 1;
        if( input->curr_pos < input->left_column ) {
            input->left_column -= 1;
        }
    }
    ptr = &input->buffer[ input->curr_pos ];
    memmove( ptr, ptr + 1, strlen( ptr + 1 ) + 1 );
    return( TRUE );

} /* deleteChar */

static void displayLine( input_buffer *input )
{
    char            display[ MAX_STR ];
    char            *buffer, *dest;
    unsigned        length;
    int             cursor_pos;

    if( EditFlags.NoInputWindow ) {
        return;
    }
    assert( strlen( input->prompt ) < MAX_STR );
    strcpy( display, input->prompt );
    length = strlen( display );
    dest = &display[ length ];
    buffer = input->buffer + input->left_column;
    while( *buffer && length < input->window.width ) {
        *dest++ = *buffer++;
        length += 1;
    }
    *dest = 0;
    cursor_pos = input->curr_pos - input->left_column + 1;
    cursor_pos += strlen( input->prompt );
#ifdef __WIN__
{
    RECT        rect;
    char        *ptr, *c;
    int         len, x;
    HWND        id;

    id = (HWND) input->window.id;
    MyHideCaret( id );
    GetClientRect( id, &rect );
    // BlankRectIndirect( input->window.id, input->window.style.background, &rect );
    c = input->cache;
    for( len = 0, ptr = input->buffer; *ptr; ptr++, len++ ) {
        if( *c != *ptr ) {
            break;
        }
        c++;
    }
    x = MyTextExtent( id, &input->window.style, input->cache, len );
    WriteString( id, x, 0, &input->window.style, display + len );
    rect.left = MyTextExtent( id, &input->window.style, display, strlen( display ) );
    BlankRectIndirect( id, input->window.style.background, &rect );
    MyShowCaret( id );
    SetCursorOnLine( input->window.id, cursor_pos, display, &input->window.style );
}
#else
    DisplayLineInWindowWithColor( input->window.id, input->window.line,
        display, &input->window.style, 0 );
    SetGenericWindowCursor( input->window.id, input->window.line, cursor_pos );
#endif

} /* displayLine */

static bool endColumn( input_buffer *input )
{
    int         width, left;
    int         column;

    column = strlen( input->buffer );
    width = input->window.width - strlen( input->prompt );
    left = input->left_column;
    if( column >= left + width || column < left ) {
        left = column - width + 1;
        if( left < 0 ) {
            left = 0;
        }
    }
    input->curr_pos = column;
    input->left_column = left;
    return( TRUE );

} /* endColumn */

static bool saveStr( input_buffer *input )
{
    strcpy( input->last_str, input->buffer );
    return( TRUE );

} /* saveStr */

static bool swapString( input_buffer *input )
{
    char        *tmp;

    tmp = alloca( strlen( input->last_str ) + 1 );
    strcpy( tmp, input->last_str );
    strcpy( input->last_str, input->buffer );
    strcpy( input->buffer, tmp );
    endColumn( input );
    return( TRUE );

} /* swapString */

static int cursorKeyFilter( input_buffer *input, int event )
{
    int         max_pos;

    max_pos = strlen( input->buffer );
    switch( event ) {
    case HANDLED:
        break;
    case VI_KEY( HOME ):
        input->curr_pos = 0;
        input->left_column = 0;
        break;
    case VI_KEY( END ):
        endColumn( input );
        break;
    case VI_KEY( RIGHT ):
        if( input->curr_pos < max_pos ) {
            input->curr_pos += 1;
            if( input->curr_pos > input->left_column + input->window.width ) {
                input->left_column += 1;
            }
        } else {
            MyBeep();
        }
        break;
    case VI_KEY( LEFT ):
        if( input->curr_pos > 0 ) {
            input->curr_pos -= 1;
            if( input->left_column > input->curr_pos ) {
                input->left_column = input->curr_pos;
            }
        }
        break;
    case VI_KEY( DEL ):
    case VI_KEY( BS ):
        saveStr( input );
        if( !deleteChar( input, event == VI_KEY( BS )
            || input->curr_pos == max_pos ) ) {
            MyBeep();
        }
        break;
    default:
        return( event );
    }
    return( HANDLED );

} /* cursorKeyFilter */

/*
 * History functions:
 *      getHistory( input_buffer * ) - replace the input buffer with the
 *          currently selected history line (input->curr_hist).
 *      addHistory( input_buffer * ) - adds the current buffer to the
 *          history list associated with the input_buffer.
 *      searchHistory( input_buffer *, char *, int ) - starts searching
 *          at the specified index and returns the index of the first
 *          matching line in the history buffer or -1 if there are none.
 *      historyFilter( input_buffer *, int ) - pick out and take care of
 *          events which deal with the command line history.
 */

static bool getHistory( input_buffer *input )
{
    int             offset;
    char            *cmd;

    offset = input->curr_hist % input->history->max;
    cmd = input->history->data[ offset ];
    if( cmd != NULL ) {
        saveStr( input );
        strcpy( input->buffer, cmd );
        endColumn( input );
        return( TRUE );
    }
    return( FALSE );

} /* getHistory */

static bool addHistory( input_buffer *input )
{
    history_data    *h;

    h = input->history;
    if( h != NULL && input->buffer[ 0 ] != 0 ) {
        AddString2( &(h->data[ h->curr % h->max ] ), input->buffer );
        h->curr += 1;
        return( TRUE );
    }
    return( FALSE );

} /* addHistory */

static int searchHistory( input_buffer *input, char *str, int curr )
{
    int             index, i, len;
    history_data    *h;

    h = input->history;
    len = strlen( str );
    for( i = 0; i < h->max; i++ ) {
        curr -= 1;
        if( curr < 0 || curr < h->curr - h->max ) {
            curr = h->curr - 1;
        }
        index = curr % h->max;
        if( !strnicmp( str, h->data[ index ], len ) ) {
            strcpy( input->buffer, h->data[ index ] );
            endColumn( input );
            return( curr );
        }
    }
    MyBeep();
    return( -1 );

} /* searchHistory */

static void doHistorySearch( input_buffer *input )
{
    int             curr;
    char            *str;
    int             event;

    curr = input->history->curr;
    str = alloca( strlen( input->buffer ) + 1 );
    strcpy( str, input->buffer );
    event = VI_KEY( CTRL_TAB );
    while( curr != -1 ) {
        if( event == VI_KEY( ALT_TAB ) || event == VI_KEY( CTRL_TAB ) ) {
            saveStr( input );
            curr = searchHistory( input, str, curr );
            displayLine( input );
            event = GetNextEvent( TRUE );
            continue;
        }
        KeyAdd( event );
        return;
    }

} /* doHistorySearch */

static int historyFilter( input_buffer *input, int event )
{
    history_data    *h;

    if( input->history == NULL || input->history->curr == 0 ) {
        return( event );
    }
    h = input->history;
    switch( event ) {
    case VI_KEY( UP ):
        input->curr_hist -= 1;
        if( input->curr_hist < 0 || input->curr_hist < h->curr - h->max ) {
            input->curr_hist = h->curr - 1;
        }
        getHistory( input );
        break;
    case VI_KEY( DOWN ):
        input->curr_hist += 1;
        if( input->curr_hist >= h->curr ) {
            input->curr_hist = h->curr - h->max;
            if( input->curr_hist < 0 ) {
                input->curr_hist = 0;
            }
        }
        getHistory( input );
        break;
    case VI_KEY( CTRL_TAB ):
    case VI_KEY( ALT_TAB ):
        doHistorySearch( input );
        break;
    default:
        return( event );
    }
    return( HANDLED );

} /* historyFilter */

/*
 * Special key (ALT_L etc... ) functions:
 *      insertString( input_buffer *, char * ) - insert the given string at
 *          the current location and bump the current position.
 *      specialKeyFilter( input_buffer *, int ) - pick out and handle any
 *          events which map to special keys.
 */

static bool insertString( input_buffer *input, char *str )
{
    while( *str ) {
        if( !insertChar( input, *str++ ) ) {
            return( FALSE );
        }
    }
    return( TRUE );

} /* insertString */

/*
 * GetTextForSpecialKey - get text for ^D,^E,^W, ALT_L, ^L, ^R
 */
bool GetTextForSpecialKey( int str_max, int event, char *tmp )
{
    int         i, l;
    #ifndef __WIN__
    int         str_len;
    #endif

    switch( event ) {
    #ifndef __WIN__
    /* these commands are no longer safe under windows, since events
     * don't fit into a char any more
     */
    case VI_KEY( CTRL_F ):
        str_len = AltDotDigits;
        if( str_len > str_max ) {
            str_len = str_max;
        }
        for( i=0; i < str_len; i++ ) {
            tmp[i] = (char)(AltDotBuffer[i]);
        }
        tmp[ str_len ] = 0;
        break;
    case VI_KEY( CTRL_D ):
        str_len = DotCmdCount;
        if( str_len > str_max ) {
            str_len = str_max;
        }
        for( i=0; i < str_len; i++ ) {
            tmp[i] = (char)(DotCmd[i]);
        }
        tmp[ str_len ] = 0;
        break;
    #endif
    case VI_KEY( CTRL_E ):
    case VI_KEY( CTRL_W ):
        tmp[ 0 ] = 0;
        GimmeCurrentWord( tmp, str_max, event == VI_KEY( CTRL_E ) );
        tmp[ str_max ] = 0;
        break;
    case VI_KEY( ALT_L ):
        i = CurrentColumn - 1;
        i = ( i > 0 ) ? i : 0;
    case VI_KEY( CTRL_L ):
        if( CurrentLine == NULL ) {
            break;
        }
        if( event == VI_KEY( CTRL_L ) ) {
            i = 0;
        }
        ExpandTabsInABuffer( &CurrentLine->data[ i ], CurrentLine->len - i,
                tmp, str_max );
        break;
    case VI_KEY( CTRL_R ):
        if( CurrentLine == NULL ) {
            break;
        }
        if( SelRgn.lines ) {
            assert( SelRgn.start_line == SelRgn.end_line );
            i = 1;
            l = CurrentLine->len + 1;
        } else {
            if( SelRgn.start_col < SelRgn.end_col ) {
                i = SelRgn.start_col;
                l = SelRgn.end_col - SelRgn.start_col + 1;
            } else {
                i = SelRgn.end_col;
                l = SelRgn.start_col - SelRgn.end_col + 1;
            }
        }
        ExpandTabsInABuffer( &CurrentLine->data[ i - 1 ], l, tmp, str_max );
        tmp[ l ] = 0;
    default:
        return( FALSE );
    }
    return( TRUE );

} /* GetTextForSpecialKey */

/*
 * InsertTextForSpecialKey - insert text for ^O, ALT_O
 */
void InsertTextForSpecialKey( int event, char *buff )
{
    linenum     line;
    int         type;

    if( CurrentFile == NULL ) {
        return;
    }

    line = CurrentLineNumber;
    type = INSERT_BEFORE;
    if( event == VI_KEY( CTRL_O ) ) {
        type = INSERT_AFTER;
        line += 1;
    }
    Modified( TRUE );
    StartUndoGroup( UndoStack );
    UndoInsert( line, line, UndoStack );
    AddNewLineAroundCurrent( buff, strlen( buff ), type );
    EndUndoGroup( UndoStack );
    DCDisplayAllLines();
    DCUpdate();

} /* InsertTextForSpecialKey */

static int specialKeyFilter( input_buffer *input, int event )
{
    char        *tmp;

    switch( event ) {
    case VI_KEY( ALT_O ):
    case VI_KEY( CTRL_O ):
        InsertTextForSpecialKey( event, input->buffer );
        break;
    case VI_KEY( CTRL_R ):
        if( !SelRgn.selected ||
            ( SelRgn.lines &&
            ( SelRgn.start_line != SelRgn.end_line ) ) ) {
            MyBeep();
            break;
        }
    case VI_KEY( CTRL_W ):
    case VI_KEY( CTRL_E ):
    case VI_KEY( ALT_L ):
    case VI_KEY( CTRL_L ):
    case VI_KEY( CTRL_D ):
    case VI_KEY( CTRL_F ):
        if( input->curr_pos != strlen( input->buffer ) ) {
            MyBeep();
        } else {
            tmp = MemAlloc( input->buffer_length );
            assert( tmp != NULL );
            GetTextForSpecialKey( input->buffer_length - strlen( input->buffer ) - 1,
                                        event, tmp );
            saveStr( input );
            insertString( input, tmp );
            MemFree( tmp );
        }
        break;
    default:
        return( event );
        break;
    }
    return( HANDLED );

} /* specialKeyFilter */

/*
 * File Completion:
 *      fileComplete( input_buffer ) - performs the actual file complete
 *          by spinning in a loop getting characters until the user
 *          selects a file or cancels. Returns TRUE if the getString
 *          routine should exit with the current string.
 */

static bool fileComplete( input_buffer *input, int first_event )
{
    bool        exit, done;
    int         ret, event;
    int         old_len;

    exit = FALSE;
    if( input->curr_pos != strlen( input->buffer ) ) {
        MyBeep();
    } else {
        saveStr( input );
        old_len = strlen( input->buffer ) - 1;
        ret = StartFileComplete( input->buffer, old_len,
                input->buffer_length, first_event );
        if( ret > 0 ) {
            MyBeep();
        } else {
            if( ret != FILE_COMPLETE ) {
                done = FALSE;
                do {
                    endColumn( input );
                    displayLine( input );
                    event = GetNextEvent( TRUE );
                    switch( event ) {
                    case VI_KEY( FAKEMOUSE ):
                    case VI_KEY( MOUSEEVENT ):
                    case VI_KEY( TAB ):
                    case VI_KEY( SHIFT_TAB ):
                    case VI_KEY( UP ):
                    case VI_KEY( DOWN ):
                    case VI_KEY( LEFT ):
                    case VI_KEY( RIGHT ):
                    case VI_KEY( PAGEDOWN ):
                    case VI_KEY( PAGEUP ):
                    case VI_KEY( ALT_END ):
                        ret = ContinueFileComplete( input->buffer,
                            old_len, input->buffer_length, event );
                        if( ret ) {
                            FinishFileComplete();
                            if( ret == FILE_COMPLETE_ENTER ) {
                                exit = TRUE;
                            }
                            done = TRUE;
                        }
                        old_len = strlen( input->buffer );
                        break;
                    default:
                        KeyAdd( event );
                        PauseFileComplete();
                        done = TRUE;
                    }
                } while( !done );
            }
        }
    }
    return( exit );

} /* fileComplete */

/*
 * Setup and Initialization functions:
 *      mouseHandler() - the routine to handle mouse events.
 *      initInput( input_buffer ) - sets the buffer to empty and
 *          initializes variables.
 *      finiInput( input_buffer ) - restores the normal cursor and
 *          removes the mouse handler.
 */

static window_id    thisWindow = -1;

static bool mouseHandler( window_id id, int x, int y )
{
    x = x;
    y = y;
    if( id != thisWindow ) {
        if( LastMouseEvent == MOUSE_PRESS ) {
            KeyAdd( VI_KEY( ESC ) );
            AddCurrentMouseEvent();
        }
    }
    return( FALSE );

} /* mouseHandler */

static void initInput( input_buffer *input )
{
    type_style      *s;
    window_id       id;

    memset( input->buffer, 0, input->buffer_length );
    input->curr_pos = 0;
    if( input->history != NULL ) {
        input->curr_hist = input->history->curr;
    }
    input->left_column = 0;
    input->overstrike = TRUE;
    s = &input->window.style;
    id = input->window.id;
    thisWindow = id;
    s->foreground = WindowAuxInfo( id, WIND_INFO_TEXT_COLOR );
    s->background = WindowAuxInfo( id, WIND_INFO_BACKGROUND_COLOR );
    s->font = WindowAuxInfo( id, WIND_INFO_TEXT_FONT );
    input->window.width = WindowAuxInfo( id, WIND_INFO_TEXT_COLS );
    PushMouseEventHandler( mouseHandler );
    NewCursor( input->window.id, NormalCursorType );
    displayLine( input );

} /* initInput */

static void finiInput( input_buffer *input )
{
    input = input;
    thisWindow = NO_WINDOW;
    PopMouseEventHandler();
    if( !EditFlags.NoInputWindow ) {
        NewCursor( input->window.id, NormalCursorType );
    }

} /* finiInput */

/*
 * getStringInWindow: main routine
 */
static bool getStringInWindow( input_buffer *input )
{
    int         event;
    int         old_mode;

    ReadingAString = TRUE;
    initInput( input );
    input->last_str = alloca( input->buffer_length );
    memset( input->last_str, 0, input->buffer_length );
    if( input->history != NULL ) {
        input->curr_hist = input->history->curr;
    }
    while( TRUE ) {
        event = GetNextEvent( FALSE );
        event = cursorKeyFilter( input, event );
        event = historyFilter( input, event );
        event = specialKeyFilter( input, event );
        switch( event ) {
        case HANDLED:
            break;
        case VI_KEY( SHIFT_TAB ):
        case VI_KEY( TAB ):
            if( !fileComplete( input, event ) ) {
                endColumn( input );
                break;
            }
            /* fall through */
        case VI_KEY( ENTER ):
            if( input->buffer[ 0 ] == NO_ADD_TO_HISTORY_KEY ) {
                strcpy( &input->buffer[ 0 ], &input->buffer[ 1 ] );
            } else {
                addHistory( input );
            }
            /* fall through */
        case VI_KEY( ESC ):
            finiInput( input );
            /*
             * this call may not be necessary if the file complete window has
             * already closed of natural causes but it doesn't harm anything
             * if called when not needed - so we leave it here.
             */
            FinishFileComplete();
            ReadingAString = FALSE;
            return( event != VI_KEY( ESC ) );
        case VI_KEY( INS ):
            input->overstrike = !input->overstrike;
            if( !EditFlags.NoInputWindow ) {
                NewCursor( input->window.id,
                    input->overstrike ? NormalCursorType : InsertCursorType );
            }
            break;
        case VI_KEY( CTRL_END ):
            saveStr( input );
            input->buffer[ input->curr_pos ] = 0;
            break;
        case VI_KEY( CTRL_X ):
        case VI_KEY( CTRL_U ):
            saveStr( input );
            input->buffer[ 0 ] = 0;
            endColumn( input );
            break;
        case VI_KEY( CTRL_INS ):
            swapString( input );
            break;
        case VI_KEY( CTRL_V ):
        case VI_KEY( CTRL_Q ):
            insertChar( input, '^' );
            displayLine( input );
            // here we have a bit of a kluge
            input->curr_pos -= 1;
            event = GetNextEvent( FALSE );
            saveStr( input );
            old_mode = input->overstrike;
            input->overstrike = TRUE;
            insertChar( input, event );
            input->overstrike = old_mode;
            break;
        case VI_KEY( ALT_END ):
            /* just want to redraw the line - for windows */
            break;
        default:
            if( (event >= 32 && event < 128) || event == VI_KEY( CTRL_A ) ) {
                saveStr( input );
                if( !insertChar( input, event ) ) {
                    MyBeep();
                }
            }
        }
        if( !EditFlags.NoInputWindow ) {
            displayLine( input );
        }
    }

} /* getStringInWindow */

bool ReadStringInWindow( window_id id, int line, char *prompt, char *str,
    int max_len, history_data *history )
{
    input_buffer        input;
    int                 rc;

    input.prompt = prompt;
    input.buffer = str;
    input.buffer_length = max_len;
    input.history = history;
    input.window.id = id;
    input.window.line = line;
#ifdef __WIN__
    input.cache = (char *)MemAlloc( max_len );
    input.cache[ 0 ] = 0;
#endif
    rc = getStringInWindow( &input );
#ifdef __WIN__
    MemFree( input.cache );
#endif
    return( rc );

} /* ReadStringInWindow */

int PromptForString( char *prompt, char *buffer,
                        int buffer_length, history_data *history )
{
    int                 err;
    window_id           id;

    if( !EditFlags.NoInputWindow ) {
        err = NewWindow2( &id, &cmdlinew_info );
        if( err ) {
            return( err );
        }
    } else {
        id = NO_WINDOW;
    }

    if( !EditFlags.NoInputWindow &&
        strlen( prompt ) >= WindowAuxInfo( id, WIND_INFO_TEXT_COLS ) ) {
        err = ERR_PROMPT_TOO_LONG;
    } else {
        err = NO_VALUE_ENTERED;
        if( ReadStringInWindow( id, 1, prompt, buffer, buffer_length, history ) ) {
            err = ERR_NO_ERR;
        }
    }

    if( !EditFlags.NoInputWindow ) {
        CloseAWindow( id );
        SetWindowCursor();
    } else {
        EditFlags.NoInputWindow = FALSE;
    }
    return( err );

} /* PromptForString */
