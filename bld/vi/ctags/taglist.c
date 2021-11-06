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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ctags.h"
#include "myio.h"

#include "clibext.h"


static char             **tagList = NULL;
static unsigned         tagCount = 0;
static unsigned long    total_size = 0;

/*
 * addToTagList - add new string to tag list
 */
static void addToTagList( char *res )
{
    size_t      len;

    len = strlen( res ) + 1;
    tagList = realloc( tagList, ( tagCount + 1 ) * sizeof( char * ) );
    if( tagList == NULL ) {
        ErrorMsgExit( "Out of memory!\n" );
    }
    tagList[tagCount] = malloc( len );
    if( tagList[tagCount] == NULL ) {
        ErrorMsgExit( "Out of memory!\n" );
    }
    memcpy( tagList[tagCount], res, len );
    total_size += (unsigned long)len;
    tagCount++;

} /* addToTagList */


/*
 * AddTag - add a new tag to our list
 */
void AddTag( const char *id )
{
    const char  *fname;
    char        *linedata;
    char        res[MAX_STR];

    linedata = GetCurrentLineDataPtr();
    fname = GetCurrentFileName();
#ifdef __ENABLE_FNAME_PROCESSING__
    if( strnicmp( id, "__F_NAME", 8 ) == 0 ) {
        char *ptr;

        id = strchr( id, '(' );
        if( id != NULL ) {
            ptr = res;
            SKIP_CHAR_SPACES( id );
            while( *id != '\0' && (*id != ',') ) {
                *ptr++ = *id++;
            }

            sprintf( ptr, "\t%s\t/%s/", fname, linedata );
            addToTagList( res );

            if( *id == ',' ) {
                ptr = res;
                SKIP_CHAR_SPACES( id );
                while( *id != '\0' && (*id != ')') ) {
                    *ptr++ = *id++;
                }

                sprintf( ptr, "\t%s\t/%s/", fname, linedata );
                addToTagList( res );
            }
        }
    } else {
#endif //__ENABLE_FNAME_PROCESSING__
        sprintf( res, "%s\t%s\t/%s/", id, fname, linedata );
        addToTagList( res );
#ifdef __ENABLE_FNAME_PROCESSING__
    }
#endif //__ENABLE_FNAME_PROCESSING__

} /* AddTag */

/*
 * compareStrings - quicksort comparison
 */
static int compareStrings( const void *p1, const void *p2 )
{
    return( strcmp( *(const char **)p1, *(const char **)p2 ) );

} /* compareStrings */

/*
 * GenerateTagsFile - emit a sorted tags file
 */
void GenerateTagsFile( const char *fname )
{
    FILE        *fp;
    unsigned    i;

    if( tagList != NULL ) {
        qsort( tagList, tagCount, sizeof( char * ), compareStrings );
        fp = fopen( fname, "w" );
        if( fp == NULL ) {
            ErrorMsgExit( "Could not open tags file \"%s\"\n", fname );
        }
        if( tagList != NULL ) {
            total_size += (unsigned long)( tagCount * sizeof( char * ) );
        }
        for( i = 0; i < tagCount; i++ ) {
            fprintf( fp, "%s\n", tagList[i] );
        }
        fclose( fp );
    }
    if( VerboseFlag ) {
        printf( "Wrote %u tags.\n", tagCount );
        printf( "Used %lu bytes to store tags.\n", total_size );
    }

} /* GenerateTagsFile */

/*
 * ReadExtraTags - read in extra tags from tags file
 */
void ReadExtraTags( const char *fname )
{
    FILE        *fp;
    char        res[MAX_STR];

    fp = fopen( fname, "r" );
    if( fp != NULL ) {
        while( myfgets( res, sizeof( res ), fp ) != NULL ) {
            addToTagList( res );
        }
        fclose( fp );
    }

} /* ReadExtraTags */

unsigned GetTagCount( void )
{
    return( tagCount );
}
