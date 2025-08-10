/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Installer data source file I/O interface.
*
****************************************************************************/


#ifndef SETUPIO_H
#define SETUPIO_H


#define TEXTBUF_SIZE        8192

typedef enum {
    DATA_BIN,
    DATA_TEXT
} data_mode;

typedef struct file_handle_t    *file_handle;

extern int          FileInit( const VBUF *archive );
extern int          FileFini( void );
extern int          FileStat( const VBUF *path, struct stat *statbuf );
extern file_handle  FileOpen( const VBUF *path, data_mode mode );
extern size_t       FileRead( file_handle afh, void *buffer, size_t length );
extern int          FileClose( file_handle afh );
extern int          FileIsPlainFS( void );
extern int          FileIsArchive( void );

#endif
