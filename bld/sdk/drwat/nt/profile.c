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
* Description:  Gets/sets initialization data from ini file
*
****************************************************************************/


#include <windows.h>
#include "font.h"
#include "drwatcom.h"
#include "intdlg.h"
#include "inipath.h"
#include "watini.h"
#include "mem.h"

#define PROFILE_SECT            "Dr_WATCOM_NT"
#define LOG_NAME                "log_name"
#define LOG_EDITOR              "log_viewer"
#define LOG_PROCESS             "log_processes"
#define LOG_STACKTRACE          "log_stacktrace"
#define LOG_MEM_INFO            "log_mem_manager"
#define LOG_MEM_DMP             "log_mem_dump"
#define LOG_NOTES               "log_query_notes"
#define LOG_AUTOLOG             "log_auto_log"
#define LOG_MODULES             "log_modules"
#define LOG_ASM_CNT             "log_asm_lines"
#define LOG_ASM_BKUP            "log_asm_bkup"
#define LOG_MAX_FILE            "log_max_file_size"
#define LOG_AUTO_ATTATCH        "log_auto_attatch"
#define LOG_EXCPT_ACTION        "exception_action"
#define LOG_EXCPT_CONT          "exception_continue"

#define DEF_LOGNAME             "c:\\drwatcom.log"
#define DEF_EDITOR              "vi.exe"
#define DEF_DBCS_EDITOR         "notepad.exe"

static char iniPath[_MAX_PATH];

/*
 * writeProfileInt
 */

static void writeProfileInt( int val, char *item ) {

    char        buf[30];

    itoa( val, buf, 10 );
    WritePrivateProfileString( PROFILE_SECT, item, buf, iniPath );
}

/*
 * SetDefProfile
 */
void SetDefProfile( void ) {

    strcpy( LogData.logname, DEF_LOGNAME );
    if( GetSystemMetrics( SM_DBCSENABLED ) ) {
        strcpy( LogData.editor, DEF_DBCS_EDITOR );
    } else {
        strcpy( LogData.editor, DEF_EDITOR );
    }
    LogData.editor_cmdline = "";
    LogData.asm_cnt = 10;
    LogData.asm_bkup = 2;
    LogData.max_flen = 1000;
    LogData.log_process = TRUE;
    LogData.log_stacktrace = TRUE;
    LogData.log_mem_manager = TRUE;
    LogData.log_mem_dmp = FALSE;
    LogData.query_notes = TRUE;
    LogData.log_modules = TRUE;
    LogData.autolog = FALSE;
    ConfigData.auto_attatch = FALSE;
    ConfigData.continue_exception = TRUE;
    ConfigData.exception_action = INT_TERMINATE;
}

/*
 * GetProfileInfo - read log info from profile
 */
void GetProfileInfo( void )
{
    GetConfigFilePath( iniPath, sizeof(iniPath) );
    strcat( iniPath, "\\" WATCOM_INI );
    SetDefProfile();
    InitMonoFont( PROFILE_SECT, iniPath, SYSTEM_FIXED_FONT, Instance );
    GetPrivateProfileString( PROFILE_SECT, LOG_NAME, LogData.logname,
                         LogData.logname, _MAX_PATH, iniPath );
    GetPrivateProfileString( PROFILE_SECT, LOG_EDITOR, LogData.editor,
                         LogData.editor, _MAX_PATH, iniPath );
    LogData.log_process = GetPrivateProfileInt( PROFILE_SECT, LOG_PROCESS,
                          LogData.log_process, iniPath );
    LogData.log_stacktrace = GetPrivateProfileInt( PROFILE_SECT,
                             LOG_STACKTRACE, LogData.log_stacktrace,
                             iniPath );
    LogData.log_mem_manager = GetPrivateProfileInt( PROFILE_SECT,
                              LOG_MEM_INFO, LogData.log_mem_manager,
                              iniPath );
    LogData.log_mem_dmp = GetPrivateProfileInt( PROFILE_SECT,
                              LOG_MEM_DMP, LogData.log_mem_dmp, iniPath );
    LogData.query_notes = GetPrivateProfileInt( PROFILE_SECT, LOG_NOTES,
                          LogData.query_notes, iniPath );
    LogData.log_modules = GetPrivateProfileInt( PROFILE_SECT, LOG_MODULES,
                          LogData.log_modules, iniPath );
    LogData.autolog = GetPrivateProfileInt( PROFILE_SECT, LOG_AUTOLOG,
                      LogData.autolog, iniPath );
    LogData.asm_cnt = GetPrivateProfileInt( PROFILE_SECT, LOG_ASM_CNT,
                         LogData.asm_cnt, iniPath );
    LogData.asm_bkup = GetPrivateProfileInt( PROFILE_SECT, LOG_ASM_BKUP,
                         LogData.asm_bkup, iniPath );
    LogData.max_flen = GetPrivateProfileInt( PROFILE_SECT, LOG_MAX_FILE,
                         LogData.max_flen, iniPath );
    ConfigData.continue_exception= GetPrivateProfileInt( PROFILE_SECT,
                    LOG_EXCPT_CONT, ConfigData.continue_exception,
                    iniPath );
    ConfigData.auto_attatch = GetPrivateProfileInt( PROFILE_SECT,
                    LOG_AUTO_ATTATCH, ConfigData.auto_attatch, iniPath );
    ConfigData.exception_action = GetPrivateProfileInt( PROFILE_SECT,
                    LOG_EXCPT_ACTION, ConfigData.exception_action,
                    iniPath );
}

/*
 * PutProfileInfo - write log info from profile
 */
void PutProfileInfo( void )
{
    SaveMonoFont( PROFILE_SECT, iniPath );
    WritePrivateProfileString( PROFILE_SECT, LOG_NAME, LogData.logname,
                               iniPath );
    WritePrivateProfileString( PROFILE_SECT, LOG_EDITOR, LogData.editor,
                               iniPath );
    writeProfileInt( LogData.log_process, LOG_PROCESS );
    writeProfileInt( LogData.log_stacktrace, LOG_STACKTRACE );
    writeProfileInt( LogData.log_mem_manager, LOG_MEM_INFO );
    writeProfileInt( LogData.log_mem_dmp, LOG_MEM_DMP );
    writeProfileInt( LogData.query_notes, LOG_NOTES );
    writeProfileInt( LogData.autolog, LOG_AUTOLOG );
    writeProfileInt( LogData.asm_cnt, LOG_ASM_CNT );
    writeProfileInt( LogData.asm_bkup, LOG_ASM_BKUP );
    writeProfileInt( LogData.max_flen, LOG_MAX_FILE );
    writeProfileInt( LogData.log_modules, LOG_MODULES );
    writeProfileInt( ConfigData.auto_attatch, LOG_AUTO_ATTATCH );
    writeProfileInt( ConfigData.continue_exception, LOG_EXCPT_CONT );
    writeProfileInt( ConfigData.exception_action, LOG_EXCPT_ACTION );
}
