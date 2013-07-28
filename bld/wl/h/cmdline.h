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
* Description:  Prototypes for command parser functions
*
****************************************************************************/


extern bool             ProcImport( void );
extern bool             ProcExport( void );
extern bool             ProcSegment( void );
extern bool             ProcAlignment( void );
extern bool             ProcHeapSize( void );
extern bool             ProcOffset( void );
extern bool             ProcNoRelocs( void );

extern bool             ProcXDbg( void );
extern bool             ProcIntDbg( void );

extern void             InitCmdFile( void );
extern void             SetSegMask(void);
extern char             *GetNextLink( void );
extern void             DoCmdFile( char * );
extern void             Syntax( void );
extern void             FreePaths( void );
extern void             Burn( void );
extern void             Ignite( void );
extern void             SetFormat( void );
extern void             AddFmtLibPaths( void );
extern bool             HintFormat( exe_format );
extern void             DecideFormat( void );
extern void             FreeFormatStuff( void );
extern void             AddCommentLib( char *, unsigned, lib_priority );
extern void             ExecSystem( char * );
extern void             PruneSystemList( void );
extern void             BurnSystemList( void );
extern void             AddLibPaths( char *, unsigned, bool );
extern void             AddLibPathsToEnd( char * );
extern void             AddLibPathsToEndList( const char *list );
extern file_list        *AddObjLib( char *, lib_priority );
extern int              DoBuffCmdParse( char *cmd );
