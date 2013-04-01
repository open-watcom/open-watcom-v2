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
* Description:  Miscellaneous, wildcard functions.
*
****************************************************************************/


#if defined( __UNIX__ )
  #include <dirent.h>
#else
  #include <direct.h>
#endif
#if defined( __WATCOMC__ ) || defined( __UNIX__ )
  #include <fnmatch.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "watcom.h"
#include "pathgrp.h"
#include "misc.h"

#ifdef __UNIX__
  #define ISVALIDENTRY(x)   ( 1 )
  #define CMP_MODE          (FNM_PATHNAME | FNM_NOESCAPE)
#else
  #define ISVALIDENTRY(x)   ( (x->d_attr & _A_VOLID) == 0 )
  #define CMP_MODE          (FNM_PATHNAME | FNM_NOESCAPE | FNM_IGNORECASE)
#endif

extern char *FixName( char *name )
{
#if defined( __DOS__ )
/*********************************
 * Down case all filenames, converting fwd-slash to back-slash
 */
    char    *ptr;
    char    hold;

    assert( name != NULL );

    for( ptr = name; (hold = *ptr) != '\0'; ++ptr ) {
        if( hold == '/' ) {
            *ptr = '\\';
        } else if( ( hold -= 'A' ) < 26 ) {     /* SIDE EFFECT!!! */
            *ptr = hold + 'a';
        }
    }
#elif defined( __OS2__ ) || defined( __NT__ )
/*********************************
 * convert fwd-slash to back-slash
 */
    char    *ptr;
    char    hold;

    assert( name != NULL );

    for( ptr = name; (hold = *ptr) != '\0'; ++ptr ) {
        if( hold == '/' ) {
            *ptr = '\\';
        }
    }
#else
/*********************************
 * no conversion
 */
#endif
    return( name );
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

static DIR  *parent = NULL;  /* we need this across invocations */
static char *path = NULL;
static char *pattern = NULL;

extern char *DoWildCard( char *base )
/***********************************************/
{
    PGROUP          pg;
    struct dirent   *entry;

    if( base != NULL ) {
        if( path != NULL ) {        /* clean up from previous invocation */
            free( path );
            path = NULL;            /* 1-jun-90 AFS */
        }
        if( pattern != NULL ) {
            free( pattern );
            pattern = NULL;
        }
        if( parent != NULL ) {
            closedir( parent );
            parent = NULL;          /* 1-jun-90 AFS */
        }
        if( strpbrk( base, "*?" ) == NULL ) {
            return( base );
        }
        // create directory name and pattern
        path = malloc( _MAX_PATH );
        pattern = malloc( _MAX_PATH );
        strcpy( path, base );
        FixName( path );

        _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        _makepath( path, pg.drive, pg.dir, ".", NULL );
        // create file name pattern
        _makepath( pattern, NULL, NULL, pg.fname, pg.ext );

        parent = opendir( path );
        if( parent == NULL ) {
            free( path );
            path = NULL;
            free( pattern );
            pattern = NULL;
            return( base );
        }
    }
    if( parent == NULL ) {
        return( NULL );
    }
    assert( path != NULL && parent != NULL );
    entry = readdir( parent );
    while( entry != NULL ) {
        if( ISVALIDENTRY( entry ) ) {
            if( fnmatch( pattern, entry->d_name, CMP_MODE ) == 0 ) {
                break;
            }
        }
        entry = readdir( parent );
    }
    if( entry == NULL ) {
        closedir( parent );
        parent = NULL;
        free( path );
        path = NULL;
        free( pattern );
        pattern = NULL;
        return( base );
    }
    _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( path, pg.drive, pg.dir, entry->d_name, NULL );
    return( path );
}

extern void DoWildCardClose( void )
/*********************************/
{
    if( path != NULL ) {
        free( path );
        path = NULL;
    }
    if( pattern != NULL ) {
        free( pattern );
        pattern = NULL;
    }
    if( parent != NULL ) {
        closedir( parent );
        parent = NULL;
    }
}
