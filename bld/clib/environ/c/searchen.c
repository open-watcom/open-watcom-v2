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


#include "variety.h"
#include "widechar.h"
#undef __INLINE_FUNCTIONS__
#include <stdio.h>
#include <stdlib.h>
#if defined(__QNX__)
  #include <unistd.h>
#else
  #include <io.h>
  #include <direct.h>
#endif
#include <string.h>
#include "rtdata.h"
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include "wenviron.h"
#endif

#if defined(__QNX__)
    #ifdef __WIDECHAR__
        #define PATH_SEPARATOR L'/'
        #define LIST_SEPARATOR L':'
    #else
        #define PATH_SEPARATOR '/'
        #define LIST_SEPARATOR ':'
    #endif
#else
    #ifdef __WIDECHAR__
        #define PATH_SEPARATOR L'\\'
        #define LIST_SEPARATOR L';'
    #else
        #define PATH_SEPARATOR '\\'
        #define LIST_SEPARATOR ';'
    #endif
#endif


_WCRTLINK void __F_NAME(_searchenv,_wsearchenv)( const CHAR_TYPE *name, const CHAR_TYPE *env_var, CHAR_TYPE *buffer )
    {
        CHAR_TYPE *p, *p2;
        int prev_errno;

        #ifdef __WIDECHAR__
            if( _RWD_wenviron == NULL )  __create_wide_environment();
        #endif

        prev_errno = _RWD_errno;
        if( __F_NAME(access,_waccess)( name, 0 ) == 0 ) {
            p = buffer;                                 /* JBS 90/3/30 */
            for( ;; ) {
                if( name[0] == PATH_SEPARATOR ) break;
                if( name[0] == __F_NAME('.',L'.') ) break;
#ifndef __QNX__
                if( name[0] == __F_NAME('/',L'/') ) break;
                if( name[0] != __F_NAME('\0',L'\0')  &&  name[1] == __F_NAME(':',L':') ) break;
#endif
                __F_NAME(getcwd,_wgetcwd)( buffer, _MAX_PATH );
                p = &buffer[ __F_NAME(strlen,wcslen)( buffer ) ];
                if( p[-1] != PATH_SEPARATOR ) *p++ = PATH_SEPARATOR;
                break;
            }
            __F_NAME(strcpy,wcscpy)( p, name );
            return;
        }
        p = __F_NAME(getenv,_wgetenv)( env_var );
        if( p != NULL ) {
            for(;;) {
                if( *p == __F_NAME('\0',L'\0') ) break;
                p2 = buffer;
                while( *p ) {
                    if( *p == LIST_SEPARATOR ) break;
                    if( *p != __F_NAME('"',L'"') ) *p2++ = *p; /* JBS 00/9/29 */
                    p++;
                }
                /* check for zero-length prefix which represents CWD */
                if( p2 != buffer ) {                    /* JBS 90/3/30 */
                    if( p2[-1] != PATH_SEPARATOR
#ifndef __QNX__
                        &&  p2[-1] != __F_NAME('/','/')
                        &&  p2[-1] != __F_NAME(':',':')
#endif
                        ) {
                        *p2++ = PATH_SEPARATOR;
                    }
                    *p2 = '\0';
                    __F_NAME(strcat,wcscat)( p2, name );
                    /* check to see if file exists */
                    if( __F_NAME(access,_waccess)( buffer, 0 ) == 0 ) {
                        __set_errno( prev_errno );
                        return;
                    }
                }
                if( *p == '\0' ) break;
                ++p;
            }
        }
        buffer[0] = '\0';
    }

