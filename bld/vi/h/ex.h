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


#ifndef _EX_INCLUDED
#define _EX_INCLUDED

/*
 * constants
 */
enum {
EX_T_APPEND,
EX_T_CHANGE,
EX_T_COPY,
EX_T_INSERT,
EX_T_JOIN,
EX_T_LIST,
EX_T_MARK,
EX_T_MOVE,
EX_T_UNDO,
EX_T_VERSION,
EX_T_VISUAL,
EX_T_EQUALS
};


/*
 * external defs (from exdata.c)
 */
extern char near ExTokens[];

/*
 * function prototypes
 */
/* ex.c */
int EnterExMode( void );
int ProcessEx( linenum, linenum, bool, int, int, char * );

/* exappend.c */
int Append( linenum, bool );
int AppendAnother( char * );

#endif
