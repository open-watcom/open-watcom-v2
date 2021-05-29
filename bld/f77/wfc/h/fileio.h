/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "fio.h"


#define MIN_BUFFER      128
#if _CPU == 8086
#define IO_BUFFER       4*1024
#else
#define IO_BUFFER       16*1024
#endif

typedef enum {
    #define pick(id,text)  FILEIO_ ## id,
    #include "_fileio.h"
    #undef pick
} io_status;

typedef struct b_file {                 // file common
    f_attrs         attrs;                  // file attributes
    FILE            *fp;                    // stream file structure pointer
    io_status       stat;                   // error status
    long            phys_offset;            // physical offset in file
                                        // file with buffered i/o
    size_t          read_len;               // amount read from buffer
    size_t          b_curs;                 // position in buffer
    size_t          high_water;             // highest byte written to in buffer
    size_t          buff_size;              // size of buffer
    char            buffer[MIN_BUFFER];     // read buffer (must be last field since
} b_file;                               // bigger buffer may be allocated)

extern void    FSetErr( io_status error, file_handle fp );
