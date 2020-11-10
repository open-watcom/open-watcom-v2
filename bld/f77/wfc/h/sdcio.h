/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  System dependent compiler I/O routines.
*
****************************************************************************/


#include "fio.h"


extern void         SDInitIO( void );
extern void         SDScratch( const char *name );
extern file_handle  SDOpen( const char *name, const char *mode );
extern file_handle  SDOpenText( const char *name, const char *mode );
extern void         SDClose( file_handle fp );
extern size_t       SDReadText( file_handle fp, char *buff, size_t len );
extern void         SDWrite( file_handle fp, const void *buff, size_t len );
extern void         SDWriteTextNL( file_handle fp, const char *buff, size_t len );
extern void         SDWriteCCChar( file_handle fp, char asa, bool nolf );
extern void         SDRewind( file_handle fp );
extern bool         SDEof( file_handle fp );
extern bool         SDError( file_handle fp, char *buff, size_t max_len );
