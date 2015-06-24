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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "filestr.h"
#include "iomode.h"
#include "liballoc.h"

#define MAX_ELEM_SIZE           ( 8 + 1 + 8 + 1 + 8 + 1 )
#define _INITIALIZED            _DYNAMIC


/*
 * Return a pointer to a string containing information about currently open
 * POSIX-level file handles.  If _fileinfo is non-zero, this string is
 * passed to child processes as an environment string.  Each handle's data
 * is of the form "PPPPPPPP:OOOOOOOO:MMMMMMMM*", where the Ps represent the
 * POSIX-level file number, the Os represent the OS-level file number, and
 * the Ms represent the file's mode bits.  The Ps, Os, and Ms are all hex
 * strings, without zero padding or 0x prefixes to conserve space.  Returns
 * NULL if there is insufficient memory.  The caller is responsible for
 * freeing the allocated memory.  The returned string begins with
 * "C_FILE_INFO=", and so forms a valid environment string.
 */
_WCRTLINK CHAR_TYPE *__F_NAME(__FormPosixHandleStr,__wFormPosixHandleStr)( void )
/*******************************************************************************/
{
    CHAR_TYPE *         p;
    int                 posixHandle, osHandle, mode;
    CHAR_TYPE           curElem[MAX_ELEM_SIZE+1];
    CHAR_TYPE           buf[9];
    size_t              len;

    /*** Allocate memory for the string ***/
    len = (__NFiles*MAX_ELEM_SIZE) + __F_NAME(strlen,wcslen)( STRING( "C_FILE_INFO=" ) ) + 1;
    p = lib_malloc( len * sizeof( CHAR_TYPE ) );
    if( p == NULL )  return( NULL );
    __F_NAME(strcpy,wcscpy)( p, STRING( "C_FILE_INFO=" ) );

    /*** Process the open files ***/
    for( posixHandle=0; posixHandle<__NFiles; posixHandle++ ) {
        __ChkTTYIOMode( posixHandle );
        mode = __GetIOMode( posixHandle );
        if( mode & _INITIALIZED ) {         /* skip it if it's not open */
            osHandle = _os_handle( posixHandle );

            /*** Build the element string ***/
            curElem[0] = NULLCHAR;
            __F_NAME(itoa,_itow)( posixHandle, buf, 16 );  /* POSIX handle */
            __F_NAME(strcat,wcscat)( curElem, buf );
            __F_NAME(strcat,wcscat)( curElem, STRING( ":" ) );  /* separator */
            __F_NAME(itoa,_itow)( osHandle, buf, 16 );      /* OS handle */
            __F_NAME(strcat,wcscat)( curElem, buf );
            __F_NAME(strcat,wcscat)( curElem, STRING( ":" ) );  /* separator */
            __F_NAME(itoa,_itow)( mode, buf, 16 );          /* file mode */
            __F_NAME(strcat,wcscat)( curElem, buf );
            __F_NAME(strcat,wcscat)( curElem, STRING( "*" ) );   /* terminator */

            __F_NAME(strcat,wcscat)( p, curElem );          /* append it */
        }
    }
    return( p );
}
