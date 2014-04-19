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


#include <ctype.h>

#include "cmdscan.h"
#include "cmdlnprs.gh"          // required generated file
#include "cmdlnprs.h"


static void addString           // STORE A STRING
    ( OPT_STRING **h            // - addr[ storage ]
    , char const *s             // - string
    , size_t len )              // - length
{
    OPT_STRING *value;

    value = _MemoryAllocate( sizeof( *value ) + len );
    stvcpy( value->data, s, len );
    value->next = *h;
    *h = value;
}


static void addNumber           // STORE A NUMBER
    ( OPT_NUMBER **h            // - addr[ storage ]
    , unsigned number )         // - number
{
    OPT_NUMBER *value;

    value = _MemoryAllocate( sizeof( *value ) );
    value->number = number;
    value->next = *h;
    *h = value;
}


void CheckErrorLimit            // VALIDATE THE ERROR LIMIT
    ( unsigned *p )             // - value
{
    p = p;
}


void CheckWarnLevel             // VALIDATE WARNING LEVEL
    ( unsigned *p )             // - value
{
    if( *p > 10 ) {
        *p = 10;
    }
}


static int scanOffNumber        // SCAN A NUMBER
    ( unsigned *pvalue )        // - target
{
    int number_scanned;
    unsigned value;
    int c;

    CmdRecogEquals();
    number_scanned = 0;
    value = 0;
    for(;;) {
        c = CmdScanLowerChar();
        if( ! isdigit( c ) ) {
            CmdScanUngetChar();
            break;
        }
        value *= 10;
        value += c - '0';
        number_scanned = 1;
    }
    if( number_scanned ) {
        *pvalue = value;
    }
    return( number_scanned );
}


void StripQuotes                // STRIP QUOTES FROM A STRING
    ( char *fname )             // - the string
{
    char *s;
    char *d;

    if( *fname == '"' ) {
        // string will shrink so we can reduce in place
        d = fname;
        for( s = d + 1; *s && *s != '"'; ++s ) {
            if( s[0] == '\\' && s[1] == '"' ) {
                ++s;
            }
            *d++ = *s;
        }
        *d = '\0';
    }
}


void OPT_CLEAN_NUMBER           // CLEAN UP NUMBERS
    ( OPT_NUMBER **h )          // - list
{
    OPT_NUMBER *s;

    while( *h ) {
        s = *h;
        *h = s->next;
        _MemoryFree( s );
    }
}


void OPT_CLEAN_STRING           // CLEAN UP STRINGS
    ( OPT_STRING **h )          // - list
{
    OPT_STRING *s;

    while( *h ) {
        s = *h;
        *h = s->next;
        _MemoryFree( s );
    }
}


int OPT_GET_ID                  // GET AN ID
    ( OPT_STRING **p )          // - target
{
    size_t len;
    char const *id;

    CmdRecogEquals();
    CmdScanChar();
    len = CmdScanId( &id );
    if( len != 0 ) {
        addString( p, id, len );
        return( 1 );
    }
    BadCmdLineId();
    return( 0 );
}


int OPT_GET_LOWER               // GET CHAR IN LOWERCASE
    ( void )
{
    return( CmdScanLowerChar() );
}


int OPT_GET_NUMBER              // PARSE: #
    ( unsigned *p )             // - target
{
    unsigned value;

    if( scanOffNumber( &value ) ) {
        *p = value;
        return( 1 );
    }
    BadCmdLineNumber();
    return( 0 );
}


int OPT_GET_NUMBER_MULTIPLE     // PARSE: OPTION #
    ( OPT_NUMBER **h )          // - target
{
    unsigned value;

    if( scanOffNumber( &value ) ) {
        addNumber( h, value );
        return( 1 );
    }
    BadCmdLineNumber();
    return( 0 );
}

int OPT_GET_NUMBER_DEFAULT( unsigned *p, unsigned default_value )
{
    unsigned value;

    if( scanOffNumber( &value ) ) {
        *p = value;
    } else {
        *p = default_value;
    }
    return( 1 );
}


int OPT_GET_DIR                 // PARSE: DIR
    ( OPT_STRING **p )          // - target
{
    size_t len;
    char const *path;

    CmdPathDelim();
    len = CmdScanFilename( &path );
    if( len != 0 ) {
        addString( p, path, len );
        StripQuotes( (*p)->data );
        return( 1 );
    }
    BadCmdLinePath();
    return( 0 );
}

int OPT_GET_FILE( OPT_STRING **p )
{
    size_t len;
    char const *fname;

    CmdRecogEquals();
    len = CmdScanFilename( &fname );
    if( len != 0 ) {
        addString( p, fname, len );
        StripQuotes( (*p)->data );
        return( 1 );
    }
    BadCmdLineFile();
    return( 0 );
}

int OPT_GET_FILE_OPT( OPT_STRING **p )
{
    size_t len;
    char const *fname;

    // handle leading option char specially
    if( CmdRecogEquals() || ! CmdDelimitChar() ) {
        // specified an '=' so accept -this-is-a-file-name.fil or /tmp/ack.tmp
        len = CmdScanFilename( &fname );
        if( len != 0 ) {
            addString( p, fname, len );
            StripQuotes( (*p)->data );
        } else {
            OPT_CLEAN_STRING( p );
        }
    }
    return( 1 );
}


int OPT_GET_PATH                // PARSE: PATH
    ( OPT_STRING **p )          // - target
{
    size_t len;
    char const *path;

    CmdPathDelim();
    len = CmdScanFilename( &path );
    if( len != 0 ) {
        addString( p, path, len );
        StripQuotes( (*p)->data );
        return( 1 );
    }
    BadCmdLinePath();
    return( 0 );
}

int OPT_RECOG                   // RECOGNIZE CHAR
    ( int c )                   // - to be recog'ed
{
    return( CmdRecogChar( c ) );
}

int OPT_RECOG_LOWER             // RECOGNIZE LOWERCASE CHAR
    ( int c )                   // - to be recog'ed
{
    return( CmdRecogLowerChar( c ) );
}

void OPT_UNGET                  // UNGET A CHARACTER
    ( void )
{
    CmdScanUngetChar();
}

int OPT_END( void )             // DETECT END OF CHAIN
{
    return( CmdDelimitChar() );
}
