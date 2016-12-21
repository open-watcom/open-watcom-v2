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
* Description:  File I/O types & modes definitions
*
****************************************************************************/

#ifndef _F77_FIO_H
#define _F77_FIO_H 1

typedef unsigned_16     f_attrs;        // file attributes

typedef struct a_file {                 // file with no buffered i/o
    f_attrs     attrs;                  // file attributes
    int         handle;                 // DOS handle
    int         stat;                   // error status
    long int    phys_offset;            // physical position in file
} a_file;

#define MIN_BUFFER    128
#if _CPU == 8086
#define IO_BUFFER     4*1024
#else
#define IO_BUFFER     16*1024
#endif

typedef struct b_file {                 // file with buffered i/o
    f_attrs     attrs;                  // file attributes
    /* unsigned_16 attrs;                  // file attributes */
    int         handle;                 // DOS handle
    int         stat;                   // error status
    long int    phys_offset;            // physical offset in file
    uint        read_len;               // amount read from buffer
    uint        b_curs;                 // position in buffer
    uint        high_water;             // highest byte written to in buffer
    uint        buff_size;              // size of buffer
    char        buffer[MIN_BUFFER];     // read buffer (must be last field since
} b_file;                               // bigger buffer may be allocated)

#define REC_TEXT                0x0001  // text records (terminated with CR/LF)
#define REC_FIXED               0x0002  // fixed records
#define REC_VARIABLE            0x0004  // variable records (length tags)
#define CARRIAGE_CONTROL        0x0008  // carriage control file
#define TRUNC_ON_WRITE          0x0010  // truncate file on write
#define SEEK                    0x0020  // seek allowed
#define WRITE_ONLY              0x0040  // no reading allowed
#define CHAR_DEVICE             0x0080  // Character device and not a file

// The following bits are only used during Openf()

#define CREATION_MASK           0xff00  // these bits used during Openf() only
#define RDONLY                  0x0100  // - read only
#define WRONLY                  0x0200  // - write only
#define RDWR                    0x0400  // - read/write
#define APPEND                  0x0800  // - append
#define S_DENYRW                0x1000  // - deny read or write access
#define S_DENYWR                0x2000  // - deny write access
#define S_DENYRD                0x4000  // - deny read access
#define S_DENYNO                0x8000  // - allow read and write access
#define S_COMPAT                0x0000  // - default is compatibility mode

// Re-use CREATION_MASK bits

#define LOGICAL_RECORD          0x0200  // start of logical record
#define PAST_EOF                0x0400  // have seeked past EOF
#define READ_AHEAD              0x0800  // buffer read
#define DIRTY_BUFFER            0x1000  // buffer has been modified
#define CC_NOCR                 0x2000  // no CR/LF
#define CC_NOLF                 0x4000  // no LF
#define BUFFERED                0x8000  // buffered i/o

#define CR      0x0d                    // carriage return
#define LF      0x0a                    // line feed
#define FF      0x0c                    // form feed

#define CTRL_Z  0x1a                    // Ctrl/Z character (EOF marker)

#define READ_ERROR      ((uint)-1)      // read error indicator

#endif
