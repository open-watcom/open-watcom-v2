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


#include <wwindows.h>
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wresall.h"
#include "clibext.h"

static int WRCountChars( unsigned char *str, char c )
{
    int     count;

    count = 0;

    while( str != NULL && *str != '\0' ) {
        str = _mbschr( str, c );
        if( str != NULL ) {
            count++;
            str = _mbsinc( str );
        }
    }

    return( count );
}

static int WRCountCharsString( unsigned char *str, unsigned char *s )
{
    int     count;

    if( str == NULL || s == NULL ) {
        return( 0 );
    }

    count = 0;
    for( ; *str != '\0'; str = _mbsinc( str ) ) {
        if( _mbschr( s, *str ) != NULL ) {
            count++;
        }
    }

    return( count );
}

// changes all occurrences of chars in 'from' to '\to' in str
char *WRAPI WRConvertStringFrom( char *_str, char *_from, char *to )
{
    char            *new;
    unsigned char   *s;
    int             len;
    int             pos;
    unsigned char   *str = (unsigned char *)_str;
    unsigned char   *from = (unsigned char *)_from;

    if( str == NULL || from == NULL || to == NULL ) {
        return( NULL );
    }
    len = strlen( _str ) + WRCountCharsString( str, from ) + 1;
    new = MemAlloc( len );
    if( new == NULL ) {
        return( NULL );
    }

    pos = 0;
    for( ; *str != '\0'; str = _mbsinc( str ) ) {
        s = _mbschr( from, *str );
        if( s != NULL ) {
            new[pos++] = '\\';
            new[pos++] = to[s - from];
        } else {
            new[pos++] = str[0];
            if( _mbislead( str[0] ) ) {
                new[pos++] = str[1];
            }
        }
    }
    new[pos] = '\0';

    return( new );
}

// changes all occurrences of 'from' to '\to' in str
char *WRAPI WRConvertFrom( char *_str, char from, char to )
{
    char            *new;
    int             len;
    int             pos;
    unsigned char   *str = (unsigned char *)_str;

    if( str == NULL ) {
        return( NULL );
    }

    len = strlen( _str ) + WRCountChars( str, from ) + 1;
    new = MemAlloc( len );
    if( new == NULL ) {
        return( NULL );
    }

    pos = 0;
    for( ; *str != '\0'; str = _mbsinc( str ) ) {
        if( *str == from ) {
            new[pos++] = '\\';
            new[pos++] = to;
        } else {
            new[pos++] = str[0];
            if( _mbislead( str[0] ) ) {
                new[pos++] = str[1];
            }
        }
    }
    new[pos] = '\0';

    return( new );
}

// changes all occurrences of '\from' to 'to' in str
char *WRAPI WRConvertTo( char *_str, char to, char from )
{
    char            *new;
    int             len;
    int             pos;
    unsigned char   *str = (unsigned char *)_str;

    if( str == NULL ) {
        return( NULL );
    }

    len = strlen( _str ) + 1;
    new = MemAlloc( len );
    if( new == NULL ) {
        return( NULL );
    }

    pos = 0;
    for( ; *str != '\0'; str = _mbsinc( str ) ) {
        if( !_mbislead( str[0] ) && !_mbislead( str[1] ) &&
            str[0] == '\\' && str[1] == from ) {
            new[pos++] = to;
            str++;
        } else {
            new[pos++] = str[0];
            if( _mbislead( str[0] ) ) {
                new[pos++] = str[1];
            }
        }
    }
    new[pos] = '\0';

    return( new );
}

// changes all occurrences of the \chars in 'from' to 'to' in str
char * WRAPI WRConvertStringTo( char *_str, char *to, char *_from )
{
    char            *new;
    unsigned char   *s;
    int             len;
    int             pos;
    unsigned char   *str = (unsigned char *)_str;
    unsigned char   *from = (unsigned char *)_from;

    if( str == NULL || to == NULL || from == NULL ) {
        return( NULL );
    }

    len = strlen( _str ) + 1;
    new = MemAlloc( len );
    if( new == NULL ) {
        return( NULL );
    }

    pos = 0;
    for( ; *str != '\0'; str = _mbsinc( str ) ) {
        if( !_mbislead( str[0] ) && !_mbislead( str[1] ) &&
            str[0] == '\\' && (s = _mbschr( from, str[1] )) != NULL ) {
            new[pos++] = to[s - from];
            str++;
        } else {
            new[pos++] = str[0];
            if( _mbislead( str[0] ) ) {
                new[pos++] = str[1];
            }
        }
    }
    new[pos] = '\0';

    return( new );
}

void WRAPI WRMassageFilter( char *_filter )
{
    char            sep;
    unsigned char   *filter = (unsigned char *)_filter;

    sep = '\t';

    while( filter != NULL && *filter != '\0' ) {
        filter = _mbschr( filter, sep );
        if( filter != NULL ) {
            *filter = '\0';
            filter++;
        }
    }
}

#if defined( __NT__ )
    #ifndef MB_ERR_INVALID_CHARS
        #define MB_ERR_INVALID_CHARS 0x00000000
    #endif
#endif

#if defined( __NT__ )

int WRAPI WRmbcs2unicodeBuf( char *src, char *dest, int len )
{
    uint_16     *new;
    int         len1, len2;

    if( dest == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        len1 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS,
                                    src, -1, NULL, 0 );
        if( len1 == 0 || len1 == ERROR_NO_UNICODE_TRANSLATION ) {
            return( FALSE );
        }
    } else {
        len1 = 1;
    }

    len1 *= sizeof( WCHAR );

    // if len is -1 then dont bother checking the buffer length
    if( len != -1 && len1 > len ) {
        return( FALSE );
    }

    new = (uint_16 *)dest;

    if( src != NULL ) {
        len2 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS,
                                    src, -1, (LPWSTR)new, len1 );
        len2 *= sizeof( WCHAR );
        if( len2 != len1 ) {
            return( FALSE );
        }
    } else {
        new[0] = '\0';
    }

    return( TRUE );
}

int WRAPI WRunicode2mbcsBuf( char *src, char *dest, int len )
{
    int         len1, len2;

    if( dest == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        len1 = WideCharToMultiByte( CP_OEMCP, 0L, (LPCWSTR)src, -1,
                                    NULL, 0, NULL, NULL );
        if( len1 == 0 ) {
            return( FALSE );
        }
    } else {
        len1 = 1;
    }

    // if len is -1 then dont bother checking the buffer length
    if( len != -1 && len1 > len ) {
        return( FALSE );
    }

    if( src != NULL ) {
        len2 = WideCharToMultiByte( CP_OEMCP, 0L, (LPCWSTR)src, -1,
                                    dest, len1, NULL, NULL );
        if( len2 != len1 ) {
            return( FALSE );
        }
    } else {
        dest[0] = '\0';
    }

    return( TRUE );
}

int WRAPI WRmbcs2unicode( char *src, char **dest, int *len )
{
    uint_16     *new;
    int         len1, len2;

    if( len == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        len1 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS,
                                    src, -1, NULL, 0 );
        if( len1 == 0 || len1 == ERROR_NO_UNICODE_TRANSLATION ) {
            return( FALSE );
        }
    } else {
        len1 = 1;
    }

    *len = len1 * sizeof( WCHAR );

    if( dest == NULL ) {
        return( TRUE );
    }

    new = MemAlloc( *len );
    if( new == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        len2 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS,
                                    src, -1, (LPWSTR)new, len1 );
        if( len2 != len1 ) {
            MemFree( new );
            return( FALSE );
        }
    } else {
        new[0] = '\0';
    }

    *dest = (char *)new;

    return( TRUE );
}

int WRAPI WRunicode2mbcs( char *src, char **dest, int *len )
{
    char        *new;
    int         len1, len2;

    if( len == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        len1 = WideCharToMultiByte( CP_OEMCP, 0L, (LPCWSTR)src, -1,
                                    NULL, 0, NULL, NULL );
        if( len1 == 0 ) {
            return( FALSE );
        }
    } else {
        len1 = 1;
    }

    *len = len1;

    if( dest == NULL ) {
        return( TRUE );
    }

    new = MemAlloc( len1 );
    if( new == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        len2 = WideCharToMultiByte( CP_OEMCP, 0L, (LPCWSTR)src, -1,
                                    new, len1, NULL, NULL );
        if( len2 != len1 ) {
            MemFree( new );
            return( FALSE );
        }
    } else {
        new[0] = '\0';
    }

    *dest = new;

    return( TRUE );
}

#else

int WRAPI WRmbcs2unicodeBuf( char *src, char *dest, int len )
{
    uint_16     *new;
    int         len1, len2;

    if( dest == NULL ) {
        return( FALSE );
    }

    len1 = 1;
    if( src != NULL ) {
        len1 += strlen( src );
    }

    len1 *= 2;

    // if len is -1 then dont bother checking the buffer length
    if( len != -1 && len1 > len ) {
        return( FALSE );
    }

    new = (uint_16 *)dest;

    if( src != NULL ) {
        for( len2 = len1 / 2; len2 >= 0; len2-- ) {
            new[len2] = (uint_16)src[len2];
        }
    } else {
        new[0] = 0;
    }

    return( TRUE );
}

int WRAPI WRunicode2mbcsBuf( char *src, char *dest, int len )
{
    uint_16     *uni_str;
    int         len1, len2;

    if( dest == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        len1 = WRStrlen32( src );
        len1 = len1 / 2;
    } else {
        len1 = 1;
    }

    // if len is -1 then dont bother checking the buffer length
    if( len != -1 && len1 > len ) {
        return( FALSE );
    }

    if( src != NULL ) {
        uni_str = (uint_16 *)src;
        for( len2 = 0; len2 <= len1; len2++ ) {
            dest[len2] = (char)uni_str[len2];
        }
    } else {
        dest[0] = '\0';
    }

    return( TRUE );
}

int WRAPI WRmbcs2unicode( char *src, char **dest, int *len )
{
    uint_16     *new;
    int         len1;

    if( len == NULL ) {
        return( FALSE );
    }

    *len = 1;
    if( src != NULL ) {
        *len += strlen( src );
    }
    *len *= 2;

    if( dest == NULL ) {
        return( TRUE );
    }

    new = MemAlloc( *len );
    if( new == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        for( len1 = *len / 2; len1 >= 0; len1-- ) {
            new[len1] = (uint_16)src[len1];
        }
    } else {
        new[0] = 0;
    }

    *dest = (char *)new;

    return( TRUE );
}

int WRAPI WRunicode2mbcs( char *src, char **dest, int *len )
{
    char        *new;
    uint_16     *uni_str;
    int         len1, len2;

    if( len == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        len1 = WRStrlen32( src );
        len1 = len1 / 2;
    } else {
        len1 = 1;
    }

    *len = len1;

    if( dest == NULL ) {
        return( TRUE );
    }

    new = MemAlloc( len1 );
    if( new == NULL ) {
        return( FALSE );
    }

    if( src != NULL ) {
        uni_str = (uint_16 *)src;
        for( len2 = 0; len2 <= len1; len2++ ) {
            new[len2] = (char)uni_str[len2];
        }
    } else {
        new[0] = '\0';
    }

    *dest = new;

    return( TRUE );
}

#endif

char * WRAPI WRWResIDNameToStr( WResIDName *name )
{
    char        *string;

    string = NULL;

    if( name != NULL ) {
        /* alloc space for the string and a \0 char at the end */
        string = MemAlloc( name->NumChars + 1 );
        if( string != NULL ) {
            /* copy the string */
            memcpy( string, name->Name, name->NumChars );
            string[name->NumChars] = '\0';
        }
    }

    return( string );
}

int WRAPI WRStrlen32( char *str )
{
    int         len;
    uint_16     *word;

    if( str == NULL ) {
        return( 0 );
    }

    len = 0;
    word = (uint_16 *)str;
    while( *word != 0 ) {
        len += 2;
        word++;
    }

    return( len );
}

int WRAPI WRStrlen( char *str, int is32bit )
{
    int         len;

    if( is32bit ) {
        len = WRStrlen32( str );
    } else {
        len = 0;
        if( str != NULL ) {
            len = strlen( str );
        }
    }

    return( len );
}

int WRAPI WRFindFnOffset( char *_name )
{
    int             offset;
    unsigned char   *cp;
    unsigned char   *last;
    unsigned char   *name = (unsigned char *)_name;

    if( name == NULL ) {
        return( -1 );
    }

    cp = name;
    last = name;
    while( *cp != 0 ) {
        if( !_mbislead( *cp ) && (*cp == ':' || *cp == '\\') ) {
            last = cp + 1;
        }
        cp = _mbsinc( cp );
    }

    if( *last == '\0' ) {
        return( -1 );
    }

    offset = last - name;

    return( offset );
}
