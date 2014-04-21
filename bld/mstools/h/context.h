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


#ifndef _CONTEXT_H
#define _CONTEXT_H


/*
 * Possible context types.
 */
#define COMMAND_LINE_CONTEXT    1
#define ENVIRON_VAR_CONTEXT     2
#define COMMAND_FILE_CONTEXT    3


/*
 * Set the maximum size of the context stack, i.e. the maximum acceptable
 * amount of context indirection.
 */
#define MAX_INDIRECTION         16


/*
 * Function prototypes.
 */
extern bool     OpenCmdLineContext( void );
extern bool     OpenEnvironContext( const char *envVar );
extern bool     OpenFileContext( const char *filename );
extern void     CloseContext( void );

extern void     PushContext( void );
extern void     PopContext( void );

extern char     GetCharContext( void );
extern void     UngetCharContext( void );

extern void     MarkPosContext( void );
extern void     GoToMarkContext( void );
extern long     GetPosContext( void );
extern void     SetPosContext( long pos );


#endif
