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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <sys\types.h>
#include "posix.h"
#include "vi.h"

/*
 * GetCWD1 - get current working directory, then allocate space for it
 */
void GetCWD1( char **str )
{
    char        bob[_MAX_PATH];

    GetCWD2( bob, _MAX_PATH );
    AddString( str, bob );

} /* GetCWD1 */

/*
 * GetCWD2 - get current working directory
 */
void GetCWD2( char *str, int maxlen )
{
    if( getcwd( str, maxlen-1 ) == NULL ) {
        str[0] = 0;
    }
    // Don't lowercase the filename
    //FileLower( str );

} /* GetCWD2 */

/*
 * ChangeDirectory - change to given drive/directory
 */
int ChangeDirectory( char *dir )
{
    int         rc;
    int         shift;
    char        *tmp;

    shift = 0;
    if( dir[1] == ':' ) {
        rc = ChangeDrive( dir[0] );
        if( rc || dir[2] == 0 ) {
            return( rc );
        }
        shift=2;
    }
    tmp = &(dir[shift]);
    rc = chdir( tmp );
    if( rc != 0 ) {
        return( ERR_DIRECTORY_OP_FAILED );
    }
    return( ERR_NO_ERR );

} /* ChangeDirectory */

/*
 * ConditionalChangeDirectory - change dir only if needed
 */
int ConditionalChangeDirectory( char *where )
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
int SetCWD( char *str )
{
    int i;

    i = ChangeDirectory( str );
    if( i ) {
        return( i );
    }
    MemFree2( &CurrentDirectory );
    GetCWD1( &CurrentDirectory );
    return( ERR_NO_ERR );

} /* SetCWD */


static int currOff;
static int totalBytes;

/*
 * addDirData - add directory file data to current buffer
 */
static void addDirData( file *cfile, char *str )
{
    int k;

    k = strlen( str );
    if( totalBytes + k + LINE_EXTRA > MAX_IO_BUFFER-2 ) {
        CreateFcbData( cfile, currOff );
        currOff = 0;
        totalBytes = 0;
    }
    memcpy( &ReadBuffer[currOff], str, k );
    memcpy( &ReadBuffer[currOff+k],crlf,2 );
    currOff += k+2;
    totalBytes += k+LINE_EXTRA;

} /* addDirData */

/*
 * FormatDirToFile - format directory listing as a file
  */
void FormatDirToFile( file *cfile, bool add_drives )
{
    int         i,j;
    int         lastdir=0;
    char        str[MAX_STR];
    direct_ent  *de;

    if( cfile->fcb_head != NULL ) {
        if( cfile->fcb_head->nullfcb ) {
            FreeEntireFcb( cfile->fcb_head );
            cfile->fcb_head = cfile->fcb_tail = NULL;
        }
    }
    currOff = 0;
    totalBytes = 0;

    /*
     * add directory data
     */
    for( i=0;i<DirFileCount;i++ ) {
        if( (DirFiles[i]->attr & _A_SUBDIR) ) {
            if( DirFiles[i]->name[0] == '.' && DirFiles[i]->name[1] == 0 ) {
                MemFree( DirFiles[i] );
                for( j=i+1;j<DirFileCount;j++ ) {
                    DirFiles[j-1] = DirFiles[j];
                }
                i--;
                DirFileCount--;
            } else {
                if( lastdir != i ) {
                    de = DirFiles[ lastdir ];
                    DirFiles[ lastdir ] = DirFiles[i];
                    for( j=i;j>lastdir+1;j-- ) {
                        DirFiles[j] = DirFiles[j-1];
                    }
                    DirFiles[ lastdir+1 ] = de;
                }
                lastdir++;
            }
        }
    }
    for( i=0;i<DirFileCount;i++ ) {
        FormatFileEntry( DirFiles[i], str );
        addDirData( cfile, str );
    }

    /*
     * add drives
     */
    if( add_drives ) {
        for( i='A';i<='Z';i++ ) {
            if( DoGetDriveType( i ) != DRIVE_NONE ) {
                MySprintf( str,"  [%c:]", (char) i-'A'+'a' );
                addDirData( cfile, str );
            }
        }
    }

    CreateFcbData( cfile, currOff );

} /* FormatDirToFile */
