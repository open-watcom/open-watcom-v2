/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bool.h"
#include "watcom.h"
#include "misc.h"
#include "pathgrp2.h"

#include "clibext.h"


#ifdef __UNIX__
    #define ISVALIDENTRY(x) (1)
    #define FNMATCH_FLAGS   (FNM_PATHNAME | FNM_NOESCAPE)
#else
    #define ISVALIDENTRY(x) ( (x->d_attr & _A_VOLID) == 0 )
    #define FNMATCH_FLAGS   (FNM_PATHNAME | FNM_NOESCAPE | FNM_IGNORECASE)
#endif

char         *NamePtr;
byte         NameLen;

static unsigned_16  ReadRecLen;
static unsigned_16  ReadRecMaxLen;
static byte         *ReadRecBuff;
static byte         *ReadRecPtr;

static char *FixName( char *name )
/********************************/
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
        } else if( hold >= 'A' && hold <= 'Z' ) {
            *ptr = hold + 'a' - 'A';
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

static DIR  *dirp = NULL;  /* we need this across invocations */
static char *path = NULL;
static char *pattern = NULL;

char *DoWildCard( char *base )
/****************************/
{
    pgroup2         pg;
    struct dirent   *dire;

    if( base != NULL ) {
        if( path != NULL ) {        /* clean up from previous invocation */
            free( path );
            path = NULL;
        }
        if( pattern != NULL ) {
            free( pattern );
            pattern = NULL;
        }
        if( dirp != NULL ) {
            closedir( dirp );
            dirp = NULL;
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

        dirp = opendir( path );
        if( dirp == NULL ) {
            free( path );
            path = NULL;
            free( pattern );
            pattern = NULL;
            return( base );
        }
    }
    if( dirp == NULL ) {
        return( NULL );
    }
    assert( path != NULL && dirp != NULL );
    while( (dire = readdir( dirp )) != NULL ) {
        if( ISVALIDENTRY( dire ) ) {
            if( fnmatch( pattern, dire->d_name, FNMATCH_FLAGS ) == 0 ) {
                break;
            }
        }
    }
    if( dire == NULL ) {
        closedir( dirp );
        dirp = NULL;
        free( path );
        path = NULL;
        free( pattern );
        pattern = NULL;
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
        free( path );
        path = NULL;
    }
    if( pattern != NULL ) {
        free( pattern );
        pattern = NULL;
    }
    if( dirp != NULL ) {
        closedir( dirp );
        dirp = NULL;
    }
}

void ReadRecInit( void )
/**********************/
{
    ReadRecBuff = NULL;
    ReadRecMaxLen = 0;
}

void ReadRecFini( void )
/**********************/
{
    free( ReadRecBuff );
}

static bool ExtendReadRecBuff( unsigned_16 size )
/***********************************************/
{
    if( ReadRecMaxLen < size ) {
        ReadRecMaxLen = size;
        if( ReadRecBuff != NULL ) {
            free( ReadRecBuff );
        }
        ReadRecBuff = malloc( ReadRecMaxLen );
        if( ReadRecBuff == NULL ) {
            printf( "**FATAL** Out of memory!\n" );
            return( false );
        }
    }
    return( true );
}

int ReadRec( FILE *fp, byte *hdr )
/********************************/
{
    bool    ok;

    if( fread( hdr, 1, 3, fp ) != 3 )
        return( ( ferror( fp ) == 0 ) ? -1 : 0 );
    ReadRecLen = GET_RECLEN( hdr );
    ok = ExtendReadRecBuff( ReadRecLen );
    if( ok ) {
        ok = ( fread( ReadRecBuff, ReadRecLen, 1, fp ) != 0 );
    }
    ReadRecPtr = ReadRecBuff;
    return( ok );
}

void RewindReadRec( void )
/************************/
{
    ReadRecPtr = ReadRecBuff;
}

bool WriteReadRec( FILE *fo, byte *hdr )
/**************************************/
{
    bool    ok;

    ok = ( fwrite( hdr, 1, 3, fo ) == 3 );
    if( ok )
        ok = ( fwrite( ReadRecBuff, 1, ReadRecLen, fo ) == ReadRecLen );
    return( ok );
}

bool IsDataToRead( void )
/***********************/
{
    return( ReadRecPtr - ReadRecBuff < ReadRecLen - 1 );
}

unsigned_16 GetUInt( void )
/*************************/
{
    unsigned_16 word;

    word = *(unsigned_16 *)ReadRecPtr;
    CONV_LE_16( word );
    ReadRecPtr += sizeof( unsigned_16 );
    return( word );
}

unsigned_32 GetOffset( bool wide )
/********************************/
{
    if( wide ) {
        unsigned_32 dword;

        dword = *(unsigned_32 *)ReadRecPtr;
        CONV_LE_32( dword );
        ReadRecPtr += sizeof( unsigned_32 );
        return( dword );
    } else {
        unsigned_16 word;

        word = *(unsigned_16 *)ReadRecPtr;
        CONV_LE_16( word );
        ReadRecPtr += sizeof( unsigned_16 );
        return( word );
    }
}

unsigned_16 GetIndex( void )
/**************************/
{
    unsigned_16 index;

    index = *ReadRecPtr++;
    if( index & 0x80 ) {
        index = ( (index & 0x7f) << 8 ) + *ReadRecPtr++;
    }
    return( index );
}

char *GetName( void )
/*******************/
{
    NameLen = *ReadRecPtr++;
    NamePtr = (char *)ReadRecPtr;
    ReadRecPtr += NameLen;
    return( NamePtr );
}

void NameTerm( void )
/*******************/
{
    *ReadRecPtr = '\0';
}

byte *GetReadPtr( void )
/**********************/
{
    return( ReadRecPtr );
}
