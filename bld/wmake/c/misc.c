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


#if defined(__QNX__)
 #include <dirent.h>
#else
 #include <direct.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "make.h"
#include "massert.h"
#include "memory.h"
#include "misc.h"
#include "mpathgrp.h"
#include "mtypes.h"

#if ! defined(__QNX__)
 #define CHECK_MASK( attr ) if((attr & IGNORE_MASK)==0) break
#endif

static ENV_TRACKER *envList;

extern char *SkipWS( const char *p )
/**********************************/
{
    while( isws( *p ) ) {
        ++p;
    }
    return( (char *)p );
}


#if defined( __DOS__ ) || defined( __WINDOWS__ )

extern char *FixName( char *name )
/*********************************
 * Down case all filenames, converting fwd-slash to back-slash
 */
{
    char *ptr;
    char hold;

    assert( name != NULL );

    ptr = name;
    hold = *ptr;
    for(;;) {
        if( hold == NULLCHAR ) break;
        if( hold == '/' ) {
            *ptr = '\\';
        } else if( ( hold -= 'A' ) < 26 ) {     /* SIDE EFFECT!!! */
            *ptr = hold + 'a';
        }
        hold = *++ptr;
        if( hold == NULLCHAR ) break;
        if( hold == '/' ) {
            *ptr = '\\';
        } else if( ( hold -= 'A' ) < 26 ) {     /* SIDE EFFECT!!! */
            *ptr = hold + 'a';
        }
        hold = *++ptr;
    }

    return( name );
}


extern int FNameCmp( const char *a, const char *b )
/*************************************************/
{
    return( strcmp( a, b ) );
}


#ifdef USE_FAR
extern int _fFNameCmp( const char FAR *a, const char FAR *b )
/***********************************************************/
{
    return( _fstrcmp( a, b ) );
}
#endif

#elif defined( __OS2__ ) || defined( __NT__ )

extern char *FixName( char *name )
/*********************************
 * convert fwd-slash to back-slash
 */
{
    char *ptr;
    char hold;

    assert( name != NULL );

    ptr = name;
    hold = *ptr;
    for(;;) {
        if( hold == NULLCHAR ) break;
        if( hold == '/' ) {
            *ptr = '\\';
        }
        hold = *++ptr;
        if( hold == NULLCHAR ) break;
        if( hold == '/' ) {
            *ptr = '\\';
        }
        hold = *++ptr;
    }

    return( name );
}


extern int FNameCmp( const char *a, const char *b )
/*************************************************/
{
    return( stricmp( a, b ) );
}


#ifdef USE_FAR
extern int _fFNameCmp( const char FAR *a, const char FAR *b )
/***********************************************************/
{
    return( _fstricmp( a, b ) );
}
#endif

#else

extern char *FixName( char *name )
/********************************/
{
    return( name );
}


extern int FNameCmp( const char *a, const char *b )
/*************************************************/
{
    return( strcmp( a, b ) );
}

#ifdef USE_FAR
extern int _fFNameCmp( const char FAR *a, const char FAR *b )
/***********************************************************/
{
    return( _fstrcmp( a, b ) );
}
#endif

#endif


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
extern const char *DoWildCard( const char *base )
/***********************************************/
{
    static DIR      *parent = NULL;  /* we need this across invocations */
    static char     *path = NULL;

    PGROUP          *pg;
    struct dirent   *entry;


    if( base != NULL ) {
        if( path != NULL ) {        /* clean up from previous invocation */
            FreeSafe( path );
            path = NULL;            /* 1-jun-90 AFS */
        }

        if( parent != NULL ) {
            closedir( parent );
            parent = NULL;          /* 1-jun-90 AFS */
        }

        if( strpbrk( base, WILD_METAS ) == NULL ) {
            return( base );
        }

        parent = opendir( base );
        if( parent == NULL ) {
            return( base );
        }

        path = MallocSafe( _MAX_PATH );
        strcpy( path, base );
    }

    if( parent == NULL ) {
        return( NULL );
    }

    assert( path != NULL && parent != NULL );

    entry = readdir( parent );
#ifdef CHECK_MASK
    while( entry != NULL ) {
        CHECK_MASK( entry->d_attr );
        entry = readdir( parent );
    }
#endif
    if( entry == NULL ) {
        closedir( parent );
        parent = NULL;
        FreeSafe( path );
        path = NULL;                    /* 1-jun-90 AFS */
        return( NULL );
    }

    pg = SplitPath( path );
    _makepath( path, pg->drive, pg->dir, entry->d_name, NULL );
    DropPGroup( pg );

    return( path );
}


extern int KWCompare( const char **p1, const char **p2 )    /* for bsearch */
/******************************************************/
{
    return( stricmp( *p1, *p2 ) );
}


extern int PutEnvSafe( ENV_TRACKER *env )
/***************************************/
/* This function takes over responsibility for freeing env */
{
    char        *p;
    ENV_TRACKER **walk;
    ENV_TRACKER *old;
    int         rc;
    size_t      len;

                                /* upper case the name */
    p = env->value;
    while( *p != '=' && *p != NULLCHAR ) {
        *p = toupper( *p );
        ++p;
    }
    rc = putenv( env->value );  /* put into environment */
    if( p[0] == '=' && p[1] == '\0' ) {
        // we are deleting the envvar, ignore errors
        rc = 0;
    }
    len = p - env->value + 1;   /* len including '=' */
    walk = &envList;
    while( *walk != NULL ) {
        if( strncmp( (*walk)->value, env->value, len ) == 0 ) {
            break;
        }
        walk = &(*walk)->next;
    }
    old = *walk;
    if( old != NULL ) {
        *walk = old->next;      /* unlink from chain */
        FreeSafe( old );
    }
    if( p[1] != 0 ) {           /* we're giving it a new value */
        env->next = envList;    /* save the memory since putenv keeps a */
        envList = env;          /* pointer to it... */
    } else {                    /* we're deleting an old value */
        FreeSafe( env );
    }
    return( rc );
}


#ifndef NDEBUG
extern void PutEnvFini( void )
/****************************/
{
    ENV_TRACKER *cur;

    while( envList != NULL ) {
        cur = envList;
        envList = cur->next;
        FreeSafe( cur );
    }
}
#endif
