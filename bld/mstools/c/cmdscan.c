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
* Description:  Command line parsing routines.
*
****************************************************************************/


#include <ctype.h>
#include <stddef.h>
#include "bool.h"
#include "context.h"
#include "memory.h"
#include "pathconv.h"
#include "cmdscan.h"


bool Quoted = FALSE;

/*
 * Append a character to a dynamically allocated string, increasing the
 * buffer size if necessary.  Returns a pointer to a buffer containing the
 * new data.
 */
static char *got_char( char *buf, size_t *bufsize, size_t offset, char ch )
/*************************************************************************/
{
    const size_t        blocksize = 64;

    /*** Increase the buffer size if necessary ***/
    while( offset+1 >= *bufsize ) {
        *bufsize += blocksize;
        buf = ReallocMem( buf, (*bufsize)+blocksize);
    }

    /*** Append the character ***/
    buf[offset] = ch;
    buf[offset+1] = '\0';
    return( buf );
}

/*
 * Skip all whitespace characters, such that the next read will retrieve the
 * first non-whitespace character.
 */
void CmdScanWhitespace( void )
/****************************/
{
    char    ch;

    do {
        ch = GetCharContext();
    } while( isspace( ch )  &&  ch != '\0' );
    if( ch != '\0' )  UngetCharContext();
}


/*
 * If the next character is ch (in either uppercase or lowercase form), it
 * is consumed and a non-zero value is returned; otherwise, it is not
 * consumed and zero is returned.
 */
bool CmdScanRecogChar( int ch )
/*****************************/
{
    if( GetCharContext() == (char)ch ) {
        return( TRUE );
    } else {
        UngetCharContext();
        return( FALSE );
    }
}


/*
 * If the next character is ch, it is consumed and a non-zero value is
 * returned; otherwise, it is not consumed and zero is returned.
 */
bool CmdScanRecogLowerChar( int ch )
/**********************************/
{
    if( tolower( (unsigned char)GetCharContext() ) == tolower( ch ) ) {
        return( TRUE );
    } else {
        UngetCharContext();
        return( FALSE );
    }
}


/*
 * Scan a string.  No leading whitespace is allowed.  Returns a pointer
 * to newly allocated memory containing the string.  If leading whitespace
 * is found, returns NULL.  Quotes embedded within a string must be escaped
 * by a preceding backslash; two consecutive backslashes are reduced to one.
 */
char *CmdScanString( void )
/*************************/
{
    char                ch;
    bool                inQuote = Quoted;   /* true if inside a quoted string */
    bool                backslash = FALSE;  /* true if last char was a '\\' */
    long                start;              /* context offset of string start */
    char *              buf = DupStrMem( "" );
    size_t              bufsize = 0;
    size_t              offset = 0;

    /*** Return NULL if there's leading whitespace or no more data ***/
    ch = GetCharContext();
    if( !Quoted && isspace( ch ) ) {
        UngetCharContext();
        return( NULL );
    } else if( ch != '\0' ) {
        UngetCharContext();
    } else {
        return( NULL );
    }

    /*** Count the number of characters in the string ***/
    start = GetPosContext();
    for( ;; ) {
        ch = GetCharContext();
        if( ch == '\0' )  break;
        if( !inQuote && isspace( ch ) )  break;
        if( ch == '"' ) {
            if( backslash ) {
                backslash = FALSE;      /* handle \" within a string */
            } else if( inQuote ) {
                if( Quoted ) {
                    Quoted = FALSE;
                    return( buf );
                }
                inQuote = FALSE;        /* end of a quoted portion */
            } else {
                inQuote = TRUE;         /* start of a quoted portion */
            }
            buf = got_char( buf, &bufsize, offset, ch );
            offset++;
        } else if( ch == '\\' ) {
            if( backslash ) {
                buf = got_char( buf, &bufsize, offset, ch );
                offset++;
                backslash = FALSE;      /* second '\\' of a pair */
                if( GetCharContext() == '"' )
                    buf = got_char( buf, &bufsize, offset++, '\\' );
                UngetCharContext();
            } else {
                backslash = TRUE;       /* first '\\' of a pair */
            }
        } else {
            if( backslash ) {
                buf = got_char( buf, &bufsize, offset, '\\' );
                offset++;
                backslash = FALSE;
            }
            buf = got_char( buf, &bufsize, offset, ch );
            offset++;
        }
    }
    if( backslash ) {                   /* store any leftover backslash */
        buf = got_char( buf, &bufsize, offset, '\\' );
        offset++;
    }

    if( ch != '\0' )  UngetCharContext();
    return( buf );
}


/*
 * Scan a filename.  No leading whitespace is allowed.  Returns a pointer
 * to newly allocated memory containing the filename string.  If filename
 * contained a quote character, returned string contains quotes. If leading
 * whitespace is found, returns NULL.
 */
char *CmdScanFileName( void )
/***************************/
{
    char *              str;
    char *              newstr;

    str = CmdScanString();
    if( str != NULL ) {
        newstr = PathConvert( str, '"' );
        FreeMem( str );
    } else {
        newstr = NULL;
    }
    return( newstr );
}

/*
 * Scan a filename without quotes.  No leading whitespace is allowed.  Returns a pointer
 * to newly allocated memory containing the filename string.  If leading
 * whitespace is found, returns NULL.
 */
char *CmdScanFileNameWithoutQuotes( void )
/***************************/
{
    char *              str;
    char *              newstr;

    str = CmdScanString();
    if( str != NULL ) {
        newstr = PathConvertWithoutQuotes( str );
        FreeMem( str );
    } else {
        newstr = NULL;
    }
    return( newstr );
}


/*
 * Scan a number.  No leading whitespace is allowed.  Returns true if a
 * number was successfully parsed, or zero on error.
 */
bool CmdScanNumber( unsigned *num )
/*********************************/
{
    char                digit;
    bool                numberScanned = FALSE;
    unsigned            value = 0;
    unsigned            base = 10;

    /* figure out if this is a hex number */
    digit = GetCharContext();
    if( digit == '0' ) {
        digit = GetCharContext();
        if( digit == 'x' || digit == 'X' ) {
            digit = GetCharContext();
            if( isxdigit( digit ) ) {
                base = 16;
                UngetCharContext();
            } else {
                UngetCharContext();
                UngetCharContext();
                UngetCharContext();
            }
        } else {
            UngetCharContext();
            UngetCharContext();
        }
    } else {
        UngetCharContext();
    }

    /* convert the string to an integer */
    for( ;; ) {
        digit = GetCharContext();
        if( isdigit( digit ) ) {
            value *= base;
            value += digit - '0';
            numberScanned = TRUE;
        } else if( base == 16 && isxdigit( digit ) ) {
            value *= base;
            value += tolower( (unsigned char)digit ) - 'a' + 10;
            numberScanned = TRUE;
        } else {
            UngetCharContext();
            break;
        }
    }

    if( numberScanned )
        *num = value;
    return( numberScanned );
}
