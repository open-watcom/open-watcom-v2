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
#include "enterdb.h"
#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__WINDOWS__) || defined(__WINDOWS_386__)
    #include <windows.h>
#elif defined(__NT__)
    #include <errno.h>
    #include <windows.h>
#elif defined( __OS2__ ) && defined( __386__ )
    #define INCL_DOSPROCESS
    #define INCL_WINDIALOGS
    #include <os2.h>
    extern unsigned (*_WindowsStdout)();
#endif

#define STR_SIZE        128

#if defined(__WIDECHAR__) && !(defined(__WINDOWS__) || defined(__WINDOWS_386__))
    #define _WCH(a)     L ## a
    #define _WCH_TYPE   wchar_t
#else
    #define _WCH(a)     a
    #define _WCH_TYPE   char
#endif

#if defined( __OS2__ ) && defined( __386__ )
  #define TITLE_STRING  "Assertion Failed!"
#else
  #define TITLE_STRING  _WCH("Assertion Failed!")
#endif

#define LEAD_STRING     _WCH("Assertion failed: ")
#define FMT_STRING      _WCH("%hs, file %hs, line %d\n")

#if !defined(__WIDECHAR__)
    static int __extra_return;
#endif

_WCRTLINK void __F_NAME(_assert,_wassert)( char *expr, char *fn, int line_num ) {
    _WCH_TYPE   str[STR_SIZE];

    #if !defined(__WIDECHAR__)
        int after_num_returns = 1;
        if( __extra_return ) {
            after_num_returns++;
            __extra_return = 0;
        }
    #endif

    #if defined(__NETWARE__)
        sprintf( str, FMT_STRING, expr, fn, line_num );
    #elif defined(__WINDOWS__) || defined(__WINDOWS_386__)
        _snprintf( str, STR_SIZE, FMT_STRING, expr, fn, line_num );
    #elif defined(__NT__)
        // __AXP__ wsprintf( str, FMT_STRING, expr, fn, line_num );
        __F_NAME(_snprintf,swprintf)( str, STR_SIZE, LEAD_STRING FMT_STRING, expr, fn, line_num );
    #else
        __F_NAME(_snprintf,swprintf)( str, STR_SIZE, LEAD_STRING FMT_STRING, expr, fn, line_num );
    #endif
    #if !defined(__WIDECHAR__)
        if( __WD_Present ) {
            char *buf;
            buf = alloca( strlen( str ) + sizeof( LEAD_STRING ) + 1 );
            strcpy( buf, LEAD_STRING );
            strcat( buf, str );
            DebuggerBreakAfterReturnWithMessage( after_num_returns, buf );
        } else
    #endif
    {
        #if defined(__WINDOWS__) || defined(__WINDOWS_386__)
            MessageBox( NULL, str, TITLE_STRING, MB_OK | MB_TASKMODAL );
        #elif defined( __NT__ )
            int rc;

            rc = __F_NAME(fputs,fputws)( str, stderr);
            if( ( rc == EOF ) && ( errno == EBADF ) ) {
                __F_NAME(_snprintf,swprintf)( str, STR_SIZE, FMT_STRING, expr, fn, line_num );
                MessageBox( NULL, str, TITLE_STRING, MB_OK | MB_TASKMODAL );
            }
        #elif defined( __OS2__ ) && defined( __386__ )
            TIB *ptib;
            PIB *ppib;
            DosGetInfoBlocks( &ptib, &ppib );
            if( ( ppib->pib_ultype == 3 ) && ( _WindowsStdout == 0 ) ) {
                #if defined( __WIDECHAR__ )
                    char outstr[MB_CUR_MAX * STR_SIZE];
                #else
                    char *outstr = str;
                #endif
                __F_NAME(_snprintf,swprintf)( str, STR_SIZE, FMT_STRING, expr, fn, line_num );
                #if defined( __WIDECHAR__ )
                    wcstombs( outstr, str, MB_CUR_MAX * STR_SIZE );
                #endif
                WinMessageBox( HWND_DESKTOP, NULL, outstr, TITLE_STRING, 0,
                                MB_SYSTEMMODAL | MB_OK );
            } else {
                __F_NAME(fputs,fputws)( str, stderr);
            }
        #else
            __F_NAME(fputs,fputws)( str, stderr);
        #endif
        abort();
    }
}

_WCRTLINK void __F_NAME(__assert,__wassert)( int value, char *expr, char *fn, int line_num ) {

    if( ! value ) {
        #if !defined(__WIDECHAR__)
            __extra_return = 1;
        #endif
        __F_NAME(_assert,_wassert)( expr, fn, line_num );
    }
}
