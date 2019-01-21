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
bool ExpandTabsInABufferUpToColumn( size_t endcol, char *in, size_t inlen, char *out, size_t outsize )
{
    size_t      i;
    size_t      j;
    bool        res;

    if( outsize > 0 ) {
        res = ExpandTabsInABuffer( in, endcol, out, outsize );
        outsize--;  /* reserve space for '\0' terminator */
        i = strlen( out );
        j = outsize - i + endcol;
        if( inlen > j ) {
            inlen = j;
        }
        for( j = endcol; j < inlen; j++ ) {
            out[i++] = in[j];
        }
        out[i] = '\0';
        return( res );
    }
    return( false );

} /* ExpandTabsInABufferUpToColumn */

/*
 * ExpandTabsInABuffer - do all tabs in a buffer
 */
bool ExpandTabsInABuffer( const char *in, size_t inlen, char *out, size_t outsize )
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
            c = (unsigned char)in[j];
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
int WinVirtualCursorPosition( char *buff, int curs )
{
    int j;
    int pos = 0;

    for( j = 0; j < curs; j++ ) {
        if( !getNextPos( (unsigned char)buff[j], &pos ) ) {
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
int WinRealCursorPosition( char *buff, int vc )
{
    int         pos, i, len;

    len = strlen( buff );
    pos = 0;
    for( i = 0; i < len; i++ ) {
        if( !getNextPos( (unsigned char)buff[i], &pos ) ) {
            break;
        }
        if( pos >= vc ) {
            return( i + 1 );
        }
    }
    return( len );

} /* WinRealCursorPosition */

/*
 * GetVirtualCursorPosition - get the virtual position of the cursor, given
 *                            the real position on the current line
 */
int GetVirtualCursorPosition( char *buff, int curs )
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
        if( !getNextPos( (unsigned char)buff[j], &pos ) ) {
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
static int realPosition( int virt_pos, char *buff, int len )
{
    int         j;
    int         pos = 0;

    if( EditFlags.Modeless ) {
        len++;
    }

    for( j = 0; j < len; j++ ) {
        if( !getNextPos( (unsigned char)buff[j], &pos ) ) {
            break;
        }
        if( pos >= virt_pos ) {
            return( j + 1 );
        }

    }
    return( len );

} /* realPosition */

/*
 * RealColumnOnCurrentLine - compute the real cursor position on the current
 *                      line, given the virtual position
 */
int RealColumnOnCurrentLine( int vc )
{
    int         cl;
    char        *buff;

    if( CurrentLine->u.ld.nolinedata ) {
        cl = WorkLine->len;
        buff = WorkLine->data;
    } else {
        cl = CurrentLine->len;
        buff = CurrentLine->data;
    }
    return( realPosition( vc, buff, cl ) );

} /* RealColumnOnCurrentLine */

/*
 * CursorPositionOffRight - determine if cursor is beyond the end of text
 */
bool CursorPositionOffRight( int vc )
{
    int         cl, j, pos = 0;
    char        *buff;


    if( CurrentLine->u.ld.nolinedata ) {
        cl = WorkLine->len;
        buff = WorkLine->data;
    } else {
        cl = CurrentLine->len;
        buff = CurrentLine->data;
    }
    if( cl == 0 ) {
        return( false );
    }

    if( EditFlags.Modeless ) {
        cl++;
    }

    for(j = 0; j < cl; j++ ) {
        if( !getNextPos( (unsigned char)buff[j], &pos ) ) {
            break;
        }
        if( pos >= vc ) {
            return( false );
        }

    }
    return( true );

} /* CursorPositionOffRight */

int RealCursorPositionInString( char *buff, int virt_pos )
{
    return( realPosition( virt_pos, buff, strlen( buff ) ) );
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
int VirtualLineLen( char *buff )
{
    int j, cl;
    int pos = 0;

    cl = strlen( buff );
    for( j = 0; j < cl; j++ ) {
        if( !getNextPos( (unsigned char)buff[j], &pos ) ) {
            break;
        }
    }
    return( pos );

} /* VirtualLineLen */

/*
 * AddLeadingTabSpace - make leading spaces tabs (if possible)
 */
bool AddLeadingTabSpace( size_t *len, char *buff, int amount )
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
    while( isspace( buff[start] ) )
        start++;
    tmp = StaticAlloc();
    j = *len;
    ExpandTabsInABuffer( buff, j, tmp, EditVars.MaxLine + 1 );
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
        if( l >= EditVars.MaxLine ) {
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
    for( l = start; l <= j; l++ ) {
        tmp[k++] = buff[l];
    }
    memcpy( buff, tmp, k );
    StaticFree( tmp );
    *len = k - 1;
    return( full );

} /* AddLeadingTabSpace */


/*
 * ConvertSpacesToTabsUpToColumn - add tabs only up to specified column
 */
bool ConvertSpacesToTabsUpToColumn( size_t endcol, char *in, size_t inlen, char *out, size_t outlen )
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
    char        oc;
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
    oc = in[endcol];
    in[endcol] = '\0';
    for( j = 0; j <= endcol; j++ ) {
        c = in[j];
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
    k--;
    in[endcol] = oc;
    if( inlen > outlen - k ) {
        inlen = outlen - k;
    }
    for( j = endcol; j < inlen; j++ ) {
        out[k++] = in[j];
    }
    out[k++] = '\0';
    return( tabme );

} /* ConvertSpacesToTabsUpToColumn */
