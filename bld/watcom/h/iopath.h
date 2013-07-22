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
* Description:  Host file system conventions macros.
*
****************************************************************************/


#if defined(__UNIX__)
 #define DIR_SEP                '/'
 #define PATH_LIST_SEP          ':'
 #define IS_DIR_SEP(c)          ((c) == DIR_SEP)
 #define IS_PATH_SEP(c)         IS_DIR_SEP( c )
 #define IS_PATH_ABS(p)         IS_DIR_SEP( (p)[0] )
 #define HAS_PATH(p)            IS_DIR_SEP( (p)[0] )
 #define IS_PATH_LIST_SEP(c)    ((c) == PATH_LIST_SEP)
 #define IS_INCL_SEP(c)         (IS_PATH_LIST_SEP( c ) || (c) == ';')
#else
 #define DIR_SEP                '\\'
 #define DRIVE_SEP              ':'
 #define PATH_LIST_SEP          ';'
 #define IS_DIR_SEP(c)          ((c) == DIR_SEP || (c) == '/')
 #define IS_PATH_SEP(c)         (IS_DIR_SEP( c ) || c == DRIVE_SEP)
 // '\foo.txt' is absolute, so is 'c:\foo.txt', but 'c:foo.txt' is not
 #define IS_PATH_ABS(p)         (IS_DIR_SEP( (p)[0] ) || (p)[0] != '\0' && (p)[1] == DRIVE_SEP && IS_DIR_SEP( (p)[2] ))
 #define HAS_PATH(p)            (IS_DIR_SEP( (p)[0] ) || (p)[0] != '\0' && (p)[1] == DRIVE_SEP)
 #define IS_PATH_LIST_SEP(c)    ((c) == PATH_LIST_SEP)
 #define IS_INCL_SEP(c)         IS_PATH_LIST_SEP( c )
#endif
