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
* Description:  Internal OS/2 2.x trap file routines.
*
****************************************************************************/


#define OPEN_CREATE  1
#define OPEN_PRIVATE 2

#define NIL_DOS_HANDLE  ((HFILE)0xFFFF)
typedef enum { USER_SCREEN, DEBUG_SCREEN } scrtype;

char        *StrCopy( char *, char * );
long        OpenFile( char *, USHORT, int );
void        RestoreScreen( void );
long        TryPath( char *, char *, char * );
long        FindFilePath( char *, char *, char * );
char        *AddDriveAndPath( char *, char * );
void        MergeArgvArray( char *, char *, unsigned );
long        TaskOpenFile( char *name, int mode, int flags );
HFILE       TaskDupFile( HFILE old, HFILE new );
long        TaskCloseFile( HFILE hdl );
