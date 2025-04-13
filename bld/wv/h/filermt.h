/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Remote file functions declaration
*
****************************************************************************/


extern void             RemoteErrMsg( sys_error, char * );
extern size_t           RemoteWriteConsole( const void *, size_t );
extern size_t           RemoteWriteConsoleNL( void );

extern error_handle     RemoteFileErase( char const * );
extern size_t           RemoteFileRead( sys_handle, void *, size_t );
extern size_t           RemoteFileWrite( sys_handle, const void *, size_t );
extern unsigned long    RemoteFileSeek( sys_handle, unsigned long, seek_method );
extern sys_handle       RemoteFileOpen( char const *, obj_attrs );
extern error_handle     RemoteFileClose( sys_handle );
extern long             RemoteFileGetDate( const char *name );
extern bool             RemoteFileSetDate( const char *name, long date );

extern file_components  RemFile;
