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

#include <stdlib.h>
#include "wio.h"
#if defined( __UNIX__ )
  #include <dirent.h>
  #define _mkdir( a, b )    mkdir( a, b )
  #define DIRFLAGS          PMODE_RW
  #define WRITEATTRS        (CurrentFile->attr)
#else
  #include <direct.h>
  #define _mkdir( a, b )    mkdir( a )
  #define DIRFLAGS          0
  #define WRITEATTRS        PMODE_RW
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

#ifndef _MAX_DRIVE
  #define _MAX_DRIVE    3
  #define _MAX_DIR      260
  #define _MAX_FNAME    256
  #define _MAX_EXT      256
#endif

#endif
