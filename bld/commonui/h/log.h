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
#include "log.rh"


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
    int                 type;
    int                 def_action;
    bool                logging;
    bool                paused;
    bool                query_for_name;
} LogConfig;

#define NO_BUF_LINES                    100

void    LogInit( HWND hwnd, HANDLE inst, void (*headerfn)( FILE * ) );
void    LogFini( void );
void    LogOut( char *res );
void    LogSetDef( void );
void    LogGetConfig( LogConfig *config );
void    LogSetConfig( LogConfig *config );
bool    LogPauseToggle( void );
bool    LogToggle( void );
void    LogConfigure( void );
void    LogLoadConfig( char *fname, char *section );
void    LogSaveConfig( char *fname, char *section );

#endif /* _LOG_H_INCLUDED */
