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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "gui.h"
#include "guiutil.h"
#include "setup.h"
#include "setupinf.h"
#include "genvbl.h"
#include "gendlg.h"
#include "utils.h"

#include "nlmver.h"

static void LenToASCIIZStr( char *tobuf, BYTE *frombuf )
{
    memcpy( tobuf, frombuf + 1, *frombuf );
    tobuf[*frombuf] = '\0';
}

int ReturnNLMVersionInfoFromFile( char *__pathName, LONG *majorVersion,
                                  LONG *minorVersion, LONG *revision, LONG *year,
                                  LONG *month, LONG *day, char *copyrightString,
                                  char *description )
{
    int         handle, bytes, offset, found = FALSE;
    LONG        *verPtr;
    NLMHDR      *nlmHeader;
    BYTE        buffer[READ_SIZE];

    handle = open( __pathName, O_BINARY | O_RDONLY, 0 );
    if( handle != EFAILURE ) {
        bytes = read( handle, buffer, READ_SIZE );
        close( handle );
        if( bytes == READ_SIZE ) {
            if( description ) {
                nlmHeader = (NLMHDR *)buffer;
                LenToASCIIZStr( description, &(nlmHeader->descriptionLength) );
            }

            for( offset = 0; !found && ( offset < READ_SIZE ); offset++ ) {
                if( !memcmp( "VeRsIoN", &buffer[offset], 7 ) ) {
                    found = TRUE;
                }
            }
            if( found ) {
                verPtr = (LONG *)(&buffer[offset + 7]);
                if( majorVersion ) {
                    *majorVersion = *verPtr++;
                }
                if( minorVersion ) {
                    *minorVersion = *verPtr++;
                }
                if( revision ) {
                    *revision = *verPtr++;
                }
                if( year ) {
                    *year = *verPtr++;
                }
                if( month ) {
                    *month = *verPtr++;
                }
                if( day ) {
                    *day = *verPtr++;
                }
                found = FALSE;
                for( ; !found && (offset < READ_SIZE); offset++ ) {
                    if( !memcmp( "CoPyRiGhT", &buffer[offset], 9 ) ) {
                        found = TRUE;
                    }
                }
                if( found ) {
                    if( copyrightString ) {
                        LenToASCIIZStr( copyrightString, &buffer[offset + 9] );
                    }
                }
                return( ESUCCESS );
            }
        }
    }
    return( EFAILURE );
}

#define NEWERNLM                    TRUE        // NLM newer than present
#define OLDERNLM                    FALSE       // NLM older than present
#define SAMENLM                     FALSE       // Same versions
#define BAD_PATH_NAME               TRUE        // Version info not found
#define NOT_IN_SYSTEM_DIRECTORY     TRUE        // NLM not found

char sysPath[] = { "SYS:\\SYSTEM\\" };

int CheckNewer( char *newNLM, char *oldNLM )
{
    int  rc;
    LONG year, month, day;
    LONG newyear, newmonth, newday;
    LONG majorVersion, minorVersion, revision;
    LONG newmajorVersion, newminorVersion, newrevision;

    /* Get the new NLMs creation date */

    if( rc = ReturnNLMVersionInfoFromFile( newNLM, &newmajorVersion,
                &newminorVersion, &newrevision, &newyear, &newmonth, &newday,
                NULL, NULL ) ) {
        return BAD_PATH_NAME;
    }

    /* Get the old NLMs creation date */

    if( rc = ReturnNLMVersionInfoFromFile( oldNLM, &majorVersion,
                &minorVersion, &revision, &year, &month, &day,
                NULL, NULL ) ) {
        return NOT_IN_SYSTEM_DIRECTORY;
    }

    if( newmajorVersion > majorVersion ) return( NEWERNLM );
    if( newmajorVersion < majorVersion ) return( OLDERNLM );

    if( newminorVersion > minorVersion ) return( NEWERNLM );
    if( newminorVersion < minorVersion ) return( OLDERNLM );

    if( newrevision > revision ) return( NEWERNLM );
    if( newrevision < revision ) return( OLDERNLM );

    if( newyear > year ) return( NEWERNLM );
    if( newyear < year ) return( OLDERNLM );

    if( newmonth > month ) return( NEWERNLM );
    if( newmonth < month ) return( OLDERNLM );

    if( newday > day ) return( NEWERNLM );
    if( newday < day ) return( OLDERNLM );

    /* must be the identical file */
    return( SAMENLM );
}

gui_message_return CheckInstallNLM( char *name, vhandle var_handle )
{
    char        unpacked_as[_MAX_PATH];
    char        temp[_MAX_PATH];
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];

    _splitpath( name, drive, dir, fname, ext );
    _makepath( unpacked_as, drive, dir, fname, "._N_" );
    if( CheckNewer( unpacked_as, name ) ) {
        _makepath( temp, NULL, sysPath, fname, ext );
        if( CheckNewer( unpacked_as, temp ) ) {
            chmod( name, S_IRWXU | S_IRWXG | S_IRWXO );
            DoCopyFile( unpacked_as, name, FALSE );
            strcpy( temp, fname );
            strcat( temp, "_NLM_installed" );
            SetVariableByName( temp, "1" );
            SetVariableByHandle( var_handle, temp );
        }
    }
    remove( unpacked_as );
    return( GUI_RET_OK );
}

