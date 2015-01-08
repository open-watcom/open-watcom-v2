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

#include "wio.h"
#include "gui.h"
#include "guiutil.h"
#include "setup.h"
#include "setupinf.h"
#include "genvbl.h"
#include "gendlg.h"
#include "utils.h"
#include "nlmver.h"

#include "clibext.h"

static void LenToASCIIZStr( char *tobuf, unsigned char *frombuf )
{
    memcpy( tobuf, frombuf + 1, *frombuf );
    tobuf[*frombuf] = '\0';
}

int ReturnNLMVersionInfoFromFile( const char *__pathName, long *majorVersion,
                                  long *minorVersion, long *revision, long *year,
                                  long *month, long *day, char *copyrightString,
                                  char *description )
{
    int             handle, bytes, offset;
    bool            found = false;
    nlm_header_3    *verPtr;
    unsigned char   buffer[READ_SIZE];

    handle = open( __pathName, O_BINARY | O_RDONLY );
    if( handle != EFAILURE ) {
        bytes = read( handle, buffer, READ_SIZE );
        close( handle );
        if( bytes == READ_SIZE ) {
            offset = offsetof( nlm_header, descriptionLength );
            if( description != NULL ) {
                LenToASCIIZStr( description, buffer + offset );
            }
            for( ; offset < READ_SIZE; offset++ ) {
                if( !memcmp( VERSION_SIGNATURE, buffer + offset, VERSION_SIGNATURE_LENGTH ) ) {
                    found = true;
                    break;
                }
            }
            if( found ) {
                verPtr = (nlm_header_3 *)( buffer + offset );
                if( majorVersion != NULL ) {
                    *majorVersion = verPtr->majorVersion;
                }
                if( minorVersion != NULL ) {
                    *minorVersion = verPtr->minorVersion;
                }
                if( revision != NULL ) {
                    *revision = verPtr->revision;
                }
                if( year != NULL ) {
                    *year = verPtr->year;
                }
                if( month != NULL ) {
                    *month = verPtr->month;
                }
                if( day != NULL ) {
                    *day = verPtr->day;
                }
                if( copyrightString != NULL ) {
                    found = false;
                    for( ; offset < READ_SIZE; offset++ ) {
                        if( !memcmp( COPYRIGHT_SIGNATURE, buffer + offset, COPYRIGHT_SIGNATURE_LENGTH ) ) {
                            found = true;
                            break;
                        }
                    }
                    if( found ) {
                        LenToASCIIZStr( copyrightString, buffer + offset + COPYRIGHT_SIGNATURE_LENGTH );
                    }
                }
                return( ESUCCESS );
            }
        }
    }
    return( EFAILURE );
}

#define NEWERNLM                    true        // NLM newer than present
#define OLDERNLM                    false       // NLM older than present
#define SAMENLM                     false       // Same versions
#define BAD_PATH_NAME               true        // Version info not found
#define NOT_IN_SYSTEM_DIRECTORY     true        // NLM not found

char sysPath[] = { "SYS:\\SYSTEM\\" };

bool CheckNewer( const char *newNLM, const char *oldNLM )
{
    int  rc;
    long year, month, day;
    long newyear, newmonth, newday;
    long majorVersion, minorVersion, revision;
    long newmajorVersion, newminorVersion, newrevision;

    /* Get the new NLMs creation date */

    if( (rc = ReturnNLMVersionInfoFromFile( newNLM, &newmajorVersion,
                &newminorVersion, &newrevision, &newyear, &newmonth, &newday,
                NULL, NULL )) != 0 ) {
        return( BAD_PATH_NAME );
    }

    /* Get the old NLMs creation date */

    if( (rc = ReturnNLMVersionInfoFromFile( oldNLM, &majorVersion,
                &minorVersion, &revision, &year, &month, &day,
                NULL, NULL )) != 0 ) {
        return( NOT_IN_SYSTEM_DIRECTORY );
    }

    if( newmajorVersion > majorVersion )
        return( NEWERNLM );
    if( newmajorVersion < majorVersion )
        return( OLDERNLM );

    if( newminorVersion > minorVersion )
        return( NEWERNLM );
    if( newminorVersion < minorVersion )
        return( OLDERNLM );

    if( newrevision > revision )
        return( NEWERNLM );
    if( newrevision < revision )
        return( OLDERNLM );

    if( newyear > year )
        return( NEWERNLM );
    if( newyear < year )
        return( OLDERNLM );

    if( newmonth > month )
        return( NEWERNLM );
    if( newmonth < month )
        return( OLDERNLM );

    if( newday > day )
        return( NEWERNLM );
    if( newday < day )
        return( OLDERNLM );

    /* must be the identical file */
    return( SAMENLM );
}

gui_message_return CheckInstallNLM( const char *name, vhandle var_handle )
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
            chmod( name, PMODE_RWX );
            DoCopyFile( unpacked_as, name, false );
            strcpy( temp, fname );
            strcat( temp, "_NLM_installed" );
            SetVariableByName( temp, "1" );
            SetVariableByHandle( var_handle, temp );
        }
    }
    remove( unpacked_as );
    return( GUI_RET_OK );
}

