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
* Description:  Host file system conventions configuration.
*
****************************************************************************/

#if defined(__QNX__) || defined(__UNIX__)
 #define C_PATH         "../c"
 #define H_PATH         "../h"
 #define PATH_SEP       "/"
 #define INCLUDE_SEP    ':'
 #define OBJ_EXT        ".o"
#elif defined(__DOS__) || defined(__OS2__) || defined(__NT__) || defined(__OSI__)
 #define C_PATH         "..\\c"
 #define H_PATH         "..\\h"
 #define PATH_SEP       "\\"
 #define INCLUDE_SEP    ';'
 #define OBJ_EXT        ".obj"
#else
 #error IOPATH.H not configured for system
#endif
#define DEF_EXT         ".def"
#define ERR_EXT         ".err"
#define MBR_EXT         ".mbr"
#define C_EXT           ".c"
#define CPP_EXT         ".i"
#define DEP_EXT         ".d"
