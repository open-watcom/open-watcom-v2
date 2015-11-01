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


extern bool         InitRFXSupp( void );
extern error_idx    RemoteRename( const char * from, const char *to );
extern error_idx    RemoteMkDir( const char *name );
extern error_idx    RemoteRmDir( const char *name );
extern error_idx    RemoteSetDrv( int drv );
extern int          RemoteGetDrv( void );
extern error_idx    RemoteSetCWD( const char *name );
extern long         RemoteGetFileAttr( const char * name );
extern error_idx    RemoteSetFileAttr( const char * name, long attrib );
extern long         RemoteGetFreeSpace( int drv );
extern error_idx    RemoteDateTime( sys_handle hdl, int *time, int *date, int set );
extern error_idx    RemoteGetCwd( int drv, char *where );
extern error_idx    RemoteFindFirst( const char *pattern, void *info, unsigned info_len, int attrib );
extern int          RemoteFindNext( void *info, unsigned info_len );
extern error_idx    RemoteFindClose( void );
extern unsigned     RenameNameToCannonical( char *name, char *fullname, unsigned fullname_len );
