/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2015 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Declaration of RFX functions for remote side.
*
****************************************************************************/


extern rc_erridx        RemoteRename( const char *, const char * );
extern rc_erridx        RemoteMkDir( const char * );
extern rc_erridx        RemoteRmDir( const char * );
extern rc_erridx        RemoteSetDrv( int );
extern int              RemoteGetDrv( void );
extern rc_erridx        RemoteSetCWD( const char * );
extern long             RemoteGetFileAttr( const char * );
extern rc_erridx        RemoteSetFileAttr( const char * , long );
extern long             RemoteGetFreeSpace( int );
extern rc_erridx        RemoteDateTime( sys_handle , int *, int *, int );
extern rc_erridx        RemoteGetCwd( int, char * );
extern rc_erridx        RemoteFindFirst( const char *, void *, unsigned , int );
extern int              RemoteFindNext( void *, unsigned );
extern rc_erridx        RemoteFindClose( void );
