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
* Description:  Header file for all extern functions in utils.c.
*
****************************************************************************/


extern bool             ModifyEnvironment( bool );
extern bool             ModifyStartup( bool );
extern bool             ModifyAssociations( bool );
extern bool             ModifyUninstall( bool );
extern bool             SpawnNextScript( );
extern COPYFILE_ERROR   DoCopyFile( const char *, char *, int );
extern bool             CopyAllFiles( void );
extern gui_message_return MsgBox( gui_window *, char *, gui_message_type, ... );
extern bool             CheckDrive( bool );
extern bool             InitInfo( char *, char * );
extern bool             GetDirParams( int, char **, char **, char **, char ** );
extern bool             FreeDirParams( char **, char **, char ** );
extern void             CloseDownMessage( bool state );
extern void             CloseDownProgram();
extern bool             IsFixedDisk( char drive );
extern bool             IsDiskette( unsigned drive );
extern unsigned         GetClusterSize( char drive );
extern char             GetDriveLetter( char *desc );
extern char             *AddInstallName( char *line, bool dorealloc );
extern char             *stristr( char*str,char*substr );
extern void             Log( char *start, ... );
extern bool             CheckUpgrade();
extern bool             LicenseFileName( char * );
extern char             *GetInstallName( void );
extern int              PromptUser( char *name, char *dlg, char *skip, char *replace, int *value );
#if defined( WINNT ) || defined( WIN )
extern bool             GetRootFromPath( char *root, char *path );
extern long             FreeSpace( char *path );
extern long             ClusterSize( char *path );
#endif
extern signed int       IncrementDLLUsageCount( char *path );
extern signed int       DecrementDLLUsageCount( char *path );
extern void             ReadVariablesFile( const char * name );
