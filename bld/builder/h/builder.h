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


#include <stdlib.h>
#include <stdio.h>
#include "bool.h"

typedef struct copy_entry copy_entry;
typedef struct copy_entry {
    copy_entry  *next;
    char        src[_MAX_PATH];
    char        dst[_MAX_PATH];
};

typedef struct include include;
struct include {
    include     *prev;
    FILE        *fp;
    unsigned    skipping;
    char        name[_MAX_PATH];
    char        cwd[_MAX_PATH];
    copy_entry  *reset_abit;
};

typedef struct ctl_file ctl_file;
struct ctl_file {
    ctl_file    *next;
    char        name[_MAX_PATH];
};

#define         MAX_LINE        4096

extern include  *IncludeStk;
extern FILE     *LogFile;

void            LogFlush( void );
void            Log( bool quiet, const char *, ... );
void            OpenLog( const char * );
void            Fatal( const char *, ... );
void            *Alloc( unsigned );
char            *SkipBlanks( const char * );
unsigned        RunIt( char * );
void            ResetArchives( copy_entry * );
void            SysInit( int argc, char *argv[] );
unsigned        SysRunCommand( const char * );
unsigned        SysChdir( const char * );
