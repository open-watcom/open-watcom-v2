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
* Description:  File I/O types & modes definitions
*
****************************************************************************/

#ifndef _FIO_H_INCLUDED
#define _FIO_H_INCLUDED

typedef enum {
    REC_TEXT            = 0x0001,  // text records (terminated with CR/LF)
    REC_FIXED           = 0x0002,  // fixed records
    SEEK                = 0x0004,  // seek allowed
    WRITE_ONLY          = 0x0008,  // no reading allowed
    READ_AHEAD          = 0x0010,  // buffer read
    BUFFERED            = 0x0020,  // buffered i/o
} f_attrs;

typedef struct b_file   *file_handle;   // file handle

extern file_handle      FStdOut;

#endif
