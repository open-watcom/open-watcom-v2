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


#ifndef STANDALONE_MERGER
#  include <wmsgdlg.hpp>
#endif

#include <wstd.h>
#include <stdio.h>
#include <stdarg.h>
#include <regexp.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "util.h"
#include "mem.h"

#ifndef STANDALONE_MERGER
#  include "wbrwin.h"
#  include "optmgr.h"
#endif

static char * browserDeaths[] = {
    "spontaneous browser combustion!",
    NULL,
    NULL,
    NULL,
    "out of memory",
    "file not found",
    NULL,
    NULL,
    "assertion failed",
    NULL
};

static char * dwarfDeaths[] = {
    "debugging information limit reached",
    "invalid version of browsing information",
    "invalid browsing information",
    "out of memory",    // DWRMALLOC or DWRREALLOC failed
    "internal browsing library problem"
};

static char * regexpDeaths[] = {
    "internal regular expression problem",
    "corrupted regular expression pointer",
    "memory corruption",
    "trailing slash found",
    "operand follows nothing",
    "unmatched square bracket",
    "invalid range",
    "nested operand",
    "empty operand",
    "unmatched round brackets",
    "too many round brackets",
    "no regular expression specified",
    "invalid case toggle"
};

extern int RegExpError;


void notYetImplemented()
//----------------------
{
    #ifndef STANDALONE_MERGER
        WMessageDialog::message( topWindow, MsgInfo, MsgOk, "Sorry -- this feature is not yet implemented", "Browser" );
    #else
        fprintf( stderr, "Sorry -- feature not yet implemented\n" );
    #endif
}

void cantOpenFile( const char * fname )
//-------------------------------------
{
    errMessage( "Unable to open file: %s\n%s ", fname, strerror( errno ) );
}

void errMessage( const char * format, ... )
//-----------------------------------------
{
    va_list arglist;

    va_start( arglist, format );

    char buffer[ 500 ];
    vsprintf( buffer, format, arglist );

    #ifndef STANDALONE_MERGER
        WMessageDialog::message( topWindow, MsgError, MsgOk, buffer, "Source Browser" );
    #else
        fprintf( stderr, "%s\n", buffer );
    #endif

    va_end( arglist );
}

void IdentifyAssassin( CauseOfDeath cause )
//-----------------------------------------
// this prints out a nice error message for a thrown exception
{
    char *      deathmsg;

    if( cause >= DEATH_BY_BAD_REGEXP ) {
        deathmsg = regexpDeaths[ (int)cause - (int)DEATH_BY_BAD_REGEXP - 1 ];
    } else if( cause >= DEATH_BY_KILLER_DWARFS ) {
        deathmsg = dwarfDeaths[ (int)cause - (int)DEATH_BY_KILLER_DWARFS ];
    } else {
        deathmsg = browserDeaths[ cause ];
    }
    if( deathmsg != NULL ) {
        errMessage( deathmsg );
    }
}

#ifndef STANDALONE_MERGER

static bool matchesAll( char * name )
//-----------------------------------
// return TRUE if we guess that the user meant "match all occurances" of a name
// NOTE: this is mainly a kludge which tries to read the user's mind.
{
    char * nameend;

    if( name == NULL || *name == '\0' ) return TRUE;
    while( isspace( *name ) ) name++;
    if( name == '\0' ) return TRUE;
    nameend = name + strlen(name) - 1;
    while( isspace( *nameend ) ) {
        *nameend = '\0';
        nameend--;
    }
    return strcmp( name, "*" ) == 0 || strcmp( name, ".*" ) == 0
           || strcmp( name, "*.*" ) == 0;
}

void * WBRRegComp( const char * cname )
//-------------------------------------
// compile a regular expression, setting options
// from the option manager
{
    OptionManager * optMgr;
    regexp *        result = NULL;
    WString         name( cname );

    optMgr = WBRWinBase::optManager();
    CaseIgnore = optMgr->getIgnoreCase();
    MagicFlag =  FALSE;
    MagicString = (optMgr->getUseRX()) ? (char *) optMgr->getMagicString()
                                       : "^$[]()~@\\.|?+*";

    if( !matchesAll( (char *) name.gets() ) ) {
        if( !optMgr->getUseRX() && optMgr->getWholeWord() ) {
            name.concat( "\\$" );
        }
        result = RegComp( name.gets() );
        if( result == NULL ) {
            throw (CauseOfDeath) (DEATH_BY_BAD_REGEXP + RegExpError);
        }
    }
    if( optMgr->getAnchored() ) {
        RegAnchor( result );
    }
    return result;
}

#else

void * WBRRegComp( const char * )
//-------------------------------
{
    return NULL;    // not needed in merger yet, but could put here
}

#endif

char * WBRStrDup( const char *src )
//---------------------------------
// this is a strdup which goes through our memory allocator.
{
    unsigned    len;
    char        *dest;

    if( src == NULL ) return NULL;
    len = strlen( src ) + 1;
    dest = (char *)WBRAlloc( len );
    memcpy( dest, src, len );
    return( dest );
}
