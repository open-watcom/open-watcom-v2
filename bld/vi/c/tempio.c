/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Temporary file/directory related routines
*
****************************************************************************/


#include "vi.h"
#include "tempio.h"

#if defined(__UNIX__)
static char altTmpDir[] = "/tmp";
#else
static char altTmpDir[] = "c:";
#endif

/*
 * VerifyTmpDir - make sure TmpDir is valid
 */
void VerifyTmpDir( void )
{
    int     i;
    char    *env_tmpdir;

    if( EditVars.TmpDir != NULL ) {
        i = strlen( EditVars.TmpDir ) - 1;
        if( EditVars.TmpDir[i] == FILE_SEP && i > 2 ) {
            /* this sucks -- we need the '\' IFF it is [drive]:\ */
            EditVars.TmpDir[i] = '\0';
        }
        if( IsDirectory( EditVars.TmpDir ) ) {
            /* strip the following file_sep char for [drive]:\ */
            if( EditVars.TmpDir[i] == FILE_SEP ) {
                EditVars.TmpDir[i] = '\0';
            }
            return;
        }
    }
    env_tmpdir = getenv( "tmp" );
    if( env_tmpdir != NULL ) {
        if( env_tmpdir[strlen( env_tmpdir ) - 1] == '\\' ) {
            char buf[FILENAME_MAX];
            strcpy( buf, env_tmpdir );
            buf[strlen( buf ) - 1] = '\0';
            ReplaceString( &EditVars.TmpDir, buf );
        } else {
            ReplaceString( &EditVars.TmpDir, env_tmpdir );
        }
    } else {
        // _mkdir( altTmpDir, DIRFLAGS );
        ReplaceString( &EditVars.TmpDir, altTmpDir );
    }

} /* VerifyTmpDir */

/*
 * MakeTmpPath - make a path to a file from TmpDir
 */
char *MakeTmpPath( char *out, const char *in )
{
    char    *p;

    out[0] = '\0';
    if( EditVars.TmpDir == NULL ) {
        char *env_tmpdir = getenv( "tmp" );
        if( env_tmpdir != NULL ) {
            p = StrMerge( 3, out, env_tmpdir, FILE_SEP_STR, in );
        } else {
            p = StrMerge( 3, out, altTmpDir, FILE_SEP_STR, in );
        }
    } else {
        p = StrMerge( 3, out, EditVars.TmpDir, FILE_SEP_STR, in );
    }
    return( p );

} /* MakeTmpPath */
