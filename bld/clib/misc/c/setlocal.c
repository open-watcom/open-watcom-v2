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
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "locales.h"
#include "localset.h"

#ifdef __WIDECHAR__
static wchar_t *ValidLocales[] = {
        L"C",           /* C_LOCALE */
        L"",            /* NATIVE_LOCALE */
        NULL            /* INVALID_LOCALE */
};
#else
static char *ValidLocales[] = {
        "C",            /* C_LOCALE */
        "",             /* NATIVE_LOCALE */
        NULL            /* INVALID_LOCALE */
};
#endif


_WCRTLINK CHAR_TYPE *__F_NAME(setlocale,_wsetlocale)( int category, CHAR_TYPE const *locale )
    {
        register int i;

        _INITLOCALESETTING
        if( category < LC_CTYPE  ||  category > LC_ALL ) {
            return( NULL );
        }
        if( locale == NULL ) {
            i = _LOCALESETTING[category];
        } else {
            i = C_LOCALE;
            if( *locale != __F_NAME('\0',L'\0') ) {
                for( ; ValidLocales[i]; ++i ) {
                    if( __F_NAME(strcmp,wcscmp)( locale, ValidLocales[i] ) == 0 )  break;
                }
            }
            if( i != INVALID_LOCALE ) {
                _LOCALESETTING[category] = i;
                if( category == LC_ALL ) {
                    _LOCALESETTING[LC_COLLATE]  = i;
                    _LOCALESETTING[LC_CTYPE]    = i;
                    _LOCALESETTING[LC_NUMERIC]  = i;
                    _LOCALESETTING[LC_TIME]     = i;
                    _LOCALESETTING[LC_MONETARY] = i;
#if defined( __QNX__ )
                    _LOCALESETTING[LC_MESSAGES] = i;
#endif
                }
            }
        }
        return( ValidLocales[ i ] );
    }

