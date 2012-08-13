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


//
// This file contains headers for functions return a character from buffered
// input;
//
//  Modified    By              Reason
//  93/01/27    Alex Brodsky    Created
//  93/03/10    Alex Brodsky    add PeekFirstChar()

typedef struct read_file {
    char        *buffer;
    char        *cur_pos;
    char        *end_pos;
    int         file_handle;
    long        size;
} read_file;

extern  int     GetFirstChar( char *buff, long size, int f_handle,
                                struct read_file *fdata );
extern  int     PeekFirstChar( char *buff, long size, int f_handle,
                                struct read_file *fdata );
extern  int     GetNextChar( struct read_file *fdata );
extern  int     PeekNextChar( struct read_file *fdata );

