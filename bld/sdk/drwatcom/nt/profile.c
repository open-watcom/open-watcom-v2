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


#include <windows.h>
#include "font.h"
#include "drwatcom.h"
#include "watini.h"
#include "mem.h"

#define PROFILE_SECT            "Dr_WATCOM_NT"
#define INIT_FILE               WATCOM_INI
#define LOG_NAME                "log_name"
#define LOG_EDITOR              "log_viewer"
#define LOG_PROCESS             "log_processes"
#define LOG_STACKTRACE          "log_stacktrace"
#define LOG_MEM_INFO            "log_mem_manager"
#define LOG_MEM_DMP             "log_mem_dump"
#define LOG_NOTES               "log_query_notes"
#define LOG_AUTOLOG             "log_auto_log"
#define LOG_MODULES             "log_modules"
#define LOG_LOG_RESTARTS        "log_log_restarts"
#define LOG_LOG_CHAINS          "log_log_chains"
#define LOG_LOG_EVENTS          "log_do_logging"
#define LOG_ASM_CNT             "log_asm_lines"
#define LOG_ASM_BKUP            "log_asm_bkup"
#define LOG_MAX_FILE            "log_max_file_size"
#define LOG_AUTO_ATTATCH        "log_auto_attatch"

#define DEF_LOGNAME             "c:\\drnt.log"
#define DEF_EDITOR              "vi.exe"

/*
 * writeProfileInt
 */

static void writeProfileInt( int val, char *item ) {

    char        buf[30];

    itoa( val, buf, 10 );
    WritePrivateProfileString( PROFILE_SECT, item, buf, INIT_FILE );
}

/*
 * SetDefProfile
 */
void SetDefProfile( void ) {

    strcpy( LogData.logname, DEF_LOGNAME );
    strcpy( LogData.editor, DEF_EDITOR );
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
    LogData.log_restarts = FALSE;
    LogData.log_chains = TRUE;
    LogData.log_events = TRUE;
    ConfigData.auto_attatch = TRUE;
}

/*
 * GetProfileInfo - read log info from profile
 */
void GetProfileInfo( void )
{
    SetDefProfile();
    InitMonoFont( PROFILE_SECT, INIT_FILE, SYSTEM_FIXED_FONT, Instance );
    GetPrivateProfileString( PROFILE_SECT, LOG_NAME, LogData.logname,
                         LogData.logname, _MAX_PATH, WATCOM_INI );
    GetPrivateProfileString( PROFILE_SECT, LOG_EDITOR, LogData.editor,
                         LogData.editor, _MAX_PATH, WATCOM_INI );
    LogData.log_process = GetPrivateProfileInt( PROFILE_SECT, LOG_PROCESS,
                          LogData.log_process, WATCOM_INI );
    LogData.log_stacktrace = GetPrivateProfileInt( PROFILE_SECT,
                             LOG_STACKTRACE, LogData.log_stacktrace,
                             WATCOM_INI );
    LogData.log_mem_manager = GetPrivateProfileInt( PROFILE_SECT,
                              LOG_MEM_INFO, LogData.log_mem_manager,
                              WATCOM_INI );
    LogData.log_mem_dmp = GetPrivateProfileInt( PROFILE_SECT,
                              LOG_MEM_DMP, LogData.log_mem_dmp, WATCOM_INI );
    LogData.query_notes = GetPrivateProfileInt( PROFILE_SECT, LOG_NOTES,
                          LogData.query_notes, WATCOM_INI );
    LogData.log_modules = GetPrivateProfileInt( PROFILE_SECT, LOG_MODULES,
                          LogData.log_modules, WATCOM_INI );
    LogData.autolog = GetPrivateProfileInt( PROFILE_SECT, LOG_AUTOLOG,
                      LogData.autolog, WATCOM_INI );
    LogData.log_restarts = GetPrivateProfileInt( PROFILE_SECT,
                              LOG_LOG_RESTARTS, LogData.log_restarts,
                              WATCOM_INI );
    LogData.log_chains = GetPrivateProfileInt( PROFILE_SECT, LOG_LOG_CHAINS,
                         LogData.log_chains, WATCOM_INI );
    LogData.log_events = GetPrivateProfileInt( PROFILE_SECT, LOG_LOG_EVENTS,
                         LogData.log_events, WATCOM_INI );
    LogData.asm_cnt = GetPrivateProfileInt( PROFILE_SECT, LOG_ASM_CNT,
                         LogData.asm_cnt, WATCOM_INI );
    LogData.asm_bkup = GetPrivateProfileInt( PROFILE_SECT, LOG_ASM_BKUP,
                         LogData.asm_bkup, WATCOM_INI );
    LogData.max_flen = GetPrivateProfileInt( PROFILE_SECT, LOG_MAX_FILE,
                         LogData.max_flen, WATCOM_INI );
    ConfigData.auto_attatch = GetPrivateProfileInt( PROFILE_SECT,
                    LOG_AUTO_ATTATCH, ConfigData.auto_attatch, WATCOM_INI );
}

/*
 * PutProfileInfo - write log info from profile
 */
void PutProfileInfo( void )
{
    SaveMonoFont( PROFILE_SECT, INIT_FILE );
    WritePrivateProfileString( PROFILE_SECT, LOG_NAME, LogData.logname,
                               INIT_FILE );
    WritePrivateProfileString( PROFILE_SECT, LOG_EDITOR, LogData.editor,
                               INIT_FILE );
    writeProfileInt( LogData.log_process, LOG_PROCESS );
    writeProfileInt( LogData.log_stacktrace, LOG_STACKTRACE );
    writeProfileInt( LogData.log_mem_manager, LOG_MEM_INFO );
    writeProfileInt( LogData.log_mem_dmp, LOG_MEM_DMP );
    writeProfileInt( LogData.query_notes, LOG_NOTES );
    writeProfileInt( LogData.autolog, LOG_AUTOLOG );
    writeProfileInt( LogData.log_restarts, LOG_LOG_RESTARTS );
    writeProfileInt( LogData.log_chains, LOG_LOG_CHAINS );
    writeProfileInt( LogData.log_events, LOG_LOG_EVENTS );
    writeProfileInt( LogData.asm_cnt, LOG_ASM_CNT );
    writeProfileInt( LogData.asm_bkup, LOG_ASM_BKUP );
    writeProfileInt( LogData.max_flen, LOG_MAX_FILE );
    writeProfileInt( LogData.log_modules, LOG_MODULES );
    writeProfileInt( ConfigData.auto_attatch, LOG_AUTO_ATTATCH );
}
