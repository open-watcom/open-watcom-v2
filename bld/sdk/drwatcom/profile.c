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
#include <sys\stat.h>
#include "drwatcom.h"
#include "watini.h"

static char     profID[] = "Dr. WATCOM";
static char     iniFile[] = WATCOM_INI;

static char     itemFilename[] = "filename";
static char     itemFlags[] = "flags";
static char     itemDisasmbackup[] = "disasmbackup";
static char     itemDisasmlines[] = "disasmlines";
static char     itemMaxlogsize[] = "maxlogsize";
static char     itemDebuggeropts[] = "debuggeropts";
static char     itemDumphow[] = "dumphow";
static char     itemAlertonwarnings[] = "alertonwarnings";

/*
 * getProfileLong - get a long integer from the profile
 */
static long getProfileLong( char *var, long init )
{
    char        tmp[32];
    char        ini[32];

    ltoa( init, ini, sizeof( ini ) );
    GetPrivateProfileString( profID, var, ini, tmp, sizeof( tmp ), iniFile );
    return( atol( tmp ) );

} /* getProfileLong */

/*
 * GetProfileInfo - read log info from profile
 */
void GetProfileInfo( void )
{
    char        tmp[256];
    struct stat st;
    int         rc;

    GetWindowsDirectory( tmp, sizeof( tmp ) );
    strcat( tmp,"\\drwatcom.log" );
    strlwr( tmp );
    GetPrivateProfileString( profID, itemFilename, tmp, LogInfo.filename,
                                        MAX_FNAME, iniFile );
    GetPrivateProfileString( profID, itemFlags,"11111111", LogInfo.flags,
                                        LOGFL_MAX+1, iniFile );
    LogInfo.disasmbackup = getProfileLong( itemDisasmbackup, 2 );
    LogInfo.disasmlines = getProfileLong( itemDisasmlines, 8 );
    LogInfo.maxlogsize = getProfileLong( itemMaxlogsize, 100000L );
    if( stat( LogInfo.filename, &st ) != -1 ) {
        if( st.st_size > LogInfo.maxlogsize ) {
            RCsprintf( tmp, STR_LOG_BIG_ERASE, st.st_size );
            rc = MessageBox( MainWindow, tmp, AppName, MB_YESNO );
            if( rc == IDYES ) {
                remove( LogInfo.filename );
            }
        }
    }
    DumpHow = getProfileLong( itemDumphow, DUMP_ALL );
    AlertOnWarnings = getProfileLong( itemAlertonwarnings, 1 );
    GetPrivateProfileString( profID, itemDebuggeropts, "/swap", DebuggerOpts,
                sizeof( DebuggerOpts ), iniFile );
    InitMonoFont( profID, iniFile, SYSTEM_FIXED_FONT, Instance );

} /* GetProfileInfo */

/*
 * writeProfileLong - write a long int to the profile
 */
static void writeProfileLong( char *name, long val  )
{
    char        tmp[32];

    ltoa( val, tmp, 10 );
    WritePrivateProfileString( profID, name, tmp, iniFile );

} /* writeProfileLong */

/*
 * PutProfileInfo - write log info from profile
 */
void PutProfileInfo( void )
{
    LogInfo.flags[LOGFL_MAX] = 0;
    WritePrivateProfileString( profID, itemFlags, LogInfo.flags, iniFile );
    WritePrivateProfileString( profID, itemFilename, LogInfo.filename, iniFile );
    writeProfileLong( itemDisasmbackup, LogInfo.disasmbackup );
    writeProfileLong( itemDisasmlines, LogInfo.disasmlines );
    writeProfileLong( itemMaxlogsize, LogInfo.maxlogsize );
    WritePrivateProfileString( profID, itemDebuggeropts, DebuggerOpts, iniFile );
    writeProfileLong( itemDumphow, DumpHow );
    writeProfileLong( itemAlertonwarnings, AlertOnWarnings );
    SaveMonoFont( profID, iniFile );

} /* PutProfileInfo */
