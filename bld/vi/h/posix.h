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
* Description:  Simplistic 'POSIX compatibility' header.
*
****************************************************************************/


#ifndef _POSIX_INCLUDED
#define _POSIX_INCLUDED

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#if defined( __UNIX__ )
  #include <unistd.h>
  #include <dirent.h>
  #define _mkdir( a, b )    mkdir( a, b )
  #define DIRFLAGS          S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
  #define WRITEATTRS        (CurrentFile->attr)
#else
  #include <direct.h>
  #include <io.h>
  #define _mkdir( a, b )    mkdir( a )
  #define DIRFLAGS 0
  #define WRITEATTRS        (S_IRWXU)
  #ifdef __IBMC__
    typedef struct dirent DIR;
  #endif
#endif

#ifndef R_OK
  #define F_OK  0
  #define W_OK  2
  #define R_OK  4
#endif

#ifndef ACCESS_WR
  #define ACCESS_WR W_OK
  #define ACCESS_RD R_OK
#endif

#ifndef S_IRWXU
  #define S_IRWXU    (S_IREAD | S_IWRITE | S_IEXEC)
#endif

#ifndef STDIN_FILENO
  #define STDIN_FILENO    0
  #define STDOUT_FILENO   1
  #define STDERR_FILENO   2
#endif

#ifndef O_BINARY
  #define O_BINARY 0
#endif

#ifndef O_TEXT
  #define O_TEXT 0
#endif

#ifndef _MAX_DRIVE
  #define _MAX_DRIVE    3
  #define _MAX_DIR      260
  #define _MAX_FNAME    256
  #define _MAX_EXT      256
#endif

#endif
