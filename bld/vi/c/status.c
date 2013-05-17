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
#include "win.h"
#ifdef __WIN__
    #include "statwnd.h"
#endif

/*
 * NewStatusWindow - create a new status window
 */
vi_rc NewStatusWindow( void )
{
    vi_rc   rc = ERR_NO_ERR;

    if( !EditFlags.WindowsStarted ) {
        return( ERR_NO_ERR );
    }
    if( StatusWindow != NO_WINDOW ) {
        CloseAWindow( StatusWindow );
        StatusWindow = NO_WINDOW;
    }
    if( EditFlags.StatusInfo ) {
        rc = NewWindow2( &StatusWindow, &statusw_info );
        UpdateStatusWindow();
    }
    return( rc );

} /* NewStatusWindow */

#ifndef __WIN__
/*
 * StatusLine - display a line in the status window
 */
void StatusLine( int line, char *str, int format )
{
    int         len, width, blanks, i, j;
    type_style  *style;

    len = strlen( str );
    width = WindowAuxInfo( StatusWindow, WIND_INFO_TEXT_COLS );
    style = &statusw_info.text;
    switch( format ) {
    case FMT_RIGHT:
        blanks = 0;
        if( width > len ) {
            blanks = width - len;
        }
        break;
    case FMT_CENTRE:
        blanks = 0;
        if( width > len ) {
            blanks = (width - len) / 2;
        }
        break;
    default:
        DisplayLineInWindow( StatusWindow, line, str );
        return;
    }
    i = 1;
    while( i <= blanks ) {
        SetCharInWindowWithColor( StatusWindow, line, i, ' ', style );
        i++;
    }
    j = 0;
    while( j < len && i <= width ) {
        SetCharInWindowWithColor( StatusWindow, line, i, str[j], style );
        j += 1;
        i += 1;
    }
    while( i <= width ) {
        SetCharInWindowWithColor( StatusWindow, line, i, ' ', style );
        i++;
    }

} /* StatusLine */
#endif

/*
 * UpdateStatusWindow - update the status window
 */
void UpdateStatusWindow( void )
{
    char        *str, *ptr;
    char        result[5 * MAX_STR];
    char        *res;
    int         digits;
    long        num;
    bool        use_num;
    int         line;
    char        numstr[12];
    int         format;

    if( StatusWindow == NO_WINDOW ||
        EditFlags.DisplayHold ||
        EditFlags.Quiet ||
        !EditFlags.StatusInfo ||
        EditVars.StatusString == NULL ) {
        return;
    }

    str = EditVars.StatusString;
    res = result;
    line = 1;
    format = FMT_LEFT;
    EditFlags.ModeInStatusLine = FALSE;
    while( *str ) {
        if( *str == '$' ) {
            str++;
            ptr = str;
            while( isdigit( *str ) ) {
                str++;
            }
            if( ptr != str ) {
                digits = strtoul( ptr, NULL, 10 );
            } else {
                digits = 0;
            }
            use_num = FALSE;
            num = 0;
            switch( *str++ ) {
            case '$':
                *res++ = '$';
                break;
            case 'c':
                *res++ = ',';
                break;
            case 'n':
                *res = 0;
                StatusLine( line, result, format );
                res = result;
                line++;
                break;
            case 'L':
                num = CurrentPos.line;
                use_num = TRUE;
                break;
            case 'C':
                num = VirtualColumnOnCurrentLine( CurrentPos.column );
                use_num = TRUE;
                break;
            case 'D':
#ifdef __WIN__
                GetDateString( res );
#else
                GetDateTimeString( res );
#endif
                res += strlen( res );
                break;
            case 'T':
                GetTimeString( res );
                res += strlen( res );
                break;
            case 'M':
                /* print the desired mode */
                EditFlags.ModeInStatusLine = TRUE;
                GetModeString( res );
                res += strlen( res );
                break;
#ifdef __WIN__
            case 'H':
                GetMenuHelpString( res );
                res += strlen( res );
                break;
            case '[':
                *res++ = STATUS_ESC_CHAR;
                *res++ = STATUS_NEXT_BLOCK;
                break;
            case '|':
                *res++ = STATUS_ESC_CHAR;
                *res++ = STATUS_FORMAT_CENTER;
                break;
            case '>':
                *res++ = STATUS_ESC_CHAR;
                *res++ = STATUS_FORMAT_RIGHT;
                break;
            case '<':
                *res++ = STATUS_ESC_CHAR;
                *res++ = STATUS_FORMAT_LEFT;
                break;
#else
            case '|':
                format = FMT_CENTRE;
                break;
            case '>':
                format = FMT_RIGHT;
                break;
            case '<':
                format = FMT_LEFT;
                break;
#endif
            }
            if( use_num ) {
                ltoa( num, numstr, 10 );
                digits -= strlen( numstr );
                while( digits > 0 ) {
                    *res++ = ' ';
                    digits--;
                }
                ptr = numstr;
                while( *ptr ) {
                    *res++ = *ptr++;
                }
            }
        } else {
            *res++ = *str++;
        }
    }
    if( res != result ) {
        *res = 0;
        StatusLine( line, result, format );
    }

} /* UpdateStatusWindow */
