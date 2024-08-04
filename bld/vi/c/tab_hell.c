/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Tab hell. The name says it all.
*
****************************************************************************/


#include "vi.h"
#include "win.h"

long TabCnt;

/*
 * getNextPos - get next cursor position, given current
 */
static bool getNextPos( int ch, int *opos )
{
    int pos;

    pos = *opos;

    if( ch < ' ' || ch > 127 ) {
        if( ch == '\t' ) {
#ifdef __WIN__
            pos += Tab( pos + 1, EditVars.HardTab );
#else
            if( EditFlags.RealTabs ) {
                pos += Tab( pos + 1, EditVars.HardTab );
            } else {
                pos++;
            }
#endif
        } else if( ch == 0 ) {
            return( false );
        } else if( EditFlags.EightBits ) {
            pos++;
        } else {
            pos += 2;
        }
    } else {
        pos++;
    }
    *opos = pos;
    return( true );

} /* getNextPos */

/*
 * InsertTabSpace - insert tabs and white space
 */
size_t InsertTabSpace( size_t j, char *buff, bool *tabme )
{
    size_t  extra;
    size_t  i;
    size_t  k;
    size_t  m;
    size_t  n;

    k = 0;
    if( *tabme ) {
        *tabme = false;
        n = EditVars.HardTab - Tab( j + 1, EditVars.HardTab );
        if( j > n ) {
            extra = j - n;
            m = extra / EditVars.HardTab;
            if( extra % EditVars.HardTab > 0 ) {
                m++;
            }
            TabCnt += m;
            *tabme = true;
            for( i = 0; i < m; i++ ) {
                buff[k++] = '\t';
            }
        } else {
            n = j;
        }
    } else {
        n = j;
    }

    /*
     * put in extra spaces
     */
    for( i = 0; i < n; i++ ) {
        buff[k++] = ' ';
    }
    return( k );

} /* InsertTabSpace */


/*
 * ExpandTabsInABufferUpToColumn - expand tabs only up to specified column
 */
bool ExpandTabsInABufferUpToColumn( size_t endcol, const char *instr, size_t inlen, char *out, size_t outsize )
{
    size_t      i;
    size_t      j;
    bool        res;

    if( outsize > 0 ) {
        res = ExpandTabsInABuffer( instr, endcol, out, outsize );
        outsize--;  /* reserve space for '\0' terminator */
        i = strlen( out );
        j = outsize - i + endcol;
        if( inlen > j ) {
            inlen = j;
        }
        for( j = endcol; j < inlen; j++ ) {
            out[i++] = instr[j];
        }
        out[i] = '\0';
        return( res );
    }
    return( false );

} /* ExpandTabsInABufferUpToColumn */

/*
 * ExpandTabsInABuffer - do all tabs in a buffer
 */
bool ExpandTabsInABuffer( const char *instr, size_t inlen, char *out, size_t outsize )
{
    size_t          j;
    size_t          k;
    size_t          tb;
    bool            tabme;
    int             c;

    /*
     * run through each character
     */
    tabme = false;
    if( outsize > 0 ) {
        outsize--;  /* reserve space for '\0' terminator */
        k = 0;
        for( j = 0; j < inlen && k < outsize; j++ ) {
            /*
             * if we have a tab, insert some spaces
             */
            c = (unsigned char)instr[j];
            if( c < ' ' || c > 127 ) {
                if( c == '\t' ) {
                    tabme = true;
                    TabCnt++;
                    tb = k + Tab( k + 1, EditVars.HardTab );
                    if( tb > outsize )
                        tb = outsize;
                    while( k < tb ) {
                        out[k++] = ' ';
                    }
                    continue;
                } else if( !EditFlags.EightBits ) {
                    tabme = true;
                    if( k + 1 >= outsize ) {
                        break;
                    }
                    if( c > 127 ) {
                        c = '?';
                    } else {
                        c = c + 'A' - 1;
                    }
                    out[k++] = '^';
                }
            }
            out[k++] = c;
        }
        out[k] = '\0';
    }
    return( tabme );

} /* ExpandTabsInABuffer */


// These 2 routines are prefered if you want a position on
// the screen (not a "cursor" position)

/*
 * WinVirtualCursorPosition - same as below but ignores goofy
 *                            insert mode considerations.
 *                            curs is base 1
 *                            returned coordinates are base 1
 */
int WinVirtualCursorPosition( const char *instr, int curs )
{
    int j;
    int pos = 0;

    for( j = 0; j < curs; j++ ) {
        if( !getNextPos( (unsigned char)instr[j], &pos ) ) {
            break;
        }
    }
    if( pos == 0 ) {
        pos = 1;
    }
    return( pos );
}

/*
 * WinRealCursorPosition - compute the real cursor position in the given
 *                         line, given the virtual position
 *                         vc is base 1
 *                         returned coordinates are base 1
 */
int WinRealCursorPosition( const char *instr, int vc )
{
    int         pos;
    int         i;
    int         inlen;

    inlen = strlen( instr );
    pos = 0;
    for( i = 0; i < inlen; i++ ) {
        if( !getNextPos( (unsigned char)instr[i], &pos ) ) {
            break;
        }
        if( pos >= vc ) {
            return( i + 1 );
        }
    }
    return( inlen );

} /* WinRealCursorPosition */

/*
 * GetVirtualCursorPosition - get the virtual position of the cursor, given
 *                            the real position on the current line
 */
int GetVirtualCursorPosition( const char *instr, int curs )
{
    int j;
    int pos = 0;

    /*
     * run through each character
     */
    if( EditFlags.InsertModeActive || EditFlags.Modeless ) {
        curs--;
    }
    for( j = 0; j < curs; j++ ) {
        if( !getNextPos( (unsigned char)instr[j], &pos ) ) {
            break;
        }
    }
    if( EditFlags.InsertModeActive || EditFlags.Modeless ) {
        return( pos + 1 );
    }
    if( pos == 0 ) {
        pos = 1;
    }
    return( pos );

} /* GetVirtualCursorPosition */

/*
 * VirtualColumnOnCurrentLine - compute the screen position of a specified
 *                          column in the current line
 */
int VirtualColumnOnCurrentLine( int ccol )
{
    int col;

    if( CurrentLine == NULL ) {
        return( 1 );
    }

    if( ccol == 0 ) {
        col = 1;
    } else {
        col = ccol;
    }
    if( CurrentLine->u.ld.nolinedata ) {
        return( GetVirtualCursorPosition( WorkLine->data, col ) );
    } else {
        return( GetVirtualCursorPosition( CurrentLine->data, col ) );
    }

} /* VirtualColumnOnCurrentLine */

/*
 * realPosition
 */
static int realPosition( int virt_pos, const char *instr, int inlen )
{
    int         j;
    int         pos = 0;

    if( EditFlags.Modeless ) {
        inlen++;
    }

    for( j = 0; j < inlen; j++ ) {
        if( !getNextPos( (unsigned char)instr[j], &pos ) ) {
            break;
        }
        if( pos >= virt_pos ) {
            return( j + 1 );
        }

    }
    return( inlen );

} /* realPosition */

/*
 * RealColumnOnCurrentLine - compute the real cursor position on the current
 *                      line, given the virtual position
 */
int RealColumnOnCurrentLine( int vc )
{
    int         inlen;
    const char  *instr;

    if( CurrentLine->u.ld.nolinedata ) {
        inlen = WorkLine->len;
        instr = WorkLine->data;
    } else {
        inlen = CurrentLine->len;
        instr = CurrentLine->data;
    }
    return( realPosition( vc, instr, inlen ) );

} /* RealColumnOnCurrentLine */

/*
 * CursorPositionOffRight - determine if cursor is beyond the end of text
 */
bool CursorPositionOffRight( int vc )
{
    int         j, pos = 0;
    int         inlen;
    const char  *instr;


    if( CurrentLine->u.ld.nolinedata ) {
        inlen = WorkLine->len;
        instr = WorkLine->data;
    } else {
        inlen = CurrentLine->len;
        instr = CurrentLine->data;
    }
    if( inlen == 0 ) {
        return( false );
    }

    if( EditFlags.Modeless ) {
        inlen++;
    }

    for(j = 0; j < inlen; j++ ) {
        if( !getNextPos( (unsigned char)instr[j], &pos ) ) {
            break;
        }
        if( pos >= vc ) {
            return( false );
        }

    }
    return( true );

} /* CursorPositionOffRight */

int RealCursorPositionInString( const char *instr, int virt_pos )
{
    return( realPosition( virt_pos, instr, strlen( instr ) ) );
}

int RealCursorPositionOnLine( linenum line_num, int virt_pos )
{
    line        *line;
    fcb         *fcb;
    int         real_pos;
    vi_rc       rc;

    real_pos = -1;
    rc = CGimmeLinePtr( line_num, &fcb, &line );
    if( rc == ERR_NO_ERR ) {
        real_pos = realPosition( virt_pos, line->data, line->len );
    }
    return( real_pos );

} /* RealCursorPositionOnLine */

/*
 * VirtualLineLen - compute the real length of a specified line
 */
int VirtualLineLen( const char *instr )
{
    int j;
    int pos = 0;
    int inlen;

    inlen = strlen( instr );
    for( j = 0; j < inlen; j++ ) {
        if( !getNextPos( (unsigned char)instr[j], &pos ) ) {
            break;
        }
    }
    return( pos );

} /* VirtualLineLen */

/*
 * AddLeadingTabSpace - make leading spaces tabs (if possible)
 */
bool AddLeadingTabSpace( size_t *len, char *data, int amount )
{
    char        *tmp;
    size_t      i;
    size_t      j;
    size_t      k;
    size_t      l;
    size_t      start;
    bool        tabme;
    bool        full;

    /*
     * expand leading stuff into spaces
     */
    start = 0;
    while( isspace( data[start] ) )
        start++;
    tmp = StaticAlloc();
    j = *len;
    ExpandTabsInABuffer( data, j, tmp, EditVars.MaxLineLen + 1 );
    i = 0;
    while( tmp[i] == ' ' )
        i++;

    /*
     * subtract/add extra spaces
     */
    full = false;
    if( amount == 0 ) {
        // no shift
        k = i;
    } else if( amount < 0 ) {
        // shift left, amount < 0
        l = -amount;
        if( i < l ) {
            k = 0;
        } else {
            k = i - l;
        }
    } else {
        // shift right, amount > 0
        l = i + amount;
        if( l >= EditVars.MaxLineLen ) {
            full = true;
            k = i;
        } else {
            for( k = i; k < l; k++ ) {
                tmp[k] = ' ';
            }
        }
    }
    tmp[k] = '\0';

    /*
     * change spaces to tabs, and add the rest of the line back
     */
    tabme = EditFlags.RealTabs;
    if( k > 0 ) {
        k = InsertTabSpace( k, tmp, &tabme );
    }
    for( l = start; l < j; l++ ) {
        tmp[k++] = data[l];
    }
    memcpy( data, tmp, k );
    data[k] = '\0';
    StaticFree( tmp );
    *len = k;
    return( full );

} /* AddLeadingTabSpace */


/*
 * ConvertSpacesToTabsUpToColumn - add tabs only up to specified column
 */
bool ConvertSpacesToTabsUpToColumn( size_t endcol, const char *instr, size_t inlen, char *out, size_t outlen )
{
    size_t      extra;
    size_t      first_blank;
    size_t      i;
    size_t      j;
    size_t      k;
    size_t      l;
    size_t      m;
    size_t      n;
    bool        blanks_inprog, tabme;
    char        c;
    bool        in_quotes;
    bool        in_single_quotes;
    bool        esc;

    /*
     * init for this line
     */
    k = 0;
    first_blank = 0;
    blanks_inprog = false;
    tabme = false;
    in_quotes = false;
    in_single_quotes = false;
    esc = false;

    /*
     * run through each character
     */
    for( j = 0; j < endcol; j++ ) {
        c = instr[j];
        if( c != ' ' || in_quotes || in_single_quotes ) {

            /*
             * if last char was blank, special processing
             */
            if( blanks_inprog ) {
                /*
                 * work out number of tabs and spaces
                 * that need to be added
                 */
                blanks_inprog = false;
                l = j - first_blank;
                if( l > 1 ) {
                    n = EditVars.HardTab - Tab( j + 1, EditVars.HardTab );
                    /*
                     * add tabs, then spaces
                     */
                    if( l > n ) {
                        extra = l - n;
                        m = extra / EditVars.HardTab;
                        if( extra % EditVars.HardTab > 0 ) {
                            m++;
                        }
                        TabCnt += m;
                        tabme = true;
                        if( k + m >= outlen ) {
                            m = outlen - k;
                        }
                        for( i = 0; i < m; i++ ) {
                            out[k++] = '\t';
                        }
                    } else {
                        n = l;
                    }
                    if( k + n >= outlen ) {
                        n = outlen - k;
                    }
                    for( i = 0; i < n; i++ ) {
                        out[k++] = ' ';
                    }
                } else {
                    if( k + 1 < outlen ) {
                        out[k++] = ' ';
                    }
                }
            }
            if( k + 1 >= outlen ) {
                break;
            }
            if( !esc ) {
                if( c == '\'' ) {
                    if( !in_quotes ) {
                        in_single_quotes = !in_single_quotes;
                    }
                } else if( c == '"' ) {
                    in_quotes = !in_quotes;
                } else if( c == '\\' ) {
                    esc = true;
                }
            } else {
                esc = false;
            }
            out[k++] = c;
            if( c == '\0' ) {
                break;
            }
        } else {
            /*
             * mark start of blanks
             */
            if( !blanks_inprog ) {
                first_blank = j;
                blanks_inprog = true;
            }
        }
    }

    /*
     * copy the rest
     */
//    k--;
    if( inlen > outlen - k ) {
        inlen = outlen - k;
    }
    for( j = endcol; j < inlen; j++ ) {
        out[k++] = instr[j];
    }
    out[k] = '\0';
    return( tabme );

} /* ConvertSpacesToTabsUpToColumn */
