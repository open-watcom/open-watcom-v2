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


#ifndef _CMDLINE_H
#define _CMDLINE_H


/*
 * Define the CmdLine type.
 */
typedef struct CmdLine {
    char **             args;       /* array of character pointers */
    char **             merged;     /* merged version of args */
    int                 maxItems;   /* # of parameters before realloc */
    int                 curItems;   /* # of parameters currently stored */
    int                 numSections;/* # of command line sections available */
} CmdLine;


/*
 * Required to terminate list of sections to merge with MergeCmdLine.
 */
#define INVALID_MERGE_CMDLINE   (-1)


/*
 * Function prototypes.
 */
extern CmdLine *InitCmdLine( int sections );
extern CmdLine *CloneCmdLine( const CmdLine *cmdLine );
extern void     DestroyCmdLine( CmdLine *cmdLine );

extern void     AppendCmdLine( CmdLine *cmdLine, int section,
                               const char *parm );
extern void     AppendFmtCmdLine( CmdLine *cmdLine, int section,
                                  const char *format, ... );
extern char **  MergeCmdLine( CmdLine *cmdLine, int section, ... );


#endif
