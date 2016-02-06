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
* Description:  Remote file access.
*
****************************************************************************/


extern int              GetCachedHandle( sys_handle remote );
extern void             InitHandleCache( void );
extern int              AddCachedHandle( int local, sys_handle remote );
extern int              DelCachedHandle( int local );
extern bool             InitFileSupp( void );
extern bool             HaveRemoteFiles( void );
extern unsigned         RemoteStringToFullName( bool executable, const char *name, char *res, trap_elen res_len );
extern sys_handle       RemoteOpen( const char *name, open_access mode );
extern unsigned         RemoteWrite( sys_handle hdl, const void *buff, unsigned len );
extern unsigned         RemoteWriteConsole( const void *buff, unsigned len );
extern unsigned         RemoteWriteConsoleNL( void );
extern unsigned         RemoteRead( sys_handle hdl, void *buff, unsigned len );
extern unsigned long    RemoteSeek( sys_handle hdl, unsigned long pos, seek_method method );
extern error_idx        RemoteClose( sys_handle hdl );
extern error_idx        RemoteErase( const char *name );
#if !defined( BUILD_RFX )
extern error_idx        RemoteFork( const char *cmd, trap_elen len );
#endif
