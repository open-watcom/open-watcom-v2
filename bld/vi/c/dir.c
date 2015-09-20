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
* Description:  Directory change functions.
*
****************************************************************************/


#include "vi.h"
#include <sys/types.h>
#include "posix.h"

#include "clibext.h"


/*
 * GetCWD1 - get current working directory, then allocate space for it
 */
void GetCWD1( char **str )
{
    char        bob[FILENAME_MAX];

    GetCWD2( bob, FILENAME_MAX );
    *str = DupString( bob );

} /* GetCWD1 */

/*
 * GetCWD2 - get current working directory
 */
void GetCWD2( char *str, int maxlen )
{
    if( getcwd( str, maxlen - 1 ) == NULL ) {
        str[0] = 0;
    }
    // Don't lowercase the filename
    //FileLower( str );

} /* GetCWD2 */

/*
 * ChangeDirectory - change to given drive/directory
 */
vi_rc ChangeDirectory( const char *dir )
{
    vi_rc       rc;
    size_t      shift;
    const char  *tmp;
    int         i;

    shift = 0;
    if( dir[1] == ':' ) {
        rc = ChangeDrive( dir[0] );
        if( rc != ERR_NO_ERR || dir[2] == 0 ) {
            return( rc );
        }
        shift = 2;
    }
    tmp = dir + shift;
    i = chdir( tmp );
    if( i != 0 ) {
        return( ERR_DIRECTORY_OP_FAILED );
    }
    return( ERR_NO_ERR );

} /* ChangeDirectory */

/*
 * ConditionalChangeDirectory - change dir only if needed
 */
vi_rc ConditionalChangeDirectory( const char *where )
{
    if( CurrentDirectory != NULL ) {
        if( !stricmp( CurrentDirectory, where ) ) {
            return( ERR_NO_ERR );
        }
    }
    return( ChangeDirectory( where ) );

} /* ConditionalChangeDirectory */

/*
 * SetCWD - set current working directory
 */
vi_rc SetCWD( const char *str )
{
    vi_rc   rc;

    rc = ChangeDirectory( str );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    MemFreePtr( (void **)&CurrentDirectory );
    GetCWD1( &CurrentDirectory );
    return( ERR_NO_ERR );

} /* SetCWD */


static int  currOff;
static int  totalBytes;

/*
 * addDirData - add directory file data to current buffer
 */
static void addDirData( file *cfile, char *str )
{
    int k;

    k = strlen( str );
    if( totalBytes + k + LINE_EXTRA > MAX_IO_BUFFER - 2 ) {
        CreateFcbData( cfile, currOff );
        currOff = 0;
        totalBytes = 0;
    }
    memcpy( &ReadBuffer[currOff], str, k );
    ReadBuffer[currOff + k] = CR;
    ReadBuffer[currOff + k + 1] = LF;
    currOff += k + 2;
    totalBytes += k + LINE_EXTRA;

} /* addDirData */

/*
 * FormatDirToFile - format directory listing as a file
  */
void FormatDirToFile( file *cfile, bool add_drives )
{
    int         i, j;
    int         lastdir = 0;
    char        str[MAX_STR];
    direct_ent  *de;

    if( cfile->fcbs.head != NULL ) {
        if( cfile->fcbs.head->nullfcb ) {
            FreeEntireFcb( cfile->fcbs.head );
            cfile->fcbs.head = cfile->fcbs.tail = NULL;
        }
    }
    currOff = 0;
    totalBytes = 0;

    /*
     * add directory data
     */
    for( i = 0; i < DirFileCount; i++ ) {
        if( DirFiles[i]->attr & _A_SUBDIR ) {
            if( DirFiles[i]->name[0] == '.' && DirFiles[i]->name[1] == 0 ) {
                MemFree( DirFiles[i] );
                for( j = i + 1; j < DirFileCount; j++ ) {
                    DirFiles[j - 1] = DirFiles[j];
                }
                i--;
                DirFileCount--;
            } else {
                if( lastdir != i ) {
                    de = DirFiles[lastdir];
                    DirFiles[lastdir] = DirFiles[i];
                    for( j = i; j > lastdir + 1; j-- ) {
                        DirFiles[j] = DirFiles[j - 1];
                    }
                    DirFiles[lastdir + 1] = de;
                }
                lastdir++;
            }
        }
    }
    for( i = 0; i < DirFileCount; i++ ) {
        FormatFileEntry( DirFiles[i], str );
        addDirData( cfile, str );
    }

    /*
     * add drives
     */
    if( add_drives ) {
        for( i = 'A'; i <= 'Z'; i++ ) {
            if( DoGetDriveType( i ) != DRIVE_NONE ) {
                MySprintf( str, "  [%c:]", (char) i - 'A' + 'a' );
                addDirData( cfile, str );
            }
        }
    }

    CreateFcbData( cfile, currOff );

} /* FormatDirToFile */
