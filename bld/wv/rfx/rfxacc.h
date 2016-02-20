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
extern error_handle RemoteRename( const char * from, const char *to );
extern error_handle RemoteMkDir( const char *name );
extern error_handle RemoteRmDir( const char *name );
extern error_handle RemoteSetDrv( int drv );
extern int          RemoteGetDrv( void );
extern error_handle RemoteSetCWD( const char *name );
extern long         RemoteGetFileAttr( const char * name );
extern error_handle RemoteSetFileAttr( const char * name, long attrib );
extern long         RemoteGetFreeSpace( int drv );
extern error_handle RemoteDateTime( sys_handle hdl, int *time, int *date, int set );
extern error_handle RemoteGetCwd( int drv, char *where );
extern error_handle RemoteFindFirst( const char *pattern, void *info, trap_elen info_len, int attrib );
extern int          RemoteFindNext( void *info, trap_elen info_len );
extern error_handle RemoteFindClose( void );
extern size_t       RenameNameToCannonical( char *name, char *fullname, trap_elen fullname_len );
