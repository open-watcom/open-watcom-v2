/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Miscellaneous wmake helper functions.
*
****************************************************************************/


#if defined( __UNIX__ )
    #include <dirent.h>
#else
    #include <direct.h>
#endif
#include <ctype.h>
#include <sys/types.h>
#include "make.h"
#include "mmemory.h"
#include "mmisc.h"
#include "mlex.h"
#include "mrcmsg.h"
#include "msg.h"
#include "pathgrp2.h"

#include "clibext.h"


#define CLOWER(c)   (((c) < 'a') ? (c) - 'A' + 'a' : (c))

#if defined( __DOS__ )
/* DOS: down case all filenames, convert fwd-slash to back-slash */
#define FIX_CHAR(c) (((c) == '/') ? '\\' : (cisalpha( (c) ) ? CLOWER(c) : (c)))
#elif defined( __OS2__ ) || defined( __NT__ ) || defined( __RDOS__ )
/* OS2, NT and RDOS: convert fwd-slash to back-slash */
#define FIX_CHAR(c) (((c) == '/') ? '\\' : (c))
#else   /* __UNIX__ */
/* UNIX: no changes */
#define FIX_CHAR(c) (c)
#endif

char *SkipWS( const char *p )
/*****************************
 * p is not const because the return value is usually used to write data.
 */
{
    while( cisws( *p ) ) {
        ++p;
    }
    return( (char *)p );
}

STATIC bool is_ws( char c )
/*************************/
{
    return( cisws( c ) );
}

STATIC bool is_ws_or_equal( char c )
/**********************************/
{
    return( cisws( c ) || c == '=' );
}

STATIC char *FindNextSep( const char *str, bool (*chk_sep)( char ) )
/******************************************************************
 * Finds next free separator character, allowing double quotes to
 * be used to specify strings with white spaces.
 */
{
    bool    dquote;
    char    c;

    dquote = false;
    while( (c = *str) != NULLCHAR ) {
        if( c == '\\' ) {
            if( dquote && str[1] != NULLCHAR ) {
                ++str;
            }
        } else if( c == '\"' ) {
            dquote = !dquote;
        } else if( !dquote && chk_sep( c ) ) {
            break;
        }
        str++;
    }

    return( (char *)str );
}

char *FindNextWS( const char *str )
/***********************************
 * Finds next free white space character, allowing double quotes to
 * be used to specify strings with white spaces.
 */
{
    return( FindNextSep( str, is_ws ) );
}

char *FindNextWSorEqual( const char *str )
/*****************************************
 * Finds next free white space or equal character, allowing double quotes to
 * be used to specify strings with white spaces.
 */
{
    return( FindNextSep( str, is_ws_or_equal ) );
}

char *FixName( char *name )
{
#if defined( __DOS__ ) || defined( __OS2__ ) || defined( __NT__ ) || defined( __RDOS__ )
/***************************************************************************************
 * DOS: down case all characters, convert fwd-slash to back-slash
 * OS2, NT and RDOS: convert fwd-slash to back-slash
 */
    char    *ptr;
    char    hold;

    assert( name != NULL );

    for( ptr = name; (hold = *ptr) != NULLCHAR; ptr++ ) {
        *ptr = FIX_CHAR( hold );
    }

    return( name );
#else
    return( name );
#endif
}

bool FNameEq( const char *a, const char *b )
/******************************************/
{
#if defined( __DOS__ ) || defined( __OS2__ ) || defined( __NT__ ) || defined( __RDOS__ )
    return( stricmp( a, b ) == 0 );
#else
    return( strcmp( a, b ) == 0 );
#endif
}


static bool FNameChrEq( char a, char b )
/**************************************/
{
#if defined( __DOS__ ) || defined( __OS2__ ) || defined( __NT__ ) || defined( __RDOS__ )
    return( ctolower( a ) == ctolower( b ) );
#else
    return( a == b );
#endif
}


#ifdef USE_FAR
bool FarFNameEq( const char FAR *a, const char FAR *b )
/*****************************************************/
{
#if defined( __DOS__ ) || defined( __OS2__ ) || defined( __NT__ ) || defined( __RDOS__ )
    return( _fstricmp( a, b ) == 0 );
#else
    return( _fstrcmp( a, b ) == 0 );
#endif
}
#endif


#define IS_WILDCARD_CHAR( x ) ((*x == '*') || (*x == '?'))

static bool __fnmatch( const char *pattern, const char *string )
/***************************************************************
 * OS specific compare function FNameChrEq
 * must be used for file names
 */
{
    const char  *p;
    int         len;
    bool        star_char;
    int         i;

    /*
     * check pattern section with wildcard characters
     */
    star_char = false;
    while( IS_WILDCARD_CHAR( pattern ) ) {
        if( *pattern == '?' ) {
            if( *string == NULLCHAR ) {
                return( false );
            }
            string++;
        } else {
            star_char = true;
        }
        pattern++;
    }
    if( *pattern == NULLCHAR ) {
        if( (*string == NULLCHAR) || star_char ) {
            return( true );
        } else {
            return( false );
        }
    }
    /*
     * check pattern section with exact match
     * ( all characters except wildcards )
     */
    p = pattern;
    len = 0;
    do {
        if( star_char ) {
            if( string[len] == NULLCHAR ) {
                return( false );
            }
            len++;
        } else {
            if( !FNameChrEq( *pattern, *string ) ) {
                return( false );
            }
            string++;
        }
        pattern++;
    } while( *pattern != NULLCHAR && !IS_WILDCARD_CHAR( pattern ) );
    if( !star_char ) {
        /*
         * match is OK, try next pattern section
         */
        return( __fnmatch( pattern, string ) );
    } else {
        /*
         * star pattern section, try locate exact match
         */
        while( *string != NULLCHAR ) {
            if( FNameChrEq( *p, *string ) ) {
                for( i = 1; i < len; i++ ) {
                    if( !FNameChrEq( *(p + i), *(string + i) ) ) {
                        break;
                    }
                }
                if( i == len ) {
                    /*
                     * if rest doesn't match, find next occurence
                     */
                    if( __fnmatch( pattern, string + len ) ) {
                        return( true );
                    }
                }
            }
            string++;
        }
        return( false );
    }
}

/*
 * THIS FUNCTION IS NOT RE-ENTRANT!
 *
 * It returns a pointer to a character string, after doing wildcard
 * substitutions.  It returns NULL when there are no more substitutions
 * possible.
 *
 * DoWildCard behaves similarly to strtok.  You first pass it a pointer
 * to a substitution string.  It checks if the string contains wildcards,
 * and if not it simply returns this string.  If the string contains
 * wildcards, it attempts an opendir with the string.  If that fails it
 * returns the string.
 *
 * If the opendir succeeds, or you pass DoWildCard a NULL pointer, it reads
 * the next normal file from the directory, and returns the filename.
 *
 * If there are no more files in the directory, or no directory is open,
 * DoWildCard returns null.
 *
 */

static DIR  *dirp = NULL;  /* we need this across invocations */
static char *path = NULL;
static char *pattern = NULL;

const char *DoWildCard( const char *base )
/***********************************************/
{
    pgroup2         pg;
    struct dirent   *dire;

    if( base != NULL ) {
        /* clean up from previous invocation */
        DoWildCardClose();

        if( strpbrk( base, WILD_METAS ) == NULL ) {
            return( base );
        }
        // create directory name and pattern
        path = MallocSafe( _MAX_PATH );
        pattern = MallocSafe( _MAX_PATH );
        strcpy( path, base );
        FixName( path );
        _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        _makepath( path, pg.drive, pg.dir, ".", NULL );
        // create file name pattern
        _makepath( pattern, NULL, NULL, pg.fname, pg.ext );

        dirp = opendir( path );
        if( dirp == NULL ) {
            DoWildCardClose();
            return( base );
        }
    }

    if( dirp == NULL ) {
        return( NULL );
    }

    assert( path != NULL && dirp != NULL );

    while( (dire = readdir( dirp )) != NULL ) {
#if !defined( __UNIX__ )
        if( dire->d_attr & IGNORE_MASK )
            continue;
#endif
        if( __fnmatch( pattern, dire->d_name ) ) {
            break;
        }
    }
    if( dire == NULL ) {
        DoWildCardClose();
        return( base );
    }

    _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( path, pg.drive, pg.dir, dire->d_name, NULL );

    return( path );
}


void DoWildCardClose( void )
/**************************/
{
    if( path != NULL ) {
        FreeSafe( path );
        path = NULL;
    }
    if( pattern != NULL ) {
        FreeSafe( pattern );
        pattern = NULL;
    }
    if( dirp != NULL ) {
        closedir( dirp );
        dirp = NULL;
    }
}


int KWCompare( const void *p1, const void *p2 )     /* for bsearch */
/*********************************************/
{
    return( stricmp( *(const char **)p1, *(const char **)p2 ) );
}
