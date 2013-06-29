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
* Description:  builder & langdat definitions
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <watcom.h>
#include <bool.h>

typedef struct copy_entry       copy_entry;
struct copy_entry {
    copy_entry  *next;
    char        src[_MAX_PATH];
    char        dst[_MAX_PATH];
};

typedef struct include          include;
struct include {
    include     *prev;
    FILE        *fp;
    unsigned    skipping;
    unsigned    ifdefskipping;
    char        name[_MAX_PATH];
    char        cwd[_MAX_PATH];
    copy_entry  *reset_abit;
};

typedef struct ctl_file         ctl_file;
struct ctl_file {
    ctl_file    *next;
    char        name[_MAX_PATH];
};

#define         MAX_LINE        4096

extern bool         Quiet;

extern include      *IncludeStk;
extern FILE         *LogFile;

extern void         LogFlush( void );
extern void         Log( bool quiet, const char *, ... );
extern void         OpenLog( const char * );
extern void         CloseLog( void );
extern const char   *LogDirEquals( char *dir );
extern void         Fatal( const char *, ... );
extern void         *Alloc( size_t );
extern char         *SkipBlanks( const char * );
extern int          RunIt( char *, bool );
extern void         ResetArchives( copy_entry * );
extern void         SysInit( int argc, char *argv[] );
extern int          SysRunCommand( const char * );
extern int          SysRunCommandPipe( const char *, int *readpipe );
extern int          SysChdir( char * );
extern int          SysDosChdir( char * );
