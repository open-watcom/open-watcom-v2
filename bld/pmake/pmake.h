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
* Description:  pmake interfaces
*
****************************************************************************/


#define PMAKE_COMMAND_SIZE  512

typedef unsigned        priority_type;

typedef unsigned        depth_type;
#define MAX_DEPTH       INT_MAX

typedef struct pmake_list {
    struct pmake_list   *next;
    priority_type       priority;
    depth_type          depth;
    char                dir_name[1];    /* variable sized */
} pmake_list;

typedef struct {
    boolbit     reverse         : 1;
    boolbit     batch           : 1;
    boolbit     display         : 1;
    boolbit     optimize        : 1;
    boolbit     want_help       : 1;
    boolbit     signaled        : 1;
    boolbit     ignore_errors   : 1;
    char        *command;
    char        *cmd_args;
    char        *makefile;
    pmake_list  *dir_list;
} pmake_data;

extern pmake_data   *PMakeBuild( pmake_data *, const char *cmd );
extern void         PMakeCommand( pmake_data *, char * );
extern void         PMakeCleanup( pmake_data * );

/*
        provided by the client
*/
extern void         PMakeOutput( const char *data );
