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
* Description:  Local file functions declaration
*
****************************************************************************/


extern void             LocalErrMsg( sys_error, char * );

extern error_handle     LocalFileErase( char const * );
extern size_t           LocalFileRead( sys_handle, void *, size_t );
extern size_t           LocalFileWrite( sys_handle, const void *, size_t );
extern unsigned long    LocalFileSeek( sys_handle, unsigned long, seek_method );
extern sys_handle       LocalFileOpen( char const *, obj_attrs );
extern error_handle     LocalFileClose( sys_handle );
extern sys_handle       LocalFileHandleSys( file_handle );
extern long             LocalFileGetDate( const char *name );
extern bool             LocalFileSetDate( const char *name, long date );

extern const file_components    LclFile;
