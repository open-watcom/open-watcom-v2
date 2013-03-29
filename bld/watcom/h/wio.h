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
* Description:  Include appropriate header defining POSIX I/O function.
*
****************************************************************************/


#ifndef _WUNISTD_H_INCLUDED
#define _WUNISTD_H_INCLUDED

#include <fcntl.h>
#include <sys/stat.h>
#if defined( __WATCOMC__ ) || defined( __UNIX__ )
    #include <unistd.h>
#else
    #include <io.h>
#endif

/*
 * MACROS to unify file permision
 *
 */

#if defined( __UNIX__ )
    #define PMODE_R         (S_IRUSR | S_IRGRP | S_IROTH)
    #define PMODE_W         (S_IWUSR | S_IWGRP | S_IWOTH)
    #define PMODE_X         (S_IXUSR | S_IXGRP | S_IXOTH)
#else
    #define PMODE_R         (S_IREAD)
    #define PMODE_W         (S_IWRITE)
    #define PMODE_X         (S_IEXEC)
#endif
#define PMODE_RW            (PMODE_R | PMODE_W)
#define PMODE_RWX           (PMODE_R | PMODE_W | PMODE_X)

#endif
