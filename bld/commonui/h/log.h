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
* Description:  File logging function prototypes.
*
****************************************************************************/


#ifndef _LOG_H_INCLUDED
#define _LOG_H_INCLUDED

#include <stdio.h>

#define LOG_ACTION_TRUNC                1
#define LOG_ACTION_APPEND               2
#define LOG_ACTION_QUERY                3

#define LOG_TYPE_BUFFER                 1
#define LOG_TYPE_CONTINUOUS             2

#define LOG_MAX_FNAME                   _MAX_PATH

/*
 * LogConfig contains information available to the calling application
 */
typedef struct logconfig {
    char                name[LOG_MAX_FNAME];
    char                curname[LOG_MAX_FNAME];
    bool                logging;
    bool                paused;
    int                 type;
    int                 def_action;
    bool                query_for_name;
} LogConfig;

typedef struct loginfo {
    LogConfig           config;
    bool                init;
    void                (*writefn)( FILE * );
    HANDLE              instance;
    HWND                hwnd;
} LogInfo;

#define NO_BUF_LINES                    100

/* Constants for log exists dialog */
#define LOG_TEXT                        101
#define LOG_APPEND                      102
#define LOG_REPLACE                     103
#define LOG_CANCEL                      IDCANCEL

/* Constants for the configure log dialog */
#define LOG_CFG_GROUP               201
#define LOG_CFG_CONT                202
#define LOG_CFG_PERIODIC            203
#define LOG_CFG_NAME_GROUP          204
#define LOG_CFG_QUERY_NAME          205
#define LOG_CFG_USE_NAME            206
#define LOG_CFG_ACTION_GROUP        207
#define LOG_CFG_REPLACE             208
#define LOG_CFG_APPEND              209
#define LOG_CFG_QUERY               210
#define LOG_CFG_OK                  IDOK
#define LOG_CFG_CANCEL              IDCANCEL
#define LOG_CFG_TYPE_GROUP          213
#define LOG_CFG_LNAME_GROUP         214
#define LOG_CFG_NAME_EDIT           215
#define LOG_CFG_BROWSE              216

void    LogInit( HWND hwnd, HANDLE inst, void (*writefn)( FILE * ) );
bool    SpyLogOpen( void );
void    SpyLogOut( char *res );
void    SetLogDef( void );
void    GetLogConfig( LogConfig *config );
void    SetLogConfig( LogConfig *config );
void    SpyLogClose( void );
bool    SpyLogPauseToggle( void );
bool    LogToggle( void );
void    LogConfigure( void );
void    LoadLogConfig( char *fname, char *section );
void    SaveLogConfig( char *fname, char *section );

WINEXPORT UINT_PTR CALLBACK LogSaveHook( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT INT_PTR CALLBACK LogExistsDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT INT_PTR CALLBACK ConfigLogDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

#endif /* _LOG_H_INCLUDED */
