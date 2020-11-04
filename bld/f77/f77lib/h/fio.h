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

#define CHAR_CR         0x0d            // carriage return
#define CHAR_LF         0x0a            // line feed
#define CHAR_FF         0x0c            // form feed

#define CHAR_CTRL_Z     0x1a            // Ctrl/Z character (EOF marker)

#define READ_ERROR      ((size_t)-1)    // read error indicator

#define MIN_BUFFER      128
#if _CPU == 8086
#define IO_BUFFER       4*1024
#else
#define IO_BUFFER       16*1024
#endif

typedef enum {
    REC_TEXT            = 0x0001,  // text records (terminated with CR/LF)
    REC_FIXED           = 0x0002,  // fixed records
    REC_VARIABLE        = 0x0004,  // variable records (length tags)
    CARRIAGE_CONTROL    = 0x0008,  // carriage control file
    TRUNC_ON_WRITE      = 0x0010,  // truncate file on write
    SEEK                = 0x0020,  // seek allowed
    WRITE_ONLY          = 0x0040,  // no reading allowed
    CHAR_DEVICE         = 0x0080,  // Character device and not a file

// The following bits are only used during Openf()

    CREATION_MASK       = 0xff00,  // these bits used during Openf() only
    RDONLY              = 0x0100,  // - read only
    WRONLY              = 0x0200,  // - write only
    RDWR                = 0x0400,  // - read/write
    APPEND              = 0x0800,  // - append
    S_DENYRW            = 0x1000,  // - deny read or write access
    S_DENYWR            = 0x2000,  // - deny write access
    S_DENYRD            = 0x4000,  // - deny read access
    S_DENYNO            = 0x8000,  // - allow read and write access
    S_COMPAT            = 0x0000,  // - default is compatibility mode

// Re-use CREATION_MASK bits

    LOGICAL_RECORD      = 0x0200,  // start of logical record
    PAST_EOF            = 0x0400,  // have seeked past EOF
    READ_AHEAD          = 0x0800,  // buffer read
    DIRTY_BUFFER        = 0x1000,  // buffer has been modified
    CC_NOCR             = 0x2000,  // no CR/LF
    CC_NOLF             = 0x4000,  // no LF
    BUFFERED            = 0x8000,  // buffered i/o
} f_attrs;

typedef struct b_file   *file_handle;   // file handle

#endif
