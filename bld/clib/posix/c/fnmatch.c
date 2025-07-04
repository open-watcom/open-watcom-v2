/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of fnmatch() - file name pattern matching.
*
****************************************************************************/


#include "variety.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <fnmatch.h>
#include "pathmac.h"
#include "intwctyp.h"
#include "xstring.h"


/* Implementation note: On non-UNIX systems, backslashes in the string
 * (but not in the pattern) are considered to be path separators and
 * identical to forward slashes when FNM_PATHNAME is set.
 */

static int icase( int c, int flags )
{
    if( flags & FNM_IGNORECASE ) {
        return( tolower( c ) );
    } else {
        return( c );
    }
}

/* Maximum length of character class name.
 * The longest is currently 'xdigit' (6 chars).
 */
#define CCL_NAME_MAX    8

/* Note: Using wctype()/iswctype() may seem odd, but that way we can avoid
 * hardcoded character class lists.
 */
static int sub_bracket( const unsigned char *p, int c )
{
    const unsigned char *s = p;
    char                sname[CCL_NAME_MAX + 1];
    int                 i;
    int                 type;

    switch( *++p ) {
    case ':':
        ++p;
        for( i = 0; i < CCL_NAME_MAX; i++ ) {
            if( !isalpha( *p ) )
                break;
            sname[i] = *p++;
        }
        sname[i] = NULLCHAR;
        if( *p++ != ':' )
            return( 0 );
        if( *p++ != ']' )
            return( 0 );
        type = __wctype( sname );
        if( type ) {
            int     rc;

            rc = p - s;
            switch( type ) {
            case WCTYPE_ALNUM:  if( isalnum( c ) ) return( rc );    break;
            case WCTYPE_ALPHA:  if( isalpha( c ) ) return( rc );    break;
            case WCTYPE_BLANK:  if( isblank( c ) ) return( rc );    break;
            case WCTYPE_CNTRL:  if( iscntrl( c ) ) return( rc );    break;
            case WCTYPE_DIGIT:  if( isdigit( c ) ) return( rc );    break;
            case WCTYPE_GRAPH:  if( isgraph( c ) ) return( rc );    break;
            case WCTYPE_LOWER:  if( islower( c ) ) return( rc );    break;
            case WCTYPE_PRINT:  if( isprint( c ) ) return( rc );    break;
            case WCTYPE_PUNCT:  if( ispunct( c ) ) return( rc );    break;
            case WCTYPE_SPACE:  if( isspace( c ) ) return( rc );    break;
            case WCTYPE_UPPER:  if( isupper( c ) ) return( rc );    break;
            case WCTYPE_XDIGIT: if( isxdigit( c ) ) return( rc );   break;
            }
            return( -rc );
        }
        return( 0 );
    case '=':
        return( 0 );
    case '.':
        return( 0 );
    default:
        return( 0 );
    }
}


static const char *cclass_match( const unsigned char *patt, int c )
{
    bool        ok;
    int         lc;
    int         state;
    bool        invert;
    int         sb;

    /* Meaning of '^' is unspecified in POSIX - consider it equal to '!' */
    invert = false;
    if( *patt == '!' || *patt == '^' ) {
        invert = true;
        ++patt;
    }
    lc = 0;
    state = 0;
    ok = false;
    while( *patt != NULLCHAR ) {
        if( *patt == ']' )
            return( ( ok ^ invert ) ? (const char *)( patt + 1 ) : NULL );

        if( *patt == '[' ) {
             sb = sub_bracket( patt, c );
             if( sb < 0 ) {
                 patt -= sb;
                 ok = false;
                 continue;
             } else if( sb > 0 ) {
                 patt += sb;
                 ok = true;
                 continue;
             }
        }

        switch( state ) {
        case 0:
            if( *patt == DIR_SEP )
                ++patt;
            if( *patt == c )
                ok = true;
            lc = *patt++;
            state = 1;
            break;
        case 1:
            if( state = (*patt == '-') ? 2 : 0 )
                ++patt;
            break;
        case 2:
            if( *patt == DIR_SEP )
                ++patt;
            if( lc <= c && c <= *patt )
                ok = true;
            ++patt;
            state = 0;
            break;
        default:
            return( NULL );
        }
    }
    return( NULL );
}


_WCRTLINK int   fnmatch( const char *patt, const char *s, int flags )
/*******************************************************************/
{
    int         cp;
    int         cs;
    int         cl;
    const char  *start = s;

    for( ;; ) {
        cp = icase( CHAR2INT( *patt++ ), flags );
        cs = CHAR2INT( *s );
        switch( cp ) {
        case NULLCHAR:
            if( (flags & FNM_LEADING_DIR) && IS_DIR_SEP( cs ) )
                return( 0 );
            return( ( cs != NULLCHAR ) ? FNM_NOMATCH : 0 );
        case '?':
            if( (flags & FNM_PATHNAME) && IS_DIR_SEP( cs ) )
                return( FNM_NOMATCH );
            if( (flags & FNM_PERIOD) && cs == '.' && s == start )
                return( FNM_NOMATCH );
            ++s;
            break;
        case '[':
            if( (flags & FNM_PATHNAME) && IS_DIR_SEP( cs ) )
                return( FNM_NOMATCH );
            if( (flags & FNM_PERIOD) && cs == '.' && s == start )
                return( FNM_NOMATCH );
            patt = cclass_match( (unsigned char *)patt, cs );
            if( patt == NULL )
                return( FNM_NOMATCH );
            ++s;
            break;
        case '*':
            if( cs == NULLCHAR )
                return( 0 );
            if( (flags & FNM_PATHNAME) && ( CHAR2INT( *patt ) == '/' ) ) {
                while( cs != NULLCHAR && !IS_DIR_SEP( cs ) ) {
                    ++s;
                    cs = CHAR2INT( *s );
                }
                break;
            }
            if( (flags & FNM_PERIOD) && cs == '.' && s == start )
                return( FNM_NOMATCH );
            if( *patt == NULLCHAR ) {
                /* Shortcut - don't examine every remaining character. */
                if( flags & FNM_PATHNAME ) {
                    if( (flags & FNM_LEADING_DIR) || !strchr( s, '/' ) ) {
                        return( 0 );
                    } else {
                        return( FNM_NOMATCH );
                    }
                } else {
                    return( 0 );
                }
            }
            while( (cl = icase( cs, flags )) != NULLCHAR ) {
                if( !fnmatch( patt, s, flags & ~FNM_PERIOD ) )
                    return( 0 );
                if( (flags & FNM_PATHNAME) && IS_DIR_SEP( cl ) ) {
                    start = s + 1;
                    break;
                }
                ++s;
                cs = CHAR2INT( *s );
            }
            return( FNM_NOMATCH );
        case '\\':
            if( (flags & FNM_NOESCAPE) == 0 ) {
                cp = icase( CHAR2INT( *patt++ ), flags );
            }
            /* Fall through */
        default:
            if( IS_DIR_SEP( cs ) )
                start = s + 1;
            cl = icase( cs, flags );
            ++s;
#ifndef __UNIX__
            if( (flags & FNM_PATHNAME) && cl == DIR_SEP )
                cl = ALT_DIR_SEP;
#endif
            if( cp != cl ) {
                return( FNM_NOMATCH );
            }
        }
    }
}
