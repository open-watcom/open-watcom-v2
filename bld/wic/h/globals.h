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


/* Global variables */
extern pSLList g_commentList;   /* Store comments */
extern pSLList g_dirList;       /* Store preprocessor directives */
extern pSLList g_currLineCode;  /* All tokens collected from beginning of current line */
extern int g_currLineNum;       /* Current line number */
extern int g_currColNum;        /* Current column number */
extern char* g_currFileName;    /* Current file name */
extern pTokPos g_currPos;       // Current position
extern long unsigned g_memUsed; /* Amount of memory used */
extern long unsigned g_numSymbols;  // Number of symbols in symbol table
extern long unsigned g_debugCount;  // Debug counter
extern long unsigned g_dribbleCounter;  // Advanced every time when dribble is called
extern pLogList g_logList;  // List of errors (and other messages?) that is output along with the rest of code
extern struct Opt g_opt;   // Options
extern char *g_usage[];
extern long unsigned g_totalNumLines;
extern long unsigned g_numErrNotDisp;
extern pHTable g_fileTable;  // Keep track of files included
