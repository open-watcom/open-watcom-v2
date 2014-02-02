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
* Description:  Implementation of fnmatch() - file name pattern matching.
*
****************************************************************************/


#include "variety.h"
#include <fnmatch.h>
#include <ctype.h>
#include <string.h>
#include <wctype.h>

/* Implementation note: On non-UNIX systems, backslashes in the string
 * (but not in the pattern) are considered to be path separators and
 * identical to forward slashes when FNM_PATHNAME is set.
 */

#ifdef __UNIX__
  #define IS_PATH_SEP(c)   (c == '/')
#else
  #define IS_PATH_SEP(c)   (c == '/' || c == '\\')
#endif


static char icase( int ch, int flags )
{
    if( flags & FNM_IGNORECASE ) {
        return( tolower( ch ) );
    } else {
        return( ch );
    }
}

/* Maximum length of character class name. 
 * The longest is currently 'xdigit' (6 chars).
 */
#define CCL_NAME_MAX    8

/* Note: Using wctype()/iswctype() may seem odd, but that way we can avoid 
 * hardcoded character class lists.
 */
static int sub_bracket( const char *p, int c )
{
    const char      *s = p;
    char            sname[CCL_NAME_MAX + 1];
    int             i;
    int             type;

    switch( *++p ) {
    case ':':
        ++p;
        for( i = 0; i < CCL_NAME_MAX; i++ ) {
            if( !isalpha(*p ) )
                break;
            sname[i] = *p++;
        }
        sname[i] = '\0';
        if( *p++ != ':' )
            return( 0 );
        if( *p++ != ']' )
            return( 0 );
        type = wctype( sname );
        if( type ) {
            int     rc;

            rc = p - s;
            return( iswctype( c, type ) ? rc : -rc );
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


static const char *cclass_match( const char *patt, int c )
{
    int	        ok = 0;
    int	        lc = 0;
    int	        state = 0;
    int         invert = 0;
    int	        sb;

    /* Meaning of '^' is unspecified in POSIX - consider it equal to '!' */
    if( *patt == '!' || *patt == '^' ) {
        invert = 1;
        ++patt;
    }
    while( *patt ) {
    	if( *patt == ']' )
    	    return( ok ^ invert ? patt + 1 : NULL );

        if( *patt == '[' ) {
             sb = sub_bracket( patt, c );
             if( sb < 0 ) {
                 patt -= sb;
                 ok = 0;
                 continue;
             } else if( sb > 0 ) {
                 patt += sb;
                 ok = 1;
                 continue;
             }
        }

        switch( state ) {
        case 0:
    	    if( *patt == '\\' )
                ++patt;
    	    if( *patt == c )
                ok = 1;
    	    lc = (int)*patt++;
    	    state = 1;
    	    break;
    	case 1:
    	    if( state = (*patt == '-') ? 2 : 0 )
                ++patt;
    	    break;
    	case 2:	
    	    if( *patt == '\\' )
                ++patt;
            if( lc <= c && c <= *patt )
                ok = 1;
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
    char        c, cl;
    const char  *start = s;

    while( 1 ) {
        c = icase( *patt++, flags );
    	switch( c ) {
        case '\0':
            if( flags & FNM_LEADING_DIR && IS_PATH_SEP( *s ) )
                return( 0 );
            return( *s ? FNM_NOMATCH : 0 );
    	case '?':
            if( flags & FNM_PATHNAME && IS_PATH_SEP( *s ) )
                return( FNM_NOMATCH );
            if( flags & FNM_PERIOD && *s == '.' && s == start )
                return( FNM_NOMATCH );
            ++s;
    	    break;
        case '[':
            if( flags & FNM_PATHNAME && IS_PATH_SEP( *s ) )
                return( FNM_NOMATCH );
            if( flags & FNM_PERIOD && *s == '.' && s == start )
                return( FNM_NOMATCH );
            patt = cclass_match( patt, *s );
    	    if( patt == NULL )
                return( FNM_NOMATCH );
    	    ++s;
    	    break;
        case '*':
            if( *s == '\0' )
                return( 0 );
            if( flags & FNM_PATHNAME && ( *patt == '/' ) ) {
                while( *s && !IS_PATH_SEP( *s ) )
                    ++s;
                break;
            }
            if( flags & FNM_PERIOD && *s == '.' && s == start )
                return( FNM_NOMATCH );
            if( *patt == '\0' ) {
                /* Shortcut - don't examine every remaining character. */
                if( flags & FNM_PATHNAME ) {
                    if( flags & FNM_LEADING_DIR || !strchr( s, '/' ) )
                        return( 0 );
                    else
                        return( FNM_NOMATCH );
                } else {
                    return( 0 );
                }
            }
            while( (cl = icase( *s, flags )) != '\0' ) {
                if( !fnmatch( patt, s, flags & ~FNM_PERIOD ) )
                    return( 0 );
                if( flags & FNM_PATHNAME && IS_PATH_SEP( cl ) ) {
                    start = s + 1;
                    break;
                }
                ++s;
            }
    	    return( FNM_NOMATCH );
        case '\\':
            if( !( flags & FNM_NOESCAPE ) ) {
                c = icase( *patt++, flags );
            }
            /* Fall through */
        default:
            if( IS_PATH_SEP( *s ) )
                start = s + 1;
            cl = icase( *s++, flags );
#ifndef __UNIX__
            if( flags & FNM_PATHNAME && cl == '\\' )
                cl = '/';
#endif
    	    if( c != cl )
                return( FNM_NOMATCH );
    	}
    }
}
