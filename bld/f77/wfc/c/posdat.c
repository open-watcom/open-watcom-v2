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
* Description:  POSIX level i/o support
*
****************************************************************************/


#include "ftnstd.h"
#include "posio.h"
#include "posdat.h"


#if defined( __WATCOMC__ ) || defined( __UNIX__ )
#define STDIN   STDIN_FILENO
#define STDOUT  STDOUT_FILENO
#define STDERR  STDERR_FILENO
#else
#define STDIN   0
#define STDOUT  1
#define STDERR  2
#endif

static  b_file          _FStdIn = { RDONLY | REC_TEXT,
                                    STDIN,
                                    POSIO_OK,
                                    0,
                                    0,
                                    0,
                                    0,
                                    MIN_BUFFER };
static  b_file          _FStdOut = { WRONLY | REC_TEXT,
                                     STDOUT,
                                     POSIO_OK,
                                     0,
                                     0,
                                     0,
                                     0,
                                     MIN_BUFFER };
static  b_file          _FStdErr = { WRONLY | REC_TEXT,
                                     STDERR,
                                     POSIO_OK,
                                     0,
                                     0,
                                     0,
                                     0,
                                     MIN_BUFFER };

file_handle     FStdIn = { &_FStdIn };
file_handle     FStdOut = { &_FStdOut };
file_handle     FStdErr = { &_FStdErr };
