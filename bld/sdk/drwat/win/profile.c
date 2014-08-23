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
#include <sys/stat.h>
#include "bool.h"
#include "drwatcom.h"
#include "watini.h"

static char     profID[] = "Dr. WATCOM";
static char     iniFile[] = WATCOM_INI;

#define FILENAME                "filename"
#define DISASM_BACKUP           "disasmbackup"
#define DISASM_LINES            "disasmlines"
#define MAX_LOG_SIZE            "maxlogsize"
#define DEBUG_OPTS              "debuggeropts"
#define DUMP_HOW                "dumphow"
#define ALERT_ON_WARNINGS       "alertonwarnings"
#define EXCPT_ACTION            "action_on_exception"
#define PLOG_STACK_TRACE        "log_stack_trace"
#define PLOG_TASKS              "log_tasks"
#define PLOG_MODULES            "log_modules"
#define PLOG_GDI                "log_gdi"
#define PLOG_USER               "log_user"
#define PLOG_MEM                "log_mem"
#define PLOG_MOD_SEGMENTS       "log_mod segments"
#define PLOG_AUTOLOG            "log_autolog"
#define PLOG_NOTE               "log_note"

/*
 * getProfileLong - get a long integer from the profile
 */
static long getProfileLong( char *var, long init )
{
    char        tmp[32];
    char        ini[32];

    ltoa( init, ini, 10 );
    GetPrivateProfileString( profID, var, ini, tmp, sizeof( tmp ), iniFile );
    return( atol( tmp ) );

} /* getProfileLong */

/*
 * GetProfileInfo - read log info from profile
 */
void GetProfileInfo( void )
{
    char        tmp[256];
    char        buf[10];
    struct stat st;
    int         rc;

    GetWindowsDirectory( tmp, sizeof( tmp ) );
    strcat( tmp,"\\drwatcom.log" );
    strlwr( tmp );
    GetPrivateProfileString( profID, FILENAME, tmp, LogInfo.filename,
                                        MAX_FNAME, iniFile );

    GetPrivateProfileString( profID, PLOG_STACK_TRACE,"1", buf,
                             sizeof( buf ), iniFile );
    LogInfo.flags[ LOGFL_STACK_TRACE ] = buf[0];

    GetPrivateProfileString( profID, PLOG_TASKS, "1", buf,
                             sizeof( buf ), iniFile );
    LogInfo.flags[ LOGFL_TASKS ] = buf[0];

    GetPrivateProfileString( profID, PLOG_MODULES, "1", buf,
                             sizeof( buf ), iniFile );
    LogInfo.flags[ LOGFL_MODULES ] = buf[0];

    GetPrivateProfileString( profID, PLOG_GDI, "1", buf,
                             sizeof( buf ), iniFile );
    LogInfo.flags[ LOGFL_GDI ] = buf[0];

    GetPrivateProfileString( profID, PLOG_USER, "1", buf,
                             sizeof( buf ), iniFile );
    LogInfo.flags[ LOGFL_USER ] = buf[0];

    GetPrivateProfileString( profID, PLOG_MEM, "1", buf,
                             sizeof( buf ), iniFile );
    LogInfo.flags[ LOGFL_MEM ] = buf[0];

    GetPrivateProfileString( profID, PLOG_MOD_SEGMENTS, "1", buf,
                             sizeof( buf ), iniFile );
    LogInfo.flags[ LOGFL_MOD_SEGMENTS ] = buf[0];

    GetPrivateProfileString( profID, PLOG_AUTOLOG, "0", buf,
                             sizeof( buf ), iniFile );
    LogInfo.flags[ LOGFL_AUTOLOG ] = buf[0];

    GetPrivateProfileString( profID, PLOG_NOTE, "1", buf,
                             sizeof( buf ), iniFile );
    LogInfo.flags[ LOGFL_NOTE ] = buf[0];

    LogInfo.disasmbackup = getProfileLong( DISASM_BACKUP, 2 );
    LogInfo.disasmlines = getProfileLong( DISASM_LINES, 8 );
    LogInfo.maxlogsize = getProfileLong( MAX_LOG_SIZE, 100000L );
    if( stat( LogInfo.filename, &st ) != -1 ) {
        if( st.st_size > LogInfo.maxlogsize ) {
            RCsprintf( tmp, STR_LOG_BIG_ERASE, st.st_size );
            rc = MessageBox( MainWindow, tmp, AppName, MB_YESNO );
            if( rc == IDYES ) {
                remove( LogInfo.filename );
            }
        }
    }
    DumpHow = getProfileLong( DUMP_HOW, DUMP_ALL );
    AlertOnWarnings = getProfileLong( ALERT_ON_WARNINGS, 1 );
    ExceptionAction = getProfileLong( EXCPT_ACTION, INT_TERMINATE );
    GetPrivateProfileString( profID, DEBUG_OPTS, "/swap", DebuggerOpts,
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
    char        buf[2];

    buf[1] = '\0';

    buf[0] = LogInfo.flags[ LOGFL_STACK_TRACE ];
    WritePrivateProfileString( profID, PLOG_STACK_TRACE, buf, iniFile );

    buf[0] = LogInfo.flags[ LOGFL_TASKS ];
    WritePrivateProfileString( profID, PLOG_TASKS, buf, iniFile );

    buf[0] = LogInfo.flags[ LOGFL_MODULES ];
    WritePrivateProfileString( profID, PLOG_MODULES, buf, iniFile );

    buf[0] = LogInfo.flags[ LOGFL_GDI ];
    WritePrivateProfileString( profID, PLOG_GDI, buf, iniFile );

    buf[0] = LogInfo.flags[ LOGFL_USER ];
    WritePrivateProfileString( profID, PLOG_USER, buf, iniFile );

    buf[0] = LogInfo.flags[ LOGFL_MEM ];
    WritePrivateProfileString( profID, PLOG_MEM, buf, iniFile );

    buf[0] = LogInfo.flags[ LOGFL_MOD_SEGMENTS ];
    WritePrivateProfileString( profID, PLOG_MOD_SEGMENTS, buf, iniFile );

    buf[0] = LogInfo.flags[ LOGFL_AUTOLOG ];
    WritePrivateProfileString( profID, PLOG_AUTOLOG, buf, iniFile );

    buf[0] = LogInfo.flags[ LOGFL_NOTE ];
    WritePrivateProfileString( profID, PLOG_NOTE, buf, iniFile );

    WritePrivateProfileString( profID, FILENAME, LogInfo.filename, iniFile );
    writeProfileLong( DISASM_BACKUP, LogInfo.disasmbackup );
    writeProfileLong( DISASM_LINES, LogInfo.disasmlines );
    writeProfileLong( MAX_LOG_SIZE, LogInfo.maxlogsize );
    WritePrivateProfileString( profID, DEBUG_OPTS, DebuggerOpts, iniFile );
    writeProfileLong( DUMP_HOW, DumpHow );
    writeProfileLong( ALERT_ON_WARNINGS, AlertOnWarnings );
    writeProfileLong( EXCPT_ACTION, ExceptionAction );
    SaveMonoFont( profID, iniFile );

} /* PutProfileInfo */
